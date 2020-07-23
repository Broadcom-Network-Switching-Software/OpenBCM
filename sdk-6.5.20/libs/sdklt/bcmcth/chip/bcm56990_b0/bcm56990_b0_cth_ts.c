/*! \file bcm56990_b0_cth_ts.c
 *
 * Chip stub for BCMCTH timesync control.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_time.h>
#include <shr/shr_debug.h>
#include <bcmbd/bcmbd_ts.h>
#include <bcmcth/bcmcth_ts_drv.h>
#include <bcmcth/bcmcth_util.h>
#include <bcmcth/bcmcth_imm_util.h>
#include <bcmdrd/chip/bcm56990_b0_enum.h>
#include <bcmdrd/chip/bcm56990_b0_defs.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmpc/bcmpc_lport.h>
#include <bcmptm/bcmptm.h>

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_TIMESYNC

/*******************************************************************************
 * Private Functions
 */
static void
bcm56990_b0_cth_pt_op_info_t_init(bcmcth_pt_op_info_t *pt_op_info)
{
    if (pt_op_info) {
        pt_op_info->drd_sid = BCM56990_B0_ENUM_COUNT;
        pt_op_info->trans_id = -1;
        pt_op_info->req_lt_sid = BCM56990_B0_ENUM_COUNT;
        pt_op_info->rsp_lt_sid = BCM56990_B0_ENUM_COUNT;
        pt_op_info->req_flags = BCMPTM_REQ_FLAGS_NO_FLAGS;
        pt_op_info->rsp_flags = BCMPTM_REQ_FLAGS_NO_FLAGS;
        pt_op_info->op = BCMPTM_OP_NOP;
        pt_op_info->dyn_info.index = -1;
        pt_op_info->dyn_info.tbl_inst = -1;
        pt_op_info->wsize = 0;
    }
    return;

}

static int
bcm56990_b0_cth_synce_ctrl_validate(int unit,
                                    ts_synce_ctrl_t *ctrl)
{
    int lport, pport;

    lport = ctrl->port;
    pport = bcmpc_lport_to_pport(unit, lport);
    if (pport == BCMPC_INVALID_PPORT) {
        ctrl->pm_port = SYNCE_INVALID_PM_PORT;
    } else {
        /*
         * [2:0] lanes per PM8x50 = (physical port -1) % 4 * 2
         * [5:3] PM8x50 per pm_grp = [(physical port -1) / 4] % 8
         * [8:6] pm_grp = (physical port -1) / 32
         */
        pport = pport -1;
        ctrl->pm_port = (pport % 4) * 2 |
                        ((pport / 4) % 8) << 3 |
                        (pport / 32) << 6;
    }
    return SHR_E_NONE;
}

static int
bcm56990_b0_cth_synce_ctrl_update(int unit,
                                  bcmltd_sid_t ltid,
                                  const bcmltd_op_arg_t *op_arg,
                                  int ctrl_num,
                                  ts_synce_ctrl_t *ctrl,
                                  bool override)
{
    bcmcth_pt_op_info_t pt_op_info;
    uint32_t *buf_ptr = NULL;
    TOP_SYNCE_CTRLr_t buf;
    ts_synce_ctrl_t *control;
    int pm_port, i;

    SHR_FUNC_ENTER(unit);
    sal_memset(&buf, 0, sizeof(buf));
    buf_ptr = (uint32_t *)&buf;

    bcm56990_b0_cth_pt_op_info_t_init(&pt_op_info);
    pt_op_info.drd_sid = TOP_SYNCE_CTRLr;
    pt_op_info.trans_id = op_arg->trans_id;
    pt_op_info.req_lt_sid = ltid;
    pt_op_info.req_flags =
        bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);
    pt_op_info.op = BCMPTM_OP_READ;
    pt_op_info.wsize = bcmdrd_pt_entry_wsize(unit, TOP_SYNCE_CTRLr);
    pt_op_info.dyn_info.index = 0;
    pt_op_info.dyn_info.tbl_inst = 0;
    SHR_IF_ERR_EXIT
       (bcmcth_pt_read(unit, &pt_op_info, buf_ptr));

    for (i = 0; i < ctrl_num; i++) {
        control = &ctrl[i];
        if (!control->update) {
            continue;
        }
        if (control->pm_port == SYNCE_INVALID_PM_PORT) {
            pm_port = 0;
        } else {
            pm_port = control->pm_port;
        }
        if (control->instance == 0) {
            TOP_SYNCE_CTRLr_L1_RCVD_SW_OVWR_VALIDf_SET(buf,
                                                       control->ovr_valid);
            TOP_SYNCE_CTRLr_L1_RCVD_FREQ_SELf_SET(buf, control->clk_divisor);
            TOP_SYNCE_CTRLr_L1_RCVD_PORT_SELf_SET(buf, pm_port);
        }
        if (control->instance == 1) {
            TOP_SYNCE_CTRLr_L1_RCVD_SW_OVWR_BKUP_VALIDf_SET(buf,
                                                            control->ovr_valid);
            TOP_SYNCE_CTRLr_L1_RCVD_BKUP_FREQ_SELf_SET(buf,
                                                       control->clk_divisor);
            TOP_SYNCE_CTRLr_L1_RCVD_BKUP_PORT_SELf_SET(buf, pm_port);
        }
    }

    TOP_SYNCE_CTRLr_L1_RCVD_SW_OVWR_ENf_SET(buf, override);

    bcm56990_b0_cth_pt_op_info_t_init(&pt_op_info);
    pt_op_info.drd_sid = TOP_SYNCE_CTRLr;
    pt_op_info.trans_id = op_arg->trans_id;
    pt_op_info.req_lt_sid = ltid;
    pt_op_info.req_flags =
        bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);
    pt_op_info.op = BCMPTM_OP_WRITE;
    pt_op_info.wsize = bcmdrd_pt_entry_wsize(unit, TOP_SYNCE_CTRLr);
    pt_op_info.dyn_info.index = 0;
    pt_op_info.dyn_info.tbl_inst = 0;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,"PT write 0x%x\n"),
                 TOP_SYNCE_CTRLr_GET(buf)));
    SHR_IF_ERR_EXIT
        (bcmcth_pt_write(unit, &pt_op_info, buf_ptr));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_cth_synce_ctrl_enable(int unit,
                                  bcmltd_sid_t ltid,
                                  const bcmltd_op_arg_t *op_arg,
                                  uint8_t inst_map,
                                  bool enable)
{
    bcmcth_pt_op_info_t pt_op_info;
    uint32_t *buf_ptr = NULL;
    TOP_SYNCE_CTRLr_t buf;
    int i;

    SHR_FUNC_ENTER(unit);
    sal_memset(&buf, 0, sizeof(buf));
    buf_ptr = (uint32_t *)&buf;

    bcm56990_b0_cth_pt_op_info_t_init(&pt_op_info);
    pt_op_info.drd_sid = TOP_SYNCE_CTRLr;
    pt_op_info.trans_id = op_arg->trans_id;
    pt_op_info.req_lt_sid = ltid;
    pt_op_info.req_flags =
        bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);
    pt_op_info.op = BCMPTM_OP_READ;
    pt_op_info.wsize = bcmdrd_pt_entry_wsize(unit, TOP_SYNCE_CTRLr);
    pt_op_info.dyn_info.index = 0;
    pt_op_info.dyn_info.tbl_inst = 0;
    SHR_IF_ERR_EXIT
       (bcmcth_pt_read(unit, &pt_op_info, buf_ptr));

    for (i = 0; i < SYNCE_MAX_CLK_TYPE; i++) {
        if (inst_map & 1 << i) {
            if (i == 0) {
                TOP_SYNCE_CTRLr_L1_RCVD_CLK_RSTNf_SET(buf, enable);
            }
            if (i == 1) {
                TOP_SYNCE_CTRLr_L1_RCVD_CLK_BKUP_RSTNf_SET(buf, enable);
            }
        }
    }

    bcm56990_b0_cth_pt_op_info_t_init(&pt_op_info);
    pt_op_info.drd_sid = TOP_SYNCE_CTRLr;
    pt_op_info.trans_id = op_arg->trans_id;
    pt_op_info.req_lt_sid = ltid;
    pt_op_info.req_flags =
        bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);
    pt_op_info.op = BCMPTM_OP_WRITE;
    pt_op_info.wsize = bcmdrd_pt_entry_wsize(unit, TOP_SYNCE_CTRLr);
    pt_op_info.dyn_info.index = 0;
    pt_op_info.dyn_info.tbl_inst = 0;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,"PT write 0x%x\n"),
                 TOP_SYNCE_CTRLr_GET(buf)));
    SHR_IF_ERR_EXIT
        (bcmcth_pt_write(unit, &pt_op_info, buf_ptr));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_cth_tod_auto(
    int unit,
    bcmltd_sid_t ltid,
    ts_tod_entry_t *entry)
{
    uint32_t entry_sz;
    uint32_t *entry_buf = NULL;
    bcmdrd_sid_t sid[2] = {EGR_TS_UTC_CONVERSIONm, EGR_TS_UTC_CONVERSION_2m};
    uint64_t timestamp = 0;
    uint32_t val[2] = {0};
    uint8_t i;
    int rv, ts_inst = 1;
    bool enable = entry->hw_update;
    bool update_sec = false, update_nsec = false;

    SHR_FUNC_ENTER(unit);

    entry_sz = bcmdrd_pt_entry_wsize(unit, sid[0]);
    entry_sz *= 4;
    SHR_ALLOC(entry_buf, entry_sz, "bcmcthTsTodData");
    SHR_NULL_CHECK(entry_buf, SHR_E_MEMORY);

    if (enable) {
        /* Get TS1 counter value */
        rv = bcmbd_ts_timestamp_nanosec_get(unit, ts_inst, &timestamp);
        if (rv == SHR_E_DISABLED) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit, "Timestamp counter is not enabled\n")));
        }
        if (SHR_BITGET(entry->fbmp, DEVICE_TS_TODt_TOD_SECf)) {
            update_sec = true;
        }
        if (SHR_BITGET(entry->fbmp, DEVICE_TS_TODt_TOD_NSECf)) {
            update_nsec = true;
        }
    }

    for (i = 0; i < 2; i++) {
        sal_memset(entry_buf, 0, entry_sz);
        SHR_IF_ERR_EXIT
            (bcmcth_imm_pipe_ireq_read(unit, ltid, sid[i], -1, 0, entry_buf));

        if (enable) {
            /* Update to HW */
            val[1] = 0;
            val[0] = 1;
            bcmdrd_pt_field_set
                (unit, sid[i], entry_buf, HW_UPDATE_ENf, val);
            val[1] = timestamp >> 32;
            val[0] = timestamp & 0xFFFFFFFF;
            bcmdrd_pt_field_set
                (unit, sid[i], entry_buf, TIMESTAMP_TA_NSECf, val);
            /* Update sec and nsec from user configured value */
            if (update_sec) {
                val[1] = entry->sec >> 32;
                val[0] = entry->sec & 0xFFFFFFFF;
                bcmdrd_pt_field_set
                    (unit, sid[i], entry_buf, TIMESTAMP_TOD_SECf, val);
            }
            if (update_nsec) {
                val[1] = 0;
                val[0] = entry->nsec;
                bcmdrd_pt_field_set
                    (unit, sid[i], entry_buf, TIMESTAMP_TOD_NSECf, val);
            }
        } else {
            val[1] = val[0]= 0;
            bcmdrd_pt_field_set(unit, sid[i], entry_buf, HW_UPDATE_ENf, val);
        }
        SHR_IF_ERR_EXIT
            (bcmcth_imm_pipe_ireq_write(unit, ltid, sid[i], -1, 0, entry_buf));
    }

exit:
    SHR_FREE(entry_buf);
    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_cth_tod_update(
    int unit,
    bcmltd_sid_t ltid,
    ts_tod_entry_t *entry)
{
    uint32_t entry_sz;
    uint32_t *entry_buf = NULL;
    bcmdrd_sid_t sid[2] = {EGR_TS_UTC_CONVERSIONm, EGR_TS_UTC_CONVERSION_2m};
    uint32_t value[2];
    uint8_t i;

    SHR_FUNC_ENTER(unit);

    if (entry->hw_update) {
        SHR_EXIT() ;
    }
    entry_sz = bcmdrd_pt_entry_wsize(unit, sid[0]);
    entry_sz *= 4;
    SHR_ALLOC(entry_buf, entry_sz, "bcmcthTsTodData");
    SHR_NULL_CHECK(entry_buf, SHR_E_MEMORY);

    for (i = 0; i < 2; i++) {
        sal_memset(entry_buf, 0, entry_sz);
        SHR_IF_ERR_EXIT
            (bcmcth_imm_pipe_ireq_read(unit, ltid, sid[i], -1, 0, entry_buf));
        value[1] = entry->sec >> 32;
        value[0] = entry->sec & 0xFFFFFFFF;
        bcmdrd_pt_field_set
            (unit, sid[i], entry_buf, TIMESTAMP_TOD_SECf, value);
        value[1] = 0;
        value[0] = entry->nsec;
        bcmdrd_pt_field_set
            (unit, sid[i], entry_buf, TIMESTAMP_TOD_NSECf, value);
        value[1] = entry->adjust >> 32;
        value[0] = entry->adjust & 0xFFFFFFFF;
        bcmdrd_pt_field_set
            (unit, sid[i], entry_buf, TIMESTAMP_TA_NSECf, value);
        SHR_IF_ERR_EXIT
            (bcmcth_imm_pipe_ireq_write(unit, ltid, sid[i], -1, 0, entry_buf));
    }
exit:
    if (entry_buf) {
        SHR_FREE(entry_buf);
    }
    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_cth_tod_get(
    int unit,
    bcmltd_sid_t ltid,
    ts_tod_entry_oper_t *entry)
{
    uint32_t entry_sz;
    uint32_t *entry_buf = NULL;
    bcmdrd_sid_t sid = EGR_TS_UTC_CONVERSIONm;
    uint32_t value[2] = {0};
    uint32_t val1[2] = {0};
    uint32_t hw_en[2] = {0};

    SHR_FUNC_ENTER(unit);
    entry_sz = bcmdrd_pt_entry_wsize(unit, sid);
    entry_sz *= 4;
    SHR_ALLOC(entry_buf, entry_sz, "bcmcthTsTodData");
    SHR_NULL_CHECK(entry_buf, SHR_E_MEMORY);
    sal_memset(entry_buf, 0, entry_sz);
    SHR_IF_ERR_EXIT
        (bcmcth_imm_pipe_ireq_read(unit, ltid, sid, -1, 0, entry_buf));

    bcmdrd_pt_field_get
            (unit, sid, entry_buf, HW_UPDATE_ENf, hw_en);
    if (hw_en[0]) {
        /*
         * If HW_UPDATE enabled, issue another read to check if the hw operation
         * is successful or not.
         */
        bcmdrd_pt_field_get
            (unit, sid, entry_buf, TIMESTAMP_TA_NSECf, val1);
        sal_memset(entry_buf, 0, entry_sz);
        SHR_IF_ERR_EXIT
            (bcmcth_imm_pipe_ireq_read(unit, ltid, sid, -1, 0, entry_buf));
        bcmdrd_pt_field_get
            (unit, sid, entry_buf, TIMESTAMP_TA_NSECf, value);
        entry->adjust = (uint64_t)value[1] << 32 | value[0];
        if (!entry->adjust || val1[1] != value[1] || val1[0] != value[0] ) {
            entry->update_fail = true;
        }
    } else {
        bcmdrd_pt_field_get
            (unit, sid, entry_buf, TIMESTAMP_TA_NSECf, value);
        entry->adjust = (uint64_t)value[1] << 32 | value[0];
    }

    bcmdrd_pt_field_get
        (unit, sid, entry_buf, TIMESTAMP_TOD_NSECf, value);
    entry->nsec = value[0];
    bcmdrd_pt_field_get
        (unit, sid, entry_buf, TIMESTAMP_TOD_SECf, value);
    entry->sec = (uint64_t)value[1] << 32 | value[0];

exit:
    SHR_FREE(entry_buf);
    SHR_FUNC_EXIT();
}

static bcmcth_ts_synce_drv_t bcm56990_b0_cth_synce_drv = {
    .synce_ctrl_enable = &bcm56990_b0_cth_synce_ctrl_enable,
    .synce_ctrl_update = &bcm56990_b0_cth_synce_ctrl_update,
    .synce_ctrl_validate = &bcm56990_b0_cth_synce_ctrl_validate
};

static bcmcth_ts_tod_drv_t bcm56990_b0_cth_tod_drv = {
    .tod_auto = &bcm56990_b0_cth_tod_auto,
    .tod_update = &bcm56990_b0_cth_tod_update,
    .tod_get = &bcm56990_b0_cth_tod_get,
};

static bcmcth_ts_drv_t bcm56990_b0_cth_ts_drv = {
    .synce_drv = &bcm56990_b0_cth_synce_drv,
    .tod_drv = &bcm56990_b0_cth_tod_drv
};

/*******************************************************************************
 * Public functions
 */
bcmcth_ts_drv_t *
bcm56990_b0_cth_ts_drv_get(int unit)
{
    return &bcm56990_b0_cth_ts_drv;
}
