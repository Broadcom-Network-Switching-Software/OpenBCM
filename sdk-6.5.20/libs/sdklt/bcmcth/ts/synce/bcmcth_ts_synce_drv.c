/*! \file bcmcth_ts_synce_drv.c
 *
 * SYNCE driver APIs
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <sal/sal_libc.h>
#include <bcmcth/bcmcth_ts_drv.h>
#include <bcmcth/bcmcth_ts_synce_drv.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmissu/issu_api.h>
#include <bcmmgmt/bcmmgmt_sysm.h>


/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_TIMESYNC

/*! TS synce info data structure */
typedef struct bcmcth_ts_synce_info_s {
    /*! Persistent info to be recovered from HA memory*/
    bcmcth_ts_synce_ha_t *ha;
    /*! clk configuration */
    ts_synce_ctrl_t ctrl[SYNCE_MAX_CLK_TYPE];
} bcmcth_ts_synce_info_t;

/*! SYNCE info */
static bcmcth_ts_synce_info_t *synce_info[BCMDRD_CONFIG_MAX_UNITS] = {0};

/*! Invalid port info */
#define INVALID_PORT_INFO BCMLTD_COMMON_SYNCE_CLK_STATE_T_T_PORT_INFO_UNAVAILABLE

/*******************************************************************************
 * Private Functions
 */

static int
synce_drv_ctrl_validate(int unit,
                        ts_synce_ctrl_t *ctrl)
{
    bcmcth_ts_synce_drv_t *synce_drv = bcmcth_ts_synce_drv_get(unit);

    if (synce_drv && synce_drv->synce_ctrl_validate) {
        return synce_drv->synce_ctrl_validate(unit, ctrl);
    }
    return SHR_E_NONE;
}

static int
synce_drv_ctrl_enable(int unit,
                      bcmltd_sid_t ltid,
                      const bcmltd_op_arg_t *op_arg,
                      uint8_t inst_map,
                      bool enable)
{
    bcmcth_ts_synce_drv_t *synce_drv = bcmcth_ts_synce_drv_get(unit);

    if (synce_drv && synce_drv->synce_ctrl_enable) {
        return synce_drv->synce_ctrl_enable
                (unit, ltid, op_arg, inst_map, enable);
    }
    return SHR_E_NONE;

}

static int
synce_drv_ctrl_update(int unit,
                      bcmltd_sid_t ltid,
                      const bcmltd_op_arg_t *op_arg,
                      int ctrl_num,
                      ts_synce_ctrl_t *ctrl,
                      bool override)
{
    bcmcth_ts_synce_drv_t *synce_drv = bcmcth_ts_synce_drv_get(unit);

    if (synce_drv && synce_drv->synce_ctrl_update) {
        return synce_drv->synce_ctrl_update
                (unit, ltid, op_arg, ctrl_num, ctrl, override);
    }
    return SHR_E_NONE;
}

static int
synce_ctrl_validate(int unit,
                    ts_synce_ctrl_entry_t *entry,
                    ts_synce_ctrl_entry_oper_t *oper)
{
    int i;
    bcmcth_ts_synce_info_t *info = synce_info[unit];
    ts_synce_ctrl_t *ctrl;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < SYNCE_MAX_CLK_TYPE; i++) {
        ctrl = &info->ctrl[i];
        sal_memset(ctrl, 0, sizeof(ts_synce_ctrl_t));
        ctrl->port = entry->port[i];
        ctrl->ovr_valid = entry->ovr_valid[i];
        ctrl->clk_divisor = entry->clk_divisor[i];
        ctrl->pm_port = SYNCE_INVALID_PM_PORT;
        ctrl->instance = i;
        SHR_IF_ERR_VERBOSE_EXIT
            (synce_drv_ctrl_validate(unit, ctrl));
        if (ctrl->pm_port == SYNCE_INVALID_PM_PORT) {
            oper->oper_stat[i] = INVALID_PORT_INFO;
        }
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "synce entry[%d] port %d clk_divisor %d "
                                "ovr_valid %d clk_recovery %d\n"),
                     i, entry->port[i], entry->clk_divisor[i],
                     entry->ovr_valid[i],entry->clk_recovery[i]));
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "synce oper[%d] port %d clk_divisor %d "
                                "ovr_valid %d oper_stat %d\n"),
                     i, oper->port[i], oper->clk_divisor[i],
                     oper->ovr_valid[i],oper->oper_stat[i]));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
synce_ctrl_update(int unit,
                  ts_synce_ctrl_entry_t *entry,
                  ts_synce_ctrl_entry_oper_t *oper)
{
    bcmcth_ts_synce_info_t *info = synce_info[unit];
    uint8_t inst_bmp;
    int i;
    bool prev, current;
    ts_synce_ctrl_t *ctrl;

    SHR_FUNC_ENTER(unit);
    inst_bmp = 0;

    for (i = 0; i < SYNCE_MAX_CLK_TYPE; i++) {
        ctrl = &info->ctrl[i];
        prev = info->ha->clk_recovery[i];
        current = entry->clk_recovery[i];
        info->ha->clk_recovery[i] = current;
        ctrl->update = false;
        if (!current) {
            if (prev ^ current) {
                /* The clk_recovery is from enabled to disabled. */
                SHR_IF_ERR_VERBOSE_EXIT
                    (synce_drv_ctrl_enable
                     (unit, entry->ltid, entry->op_arg, (1 << i), false));
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "clk_recovery[%d] 1->0\n"), i));
            }
        } else {
            if (prev ^ current) {
                /* The clk_recovery is from disabled to enabled. */
                inst_bmp |= 1 << i;
                ctrl->update = true;
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "clk_recovery[%d] 0->1\n"), i));

                /* Update oper when clk_recovery is enabled. */
                oper->clk_divisor[i] = entry->clk_divisor[i];
                if (oper->oper_stat[i] != INVALID_PORT_INFO) {
                    oper->port[i] = entry->port[i];
                    info->ha->port_oper[i] = entry->port[i];
                }
                oper->ovr_valid[i] = entry->ovr_valid[i];
                oper->override = entry->override;
                info->ha->clk_divisor_oper[i] = entry->clk_divisor[i];
                info->ha->ovr_valid_oper[i] = entry->ovr_valid[i];
                info->ha->override_oper = entry->override;
            } else {
                oper->clk_divisor[i] = info->ha->clk_divisor_oper[i];
                if (oper->oper_stat[i] != INVALID_PORT_INFO) {
                    oper->port[i] = info->ha->port_oper[i];
                }
                oper->ovr_valid[i] = info->ha->ovr_valid_oper[i];
                oper->override = info->ha->override_oper;
            }
        }
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "synce entry[%d] port %d clk_divisor %d "
                                "ovr_valid %d clk_recovery %d\n"),
                     i, entry->port[i], entry->clk_divisor[i],
                     entry->ovr_valid[i],entry->clk_recovery[i]));
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "synce oper[%d] port %d clk_divisor %d "
                                "ovr_valid %d oper_stat %d\n"),
                     i, oper->port[i], oper->clk_divisor[i],
                     oper->ovr_valid[i],oper->oper_stat[i]));
    }

    if (inst_bmp) {
        SHR_IF_ERR_VERBOSE_EXIT
            (synce_drv_ctrl_enable
             (unit, entry->ltid, entry->op_arg, inst_bmp, false));
        SHR_IF_ERR_VERBOSE_EXIT
            (synce_drv_ctrl_update(unit,
                                   entry->ltid,
                                   entry->op_arg,
                                   SYNCE_MAX_CLK_TYPE,
                                   info->ctrl,
                                   entry->override));
        SHR_IF_ERR_VERBOSE_EXIT
            (synce_drv_ctrl_enable
             (unit, entry->ltid, entry->op_arg, inst_bmp, true));
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
* Public functions
 */

int
bcmcth_ts_synce_sw_resources_alloc(int unit, bool warm)
{
    bcmcth_ts_synce_info_t *info = NULL;
    uint32_t ha_alloc_size = 0,ha_req_size = 0;

    SHR_FUNC_ENTER(unit);

    SHR_ALLOC(info, sizeof(bcmcth_ts_synce_info_t), "bcmcthTsSynceInfo");
    SHR_NULL_CHECK(info, SHR_E_MEMORY);
    sal_memset(info, 0, sizeof(bcmcth_ts_synce_info_t));
    synce_info[unit] = info;

    /* Alloc the HA synce info SW structure for this unit */
    ha_alloc_size = sizeof(bcmcth_ts_synce_ha_t);
    ha_req_size = ha_alloc_size;
    info->ha = shr_ha_mem_alloc(unit, BCMMGMT_TS_COMP_ID,
                                BCMTS_SYNCE_SUB_COMP_ID,
                                "bcmcthTsSynceData",
                                &ha_alloc_size);

    SHR_NULL_CHECK(info->ha, SHR_E_MEMORY);
    SHR_IF_ERR_VERBOSE_EXIT
        ((ha_alloc_size < ha_req_size) ? SHR_E_MEMORY : SHR_E_NONE);

    if (!warm) {
        sal_memset(info->ha, 0, ha_alloc_size);

        SHR_IF_ERR_VERBOSE_EXIT(
                bcmissu_struct_info_report(unit, BCMMGMT_MON_COMP_ID,
                                           BCMTS_SYNCE_SUB_COMP_ID, 0,
                                           ha_req_size, 1,
                                           BCMCTH_TS_SYNCE_HA_T_ID));
    }

exit:
    SHR_FUNC_EXIT();
}

void
bcmcth_ts_synce_free(int unit)
{
    if (synce_info[unit] == NULL) {
        return;
    }
    SHR_FREE(synce_info[unit]);
}

int
bcmcth_ts_synce_control_update(int unit,
                               ts_synce_ctrl_entry_t *entry,
                               ts_synce_ctrl_entry_oper_t *oper)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (synce_ctrl_validate(unit, entry, oper));

    SHR_IF_ERR_VERBOSE_EXIT
        (synce_ctrl_update(unit, entry, oper));
exit:
    SHR_FUNC_EXIT();
}

