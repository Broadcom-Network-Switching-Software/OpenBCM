/*! \file bcmpc_pm_port_drv.h
 *
 * PortMacro Port Block Manager APIs.
 *
 * Declaration of the structures, enumerations, and functions to implement
 * the PortMacro Port block manager.
 *
 * The port block in the PortMacro provides controls and status from MAC
 * TSC cores. It also provides interface to the interrupts for different
 * blocks in the PortMacro. May also provide controls for MIB operations.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPC_PM_PORT_DRV_H
#define BCMPC_PM_PORT_DRV_H

#include <bcmpc/bcmpc_pm_acc.h>
#include <bcmpc/bcmpc_pm.h>

/******************************************************************************
 * PortMacro Port block driver functions
 */

/*!
 * \brief Configure the port mode of a PortMacro.
 *
 * This function is used to configure the port mode.
 * Used to specify the number of ports enabled and
 * the number of lanes used by the ports.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] init 1 to initialize; 0 to clean up.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*pm_version_get_f)(bcmpc_pm_access_t *pa, pm_pport_t pm_pport,
                    pm_version_info_t *pm_version);

/*!
 * \brief Set Port reset state.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] reset 1 to reset; 0 to out of reset.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*pm_port_mode_set_f)(bcmpc_pm_access_t *pa, pm_pport_t pm_pport,
                      pm_info_t *pm_info, pm_port_mode_info_t *mode);

/*!
 * \brief Get Port reset state.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] reset 1 to reset; 0 to out of reset.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*pm_port_mode_get_f)(bcmpc_pm_access_t *pa, pm_pport_t pm_pport,
                      pm_info_t *pm_info, pm_port_mode_info_t *mode);


/*!
 * \brief This function is used to set MAC controls.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] control MAC property to be enabled or disabled.
 * \param [in] enable  1 to enable; 0 to disable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*pm_port_mac_control_set_f)(bcmpc_pm_access_t *pa, pm_pport_t pm_pport,
                             pm_info_t *pm_info, pm_mac_control_info_t control,
                             int enable);

/*!
 * \brief This function is used to get the current state of
 *  MAC controls.
 *
 * \param [in] pa       - PMAC access info.
 * \param [in] pm_pport - Port macro physical port number.
 * \param [in] control  - MAC property to be enabled or disabled.
 * \param [out] enable  - returns 1 (enable) or 0 (disable).
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*pm_port_mac_control_get_f)(bcmpc_pm_access_t *pa, pm_pport_t pm_pport,
                             pm_info_t *pm_info, pm_mac_control_info_t control,
                             int *enable);

/*!
 * \brief This function is used to get TSC lock status.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] pll_lock_status returns the pll
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*pm_port_tsc_pll_lock_status_get_f)(bcmpc_pm_access_t *pa,
                                     pm_pport_t pm_pport,
                                     pm_tsc_pll_lock_status_t *pll_lock_status);

/*!
 * \brief This function is used to set TSC controls.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] flags Port macro physical port number.
 * \param [in] tsc_controls  Control to enable or disable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*pm_port_tsc_control_set_f)(bcmpc_pm_access_t *pa, pm_pport_t pm_pport,
                             uint32_t flags, pm_tsc_control_t *tsc_control);

/*!
 * \brief This function is used to get TSC controls.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] flags Port macro physical port number.
 * \param [out] tsc_controls  Control to enable or disable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*pm_port_tsc_control_get_f)(bcmpc_pm_access_t *pa, pm_pport_t pm_pport,
                             uint32_t flags, pm_tsc_control_t *tsc_control);

/*!
 * \brief This function is used to get individual TSC lane status.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port  macro physical port number.
 * \param [out] lane_status - individual lane status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*pm_port_tsc_lane_status_get_f)(bcmpc_pm_access_t *pa,
                                 pm_pport_t pm_pport,
                                 pm_tsc_lane_status_info_t *lane_status);

/*!
 * \brief This function is used to configure lag failover.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport - Port  macro physical port number.
 * \param [in] toggle - toggle the current state.
 * \param [in] enable - enable or disable the lag failover.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*pm_port_lag_failover_config_set_f)(bcmpc_pm_access_t *pa,
                                     pm_pport_t pm_pport,
                                     bool toggle, int enable);

/*!
 * \brief This function is used to get lag failover.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport - Port  macro physical port number.
 * \param [out] enable - enable or disable the lag failover.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*pm_port_lag_failover_config_get_f)(bcmpc_pm_access_t *pa,
                                     pm_pport_t pm_pport,
                                     int *enable);

/*!
 * \brief This function is used to configure the behavior
 * on local faults.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport - Port  macro physical port number.
 * \param [in] flags - Port  macro physical port number.
 * \param [in] enable - enable or disable the link fault
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*pm_port_link_fault_config_set_f)(bcmpc_pm_access_t *pa,
                                          pm_pport_t pm_pport,
                                          uint32_t flags, int enable);

/*!
 * \brief This function is used to get link fault behavior.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport - Port  macro physical port number.
 * \param [in] flags - Port  macro physical port number.
 * \param [out] enable - enable or disable the link fault
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*pm_port_link_fault_config_get_f)(bcmpc_pm_access_t *pa,
                                          pm_pport_t pm_pport,
                                          uint32_t flags, int *enable);

#if 0
typedef int
(*pm_port_interrupt_enable_set_f)(bcmpc_pm_access_t *pa,
                                  pm_pport_t pm_pport,

typedef int
(*pm_port_interrupt_enable_get_f)(bcmpc_pm_access_t *pa,
                                  pm_pport_t pm_pport,

typedef int
(*pm_port_interrupt_status_get_f)(bcmpc_pm_access_t *pa,
                                  pm_pport_t pm_pport,
#endif

/*******************************************************************************
 * PortMacro Port block driver object
 */

/*!
 * \brief PortMacro Port block driver.
 *
 * The PortMacro (PM) Port block driver is used to control/configure
 * global settings of MAC and TSC cores of a PortMacro.
 * Examples of some of the operations include
 *      Configuring the port mode of a PortMacro.
 *      Enable/Disable individual ports of a PortMacro.
 *      Power On/Off the TSC core.
 *      Setting active PLL of a TSC core (if PortMacro supports multiple PLLs).
 *
 * Some of the operations are specific to a Portmacro type.
 */
typedef struct pm_port_drv_s {

    /*! Driver name. */
    const char *name;

    /*! PM version set. */
    pm_version_get_f pm_version_get;

    /*! PM Core mode set. */
    pm_port_mode_set_f pm_port_mode_set;

    /*! PM Core mode get. */
    pm_port_mode_get_f pm_port_mode_get;

    /*! PortMacro MAC Core control set. */
    pm_port_mac_control_set_f pm_port_mac_control_set;

    /*! PortMacro MAC Core control get. */
    pm_port_mac_control_get_f pm_port_mac_control_get;

    /*! PortMacro TSC Core PLL lock status get. */
    pm_port_tsc_pll_lock_status_get_f  pm_port_tsc_pll_lock_status_get;

    /*! Controls for PortMacro TSC Core. */
    pm_port_tsc_control_set_f pm_port_tsc_control_set;

    /*! Controls for PortMacro TSC Core. */
    pm_port_tsc_control_get_f pm_port_tsc_control_get;

    /*! PortMacro TSC Core PLL lock status get. */
    pm_port_tsc_lane_status_get_f  pm_port_tsc_lane_status_get;

    /*! PortMacro  lag failover config set. */
    pm_port_lag_failover_config_set_f  pm_port_lag_failover_config_set;

    /*! PortMacro  lag failover config get. */
    pm_port_lag_failover_config_get_f  pm_port_lag_failover_config_get;

    /*! PortMacro port link fault status control set. */
    pm_port_link_fault_config_set_f  pm_port_link_fault_config_set;

    /*! PortMacro port link fault status control get. */
    pm_port_link_fault_config_get_f  pm_port_link_fault_config_get;

} pm_port_drv_t;


/*******************************************************************************
 * PM driver instance
 */

/*! PM4x10 PM XLPort driver. */
extern pm_port_drv_t pm4x10_xlport_drv;

/*! PM4x25 PM CLPort driver. */
extern pm_port_drv_t pm4x25_clport_drv;

/*! PM8x50 PM CDPort driver. */
extern pm_port_drv_t pm8x50_cdport_drv;

#endif /* BCMPC_PM_PORT_DRV_H */
