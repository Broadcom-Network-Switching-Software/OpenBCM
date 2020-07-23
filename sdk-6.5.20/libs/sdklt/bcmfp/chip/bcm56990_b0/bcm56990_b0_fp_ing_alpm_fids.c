/*! \file bcm56990_b0_fp_ing_alpm_fids.c
 *
 * API to initialize stage attributes for
 * Tomahawk4-G(56990_B0) device.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_util.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/bcmfp_stage_internal.h>
#include <bcmfp/chip/bcm56990_b0_fp.h>
#include <bcmdrd/chip/bcm56990_b0_enum.h>
#include <bcmcfg/bcmcfg_lt.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmlrd/bcmlrd_map.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

int
bcm56990_b0_fp_ing_alpm_fids_init(int unit, bcmfp_stage_t *stage)
{
    uint64_t value = 0;
    bcmltd_fid_t field;
    const bcmlrd_map_t *map = NULL;
    size_t size = 0;
    bcmfp_tbl_compress_alpm_t *compress_alpm_tbl = NULL;
    bcmltd_fid_t *key_fids = NULL;
    bcmltd_fid_t *mask_fids = NULL;
    uint8_t num_key_fids = 0;
    uint8_t num_mask_fids = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    if (stage->tbls.num_compress_fid_remote_info == 0) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_map_get(unit, FP_CONFIGt, &map));
    field = FP_CONFIGt_FP_ING_MANUAL_COMPf;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcfg_field_get(unit, FP_CONFIGt, field, &value));
    if (value) {
        BCMFP_STAGE_FLAGS_REMOVE(stage,
                          BCMFP_STAGE_LIST_COMPRESSION);
        stage->compress_enabled = FALSE;
        SHR_EXIT();
    } else {
        stage->compress_enabled = TRUE;
    }

    value = 0;
    field = FP_CONFIGt_FP_ING_COMP_SRC_IP4_ONLYf;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcfg_field_get(unit, FP_CONFIGt, field, &value));
    if (value) {
        stage->compress_ip_addr_only[1] = TRUE;
    } else {
        stage->compress_ip_addr_only[1] = FALSE;
    }

    value = 0;
    field = FP_CONFIGt_FP_ING_COMP_DST_IP4_ONLYf;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcfg_field_get(unit, FP_CONFIGt, field, &value));
    if (value) {
        stage->compress_ip_addr_only[0] = TRUE;
    } else {
        stage->compress_ip_addr_only[0] = FALSE;
    }

    value = 0;
    field = FP_CONFIGt_FP_ING_COMP_SRC_IP6_ONLYf;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcfg_field_get(unit, FP_CONFIGt, field, &value));
    if (value) {
        stage->compress_ip_addr_only[3] = TRUE;
    } else {
        stage->compress_ip_addr_only[3] = FALSE;
    }

    value = 0;
    field = FP_CONFIGt_FP_ING_COMP_DST_IP6_ONLYf;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcfg_field_get(unit, FP_CONFIGt, field, &value));
    if (value) {
        stage->compress_ip_addr_only[2] = TRUE;
    } else {
        stage->compress_ip_addr_only[2] = FALSE;
    }

    /* Source IPV4 L3 ALPM FIDs */
    size = sizeof(bcmfp_tbl_compress_alpm_t);
    BCMFP_ALLOC(compress_alpm_tbl, size, "bcmfpL3AlpmIPv4SrcTable");
    stage->tbls.compress_alpm_tbl[1] = compress_alpm_tbl;
    if (stage->compress_ip_addr_only[1] == TRUE) {
        compress_alpm_tbl->sid = L3_IPV4_COMP_SRCt;
        num_key_fids = 1;
        num_mask_fids = num_key_fids;
        size = (num_key_fids * sizeof(bcmltd_fid_t));
        BCMFP_ALLOC(key_fids, size, "bcmfpL3AlpmIPv4SrcTableKeyFids");
        compress_alpm_tbl->key_fids = key_fids;
        compress_alpm_tbl->num_key_fids = num_key_fids;
        size = (num_mask_fids * sizeof(bcmltd_fid_t));
        BCMFP_ALLOC(mask_fids, size, "bcmfpL3AlpmIPv4SrcTableMaskFids");
        compress_alpm_tbl->mask_fids = mask_fids;
        compress_alpm_tbl->num_mask_fids = num_mask_fids;
        key_fids[0] = L3_IPV4_COMP_SRCt_IPV4f;
        mask_fids[0] = L3_IPV4_COMP_SRCt_IPV4_MASKf;
        compress_alpm_tbl->cid_fid = L3_IPV4_COMP_SRCt_CLASS_IDf;
    } else {
        compress_alpm_tbl->sid = L3_IPV4_COMP_SRCt;
        num_key_fids = 7;
        num_mask_fids = num_key_fids;
        size = (num_key_fids * sizeof(bcmltd_fid_t));
        BCMFP_ALLOC(key_fids, size, "bcmfpL3AlpmIPv4SrcTableKeyFids");
        compress_alpm_tbl->key_fids = key_fids;
        compress_alpm_tbl->num_key_fids = num_key_fids;
        size = (num_mask_fids * sizeof(bcmltd_fid_t));
        BCMFP_ALLOC(mask_fids, size, "bcmfpL3AlpmIPv4SrcTableMaskFids");
        compress_alpm_tbl->mask_fids = mask_fids;
        compress_alpm_tbl->num_mask_fids = num_mask_fids;
        key_fids[0] = L3_IPV4_COMP_SRCt_IPV4f;
        mask_fids[0] = L3_IPV4_COMP_SRCt_IPV4_MASKf;
        key_fids[1] = L3_IPV4_COMP_SRCt_MACf;
        mask_fids[1] = L3_IPV4_COMP_SRCt_MAC_MASKf;
        key_fids[2] = L3_IPV4_COMP_SRCt_VRFf;
        mask_fids[2] = L3_IPV4_COMP_SRCt_VRF_MASKf;
        key_fids[3] = L3_IPV4_COMP_SRCt_L4_PORTf;
        mask_fids[3] = L3_IPV4_COMP_SRCt_L4_PORT_MASKf;
        key_fids[4] = L3_IPV4_COMP_SRCt_IP_PROTOCOLf;
        mask_fids[4] = L3_IPV4_COMP_SRCt_IP_PROTOCOL_MASKf;
        key_fids[5] = L3_IPV4_COMP_SRCt_TCP_FLAGSf;
        mask_fids[5] = L3_IPV4_COMP_SRCt_TCP_FLAGS_MASKf;
        key_fids[6] = L3_IPV4_COMP_SRCt_IP_FLAGSf;
        mask_fids[6] = L3_IPV4_COMP_SRCt_IP_FLAGS_MASKf;
        compress_alpm_tbl->cid_fid = L3_IPV4_COMP_SRCt_CLASS_IDf;
    }

    compress_alpm_tbl = NULL;
    num_key_fids = 0;
    num_mask_fids = 0;
    key_fids = NULL;
    mask_fids = NULL;
    /* Destination IPV4 L3 ALPM FIDs */
    size = sizeof(bcmfp_tbl_compress_alpm_t);
    BCMFP_ALLOC(compress_alpm_tbl, size, "bcmfpL3AlpmIPv4DstTable");
    stage->tbls.compress_alpm_tbl[0] = compress_alpm_tbl;
    if (stage->compress_ip_addr_only[0] == TRUE) {
        compress_alpm_tbl->sid = L3_IPV4_COMP_DSTt;
        num_key_fids = 1;
        num_mask_fids = num_key_fids;
        size = (num_key_fids * sizeof(bcmltd_fid_t));
        BCMFP_ALLOC(key_fids, size, "bcmfpL3AlpmIPv4DstTableKeyFids");
        compress_alpm_tbl->key_fids = key_fids;
        compress_alpm_tbl->num_key_fids = num_key_fids;
        size = (num_mask_fids * sizeof(bcmltd_fid_t));
        BCMFP_ALLOC(mask_fids, size, "bcmfpL3AlpmIPv4DstTableMaskFids");
        compress_alpm_tbl->mask_fids = mask_fids;
        compress_alpm_tbl->num_mask_fids = num_mask_fids;
        key_fids[0] = L3_IPV4_COMP_DSTt_IPV4f;
        mask_fids[0] = L3_IPV4_COMP_DSTt_IPV4_MASKf;
        compress_alpm_tbl->cid_fid = L3_IPV4_COMP_DSTt_CLASS_IDf;
    } else {
        compress_alpm_tbl->sid = L3_IPV4_COMP_DSTt;
        num_key_fids = 7;
        num_mask_fids = num_key_fids;
        size = (num_key_fids * sizeof(bcmltd_fid_t));
        BCMFP_ALLOC(key_fids, size, "bcmfpL3AlpmIPv4DstTableKeyFids");
        compress_alpm_tbl->key_fids = key_fids;
        compress_alpm_tbl->num_key_fids = num_key_fids;
        size = (num_mask_fids * sizeof(bcmltd_fid_t));
        BCMFP_ALLOC(mask_fids, size, "bcmfpL3AlpmIPv4DstTableMaskFids");
        compress_alpm_tbl->mask_fids = mask_fids;
        compress_alpm_tbl->num_mask_fids = num_mask_fids;
        key_fids[0] = L3_IPV4_COMP_DSTt_IPV4f;
        mask_fids[0] = L3_IPV4_COMP_DSTt_IPV4_MASKf;
        key_fids[1] = L3_IPV4_COMP_DSTt_MACf;
        mask_fids[1] = L3_IPV4_COMP_DSTt_MAC_MASKf;
        key_fids[2] = L3_IPV4_COMP_DSTt_VRFf;
        mask_fids[2] = L3_IPV4_COMP_DSTt_VRF_MASKf;
        key_fids[3] = L3_IPV4_COMP_DSTt_L4_PORTf;
        mask_fids[3] = L3_IPV4_COMP_DSTt_L4_PORT_MASKf;
        key_fids[4] = L3_IPV4_COMP_DSTt_IP_PROTOCOLf;
        mask_fids[4] = L3_IPV4_COMP_DSTt_IP_PROTOCOL_MASKf;
        key_fids[5] = L3_IPV4_COMP_DSTt_TCP_FLAGSf;
        mask_fids[5] = L3_IPV4_COMP_DSTt_TCP_FLAGS_MASKf;
        key_fids[6] = L3_IPV4_COMP_DSTt_IP_FLAGSf;
        mask_fids[6] = L3_IPV4_COMP_DSTt_IP_FLAGS_MASKf;
        compress_alpm_tbl->cid_fid = L3_IPV4_COMP_DSTt_CLASS_IDf;
    }

    compress_alpm_tbl = NULL;
    num_key_fids = 0;
    num_mask_fids = 0;
    key_fids = NULL;
    mask_fids = NULL;
    /* Source IPV6 L3 ALPM FIDs */
    size = sizeof(bcmfp_tbl_compress_alpm_t);
    BCMFP_ALLOC(compress_alpm_tbl, size, "bcmfpL3AlpmIPv6SrcTable");
    stage->tbls.compress_alpm_tbl[3] = compress_alpm_tbl;
    if (stage->compress_ip_addr_only[3] == TRUE) {
        compress_alpm_tbl->sid = L3_IPV6_COMP_SRCt;
        num_key_fids = 2;
        num_mask_fids = num_key_fids;
        size = (num_key_fids * sizeof(bcmltd_fid_t));
        BCMFP_ALLOC(key_fids, size, "bcmfpL3AlpmIPv6SrcTableKeyFids");
        compress_alpm_tbl->key_fids = key_fids;
        compress_alpm_tbl->num_key_fids = num_key_fids;
        size = (num_mask_fids * sizeof(bcmltd_fid_t));
        BCMFP_ALLOC(mask_fids, size, "bcmfpL3AlpmIPv6SrcTableMaskFids");
        compress_alpm_tbl->mask_fids = mask_fids;
        compress_alpm_tbl->num_mask_fids = num_mask_fids;
        key_fids[0] = L3_IPV6_COMP_SRCt_IPV6u_LOWERf;
        key_fids[1] = L3_IPV6_COMP_SRCt_IPV6u_UPPERf;
        mask_fids[0] = L3_IPV6_COMP_SRCt_IPV6u_LOWER_MASKf;
        mask_fids[1] = L3_IPV6_COMP_SRCt_IPV6u_UPPER_MASKf;
        compress_alpm_tbl->cid_fid = L3_IPV6_COMP_SRCt_CLASS_IDf;
    } else {
        compress_alpm_tbl->sid = L3_IPV6_COMP_SRCt;
        num_key_fids = 6;
        num_mask_fids = num_key_fids;
        size = (num_key_fids * sizeof(bcmltd_fid_t));
        BCMFP_ALLOC(key_fids, size, "bcmfpL3AlpmIPv6SrcTableKeyFids");
        compress_alpm_tbl->key_fids = key_fids;
        compress_alpm_tbl->num_key_fids = num_key_fids;
        size = (num_mask_fids * sizeof(bcmltd_fid_t));
        BCMFP_ALLOC(mask_fids, size, "bcmfpL3AlpmIPv6SrcTableMaskFids");
        compress_alpm_tbl->mask_fids = mask_fids;
        compress_alpm_tbl->num_mask_fids = num_mask_fids;
        key_fids[0] = L3_IPV6_COMP_SRCt_IPV6u_LOWERf;
        key_fids[1] = L3_IPV6_COMP_SRCt_IPV6u_UPPERf;
        key_fids[2] = L3_IPV6_COMP_SRCt_VRFf;
        key_fids[3] = L3_IPV6_COMP_SRCt_L4_PORTf;
        key_fids[4] = L3_IPV6_COMP_SRCt_IP_PROTOCOLf;
        key_fids[5] = L3_IPV6_COMP_SRCt_TCP_FLAGSf;
        mask_fids[0] = L3_IPV6_COMP_SRCt_IPV6u_LOWER_MASKf;
        mask_fids[1] = L3_IPV6_COMP_SRCt_IPV6u_UPPER_MASKf;
        mask_fids[2] = L3_IPV6_COMP_SRCt_VRF_MASKf;
        mask_fids[3] = L3_IPV6_COMP_SRCt_L4_PORT_MASKf;
        mask_fids[4] = L3_IPV6_COMP_SRCt_IP_PROTOCOL_MASKf;
        mask_fids[5] = L3_IPV6_COMP_SRCt_TCP_FLAGS_MASKf;
        compress_alpm_tbl->cid_fid = L3_IPV6_COMP_SRCt_CLASS_IDf;
    }

    compress_alpm_tbl = NULL;
    num_key_fids = 0;
    num_mask_fids = 0;
    key_fids = NULL;
    mask_fids = NULL;
    /* Destination IPV6 L3 ALPM FIDs */
    size = sizeof(bcmfp_tbl_compress_alpm_t);
    BCMFP_ALLOC(compress_alpm_tbl, size, "bcmfpL3AlpmIPv6DstTable");
    stage->tbls.compress_alpm_tbl[2] = compress_alpm_tbl;
    if (stage->compress_ip_addr_only[2] == TRUE) {
        compress_alpm_tbl->sid = L3_IPV6_COMP_DSTt;
        num_key_fids = 2;
        num_mask_fids = num_key_fids;
        size = (num_key_fids * sizeof(bcmltd_fid_t));
        BCMFP_ALLOC(key_fids, size, "bcmfpL3AlpmIPv6DstTableKeyFids");
        compress_alpm_tbl->key_fids = key_fids;
        compress_alpm_tbl->num_key_fids = num_key_fids;
        size = (num_mask_fids * sizeof(bcmltd_fid_t));
        BCMFP_ALLOC(mask_fids, size, "bcmfpL3AlpmIPv6DstTableMaskFids");
        compress_alpm_tbl->mask_fids = mask_fids;
        compress_alpm_tbl->num_mask_fids = num_mask_fids;
        key_fids[0] = L3_IPV6_COMP_DSTt_IPV6u_LOWERf;
        key_fids[1] = L3_IPV6_COMP_DSTt_IPV6u_UPPERf;
        mask_fids[0] = L3_IPV6_COMP_DSTt_IPV6u_LOWER_MASKf;
        mask_fids[1] = L3_IPV6_COMP_DSTt_IPV6u_UPPER_MASKf;
        compress_alpm_tbl->cid_fid = L3_IPV6_COMP_DSTt_CLASS_IDf;
    } else {
        compress_alpm_tbl->sid = L3_IPV6_COMP_DSTt;
        num_key_fids = 6;
        num_mask_fids = num_key_fids;
        size = (num_key_fids * sizeof(bcmltd_fid_t));
        BCMFP_ALLOC(key_fids, size, "bcmfpL3AlpmIPv6DstTableKeyFids");
        compress_alpm_tbl->key_fids = key_fids;
        compress_alpm_tbl->num_key_fids = num_key_fids;
        size = (num_mask_fids * sizeof(bcmltd_fid_t));
        BCMFP_ALLOC(mask_fids, size, "bcmfpL3AlpmIPv6DstTableMaskFids");
        compress_alpm_tbl->mask_fids = mask_fids;
        compress_alpm_tbl->num_mask_fids = num_mask_fids;
        key_fids[0] = L3_IPV6_COMP_DSTt_IPV6u_LOWERf;
        key_fids[1] = L3_IPV6_COMP_DSTt_IPV6u_UPPERf;
        key_fids[2] = L3_IPV6_COMP_DSTt_VRFf;
        key_fids[3] = L3_IPV6_COMP_DSTt_L4_PORTf;
        key_fids[4] = L3_IPV6_COMP_DSTt_IP_PROTOCOLf;
        key_fids[5] = L3_IPV6_COMP_DSTt_TCP_FLAGSf;
        mask_fids[0] = L3_IPV6_COMP_DSTt_IPV6u_LOWER_MASKf;
        mask_fids[1] = L3_IPV6_COMP_DSTt_IPV6u_UPPER_MASKf;
        mask_fids[2] = L3_IPV6_COMP_DSTt_VRF_MASKf;
        mask_fids[3] = L3_IPV6_COMP_DSTt_L4_PORT_MASKf;
        mask_fids[4] = L3_IPV6_COMP_DSTt_IP_PROTOCOL_MASKf;
        mask_fids[5] = L3_IPV6_COMP_DSTt_TCP_FLAGS_MASKf;
        compress_alpm_tbl->cid_fid = L3_IPV6_COMP_DSTt_CLASS_IDf;
    }
exit:
    SHR_FUNC_EXIT();
}
