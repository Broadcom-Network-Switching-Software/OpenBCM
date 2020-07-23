/*! \file ser_config.c
 *
 * Interface functions for SER
 *
 * This file contains the initialization code of SER.
 * This file also contains SER code for public use
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmdrd_config.h>
#include <shr/shr_error.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmptm/bcmptm_ser_internal.h>
#include <sal/sal_sleep.h>
#include <bcmptm/bcmptm_scor_internal.h>
#include <bcmdrd/bcmdrd_pt_ser.h>
#include <bcmptm/bcmptm_chip_internal.h>
#include <shr/shr_ha.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmptm/bcmptm_internal.h>
#include <bcmissu/issu_api.h>

#include <bcmptm/generated/bcmptm_ser_local.h>

#include "ser_config.h"
#include "ser_intr_dispatch.h"
#include "ser_tcam.h"
#include "ser_bd.h"

/******************************************************************************
 * Local definitions
 */
#define  BSL_LOG_MODULE  BSL_LS_BCMPTM_SER

/*!
  * \brief max number of interrupt register
  */
#define MAX_NUM_INTR_REGS        12

/*! Used to call routines in CTH sub-component to update/get data to/from IMM LTs */
bcmptm_ser_cth_drv_t *bcmptm_ser_cth_drv = NULL;

/*!\brief Information needs to be recovered from HA memory space during Warmboot */
static bcmptm_ser_sinfo_t *bcmptm_ser_ha_info[BCMDRD_CONFIG_MAX_UNITS];

/******************************************************************************
 * Private Functions
 */
/*
 * Clear PTs which can't be cleared during blocks(IP/EP/MMU etc.) reset.
 * This operation should be done before SER enablement.
 */
static int
bcmptm_ser_mem_clear(int unit)
{
    bcmptm_ser_mem_clr_list_t *mem_list = NULL;
    int  num_mem = 0;
    int  rv = SHR_E_NONE;
    int  num_insts = 0, i = 0, inst = -1;
    const uint32_t *default_entry_data = NULL;
    uint32_t entry_data[BCMDRD_MAX_PT_WSIZE], ent_size;
    int flag = 0, copy_num = 0, index = 0;
    bool cache_en = FALSE, valid = FALSE, dma_mode = 0;
    int index_num = 0;
    bcmdrd_sid_t sid = INVALIDm;

    SHR_FUNC_ENTER(unit);

    rv = bcmptm_ser_mem_clr_list_get(unit, &mem_list, &num_mem);
    if (rv == SHR_E_UNAVAIL) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Device need not to clear MMU "
                              "memory table specifically\n")));
        rv = SHR_E_NONE;
        SHR_ERR_EXIT(rv);
    }
    SHR_IF_ERR_EXIT(rv);

    /* Those MMU memory buffer must be cleared, because its data can be cleared when MMU block is reset */
    for (i = 0; i < num_mem; i++) {
        sid = mem_list[i].mem_sid;
        index = mem_list[i].index;

        rv = bcmptm_ser_tbl_inst_num_get(unit, sid, &num_insts, &copy_num);
        SHR_IF_ERR_EXIT(rv);
        /* DMA mode */
        dma_mode = (index < 0) ? 1 : 0;
        if (dma_mode) {
            if (PT_IS_PORT_OR_SW_PORT_REG(unit, sid)) {
                index_num = num_insts;
                num_insts = 1;
            } else {
                index_num = bcmptm_scor_pt_index_count(unit, sid);
            }
        } else {
            default_entry_data = bcmdrd_pt_default_value_get(unit, sid);
            ent_size = bcmdrd_pt_entry_size(unit, sid);
            if (default_entry_data != NULL) {
                sal_memcpy(entry_data, default_entry_data, ent_size);
            } else {
                sal_memset(entry_data, 0, ent_size);
            }
            rv = bcmptm_pt_attr_is_cacheable(unit, sid, &cache_en);
            SHR_IF_ERR_EXIT(rv);

            flag = (cache_en) ? BCMPTM_SCOR_REQ_FLAGS_ALSO_WRITE_CACHE : 0;
        }
        for (inst = 0; inst < num_insts; inst++) {
            valid = bcmdrd_pt_index_valid(unit, sid, inst, index);
            if (!valid) {
                continue;
            }
            if (dma_mode) {
                rv = bcmptm_ser_mem_dma_clear(unit, sid, inst, index_num);
                SHR_IF_ERR_EXIT(rv);
            } else {
                rv = bcmptm_ser_pt_write(unit, sid, index,
                                         inst, -1, entry_data, flag);
                SHR_IF_ERR_EXIT(rv);
            }
        }
    }

exit:
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Fail to clear MMU memory tables\n")));
    }
    SHR_FUNC_EXIT();
}

/*
 * Configure some registers after SER enablement.
 */
static int
bcmptm_ser_mem_init_after_ser_enable(int unit)
{
    bcmptm_ser_ctrl_reg_info_t *regs_ctrl_info = NULL;
    int reg_num = 0;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    rv = bcmptm_ser_clear_mem_after_ser_enable(unit, &regs_ctrl_info, &reg_num);
    if (rv == SHR_E_UNAVAIL) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Device may need to clear "
                             "memory tables after enabling SER checking\n")));
        SHR_EXIT();
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_ser_ctrl_reg_operate(unit, regs_ctrl_info, reg_num);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Fail to init memory tables"
                             " after enabling SER checking\n")));
    }
    SHR_FUNC_EXIT();
}

/*
 * Configure some registers before SER enablement.
 */
static int
bcmptm_ser_mem_init_before_ser_enable(int unit)
{
    bcmptm_ser_ctrl_reg_info_t *regs_ctrl_info = NULL;
    int  reg_num = 0;
    int  rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    rv = bcmptm_ser_clear_mem_before_ser_enable(unit, &regs_ctrl_info, &reg_num);
    if (rv == SHR_E_UNAVAIL) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Device may need to clear "
                             "memory tables before enabling SER checking\n")));
        rv = SHR_E_NONE;
        SHR_ERR_EXIT(rv);
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_ser_ctrl_reg_operate(unit, regs_ctrl_info, reg_num);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Fail to init memory tables"
                             " before enabling SER checking\n")));
    }
    SHR_FUNC_EXIT();
}

/*
 * Enable DROP_PKT for some PTs.
 */
static void
bcmptm_ser_pkt_drop_ctrl_reg_enable(int unit)
{
    bcmdrd_pt_ser_en_ctrl_t en_ctrl;
    bcmdrd_fid_t fids[2] = {INVALIDf, INVALIDf};
    bcmdrd_sid_t *ctrl_reg_mute_list = NULL;
    int ctrl_reg_mute_num = 0, i, rv;
    size_t sid_count = 0;
    bcmdrd_sid_t sid, ctrl_sid;

    /* skip appointed memory tables */
    rv = bcmptm_ser_mute_drop_pkt_reg_list_get
        (unit, &ctrl_reg_mute_list, &ctrl_reg_mute_num);
    if (rv == SHR_E_UNAVAIL) {
        return;
    }
    rv = bcmptm_pt_sid_count_get(unit, &sid_count);
    if (SHR_FAILURE(rv)) {
        return;
    }
    for (sid = 0; sid < sid_count; sid++) {
        rv = bcmdrd_pt_ser_drop_pkt_ctrl_get(unit, sid, &en_ctrl);
        if (SHR_FAILURE(rv)) {
            continue;
        }
        ctrl_sid = en_ctrl.sid;
        for (i = 0; i < ctrl_reg_mute_num; i++) {
            if (ctrl_sid == ctrl_reg_mute_list[i]) {
                break;
            }
        }
        /* Found */
        if (i < ctrl_reg_mute_num) {
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Skip to enable DROP_PKT of PT [%s]\n"),
                       bcmdrd_pt_sid_to_name(unit, sid)));
            continue;
        }
        fids[0] = en_ctrl.fid;
        rv = bcmptm_ser_reg_multi_fields_modify(unit, ctrl_sid, 0, fids,
                                                1, NULL, NULL);
        if (SHR_SUCCESS(rv)) {
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Enable DROP_PKT of PT [%s]\n"),
                       bcmdrd_pt_sid_to_name(unit, sid)));
        }
    }
}

/*
 * Enable or disable SER check and SER ECC single bit reporting for all PTs.
 */
static void
bcmptm_ser_pts_ser_enable(int unit)
{
    bcmdrd_sid_t sid;
    int rv = SHR_E_NONE, enable, enable_1bit;
    size_t sid_count = 0;
    uint32_t enable_single_bit;
    bcmptm_ser_cth_drv_t *cth_drv = bcmptm_ser_cth_drv_get();
    bcmdrd_sid_t *mem_ser_mute_list = NULL;
    int mem_ser_mute_num = 0, i;

    rv = bcmptm_pt_sid_count_get(unit, &sid_count);
    if (SHR_FAILURE(rv)) {
        return;
    }
    enable_single_bit = cth_drv->ctrl_fld_val_get
        (unit, SER_CONTROLt_REPORT_SINGLE_BIT_ECCf);
    /* skip appointed memory tables */
    (void)bcmptm_ser_mute_mem_list_get(unit, &mem_ser_mute_list, &mem_ser_mute_num);

    for (sid = 0; sid < sid_count; sid++) {
        for (i = 0; i < mem_ser_mute_num; i++) {
            if (sid == mem_ser_mute_list[i]) {
                break;
            }
        }
        /* Found */
        if (i < mem_ser_mute_num) {
            continue;
        }
        rv = bcmptm_ser_pt_ser_enable(unit, sid, BCMDRD_SER_EN_TYPE_EC, 1);
        enable = SHR_FAILURE(rv) ? 0 : 1;
        if (rv == SHR_E_UNAVAIL) {
            continue;
        }
        enable_1bit = 0;
        if (enable_single_bit) {
            bcmptm_ser_pt_ser_enable(unit, sid, BCMDRD_SER_EN_TYPE_ECC1B, 1);
            enable_1bit = SHR_FAILURE(rv) ? 0 : 1;
        }
        (void)cth_drv->pt_ctrl_imm_insert(unit, sid, enable, enable_1bit);
    }
}

/*
 * Enable or disable SER check or SER ECC single bit reporting
 * for all internal buffers or buses.
 */
static void
bcmptm_ser_all_buffer_bus_enable(int unit)
{
    bcmdrd_ser_rmr_id_t rid;
    int rv = SHR_E_NONE;
    size_t num_rid = 0, i;
    bcmdrd_ser_rmr_id_t *rid_list = NULL;

    rv = bcmdrd_pt_ser_rmr_id_list_get(unit, 0, NULL, &num_rid);
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                           "Fail to get information of controlling register from drd\n")));
        return;
    }
    rid_list = sal_alloc(sizeof(bcmdrd_ser_rmr_id_t) * (num_rid + 1), "bcmptmSerRmrIds");
    if (rid_list == NULL) {
        LOG_WARN(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                           "Fail to allocate memory space\n")));
        return;
    }

    (void)bcmdrd_pt_ser_rmr_id_list_get(unit, num_rid, rid_list, &num_rid);

    for (i = 0; i < num_rid; i++) {
        rid = rid_list[i];

        (void)bcmptm_ser_buf_bus_enable(unit, rid, BCMDRD_SER_EN_TYPE_EC, 1);
        (void)bcmptm_ser_buf_bus_enable(unit, rid, BCMDRD_SER_EN_TYPE_ECC1B, 1);
    }
    sal_free(rid_list);
}

/*
 * Enable or disable SER check of all blocks.
 */
static int
bcmptm_ser_blocks_enable(int unit, int enable)
{
    bcmptm_ser_ctrl_en_t *ctrl_regs = NULL, *cur_ctrl_reg;
    int i = 0, ctrl_item_num = 0, rv = SHR_E_NONE;
    bcmdrd_fid_t field_list[2] = {INVALIDf, INVALIDf};
    bcmdrd_sid_t en_reg = INVALIDr;

    SHR_FUNC_ENTER(unit);

    rv = bcmptm_ser_blk_ctrl_regs_get(unit, &ctrl_regs, &ctrl_item_num);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    for (i = 0; i < ctrl_item_num; i++) {
        cur_ctrl_reg = &ctrl_regs[i];

        en_reg = cur_ctrl_reg->enable_reg;
        field_list[0] = cur_ctrl_reg->enable_field;
        if (field_list[0] != INVALIDf) {
            rv = bcmptm_ser_reg_multi_fields_modify(unit, en_reg, 0, field_list,
                                                    enable ? 1 : 0, NULL, NULL);
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        } else if (cur_ctrl_reg->enable_field_list != NULL) {
            rv = bcmptm_ser_reg_multi_fields_modify(unit, en_reg, 0,
                                                    cur_ctrl_reg->enable_field_list,
                                                    enable ? 1 : 0, NULL, NULL);
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    }

exit:
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Fail to enable ser blocks\n")));
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public Functions
 */
int
bcmptm_ser_tcam_hw_scan_init(int unit, int enable)
{
    bcmptm_ser_ctrl_reg_info_t *regs_ctrl_info = NULL;
    int  reg_num = 0;
    int  rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    rv = bcmptm_ser_tcam_hw_scan_reg_info_get(unit, enable,
                                              &regs_ctrl_info, &reg_num);
    if (rv == SHR_E_UNAVAIL) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Device may need not to enable IDB SER scan\n")));
        rv = SHR_E_NONE;
        SHR_ERR_EXIT(rv);
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_ser_ctrl_reg_operate(unit, regs_ctrl_info, reg_num);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Fail to start IDB SER scan\n")));
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_pt_clear(int unit, int flags)
{
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    if (flags & PT_CLEAR_BEFORE_SER_ENABLE) {
        rv = bcmptm_ser_mem_clear(unit);
        SHR_IF_ERR_VERBOSE_EXIT(rv);

        rv = bcmptm_ser_mem_init_before_ser_enable(unit);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    if (flags & PT_CLEAR_AFTER_SER_ENABLE) {
        rv = bcmptm_ser_mem_init_after_ser_enable(unit);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_ser_ha_info_init(int unit, bool warm)
{
    uint32_t alloc_size = sizeof(bcmptm_ser_sinfo_t);

    bcmptm_ser_ha_info[unit] = shr_ha_mem_alloc(unit, BCMMGMT_SER_COMP_ID,
                                                BCMPTM_HA_SUBID_SER_MISC_INFO,
                                                "serCfgInfo",
                                                &alloc_size);
    if (bcmptm_ser_ha_info[unit] == NULL ||
        alloc_size < sizeof(bcmptm_ser_sinfo_t)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to allocate HA memory space for SER.\n")));
        return SHR_E_MEMORY;
    }
    if (!warm) {
        sal_memset(bcmptm_ser_ha_info[unit], 0, alloc_size);
        return bcmissu_struct_info_report(unit,
                                          BCMMGMT_SER_COMP_ID,
                                          BCMPTM_HA_SUBID_SER_MISC_INFO,
                                          0, sizeof(bcmptm_ser_sinfo_t), 1,
                                          BCMPTM_SER_SINFO_T_ID);
    } else {
        return SHR_E_NONE;
    }
}

bcmptm_ser_sinfo_t *
bcmptm_ser_ha_info_get(int unit)
{
    return bcmptm_ser_ha_info[unit];
}

bool
bcmptm_ser_log_overlaid_get(int unit)
{
    bcmptm_ser_sinfo_t *ha_info = bcmptm_ser_ha_info_get(unit);

    return ha_info->ser_log_overlaid;
}

void
bcmptm_ser_log_overlaid_set(int unit, bool ovrlaid)
{
    bcmptm_ser_sinfo_t *ha_info = bcmptm_ser_ha_info_get(unit);

    ha_info->ser_log_overlaid = ovrlaid;
}

int
bcmptm_ser_tcam_single_bit_auto_correct_enable(int unit, int enable)
{
    bcmdrd_fid_t field_list[2] = {INVALIDf, INVALIDf};
    bcmdrd_sid_t *tcam_ctrl_list = NULL;
    int tcam_ctrl_num = 0, i;
    int rv = SHR_E_NONE;

    bcmptm_ser_tcam_ctrl_reg_list_get(unit, &tcam_ctrl_list, &tcam_ctrl_num, &field_list[0]);

    for (i = 0; i < tcam_ctrl_num; i++) {
        rv = bcmptm_ser_reg_multi_fields_modify(unit, tcam_ctrl_list[i], 0, field_list,
                                                enable ? 1 : 0, NULL, NULL);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Fail to %s auto correction for tcam single"
                                  "bit. SID:%s\n"),
                       enable ? "enable": "disable",
                       bcmdrd_pt_sid_to_name(unit, tcam_ctrl_list[i])));
            return rv;
        }
    }

    return rv;
}

int
bcmptm_ser_comp_config(int unit, bool warm)
{
    int rv = SHR_E_NONE;
    bool enable = SER_CHECKING_ENABLE;
    bcmptm_ser_cth_drv_t *cth_drv = bcmptm_ser_cth_drv_get();

    SHR_FUNC_ENTER(unit);

    enable = cth_drv->ser_check_enable(unit);
    if (enable == SER_CHECKING_DISABLE) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "SER function is disabled\n")));
        SHR_EXIT();
    }
    if (warm) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "SER Warmboot!!\n")));
        rv = bcmptm_ser_tcam_cmic_scan_init(unit, warm);
        /* Exit */
        SHR_ERR_EXIT(rv);
    }

    /* cold boot */
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Welcome to SER configuration\n")));
    rv = bcmptm_pt_clear(unit, PT_CLEAR_BEFORE_SER_ENABLE);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    /* enable parity or ecc checking of memory tables, registers */
    bcmptm_ser_pts_ser_enable(unit);

    bcmptm_ser_pkt_drop_ctrl_reg_enable(unit);

    rv = bcmptm_ser_tcam_single_bit_auto_correct_enable(unit, 1);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_ser_tcam_cmic_scan_init(unit, warm);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    /* enable parity or ecc checking of buffers and buses */
    bcmptm_ser_all_buffer_bus_enable(unit);

    rv = bcmptm_ser_blocks_enable(unit, 1);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_pt_clear(unit, PT_CLEAR_AFTER_SER_ENABLE);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_ser_tcam_hw_scan_init(unit, 1);
    SHR_IF_ERR_VERBOSE_EXIT(rv);
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_ser_pt_ser_enable(int unit, bcmdrd_sid_t sid,
                         bcmdrd_ser_en_type_t ctrl_type, int enable)
{
    int rv = SHR_E_NONE;
    bcmdrd_pt_ser_en_ctrl_t ctrl_info;
    bool is_tcam = 0;
    bcmdrd_fid_t field_list[2];

    SHR_FUNC_ENTER(unit);

    sal_memset(&ctrl_info, 0, sizeof(ctrl_info));
    is_tcam = bcmdrd_pt_attr_is_cam(unit, sid);

    if (is_tcam && ctrl_type == BCMDRD_SER_EN_TYPE_EC) {
        ctrl_type = BCMDRD_SER_EN_TYPE_TCAM;
    }
    rv = bcmdrd_pt_ser_en_ctrl_get(unit, ctrl_type, sid, &ctrl_info);

    if (SHR_FAILURE(rv)) {
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              " memory[%s] may not have capability "
                              "of [type==%d] parity checking\n"),
                   bcmdrd_pt_sid_to_name(unit, sid), ctrl_type));
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    field_list[0] = ctrl_info.fid;
    field_list[1] = INVALIDf;
    rv = bcmptm_ser_reg_multi_fields_modify(unit, ctrl_info.sid, 0, field_list,
                                            enable ? 1 : 0, NULL, NULL);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if (SHR_FAILURE(rv) && (SHR_E_UNAVAIL != rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to %s memory [%s] parity or ecc checking\n"),
                   enable ? "enable": "disable",
                   bcmdrd_pt_sid_to_name(unit, sid)));
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_ser_buf_bus_enable(int unit, bcmdrd_ser_rmr_id_t rid,
                          bcmdrd_ser_en_type_t ctrl_type, int enable)
{
    int rv = SHR_E_NONE;
    bcmdrd_pt_ser_en_ctrl_t ctrl_info;
    bcmdrd_fid_t field_list[2];

    SHR_FUNC_ENTER(unit);

    sal_memset(&ctrl_info, 0, sizeof(ctrl_info));

    rv = bcmdrd_pt_ser_rmr_en_ctrl_get(unit, ctrl_type, rid, &ctrl_info);
     if (SHR_FAILURE(rv)) {
         LOG_VERBOSE(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "Fail to get buffer or bus [%d]"
                                 " controlling information\n"), rid));
         SHR_IF_ERR_VERBOSE_EXIT(rv);
     }
     rv = bcmdrd_pt_is_valid(unit, ctrl_info.sid);
     if (SHR_FAILURE(rv)) {
         LOG_WARN(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Controlling SID [%d] is invalid\n"),
                   ctrl_info.sid));
         SHR_IF_ERR_VERBOSE_EXIT(rv);
     }
     LOG_DEBUG(BSL_LOG_MODULE,
               (BSL_META_U(unit,
                           "[%s], its control type [%d],"
                           " control register [%s], control field [%d]\n"),
                bcmdrd_pt_ser_rmr_to_name(unit, rid), ctrl_type,
                bcmdrd_pt_sid_to_name(unit, ctrl_info.sid), ctrl_info.fid));

     field_list[0] = ctrl_info.fid;
     field_list[1] = INVALIDf;
     rv = bcmptm_ser_reg_multi_fields_modify(unit, ctrl_info.sid, 0, field_list,
                                             enable ? 1 : 0, NULL, NULL);
     if (SHR_FAILURE(rv)) {
         LOG_WARN(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to write controlling register\n")));
         SHR_IF_ERR_VERBOSE_EXIT(rv);
     }

exit:
    SHR_FUNC_EXIT();
}

void
bcmptm_ser_cth_drv_register(bcmptm_ser_cth_drv_t *cth_drv_fn)
{
    bcmptm_ser_cth_drv = cth_drv_fn;
}

bcmptm_ser_cth_drv_t *
bcmptm_ser_cth_drv_get(void)
{
    return bcmptm_ser_cth_drv;
}

