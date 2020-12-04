/*! \file bcmpc_pm_xlport.h
 *
 * XLPORT driver.
 *
 * A XLPORT contains 1 CDPORT and 2 CDMACs, and supports 8 ports at most.
 *
 * In this driver, we always use the port number 0~7 to access the XLPORT
 * and CDMAC per-port registers without considering which CDMAC should be
 * configured.
 *
 * The lower level register access driver would know how to determine which
 * CDMAC should be accessed, i.e. port 0~3 belongs the first CDMAC and port 4~7
 * belongs to the second CDMAC.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPC_PM_XLPORT_H
#define BCMPC_PM_XLPORT_H

#include <bcmpc/bcmpc_pm_drv_types.h>
#include <bcmpmac/bcmpmac_util_internal.h>


/*******************************************************************************
 * Local definitions
 */



/******************************************************************************
 * Private functions
 */

static inline int
xlport_pm_version_get(bcmpc_pm_access_t *pa, pm_pport_t port,
                      pm_version_info_t *info)
{
    return SHR_E_NONE;

}

static inline int
xlport_port_mode_set(bcmpc_pm_access_t *pa, pm_pport_t port,
                     pm_info_t *pm_info, pm_port_mode_info_t *mode)
{
    return SHR_E_NONE;
}

static inline int
xlport_port_mode_get(bcmpc_pm_access_t *pa, pm_pport_t port,
                     pm_info_t *pm_info, pm_port_mode_info_t *mode)
{
    return SHR_E_NONE;
}

static inline int
xlport_mac_control_set(bcmpc_pm_access_t *pa, pm_pport_t port,
                       pm_info_t *pm_info, pm_mac_control_info_t mac_control,
                       int enable)
{
    return SHR_E_NONE;
}

static inline int
xlport_mac_control_get(bcmpc_pm_access_t *pa, pm_pport_t port,
                       pm_info_t *pm_info, pm_mac_control_info_t mac_control,
                       int *enable)
{
    return SHR_E_NONE;
}

static inline int
xlport_tsc_pll_lock_status_get(bcmpc_pm_access_t *pa, pm_pport_t port,
                               pm_tsc_pll_lock_status_t *pll_status)
{
    return SHR_E_NONE;
}

static inline int
xlport_port_tsc_control_set(bcmpc_pm_access_t *pa, pm_pport_t pm_pport,
                            uint32_t flags, pm_tsc_control_t *tsc_control)
{
    return SHR_E_NONE;
}

static inline int
xlport_port_tsc_control_get(bcmpc_pm_access_t *pa, pm_pport_t pm_pport,
                            uint32_t flags, pm_tsc_control_t *tsc_control)
{
    return SHR_E_NONE;
}

static inline int
xlport_port_tsc_lane_status_get(bcmpc_pm_access_t *pa,
                                pm_pport_t pm_pport,
                                pm_tsc_lane_status_info_t *lane_status)
{
    return SHR_E_NONE;
}

static inline int
xlport_port_lag_failover_config_set(bcmpc_pm_access_t *pa,
                                    pm_pport_t pm_pport,
                                    bool toggle, int enable)
{
    return SHR_E_NONE;
}

static inline int
xlport_port_lag_failover_config_get(bcmpc_pm_access_t *pa,
                                    pm_pport_t pm_pport,
                                    int *enable)
{
    return SHR_E_NONE;
}

static inline int
xlport_port_link_fault_config_set(bcmpc_pm_access_t *pa,
                                  pm_pport_t pm_pport,
                                  uint32_t flags, int enable)
{
    return SHR_E_NONE;
}

static inline int
xlport_port_link_fault_config_get(bcmpc_pm_access_t *pa,
                                  pm_pport_t pm_pport,
                                  uint32_t flags, int *enable)
{
    return SHR_E_NONE;
}
#endif /* BCMPC_PM_XLPORT_H */

