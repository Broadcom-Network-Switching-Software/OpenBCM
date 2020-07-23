/*! \file bcmptm_rm_hash_pt_register.c
 *
 * Physical table register function for hash resource manager
 *
 * This file contains physical table register function for physical hash tables.
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
#include <shr/shr_ha.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmltd/chip/bcmltd_limits.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmptm/bcmptm_chip_internal.h>
#include <bcmptm/bcmptm_cth_uft_be_internal.h>
#include "bcmptm_rm_hash.h"
#include "bcmptm_rm_hash_pt_register.h"
#include "bcmptm_rm_hash_robust_hash.h"
#include "bcmptm_rm_hash_utils.h"
#include "bcmptm_rm_hash_state_mgmt.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMHASH

/*******************************************************************************
 * Typedefs
 */


/*******************************************************************************
 * Private variables
 */

static rm_hash_pt_info_t rm_hash_pt_info[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private Functions
 */
/*!
 * \brief Get the overlay sid.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Physical hash table sid.
 * \param [out] ovly_sid Physical hash table overlay sid.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_ovly_sid_get(int unit,
                     bcmdrd_sid_t sid,
                     bcmdrd_sid_t *ovly_sid)
{
    const bcmptm_overlay_info_t *ovly_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_pt_overlay_info_get(unit, sid, &ovly_info));
    if ((ovly_info->mode != BCMPTM_OINFO_MODE_NONE)) {
        SHR_IF_ERR_EXIT
            (ovly_info->mode != BCMPTM_OINFO_MODE_HASH ?
                                SHR_E_INTERNAL : SHR_E_NONE);
        *ovly_sid = ovly_info->w_sid;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Determine if the given SID has been registered in pt info.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Physical table SID to be checked.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static bool
rm_hash_sid_registered(int unit,
                       bcmdrd_sid_t sid,
                       rm_hash_pt_info_t *pt_info)
{
    uint8_t rbank;

    for (rbank = 0; rbank < pt_info->num_rbanks; rbank++) {
        if (pt_info->rbank_sid[rbank] == sid) {
            return TRUE;
        }
    }
    return FALSE;
}

/*!
 * \brief Get the bank base bucket number.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Physical table SID.
 * \param [in] bucket_size Bucket size.
 * \param [out] count Buffer to record the result.
 *
 * \retval NONE.
 */
static void
rm_hash_bank_base_bucket_cnt_get(int unit,
                                 bcmdrd_sid_t sid,
                                 uint8_t bucket_size,
                                 uint32_t *count)
{
    uint32_t be_cnt;
    int index_min, index_max;

    index_min = bcmdrd_pt_index_min(unit, sid);
    index_max = bcmdrd_pt_index_max(unit, sid);
    if ((index_min) < 0 || (index_max < 0) || (index_min > index_max)) {
        be_cnt = 0;
    } else {
        be_cnt = (1 + index_max - index_min);
    }
    *count = be_cnt / bucket_size;
}

/*!
 * \brief Register all the required information into pt info.
 *
 * \param [in] unit Unit number.
 * \param [in] base_view_info Base view information in a group.
 * \param [in] key_format A specific key format in a group.
 * \param [out] pt_info Pointer to rm_hash_pt_info_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_key_format_register(int unit,
                            const bcmptm_rm_hash_view_info_t *base_view_info,
                            const bcmptm_rm_hash_key_format_t *key_format,
                            rm_hash_pt_info_t *pt_info)
{
    const bcmptm_rm_hash_view_info_t *view_info[RM_HASH_MAX_HW_ENTRY_INFO_COUNT] = {NULL};
    bcmdrd_sid_t ovly_sid = INVALIDm, wsid = INVALIDm;
    const bcmptm_rm_hash_vec_bank_attr_t *bank_attr = NULL;
    uint8_t bank_idx, rbank, view_cnt = 0;
    uint8_t ent_size;
    uint32_t bb_cnt = 0;

    SHR_FUNC_ENTER(unit);

    if (pt_info->pre_config == TRUE) {
        SHR_EXIT();
    }

    pt_info->bank_separate_sid = key_format->bank_separate_sid;
    /*
     * For some devices, all the bucket modes are supported for
     * all the hash tables.
     */
    if (key_format->bank_separate_sid == TRUE) {
        pt_info->bmf = RM_HASH_BM0 | RM_HASH_BM1 | RM_HASH_BM2;
    }

    /*
     * This key format may correspond to multiple SIDs. Get all the valid SIDs
     * for this key format.
     */
    bcmptm_rm_hash_key_format_view_list_get(unit,
                                            base_view_info,
                                            key_format,
                                            view_info,
                                            &view_cnt);
    if (view_cnt == 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    /* Ensure using the widest SID to register in pt info. */
    wsid = view_info[0]->sid[0];
    SHR_IF_ERR_EXIT
        (rm_hash_ovly_sid_get(unit, wsid, &ovly_sid));
    if (ovly_sid != INVALIDm) {
        wsid = ovly_sid;
    }
    if (rm_hash_sid_registered(unit, wsid, pt_info) == FALSE) {
        /*
         * This SID has not been registered in pt info. Retrieve the information
         * about the number of valid banks this key format spans and base bucket
         * number for each valid bank.
         * The base bucket count and bank count information here is the
         * information in LRD database. The valid bank bitmap can be updated
         * runtime for UFT bank based LT.
         * Also, if there is no banks for a LT in LRD database, the per bank
         * attribute in pt info will NOT be set accordlingly.
         */
        /* Update physical bucket size information. */
        bank_attr = key_format->hash_vector_attr->bank_attr;
        rbank = pt_info->num_rbanks;
        for (bank_idx = 0; bank_idx < key_format->num_banks; bank_idx++) {
            pt_info->base_bkt_cnt[rbank] = bank_attr[bank_idx].mask + 1;
            pt_info->vec_info[rbank].mask = bank_attr[bank_idx].mask;
            if (key_format->bank_separate_sid == TRUE) {
                /* For different SKU support. */
                rm_hash_bank_base_bucket_cnt_get(unit,
                                                 key_format->single_view_sid[0],
                                                 key_format->bucket_size,
                                                 &bb_cnt);
                pt_info->base_bkt_cnt[rbank] = bb_cnt;
                pt_info->vec_info[rbank].mask = bb_cnt - 1;
            }
            rbank++;
        }

        /* Update various SID information. */
        rbank = pt_info->num_rbanks;
        for (bank_idx = 0; bank_idx < key_format->num_banks; bank_idx++) {
            pt_info->single_view_sid[rbank] = key_format->single_view_sid;
            pt_info->key_attrib[rbank].attr_sid = key_format->key_attr_sid;
            pt_info->lp_sid[rbank] = view_info[0]->lp_sid;
            pt_info->rbank_sid[rbank] = wsid;
            for (ent_size = 0; ent_size <= RM_HASH_MAX_ENT_SIZE; ent_size++) {
                pt_info->sid_list[rbank].view_sid[ent_size] = INVALIDm;
            }
            rbank++;
        }

        /* Update hash type information. */
        rbank = pt_info->num_rbanks;
        for (bank_idx = 0; bank_idx < key_format->num_banks; bank_idx++) {
            pt_info->hash_type[rbank] = key_format->hash_type;
            pt_info->base_bkt_size[rbank] = key_format->bucket_size;
            if (key_format->hash_type == BCMPTM_RM_HASH_TYPE_DUAL) {
                pt_info->lbkt_size[rbank] = pt_info->base_bkt_size[rbank] / 2;
            } else {
                pt_info->lbkt_size[rbank] = pt_info->base_bkt_size[rbank];
            }
            rbank++;
        }
        /* Update registered bank information. */
        pt_info->num_rbanks += key_format->num_banks;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Register all the valid view SID information into pt info.
 *
 * \param [in] unit Unit number.
 * \param [in] base_view_info Base view information in a group.
 * \param [in] key_format A specific key format in a group.
 * \param [out] pt_info Pointer to rm_hash_pt_info_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_key_format_sid_register(int unit,
                                const bcmptm_rm_hash_view_info_t *base_view_info,
                                const bcmptm_rm_hash_key_format_t *key_format,
                                rm_hash_pt_info_t *pt_info)
{
    const bcmptm_rm_hash_view_info_t *view_info[RM_HASH_MAX_HW_ENTRY_INFO_COUNT] = {NULL};
    const bcmptm_rm_hash_view_info_t *view = NULL;
    bcmdrd_sid_t ovly_sid = INVALIDm, wsid = INVALIDm;
    uint8_t bank_idx, rbank, view_cnt = 0;
    uint8_t rbank_cnt, rbank_list[RM_HASH_SHR_MAX_BANK_COUNT] = {0};
    uint8_t view_idx, ent_size;


    SHR_FUNC_ENTER(unit);

    /*
     * This key format may correspond to multiple SIDs. Get all the valid SIDs
     * for this key format.
     */
    bcmptm_rm_hash_key_format_view_list_get(unit,
                                            base_view_info,
                                            key_format,
                                            view_info,
                                            &view_cnt);
    if (view_cnt == 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    /* Ensure using the widest SID to register in pt info. */
    wsid = view_info[0]->sid[0];
    SHR_IF_ERR_EXIT
        (rm_hash_ovly_sid_get(unit, wsid, &ovly_sid));
    if (ovly_sid != INVALIDm) {
        wsid = ovly_sid;
    }

    /* Update valid sid info for this key format. */
    bcmptm_rm_hash_sid_rbank_list_get(unit, wsid, pt_info, rbank_list, &rbank_cnt);
    for (bank_idx = 0; bank_idx < rbank_cnt; bank_idx++) {
        rbank = rbank_list[bank_idx];
        for (view_idx = 0; view_idx < view_cnt; view_idx++) {
            view = view_info[view_idx];
            ent_size = view->num_base_entries;
            pt_info->sid_list[rbank].view_sid[ent_size] = view->sid[0];
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the widest bucket mode.
 *
 * \param [in] unit Unit number.
 * \param [in] bmf Bucket mode flags.
 * \param [out] max_bm Maximum bucket mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_max_bkt_mode_get(int unit, uint8_t bmf, rm_hash_bm_t *max_bm)
{
    SHR_FUNC_ENTER(unit);

    if (bmf & RM_HASH_BM2) {
        *max_bm = RM_HASH_BM_2;
        SHR_EXIT();
    }
    if (bmf & RM_HASH_BM1) {
        *max_bm = RM_HASH_BM_1;
        SHR_EXIT();
    }
    if (bmf & RM_HASH_BM0) {
        *max_bm = RM_HASH_BM_0;
        SHR_EXIT();
    }
    if (bmf & RM_HASH_BMN) {
        *max_bm = RM_HASH_BM_N;
        SHR_EXIT();
    }
    SHR_ERR_EXIT(SHR_E_PARAM);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set Software logical bucket size for a pt info.
 *
 * \param [in] unit Unit number.
 * \param [out] pt_info Pointer to rm_hash_pt_info_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_pt_info_slb_size_set(int unit, rm_hash_pt_info_t *pt_info)
{
    uint8_t lbkt_size = pt_info->lbkt_size[0];

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (rm_hash_max_bkt_mode_get(unit, pt_info->bmf, &pt_info->t_bm));
    /*
     * num_base_entries in rm hash more info structure describes
     * how many hardware logical entries are contained in a hardware
     * logical bucket.
     */
    switch (pt_info->t_bm) {
    case RM_HASH_BM_0:
    case RM_HASH_BM_N:
        pt_info->slb_size = lbkt_size;
        break;
    case RM_HASH_BM_1:
        pt_info->slb_size = lbkt_size * 2;
        break;
    case RM_HASH_BM_2:
        pt_info->slb_size = lbkt_size * 4;
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    pt_info->slb_num_entries = pt_info->slb_size;
    if (pt_info->narrow_mode_supported == TRUE) {
        pt_info->slb_num_entries = RM_HASH_MAX_ENT_NUM_IN_NARROW_MODE;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the number of SLBs for each register bank in pt info.
 *
 * \param [in] unit Unit number.
 * \param [out] pt_info Pointer to rm_hash_pt_info_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_pt_info_rbank_num_slbs_set(int unit, rm_hash_pt_info_t *pt_info)
{
    uint8_t rbank;
    rm_hash_bm_t bm;
    uint32_t num_base_bkts = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (rm_hash_max_bkt_mode_get(unit, pt_info->bmf, &bm));

    for (rbank = 0; rbank < pt_info->num_rbanks; rbank++) {
        /* Get the number of base buckets for this bank. */
        num_base_bkts = pt_info->base_bkt_cnt[rbank];
        switch (bm) {
        case RM_HASH_BM_N:
        case RM_HASH_BM_0:
            pt_info->rbank_num_slbs[rbank] = num_base_bkts;
            break;
        case RM_HASH_BM_1:
            pt_info->rbank_num_slbs[rbank] = num_base_bkts / 2;
            break;
        case RM_HASH_BM_2:
            pt_info->rbank_num_slbs[rbank] = num_base_bkts / 4;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the base SLB for each bank in pt info.
 *
 * \param [in] unit Unit number.
 * \param [out] pt_info Pointer to rm_hash_pt_info_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static void
rm_hash_pt_info_rbank_slb_base_set(int unit, rm_hash_pt_info_t *pt_info)
{
    uint8_t rbank;
    uint32_t num_slbs = 0;

    for (rbank = 0; rbank < pt_info->num_rbanks; rbank++) {
        pt_info->rbank_slb_base[rbank] = num_slbs;
        num_slbs += pt_info->rbank_num_slbs[rbank];
    }
}

/*!
 * \brief Get SW bucket mode for an key format.
 *
 * \param [in] unit Unit number.
 * \param [in] key_format Pointer to bcmptm_rm_hash_key_format_t structure.
 * \param [out] bm Pointer to rm_hash_bm_t structure.
 * \param [out] bmf Pointer to uint8_t object.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_key_format_bkt_mode_get(int unit,
                                const bcmptm_rm_hash_key_format_t *key_format,
                                bool narrow_mode,
                                rm_hash_bm_t *bm,
                                uint8_t *bmf,
                                rm_hash_entry_narrow_mode_t *enm)
{
    bcmbd_pt_dyn_info_t dyn_info = {-1, -1};

    SHR_FUNC_ENTER(unit);

    if (key_format->key_attr_sid == NULL) {
        switch (key_format->entry_mode) {
        case BCMPTM_RM_HASH_ENTRY_MODE_EM_128:
        case BCMPTM_RM_HASH_ENTRY_MODE_EM_160:
            *bm = RM_HASH_BM_0;
            *bmf = RM_HASH_BM0;
            SHR_EXIT();
        case BCMPTM_RM_HASH_ENTRY_MODE_EM_320:
            *bm = RM_HASH_BM_1;
            *bmf = RM_HASH_BM1;
            SHR_EXIT();
        case BCMPTM_RM_HASH_ENTRY_MODE_NONE:
            *bm = RM_HASH_BM_N;
            *bmf = RM_HASH_BMN;
            SHR_EXIT();
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    } else {
        bool cache_valid = FALSE;
        bcmltd_sid_t ltid = 0;
        uint16_t dft_id = 0;
        uint16_t key_type = 0;
        uint32_t bkt_mode = 0;
        uint32_t rd_rsp_flags = 0;
        uint32_t e_words[BCMPTM_MAX_PT_ENTRY_WORDS] = {0};

        if (key_format->key_type_val != NULL) {
            key_type = *key_format->key_type_val;
        }
        dyn_info.index = key_type;
        SHR_IF_ERR_EXIT
            (bcmptm_cmdproc_read(unit,
                                 0,
                                 0,
                                 *key_format->key_attr_sid,
                                 &dyn_info,
                                 NULL,
                                 BCMPTM_MAX_PT_ENTRY_WORDS,
                                 e_words,
                                 &cache_valid,
                                 &ltid,
                                 &dft_id,
                                 &rd_rsp_flags));
        /*
         * The start bit and end bit position of BUCKET_MODE field in
         * KEY_TYPE_ATTRIBUTE table is chip wide constant.
         */
        bcmdrd_field_get(e_words, 0, 1, &bkt_mode);
        if (narrow_mode == TRUE) {
            switch (bkt_mode) {
            case 0:
                *bm = RM_HASH_BM_0;
                *bmf = RM_HASH_BM0;
                *enm = BCMPTM_RM_HASH_ENTRY_NARROW_MODE_NONE;
                SHR_EXIT();
            case 1:
                *bm = RM_HASH_BM_1;
                *bmf = RM_HASH_BM1;
                *enm = BCMPTM_RM_HASH_ENTRY_NARROW_MODE_NONE;
                SHR_EXIT();
            case 2:
                *bm = RM_HASH_BM_0;
                *bmf = RM_HASH_BM0;
                *enm = BCMPTM_RM_HASH_ENTRY_NARROW_MODE_HALF;
                SHR_EXIT();
            case 3:
                *bm = RM_HASH_BM_0;
                *bmf = RM_HASH_BM0;
                *enm = BCMPTM_RM_HASH_ENTRY_NARROW_MODE_THIRD;
                SHR_EXIT();
            default:
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
        } else {
            *enm = BCMPTM_RM_HASH_ENTRY_NARROW_MODE_NONE;
            switch (bkt_mode) {
            case 0:
                *bm = RM_HASH_BM_0;
                *bmf = RM_HASH_BM0;
                SHR_EXIT();
            case 1:
                *bm = RM_HASH_BM_1;
                *bmf = RM_HASH_BM1;
                SHR_EXIT();
            case 2:
                *bm = RM_HASH_BM_2;
                *bmf = RM_HASH_BM2;
                SHR_EXIT();
            default:
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the maximum bank number for a LT.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_info Pointer to bcmptm_rm_hash_lt_info_t structure.
 * \param [out] bank_cnt Pointer to uint8_t objects to hold the bank count.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_lt_bank_cnt_get(int unit,
                        bcmptm_rm_hash_lt_info_t *lt_info,
                        uint8_t *bank_cnt)
{
    const bcmptm_rm_hash_grp_key_info_t *grp_key_info = NULL;
    const bcmptm_rm_hash_key_format_t *key_format = NULL;
    uint8_t tmp_cnt = 0;
    uint8_t grp_cnt, grp_idx, fmt_cnt, fmt_idx;

    SHR_FUNC_ENTER(unit);

    bcmptm_rm_hash_lt_map_grp_cnt_get(unit, lt_info, &grp_cnt);
    for (grp_idx = 0; grp_idx < grp_cnt; grp_idx++) {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_grp_key_info_get(unit, lt_info, grp_idx,
                                             &grp_key_info));
        fmt_cnt = grp_key_info->key_format_count;
        for (fmt_idx = 0; fmt_idx < fmt_cnt; fmt_idx++) {
            key_format = grp_key_info->key_format + fmt_idx;
            tmp_cnt += key_format->num_banks;
        }
    }
    *bank_cnt = tmp_cnt;

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */
void
bcmptm_rm_hash_sid_based_event_handler(int unit)
{
    rm_hash_pt_info_t *pt_info = NULL;
    bcmptm_pt_banks_info_t banks_info;
    uint8_t rbank_list[RM_HASH_SHR_MAX_BANK_COUNT], rbank, rbank_cnt;
    uint8_t idx, bank_idx;
    bcmdrd_sid_t sid;
    rm_hash_vector_info_t *vec_info = NULL;
    bool rb_en = FALSE;

    (void)bcmptm_rm_hash_pt_info_get(unit, &pt_info);

    for (bank_idx = 0; bank_idx < pt_info->num_rbanks; bank_idx++) {
        sid = pt_info->rbank_sid[bank_idx];
        bcmptm_rm_hash_sid_rbank_list_get(unit, sid, pt_info, rbank_list,
                                          &rbank_cnt);
        (void)bcmptm_cth_uft_bank_info_get_from_ptcache(unit, sid,
                                                        &banks_info);
        if (banks_info.bank_cnt > rbank_cnt) {
            /* The bank count exceeds the maximum bank count. */
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "The bank count exceeds the max count.\n")));
        }

        for (idx = 0; idx < banks_info.bank_cnt; idx++) {
            /*
             * Update hash vector type and offset based on information from
             * UFT manager.
             */
            rbank = rbank_list[idx];
            vec_info = &pt_info->vec_info[rbank];
            if (vec_info->type != banks_info.vector_type) {
                vec_info->type = banks_info.vector_type;
            }
            if (vec_info->offset != banks_info.bank[idx].hash_offset) {
                vec_info->offset = banks_info.bank[idx].hash_offset;
            }
            /*
             * Configure robust hash. For some devices, a multiple-bank table
             * has single robust hash configuration. All the banks share the same
             * robust hash configuration.
             */
            if (idx == 0) {
                rb_en = banks_info.bank[idx].robust_en;
                if (pt_info->rhash_ctrl[rbank] != NULL) {
                    bcmptm_rm_hash_rbust_enable_set(unit,
                                                    pt_info->rhash_ctrl[rbank],
                                                    rb_en);
                }
            }
        }
    }
}

int
bcmptm_rm_hash_pt_reg_info_add(int unit, bcmltd_sid_t ltid)
{
    bcmptm_rm_hash_lt_info_t lt_info;
    uint16_t dtype_list[RM_HASH_SHR_MAX_BANK_COUNT], *dtype = NULL;
    const bcmptm_rm_hash_grp_key_info_t *grp_key_info = NULL;
    const bcmptm_rm_hash_view_info_t *base_view_info = NULL;
    uint8_t grp_idx, grp_cnt = 0, fmt_idx, fmt_cnt = 0, bank_cnt = 0;
    const bcmptm_rm_hash_key_format_t *key_format = NULL;
    uint8_t sid_cnt = 0;
    bcmdrd_sid_t sid_list[RM_HASH_MAX_HW_ENTRY_INFO_COUNT] = {INVALIDm};
    rm_hash_pt_info_t *pt_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* For a given LTID, retrieve its LRD info. */
    sal_memset(&lt_info, 0, sizeof(bcmptm_rm_hash_lt_info_t));
    dtype = &dtype_list[0];
    SHR_IF_ERR_EXIT
        (bcmptm_cmdproc_ptrm_info_get(unit, ltid, 0, FALSE, &lt_info, NULL,
                                      &dtype));
    /*
     * Get the number of mapping groups for this LT. For older devices,
     * the group number is always 1. For newer devices, the number of group
     * may be greater than 1.
     */
    bcmptm_rm_hash_lt_map_grp_cnt_get(unit, &lt_info, &grp_cnt);
    for(grp_idx = 0; grp_idx < grp_cnt; grp_idx++) {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_base_view_info_get(unit, &lt_info, grp_idx,
                                               &base_view_info));
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_grp_key_info_get(unit, &lt_info, grp_idx,
                                             &grp_key_info));
        fmt_cnt = grp_key_info->key_format_count;
        for (fmt_idx = 0; fmt_idx < fmt_cnt; fmt_idx++) {
            key_format = grp_key_info->key_format + fmt_idx;
            pt_info = &rm_hash_pt_info[unit];
            /*
             * Iterate all the valid SIDs of this key format, and allocate a node
             * for each SID. Essentially, all the SIDs of this key format will
             * correspond to a single widest SID.
             */
            bcmptm_rm_hash_key_format_sids_get(unit, base_view_info,
                                               key_format, sid_list,
                                               &sid_cnt);
            if (sid_cnt == 0) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            SHR_IF_ERR_EXIT
                (rm_hash_key_format_register(unit,
                                             base_view_info,
                                             key_format,
                                             pt_info));
            SHR_IF_ERR_EXIT
                (rm_hash_key_format_sid_register(unit,
                                                 base_view_info,
                                                 key_format,
                                                 pt_info));
        }
    }

    SHR_IF_ERR_EXIT
        (rm_hash_lt_bank_cnt_get(unit, &lt_info, &bank_cnt));

    if ((pt_info != NULL) && (bank_cnt > pt_info->max_num_mapping_banks)) {
        pt_info->max_num_mapping_banks = bank_cnt;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_pt_info_vec_init(int unit, bool warm)
{
    rm_hash_pt_info_t *pt_info = NULL;
    bcmptm_pt_banks_info_t banks_info;
    bcmptm_rm_hash_def_bank_info_t def_bank_info;
    uint8_t rbank_list[RM_HASH_SHR_MAX_BANK_COUNT], rbank_cnt, bank_idx, rbank;
    bcmptm_rm_hash_vector_type_t vec_type;
    uint8_t en_bank_cnt, idx;
    bcmdrd_sid_t sid;
    uint32_t seed = 0xFFFFFD, mask = 0;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_pt_info_get(unit, &pt_info));

    if (pt_info->bank_separate_sid == TRUE) {
        SHR_EXIT();
    }

    for (bank_idx = 0; bank_idx < pt_info->num_rbanks; bank_idx++) {
        sid = pt_info->rbank_sid[bank_idx];
        bcmptm_rm_hash_sid_rbank_list_get(unit, sid, pt_info, rbank_list,
                                          &rbank_cnt);
        rv = bcmptm_cth_uft_bank_info_get_from_ptcache(unit, sid,
                                                       &banks_info);
        if (rv == SHR_E_NONE) {
            vec_type    = banks_info.vector_type;
            en_bank_cnt = banks_info.bank_cnt;
            if (en_bank_cnt > rbank_cnt) {
                /* The bank count exceeds the maximum possible bank count. */
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            for (idx = 0; idx < en_bank_cnt; idx++) {
                rbank = rbank_list[idx];
                pt_info->vec_info[rbank].type = vec_type;
                pt_info->vec_info[rbank].offset =
                    banks_info.bank[idx].hash_offset;
            }

            rbank = rbank_list[0];
            if (pt_info->rhash_ctrl[rbank] != NULL) {
                /*
                 * Update robust hash enable state. For some older devices,
                 * robust random seed has not been exposed by LT.
                 */
                for (idx = 0; idx < rbank_cnt; idx++) {
                    rbank = rbank_list[idx];
                    SHR_IF_ERR_EXIT
                        (bcmptm_rm_hash_rbust_info_config(unit, warm, TRUE,
                                                          pt_info->rhash_ctrl[rbank],
                                                          NULL, 0, 0, seed));
                    bcmptm_rm_hash_rbust_enable_set
                        (unit,
                         pt_info->rhash_ctrl[rbank],
                         banks_info.bank[idx].robust_en);
                }
            }
        } else if (rv == SHR_E_UNAVAIL) {
            /*
             * For some devices, if UFT mgr cannot provide the information,
             * Try to use the per chip default values.
             */
            rbank = rbank_list[0];
            mask = pt_info->vec_info[rbank].mask;
            SHR_IF_ERR_EXIT
                (bcmptm_hash_def_bank_info_get(unit, mask, &def_bank_info));
            vec_type    = def_bank_info.vector_type;
            en_bank_cnt = def_bank_info.bank_cnt;
            if (en_bank_cnt > rbank_cnt) {
                /* The bank count exceeds the maximum possible bank count. */
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }

            for (idx = 0; idx < en_bank_cnt; idx++) {
                rbank = rbank_list[idx];
                pt_info->vec_info[rbank].type = vec_type;
                pt_info->vec_info[rbank].offset =
                    def_bank_info.hash_offset[idx];
            }

            rbank = rbank_list[0];
            if (pt_info->rhash_ctrl[rbank] != NULL) {
                /*
                 * Update robust hash enable state. For some older devices,
                 * robust random seed has not been exposed by LT.
                 */
                for (idx = 0; idx < rbank_cnt; idx++) {
                    rbank = rbank_list[idx];
                    SHR_IF_ERR_EXIT
                        (bcmptm_rm_hash_rbust_info_config(unit, warm, TRUE,
                                                          pt_info->rhash_ctrl[rbank],
                                                          NULL, 0, 0, seed));
                    bcmptm_rm_hash_rbust_enable_set
                        (unit,
                         pt_info->rhash_ctrl[rbank],
                         def_bank_info.robust_en[idx]);
                }
            }
        } else {
            SHR_ERR_EXIT(rv);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_pt_info_key_attrib_update(int unit,
                                         bcmdrd_sid_t sid,
                                         rm_hash_pt_info_t *pt_info,
                                         const bcmptm_rm_hash_key_format_t *key_format,
                                         rm_hash_entry_narrow_mode_t *e_nm)
{
    uint8_t rbank_cnt, rbank, rbank_list[RM_HASH_SHR_MAX_BANK_COUNT] = {0};
    rm_hash_bm_t bm = 0;
    uint8_t bmf = 0, bank_idx;
    uint16_t key_type = 0;
    rm_hash_entry_narrow_mode_t enm = BCMPTM_RM_HASH_ENTRY_NARROW_MODE_NONE;
    bool bank_in_narrow_mode = FALSE;

    SHR_FUNC_ENTER(unit);

    if (key_format->key_type_val == NULL) {
        /*
         * For dynamic hash LTs, the bucket mode will be provided dynamically
         * by caller.
         */
        SHR_EXIT();
    }
    bcmptm_rm_hash_sid_rbank_list_get(unit,
                                      sid,
                                      pt_info,
                                      rbank_list,
                                      &rbank_cnt);
    key_type = key_format->key_type_val[0];
    for (bank_idx = 0; bank_idx < rbank_cnt; bank_idx++) {
        rbank = rbank_list[bank_idx];
        bank_in_narrow_mode = pt_info->bank_in_narrow_mode[rbank];
        SHR_IF_ERR_EXIT
            (rm_hash_key_format_bkt_mode_get(unit,
                                             key_format,
                                             bank_in_narrow_mode,
                                             &bm,
                                             &bmf,
                                             &enm));
        pt_info->key_attrib[rbank].attrib[key_type].bm = bm;
        pt_info->key_attrib[rbank].attrib[key_type].enm = enm;
        pt_info->key_attrib[rbank].attrib[key_type].valid = TRUE;
        pt_info->bmf |= bmf;
        if (bank_idx == 0) {
            *e_nm = enm;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_pt_info_slb_info_update(int unit, rm_hash_pt_info_t *pt_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (rm_hash_pt_info_slb_size_set(unit, pt_info));

    SHR_IF_ERR_EXIT
        (rm_hash_pt_info_rbank_num_slbs_set(unit, pt_info));

    rm_hash_pt_info_rbank_slb_base_set(unit, pt_info);

exit:
    SHR_FUNC_EXIT();
}

void
bcmptm_rm_hash_robust_hash_info_update(int unit,
                                       rm_hash_rhash_ctrl rhash_ctrl,
                                       rm_hash_pt_info_t *pt_info,
                                       uint8_t *rbank_list,
                                       uint8_t num_rbanks)
{
    uint8_t bank_idx, rbank;

    for (bank_idx = 0; bank_idx < num_rbanks; bank_idx++) {
        rbank = rbank_list[bank_idx];
        pt_info->rhash_ctrl[rbank] = rhash_ctrl;
    }
}

void
bcmptm_rm_hash_sid_rbank_list_get(int unit,
                                  bcmdrd_sid_t sid,
                                  rm_hash_pt_info_t *pt_info,
                                  uint8_t *rbank_list,
                                  uint8_t *rbank_cnt)
{
    uint8_t rbank, tmp_cnt = 0;

    for (rbank = 0; rbank < pt_info->num_rbanks; rbank++) {
        if (pt_info->rbank_sid[rbank] == sid) {
            *rbank_list = rbank;
            rbank_list++;
            tmp_cnt++;
        }
    }
    *rbank_cnt = tmp_cnt;
}

int
bcmptm_rm_hash_pt_info_get(int unit, rm_hash_pt_info_t **pt_info)
{

    SHR_FUNC_ENTER(unit);

    *pt_info = &rm_hash_pt_info[unit];

    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_robust_hash_ctrl_get(int unit,
                                    bcmdrd_sid_t sid,
                                    rm_hash_rhash_ctrl *rhash_ctrl)
{
    rm_hash_pt_info_t *pt_info;
    uint8_t rbank, rbank_cnt, rbank_list[RM_HASH_SHR_MAX_BANK_COUNT] = {0};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_pt_info_get(unit, &pt_info));
    bcmptm_rm_hash_sid_rbank_list_get(unit,
                                      sid,
                                      pt_info,
                                      rbank_list,
                                      &rbank_cnt);
    rbank = rbank_list[0];
    *rhash_ctrl = pt_info->rhash_ctrl[rbank];

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_bank_has_separated_sid(int unit, bool *separate)
{

    SHR_FUNC_ENTER(unit);

    *separate = rm_hash_pt_info[unit].bank_separate_sid;

    SHR_FUNC_EXIT();
}

void
bcmptm_rm_hash_pt_reg_info_cleanup(int unit)
{
    rm_hash_pt_info_t *pt_info = &rm_hash_pt_info[unit];

    sal_memset(pt_info, 0, sizeof(*pt_info));
}

void
bcmptm_rm_hash_pt_info_bank_mode_init(int unit,
                                      uint8_t num_rbanks,
                                      uint8_t *rbank_list,
                                      bool pipe_unique)
{
    rm_hash_pt_info_t *pt_info;
    uint8_t idx, rbank;
    rm_hash_bank_mode_t mode;

    pt_info = &rm_hash_pt_info[unit];

    for (idx = 0; idx < num_rbanks; idx++) {
        rbank = rbank_list[idx];
        mode = pt_info->bank_mode[rbank];
        switch (mode) {
            case RM_HASH_BANK_MODE_NONE:
                if (pipe_unique == TRUE) {
                    pt_info->bank_mode[rbank] = RM_HASH_BANK_MODE_PERPILE;
                } else {
                    pt_info->bank_mode[rbank] = RM_HASH_BANK_MODE_GLOBAL;
                }
                break;
            case RM_HASH_BANK_MODE_GLOBAL:
                if (pipe_unique == TRUE) {
                   pt_info->bank_mode[rbank] = RM_HASH_BANK_MODE_MIXED;
                   pt_info->bank_mixed_mode = TRUE;
                }
                break;
            case RM_HASH_BANK_MODE_PERPILE:
                if (pipe_unique == FALSE) {
                   pt_info->bank_mode[rbank] = RM_HASH_BANK_MODE_MIXED;
                   pt_info->bank_mixed_mode = TRUE;
                }
                break;
            default:
                break;
        }
    }
}

/*!
 * \brief Configure the per unit hash control object.
 *
 * \param [in] unit Unit number.
 *
 * \retval NONE.
 */
int
bcmptm_rm_hash_pt_info_pre_config(int unit)
{
    const bcmptm_rm_hash_table_info_t *tbl_info = NULL;
    rm_hash_pt_info_t *pt_info = NULL;
    uint8_t tbl_idx, bank_idx, tbl_banks, num_banks = 0, num_tbl_info = 0;
    uint32_t base_bkt_cnt, bb_cnt = 0;
    uint8_t ent_size;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_pt_info_get(unit, &pt_info));

    rv = bcmptm_hash_table_info_get(unit, &tbl_info, &num_tbl_info);
    if ((rv == SHR_E_UNAVAIL) || (num_tbl_info == 0)) {
        pt_info->pre_config = FALSE;
        SHR_EXIT();
    }
    if (pt_info->pre_config == TRUE) {
        SHR_EXIT();
    }

    pt_info->bank_separate_sid = tbl_info->separated_sid;
    for (tbl_idx = 0; tbl_idx < num_tbl_info; tbl_idx++, tbl_info++) {
        pt_info->bmf |= tbl_info->bm_flags;
        tbl_banks = tbl_info->num_banks;
        for (bank_idx = 0; bank_idx < tbl_banks; bank_idx++) {
            /* Base bucket configuration. */
            base_bkt_cnt = tbl_info->base_bkt_cnt[bank_idx];
            pt_info->base_bkt_cnt[num_banks] = base_bkt_cnt;
            pt_info->vec_info[num_banks].mask = base_bkt_cnt - 1;
            if (tbl_info->separated_sid == TRUE) {
                /* For different SKU support. */
                rm_hash_bank_base_bucket_cnt_get(unit,
                                                 tbl_info->single_view_sid,
                                                 tbl_info->bkt_size,
                                                 &bb_cnt);
                pt_info->base_bkt_cnt[num_banks] = bb_cnt;
                pt_info->vec_info[num_banks].mask = bb_cnt - 1;
            }

            /* Various SID configuration. */
            pt_info->single_view_sid[num_banks] = &tbl_info->single_view_sid;
            pt_info->key_attrib[num_banks].attr_sid = &tbl_info->key_attr_sid;
            pt_info->lp_sid[num_banks] = &tbl_info->lp_sid;
            pt_info->rbank_sid[num_banks] = tbl_info->sid;
            for (ent_size = 0; ent_size <= RM_HASH_MAX_ENT_SIZE; ent_size++) {
                pt_info->sid_list[num_banks].view_sid[ent_size] = INVALIDm;
            }

            /* Misc. configuration */
            pt_info->hash_type[num_banks] = tbl_info->type;
            pt_info->base_bkt_size[num_banks] = tbl_info->bkt_size;
            if (tbl_info->type == BCMPTM_RM_HASH_TYPE_DUAL) {
                pt_info->lbkt_size[num_banks] = pt_info->base_bkt_size[num_banks] / 2;
            } else {
                pt_info->lbkt_size[num_banks] = pt_info->base_bkt_size[num_banks];
            }
            /* Narrow mode info initialization. */
            pt_info->nm_info[num_banks] = tbl_info->nm_info;
            if (tbl_info->nm_info != NULL) {
                pt_info->narrow_mode_supported = TRUE;
            }

            num_banks++;
        }
    }
    pt_info->num_rbanks = num_banks;
    pt_info->pre_config = TRUE;

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_narrow_mode_state_update(int unit, rm_hash_pt_info_t *pt_info)
{
    uint8_t rbank;
    const bcmptm_rm_hash_narrow_mode_info_t *nm_info = NULL;
    bcmbd_pt_dyn_info_t dyn_info = {0, -1};
    uint16_t start_bit, end_bit;
    uint32_t value = 0;

    SHR_FUNC_ENTER(unit);

    for (rbank = 0; rbank < pt_info->num_rbanks; rbank++) {
        nm_info = pt_info->nm_info[rbank];
        if (nm_info != NULL) {
            bool cache_valid = FALSE;
            bcmltd_sid_t ltid = 0;
            uint16_t dft_id = 0;
            uint32_t rd_rsp_flags = 0;
            uint32_t e_words[BCMPTM_MAX_PT_ENTRY_WORDS] = {0};
            SHR_IF_ERR_EXIT
                (bcmptm_cmdproc_read(unit,
                                     0,
                                     0,
                                     nm_info->nm_ctrl_sid,
                                     &dyn_info,
                                     NULL,
                                     BCMPTM_MAX_PT_ENTRY_WORDS,
                                     e_words,
                                     &cache_valid,
                                     &ltid,
                                     &dft_id,
                                     &rd_rsp_flags));
            start_bit = nm_info->nm_field_start_bit;
            end_bit = nm_info->nm_field_start_bit + nm_info->nm_field_width - 1;
            bcmdrd_field_get(e_words, start_bit, end_bit, &value);
            if (value > 0) {
                /* Narrow mode is enabled for this table. */
                pt_info->bank_in_narrow_mode[rbank] = TRUE;
            } else {
                pt_info->bank_in_narrow_mode[rbank] = FALSE;
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}


int
bcmptm_rm_hash_pt_reg_info_dump(int unit)
{
    rm_hash_pt_info_t *pt_info = NULL;
    uint8_t rbank, idx, ent_size;
    rm_hash_bank_key_type_attrib_t *attrib = NULL;

    SHR_FUNC_ENTER(unit);

    pt_info = &rm_hash_pt_info[unit];
    LOG_VERBOSE(BSL_LOG_MODULE,
               (BSL_META_U(unit,
                           "\n*******************************************\n"
                           "bucket mode flags:[%d], \n"
                           "num_rbanks:[%d], \n"
                           "bank_separate_sid: [%d], \n"
                           "slb_size:[%d], \n"
                           "max_num_mapping_banks:[%d] \n"),
                           pt_info->bmf,
                           pt_info->num_rbanks,
                           pt_info->bank_separate_sid,
                           pt_info->slb_size,
                           pt_info->max_num_mapping_banks));

    for (rbank = 0; rbank < pt_info->num_rbanks; rbank++) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                   (BSL_META_U(unit,
                               "rbank:[%d], "
                               "base bucket size:[%d] \n"),
                               rbank,
                               pt_info->base_bkt_size[rbank]));
    }

    for (rbank = 0; rbank < pt_info->num_rbanks; rbank++) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                   (BSL_META_U(unit,
                               "rbank:[%d], "
                               "sid:[%s] \n"),
                               rbank,
                               bcmdrd_pt_sid_to_name(unit, pt_info->rbank_sid[rbank])));
        for (ent_size = 0; ent_size <= RM_HASH_MAX_ENT_SIZE; ent_size++) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                       (BSL_META_U(unit,
                                   "ent_size:[%d], "
                                   "sid:[%s] \n"),
                                   ent_size,
                                   bcmdrd_pt_sid_to_name(unit, pt_info->sid_list[rbank].view_sid[ent_size])));
        }
    }

    for (rbank = 0; rbank < pt_info->num_rbanks; rbank++) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                   (BSL_META_U(unit,
                               "rbank:[%d], "
                               "base bucket count:[%d] \n"),
                               rbank,
                               pt_info->base_bkt_cnt[rbank]));
    }

    for (rbank = 0; rbank < pt_info->num_rbanks; rbank++) {
        attrib = &pt_info->key_attrib[rbank];
        for (idx = 0; idx < RM_HASH_MAX_KEY_TYPE_COUNT; idx++) {
            if (attrib->attrib[idx].valid == TRUE) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                           (BSL_META_U(unit,
                                     "rbank:[%d], "
                                     "key type:[%d], "
                                     "valid:[%d], "
                                     "bkt_mode:[%d]\n"),
                                     rbank,
                                     idx,
                                     attrib->attrib[idx].valid,
                                     attrib->attrib[idx].bm));
            }
        }
    }

    for (rbank = 0; rbank < pt_info->num_rbanks; rbank++) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                   (BSL_META_U(unit,
                               "rbank:[%d], "
                               "vector info (type):[%d], \n"
                               "vector info (offset):[%d], \n"
                               "vector info (mask):[%d] \n"),
                               rbank,
                               pt_info->vec_info[rbank].type,
                               pt_info->vec_info[rbank].offset,
                               pt_info->vec_info[rbank].mask));
    }

    for (rbank = 0; rbank < pt_info->num_rbanks; rbank++) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                   (BSL_META_U(unit,
                               "rbank:[%d], "
                               "rbank_num_slbs:[%d], "
                               "rbank_base:[%d] \n"),
                               rbank,
                               pt_info->rbank_num_slbs[rbank],
                               pt_info->rbank_slb_base[rbank]));
    }

    SHR_FUNC_EXIT();
}

