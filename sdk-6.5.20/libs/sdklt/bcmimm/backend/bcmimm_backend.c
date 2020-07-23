/*! \file bcmimm_backend.c
 *
 * In-Memory back-end implementation.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_assert.h>
#include <sal/sal_rwlock.h>
#include <sal/sal_mutex.h>
#include <shr/shr_debug.h>
#include <shr/shr_util.h>
#include <shr/shr_ha.h>
#include <shr/shr_hash.h>
#include <bcmdrd_config.h>
#include <bcmissu/issu_api.h>
#include <bcmimm/bcmimm_backend.h>
#include <bcmimm/generated/back_e_ha.h>
#include "be_internals.h"

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMIMM_BACKEND

/* The first sub component to use. Do not use 0 since this considered NULL */
#define MIN_SUB_COMP        1

#define ROUND_TO_WORD(x) (((x) + 3) / 4) * 4

#define VALIDATE_INPUT(t_info, hdl) \
    if ((hdl >> 16) >= BCMDRD_CONFIG_MAX_UNITS) {                   \
        SHR_ERR_EXIT(SHR_E_PARAM);                           \
    }                                                               \
    if (((hdl & 0xFFFF) >= active_tables[hdl >> 16].array_size) || \
        !t_info || !t_info->tbl ||                                  \
        (t_info->tbl->signature != BCMIMM_BLK_SIGNATURE)) {         \
        SHR_ERR_EXIT(SHR_E_PARAM);                           \
    }

#define DATA_TABLE_CTRL_CHUNK   10

/*
 * per unit we keep all the tables in an array that points to them. The table
 * handle contains the index in this array. This allows us to quickly find
 * the relevant table.
*/
typedef struct {
    uint32_t array_size;
    uint32_t active_tbls;
    bcmimm_tbl_info_t *tbls;
} tables_array_t;

shr_ha_mod_id bcmimm_be_comp_id;

static tables_array_t active_tables[BCMDRD_CONFIG_MAX_UNITS];

shr_ha_sub_id bcmimm_available_subs[BCMDRD_CONFIG_MAX_UNITS];

bcmimm_free_list_t *bcmimm_free_elem[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Allocate active table control structure.
 *
 * This function allocates (or extends) the array size that points to the
 * currently active tables.
 *
 * \param [in] unit This is the unit associated with this table.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int active_table_alloc(int unit)
{
    tables_array_t *act_tbl = &active_tables[unit];
    uint32_t new_size;
    uint32_t curr_size = act_tbl->array_size * sizeof(bcmimm_tbl_info_t);

    if (act_tbl->active_tbls == act_tbl->array_size) {
        new_size = curr_size +
            DATA_TABLE_CTRL_CHUNK * sizeof(bcmimm_tbl_info_t);

        act_tbl->tbls = shr_util_realloc(act_tbl->tbls,
                                         curr_size,
                                         new_size,
                                         "bcmimmBeTblInf");
        if (act_tbl->tbls == NULL) {
            return SHR_E_MEMORY;
        }
        sal_memset(((uint8_t *)act_tbl->tbls) + curr_size,
                   0,
                   new_size - curr_size);
        act_tbl->array_size += DATA_TABLE_CTRL_CHUNK;
    }
    return SHR_E_NONE;
}

/*!
 * \brief Indicator if a specific table already exist in the active tables.
 *
 * This function searches the active tables of a given unit for a table that
 * matches the module and sub-module IDs.
 *
 * \param [in] unit This is the unit associated with this table.
 * \param [in] comp_id Is the component ID to match.
 * \param [in] sub_id Is the sub-component ID to match.
 *
 * \return True if the table exist and false otherwise.
 */
static bool table_exist(int unit, shr_ha_mod_id comp_id, shr_ha_sub_id sub_id)
{
    tables_array_t *act_tbl = &active_tables[unit];
    uint32_t j;
    tbl_header_t *tbl;

    for (j = 0; j < act_tbl->active_tbls; j++) {
        tbl = act_tbl->tbls[j].tbl;
        if (tbl && (tbl->comp_id == comp_id) && (tbl->sub_id == sub_id)){
            return true;
        }
    }
    return false;
}

/*******************************************************************************
 * Public functions
 */
int bcmimm_be_init(int unit, shr_ha_mod_id comp_id, bool warm)
{

    SHR_FUNC_ENTER(unit);

    bcmimm_available_subs[unit] = MIN_SUB_COMP;
    bcmimm_be_comp_id = comp_id;
    bcmimm_free_lists_retrieve(unit, MIN_SUB_COMP);

    SHR_FUNC_EXIT();
}

int bcmimm_be_cleanup(int unit)
{
    tables_array_t *act_tbl = &active_tables[unit];
    bcmimm_free_list_t *free_list;
    bcmimm_free_list_t *tmp;
    uint32_t j;

    SHR_FUNC_ENTER(unit);

    /* Find if all the tables has been destroyed */
    for (j = 0; j < act_tbl->active_tbls; j++) {
        if (act_tbl->tbls[j].tbl) {
            SHR_ERR_EXIT(SHR_E_BUSY);
        }
    }

    if (act_tbl->tbls) {
        SHR_FREE(act_tbl->tbls);
        act_tbl->active_tbls = 0;
        act_tbl->array_size = 0;
    }

    bcmimm_available_subs[unit] = MIN_SUB_COMP;  /* Reset the sub component */
    free_list = bcmimm_free_elem[unit]; /* Clear the free field list */
    bcmimm_free_elem[unit] = NULL;

    while (free_list) {
        sal_mutex_destroy(free_list->lock);
        tmp = free_list;
        free_list = free_list->next;
        sal_free(tmp);
    }

exit:
    SHR_FUNC_EXIT();
}

int bcmimm_be_table_create(int unit,
                           shr_ha_mod_id comp_id,
                           shr_ha_sub_id sub_id,
                           size_t key_len,
                           size_t data_len,
                           size_t rows,
                           bool warm,
                           const char *tbl_name,
                           bool big_endian,
                           bcmimm_be_tbl_hdl_t *hdl)
{
    uint32_t entry_len;
    uint32_t len;
    tbl_header_t *tbl;
    tables_array_t *act_tbl = &active_tables[unit];
    bcmimm_tbl_info_t *t_info;
    uint32_t idx;
    uint32_t field_len;
    char ha_id[MAX_BLK_ID_STR_LEN];

    SHR_FUNC_ENTER(unit);
    /* Verify that the same table was not already been created */
    if (table_exist(unit, comp_id, sub_id)) {
        SHR_ERR_EXIT(SHR_E_EXISTS);
    }
    entry_len = ROUND_TO_WORD(key_len +
                              (BCMIMM_ELEM_PTR_SIZE * 2) +
                              BCMIMM_ELEM_REF_CNT_SIZE);
    len = sizeof(tbl_header_t) + entry_len * rows;

    SHR_IF_ERR_EXIT(active_table_alloc(unit));

    for (idx = 0; idx < act_tbl->active_tbls; idx++) {
        if (!act_tbl->tbls[idx].tbl) {
            break;
        }
    }
    if (idx == act_tbl->active_tbls) {
        idx = act_tbl->active_tbls++;
    }
    t_info = &act_tbl->tbls[idx];

    /* Set the entry free list */
    t_info->entry_free_list = bcmimm_free_list_find(unit,
                                             entry_len,
                                             &bcmimm_free_elem[unit]);
    SHR_NULL_CHECK(t_info->entry_free_list, SHR_E_MEMORY);

    /* Field length contains the data size + field ID + element pointer (HA) */
    field_len = ROUND_TO_WORD(data_len +
                              BCMIMM_ELEM_PTR_SIZE +
                              BCMIMM_BE_FID_SIZE);

    /* Set the field free list */
    t_info->field_free_list = bcmimm_free_list_find(unit,
                                             field_len,
                                             &bcmimm_free_elem[unit]);
    SHR_NULL_CHECK(t_info->field_free_list, SHR_E_MEMORY);

    t_info->unit = unit;
    t_info->lock = sal_rwlock_create("IMM_BE");
    SHR_NULL_CHECK(t_info->lock, SHR_E_MEMORY);
    t_info->sorted_fid = NULL;
    t_info->fid_array_size = 0;

    if (!warm) {
        SHR_IF_ERR_EXIT
            (shr_ha_str_to_id(tbl_name,
                              SHR_HA_BLK_USAGE_LT,
                              MAX_BLK_ID_STR_LEN,
                              ha_id));
    }
    tbl = shr_ha_mem_alloc(unit,
                           comp_id,
                           sub_id,
                           (warm ? NULL : ha_id),
                           &len);
    SHR_NULL_CHECK(tbl, SHR_E_MEMORY);
    t_info->tbl = tbl;

    /*
     * It is possible that we are coming to this function after this table
     * was already created. When we shut down we keep the HA information
     * in tact so we can recover for warm boot. However, if it is cold boot
     * and the block signature matches (i.e. valid block) then we need to
     * first clean up the old dynamic data and entry elements and return them
     * back to their respective free lists.
     */
    if (!warm && tbl->signature == BCMIMM_BLK_SIGNATURE) {
        bcmimm_table_content_clear(t_info);
    }
    if (!warm || tbl->signature != BCMIMM_BLK_SIGNATURE) {
        sal_memset(tbl, 0, len);
        tbl->comp_id = comp_id;
        tbl->sub_id = sub_id;
        tbl->data_len = data_len;
        tbl->key_len = key_len;
        tbl->num_of_rows = rows;
        tbl->num_of_ent = 0;
        tbl->ent_len = (uint8_t)entry_len;
        tbl->basic_tbl = true;
        tbl->signature = BCMIMM_BLK_SIGNATURE;
        /*
         * Report only for the table header, the rest of the HA block will
         * be handled by the IMM.
         */
        bcmissu_struct_info_report(unit, comp_id, sub_id,
                                   0, sizeof(tbl_header_t), 1,
                                   TBL_HEADER_T_ID);
    } else if (bcmissu_is_active()) {
        if (tbl->data_len < data_len) {
            /* Can only handle data size increase */
            SHR_IF_ERR_EXIT
                (bcmimm_be_update_data_len(unit, t_info, data_len, big_endian));
            t_info->tbl->data_len = data_len;
        }
        if (tbl->key_len != key_len) {
            SHR_IF_ERR_EXIT
                (bcmimm_be_update_key_len(unit, t_info, key_len,
                                          entry_len, big_endian));
            tbl = t_info->tbl;
            tbl->ent_len = (uint8_t)entry_len;
            tbl->key_len = key_len;
        }
        if (tbl->data_len > data_len) {
            if (!tbl->basic_tbl) {
                LOG_WARN(BSL_LOG_MODULE,
                         (BSL_META_U(unit, "Mismatch of table types table=%s\n"),
                          tbl_name));
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
        }
    } else if (tbl->data_len != data_len || tbl->key_len != key_len) {
        /*
         * In warm boot the table internals must match the requested sizes.
         */
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                     "Mismatch of table sub_id between cold and warm boot"
                     " table %s\n"), tbl_name));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }


    *hdl = (bcmimm_be_tbl_hdl_t)
                    ((((shr_ha_mod_id)(unit & 0xFFFF)) << 16) + idx);
exit:
    SHR_FUNC_EXIT();
}

int bcmimm_be_direct_table_create(int unit,
                                  shr_ha_mod_id comp_id,
                                  shr_ha_sub_id sub_id,
                                  size_t data_fields,
                                  size_t rows,
                                  uint32_t start_idx,
                                  bool array,
                                  bool warm,
                                  const char *tbl_name,
                                  bcmimm_be_tbl_hdl_t *hdl)
{
    uint32_t entry_len;
    uint32_t idx;
    tables_array_t *act_tbl = &active_tables[unit];
    bcmimm_tbl_info_t *t_info;
    tbl_header_t *hdr;
    uint32_t len, actual;
    direct_tbl_basic_t *basic_tbl;
    direct_tbl_array_t *array_tbl;
    void *buf;
    char ha_id[MAX_BLK_ID_STR_LEN];

    SHR_FUNC_ENTER(unit);
    /* Verify that the same table was not already been created */
    if (table_exist(unit, comp_id, sub_id)) {
        SHR_ERR_EXIT(SHR_E_EXISTS);
    }
    if (array) {
        entry_len = sizeof(direct_tbl_array_field_t) * data_fields +
            sizeof(direct_tbl_entry_array_t);
        len = entry_len * rows + sizeof(direct_tbl_array_t);
    } else {
        entry_len = sizeof(direct_tbl_basic_field_t) * data_fields +
            sizeof(direct_tbl_entry_basic_t);
        len = entry_len * rows + sizeof(direct_tbl_basic_t);
    }

    SHR_IF_ERR_EXIT(active_table_alloc(unit));

    for (idx = 0; idx < act_tbl->active_tbls; idx++) {
        if (!act_tbl->tbls[idx].tbl) {
            break;
        }
    }
    if (idx == act_tbl->active_tbls) {
        idx = act_tbl->active_tbls++;
    }
    t_info = &act_tbl->tbls[idx];
    t_info->entry_free_list = NULL;
    t_info->field_free_list = NULL;
    t_info->unit = unit;
    t_info->lock = sal_rwlock_create("IMM_BE");
    SHR_NULL_CHECK(t_info->lock, SHR_E_MEMORY);
    t_info->sorted_fid = NULL;
    t_info->fid_array_size = 0;
    actual = len;

    if (!warm) {
        SHR_IF_ERR_EXIT
            (shr_ha_str_to_id(tbl_name,
                              SHR_HA_BLK_USAGE_LT,
                              MAX_BLK_ID_STR_LEN,
                              ha_id));
    }
    buf = shr_ha_mem_alloc(unit,
                           comp_id,
                           sub_id,
                           (warm ? NULL : ha_id),
                           &actual);
    SHR_NULL_CHECK(buf, SHR_E_MEMORY);
    if (actual < len) {
        if (!warm) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
        buf = shr_ha_mem_realloc(unit, buf, len);
        SHR_NULL_CHECK(buf, SHR_E_MEMORY);
    }
    hdr = buf;
    t_info->tbl = hdr;
    if (!warm) {
        sal_memset(buf, 0, actual);
    }
    if (warm && hdr->signature == BCMIMM_BLK_SIGNATURE) {
        if (bcmissu_is_active()) {
            if (hdr->ent_len < entry_len) { /* Need to adjust the entry length */
                bcmimm_be_adjust_direct_entry_length(t_info, entry_len);
                hdr->ent_len = entry_len;
            }
            if (hdr->num_of_rows < rows) {
                uint8_t *new_sect;
                if (hdr->num_of_rows > rows) {
                    SHR_ERR_EXIT(SHR_E_CONFIG);
                }
                /* Initialize all the new rows */
                if (array) {
                    array_tbl = (direct_tbl_array_t *)buf;
                    new_sect = array_tbl->body + (entry_len * hdr->num_of_rows);
                } else {
                    basic_tbl = (direct_tbl_basic_t *)buf;
                    new_sect = basic_tbl->body + (entry_len * hdr->num_of_rows);
                }
                sal_memset(new_sect, 0, entry_len * (rows - hdr->num_of_rows));
                hdr->num_of_rows = rows;
            }
        } else if (hdr->ent_len < entry_len || hdr->num_of_rows < rows) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                         "Parameters mismatch for direct table between cold and warm boot"
                         " table %s\n"), tbl_name));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    } else {
        hdr->basic_tbl = false;
        hdr->comp_id = comp_id;
        hdr->sub_id = sub_id;
        hdr->data_len = data_fields;
        hdr->key_len = 0;
        hdr->num_of_rows = rows;
        hdr->num_of_ent = 0;
        hdr->ent_len = entry_len;
        hdr->array_tbl = array;
        hdr->signature = BCMIMM_BLK_SIGNATURE;
        /*
         * Report only for the table header, the rest of the HA block will
         * be handled by the IMM.
         */
        bcmissu_struct_info_report(unit, comp_id, sub_id,
                                   0, sizeof(tbl_header_t), 1,
                                   TBL_HEADER_T_ID);
        if (array) {
            array_tbl = (direct_tbl_array_t *)buf;
            array_tbl->rows = rows;
            array_tbl->start_idx = start_idx;
            sal_memset(array_tbl->body,  0, entry_len * rows);
        } else {
            basic_tbl = (direct_tbl_basic_t *)buf;
            basic_tbl->rows = rows;
            basic_tbl->start_idx = start_idx;
            sal_memset(basic_tbl->body,  0, entry_len * rows);
        }
    }

    *hdl = (bcmimm_be_tbl_hdl_t)((((uint16_t)(unit & 0xFFFF)) << 16) + idx);
exit:
    SHR_FUNC_EXIT();
}


int bcmimm_be_table_release(bcmimm_be_tbl_hdl_t hdl)
{
    bcmimm_tbl_info_t *t_info = &active_tables[hdl >> 16].tbls[hdl & 0xFFFF];

    SHR_FUNC_ENTER(t_info ? t_info->unit : BSL_UNIT_UNKNOWN);

    VALIDATE_INPUT(t_info, hdl);

    /* Free resources that are not HA resources */
    if (t_info->sorted_fid) {
        sal_free(t_info->sorted_fid);
    }
    if (t_info->traverse_buf) {
        sal_free(t_info->traverse_buf);
    }
    if (t_info->lock) {
        sal_rwlock_destroy(t_info->lock);
    }
    /* Release the table info struct */
    sal_memset(t_info, 0, sizeof(*t_info));

    /* Decrease the number of active tables for this unit */
    active_tables[hdl >> 16].active_tbls--;
exit:
    SHR_FUNC_EXIT();
}

int bcmimm_be_table_free(bcmimm_be_tbl_hdl_t hdl)
{
    bcmimm_tbl_info_t *t_info = &active_tables[hdl >> 16].tbls[hdl & 0xFFFF];
    int unit;

    SHR_FUNC_ENTER(t_info ? t_info->unit : BSL_UNIT_UNKNOWN);

    VALIDATE_INPUT(t_info, hdl);

    unit = t_info->unit;
    SHR_IF_ERR_EXIT(bcmimm_be_table_clear(hdl));

    /* Free resources that are not HA resources */
    sal_rwlock_destroy(t_info->lock);
    if (t_info->sorted_fid) {
        sal_free(t_info->sorted_fid);
    }
    if (t_info->traverse_buf) {
        sal_free(t_info->traverse_buf);
    }

    shr_ha_mem_free(unit, t_info->tbl);

    /* Release the table info struct. This will enable the slot ot be reused */
    sal_memset(t_info, 0, sizeof(*t_info));

    /* Decrease the number of active tables for this unit */
    active_tables[unit].active_tbls--;

exit:
    SHR_FUNC_EXIT();
}

size_t bcmimm_be_table_size_get(bcmimm_be_tbl_hdl_t hdl)
{
    bcmimm_tbl_info_t *t_info = &active_tables[hdl >> 16].tbls[hdl & 0xFFFF];

    if ((hdl >> 16) >= BCMDRD_CONFIG_MAX_UNITS) {
        return 0;
    }
    if (((hdl & 0xFFFF) >= active_tables[hdl >> 16].active_tbls) ||
        !t_info || !t_info->tbl ||
        (t_info->tbl->signature != BCMIMM_BLK_SIGNATURE)) {
        return 0;
    }
    return t_info->tbl->num_of_ent;
}

int bcmimm_be_table_snapshot_create(bcmimm_be_tbl_hdl_t hdl)
{
    bcmimm_tbl_info_t *t_info = &active_tables[hdl >> 16].tbls[hdl & 0xFFFF];
    bool locked = false;

    SHR_FUNC_ENTER(t_info ? t_info->unit : BSL_UNIT_UNKNOWN);

    VALIDATE_INPUT(t_info, hdl);
    /* Take the table lock */
    SHR_IF_ERR_EXIT(sal_rwlock_rlock(t_info->lock, SAL_RWLOCK_FOREVER));
    locked = true;
    if (t_info->traverse_buf) {
        SHR_VERBOSE_EXIT(SHR_E_EXISTS);
    }
    SHR_FREE(t_info->traverse_buf);

    if (!t_info->tbl->basic_tbl) {
        SHR_IF_ERR_EXIT(bcmimm_be_direct_snapshot_create(t_info));
    } else {
        SHR_IF_ERR_EXIT(bcmimm_prep_traverse_buf(t_info));
    }

exit:
    if (locked) {
        sal_rwlock_give(t_info->lock);
    }
    SHR_FUNC_EXIT();
}

void bcmimm_be_table_snapshot_free(bcmimm_be_tbl_hdl_t hdl)
{
    bcmimm_tbl_info_t *t_info = &active_tables[hdl >> 16].tbls[hdl & 0xFFFF];

    sal_rwlock_rlock(t_info->lock, SAL_RWLOCK_FOREVER);
    if (t_info->traverse_buf) {
        sal_free(t_info->traverse_buf);
        t_info->traverse_buf = NULL;
    }
    sal_rwlock_give(t_info->lock);
}

int bcmimm_be_table_get_first(bcmimm_be_tbl_hdl_t hdl,
                              bool use_snapshot,
                              void *key,
                              bcmimm_be_fields_t *out)
{
    bcmimm_tbl_info_t *t_info = &active_tables[hdl >> 16].tbls[hdl & 0xFFFF];
    uint32_t *ptr;
    uint8_t *ent;
    bool locked = false;
    uint32_t start_ptr  = 0;

    SHR_FUNC_ENTER(t_info ? t_info->unit : BSL_UNIT_UNKNOWN);

    VALIDATE_INPUT(t_info, hdl);
    if ((key == NULL) || (out == NULL)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (!t_info->traverse_buf && use_snapshot) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Take the table lock */
    SHR_IF_ERR_EXIT(sal_rwlock_rlock(t_info->lock, SAL_RWLOCK_FOREVER));
    locked = true;
    if (!t_info->tbl->basic_tbl) {
        uint32_t *direct_key = (uint32_t *)key;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmimm_be_direct_entry_get_first(t_info, use_snapshot, direct_key,
                                              &out->count,out->fdata));
        SHR_EXIT();
    }
    if (use_snapshot) {
        ent = bcmimm_next_entry_find(t_info,
                                     true,
                                     NULL);
    } else {
        ent = bcmimm_next_entry_raw_find(t_info,
                                         true,
                                         0,
                                         &start_ptr);
    }
    if (!ent) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }
    sal_memcpy(key, ent, t_info->tbl->key_len);

    ptr = BCMIMM_ENTRY_PTR_GET(ent, t_info->tbl->ent_len);
    ptr--;
    SHR_IF_ERR_EXIT(bcmimm_data_field_get(t_info, *ptr, out));

exit:
    if (locked) {
        sal_rwlock_give(t_info->lock);
    }
    SHR_FUNC_EXIT();
}

int bcmimm_be_table_get_next(bcmimm_be_tbl_hdl_t hdl,
                             bool use_snapshot,
                             void *in_key,
                             void *out_key,
                             bcmimm_be_fields_t *out)
{
    bcmimm_tbl_info_t *t_info = &active_tables[hdl >> 16].tbls[hdl & 0xFFFF];
    uint32_t *ptr;
    uint8_t *ent;
    bool locked = false;

    SHR_FUNC_ENTER(t_info ? t_info->unit : BSL_UNIT_UNKNOWN);

    VALIDATE_INPUT(t_info, hdl);
    if ((in_key == NULL) || (out_key == NULL) || (out == NULL)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (use_snapshot && !t_info->traverse_buf) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Take the table lock */
    SHR_IF_ERR_EXIT(sal_rwlock_rlock(t_info->lock, SAL_RWLOCK_FOREVER));
    locked = true;
    if (!t_info->tbl->basic_tbl) {
        uint32_t direct_key = *(uint32_t *)in_key;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmimm_be_direct_entry_get_next(t_info, use_snapshot, &direct_key,
                                             &out->count, out->fdata));
        *(uint32_t *)out_key = direct_key;
        SHR_EXIT();
    }
    /* Find the next entry and pointer */
    if (use_snapshot) {
        ent = bcmimm_next_entry_find(t_info,
                                     false,
                                     in_key);
    } else {
        uint32_t start_idx;
        uint32_t start_ptr;
        /* Find the previous entry and pointer */
        start_idx = shr_hash_elf(in_key, t_info->tbl->key_len) %
                        t_info->tbl->num_of_rows;
        ent = bcmimm_entry_find(t_info, in_key, &ptr , NULL);
        if (ent) {
            start_ptr = *ptr;
            /* Search from the next element */
            ent = bcmimm_next_entry_raw_find(t_info,
                                             false,
                                             start_idx,
                                             &start_ptr);
        }
    }
    if (!ent) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }
    sal_memcpy(out_key, ent, t_info->tbl->key_len);

    ptr = BCMIMM_ENTRY_PTR_GET(ent, t_info->tbl->ent_len);
    ptr--;
    SHR_IF_ERR_EXIT(bcmimm_data_field_get(t_info, *ptr, out));

exit:
    if (locked) {
        sal_rwlock_give(t_info->lock);
    }
    SHR_FUNC_EXIT();
}

int bcmimm_be_table_clear(bcmimm_be_tbl_hdl_t hdl)
{
    bcmimm_tbl_info_t *t_info = &active_tables[hdl >> 16].tbls[hdl & 0xFFFF];

    SHR_FUNC_ENTER(t_info ? t_info->unit : BSL_UNIT_UNKNOWN);
    VALIDATE_INPUT(t_info, hdl);

    /* Take the table lock */
    if (sal_rwlock_wlock(t_info->lock, SAL_RWLOCK_FOREVER) < 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    if (t_info->tbl->basic_tbl) {
        bcmimm_table_content_clear(t_info);
    } else {
        bcmimm_be_direct_tbl_clear(t_info);
    }

exit:
    sal_rwlock_give(t_info->lock);
    SHR_FUNC_EXIT();
}

uint32_t bcmimm_be_table_cnt_get(bcmimm_be_tbl_hdl_t hdl)
{
    bcmimm_tbl_info_t *t_info = &active_tables[hdl >> 16].tbls[hdl & 0xFFFF];

    if ((hdl >> 16) >= BCMDRD_CONFIG_MAX_UNITS ||
        (hdl & 0xFFFF) >= active_tables[hdl >> 16].array_size) {
        return 0;
    }
    if (!t_info || !t_info->tbl ||
        t_info->tbl->signature != BCMIMM_BLK_SIGNATURE) {
        return 0;
    }
    if (t_info->tbl->basic_tbl) {
        return t_info->tbl->num_of_ent;
    } else {
        return bcmimm_be_direct_tbl_cnt_get(t_info);
    }
}

int bcmimm_be_entry_insert(bcmimm_be_tbl_hdl_t hdl,
                           const void *key,
                           const bcmimm_be_fields_t *in)
{
    bcmimm_tbl_info_t *t_info = &active_tables[hdl >> 16].tbls[hdl & 0xFFFF];
    uint32_t idx;
    uint8_t *entry;
    uint32_t *ent_ptr;
    uint16_t *entry_lock_cnt;
    tbl_header_t *tbl;

    SHR_FUNC_ENTER(t_info ? t_info->unit : BSL_UNIT_UNKNOWN);
    VALIDATE_INPUT(t_info, hdl);

    tbl = t_info->tbl;
    if (!tbl->basic_tbl) {
        uint32_t *direct_key = (uint32_t *)key;
        if (sal_rwlock_wlock(t_info->lock, SAL_RWLOCK_FOREVER) < 0) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        SHR_IF_ERR_EXIT
            (bcmimm_be_direct_entry_insert(t_info, *direct_key,
                                           in->count, in->fdata));
        SHR_EXIT();
    }
    idx = shr_hash_elf(key, tbl->key_len) % tbl->num_of_rows;
    entry = (uint8_t *)tbl + sizeof(tbl_header_t);
    entry += idx * tbl->ent_len;
    ent_ptr = BCMIMM_ENTRY_PTR_GET(entry, tbl->ent_len);
    /* Take the table lock */
    if (sal_rwlock_wlock(t_info->lock, SAL_RWLOCK_FOREVER) < 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (BCMIMM_IS_OCCUPIED(*ent_ptr)) {
        /* Verify that the key is not already there */
        if (sal_memcmp(key, entry, tbl->key_len) == 0) {
            SHR_ERR_EXIT(SHR_E_EXISTS);
        }

        /*
         * The bin is occupied, go throuh the link list until the last
         * element.
         */
        while (!BCMIMM_IS_NULL_PTR(*ent_ptr)) {
            entry = bcmimm_entry_get (t_info->entry_free_list, *ent_ptr);
            if (sal_memcmp(key, entry, tbl->key_len) == 0) {
                SHR_ERR_EXIT(SHR_E_EXISTS);
            }
            ent_ptr = BCMIMM_ENTRY_PTR_GET(entry, tbl->ent_len);
        }

        /*
         * This function allocates new entry from the free list and assign the
         * pointer value (ent_ptr) to point to this newly allocated entry.
         * Note the ent_ptr is a pointer to the next element pointer area
         * of the last entry in the list. So once this function returns
         * the last entry will point to the newly allocated entry.
        */
        entry = bcmimm_free_list_elem_get(t_info->unit,
                                   t_info->entry_free_list,
                                   ent_ptr);
        SHR_NULL_CHECK(entry, SHR_E_MEMORY);
        /* Assigned to the last entry pointer */
        ent_ptr = BCMIMM_ENTRY_PTR_GET(entry, tbl->ent_len);
    }
    sal_memcpy(entry, key, tbl->key_len);
    *ent_ptr = 0;   /* 'NULL' termination */
    entry_lock_cnt = BCMIMM_ENTRY_LOCK_CNT_GET(entry, tbl->ent_len);
    *entry_lock_cnt = 0;
    BCMIMM_OCCUPIED_SET(*ent_ptr);
    ent_ptr--;
    t_info->tbl->num_of_ent++;
    SHR_IF_ERR_EXIT(bcmimm_data_field_insert(t_info, ent_ptr, in));
exit:
    sal_rwlock_give(t_info->lock);
    SHR_FUNC_EXIT();
}

int bcmimm_be_entry_delete(bcmimm_be_tbl_hdl_t hdl, const void *key)
{
    bcmimm_tbl_info_t *t_info = &active_tables[hdl >> 16].tbls[hdl & 0xFFFF];
    uint8_t *entry;
    uint8_t *prev_entry;
    uint32_t *ent_ptr;
    uint32_t *prev_ent_ptr;
    uint32_t del_ent_ptr; /* The pointer value of the entry to delete */
    size_t ptr_offset = t_info->tbl->ent_len - BCMIMM_ELEM_PTR_SIZE;

    SHR_FUNC_ENTER(t_info ? t_info->unit : BSL_UNIT_UNKNOWN);
    VALIDATE_INPUT(t_info, hdl);

    if (!t_info->tbl->basic_tbl) {
        uint32_t *direct_key = (uint32_t *)key;
        if (sal_rwlock_wlock(t_info->lock, SAL_RWLOCK_FOREVER) < 0) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        SHR_IF_ERR_EXIT
            (bcmimm_be_direct_entry_delete(t_info, *direct_key));
        SHR_EXIT();
    }

    /* Take the table lock */
    if (sal_rwlock_wlock(t_info->lock, SAL_RWLOCK_FOREVER) < 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    entry = bcmimm_entry_find(t_info, key, &ent_ptr, &prev_entry);

    if (entry == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    /* Don't delete locked entries */
    if (BCMIMM_IS_LOCKED(*ent_ptr)) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    /* Free the data fields */
    bcmimm_free_list_elem_put(t_info->field_free_list, *(ent_ptr-1));
    t_info->tbl->num_of_ent--;

    if (prev_entry == NULL) {
        /*
         * The entry was the first entry. In this case if there is a following
         * entry in the same bin then copy the following entry on to the first
         * one. After, free the next entry. Otherwise (only entry in the bin),
         * simply clear the occupied bit.
         */
        if (BCMIMM_IS_NULL_PTR(*ent_ptr)) {
            /* No other entry in this bin */
            BCMIMM_OCCUPIED_CLEAR(*ent_ptr);
            SHR_EXIT();
        } else {
            prev_entry = entry;
            del_ent_ptr = *ent_ptr;
            entry = bcmimm_entry_get (t_info->entry_free_list, *ent_ptr);
            /*
             * Copy the next entry into the first one. This makes the first
             * entry to point to the next->next. so we short circuit the
             * second entry in the bin. It also means that the field list
             * is pointed by the first entry.
             */
            sal_memcpy(prev_entry, entry, t_info->tbl->ent_len);
            /* Last need to set the entry to delete pointer to NULL */
            ent_ptr = (uint32_t *)(entry + ptr_offset);
            *ent_ptr = 0;
        }
    } else {
        prev_ent_ptr = (uint32_t *)(prev_entry + ptr_offset);
        del_ent_ptr = *prev_ent_ptr;
        *prev_ent_ptr = *ent_ptr; /* Point to the next->next */
        *ent_ptr = 0;   /* Null terminate the entry */
    }
    bcmimm_free_list_elem_put(t_info->entry_free_list, del_ent_ptr);

exit:
    sal_rwlock_give(t_info->lock);
    SHR_FUNC_EXIT();
}

int bcmimm_be_entry_lookup(bcmimm_be_tbl_hdl_t hdl,
                           const void *key,
                           bcmimm_be_fields_t *out)
{
    bcmimm_tbl_info_t *t_info = &active_tables[hdl >> 16].tbls[hdl & 0xFFFF];
    uint8_t *entry;
    uint32_t *ent_ptr;

    SHR_FUNC_ENTER(t_info ? t_info->unit : BSL_UNIT_UNKNOWN);
    VALIDATE_INPUT(t_info, hdl);
    /* Take the table lock */
    if (sal_rwlock_wlock(t_info->lock, SAL_RWLOCK_FOREVER) < 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    if (!t_info->tbl->basic_tbl) {
        uint32_t *direct_key = (uint32_t *)key;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmimm_be_direct_entry_lookup(t_info, *direct_key,
                                           &out->count, out->fdata));
        SHR_EXIT();
    }

    entry = bcmimm_entry_find(t_info, key, &ent_ptr, NULL);
    if (!entry) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }
    ent_ptr--;
    SHR_IF_ERR_EXIT(bcmimm_data_field_get(t_info, *ent_ptr, out));

exit:
    sal_rwlock_give(t_info->lock);
    SHR_FUNC_EXIT();
}

int bcmimm_be_entry_update(bcmimm_be_tbl_hdl_t hdl,
                           const void *key,
                           const bcmimm_be_fields_upd_t *in)
{
    bcmimm_tbl_info_t *t_info = &active_tables[hdl >> 16].tbls[hdl & 0xFFFF];
    uint8_t *entry;
    uint32_t *ent_ptr;

    SHR_FUNC_ENTER(t_info ? t_info->unit : BSL_UNIT_UNKNOWN);
    VALIDATE_INPUT(t_info, hdl);

    /* Take the table lock */
    if (sal_rwlock_wlock(t_info->lock, SAL_RWLOCK_FOREVER) < 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    if (!t_info->tbl->basic_tbl) {
        uint32_t *direct_key = (uint32_t *)key;
        SHR_IF_ERR_EXIT
            (bcmimm_be_direct_entry_update(t_info, *direct_key,
                                           in->count, in->del, in->fdata));
        SHR_EXIT();
    }

    entry = bcmimm_entry_find(t_info, key, &ent_ptr, NULL);
    if (!entry) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    /* Don't update locked entries */
    if (BCMIMM_IS_LOCKED(*ent_ptr)) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

    ent_ptr--;
    SHR_IF_ERR_EXIT(bcmimm_data_field_update(t_info, ent_ptr, in));

exit:
    sal_rwlock_give(t_info->lock);
    SHR_FUNC_EXIT();
}

int bcmimm_be_entry_lock(bcmimm_be_tbl_hdl_t hdl, const void *key)
{
    bcmimm_tbl_info_t *t_info = &active_tables[hdl >> 16].tbls[hdl & 0xFFFF];
    uint8_t *entry;
    uint32_t *ent_ptr;
    uint16_t *entry_lock_cnt;

    SHR_FUNC_ENTER(t_info ? t_info->unit : BSL_UNIT_UNKNOWN);
    VALIDATE_INPUT(t_info, hdl);
    /* Take the table lock */
    if (sal_rwlock_wlock(t_info->lock, SAL_RWLOCK_FOREVER) < 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    if (!t_info->tbl->basic_tbl) {
        SHR_IF_ERR_EXIT(bcmimm_be_direct_entry_lock(t_info, (uint32_t *)key));
        SHR_EXIT();
    }
    entry = bcmimm_entry_find(t_info, key, &ent_ptr, NULL);
    if (!entry) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }
    entry_lock_cnt = BCMIMM_ENTRY_LOCK_CNT_GET(entry, t_info->tbl->ent_len);
    if (*entry_lock_cnt == 0xFFFF) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }
    if ((*entry_lock_cnt)++ == 0) {
        BCMIMM_ENTRY_LOCK(*ent_ptr);
    }
exit:
    sal_rwlock_give(t_info->lock);
    SHR_FUNC_EXIT();
}

int bcmimm_be_entry_unlock(bcmimm_be_tbl_hdl_t hdl, const void *key)
{
    bcmimm_tbl_info_t *t_info = &active_tables[hdl >> 16].tbls[hdl & 0xFFFF];
    uint8_t *entry;
    uint32_t *ent_ptr;
    uint16_t *entry_lock_cnt;

    SHR_FUNC_ENTER(t_info ? t_info->unit : BSL_UNIT_UNKNOWN);
    VALIDATE_INPUT(t_info, hdl);
    /* Take the table lock */
    if (sal_rwlock_wlock(t_info->lock, SAL_RWLOCK_FOREVER) < 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    if (!t_info->tbl->basic_tbl) {
        SHR_IF_ERR_EXIT(bcmimm_be_direct_entry_unlock(t_info, (uint32_t *)key));
        SHR_EXIT();
    }
    entry = bcmimm_entry_find(t_info, key, &ent_ptr, NULL);
    if (!entry) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }
    entry_lock_cnt = BCMIMM_ENTRY_LOCK_CNT_GET(entry, t_info->tbl->ent_len);
    if (*entry_lock_cnt > 0) {
        if (--(*entry_lock_cnt) == 0) {
            BCMIMM_ENTRY_UNLOCK(*ent_ptr);
        }
    }

exit:
    sal_rwlock_give(t_info->lock);
    SHR_FUNC_EXIT();
}

int bcmimm_be_get_lock_cnt(bcmimm_be_tbl_hdl_t hdl,
                           const void *key,
                           uint32_t *lock_cnt)
{
    bcmimm_tbl_info_t *t_info = &active_tables[hdl >> 16].tbls[hdl & 0xFFFF];
    uint8_t *entry;
    uint32_t *ent_ptr;
    uint16_t *entry_lock_cnt;
    int rv;

    SHR_FUNC_ENTER(t_info ? t_info->unit : BSL_UNIT_UNKNOWN);
    VALIDATE_INPUT(t_info, hdl);

    if (!lock_cnt) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Take the table lock */
    if (sal_rwlock_wlock(t_info->lock, SAL_RWLOCK_FOREVER) < 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    if (!t_info->tbl->basic_tbl) {
        rv = bcmimm_be_direct_entry_lock_cnt(t_info, (uint32_t *)key,
                                             lock_cnt);
        if (rv == SHR_E_NOT_FOUND) {
            SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
        }
        SHR_IF_ERR_EXIT(rv);
        SHR_EXIT();
    }

    entry = bcmimm_entry_find(t_info, key, &ent_ptr, NULL);
    if (entry == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }
    entry_lock_cnt = BCMIMM_ENTRY_LOCK_CNT_GET(entry, t_info->tbl->ent_len);

    *lock_cnt = *entry_lock_cnt;
exit:
    sal_rwlock_give(t_info->lock);
    SHR_FUNC_EXIT();
}

int bcmimm_be_data_resize(bcmimm_be_tbl_hdl_t hdl, size_t d_size)
{
    bcmimm_tbl_info_t *t_info = &active_tables[hdl >> 16].tbls[hdl & 0xFFFF];

    SHR_FUNC_ENTER(t_info ? t_info->unit : BSL_UNIT_UNKNOWN);
    VALIDATE_INPUT(t_info, hdl);
    SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);

exit:
    SHR_FUNC_EXIT();
}

int bcmimm_be_entry_overwrite(bcmimm_be_tbl_hdl_t hdl,
                              const void *key,
                              bcmimm_be_fields_t *in)
{
    bcmimm_tbl_info_t *t_info = &active_tables[hdl >> 16].tbls[hdl & 0xFFFF];
    uint8_t *entry;
    uint32_t *ent_ptr;

    SHR_FUNC_ENTER(t_info ? t_info->unit : BSL_UNIT_UNKNOWN);
    VALIDATE_INPUT(t_info, hdl);

    /* Take the table lock */
    SHR_IF_ERR_EXIT(sal_rwlock_rlock(t_info->lock, SAL_RWLOCK_FOREVER));
    if (!t_info->tbl->basic_tbl) {
        SHR_IF_ERR_EXIT(bcmimm_be_direct_entry_overide(t_info,
                                                       (uint32_t *)key, in));
        SHR_EXIT();
    }
    entry = bcmimm_entry_find(t_info, key, &ent_ptr, NULL);
    if (!entry) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    ent_ptr--;
    SHR_IF_ERR_EXIT(bcmimm_field_id_overwrite(t_info, ent_ptr, in));

exit:
    sal_rwlock_give(t_info->lock);
    SHR_FUNC_EXIT();
}
