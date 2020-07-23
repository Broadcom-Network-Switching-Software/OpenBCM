/*! \file bcm56780_a0_fp_ing_alpm_fids.c
 *
 * API to initialize stage attributes for
 * Trident4-X9(56780_A0) device.
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
#include <bcmfp/chip/bcm56780_a0_fp.h>
#include <bcmdrd/chip/bcm56780_a0_enum.h>
#include <bcmcfg/bcmcfg_lt.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmlrd/bcmlrd_map.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

int
bcm56780_a0_fp_ing_alpm_fids_init(int unit, bcmfp_stage_t *stage)
{
    uint64_t value = 0;
    bcmltd_fid_t field;
    bcmltd_sid_t ltid;
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
        stage->compress_ip_addr_only[0] = TRUE;
    } else {
        stage->compress_ip_addr_only[0] = FALSE;
    }

    value = 0;
    field = FP_CONFIGt_FP_ING_COMP_DST_IP4_ONLYf;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcfg_field_get(unit, FP_CONFIGt, field, &value));
    if (value) {
        stage->compress_ip_addr_only[1] = TRUE;
    } else {
        stage->compress_ip_addr_only[1] = FALSE;
    }

    value = 0;
    field = FP_CONFIGt_FP_ING_COMP_SRC_IP6_ONLYf;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcfg_field_get(unit, FP_CONFIGt, field, &value));
    if (value) {
        stage->compress_ip_addr_only[2] = TRUE;
    } else {
        stage->compress_ip_addr_only[2] = FALSE;
    }

    value = 0;
    field = FP_CONFIGt_FP_ING_COMP_DST_IP6_ONLYf;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcfg_field_get(unit, FP_CONFIGt, field, &value));
    if (value) {
        stage->compress_ip_addr_only[3] = TRUE;
    } else {
        stage->compress_ip_addr_only[3] = FALSE;
    }

    /* Source IPV4 L3 ALPM FIDs */
    size = sizeof(bcmfp_tbl_compress_alpm_t);
    BCMFP_ALLOC(compress_alpm_tbl, size, "bcmfpL3AlpmIPv4SrcTable");
    stage->tbls.compress_alpm_tbl[0] = compress_alpm_tbl;
    ltid = bcmlrd_table_name_to_idx(unit, "L3_IPV4_COMP_SRC");
    compress_alpm_tbl->sid = ltid;
    if (stage->compress_ip_addr_only[0] == TRUE) {
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
        SHR_IF_ERR_EXIT
            (bcmlrd_field_name_to_idx(unit, ltid, "IPV4", &field));
        key_fids[0] = field;
        SHR_IF_ERR_EXIT
            (bcmlrd_field_name_to_idx(unit, ltid, "IPV4_MASK", &field));
        mask_fids[0] = field;
        SHR_IF_ERR_EXIT
            (bcmlrd_field_name_to_idx(unit, ltid, "CLASS_ID", &field));
        compress_alpm_tbl->cid_fid = field;
    } else {
        num_key_fids = 2;
        num_mask_fids = num_key_fids;
        size = (num_key_fids * sizeof(bcmltd_fid_t));
        BCMFP_ALLOC(key_fids, size, "bcmfpL3AlpmIPv4SrcTableKeyFids");
        compress_alpm_tbl->key_fids = key_fids;
        compress_alpm_tbl->num_key_fids = num_key_fids;
        size = (num_mask_fids * sizeof(bcmltd_fid_t));
        BCMFP_ALLOC(mask_fids, size, "bcmfpL3AlpmIPv4SrcTableMaskFids");
        compress_alpm_tbl->mask_fids = mask_fids;
        compress_alpm_tbl->num_mask_fids = num_mask_fids;
        SHR_IF_ERR_EXIT
            (bcmlrd_field_name_to_idx(unit, ltid, "IPV4", &field));
        key_fids[0] = field;
        SHR_IF_ERR_EXIT
            (bcmlrd_field_name_to_idx(unit, ltid, "IPV4_MASK", &field));
        mask_fids[0] = field;
        SHR_IF_ERR_EXIT
            (bcmlrd_field_name_to_idx(unit, ltid, "CLASS_ID", &field));
        compress_alpm_tbl->cid_fid = field;
        SHR_IF_ERR_EXIT
            (bcmlrd_field_name_to_idx(unit, ltid, "L4_PORT", &field));
        key_fids[1] = field;
        SHR_IF_ERR_EXIT
            (bcmlrd_field_name_to_idx(unit, ltid, "L4_PORT_MASK", &field));
        mask_fids[1] = field;
    }

    compress_alpm_tbl = NULL;
    num_key_fids = 0;
    num_mask_fids = 0;
    key_fids = NULL;
    mask_fids = NULL;
    /* Destination IPV4 L3 ALPM FIDs */
    size = sizeof(bcmfp_tbl_compress_alpm_t);
    BCMFP_ALLOC(compress_alpm_tbl, size, "bcmfpL3AlpmIPv4DstTable");
    stage->tbls.compress_alpm_tbl[1] = compress_alpm_tbl;
    ltid = bcmlrd_table_name_to_idx(unit, "L3_IPV4_COMP_DST");
    compress_alpm_tbl->sid = ltid;
    if (stage->compress_ip_addr_only[1] == TRUE) {
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
        SHR_IF_ERR_EXIT
            (bcmlrd_field_name_to_idx(unit, ltid, "IPV4", &field));
        key_fids[0] = field;
        SHR_IF_ERR_EXIT
            (bcmlrd_field_name_to_idx(unit, ltid, "IPV4_MASK", &field));
        mask_fids[0] = field;
        SHR_IF_ERR_EXIT
            (bcmlrd_field_name_to_idx(unit, ltid, "CLASS_ID", &field));
        compress_alpm_tbl->cid_fid = field;
    } else {
        num_key_fids = 2;
        num_mask_fids = num_key_fids;
        size = (num_key_fids * sizeof(bcmltd_fid_t));
        BCMFP_ALLOC(key_fids, size, "bcmfpL3AlpmIPv4DstTableKeyFids");
        compress_alpm_tbl->key_fids = key_fids;
        compress_alpm_tbl->num_key_fids = num_key_fids;
        size = (num_mask_fids * sizeof(bcmltd_fid_t));
        BCMFP_ALLOC(mask_fids, size, "bcmfpL3AlpmIPv4DstTableMaskFids");
        compress_alpm_tbl->mask_fids = mask_fids;
        compress_alpm_tbl->num_mask_fids = num_mask_fids;
        SHR_IF_ERR_EXIT
            (bcmlrd_field_name_to_idx(unit, ltid, "IPV4", &field));
        key_fids[0] = field;
        SHR_IF_ERR_EXIT
            (bcmlrd_field_name_to_idx(unit, ltid, "IPV4_MASK", &field));
        mask_fids[0] = field;
        SHR_IF_ERR_EXIT
            (bcmlrd_field_name_to_idx(unit, ltid, "CLASS_ID", &field));
        compress_alpm_tbl->cid_fid = field;
        SHR_IF_ERR_EXIT
            (bcmlrd_field_name_to_idx(unit, ltid, "L4_PORT", &field));
        key_fids[1] = field;
        SHR_IF_ERR_EXIT
            (bcmlrd_field_name_to_idx(unit, ltid, "L4_PORT_MASK", &field));
        mask_fids[1] = field;
    }

    compress_alpm_tbl = NULL;
    num_key_fids = 0;
    num_mask_fids = 0;
    key_fids = NULL;
    mask_fids = NULL;
    /* Source IPV6 L3 ALPM FIDs */
    size = sizeof(bcmfp_tbl_compress_alpm_t);
    BCMFP_ALLOC(compress_alpm_tbl, size, "bcmfpL3AlpmIPv6SrcTable");
    stage->tbls.compress_alpm_tbl[2] = compress_alpm_tbl;
    ltid = bcmlrd_table_name_to_idx(unit, "L3_IPV6_COMP_SRC");
    compress_alpm_tbl->sid = ltid;
    if (stage->compress_ip_addr_only[2] == TRUE) {
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
        SHR_IF_ERR_EXIT
            (bcmlrd_field_name_to_idx(unit, ltid, "IPV6_LOWER", &field));
        key_fids[0] = field;
        SHR_IF_ERR_EXIT
            (bcmlrd_field_name_to_idx(unit, ltid, "IPV6_LOWER_MASK", &field));
        mask_fids[0] = field;
        SHR_IF_ERR_EXIT
            (bcmlrd_field_name_to_idx(unit, ltid, "IPV6_UPPER", &field));
        key_fids[1] = field;
        SHR_IF_ERR_EXIT
            (bcmlrd_field_name_to_idx(unit, ltid, "IPV6_UPPER_MASK", &field));
        mask_fids[1] = field;
        SHR_IF_ERR_EXIT
            (bcmlrd_field_name_to_idx(unit, ltid, "CLASS_ID", &field));
        compress_alpm_tbl->cid_fid = field;
    } else {
        num_key_fids = 3;
        num_mask_fids = num_key_fids;
        size = (num_key_fids * sizeof(bcmltd_fid_t));
        BCMFP_ALLOC(key_fids, size, "bcmfpL3AlpmIPv6SrcTableKeyFids");
        compress_alpm_tbl->key_fids = key_fids;
        compress_alpm_tbl->num_key_fids = num_key_fids;
        size = (num_mask_fids * sizeof(bcmltd_fid_t));
        BCMFP_ALLOC(mask_fids, size, "bcmfpL3AlpmIPv6SrcTableMaskFids");
        compress_alpm_tbl->mask_fids = mask_fids;
        compress_alpm_tbl->num_mask_fids = num_mask_fids;
        SHR_IF_ERR_EXIT
            (bcmlrd_field_name_to_idx(unit, ltid, "IPV6_LOWER", &field));
        key_fids[0] = field;
        SHR_IF_ERR_EXIT
            (bcmlrd_field_name_to_idx(unit, ltid, "IPV6_LOWER_MASK", &field));
        mask_fids[0] = field;
        SHR_IF_ERR_EXIT
            (bcmlrd_field_name_to_idx(unit, ltid, "IPV6_UPPER", &field));
        key_fids[1] = field;
        SHR_IF_ERR_EXIT
            (bcmlrd_field_name_to_idx(unit, ltid, "IPV6_UPPER_MASK", &field));
        mask_fids[1] = field;
        SHR_IF_ERR_EXIT
            (bcmlrd_field_name_to_idx(unit, ltid, "CLASS_ID", &field));
        compress_alpm_tbl->cid_fid = field;
        SHR_IF_ERR_EXIT
            (bcmlrd_field_name_to_idx(unit, ltid, "L4_PORT", &field));
        key_fids[2] = field;
        SHR_IF_ERR_EXIT
            (bcmlrd_field_name_to_idx(unit, ltid, "L4_PORT_MASK", &field));
        mask_fids[2] = field;
    }

    compress_alpm_tbl = NULL;
    num_key_fids = 0;
    num_mask_fids = 0;
    key_fids = NULL;
    mask_fids = NULL;
    /* Destination IPV6 L3 ALPM FIDs */
    size = sizeof(bcmfp_tbl_compress_alpm_t);
    BCMFP_ALLOC(compress_alpm_tbl, size, "bcmfpL3AlpmIPv6DstTable");
    stage->tbls.compress_alpm_tbl[3] = compress_alpm_tbl;
    ltid = bcmlrd_table_name_to_idx(unit, "L3_IPV6_COMP_DST");
    compress_alpm_tbl->sid = ltid;
    if (stage->compress_ip_addr_only[3] == TRUE) {
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
        SHR_IF_ERR_EXIT
            (bcmlrd_field_name_to_idx(unit, ltid, "IPV6_LOWER", &field));
        key_fids[0] = field;
        SHR_IF_ERR_EXIT
            (bcmlrd_field_name_to_idx(unit, ltid, "IPV6_LOWER_MASK", &field));
        mask_fids[0] = field;
        SHR_IF_ERR_EXIT
            (bcmlrd_field_name_to_idx(unit, ltid, "IPV6_UPPER", &field));
        key_fids[1] = field;
        SHR_IF_ERR_EXIT
            (bcmlrd_field_name_to_idx(unit, ltid, "IPV6_UPPER_MASK", &field));
        mask_fids[1] = field;
        SHR_IF_ERR_EXIT
            (bcmlrd_field_name_to_idx(unit, ltid, "CLASS_ID", &field));
        compress_alpm_tbl->cid_fid = field;
    } else {
        num_key_fids = 3;
        num_mask_fids = num_key_fids;
        size = (num_key_fids * sizeof(bcmltd_fid_t));
        BCMFP_ALLOC(key_fids, size, "bcmfpL3AlpmIPv6DstTableKeyFids");
        compress_alpm_tbl->key_fids = key_fids;
        compress_alpm_tbl->num_key_fids = num_key_fids;
        size = (num_mask_fids * sizeof(bcmltd_fid_t));
        BCMFP_ALLOC(mask_fids, size, "bcmfpL3AlpmIPv6DstTableMaskFids");
        compress_alpm_tbl->mask_fids = mask_fids;
        compress_alpm_tbl->num_mask_fids = num_mask_fids;
        SHR_IF_ERR_EXIT
            (bcmlrd_field_name_to_idx(unit, ltid, "IPV6_LOWER", &field));
        key_fids[0] = field;
        SHR_IF_ERR_EXIT
            (bcmlrd_field_name_to_idx(unit, ltid, "IPV6_LOWER_MASK", &field));
        mask_fids[0] = field;
        SHR_IF_ERR_EXIT
            (bcmlrd_field_name_to_idx(unit, ltid, "IPV6_UPPER", &field));
        key_fids[1] = field;
        SHR_IF_ERR_EXIT
            (bcmlrd_field_name_to_idx(unit, ltid, "IPV6_UPPER_MASK", &field));
        mask_fids[1] = field;
        SHR_IF_ERR_EXIT
            (bcmlrd_field_name_to_idx(unit, ltid, "CLASS_ID", &field));
        compress_alpm_tbl->cid_fid = field;
        SHR_IF_ERR_EXIT
            (bcmlrd_field_name_to_idx(unit, ltid, "L4_PORT", &field));
        key_fids[2] = field;
        SHR_IF_ERR_EXIT
            (bcmlrd_field_name_to_idx(unit, ltid, "L4_PORT_MASK", &field));
        mask_fids[2] = field;
    }
exit:
    SHR_FUNC_EXIT();
}
