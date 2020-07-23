/*! \file bcmptm_rm_hash_lt_ctrl.c
 *
 * Resource Manager for Hash Table
 *
 * This file contains the resource manager for physical hash tables
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
#include <bcmdrd/bcmdrd_field.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmltd/chip/bcmltd_limits.h>
#include <bcmcfg/bcmcfg_lt.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmptm/bcmptm_cth_uft_be_internal.h>
#include "bcmptm_rm_hash.h"
#include "bcmptm_rm_hash_lt_ctrl.h"
#include "bcmptm_rm_hash_pt_register.h"
#include "bcmptm_rm_hash_utils.h"
#include "bcmptm_rm_hash_robust_hash.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMHASH
#define RM_HASH_PAIRED_BANK_CNT         2
#define RM_HASH_INIT_LTID_NUM          64
#define RM_HASH_INC_LTID_NUM           32

/*******************************************************************************
 * Typedefs
 */
/*
 *! \brief Information about lt ctrl of a device.
 *
 * This data structure describes the information about hash lt ctrl for a device.
 */
typedef struct rm_hash_dev_lt_ctrl_s {
    /*! \brief Count of lt ctrl for a device */
    uint16_t cnt;

    /*! \brief Current number of registered ltid */
    uint16_t curr;

    /*! \brief Array of lt ctrls */
    rm_hash_lt_ctrl_t array[1];
} rm_hash_dev_lt_ctrl_t;


/*******************************************************************************
 * Private variables
 */

static rm_hash_dev_lt_ctrl_t *dev_lt_ctrl[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private Functions
 */
/*!
 * \brief This function allocates required lt_ctrl space for a device.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_lt_ctrl_alloc(int unit)
{
    uint16_t num_ltids = RM_HASH_INIT_LTID_NUM;
    uint32_t req_size = 0;

    SHR_FUNC_ENTER(unit);

    req_size = (num_ltids * sizeof(rm_hash_lt_ctrl_t)) +
                sizeof(rm_hash_dev_lt_ctrl_t);

    dev_lt_ctrl[unit] = sal_alloc(req_size, "bcmptmRmhashLtCtrlInfo");

    if (dev_lt_ctrl[unit] == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    sal_memset(dev_lt_ctrl[unit], 0, req_size);
    dev_lt_ctrl[unit]->cnt = num_ltids;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief This function free allocated lt_ctrl space for a device.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_lt_ctrl_free(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (dev_lt_ctrl[unit] != NULL) {
        sal_free(dev_lt_ctrl[unit]);
        dev_lt_ctrl[unit] = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Configure lt_ctrl for a specific hash table ltid
 *
 * \param [in] unit Unit number.
 * \param [in] lt_ctrl Pointer to lt control object of the ltid.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_lt_ctrl_set(int unit, rm_hash_lt_ctrl_t *lt_ctrl)
{
    uint16_t curr = 0, idx = 0, k = 0, num_move = 0;
    uint16_t num_ltids = 0;
    uint32_t req_size = 0, original_size = 0;
    rm_hash_dev_lt_ctrl_t *tmp = NULL;

    SHR_FUNC_ENTER(unit);

    curr = dev_lt_ctrl[unit]->curr;

    if (curr >= dev_lt_ctrl[unit]->cnt) {
        num_ltids = dev_lt_ctrl[unit]->cnt + RM_HASH_INC_LTID_NUM;
        req_size = (num_ltids * sizeof(rm_hash_lt_ctrl_t)) +
                   sizeof(rm_hash_dev_lt_ctrl_t);
        tmp = sal_alloc(req_size, "bcmptmRmhashLtCtrlInfo");
        if (tmp == NULL) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
        original_size = (dev_lt_ctrl[unit]->cnt * sizeof(rm_hash_lt_ctrl_t)) +
                        sizeof(rm_hash_dev_lt_ctrl_t);
        sal_memcpy(tmp, dev_lt_ctrl[unit], original_size);
        sal_free(dev_lt_ctrl[unit]);
        dev_lt_ctrl[unit] = tmp;
        dev_lt_ctrl[unit]->cnt = num_ltids;
    }

    for (idx = 0; idx < curr; idx++) {
        if (lt_ctrl->ltid < dev_lt_ctrl[unit]->array[idx].ltid) {
            break;
        }
    }
    num_move = curr - idx;
    for (k = curr - 1; num_move > 0; k--, num_move--) {
        dev_lt_ctrl[unit]->array[k + 1] = dev_lt_ctrl[unit]->array[k];
    }
    dev_lt_ctrl[unit]->array[idx] = *lt_ctrl;
    dev_lt_ctrl[unit]->curr++;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Determine if two field lists have identical position.
 *
 * \param [in] field_list_a Field list a.
 * \param [in] field_list_b Field list b.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static bool
rm_hash_field_list_identical(const bcmlrd_hw_field_list_t *field_list_a,
                             const bcmlrd_hw_field_list_t *field_list_b)
{
    uint8_t idx;

    if (field_list_a->num_fields != field_list_b->num_fields) {
        return FALSE;
    }

    for (idx = 0; idx < field_list_a->num_fields; idx++) {
        if (field_list_a->field_start_bit[idx] !=
            field_list_b->field_start_bit[idx]) {
            return FALSE;
        }
        if (field_list_a->field_width[idx] !=
            field_list_b->field_width[idx]) {
            return FALSE;
        }
    }
    return TRUE;
}

/*!
 * \brief Determine if field exchange operation is required.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_info LRD information for a LT.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static bool
rm_hash_key_field_exchange_required(int unit,
                                    bcmptm_rm_hash_lt_info_t *lt_info)
{
    uint8_t grp_idx, grp_cnt = 0;
    const bcmptm_rm_hash_grp_key_info_t *base_grp_key_info = NULL;
    const bcmptm_rm_hash_grp_key_info_t *grp_key_info = NULL;
    const bcmptm_rm_hash_key_format_t *fmt = NULL, *base_key_fmt = NULL;
    const bcmlrd_hw_field_list_t *key_fields = NULL, *base_key_fields = NULL;
    base_grp_key_info = lt_info->enh_more_info->lt_key_info->grp_key_info;
    base_key_fmt = base_grp_key_info->key_format;
    base_key_fields = base_key_fmt->hw_key_fields;

    /* Get the total number of mapping groups for this LT. */
    bcmptm_rm_hash_lt_map_grp_cnt_get(unit, lt_info, &grp_cnt);
    if (grp_cnt <= 1) {
        return FALSE;
    }
    if (base_key_fields == NULL) {
        /*
         * For dynamic hash LTs, it is assumed that key position will keep same
         * in all its banks.
         */
        return FALSE;
    }
    for (grp_idx = 0; grp_idx < grp_cnt; grp_idx++) {
        grp_key_info = base_grp_key_info + grp_idx;
        /*
         * For newer devices, it is assumed that a LT only has one
         * key format in a group.
         */
        fmt = grp_key_info->key_format;
        key_fields = fmt->hw_key_fields;
        if (rm_hash_field_list_identical(base_key_fields, key_fields) == FALSE) {
            return TRUE;
        }
    }
    return FALSE;
}

/*!
 * \brief Get SID list for an LT mapping instance.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_ctrl Pointer to rm_hash_lt_ctrl_t structure.
 * \param [in] map_info Pointer to rm_hash_map_info_t structure.
 * \param [in] map_sid Buffer to store the SID list for the given mapping.
 *
 * \retval NONE.
 */
static void
rm_hash_lt_existing_map_sid_get(int unit,
                                rm_hash_lt_ctrl_t *lt_ctrl,
                                rm_hash_map_info_t *map_info,
                                bcmdrd_sid_t *map_sid)
{
    uint8_t inst, fmt_idx;
    rm_hash_fmt_info_t *fmt_info;

    for (inst = 0; inst < map_info->num_insts; inst++) {
        fmt_idx = map_info->fmt_idx[inst];
        fmt_info = &lt_ctrl->fmt_info[fmt_idx];
        map_sid[inst] = fmt_info->sid;
    }
}

/*!
 * \brief Clear hit context information for the specified hash LT.
 *
 * When banks are removed from a hash LT, there will be no entry for this LT.
 * In such case, hit context information for this LT should be cleared.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_ctrl Pointer to the rm_hash_lt_ctrl_t structure.
 *
 * \retval NONE
 */
static int
rm_hash_lt_ctrl_hit_context_clear(int unit,
                                  rm_hash_lt_ctrl_t *lt_ctrl)
{
    uint8_t map_idx, inst, profile_idx;
    rm_hash_map_info_t *map_info = NULL;
    bcmltd_op_arg_t op_arg = {0};
    bcmdrd_sid_t grp_sid, map_sid[RM_HASH_LOOKUP_MAX_CNT];

    SHR_FUNC_ENTER(unit);

    if (lt_ctrl->flex_ctr_info.num_context > 0) {
        /*
         * Clear hardware HIT index profile tables, irrespective of whether
         * flexctr has interest on this LT.
         */
        for (map_idx = 0; map_idx < lt_ctrl->num_maps; map_idx++) {
            map_info = &lt_ctrl->map_info[map_idx];
            rm_hash_lt_existing_map_sid_get(unit, lt_ctrl, map_info, map_sid);

            for (inst = 0; inst < map_info->num_insts; inst++) {
                /* Get the widest table view. */
                SHR_IF_ERR_EXIT
                    (bcmptm_rm_hash_grp_sid_get(unit, map_sid[inst], &grp_sid));
                /*
                 * Clear all the entries of hit index profile table at present stage,
                 * will clear entries of the specified LT in the future.
                 */
                for (profile_idx = 0; profile_idx < RM_HASH_HIT_PROFILE_ENTRY_CNT;
                     profile_idx++) {
                    SHR_IF_ERR_EXIT
                        (bcmptm_uft_fc_hit_context_set_rm_hash(unit,
                                                               lt_ctrl->ltid,
                                                               &op_arg,
                                                               grp_sid,
                                                               profile_idx,
                                                               0));
                }
            }
        }
        /*
         * Clear software state. For those banks that are still enabled
         * the context will be set again by rm_hash_lt_ctrl_hit_context_set.
         */
        for (map_idx = 0; map_idx < lt_ctrl->num_maps; map_idx++) {
            map_info = &lt_ctrl->map_info[map_idx];
            sal_memset(map_info->hit_context, 0, sizeof(map_info->hit_context));
        }
        sal_memset(lt_ctrl->flex_ctr_info.tile_id_list, 0,
                   sizeof(lt_ctrl->flex_ctr_info.tile_id_list));
        lt_ctrl->flex_ctr_info.num_context = 0;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set hit context information for the specified hash LT.
 *
 * When additional banks are assigned to an LT, the hit context information
 * for this LT on the new enabled banks should be configured.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_ctrl Pointer to the rm_hash_lt_ctrl_t structure.
 * \param [in] map_idx Mapping index of the LT.
 * \param [in] map_sid Physical table symbol id list for this mapping.
 * \param [in] num_insts Number of instances for this mapping.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_lt_ctrl_hit_context_set(int unit,
                                rm_hash_lt_ctrl_t *lt_ctrl,
                                uint8_t map_idx,
                                bcmdrd_sid_t *map_sid,
                                uint8_t num_insts)
{
    uint8_t inst, idx, tile_id, table_id, profile_idx, offset;
    const bcmptm_rm_hash_hit_tbl_info_t *hit_tbl_info = NULL;
    bcmdrd_sid_t grp_sid;
    rm_hash_map_info_t *map_info = &lt_ctrl->map_info[map_idx];
    bcmltd_op_arg_t op_arg = {0};

    SHR_FUNC_ENTER(unit);

    for (inst = 0; inst < num_insts; inst++) {
        /* Get hit table information based on sid. */
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_hit_tbl_info_get(unit,
                                             map_sid[inst],
                                             &hit_tbl_info));
        if (hit_tbl_info == NULL) {
            /* There is no hit context for this sid. */
            continue;
        }
        tile_id = hit_tbl_info->tile_id;
        table_id = hit_tbl_info->table_id;
        offset = hit_tbl_info->offset;
        for (idx = 0; idx < lt_ctrl->flex_ctr_info.num_context; idx++) {
            if (lt_ctrl->flex_ctr_info.tile_id_list[idx] == tile_id) {
                /* The tile has been enabled for this LT already. */
                break;
            }
        }
        if (idx == lt_ctrl->flex_ctr_info.num_context) {
            /* This is a new enabled tile for this LT. */
            lt_ctrl->flex_ctr_info.tile_id_list[idx] = tile_id;
            lt_ctrl->flex_ctr_info.num_context++;

            if (lt_ctrl->flex_ctr_info.lookup_entry_move_reporting[inst] == TRUE) {
                /*
                 * The proceeding tiles must have been configured already,
                 * just configure this tile.
                 */
                SHR_IF_ERR_EXIT
                    (bcmptm_rm_hash_grp_sid_get(unit, map_sid[inst], &grp_sid));

                for (profile_idx = 0; profile_idx < RM_HASH_HIT_PROFILE_ENTRY_CNT;
                     profile_idx++) {
                    SHR_IF_ERR_EXIT
                        (bcmptm_uft_fc_hit_context_set_rm_hash(unit,
                                                               lt_ctrl->ltid,
                                                               &op_arg,
                                                               grp_sid,
                                                               profile_idx,
                                                               idx << offset));
                }
            }
        }
        map_info->hit_context[inst].tile_id = idx;
        map_info->hit_context[inst].table_id = table_id;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get SID list for a new LT mapping instance.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] banks_info Pointer to bcmptm_pt_banks_info_t structure.
 * \param [in] index Index into enabled bank information provided by UFT mgr.
 * \param [out] map_sid Buffer to store the enabled SID.
 * \param [out] map_rbank Buffer to store the bank number of the enabled SID.
 * \param [out] num_insts Buffer to store the number of SID for this mapping.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_lt_new_map_sid_get(int unit,
                           rm_hash_pt_info_t *pt_info,
                           bcmptm_pt_banks_info_t *banks_info,
                           uint8_t index,
                           bcmdrd_sid_t *map_sid,
                           uint8_t *map_rbank,
                           uint8_t *num_insts)
{
    bcmdrd_sid_t lookup0_sid, lookup1_sid;
    uint8_t rbank_cnt, rbank_list[RM_HASH_SHR_MAX_BANK_COUNT] = {0};

    SHR_FUNC_ENTER(unit);

    lookup0_sid = banks_info->bank[index].bank_sid;
    if (lookup0_sid == INVALIDm) {
        /* SID for the lookup 0 must be valid. */
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    bcmptm_rm_hash_sid_rbank_list_get(unit,
                                      lookup0_sid,
                                      pt_info,
                                      rbank_list,
                                      &rbank_cnt);
    if (rbank_cnt == 0) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
    map_sid[0] = lookup0_sid;
    map_rbank[0] = rbank_list[0];
    *num_insts = 1;

    lookup1_sid = banks_info->paired_bank[index].bank_sid;
    if (lookup1_sid == INVALIDm) {
        /* This map has only one instance. */
        SHR_EXIT();
    } else {
        bcmptm_rm_hash_sid_rbank_list_get(unit,
                                          lookup1_sid,
                                          pt_info,
                                          rbank_list,
                                          &rbank_cnt);
        if (rbank_cnt == 0) {
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }
        map_sid[1] = lookup1_sid;
        map_rbank[1] = rbank_list[0];
        *num_insts = 2;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check whether the new enabled SID already exists.
 *
 * \param [in] map_info Pointer to rm_hash_map_info_t structure.
 * \param [in] ex_map_sid The list of existing SID list
 * \param [in] new_map_sid The list of new assigned SID list.
 * \param [in] new_num_insts Number of instances.
 *
 * \retval TRUE or FALSE.
 */
static bool
rm_hash_map_sid_is_equal(rm_hash_map_info_t *map_info,
                         bcmdrd_sid_t *ex_map_sid,
                         bcmdrd_sid_t *new_map_sid,
                         uint8_t new_num_insts)
{
    uint8_t idx;

    if (map_info->num_insts != new_num_insts) {
        return FALSE;
    }

    for (idx = 0; idx < new_num_insts; idx++) {
        if (ex_map_sid[idx] != new_map_sid[idx]) {
            return FALSE;
        }
    }
    return TRUE;
}

/*!
 * \brief Clear runtime bank enabling status for a hash LT.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_ctrl Pointer to rm_hash_lt_ctrl_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static void
rm_hash_lt_ctrl_map_bank_enable_clear(int unit, rm_hash_lt_ctrl_t *lt_ctrl)
{
    uint8_t fmt_idx;
    rm_hash_fmt_info_t *fmt_info;

    for (fmt_idx = 0; fmt_idx < lt_ctrl->num_fmts; fmt_idx++) {
        fmt_info = &lt_ctrl->fmt_info[fmt_idx];
        fmt_info->num_en_rbanks = 0;
    }
    /* Invalid all the mappings for the LT. */
    lt_ctrl->num_maps = 0;
}

/*!
 * \brief Update bank enabling status for a hash LT.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_ctrl Pointer to rm_hash_lt_ctrl_t structure.
 * \param [in] map_info Pointer to rm_hash_map_info_t structure.
 * \param [in] map_sid List of map sid.
 * \param [in] map_rbank_list Bank list of the map.
 * \param [in] num_insts Number of instance for the map.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_lt_ctrl_multi_map_bank_update(int unit,
                                      rm_hash_lt_ctrl_t *lt_ctrl,
                                      rm_hash_map_info_t *map_info,
                                      bcmdrd_sid_t *map_sid,
                                      uint8_t *map_rbank_list,
                                      uint8_t num_insts)
{
    uint8_t fmt_idx, tmp_grp_idx;
    rm_hash_fmt_info_t *fmt_info;

    SHR_FUNC_ENTER(unit);

    if (num_insts > map_info->num_insts) {
        /* New added pair bank. */
        for (fmt_idx = 0; fmt_idx < lt_ctrl->num_fmts; fmt_idx++) {
            fmt_info = &lt_ctrl->fmt_info[fmt_idx];
            if (fmt_info->sid == map_sid[1]) {
                /* Update bank enable status. */
                if (fmt_info->rbank_list[0] != map_rbank_list[1]) {
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
                }
                fmt_info->num_en_rbanks = 1;
                fmt_info->en_rbank_list[0] = map_rbank_list[1];
                map_info->fmt_idx[1] = fmt_idx;
                tmp_grp_idx = lt_ctrl->fmt_grp_idx[fmt_idx];
                map_info->grp_idx[1] = tmp_grp_idx;
                map_info->key_format[1] = fmt_info->key_format;
                map_info->num_insts = num_insts;
            }
        }
    }
    if (num_insts < map_info->num_insts) {
        /* Removed pair bank. */
        fmt_idx = map_info->fmt_idx[1];
        fmt_info = &lt_ctrl->fmt_info[fmt_idx];
        fmt_info->num_en_rbanks = 0;
        fmt_info->en_rbank_list[0] = 0;
        map_info->fmt_idx[1] = 0;
        map_info->grp_idx[1] = 0;
        map_info->key_format[1] = NULL;
        map_info->num_insts = 1;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update bank enabling status for a hash LT.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_ctrl Pointer to rm_hash_lt_ctrl_t structure.
 * \param [in] map_sid List of map sid.
 * \param [in] map_rbank_list Bank list of the map.
 * \param [in] num_insts Number of instance for the map.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_lt_ctrl_multi_map_bank_enable(int unit,
                                      rm_hash_lt_ctrl_t *lt_ctrl,
                                      bcmdrd_sid_t *map_sid,
                                      uint8_t *map_rbank_list,
                                      uint8_t num_insts)
{
    uint8_t fmt_idx, inst, grp_idx, tmp_grp_idx, map_idx, num_maps = 0;
    rm_hash_fmt_info_t *fmt_info;
    uint8_t srch_map;
    bcmdrd_sid_t sid;
    uint32_t grp_size, tmp_grp_size;
    int idx, pos = 0;
    rm_hash_map_info_t *map_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Determine if the SID has been enabled. */
    for (map_idx = 0; map_idx < lt_ctrl->num_maps; map_idx++) {
        map_info = &lt_ctrl->map_info[map_idx];
        if (num_insts != map_info->num_insts) {
            continue;
        }
        for (inst = 0; inst < map_info->num_insts; inst++) {
            fmt_idx = map_info->fmt_idx[inst];
            fmt_info = &lt_ctrl->fmt_info[fmt_idx];
            if (map_sid[inst] != fmt_info->sid) {
                break;
            }
        }
        if (inst == map_info->num_insts) {
            /* SID for each instance is identical. */
            break;
        }
    }
    if (map_idx < lt_ctrl->num_maps) {
        /* Already configured. */
        SHR_EXIT();
    }

    num_maps = lt_ctrl->num_maps;

    for (inst = 0; inst < num_insts; inst++) {
        sid = map_sid[inst];
        for (fmt_idx = 0; fmt_idx < lt_ctrl->num_fmts; fmt_idx++) {
            fmt_info = &lt_ctrl->fmt_info[fmt_idx];
            if (fmt_info->sid == sid) {
                /* Update bank enable status. */
                if (fmt_info->rbank_list[0] != map_rbank_list[inst]) {
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
                }
                fmt_info->num_en_rbanks = 1;
                fmt_info->en_rbank_list[0]= map_rbank_list[inst];
                lt_ctrl->map_info[num_maps].fmt_idx[inst] = fmt_idx;
                tmp_grp_idx = lt_ctrl->fmt_grp_idx[fmt_idx];
                lt_ctrl->map_info[num_maps].grp_idx[inst] = tmp_grp_idx;
                lt_ctrl->map_info[num_maps].key_format[inst] = fmt_info->key_format;
            }
        }
    }
    lt_ctrl->map_info[num_maps].num_insts = num_insts;

    LOG_VERBOSE(BSL_LOG_MODULE,
               (BSL_META_U(unit,
                           "ltid: %d, map idx: %d, num_inst: %d \n"),
                           lt_ctrl->ltid, num_maps, num_insts));

    /*
     * Each instance in a mapping must have the same table size, even they
     * may belong to different groups.
     */
    grp_idx = lt_ctrl->map_info[num_maps].grp_idx[0];
    grp_size = lt_ctrl->grp_size[grp_idx];

    for (idx = 0; idx < lt_ctrl->num_maps; idx++) {
        srch_map = lt_ctrl->srch_map_idx[idx];
        tmp_grp_idx = lt_ctrl->map_info[srch_map].grp_idx[0];
        tmp_grp_size = lt_ctrl->grp_size[tmp_grp_idx];
        if (grp_size > tmp_grp_size) {
            pos = idx;
            break;
        }
    }
    if (idx < lt_ctrl->num_maps) {
        /*
         * The size of the new mapping is greater than some existing
         * mapping. All the elements from pos should be shifted.
         */
        for (idx = lt_ctrl->num_maps - 1; idx >= pos; idx--) {
            lt_ctrl->srch_map_idx[idx + 1] = lt_ctrl->srch_map_idx[idx];
        }
        lt_ctrl->srch_map_idx[pos] = num_maps;
    } else {
        /*
         * The size of the new mapping is less than any existing
         * mapping. None of the elements from pos should be shifted.
         */
        pos = lt_ctrl->num_maps;
        lt_ctrl->srch_map_idx[pos] = num_maps;
    }
    lt_ctrl->num_maps++;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update bank enabling status for a hash LT.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_ctrl Pointer to rm_hash_lt_ctrl_t structure.
 * \param [in] map_sid List of map sid.
 * \param [in] num_rbank Number of banks.
 * \param [in] map_rbank_list Bank list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_lt_ctrl_single_map_bank_enable(int unit,
                                       rm_hash_lt_ctrl_t *lt_ctrl,
                                       bcmdrd_sid_t map_sid,
                                       uint8_t num_rbank,
                                       uint8_t *map_rbank_list)
{
    uint8_t fmt_idx, idx;
    rm_hash_fmt_info_t *fmt_info;

    SHR_FUNC_ENTER(unit);

    for (fmt_idx = 0; fmt_idx < lt_ctrl->num_fmts; fmt_idx++) {
        fmt_info = &lt_ctrl->fmt_info[fmt_idx];
        /* Multiple distinct formats may have same SID. */
        if (fmt_info->sid == map_sid) {
            fmt_info->num_en_rbanks = num_rbank;
            for (idx = 0; idx < num_rbank; idx++) {
                fmt_info->en_rbank_list[idx]= map_rbank_list[idx];
            }
        }
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Configure runtime single map lt to pt mapping for some devices.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_ctrl Pointer to rm_hash_lt_ctrl_t structure.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_single_map_lt_config(int unit,
                             rm_hash_lt_ctrl_t *lt_ctrl,
                             rm_hash_pt_info_t *pt_info)
{
    uint8_t fmt_idx, rbank_cnt, inst = 0;
    rm_hash_fmt_info_t *fmt_info = NULL;
    bcmdrd_sid_t sid;
    uint8_t rbank, rbank_list[RM_HASH_SHR_MAX_BANK_COUNT] = {0};
    bcmptm_pt_banks_info_t banks_info;
    uint32_t mask = 0;
    bcmptm_rm_hash_def_bank_info_t def_bank_info;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    lt_ctrl->num_maps = 1;

    for (fmt_idx = 0; fmt_idx < lt_ctrl->num_fmts; fmt_idx++) {
        fmt_info = &lt_ctrl->fmt_info[fmt_idx];
        sid = fmt_info->sid;
        bcmptm_rm_hash_sid_rbank_list_get(unit, sid, pt_info, rbank_list,
                                          &rbank_cnt);
        sal_memset(&banks_info, 0, sizeof(banks_info));
        rv = bcmptm_cth_uft_bank_info_get_from_ptcache(unit, sid,
                                                       &banks_info);
        if (rv == SHR_E_NONE) {
            if (banks_info.bank_cnt == 0) {
                /* The physical table represented by SID has been disabled. */
                continue;
            }
            lt_ctrl->map_info[0].fmt_idx[inst] = fmt_idx;
            lt_ctrl->map_info[0].grp_idx[inst] = lt_ctrl->fmt_grp_idx[fmt_idx];
            lt_ctrl->map_info[0].key_format[inst] = fmt_info->key_format;
            inst++;
            SHR_IF_ERR_EXIT
                (rm_hash_lt_ctrl_single_map_bank_enable(unit,
                                                        lt_ctrl,
                                                        sid,
                                                        banks_info.bank_cnt,
                                                        rbank_list));
        } else if (rv == SHR_E_UNAVAIL) {
            /*
             * For some devices, if UFT mgr cannot provide the information,
             * Try to use the per chip default values.
             */
            rbank = rbank_list[0];
            mask = pt_info->vec_info[rbank].mask;
            sal_memset(&def_bank_info, 0, sizeof(def_bank_info));
            SHR_IF_ERR_EXIT
                (bcmptm_hash_def_bank_info_get(unit, mask,
                                               &def_bank_info));
            if (def_bank_info.bank_cnt == 0) {
                /* The physical table represented by SID has been disabled. */
                continue;
            }
            lt_ctrl->map_info[0].fmt_idx[inst] = fmt_idx;
            lt_ctrl->map_info[0].grp_idx[inst] = lt_ctrl->fmt_grp_idx[fmt_idx];
            lt_ctrl->map_info[0].key_format[inst] = fmt_info->key_format;
            inst++;
            SHR_IF_ERR_EXIT
                (rm_hash_lt_ctrl_single_map_bank_enable(unit,
                                                        lt_ctrl,
                                                        sid,
                                                        def_bank_info.bank_cnt,
                                                        rbank_list));
        } else {
            SHR_ERR_EXIT(rv);
        }
    }
    lt_ctrl->map_info[0].num_insts = inst;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Configure runtime multiple lt to pt mappings for some devices.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_ctrl Pointer to rm_hash_lt_ctrl_t structure.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_multi_map_lt_config(int unit,
                            rm_hash_lt_ctrl_t *lt_ctrl,
                            rm_hash_pt_info_t *pt_info)
{
    bcmptm_pt_banks_info_t banks_info;
    uint8_t en_map_cnt = 0, map_idx, num_insts = 0, bank_idx;
    bcmdrd_sid_t new_map_sid[RM_HASH_LOOKUP_MAX_CNT];
    bcmdrd_sid_t existing_map_sid[RM_HASH_LOOKUP_MAX_CNT];
    uint8_t map_rbank[RM_HASH_SHR_MAX_BANK_COUNT] = {0};

    SHR_FUNC_ENTER(unit);

    sal_memset(&banks_info, 0, sizeof(banks_info));
    SHR_IF_ERR_EXIT
        (bcmptm_cth_uft_bank_info_get_from_lt(unit, lt_ctrl->ltid, 0,
                                              &banks_info));
    /*
     * Get the number of enable maps for the LT. An LT may have multiple
     * enabled maps. For LTs that support multiple lookups, a map may
     * have multiple instances.
     */
    en_map_cnt = banks_info.bank_cnt;
    /*
     * Determine whether additional banks are added or existing banks are removed.
     * New banks can be added when the number of entries is non-zero. Existing banks
     * can be removed only there is no entries.
     */
    if (lt_ctrl->num_maps <= en_map_cnt) {
        /*
         * There are new additional banks enabled, add the new bank
         * into the mapping structure.
         */
        for (bank_idx = 0; bank_idx < en_map_cnt; bank_idx++) {
            SHR_IF_ERR_EXIT
                (rm_hash_lt_new_map_sid_get(unit,
                                            pt_info,
                                            &banks_info,
                                            bank_idx,
                                            new_map_sid,
                                            map_rbank,
                                            &num_insts));
            /*
             * Check whether the provided bank already exists in the current
             * mapping. If yes, no operation required, otherwise, add it as
             * new bank mapping.
             */
            for (map_idx = 0; map_idx < lt_ctrl->num_maps; map_idx++) {
                rm_hash_map_info_t *map_info = NULL;
                map_info = &lt_ctrl->map_info[map_idx];
                rm_hash_lt_existing_map_sid_get(unit,
                                                lt_ctrl,
                                                map_info,
                                                existing_map_sid);
                if (rm_hash_map_sid_is_equal(map_info,
                                             existing_map_sid,
                                             new_map_sid,
                                             num_insts) == TRUE) {
                    break;
                }
                if (new_map_sid[0] == existing_map_sid[0]) {
                    SHR_IF_ERR_EXIT
                        (rm_hash_lt_ctrl_multi_map_bank_update(unit,
                                                               lt_ctrl,
                                                               map_info,
                                                               new_map_sid,
                                                               map_rbank,
                                                               num_insts));
                    break;
                }
            }
            if (map_idx == lt_ctrl->num_maps) {
                /* There are new banks assigned. */
                SHR_IF_ERR_EXIT
                    (rm_hash_lt_ctrl_multi_map_bank_enable(unit,
                                                           lt_ctrl,
                                                           new_map_sid,
                                                           map_rbank,
                                                           num_insts));
                SHR_IF_ERR_EXIT
                    (rm_hash_lt_ctrl_hit_context_set(unit,
                                                     lt_ctrl,
                                                     map_idx,
                                                     new_map_sid,
                                                     num_insts));
            }
        }
    } else {
        /* Some banks are removed. */
        rm_hash_lt_ctrl_map_bank_enable_clear(unit, lt_ctrl);
        rm_hash_lt_ctrl_hit_context_clear(unit, lt_ctrl);

        for (bank_idx = 0; bank_idx < en_map_cnt; bank_idx++) {
            SHR_IF_ERR_EXIT
                (rm_hash_lt_new_map_sid_get(unit,
                                            pt_info,
                                            &banks_info,
                                            bank_idx,
                                            new_map_sid,
                                            map_rbank,
                                            &num_insts));
            /* There are new banks assigned. */
            SHR_IF_ERR_EXIT
                (rm_hash_lt_ctrl_multi_map_bank_enable(unit,
                                                       lt_ctrl,
                                                       new_map_sid,
                                                       map_rbank,
                                                       num_insts));
            SHR_IF_ERR_EXIT
                (rm_hash_lt_ctrl_hit_context_set(unit,
                                                 lt_ctrl,
                                                 bank_idx,
                                                 new_map_sid,
                                                 num_insts));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */
int
bcmptm_rm_hash_lt_ctrl_get(int unit,
                           bcmltd_sid_t ltid,
                           rm_hash_lt_ctrl_t **plt_ctrl)
{
    rm_hash_lt_ctrl_t *lt_ctrl = NULL;
    uint16_t num_ltids = dev_lt_ctrl[unit]->curr;
    uint16_t start = 0, end = 0, idx = 0, mid = 0;

    SHR_FUNC_ENTER(unit);

    end = num_ltids - 1;
    while (idx < num_ltids) {
        mid = (start + end) / 2;
        lt_ctrl = dev_lt_ctrl[unit]->array + mid;
        if (ltid == lt_ctrl->ltid) {
            *plt_ctrl = lt_ctrl;
            SHR_EXIT();
        } else if (ltid < lt_ctrl->ltid) {
            end = mid - 1;
            if ((mid == 0) || (end < start)) {
                SHR_ERR_EXIT(SHR_E_NOT_FOUND);
            }
        } else {
            start = mid + 1;
            if ((mid == num_ltids) || (start > end)) {
                SHR_ERR_EXIT(SHR_E_NOT_FOUND);
            }
        }
        idx++;
    }
    SHR_ERR_EXIT(SHR_E_FAIL);

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_lt_ctrl_add(int unit, bcmltd_sid_t ltid)
{
    bcmptm_rm_hash_lt_info_t *lt_info = NULL;
    uint16_t dtype_list[RM_HASH_SHR_MAX_BANK_COUNT], *dtype = dtype_list;
    const bcmptm_rm_hash_grp_key_info_t *grp_key_info = NULL;
    const bcmptm_rm_hash_view_info_t *base_view_info = NULL;
    uint8_t grp_idx, grp_cnt = 0, fmt_idx, fmt_cnt = 0, format_idx;
    const bcmptm_rm_hash_key_format_t *key_format = NULL;
    uint8_t sid_cnt = 0, idx, bank;
    bcmdrd_sid_t grp_sid = INVALIDm;
    bcmdrd_sid_t sid_list[RM_HASH_MAX_HW_ENTRY_INFO_COUNT] = {INVALIDm};
    rm_hash_pt_info_t *pt_info = NULL;
    rm_hash_lt_ctrl_t *lt_ctrl = NULL;
    rm_hash_fmt_info_t *fmt_info = NULL;
    uint32_t size;

    SHR_FUNC_ENTER(unit);

    lt_ctrl = sal_alloc(sizeof(rm_hash_lt_ctrl_t), "bcmptmRmhashLtCtrlTemp");
    SHR_NULL_CHECK(lt_ctrl, SHR_E_MEMORY);
    sal_memset(lt_ctrl, 0, sizeof(*lt_ctrl));

    lt_ctrl->ltid = ltid;
    lt_info = &(lt_ctrl->lt_info);

    /* For a given LTID, retrieve its LRD info. */
    SHR_IF_ERR_EXIT
        (bcmptm_cmdproc_ptrm_info_get(unit, ltid, 0, FALSE, lt_info, NULL,
                                      &dtype));
    /*
     * Get the number of lt to pt mapping groups for this LT. For older devices,
     * the group number is always 1. For flexible devices, the number of group
     * may be greater than 1.
     */
    bcmptm_rm_hash_lt_map_grp_cnt_get(unit, lt_info, &grp_cnt);
    for(grp_idx = 0; grp_idx < grp_cnt; grp_idx++) {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_base_view_info_get(unit, lt_info, grp_idx,
                                               &base_view_info));
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_grp_key_info_get(unit, lt_info, grp_idx,
                                             &grp_key_info));
        fmt_cnt = grp_key_info->key_format_count;
        for (fmt_idx = 0; fmt_idx < fmt_cnt; fmt_idx++) {
            key_format = grp_key_info->key_format + fmt_idx;
            /*
             * Iterate all the valid SIDs of this key format, and allocate a node
             * for each SID. Essentially, all the SIDs of this key format will
             * correspond to a single widest SID.
             */
            bcmptm_rm_hash_key_format_sids_get(unit, base_view_info,
                                               key_format, sid_list,
                                               &sid_cnt);
            /*
             * For a specific key format, all its valid SIDs will correspond to
             * a single widest SID thru which the pt info is organized.
             */
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_grp_sid_get(unit, sid_list[0], &grp_sid));
            /*
             * Get the corresponding node based on reg sid. If there is no valid
             * banks for a LT in LRD database, the LT ctrl here will not have
             * valid info.
             */
            SHR_IF_ERR_EXIT
                (bcmptm_rm_hash_pt_info_get(unit, &pt_info));
            fmt_info = &(lt_ctrl->grp_info[grp_idx].fmt_info[fmt_idx]);
            fmt_info->key_format = key_format;
            fmt_info->sid = grp_sid;
            fmt_info->pt_info = pt_info;
            bcmptm_rm_hash_sid_rbank_list_get(unit, grp_sid, pt_info,
                                              fmt_info->rbank_list,
                                              &fmt_info->num_rbanks);
            /* Duplicate the format info into the format info array. */
            format_idx = lt_ctrl->num_fmts;
            sal_memcpy(&(lt_ctrl->fmt_info[format_idx]),
                       fmt_info, sizeof(*fmt_info));
            lt_ctrl->fmt_grp_idx[format_idx] = grp_idx;
            lt_ctrl->num_fmts++;
            if (fmt_idx == 0) {
                /* Compute the size of the group. */
                size = 0;
                for (idx = 0; idx < fmt_info->num_rbanks; idx++) {
                    bank = fmt_info->rbank_list[idx];
                    size += pt_info->base_bkt_cnt[bank];
                }
                lt_ctrl->grp_size[grp_idx] = size;
            }
            lt_ctrl->grp_info[grp_idx].num_fmts++;
        }
        lt_ctrl->num_grps++;
    }

    lt_ctrl->key_field_exchange = rm_hash_key_field_exchange_required(unit,
                                                                     lt_info);
    lt_ctrl->data_field_exchange = FALSE;

    SHR_IF_ERR_EXIT
        (rm_hash_lt_ctrl_set(unit, lt_ctrl));

exit:
    SHR_FREE(lt_ctrl);
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_lt_ctrl_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(rm_hash_lt_ctrl_alloc(unit));

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_lt_ctrl_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(rm_hash_lt_ctrl_free(unit));

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_lt_rbust_info_init(int unit)
{
    rm_hash_lt_ctrl_t *lt_ctrl = NULL;
    uint16_t ltid_idx;
    uint8_t grp_idx, fmt_idx;
    rm_hash_grp_info_t *grp_info = NULL;
    rm_hash_fmt_info_t *fmt_info = NULL;
    const bcmptm_rm_hash_key_format_t *key_format = NULL;
    rm_hash_rhash_ctrl rhash_ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    for (ltid_idx = 0; ltid_idx < dev_lt_ctrl[unit]->curr; ltid_idx++) {
        lt_ctrl = dev_lt_ctrl[unit]->array + ltid_idx;
        for (grp_idx = 0; grp_idx < lt_ctrl->num_grps; grp_idx++) {
            grp_info = &lt_ctrl->grp_info[grp_idx];
            for (fmt_idx = 0; fmt_idx < grp_info->num_fmts; fmt_idx++) {
                fmt_info = &grp_info->fmt_info[fmt_idx];
                key_format = fmt_info->key_format;
                /* Update robust hash configuration */
                if ((key_format->rh_remap_a_sid != NULL) &&
                    (key_format->rh_remap_b_sid != NULL)) {
                    SHR_IF_ERR_EXIT
                        (bcmptm_rm_hash_rbust_info_init(unit,
                                                        key_format,
                                                        &rhash_ctrl));
                    bcmptm_rm_hash_robust_hash_info_update(unit,
                                                           rhash_ctrl,
                                                           fmt_info->pt_info,
                                                           fmt_info->rbank_list,
                                                           fmt_info->num_rbanks);
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_lt_rbust_hash_config(int unit, bool warm)
{
    rm_hash_lt_ctrl_t *lt_ctrl = NULL;
    uint16_t ltid_idx;
    bcmptm_pt_banks_info_t banks_info;
    uint8_t bank_idx, rbank, idx;
    bcmdrd_sid_t sid;
    uint8_t rbank_cnt, rbank_list[RM_HASH_SHR_MAX_BANK_COUNT] = {0};
    rm_hash_pt_info_t *pt_info;
    bool rb_en = FALSE, separate = FALSE;
    uint32_t seed;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_bank_has_separated_sid(unit, &separate));

    if (separate == FALSE) {
        /* This has been configured. */
        SHR_EXIT();
    }

    for (ltid_idx = 0; ltid_idx < dev_lt_ctrl[unit]->curr; ltid_idx++) {
        lt_ctrl = dev_lt_ctrl[unit]->array + ltid_idx;
        sal_memset(&banks_info, 0, sizeof(banks_info));
        SHR_IF_ERR_EXIT
            (bcmptm_cth_uft_bank_info_get_from_lt(unit, lt_ctrl->ltid, 0,
                                                  &banks_info));

        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_pt_info_get(unit, &pt_info));

        for (bank_idx = 0; bank_idx < banks_info.bank_cnt; bank_idx++) {
            for (idx = 0; idx < RM_HASH_PAIRED_BANK_CNT; idx++) {
                if (idx == 0) {
                    sid   = banks_info.bank[bank_idx].bank_sid;
                    rb_en = banks_info.bank[bank_idx].robust_en;
                    seed  = banks_info.bank[bank_idx].seed;
                } else {
                    sid   = banks_info.paired_bank[bank_idx].bank_sid;
                    rb_en = banks_info.paired_bank[bank_idx].robust_en;
                    seed  = banks_info.paired_bank[bank_idx].seed;
                }
                if (sid != INVALIDm) {
                    bcmptm_rm_hash_sid_rbank_list_get(unit, sid, pt_info,
                                                      rbank_list, &rbank_cnt);
                    if (rbank_cnt == 0) {
                        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
                    }
                    rbank = rbank_list[0];
                    SHR_IF_ERR_EXIT
                        (bcmptm_rm_hash_rbust_info_config(unit, warm, TRUE,
                                                          pt_info->rhash_ctrl[rbank],
                                                          NULL, 0, 0,
                                                          seed));
                    /* Update robust hash enable state. */
                    bcmptm_rm_hash_rbust_enable_set(unit,
                                                    pt_info->rhash_ctrl[rbank],
                                                    rb_en);
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_robust_hash_update(int unit,
                                  bcmltd_sid_t ltid,
                                  uint32_t trans_id,
                                  bcmptm_rm_hash_robust_hash_write write_cb)
{
    uint16_t ltid_idx, bank_idx, idx;
    bcmdrd_sid_t sid;
    bool rb_en = FALSE, separate = FALSE;
    bcmptm_pt_banks_info_t banks_info;
    rm_hash_lt_ctrl_t *lt_ctrl = NULL;
    rm_hash_rhash_ctrl rhash_ctrl = NULL;
    uint32_t seed;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_bank_has_separated_sid(unit, &separate));
    if (separate == FALSE) {
        SHR_EXIT();
    }

    for (ltid_idx = 0; ltid_idx < dev_lt_ctrl[unit]->curr; ltid_idx++) {
        lt_ctrl = dev_lt_ctrl[unit]->array + ltid_idx;
        ltid = lt_ctrl->ltid;
        SHR_IF_ERR_EXIT
            (bcmptm_cth_uft_bank_info_get_from_lt(unit, ltid, 0, &banks_info));
        for (bank_idx = 0; bank_idx < banks_info.bank_cnt; bank_idx++) {
            for (idx = 0; idx < RM_HASH_PAIRED_BANK_CNT; idx++) {
                if (idx == 0) {
                    sid = banks_info.bank[bank_idx].bank_sid;
                    rb_en = banks_info.bank[bank_idx].robust_en;
                    seed = banks_info.bank[bank_idx].seed;
                } else {
                    sid = banks_info.paired_bank[bank_idx].bank_sid;
                    rb_en = banks_info.paired_bank[bank_idx].robust_en;
                    seed = banks_info.paired_bank[bank_idx].seed;
                }
                if (sid != INVALIDm) {
                    SHR_IF_ERR_EXIT
                        (bcmptm_rm_hash_robust_hash_ctrl_get(unit, sid,
                                                             &rhash_ctrl));
                    LOG_VERBOSE(BSL_LOG_MODULE,
                               (BSL_META_U(unit,
                               "sid: %s, seed: %d, enable: %d \n"),
                                bcmdrd_pt_sid_to_name(unit, sid), seed, rb_en));
                    SHR_IF_ERR_EXIT
                        (bcmptm_rm_hash_rbust_info_config(unit, FALSE, FALSE,
                                                          rhash_ctrl,
                                                          write_cb,
                                                          ltid,
                                                          trans_id,
                                                          seed));
                    bcmptm_rm_hash_rbust_enable_set(unit, rhash_ctrl, rb_en);
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_lt_bank_enable_init(int unit)
{
    bool separate = FALSE;
    rm_hash_pt_info_t *pt_info = NULL;
    rm_hash_lt_ctrl_t *lt_ctrl = NULL;
    uint16_t ltid_idx;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_bank_has_separated_sid(unit, &separate));

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_pt_info_get(unit, &pt_info));

    for (ltid_idx = 0; ltid_idx < dev_lt_ctrl[unit]->curr; ltid_idx++) {
        lt_ctrl = dev_lt_ctrl[unit]->array + ltid_idx;
        /*
         * RM hash doesn't make any assumption about bank enabling status for
         * an LT. Such information always come from UFT manager.
         */
        if (separate == TRUE) {
            SHR_IF_ERR_EXIT
                (rm_hash_multi_map_lt_config(unit, lt_ctrl, pt_info));
        } else {
            rm_hash_lt_ctrl_map_bank_enable_clear(unit, lt_ctrl);
            SHR_IF_ERR_EXIT
                (rm_hash_single_map_lt_config(unit, lt_ctrl, pt_info));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_lt_hash_vec_attr_update(int unit)
{
    rm_hash_lt_ctrl_t *lt_ctrl = NULL;
    uint16_t ltid_idx;
    bcmptm_pt_banks_info_t banks_info;
    uint8_t bank_idx, rbank, idx, offset;
    bcmdrd_sid_t sid;
    uint8_t rbank_cnt, rbank_list[RM_HASH_SHR_MAX_BANK_COUNT] = {0};
    rm_hash_pt_info_t *pt_info;
    bool separate = FALSE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_bank_has_separated_sid(unit, &separate));

    if (separate == FALSE) {
        /* This has been configured. */
        SHR_EXIT();
    }

    for (ltid_idx = 0; ltid_idx < dev_lt_ctrl[unit]->curr; ltid_idx++) {
        lt_ctrl = dev_lt_ctrl[unit]->array + ltid_idx;
        sal_memset(&banks_info, 0, sizeof(banks_info));
        SHR_IF_ERR_EXIT
            (bcmptm_cth_uft_bank_info_get_from_lt(unit, lt_ctrl->ltid, 0,
                                                  &banks_info));
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_pt_info_get(unit, &pt_info));

        for (bank_idx = 0; bank_idx < banks_info.bank_cnt; bank_idx++) {
            for (idx = 0; idx < RM_HASH_PAIRED_BANK_CNT; idx++) {
                if (idx == 0) {
                    sid = banks_info.bank[bank_idx].bank_sid;
                    offset = banks_info.bank[bank_idx].hash_offset;
                } else {
                    sid = banks_info.paired_bank[bank_idx].bank_sid;
                    offset = banks_info.paired_bank[bank_idx].hash_offset;
                }
                if (sid != INVALIDm) {
                    bcmptm_rm_hash_sid_rbank_list_get(unit,
                                                      sid,
                                                      pt_info,
                                                      rbank_list,
                                                      &rbank_cnt);
                    if (rbank_cnt == 0) {
                        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
                    }
                    rbank = rbank_list[0];
                    /* Update hash offset */
                    pt_info->vec_info[rbank].type = banks_info.vector_type;
                    pt_info->vec_info[rbank].offset = offset;
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_ltid_based_event_handler(int unit)
{

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_lt_hash_vec_attr_update(unit));

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_lt_bkt_mode_init(int unit)
{
    rm_hash_lt_ctrl_t *lt_ctrl = NULL;
    uint16_t ltid_idx;
    uint8_t map_idx, inst, fmt_idx;
    rm_hash_map_info_t *map_info = NULL;
    rm_hash_fmt_info_t *fmt_info = NULL;
    const bcmptm_rm_hash_key_format_t *key_format = NULL;
    rm_hash_pt_info_t *pt_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_pt_info_get(unit, &pt_info));

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_narrow_mode_state_update(unit, pt_info));

    for (ltid_idx = 0; ltid_idx < dev_lt_ctrl[unit]->curr; ltid_idx++) {
        lt_ctrl = dev_lt_ctrl[unit]->array + ltid_idx;
        for (map_idx = 0; map_idx < lt_ctrl->num_maps; map_idx++) {
            map_info = &lt_ctrl->map_info[map_idx];
            for (inst = 0; inst < map_info->num_insts; inst++) {
                fmt_idx = map_info->fmt_idx[inst];
                fmt_info = &lt_ctrl->fmt_info[fmt_idx];
                key_format = fmt_info->key_format;
                SHR_IF_ERR_EXIT
                    (bcmptm_rm_hash_pt_info_key_attrib_update(unit,
                                                              fmt_info->sid,
                                                              pt_info,
                                                              key_format,
                                                              &lt_ctrl->e_nm));
            }
        }
    }
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_pt_info_slb_info_update(unit, pt_info));

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_move_depth_change(int unit)
{
    rm_hash_lt_ctrl_t *lt_ctrl = NULL;
    uint16_t ltid_idx;
    bcmltd_sid_t ltid;
    uint8_t move_depth;

    SHR_FUNC_ENTER(unit);

    for (ltid_idx = 0; ltid_idx < dev_lt_ctrl[unit]->curr; ltid_idx++) {
        lt_ctrl = dev_lt_ctrl[unit]->array + ltid_idx;
        ltid = lt_ctrl->ltid;
        SHR_IF_ERR_EXIT
            (bcmptm_cth_uft_lt_move_depth_get(unit, ltid, &move_depth));
        lt_ctrl->move_depth = move_depth;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_lt_move_depth_update(int unit, bcmltd_sid_t lt_id,
                                    int move_depth)
{
    rm_hash_lt_ctrl_t *lt_ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_lt_ctrl_get(unit, lt_id, &lt_ctrl));
    lt_ctrl->move_depth = move_depth;

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_lt_ctrl_move_depth_init(int unit)
{
    uint8_t depth = 0;
    uint16_t ltid_idx = 0;
    rm_hash_lt_ctrl_t *lt_ctrl = NULL;
    bcmltd_sid_t ltid;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    for (ltid_idx = 0; ltid_idx < dev_lt_ctrl[unit]->curr; ltid_idx++) {
        lt_ctrl = dev_lt_ctrl[unit]->array + ltid_idx;
        ltid = lt_ctrl->ltid;
        rv = bcmptm_cth_uft_lt_move_depth_get(unit, ltid, &depth);
        if (rv == SHR_E_NONE) {
            lt_ctrl->move_depth = depth;
        } else if (rv == SHR_E_UNAVAIL) {
            lt_ctrl->move_depth = BCMPTM_RM_HASH_DEF_HASH_REORDERING_DEPTH;
        } else {
            SHR_ERR_EXIT(rv);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_lt_ctrl_pipe_mode_init(int unit)
{
    rm_hash_lt_ctrl_t *lt_ctrl = NULL;
    bcmptm_rm_hash_lt_info_t *lt_info = NULL;
    uint16_t ltid_idx = 0, fmt_idx = 0;
    bool conditional_pipe;
    bcmltd_sid_t ltid;
    bcmltd_fid_t lfid;
    uint64_t pipe_unique = 0;
    rm_hash_fmt_info_t *fmt_info = NULL;

    SHR_FUNC_ENTER(unit);

    for (ltid_idx = 0; ltid_idx < dev_lt_ctrl[unit]->curr; ltid_idx++) {
        lt_ctrl = dev_lt_ctrl[unit]->array + ltid_idx;
        lt_info = &lt_ctrl->lt_info;
        /* Determine if the LT has conditional pipe field. */
        conditional_pipe = lt_info->enh_more_info->conditional_pipe;
        if (conditional_pipe == TRUE) {
            ltid = lt_info->enh_more_info->conditional_ltid;
            lfid = lt_info->enh_more_info->conditional_lfid;
            SHR_IF_ERR_EXIT
                (bcmcfg_field_get(unit, ltid, lfid, &pipe_unique));
            if (pipe_unique == 1) {
                lt_ctrl->is_per_pipe = TRUE;
            }
        }
        for (fmt_idx = 0; fmt_idx < lt_ctrl->num_fmts; fmt_idx++) {
            fmt_info = &lt_ctrl->fmt_info[fmt_idx];
            bcmptm_rm_hash_pt_info_bank_mode_init(unit,
                                                  fmt_info->num_rbanks,
                                                  fmt_info->rbank_list,
                                                  lt_ctrl->is_per_pipe);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_lt_ctrl_hit_context_get(int unit,
                                       rm_hash_lt_ctrl_t *lt_ctrl,
                                       bcmdrd_sid_t sid,
                                       bcmptm_rm_hash_hit_context_t *hit_context,
                                       bool *context_exist)
{
    uint8_t map_idx, inst, fmt_idx;
    rm_hash_map_info_t *map_info = NULL;
    rm_hash_fmt_info_t *fmt_info = NULL;

    SHR_FUNC_ENTER(unit);

    for (map_idx = 0; map_idx < lt_ctrl->num_maps; map_idx++) {
        map_info = &lt_ctrl->map_info[map_idx];
        for (inst = 0; inst < map_info->num_insts; inst++) {
            fmt_idx = map_info->fmt_idx[inst];
            fmt_info = &lt_ctrl->fmt_info[fmt_idx];
            if (fmt_info->sid == sid) {
                *hit_context = map_info->hit_context[inst];
                *context_exist = TRUE;
                SHR_EXIT();
            }
        }
    }
    *context_exist = FALSE;

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_hit_context_configure(int unit,
                                     bool report_enable,
                                     rm_hash_lt_ctrl_t *lt_ctrl)
{
    uint8_t map_idx, inst, profile_idx, context_id, offset;
    rm_hash_map_info_t *map_info = NULL;
    bcmltd_op_arg_t op_arg = {0};
    bcmdrd_sid_t grp_sid, map_sid[RM_HASH_LOOKUP_MAX_CNT];
    const bcmptm_rm_hash_hit_tbl_info_t *hit_tbl_info = NULL;

    SHR_FUNC_ENTER(unit);

    if (report_enable == TRUE) {
        for (map_idx = 0; map_idx < lt_ctrl->num_maps; map_idx++) {
            map_info = &lt_ctrl->map_info[map_idx];
            rm_hash_lt_existing_map_sid_get(unit, lt_ctrl, map_info, map_sid);
            for (inst = 0; inst < map_info->num_insts; inst++) {
                /* Get the widest table view. */
                SHR_IF_ERR_EXIT
                    (bcmptm_rm_hash_grp_sid_get(unit, map_sid[inst], &grp_sid));
                SHR_IF_ERR_EXIT
                    (bcmptm_rm_hash_hit_tbl_info_get(unit,
                                                     grp_sid,
                                                     &hit_tbl_info));
                offset = hit_tbl_info->offset;
                /*
                 * Set all the entries of hit index profile table at present stage,
                 * will set entries of the specified LT in the future.
                 */
                for (profile_idx = 0; profile_idx < RM_HASH_HIT_PROFILE_ENTRY_CNT;
                     profile_idx++) {
                    context_id = map_info->hit_context[inst].tile_id;
                    context_id = context_id << offset;
                    SHR_IF_ERR_EXIT
                        (bcmptm_uft_fc_hit_context_set_rm_hash(unit,
                                                               lt_ctrl->ltid,
                                                               &op_arg,
                                                               grp_sid,
                                                               profile_idx,
                                                               context_id));
                }
            }
        }
    } else {
        for (map_idx = 0; map_idx < lt_ctrl->num_maps; map_idx++) {
            map_info = &lt_ctrl->map_info[map_idx];
            rm_hash_lt_existing_map_sid_get(unit, lt_ctrl, map_info, map_sid);
            for (inst = 0; inst < map_info->num_insts; inst++) {
                /* Get the widest table view. */
                SHR_IF_ERR_EXIT
                    (bcmptm_rm_hash_grp_sid_get(unit, map_sid[inst], &grp_sid));
                /*
                 * Clear all the entries of hit index profile table at present stage,
                 * will clear entries of the specified LT in the future.
                 */
                for (profile_idx = 0; profile_idx < RM_HASH_HIT_PROFILE_ENTRY_CNT;
                     profile_idx++) {
                    SHR_IF_ERR_EXIT
                        (bcmptm_uft_fc_hit_context_set_rm_hash(unit,
                                                               lt_ctrl->ltid,
                                                               &op_arg,
                                                               grp_sid,
                                                               profile_idx,
                                                               0));
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_lt_ctrl_dump(int unit, bcmltd_sid_t ltid)
{
    rm_hash_lt_ctrl_t *lt_ctrl = NULL;
    rm_hash_grp_info_t *grp_info = NULL;
    rm_hash_fmt_info_t *fmt_info = NULL;
    uint8_t grp_idx, fmt_idx, bank_idx, inst, map;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_lt_ctrl_get(unit,
                                    ltid,
                                    &lt_ctrl));
    LOG_VERBOSE(BSL_LOG_MODULE,
               (BSL_META_U(unit,
                          "\n*******************************************\n"
                          "ltid:[%d], \n"
                          "key field exchange:[%d], \n"
                          "data field exchange:[%d], \n"
                          "number of groups:[%d], \n"
                          "move depth:[%d] \n"),
                          ltid,
                          lt_ctrl->key_field_exchange,
                          lt_ctrl->data_field_exchange,
                          lt_ctrl->num_grps,
                          lt_ctrl->move_depth));

    for (grp_idx = 0; grp_idx < lt_ctrl->num_grps; grp_idx++) {
        grp_info = &lt_ctrl->grp_info[grp_idx];
        LOG_VERBOSE(BSL_LOG_MODULE,
                   (BSL_META_U(unit,
                               "\n\n"
                               "group index:[%d] \n"),
                               grp_idx));
        for (fmt_idx = 0; fmt_idx < grp_info->num_fmts; fmt_idx++) {
            fmt_info = &grp_info->fmt_info[fmt_idx];
            LOG_VERBOSE(BSL_LOG_MODULE,
                       (BSL_META_U(unit,
                                   "\nkey format:[%p], \n"
                                   "SID:[%s], \n"
                                   "pt info:[%p], \n"
                                   "number of enabled banks:[%d] \n"),
                                   (void*)fmt_info->key_format,
                                   bcmdrd_pt_sid_to_name(unit, fmt_info->sid),
                                   (void*)fmt_info->pt_info,
                                   fmt_info->num_en_rbanks));
            for (bank_idx = 0; bank_idx < fmt_info->num_en_rbanks; bank_idx++) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                           (BSL_META_U(unit,
                                       "idx:[%d], \n"
                                       "enabled rbank:[%d] \n"),
                                       bank_idx,
                                       fmt_info->en_rbank_list[bank_idx]));
            }
        }
    }
    LOG_VERBOSE(BSL_LOG_MODULE,
               (BSL_META_U(unit,
                          "\n##############################################\n"
                          "ltid:[%d], \n"
                          "number of formats:[%d] \n"),
                          ltid,
                          lt_ctrl->num_fmts));
    for (fmt_idx = 0; fmt_idx < lt_ctrl->num_fmts; fmt_idx++) {
        fmt_info = &lt_ctrl->fmt_info[fmt_idx];
        LOG_VERBOSE(BSL_LOG_MODULE,
                   (BSL_META_U(unit,
                               "key format:[%p], \n"
                               "SID:[%s], \n"
                               "pt info:[%p], \n"
                               "number of enabled banks:[%d] \n"),
                               (void*)fmt_info->key_format,
                               bcmdrd_pt_sid_to_name(unit, fmt_info->sid),
                               (void*)fmt_info->pt_info,
                               fmt_info->num_en_rbanks));
        for (bank_idx = 0; bank_idx < fmt_info->num_en_rbanks; bank_idx++) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                       (BSL_META_U(unit,
                                   "idx:[%d], \n"
                                   "enabled rbank:[%d] \n"),
                                   bank_idx,
                                   fmt_info->en_rbank_list[bank_idx]));
        }
    }
    for (map = 0; map < lt_ctrl->num_maps; map++) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n"
                             "ltid:[%d], \n"
                             "number of formats in the map:[%d], inst:[%d]\n"),
                             ltid, map, lt_ctrl->map_info[map].num_insts));
        for (inst = 0; inst < lt_ctrl->map_info[map].num_insts; inst++) {
            fmt_idx = lt_ctrl->map_info[map].fmt_idx[inst];
            fmt_info = &lt_ctrl->fmt_info[fmt_idx];
            LOG_VERBOSE(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                "key format:[%p], \n"
                                "SID:[%s], \n"
                                "pt info:[%p], \n"
                                "number of enabled banks:[%d] \n"),
                                (void*)fmt_info->key_format,
                                bcmdrd_pt_sid_to_name(unit, fmt_info->sid),
                                (void*)fmt_info->pt_info,
                                fmt_info->num_en_rbanks));
            for (bank_idx = 0; bank_idx < fmt_info->num_en_rbanks; bank_idx++) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                           (BSL_META_U(unit,
                                       "idx:[%d], \n"
                                       "enabled rbank:[%d] \n"),
                                       bank_idx,
                                       fmt_info->en_rbank_list[bank_idx]));
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_dev_lt_ctrl_dump(int unit)
{
    uint16_t ltid_idx = 0;
    rm_hash_lt_ctrl_t *lt_ctrl = NULL;
    bcmltd_sid_t ltid;

    SHR_FUNC_ENTER(unit);

    for (ltid_idx = 0; ltid_idx < dev_lt_ctrl[unit]->curr; ltid_idx++) {
        lt_ctrl = dev_lt_ctrl[unit]->array + ltid_idx;
        ltid = lt_ctrl->ltid;
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_lt_ctrl_dump(unit, ltid));
    }

exit:
    SHR_FUNC_EXIT();
}

