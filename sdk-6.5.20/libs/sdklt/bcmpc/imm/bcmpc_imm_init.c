/*! \file bcmpc_imm_init.c
 *
 * BCMPC routines for initializing the IMM interface.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmpc/bcmpc_imm_internal.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_IMM


/*******************************************************************************
 * Public functions
 */

int
bcmpc_imm_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpc_imm_port_register(unit));
    SHR_IF_ERR_EXIT
        (bcmpc_imm_port_phys_map_register(unit));
    SHR_IF_ERR_EXIT
        (bcmpc_imm_pm_core_register(unit));
    SHR_IF_ERR_EXIT
        (bcmpc_imm_tx_taps_config_register(unit));
    SHR_IF_ERR_EXIT
        (bcmpc_imm_pmd_firmware_config_register(unit));
    SHR_IF_ERR_EXIT
        (bcmpc_imm_mac_control_register(unit));
    SHR_IF_ERR_EXIT
        (bcmpc_imm_phy_control_register(unit));
    SHR_IF_ERR_EXIT
        (bcmpc_imm_pfc_register(unit));
    SHR_IF_ERR_EXIT
        (bcmpc_imm_autoneg_profile_register(unit));
    SHR_IF_ERR_EXIT
        (bcmpc_imm_lm_ctrl_register(unit));
    SHR_IF_ERR_EXIT
        (bcmpc_imm_lm_port_ctrl_register(unit));
    SHR_IF_ERR_EXIT
        (bcmpc_imm_port_info_register(unit));
    SHR_IF_ERR_EXIT
        (bcmpc_imm_port_timesync_register(unit));
    SHR_IF_ERR_EXIT
        (bcmpc_imm_fdr_port_control_register(unit));
    SHR_IF_ERR_EXIT
        (bcmpc_imm_port_fdr_stats_register(unit));
    SHR_IF_ERR_EXIT
        (bcmpc_imm_port_monitor_register(unit));
exit:
    SHR_FUNC_EXIT();
}

