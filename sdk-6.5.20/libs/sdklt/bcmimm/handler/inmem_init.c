/*! \file inmem_init.c
 *
 *  Initializes in memory component front end.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_mutex.h>
#include <sal/sal_sem.h>
#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>
#include <sal/sal_assert.h>
#include <sal/sal_rwlock.h>
#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <shr/shr_fmm.h>
#include <shr/shr_ha.h>
#include <bcmdrd_config.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmissu/issu_api.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmcfg/comp/bcmcfg_feature_ctl.h>
#include <bcmimm/bcmimm_backend.h>
#include <bcmimm/generated/front_e_ha.h>
#include <bcmimm/bcmimm_basic.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmimm/bcmimm_internal.h>

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMIMM_FRONTEND

/* This defines the largest supported transaction size. */
#define MAX_AUX_ENTRIES 100

#define BLK_SIGNATURE   0xd582ab66

typedef struct {
    uint8_t *buff;
    sal_sem_t sync;
} working_buff_t;

static working_buff_t bcmimm_working_buffers[BCMDRD_CONFIG_MAX_UNITS];

/*
 * This is a running number per unit. For every table created, this
 * number increments by one. This array keeps the last value per unit.
 */
static shr_ha_sub_id imm_fe_sub_id[BCMDRD_CONFIG_MAX_UNITS];

/* This is the in-memory component ID. It is applicable for every unit. */
static shr_ha_mod_id imm_fe_comp_id = BCMMGMT_IMM_FE_COMP_ID;

/*
 * This array records an ISSU event that should be considered during init.
 */
static bool issu_event[BCMDRD_CONFIG_MAX_UNITS];

/*
 * This variable indicates if the operation mode is with atomic transaction
 * enabled or not. If atomic transaction disabled there is no need to keep
 * HA memory to recover from failure to stage or from crash. Warm boot is
 * still supported.
 */
static bool atomic_trans_ena = false;

/*
 * This variable maintains the state of the IMM with respect to each unit.
 */
static bool initialized_unit[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Global Variables
 */
imm_ha_blk_t *bcmimm_tables_list[BCMDRD_CONFIG_MAX_UNITS];

bcmimm_lt_event_t *bcmimm_tables_cb[BCMDRD_CONFIG_MAX_UNITS];

bcmimm_table_dynamic_info_t *bcmimm_dynamic_info[BCMDRD_CONFIG_MAX_UNITS];

bcmimm_dynamic_blk_t bcmimm_dynamic_buffs[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private functions
 */
/*!
 *\brief Allocate generic buffer HA block.
 *
 * \param [in] unit Is the unit to allocate the block for.
 * \param [in] warm indicates if this is cold or warm boot.
 * \param [in] issu Indicates if it is an upgrade. For upgrade there is no
 * reason to maintain the old buffers since they are empty. Therefore for
 * issu we start from scratch.
 * \param [in] sub Is the sub module to use for HA block allocation.
 * \param [in] blk_id The block ID
 * \param [out] d_blk_mgmt Returned pointer to the allocated HA block.
 * \param [in,out] len Contains the desired block length as input and being
 * update with the actual allocated length for output.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int generic_blk_alloc(int unit,
                             bool warm,
                             bool issu,
                             shr_ha_sub_id sub,
                             const char *blk_id,
                             bcmimm_dynamic_blk_mgmt_t *d_blk_mgmt,
                             uint32_t *len)
{
    generic_array_blk_t *blk_hdr;
    uint32_t req_len = *len;
    uint8_t *buf;

    SHR_FUNC_ENTER(unit);
    buf = shr_ha_mem_alloc(unit,
                           imm_fe_comp_id,
                           sub,
                           blk_id,
                           len);
    SHR_NULL_CHECK(buf, SHR_E_MEMORY);
    if (*len < req_len) {
        buf = shr_ha_mem_realloc(unit, buf, req_len);
        SHR_NULL_CHECK(buf, SHR_E_MEMORY);
        *len = req_len;
    }

    blk_hdr = (generic_array_blk_t *)buf;
    if (!warm || (blk_hdr->signature != BLK_SIGNATURE || issu)) {
        sal_memset(blk_hdr, 0, *len);
        blk_hdr->blk_size = *len;
        blk_hdr->free_offset = sizeof(generic_array_blk_t);
        blk_hdr->signature = BLK_SIGNATURE;

        if (!issu) {
            bcmissu_struct_info_report(unit, imm_fe_comp_id, sub,
                                       0, sizeof(generic_array_blk_t), 1,
                                       GENERIC_ARRAY_BLK_T_ID);
        }
    }
    d_blk_mgmt->blk = buf;
    d_blk_mgmt->sync_obj = sal_mutex_create("bcmimmSync");
    SHR_NULL_CHECK(d_blk_mgmt->sync_obj, SHR_E_MEMORY);
exit:
    SHR_FUNC_EXIT();
}

/*!
 *\brief Extract data field value to a single uint64.
 *
 * \param [in] src Is the data field to extract its value.
 * \param [in] dtag Is the data dag indicating the type of the value in \c src.
 *
 * \return The data value of the field as uint64 type.
 */
static uint64_t extract_union_value(bcmltd_field_data_t *src,
                                    bcmltd_field_data_tag_t dtag)
{
    uint64_t val;

    switch (dtag) {
    case BCMLT_FIELD_DATA_TAG_BOOL:
        val = (uint64_t)src->is_true;
        break;
    case BCMLT_FIELD_DATA_TAG_U8:
        val = (uint64_t)src->u8;
        break;
    case BCMLT_FIELD_DATA_TAG_U16:
        val = (uint64_t)src->u16;
        break;
    case BCMLT_FIELD_DATA_TAG_U32:
        val = (uint64_t)src->u32;
        break;
    case BCMLT_FIELD_DATA_TAG_U64:
        val = src->u64;
        break;
    default:
        val = 0;
        break;
    }

    return val;
}

/*!
 * \brief Counts SIDs that are in tables but not in unit table.
 *
 * This function counts the number of new entries required to accommodate the
 * list of LTs from the array \c tables within the unit tables. The unit table
 * structure is kept in HA and therefore contains the tables from the previous
 * configuration.
 * This function is only being called when the unit table structure is being
 * reused (unit was detached and reattached).
 *
 * \param [in] unit_tbl Unit table structure contain an array of all LTs from
 * previous configuration.
 * \param [in] tables The current list of IMM table (for this specific
 * configuration).
 * \param [in] tbl_cnt The number of elements in the array \c tables.
 *
 * \return The number of required entries to store new tables.
 */
static uint32_t find_missing_sids(imm_ha_blk_t *unit_tbl,
                                  bcmlrd_sid_t *tables,
                                  size_t tbl_cnt)
{
    table_info_t *tbl;
    uint32_t missing = 0;
    size_t j;
    uint32_t k;

    for (j = 0; j < tbl_cnt; j++) {
        for (k = 0, tbl = unit_tbl->tbl; k < unit_tbl->allocated; k++, tbl++) {
            if (tbl->sid == tables[j]) {
                break;
            }
        }
        if (k == unit_tbl->allocated) {  /* SID was not found in unit tables */
            missing++;
        }
    }
    if (unit_tbl->array_size - unit_tbl->allocated >= missing) {
        return 0;
    } else {
        return missing - (unit_tbl->array_size - unit_tbl->allocated);
    }
}

/*!
 * \brief Allocate fixed HA blocks.
 *
 * This function allocates known HA block for the unit. The block
 * being used to manage all the FE tables for the given unit.
 *
 * \param [in] unit Is the unit associated with the blocks.
 * \param [in] tables The current list of IMM table (for this specific
 * configuration).
 * \param [out] cold Indicates if it is a cold or warm boot.
 * \param [out] retain_cont Indicate if the content of the tables should be
 * retained.
 * \param [in,out] total_lts For input it is the array size of \c tables. For
 * output it is the array size allocated to retain table information. This
 * number is equal or larger then the number of IMM tables since it carries LT
 * from previous configuration that might not be in use (not mapped) in the
 * current configuration.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int ha_blk_table_alloc(int unit,
                              bcmlrd_sid_t *tables,
                              bool *warm,
                              bool *retain_cont,
                              size_t *total_lts)
{
    uint32_t len = *total_lts * sizeof(table_info_t) + sizeof(imm_ha_blk_t);
    uint32_t actual_len = len;
    imm_ha_blk_t *unit_tbl;
    uint32_t j;
    uint32_t addition_tbls = 0;
    char ha_id[MAX_BLK_ID_STR_LEN];

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (shr_ha_str_to_id("GENERIC_TBL_COMP_ID",
                          SHR_HA_BLK_USAGE_TYPE,
                          MAX_BLK_ID_STR_LEN,
                          ha_id));
    unit_tbl = shr_ha_mem_alloc(unit,
                                imm_fe_comp_id,
                                GENERIC_TBL_COMP_ID,
                                ha_id,
                                &actual_len);
    SHR_NULL_CHECK(unit_tbl, SHR_E_MEMORY);
    /*
     * It is possible that configuration file change at cold boot will
     * result in more or different tables then before so reallocate the HA
     * block to the desired size. The point is that the table array will be
     * a superset of all IMM tables for all possible configurations.
     * The first block of code refines the desired len calculation by adding
     * the space required to insert LTs that are not yet there. Note that
     * configuration can't change in warm boot.
     */
    if (!(*warm) && unit_tbl->signature == BLK_SIGNATURE) {
        addition_tbls = find_missing_sids(unit_tbl, tables, *total_lts);
        if (addition_tbls) {
            len = actual_len + addition_tbls * sizeof(table_info_t);
        }
    }
    if (actual_len < len) {
        uint8_t *blk_end;
        unit_tbl = shr_ha_mem_realloc(unit, unit_tbl, len);
        SHR_NULL_CHECK(unit_tbl, SHR_E_MEMORY);
        /* Clean the new memory (the top of the block) */
        blk_end = (uint8_t *)unit_tbl;
        blk_end += actual_len;
        sal_memset(blk_end, 0, len - actual_len);
        unit_tbl->array_size = *total_lts + addition_tbls;
    }
    if (*warm || unit_tbl->signature == BLK_SIGNATURE) {
        *retain_cont = true;
        *total_lts = unit_tbl->array_size;
    } else {   /* First cold start */
        *retain_cont = false;
        sal_memset(unit_tbl, 0, len);
        unit_tbl->signature = BLK_SIGNATURE;
        bcmissu_struct_info_report(unit,
                                   imm_fe_comp_id,
                                   GENERIC_TBL_COMP_ID,
                                   0,
                                   len, 1,
                                   IMM_HA_BLK_T_ID);
        unit_tbl->array_size = *total_lts;
    }

    bcmimm_tables_list[unit] = unit_tbl;
    for (j = 0; j < unit_tbl->allocated; j++) {
        unit_tbl->tbl[j].blk_hdl = BCMIMM_BE_INVALID_TBL_HDL;
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Allocate Generic HA blocks for data, key and transaction buffers.
 *
 * This function allocates HA blocks for the unit. These blocks
 * being used as buffers for the data and key portions for all the tables.
 * The auxilury table being used in the case were a single table require
 * more than one entry in a transaction. This entry will be kept in a buffer(s)
 * provided by the auxilury block.
 *
 * \param [in] unit Is the unit associated with the blocks.
 * \param [in] data_size Is the desired block size for the data buffers.
 * \param [in] key_size Is the desired block size for the key buffers.
 * \param [in] aux_size Is the desired block size to handle extra key and data
 * buffers.
 * \param [in] warm Indicates if it is a cold or warm boot.
 * \param [in] issu Indicates if it is an upgrade. For upgrade there is no
 * reason to maintain the old buffers since they are empty. Therefore for
 * ISSU we start from scratch.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int ha_buff_blk_alloc(int unit,
                             size_t data_size,
                             size_t key_size,
                             size_t aux_size,
                             bool warm,
                             bool issu)
{
    bcmimm_dynamic_blk_t *tbl_blk;
    uint32_t len;
    char ha_id[MAX_BLK_ID_STR_LEN];

    SHR_FUNC_ENTER(unit);

    tbl_blk = &bcmimm_dynamic_buffs[unit];
    /* Allocate data table */
    len = data_size + sizeof(generic_array_blk_t);
    SHR_IF_ERR_EXIT
        (shr_ha_str_to_id("data_working_buf",
                          SHR_HA_BLK_USAGE_OTHER,
                          MAX_BLK_ID_STR_LEN,
                          ha_id));
    SHR_IF_ERR_EXIT(generic_blk_alloc(unit,
                                      warm,
                                      issu,
                                      GENERIC_DATA_COMP_ID,
                                      ha_id,
                                      &tbl_blk->data_blk,
                                      &len));

    /* Allocate key table */
    len = key_size + sizeof(generic_array_blk_t);
    SHR_IF_ERR_EXIT
        (shr_ha_str_to_id("key_working_buf",
                          SHR_HA_BLK_USAGE_OTHER,
                          MAX_BLK_ID_STR_LEN,
                          ha_id));
    SHR_IF_ERR_EXIT(generic_blk_alloc(unit,
                                      warm,
                                      issu,
                                      GENERIC_KEY_COMP_ID,
                                      ha_id,
                                      &tbl_blk->key_blk,
                                      &len));

    if (atomic_trans_ena) {
        /*
         * Allocate the auxiluary table. This will be used to store data/keys for
         * transactions with more than a single entry per table. The size should be
         * calculated to be N time the largest entry.
         */
        len = aux_size + sizeof(generic_array_blk_t);
        SHR_IF_ERR_EXIT
            (shr_ha_str_to_id("aux_working_buf",
                              SHR_HA_BLK_USAGE_OTHER,
                              MAX_BLK_ID_STR_LEN,
                              ha_id));
        SHR_IF_ERR_EXIT(generic_blk_alloc(unit,
                                          warm,
                                          issu,
                                          AUX_TBL_COMP_ID,
                                          ha_id,
                                          &tbl_blk->aux_blk,
                                          &len));
    } else {
       tbl_blk->aux_blk.blk = NULL;
    }
    /*
     * Allocate the transaction table.
     */
    len = BCMIMM_MAX_TRANS_SIZE * sizeof(imm_trans_elem_t) +
            sizeof(generic_array_blk_t) + sizeof(imm_trans_info_t);
    SHR_IF_ERR_EXIT
        (shr_ha_str_to_id("TRANS_TBL_COMP_ID",
                          SHR_HA_BLK_USAGE_TYPE,
                          MAX_BLK_ID_STR_LEN,
                          ha_id));
    SHR_IF_ERR_EXIT(generic_blk_alloc(unit,
                                      warm,
                                      issu,
                                      TRANS_TBL_COMP_ID,
                                      ha_id,
                                      &tbl_blk->trans_blk,
                                      &len));
    if (!warm) {
        len = BCMIMM_MAX_TRANS_SIZE * sizeof(imm_trans_elem_t);
        bcmissu_struct_info_report(unit, imm_fe_comp_id, TRANS_TBL_COMP_ID,
                                   sizeof(generic_array_blk_t),
                                   sizeof(imm_trans_info_t) +
                                   len, 1,
                                   IMM_TRANS_INFO_T_ID);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Allocate table information pointer from the management block.
 *
 * This function allocates a table information block from the table array
 * managed by the tables list.
 *
 * \param [in] unit Is the unit associated with the blocks.
 *
 * \return Pointer into requested table info.
 */
static table_info_t *ha_table_block_alloc(int unit)
{
    table_info_t *t_info = NULL;

    if (bcmimm_tables_list[unit]->array_size <=
        bcmimm_tables_list[unit]->allocated) {
        return NULL;
    }
    bcmimm_tables_list[unit]->allocated++;
    t_info = &bcmimm_tables_list[unit]->tbl[bcmimm_tables_list[unit]->free_idx];
    t_info->fields_idx = bcmimm_tables_list[unit]->free_idx++;
    return t_info;
}

/*!
 * \brief Free last table information pointer back to the management block.
 *
 * This function allocates a table information block from the table array
 * managed by the tables list.
 *
 * \param [in] unit Is the unit associated with the blocks.
 *
 * \return Pointer into requested table info.
 */
static void ha_table_last_block_free(int unit, table_info_t *blk)
{
    /* Verify that it is indeed the last allocated block */
    if (blk !=
        &bcmimm_tables_list[unit]->tbl[bcmimm_tables_list[unit]->free_idx-1]) {
        return;
    }
    bcmimm_tables_list[unit]->allocated--;
    bcmimm_tables_list[unit]->free_idx--;
}

static int alloc_table_dynamic_info(int unit, table_info_t *t_info)
{
    bcmimm_table_dynamic_info_t *dyn_info;

    SHR_FUNC_ENTER(unit);
    dyn_info = &bcmimm_dynamic_info[unit][t_info->fields_idx];
    if (t_info->data_fld_cnt > 0) {
        SHR_ALLOC(dyn_info->data_fields,
                  sizeof(field_info_t) * t_info->data_fld_cnt,
                  "bcmimmDataFld");
        SHR_NULL_CHECK(dyn_info->data_fields, SHR_E_MEMORY);
    } else {
        dyn_info->data_fields = NULL;
    }

    /* Allocate key fields if exist */
    if (t_info->key_fld_cnt) {
        SHR_ALLOC(dyn_info->key_fields,
                  sizeof(field_info_t) * t_info->key_fld_cnt,
                  "bcmimmKeyFld");
        SHR_NULL_CHECK(dyn_info->key_fields, SHR_E_MEMORY);
    } else {
        dyn_info->key_fields = NULL;
    }
exit:
    SHR_FUNC_EXIT();
}

int bcmimm_table_dimension_find(int unit,
                                table_info_t *t_info,
                                size_t num_fid,
                                bcmlrd_client_field_info_t *f_info)
{
    size_t j;
    bcmlrd_client_field_info_t *info;
    bool use_array = false;
    uint32_t field_size;
    uint32_t max_depth = 0;
    uint32_t max_field_size = 0;
    uint32_t max_num_of_flds = 0; /* The maximal number of fields in an entry */
    uint32_t data_f_len; /* Data field length */

    /* Go through all the fields */
    for (j = 0, info = f_info; j < num_fid; j++, info++) {
        if (info->key) {
            t_info->key_fld_cnt++;
            t_info->key_arry_size += info->width;  /* Width in bits */
        } else {
            t_info->data_fld_cnt++;
            /* Convert width from bits to bytes */
            field_size = BCMIMM_BITS_TO_BYTES(info->width);
            if (field_size > max_field_size) {
                max_field_size = field_size;
            }
            if (info->depth > max_depth) {
                max_depth = info->depth;
            }
            if ((!use_array) && ((info->depth) || (info->elements > 1))) {
                use_array = true;
            }
            if (info->depth > 0) {
                max_num_of_flds += info->depth;
            } else {
                max_num_of_flds += info->elements;
            }
        }
    }

    t_info->max_num_field = max_num_of_flds;

    if (max_field_size < sizeof(uint64_t)) {
        /* Round the data size up to the next 4 bytes */
        t_info->f_data_size = ((max_field_size + 3) / 4) * 4;
    }  else {
        t_info->f_data_size = sizeof(uint64_t);/* This is the maximal scalar */
    }

    if (use_array) {
        if (max_depth < 0x100) {
            t_info->array_idx_size = 1;
        } else if (max_depth < 0x10000) {
            t_info->array_idx_size = 2;
        } else {  /* Max size */
            t_info->array_idx_size = 3;
        }
    } else {
        t_info->array_idx_size = 0;
    }

    /*
     * The cache memory for each field is as follows:
     * field data size + field ID + boolean.
     * The purpose of the boolean is to indicate true for fields that should
     * be deleted.
     */
    if (t_info->type == IMM_DIRECT) {
        if (use_array) {
            data_f_len = sizeof(bcmimm_be_direct_tbl_field_array_t);
        } else {
            data_f_len = sizeof(bcmimm_be_direct_tbl_field_basic_t);
        }
        data_f_len +=  sizeof(bool);
        /* Overwrite the key bit size */
        t_info->key_arry_size = 8 * sizeof(uint32_t);
    } else {
        data_f_len = t_info->f_data_size + BCMIMM_BE_FID_SIZE + sizeof(bool);
    }
    t_info->data_bits_len = max_num_of_flds * data_f_len;

    return SHR_E_NONE;
}

/*
 * Go through the field list and set the attribute of each one.
 * Attributes include min/max/default values, field ID and width.
 */
static void set_field_attrib(table_info_t *tbl,
                             bcmimm_table_dynamic_info_t *dyn_info,
                             bcmlrd_client_field_info_t *f_info,
                             size_t num_fid)
{
    field_info_t *key = dyn_info->key_fields;
    field_info_t *data = dyn_info->data_fields;
    size_t j;
    bcmlrd_client_field_info_t *info;

    for (j = 0, info = f_info; j < num_fid; j++, info++) {
        if (info->key) {
            key->fid = info->id;
            key->width = info->width;
            key->read_only = false;
            key->min_val = extract_union_value(&info->min, info->dtag);
            key->max_val = extract_union_value(&info->max, info->dtag);
            key->def_val = extract_union_value(&info->def, info->dtag);
            key->array_size = 0;
            key++;
        } else {
            assert (data != NULL);
            data->fid = info->id;
            data->width = info->width;
            data->read_only = (info->access == BCMLTD_FIELD_ACCESS_READ_ONLY);
            data->min_val = extract_union_value(&info->min, info->dtag);
            data->max_val = extract_union_value(&info->max, info->dtag);
            data->def_val = extract_union_value(&info->def, info->dtag);
            if (info->depth) {
                data->array_size = info->depth;
            } else {
                data->array_size = info->elements;
            }
            data++;
        }
    }
}

/*!
 * \brief Allocate working buffer
 *
 * This function being called to allocate a working buffer. The working buffer
 * is being used mostly by the south bound API.
 *
 * \param [in] unit The unit associated with the working buffer.
 * \param [in] size The size of the buffer to allocate.
 *
 * \return SHR_E_NONE on success.
 * \return SHR_E_MEMORY on failure.
 */
static int allocate_working_buffer(int unit, size_t size)
{
    SHR_FUNC_ENTER(unit);

    /* Allocate the working buffer for each unit */
    SHR_ALLOC(bcmimm_working_buffers[unit].buff, size, "bcmimmWB");
    SHR_NULL_CHECK(bcmimm_working_buffers[unit].buff, SHR_E_MEMORY);

    bcmimm_working_buffers[unit].sync = sal_sem_create("bcmimmWB",
                                                       SAL_SEM_BINARY, 1);
    SHR_NULL_CHECK(bcmimm_working_buffers[unit].sync, SHR_E_MEMORY);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Free working buffer
 *
 * This function free previously allocated working buffer and its resources.
 *
 * \param [in] unit The unit associated with the working buffer.
 *
 * \return None.
 */
static void free_working_buffer(int unit)
{
    if (bcmimm_working_buffers[unit].buff) {
        SHR_FREE(bcmimm_working_buffers[unit].buff);
    }
    if (bcmimm_working_buffers[unit].sync) {
        sal_sem_destroy(bcmimm_working_buffers[unit].sync);
        bcmimm_working_buffers[unit].sync = NULL;
    }
}

/*!
 * \brief Determine the minimum table index based on key fields minimum values.
 *
 * This function goes through the key fields and construct the minimum table
 * index based on the minimum value of every key field.
 *
 * \param [in,out] dyn_info Contains the key fields attributes (for input) and
 * the table minimum index (for output).
 * \param [in] tbl Information about the LT
 */
static void determine_min_idx(bcmimm_table_dynamic_info_t *dyn_info,
                              table_info_t *tbl)
{
    field_info_t *fld = dyn_info->key_fields;
    uint32_t min_idx = 0;
    uint32_t offset = 0;
    uint32_t j;

    for (j = 0; j < tbl->key_fld_cnt && fld; j++, fld++) {
        min_idx |= fld->min_val << offset;
        offset += fld->width;
    }
    dyn_info->min_idx = min_idx;
}

/*!
 * \brief Initializes in-memory logical table
 *
 * This function being called to initialize an in-memory LT for cold start.
 * The function parses all the fields to determine the size of the key buffer
 * and the maximal size required to host all the data fields. It then
 * initializes all the data and key fields array with the field IDs and
 * field width.
 *
 * \param [in] unit Is the unit associated with the blocks.
 * \param [in] tname is the logical table name.
 * \param [in] warm Indicates if it is warm boot.
 * \param [in] retain_cont Indicates if there is a desire to retain the same
 * table info indexes as before. This feature is useful when a device reattached
 * as the underline HA memory may not be freed, resulting in pre-allocated
 * Ha blocks that will retain their size (like warm boot). If the table info
 * indexes will not be retained then the front end might try to allocate a
 * back-end table with a certain size (based on incremental sub module ID) but
 * that back-end table had already been allocated in previous run with different
 * size.
 * \param [in] direct_tbl Indicating if this table is direct table or not.
 * \param [out] tbl_info Is the newly created pointer to the table information.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int table_init(int unit,
                      uint32_t sid,
                      bool warm,
                      bool retain_cont,
                      bool direct_tbl,
                      table_info_t **tbl_info)
{
    size_t num_fid;
    bcmlrd_client_field_info_t *f_info = NULL;
    const bcmlrd_table_rep_t *lt_info;
    table_info_t *t_info = NULL;
    bcmlrd_table_attrib_t tbl_attr;
    bcmimm_table_dynamic_info_t *dyn_info;

    SHR_FUNC_ENTER(unit);
    lt_info = bcmlrd_table_get(sid);
    SHR_NULL_CHECK(lt_info, SHR_E_BADID);
    num_fid = lt_info->fields;

    SHR_IF_ERR_EXIT
        (bcmlrd_table_attributes_get(unit, lt_info->name, &tbl_attr));

    SHR_ALLOC(f_info, sizeof(bcmlrd_client_field_info_t) * num_fid,
              "bcmimmInit");
    SHR_NULL_CHECK(f_info, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT
        (bcmlrd_table_fields_def_get(unit, lt_info->name,
                                     num_fid, f_info, &num_fid));

    t_info = bcmimm_tbl_find(unit, sid, NULL);
    if (!warm || !t_info) {
        uint16_t tmp_idx;
        shr_ha_sub_id sub;
        if (retain_cont) {
            t_info = bcmimm_tbl_find(unit, sid, NULL);
        }
        if (!t_info) {
            t_info = ha_table_block_alloc(unit);
            sub = 0;
        } else {
            sub = t_info->sub;
        }
        SHR_NULL_CHECK(t_info, SHR_E_INTERNAL);
        tmp_idx = t_info->fields_idx;
        sal_memset(t_info, 0, sizeof(*t_info));
        t_info->fields_idx = tmp_idx;
        if (retain_cont) {
            t_info->sub = sub;
        }
    }

    t_info->key_fld_cnt = 0;
    t_info->key_arry_size = 0;
    t_info->data_fld_cnt = 0;

    t_info->sid = sid;
    t_info->interactive = tbl_attr.interactive;
    t_info->type = (direct_tbl ? IMM_DIRECT : IMM_BASIC);

    SHR_IF_ERR_EXIT(bcmimm_table_dimension_find(unit, t_info, num_fid, f_info));
    SHR_IF_ERR_EXIT(alloc_table_dynamic_info(unit, t_info));

    dyn_info = &bcmimm_dynamic_info[unit][t_info->fields_idx];
    set_field_attrib(t_info, dyn_info, f_info, num_fid);
    dyn_info->tbl_name = lt_info->name;
    dyn_info->index_alloc = lt_info->flags & BCMLRD_TABLE_F_TYPE_INDEX_ALLOC;

    /* For direct tables it is important to find the minimum index */
    if (t_info->type == IMM_DIRECT) {
        determine_min_idx(dyn_info, t_info);
    } else {
        dyn_info->min_idx = 0;
    }
    *tbl_info = t_info;
exit:
    SHR_FREE(f_info);
    if (SHR_FUNC_ERR() && t_info != NULL) {
        ha_table_last_block_free(unit, t_info);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Free the tables resources for all tables of a unit.
 *
 * This function goes through all the LT of a given unit and frees the LT
 * resources.
 *
 * \param [in] unit Is the unit associated with the blocks.
 * \param [in] idx Is the number of IMM LTs that were initialized for this unit.
 *
 * \return None.
 */
static void free_tbls_resources(int unit, uint32_t idx)
{
    uint32_t j;
    table_info_t *tbl;
    bcmimm_table_dynamic_info_t *dyn_info;

    for (j = 0; j <= idx; j++) {
        tbl = &bcmimm_tables_list[unit]->tbl[j];
        dyn_info = &bcmimm_dynamic_info[unit][tbl->fields_idx];
        SHR_FREE(dyn_info->data_fields);
        SHR_FREE(dyn_info->key_fields);
        if (tbl->blk_hdl != BCMIMM_BE_INVALID_TBL_HDL) {
            bcmimm_be_table_release(tbl->blk_hdl);
            tbl->blk_hdl = BCMIMM_BE_INVALID_TBL_HDL;
            imm_fe_sub_id[unit]--;
        }
    }
}

/*!
 * \brief Warm Initialization of in-memory logical tables
 *
 * This function being called to initialize an in-memory LTs in warm boot.
 * Unlike cold boot, the tables had already been initialized and resides in
 * HA memory. What left is to retrieve these tables and to allocate the
 * resources that are stored in regular memory.
 *
 * \param [in] unit Is the unit associated with the blocks.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int init_warm(int unit)
{
    uint32_t idx;
    table_info_t *tbl;
    size_t num_fid;
    bcmlrd_client_field_info_t *f_info = NULL;
    size_t key_size;
    size_t max_entry_size = 0;
    bcmimm_table_dynamic_info_t *dyn_info;
    const bcmlrd_table_rep_t *lrd_tbl_info;

    SHR_FUNC_ENTER(unit);
    /* Tables_list for this unit was already allocated */
    for (idx = 0; idx < bcmimm_tables_list[unit]->allocated; idx++) {
        tbl = &bcmimm_tables_list[unit]->tbl[idx];

        dyn_info = &bcmimm_dynamic_info[unit][tbl->fields_idx];
        dyn_info->data_fields = NULL;
        if (tbl->data_fld_cnt > 0) {
            SHR_ALLOC(dyn_info->data_fields,
                      sizeof(field_info_t) * tbl->data_fld_cnt,
                      "bcmimmDataFld");
            SHR_NULL_CHECK(dyn_info->data_fields, SHR_E_MEMORY);
        }

        dyn_info->key_fields = NULL;
        lrd_tbl_info = bcmlrd_table_get(tbl->sid);
        SHR_NULL_CHECK(lrd_tbl_info, SHR_E_INTERNAL);
        dyn_info->tbl_name = lrd_tbl_info->name;
        dyn_info->index_alloc =
            lrd_tbl_info->flags & BCMLRD_TABLE_F_TYPE_INDEX_ALLOC;
        if (tbl->key_fld_cnt > 0) {
            SHR_ALLOC(dyn_info->key_fields,
                      sizeof(field_info_t) * tbl->key_fld_cnt,
                      "bcmimmKeyFld");
            SHR_NULL_CHECK(dyn_info->key_fields, SHR_E_MEMORY);
            key_size = BCMIMM_BITS_TO_BYTES(tbl->key_arry_size);
        } else {
            key_size = 1;  /* Key size must be minimum 1 byte */
        }

        /* Now allocate the backend block */
        if (tbl->type == IMM_BASIC) {
            SHR_IF_ERR_EXIT
                (bcmimm_be_table_create(unit,
                                        imm_fe_comp_id,
                                        imm_fe_sub_id[unit]++,
                                        key_size,
                                        tbl->f_data_size,
                                        1,  /* The size is already set in WB */
                                        true,
                                        NULL,
                                        bcmimm_big_endian,
                                        (bcmimm_be_tbl_hdl_t *)&tbl->blk_hdl));
        } else {
            SHR_IF_ERR_EXIT
                (bcmimm_be_direct_table_create(unit,
                                        imm_fe_comp_id,
                                        imm_fe_sub_id[unit]++,
                                        tbl->max_num_field,
                                        1,  /* The size is already set in WB */
                                        0, /* The start idx is kept in the BE*/
                                        tbl->array_idx_size > 0,
                                        true,
                                        NULL,
                                        (bcmimm_be_tbl_hdl_t *)&tbl->blk_hdl));
        }

        SHR_IF_ERR_EXIT(bcmlrd_field_count_get(unit, tbl->sid, &num_fid));
        SHR_ALLOC(f_info, sizeof(bcmlrd_client_field_info_t) * num_fid,
                  "bcmimmTblInfo");
        SHR_NULL_CHECK(f_info, SHR_E_MEMORY);
        SHR_IF_ERR_EXIT(bcmlrd_table_fields_def_get(unit,
                                                    dyn_info->tbl_name, num_fid,
                                                    f_info, &num_fid));

        set_field_attrib(tbl, dyn_info, f_info, num_fid);
        SHR_FREE(f_info);  /* Can be optimized by checking the desired size */

        if (max_entry_size < tbl->data_bits_len) {
            max_entry_size = tbl->data_bits_len;
        }
    }

    max_entry_size += BUFF_HEADER_SIZE + sizeof(uint64_t);
    SHR_IF_ERR_EXIT(allocate_working_buffer(unit, max_entry_size));

exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(f_info);
        free_tbls_resources(unit, idx);
        free_working_buffer(unit);
    }
    SHR_FUNC_EXIT();
}


/*!
 * \brief Obtain the LTIDs associated with the IMM component.
 *
 * This function obtains an array of LTIDs that are associated with the IMM
 * component.
 *
 * \param [in] unit Associated with the request.
 * \param [out] table Array of IMM LTIDs.
 * \param [out] imm_lts IMM LTIDs count of the basic handler.
 * \param [out] imm_direct IMM LTIDs count of the direct hander.
 *
 * \return SHR_E_NONE - Success.
 * \return SHR_E_MEMORY - Failued to allocate memory
 * \return SHR_E_INTERNAL - Conflicting LRD output numbers.
 */
static int obtain_imm_lt_ids(int unit,
                             bcmlrd_sid_t **tables,
                             size_t *imm_lts,
                             size_t *imm_direct)
{
    size_t total_lts, actual;
    bcmlrd_sid_t *tbls = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlrd_custom_tables_count_get(unit, "bcmimm", "basic", imm_lts));
    SHR_IF_ERR_EXIT
        (bcmlrd_custom_tables_count_get(unit, "bcmimm", "direct", imm_direct));
    if ((*imm_lts) + (*imm_direct) == 0) {
        SHR_EXIT();  /* Possible not to have IMM tables */
    }
    total_lts = (*imm_lts) + (*imm_direct);

    SHR_ALLOC(tbls, sizeof(bcmlrd_sid_t) * total_lts, "bcmimmTbls");
    SHR_NULL_CHECK(tbls, SHR_E_MEMORY);

    SHR_IF_ERR_EXIT
        (bcmlrd_custom_tables_get(unit, "bcmimm", "basic",
                                  (*imm_lts), tbls, &actual));
    if (actual != (*imm_lts)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    if ((*imm_direct) > 0) {
        SHR_IF_ERR_EXIT
            (bcmlrd_custom_tables_get(unit, "bcmimm", "direct",
                                      (*imm_direct), &tbls[(*imm_lts)], &actual));
        if (actual != (*imm_direct)) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }
exit:
    if (SHR_FUNC_ERR()) {
        /* Clean the table information. */
        SHR_FREE(tbls);
    } else {
        *tables = tbls;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initializes all IMM tables resources.
 *
 * This function go through each LT and determine the required resources
 * for that table. It makes the first connection between the table structures
 * kept in HA memory and initializes them. It also allocates dynamic info
 * structures that can change based on configuration.
 *
 * \param [in] unit Associated with this set of LTs.
 * \param [in] warm Indicates cold or warm boot.
 * \param [in] issu Indicates if this is ISSU start or normal start.
 * \param [in] retain_count Indicates if new structures should be created
 * or only can be reused.
 * \param [in] basic_imm_lts Number of IMM LTs with basic handler.
 * \param [in] total_lts Total number of IMM LTs. This number is the sum of
 * \c basic_imm_lts and the number of IMM LTs that uses direct handler.
 * \param [in] tables An array of all the IMM LT IDs.
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_MEMORY Failed to allocate memory.
 */
static int allocate_tables_resources(int unit,
                                     bool warm,
                                     bool issu,
                                     bool retain_cont,
                                     size_t basic_imm_lts,
                                     size_t total_lts,
                                     bcmlrd_sid_t *tables)
{
    uint32_t j;
    size_t total_data_len = 0;
    size_t total_key_len = 0;
    size_t max_data_len = 0;
    size_t aux_size;
    table_info_t *tbl_info = NULL;
    bcmlrd_map_t const *map;

    SHR_FUNC_ENTER(unit);
    /*
     * This loop calculates the resources require by every table. It than
     * adds the buffer resources to the total required by all tables.
     * These totals will be used when allocating the generic data and
     * key buffers.
     */
     for (j = 0; j < total_lts; j++) {
         /* Verify that the table is mapped. Continue if not */
         if (bcmlrd_map_get(unit,
                            tables[j],
                            &map) == SHR_E_UNAVAIL) {
             continue;
         }
         SHR_IF_ERR_EXIT
             (table_init(unit, tables[j], warm, retain_cont,
                         j >=  basic_imm_lts, &tbl_info));
         /* Track the largest data length. Used for auxiliary calculation. */
         if (max_data_len < tbl_info->data_bits_len) {
             max_data_len = tbl_info->data_bits_len;
         }
         total_data_len += tbl_info->data_bits_len + BUFF_HEADER_SIZE;
         if (tbl_info->key_fld_cnt > 0) {
             total_key_len += BCMIMM_BITS_TO_BYTES(tbl_info->key_arry_size);
         } else {
             total_key_len++;
         }
         /*
          * Make sure that the free sub list will not overlap with
          * pre-allocated subs.
          */
         if (imm_fe_sub_id[unit] <= tbl_info->sub) {
             imm_fe_sub_id[unit] = tbl_info->sub + 1;
         }
     }

     /* Auxiliary table entries includes data + buffer header + key size. */
     aux_size = max_data_len + BUFF_HEADER_SIZE + sizeof(uint64_t);

     SHR_IF_ERR_EXIT(allocate_working_buffer(unit, aux_size));

     /* Now allocate the generic HA buffer blocks */
     SHR_IF_ERR_EXIT(ha_buff_blk_alloc(unit,
                                       total_data_len,
                                       total_key_len,
                                       aux_size * MAX_AUX_ENTRIES,
                                       warm,
                                       issu));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Allocate back end tables to every IMM table.
 *
 * This function allocates back-end tables based on each LT resources as
 * it was calculated by \ref allocate_tables_resources().
 *
 * \param [in] unit Associated with this set of LTs.
 * \param [in] warm Indicates cold or warm boot.
 * \param [in] issu Indicates if this is ISSU start or normal start.
 * \param [in] tables An array of all the IMM LT IDs.
 * \param [in] basic_imm_lts Number of IMM LTs with basic handler.
 * \param [in] total_lts Total number of IMM LTs. This number is the sum of
 * \c basic_imm_lts and the number of IMM LTs that uses direct handler.
 */
static int allocate_tables_be(int unit,
                              bool warm,
                              bool issu,
                              bcmlrd_sid_t *tables,
                              size_t basic_imm_lts,
                              size_t total_lts)
{
    uint32_t j;
    uint32_t k;
    table_info_t *tbl_info = NULL;
    bcmlrd_map_t const *map;
    int rv;
    size_t rows;
    size_t key_size;
    shr_ha_sub_id sub_id;
    bool basic_tbl;
    bcmimm_table_dynamic_info_t *dyn_info;

    SHR_FUNC_ENTER(unit);
    /*
     * Allocate the buffers for each table and create all the back-end
     * tables.
     */
     for (j = 0; j < total_lts; j++) {
         /* Verify that the table is mapped. Continue if not */
         if (bcmlrd_map_get(unit,
                            tables[j],
                            &map) == SHR_E_UNAVAIL) {
             continue;
         }

         /* Find the entry limit attribute value */
         rows = 0;
         for (k = 0; k < map->table_attr->attributes; k++) {
             if (map->table_attr->attr[k].key ==
                     BCMLRD_MAP_TABLE_ATTRIBUTE_ENTRY_LIMIT) {
                 rows = map->table_attr->attr[k].value;
                 break;
             }
         }

         tbl_info = bcmimm_tbl_find(unit, tables[j], NULL);
         assert(tbl_info);
         assert(tbl_info->sid == tables[j]);
         SHR_IF_ERR_EXIT(bcmimm_table_buffers_init(unit, tbl_info));
         dyn_info = &bcmimm_dynamic_info[unit][tbl_info->fields_idx];

         /* Find if the table is update only table */
         if ((map->entry_ops & BCMLRD_MAP_TABLE_ENTRY_OPERATION_INSERT) == 0 &&
             (map->entry_ops & BCMLRD_MAP_TABLE_ENTRY_OPERATION_DELETE) == 0 &&
             (map->entry_ops & BCMLRD_MAP_TABLE_ENTRY_OPERATION_UPDATE))
         {
             dyn_info->update_only = true;
         } else {
             dyn_info->update_only = false;
         }

         if (j < basic_imm_lts) {
             basic_tbl = true;
             tbl_info->type = IMM_BASIC;
         } else {
             basic_tbl = false;
             tbl_info->type = IMM_DIRECT;
         }

         tbl_info->entry_limit = rows;
         if (basic_tbl) { /* Save some space for basic tables */
             if (rows == 0) {
                 LOG_VERBOSE(BSL_LOG_MODULE,
                             (BSL_META_U(unit,
                                         "table %s entry_limit was not specified"
                                         "\n"),
                              dyn_info->tbl_name));
                 rows = 30;
             } else if (rows > 200 && rows < 1000) {
                 rows /= 4;
             } else if (rows > 1000) {
                 rows /= 8;
             }
         }

         if (tbl_info->sub != 0) {
             sub_id = tbl_info->sub; /* Reuse the same sub ID */
         } else {
             tbl_info->sub = imm_fe_sub_id[unit];
             sub_id = imm_fe_sub_id[unit]++;
         }
         if (tbl_info->key_fld_cnt == 0) {
             key_size = 1;  /* Key size must be minimum 1 byte */
         } else {
             key_size = BCMIMM_BITS_TO_BYTES(tbl_info->key_arry_size);
         }
         if (basic_tbl) {
             rv = bcmimm_be_table_create
                 (unit,
                  imm_fe_comp_id,
                  sub_id,
                  key_size,
                  tbl_info->f_data_size,
                  rows,
                  warm,
                  dyn_info->tbl_name,
                  bcmimm_big_endian,
                  (bcmimm_be_tbl_hdl_t *)&tbl_info->blk_hdl);
         }  else {
             rv = bcmimm_be_direct_table_create
                 (unit,
                  imm_fe_comp_id,
                  sub_id,
                  tbl_info->max_num_field,
                  rows,
                  dyn_info->min_idx,
                  tbl_info->array_idx_size > 0,
                  warm,
                  dyn_info->tbl_name,
                  (bcmimm_be_tbl_hdl_t *)&tbl_info->blk_hdl);
         }
         if (SHR_FAILURE(rv)) {
             if (!issu || rv != SHR_E_EXISTS) {
                 SHR_ERR_EXIT(rv);
             }
         }
     }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialization of all in-memory logical tables for a specific unit.
 *
 * This function is being called to initialize all the in-memory LTs associated
 * with a particular unit.
 *
 * \param [in] unit Is the unit to initialize all its associated in-memory
 * tables.
 * \param [in] warm Indicates if the init is cold or warm.
 * \param [in] issu Indicates if there is an on-going issu activity.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int init_unit_tables(int unit, bool warm, bool issu)
{
    size_t imm_lts, imm_direct, total_lts;
    bcmlrd_sid_t *tables = NULL;
    bool retain_cont = false;
    size_t actual_lts;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(obtain_imm_lt_ids(unit, &tables, &imm_lts, &imm_direct));
    total_lts = imm_lts + imm_direct;
    actual_lts = total_lts;
    SHR_IF_ERR_EXIT(ha_blk_table_alloc(unit, tables, &warm,
                                       &retain_cont, &actual_lts));
    SHR_ALLOC(bcmimm_dynamic_info[unit],
              sizeof(bcmimm_table_dynamic_info_t) * actual_lts,
              "bcmimmDynInfo");
    SHR_NULL_CHECK(bcmimm_dynamic_info[unit], SHR_E_MEMORY);
    sal_memset(bcmimm_dynamic_info[unit], 0,
               sizeof(bcmimm_table_dynamic_info_t) * actual_lts);

    imm_fe_sub_id[unit] = MIN_SUB_ID;

    /* Start by allocating global resources */
    SHR_ALLOC(bcmimm_tables_cb[unit],
              actual_lts * sizeof(bcmimm_lt_event_t),
              "bcmimmCb");
    SHR_NULL_CHECK(bcmimm_tables_cb[unit], SHR_E_MEMORY);
    sal_memset(bcmimm_tables_cb[unit], 0,
               actual_lts * sizeof(bcmimm_lt_event_t));

    bcmimm_be_init(unit, BCMMGMT_IMM_BE_COMP_ID, warm);
    if (warm && !issu) {
        /*
         * Now allocate the generic HA buffer blocks. At this point we don't
         * know the desired buffer size. Fortunately, since this is warm boot
         * the HA blocks already have their proper size, so we simply tapping
         * to them.
         */
        SHR_IF_ERR_EXIT(ha_buff_blk_alloc(unit,
                                          1000,
                                          100,
                                          100 * MAX_AUX_ENTRIES,
                                          warm,
                                          issu));
        SHR_IF_ERR_EXIT(init_warm(unit));
        SHR_EXIT();
    }

    if (warm && issu) {
        SHR_IF_ERR_EXIT(bcmimm_issu_upgrade(unit, tables, imm_lts,
                                            imm_direct, &imm_fe_sub_id[unit]));
    }

    SHR_IF_ERR_EXIT(allocate_tables_resources(unit, warm, issu, retain_cont,
                                              imm_lts, total_lts, tables));

    SHR_IF_ERR_EXIT(allocate_tables_be(unit, warm, issu, tables,
                                       imm_lts, total_lts));


exit:
    /* Clean the table information. */
    SHR_FREE(tables);
    if (SHR_FUNC_ERR()) {
        free_working_buffer(unit);
    }
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * External functions local to this module
 */
void bcmimm_set_issu_event(int unit)
{
    issu_event[unit] = true;
}

int bcmimm_unit_init(int unit, uint8_t comp_id, bool warm)
{
    bcmcfg_feature_ctl_config_t feature_conf;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    imm_fe_comp_id = comp_id;
    imm_fe_sub_id[unit] = MIN_SUB_ID;

    rv = bcmcfg_feature_ctl_config_get(&feature_conf);
    if (SHR_FAILURE(rv) || feature_conf.enable_atomic_trans == 0) {
        atomic_trans_ena = false;
        bcmimm_ha_enable_set(false);
    } else {
        atomic_trans_ena = true;
        bcmimm_ha_enable_set(true);
    }

    SHR_IF_ERR_EXIT(init_unit_tables(unit, warm, issu_event[unit]));
    initialized_unit[unit] = true;

exit:
    SHR_FUNC_EXIT();
}

int bcmimm_unit_shutdown(int unit)
{
    uint32_t idx;
    table_info_t *tbl;
    bcmimm_dynamic_blk_mgmt_t *blk_hdr;
    bcmimm_dynamic_blk_t *unit_buffs;
    bcmimm_table_dynamic_info_t *dyn_info;

    SHR_FUNC_ENTER(unit);

    if (initialized_unit[unit]) {
        /*
         * The cleanup activity doesn't close or delete HA files. The reason is
         * that this activity will be done by the application if it wants to start
         * cold (i.e. open new HA file). If the application wants warm boot then
         * everything is available.
        */
        for (idx = 0; idx < bcmimm_tables_list[unit]->allocated; idx++) {
            tbl = &bcmimm_tables_list[unit]->tbl[idx];
            /* Close the back-end table */
            if (tbl->blk_hdl != BCMIMM_BE_INVALID_TBL_HDL) {
                SHR_IF_ERR_EXIT(bcmimm_be_table_release(tbl->blk_hdl));
                dyn_info = &bcmimm_dynamic_info[unit][tbl->fields_idx];
                SHR_FREE(dyn_info->key_fields);
                SHR_FREE(dyn_info->data_fields);
            }
        }
        SHR_FREE(bcmimm_dynamic_info[unit]);

        unit_buffs = &bcmimm_dynamic_buffs[unit];
        blk_hdr = &unit_buffs->data_blk;
        sal_mutex_destroy(blk_hdr->sync_obj);
        if (atomic_trans_ena) {
            blk_hdr = &unit_buffs->aux_blk;
            sal_mutex_destroy(blk_hdr->sync_obj);
        }
        blk_hdr = &unit_buffs->trans_blk;
        sal_mutex_destroy(blk_hdr->sync_obj);
        blk_hdr = &unit_buffs->key_blk;
        sal_mutex_destroy(blk_hdr->sync_obj);

        free_working_buffer(unit);

        SHR_FREE(bcmimm_tables_cb[unit]);
    }

exit:
    initialized_unit[unit] = false;
    SHR_FUNC_EXIT();
}

uint8_t *bcmimm_working_buff_alloc(int unit)
{
    sal_sem_take(bcmimm_working_buffers[unit].sync, SAL_SEM_FOREVER);
    return bcmimm_working_buffers[unit].buff;
}

void bcmimm_working_buff_free(int unit)
{
    sal_sem_give(bcmimm_working_buffers[unit].sync);
}

bool bcmimm_unit_is_active(int unit)
{
    if (unit < 0 || unit >= BCMDRD_CONFIG_MAX_UNITS) {
        return false;
    }
    return initialized_unit[unit];
}

/*******************************************************************************
 * Public functions
 */
int bcmimm_basic_init(int unit,
                      const bcmltd_handler_lifecycle_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_FUNC_EXIT();
}

int bcmimm_basic_cleanup(int unit,
                         const bcmltd_handler_lifecycle_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_FUNC_EXIT();
}
