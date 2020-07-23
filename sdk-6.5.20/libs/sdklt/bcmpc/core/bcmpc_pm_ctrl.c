/*! \file bcmpc_pm_ctrl.c
 *
 * BCMPC PM control APIs
 *
 * This file implements the functions for configuring MAC/PHY in a PM.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <bsl/bsl.h>
#include <shr/shr_debug.h>

#include <bcmpc/bcmpc_lport.h>
#include <bcmpc/bcmpc_pm_internal.h>
#include <bcmpc/bcmpc_pm_internal.h>
#include <bcmpc/bcmpc_pmgr_internal.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_PM

/*******************************************************************************
 * Private functions
 */


/*******************************************************************************
 * Internal Public functions
 */

void
bcmpc_mac_control_t_init(bcmpc_mac_control_t *mac_ctrl)
{
    sal_memset(mac_ctrl, 0, sizeof(*mac_ctrl));
}

int
bcmpc_internal_mac_control_insert(int unit, bcmpc_lport_t lport,
                                  bcmpc_mac_control_t *mcfg)
{
    int rv;
    bcmpc_pport_t pport;
    bcmpc_port_cfg_t pcfg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmpc_port_phys_map_get(unit, lport, &pport), SHR_E_NOT_FOUND);

    rv = bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&lport), P(&pcfg));
    if (rv == SHR_E_NOT_FOUND) {
        SHR_EXIT();
    } else {
        SHR_IF_ERR_EXIT
            (bcmpc_pmgr_mac_control_set(unit, lport, pport, mcfg, *mcfg));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_internal_mac_control_update(int unit, bcmpc_lport_t lport,
                                  bcmpc_mac_control_t *mcfg,
                                  bcmpc_mac_control_t default_cfg)
{
    int rv;
    bcmpc_pport_t pport;
    bcmpc_port_cfg_t pcfg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmpc_port_phys_map_get(unit, lport, &pport), SHR_E_NOT_FOUND);

    rv = bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&lport), P(&pcfg));
    if (rv == SHR_E_NOT_FOUND) {
        SHR_EXIT();
    } else {
        SHR_IF_ERR_EXIT
            (bcmpc_pmgr_mac_control_set(unit, lport, pport, mcfg, default_cfg));
    }

exit:
    SHR_FUNC_EXIT();
}

void
bcmpc_phy_control_t_init(bcmpc_phy_control_t *phy_cfg)
{
    sal_memset(phy_cfg, 0, sizeof(*phy_cfg));
}

int
bcmpc_internal_phy_control_insert(int unit, bcmpc_port_lane_t *port_lane,
                                  bcmpc_phy_control_t *phy_cfg)
{
    int rv;
    bcmpc_lport_t lport;
    bcmpc_pport_t pport;
    bcmpc_port_cfg_t pcfg;

    SHR_FUNC_ENTER(unit);

    lport = port_lane->lport;
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmpc_port_phys_map_get(unit, lport, &pport), SHR_E_NOT_FOUND);

    rv = bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&lport), P(&pcfg));
    if (rv == SHR_E_NOT_FOUND) {
        SHR_EXIT();
    } else {
        SHR_IF_ERR_EXIT
            (bcmpc_pmgr_phy_control_set(unit, port_lane, pport, phy_cfg));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_internal_phy_control_update(int unit, bcmpc_port_lane_t *port_lane,
                                  bcmpc_phy_control_t *phy_cfg)
{
    int rv;
    bcmpc_lport_t lport;
    bcmpc_pport_t pport;
    bcmpc_port_cfg_t pcfg;

    SHR_FUNC_ENTER(unit);

    lport = port_lane->lport;
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmpc_port_phys_map_get(unit, lport, &pport), SHR_E_NOT_FOUND);

    rv = bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&lport), P(&pcfg));
    if (rv == SHR_E_NOT_FOUND) {
        SHR_EXIT();
    } else {
        SHR_IF_ERR_EXIT
            (bcmpc_pmgr_phy_control_set(unit, port_lane, pport, phy_cfg));
    }

exit:
    SHR_FUNC_EXIT();
}

void
bcmpc_pfc_ctrl_t_init(bcmpc_pfc_control_t *data)
{
    sal_memset(data, 0, sizeof(*data));
}

int
bcmpc_internal_pfc_ctrl_insert(int unit, bcmpc_lport_t lport, bcmpc_pfc_control_t *pctrl)
{
    int rv;
    bcmpc_pport_t pport;
    bcmpc_port_cfg_t pcfg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmpc_port_phys_map_get(unit, lport, &pport), SHR_E_NOT_FOUND);

    rv = bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&lport), P(&pcfg));
    if (rv == SHR_E_NOT_FOUND) {
        SHR_EXIT();
    } else {
        SHR_IF_ERR_EXIT
            (bcmpc_pmgr_pfc_control_set(unit, lport, pport, pctrl));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_internal_pfc_ctrl_update(int unit, bcmpc_lport_t lport, bcmpc_pfc_control_t *pctrl)
{
    int rv;
    bcmpc_pport_t pport;
    bcmpc_port_cfg_t pcfg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmpc_port_phys_map_get(unit, lport, &pport), SHR_E_NOT_FOUND);

    rv = bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&lport), P(&pcfg));
    if (rv == SHR_E_NOT_FOUND) {
        SHR_EXIT();
    } else {
        SHR_IF_ERR_EXIT
            (bcmpc_pmgr_pfc_control_set(unit, lport, pport, pctrl));
    }

exit:
    SHR_FUNC_EXIT();
}

