/*! \file bcmpc_pm_drv_internal.h
 *
 * BCMPC PM Driver Object.
 *
 * The PM driver is responsible for handling the PM-specific abilities and
 * limitions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPC_PM_DRV_INTERNAL_H
#define BCMPC_PM_DRV_INTERNAL_H

#include <phymod/phymod.h>
#include <bcmpc/bcmpc_pm.h>
#include <bcmpc/bcmpc_lport_internal.h>
#include <bcmpmac/bcmpmac_types.h>

/*!
 * \brief Take the portmacro information database lock.
 *
 * \param [in] _p Portmacro database structure (pm_info_t).
 *
 * \returns None.
 */
#define PM_LOCK(_p) \
            (sal_mutex_take(_p->pm_lock, SAL_MUTEX_FOREVER))

/*!
 * \brief Give the portmacro information database lock.
 *
 * \param [in] _p Portmacro database structure (pm_info_t).
 *
 * \returns None.
 */
#define PM_UNLOCK(_p) \
            (sal_mutex_give(_p->pm_lock))

/*******************************************************************************
 * PM driver functions
 */

/*!
 * \brief Speed configuration.
 *
 * This structure groups the speed-related parameters.
 *
 * The PM driver, \ref bcmpc_pm_drv_t, would use all or partial fields of
 * this structure to validate the given speed configuration.
 *
 * \param [in] unit Unit number.
 */
typedef struct bcmpc_speed_cfg_s {

    /*! Number of lanes. */
    uint32_t num_lanes;

    /*! Speed in Mbps. */
    uint32_t speed;

    /*! FEC mode. */
    bcmpc_fec_t fec;

} bcmpc_speed_cfg_t;


/*!
 * \name PM driver functions
 *
 * The function prototypes for \ref bcmpc_pm_drv_t.
 */

/*! \{ */

/*!
 * \brief PM operating mode validation function pointer type.
 *
 * This function is used to validate the PM-specific operating mode.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_mode PM mode.
 *
 * \retval SHR_E_NONE The \c pm_mode is valid.
 * \retval SHR_E_FAIL The \c pm_mode is invalid.
 */
typedef int
(*bcmpc_pm_mode_validate_f)(int unit, bcmpc_pm_mode_t *pm_mode);

/*!
 * \brief Port speed configuration validation function.
 *
 * This function is used to validate the PM-specific port speed value based on
 * the number of lanes which are used by the port.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [in] speed_cfg Speed configuration.
 *
 * \retval SHR_E_NONE The \c speed is valid.
 * \retval SHR_E_FAIL The \c speed is invalid.
 */
typedef int
(*bcmpc_pm_port_speed_config_validate_f)(int unit, const pm_access_t *pm_acc,
                                         pm_oper_status_t *op_st,
                                         pm_info_t *pm_info,
                                         pm_speed_config_t *speed_cfg);

/*!
 * \brief Port lane map validation function.
 *
 * \param [in] unit Unit number.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [in] lane_mask Port configured lane mask.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_port_lane_map_validate_f)(int unit,
                                     pm_oper_status_t *op_st,
                                     pm_info_t *pm_info, int lane_mask);

/*!
 * \brief PM port attach function.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [in] add_info Port configuration information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_core_attach_f)(int unit, const pm_access_t *pm_acc,
                          pm_oper_status_t *op_st,
                          pm_info_t *pm_info,
                          pm_port_add_info_t *add_info);

/*!
 * \brief PM port detach function.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_port_detach_f)(int unit, const pm_access_t *pm_acc,
                          pm_oper_status_t *op_st, pm_info_t *pm_info);

/*!
 * \brief PM destroy function, called during exit to free up
 * internal resources allocated for port macro.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_info PM state structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_destroy_f)(int unit, pm_info_t *pm_info);

/*!
 * \brief PM port add function.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_info PM state structure.
 * \param [in] add_info Port configuration information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_port_add_f)(int unit, const pm_access_t *pm_acc,
                       pm_oper_status_t *op_st, pm_info_t *pm_info,
                       pm_port_add_info_t *add_info);

/*!
 * \brief PM port speed set function.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [in] speed_config Port speed configuration information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_port_speed_set_f)(int unit, const pm_access_t *pm_acc,
                             pm_oper_status_t *op_st, pm_info_t *pm_info,
                             pm_speed_config_t *speed_config);

/*!
 * \brief PM port speed get function.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [out] speed_config Port speed configuration information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_port_speed_get_f)(int unit, const pm_access_t *pm_acc,
                             pm_oper_status_t *op_st, pm_info_t *pm_info,
                             pm_speed_config_t *speed_config);

/*!
 * \brief PM port configuration validate function.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [in] speed_config Port speed configuration information.
 * \param [in] is_first The port is the first port created or not.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_port_config_validate_f)(int unit, const pm_access_t *pm_acc,
                                   pm_oper_status_t *op_st, pm_info_t *pm_info,
                                   pm_speed_config_t *speed_config, int is_first);

/*!
 * \brief PM port looback set function.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [in] loopback_type Supported looback type.
 * \param [in] enable Enable/disable loopback function.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_port_loopback_set_f)(int unit, const pm_access_t *pm_acc,
                                pm_oper_status_t *op_st, pm_info_t *pm_info,
                                pm_loopback_mode_t loopback_type, int enable);

/*!
 * \brief PM port looback get function.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [in] loopback_type Supported looback type.
 * \param [in] enable loopback enable status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_port_loopback_get_f)(int unit, const pm_access_t *pm_acc,
                                pm_oper_status_t *op_st, pm_info_t *pm_info,
                                pm_loopback_mode_t loopback_type, int *enable);

/*!
 * \brief PM port autonegotiation advertising abilities set function.
 *
 * \param [in] unit          Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info       PM state structure.
 * \param [in] num_abilities Number of instances in the ability array.
 * \param [in] ability       Ability array.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval SHR_E_FAIL   Failure.
 * \retval SHR_E_UNAVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_port_ability_advert_set_f)(int unit, const pm_access_t *pm_acc,
                                      pm_oper_status_t *op_st,
                                      pm_info_t *pm_info,
                                      int num_abilities,
                                      pm_port_ability_t *ability);

/*!
 * \brief PM port autonegotiation advertising abilities get function.
 *
 * \param [in]  unit                 Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in]  pm_info              PM state structure.
 * \param [in]  max_num_abilities    Maximum number of instances
 *                                   can be returned in the ability array.
 * \param [out] actual_num_abilities Number of valid instances returned
 *                                   in the ability array.
 * \param [out] ability              Ability array.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval SHR_E_FAIL   Failure.
 * \retval SHR_E_UNAVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_port_ability_advert_get_f)(int unit, const pm_access_t *pm_acc,
                                      pm_oper_status_t *op_st,
                                      pm_info_t *pm_info,
                                      uint32_t max_num_abilities,
                                      uint32_t *actual_num_abilities,
                                      pm_port_ability_t *ability);

/*!
 * \brief PM port link partner's autonegotiation
 *        advertising abilities get function.
 *
 * \param [in]  unit                 Unit number.
 * \param [in]  pm_acc               MAC and PHY access information structure.
 * \param [out] op_st                Operation status for this function call.
 * \param [in]  pm_info              PM state structure.
 * \param [in]  max_num_abilities    Maximum number of instances
 *                                   can be returned in the ability array.
 * \param [out] actual_num_abilities Number of valid instances returned
 *                                   in the ability array.
 * \param [out] ability              Ability array.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval SHR_E_FAIL   Failure.
 * \retval SHR_E_UNAVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_port_ability_remote_get_f)(int unit, const pm_access_t *pm_acc,
                                      pm_oper_status_t *op_st,
                                      pm_info_t *pm_info,
                                      uint32_t max_num_abilities,
                                      uint32_t *actual_num_abilities,
                                      pm_port_ability_t *ability);

/*!
 * \brief PM port autonegotiation enable/disable function.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in]  pm_info   PM state structure.
 * \param [in] an   Autonegotiation control.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval SHR_E_FAIL   Failure.
 * \retval SHR_E_UNAVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_port_autoneg_set_f)(int unit, const pm_access_t *pm_acc,
                               pm_oper_status_t *op_st, pm_info_t *pm_info,
                               phymod_autoneg_control_t *an);

/*!
 * \brief PM port autonegotiation enable/disable get function.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in]  pm_info   PM state structure.
 * \param [out] an  Autonegotiation control.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval SHR_E_FAIL   Failure.
 * \retval SHR_E_UNAVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_port_autoneg_get_f)(int unit, const pm_access_t *pm_acc,
                               pm_oper_status_t *op_st,
                               pm_info_t *pm_info,
                               phymod_autoneg_control_t *an);

/*!
 * \brief PM port autonegotiation status get function.
 *
 * \param [in]  unit      Unit number.
 * \param [in]  pm_acc      MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in]  pm_info   PM state structure.
 * \param [out] an_status Autonegotiation status.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval SHR_E_FAIL   Failure.
 * \retval SHR_E_UNAVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_port_autoneg_status_get_f)(int unit, const pm_access_t *pm_acc,
                                      pm_oper_status_t *op_st,
                                      pm_info_t *pm_info,
                                      phymod_autoneg_status_t *an_status);

/*!
 * \brief PM port encapsulation set function.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [in] encap Encap type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_port_encap_set_f)(int unit, const pm_access_t *pm_acc,
                             pm_oper_status_t *op_st, pm_info_t *pm_info,
                             pm_encap_t encap);

/*!
 * \brief PM port encapsulation get function.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [out] encap Encap type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_port_encap_get_f)(int unit, const pm_access_t *pm_acc,
                             pm_oper_status_t *op_st, pm_info_t *pm_info,
                             pm_encap_t *encap);

/*!
 * \brief PM port pause set function.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [in] pause_ctrl Pause control configuartion.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_port_pause_ctrl_set_f)(int unit, const pm_access_t *pm_acc,
                                  pm_oper_status_t *op_st, pm_info_t *pm_info,
                                  pm_pause_ctrl_t *pause_ctrl);

/*!
 * \brief PM port pause get function.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [out] pause_ctrl Pause control configuartion.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_port_pause_ctrl_get_f)(int unit, const pm_access_t *pm_acc,
                                  pm_oper_status_t *op_st,
                                  pm_info_t *pm_info,
                                  pm_pause_ctrl_t *pause_ctrl);

/*!
 * \brief PM port pause address set function.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [out] pause_addr Pause MAC address.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_port_pause_addr_set_f)(int unit, const pm_access_t *pm_acc,
                                  pm_oper_status_t *op_st,
                                  pm_info_t *pm_info,
                                  pm_mac_t pause_addr);

/*!
 * \brief PM port pause address get function.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [out] pause_addr Pause MAC address.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_port_pause_addr_get_f)(int unit, const pm_access_t *pm_acc,
                                  pm_oper_status_t *op_st,
                                  pm_info_t *pm_info, pm_mac_t pause_addr);

/*!
 * \brief PM port max frame size set function.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [out] max_frame_size Maximum frame size.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_port_frame_max_set_f)(int unit, const pm_access_t *pm_acc,
                                 pm_oper_status_t *op_st,
                                 pm_info_t *pm_info, uint32_t max_frame_size);

/*!
 * \brief PM port max frame size get function.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [out] max_frame_size Maximum frame size.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_port_frame_max_get_f)(int unit, const pm_access_t *pm_acc,
                                 pm_oper_status_t *op_st,
                                 pm_info_t *pm_info, uint32_t *max_frame_size);

/*!
 * \brief PM port enable status set function.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [in] flags. Indicator to check port enable status
 * \param [out] enable Port enable value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_port_enable_set_f)(int unit, const pm_access_t *pm_acc,
                              pm_oper_status_t *op_st, pm_info_t *pm_info,
                              uint32_t flags, uint32_t enable);

/*!
 * \brief PM port enable status get function.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [in] flags. Indicator to check port enable status
 * \param [out] enable Port enable status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_port_enable_get_f)(int unit, const pm_access_t *pm_acc,
                              pm_oper_status_t *op_st, pm_info_t *pm_info,
                              uint32_t flags, uint32_t *enable);

/*!
 * \brief PM port link status get function.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [out] link_status Port link status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_port_link_status_get_f)(int unit, const pm_access_t *pm_acc,
                                   pm_oper_status_t *op_st,
                                   pm_info_t *pm_info, uint32_t *link_status);

/*!
 * \brief PM port lag failover lookback get function.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [out] enable Failover looback enable status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_port_lag_failover_lpbk_get_f)(int unit, const pm_access_t *pm_acc,
                                         pm_oper_status_t *op_st,
                                         pm_info_t *pm_info, uint32_t *enable);

/*!
 * \brief PM port tx lane config set function.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [in] tx_config Tx lane config.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_port_tx_set_f)(int unit, const pm_access_t *pm_acc,
                          pm_oper_status_t *op_st, pm_info_t *pm_info,
                          phymod_tx_t *tx_config);

/*!
 * \brief PM port tx taps status get function.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [in] tx_config Tx lane config.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_port_tx_get_f)(int unit, const pm_access_t *pm_acc,
                          pm_oper_status_t *op_st, pm_info_t *pm_info,
                          phymod_tx_t *tx_config);

/*!
 * \brief PM port PMD lane config set function.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [in] fw_lane_config Firmware lane config.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_port_pmd_lane_cfg_set_f)(int unit, const pm_access_t *pm_acc,
                               pm_oper_status_t *op_st, pm_info_t *pm_info,
                               phymod_firmware_lane_config_t *fw_lane_config);

/*!
 * \brief PM port PMD lane config get function.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [out] fw_lane_config Firmware lane config.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_port_pmd_lane_cfg_get_f)(int unit, const pm_access_t *pm_acc,
                                pm_oper_status_t *op_st,
                                pm_info_t *pm_info,
                                phymod_firmware_lane_config_t *fw_lane_config);

/*!
 * \brief Set LAG failover enable state.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [in] enable 1 to enable, otherwise disable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_port_failover_enable_set_f)(int unit, const pm_access_t *pm_acc,
                                       pm_oper_status_t *op_st,
                                       pm_info_t *pm_info, uint32_t enale);

/*!
 * \brief Get LAG failover enable state.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [out] enable 1 for enabled, otherwise disabled.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_port_failover_enable_get_f)(int unit, const pm_access_t *pm_acc,
                                       pm_oper_status_t *op_st,
                                       pm_info_t *pm_info, uint32_t *enale);

/*!
 * \brief Set port link training enabled .
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [in] enable 1 to enable, otherwise disable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_port_link_training_set_f)(int unit, const pm_access_t *pm_acc,
                                     pm_oper_status_t *op_st,
                                     pm_info_t *pm_info, uint32_t enale);

/*!
 * \brief Get port link training enable state.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [out] enable 1 for enabled, otherwise disabled.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_port_link_training_get_f)(int unit, const pm_access_t *pm_acc,
                                     pm_oper_status_t *op_st,
                                     pm_info_t *pm_info, uint32_t *enale);

/*!
 * \brief Set MAC tx enabled.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [out] enable 1 to enable, otherwise disable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_mac_tx_enable_set_f)(int unit, const pm_access_t *pm_acc,
                                pm_oper_status_t *op_st,
                                pm_info_t *pm_info, uint32_t tx_enable);

/*!
 * \brief Set MAC rx enabled.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [out] enable 1 to enable, otherwise disable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_mac_rx_enable_set_f)(int unit, const pm_access_t *pm_acc,
                                pm_oper_status_t *op_st,
                                pm_info_t *pm_info, uint32_t rx_enable);

/*!
 * \brief Get mac tx/rx enable state.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [out] tx_enable 1 for tx enabled, otherwise disabled.
 * \param [out] rx_enable 1 for rx enabled, otherwise disabled.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_mac_enable_get_f)(int unit, const pm_access_t *pm_acc,
                             pm_oper_status_t *op_st, pm_info_t *pm_info,
                             uint32_t *tx_enable, uint32_t *rx_enable);

/*!
 * \brief Enable mac local, remote fault.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [out] dis Fault data structure for fault state.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_mac_fault_disable_set_f)(int unit, const pm_access_t *pm_acc,
                                    pm_oper_status_t *op_st,
                                    pm_info_t *pm_info,
                                    bcmpmac_fault_disable_t *dis);
/*!
 * \brief Get mac fault state.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [out] st Fault data structure for fault state.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_mac_fault_status_get_f)(int unit, const pm_access_t *pm_acc,
                                   pm_oper_status_t *op_st,
                                   pm_info_t *pm_info,
                                   bcmpmac_fault_status_t *st);

/*!
 * \brief Get mac fault disable state.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [out] st Fault disable data structure for fault disable state.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_mac_fault_disable_get_f)(int unit, const pm_access_t *pm_acc,
                                    pm_oper_status_t *op_st,
                                    pm_info_t *pm_info,
                                    bcmpmac_fault_disable_t *st);

/*!
 * \brief Set MAC average inter packet gap size.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [in] ipg_size Average IPG size.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_mac_avg_ipg_set_f)(int unit, const pm_access_t *pm_acc,
                              pm_oper_status_t *op_st, pm_info_t *pm_info,
                              uint8_t ipg_size);
/*!
 * \brief Get MAC average inter packet gap size.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [out] ipg_size Average IPG size.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_mac_avg_ipg_get_f)(int unit, const pm_access_t *pm_acc,
                              pm_oper_status_t *op_st, pm_info_t *pm_info,
                              uint8_t *ipg_size);

/*!
 * \brief Set pfc configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [in] pctrl PFC configuraiton.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_pfc_set_f)(int unit, const pm_access_t *pm_acc,
                      pm_oper_status_t *op_st, pm_info_t *pm_info,
                      bcmpmac_pfc_t *pctrl);

/*!
 * \brief Get pfc configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [out] pctrl PFC configuraiton.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_pfc_get_f)(int unit, const pm_access_t *pm_acc,
                      pm_oper_status_t *op_st, pm_info_t *pm_info,
                      bcmpmac_pfc_t *pctrl);

/*!
 * \brief Get phy status.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [out] phy_stat Phy status structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_phy_status_get_f)(int unit, const pm_access_t *pm_acc,
                             pm_oper_status_t *op_st, pm_info_t *pm_info,
                             pm_phy_status_t *phy_stat);

/*!
 * \brief Set phy configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [in] flags PM control flags.
 * \param [out] pm_ctrl PM phy control structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_phy_control_set_f)(int unit, const pm_access_t *pm_acc,
                              pm_oper_status_t *op_st, pm_info_t *pm_info,
                              uint32_t flags, pm_phy_control_t *pm_ctrl);

/*!
 * \brief Set port MIB oversize value.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [in] size MIB oversize value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_mib_oversize_set_f)(int unit, const pm_access_t *pm_acc,
                               pm_oper_status_t *op_st,
                               pm_info_t *pm_info, uint32_t size);

/*!
 * \brief Get port MIB oversize value.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [out] size MIB oversize value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_mib_oversize_get_f)(int unit, const pm_access_t *pm_acc,
                               pm_oper_status_t *op_st,
                               pm_info_t *pm_info, uint32_t *size);

/*!
 * \brief PM port abilities get function.
 *
 * \param [in]  unit                 Unit number.
 * \param [in]  pm_acc               MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in]  pm_info              PM state structure.
 * \param [int] ability_type         Port ability_type.
 * \param [int] num_abilities        Number of port abilities returned.
 * \param [out] pabilities           Port ability array.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval SHR_E_FAIL   Failure.
 * \retval SHR_E_UNAVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_port_abilities_get_f)(int unit, const pm_access_t *pm_acc,
                                 pm_oper_status_t *op_st,
                                 pm_info_t *pm_info,
                                 pm_port_ability_type_t ability_type,
                                 uint32_t *num_abilities,
                                 pm_port_ability_t *pabilities);

/*!
 * \brief Set control frames(ethertype 0x8808) handling in the switch.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [in] enable  1 - Control frames passed to system side.
 *                     0 - Control frames dropped in MAC.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_control_frames_pass_set_f)(int unit, const pm_access_t *pm_acc,
                                      pm_oper_status_t *op_st,
                                      pm_info_t *pm_info, uint32_t enable);

/*!
 * \brief Get control frames(ethertype 0x8808) handling in the switch.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [out] enable  1 - Control frames are passed to system side.
 *                     0 - Control frames are dropped in MAC.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_control_frames_pass_get_f)(int unit, const pm_access_t *pm_acc,
                                      pm_oper_status_t *op_st,
                                      pm_info_t *pm_info, uint32_t *enable);

/*!
 * \brief Set pause frame handling in the switch.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [in] enable  1 - Pause frames passed to system side.
 *                     0 - Pause frames dropped in MAC.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_pause_frames_pass_set_f)(int unit, const pm_access_t *pm_acc,
                                    pm_oper_status_t *op_st,
                                    pm_info_t *pm_info, uint32_t enable);

/*!
 * \brief Set pause frame handling in the switch.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [out] enable  1 - Pause frames are passed to system side.
 *                      0 - Pause frames are dropped in MAC.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_pause_frames_pass_get_f)(int unit, const pm_access_t *pm_acc,
                                    pm_oper_status_t *op_st,
                                    pm_info_t *pm_info, uint32_t *enable);

/*!
 * \brief Power on/off PORT TSC RX and TX data path.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [in] power_on Set to power on TSC, otherwise, power off TSC.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_tsc_ctrl_f)(int unit, const pm_access_t *pm_acc,
                       pm_oper_status_t *op_st, pm_info_t *pm_info,
                       int power_on);

/*!
 * \brief Get PM vco rate.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [out] vco_rate Current PM vco rates.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_vco_get_f)(int unit, const pm_access_t *pm_acc,
                      pm_oper_status_t *op_st, pm_info_t *pm_info,
                      pm_vco_t *vco_rate);

/*!
 * \brief Set timesync configuration for a given logical port.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [in] config Timesync configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_port_timesync_config_set_f)(int unit, const pm_access_t *pm_acc,
                                       pm_oper_status_t *op_st,
                                       pm_info_t *pm_info,
                                       pm_phy_timesync_config_t *config);

/*!
 * \brief Set SyncE clock mode for a given logical port.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [in] cfg SyncE clock mode configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_port_synce_clk_ctrl_set_f)(int unit, const pm_access_t *pm_acc,
                                      pm_oper_status_t *op_st,
                                      pm_info_t *pm_info,
                                      pm_port_synce_clk_ctrl_t *cfg);

/*!
 * \brief Get SyncE clock mode for a given logical port.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [out] cfg SyncE clock mode configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_port_synce_clk_ctrl_get_f)(int unit, const pm_access_t *pm_acc,
                                      pm_oper_status_t *op_st,
                                      pm_info_t *pm_info,
                                      pm_port_synce_clk_ctrl_t *cfg);

/*!
 * \brief Port link up event handler.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_port_phy_link_up_event_f)(int unit, const pm_access_t *pm_acc,
                                     pm_oper_status_t *op_st,
                                     pm_info_t *pm_info);

/*!
 * \brief Port link down event handler.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_port_phy_link_down_event_f)(int unit, const pm_access_t *pm_acc,
                                       pm_oper_status_t *op_st,
                                       pm_info_t *pm_info);

/*!
 * \brief PM port sw state update function.
 *
 * \param [in]  unit                 Unit number.
 * \param [in]  pm_acc               MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in]  pm_info              PM state structure.
 * \retval SHR_E_NONE   No errors.
 * \retval SHR_E_FAIL   Failure.
 * \retval SHR_E_UNAVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_port_sw_state_update_f)(int unit, const pm_access_t *pm_acc,
                                   pm_oper_status_t *op_st,
                                   pm_info_t *pm_info);
/*!
 * \brief Set runt threshold on a port.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [in] value Size of packet below which is it marked as runt.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_runt_threshold_set_f)(int unit, const pm_access_t *pm_acc,
                                 pm_oper_status_t *op_st,
                                 pm_info_t *pm_info, uint32_t value);

/*!
 * \brief Get runt threshold on a port.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [out] value Size of packet below which is it marked as runt.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_runt_threshold_get_f)(int unit, const pm_access_t *pm_acc,
                                 pm_oper_status_t *op_st,
                                 pm_info_t *pm_info, uint32_t *value);

/*!
 * \brief configure RLM on port.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [in] rlm_config RLM config.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_port_rlm_config_set_f)(int unit, const pm_access_t *pm_acc,
                                  pm_oper_status_t *op_st, pm_info_t *pm_info,
                                  pm_port_rlm_config_t *rlm_config);

/*!
 * \brief PM init.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [in] pm_info PM state structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_init_f)(int unit, const pm_access_t *pm_acc, pm_info_t *pm_info);

/*!
 * \brief Get port link training status.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [out] locked 1 for training locked, otherwise training is not locked.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_port_link_training_status_get_f)(int unit, const pm_access_t *pm_acc,
                                            pm_oper_status_t *op_st,
                                            pm_info_t *pm_info,
                                            uint32_t *locked);

/*!
 * \brief Get Reduce Lane Mode (RLM) status for a logical port.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operatoin status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [out] rlm_status RLM status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNAVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_port_rlm_status_get_f)(int unit, const pm_access_t *pm_acc,
                                  pm_oper_status_t *op_st,
                                  pm_info_t *pm_info,
                                  pm_port_rlm_status_t *rlm_status);

/*!
 * \brief Calculate VCO rate for the list of speeds configured on the
 *        Port Macro.
 *
 * \param [in] unit Unit number.
 * \param [out] op_st Operatoin status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [out] vco_select VCO rate selected based on speed list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNAVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_vco_rate_calculate_f)(int unit, pm_oper_status_t *op_st,
                                pm_info_t *pm_info,
                                pm_vco_setting_t *vco_select);

/*!
 * \brief PM port default PMD firmware lane configuration get.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [in] speed_config Port speed configuration information.
 * \param [out] lane_config Port PMD firmware lane configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_default_pmd_lane_config_get_f)(int unit, const pm_access_t *pm_acc,
                                    pm_oper_status_t *op_st, pm_info_t *pm_info,
                                    pm_speed_config_t *speed_config,
                                    phymod_firmware_lane_config_t *lane_config);

/*!
 * \brief Get PM tSC Core TX/X polarity config.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [out] polarity TX,RX polarity flip structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_phy_polarity_get_f)(int unit, const pm_access_t *pm_acc,
                               pm_oper_status_t *op_st, pm_info_t *pm_info,
                               phymod_polarity_t *polarity);

/*!
 * \brief PM port default TX TAPs configuration get.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [in] speed_config Port speed configuration information.
 * \param [out] tx_taps_config Port TX TAPs configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_default_tx_taps_config_get_f)(int unit, const pm_access_t *pm_acc,
                                         pm_oper_status_t *op_st, pm_info_t *pm_info,
                                         pm_speed_config_t *speed_config,
                                         phymod_tx_t *tx_taps_config);

/*!
 * \brief Get PM TSC Core TX/X lane map settings
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [out] lane_map TX,RX lane map structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_core_lane_map_get_f)(int unit, const pm_access_t *pm_acc,
                                pm_oper_status_t *op_st, pm_info_t *pm_info,
                                phymod_lane_map_t *lane_map);

/*!
 * \brief Enable/Disable transmission of traffic on a port.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [in] enable 1 - Stall the transmission of traffic.
 *                    0 - TX on port enabled.
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_stall_tx_set_f)(int unit, const pm_access_t *pm_acc,
                           pm_oper_status_t *op_st, pm_info_t *pm_info,
                           bool enable);

/*!
 * \brief Get stall transmission feature status on a port.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [out] enable 1 - Stall TX enabled on a port.
 *                     0 - Disabled.
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pm_stall_tx_get_f)(int unit, const pm_access_t *pm_acc,
                           pm_oper_status_t *op_st, pm_info_t *pm_info,
                           bool *enable);

/*!
 * \brief Get port MAC Receive Statistic Vector (RSV).
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [in] flags Bitmask to indicate the vectors to set.
 * \param [out] value  Bitmask value.
 *                    1 - Indicates enable (Pass the packet).
 *                        Value in mask is set to 0.
 *                    0 - Indicates disable (Purge the packet).
 *                        Value in mask is set to 1.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_port_mac_rsv_mask_get_f)(int unit, const pm_access_t *pm_acc,
                                 pm_oper_status_t *op_st, pm_info_t *pm_info,
                                 uint32_t flags, uint32_t *value);

/*!
 * \brief Set port MAC Receive Statistic Vector (RSV).
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [in] flags Bitmask to indicate the vectors to set.
 * \param [in] value  Bitmask value.
 *                    1 - Enable (Pass the packet).
 *                        Value in mask is set to 0.
 *                    0 - Disable (Purge the packet).
 *                        Value in mask is set to 1.
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_port_mac_rsv_mask_set_f)(int unit, const pm_access_t *pm_acc,
                                 pm_oper_status_t *op_st, pm_info_t *pm_info,
                                 uint32_t flags, uint32_t value);

/*!
 * \brief Get port TX packet timestamp from FIFO.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [out] timestamp_info Packet timestamp data structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_port_tx_timestamp_info_get_f)(int unit, const pm_access_t *pm_acc,
                                      pm_oper_status_t *op_st,
                                      pm_info_t *pm_info,
                                      pm_tx_timestamp_info_t *timestamp_info);

/*!
 * \brief Get egress 1588 timestamp mode.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [out] timestamp_mode Timestamp mode enumeration
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpc_egress_timestamp_mode_get_f)(int unit, const pm_access_t *pm_acc,
                                     pm_oper_status_t *op_st,
                                     pm_info_t *pm_info,
                                     pm_egr_timestamp_mode_t *timestamp_mode);

/*!
 * \brief Set egress 1588 timestamp mode.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [in] timestamp_mode Timestamp mode enumeration
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpc_egress_timestamp_mode_set_f)(int unit, const pm_access_t *pm_acc,
                                     pm_oper_status_t *op_st,
                                     pm_info_t *pm_info,
                                     pm_egr_timestamp_mode_t timestamp_mode);

/*!
 * \brief Get RSFEC uncorrectable counter for a port
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [in] pm_info PM state structure.
 * \param [out] FEC uncorrectable counter
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpc_phy_rsfec_uncorrectable_counter_get_f)(int unit, const pm_access_t *pm_acc,
                                               pm_info_t *pm_info,
                                               uint64_t *count);

/*!
 * \brief Get FEC correctable counter for a port
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [in] pm_info PM state structure.
 * \param [out] FEC correctable counter
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpc_phy_rsfec_correctable_counter_get_f)(int unit, const pm_access_t *pm_acc,
                                             pm_info_t *pm_info,
                                             uint64_t *count);

/*!
 * \brief Get RSFEC symbol error counter for a port
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [in] pm_info PM state structure.
 * \param [out] RSFEC symbol error counter
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpc_phy_rsfec_symbol_error_counter_get_f)(int unit, const pm_access_t *pm_acc,
                                              pm_info_t *pm_info,
                                              uint64_t *count);

/*!
 * \brief Get interrupt enable
 *
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [in] port Port macro physical port number.
 * \param [in] pc_intr_type Interrupt type
 * \param [out] enable 1 - interrupt enabled.
 *                     0 - interrupt disabled.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpc_port_intr_enable_get_f)(int unit, const pm_access_t *pm_acc,
                                pm_oper_status_t *op_st,
                                pm_info_t *pm_info,
                                bcmpc_intr_type_t pc_intr_type,
                                uint32_t *enable);

/*!
 * \brief Set interrupt enable
 *
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [in] port Port macro physical port number.
 * \param [in] pc_intr_type Interrupt type
 * \param [in] enable 1 Enable interrupt at port level
 *                    0 Disable interrupt at port level
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpc_port_intr_enable_set_f)(int unit, const pm_access_t *pm_acc,
                                pm_oper_status_t *op_st,
                                pm_info_t *pm_info,
                                bcmpc_intr_type_t pc_intr_type,
                                uint32_t enable);

/*!
 * \brief Process interrupts on a port
 *
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [in] pc_intr_type Interrupt type
 * \param [out] intr_cleared Interrupt cleared.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpc_pm_port_interrupt_process_f)(int unit, const pm_access_t *pm_acc,
                                     pm_oper_status_t *op_st,
                                     pm_info_t *pm_info,
                                     bcmpc_intr_type_t pc_intr_type,
                                     uint32_t *intr_cleared);

/*! \} */
/*******************************************************************************
 * PM driver object
 */

/*!
 * \brief Port manager driver.
 *
 * The PortMacro (PM) driver is used to sustain the PM-specific abilities e.g.
 * the restriction of the speed mode, speed combination, speed-to-VCO mapping
 * and so on.
 *
 * Comparing to PMAC and PHYMOD driver, the PM driver does not provide any
 * hardware access operations but just use to get the PM-specific abilities and
 * limitations.
 */
typedef struct bcmpc_pm_drv_s {

    /*! PM operating mode validate. */
    bcmpc_pm_mode_validate_f pm_mode_validate;

    /*! Port speed validate. */
    bcmpc_pm_port_speed_config_validate_f speed_config_validate;

    /*! PM port lane map validate. */
    bcmpc_pm_port_lane_map_validate_f port_lane_map_validate;

    /*! Attach to  Port Macro. */
    bcmpc_pm_core_attach_f core_attach;

    /*! Detach from Port Macro. */
    bcmpc_pm_port_detach_f port_detach;

    /*! Destroy port Macro allocated resources. */
    bcmpc_pm_destroy_f pm_destroy;

    /*! Add a port to Port Macro. */
    bcmpc_pm_port_add_f port_add;

    /*! Port speed config set. */
    bcmpc_pm_port_speed_set_f pm_port_speed_config_set;

    /*! Port speed config get. */
    bcmpc_pm_port_speed_get_f pm_port_speed_config_get;

    /*! PM configured VCOs get. */
    bcmpc_pm_port_config_validate_f pm_port_config_validate;

    /*! PM port loopback set. */
    bcmpc_pm_port_loopback_set_f pm_port_loopback_set;

    /*! PM port loopback get. */
    bcmpc_pm_port_loopback_get_f pm_port_loopback_get;

    /*! PM port autonegotiation advertising abilities set. */
    bcmpc_pm_port_ability_advert_set_f pm_port_ability_advert_set;

    /*! PM port autonegotiation advertising abilities get. */
    bcmpc_pm_port_ability_advert_get_f pm_port_ability_advert_get;

    /*! PM port link partner's autonegotiation advertising abilities get. */
    bcmpc_pm_port_ability_remote_get_f pm_port_ability_remote_get;

    /*! PM port autonegotiation enable/disable. */
    bcmpc_pm_port_autoneg_set_f pm_port_autoneg_set;

    /*! PM port autonegotiation enable/disable get. */
    bcmpc_pm_port_autoneg_get_f pm_port_autoneg_get;

    /*! PM port autonegotiation status get. */
    bcmpc_pm_port_autoneg_status_get_f pm_port_autoneg_status_get;

    /*! PM port encap set. */
    bcmpc_pm_port_encap_set_f pm_port_encap_set;

    /*! PM port encap get. */
    bcmpc_pm_port_encap_get_f pm_port_encap_get;

    /*! PM port pause control set. */
    bcmpc_pm_port_pause_ctrl_set_f pm_port_pause_ctrl_set;

    /*! PM port pause control get. */
    bcmpc_pm_port_pause_ctrl_get_f pm_port_pause_ctrl_get;

    /*! PM port pause address set. */
    bcmpc_pm_port_pause_addr_set_f pm_port_pause_addr_set;

    /*! PM port pause address get. */
    bcmpc_pm_port_pause_addr_get_f pm_port_pause_addr_get;

    /*! PM port max frame size set. */
    bcmpc_pm_port_frame_max_set_f pm_port_frame_max_set;

    /*! PM port max frame size get. */
    bcmpc_pm_port_frame_max_get_f pm_port_frame_max_get;

    /*! PM port enable set. */
    bcmpc_pm_port_enable_set_f pm_port_enable_set;

    /*! PM port enable get. */
    bcmpc_pm_port_enable_get_f pm_port_enable_get;

    /*! PM phy link status get. */
    bcmpc_pm_port_link_status_get_f pm_port_link_status_get;

    /*! PM port lag failover loopback get. */
    bcmpc_pm_port_lag_failover_lpbk_get_f pm_port_lag_failover_lpbk_get;

    /*! PM port tx taps config set. */
    bcmpc_pm_port_tx_set_f pm_port_tx_set;

    /*! PM port tx taps config get. */
    bcmpc_pm_port_tx_get_f pm_port_tx_get;

    /*! PM port pmd lane config set. */
    bcmpc_pm_port_pmd_lane_cfg_set_f pm_port_pmd_lane_cfg_set;

    /*! PM port pmd lane config set. */
    bcmpc_pm_port_pmd_lane_cfg_get_f pm_port_pmd_lane_cfg_get;

    /*! PM LAG failover enable set. */
    bcmpc_pm_port_failover_enable_set_f pm_port_failover_enable_set;

    /*! PM LAG failover enable get. */
    bcmpc_pm_port_failover_enable_get_f pm_port_failover_enable_get;

    /*! PM port link training set. */
    bcmpc_pm_port_link_training_set_f pm_port_link_training_set;

    /*! PM port link training get. */
    bcmpc_pm_port_link_training_get_f pm_port_link_training_get;

    /*! PM mac TX enable set. */
    bcmpc_pm_mac_tx_enable_set_f pm_mac_tx_enable_set;

    /*! PM mac RX enable set. */
    bcmpc_pm_mac_rx_enable_set_f pm_mac_rx_enable_set;

    /*! PM mac enable get. */
    bcmpc_pm_mac_enable_get_f pm_mac_enable_get;

    /*! PM mac local, remote fault set. */
    bcmpc_pm_mac_fault_disable_set_f pm_mac_fault_disable_set;

    /*! PM mac enable get. */
    bcmpc_pm_mac_fault_status_get_f pm_mac_fault_status_get;

    /*! PM mac enable get. */
    bcmpc_pm_mac_fault_disable_get_f pm_mac_fault_disable_get;

    /*! PM mac average ipg size set. */
    bcmpc_pm_mac_avg_ipg_set_f pm_mac_avg_ipg_set;

    /*! PM mac average ipg size get. */
    bcmpc_pm_mac_avg_ipg_get_f pm_mac_avg_ipg_get;

    /*! PM pfc set. */
    bcmpc_pm_pfc_set_f pm_pfc_set;

    /*! PM pfc get. */
    bcmpc_pm_pfc_get_f pm_pfc_get;

    /*! PM phy state get. */
    bcmpc_pm_phy_status_get_f pm_phy_status_get;

    /*! PM phy control set. */
    bcmpc_pm_phy_control_set_f pm_phy_control_set;

    /*! PM port mib oversize set. */
    bcmpc_pm_mib_oversize_set_f pm_mib_oversize_set;

    /*! PM port mib oversize get. */
    bcmpc_pm_mib_oversize_get_f pm_mib_oversize_get;

    /*! PM port abilities get. */
    bcmpc_pm_port_abilities_get_f pm_port_abilities_get;

    /*! Set pause frame handling in the switch. */
    bcmpc_pm_pause_frames_pass_set_f pause_frames_pass_set;

    /*! Get pause frame handling in the switch. */
    bcmpc_pm_pause_frames_pass_get_f pause_frames_pass_get;

    /*! Set control frame handling in the switch. */
    bcmpc_pm_control_frames_pass_set_f control_frames_pass_set;

    /*! Get control frame handling in the switch. */
    bcmpc_pm_control_frames_pass_get_f control_frames_pass_get;

    /*! Control PM TSC. */
    bcmpc_pm_tsc_ctrl_f pm_tsc_ctrl;

    /*! Get PM vco rate. */
    bcmpc_pm_vco_get_f pm_vco_get;

    /*! Set timesync configuration for a port. */
    bcmpc_pm_port_timesync_config_set_f pm_port_timesync_config_set;

    /*! Set SyncE clock mode. */
    bcmpc_pm_port_synce_clk_ctrl_set_f pm_port_synce_clk_ctrl_set;

    /*! Get SyncE clock mode. */
    bcmpc_pm_port_synce_clk_ctrl_get_f pm_port_synce_clk_ctrl_get;

    /*! PM link up event. */
    bcmpc_pm_port_phy_link_up_event_f pm_port_phy_link_up_event;

    /*! PM link down event. */
    bcmpc_pm_port_phy_link_down_event_f pm_port_phy_link_down_event;

    /*! PM port SW state update */
    bcmpc_pm_port_sw_state_update_f pm_port_sw_state_update;

    /*! Set runt threshold on a port. */
    bcmpc_pm_runt_threshold_set_f runt_threshold_set;

    /*! Get runt threshold on a port. */
    bcmpc_pm_runt_threshold_get_f runt_threshold_get;

    /*! RLM port config set  */
    bcmpc_pm_port_rlm_config_set_f pm_port_rlm_config_set;

    /*! PM init  */
    bcmpc_pm_init_f pm_init;

    /*! PM port link training status get. */
    bcmpc_pm_port_link_training_status_get_f pm_port_link_training_status_get;

    /*! PM port Reduce Lane Mode status get. */
    bcmpc_pm_port_rlm_status_get_f pm_port_rlm_status_get;

    /*! PM VCO rate calculate. */
    bcmpc_pm_vco_rate_calculate_f pm_vco_rate_calculate;

    /*! PM port default PMD firmware lane config get. */
    bcmpc_pm_default_pmd_lane_config_get_f pm_default_pmd_lane_config_get;

    /*! PM TSC core TX/RX polarity. */
    bcmpc_pm_phy_polarity_get_f pm_phy_polarity_get;

    /*! PM port default TX TAPs config get. */
    bcmpc_pm_default_tx_taps_config_get_f pm_default_tx_taps_config_get;

    /*! Get PM TSC core lane map settings. */
    bcmpc_pm_core_lane_map_get_f pm_core_lane_map_get;

    /*! Set TX stall on a port. */
    bcmpc_pm_stall_tx_set_f pm_port_stall_tx_set;

    /*! Get TX stall status on a port. */
    bcmpc_pm_stall_tx_get_f pm_port_stall_tx_get;

    /*! Port MAC Receive Statistic Vector (RSV) get function. */
    bcmpc_port_mac_rsv_mask_get_f port_rsv_mask_get;

    /*! Port MAC Receive Statistic Vector (RSV) set function. */
    bcmpc_port_mac_rsv_mask_set_f port_rsv_mask_set;

    /*! Get port TX packet timestamp from FIFO. */
    bcmpc_port_tx_timestamp_info_get_f port_tx_timestamp_info_get;

    /*! Port Macro egress 1588 timestamping mode get. */
    bcmpc_egress_timestamp_mode_get_f pm_egress_timestamp_mode_get;

    /*! Port Macro egress 1588 timestamping mode set. */
    bcmpc_egress_timestamp_mode_set_f pm_egress_timestamp_mode_set;

    /*! Port Macro FEC cl91 uncorrectable counter get. */
    bcmpc_phy_rsfec_uncorrectable_counter_get_f
                                     pm_phy_rsfec_uncorrectable_counter_get;

    /*! Port Macro FEC cl91 correctable counter get. */
    bcmpc_phy_rsfec_correctable_counter_get_f
                                     pm_phy_rsfec_correctable_counter_get;

    /*! Port Macro RSFEC symbol error counter get. */
    bcmpc_phy_rsfec_symbol_error_counter_get_f
                                     pm_phy_rsfec_symbol_error_counter_get;

    /*! Port interrupt enable get. */
    bcmpc_port_intr_enable_get_f port_intr_enable_get;

    /*! Port interrupt enable set. */
    bcmpc_port_intr_enable_set_f port_intr_enable_set;

    /*! Port interrupt processing. */
    bcmpc_pm_port_interrupt_process_f port_interrupt_process;

} bcmpc_pm_drv_t;


/*******************************************************************************
 * PM driver instance
 */

/*! PM4x10 PM driver. */
extern bcmpc_pm_drv_t bcmpc_pm_drv_pm4x10;

/*! PM4x25 PM driver. */
extern bcmpc_pm_drv_t bcmpc_pm_drv_pm4x25;

/*! PM8X50 PM driver. */
extern bcmpc_pm_drv_t bcmpc_pm_drv_pm8x50;

/*! PM8X50 GEN3 PM driver. */
extern bcmpc_pm_drv_t bcmpc_pm_drv_pm8x50_gen3;

/*! PM8X100 PM driver. */
extern bcmpc_pm_drv_t bcmpc_pm_drv_pm8x100;

/*! PM8X100 DPLL PM driver. */
extern bcmpc_pm_drv_t bcmpc_pm_drv_pm8x100_dpll;

/*! QTC PM driver. */
extern bcmpc_pm_drv_t bcmpc_pm_drv_pmqtc;

/*! SGMII4Px2 PM driver. */
extern bcmpc_pm_drv_t bcmpc_pm_drv_pmsgmii4px2;

/*! Common driver. */
extern bcmpc_pm_drv_t bcmpc_pm_drv_common;

#endif /* BCMPC_PM_DRV_INTERNAL_H */
