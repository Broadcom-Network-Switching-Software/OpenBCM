/*! \file cth_uft_be.c
 *
 * Backend part of CTH_UFT
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
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/chip/bcmltd_table_constants.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmptm/bcmptm.h>
#include <bcmptm/bcmptm_cth_uft_be_internal.h>
#include "cth_uft_be.h"

/*******************************************************************************
 * Defines
 */
#define CTH_UFT_BE_ALLOC(_ptr, _sz, _str) \
        SHR_ALLOC(_ptr, _sz, _str); \
        SHR_NULL_CHECK(_ptr, SHR_E_MEMORY)

#define BSL_LOG_MODULE  BSL_LS_BCMPTM_CTHUFT

#define UFT_ALPM_LEVEL_MAX (3)

/*******************************************************************************
 * Private variables
 */

static uft_be_dev_info_t* uft_be_dev_info[BCMDRD_CONFIG_MAX_UNITS] = {0};
static uft_be_driver_t* uft_be_driver[BCMDRD_CONFIG_MAX_UNITS] = {0};

/*! \brief bank types. */
typedef enum {
    BANK_RM_HASH,
    BANK_RM_TCAM,
    BANK_RM_ALPM
} bank_type_t;

/*******************************************************************************
 * Private Functions
 */

static inline int uft_grp_attri_to_bank_type(em_grp_attri_t grp_attri,
                                             em_bank_type_t *bank_type)
{
    switch (grp_attri) {
    case EM_GROUP_ATTRI_EM:
    case EM_GROUP_ATTRI_FP:
    case EM_GROUP_ATTRI_FT:
    case EM_GROUP_ATTRI_FT_FP:
    case EM_GROUP_ATTRI_ALPM_LV3:
    case EM_GROUP_ATTRI_L3UC_DST:
    case EM_GROUP_ATTRI_L3UC_SRC:
    case EM_GROUP_ATTRI_COMP_DST:
    case EM_GROUP_ATTRI_COMP_SRC:
    case EM_GROUP_ATTRI_L3UC_DST_MTOP:
    case EM_GROUP_ATTRI_L3UC_SRC_MTOP:
    case EM_GROUP_ATTRI_COMP_DST_MTOP:
    case EM_GROUP_ATTRI_COMP_SRC_MTOP:
        *bank_type = EM_BANK_TYPE_HASH;
        return SHR_E_NONE;
    case EM_GROUP_ATTRI_ALPM_LV2:
        *bank_type = EM_BANK_TYPE_MINI_BANK;
        return SHR_E_NONE;
    case EM_GROUP_ATTRI_ALPM_LV1:
    case EM_GROUP_ATTRI_TCAM:
        *bank_type = EM_BANK_TYPE_TCAM;
        return SHR_E_NONE;
    case EM_GROUP_ATTRI_INDEX:
        *bank_type = EM_BANK_TYPE_INDEX;
        return SHR_E_NONE;
    default:
        return SHR_E_PARAM;
    }
}

/*
 * Get paired bank for LOOKUP1 of a logical table.
 * It is used only for tile mode.
 */
static int
uft_lt_field_get(int unit, bcmlrd_sid_t ltid, uint32_t max_fields,
                 bcmlrd_fid_t key_fid, uint32_t key,
                 bcmlrd_fid_t val_fid, uint32_t *value)
{
    bcmltd_fields_t input = {0}, output = {0};
    size_t i;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(value, SHR_E_PARAM);

    input.count = 1;
    output.count = max_fields;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_uft_lt_fields_buff_alloc(unit, &input));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_uft_lt_fields_buff_alloc(unit, &output));

    input.field[0]->id = key_fid;
    input.field[0]->data = key;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_entry_lookup(unit, ltid, &input, &output));
    for (i = 0; i < output.count; i++) {
        if (output.field[i]->id == val_fid) {
            *value = output.field[i]->data;
            SHR_EXIT();
        }
    }

    SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
exit:
    (void) bcmptm_uft_lt_fields_buff_free(unit, &input);
    (void) bcmptm_uft_lt_fields_buff_free(unit, &output);
    SHR_FUNC_EXIT();
}

/*!
 * Get group id from ptsid.
 * It is only used for XGS device.
 *
 * \param [in] unit Device unit.
 * \param [in] ptsid Physical tabel id of a bank.
 * \param [out] grp_id Returned group id.
 *
 * \return SHR_E_XXX.
 */
static int
uft_ptsid_to_grpid(int unit, bcmdrd_sid_t ptsid, uint32_t *grp_id)
{
    int idx;
    uint32_t i;
    const bcmptm_uft_var_drv_t *var = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(grp_id, SHR_E_PARAM);
    SHR_NULL_CHECK(uft_be_dev_info[unit], SHR_E_INIT);

    var = uft_be_dev_info[unit]->var;
    if (var != NULL) {
        for (i = 0; i < var->em_group_cnt; i++) {
            if (ptsid == var->pt_info[i].pt_sid) {
                *grp_id = i;
                SHR_EXIT();
            }
        }
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    for (idx = 0; idx < uft_be_dev_info[unit]->pt_cnt; idx++) {
        if (ptsid == uft_be_dev_info[unit]->ptsid[idx]) {
            *grp_id = idx;
            SHR_EXIT();
        }
    }
    SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Get group id for a specific ptsid.
 * It is only used for XFS device.
 *
 * \param [in] unit Device unit.
 * \param [in] ptsid Physical tabel id of a bank.
 * \param [out] grp_id Returned group id.
 *
 * \return SHR_E_XXX.
 */
static int
uft_ptsid_to_bankid(int unit, bcmdrd_sid_t ptsid,
                    uint32_t *bank_id, bool *is_alpm)
{
    int idx;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bank_id, SHR_E_PARAM);
    SHR_NULL_CHECK(is_alpm, SHR_E_PARAM);

    if (uft_be_dev_info[unit] == NULL) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    for (idx = 0; idx < uft_be_dev_info[unit]->pt_cnt; idx++) {
        if (ptsid == uft_be_dev_info[unit]->bank_ptsid[idx]) {
            *bank_id = idx;
            *is_alpm = 0;
            SHR_EXIT();
        }
    }

    if (uft_be_dev_info[unit]->alpm_ptsid != NULL) {
        for (idx = 0; idx < uft_be_dev_info[unit]->pt_cnt; idx++) {
            if (ptsid == uft_be_dev_info[unit]->alpm_ptsid[idx]) {
                *bank_id = idx;
                *is_alpm = 1;
                SHR_EXIT();
            }
        }
    }

    SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Get offset value for a bank_id.
 * UFT mini bank doesn't have valid offset value, would return -7.
 *
 * \param [in] unit Device unit.
 * \param [in] ptsid Physical tabel id of a bank.
 * \param [out] grp_id Returned group id.
 *
 * \return SHR_E_XXX.
 */
static int
uft_bank_offset_get(int unit, uint32_t bank_id, uint8_t *offset)
{
    bcmltd_fields_t input = {0}, output = {0};
    uint32_t hash_bank_id = 0;
    size_t idx;
    int rv;

    SHR_FUNC_ENTER(unit);

    /* For non-hash bask, offset is set to 0. It is useless. */
    rv = bcmptm_uft_bank_global_to_hash(unit, bank_id, &hash_bank_id);
    if (rv != SHR_E_NONE) {
        *offset = 0;
        SHR_EXIT();
    }

    input.count  = 1;
    input.field  = NULL;
    output.count = DEVICE_EM_BANKt_FIELD_COUNT;
    output.field = NULL;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_uft_lt_fields_buff_alloc(unit, &input));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_uft_lt_fields_buff_alloc(unit, &output));
    input.field[0]->id = DEVICE_EM_BANKt_DEVICE_EM_BANK_IDf;
    input.field[0]->data = hash_bank_id;
    rv = bcmimm_entry_lookup(unit, DEVICE_EM_BANKt, &input, &output);
    if (rv == SHR_E_NONE) {
        for (idx = 0; idx < output.count; idx++) {
            if (output.field[idx]->id == DEVICE_EM_BANKt_OFFSETf) {
                *offset = output.field[idx]->data;
                break;
            }
        }
    } else if (rv == SHR_E_NOT_FOUND) {
        SHR_EXIT();
    } else {
        SHR_ERR_EXIT(rv);
    }

exit:
    (void) bcmptm_uft_lt_fields_buff_free(unit, &input);
    (void) bcmptm_uft_lt_fields_buff_free(unit, &output);
    SHR_FUNC_EXIT();
}

static int
uft_grp_cfg_get(int unit, uint32_t grp_id,
                uint32_t *bank_arr, uint32_t bank_size,
                uint32_t *r_bank_cnt,
                bool *robust, uint32_t *vec_type)
{
    bcmltd_fields_t input = {0}, output = {0};
    size_t idx;
    uint32_t fid, fidx;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    input.count  = 1;
    output.count = UFT_FIELD_COUNT_MAX;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_uft_lt_fields_buff_alloc(unit, &input));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_uft_lt_fields_buff_alloc(unit, &output));

    input.field[0]->id = DEVICE_EM_GROUPt_DEVICE_EM_GROUP_IDf;
    input.field[0]->data = grp_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_entry_lookup(unit, DEVICE_EM_GROUPt, &input, &output));
    for (idx = 0; idx < output.count; idx++) {
        fid = output.field[idx]->id;
        fidx = output.field[idx]->idx;
        fval = output.field[idx]->data;
        switch (fid) {
        case DEVICE_EM_GROUPt_NUM_BANKSf:
            if (r_bank_cnt) {
                *r_bank_cnt = fval;
            }
            break;
        case DEVICE_EM_GROUPt_DEVICE_EM_BANK_IDf:
            if (bank_arr) {
                if (fidx >= bank_size) {
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                bank_arr[fidx] = fval;
            }
            break;
        case DEVICE_EM_GROUPt_ROBUSTf:
            if (robust) {
                *robust = fval;
            }
            break;
        case DEVICE_EM_GROUPt_VECTOR_TYPEf:
            if (vec_type) {
                *vec_type = fval;
            }
            break;
        default:
            break;
        }
    }

exit:
    (void) bcmptm_uft_lt_fields_buff_free(unit, &input);
    (void) bcmptm_uft_lt_fields_buff_free(unit, &output);
    SHR_FUNC_EXIT();
}


static int
uft_grp_lt_parse(int unit, uint32_t grp_id,
                 bcmptm_pt_banks_info_t *banks_info)
{
    uint32_t v_pbank[UFT_VAL_BANKS_MAX] = {0};
    uint32_t f_pbank[UFT_VAL_BANKS_MAX] = {0};
    uint32_t tile_id[UFT_VAL_BANKS_MAX] = {0};
    uint32_t f_bank_cnt = 0, v_bank_cnt = 0;
    uint8_t *offset_ptr = NULL;
    size_t idx;
    bool robust_en = FALSE;
    int rv = 0;
    bcmptm_bank_attr_t *bank_attr = NULL;
    const bcmptm_uft_var_drv_t *var = uft_be_dev_info[unit]->var;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_uft_grp_fixed_bank_get(unit, grp_id,
                                       f_pbank, UFT_VAL_BANKS_MAX,
                                       &f_bank_cnt));
    SHR_IF_ERR_EXIT
        (uft_grp_cfg_get(unit, grp_id,
                         v_pbank, UFT_VAL_BANKS_MAX,
                         &v_bank_cnt,
                         &robust_en, &(banks_info->vector_type)));


    for (idx = 0; idx < f_bank_cnt; idx++) {
        bank_attr = &(banks_info->bank[idx]);
        offset_ptr = &(bank_attr->hash_offset);
        SHR_IF_ERR_VERBOSE_EXIT
            (uft_bank_offset_get(unit, f_pbank[idx], offset_ptr));
        bank_attr->bank_id = f_pbank[idx];
    }
    for (idx = 0; idx < v_bank_cnt; idx++) {
        bank_attr = &(banks_info->bank[idx + f_bank_cnt]);
        offset_ptr = &(bank_attr->hash_offset);
        SHR_IF_ERR_VERBOSE_EXIT
            (uft_bank_offset_get(unit, v_pbank[idx], offset_ptr++));
        bank_attr->bank_id = v_pbank[idx];
    }

    banks_info->bank_cnt = f_bank_cnt + v_bank_cnt;

    for (idx = 0; idx < banks_info->bank_cnt; idx++) {
        bank_attr = &(banks_info->bank[idx]);
        if ((var == NULL) || (var->legacy == TRUE)) {
            bank_attr->robust_en = robust_en;
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_uft_bank_info_get(unit, bank_attr->bank_id,
                                          FALSE,
                                          &(bank_attr->bank_sid),
                                          &(tile_id[idx])));

             rv = bcmptm_uft_tile_cfg_get(unit, tile_id[idx],
                                          NULL,
                                          &(bank_attr->robust_en),
                                          &(bank_attr->seed));
             SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check whether a bank is assigned to a given group.
 * Used in bank management based on tile mode.
 *
 * \param [in] unit Device unit.
 * \param [in] bank_id Bank ID.
 * \param [in] alpm Bank type.
 * \param [out] used TRUE: bank is used. FALSE: bank is not used.
 *
 * \return SHR_E_XXX.
 */
static int
uft_bank_assigned(int unit, uint32_t bank_id, uint32_t grp_id, bool *assigned)
{
    bcmltd_fields_t input = {0}, output = {0};
    uint32_t v_bank[UFT_VAL_BANKS_MAX] = {0};
    uint32_t v_bank_cnt = 0;
    uint32_t fid, fidx;
    uint64_t fval;
    uint32_t idx;

    SHR_FUNC_ENTER(unit);

    input.count  = 1;
    output.count = UFT_FIELD_COUNT_MAX;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_uft_lt_fields_buff_alloc(unit, &input));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_uft_lt_fields_buff_alloc(unit, &output));

    input.field[0]->id = DEVICE_EM_GROUPt_DEVICE_EM_GROUP_IDf;
    input.field[0]->data = grp_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_entry_lookup(unit, DEVICE_EM_GROUPt, &input, &output));
    for (idx = 0; idx < output.count; idx++) {
        fid = output.field[idx]->id;
        if (fid == DEVICE_EM_GROUPt_DEVICE_EM_BANK_IDf) {
            fval = output.field[idx]->data;
            fidx = output.field[idx]->idx;
            v_bank[fidx] = fval;
        } else if (fid == DEVICE_EM_GROUPt_NUM_BANKSf) {
            v_bank_cnt = output.field[idx]->data;
        }
    }

    for (idx = 0; idx < v_bank_cnt; idx++) {
        if (v_bank[idx] == bank_id) {
            *assigned = TRUE;
            SHR_EXIT();
        }
    }
    *assigned = FALSE;

exit:
    (void) bcmptm_uft_lt_fields_buff_free(unit, &input);
    (void) bcmptm_uft_lt_fields_buff_free(unit, &output);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get banks info for a given ptsid.
 * It is only used for tile mode based bank management.
 *
 * \param [in] unit Device unit.
 * \param [in] type Bank type.
 * \param [in] ptsid PT symbol ID.
 * \param [out] banks_info Returned banks info.
 *
 * \return SHR_E_XXX.
 */
static int
uft_bank_info_get_tile_mode(int unit,
                            bank_type_t type,
                            bcmdrd_sid_t ptsid,
                            bcmptm_pt_banks_info_t *banks_info)
{
    uint32_t bank_id = 0, tile_id, tile_mode;
    bool is_alpm, grp_alpm;
    bool assigned = 0;
    uint32_t grp_arr[TABLE_EM_GROUPS_MAX] = {0};
    uint32_t r_grp_cnt = 0;
    uint32_t idx;
    const em_grp_info_t **em_grp;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    sal_memset(banks_info, 0, sizeof(bcmptm_pt_banks_info_t));
    rv = uft_ptsid_to_bankid(unit, ptsid, &bank_id, &is_alpm);
    if (SHR_FAILURE(rv)) {
        if (rv == SHR_E_NOT_FOUND) {
            SHR_EXIT();
        } else {
            SHR_ERR_EXIT(rv);
        }
    }

    if (type == BANK_RM_ALPM) {
        is_alpm = TRUE;
    } else if (type == BANK_RM_TCAM) {
        is_alpm = FALSE;
    }

    if (bank_id >= uft_be_dev_info[unit]->var->em_bank_cnt) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /*
     * For bank management based on tile mode, one bank can only belong to
     * a tile. Then using current tile mode, all valid groups are found.
     */
    tile_id = uft_be_dev_info[unit]->var->tile_id[bank_id];
    SHR_IF_ERR_EXIT
        (bcmptm_uft_tile_cfg_get(unit, tile_id, &tile_mode, NULL, NULL));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_uft_grp_get_per_tile_mode(unit, tile_mode,
                                          grp_arr, TABLE_EM_GROUPS_MAX,
                                          &r_grp_cnt));

    em_grp = uft_be_dev_info[unit]->var->em_group;
    for (idx = 0; idx < r_grp_cnt; idx++) {
        grp_alpm = em_grp[grp_arr[idx]]->is_alpm;
        if (grp_alpm == is_alpm) {
            assigned = FALSE;
            SHR_IF_ERR_EXIT
                (uft_bank_assigned(unit, bank_id, grp_arr[idx], &assigned));
            if (assigned) {
                banks_info->bank_cnt = 1;
                banks_info->bank[0].bank_id = bank_id;
                SHR_EXIT();
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
uft_grp_validate(int unit, uint32_t grp_id, bool *valid)
{
    uint32_t primary_grp = 0, bank_cnt = 0;

    SHR_FUNC_ENTER(unit);

    *valid = TRUE;
    if (uft_be_dev_info[unit]->primary_grp != NULL) {
        primary_grp = uft_be_dev_info[unit]->primary_grp[grp_id];
        if (primary_grp != grp_id) {
            SHR_IF_ERR_VERBOSE_EXIT
                (uft_grp_cfg_get(unit, primary_grp, NULL, 0,
                                 &bank_cnt, NULL, NULL));
            /*
             * When group's primary group's bank count is non-zero,
             * this group becomes invalid.
             */
            if (bank_cnt) {
                *valid = FALSE;
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get banks info for a given ptsid.
 *
 * \param [in] unit Device unit.
 * \param [in] type Bank type.
 * \param [in] ptsid PT symbol ID.
 * \param [out] banks_info Returned banks info.
 *
 * \return SHR_E_XXX.
 */
static int
uft_banks_info_get(int unit,
                   bank_type_t type,
                   bcmdrd_sid_t ptsid,
                   bcmptm_pt_banks_info_t *banks_info)
{
    uint32_t grp_id = 0;
    bool grp_valid = TRUE;

    SHR_FUNC_ENTER(unit);

    if (uft_be_dev_info[unit] == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    sal_memset(banks_info, 0, sizeof(bcmptm_pt_banks_info_t));

    if (uft_be_dev_info[unit]->var != NULL &&
        uft_be_dev_info[unit]->var->legacy == FALSE) {
        return uft_bank_info_get_tile_mode(unit, type, ptsid, banks_info);
    } else {
        /*
         * For XGS device, each ptsid represents the EM_GROUP_ID.
         * For XFS device, ecah ptsid represents the EM_BANK_ID.
         */
        SHR_IF_ERR_VERBOSE_EXIT
            (uft_ptsid_to_grpid(unit, ptsid, &grp_id));
        SHR_IF_ERR_VERBOSE_EXIT
            (uft_grp_validate(unit, grp_id, &grp_valid));
        if (!grp_valid) {
            SHR_EXIT();
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (uft_grp_lt_parse(unit, grp_id, banks_info));
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get fixed or flexible banks for a group.
 *
 * \param [in] unit Device unit.
 * \param [in] grp_id Group ID.
 * \param [out] bank_arr Returned bank array.
 * \param [in] bank_size Size of bank array.
 * \param [out] r_cnt Actual returned count of banks.
 *
 * \return SHR_E_XXX.
 */
static int
uft_grp_bank_get_tile_mode(int unit,
                           uint32_t grp_id,
                           uint32_t *bank_arr,
                           uint32_t  bank_size,
                           uint32_t *r_bank_cnt)
{
    bcmltd_fields_t input = {0}, output = {0};
    int rv;
    size_t idx;
    uint32_t fid, fidx;
    uint64_t fval;
    uint32_t f_bank[UFT_VAL_BANKS_MAX] = {0};
    uint32_t v_bank[UFT_VAL_BANKS_MAX] = {0};
    uint32_t f_bank_cnt = 0, v_bank_cnt = 0;
    uint32_t cnt;

    SHR_FUNC_ENTER(unit);

    input.count  = 1;
    output.count = UFT_FIELD_COUNT_MAX;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_uft_lt_fields_buff_alloc(unit, &input));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_uft_lt_fields_buff_alloc(unit, &output));

    input.field[0]->id = DEVICE_EM_GROUPt_DEVICE_EM_GROUP_IDf;
    input.field[0]->data = grp_id;
    rv = bcmimm_entry_lookup(unit, DEVICE_EM_GROUPt, &input, &output);
    if (rv == SHR_E_NOT_FOUND) {
        *r_bank_cnt = 0;
        SHR_EXIT();
    } else if (rv != SHR_E_NONE) {
        SHR_ERR_EXIT(rv);
    }

    for (idx = 0; idx < output.count; idx++) {
        fid = output.field[idx]->id;
        fval = output.field[idx]->data;
        fidx = output.field[idx]->idx;
        switch (fid) {
        case DEVICE_EM_GROUPt_DEVICE_EM_BANK_IDf:
            v_bank[fidx] = fval;
            break;
        case DEVICE_EM_GROUPt_NUM_BANKSf:
            v_bank_cnt = fval;
            break;
        default:
            break;
        }
    }

    SHR_IF_ERR_EXIT
        (bcmptm_uft_grp_fixed_bank_get(unit, grp_id,
                                       f_bank, UFT_VAL_BANKS_MAX,
                                       &f_bank_cnt));

    /*
     * For a group, we need to get both fixed banks and flexible banks.
     * But on current tile mode support design, a group should only has
     * fixed banks or only has flexible banks.
     */
    cnt = 0;
    for (idx = 0; idx < f_bank_cnt; idx++) {
        if (cnt >= bank_size) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        bank_arr[cnt++] = f_bank[idx];
    }
    for (idx = 0; idx < v_bank_cnt; idx++) {
        if (cnt >= bank_size) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        bank_arr[cnt++] = v_bank[idx];
    }

    *r_bank_cnt = cnt;

exit:
    (void) bcmptm_uft_lt_fields_buff_free(unit, &input);
    (void) bcmptm_uft_lt_fields_buff_free(unit, &output);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get mapped group for a logical table.
 * ALPM LTs need to pass the bank type through alpm level.
 * It is only used for tile mode based bank management.
 *
 * \param [in] unit Device unit.
 * \param [in] ltid Logical table ID.
 * \param [in] alpm_level ALPM level.
 * \param [in] lookup_num Lookup number of LT.
 * \param [in] grp_valid Array pointer for indicating validity of all groups.
 * \param [out] grp_arr Returned group array.
 * \param [in] grp_size Size of group array.
 * \param [out] r_grp_cnt Actual returned count of groups.
 *
 * \return SHR_E_XXX.
 */
static int
uft_grp_get(int unit, bcmltd_sid_t ltid,
            uint32_t alpm_level, int lookup_num, bool *grp_valid,
            uint32_t *grp_arr, uint32_t grp_size, uint32_t *r_grp_cnt)
{
    size_t idx, idy;
    uint32_t cnt = 0;
    const bcmptm_uft_var_drv_t *var;
    const em_grp_info_t **em_grp;

    SHR_FUNC_ENTER(unit);

    if (alpm_level > UFT_ALPM_LEVEL_MAX) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    var = uft_be_dev_info[unit]->var;
    SHR_NULL_CHECK(var, SHR_E_PARAM);
    em_grp = var->em_group;

    for (idx = 0; idx < var->em_group_cnt; idx++) {
        if (em_grp[idx]->grp_id != idx) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        if (!grp_valid[idx]) {
            continue;
        }
        if (alpm_level != em_grp[idx]->alpm_level) {
            continue;
        }
        if (lookup_num == 0) {
            for (idy = 0; idy < em_grp[idx]->lookup0_lt_count; idy++) {
                if (em_grp[idx]->lookup0_lt[idy] == ltid) {
                    if (grp_arr != NULL) {
                        if (cnt >= grp_size) {
                            SHR_ERR_EXIT(SHR_E_INTERNAL);
                        }
                        grp_arr[cnt] = idx;
                        cnt++;
                    }
                }
            }
        } else {
            for (idy = 0; idy < em_grp[idx]->lookup1_lt_count; idy++) {
                if (em_grp[idx]->lookup1_lt[idy] == ltid) {
                    if (grp_arr != NULL) {
                        if (cnt >= grp_size) {
                            SHR_ERR_EXIT(SHR_E_INTERNAL);
                        }
                        grp_arr[cnt] = idx;
                        cnt++;
                    }
                }
            }
        }
    }

    *r_grp_cnt = cnt;

exit:
    SHR_FUNC_EXIT();
}

/*
 * Get paired bank for LOOKUP1 of a logical table.
 * It is used only for tile mode.
 */
static int
uft_paired_bank_get(int unit, uint32_t bank_id, uint32_t *pair_bank)
{
    uint32_t invalid_bank = 0;
    uint32_t pair_key = 0, paired_value = 0;
    int rv = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(uft_be_dev_info[unit], SHR_E_INIT);
    SHR_NULL_CHECK(pair_bank, SHR_E_PARAM);

    rv = bcmptm_uft_bank_global_to_pair(unit, bank_id, &pair_key);
    if (rv != SHR_E_NONE) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (uft_lt_field_get(unit,
                          DEVICE_EM_BANK_PAIRt,
                          DEVICE_EM_BANK_PAIRt_FIELD_COUNT,
                          DEVICE_EM_BANK_PAIRt_DEVICE_EM_BANK_IDf,
                          pair_key,
                          DEVICE_EM_BANK_PAIRt_PAIRED_DEVICE_EM_BANK_IDf,
                          &paired_value));

    SHR_IF_ERR_EXIT
        (bcmlrd_field_symbol_to_value
            (unit, DEVICE_EM_BANK_PAIRt,
             DEVICE_EM_BANK_PAIRt_PAIRED_DEVICE_EM_BANK_IDf,
             "INVALID", &invalid_bank));
    if (paired_value == invalid_bank) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_EXIT
        (bcmptm_uft_bank_pair_to_global(unit, paired_value, pair_bank));
exit:
    SHR_FUNC_EXIT();
}

static int
uft_em_tile_mode_check(int unit, bool *tile_mode_valid, uint32_t tile_mode_cnt)
{
    const bcmptm_uft_var_drv_t *var = NULL;
    uint32_t idx = 0;
    uint32_t mode = 0;

    SHR_FUNC_ENTER(unit);

    var = uft_be_dev_info[unit]->var;
    SHR_NULL_CHECK(var, SHR_E_PARAM);
    if (var->tile_mode_cnt != tile_mode_cnt) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    for (idx = 0; idx < var->em_tile_cnt; idx++) {
        SHR_IF_ERR_EXIT_VERBOSE_IF
            (bcmptm_uft_tile_cfg_get(unit, idx, &mode, NULL, NULL),
             SHR_E_NOT_FOUND);
        if (mode >= var->tile_mode_cnt) {
            /* Mode is disabled. */
            continue;
        }
        tile_mode_valid[mode] = TRUE;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
uft_em_grp_check(int unit, bool *grp_valid, uint32_t grp_cnt)
{
    const bcmptm_uft_var_drv_t *var = NULL;
    size_t size;
    bool *tile_mode_valid = NULL;
    uint32_t idx = 0;
    const em_grp_info_t *em_grp = NULL;

    SHR_FUNC_ENTER(unit);

    /* Calculates all the tile's current working mode. */
    var = uft_be_dev_info[unit]->var;
    SHR_NULL_CHECK(var, SHR_E_PARAM);
    size = var->tile_mode_cnt * sizeof(bool);
    tile_mode_valid = sal_alloc(size, "bcmptmUftTileModeValid");
    SHR_NULL_CHECK(tile_mode_valid, SHR_E_MEMORY);
    sal_memset(tile_mode_valid, 0, size);
    SHR_IF_ERR_EXIT_VERBOSE_IF
        (uft_em_tile_mode_check(unit, tile_mode_valid, var->tile_mode_cnt),
         SHR_E_NOT_FOUND);

    for (idx = 0; idx < var->em_group_cnt; idx++) {
        em_grp = var->em_group[idx];
        if (em_grp->grp_id != idx) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        if (tile_mode_valid[em_grp->tile_mode]) {
            grp_valid[idx] = TRUE;
        } else {
            grp_valid[idx] = FALSE;
        }
    }

exit:
    SHR_FREE(tile_mode_valid);
    SHR_FUNC_EXIT();
}

static int
uft_bank_info_get_from_lt_tile_mode(int unit,
                                    bcmltd_sid_t ltid,
                                    int alpm_level,
                                    bcmptm_pt_banks_info_t *banks_info)
{
    int rv;
    uint32_t *lookup0_grp_p = NULL, *lookup1_grp_p = NULL;
    uint32_t r_lookup0_grp_cnt = 0, r_lookup1_grp_cnt = 0;
    uint32_t *lookup0_bank_p = NULL, *lookup1_bank_p = NULL;
    uint32_t r_lookup0_bank_cnt = 0, r_lookup1_bank_cnt = 0;
    size_t size;
    uint32_t idx, cnt;
    uint32_t lt_lookups = 1;
    uint32_t pair_bank_id = 0;
    uint32_t tile_id;
    bcmptm_bank_attr_t *bank_attr = NULL, *p_bank_attr = NULL;
    /* Restore the valid groups in real time. */
    bool *grp_valid = NULL;
    uint32_t grp_cnt = uft_be_dev_info[unit]->var->em_group_cnt;

    SHR_FUNC_ENTER(unit);

    sal_memset(banks_info, 0, sizeof(bcmptm_pt_banks_info_t));

    size = sizeof(uint32_t) * TABLE_EM_GROUPS_MAX;
    lookup0_grp_p = sal_alloc(size, "bcmptmUftGrpIDArr");
    SHR_NULL_CHECK(lookup0_grp_p, SHR_E_MEMORY);
    sal_memset(lookup0_grp_p, 0, size);
    lookup1_grp_p = sal_alloc(size, "bcmptmUftGrpIDArr");
    SHR_NULL_CHECK(lookup1_grp_p, SHR_E_MEMORY);
    sal_memset(lookup1_grp_p, 0, size);

    size = sizeof(uint32_t) * UFT_VAL_BANKS_MAX;
    lookup0_bank_p = sal_alloc(size, "bcmptmUftBankIDArr");
    SHR_NULL_CHECK(lookup0_bank_p, SHR_E_MEMORY);
    sal_memset(lookup0_bank_p, 0, size);
    lookup1_bank_p = sal_alloc(size, "bcmptmUftBankIDArr");
    SHR_NULL_CHECK(lookup1_bank_p, SHR_E_MEMORY);
    sal_memset(lookup1_bank_p, 0, size);

    /* Calculate the valid groups in real time. */
    size = sizeof(bool) * grp_cnt;
    grp_valid = sal_alloc(size, "bcmptmUftGrpValidArr");
    SHR_NULL_CHECK(grp_valid, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT_VERBOSE_IF
        (uft_em_grp_check(unit, grp_valid, grp_cnt),
         SHR_E_NOT_FOUND);

    /* Get group array for a LT's lookup0. */
    SHR_IF_ERR_VERBOSE_EXIT
        (uft_grp_get(unit, ltid, alpm_level, 0, grp_valid,
                     lookup0_grp_p, TABLE_EM_GROUPS_MAX, &r_lookup0_grp_cnt));
    if (r_lookup0_grp_cnt == 0) {
        SHR_EXIT();
    }

    /*
     * For tile mode support bank management,
     * a LT is mapped to several groups.
     * Each hash group is assigned with up to 2 banks.
     * Each tcam group is assigned with up to 16 banks.
     */
    cnt = 0;
    for (idx = 0; idx < r_lookup0_grp_cnt; idx++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (uft_grp_bank_get_tile_mode(unit,
                                        lookup0_grp_p[idx],
                                        lookup0_bank_p + cnt,
                                        UFT_VAL_BANKS_MAX - cnt,
                                        &r_lookup0_bank_cnt));

        cnt += r_lookup0_bank_cnt;
    }
    r_lookup0_bank_cnt = cnt;

    /* Check if a LT might require 2 lookup. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_uft_lt_lookup_cnt_get(unit, ltid, &lt_lookups));

    if (lt_lookups == 2) {
        /* Get group array for a LT's lookup1. */
        SHR_IF_ERR_VERBOSE_EXIT
            (uft_grp_get(unit, ltid, alpm_level, 1, grp_valid,
                         lookup1_grp_p, TABLE_EM_GROUPS_MAX,
                         &r_lookup1_grp_cnt));

        cnt = 0;
        for (idx = 0; idx < r_lookup1_grp_cnt; idx++) {
            SHR_IF_ERR_VERBOSE_EXIT
                (uft_grp_bank_get_tile_mode(unit,
                                            lookup1_grp_p[idx],
                                            lookup1_bank_p + cnt,
                                            UFT_VAL_BANKS_MAX - cnt,
                                            &r_lookup1_bank_cnt));

            cnt += r_lookup1_bank_cnt;
        }
        r_lookup1_bank_cnt = cnt;
    }

    for (idx = 0; idx < r_lookup0_bank_cnt; idx++) {
        banks_info->bank[idx].bank_id = lookup0_bank_p[idx];
        banks_info->paired_bank[idx].bank_sid = INVALIDm;
    }
    banks_info->bank_cnt = r_lookup0_bank_cnt;

    if (lt_lookups == 2) {
        for (idx = 0; idx < banks_info->bank_cnt; idx++) {
            p_bank_attr = &(banks_info->paired_bank[idx]);
            rv = uft_paired_bank_get(unit,
                                     lookup0_bank_p[idx],
                                     &pair_bank_id);
            if (rv == SHR_E_NONE) {
                p_bank_attr->bank_id = pair_bank_id;
                SHR_IF_ERR_EXIT
                    (bcmptm_uft_bank_info_get(unit, pair_bank_id, alpm_level,
                                              &(p_bank_attr->bank_sid), NULL));
            }
        }
    }

    /*
     * After find all the bank id information,
     * get the related vector_type, robust, seed, offset and sid info.
     * vector_type is fixed for XFS device.
     */
    banks_info->vector_type = BCMPTM_RM_HASH_VEC_CRC32A_CRC32B;
    for (idx = 0; idx < banks_info->bank_cnt; idx++) {
        bank_attr = &(banks_info->bank[idx]);
        SHR_IF_ERR_EXIT
            (uft_bank_offset_get(unit, bank_attr->bank_id,
                                 &(bank_attr->hash_offset)));
        SHR_IF_ERR_EXIT
            (bcmptm_uft_bank_info_get(unit, bank_attr->bank_id,
                                      alpm_level,
                                      &(bank_attr->bank_sid),
                                      &tile_id));
        SHR_IF_ERR_EXIT
            (bcmptm_uft_tile_cfg_get(unit, tile_id,
                                     NULL,
                                     &(bank_attr->robust_en),
                                     &(bank_attr->seed)));
    }

    /* Get the related robust, seed, offset and sid info for paired banks. */
    if (lt_lookups == 2) {
        for (idx = 0; idx < banks_info->bank_cnt; idx++) {
            p_bank_attr = &(banks_info->paired_bank[idx]);
            if (p_bank_attr->bank_sid == INVALIDm) {
                continue;
            }
            SHR_IF_ERR_EXIT
                (uft_bank_offset_get(unit, p_bank_attr->bank_id,
                                     &(p_bank_attr->hash_offset)));
            SHR_IF_ERR_EXIT
                (bcmptm_uft_bank_info_get(unit, p_bank_attr->bank_id,
                                          alpm_level, NULL,
                                          &tile_id));
            SHR_IF_ERR_EXIT
                (bcmptm_uft_tile_cfg_get(unit, tile_id,
                                         NULL,
                                         &(p_bank_attr->robust_en),
                                         &(p_bank_attr->seed)));
       }
    }


    /* Only returns bank_sid for lookup1 banks. */
    if (lt_lookups == 2) {
        banks_info->lookup1_bank_cnt = r_lookup1_bank_cnt;
        for (idx = 0; idx < banks_info->lookup1_bank_cnt; idx++) {
            SHR_IF_ERR_EXIT
                (bcmptm_uft_bank_info_get
                    (unit, lookup1_bank_p[idx],
                     alpm_level, &(banks_info->lookup1_bank_sid[idx]), NULL));
       }
    }

exit:
    SHR_FREE(lookup0_grp_p);
    SHR_FREE(lookup1_grp_p);
    SHR_FREE(lookup0_bank_p);
    SHR_FREE(lookup1_bank_p);
    SHR_FREE(grp_valid);
    SHR_FUNC_EXIT();
}

static int
uft_lt_hw_read(int unit, bcmltd_sid_t lt_id,
               const bcmltd_op_arg_t *op_arg,
               bcmdrd_sid_t pt_id, int index,
               uint32_t entry_size, void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    uint32_t rsp_entry_wsize;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;
    uint64_t req_flags = 0;

    SHR_FUNC_ENTER(unit);

    pt_info.index = index;
    pt_info.tbl_inst = -1;
    rsp_entry_wsize = bcmdrd_pt_entry_wsize(unit, pt_id);
    if (entry_size < rsp_entry_wsize) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit, req_flags, pt_id, &pt_info,
                                    NULL, NULL, BCMPTM_OP_READ,
                                    NULL, rsp_entry_wsize, lt_id,
                                    op_arg->trans_id, NULL, NULL,
                                    entry_data, &rsp_ltid, &rsp_flags));

 exit:
    SHR_FUNC_EXIT();
}

static int
uft_lt_hw_write(int unit, bcmltd_sid_t lt_id,
                const bcmltd_op_arg_t *op_arg,
                bcmdrd_sid_t pt_id, int index, void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;
    uint64_t req_flags = 0;

    SHR_FUNC_ENTER(unit);

    pt_info.index = index;
    pt_info.tbl_inst = -1;
    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit, req_flags, pt_id, &pt_info,
                                    NULL, NULL, BCMPTM_OP_WRITE,
                                    entry_data, 0, lt_id,
                                    op_arg->trans_id, NULL, NULL,
                                    NULL, &rsp_ltid, &rsp_flags));

 exit:
    SHR_FUNC_EXIT();
}

static int
uft_fc_hit_context_set(int unit,
                       bcmltd_sid_t lt_id,
                       const bcmltd_op_arg_t *op_arg,
                       uint32_t tile_id,
                       uint32_t index,
                       uint32_t hit_context)
{
    const bcmptm_uft_var_drv_t *var = NULL;
    uint32_t i;
    bool found = TRUE;
    bcmdrd_sid_t ptsid = INVALIDm;
    bcmdrd_fid_t fid = INVALIDf;
    uint32_t entry[UFT_MAX_PT_ENTRY_WORDS];
    const char *tile_name = NULL;
    const uft_tile_hit_profile_info_t *hit_prof;
    uint32_t hit_prof_cnt = 0;

    SHR_FUNC_ENTER(unit);

    if (uft_be_dev_info[unit] == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }
    var = uft_be_dev_info[unit]->var;
    SHR_NULL_CHECK(var, SHR_E_PARAM);

    hit_prof = uft_be_dev_info[unit]->hit_prof;
    hit_prof_cnt = uft_be_dev_info[unit]->hit_prof_cnt;
    SHR_NULL_CHECK(hit_prof, SHR_E_PARAM);

    if (tile_id >= var->em_tile_cnt) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_value_to_symbol(unit,
                                      DEVICE_EM_TILE_INFOt,
                                      DEVICE_EM_TILE_INFOt_DEVICE_EM_TILE_IDf,
                                      tile_id, &tile_name));
    for (i = 0; i < hit_prof_cnt; i++) {
        if (sal_strcmp(tile_name, hit_prof[i].tile_name) == 0) {
            ptsid = hit_prof[i].prof_ptsid;
            fid = hit_prof[i].hc_fid;
            found = TRUE;
        }
    }
    if (!found) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (uft_lt_hw_read(unit, lt_id, op_arg,
                        ptsid, index, UFT_MAX_PT_ENTRY_WORDS, entry));
    bcmdrd_pt_field_set(unit, ptsid, entry, fid, &hit_context);
    SHR_IF_ERR_VERBOSE_EXIT
        (uft_lt_hw_write(unit, lt_id, op_arg, ptsid, index, entry));

exit:
    SHR_FUNC_EXIT();
}

static int
uft_fc_hit_context_get(int unit,
                       bcmltd_sid_t lt_id,
                       const bcmltd_op_arg_t *op_arg,
                       uint32_t tile_id,
                       uint32_t index,
                       uint32_t *hit_context)
{
    const bcmptm_uft_var_drv_t *var = NULL;
    uint32_t i;
    bool found = TRUE;
    bcmdrd_sid_t ptsid = INVALIDm;
    bcmdrd_fid_t fid = INVALIDf;
    uint32_t entry[UFT_MAX_PT_ENTRY_WORDS];
    const char *tile_name = NULL;
    const uft_tile_hit_profile_info_t *hit_prof;
    uint32_t hit_prof_cnt = 0;

    SHR_FUNC_ENTER(unit);

    if (uft_be_dev_info[unit] == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }
    var = uft_be_dev_info[unit]->var;
    SHR_NULL_CHECK(var, SHR_E_PARAM);

    hit_prof = uft_be_dev_info[unit]->hit_prof;
    hit_prof_cnt = uft_be_dev_info[unit]->hit_prof_cnt;
    SHR_NULL_CHECK(hit_prof, SHR_E_PARAM);

    if (tile_id >= var->em_tile_cnt) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_value_to_symbol(unit,
                                      DEVICE_EM_TILE_INFOt,
                                      DEVICE_EM_TILE_INFOt_DEVICE_EM_TILE_IDf,
                                      tile_id, &tile_name));
    for (i = 0; i < hit_prof_cnt; i++) {
        if (sal_strcmp(tile_name, hit_prof[i].tile_name) == 0) {
            ptsid = hit_prof[i].prof_ptsid;
            fid = hit_prof[i].hc_fid;
            found = TRUE;
        }
    }
    if (!found) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (uft_lt_hw_read(unit, lt_id, op_arg,
                        ptsid, index, UFT_MAX_PT_ENTRY_WORDS, entry));
    bcmdrd_pt_field_get(unit, ptsid, entry, fid, hit_context);

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */

int
bcmptm_uft_lt_fields_buff_alloc(int unit, bcmltd_fields_t *in)
{
    uint32_t idx = 0;
    bcmltd_field_t *f_ptr = NULL;
    size_t cnt;

    SHR_FUNC_ENTER(unit);

    if (!in || (in->count < 1)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    cnt = in->count;
    in->field = sal_alloc(cnt * sizeof(bcmltd_field_t *),
                          "bcmptmUftLtFieldPtArr");
    SHR_NULL_CHECK(in->field, SHR_E_MEMORY);
    sal_memset(in->field, 0, cnt * sizeof(bcmltd_field_t *));

    /* Allocate all fields struct at one time. */
    f_ptr = sal_alloc(cnt * sizeof(bcmltd_field_t), "bcmptmUftLtFieldArr");
    SHR_NULL_CHECK(f_ptr, SHR_E_MEMORY);
    sal_memset(f_ptr, 0, cnt * sizeof(bcmltd_field_t));

    for (idx = 0; idx < cnt; idx++) {
        in->field[idx] = f_ptr + idx;
    }

exit:
    if(SHR_FUNC_ERR()) {
        (void) bcmptm_uft_lt_fields_buff_free(unit, in);
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_uft_lt_fields_buff_free(int unit, bcmltd_fields_t *in)
{
    SHR_FUNC_ENTER(unit);

    if(!in) {
        SHR_EXIT();
    }
    if (in->field) {
        SHR_FREE(in->field[0]);
        SHR_FREE(in->field);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_cth_uft_lt_move_depth_get(int unit, bcmltd_sid_t ltid,
                                 uint8_t *move_depth)
{
    int rv;
    uint32_t fid;
    bcmltd_fields_t input = {0}, output = {0};
    size_t idx;
    uint32_t num_actual = 0;
    uint64_t def_val = 0;
    const bcmlrd_map_t *map = NULL;

    SHR_FUNC_ENTER(unit);

    if (uft_be_dev_info[unit] == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    rv = bcmlrd_map_get(unit, TABLE_EM_CONTROLt, &map);
    if (SHR_FAILURE(rv) || !map) {
        *move_depth = BCMPTM_RM_HASH_DEF_HASH_REORDERING_DEPTH;
        SHR_EXIT();
    }

    input.count  = 1;
    input.field  = NULL;
    output.count = UFT_FIELD_COUNT_MAX;
    output.field = NULL;
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_uft_lt_fields_buff_alloc(unit, &input));
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_uft_lt_fields_buff_alloc(unit, &output));
    input.field[0]->id = TABLE_EM_CONTROLt_TABLE_IDf;
    input.field[0]->data = ltid;
    rv = bcmimm_entry_lookup(unit, TABLE_EM_CONTROLt, &input, &output);
    if (rv == SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_field_default_get(unit, TABLE_EM_CONTROLt,
                                      TABLE_EM_CONTROLt_MOVE_DEPTHf,
                                      1, &def_val, &num_actual));
        *move_depth = def_val;
        SHR_EXIT();
    }

    for (idx = 0; idx < output.count; idx++) {
        fid = output.field[idx]->id;
        if (fid == TABLE_EM_CONTROLt_MOVE_DEPTHf) {
            *move_depth = output.field[idx]->data;
            break;
        }
    }

exit:
    (void) bcmptm_uft_lt_fields_buff_free(unit, &input);
    (void) bcmptm_uft_lt_fields_buff_free(unit, &output);
    SHR_FUNC_EXIT();
}

int
bcmptm_cth_uft_bank_info_get_from_rmalpm(int unit, bcmdrd_sid_t ptsid,
                                         bcmptm_pt_banks_info_t *banks_info)
{
   return uft_banks_info_get(unit, BANK_RM_ALPM, ptsid, banks_info);
}

int
bcmptm_cth_uft_bank_info_get_from_rmtcam(int unit, bcmdrd_sid_t ptsid,
                                         bcmptm_pt_banks_info_t *banks_info)
{
   return uft_banks_info_get(unit, BANK_RM_TCAM, ptsid, banks_info);
}

int
bcmptm_cth_uft_bank_info_get_from_ptcache(int unit, bcmdrd_sid_t ptsid,
                                          bcmptm_pt_banks_info_t *banks_info)
{
   return uft_banks_info_get(unit, BANK_RM_HASH, ptsid, banks_info);
}

int
bcmptm_uft_bank_hw_attr_get_from_lt(int unit, bcmltd_sid_t ltid,
                                    uint32_t lookup_num,
                                    bcmptm_lt_lookup_banks_info_t *banks_info)
{
    size_t size;
    uint32_t idx, i, cnt, mode;
    const bcmptm_uft_var_drv_t *var = NULL;
    const em_tile_mode_info_t *tile_mode = NULL;
    const em_lt_lookup_info_tile_mode_t *lt_lkp = NULL;
    bool *tile_mode_is_set = NULL;
    uint32_t working_mode;

    SHR_FUNC_ENTER(unit);

    if (uft_be_dev_info[unit] == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    sal_memset(banks_info, 0, sizeof(bcmptm_lt_lookup_banks_info_t));
    if (uft_be_dev_info[unit]->var == NULL) {
        SHR_EXIT();
    }

    var = uft_be_dev_info[unit]->var;
    cnt = 0;

    /* Calculates all the tile's current working mode. */
    size = var->tile_mode_cnt * sizeof(bool);
    tile_mode_is_set = sal_alloc(size, "bcmptmUftTileModeIsSet");
    SHR_NULL_CHECK(tile_mode_is_set, SHR_E_MEMORY);
    sal_memset(tile_mode_is_set, 0, size);
    for (idx = 0; idx < var->em_tile_cnt; idx++) {
        SHR_IF_ERR_EXIT
            (bcmptm_uft_tile_cfg_get(unit, idx, &working_mode, NULL, NULL));
        if (working_mode >= var->tile_mode_cnt) {
            /* Mode is disabled or invalid. */
            continue;
        }
        tile_mode_is_set[working_mode] = TRUE;
    }

    if (lookup_num == 0) {
        for (mode = 0; mode < var->tile_mode_cnt; mode++) {
            if (!tile_mode_is_set[mode]) {
                continue;
            }
            tile_mode = var->tile_mode[mode];
            for (idx = 0; idx < tile_mode->lt_lookup0_count; idx++) {
                lt_lkp = &(tile_mode->lt_lookup0[idx]);
                if (lt_lkp->ltid == ltid) {
                    for (i = 0; i < lt_lkp->bank_count; i++) {
                        if (cnt >= UFT_VAL_BANKS_MAX) {
                            SHR_ERR_EXIT(SHR_E_INTERNAL);
                        }
                        SHR_IF_ERR_EXIT
                            (bcmptm_uft_bank_info_get
                                (unit, lt_lkp->bank[i], FALSE,
                                 &(banks_info->bank_attr[cnt].bank_sid), NULL));
                        banks_info->bank_attr[cnt].hw_ltid = lt_lkp->hw_ltid[i];
                        cnt++;
                    }
                }
            }
        }
        banks_info->bank_cnt = cnt;
    } else if (lookup_num == 1) {
        for (mode = 0; mode < var->tile_mode_cnt; mode++) {
            if (!tile_mode_is_set[mode]) {
                continue;
            }
            tile_mode = var->tile_mode[mode];
            for (idx = 0; idx < tile_mode->lt_lookup1_count; idx++) {
                lt_lkp = &(tile_mode->lt_lookup1[idx]);
                if (lt_lkp->ltid == ltid) {
                    for (i = 0; i < lt_lkp->bank_count; i++) {
                        if (cnt >= UFT_VAL_BANKS_MAX) {
                            SHR_ERR_EXIT(SHR_E_INTERNAL);
                        }
                        SHR_IF_ERR_EXIT
                            (bcmptm_uft_bank_info_get
                                (unit, lt_lkp->bank[i], FALSE,
                                 &(banks_info->bank_attr[cnt].bank_sid), NULL));
                        banks_info->bank_attr[cnt].hw_ltid = lt_lkp->hw_ltid[i];
                        cnt++;
                    }
                }
            }
        }
        banks_info->bank_cnt = cnt;
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FREE(tile_mode_is_set);
    SHR_FUNC_EXIT();
}

int
bcmptm_cth_uft_bank_info_get_from_lt(int unit, bcmltd_sid_t ltid,
                                     int alpm_level,
                                     bcmptm_pt_banks_info_t *banks_info)
{
    int rv;
    uint32_t grp_id = 0;
    const bcmptm_uft_var_drv_t *var = NULL;

    SHR_FUNC_ENTER(unit);

    if (uft_be_dev_info[unit] == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    if (alpm_level > UFT_ALPM_LEVEL_MAX) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    var = uft_be_dev_info[unit]->var;
    if (var != NULL && uft_be_dev_info[unit]->var->legacy == FALSE) {
        return uft_bank_info_get_from_lt_tile_mode(unit, ltid,
                                                   alpm_level, banks_info);
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_cth_uft_grp_id_get(unit, ltid, alpm_level, &grp_id));
        rv = uft_grp_lt_parse(unit, grp_id, banks_info);
        if (rv == SHR_E_NOT_FOUND) {
            banks_info->bank_cnt = 0;
            SHR_EXIT();
        } else if (rv != SHR_E_NONE) {
            SHR_ERR_EXIT(rv);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_cth_uft_lt_lts_tcam_get(int unit, bcmltd_sid_t ltid,
                               bcmdrd_sid_t *ptsid_array,
                               uint32_t ptsid_array_size,
                               uint32_t *r_cnt)
{
    const bcmptm_uft_var_drv_t *var = NULL;
    const em_tile_mode_info_t *tm = NULL;
    int rv;
    uint32_t *grp_arr = NULL;
    uint32_t grp_cnt = 0;
    size_t size;
    uint32_t idx, idy, cnt;
    uint32_t grp_id, bank_cnt, mode;
    /* Restore the valid groups in real time. */
    bool *grp_valid = NULL;

    SHR_FUNC_ENTER(unit);

    if (uft_be_dev_info[unit] == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }
    var = uft_be_dev_info[unit]->var;
    SHR_NULL_CHECK(var, SHR_E_PARAM);
    if ((ptsid_array == NULL) && (ptsid_array_size > 0)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_NULL_CHECK(r_cnt, SHR_E_PARAM);


    size = sizeof(uint32_t) * TABLE_EM_GROUPS_MAX;
    grp_arr = sal_alloc(size, "bcmptmUftGrpIDArr");
    SHR_NULL_CHECK(grp_arr, SHR_E_MEMORY);
    sal_memset(grp_arr, 0, size);

    /* Calculate the valid groups in real time. */
    size = sizeof(bool) * var->em_group_cnt;;
    grp_valid = sal_alloc(size, "bcmptmUftGrpValidArr");
    SHR_NULL_CHECK(grp_valid, SHR_E_MEMORY);
    rv = uft_em_grp_check(unit, grp_valid, var->em_group_cnt);
    if (rv == SHR_E_NOT_FOUND) {
        *r_cnt = 0;
        SHR_EXIT();
    }

    /* Get group array for a LT's lookup0. */
    SHR_IF_ERR_VERBOSE_EXIT
        (uft_grp_get(unit, ltid, 0, 0, grp_valid,
                     grp_arr, TABLE_EM_GROUPS_MAX, &grp_cnt));

    cnt = 0;
    for (idx = 0; idx < grp_cnt; idx++) {
        grp_id = grp_arr[idx];
        bank_cnt = 0;
        SHR_IF_ERR_VERBOSE_EXIT
            (uft_grp_cfg_get(unit, grp_id, NULL, 0, &bank_cnt, NULL, NULL));
        if (bank_cnt == 0) {
            continue;
        }
        mode = var->em_group[grp_id]->tile_mode;
        tm = var->tile_mode[mode];
        for (idy = 0; idy < tm->em_ft_lts_tcam_count; idy++) {
            if (ltid == tm->em_ft_lts_tcam[idy].lt_id) {
                if (ptsid_array != NULL) {
                    if (cnt >= ptsid_array_size) {
                        SHR_ERR_EXIT(SHR_E_PARAM);
                    }
                    ptsid_array[cnt] = tm->em_ft_lts_tcam[idy].sid_lts_tcam;
                }
                cnt++;
            }
        }
    }

    *r_cnt = cnt;

exit:
    SHR_FREE(grp_valid);
    SHR_FREE(grp_arr);
    SHR_FUNC_EXIT();
}

int
bcmptm_uft_lt_lookup_cnt_get(int unit, bcmltd_sid_t ltid,
                             uint32_t *lookup_cnt)
{
    const bcmptm_uft_var_drv_t *var = NULL;
    uint32_t idx;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(uft_be_dev_info[unit], SHR_E_INIT);
    SHR_NULL_CHECK(lookup_cnt, SHR_E_PARAM);
    var = uft_be_dev_info[unit]->var;
    SHR_NULL_CHECK(var, SHR_E_INTERNAL);

    for (idx = 0; idx < var->lt_info_cnt; idx++) {
        if (ltid == var->lt_info[idx].ltid) {
            *lookup_cnt = var->lt_info[idx].lookup_cnt;
            SHR_EXIT();
        }
    }
    SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_cth_uft_grp_id_get(int unit, bcmltd_sid_t ltid, uint32_t alpm_level,
                          uint32_t *grp_id)
{
    bcmltd_fields_t input = {0}, output = {0};
    size_t i;
    uint32_t id;
    const em_grp_info_t *grp_info = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(uft_be_dev_info[unit], SHR_E_INIT);

    if (alpm_level > UFT_ALPM_LEVEL_MAX) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    input.count = 1;
    output.count = UFT_FIELD_COUNT_MAX;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_uft_lt_fields_buff_alloc(unit, &input));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_uft_lt_fields_buff_alloc(unit, &output));

    input.field[0]->id = TABLE_EM_INFOt_TABLE_IDf;
    input.field[0]->data = ltid;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_entry_lookup(unit, TABLE_EM_INFOt, &input, &output));
    for (i = 0; i < output.count; i++) {
        if (output.field[i]->id == TABLE_EM_INFOt_DEVICE_EM_GROUP_IDf) {
            id = output.field[i]->data;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_uft_grp_info_get(unit, id, &grp_info));
            /* ALPM LT can map up to 3 groups. */
            if (alpm_level) {
                if (alpm_level == grp_info->attri) {
                    *grp_id = id;
                    SHR_EXIT();
                }
            } else {
                *grp_id = id;
                SHR_EXIT();
            }
        }
    }

    SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);

exit:
    (void) bcmptm_uft_lt_fields_buff_free(unit, &input);
    (void) bcmptm_uft_lt_fields_buff_free(unit, &output);
    SHR_FUNC_EXIT();
}

int
bcmptm_uft_grp_fixed_bank_get(int unit, uint32_t grp_id,
                              uint32_t *bank_arr, uint32_t bank_size,
                              uint32_t *r_bank_cnt)
{
    bcmltd_fields_t input = {0}, output = {0};
    size_t idx;
    uint32_t fid, fidx;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    input.count  = 1;
    output.count = UFT_FIELD_COUNT_MAX;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_uft_lt_fields_buff_alloc(unit, &input));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_uft_lt_fields_buff_alloc(unit, &output));

    input.field[0]->id = DEVICE_EM_GROUP_INFOt_DEVICE_EM_GROUP_IDf;
    input.field[0]->data = grp_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_entry_lookup(unit, DEVICE_EM_GROUP_INFOt, &input, &output));
    for (idx = 0; idx < output.count; idx++) {
        fid = output.field[idx]->id;
        fidx = output.field[idx]->idx;
        fval = output.field[idx]->data;
        switch (fid) {
        case DEVICE_EM_GROUP_INFOt_NUM_FIXED_BANKSf:
            *r_bank_cnt = fval;
            break;
        case DEVICE_EM_GROUP_INFOt_FIXED_DEVICE_EM_BANK_IDf:
            if (fidx >= bank_size) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            bank_arr[fidx] = fval;
            break;
        default:
            break;
        }
    }

exit:
    (void) bcmptm_uft_lt_fields_buff_free(unit, &input);
    (void) bcmptm_uft_lt_fields_buff_free(unit, &output);
    SHR_FUNC_EXIT();
}

int
bcmptm_uft_grp_info_get(int unit,
                        uint32_t grp_id,
                        const em_grp_info_t **grp_info)
{
    const bcmptm_uft_var_drv_t *var = uft_be_dev_info[unit]->var;
    const em_grp_info_t **em_grp;
    uint32_t idx;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(var, SHR_E_PARAM);
    SHR_NULL_CHECK(grp_info, SHR_E_PARAM);

    em_grp = var->em_group;
    for (idx = 0; idx < var->em_group_cnt; idx++) {
        if (em_grp[idx]->grp_id == grp_id) {
            *grp_info = em_grp[idx];
            SHR_EXIT();
        }
    }
    SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get all group belonging to a specific tile mode.
 *
 * \param [in] unit Device unit.
 * \param [in] tile_mode Tile mode ID.
 * \param [out] grp_arr Returned group array.
 * \param [in] grp_size Size of group array.
 * \param [out] r_grp_cnt Actual returned count of groups.
 *
 * \return SHR_E_XXX.
 */
int
bcmptm_uft_grp_get_per_tile_mode(int unit,
                                 uint32_t tile_mode,
                                 uint32_t *grp_arr,
                                 uint32_t  grp_size,
                                 uint32_t *r_grp_cnt)
{
    const em_grp_info_t **em_grp;
    uint32_t idx = 0, cnt = 0;

    SHR_FUNC_ENTER(unit);

    if (uft_be_dev_info[unit] == NULL) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (uft_be_dev_info[unit]->var == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    em_grp = uft_be_dev_info[unit]->var->em_group;
    for (idx = 0; idx < uft_be_dev_info[unit]->var->em_group_cnt; idx++) {
        if (em_grp[idx]->tile_mode == tile_mode) {
            if (grp_arr) {
                if (cnt >= grp_size) {
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                grp_arr[cnt] = em_grp[idx]->grp_id;
             }
             cnt++;
        }
    }

    *r_grp_cnt = cnt;

exit:
    SHR_FUNC_EXIT();
}


int
bcmptm_uft_tile_mode_validate(int unit, uint32_t tile_id, uint32_t tile_mode)
{
    uint32_t idx;
    const bcmptm_uft_var_drv_t *var = uft_be_dev_info[unit]->var;

    SHR_FUNC_ENTER(unit);

    if (tile_id >= var->em_tile_cnt) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    for (idx = 0; idx < var->em_tile[tile_id].ava_mode_count; idx++) {
        if (tile_mode == var->em_tile[tile_id].ava_mode[idx]) {
            SHR_EXIT();
        }
    }
    SHR_ERR_EXIT(SHR_E_PARAM);

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_uft_tile_grp_get(int unit, uint32_t tile_id,
                        uint32_t *grp_arr,
                        uint32_t  grp_size,
                        uint32_t *r_grp_cnt)
{
    uint32_t tile_mode;
    const em_grp_info_t **em_grp;
    const bcmptm_uft_var_drv_t *var = uft_be_dev_info[unit]->var;
    uint32_t idx = 0, cnt = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(r_grp_cnt, SHR_E_PARAM);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_uft_tile_cfg_get(unit, tile_id, &tile_mode, NULL, NULL));

    em_grp = var->em_group;
    for (idx = 0; idx < var->em_group_cnt; idx++) {
        if (em_grp[idx]->tile_mode == tile_mode) {
            if (grp_arr != NULL) {
                if (cnt >= grp_size) {
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
                }
                grp_arr[cnt] = em_grp[idx]->grp_id;
            }
            cnt++;
        }
    }
    *r_grp_cnt = cnt;

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_uft_tile_cfg_get(int unit, uint32_t tile_id,
                        uint32_t *tile_mode, bool *robust, uint32_t *seed)
{
    uint32_t fid;
    uint64_t fval;
    bcmltd_fields_t input = {0}, output = {0};
    size_t idx;

    SHR_FUNC_ENTER(unit);

    input.count  = 1;
    input.field  = NULL;
    output.count = DEVICE_EM_TILEt_FIELD_COUNT;
    output.field = NULL;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_uft_lt_fields_buff_alloc(unit, &input));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_uft_lt_fields_buff_alloc(unit, &output));

    input.field[0]->id = DEVICE_EM_TILEt_DEVICE_EM_TILE_IDf;
    input.field[0]->data = tile_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_entry_lookup(unit, DEVICE_EM_TILEt, &input, &output));
    for (idx = 0; idx < output.count; idx++) {
        fid = output.field[idx]->id;
        fval = output.field[idx]->data;
        switch (fid) {
        case DEVICE_EM_TILEt_MODEf:
            if (tile_mode) {
                *tile_mode = fval;
            }
            break;
        case DEVICE_EM_TILEt_ROBUSTf:
            if (robust) {
                *robust = fval;
            }
            break;
        case DEVICE_EM_TILEt_SEEDf:
            if (seed) {
                *seed = fval;
            }
            break;
        default:
            break;
        }
    }

exit:
    (void) bcmptm_uft_lt_fields_buff_free(unit, &input);
    (void) bcmptm_uft_lt_fields_buff_free(unit, &output);
    SHR_FUNC_EXIT();
}

int
bcmptm_uft_tile_info_get(int unit,
                        uint32_t tile_id,
                        const em_tile_info_t **tile_info)
{
    const bcmptm_uft_var_drv_t *var = uft_be_dev_info[unit]->var;
    const em_tile_info_t *em_tile;
    uint32_t idx;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(var, SHR_E_PARAM);
    SHR_NULL_CHECK(tile_info, SHR_E_PARAM);

    em_tile = var->em_tile;
    for (idx = 0; idx < var->em_group_cnt; idx++) {
        if (em_tile[idx].tile_id == tile_id) {
            *tile_info = (&em_tile[idx]);
            SHR_EXIT();
        }
    }
    SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpmt_uft_bank_used(int unit, uint32_t bank_id, bool alpm, bool *used)
{
    bcmltd_fields_t input = {0}, output = {0};
    int rv = 0;
    uint32_t v_bank[UFT_VAL_BANKS_MAX] = {0};
    uint32_t v_bank_cnt = 0;
    uint32_t fid, fidx;
    uint64_t fval;
    uint32_t idx;
    uint32_t grp_id = 0;
    const em_grp_info_t *grp_info = NULL;

    SHR_FUNC_ENTER(unit);

    input.count = 1;
    output.count = UFT_FIELD_COUNT_MAX;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_uft_lt_fields_buff_alloc(unit, &input));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_uft_lt_fields_buff_alloc(unit, &output));
    rv = bcmimm_entry_get_first(unit, DEVICE_EM_GROUPt, &output);

    while (rv == SHR_E_NONE) {
        for (idx = 0; idx < output.count; idx++) {
            fid = output.field[idx]->id;
            fval = output.field[idx]->data;
            fidx = output.field[idx]->idx;
            switch (fid) {
            case DEVICE_EM_GROUPt_DEVICE_EM_BANK_IDf:
                v_bank[fidx] = fval;
                break;
            case DEVICE_EM_GROUPt_NUM_BANKSf:
                v_bank_cnt = fval;
                break;
            case DEVICE_EM_GROUPt_DEVICE_EM_GROUP_IDf:
                grp_id = fval;
            default:
                break;
            }
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_uft_grp_info_get(unit, grp_id, &grp_info));
        if (alpm == grp_info->is_alpm) {
            for (idx = 0; idx < v_bank_cnt; idx++) {
                if (v_bank[idx] == bank_id) {
                    *used = TRUE;
                    SHR_EXIT();
                }
            }
        }
        for (idx = 0; idx < grp_info->fixed_cnt; idx++) {
            if (grp_info->fixed_banks[idx] == bank_id) {
                *used = TRUE;
                SHR_EXIT();
            }
        }

        input.count = 1;
        input.field[0]->id = DEVICE_EM_GROUPt_DEVICE_EM_GROUP_IDf;
        input.field[0]->data = grp_id;
        output.count = UFT_FIELD_COUNT_MAX;
        rv = bcmimm_entry_get_next(unit, DEVICE_EM_GROUPt, &input, &output);
    }

    *used = FALSE;

exit:
    (void) bcmptm_uft_lt_fields_buff_free(unit, &input);
    (void) bcmptm_uft_lt_fields_buff_free(unit, &output);
    SHR_FUNC_EXIT();
}

int
bcmptm_uft_bank_info_get(int unit,
                         uint32_t bank_id,
                         bool alpm,
                         bcmdrd_sid_t *b_sid,
                         uint32_t *tile_id)
{
    const bcmptm_uft_var_drv_t *var = NULL;
    const em_bank_info_t *em_bank = NULL;
    const em_mode_bank_info_t *em_mode_bank = NULL;
    uint32_t tid = 0, tm = 0, idx = 0;

    SHR_FUNC_ENTER(unit);

    var = uft_be_dev_info[unit]->var;
    SHR_NULL_CHECK(var, SHR_E_PARAM);

    if (bank_id >= var->em_bank_cnt) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    tid = var->tile_id[bank_id];
    if (b_sid) {
        em_bank = &(var->em_bank[bank_id]);
        em_mode_bank = em_bank->mode_bank_info;
        if (em_mode_bank) {
            SHR_IF_ERR_EXIT
                (bcmptm_uft_tile_cfg_get(unit, tid, &tm, NULL, NULL));
            for (idx = 0; idx < em_mode_bank->tile_mode_count; idx++) {
                if (em_mode_bank->tile_mode[idx] == tm) {
                    *b_sid = em_mode_bank->w_ptid_array[idx];
                    break;
                }
            }
        } else {
            if (!alpm) {
                *b_sid = var->bank_ptsid[bank_id];
            } else {
                *b_sid = var->alpm_ptsid[bank_id];
            }
        }
    }

    if (tile_id) {
        *tile_id = tid;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_uft_bank_type_get(int unit,
                         uint32_t bank_id,
                         em_bank_type_t *type)
{
    const bcmptm_uft_var_drv_t *var = uft_be_dev_info[unit]->var;
    uint32_t g_idx, cnt;
    const em_grp_info_t *em_grp = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(var, SHR_E_PARAM);

    if (bank_id >= var->em_bank_cnt) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    for (g_idx = 0; g_idx < var->em_group_cnt; g_idx++) {
        em_grp = var->em_group[g_idx];
        for (cnt = 0; cnt < em_grp->cap_cnt; cnt++) {
            if (em_grp->cap_banks[cnt] == bank_id) {
                SHR_IF_ERR_EXIT
                    (uft_grp_attri_to_bank_type(em_grp->attri, type));
            }
        }

        for (cnt = 0; cnt < em_grp->fixed_cnt; cnt++) {
            if (em_grp->fixed_banks[cnt] == bank_id) {
                SHR_IF_ERR_EXIT
                    (uft_grp_attri_to_bank_type(em_grp->attri, type));

            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get paired bank for a primary bank in a group.
 *
 * \param [in] unit Device unit.
 * \param [in] grp_id Group ID.
 * \param [in] bank_id Bank ID.
 * \param [out] pair_bank Returned paired bank.
 *
 * \return SHR_E_NOT_FOUND.
 */
int
bcmptm_uft_pair_bank_get(int unit,
                         uint32_t grp_id,
                         uint32_t bank_id,
                         uint32_t *pair_bank)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(pair_bank, SHR_E_PARAM);
    /* For non-variant support device, there is no pair bank. */
    SHR_NULL_CHECK(uft_be_dev_info[unit]->var, SHR_E_NOT_FOUND);
    if (uft_be_dev_info[unit]->var->legacy == TRUE) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    return uft_paired_bank_get(unit, bank_id, pair_bank);

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_uft_pts_get(int unit, int arr_size, bcmdrd_sid_t *pt_arr)
{
    int i;

    SHR_FUNC_ENTER(unit);

    if ((pt_arr == NULL) || (arr_size < uft_be_dev_info[unit]->pt_cnt)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (uft_be_dev_info[unit]->ptsid != NULL) {
        for (i = 0; i < uft_be_dev_info[unit]->pt_cnt; i++) {
            pt_arr[i] = uft_be_dev_info[unit]->ptsid[i];
        }
    } else if (uft_be_dev_info[unit]->bank_ptsid != NULL) {
        for (i = 0; i < uft_be_dev_info[unit]->pt_cnt; i++) {
            pt_arr[i] = uft_be_dev_info[unit]->bank_ptsid[i];
        }
        for (i = 0; i < uft_be_dev_info[unit]->pt_cnt; i++) {
            if (pt_arr[i] == INVALIDm) {
                pt_arr[i] = uft_be_dev_info[unit]->alpm_ptsid[i];
            }
        }
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_uft_tile_mode_enabled(int unit, bool *tile_mode_en)
{
    const bcmptm_uft_var_drv_t *var = uft_be_dev_info[unit]->var;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(tile_mode_en, SHR_E_PARAM);

    if (uft_be_dev_info[unit] == NULL) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }
    if (var == NULL) {
        *tile_mode_en = FALSE;
        SHR_EXIT();
    } else if (var->legacy == TRUE) {
        *tile_mode_en = FALSE;
        SHR_EXIT();
    }

    *tile_mode_en = TRUE;

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_uft_bank_pair_to_global(int unit, uint32_t pair, uint32_t *global)
{
    const char *bank_name = NULL;

    SHR_FUNC_ENTER(unit);

    /*
     * Do the bank id transform from DEVICE_EM_BANK_PAIR_KEY_T to
     * DEVICE_EM_BANK_ID_KEY_T.
     */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_value_to_symbol(unit,
                                      DEVICE_EM_BANK_PAIRt,
                                      DEVICE_EM_BANK_PAIRt_DEVICE_EM_BANK_IDf,
                                      pair, &bank_name));
    SHR_IF_ERR_EXIT
        (bcmlrd_field_symbol_to_value(unit,
                                      DEVICE_EM_BANK_INFOt,
                                      DEVICE_EM_BANK_INFOt_DEVICE_EM_BANK_IDf,
                                      bank_name, global));
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_uft_bank_global_to_pair(int unit, uint32_t global, uint32_t *pair)
{
    const char *bank_name = NULL;

    SHR_FUNC_ENTER(unit);

    /*
     * Do the bank id transform from DEVICE_EM_BANK_PAIR_KEY_T to
     * DEVICE_EM_BANK_ID_KEY_T.
     */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_value_to_symbol(unit,
                                      DEVICE_EM_BANK_INFOt,
                                      DEVICE_EM_BANK_INFOt_DEVICE_EM_BANK_IDf,
                                      global, &bank_name));
    SHR_IF_ERR_EXIT
        (bcmlrd_field_symbol_to_value(unit,
                                      DEVICE_EM_BANK_PAIRt,
                                      DEVICE_EM_BANK_PAIRt_DEVICE_EM_BANK_IDf,
                                      bank_name, pair));
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_uft_bank_hash_to_global(int unit, uint32_t hash, uint32_t *global)
{
    const char *bank_name = NULL;

    SHR_FUNC_ENTER(unit);

    /*
     * Do the bank id transform from DEVICE_HASH_BANK_ID_KEY_T to
     * DEVICE_EM_BANK_ID_KEY_T under the assumption (also correct)
     * that the enum symbols (but not necessarily values) are
     * the same across the two types.
     */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_value_to_symbol(unit,
                                      DEVICE_EM_BANKt,
                                      DEVICE_EM_BANKt_DEVICE_EM_BANK_IDf,
                                      hash, &bank_name));
    SHR_IF_ERR_EXIT
        (bcmlrd_field_symbol_to_value(unit,
                                      DEVICE_EM_BANK_INFOt,
                                      DEVICE_EM_BANK_INFOt_DEVICE_EM_BANK_IDf,
                                      bank_name, global));
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_uft_bank_global_to_hash(int unit, uint32_t global, uint32_t *hash)
{
    const char *bank_name = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);

    /*
     * Do the bank id transform from DEVICE_EM_BANK_ID_KEY_T to
     * DEVICE_HASH_BANK_ID_KEY_T.
     */
    rv = bcmlrd_field_value_to_symbol(unit,
                                      DEVICE_EM_BANK_INFOt,
                                      DEVICE_EM_BANK_INFOt_DEVICE_EM_BANK_IDf,
                                      global, &bank_name);
    if (rv == SHR_E_UNAVAIL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }
    SHR_IF_ERR_EXIT(rv);

    rv = bcmlrd_field_symbol_to_value(unit,
                                      DEVICE_EM_BANKt,
                                      DEVICE_EM_BANKt_DEVICE_EM_BANK_IDf,
                                      bank_name, hash);
    if (rv == SHR_E_UNAVAIL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_uft_be_device_info_init(int unit,
                               const bcmptm_uft_var_drv_t *var_info,
                               uft_be_dev_info_t *dev)
{
    SHR_FUNC_ENTER(unit);

    if (UFT_BE_DRV(unit)->device_info_init == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (UFT_BE_DRV_EXEC(unit, device_info_init)(unit, var_info, dev));

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_cth_uft_be_init(int unit, bool warm)
{
    int rv;
    const bcmptm_uft_var_drv_t *var = NULL;

    SHR_FUNC_ENTER(unit);

    CTH_UFT_BE_ALLOC(UFT_BE_DEV(unit), sizeof(uft_be_dev_info_t),
        "bcmptmUftBeDevInfo");
    sal_memset(UFT_BE_DEV(unit), 0, sizeof(uft_be_dev_info_t));

    CTH_UFT_BE_ALLOC(UFT_BE_DRV(unit), sizeof(uft_be_driver_t),
        "bcmptmUftBeDrv");
    sal_memset(UFT_BE_DRV(unit), 0, sizeof(uft_be_driver_t));

    rv = bcmptm_uft_be_driver_register(unit, UFT_BE_DRV(unit));
    if (rv == SHR_E_UNAVAIL) {
        (void) bcmptm_cth_uft_be_cleanup(unit, warm);
        SHR_EXIT();
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_uft_var_info_get(unit, &var));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_uft_be_device_info_init(unit, var, UFT_BE_DEV(unit)));

exit:
    if (SHR_FUNC_ERR()) {
        (void) bcmptm_cth_uft_be_cleanup(unit, warm);
    }

    SHR_FUNC_EXIT();
}

int
bcmptm_cth_uft_be_cleanup(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    SHR_FREE(UFT_BE_DEV(unit));
    SHR_FREE(UFT_BE_DRV(unit));

    SHR_FUNC_EXIT();
}

int
bcmptm_uft_lt_lts_tcam_dump(int unit, bcmltd_sid_t ltid)
{
    bcmdrd_sid_t *ptsid_arr = NULL;
    uint32_t ptsid_arr_cnt = 0, r_cnt = 0, idx;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_cth_uft_lt_lts_tcam_get(unit, ltid,
                                        NULL, 0, &r_cnt));
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                           "\n*****************************************\n")));
    if (r_cnt == 0) {
        SHR_EXIT();
    }

    ptsid_arr_cnt = r_cnt;
    ptsid_arr = sal_alloc(ptsid_arr_cnt * sizeof(bcmdrd_sid_t),
                          "bcmptmUftPtsidArr");
    SHR_NULL_CHECK(ptsid_arr, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT
        (bcmptm_cth_uft_lt_lts_tcam_get(unit, ltid,
                                        ptsid_arr, ptsid_arr_cnt,
                                        &r_cnt));
    for (idx = 0; idx < r_cnt; idx++) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "\nLTS_TCAM sid %d: %s\n"),
                     idx, bcmdrd_pt_sid_to_name(unit, ptsid_arr[idx])));
    }

exit:
    SHR_FREE(ptsid_arr);
    SHR_FUNC_EXIT();
}

int
bcmptm_uft_alpm_pdd_type_get(int unit, uint32_t pdd_type_size,
                             int mtop,
                             uint32_t *pdd_type_array, uint32_t *r_cnt)
{
    uint32_t t, idx, cnt;
    const bcmptm_uft_var_drv_t *var = NULL;
    const em_tile_info_t *tile_info = NULL;
    const em_tile_mode_info_t *tile_mode = NULL;
    uint32_t work_tm;

    SHR_FUNC_ENTER(unit);

    if (uft_be_dev_info[unit] == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }
    SHR_NULL_CHECK(r_cnt, SHR_E_PARAM);

    *r_cnt = 0;
    if (uft_be_dev_info[unit]->var == NULL) {
        SHR_EXIT();
    }

    var = uft_be_dev_info[unit]->var;
    cnt = 0;
    for (t = 0; t < var->em_tile_cnt; t++) {
        tile_info = &(var->em_tile[t]);
        if (tile_info->mtop != (bool) mtop) {
            continue;
        }
        if (tile_info->alpm_pdd_type_available) {
            SHR_IF_ERR_EXIT
                (bcmptm_uft_tile_cfg_get(unit, t, &work_tm, NULL, NULL));
            if (work_tm >= var->tile_mode_cnt) {
                /* Mode is disabled or invalid. */
                for (idx = 0; idx < var->alpm_pdd_type_cnt_per_tile; idx++) {
                    if (pdd_type_array) {
                        if (cnt >= pdd_type_size) {
                            SHR_ERR_EXIT(SHR_E_PARAM);
                        }
                        pdd_type_array[cnt] = EM_TILE_ALPM_PDD_TYPE_NONE;
                    }
                    cnt++;
                }
                continue;
            }
            tile_mode = var->tile_mode[work_tm];
            for (idx = 0; idx < tile_mode->alpm_pdd_type_count; idx++) {
                if (pdd_type_array) {
                    if (cnt >= pdd_type_size) {
                        SHR_ERR_EXIT(SHR_E_PARAM);
                    }
                    pdd_type_array[cnt] = tile_mode->alpm_pdd_type[idx];
                }
                cnt++;
            }
        }
    }
    *r_cnt = cnt;

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_uft_alpm_key_mux_type_get(int unit, uint32_t key_mux_type_size,
                                 int mtop,
                                 uint32_t *key_mux_type_array,
                                 uint32_t *r_cnt)
{
    uint32_t t, idx, cnt;
    const bcmptm_uft_var_drv_t *var = NULL;
    const em_tile_info_t *tile_info = NULL;
    const em_tile_mode_info_t *tile_mode = NULL;
    uint32_t work_tm;

    SHR_FUNC_ENTER(unit);

    if (uft_be_dev_info[unit] == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }
    SHR_NULL_CHECK(r_cnt, SHR_E_PARAM);

    *r_cnt = 0;
    if (uft_be_dev_info[unit]->var == NULL) {
        SHR_EXIT();
    }

    var = uft_be_dev_info[unit]->var;
    cnt = 0;
    for (t = 0; t < var->em_tile_cnt; t++) {
        tile_info = &(var->em_tile[t]);
        if (tile_info->mtop != (bool) mtop) {
            continue;
        }
        if (tile_info->alpm_key_mux_type_available) {
            SHR_IF_ERR_EXIT
                (bcmptm_uft_tile_cfg_get(unit, t, &work_tm, NULL, NULL));
            if (work_tm >= var->tile_mode_cnt) {
                /* Mode is disabled or invalid. */
                for (idx = 0; idx < var->alpm_key_mux_type_cnt_per_tile; idx++) {
                    if (key_mux_type_array) {
                        if (cnt >= key_mux_type_size) {
                            SHR_ERR_EXIT(SHR_E_PARAM);
                        }
                        key_mux_type_array[cnt] = EM_TILE_ALPM_KEY_MUX_TYPE_NONE;
                    }
                    cnt++;
                }
                continue;
            }
            tile_mode = var->tile_mode[work_tm];
            for (idx = 0; idx < tile_mode->alpm_key_mux_type_count; idx++) {
                if (key_mux_type_array) {
                    if (cnt >= key_mux_type_size) {
                        SHR_ERR_EXIT(SHR_E_PARAM);
                    }
                    key_mux_type_array[cnt] = tile_mode->alpm_key_mux_type[idx];
                }
                cnt++;
            }
        }
    }
    *r_cnt = cnt;

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_uft_fc_hit_context_set_rm_hash(int unit,
                                      bcmltd_sid_t lt_id,
                                      const bcmltd_op_arg_t *op_arg,
                                      bcmdrd_sid_t w_ptid,
                                      uint32_t index,
                                      uint32_t hit_context)
{
    const bcmptm_uft_var_drv_t *var = NULL;
    uint32_t i, tile_id = 0;
    bool found = TRUE;

    SHR_FUNC_ENTER(unit);

    if (uft_be_dev_info[unit] == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }
    var = uft_be_dev_info[unit]->var;
    SHR_NULL_CHECK(var, SHR_E_PARAM);

    for (i = 0; i < var->em_bank_cnt; i++) {
        if (w_ptid == var->em_bank[i].w_ptid) {
            found = TRUE;
            tile_id = var->tile_id[i];
            break;
        }
    }

    if (found) {
        SHR_IF_ERR_VERBOSE_EXIT
            (uft_fc_hit_context_set(unit, lt_id, op_arg,
                                    tile_id, index, hit_context));
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_uft_fc_hit_context_get_rm_hash(int unit,
                                      bcmltd_sid_t lt_id,
                                      const bcmltd_op_arg_t *op_arg,
                                      bcmdrd_sid_t w_ptid,
                                      uint32_t index,
                                      uint32_t *hit_context)
{
    const bcmptm_uft_var_drv_t *var = NULL;
    uint32_t i, tile_id = 0;
    bool found = TRUE;

    SHR_FUNC_ENTER(unit);

    if (uft_be_dev_info[unit] == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }
    var = uft_be_dev_info[unit]->var;
    SHR_NULL_CHECK(var, SHR_E_PARAM);

    for (i = 0; i < var->em_bank_cnt; i++) {
        if (w_ptid == var->em_bank[i].w_ptid) {
            found = TRUE;
            tile_id = var->tile_id[i];
            break;
        }
    }

    if (found) {
        SHR_IF_ERR_VERBOSE_EXIT
            (uft_fc_hit_context_get(unit, lt_id, op_arg,
                                    tile_id, index, hit_context));
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_uft_fc_hit_context_set_rm_index(int unit,
                                       bcmltd_sid_t lt_id,
                                       const bcmltd_op_arg_t *op_arg,
                                       uint32_t tile_id,
                                       uint32_t hit_context)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (uft_fc_hit_context_set(unit, lt_id, op_arg,
                                tile_id, 0, hit_context));

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_uft_fc_hit_context_get_rm_index(int unit,
                                       bcmltd_sid_t lt_id,
                                       const bcmltd_op_arg_t *op_arg,
                                       uint32_t tile_id,
                                       uint32_t *hit_context)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (uft_fc_hit_context_get(unit, lt_id, op_arg,
                                tile_id, 0, hit_context));

exit:
    SHR_FUNC_EXIT();
}

