/*! \file bcmcth_dlb_trunk_imm.c
 *
 * DLB_TRUNK LT imm utilities.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <shr/shr_debug.h>
#include <shr/shr_fmm.h>
#include <shr/shr_ha.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmcth/bcmcth_dlb_util.h>
#include <bcmcth/bcmcth_dlb_drv.h>
#include <bcmissu/issu_api.h>

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_DLB

#define MAX_INACTIVITY_DURATIUON 0x7fff

/*******************************************************************************
 * Private functions
 */
static bcmcth_dlb_trunk_info_t *dlb_trunk_info[BCMPORT_NUM_UNITS_MAX] = {0};

#define BCMCTH_DLB_TRUNK_PTR(_u) (dlb_trunk_info[(_u)])

/*!
 * \brief Get DLB_TRUNK blk local buffer pointer.
 *
 * \param [in] _u BCM Unit number.
 *
 * \returns DLB_TRUNK LT local blk buffer pointer.
 */

#define BCMCTH_DLB_TRUNK_TBL_BLK_PTR(_u)   \
         ((BCMCTH_DLB_TRUNK_PTR(_u))->blk_ptr)

/*!
 * \brief Get DLB_TRUNK blk HA buffer pointer.
 *
 * \param [in] _u BCM Unit number.
 *
 * \returns DLB_TRUNK LT HA blk buffer pointer.
 */

#define BCMCTH_DLB_TRUNK_TBL_HA_BLK_PTR(_u)  \
         ((BCMCTH_DLB_TRUNK_PTR(_u))->ha_blk_ptr)


/*!
 * \brief Get DLB_TRUNK entry local buffer pointer.
 *
 * \param [in] _u BCM Unit number.
 *
 * \returns DLB_TRUNK LT local entry buffer pointer.
 */

#define BCMCTH_DLB_TRUNK_TBL_ENT_PTR(_u)   \
         ((BCMCTH_DLB_TRUNK_PTR(_u))->blk_ptr->ent_arr)

/*!
 * \brief Get DLB_TRUNK entry HA buffer pointer.
 *
 * \param [in] _u BCM Unit number.
 *
 * \returns DLB_TRUNK LT HA entry buffer pointer.
 */

#define BCMCTH_DLB_TRUNK_TBL_HA_ENT_PTR(_u)  \
         ((BCMCTH_DLB_TRUNK_PTR(_u))->ha_blk_ptr->ent_arr)

/*!
 * \brief Get DLB_TRUNK mbmp local buffer pointer.
 *
 * \param [in] _u BCM Unit number.
 *
 * \returns DLB_TRUNK LT local mbmp entry buffer pointer.
 */

#define BCMCTH_DLB_TRUNK_TBL_MBMP_PTR(_u)   \
         ((BCMCTH_DLB_TRUNK_PTR(_u))->blk_ptr->mbmp_arr)

/*!
 * \brief Get DLB_TRUNK mbmp HA buffer pointer.
 *
 * \param [in] _u BCM Unit number.
 *
 * \returns DLB_TRUNK LT mbmp entry buffer pointer.
 */

#define BCMCTH_DLB_TRUNK_TBL_HA_MBMP_PTR(_u)  \
         ((BCMCTH_DLB_TRUNK_PTR(_u))->ha_blk_ptr->mbmp_arr)

/*!
 * \brief Get the flow set base pointer of an DLB_TRUNK entry.
 *
 * Given a table base pointer and the entry offset index from the base pointer
 * get the flow set base pointer of an entry.
 *
 * \param [in] _u BCM Unit number.
 * \param [in] _i Entry offset index in the entry array.
 *
 *
 * \return flow set base pointer.
 * table.
 */
#define BCMCTH_DLB_TRUNK_TBL_FLOW_SET_BASE_GET(_u, _i)    \
        (((BCMCTH_DLB_TRUNK_PTR(_u))->blk_ptr->ent_arr->dlb_trunk_ent_arr[_i]).\
           flow_set_base_ptr)

/*!
 * \brief Get the flow set size of an DLB_TRUNK entry.
 *
 * Given a table base pointer and the entry offset index from the base pointer
 * get the flow set size of an entry.
 *
 * \param [in] _u BCM Unit number.
 * \param [in] _i Entry offset index in the entry array.
 *
 * \return flow set size.
 * table.
 */
#define BCMCTH_DLB_TRUNK_TBL_FLOW_SET_SIZE_GET(_u, _i)    \
         (((BCMCTH_DLB_TRUNK_PTR(_u))->blk_ptr->ent_arr->dlb_trunk_ent_arr[_i]).\
         flow_set_size)

/*!
 * \brief Get the primary path size DLB_TRUNK entry.
 *
 * Given a table base pointer and the entry offset index from the base pointer
 * get the pirmay path size of an entry.
 *
 * \param [in] _u   BCM Unit number.
 * \param [in] _i   Entry offset index in the entry array.
 *
 * \return flow set size.
 * table.
 */
#define BCMCTH_DLB_TRUNK_TBL_NUM_PATHS_GET(_u, _i)    \
    (((BCMCTH_DLB_TRUNK_PTR(_u))->blk_ptr->ent_arr->dlb_trunk_ent_arr[_i]).\
     num_paths)

/*!
 * \brief Get the Alternate path size DLB_TRUNK entry.
 *
 * Given a table base pointer and the entry offset index from the base pointer
 * get the alternate path size of an entry.
 *
 * \param [in] _u   BCM Unit number.
 * \param [in] _i   Entry offset index in the entry array.
 * \param [in] _val value to set.
 *
 * \return flow set size.
 * table.
 */
#define BCMCTH_DLB_TRUNK_TBL_NUM_ALT_PATHS_GET(_u, _i)   \
    (((BCMCTH_DLB_TRUNK_PTR(_u))->blk_ptr->ent_arr->dlb_trunk_ent_arr[_i]).\
    num_alternate_paths)

/*!
 * \brief Set the flow set base pointer for an DLB_TRUNK entry.
 *
 * Given a table base pointer and the entry offset index from the base pointer
 * set the flow set base pointer of an entry.
 *
 * \param [in] _u BCM Unit number.
 * \param [in] _i Entry offset index in the entry array.
 * \param [in] _val value to set.
 *
 * \return flow set base pointer.
 * table.
 */
#define BCMCTH_DLB_TRUNK_TBL_FLOW_SET_BASE_SET(_u, _i, _val)    \
        (((BCMCTH_DLB_TRUNK_PTR(_u))->blk_ptr->ent_arr->dlb_trunk_ent_arr[_i]).\
        flow_set_base_ptr) = _val

/*!
 * \brief Set the flow set size for an DLB_TRUNK entry.
 *
 * Given a table base pointer and the entry offset index from the base pointer
 * set the flow set size of an entry.
 *
 * \param [in] _u   BCM Unit number.
 * \param [in] _i   Entry offset index in the entry array.
 * \param [in] _val value to set.
 *
 * \return flow set size.
 * table.
 */
#define BCMCTH_DLB_TRUNK_TBL_FLOW_SET_SIZE_PTR_SET(_u, _i, _val)    \
        (((BCMCTH_DLB_TRUNK_PTR(_u))->blk_ptr->ent_arr->dlb_trunk_ent_arr[_i]).\
        flow_set_size) = _val

/*!
 * \brief Set the primary path size DLB_TRUNK entry.
 *
 * Given a table base pointer and the entry offset index from the base pointer
 * set the pirmay path size of an entry.
 *
 * \param [in] _u   BCM Unit number.
 * \param [in] _i   Entry offset index in the entry array.
 * \param [in] _val value to set.
 *
 * \return flow set size.
 * table.
 */
#define BCMCTH_DLB_TRUNK_TBL_NUM_PATHS_SET(_u, _i, _val)    \
    (((BCMCTH_DLB_TRUNK_PTR(_u))->blk_ptr->ent_arr->dlb_trunk_ent_arr[_i]).\
    num_paths) = _val

/*!
 * \brief Set the Alternate path size DLB_TRUNK entry.
 *
 * Given a table base pointer and the entry offset index from the base pointer
 * set the alternate path size of an entry.
 *
 * \param [in] _u   BCM Unit number.
 * \param [in] _i   Entry offset index in the entry array.
 * \param [in] _val value to set.
 *
 * \return flow set size.
 * table.
 */
#define BCMCTH_DLB_TRUNK_TBL_NUM_ALT_PATHS_SET(_u, _i, _val)    \
    (((BCMCTH_DLB_TRUNK_PTR(_u))->blk_ptr->ent_arr->dlb_trunk_ent_arr[_i]).\
    num_alternate_paths) = _val


/*************************************************************************
 * Private functions
 */
/*!
 * \brief IMM input fields parse routine.
 *
 * Parse DLB_TRUNK IMM entry fields and save the data to entry.
 *
 * \param [in]  unit Unit number.
 * \param [in]  key field list.
 * \param [in]  data field list.
 * \param [out] entry DLB_TRUNK LT entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c in to \c LT entry.
 */
static int
dlb_trunk_lt_entry_key_parse(int unit,
                      const bcmltd_field_t *key,
                      bcmcth_dlb_trunk_t *entry)
{
    uint32_t fid = 0;
    uint64_t key_val = 0;

    SHR_FUNC_ENTER(unit);

    /* Read the DLB_ID */
    fid = DLB_TRUNKt_DLB_IDf;
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_dlb_imm_field_get(unit,
                                         key,
                                         fid,
                                         &key_val));
    BCMCTH_DLB_LT_FIELD_SET(entry->fbmp,
                                    BCMCTH_DLB_TRUNK_LT_FIELD_DLB_ID);
    entry->dlb_id = (bcmcth_dlb_id_t) key_val;

exit:
    SHR_FUNC_EXIT();

}

/*!
 * \brief IMM input fields parse routine.
 *
 * Parse DLB_TRUNK IMM entry fields and save the data to entry.
 *
 * \param [in]  unit Unit number.
 * \param [in]  key field list.
 * \param [in]  data field list.
 * \param [out] entry DLB_TRUNK LT entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c in to \c LT entry.
 */
static int
dlb_trunk_lt_entry_data_parse(int unit,
                      const bcmltd_field_t *data,
                      bcmcth_dlb_trunk_t *entry)
{
    uint32_t fid = 0;
    uint64_t fval = 0;
    const bcmltd_field_t *in = NULL;
    uint32_t arr_idx = 0;

    SHR_FUNC_ENTER(unit);

    /* For each field ID access the IMM table data fields */
    for (in = data; (in != NULL); in = in->next) {
        fid = in->id;
        fval = in->data;
        arr_idx = in->idx;
        switch(fid) {
            case DLB_TRUNKt_INACTIVITY_TIMEf:
                BCMCTH_DLB_LT_FIELD_SET(
                    entry->fbmp,
                    BCMCTH_DLB_TRUNK_LT_FIELD_INACTIVITY_TIME);
                entry->inactivity_time = (uint16_t) fval;
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(
                                    unit,
                                    "INACTIVITY_TIMEf [i=%d] fval=%d.\n"),
                                    (unsigned int)arr_idx,
                                    (uint32_t)fval));
                break;
            case DLB_TRUNKt_ASSIGNMENT_MODEf:
                BCMCTH_DLB_LT_FIELD_SET(
                   entry->fbmp,
                   BCMCTH_DLB_TRUNK_LT_FIELD_ASSIGNMENT_MODE);
                entry->assignment_mode = (uint8_t) fval;
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(
                                    unit,
                                    "ASSIGNMENT_MODEf [i=%d] fval=%d.\n"),
                                    (unsigned int)arr_idx,
                                    (uint32_t)fval));
                break;

            case DLB_TRUNKt_FLOW_SET_SIZEf:
                BCMCTH_DLB_LT_FIELD_SET(
                   entry->fbmp,
                   BCMCTH_DLB_TRUNK_LT_FIELD_FLOW_SET_SIZE);
                entry->flow_set_size = (uint8_t) fval;
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(
                                    unit,
                                    "FLOW_SET_SIZEf [i=%d] fval=%d.\n"),
                                    (unsigned int)arr_idx,
                                    (uint32_t) fval));

                break;
            case DLB_TRUNKt_ALTERNATE_PATH_COSTf:
                BCMCTH_DLB_LT_FIELD_SET(
                   entry->fbmp,
                   BCMCTH_DLB_TRUNK_LT_FIELD_ALTERNATE_PATH_COST);

                entry->alternate_path_cost = (uint8_t) fval;
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(
                                     unit,
                                     "ALTERNATE_PATH_COSTf [i=%d] fval=%d.\n"),
                                     (unsigned int)arr_idx,
                                     (uint32_t) fval));
                break;
            case DLB_TRUNKt_ALTERNATE_PATH_BIASf:
                BCMCTH_DLB_LT_FIELD_SET(
                   entry->fbmp,
                   BCMCTH_DLB_TRUNK_LT_FIELD_ALTERNATE_PATH_BIAS);
                entry->alternate_path_bias = (uint8_t) fval;
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(
                                    unit,
                                    "ALTERNATE_PATH_BIASf [i=%d] fval=%d.\n"),
                                    (unsigned int)arr_idx,
                                    (uint32_t) fval));
                break;
            case DLB_TRUNKt_PRIMARY_PATH_THRESHOLDf:
                BCMCTH_DLB_LT_FIELD_SET(
                   entry->fbmp,
                   BCMCTH_DLB_TRUNK_LT_FIELD_PRIMARY_PATH_THRESHOLD);
                entry->primary_path_threshold = (uint8_t) fval;
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(
                                unit,
                                "PRIMARY_PATH_THRESHOLDf [i=%d] fval=%d.\n"),
                                (unsigned int)arr_idx,
                                (uint32_t) fval));
                break;
            case DLB_TRUNKt_NUM_PATHSf:
                BCMCTH_DLB_LT_FIELD_SET(
                   entry->fbmp,
                  BCMCTH_DLB_TRUNK_LT_FIELD_NUM_PATHS);

                entry->num_paths = (uint8_t) fval;
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(
                                    unit,
                                    "NUM_PATHSf [i=%d] fval=%d.\n"),
                                    (unsigned int)arr_idx,
                                    (uint32_t) fval));
                break;
            case DLB_TRUNKt_PORT_IDf:
                if (arr_idx >= BCMCTH_DLB_TRUNK_GROUP_MAX_MEMBERS) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(
                                     unit,
                                     "PORT_IDf [i=%d] fld_idx=%d invalid.\n"),
                                     (unsigned int)arr_idx,
                                     arr_idx));
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                BCMCTH_DLB_LT_FIELD_SET(
                   entry->fbmp,
                   BCMCTH_DLB_TRUNK_LT_FIELD_PORT_IDS);
                BCMCTH_DLB_LT_FIELD_SET(entry->pri_port_array_bmp, arr_idx);
                entry->port_id[arr_idx] = (uint8_t) fval;
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(
                                    unit,
                                    "PORT_IDf [i=%d] fval=%d.\n"),
                                    (unsigned int)arr_idx,
                                    arr_idx));
                break;
            case DLB_TRUNKt_ALTERNATE_NUM_PATHSf:
                BCMCTH_DLB_LT_FIELD_SET(
                   entry->fbmp,
                  BCMCTH_DLB_TRUNK_LT_FIELD_ALTERNATE_NUM_PATHS);
                entry->num_alternate_paths = (uint8_t) fval;
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(
                                    unit,
                                    "ALTERNATE_NUM_PATHSf [i=%d] fval=%d.\n"),
                                    (unsigned int)arr_idx,
                                    (uint32_t) fval));
                break;
            case DLB_TRUNKt_ALTERNATE_PORT_IDf:
                if (arr_idx >= BCMCTH_DLB_TRUNK_GROUP_MAX_MEMBERS) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(
                                        unit,
                                        "ALTERNATE_PORT_IDf [i=%d] "\
                                        "fld_idx=%d invalid.\n"),
                                        (unsigned int)arr_idx, arr_idx));
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                BCMCTH_DLB_LT_FIELD_SET(
                   entry->fbmp,
                   BCMCTH_DLB_TRUNK_LT_FIELD_ALTERNATE_PORT_IDS);
                BCMCTH_DLB_LT_FIELD_SET(entry->alt_port_array_bmp, arr_idx);
                entry->alternate_port_id[arr_idx] = (uint8_t) fval;
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(
                                    unit,
                                    "ALTERNATE_PORT_IDf [i=%d] fval=%d.\n"),
                                    (unsigned int)arr_idx,
                                    (uint32_t) fval));
                break;
            default:
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(
                                    unit,
                                    "Invalid field ID = %d.\n"),
                                    fid));
                SHR_ERR_EXIT(SHR_E_PARAM);
                break;
        }
    }

exit:
    SHR_FUNC_EXIT();

}


static int
dlb_trunk_lt_fields_free(int unit,
                        bcmltd_fields_t *in,
                        size_t num_fields)
{
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    if (in->field) {
        for (i = 0; i < num_fields; i++) {
            if (in->field[i]) {
                SHR_FREE(in->field[i]);
            }
        }
        SHR_FREE(in->field);
    }

    SHR_FUNC_EXIT();
}

static int
dlb_trunk_lt_fields_allocate (int unit,
                                bcmltd_fields_t *in,
                                size_t num_fields)
{
    uint32_t i;
    size_t alloc_sz;

    SHR_FUNC_ENTER(unit);

    /* Allocate fields buffers */
    alloc_sz = sizeof(bcmltd_field_t *) * num_fields;
    SHR_ALLOC(in->field, alloc_sz, "bcmcthDlbTrunkEntryFields");
    SHR_NULL_CHECK(in->field, SHR_E_MEMORY);
    sal_memset(in->field, 0, alloc_sz);

    in->count = num_fields;

    for (i = 0; i < num_fields; i++) {
        SHR_ALLOC(in->field[i], sizeof(bcmltd_field_t), "bcmcthDlbTrunkEntryField");
        SHR_NULL_CHECK(in->field[i], SHR_E_MEMORY);
        sal_memset(in->field[i], 0, sizeof(bcmltd_field_t));
    }

exit:
    if (SHR_FUNC_ERR()) {
        (void)dlb_trunk_lt_fields_free(unit, in, num_fields);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Get all current imm DLB_TRUNK LT fields.
 *
 * Get the current fields from imm DLB_TRUNK LT
 *
 * \param [in] unit Unit number.
 * \param [out] entry DLB_TRUNK LT entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to geet current LT entry fields.
 */
static int
dlb_trunk_lt_fields_lookup(int unit,
                          bcmcth_dlb_trunk_t *lt_entry)
{
    bcmltd_fields_t key_fields = {0};
    bcmltd_fields_t imm_fields = {0};
    bcmltd_fields_t *flist;
    bcmltd_field_t *data_field;
    size_t num_fields = DLB_FIELD_COUNT_MAX;
    size_t i;
    uint32_t fid;
    uint64_t fval;
    uint32_t arr_idx;

    SHR_FUNC_ENTER(unit);

    /* Allocate key field */
    SHR_IF_ERR_EXIT
        (dlb_trunk_lt_fields_allocate(unit,
                                     &key_fields,
                                     1));

    key_fields.field[0]->id   = DLB_TRUNKt_DLB_IDf;
    key_fields.field[0]->data = lt_entry->dlb_id;

    SHR_IF_ERR_EXIT
        (dlb_trunk_lt_fields_allocate(unit,
                                     &imm_fields,
                                     num_fields));

    /* Lookup IMM table; error if entry not found */
    SHR_IF_ERR_EXIT
        (bcmimm_entry_lookup(unit,
                             DLB_TRUNKt,
                             &key_fields,
                             &imm_fields));

    flist = &imm_fields;

    /* Parse IMM table data fields */
    for (i = 0; i < flist->count; i++) {
        data_field = flist->field[i];
        fid = data_field->id;
        fval = data_field->data;
        arr_idx = data_field->idx;

        switch (fid) {
            case DLB_TRUNKt_INACTIVITY_TIMEf:
                lt_entry->inactivity_time = (uint16_t) fval;
                break;
            case DLB_TRUNKt_ASSIGNMENT_MODEf:
                lt_entry->assignment_mode = (uint8_t) fval;
                break;
            case DLB_TRUNKt_FLOW_SET_SIZEf:
                lt_entry->flow_set_size = (uint8_t) fval;
                break;
            case DLB_TRUNKt_ALTERNATE_PATH_COSTf:
                lt_entry->alternate_path_cost = (uint8_t) fval;
                break;
            case DLB_TRUNKt_ALTERNATE_PATH_BIASf:
                lt_entry->alternate_path_bias = (uint8_t) fval;
                break;
            case DLB_TRUNKt_PRIMARY_PATH_THRESHOLDf:
                lt_entry->primary_path_threshold = (uint8_t) fval;
                break;
            case DLB_TRUNKt_NUM_PATHSf:
                lt_entry->num_paths = (uint8_t) fval;
                break;
            case DLB_TRUNKt_PORT_IDf:
                lt_entry->port_id[arr_idx] = (shr_port_t) fval;
                break;
            case DLB_TRUNKt_ALTERNATE_NUM_PATHSf:
                lt_entry->num_alternate_paths = (shr_port_t) fval;
                break;
            case DLB_TRUNKt_ALTERNATE_PORT_IDf:
                lt_entry->alternate_port_id[arr_idx] = (uint8_t) fval;
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

exit:
    (void)dlb_trunk_lt_fields_free(unit, &key_fields, 1);
    (void)dlb_trunk_lt_fields_free(unit, &imm_fields, num_fields);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize all fields value as SW default value.
 *
 * This initialization is only used for Insert operation.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry DLB_TRUNK LT entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Input parameter failed validation check.
 */
static int
dlb_trunk_lt_default_values_init(int unit,
                                bcmcth_dlb_trunk_t *lt_entry)
{
    uint64_t def_val = 0;
    uint32_t num;
    SHR_FUNC_ENTER(unit);

    /* validate input parameter. */
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    if (!BCMCTH_DLB_LT_FIELD_GET(lt_entry->fbmp,
                                 BCMCTH_DLB_TRUNK_LT_FIELD_INACTIVITY_TIME)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit,
                                     DLB_TRUNKt,
                                     DLB_TRUNKt_INACTIVITY_TIMEf,
                                     1, &def_val, &num));
        lt_entry->inactivity_time = (uint8_t)def_val;
        BCMCTH_DLB_LT_FIELD_SET(lt_entry->fbmp,
                                BCMCTH_DLB_TRUNK_LT_FIELD_INACTIVITY_TIME);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Validate the ASSIGNMENT_MODE if is FIXED mode.
 *
 * \param [in]  unit      Unit number.
 * \param [in]  lt_entry  DLB_TRUNK LT entry data buffer.
 * \param [out] is_fixed  Flag set to true if fixed mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Input parameter failed validation check.
 */
static int
dlb_trunk_lt_entry_assignment_mode_is_fixed(
                                      int unit,
                                      bcmcth_dlb_trunk_t *lt_entry,
                                      bool *is_fixed)
{
    const bcmlrd_field_data_t *field_data;
    const bcmltd_enum_type_t *edata;
    uint32_t i = 0;
    int fixed_val = -1;
    SHR_FUNC_ENTER(unit);

    /* validate input parameter. */
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);
    SHR_NULL_CHECK(is_fixed, SHR_E_PARAM);

    *is_fixed = FALSE;
    SHR_IF_ERR_EXIT
        (bcmlrd_field_get(unit,
                          lt_entry->glt_sid,
                          DLB_TRUNKt_ASSIGNMENT_MODEf,
                          &field_data));

    edata = field_data->edata;
    if (edata != NULL) {
        for (i = 0; i < edata->num_sym; i++) {
            if (sal_strcmp(edata->sym[i].name, "FIXED") == 0) {
                fixed_val = edata->sym[i].val;
                break;
            }
        }
    }
    if (fixed_val < 0) {
        /* No match - fixed mode not found. */
        SHR_EXIT();
    }

    if (lt_entry->assignment_mode == fixed_val) {
        *is_fixed = TRUE;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Validate if the flow set programming is needed
 *        for the DLB_TRUNK group under fixed assignment
 *        mode.
 *
 * \param [in]  unit                Unit number.
 * \param [in]  lt_entry            DLB_TRUNK LT entry data buffer.
 * \param [in]  prev_num_paths      DLB_TRUNK num paths.
 * \param [in]  prev_num_alt_paths  DLB_TRUNK num alternate paths.
 * \param [out] fixed_flowset_init  Flag set to indicate flowset
 *                                  programming.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Input parameter failed validation check.
 */
static int
dlb_trunk_lt_entry_fixed_flowset_init_get(
                                      int unit,
                                      bcmcth_dlb_trunk_t *lt_entry,
                                      int prev_num_paths,
                                      int prev_num_alt_paths,
                                      bool *fixed_flowset_init)
{
    int i = 0;
    bool is_fixed = FALSE;
    SHR_FUNC_ENTER(unit);

    /* validate input parameter. */
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);
    SHR_NULL_CHECK(fixed_flowset_init, SHR_E_PARAM);


    *fixed_flowset_init = FALSE;
    SHR_IF_ERR_EXIT
        (dlb_trunk_lt_entry_assignment_mode_is_fixed(
                          unit,
                          lt_entry,
                          &is_fixed));

    if (is_fixed != TRUE) {
        SHR_EXIT();
    }
    if ((lt_entry->num_paths != prev_num_paths) ||
        (lt_entry->num_alternate_paths != prev_num_alt_paths)) {
        /* No need to check for the Port or alternate ports. */
        *fixed_flowset_init = TRUE;
        SHR_EXIT();
    }

    if (BCMCTH_DLB_LT_FIELD_GET(
                     lt_entry->fbmp,
                     BCMCTH_DLB_TRUNK_LT_FIELD_PORT_IDS)) {

        for (i = 0; i < lt_entry->num_paths; i++) {
            if (BCMCTH_DLB_LT_FIELD_GET(lt_entry->pri_port_array_bmp,
                                        i)) {
                *fixed_flowset_init = TRUE;
                SHR_EXIT();
            }
        }

    }

    if (BCMCTH_DLB_LT_FIELD_GET(
                     lt_entry->fbmp,
                     BCMCTH_DLB_TRUNK_LT_FIELD_ALTERNATE_PORT_IDS)) {

        for (i = 0; i < lt_entry->num_alternate_paths; i++) {
            if (BCMCTH_DLB_LT_FIELD_GET(lt_entry->alt_port_array_bmp,
                                        i)) {
                *fixed_flowset_init = TRUE;
                SHR_EXIT();
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Insert or update the LT entry.
 *
 * Parse Insert or update the DLB_TRUNK Lt entry
 *
 * \param [in]  unit Unit number.
 * \param [in]  op_arg Operational argument.
 * \param [in]  sid LTD Symbol ID.
 * \param [in]  key field list.
 * \param [in]  data field list.
 * \param [out] event_reason reason code.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to insert or update the LT entry.
 */
static int
dlb_trunk_update(int unit,
                 const bcmltd_op_arg_t *op_arg,
                 bcmltd_sid_t sid,
                 const bcmltd_field_t *key,
                 const bcmltd_field_t *data,
                 bcmimm_entry_event_t event_reason)
{
    bcmcth_dlb_trunk_t dlb_trunk_entry;
    bool flow_set_init = FALSE;
    uint32_t prev_num_paths = 0;
    uint32_t prev_num_alternate_paths = 0;
    bool fixed_update_flowset_init = FALSE;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(key, SHR_E_PARAM);
    SHR_NULL_CHECK(data, SHR_E_PARAM);

    if (sid != DLB_TRUNKt) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    sal_memset(&dlb_trunk_entry, 0, sizeof(dlb_trunk_entry));
    dlb_trunk_entry.op_arg = op_arg;
    dlb_trunk_entry.glt_sid = sid;
    /* Get the dlb_id for reading the data from backup. */
    SHR_IF_ERR_VERBOSE_EXIT(
        dlb_trunk_lt_entry_key_parse(unit,
                                 key,
                                 &dlb_trunk_entry));

    if (event_reason == BCMIMM_ENTRY_UPDATE) {
        SHR_IF_ERR_EXIT
            (dlb_trunk_lt_fields_lookup(unit, &dlb_trunk_entry));
        dlb_trunk_entry.prev_flow_set_size =
                          BCMCTH_DLB_TRUNK_TBL_FLOW_SET_SIZE_GET(
                                          unit,
                                          dlb_trunk_entry.dlb_id);
        dlb_trunk_entry.prev_flow_set_base =
                          BCMCTH_DLB_TRUNK_TBL_FLOW_SET_BASE_GET(
                                          unit,
                                          dlb_trunk_entry.dlb_id);
        dlb_trunk_entry.flow_set_base =
                          BCMCTH_DLB_TRUNK_TBL_FLOW_SET_BASE_GET(
                                          unit,
                                          dlb_trunk_entry.dlb_id);
        prev_num_paths =
        dlb_trunk_entry.num_paths =
                          BCMCTH_DLB_TRUNK_TBL_NUM_PATHS_GET(
                                          unit,
                                          dlb_trunk_entry.dlb_id);

        prev_num_alternate_paths =
        dlb_trunk_entry.num_alternate_paths =
                          BCMCTH_DLB_TRUNK_TBL_NUM_ALT_PATHS_GET(
                                          unit,
                                          dlb_trunk_entry.dlb_id);
    }

    SHR_IF_ERR_VERBOSE_EXIT(
        dlb_trunk_lt_entry_data_parse(unit,
                                 data,
                                 &dlb_trunk_entry));

    if (event_reason == BCMIMM_ENTRY_INSERT) {
        SHR_IF_ERR_EXIT
            (dlb_trunk_lt_default_values_init(unit, &dlb_trunk_entry));
    } else if (event_reason == BCMIMM_ENTRY_UPDATE) {
        SHR_IF_ERR_EXIT
            (dlb_trunk_lt_entry_fixed_flowset_init_get
                                     (unit,
                                      &dlb_trunk_entry,
                                      prev_num_paths,
                                      prev_num_alternate_paths,
                                      &fixed_update_flowset_init));
    }

    /*
     * In ASSIGNEMNT_MODE = FIXED , DLB_LAG_FLOWSET
     * is reprogrammed if there is a new port addition
     * to the primary ports or alternate ports in LT
     * update operation.
     */
    if (fixed_update_flowset_init) {
        flow_set_init = TRUE;
    } else {
        if ((dlb_trunk_entry.flow_set_size == 0) ||
            ((dlb_trunk_entry.prev_flow_set_size) &&
             (dlb_trunk_entry.flow_set_size ==
             dlb_trunk_entry.prev_flow_set_size))) {
            flow_set_init = FALSE;
        } else  {
            flow_set_init = TRUE;
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_dlb_trunk_set(unit,
                                      BCMCTH_DLB_TRUNK_TBL_BLK_PTR(unit),
                                      &dlb_trunk_entry,
                                      flow_set_init));

    /* Set the flow_set_base in local bk. */
    SHR_IF_ERR_VERBOSE_EXIT(
    BCMCTH_DLB_TRUNK_TBL_FLOW_SET_BASE_SET(unit,
                                           dlb_trunk_entry.dlb_id,
                                           dlb_trunk_entry.flow_set_base));

    /* Set the flow_set_size in local bk. */
    SHR_IF_ERR_VERBOSE_EXIT(
    BCMCTH_DLB_TRUNK_TBL_FLOW_SET_SIZE_PTR_SET(unit,
                                               dlb_trunk_entry.dlb_id,
                                               dlb_trunk_entry.flow_set_size));

    /* Set the num paths in local bk. */
    SHR_IF_ERR_VERBOSE_EXIT(
    BCMCTH_DLB_TRUNK_TBL_NUM_PATHS_SET(
                              unit,
                              dlb_trunk_entry.dlb_id,
                              dlb_trunk_entry.num_paths));

    /* Set the num alternate paths in local bk. */
    SHR_IF_ERR_VERBOSE_EXIT(
    BCMCTH_DLB_TRUNK_TBL_NUM_ALT_PATHS_SET(
                             unit,
                             dlb_trunk_entry.dlb_id,
                             dlb_trunk_entry.num_alternate_paths));
    SHR_EXIT();
exit:

    SHR_FUNC_EXIT();
}

/*!
 * \brief DLB_TRUNK imm commit call back
 *
 * Discard the backup state as the current
 * transaction is getting committed.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
dlb_trunk_commit(int unit,
                 bcmltd_sid_t sid,
                 uint32_t trans_id,
                 void *context)
{
    int dlb_grp_cnt = 0;
    int flow_set_blk_cnt = 0;
    uint32_t mbmp_size = 0;
    uint32_t mbmp_req_size = 0;
    uint32_t ent_array_alloc_size = 0;
    uint32_t ent_req_size = 0;

    SHR_FUNC_ENTER(unit);

    if (sid != DLB_TRUNKt) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_dlb_trunk_param_get(unit,
                             &dlb_grp_cnt,
                             &flow_set_blk_cnt));
    ent_array_alloc_size = dlb_grp_cnt *
                sizeof(bcmcth_dlb_trunk_entry_info_t);
    ent_req_size = sizeof(bcmcth_dlb_trunk_entry_ha_blk_t) +
                ent_array_alloc_size;

    mbmp_size = SHR_BITALLOCSIZE(flow_set_blk_cnt);
    mbmp_req_size = sizeof(bcmcth_dlb_trunk_mbmp_ha_blk_t) +
                    mbmp_size;

    sal_memcpy(BCMCTH_DLB_TRUNK_TBL_HA_ENT_PTR(unit),
               BCMCTH_DLB_TRUNK_TBL_ENT_PTR(unit),
               ent_req_size);

    sal_memcpy(BCMCTH_DLB_TRUNK_TBL_HA_MBMP_PTR(unit),
               BCMCTH_DLB_TRUNK_TBL_MBMP_PTR(unit),
               mbmp_req_size);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief DLB_TRUNK transaction abort
 *
 * Rollback the DLB Trunk entry from backup state
 * as current transaction is getting aborted.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval None.
 */
static void
dlb_trunk_abort(int unit,
                bcmltd_sid_t sid,
                uint32_t trans_id,
                void *context)
{
    int dlb_grp_cnt = 0;
    int flow_set_blk_cnt = 0;
    uint32_t mbmp_size = 0;
    uint32_t mbmp_req_size = 0;
    uint32_t ent_array_alloc_size = 0;
    uint32_t ent_req_size = 0;

    bcmcth_dlb_trunk_param_get(unit,
                        &dlb_grp_cnt,
                        &flow_set_blk_cnt);
    ent_array_alloc_size = dlb_grp_cnt *
                sizeof(bcmcth_dlb_trunk_entry_info_t);
    ent_req_size = sizeof(bcmcth_dlb_trunk_entry_ha_blk_t) +
                ent_array_alloc_size;

    mbmp_size = SHR_BITALLOCSIZE(flow_set_blk_cnt);
    mbmp_req_size = sizeof(bcmcth_dlb_trunk_mbmp_ha_blk_t) +
                    mbmp_size;

    sal_memcpy(BCMCTH_DLB_TRUNK_TBL_ENT_PTR(unit),
               BCMCTH_DLB_TRUNK_TBL_HA_ENT_PTR(unit),
               ent_req_size);
    sal_memcpy(BCMCTH_DLB_TRUNK_TBL_MBMP_PTR(unit),
               BCMCTH_DLB_TRUNK_TBL_HA_MBMP_PTR(unit),
               mbmp_req_size);
}

/*!
 * \brief DLB_TRUNK LT entry delete
 *
 * \param [in] unit Unit number.
 * \param [in] op_arg Operational argument.
 * \param [in] sid LTD Symbol ID.
 * \param [in] key List of the key fields.
 * \param [in] data List of the data fields.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
dlb_trunk_delete(int unit,
                 const bcmltd_op_arg_t *op_arg,
                 bcmltd_sid_t sid,
                 const bcmltd_field_t *key,
                 const bcmltd_field_t *data)
{
    bcmcth_dlb_trunk_t dlb_trunk_entry;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(key, SHR_E_PARAM);

    sal_memset(&dlb_trunk_entry, 0, sizeof(dlb_trunk_entry));
    dlb_trunk_entry.op_arg = op_arg;
    dlb_trunk_entry.glt_sid = sid;

    SHR_IF_ERR_VERBOSE_EXIT(
        dlb_trunk_lt_entry_key_parse(unit,
                                 key,
                                 &dlb_trunk_entry));
    dlb_trunk_entry.flow_set_size =
                          BCMCTH_DLB_TRUNK_TBL_FLOW_SET_SIZE_GET(
                                          unit,
                                          dlb_trunk_entry.dlb_id);
    dlb_trunk_entry.flow_set_base =
                          BCMCTH_DLB_TRUNK_TBL_FLOW_SET_BASE_GET(
                                          unit,
                                          dlb_trunk_entry.dlb_id);

    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_dlb_trunk_del(
                           unit,
                           BCMCTH_DLB_TRUNK_TBL_BLK_PTR(unit),
                           &dlb_trunk_entry));

    /* Set the flow_set_base in bk. */
    SHR_IF_ERR_VERBOSE_EXIT(
    BCMCTH_DLB_TRUNK_TBL_FLOW_SET_BASE_SET(unit,
                                          dlb_trunk_entry.dlb_id,
                                          0));

    /* Set the flow_set_size in bk. */
    SHR_IF_ERR_VERBOSE_EXIT(
    BCMCTH_DLB_TRUNK_TBL_FLOW_SET_SIZE_PTR_SET(unit,
                                          dlb_trunk_entry.dlb_id,
                                          0));
    /* Set the num paths in local bk. */
    SHR_IF_ERR_VERBOSE_EXIT(
    BCMCTH_DLB_TRUNK_TBL_NUM_PATHS_SET(
                              unit,
                              dlb_trunk_entry.dlb_id,
                              0));

    /* Set the num alternate paths in local bk. */
    SHR_IF_ERR_VERBOSE_EXIT(
    BCMCTH_DLB_TRUNK_TBL_NUM_ALT_PATHS_SET(
                             unit,
                             dlb_trunk_entry.dlb_id,
                             0));

    SHR_EXIT();
    exit:

    SHR_FUNC_EXIT();
}

/*!
 * \brief DLB_TRUNK table change callback function for staging.
 *
 * Handle DLB_TRUNK IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] op_arg Operational argument.
 * \param [in] event_reason Call back reason.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 * \param [out] output_fields This is a linked list of fields,
 * can be used by the component to add fields into the entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL LM fails to handle LT change events.
 */
static int
dlb_trunk_stage(int unit,
                bcmltd_sid_t sid,
                const bcmltd_op_arg_t *op_arg,
                bcmimm_entry_event_t event_reason,
                const bcmltd_field_t *key,
                const bcmltd_field_t *data,
                void *context,
                bcmltd_fields_t *output_fields)
{
    SHR_FUNC_ENTER(unit);

    if (output_fields) {
        output_fields->count = 0;
    }
    switch (event_reason) {
        case BCMIMM_ENTRY_INSERT:
        case BCMIMM_ENTRY_UPDATE:
            if (data != NULL) {
                SHR_IF_ERR_VERBOSE_EXIT(dlb_trunk_update(unit,
                                                 op_arg,
                                                 sid,
                                                 key,
                                                 data,
                                                 event_reason));
            }
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT(dlb_trunk_delete(unit,
                                             op_arg,
                                             sid,
                                             key,
                                             data));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief DLB_TRUNK In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to DLB_TRUNK logical table entry commit stages.
 */
static bcmimm_lt_cb_t bcmcth_dlb_trunk_imm_callback = {
  /*!
   * Validate function - Validates the DLB group usage.
   */
  .validate = NULL,

  /*! Staging function. */
  .op_stage    = dlb_trunk_stage,

  /*! Commit function. */
  .commit   = dlb_trunk_commit,

  /*! Abort function. */
  .abort    = dlb_trunk_abort
};

/**************************************************************************
 * Public functions
 */
/*!
 * \brief DLB_TRUNK information resources alloc.
 *
 *
 * \param [in] unit Unit number.
 * \param [in] warm flag indicating cold boot or warm boot
 * enable for warm boot.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Input parameter failed validation check.
 */
int
bcmcth_dlb_trunk_resources_alloc(int unit, bool warm)
{
    int dlb_grp_cnt = 0;
    uint32_t ha_alloc_size = 0;
    uint32_t ent_array_alloc_size = 0;
    uint32_t ent_req_size = 0;
    uint32_t mbmp_size = 0;
    uint32_t mbmp_req_size = 0;
    uint32_t ref_sign_1 = 0x88880000;
    uint32_t ref_sign_2 = 0x88880001;
    bcmcth_dlb_trunk_entry_ha_blk_t *grp_ptr = NULL;
    bcmcth_dlb_trunk_mbmp_ha_blk_t *mbmp_ptr = NULL;
    bcmcth_dlb_trunk_entry_ha_blk_t *local_grp_ptr = NULL;
    bcmcth_dlb_trunk_mbmp_ha_blk_t *local_mbmp_ptr = NULL;
    int flow_set_blk_cnt = 0;
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_dlb_trunk_param_get(unit,
                                         &dlb_grp_cnt,
                                         &flow_set_blk_cnt));

    if (!BCMCTH_DLB_TRUNK_PTR(unit)) {
        SHR_ALLOC
            (BCMCTH_DLB_TRUNK_PTR(unit),
            sizeof(bcmcth_dlb_trunk_info_t),
            "bcmcthDlbTrunk");
        SHR_NULL_CHECK(BCMCTH_DLB_TRUNK_PTR(unit), SHR_E_MEMORY);
        sal_memset(BCMCTH_DLB_TRUNK_PTR(unit),
                   0, sizeof(bcmcth_dlb_trunk_info_t));
    }

    mbmp_size = SHR_BITALLOCSIZE(flow_set_blk_cnt);
    mbmp_req_size = sizeof(bcmcth_dlb_trunk_mbmp_ha_blk_t) +
                    mbmp_size;

    ent_array_alloc_size = dlb_grp_cnt *
                sizeof(bcmcth_dlb_trunk_entry_info_t);
    ent_req_size = sizeof(bcmcth_dlb_trunk_entry_ha_blk_t) +
                ent_array_alloc_size;
    ha_alloc_size = ent_req_size;

    if (!BCMCTH_DLB_TRUNK_TBL_BLK_PTR(unit)) {
        SHR_ALLOC(BCMCTH_DLB_TRUNK_TBL_BLK_PTR(unit),
              sizeof(bcmcth_dlb_trunk_blk_t),
              "bcmcthDLBTrunkTempBlkPtr");
        sal_memset(BCMCTH_DLB_TRUNK_TBL_BLK_PTR(unit), 0,
          sizeof(bcmcth_dlb_trunk_blk_t));
    }

    if (!BCMCTH_DLB_TRUNK_TBL_HA_BLK_PTR(unit)) {
        SHR_ALLOC(BCMCTH_DLB_TRUNK_TBL_HA_BLK_PTR(unit),
              sizeof(bcmcth_dlb_trunk_blk_t),
              "bcmcthDLBTrunkBlkPtr");
        sal_memset(BCMCTH_DLB_TRUNK_TBL_HA_BLK_PTR(unit), 0,
          sizeof(bcmcth_dlb_trunk_blk_t));
    }

    /* HA entry info. */
    if (!BCMCTH_DLB_TRUNK_TBL_HA_ENT_PTR(unit)) {
        grp_ptr = shr_ha_mem_alloc(unit,
                             BCMMGMT_DLB_COMP_ID,
                             BCMPTM_HA_SUBID_DLB_TRUNK_ENT,
                             "bcmcthDlbTrunkEntryHaInfo",
                             &ha_alloc_size);
        SHR_IF_ERR_VERBOSE_EXIT((ha_alloc_size < ent_req_size) ?
                        SHR_E_MEMORY : SHR_E_NONE);
        SHR_NULL_CHECK
            (grp_ptr, SHR_E_MEMORY);
        BCMCTH_DLB_TRUNK_TBL_HA_ENT_PTR(unit) = grp_ptr;
        if (!warm) {
            sal_memset(grp_ptr, 0, ha_alloc_size);
            sal_memset(BCMCTH_DLB_TRUNK_TBL_HA_ENT_PTR(unit),
                       0, ha_alloc_size);
            BCMCTH_DLB_TRUNK_TBL_HA_ENT_PTR(unit)->array_size
                 = dlb_grp_cnt;
            BCMCTH_DLB_TRUNK_TBL_HA_ENT_PTR(unit)->signature
                 = ref_sign_1;
            SHR_IF_ERR_VERBOSE_EXIT(
            bcmissu_struct_info_report(unit, BCMMGMT_DLB_COMP_ID,
                                       BCMPTM_HA_SUBID_DLB_TRUNK_ENT, 0,
                                       ent_req_size, 1,
                                       BCMCTH_DLB_TRUNK_ENTRY_HA_BLK_T_ID));
         } else {
            SHR_IF_ERR_MSG_EXIT
            ((grp_ptr->signature != ref_sign_1
              ? SHR_E_FAIL : SHR_E_NONE),
              (BSL_META_U(unit,
                          "\n\tDLB trunk entry: Signature mismatch "
                          "exp=0x%-8x act=0x%-8x\n"),
               ref_sign_1,
               grp_ptr->signature));
        }
        LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\n\t DLB trunk entry: Signature=0x%-8x HA addr=0x%p\n"),
                 grp_ptr->signature,
                 (void*)grp_ptr));
    }

   /* Book keeping local entry info to the component.*/
    if (!BCMCTH_DLB_TRUNK_TBL_ENT_PTR(unit)) {
        SHR_ALLOC
            (BCMCTH_DLB_TRUNK_TBL_ENT_PTR(unit),
            ha_alloc_size,
            "bcmcthDlbTrunkTableLocalInfo");
        SHR_NULL_CHECK(BCMCTH_DLB_TRUNK_TBL_ENT_PTR(unit), SHR_E_MEMORY);
        SHR_IF_ERR_VERBOSE_EXIT((ha_alloc_size < ent_req_size) ?
                        SHR_E_MEMORY : SHR_E_NONE);

        local_grp_ptr = BCMCTH_DLB_TRUNK_TBL_ENT_PTR(unit);
        if (!warm) {
            sal_memset(BCMCTH_DLB_TRUNK_TBL_ENT_PTR(unit), 0, ha_alloc_size);
            BCMCTH_DLB_TRUNK_TBL_ENT_PTR(unit)->array_size
                 = dlb_grp_cnt;
            BCMCTH_DLB_TRUNK_TBL_ENT_PTR(unit)->signature
                 = ref_sign_1;
        } else {
            sal_memcpy(BCMCTH_DLB_TRUNK_TBL_ENT_PTR(unit),
               BCMCTH_DLB_TRUNK_TBL_HA_ENT_PTR(unit), ha_alloc_size);
        }
    }

    mbmp_size = SHR_BITALLOCSIZE(flow_set_blk_cnt);
    mbmp_req_size = sizeof(bcmcth_dlb_trunk_mbmp_ha_blk_t) +
                    mbmp_size;
    ha_alloc_size = mbmp_req_size;
   /* HA mbmp info. */
    if (!BCMCTH_DLB_TRUNK_TBL_HA_MBMP_PTR(unit)) {
        mbmp_ptr = shr_ha_mem_alloc(unit,
                             BCMMGMT_DLB_COMP_ID,
                             BCMPTM_HA_SUBID_DLB_TRUNK_MBMP,
                             "bcmcthDlbTrunkMbmpHaInfo",
                             &ha_alloc_size);
        SHR_IF_ERR_VERBOSE_EXIT((ha_alloc_size < mbmp_req_size) ?
                        SHR_E_MEMORY : SHR_E_NONE);
        SHR_NULL_CHECK
            (mbmp_ptr, SHR_E_MEMORY);
        BCMCTH_DLB_TRUNK_TBL_HA_MBMP_PTR(unit) = mbmp_ptr;
        if (!warm) {
            sal_memset(mbmp_ptr, 0, ha_alloc_size);
            sal_memset(BCMCTH_DLB_TRUNK_TBL_HA_MBMP_PTR(unit),
                       0, ha_alloc_size);
            BCMCTH_DLB_TRUNK_TBL_HA_MBMP_PTR(unit)->array_size
                 = mbmp_size/ sizeof(uint32_t);
            BCMCTH_DLB_TRUNK_TBL_HA_MBMP_PTR(unit)->signature
                 = ref_sign_2;
            sal_memset(BCMCTH_DLB_TRUNK_TBL_HA_MBMP_PTR(unit)->mbmp,
                       0,
                       mbmp_size);
            SHR_IF_ERR_VERBOSE_EXIT(
            bcmissu_struct_info_report(unit, BCMMGMT_DLB_COMP_ID,
                                       BCMPTM_HA_SUBID_DLB_TRUNK_MBMP, 0,
                                       mbmp_req_size, 1,
                                       BCMCTH_DLB_TRUNK_MBMP_HA_BLK_T_ID));
         } else {
            SHR_IF_ERR_MSG_EXIT
            ((mbmp_ptr->signature != ref_sign_2
              ? SHR_E_FAIL : SHR_E_NONE),
              (BSL_META_U(unit,
                          "\n\tWB DLB trunk mbmp entry: Signature mismatch "
                          "exp=0x%-8x act=0x%-8x\n"),
               ref_sign_2,
               mbmp_ptr->signature));
        }
        LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\n\t DLB trunk mbmp entry: Signature=0x%-8x HA addr=0x%p\n"),
                 mbmp_ptr->signature,
                 (void*)mbmp_ptr));
    }

   /* Book keeping local to the component.*/
    if (!BCMCTH_DLB_TRUNK_TBL_MBMP_PTR(unit)) {
        SHR_ALLOC
            (BCMCTH_DLB_TRUNK_TBL_MBMP_PTR(unit),
            ha_alloc_size,
            "bcmcthDlbTrunkTableLocalMbmpInfo");
        SHR_NULL_CHECK(BCMCTH_DLB_TRUNK_TBL_MBMP_PTR(unit), SHR_E_MEMORY);
        SHR_IF_ERR_VERBOSE_EXIT((ha_alloc_size < mbmp_req_size) ?
                        SHR_E_MEMORY : SHR_E_NONE);
        local_mbmp_ptr = BCMCTH_DLB_TRUNK_TBL_MBMP_PTR(unit);
        if (!warm) {
            sal_memset(BCMCTH_DLB_TRUNK_TBL_MBMP_PTR(unit), 0, ha_alloc_size);
            BCMCTH_DLB_TRUNK_TBL_MBMP_PTR(unit)->array_size
                 = mbmp_size/ sizeof(uint32_t);
            BCMCTH_DLB_TRUNK_TBL_MBMP_PTR(unit)->signature
                 = ref_sign_2;
        } else {
            sal_memcpy(BCMCTH_DLB_TRUNK_TBL_MBMP_PTR(unit),
               BCMCTH_DLB_TRUNK_TBL_HA_MBMP_PTR(unit), ha_alloc_size);
        }
    }

exit:
    if (SHR_FUNC_ERR()) {
        if (grp_ptr != NULL) {
            bcmissu_struct_info_clear(unit,
                                      BCMCTH_DLB_TRUNK_ENTRY_HA_BLK_T_ID,
                                      BCMMGMT_DLB_COMP_ID,
                                      BCMPTM_HA_SUBID_DLB_TRUNK_ENT);
            shr_ha_mem_free(unit, grp_ptr);
        }
        if (local_grp_ptr != NULL) {
            SHR_FREE(local_grp_ptr);
        }
        if (mbmp_ptr != NULL) {
            bcmissu_struct_info_clear(unit,
                                      BCMCTH_DLB_TRUNK_MBMP_HA_BLK_T_ID,
                                      BCMMGMT_DLB_COMP_ID,
                                      BCMPTM_HA_SUBID_DLB_TRUNK_MBMP);
            shr_ha_mem_free(unit, mbmp_ptr);
        }
        if (local_mbmp_ptr != NULL) {
           SHR_FREE(local_mbmp_ptr);
        }
        if (BCMCTH_DLB_TRUNK_TBL_BLK_PTR(unit)) {
            SHR_FREE(BCMCTH_DLB_TRUNK_TBL_BLK_PTR(unit));
        }
        if (BCMCTH_DLB_TRUNK_TBL_HA_BLK_PTR(unit)) {
            SHR_FREE(BCMCTH_DLB_TRUNK_TBL_HA_BLK_PTR(unit));
        }
        if (BCMCTH_DLB_TRUNK_PTR(unit)) {
           SHR_FREE(BCMCTH_DLB_TRUNK_PTR(unit));
        }
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief DLB_TRUNK information resources free.
 *
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Input parameter failed validation check.
 */
int
bcmcth_dlb_trunk_resources_free(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(BCMCTH_DLB_TRUNK_TBL_ENT_PTR(unit), SHR_E_PARAM);
    SHR_FREE(BCMCTH_DLB_TRUNK_TBL_ENT_PTR(unit));
    BCMCTH_DLB_TRUNK_TBL_ENT_PTR(unit) = NULL;

    SHR_NULL_CHECK(BCMCTH_DLB_TRUNK_TBL_MBMP_PTR(unit), SHR_E_PARAM);
    SHR_FREE(BCMCTH_DLB_TRUNK_TBL_MBMP_PTR(unit));
    BCMCTH_DLB_TRUNK_TBL_MBMP_PTR(unit) = NULL;

    SHR_NULL_CHECK(BCMCTH_DLB_TRUNK_TBL_BLK_PTR(unit), SHR_E_PARAM);
    SHR_FREE(BCMCTH_DLB_TRUNK_TBL_BLK_PTR(unit));

    SHR_NULL_CHECK(BCMCTH_DLB_TRUNK_TBL_HA_BLK_PTR(unit), SHR_E_PARAM);
    SHR_FREE(BCMCTH_DLB_TRUNK_TBL_HA_BLK_PTR(unit));

    SHR_NULL_CHECK(BCMCTH_DLB_TRUNK_PTR(unit), SHR_E_PARAM);
    SHR_FREE(BCMCTH_DLB_TRUNK_PTR(unit));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief DLB_TRUNK LT imm registration
 *
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failed to register the imm callbacks.
 */
int
bcmcth_dlb_trunk_imm_init(int unit)
{
    const bcmlrd_map_t *map = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);

    /*
     * To register callback for DLB LTs here.
     */
    rv = bcmlrd_map_get(unit, DLB_TRUNKt, &map);

    if (SHR_SUCCESS(rv) && map) {
       SHR_IF_ERR_VERBOSE_EXIT
       (bcmimm_lt_event_reg(unit,
                            DLB_TRUNKt,
                            &bcmcth_dlb_trunk_imm_callback,
                            NULL));
    }

exit:
    SHR_FUNC_EXIT();
}
