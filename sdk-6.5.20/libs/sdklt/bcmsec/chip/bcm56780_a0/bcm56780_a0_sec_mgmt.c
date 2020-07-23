/*! \file bcm56780_a0_sec_mgmt.c
 *
 * File containing management related defines and internal function for
 * bcm56780_a0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmbd/bcmbd.h>

#include <bcmdrd/bcmdrd_types.h>

#include <bcmsec/bcmsec_types.h>
#include <bcmsec/chip/bcm56780_a0_sec.h>
#include <bcmdrd/chip/bcm56780_a0_enum.h>
#include <bcmsec/bcmsec_pt_internal.h>
#include <bcmsec/imm/bcmsec_imm_defines_internal.h>
#include <bcmsec/imm/bcmsec_imm_defines_internal.h>
#include "bcm56780_a0_sec_mgmt.h"

#define BSL_LOG_MODULE  BSL_LS_BCMSEC_MGMT
/*******************************************************************************
 * Local definitions
 */
/*******************************************************************************
 * Private functions
 */
/*******************************************************************************
 * Public functions
 */
int
bcm56780_a0_sec_decrypt_mgmt(int unit, bcmltd_sid_t ltid,
                             bcmsec_decrypt_mgmt_cfg_t *decrypt_mgmt_cfg)
{
    uint32_t blk_id;
    bcmdrd_fid_t fid_etype[] = {
        ETYPE_0f, ETYPE_1f, ETYPE_2f, ETYPE_3f,
        };
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    bcmsec_pt_info_t pt_info = {0};
    uint32_t ltmbuf[BCMSEC_MAX_ENTRY_WSIZE] = {0};
    uint32_t fval_arr[2] = {0}, fval[1];
    int i;
    uint64_t flags = 0;

    SHR_FUNC_ENTER(unit);

    blk_id = decrypt_mgmt_cfg->blk_id;

    /* SEC_DECRYPT_MGMTt_DST_MACf */
    sid = ISEC_MGMTRULE_CFG_DAr;
    fid = DAf;
    for (i = 0; i < 8; i++) {
        BCMSEC_PT_DYN_INFO(pt_info, i, blk_id, flags);
        fval_arr[0] = (uint32_t)decrypt_mgmt_cfg->dst_mac[i];
        fval_arr[1] = (uint32_t)(decrypt_mgmt_cfg->dst_mac[i] >> 32);
        SHR_IF_ERR_EXIT
            (bcmsec_field_set(unit, sid, fid, ltmbuf, fval_arr));
        SHR_IF_ERR_EXIT
            (bcmsec_pt_indexed_write(unit, sid, ltid, &pt_info, ltmbuf));
    }

    /* SEC_DECRYPT_MGMTt_ETHERTYPEf */
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    for (i = 0; i < 8; i++) {
        BCMSEC_PT_DYN_INFO(pt_info, i / 4, blk_id, flags);
        sid = ISEC_MGMTRULE_CFG_ETYPEr;
        SHR_IF_ERR_EXIT
            (bcmsec_field_set(unit, sid, fid_etype[i % 4], ltmbuf,
                              &decrypt_mgmt_cfg->ethertype[i]));
        SHR_IF_ERR_EXIT
            (bcmsec_pt_indexed_write(unit, sid, ltid, &pt_info, ltmbuf));
    }

    /* SEC_DECRYPT_MGMTt_DST_MAC_RANGE_LOWf */
    BCMSEC_PT_DYN_INFO(pt_info, 0, blk_id, flags);
    sid = ISEC_MGMTRULE_DA_RANGE_LOWr;
    fid = DAf;
    fval_arr[0] = (uint32_t)decrypt_mgmt_cfg->dst_mac_range_low;
    fval_arr[1] = (uint32_t)(decrypt_mgmt_cfg->dst_mac_range_low >> 32);
    SHR_IF_ERR_EXIT
        (bcmsec_field_set(unit, sid, fid, ltmbuf, fval_arr));
    SHR_IF_ERR_EXIT
        (bcmsec_pt_indexed_write(unit, sid, ltid, &pt_info, ltmbuf));

    /* SEC_DECRYPT_MGMTt_DST_MAC_RANGE_HIGHf */
    sid = ISEC_MGMTRULE_DA_RANGE_HIGHr;
    fid = DAf;
    fval_arr[0] = (uint32_t)decrypt_mgmt_cfg->dst_mac_range_high;
    fval_arr[1] = (uint32_t)(decrypt_mgmt_cfg->dst_mac_range_high >> 32);
    SHR_IF_ERR_EXIT
        (bcmsec_field_set(unit, sid, fid, ltmbuf, fval_arr));
    SHR_IF_ERR_EXIT
        (bcmsec_pt_indexed_write(unit, sid, ltid, &pt_info, ltmbuf));

    /*
     * SEC_DECRYPT_MGMTt_DST_MAC_GROUP_0f and
     * SEC_DECRYPT_MGMTt_ETHERTYPE_GROUP_0f
     */
    sid = ISEC_MGMTRULE_DA_ETYPE_1STr;
    fid = DAf;
    fval_arr[0] = (uint32_t)decrypt_mgmt_cfg->dst_mac_group_0;
    fval_arr[1] = (uint32_t)(decrypt_mgmt_cfg->dst_mac_group_0 >> 32);
    SHR_IF_ERR_EXIT
        (bcmsec_field_set(unit, sid, fid, ltmbuf, fval_arr));
    fid = ETYPEf;
    fval[0] = decrypt_mgmt_cfg->ethertype_group_0;
    SHR_IF_ERR_EXIT
        (bcmsec_field_set(unit, sid, fid, ltmbuf,
                          fval));
    SHR_IF_ERR_EXIT
        (bcmsec_pt_indexed_write(unit, sid, ltid, &pt_info, ltmbuf));

    /*
     * SEC_DECRYPT_MGMTt_DST_MAC_GROUP_1f and
     * SEC_DECRYPT_MGMTt_ETHERTYPE_GROUP_1f
     */
    sid = ISEC_MGMTRULE_DA_ETYPE_2NDr;
    fid = DAf;
    fval_arr[0] = (uint32_t)decrypt_mgmt_cfg->dst_mac_group_1;
    fval_arr[1] = (uint32_t)(decrypt_mgmt_cfg->dst_mac_group_1 >> 32);
    SHR_IF_ERR_EXIT
        (bcmsec_field_set(unit, sid, fid, ltmbuf, fval_arr));
    fid = ETYPEf;
    SHR_IF_ERR_EXIT
        (bcmsec_field_set(unit, sid, fid, ltmbuf,
                          &decrypt_mgmt_cfg->ethertype_group_1));
    SHR_IF_ERR_EXIT
        (bcmsec_pt_indexed_write(unit, sid, ltid, &pt_info, ltmbuf));

exit:
    SHR_FUNC_EXIT();
}
