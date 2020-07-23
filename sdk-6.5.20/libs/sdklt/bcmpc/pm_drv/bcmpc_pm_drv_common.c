/*! \file bcmpc_pm_drv_common.c
 *
 * PM common driver
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_util.h>

#include <bcmpc/bcmpc_util_internal.h>
#include <bcmpc/bcmpc_pm_drv_internal.h>
#include <bcmpc/bcmpc_topo_internal.h>
#include <bcmpc/bcmpc_pm.h>

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_PM

static int
pm_drv_get(int unit, pm_info_t *pm_info, bcmpc_pm_drv_t **pm_drv)
{
    SHR_FUNC_ENTER(unit);

    if ((pm_info->type == PM_TYPE_PM8X50) ||
        (pm_info->type == PM_TYPE_PM8X50_GEN2)) {
        *pm_drv = &bcmpc_pm_drv_pm8x50;
    } else if (pm_info->type == PM_TYPE_PM8X50_GEN3) {
        *pm_drv = &bcmpc_pm_drv_pm8x50_gen3;
    } else if (pm_info->type == PM_TYPE_PM4X10) {
        *pm_drv = &bcmpc_pm_drv_pm4x10;
    } else if (pm_info->type == PM_TYPE_PM8X100) {
        *pm_drv = &bcmpc_pm_drv_pm8x100;
    } else if (pm_info->type == PM_TYPE_PM8X100_GEN2) {
        *pm_drv = &bcmpc_pm_drv_pm8x100_dpll;
    } else {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Common PM driver function vector.
 */
static int
pm_port_lane_map_validate(int unit, pm_oper_status_t *op_st,
                          pm_info_t *pm_info, int lane_map)
{
    bcmpc_pm_drv_t *pm_drv = NULL;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->port_lane_map_validate) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->port_lane_map_validate(unit, op_st, pm_info, lane_map));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm_port_speed_config_validate(int unit, const pm_access_t *pm_acc,
                              pm_oper_status_t *op_st, pm_info_t *pm_info,
                              pm_speed_config_t *spd_cfg)
{
    bcmpc_pm_drv_t *pm_drv = NULL;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->speed_config_validate) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->speed_config_validate(unit, pm_acc, op_st, pm_info,
                                           spd_cfg));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm_core_attach(int unit, const pm_access_t *pm_acc,
               pm_oper_status_t *op_st, pm_info_t *pm_info,
               pm_port_add_info_t *add_info)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    SHR_NULL_CHECK(pm_drv->core_attach, SHR_E_UNAVAIL);
    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv->core_attach(unit, pm_acc, op_st, pm_info, add_info));

exit:
    SHR_FUNC_EXIT();
}

static int
pm_port_detach(int unit, const pm_access_t *pm_acc,
               pm_oper_status_t *op_st, pm_info_t *pm_info)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    SHR_NULL_CHECK(pm_drv->port_detach, SHR_E_UNAVAIL);
    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv->port_detach(unit, pm_acc, op_st, pm_info));

exit:
    SHR_FUNC_EXIT();
}

static int
pm_port_add(int unit, const pm_access_t *pm_acc,
            pm_oper_status_t *op_st, pm_info_t *pm_info,
            pm_port_add_info_t *add_info)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    SHR_NULL_CHECK(pm_drv->port_add, SHR_E_UNAVAIL);
    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv->port_add(unit, pm_acc, op_st, pm_info, add_info));

exit:
    SHR_FUNC_EXIT();
}

static int
pm_port_speed_config_set(int unit, const pm_access_t *pm_acc,
                         pm_oper_status_t *op_st, pm_info_t *pm_info,
                         pm_speed_config_t *speed_config)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->pm_port_speed_config_set) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_port_speed_config_set(unit, pm_acc, op_st, pm_info,
                                           speed_config));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm_port_speed_config_get(int unit, const pm_access_t *pm_acc,
                         pm_oper_status_t *op_st, pm_info_t *pm_info,
                         pm_speed_config_t *speed_config)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->pm_port_speed_config_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_port_speed_config_get(unit, pm_acc, op_st, pm_info,
                                           speed_config));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm_port_config_validate(int unit, const pm_access_t *pm_acc,
                        pm_oper_status_t *op_st, pm_info_t *pm_info,
                        pm_speed_config_t *spd_cfg, int is_first)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->pm_port_config_validate) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_port_config_validate(unit, pm_acc, op_st, pm_info,
                                             spd_cfg, is_first));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm_port_loopback_set(int unit, const pm_access_t *pm_acc,
                     pm_oper_status_t *op_st, pm_info_t *pm_info,
                     pm_loopback_mode_t loopback_type, int enable)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->pm_port_loopback_set) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_port_loopback_set(unit, pm_acc, op_st, pm_info,
                                          loopback_type, enable));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm_port_loopback_get(int unit, const pm_access_t *pm_acc,
                     pm_oper_status_t *op_st, pm_info_t *pm_info,
                     pm_loopback_mode_t loopback_type, int *enable)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->pm_port_loopback_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_port_loopback_get(unit, pm_acc, op_st, pm_info,
                                          loopback_type, enable));
    } else {
         SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}


static int
pm_port_encap_set(int unit, const pm_access_t *pm_acc,
                  pm_oper_status_t *op_st, pm_info_t *pm_info,
                  pm_encap_t encap)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->pm_port_encap_set) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_port_encap_set(unit, pm_acc, op_st, pm_info, encap));
    } else {
         SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm_port_encap_get(int unit, const pm_access_t *pm_acc,
                  pm_oper_status_t *op_st, pm_info_t *pm_info,
                  pm_encap_t *encap)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->pm_port_encap_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_port_encap_get(unit, pm_acc, op_st, pm_info, encap));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm_port_pause_control_set(int unit, const pm_access_t *pm_acc,
                          pm_oper_status_t *op_st, pm_info_t *pm_info,
                          pm_pause_ctrl_t *pause_ctrl)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->pm_port_pause_ctrl_set) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_port_pause_ctrl_set(unit, pm_acc, op_st,
                                            pm_info, pause_ctrl));
    } else {
         SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm_port_pause_control_get(int unit, const pm_access_t *pm_acc,
                          pm_oper_status_t *op_st, pm_info_t *pm_info,
                          pm_pause_ctrl_t *pause_ctrl)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->pm_port_pause_ctrl_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_port_pause_ctrl_get(unit, pm_acc, op_st,
                                            pm_info, pause_ctrl));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm_port_pause_addr_set(int unit, const pm_access_t *pm_acc,
                       pm_oper_status_t *op_st, pm_info_t *pm_info,
                       pm_mac_t pause_addr)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->pm_port_pause_addr_set) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_port_pause_addr_set(unit, pm_acc, op_st,
                                            pm_info, pause_addr));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm_port_pause_addr_get(int unit, const pm_access_t *pm_acc,
                       pm_oper_status_t *op_st, pm_info_t *pm_info,
                       pm_mac_t pause_addr)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->pm_port_pause_addr_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_port_pause_addr_get(unit, pm_acc, op_st,
                                            pm_info, pause_addr));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm_port_max_frame_size_set(int unit, const pm_access_t *pm_acc,
                           pm_oper_status_t *op_st, pm_info_t *pm_info,
                           uint32_t max_frame_size)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->pm_port_frame_max_set) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_port_frame_max_set(unit, pm_acc, op_st, pm_info,
                                           max_frame_size));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}


static int
pm_port_max_frame_size_get(int unit, const pm_access_t *pm_acc,
                           pm_oper_status_t *op_st, pm_info_t *pm_info,
                           uint32_t *max_frame_size)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->pm_port_frame_max_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_port_frame_max_get(unit, pm_acc, op_st,
                                           pm_info, max_frame_size));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm_port_enable_set(int unit, const pm_access_t *pm_acc,
                   pm_oper_status_t *op_st, pm_info_t *pm_info,
                   uint32_t flag, uint32_t enable)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->pm_port_enable_set) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_port_enable_set(unit, pm_acc, op_st, pm_info,
                                        flag, enable));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm_port_enable_get(int unit, const pm_access_t *pm_acc,
                   pm_oper_status_t *op_st, pm_info_t *pm_info,
                   uint32_t flag, uint32_t *enable)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->pm_port_enable_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_port_enable_get(unit, pm_acc, op_st, pm_info,
                                        flag, enable));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm_port_link_status_get(int unit, const pm_access_t *pm_acc,
                        pm_oper_status_t *op_st, pm_info_t *pm_info,
                        uint32_t *link_status)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->pm_port_link_status_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_port_link_status_get(unit, pm_acc, op_st,
                                             pm_info, link_status));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm_port_lag_failover_loopback_get(int unit, const pm_access_t *pm_acc,
                                  pm_oper_status_t *op_st,
                                  pm_info_t *pm_info, uint32_t *enable)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->pm_port_lag_failover_lpbk_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_port_lag_failover_lpbk_get(unit, pm_acc, op_st, pm_info,
                                                   enable));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}


static int
pm_port_ability_advert_set(int unit, const pm_access_t *pm_acc,
                           pm_oper_status_t *op_st, pm_info_t *pm_info,
                           int num_abilities, pm_port_ability_t* ability)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->pm_port_ability_advert_set) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_port_ability_advert_set(unit, pm_acc, op_st, pm_info,
                                                num_abilities, ability));
    } else {
         SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm_port_ability_advert_get(int unit, const pm_access_t *pm_acc,
                           pm_oper_status_t *op_st, pm_info_t *pm_info,
                           uint32_t max_num_abilities,
                           uint32_t *actual_num_abilities,
                           pm_port_ability_t *ability)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->pm_port_ability_advert_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_port_ability_advert_get(unit, pm_acc, op_st, pm_info,
                           max_num_abilities, actual_num_abilities, ability));
    } else {
         SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm_port_ability_remote_get(int unit, const pm_access_t *pm_acc,
                           pm_oper_status_t *op_st, pm_info_t *pm_info,
                           uint32_t max_num_abilities,
                           uint32_t *actual_num_abilities,
                           pm_port_ability_t* ability)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->pm_port_ability_remote_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_port_ability_remote_get(unit, pm_acc, op_st, pm_info,
                           max_num_abilities, actual_num_abilities, ability));
    } else {
         SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm_port_autoneg_set(int unit, const pm_access_t *pm_acc,
                    pm_oper_status_t *op_st, pm_info_t *pm_info,
                    phymod_autoneg_control_t* an)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->pm_port_autoneg_set) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_port_autoneg_set(unit, pm_acc, op_st, pm_info, an));
    } else {
         SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm_port_autoneg_get(int unit, const pm_access_t *pm_acc,
                    pm_oper_status_t *op_st, pm_info_t *pm_info,
                    phymod_autoneg_control_t* an)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->pm_port_autoneg_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_port_autoneg_get(unit, pm_acc, op_st, pm_info, an));
    } else {
         SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm_port_autoneg_status_get(int unit, const pm_access_t *pm_acc,
                           pm_oper_status_t *op_st, pm_info_t *pm_info,
                           phymod_autoneg_status_t* an_status)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->pm_port_autoneg_status_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_port_autoneg_status_get(unit, pm_acc, op_st, pm_info,
                                             an_status));
    } else {
         SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm_port_tx_set(int unit, const pm_access_t *pm_acc,
               pm_oper_status_t *op_st, pm_info_t *pm_info,
               phymod_tx_t *tx_config)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->pm_port_tx_set) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_port_tx_set(unit, pm_acc, op_st, pm_info, tx_config));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm_port_tx_get(int unit, const pm_access_t *pm_acc,
               pm_oper_status_t *op_st, pm_info_t *pm_info,
               phymod_tx_t *tx_config)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->pm_port_tx_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_port_tx_get(unit, pm_acc, op_st, pm_info, tx_config));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm_port_pmd_lane_cfg_set(int unit, const pm_access_t *pm_acc,
                         pm_oper_status_t *op_st, pm_info_t *pm_info,
                         phymod_firmware_lane_config_t *fw_lane_config)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->pm_port_pmd_lane_cfg_set) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_port_pmd_lane_cfg_set(unit, pm_acc, op_st, pm_info,
                                              fw_lane_config));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm_port_pmd_lane_cfg_get(int unit, const pm_access_t *pm_acc,
                         pm_oper_status_t *op_st, pm_info_t *pm_info,
                         phymod_firmware_lane_config_t *fw_lane_config)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->pm_port_pmd_lane_cfg_set) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_port_pmd_lane_cfg_get(unit, pm_acc, op_st, pm_info,
                                              fw_lane_config));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}



static int
pm_port_failover_enable_set(int unit, const pm_access_t *pm_acc,
                            pm_oper_status_t *op_st, pm_info_t *pm_info,
                            uint32_t enable)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->pm_port_failover_enable_set) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_port_failover_enable_set(unit, pm_acc, op_st, pm_info,
                                                 enable));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm_port_failover_enable_get(int unit, const pm_access_t *pm_acc,
                            pm_oper_status_t *op_st, pm_info_t *pm_info,
                            uint32_t *enable)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->pm_port_failover_enable_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_port_failover_enable_get(unit, pm_acc, op_st, pm_info,
                                                 enable));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm_port_link_training_set(int unit, const pm_access_t *pm_acc,
                          pm_oper_status_t *op_st, pm_info_t *pm_info,
                          uint32_t enable)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    SHR_NULL_CHECK(pm_drv->pm_port_link_training_set, SHR_E_UNAVAIL);
    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv->pm_port_link_training_set(unit, pm_acc, op_st,
                                           pm_info, enable));

exit:
    SHR_FUNC_EXIT();
}

static int
pm_port_link_training_get(int unit, const pm_access_t *pm_acc,
                          pm_oper_status_t *op_st, pm_info_t *pm_info,
                          uint32_t *enable)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    SHR_NULL_CHECK(pm_drv->pm_port_link_training_get, SHR_E_UNAVAIL);
    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv->pm_port_link_training_get(unit, pm_acc, op_st,
                                           pm_info, enable));

exit:
    SHR_FUNC_EXIT();
}

static int
pm_mac_tx_enable_set(int unit, const pm_access_t *pm_acc,
                     pm_oper_status_t *op_st, pm_info_t *pm_info,
                     uint32_t tx_enable)
{
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    SHR_NULL_CHECK(pmacd->tx_enable_set, SHR_E_UNAVAIL);
    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->tx_enable_set(&pmac_acc, pm_acc->pmac_blkport, tx_enable));

exit:
    SHR_FUNC_EXIT();
}

static int
pm_mac_rx_enable_set(int unit, const pm_access_t *pm_acc,
                     pm_oper_status_t *op_st, pm_info_t *pm_info,
                     uint32_t rx_enable)
{
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    SHR_NULL_CHECK(pmacd->rx_enable_set, SHR_E_UNAVAIL);
    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->rx_enable_set(&pmac_acc, pm_acc->pmac_blkport, rx_enable));

exit:
    SHR_FUNC_EXIT();
}

static int
pm_mac_enable_get(int unit, const pm_access_t *pm_acc,
                  pm_oper_status_t *op_st, pm_info_t *pm_info,
                  uint32_t *tx_enable, uint32_t *rx_enable)
{
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    SHR_NULL_CHECK(pmacd->tx_enable_get, SHR_E_UNAVAIL);
    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->tx_enable_get(&pmac_acc, pm_acc->pmac_blkport, tx_enable));

    SHR_NULL_CHECK(pmacd->rx_enable_get, SHR_E_UNAVAIL);
    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->rx_enable_get(&pmac_acc, pm_acc->pmac_blkport, rx_enable));

exit:
    SHR_FUNC_EXIT();
}

static int
pm_mac_fault_status_get(int unit, const pm_access_t *pm_acc,
                        pm_oper_status_t *op_st, pm_info_t *pm_info,
                        bcmpmac_fault_status_t *st)
{
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    SHR_NULL_CHECK(pmacd->fault_status_get, SHR_E_UNAVAIL);
    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->fault_status_get(&pmac_acc, pm_acc->pmac_blkport, st));

exit:
    SHR_FUNC_EXIT();
}

static int
pm_mac_fault_disable_set(int unit, const pm_access_t *pm_acc,
                         pm_oper_status_t *op_st, pm_info_t *pm_info,
                         bcmpmac_fault_disable_t *dis)
{
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    SHR_NULL_CHECK(pmacd->fault_disable_set, SHR_E_UNAVAIL);
    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->fault_disable_set(&pmac_acc, pm_acc->pmac_blkport, dis));

exit:
    SHR_FUNC_EXIT();
}

static int
pm_mac_fault_disable_get(int unit, const pm_access_t *pm_acc,
                         pm_oper_status_t *op_st, pm_info_t *pm_info,
                         bcmpmac_fault_disable_t *st)
{
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    SHR_NULL_CHECK(pmacd->fault_disable_get, SHR_E_UNAVAIL);
    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->fault_disable_get(&pmac_acc, pm_acc->pmac_blkport, st));

exit:
    SHR_FUNC_EXIT();
}

static int
pm_mac_avg_ipg_set(int unit, const pm_access_t *pm_acc,
                   pm_oper_status_t *op_st, pm_info_t *pm_info,
                   uint8_t ipg_size)
{
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    SHR_NULL_CHECK(pmacd->avg_ipg_set, SHR_E_UNAVAIL);
    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->avg_ipg_set(&pmac_acc, pm_acc->pmac_blkport, ipg_size));

exit:
    SHR_FUNC_EXIT();
}

static int
pm_mac_avg_ipg_get(int unit, const pm_access_t *pm_acc,
                   pm_oper_status_t *op_st, pm_info_t *pm_info,
                   uint8_t *ipg_size)
{
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    SHR_NULL_CHECK(pmacd->avg_ipg_get, SHR_E_UNAVAIL);
    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->avg_ipg_get(&pmac_acc, pm_acc->pmac_blkport, ipg_size));

exit:
    SHR_FUNC_EXIT();
}

static int
pm_pfc_set(int unit, const pm_access_t *pm_acc, pm_oper_status_t *op_st,
           pm_info_t *pm_info, bcmpmac_pfc_t *pfc_cfg)
{
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    SHR_NULL_CHECK(pmacd->pfc_set, SHR_E_UNAVAIL);
    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->pfc_set(&pmac_acc, pm_acc->pmac_blkport, pfc_cfg));

    SHR_NULL_CHECK(pmacd->pass_pfc_set, SHR_E_UNAVAIL);
    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->pass_pfc_set(&pmac_acc, pm_acc->pmac_blkport,
                             pfc_cfg->pfc_pass));

exit:
    SHR_FUNC_EXIT();
}

static int
pm_pfc_get(int unit, const pm_access_t *pm_acc, pm_oper_status_t *op_st,
           pm_info_t *pm_info, bcmpmac_pfc_t *pfc_cfg)
{
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    SHR_NULL_CHECK(pmacd->pfc_get, SHR_E_UNAVAIL);
    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->pfc_get(&pmac_acc, pm_acc->pmac_blkport, pfc_cfg));

    SHR_NULL_CHECK(pmacd->pass_pfc_get, SHR_E_UNAVAIL);
    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->pass_pfc_get(&pmac_acc, pm_acc->pmac_blkport,
                             &(pfc_cfg->pfc_pass)));

exit:
    SHR_FUNC_EXIT();
}

static int
pm_phy_status_get(int unit, const pm_access_t *pm_acc, pm_oper_status_t *op_st,
                  pm_info_t *pm_info, pm_phy_status_t *phy_st)
{

    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    SHR_NULL_CHECK(pm_drv->pm_phy_status_get, SHR_E_UNAVAIL);
    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv->pm_phy_status_get(unit, pm_acc, op_st, pm_info, phy_st));

exit:
    SHR_FUNC_EXIT();
}

static int
pm_phy_control_set(int unit, const pm_access_t *pm_acc,
                   pm_oper_status_t *op_st, pm_info_t *pm_info,
                   uint32_t flags, pm_phy_control_t *phy_ctrl)
{

    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    SHR_NULL_CHECK(pm_drv->pm_phy_control_set, SHR_E_UNAVAIL);
    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv->pm_phy_control_set(unit, pm_acc, op_st, pm_info,
                                    flags, phy_ctrl));

exit:
    SHR_FUNC_EXIT();
}

static int
pm_mib_oversize_set(int unit, const pm_access_t *pm_acc,
                    pm_oper_status_t *op_st, pm_info_t *pm_info,
                    uint32_t size)
{
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    SHR_NULL_CHECK(pmacd->mib_oversize_set, SHR_E_UNAVAIL);
    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->mib_oversize_set(&pmac_acc, pm_acc->pmac_blkport, size));

exit:
    SHR_FUNC_EXIT();
}

static int
pm_mib_oversize_get(int unit, const pm_access_t *pm_acc,
                    pm_oper_status_t *op_st, pm_info_t *pm_info,
                    uint32_t *size)
{
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    SHR_NULL_CHECK(pmacd->mib_oversize_get, SHR_E_UNAVAIL);
    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->mib_oversize_get(&pmac_acc, pm_acc->pmac_blkport, size));

exit:
    SHR_FUNC_EXIT();
}

static int
pm_port_abilities_get(int unit, const pm_access_t *pm_acc,
                      pm_oper_status_t *op_st, pm_info_t *pm_info,
                      pm_port_ability_type_t ability_type,
                      uint32_t *num_abilities,
                      pm_port_ability_t *pabilities)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->pm_port_ability_advert_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_port_abilities_get(unit, pm_acc, op_st, pm_info,
                                           ability_type, num_abilities,
                                           pabilities));
    } else {
         SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm_control_frames_pass_set(int unit, const pm_access_t *pm_acc,
                           pm_oper_status_t *op_st, pm_info_t *pm_info,
                           uint32_t enable)
{
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    SHR_NULL_CHECK(pmacd->pass_control_set, SHR_E_UNAVAIL);
    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->pass_control_set(&pmac_acc, pm_acc->pmac_blkport, enable));

exit:
    SHR_FUNC_EXIT();
}

static int
pm_control_frames_pass_get(int unit, const pm_access_t *pm_acc,
                           pm_oper_status_t *op_st, pm_info_t *pm_info,
                           uint32_t *enable)
{
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    SHR_NULL_CHECK(pmacd->pass_control_get, SHR_E_UNAVAIL);
    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->pass_control_get(&pmac_acc, pm_acc->pmac_blkport, enable));

exit:
    SHR_FUNC_EXIT();
}

static int
pm_pause_frames_pass_set(int unit, const pm_access_t *pm_acc,
                         pm_oper_status_t *op_st, pm_info_t *pm_info,
                         uint32_t enable)
{
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    SHR_NULL_CHECK(pmacd->pass_pause_set, SHR_E_UNAVAIL);
    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->pass_pause_set(&pmac_acc, pm_acc->pmac_blkport, enable));

exit:
    SHR_FUNC_EXIT();
}

static int
pm_pause_frames_pass_get(int unit, const pm_access_t *pm_acc,
                         pm_oper_status_t *op_st, pm_info_t *pm_info,
                         uint32_t *enable)
{
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    SHR_NULL_CHECK(pmacd->pass_pause_get, SHR_E_UNAVAIL);
    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->pass_pause_get(&pmac_acc, pm_acc->pmac_blkport, enable));

exit:
    SHR_FUNC_EXIT();
}

static int
pm_tsc_ctrl(int unit, const pm_access_t *pm_acc, pm_oper_status_t *op_st,
            pm_info_t *pm_info, int tsc_pwr_on)
{
    bcmpmac_tsc_ctrl_t tsc_ctrl;
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    SHR_NULL_CHECK(pmacd->tsc_ctrl_get, SHR_E_UNAVAIL);
    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->tsc_ctrl_get(&pmac_acc, pm_acc->pmac_blkport, &tsc_ctrl));

    if (tsc_ctrl.tsc_pwrdwn && tsc_pwr_on) {
        /* Power on TSC. */
        SHR_IF_ERR_VERBOSE_EXIT
            (pmacd->tsc_ctrl_set(&pmac_acc, pm_acc->pmac_blkport, 1));
    } else if ((!tsc_ctrl.tsc_pwrdwn) && (!tsc_pwr_on)) {
        /* Power down TSC. */
        SHR_IF_ERR_VERBOSE_EXIT
            (pmacd->tsc_ctrl_set(&pmac_acc, pm_acc->pmac_blkport, 0));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
pm_vco_get(int unit, const pm_access_t *pm_acc, pm_oper_status_t *op_st,
           pm_info_t *pm_info, pm_vco_t *vco_rate)
{
    bcmpc_pm_drv_t *pm_drv = NULL;
    bcmpmac_tsc_ctrl_t tsc_ctrl;
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    SHR_NULL_CHECK(pmacd->tsc_ctrl_get, SHR_E_UNAVAIL);
    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->tsc_ctrl_get(&pmac_acc, pm_acc->pmac_blkport, &tsc_ctrl));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (!tsc_ctrl.tsc_rstb || tsc_ctrl.tsc_pwrdwn) {
        *vco_rate = PM_VCO_INVALID;
    } else if (pm_drv->pm_vco_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_vco_get(unit, pm_acc, op_st, pm_info, vco_rate));
    } else {
         SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm_port_timesync_config_set(int unit, const pm_access_t *pm_acc,
                            pm_oper_status_t *op_st, pm_info_t *pm_info,
                            pm_phy_timesync_config_t *config)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->pm_port_timesync_config_set) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_port_timesync_config_set(unit, pm_acc, op_st, pm_info,
                                                 config));
    } else {
         SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm_port_synce_clk_ctrl_set(int unit, const pm_access_t *pm_acc,
                           pm_oper_status_t *op_st, pm_info_t *pm_info,
                           pm_port_synce_clk_ctrl_t *cfg)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->pm_port_synce_clk_ctrl_set) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_port_synce_clk_ctrl_set(unit, pm_acc, op_st,
                                                pm_info, cfg));
    } else {
         SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm_port_synce_clk_ctrl_get(int unit, const pm_access_t *pm_acc,
                           pm_oper_status_t *op_st, pm_info_t *pm_info,
                           pm_port_synce_clk_ctrl_t *cfg)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->pm_port_synce_clk_ctrl_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_port_synce_clk_ctrl_get(unit, pm_acc, op_st,
                                                pm_info, cfg));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm_port_phy_link_up_event(int unit, const pm_access_t *pm_acc,
                          pm_oper_status_t *op_st, pm_info_t *pm_info)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->pm_port_phy_link_up_event) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_port_phy_link_up_event(unit, pm_acc, op_st, pm_info));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm_port_phy_link_down_event(int unit, const pm_access_t *pm_acc,
                            pm_oper_status_t *op_st, pm_info_t *pm_info)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->pm_port_phy_link_down_event) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_port_phy_link_down_event(unit, pm_acc, op_st, pm_info));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm_port_sw_state_update(int unit, const pm_access_t *pm_acc,
                        pm_oper_status_t *op_st, pm_info_t *pm_info)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->pm_port_sw_state_update) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_port_sw_state_update(unit, pm_acc, op_st, pm_info));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm_runt_threshold_set(int unit, const pm_access_t *pm_acc,
                      pm_oper_status_t *op_st, pm_info_t *pm_info,
                      uint32_t value)
{
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    SHR_NULL_CHECK(pmacd->runt_threshold_set, SHR_E_UNAVAIL);
    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->runt_threshold_set(&pmac_acc, pm_acc->pmac_blkport, value));

exit:
    SHR_FUNC_EXIT();
}

static int
pm_runt_threshold_get(int unit, const pm_access_t *pm_acc,
                      pm_oper_status_t *op_st, pm_info_t *pm_info,
                      uint32_t *value)
{
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    SHR_NULL_CHECK(pmacd->runt_threshold_get, SHR_E_UNAVAIL);
    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->runt_threshold_get(&pmac_acc, pm_acc->pmac_blkport, value));

exit:
    SHR_FUNC_EXIT();
}

static int
pm_port_rlm_config_set(int unit, const pm_access_t *pm_acc,
                       pm_oper_status_t *op_st, pm_info_t *pm_info,
                       pm_port_rlm_config_t *rlm_config)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->pm_port_rlm_config_set) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_port_rlm_config_set(unit, pm_acc, op_st,
                                            pm_info, rlm_config));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int pm_destroy(int unit, pm_info_t *pm_info)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->pm_destroy) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_destroy(unit, pm_info));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int pm_init(int unit, const pm_access_t *pm_acc, pm_info_t *pm_info)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->pm_init) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_init(unit, pm_acc, pm_info));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm_port_link_training_status_get(int unit, const pm_access_t *pm_acc,
                                 pm_oper_status_t *op_st, pm_info_t *pm_info,
                                 uint32_t *locked)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->pm_port_link_training_status_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_port_link_training_status_get(unit, pm_acc, op_st,
                                                      pm_info, locked));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm_port_rlm_status_get(int unit, const pm_access_t *pm_acc,
                       pm_oper_status_t *op_st, pm_info_t *pm_info,
                       pm_port_rlm_status_t *rlm_status)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->pm_port_rlm_status_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_port_rlm_status_get(unit, pm_acc, op_st, pm_info,
                                            rlm_status));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm_vco_rate_calculate(int unit, pm_oper_status_t *op_st,
                      pm_info_t *pm_info, pm_vco_setting_t *vco_select)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->pm_vco_rate_calculate) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_vco_rate_calculate(unit, op_st, pm_info, vco_select));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm_port_default_lane_config_get(int unit, const pm_access_t *pm_acc,
                                pm_oper_status_t *op_st, pm_info_t *pm_info,
                                pm_speed_config_t *speed_config,
                                phymod_firmware_lane_config_t *lane_config)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->pm_default_pmd_lane_config_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_default_pmd_lane_config_get(unit, pm_acc, op_st,
                                                    pm_info,
                                                    speed_config, lane_config));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm_phy_polarity_get(int unit, const pm_access_t *pm_acc,
                    pm_oper_status_t *op_st, pm_info_t *pm_info,
                    phymod_polarity_t *polarity)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->pm_phy_polarity_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_phy_polarity_get(unit, pm_acc, op_st,
                                         pm_info, polarity));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm_port_default_tx_taps_config_get(int unit, const pm_access_t *pm_acc,
                                   pm_oper_status_t *op_st, pm_info_t *pm_info,
                                   pm_speed_config_t *speed_config,
                                   phymod_tx_t *tx_taps_config)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->pm_default_tx_taps_config_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_default_tx_taps_config_get(unit, pm_acc, op_st,
                                                   pm_info,
                                                   speed_config, tx_taps_config));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm_core_lane_map_get(int unit, const pm_access_t *pm_acc,
                    pm_oper_status_t *op_st, pm_info_t *pm_info,
                    phymod_lane_map_t *lane_map)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->pm_core_lane_map_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_core_lane_map_get(unit, pm_acc, op_st,
                                          pm_info, lane_map));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm_port_stall_tx_enable_get(int unit, const pm_access_t *pm_acc,
                            pm_oper_status_t *op_st, pm_info_t *pm_info,
                            bool *enable)
{
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    SHR_NULL_CHECK(pmacd->stall_tx_enable_get, SHR_E_UNAVAIL);
    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->stall_tx_enable_get(&pmac_acc, pm_acc->pmac_blkport, enable));
exit:
    SHR_FUNC_EXIT();
}

static int
pm_port_stall_tx_enable_set(int unit, const pm_access_t *pm_acc,
                            pm_oper_status_t *op_st, pm_info_t *pm_info,
                            bool enable)
{
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    SHR_NULL_CHECK(pmacd->stall_tx_enable_set, SHR_E_UNAVAIL);
    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->stall_tx_enable_set(&pmac_acc, pm_acc->pmac_blkport, enable));

exit:
    SHR_FUNC_EXIT();
}

static int
pm_port_rsv_mask_get(int unit, const pm_access_t *pm_acc,
                     pm_oper_status_t *op_st, pm_info_t *pm_info,
                     uint32_t flags, uint32_t *value)
{
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    SHR_NULL_CHECK(pmacd->rsv_mask_get, SHR_E_UNAVAIL);
    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->rsv_mask_get(&pmac_acc, pm_acc->pmac_blkport, value));
exit:
    SHR_FUNC_EXIT();
}

static int
pm_port_rsv_mask_set(int unit, const pm_access_t *pm_acc,
                     pm_oper_status_t *op_st, pm_info_t *pm_info,
                     uint32_t flags, uint32_t value)
{
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    SHR_NULL_CHECK(pmacd->rsv_mask_set, SHR_E_UNAVAIL);
    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->rsv_mask_set(&pmac_acc, pm_acc->pmac_blkport, value));
exit:
    SHR_FUNC_EXIT();
}

static int
pm_port_tx_timestamp_info_get(int unit, const pm_access_t *pm_acc,
                              pm_oper_status_t *op_st, pm_info_t *pm_info,
                              pm_tx_timestamp_info_t *timestamp_info)
{
    bcmpc_pm_drv_t *pm_drv = NULL;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->port_tx_timestamp_info_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->port_tx_timestamp_info_get(unit, pm_acc, op_st,
                                                pm_info, timestamp_info));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }


exit:
    SHR_FUNC_EXIT();
}

static int
pm_egress_timestamp_mode_get(int unit, const pm_access_t *pm_acc,
                             pm_oper_status_t *op_st, pm_info_t *pm_info,
                             pm_egr_timestamp_mode_t *timestamp_mode)
{
    bcmpc_pm_drv_t *pm_drv = NULL;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->pm_egress_timestamp_mode_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_egress_timestamp_mode_get(unit, pm_acc, op_st,
                                                  pm_info, timestamp_mode));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm_egress_timestamp_mode_set(int unit, const pm_access_t *pm_acc,
                             pm_oper_status_t *op_st, pm_info_t *pm_info,
                             pm_egr_timestamp_mode_t timestamp_mode)
{
    bcmpc_pm_drv_t *pm_drv = NULL;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->pm_egress_timestamp_mode_set) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_egress_timestamp_mode_set(unit, pm_acc, op_st,
                                                  pm_info, timestamp_mode));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm_phy_rsfec_uncorrectable_counter_get(int unit, const pm_access_t *pm_acc,
                                       pm_info_t *pm_info,
                                       uint64_t *count)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->pm_phy_rsfec_uncorrectable_counter_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_phy_rsfec_uncorrectable_counter_get(unit, pm_acc,
                                                            pm_info,
                                                            count));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm_phy_rsfec_correctable_counter_get(int unit, const pm_access_t *pm_acc,
                                     pm_info_t *pm_info,
                                     uint64_t *count)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->pm_phy_rsfec_correctable_counter_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_phy_rsfec_correctable_counter_get(unit, pm_acc,
                                                          pm_info,
                                                          count));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm_phy_rsfec_symbol_error_counter_get (int unit, const pm_access_t *pm_acc,
                                       pm_info_t *pm_info,
                                       uint64_t *count)
{
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->pm_phy_rsfec_symbol_error_counter_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_phy_rsfec_symbol_error_counter_get(unit, pm_acc,
                                                          pm_info,
                                                          count));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm_port_intr_enable_get(int unit, const pm_access_t *pm_acc,
                        pm_oper_status_t *op_st, pm_info_t *pm_info,
                        bcmpc_intr_type_t intr_type,
                        uint32_t *enable)
{
    bcmpc_pm_drv_t *pm_drv = NULL;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->port_intr_enable_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->port_intr_enable_get(unit, pm_acc, op_st,
                                          pm_info, intr_type, enable));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm_port_intr_enable_set(int unit, const pm_access_t *pm_acc,
                        pm_oper_status_t *op_st, pm_info_t *pm_info,
                        bcmpc_intr_type_t intr_type,
                        uint32_t enable)
{
    bcmpc_pm_drv_t *pm_drv = NULL;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->port_intr_enable_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->port_intr_enable_set(unit, pm_acc, op_st,
                                          pm_info, intr_type, enable));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}


static int
pm_port_interrupt_process(int unit, const pm_access_t *pm_acc,
                           pm_oper_status_t *op_st, pm_info_t *pm_info,
                           bcmpc_intr_type_t intr_type,
                           uint32_t *intr_cleared)
{
    bcmpc_pm_drv_t *pm_drv = NULL;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv_get(unit, pm_info, &pm_drv));

    if (pm_drv->port_interrupt_process) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->port_interrupt_process(unit, pm_acc, op_st,
                                            pm_info, intr_type, intr_cleared));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

bcmpc_pm_drv_t bcmpc_pm_drv_common = {
    .port_lane_map_validate = pm_port_lane_map_validate,
    .speed_config_validate = pm_port_speed_config_validate,
    .core_attach = pm_core_attach,
    .port_detach = pm_port_detach,
    .pm_init = pm_init,
    .pm_destroy = pm_destroy,
    .port_add = pm_port_add,
    .pm_port_speed_config_set = pm_port_speed_config_set,
    .pm_port_speed_config_get = pm_port_speed_config_get,
    .pm_port_config_validate = pm_port_config_validate,
    .pm_port_loopback_set = pm_port_loopback_set,
    .pm_port_loopback_get = pm_port_loopback_get,
    .pm_port_ability_advert_set = pm_port_ability_advert_set,
    .pm_port_ability_advert_get = pm_port_ability_advert_get,
    .pm_port_ability_remote_get = pm_port_ability_remote_get,
    .pm_port_autoneg_set = pm_port_autoneg_set,
    .pm_port_autoneg_get = pm_port_autoneg_get,
    .pm_port_autoneg_status_get = pm_port_autoneg_status_get,
    .pm_port_encap_set = pm_port_encap_set,
    .pm_port_encap_get = pm_port_encap_get,
    .pm_port_pause_ctrl_set = pm_port_pause_control_set,
    .pm_port_pause_ctrl_get = pm_port_pause_control_get,
    .pm_port_pause_addr_set = pm_port_pause_addr_set,
    .pm_port_pause_addr_get = pm_port_pause_addr_get,
    .pm_port_frame_max_set = pm_port_max_frame_size_set,
    .pm_port_frame_max_get = pm_port_max_frame_size_get,
    .pm_port_enable_set = pm_port_enable_set,
    .pm_port_enable_get = pm_port_enable_get,
    .pm_port_link_status_get = pm_port_link_status_get,
    .pm_port_lag_failover_lpbk_get = pm_port_lag_failover_loopback_get,
    .pm_port_tx_set = pm_port_tx_set,
    .pm_port_tx_get = pm_port_tx_get,
    .pm_port_pmd_lane_cfg_set = pm_port_pmd_lane_cfg_set,
    .pm_port_pmd_lane_cfg_get = pm_port_pmd_lane_cfg_get,
    .pm_port_failover_enable_set = pm_port_failover_enable_set,
    .pm_port_failover_enable_get = pm_port_failover_enable_get,
    .pm_port_link_training_set = pm_port_link_training_set,
    .pm_port_link_training_get = pm_port_link_training_get,
    .pm_mac_tx_enable_set = pm_mac_tx_enable_set,
    .pm_mac_rx_enable_set = pm_mac_rx_enable_set,
    .pm_mac_enable_get = pm_mac_enable_get,
    .pm_mac_fault_disable_set = pm_mac_fault_disable_set,
    .pm_mac_fault_status_get = pm_mac_fault_status_get,
    .pm_mac_fault_disable_get = pm_mac_fault_disable_get,
    .pm_mac_avg_ipg_get = pm_mac_avg_ipg_get,
    .pm_mac_avg_ipg_set = pm_mac_avg_ipg_set,
    .pm_pfc_set = pm_pfc_set,
    .pm_pfc_get = pm_pfc_get,
    .pm_phy_status_get = pm_phy_status_get,
    .pm_phy_control_set = pm_phy_control_set,
    .pm_mib_oversize_set = pm_mib_oversize_set,
    .pm_mib_oversize_get = pm_mib_oversize_get,
    .pm_port_abilities_get = pm_port_abilities_get,
    .pause_frames_pass_set = pm_pause_frames_pass_set,
    .pause_frames_pass_get = pm_pause_frames_pass_get,
    .control_frames_pass_set = pm_control_frames_pass_set,
    .control_frames_pass_get = pm_control_frames_pass_get,
    .pm_tsc_ctrl = pm_tsc_ctrl,
    .pm_vco_get = pm_vco_get,
    .pm_port_timesync_config_set = pm_port_timesync_config_set,
    .pm_port_synce_clk_ctrl_set = pm_port_synce_clk_ctrl_set,
    .pm_port_synce_clk_ctrl_get = pm_port_synce_clk_ctrl_get,
    .pm_port_phy_link_up_event = pm_port_phy_link_up_event,
    .pm_port_phy_link_down_event = pm_port_phy_link_down_event,
    .pm_port_sw_state_update = pm_port_sw_state_update,
    .runt_threshold_set = pm_runt_threshold_set,
    .runt_threshold_get = pm_runt_threshold_get,
    .pm_port_rlm_config_set = pm_port_rlm_config_set,
    .pm_port_link_training_status_get = pm_port_link_training_status_get,
    .pm_port_rlm_status_get = pm_port_rlm_status_get,
    .pm_vco_rate_calculate = pm_vco_rate_calculate,
    .pm_default_pmd_lane_config_get = pm_port_default_lane_config_get,
    .pm_phy_polarity_get = pm_phy_polarity_get,
    .pm_default_tx_taps_config_get = pm_port_default_tx_taps_config_get,
    .pm_core_lane_map_get = pm_core_lane_map_get,
    .pm_port_stall_tx_get = pm_port_stall_tx_enable_get,
    .pm_port_stall_tx_set = pm_port_stall_tx_enable_set,
    .port_rsv_mask_get = pm_port_rsv_mask_get,
    .port_rsv_mask_set = pm_port_rsv_mask_set,
    .port_tx_timestamp_info_get = pm_port_tx_timestamp_info_get,
    .pm_egress_timestamp_mode_get = pm_egress_timestamp_mode_get,
    .pm_egress_timestamp_mode_set = pm_egress_timestamp_mode_set,
    .pm_phy_rsfec_uncorrectable_counter_get = pm_phy_rsfec_uncorrectable_counter_get,
    .pm_phy_rsfec_correctable_counter_get = pm_phy_rsfec_correctable_counter_get,
    .pm_phy_rsfec_symbol_error_counter_get = pm_phy_rsfec_symbol_error_counter_get,
    .port_intr_enable_get = pm_port_intr_enable_get,
    .port_intr_enable_set = pm_port_intr_enable_set,
    .port_interrupt_process = pm_port_interrupt_process
};
