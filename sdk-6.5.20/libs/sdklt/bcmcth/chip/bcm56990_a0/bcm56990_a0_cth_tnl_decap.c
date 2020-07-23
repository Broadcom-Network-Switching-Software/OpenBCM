/*! \file bcm56990_a0_cth_tnl_decap.c
 *
 * This file defines the detach, attach routines of
 * TNL decap driver for bcm56990_a0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <bcmdrd_config.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmdrd/chip/bcm56990_a0_enum.h>
#include <bcmdrd/bcmdrd_types.h>
#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <sal/sal_libc.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmcth/bcmcth_tnl_drv.h>
#include <bcmcth/bcmcth_imm_util.h>

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_TNL

#define BCM56990_A0_BOS_ACTION_L3_IIF                   2
#define BCM56990_A0_BOS_ACTION_SWAP_NHI                 3
#define BCM56990_A0_BOS_ACTION_L3_NHI                   4
#define BCM56990_A0_BOS_ACTION_L3_ECMP                  5
#define BCM56990_A0_BOS_ACTION_SWAP_ECMP                6

#define BCM56990_A0_NON_BOS_ACTION_POP                  1
#define BCM56990_A0_NON_BOS_ACTION_PHP_NHI              2
#define BCM56990_A0_NON_BOS_ACTION_SWAP_NHI             3
#define BCM56990_A0_NON_BOS_ACTION_SWAP_ECMP            4
#define BCM56990_A0_NON_BOS_ACTION_PHP_ECMP             5

uint32_t bcm56990_a0_tnl_mpls_decap_hw_sid[] = {
    MPLS_ENTRY_SINGLEm,
    L3_TUNNEL_SINGLEm,
};

bcmptm_rm_hash_entry_mode_t bcm56990_a0_tnl_mpls_entry_attrs_list[] = {
    BCMPTM_RM_HASH_ENTRY_MODE_TNL_DECAP_MPLS,
    BCMPTM_RM_HASH_ENTRY_MODE_TNL_DECAP_L3_TNL,
};

/*******************************************************************************
 * Private functions
 */
static int
bcm56990_a0_tnl_mpls_decap_lt_fid_to_hw_fid(int unit,
                                            uint32_t lt_fid,
                                            uint32_t bos_action,
                                            uint32_t non_bos_action,
                                            uint32_t *hw_fid)
{
    SHR_FUNC_ENTER(unit);

    switch (lt_fid) {
        case TNL_MPLS_DECAPt_MPLS_LABELf:
            *hw_fid = MPLSv_MPLS_LABELf;
            break;
        case TNL_MPLS_DECAPt_MODPORTf:
            *hw_fid = MPLSv_PORT_NUMf;
            break;
        case TNL_MPLS_DECAPt_BOS_ACTIONSf:
            *hw_fid = MPLSv_MPLS_ACTION_IF_BOSf;
            break;
        case TNL_MPLS_DECAPt_NON_BOS_ACTIONSf:
            *hw_fid = MPLSv_MPLS_ACTION_IF_NOT_BOSf;
            break;
        case TNL_MPLS_DECAPt_L3_IIF_IDf:
            if (bos_action != BCM56990_A0_BOS_ACTION_L3_IIF) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            *hw_fid = MPLSv_L3_IIFf;
            break;
        case TNL_MPLS_DECAPt_NHOP_IDf:
            if ((bos_action != BCM56990_A0_BOS_ACTION_SWAP_NHI) &&
                (bos_action != BCM56990_A0_BOS_ACTION_L3_NHI) &&
                (non_bos_action != BCM56990_A0_NON_BOS_ACTION_PHP_NHI) &&
                (non_bos_action != BCM56990_A0_NON_BOS_ACTION_SWAP_NHI)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            *hw_fid = MPLSv_NEXT_HOP_INDEXf;
            break;
        case TNL_MPLS_DECAPt_ECMP_IDf:
            if ((bos_action != BCM56990_A0_BOS_ACTION_L3_ECMP) &&
                (bos_action != BCM56990_A0_BOS_ACTION_SWAP_ECMP) &&
                (non_bos_action != BCM56990_A0_NON_BOS_ACTION_SWAP_ECMP) &&
                (non_bos_action != BCM56990_A0_NON_BOS_ACTION_PHP_ECMP)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            *hw_fid = MPLSv_ECMP_PTRf;
            break;
        case TNL_MPLS_DECAPt_USE_TTL_FROM_DECAP_HDRf:
            *hw_fid = MPLSv_DECAP_USE_TTLf;
            break;
        case TNL_MPLS_DECAPt_KEEP_PAYLOAD_DSCPf:
            *hw_fid = MPLSv_DO_NOT_CHANGE_PAYLOAD_DSCPf;
            break;
        case TNL_MPLS_DECAPt_TNL_EXP_TO_INNER_EXPf:
            *hw_fid = MPLSv_DECAP_USE_EXP_FOR_INNERf;
            break;
        case TNL_MPLS_DECAPt_IPV6_PAYLOADf:
            *hw_fid = MPLSv_V6_ENABLEf;
            break;
        case TNL_MPLS_DECAPt_IPV4_PAYLOADf:
            *hw_fid = MPLSv_V4_ENABLEf;
            break;
        case TNL_MPLS_DECAPt_DROPf:
            *hw_fid = MPLSv_DROP_DATA_ENABLEf;
            break;
        case TNL_MPLS_DECAPt_EXP_MAP_ACTIONf:
            *hw_fid = MPLSv_DECAP_USE_EXP_FOR_PRIf;
            break;
        case TNL_MPLS_DECAPt_PHB_ING_MPLS_EXP_TO_INT_PRI_IDf:
            *hw_fid = MPLSv_EXP_MAPPING_PTRf;
            break;
        case TNL_MPLS_DECAPt_INT_PRIf:
            *hw_fid = MPLSv_NEW_PRIf;
            break;
        case TNL_MPLS_DECAPt_ECN_MPLS_EXP_TO_IP_ECN_IDf:
            *hw_fid = MPLSv_EXP_TO_IP_ECN_MAPPING_PTRf;
            break;
        case TNL_MPLS_DECAPt_BFDf:
            *hw_fid = MPLSv_BFD_ENABLEf;
            break;
        case TNL_MPLS_DECAPt_CTR_ING_EFLEX_OBJECTf:
            *hw_fid = MPLSv_FLEX_CTR_OBJECTf;
            break;
        case TNL_MPLS_DECAPt_TNL_MPLS_CTR_ING_EFLEX_ACTION_IDf:
            *hw_fid = MPLSv_FLEX_CTR_ACTION_SELf;
            break;
        default:
            break;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_tnl_mpls_decap_trunk_lt_fid_to_hw_fid(int unit,
                                                  uint32_t lt_fid,
                                                  uint32_t bos_action,
                                                  uint32_t non_bos_action,
                                                  uint32_t *hw_fid)
{
    SHR_FUNC_ENTER(unit);

    switch (lt_fid) {
        case TNL_MPLS_DECAP_TRUNKt_MPLS_LABELf:
            *hw_fid = MPLSv_MPLS_LABELf;
            break;
        case TNL_MPLS_DECAP_TRUNKt_TRUNK_IDf:
            *hw_fid = MPLSv_TGIDf;
            break;
        case TNL_MPLS_DECAP_TRUNKt_BOS_ACTIONSf:
            *hw_fid = MPLSv_MPLS_ACTION_IF_BOSf;
            break;
        case TNL_MPLS_DECAP_TRUNKt_NON_BOS_ACTIONSf:
            *hw_fid = MPLSv_MPLS_ACTION_IF_NOT_BOSf;
            break;
        case TNL_MPLS_DECAP_TRUNKt_L3_IIF_IDf:
            if (bos_action != BCM56990_A0_BOS_ACTION_L3_IIF) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            *hw_fid = MPLSv_L3_IIFf;
            break;
        case TNL_MPLS_DECAP_TRUNKt_NHOP_IDf:
            if ((bos_action != BCM56990_A0_BOS_ACTION_SWAP_NHI) &&
                (bos_action != BCM56990_A0_BOS_ACTION_L3_NHI) &&
                (non_bos_action != BCM56990_A0_NON_BOS_ACTION_PHP_NHI) &&
                (non_bos_action != BCM56990_A0_NON_BOS_ACTION_SWAP_NHI)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            *hw_fid = MPLSv_NEXT_HOP_INDEXf;
            break;
        case TNL_MPLS_DECAP_TRUNKt_ECMP_IDf:
            if ((bos_action != BCM56990_A0_BOS_ACTION_L3_ECMP) &&
                (bos_action != BCM56990_A0_BOS_ACTION_SWAP_ECMP) &&
                (non_bos_action != BCM56990_A0_NON_BOS_ACTION_SWAP_ECMP) &&
                (non_bos_action != BCM56990_A0_NON_BOS_ACTION_PHP_ECMP)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            *hw_fid = MPLSv_ECMP_PTRf;
            break;
        case TNL_MPLS_DECAP_TRUNKt_USE_TTL_FROM_DECAP_HDRf:
            *hw_fid = MPLSv_DECAP_USE_TTLf;
            break;
        case TNL_MPLS_DECAP_TRUNKt_KEEP_PAYLOAD_DSCPf:
            *hw_fid = MPLSv_DO_NOT_CHANGE_PAYLOAD_DSCPf;
            break;
        case TNL_MPLS_DECAP_TRUNKt_TNL_EXP_TO_INNER_EXPf:
            *hw_fid = MPLSv_DECAP_USE_EXP_FOR_INNERf;
            break;
        case TNL_MPLS_DECAP_TRUNKt_IPV6_PAYLOADf:
            *hw_fid = MPLSv_V6_ENABLEf;
            break;
        case TNL_MPLS_DECAP_TRUNKt_IPV4_PAYLOADf:
            *hw_fid = MPLSv_V4_ENABLEf;
            break;
        case TNL_MPLS_DECAP_TRUNKt_DROPf:
            *hw_fid = MPLSv_DROP_DATA_ENABLEf;
            break;
        case TNL_MPLS_DECAP_TRUNKt_EXP_MAP_ACTIONf:
            *hw_fid = MPLSv_DECAP_USE_EXP_FOR_PRIf;
            break;
        case TNL_MPLS_DECAP_TRUNKt_PHB_ING_MPLS_EXP_TO_INT_PRI_IDf:
            *hw_fid = MPLSv_EXP_MAPPING_PTRf;
            break;
        case TNL_MPLS_DECAP_TRUNKt_INT_PRIf:
            *hw_fid = MPLSv_NEW_PRIf;
            break;
        case TNL_MPLS_DECAP_TRUNKt_ECN_MPLS_EXP_TO_IP_ECN_IDf:
            *hw_fid = MPLSv_EXP_TO_IP_ECN_MAPPING_PTRf;
            break;
        case TNL_MPLS_DECAP_TRUNKt_BFDf:
            *hw_fid = MPLSv_BFD_ENABLEf;
            break;
        case TNL_MPLS_DECAP_TRUNKt_CTR_ING_EFLEX_OBJECTf:
            *hw_fid = MPLSv_FLEX_CTR_OBJECTf;
            break;
        case TNL_MPLS_DECAP_TRUNKt_TNL_MPLS_CTR_ING_EFLEX_ACTION_IDf:
            *hw_fid = MPLSv_FLEX_CTR_ACTION_SELf;
            break;
        default:
            break;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_tnl_decap_lt_to_hw_map(int unit,
                                   bcmltd_sid_t lt_sid,
                                   bcmdrd_sid_t sid,
                                   const bcmltd_fields_t *in,
                                   uint32_t **ekw,
                                   uint32_t **edw)
{
    int      rv = SHR_E_NONE;
    int      count = 0, i = 0;
    uint32_t fid = 0, hw_fid = 0;
    uint64_t bos_action = 0;
    uint64_t non_bos_action = 0;
    uint64_t data = 0;
    uint32_t fbuf[2] = {0};
    bcmlrd_field_def_t  fdef;

    SHR_FUNC_ENTER(unit);

    if (lt_sid == TNL_MPLS_DECAPt) {
        fid = TNL_MPLS_DECAPt_BOS_ACTIONSf;
    } else if (lt_sid == TNL_MPLS_DECAP_TRUNKt) {
        fid = TNL_MPLS_DECAP_TRUNKt_BOS_ACTIONSf;
    }
    rv = bcmcth_imm_fields_get(unit, in, fid, &bos_action);
    if (rv == SHR_E_NOT_FOUND) {
        bcmdrd_pt_field_get(unit,
                            sid,
                            edw[0],
                            MPLSv_MPLS_ACTION_IF_BOSf,
                            fbuf);
        bos_action = fbuf[0];
    }

    if (lt_sid == TNL_MPLS_DECAPt) {
        fid = TNL_MPLS_DECAPt_NON_BOS_ACTIONSf;
    } else if (lt_sid == TNL_MPLS_DECAP_TRUNKt) {
        fid = TNL_MPLS_DECAP_TRUNKt_NON_BOS_ACTIONSf;
    }
    rv = bcmcth_imm_fields_get(unit, in, fid, &non_bos_action);
    if (rv == SHR_E_NOT_FOUND) {
        bcmdrd_pt_field_get(unit,
                            sid,
                            edw[0],
                            MPLSv_MPLS_ACTION_IF_NOT_BOSf,
                            fbuf);
        non_bos_action = fbuf[0];
    }

    count = in->count;

    for (i = 0; i < count; i++) {
        fid = in->field[i]->id;
        data = in->field[i]->data;

        sal_memset(&fdef, 0, sizeof(fdef));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_table_field_def_get(unit, lt_sid, fid, &fdef));

        if (lt_sid == TNL_MPLS_DECAPt) {
            rv = bcm56990_a0_tnl_mpls_decap_lt_fid_to_hw_fid(unit,
                                                             fid,
                                                             bos_action,
                                                             non_bos_action,
                                                             &hw_fid);
        } else if (lt_sid == TNL_MPLS_DECAP_TRUNKt) {
            rv = bcm56990_a0_tnl_mpls_decap_trunk_lt_fid_to_hw_fid(unit,
                                                                   fid,
                                                                   bos_action,
                                                                   non_bos_action,
                                                                   &hw_fid);
        } else {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        if (rv != SHR_E_NONE) {
            continue;
        }

        BCMCTH_TNL_DECAP_UINT64_TO_UINT32(data, fbuf);

        if (fdef.key == true) {
            bcmdrd_pt_field_set(unit,
                                sid,
                                ekw[0],
                                hw_fid,
                                fbuf);
        } else {
            bcmdrd_pt_field_set(unit,
                                sid,
                                edw[0],
                                hw_fid,
                                fbuf);
        }
    }

    /* Program KEY_TYPE. */
    hw_fid = KEY_TYPEf;
    data = 0;
    BCMCTH_TNL_DECAP_UINT64_TO_UINT32(data, fbuf);
    bcmdrd_pt_field_set(unit, sid, ekw[0], hw_fid, fbuf);

    /* Program BASE_VALID as 1. */
    hw_fid = BASE_VALIDf;
    data = 1;
    BCMCTH_TNL_DECAP_UINT64_TO_UINT32(data, fbuf);
    bcmdrd_pt_field_set(unit, sid, ekw[0], hw_fid, fbuf);

    /* Program TRUNK_TYPE. */
    hw_fid = MPLSv_Tf;
    data = 0;
    if (lt_sid == TNL_MPLS_DECAP_TRUNKt) {
        data = 1;
    }
    BCMCTH_TNL_DECAP_UINT64_TO_UINT32(data, fbuf);
    bcmdrd_pt_field_set(unit, sid, ekw[0], hw_fid, fbuf);

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_tnl_decap_hw_to_lt_map(int unit,
                                   bcmltd_sid_t lt_sid,
                                   bcmdrd_sid_t sid,
                                   uint32_t **ekw,
                                   uint32_t **edw,
                                   bcmltd_fields_t *out)
{
    int         rv = SHR_E_NONE;
    size_t      num_fid = 0, count = 0;
    uint32_t    table_sz = 0, i = 0;
    uint32_t    fid = 0, hw_fid = 0;
    uint32_t    bos_action = 0;
    uint32_t    non_bos_action = 0;
    uint64_t    data = 0;
    uint32_t    fbuf[2] = {0};
    bcmlrd_fid_t   *fid_list = NULL;
    bcmlrd_field_def_t  fdef;

    SHR_FUNC_ENTER(unit);

    bcmdrd_pt_field_get(unit,
                        sid,
                        edw[0],
                        MPLSv_MPLS_ACTION_IF_BOSf,
                        fbuf);
    bos_action = fbuf[0];

    bcmdrd_pt_field_get(unit,
                        sid,
                        edw[0],
                        MPLSv_MPLS_ACTION_IF_NOT_BOSf,
                        fbuf);
    non_bos_action = fbuf[0];

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_count_get(unit, lt_sid, &num_fid));

    table_sz = (num_fid * sizeof(bcmlrd_fid_t));
    SHR_ALLOC(fid_list, table_sz, "bcmcthTnlFldList");
    SHR_NULL_CHECK(fid_list, SHR_E_MEMORY);
    sal_memset(fid_list, 0, table_sz);

    /* Get the list of valid fields for this LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_list_get(unit,
                               lt_sid,
                               num_fid,
                               fid_list,
                               &count));

    out->count = 0;
    for (i = 0; i < count; i++) {
        fid = fid_list[i];

        fbuf[0] = fbuf[1] = 0;

        sal_memset(&fdef, 0, sizeof(fdef));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_table_field_def_get(unit, lt_sid, fid, &fdef));

        if (lt_sid == TNL_MPLS_DECAPt) {
            rv = bcm56990_a0_tnl_mpls_decap_lt_fid_to_hw_fid(unit,
                                                             fid,
                                                             bos_action,
                                                             non_bos_action,
                                                             &hw_fid);
        } else if (lt_sid == TNL_MPLS_DECAP_TRUNKt) {
            rv = bcm56990_a0_tnl_mpls_decap_trunk_lt_fid_to_hw_fid(unit,
                                                                   fid,
                                                                   bos_action,
                                                                   non_bos_action,
                                                                   &hw_fid);
        } else {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        if (rv != SHR_E_NONE) {
            continue;
        }

        if (fdef.key == true) {
            bcmdrd_pt_field_get(unit,
                                sid,
                                ekw[0],
                                hw_fid,
                                fbuf);
        } else {
            bcmdrd_pt_field_get(unit,
                                sid,
                                edw[0],
                                hw_fid,
                                fbuf);
        }

        BCMCTH_TNL_DECAP_UINT32_TO_UINT64(fbuf, data);
        out->field[out->count]->id = fid;
        out->field[out->count]->data = data;
        out->count++;
    }

exit:
    SHR_FREE(fid_list);
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

bcmcth_tnl_decap_drv_t bcm56990_a0_tnl_decap_drv = {
    .sid_count = 2,
    .sid_list = bcm56990_a0_tnl_mpls_decap_hw_sid,
    .entry_attrs_list = bcm56990_a0_tnl_mpls_entry_attrs_list,
    .mpls_decap_lt_to_hw_map = bcm56990_a0_tnl_decap_lt_to_hw_map,
    .mpls_decap_hw_to_lt_map = bcm56990_a0_tnl_decap_hw_to_lt_map,
};

bcmcth_tnl_decap_drv_t *
bcm56990_a0_cth_tnl_decap_drv_get(int unit)
{
    return &bcm56990_a0_tnl_decap_drv;
}
