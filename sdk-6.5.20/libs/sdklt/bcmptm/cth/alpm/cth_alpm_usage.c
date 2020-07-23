/*! \file cth_alpm_usage.c
 *
 * Custom Handler for ALPM usage tables
 *
 * This file contains the main function for CTH ALPM USAGE.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_assert.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmevm/bcmevm_api.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmptm/bcmptm.h>
#include <bcmptm/bcmptm_rm_alpm_internal.h>
#include "cth_alpm_usage.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE  BSL_LS_BCMPTM_CTHALPM

/*******************************************************************************
 * Private functions
 */
static void
cth_alpm_usage_info_set(int u, uint64_t fval,
                        bcmptm_cth_alpm_usage_info_t *info)
{
    /* Set key type */
    switch (fval) {
        case KEY_TYPE_IPV4:
        case KEY_TYPE_IPV4_VRF:
        case KEY_TYPE_IPV4_OVERRIDE:
            info->kt = KEY_IPV4;
            break;
        case KEY_TYPE_IPV6_SINGLE:
        case KEY_TYPE_IPV6_SINGLE_VRF:
        case KEY_TYPE_IPV6_SINGLE_OVERRIDE:
            info->kt = KEY_IPV6_SINGLE;
            break;
        case KEY_TYPE_IPV6_DOUBLE:
        case KEY_TYPE_IPV6_DOUBLE_VRF:
        case KEY_TYPE_IPV6_DOUBLE_OVERRIDE:
            info->kt = KEY_IPV6_DOUBLE;
            break;
        case KEY_TYPE_IPV6_QUAD:
        case KEY_TYPE_IPV6_QUAD_VRF:
        case KEY_TYPE_IPV6_QUAD_OVERRIDE:
            info->kt = KEY_IPV6_QUAD;
            break;
        case KEY_TYPE_COMP_V4:
            info->kt = KEY_COMP0_V4;
            break;
        case KEY_TYPE_COMP_V6:
            info->kt = KEY_COMP0_V6;
            break;
        case KEY_TYPE_L3MC_V4:
            info->kt = KEY_L3MC_V4;
            break;
        case KEY_TYPE_L3MC_V6:
            info->kt = KEY_L3MC_V6;
            break;
        default:
            info->kt = KEY_TYPE_INVALID;
            break;
    }

    /* Set VRF type */
    switch (fval) {
        case KEY_TYPE_IPV4:
        case KEY_TYPE_IPV6_SINGLE:
        case KEY_TYPE_IPV6_DOUBLE:
        case KEY_TYPE_IPV6_QUAD:
            info->vt = VRF_GLOBAL;
            break;
        case KEY_TYPE_IPV4_VRF:
        case KEY_TYPE_IPV6_SINGLE_VRF:
        case KEY_TYPE_IPV6_DOUBLE_VRF:
        case KEY_TYPE_IPV6_QUAD_VRF:
            info->vt = VRF_PRIVATE;
            break;
        case KEY_TYPE_IPV4_OVERRIDE:
        case KEY_TYPE_IPV6_SINGLE_OVERRIDE:
        case KEY_TYPE_IPV6_DOUBLE_OVERRIDE:
        case KEY_TYPE_IPV6_QUAD_OVERRIDE:
            info->vt = VRF_OVERRIDE;
            break;
        case KEY_TYPE_COMP_V4:
        case KEY_TYPE_COMP_V6:
        case KEY_TYPE_L3MC_V4:
        case KEY_TYPE_L3MC_V6:
            info->vt = VRF_DEFAULT;
            break;
        default:
            info->vt = VRF_TYPE_INVALID;
            break;
    }
}

static void
cth_alpm_usage_keys_map(int u,
                        const bcmltd_fields_t *fields,
                        bcmptm_cth_alpm_usage_info_t *info)
{
    size_t idx;

    /* Parse data field. */
    for (idx = 0; idx < fields->count; idx++) {
        if (fields->field[idx]->id == L3_ALPM_LEVEL_1_USAGEt_KEY_TYPEf) {
            cth_alpm_usage_info_set(u, fields->field[idx]->data, info);
        }
        if ((fields->field[idx]->id == L3_ALPM_LEVEL_2_USAGEt_DBf) ||
            (fields->field[idx]->id == L3_ALPM_LEVEL_3_USAGEt_DBf)) {
            info->db = fields->field[idx]->data;
        }
    }
}

static int
cth_alpm_usage_fields_free(int unit, size_t num_fields,
                           bcmltd_fields_t *fields)
{
    size_t idx;
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(fields, SHR_E_PARAM);

    if (fields->field != NULL) {
        for (idx = 0; idx < num_fields; idx++) {
            if (fields->field[idx] != NULL) {
                shr_fmm_free(fields->field[idx]);
            }
        }
        SHR_FREE(fields->field);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
cth_alpm_usage_fields_alloc(int unit, size_t num_fields,
                            bcmltd_fields_t *fields)
{
    size_t idx;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(fields, SHR_E_PARAM);

    /* Allocate fields buffers */
    SHR_ALLOC(fields->field,
              sizeof(bcmltd_field_t *) * num_fields,
              "bcmcthAlpmUsageFields");

    SHR_NULL_CHECK(fields->field, SHR_E_MEMORY);
    sal_memset(fields->field, 0, sizeof(bcmltd_field_t *) * num_fields);
    fields->count = num_fields;

    for (idx = 0; idx < num_fields; idx++) {
        fields->field[idx] = shr_fmm_alloc();
        SHR_NULL_CHECK(fields->field[idx], SHR_E_MEMORY);
        sal_memset(fields->field[idx], 0, sizeof(bcmltd_field_t));
    }

exit:
    if (SHR_FUNC_ERR()) {
        cth_alpm_usage_fields_free(unit, num_fields, fields);
    }
    SHR_FUNC_EXIT();
}


int
bcmptm_cth_alpm_usage_pre_insert(int u, int mtop)
{
    uint32_t num_keys;
    int i;
    size_t idx;
    bcmltd_fields_t fields = {0};
    const bcmlrd_map_t *map = NULL;
    static bcmltd_sid_t sid[] = {
        L3_ALPM_LEVEL_1_USAGEt,
        L3_ALPM_LEVEL_2_USAGEt,
        L3_ALPM_LEVEL_3_USAGEt
    };
    uint32_t key_fids[] = {
        L3_ALPM_LEVEL_1_USAGEt_KEY_TYPEf,
        L3_ALPM_LEVEL_2_USAGEt_DBf,
        L3_ALPM_LEVEL_3_USAGEt_DBf
    };

    SHR_FUNC_ENTER(u);

    if (mtop == ALPM_1) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (cth_alpm_usage_fields_alloc(u, 1, &fields));

    for (i = 0; i < 3; i++) {
        if (SHR_SUCCESS(bcmlrd_map_get(u, sid[i], &map)) && map) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlrd_map_table_attr_get(u, sid[i],
                                   BCMLRD_MAP_TABLE_ATTRIBUTE_ENTRY_LIMIT,
                                   &num_keys));

            for (idx = 0; idx < num_keys; idx++) {
                fields.field[0]->id = key_fids[i];
                fields.field[0]->data = idx;
                SHR_IF_ERR_VERBOSE_EXIT(bcmimm_entry_insert(u, sid[i],
                                        &fields));
            }
        }
    }

exit:
    cth_alpm_usage_fields_free(u, 1, &fields);
    SHR_FUNC_EXIT();
}

static void
cth_alpm_usage_level1_update_fields(int u, bcmptm_cth_alpm_usage_info_t *info,
                                    bcmltd_fields_t *output_fields)
{
    uint32_t idx = 0;
    uint32_t fid;

    for (idx = 0; idx < output_fields->count; idx++) {
        fid = output_fields->field[idx]->id;
        switch (fid) {
            case L3_ALPM_LEVEL_1_USAGEt_MAX_ENTRIESf:
                output_fields->field[idx]->data = info->l1_max_entries;
                break;
            case L3_ALPM_LEVEL_1_USAGEt_INUSE_ENTRIESf:
                output_fields->field[idx]->data = info->l1_used_entries;
                break;
            case L3_ALPM_LEVEL_1_USAGEt_ENTRY_UTILIZATIONf:
                output_fields->field[idx]->data = info->l1_entries_utilization;
                break;
            default:
                break;
        }
    }
}

static void
cth_alpm_usage_leveln_update_fields(int u, bcmptm_cth_alpm_usage_info_t *info,
                                    bcmltd_fields_t *output_fields)
{
    uint32_t idx = 0;
    uint32_t fid;

    for (idx = 0; idx < output_fields->count; idx++) {
        fid = output_fields->field[idx]->id;
        switch (fid) {
            case L3_ALPM_LEVEL_2_USAGEt_MAX_RAW_BUCKETSf:
                output_fields->field[idx]->data = info->ln_max_rbkts;
                break;
            case L3_ALPM_LEVEL_2_USAGEt_INUSE_RAW_BUCKETSf:
                output_fields->field[idx]->data = info->ln_used_rbkts;
                break;
            case L3_ALPM_LEVEL_2_USAGEt_RAW_BUCKET_UTILIZATIONf:
                output_fields->field[idx]->data = info->ln_rbkts_utilization;
                break;
            case L3_ALPM_LEVEL_2_USAGEt_MAX_ENTRIESf:
                output_fields->field[idx]->data = info->ln_max_entries_rbkts;
                break;
            case L3_ALPM_LEVEL_2_USAGEt_INUSE_ENTRIESf:
                output_fields->field[idx]->data = info->ln_used_entries_rbkts;
                break;
            case L3_ALPM_LEVEL_2_USAGEt_ENTRY_UTILIZATIONf:
                output_fields->field[idx]->data = info->ln_entries_utilization_rbkts;
                break;
            default:
                break;
        }
    }
}

static int
cth_alpm_usage_update_fields(int u, bcmptm_cth_alpm_usage_info_t *info,
                             bcmltd_fields_t *output_fields, int level)
{
    SHR_FUNC_ENTER(u);
    SHR_NULL_CHECK(output_fields, SHR_E_PARAM);

    bcmptm_rm_alpm_usage_get(u, ALPM_0, level, info);

    if (level == LEVEL1) {
        cth_alpm_usage_level1_update_fields(u, info, output_fields);
    } else  {
        cth_alpm_usage_leveln_update_fields(u, info, output_fields);
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief L3_ALPM_LEVEL_#_USAGE IMM table change callback function for lookup.
 *
 * \param [in] u Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] trans_id Transaction ID.
 * \param [in] context Is a pointer that was given during registration.
 *                     The callback can use this pointer to retrieve some
 *                     context.
 * \param [in] lkup_type Indicates the type of lookup that is being performed.
 * \param [in] in Is a structure which contains the key fields but may include
 *                also other fields. The component should only focus on the
 *                key
 *                fields.
 * \param [in,out] out this structure contains all the fields of the table, so
 *                     that the component should not delete or add any
 *                     field to
 *                     this structure. The data values of the field were
 *                     set by
 *                     the IMM so the component may only overwrite the
 *                     data
 *                     section of the fields.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
cth_alpm_usage_imm_lookup(int u,
                          bcmltd_sid_t sid,
                          uint32_t trans_id,
                          void *context,
                          bcmimm_lookup_type_t lkup_type,
                          const bcmltd_fields_t *in,
                          bcmltd_fields_t *out)
{
    bcmptm_cth_alpm_usage_info_t info;
    const bcmltd_fields_t *key_fields = NULL;
    int level = 0;

    SHR_FUNC_ENTER(u);
    sal_memset(&info, 0, sizeof(bcmptm_cth_alpm_usage_info_t));

    /* key fields are in the "out" parameter for traverse operations */
    key_fields = (lkup_type == BCMIMM_TRAVERSE) ? out : in;

    /* Map LT key_types to RM_ALPM key and  VRF types. */
    cth_alpm_usage_keys_map(u, key_fields, &info);

    if (sid == L3_ALPM_LEVEL_1_USAGEt) {
        level = LEVEL1;
    } else if (sid == L3_ALPM_LEVEL_2_USAGEt) {
        level = LEVEL2;
    } else if (sid == L3_ALPM_LEVEL_3_USAGEt) {
        level = LEVEL3;
    }

    SHR_IF_ERR_EXIT(
        cth_alpm_usage_update_fields(u, &info, out, level));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief L3_ALPM_LEVEL_#_USAGE In-memory event callback structure.
 *
 * This structure contains callback functions that conrrespond
 * to L3_ALPM_LEVEL_#_USAGEt logical table entry commit stages.
 */
static bcmimm_lt_cb_t cth_alpm_usage_imm_cb = {

    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = NULL,

    /*! Lookup funtions. */
    .lookup = cth_alpm_usage_imm_lookup,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};

int
bcmptm_cth_alpm_usage_imm_register(int u, int mtop)
{
    const bcmlrd_map_t *map = NULL;

    SHR_FUNC_ENTER(u);

    if (SHR_SUCCESS(bcmlrd_map_get(u, L3_ALPM_LEVEL_1_USAGEt, &map)) && map) {
        SHR_IF_ERR_EXIT(
            bcmimm_lt_event_reg(u,
                                L3_ALPM_LEVEL_1_USAGEt,
                                &cth_alpm_usage_imm_cb,
                                NULL));
    }
    if (SHR_SUCCESS(bcmlrd_map_get(u, L3_ALPM_LEVEL_2_USAGEt, &map)) && map) {
        SHR_IF_ERR_EXIT(
            bcmimm_lt_event_reg(u,
                                L3_ALPM_LEVEL_2_USAGEt,
                                &cth_alpm_usage_imm_cb,
                                NULL));
    }
    if (SHR_SUCCESS(bcmlrd_map_get(u, L3_ALPM_LEVEL_3_USAGEt, &map)) && map) {
        SHR_IF_ERR_EXIT(
            bcmimm_lt_event_reg(u,
                                L3_ALPM_LEVEL_3_USAGEt,
                                &cth_alpm_usage_imm_cb,
                                NULL));
    }
exit:
    SHR_FUNC_EXIT();
}
