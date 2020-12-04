/*! \file bcmpc_pm_lane.c
 *
 * BCMPC PM Lane APIs
 *
 * This file implements the functions for configuring the lanes within a PM.
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
bcmpc_pmd_firmware_config_t_init(bcmpc_pmd_firmware_config_t *pmdf_cfg)
{
    sal_memset(pmdf_cfg, 0, sizeof(*pmdf_cfg));
}

int
bcmpc_internal_pmd_firmware_config_insert(int unit,
                                          bcmpc_lport_t lport,
                                          bcmpc_pmd_firmware_config_t *pmd_cfg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpc_pmgr_pmd_firmware_config_set(unit, lport, pmd_cfg));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_internal_pmd_firmware_config_update(int unit,
                                          bcmpc_lport_t lport,
                                          bcmpc_pmd_firmware_config_t *pmd_cfg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpc_pmgr_pmd_firmware_config_set(unit, lport, pmd_cfg));

exit:
    SHR_FUNC_EXIT();
}

void
bcmpc_tx_taps_config_t_init(bcmpc_tx_taps_config_t *taps_cfg)
{
    sal_memset(taps_cfg, 0, sizeof(*taps_cfg));
}


int
bcmpc_internal_tx_taps_config_insert(int unit, bcmpc_port_lane_t *port_lane,
                                     bcmpc_tx_taps_config_t *tx_cfg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpc_pmgr_tx_taps_config_set(unit, port_lane, tx_cfg));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_internal_tx_taps_config_update(int unit, bcmpc_port_lane_t *port_lane,
                                     bcmpc_tx_taps_config_t *tx_cfg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpc_pmgr_tx_taps_config_set(unit, port_lane, tx_cfg));

exit:
    SHR_FUNC_EXIT();
}

