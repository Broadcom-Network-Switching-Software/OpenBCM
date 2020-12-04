/*! \file ser_testutil.c
 *
 * Functions for SER test.
 *
 * APIs for SER test.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmptm/bcmptm_ser_internal.h>
#include <bcmptm/bcmptm_ser_testutil.h>
#include <bcmptm/bcmptm_scor_internal.h>
#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <bcmcfg/bcmcfg_lt.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <sal/sal_time.h>

#include "ser_correct.h"

/******************************************************************************
 * Local definitions
 */
#define  BSL_LOG_MODULE  BSL_LS_BCMPTM_SER

/******************************************************************************
 * Private Functions
 */

/*
 * TCAM mode including unique mode and wide mode
 * can be configured during initialization.
 * This routine is used to get and save the mode of the TCAMs.
 */
static bool
bcmptm_ser_testutil_tcam_mode_check(int unit, bcmptm_ser_tcam_generic_info_t *tcam_info)
{
    uint64_t fld_val;
    int rv = SHR_E_NONE;

    if (tcam_info->cfg_lt_sid == 0xffffffff ||
        tcam_info->cfg_lt_fid == 0xffffffff) {
        return FALSE;
    }
    rv = bcmcfg_field_get(unit, tcam_info->cfg_lt_sid,
                          tcam_info->cfg_lt_fid, &fld_val);
    if (SHR_FAILURE(rv) || (uint32_t)fld_val != 1) {
        return FALSE;
    }
    return TRUE;
}

/******************************************************************************
 * Public Functions
 */
bool
bcmptm_ser_testutil_pt_inst_remap(int unit, bcmdrd_sid_t sid,
                                  int *tbl_inst, int *tbl_copy)
{
    return bcmptm_ser_pt_inst_remap(unit, sid, tbl_inst, tbl_copy);
}

bool
bcmptm_ser_testutil_pt_copy_no_valid(int unit, bcmdrd_sid_t sid,
                                     int tbl_inst, int copy_no)
{
    return bcmptm_ser_pt_copy_no_valid(unit, sid, tbl_inst, copy_no);
}

int
bcmptm_ser_testutil_index_valid(int unit, bcmdrd_sid_t sid,
                                int index, int *max_index)
{
    return bcmptm_ser_sram_index_valid(unit, sid, index, max_index);
}

int
bcmptm_ser_testutil_blk_type_map(int unit, int blk_type,
                                 int *blk_type_overlay, int *ser_blk_type)
{
    return bcmptm_ser_blk_type_map(unit, blk_type, blk_type_overlay, ser_blk_type);
}

void
bcmptm_ser_testutil_mem_remap(int unit, bcmdrd_sid_t sid, int index,
                              int inject, bcmdrd_sid_t *remap_sid,
                              int *remap_index, bool *shadow_table)
{
    int inject_err_to_key_fld = 0, rv, ecc_checking;
    bcmptm_ser_correct_info_t spci;

    *remap_sid = sid;
    *remap_index = index;
    rv = bcmptm_ser_mem_ecc_info_get(unit, sid, index, remap_sid,
                                     remap_index, &ecc_checking);
    if (SHR_SUCCESS(rv) && (sid != *remap_sid)) {
        return;
    }
    /* Map to injected view */
    rv = bcmptm_ser_tcam_remap(unit, sid, remap_sid, &inject_err_to_key_fld);
    if (SHR_SUCCESS(rv)) {
        sid = *remap_sid;
        /* don't exit */
    }
    if (inject) {
        return;
    }
    /* Remap to corrected view */
    *remap_index = index;
    rv = bcmptm_ser_sid_remap(unit, sid, remap_sid, remap_index);
    if (SHR_SUCCESS(rv) && (sid != *remap_sid)) {
        /* exit */
        return;
    }
    sal_memset(&spci, 0, sizeof(spci));
    spci.sid[0] = sid;
    rv = bcmptm_ser_mmu_mem_remap(unit, &spci, 0, 0);
    if (SHR_SUCCESS(rv) && (sid != spci.sid[0])) {
        *remap_sid = spci.sid[0];
        if (shadow_table) {
            *shadow_table = (spci.flags & BCMPTM_SER_FLAG_ERR_IN_SHADOW) ? 1: 0;
        }
    }
    return;
}

int
bcmptm_ser_testutil_pt_read(int unit, bcmdrd_sid_t  sid,
                            int index, int tbl_inst, int tbl_copy,
                            uint32_t *data, size_t wsize, bool hw)
{
    int flag = hw ? BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE : 0;

    return bcmptm_ser_pt_read(unit, sid, index, tbl_inst, tbl_copy,
                              data, wsize, flag);
}

int
bcmptm_ser_testutil_pt_hw_write(int unit, bcmdrd_sid_t sid,
                                int index, int tbl_inst, int tbl_copy,
                                uint32_t *data, bool hw_only)
{
    int flag = hw_only ? 0 : BCMPTM_SCOR_REQ_FLAGS_ALSO_WRITE_CACHE;

    return bcmptm_ser_pt_write(unit, sid, index, tbl_inst, tbl_copy, data, flag);
}

void
bcmptm_ser_testutil_blk_parity_ctrl_validate(int unit)
{
    bcmdrd_fid_t fid, *fid_list;
    bcmdrd_sid_t en_reg = INVALIDr;
    bcmptm_ser_ctrl_en_t *ctrl_regs = NULL, *cur_ctrl_reg;
    int i = 0, ctrl_item_num = 0, rv = SHR_E_NONE, j;

    rv = bcmptm_ser_blk_ctrl_regs_get(unit, &ctrl_regs, &ctrl_item_num);
    if (SHR_FAILURE(rv)) {
        return;
    }
    for (i = 0; i < ctrl_item_num; i++) {
        cur_ctrl_reg = &ctrl_regs[i];
        en_reg = cur_ctrl_reg->enable_reg;
        fid = cur_ctrl_reg->enable_field;
        if (fid != INVALIDf) {
            rv = bcmptm_ser_testutil_ctrl_reg_validate(unit, en_reg, fid);
        } else {
            fid_list = cur_ctrl_reg->enable_field_list;
            for (j = 0; ; j++) {
                fid = fid_list[j];
                if (fid == INVALIDf) {
                    break;
                }
                rv = bcmptm_ser_testutil_ctrl_reg_validate(unit, en_reg, fid);
                if (SHR_FAILURE(rv)) {
                    break;
                }
            }
        }
        if (SHR_FAILURE(rv)) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "ERR: control register[%s] is not enabled.\n"),
                      bcmdrd_pt_sid_to_name(unit, en_reg)));
        }
    }
}

int
bcmptm_ser_testutil_ctrl_reg_validate(int unit, bcmdrd_sid_t sid, bcmdrd_fid_t fid)
{
    int rv = SHR_E_NONE;
    int inst_num, copy_num, index_num;
    int inst = 0, copy, index = 0;
    int blktype;
    uint32_t pipe_map;
    uint32_t entry_data[BCMDRD_MAX_PT_WSIZE], fval, valid_fval;
    const bcmdrd_symbols_t *symbols;
    bcmdrd_sym_field_info_t finfo;

    SHR_FUNC_ENTER(unit);

    rv = bcmptm_ser_tbl_inst_num_get(unit, sid, &inst_num, &copy_num);
    SHR_IF_ERR_EXIT(rv);

    rv = bcmptm_ser_sram_index_valid(unit, sid, 0, &index_num);
    index_num += 1;
    SHR_IF_ERR_EXIT(rv);

    (void)bcmptm_ser_expected_value_get(unit, sid, fid, &valid_fval);

    blktype = bcmdrd_pt_blktype_get(unit, sid, 0);
    (void)bcmdrd_dev_valid_blk_pipes_get(unit, 0, blktype, &pipe_map);
    for (copy = -1; copy < copy_num; copy++) {
        for (inst = 0; inst < inst_num; inst++) {
            if (!(pipe_map & (1 << inst))) {
                continue;
            }
            for (index = 0; index < index_num; index++) {
                if (!bcmdrd_pt_index_valid(unit, sid, inst, index)) {
                    continue;
                }
                rv = bcmptm_ser_pt_read(unit, sid, index, inst, copy,
                                        entry_data, BCMDRD_MAX_PT_WSIZE,
                                        BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE);
                SHR_IF_ERR_EXIT(rv);

                rv = bcmdrd_pt_field_get(unit, sid, entry_data, fid, &fval);
                SHR_IF_ERR_EXIT(rv);

                if (valid_fval != fval) {
                    rv = SHR_E_FAIL;
                    SHR_IF_ERR_EXIT(rv);
                }
            }
        }
    }

exit:
    if (SHR_FAILURE(rv)) {
        symbols = bcmdrd_dev_symbols_get(unit, 0);
        (void)bcmdrd_sym_field_info_get(symbols, sid, fid, &finfo);
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Fail to check field[%s] of control register [%s], inst [%d] index[%d].\n"),
                  finfo.name, bcmdrd_pt_sid_to_name(unit, sid), inst, index));
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_ser_testutil_refresh_regs_disable(int unit, int disable)
{
    return bcmptm_ser_refresh_regs_config(unit, disable);
}

bool
bcmptm_ser_testutil_pt_acctype_is_addr_split(int unit, bcmdrd_sid_t pt_id)
{
    const char *acctype_str = NULL;
    uint32_t acc_type = 0;

    (void)bcmptm_ser_pt_acctype_get(unit, pt_id, &acc_type, &acctype_str);
    if (PT_IS_ADDR_SPLIT_ACC_TYPE(acctype_str)) {
        return TRUE;
    } else {
        return FALSE;
    }
}

bool
bcmptm_ser_testutil_pt_acctype_is_data_split(int unit, bcmdrd_sid_t pt_id)
{
    const char *acctype_str = NULL;
    uint32_t acc_type = 0;

    (void)bcmptm_ser_pt_acctype_get(unit, pt_id, &acc_type, &acctype_str);
    if (PT_IS_DATA_SPLIT_ACC_TYPE(acctype_str)) {
        return TRUE;
    } else {
        return FALSE;
    }
}

bool
bcmptm_ser_testutil_pt_acctype_is_addr_split_split(int unit, bcmdrd_sid_t pt_id)
{
    const char *acctype_str = NULL;
    uint32_t acc_type = 0;

    (void)bcmptm_ser_pt_acctype_get(unit, pt_id, &acc_type, &acctype_str);
    if (PT_IS_ADDR_SPLIT_SPLIT_ACC_TYPE(acctype_str)) {
        return TRUE;
    } else {
        return FALSE;
    }
}

int
bcmptm_ser_testutil_all_pts_enable(int unit, int enable)
{
    int rv = SHR_E_NONE;
    size_t sid, sid_count;

    SHR_FUNC_ENTER(unit);

    rv = bcmptm_pt_sid_count_get(unit, &sid_count);
    SHR_IF_ERR_EXIT(rv);

    for (sid = 0; sid < sid_count; sid++) {
        (void)bcmptm_ser_pt_ser_enable(unit, (bcmdrd_sid_t)sid,
                                       BCMDRD_SER_EN_TYPE_EC, enable);
        (void)bcmptm_ser_pt_ser_enable(unit, (bcmdrd_sid_t)sid,
                                       BCMDRD_SER_EN_TYPE_ECC1B, enable);
    }

exit:
    SHR_FUNC_EXIT();
}

void
bcmptm_ser_testutil_is_unique_mode(int unit, bcmdrd_sid_t sid, bool *unique_mode)
{
    bcmptm_ser_tcam_generic_info_t *tcam_ser_info = NULL;
    int tcam_info_num = 0, i = 0;
    const char *acctype_str = NULL;
    uint32_t acc_type = 0;

    if (bcmdrd_pt_attr_is_cam(unit, sid)) {
        (void)bcmptm_ser_tcam_info_get(unit, &tcam_ser_info, &tcam_info_num);
        for (i = 0; i < tcam_info_num; i++) {
            if (tcam_ser_info[i].mem == sid) {
                if (bcmptm_ser_testutil_tcam_mode_check(unit, &tcam_ser_info[i])) {
                    *unique_mode = 1;
                } else {
                    *unique_mode = 0;
                }
                break;
            }
        }
    } else {
        (void)bcmptm_ser_pt_acctype_get(unit, sid, &acc_type, &acctype_str);
        if (PT_IS_UNIQUE_ACC_TYPE(acctype_str) ||
            PT_IS_SINGLE_ACC_TYPE(acctype_str)) {
            *unique_mode = 1;
        } else {
            *unique_mode = 0;
        }
    }
}

void
bcmptm_ser_testutil_parity_field_clear(int unit, bcmdrd_sid_t sid,
                                       uint32_t *entry_data)
{
    bcmptm_ser_parity_field_clear(unit, sid, entry_data);
}

