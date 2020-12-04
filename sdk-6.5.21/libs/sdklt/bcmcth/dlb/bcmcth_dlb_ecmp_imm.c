/*! \file bcmcth_dlb_ecmp_imm.c
 *
 * DLB ECMP interface to in-memory table.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>
#include <shr/shr_ha.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmcth/bcmcth_dlb_drv.h>
#include <bcmcth/bcmcth_dlb_util.h>
#include <bcmissu/issu_api.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_DLB

/*******************************************************************************
 * Local definitions
 */

static bcmcth_dlb_ecmp_info_t *dlb_ecmp_info[BCMPORT_NUM_UNITS_MAX] = {0};

#define BCMCTH_DLB_ECMP_INFO(_u) (dlb_ecmp_info[(_u)])

/*!
 * \brief Get DLB_ECMP LT Entry ref cnt local buffer pointer.
 *
 * \param [in] _u BCM Unit number.
 *
 * \returns DLB_ECMP LT Entry ref cnt buffer pointer.
 */

#define BCMCTH_DLB_ECMP_TBL_ENT(_u) ((BCMCTH_DLB_ECMP_INFO(_u))->ent_arr)

/*!
 * \brief Get DLB_ECMP LT Entry ref cnt HA buffer pointer.
 *
 * \param [in] _u BCM Unit number.
 *
 * \returns DLB_ECMP LT Entry ref cnt buffer pointer.
 */

#define BCMCTH_DLB_ECMP_TBL_HA_ENT(_u) ((BCMCTH_DLB_ECMP_INFO(_u))->ha_ent_arr)

/*!
 * \brief Get the flow set base pointer of an DLB_ECMP entry.
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
#define BCMCTH_DLB_ECMP_TBL_FLOW_SET_BASE_GET(_u, _i)                   \
    (((BCMCTH_DLB_ECMP_INFO(_u))->ent_arr[_i]).flow_set_base_ptr)

/*!
 * \brief Get the flow set size of an DLB_ECMP entry.
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
#define BCMCTH_DLB_ECMP_TBL_FLOW_SET_SIZE_GET(_u, _i)           \
    (((BCMCTH_DLB_ECMP_INFO(_u))->ent_arr[_i]).flow_set_size)

/*!
 * \brief Set the flow set base pointer for an DLB_ECMP entry.
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
#define BCMCTH_DLB_ECMP_TBL_FLOW_SET_BASE_SET(_u, _i, _val)             \
    (((BCMCTH_DLB_ECMP_INFO(_u))->ent_arr[_i]).flow_set_base_ptr) = _val

/*!
 * \brief Set the flow set size for an DLB_ECMP entry.
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
#define BCMCTH_DLB_ECMP_TBL_FLOW_SET_SIZE_PTR_SET(_u, _i, _val)         \
    (((BCMCTH_DLB_ECMP_INFO(_u))->ent_arr[_i]).flow_set_size) = _val

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief imm DLB_ECMPt notification input key fields parsing.
 *
 * Parse imm DLB_ECMPt input key fields.
 *
 * \param [in] unit Unit number.
 * \param [in] key IMM input key field array.
 * \param [in] data IMM input data field array.
 * \param [out] DLB ECMP LT infomation data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c key \c data to \c lt_info.
 */
static int
dlb_ecmp_lt_fields_key_parse(int unit,
                             const bcmltd_field_t *key,
                             bcmcth_dlb_ecmp_t *lt_entry)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    /* Parse key field */
    gen_field = key;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
            case DLB_ECMPt_DLB_IDf:
                BCMCTH_DLB_LT_FIELD_SET(lt_entry->fbmp,
                                        BCMCTH_DLB_ECMP_LT_FIELD_DLB_ID);
                lt_entry->dlb_id = (bcmcth_dlb_id_t) fval;
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
        gen_field = gen_field->next;
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief imm DLB_ECMPt notification input data fields parsing.
 *
 * Parse imm DLB_ECMPt input data fields.
 *
 * \param [in] unit Unit number.
 * \param [in] key IMM input key field array.
 * \param [in] data IMM input data field array.
 * \param [out] DLB ECMP LT infomation data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c key \c data to \c lt_info.
 */
static int
dlb_ecmp_lt_fields_data_parse(int unit,
                              const bcmltd_field_t *data,
                              bcmcth_dlb_ecmp_t *lt_entry)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;
    uint32_t arr_idx;

    SHR_FUNC_ENTER(unit);

    /* Parse data field */
    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;
        arr_idx = gen_field->idx;

        switch (fid) {
            case DLB_ECMPt_INACTIVITY_TIMEf:
                BCMCTH_DLB_LT_FIELD_SET(lt_entry->fbmp,
                                        BCMCTH_DLB_ECMP_LT_FIELD_INACTIVITY_TIME);
                lt_entry->inactivity_time = (uint16_t) fval;
                break;
            case DLB_ECMPt_ASSIGNMENT_MODEf:
                BCMCTH_DLB_LT_FIELD_SET(lt_entry->fbmp,
                                        BCMCTH_DLB_ECMP_LT_FIELD_ASSIGNMENT_MODE);
                lt_entry->assignment_mode = (uint8_t) fval;
                break;
            case DLB_ECMPt_FLOW_SET_SIZEf:
                BCMCTH_DLB_LT_FIELD_SET(lt_entry->fbmp,
                                        BCMCTH_DLB_ECMP_LT_FIELD_FLOW_SET_SIZE);
                lt_entry->flow_set_size = (uint8_t) fval;
                break;
            case DLB_ECMPt_ALTERNATE_PATH_COSTf:
                BCMCTH_DLB_LT_FIELD_SET(lt_entry->fbmp,
                                        BCMCTH_DLB_ECMP_LT_FIELD_ALTERNATE_PATH_COST);
                lt_entry->alternate_path_cost = (uint8_t) fval;
                break;
            case DLB_ECMPt_ALTERNATE_PATH_BIASf:
                BCMCTH_DLB_LT_FIELD_SET(lt_entry->fbmp,
                                        BCMCTH_DLB_ECMP_LT_FIELD_ALTERNATE_PATH_BIAS);
                lt_entry->alternate_path_bias = (uint8_t) fval;
                break;
            case DLB_ECMPt_PRIMARY_PATH_THRESHOLDf:
                BCMCTH_DLB_LT_FIELD_SET(lt_entry->fbmp,
                                        BCMCTH_DLB_ECMP_LT_FIELD_PRIMARY_PATH_THRESHOLD);
                lt_entry->primary_path_threshold = (uint8_t) fval;
                break;
            case DLB_ECMPt_NUM_PATHSf:
                BCMCTH_DLB_LT_FIELD_SET(lt_entry->fbmp,
                                        BCMCTH_DLB_ECMP_LT_FIELD_NUM_PATHS);
                lt_entry->num_paths = (uint8_t) fval;
                break;
            case DLB_ECMPt_NHOP_IDf:
                if (arr_idx >= BCMCTH_DLB_ECMP_GROUP_MAX_MEMBERS) {
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                BCMCTH_DLB_LT_FIELD_SET(lt_entry->fbmp,
                                        BCMCTH_DLB_ECMP_LT_FIELD_NHOP_IDS);
                BCMCTH_DLB_LT_FIELD_SET(lt_entry->pri_nh_array_bmp, arr_idx);
                lt_entry->nhop_id[arr_idx] = (bcmcth_dlb_nhop_id_t) fval;
                break;
            case DLB_ECMPt_PORT_IDf:
                if (arr_idx >= BCMCTH_DLB_ECMP_GROUP_MAX_MEMBERS) {
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                BCMCTH_DLB_LT_FIELD_SET(lt_entry->fbmp,
                                        BCMCTH_DLB_ECMP_LT_FIELD_PORT_IDS);
                BCMCTH_DLB_LT_FIELD_SET(lt_entry->pri_port_array_bmp, arr_idx);
                lt_entry->port_id[arr_idx] = (shr_port_t) fval;
                break;
            case DLB_ECMPt_ALTERNATE_NUM_PATHSf:
                BCMCTH_DLB_LT_FIELD_SET(lt_entry->fbmp,
                                        BCMCTH_DLB_ECMP_LT_FIELD_ALTERNATE_NUM_PATHS);
                lt_entry->num_alternate_paths = (uint8_t) fval;
                break;
            case DLB_ECMPt_ALTERNATE_NHOP_IDf:
                if (arr_idx >= BCMCTH_DLB_ECMP_GROUP_MAX_MEMBERS) {
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                BCMCTH_DLB_LT_FIELD_SET(lt_entry->fbmp,
                                        BCMCTH_DLB_ECMP_LT_FIELD_ALTERNATE_NHOP_IDS);
                BCMCTH_DLB_LT_FIELD_SET(lt_entry->alt_nh_array_bmp, arr_idx);
                lt_entry->alternate_nhop_id[arr_idx] = (bcmcth_dlb_nhop_id_t) fval;
                break;
            case DLB_ECMPt_ALTERNATE_PORT_IDf:
                if (arr_idx >= BCMCTH_DLB_ECMP_GROUP_MAX_MEMBERS) {
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                BCMCTH_DLB_LT_FIELD_SET(lt_entry->fbmp,
                                        BCMCTH_DLB_ECMP_LT_FIELD_ALTERNATE_PORT_IDS);
                BCMCTH_DLB_LT_FIELD_SET(lt_entry->alt_port_array_bmp, arr_idx);
                lt_entry->alternate_port_id[arr_idx] = (shr_port_t) fval;
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
        gen_field = gen_field->next;
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Initialize all fields value as SW default value.
 *
 * This initialization is only used for Insert operation.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry DLB_ECMP LT entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Input parameter failed validation check.
 */
static int
dlb_ecmp_lt_default_values_init(int unit,
                                bcmcth_dlb_ecmp_t *lt_entry)
{
    uint64_t def_val = 0;
    uint32_t num;
    uint32_t i;
    SHR_FUNC_ENTER(unit);

    /* validate input parameter. */
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    if (!BCMCTH_DLB_LT_FIELD_GET(lt_entry->fbmp,
                                 BCMCTH_DLB_ECMP_LT_FIELD_INACTIVITY_TIME)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit,
                                     DLB_ECMPt,
                                     DLB_ECMPt_INACTIVITY_TIMEf,
                                     1, &def_val, &num));
        lt_entry->inactivity_time = (uint8_t)def_val;
        BCMCTH_DLB_LT_FIELD_SET(lt_entry->fbmp,
                                BCMCTH_DLB_ECMP_LT_FIELD_INACTIVITY_TIME);
    }

    if (!BCMCTH_DLB_LT_FIELD_GET(lt_entry->fbmp,
                                 BCMCTH_DLB_ECMP_LT_FIELD_ASSIGNMENT_MODE)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit,
                                     DLB_ECMPt,
                                     DLB_ECMPt_ASSIGNMENT_MODEf,
                                     1, &def_val, &num));
        lt_entry->assignment_mode = (uint8_t)def_val;
        BCMCTH_DLB_LT_FIELD_SET(lt_entry->fbmp,
                                BCMCTH_DLB_ECMP_LT_FIELD_ASSIGNMENT_MODE);
    }

    if (!BCMCTH_DLB_LT_FIELD_GET(lt_entry->fbmp,
                                 BCMCTH_DLB_ECMP_LT_FIELD_FLOW_SET_SIZE)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit,
                                     DLB_ECMPt,
                                     DLB_ECMPt_FLOW_SET_SIZEf,
                                     1, &def_val, &num));
        lt_entry->flow_set_size = (uint8_t)def_val;
        BCMCTH_DLB_LT_FIELD_SET(lt_entry->fbmp,
                                BCMCTH_DLB_ECMP_LT_FIELD_FLOW_SET_SIZE);
    }

    if (!BCMCTH_DLB_LT_FIELD_GET(lt_entry->fbmp,
                                 BCMCTH_DLB_ECMP_LT_FIELD_ALTERNATE_PATH_COST)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit,
                                     DLB_ECMPt,
                                     DLB_ECMPt_ALTERNATE_PATH_COSTf,
                                     1, &def_val, &num));
        lt_entry->alternate_path_cost = (uint8_t)def_val;
        BCMCTH_DLB_LT_FIELD_SET(lt_entry->fbmp,
                                BCMCTH_DLB_ECMP_LT_FIELD_ALTERNATE_PATH_COST);
    }

    if (!BCMCTH_DLB_LT_FIELD_GET(lt_entry->fbmp,
                                 BCMCTH_DLB_ECMP_LT_FIELD_ALTERNATE_PATH_BIAS)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit,
                                     DLB_ECMPt,
                                     DLB_ECMPt_ALTERNATE_PATH_BIASf,
                                     1, &def_val, &num));
        lt_entry->alternate_path_bias = (uint8_t)def_val;
        BCMCTH_DLB_LT_FIELD_SET(lt_entry->fbmp,
                                BCMCTH_DLB_ECMP_LT_FIELD_ALTERNATE_PATH_BIAS);
    }

    if (!BCMCTH_DLB_LT_FIELD_GET(lt_entry->fbmp,
                                 BCMCTH_DLB_ECMP_LT_FIELD_PRIMARY_PATH_THRESHOLD)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit,
                                     DLB_ECMPt,
                                     DLB_ECMPt_PRIMARY_PATH_THRESHOLDf,
                                     1, &def_val, &num));
        lt_entry->primary_path_threshold = (uint8_t)def_val;
        BCMCTH_DLB_LT_FIELD_SET(lt_entry->fbmp,
                                BCMCTH_DLB_ECMP_LT_FIELD_PRIMARY_PATH_THRESHOLD);
    }

    if (!BCMCTH_DLB_LT_FIELD_GET(lt_entry->fbmp,
                                 BCMCTH_DLB_ECMP_LT_FIELD_NUM_PATHS)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit,
                                     DLB_ECMPt,
                                     DLB_ECMPt_NUM_PATHSf,
                                     1, &def_val, &num));
        lt_entry->num_paths = (uint8_t)def_val;
        BCMCTH_DLB_LT_FIELD_SET(lt_entry->fbmp,
                                BCMCTH_DLB_ECMP_LT_FIELD_NUM_PATHS);
    }

    if (!BCMCTH_DLB_LT_FIELD_GET(lt_entry->fbmp,
                                 BCMCTH_DLB_ECMP_LT_FIELD_NHOP_IDS)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit,
                                     DLB_ECMPt,
                                     DLB_ECMPt_NHOP_IDf,
                                     1, &def_val, &num));
        for (i = 0; i < BCMCTH_DLB_ECMP_GROUP_MAX_MEMBERS; i++) {
            lt_entry->nhop_id[i] = (bcmcth_dlb_nhop_id_t)def_val;
        }
        BCMCTH_DLB_LT_FIELD_SET(lt_entry->fbmp,
                                BCMCTH_DLB_ECMP_LT_FIELD_NHOP_IDS);
    }

    if (!BCMCTH_DLB_LT_FIELD_GET(lt_entry->fbmp,
                                 BCMCTH_DLB_ECMP_LT_FIELD_PORT_IDS)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit,
                                     DLB_ECMPt,
                                     DLB_ECMPt_PORT_IDf,
                                     1, &def_val, &num));
        for (i = 0; i < BCMCTH_DLB_ECMP_GROUP_MAX_MEMBERS; i++) {
            lt_entry->port_id[i] = (shr_port_t)def_val;
        }

        BCMCTH_DLB_LT_FIELD_SET(lt_entry->fbmp,
                                BCMCTH_DLB_ECMP_LT_FIELD_PORT_IDS);
    }

    if (!BCMCTH_DLB_LT_FIELD_GET(lt_entry->fbmp,
                                 BCMCTH_DLB_ECMP_LT_FIELD_ALTERNATE_NUM_PATHS)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit,
                                     DLB_ECMPt,
                                     DLB_ECMPt_ALTERNATE_NUM_PATHSf,
                                     1, &def_val, &num));
        lt_entry->num_alternate_paths = (uint8_t)def_val;
        BCMCTH_DLB_LT_FIELD_SET(lt_entry->fbmp,
                                BCMCTH_DLB_ECMP_LT_FIELD_ALTERNATE_NUM_PATHS);
    }

    if (!BCMCTH_DLB_LT_FIELD_GET(lt_entry->fbmp,
                                 BCMCTH_DLB_ECMP_LT_FIELD_ALTERNATE_NHOP_IDS)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit,
                                     DLB_ECMPt,
                                     DLB_ECMPt_ALTERNATE_NHOP_IDf,
                                     1, &def_val, &num));
        for (i = 0; i < BCMCTH_DLB_ECMP_GROUP_MAX_MEMBERS; i++) {
            lt_entry->alternate_nhop_id[i] = (bcmcth_dlb_nhop_id_t)def_val;
        }
        BCMCTH_DLB_LT_FIELD_SET(lt_entry->fbmp,
                                BCMCTH_DLB_ECMP_LT_FIELD_ALTERNATE_NHOP_IDS);
    }

    if (!BCMCTH_DLB_LT_FIELD_GET(lt_entry->fbmp,
                                 BCMCTH_DLB_ECMP_LT_FIELD_ALTERNATE_PORT_IDS)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit,
                                     DLB_ECMPt,
                                     DLB_ECMPt_ALTERNATE_PORT_IDf,
                                     1, &def_val, &num));
        for (i = 0; i < BCMCTH_DLB_ECMP_GROUP_MAX_MEMBERS; i++) {
            lt_entry->alternate_port_id[i] = (shr_port_t)def_val;
        }

        BCMCTH_DLB_LT_FIELD_SET(lt_entry->fbmp,
                                BCMCTH_DLB_ECMP_LT_FIELD_ALTERNATE_PORT_IDS);
    }
exit:
    SHR_FUNC_EXIT();
}

static int
dlb_ecmp_lt_fields_free(int unit,
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
dlb_ecmp_lt_fields_allocate (int unit,
                                bcmltd_fields_t* in,
                                size_t num_fields)
{
    uint32_t i;
    size_t alloc_sz;

    SHR_FUNC_ENTER(unit);

    /* Allocate fields buffers */
    alloc_sz = sizeof(bcmltd_field_t *) * num_fields;
    SHR_ALLOC(in->field, alloc_sz, "bcmcthDlbEcmpEntryFields");
    SHR_NULL_CHECK(in->field, SHR_E_MEMORY);
    sal_memset(in->field, 0, alloc_sz);

    in->count = num_fields;

    for (i = 0; i < num_fields; i++) {
        SHR_ALLOC(in->field[i], sizeof(bcmltd_field_t), "bcmcthDlbEcmpEntryField");
        SHR_NULL_CHECK(in->field[i], SHR_E_MEMORY);
        sal_memset(in->field[i], 0, sizeof(bcmltd_field_t));
    }

exit:
    if (SHR_FUNC_ERR()) {
        (void)dlb_ecmp_lt_fields_free(unit, in, num_fields);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Get all current imm DLB_ECMP LT fields.
 *
 * Get the current fields from imm DLB_ECMP LT
 *
 * \param [in] unit Unit number.
 * \param [out] entry DLB_ECMP LT entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to geet current LT entry fields.
 */
static int
dlb_ecmp_lt_fields_lookup(int unit,
                          bcmcth_dlb_ecmp_t *lt_entry)
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
        (dlb_ecmp_lt_fields_allocate(unit,
                                     &key_fields,
                                     1));

    key_fields.field[0]->id   = DLB_ECMPt_DLB_IDf;
    key_fields.field[0]->data = lt_entry->dlb_id;

    SHR_IF_ERR_EXIT
        (dlb_ecmp_lt_fields_allocate(unit,
                                     &imm_fields,
                                     num_fields));

    /* Lookup IMM table; error if entry not found */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_entry_lookup(unit,
                             DLB_ECMPt,
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
            case DLB_ECMPt_INACTIVITY_TIMEf:
                lt_entry->inactivity_time = (uint16_t) fval;
                break;
            case DLB_ECMPt_ASSIGNMENT_MODEf:
                lt_entry->assignment_mode = (uint8_t) fval;
                break;
            case DLB_ECMPt_FLOW_SET_SIZEf:
                lt_entry->flow_set_size = (uint8_t) fval;
                break;
            case DLB_ECMPt_ALTERNATE_PATH_COSTf:
                lt_entry->alternate_path_cost = (uint8_t) fval;
                break;
            case DLB_ECMPt_ALTERNATE_PATH_BIASf:
                lt_entry->alternate_path_bias = (uint8_t) fval;
                break;
            case DLB_ECMPt_PRIMARY_PATH_THRESHOLDf:
                lt_entry->primary_path_threshold = (uint8_t) fval;
                break;
            case DLB_ECMPt_NUM_PATHSf:
                lt_entry->num_paths = (uint8_t) fval;
                break;
            case DLB_ECMPt_NHOP_IDf:
                lt_entry->nhop_id[arr_idx] = (bcmcth_dlb_nhop_id_t) fval;
                break;
            case DLB_ECMPt_PORT_IDf:
                lt_entry->port_id[arr_idx] = (shr_port_t) fval;
                break;
            case DLB_ECMPt_ALTERNATE_NUM_PATHSf:
                lt_entry->num_alternate_paths = (shr_port_t) fval;
                break;
            case DLB_ECMPt_ALTERNATE_NHOP_IDf:
                lt_entry->alternate_nhop_id[arr_idx] = (bcmcth_dlb_nhop_id_t) fval;
                break;
            case DLB_ECMPt_ALTERNATE_PORT_IDf:
                lt_entry->alternate_port_id[arr_idx] = (uint8_t) fval;
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

exit:
    (void)dlb_ecmp_lt_fields_free(unit, &key_fields, 1);
    (void)dlb_ecmp_lt_fields_free(unit, &imm_fields, num_fields);
    SHR_FUNC_EXIT();
}


/*!
 * \brief Validate the ASSIGNMENT_MODE if is FIXED mode.
 *
 * \param [in]  unit      Unit number.
 * \param [in]  lt_entry  DLB_ECMP LT entry data buffer.
 * \param [out] is_fixed  Flag set to true if fixed mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Input parameter failed validation check.
 */
static int
dlb_ecmp_lt_entry_assignment_mode_is_fixed(int unit,
                                           bcmcth_dlb_ecmp_t *lt_entry,
                                           bool *is_fixed)
{
    int rv = SHR_E_NONE;
    uint32_t fixed_val = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);
    SHR_NULL_CHECK(is_fixed, SHR_E_PARAM);

    *is_fixed = FALSE;
    rv = bcmlrd_field_symbol_to_value(unit, DLB_ECMPt,
                                      DLB_ECMPt_ASSIGNMENT_MODEf,
                                      "FIXED", &fixed_val);
    if (rv != SHR_E_NONE) {
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
dlb_ecmp_lt_entry_fixed_flowset_init_get(int unit,
                                         bcmcth_dlb_ecmp_t *lt_entry,
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
        (dlb_ecmp_lt_entry_assignment_mode_is_fixed(unit,
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

    if (BCMCTH_DLB_LT_FIELD_GET(lt_entry->fbmp,
                                BCMCTH_DLB_ECMP_LT_FIELD_PORT_IDS)) {

        for (i = 0; i < lt_entry->num_paths; i++) {
            if (BCMCTH_DLB_LT_FIELD_GET(lt_entry->pri_port_array_bmp, i)) {
                *fixed_flowset_init = TRUE;
                SHR_EXIT();
            }
        }

    }

    if (BCMCTH_DLB_LT_FIELD_GET(lt_entry->fbmp,
                                BCMCTH_DLB_ECMP_LT_FIELD_ALTERNATE_PORT_IDS)) {

        for (i = 0; i < lt_entry->num_alternate_paths; i++) {
            if (BCMCTH_DLB_LT_FIELD_GET(lt_entry->alt_port_array_bmp, i)) {
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
 * Parse Insert or update the DLB_ECMP Lt entry
 *
 * \param [in]  unit Unit number.
 * \param [in]  op_arg Operation arguments.
 * \param [in]  sid LTD Symbol ID.
 * \param [in]  key field list.
 * \param [in]  data field list.
 * \param [out] event_reason reason code.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to insert or update the LT entry.
 */
static int
dlb_ecmp_update(int unit,
                const bcmltd_op_arg_t *op_arg,
                bcmltd_sid_t sid,
                const bcmltd_field_t *key,
                const bcmltd_field_t *data,
                bcmimm_entry_event_t event_reason)
{
    bcmcth_dlb_ecmp_t lt_entry;
    bool flow_set_init = FALSE;
    uint32_t prev_num_paths = 0;
    uint32_t prev_num_alternate_paths = 0;
    bool fixed_update_flowset_init = FALSE;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(key, SHR_E_PARAM);
    SHR_NULL_CHECK(data, SHR_E_PARAM);

    if (sid != DLB_ECMPt) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    sal_memset(&lt_entry, 0, sizeof(lt_entry));
    lt_entry.op_arg = op_arg;
    lt_entry.glt_sid = sid;
    SHR_IF_ERR_EXIT
        (dlb_ecmp_lt_fields_key_parse(unit, key, &lt_entry));

    if (event_reason == BCMIMM_ENTRY_UPDATE) {
        SHR_IF_ERR_EXIT
            (dlb_ecmp_lt_fields_lookup(unit, &lt_entry));
        lt_entry.prev_flow_set_size =
            BCMCTH_DLB_ECMP_TBL_FLOW_SET_SIZE_GET(unit, lt_entry.dlb_id);
        lt_entry.prev_flow_set_base =
            BCMCTH_DLB_ECMP_TBL_FLOW_SET_BASE_GET(unit, lt_entry.dlb_id);
        lt_entry.flow_set_base = lt_entry.prev_flow_set_base;
        prev_num_paths = lt_entry.num_paths;
        prev_num_alternate_paths = lt_entry.num_alternate_paths;
    }

    SHR_IF_ERR_EXIT
            (dlb_ecmp_lt_fields_data_parse(unit, data, &lt_entry));

    if (event_reason == BCMIMM_ENTRY_INSERT) {
        SHR_IF_ERR_EXIT
            (dlb_ecmp_lt_default_values_init(unit, &lt_entry));
        lt_entry.prev_flow_set_base = DLB_INVALID_FLOWSET_BASE;
    } else if (event_reason == BCMIMM_ENTRY_UPDATE) {
        SHR_IF_ERR_EXIT
            (dlb_ecmp_lt_entry_fixed_flowset_init_get(
                unit,
                &lt_entry,
                prev_num_paths,
                prev_num_alternate_paths,
                &fixed_update_flowset_init));
    }

    /*
     * In ASSIGNEMNT_MODE = FIXED , DLB_ECMP_FLOWSET
     * is reprogrammed if there is a new port addition
     * to the primary ports or alternate ports in LT
     * update operation.
     */
    if (fixed_update_flowset_init) {
        flow_set_init = TRUE;
    } else {
        if ((lt_entry.flow_set_size == 0) ||
            ((lt_entry.prev_flow_set_size) &&
             (lt_entry.flow_set_size == lt_entry.prev_flow_set_size))) {
            flow_set_init = FALSE;
        } else  {
            flow_set_init = TRUE;
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_dlb_ecmp_set(unit, &lt_entry, flow_set_init));

    if (BCMCTH_DLB_LT_FIELD_GET(lt_entry.fbmp,
                                BCMCTH_DLB_ECMP_LT_FIELD_FLOW_SET_SIZE)) {
        /* Set the flow_set_base in local bk. */
        BCMCTH_DLB_ECMP_TBL_FLOW_SET_BASE_SET(unit,
                                              lt_entry.dlb_id,
                                              lt_entry.flow_set_base);

        /* Set the flow_set_size in local bk. */
        BCMCTH_DLB_ECMP_TBL_FLOW_SET_SIZE_PTR_SET(unit,
                                                  lt_entry.dlb_id,
                                                  lt_entry.flow_set_size);
    }

    SHR_EXIT();
exit:

    SHR_FUNC_EXIT();
}


/*!
 * \brief DLB_ECMP LT entry delete
 *
 * \param [in] unit Unit number.
 * \param [in] op_arg Operation arguments.
 * \param [in] sid LTD Symbol ID.
 * \param [in] key List of the key fields.
 * \param [in] data List of the data fields.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
dlb_ecmp_delete(int unit,
                const bcmltd_op_arg_t *op_arg,
                bcmltd_sid_t sid,
                const bcmltd_field_t *key,
                const bcmltd_field_t *data)
{
    bcmcth_dlb_ecmp_t lt_entry;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(key, SHR_E_PARAM);

    sal_memset(&lt_entry, 0, sizeof(lt_entry));
    lt_entry.op_arg = op_arg;
    lt_entry.glt_sid = sid;

    SHR_IF_ERR_EXIT
        (dlb_ecmp_lt_fields_key_parse(unit, key, &lt_entry));
    lt_entry.flow_set_size =
        BCMCTH_DLB_ECMP_TBL_FLOW_SET_SIZE_GET(unit, lt_entry.dlb_id);
    lt_entry.flow_set_base =
        BCMCTH_DLB_ECMP_TBL_FLOW_SET_BASE_GET(unit, lt_entry.dlb_id);

    SHR_IF_ERR_EXIT
        (bcmcth_dlb_ecmp_del(unit, &lt_entry));

    /* Set the flow_set_base in bk. */
    BCMCTH_DLB_ECMP_TBL_FLOW_SET_BASE_SET(unit, lt_entry.dlb_id, 0);
    /* Set the flow_set_size in bk. */
    BCMCTH_DLB_ECMP_TBL_FLOW_SET_SIZE_PTR_SET(unit, lt_entry.dlb_id, 0);

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief DLB_ECMP LT stage callback function of IMM event handler.
 *
 * \param [in] unit Device unit number.
 * \param [in] sid Logical table ID.
 * \param [in] op_arg Operation arguments.
 * \param [in] event_reason Reason for the entry event.
 * \param [in] key_flds Linked list of the key fields identifying the entry.
 * \param [in] data_flds Linked list of the data fields in the modified entry.
 * \param [in] context Pointer that was given during registration.
 *                     The callback can use this pointer to retrieve some
 *                     context.
 * \param [out] output_fields This output parameter can be used by the component
 * to add fields into the entry. Typically this should be used for read-only
 * type fields that otherwise can not be set by the application.
 * The component must set the count field of the output_fields parameter to
 * indicate the actual number of fields that were set.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
dlb_ecmp_stage(int unit,
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
            SHR_IF_ERR_VERBOSE_EXIT
                (dlb_ecmp_update(unit, op_arg, sid,
                                 key, data, event_reason));
        }
        break;
    case BCMIMM_ENTRY_DELETE:
        SHR_IF_ERR_VERBOSE_EXIT
            (dlb_ecmp_delete(unit, op_arg, sid,
                             key, data));
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief DLB_ECMP imm commit call back
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
dlb_ecmp_commit(int unit,
                bcmltd_sid_t sid,
                uint32_t trans_id,
                void *context)
{
    int dlb_grp_cnt = 0;
    SHR_FUNC_ENTER(unit);

    if (sid != DLB_ECMPt) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_EXIT(bcmcth_dlb_grp_cnt_get(unit, &dlb_grp_cnt));

    sal_memcpy(BCMCTH_DLB_ECMP_TBL_HA_ENT(unit),
               BCMCTH_DLB_ECMP_TBL_ENT(unit),
               dlb_grp_cnt * sizeof(bcmcth_dlb_ecmp_entry_info_t));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief DLB_ECMP transaction abort
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
dlb_ecmp_abort(int unit,
               bcmltd_sid_t sid,
               uint32_t trans_id,
               void *context)
{
    int dlb_grp_cnt = 0;

    bcmcth_dlb_grp_cnt_get(unit, &dlb_grp_cnt);

    sal_memcpy(BCMCTH_DLB_ECMP_TBL_ENT(unit),
               BCMCTH_DLB_ECMP_TBL_HA_ENT(unit),
               dlb_grp_cnt * sizeof(bcmcth_dlb_ecmp_entry_info_t));

    bcmcth_dlb_ecmp_info_recover(unit, BCMCTH_DLB_ECMP_TBL_HA_ENT(unit));
}

/*!
 * \brief DLB_ECMPt In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to DLB_ECMPt logical table entry commit stages.
 */
static bcmimm_lt_cb_t bcmcth_dlb_ecmp_imm_callback = {

    /*! Validate function. */
    .validate = NULL,

    /*! Extended staging function. */
    .op_stage = dlb_ecmp_stage,

    /*! Commit function. */
    .commit = dlb_ecmp_commit,

    /*! Abort function. */
    .abort = dlb_ecmp_abort
};

/*******************************************************************************
 * Public Functions
 */

int
bcmcth_dlb_ecmp_resources_alloc(int unit, bool warm)
{
    int dlb_grp_cnt = 0;
    void *ptr;
    uint32_t alloc_sz;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmcth_dlb_grp_cnt_get(unit, &dlb_grp_cnt));

    ptr = BCMCTH_DLB_ECMP_INFO(unit);
    alloc_sz = sizeof(bcmcth_dlb_ecmp_info_t);
    SHR_ALLOC(ptr, alloc_sz, "bcmcthDlbEcmp");
    SHR_NULL_CHECK(ptr, SHR_E_MEMORY);
    BCMCTH_DLB_ECMP_INFO(unit) = ptr;
    sal_memset(ptr, 0, alloc_sz);

    alloc_sz = dlb_grp_cnt * sizeof(bcmcth_dlb_ecmp_entry_info_t);
    ptr = shr_ha_mem_alloc(unit,
                           BCMMGMT_DLB_COMP_ID,
                           BCMPTM_HA_SUBID_DLB_ECMP,
                           "bcmcthDlbEcmpTableHaInfo",
                           &alloc_sz);
    SHR_NULL_CHECK(ptr, SHR_E_MEMORY);
    BCMCTH_DLB_ECMP_TBL_HA_ENT(unit) = ptr;
    if (!warm) {
        sal_memset(ptr, 0, alloc_sz);

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmissu_struct_info_report(unit,
                BCMMGMT_DLB_COMP_ID,
                BCMPTM_HA_SUBID_DLB_ECMP,
                0,
                sizeof(bcmcth_dlb_ecmp_entry_info_t),
                dlb_grp_cnt,
                BCMCTH_DLB_ECMP_ENTRY_INFO_T_ID));
    }

    ptr = BCMCTH_DLB_ECMP_TBL_ENT(unit);
    alloc_sz = dlb_grp_cnt * sizeof(bcmcth_dlb_ecmp_entry_info_t);
    SHR_ALLOC(ptr, alloc_sz, "bcmcthDlbEcmpTableLocalInfo");
    SHR_NULL_CHECK(ptr, SHR_E_MEMORY);
    BCMCTH_DLB_ECMP_TBL_ENT(unit) = ptr;
    if (warm) {
        sal_memcpy(ptr, BCMCTH_DLB_ECMP_TBL_HA_ENT(unit), alloc_sz);
    } else {
        sal_memset(ptr, 0, alloc_sz);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_dlb_ecmp_resources_free(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(BCMCTH_DLB_ECMP_TBL_ENT(unit), SHR_E_PARAM);
    SHR_FREE(BCMCTH_DLB_ECMP_TBL_ENT(unit));

    SHR_NULL_CHECK(BCMCTH_DLB_ECMP_INFO(unit), SHR_E_PARAM);
    SHR_FREE(BCMCTH_DLB_ECMP_INFO(unit));
exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_dlb_ecmp_imm_init(int unit)
{
    const bcmlrd_map_t *map = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);

    /*
     * To register callback for DLB LTs here.
     */
    rv = bcmlrd_map_get(unit, DLB_ECMPt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 DLB_ECMPt,
                                 &bcmcth_dlb_ecmp_imm_callback,
                                 NULL));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_dlb_ecmp_info_get(int unit, bcmcth_dlb_ecmp_info_t **ecmp_info)
{
    SHR_FUNC_ENTER(unit);

    (*ecmp_info) = dlb_ecmp_info[unit];
    SHR_NULL_CHECK(*ecmp_info, SHR_E_INTERNAL);

exit:
    SHR_FUNC_EXIT();
}

