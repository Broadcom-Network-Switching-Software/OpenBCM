/*! \file bcmpc_pmgr_drv_internal.h
 *
 * BCMPC PMGR Driver Object.
 *
 * Declare the PMGR driver structures and publish the driver instances.
 *
 * The Port Manager driver would handle the PMAC and PHY programming sequence.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPC_PMGR_DRV_INTERNAL_H
#define BCMPC_PMGR_DRV_INTERNAL_H

#include <bcmpc/bcmpc_pmgr.h>
#include <bcmpc/bcmpc_fdr.h>
#include <bcmpc/bcmpc_drv_internal.h>
#include <bcmpc/bcmpc_pm_internal.h>
#include <bcmpc/bcmpc_lport_internal.h>
#include <bcmpc/bcmpc_pmgr_internal.h>


/*******************************************************************************
 * PMGR driver functions
 */

/*!
 * \name Port Manager driver functions
 *
 * The function prototypes for \ref bcmpc_pmgr_drv_t.
 */

/*! \{ */

/*!
 * \brief Enable the PM port of the given physical port.
 *
 * See \ref bcmpc_pmgr_pm_port_enable.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Device physical port.
 * \param [in] enable 1 to enalbe, 0 to disable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_pmgr_pm_port_enable_f)(int unit, bcmpc_pport_t pport,
                               int enable);

/*!
 * \brief Set the port configuration of the given physical port.
 *
 * See \ref bcmpc_pmgr_port_cfg_set.
 *
 * Note that the implementation of this function should only update the hardware
 * if the configuration has changed.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Device physical port.
 * \param [in] lport The associated logical port.
 * \param [in] cfg Port configuration to set.
 * \param [in] is_new The port is a newly created port or not.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_port_cfg_set_f)(int unit, bcmpc_pport_t pport, bcmpc_lport_t lport,
                        bcmpc_port_cfg_t *cfg, bool is_new);

/*!
 * \brief Get the port status of the given physical port.
 *
 * See \ref bcmpc_pmgr_port_status_get.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Device logical port.
 * \param [in] pport Device physical port.
 * \param [out] st Port status buffer for get.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_pmgr_port_status_get_f)(int unit, bcmpc_lport_t lport,
                                bcmpc_pport_t pport,
                                bcmpc_pmgr_port_status_t *st);

/*!
 * \brief Get the PHY link info of the given physical port.
 *
 * This function is used to get PHY link information e.g. PCS/PMD link error.
 *
 * It provides the detail information for understanding why link state is down.
 * When PHY link errors are detected, the corresponding bits (\ref
 * bcmpc_pmgr_link_event_t) in \c st will be set.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Device physical port.
 * \param [out] st Link status buffer for get.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_pmgr_phy_link_info_get_f)(int unit, bcmpc_pport_t pport,
                                  bcmpc_pmgr_link_status_t *st);

/*!
 * \brief Get the PHY link state of the given physical port.
 *
 * This function only reports the PHY link state. When link up,
 * the bit \ref BCMPC_PMGR_LINK_EVENT_LINK in \c st will be set.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Device physical port.
 * \param [out] st Link status buffer for get.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_pmgr_phy_link_state_get_f)(int unit, bcmpc_pport_t pport,
                                   bcmpc_pmgr_link_status_t *st);

/*!
 * \brief Get the fault state of the given physical port.
 *
 * This function reports the MAC local and remote fault state.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Device physical port.
 * \param [out] st Link status buffer for get.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_pmgr_fault_state_get_f)(int unit, bcmpc_pport_t pport,
                                bcmpc_pmgr_link_status_t *st);

/*!
 * \brief Set the port mode of a PM.
 *
 * See \ref bcmpc_pmgr_core_mode_set.
 *
 * \param [in] unit Unit number.
 * \param [in] pport The first device physical port of the PM.
 * \param [in] mode PM mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_pmgr_core_mode_set_f)(int unit, bcmpc_pport_t pport,
                              bcmpc_pm_mode_t *mode);

/*!
 * \brief Initialize the PHY of the given physical port.
 *
 * See \ref bcmpc_pmgr_phy_init.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] pcfg Port configuration.
 * \param [in] ccfg Core configuration.
 * \param [in] do_core_init True to do core initialization.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_pmgr_phy_init_f)(int unit, bcmpc_pport_t pport, bcmpc_port_cfg_t *pcfg,
                         bcmpc_pm_core_cfg_t *ccfg, bool do_core_init);

/*!
 * \brief Get LAG failover loopback status.
 *
 * See \ref bcmpc_pmgr_failover_loopback_get.
 *
 * \param [in] unit Unit number
 * \param [in] pport Device physical port.
 * \param [out] failover_loopback 1 when in lag failover loopback state,
 *                                otherwise 0.
 *
 * \retval SHR_E_NONE No errors
 */
typedef int
(*bcmpc_pmgr_failover_loopback_get_f)(int unit, bcmpc_pport_t pport,
                                      uint32_t *failover_loopback);

/*!
 * \brief Get LAG failover enable state.
 *
 * See \ref bcmpc_pmgr_failover_enable_get.
 *
 * \param [in] unit Unit number
 * \param [in] lport Device logical port.
 * \param [in] pport Device physical port.
 * \param [out] enable 1 for enabled, otherwise disabled.
 *
 * \retval SHR_E_NONE No errors
 */
typedef int
(*bcmpc_pmgr_failover_enable_get_f)(int unit, bcmpc_pport_t pport,
                                    uint32_t *enable);

/*!
 * \brief Set LAG failover enable state.
 *
 * See \ref bcmpc_pmgr_failover_enable_set.
 *
 * \param [in] unit Unit number
 * \param [in] pport Device physical port.
 * \param [in] enable 1 to enable, otherwise to disable.
 *
 * \retval SHR_E_NONE No errors
 */
typedef int
(*bcmpc_pmgr_failover_enable_set_f)(int unit, bcmpc_pport_t pport,
                                    uint32_t enable);

/*!
 * \brief Bring port out of LAG failover loopback.
 *
 * See \ref bcmpc_pmgr_failover_loopback_remove.
 *
 * \param [in] unit Unit number
 * \param [in] pport Device physical port.
 *
 * \retval SHR_E_NONE No errors
 */
typedef int
(*bcmpc_pmgr_failover_loopback_remove_f)(int unit, bcmpc_pport_t pport);

/*!
 * \brief Set PFC configuration.
 *
 * \param [in] unit Unit number
 * \param [in] lport Device logical port.
 * \param [in] pport Device physical port.
 * \param [in] pctrl PFC configuration.
 *
 * \retval SHR_E_NONE No errors
 */
typedef int
(*bcmpc_pmgr_pfc_set_f)(int unit, bcmpc_lport_t lport,
                        bcmpc_pport_t pport, bcmpc_pfc_control_t *pctrl);

/*!
 * \brief Get PFC configuration.
 *
 * \param [in] unit Unit number
 * \param [in] lport Device logical port.
 * \param [in] pport Device physical port.
 * \param [out] pctrl PFC configuration.
 *
 * \retval SHR_E_NONE No errors
 */
typedef int
(*bcmpc_pmgr_pfc_get_f)(int unit, bcmpc_lport_t lport,
                        bcmpc_pport_t pport, bcmpc_pfc_control_t *pctrl);

/*!
 * \brief PM phymod core init.
 *
 * \param [in] unit Unit number
 * \param [in] lport Device logical port.
 * \param [in] pcfg Port configuration.
 * \param [in] enable 1 to enalbe, 0 to disable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_pmgr_pm_port_attach_f)(int unit, bcmpc_lport_t lport,
                               bcmpc_port_cfg_t *pcfg,
                               bcmpc_pm_core_cfg_t *ccfg, int enable);

/*!
 * \brief Get PM port speed config.
 *
 * \param [in] unit Unit number
 * \param [in] lport Device logical port.
 * \param [out] pcfg Port configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_pmgr_pm_port_speed_get_f)(int unit, bcmpc_lport_t lport,
                                  bcmpc_port_cfg_t *pcfg);

/*!
 * \brief Port configuration validation on the PM.
 *
 * See \ref bcmpc_pmgr_pm_port_config_validate.
 *
 * \param [in] unit Unit number
 * \param [in] pcfg Port configuration.
 * \param [in] is_first The port is the first port created or not.
 * \param [in] is_new The port is a newly created port or not.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_pmgr_pm_port_config_validate_f)(int unit, bcmpc_port_cfg_t *pcfg,
                                        int is_first, int is_new);

/*!
 * \brief Set auto-negotiation abilities for a logical port.
 *
 * See \ref bcmpc_pmgr_pm_port_autoneg_advert_set.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] an_cfg Autoneg profile array list.
 * \param [in] pcfg Port configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_pmgr_pm_port_autoneg_advert_set_f)(int unit,
                                           bcmpc_lport_t lport,
                                           bcmpc_autoneg_prof_t *an_cfg,
                                           bcmpc_port_cfg_t *pcfg);

/*!
 * \brief Set the mac control of the given port.
 *
 * See \ref bcmpc_pmgr_mac_control_set.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] pport Device physical port.
 * \param [out] mctrl Mac control to be configured.
 * \param [in] default_mctrl Default Mac control for reference
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_pmgr_mac_control_set_f)(int unit, bcmpc_lport_t lport,
                               bcmpc_pport_t pport,
                               bcmpc_mac_control_t *mctrl,
                               bcmpc_mac_control_t default_mctrl);

/*!
 * \brief Get the mac status of the given port.
 *
 * See \ref bcmpc_pmgr_mac_control_get.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] pport Device physical port.
 * \param [out] mst MAC status get.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_pmgr_mac_control_get_f)(int unit, bcmpc_lport_t lport,
                                bcmpc_pport_t pport,
                                bcmpc_mac_control_t *mst);

/*!
 * \brief Set the phy control of the given port and lane.
 *
 * See \ref bcmpc_pmgr_phy_control_set.
 *
 * \param [in] unit Unit number.
 * \param [in] port_lane pointer to data structure of port_lane.
 * \param [in] pport Device physical port.
 * \param [out] pctrl Phy control to be configured.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_pmgr_phy_control_set_f)(int unit, bcmpc_port_lane_t *port_lane,
                                bcmpc_pport_t pport,
                                bcmpc_phy_control_t *pctrl);

/*!
 * \brief Get the phy status of the given port and lane.
 *
 * See \ref bcmpc_pmgr_phy_status_get.
 *
 * \param [in] unit Unit number.
 * \param [in] port_lane pointer to data structure of port_lane.
 * \param [in] pport Device physical port.
 * \param [out] pst Phy status get.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_pmgr_phy_status_get_f)(int unit, bcmpc_port_lane_t *port_lane,
                               bcmpc_pport_t pport,
                               bcmpc_pmgr_phy_status_t *pst);

/*!
 * \brief Set the tx taps control of the given port and lane.
 *
 * See \ref bcmpc_pmgr_phy_control_set.
 *
 * \param [in] unit Unit number.
 * \param [in] port_lane pointer to data structure of port_lane.
 * \param [in] pport Device physical port.
 * \param [out] pctrl tx taps configuration to be .
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_pmgr_tx_taps_control_set_f)(int unit, bcmpc_port_lane_t *port_lane,
                                    bcmpc_pport_t pport,
                                    bcmpc_tx_taps_config_t *pctrl);

/*!
 * \brief Get the tx taps status of the given physical port.
 *
 * See \ref bcmpc_pmgr_phy_status_get.
 *
 * \param [in] unit Unit number.
 * \param [in] port_lane pointer to data structure of port_lane.
 * \param [in] pport Device physical port.
 * \param [out] pst tx taps status get.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_pmgr_tx_taps_status_get_f)(int unit, bcmpc_port_lane_t *port_lane,
                                   bcmpc_pport_t pport,
                                   bcmpc_pmgr_tx_taps_status_t *pst);

/*!
 * \brief Set the pmd firmware control of the given port and lane.
 *
 * See \ref bcmpc_pmgr_phy_control_set.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Device logical port.
 * \param [in] pport Device physical port.
 * \param [out] fm_cfg PMD firmware configuration to be set.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_pmgr_pmd_firmware_control_set_f)(int unit, bcmpc_lport_t lport,
                                         bcmpc_pport_t pport,
                                         bcmpc_pmd_firmware_config_t *fm_cfg);

/*!
 * \brief Get the tx taps status of the given physical port.
 *
 * See \ref bcmpc_pmgr_phy_status_get.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Device logical port.
 * \param [in] pport Device physical port.
 * \param [out] fm_st PMD firmware status get.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_pmgr_pmd_firmware_status_get_f)(int unit, bcmpc_lport_t lport,
                                        bcmpc_pport_t pport,
                                        bcmpc_pmgr_pmd_firmware_status_t *fm_st);

/*!
 * \brief Get port abilities for the given logical port.
 *
 * See \ref bcmpc_pmgr_port_abilities_get.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Device logical port.
 * \param [in] pport Device physical port.
 * \param [in] type  port ability type.
 * \param [out] pabilities  port abilities get.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_pmgr_port_abilities_get_f)(int unit, bcmpc_lport_t lport,
                                   bcmpc_pport_t pport,
                                   pm_port_ability_type_t type,
                                   bcmpc_pmgr_port_abilities_t *pabilities);

/*!
 * \brief Detach PM logical port.
 *
 * \param [in] unit Unit number
 * \param [in] pport Device physical port.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_pmgr_pm_port_detach_f)(int unit, bcmpc_pport_t pport);

/*!
 * \brief Power on PM TSC.
 *
 * \param [in] unit Unit number
 * \param [in] pport Device physical port.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_pmgr_pm_tsc_power_on_f)(int unit, bcmpc_pport_t pport);

/*!
 * \brief Get current PM vco rates.
 *
 * \param [in] unit Unit number
 * \param [in] pport Device physical port.
 * \param [out] PM vco rates.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_pmgr_pm_vco_rate_get_f)(int unit, bcmpc_pport_t pport,
                                bcmpc_vco_type_t *vcos);

/*!
 * \brief Set timesync configuration for a given logical port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Device logical port.
 * \param [in] pport Device physical port.
 * \param [in] config Timesync configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_pmgr_timesync_set_f)(int unit, bcmpc_lport_t lport,
                             bcmpc_pport_t pport,
                             bcmpc_port_timesync_t *config);

/*!
 * \brief PM port sw state update function.
 *
 * \param [in]  unit                 Unit number.
 * \param [in]  pport                physical port number
 * \retval SHR_E_NONE   No errors.
 * \retval SHR_E_FAIL   Failure.
 * \retval SHR_E_UNAVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pmgr_port_sw_state_update_f)(int unit, bcmpc_pport_t pport);

/*!
 * \brief Function for pupulating the PC_PORT_INFO LT.
 *
 * \param [in]  unit    Unit number.
 * \param [in]  pport   physical port number.
 * \param [in]  op_st   Operation status returned for PM driver call.
 * \retval SHR_E_NONE   No errors.
 * \retval SHR_E_FAIL   Failure.
 * \retval SHR_E_UNAVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_port_info_table_update_f)(int unit, bcmpc_pport_t pport,
                                  pm_oper_status_t *op_st);
/*!
 * \brief PM initialization.
 *
 * \param [in]  unit    Unit number.
 * \param [in]  pport   physical port number.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval SHR_E_FAIL   Failure.
 * \retval SHR_E_UNAVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pmgr_pm_init_f)(int unit, bcmpc_pport_t pport);

/*!
 * \brief Get Reduce Lane Mode (RLM) status for a logical port.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [out] status RLM status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failures.
 * \retval SHR_E_UNAVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pmgr_port_rlm_status_get_f)(int unit, bcmpc_pport_t pport,
                                    bcmpc_pmgr_port_rlm_status_t *status);

/*!
 * \brief Calculate PM VCO rate.
 *
 * \param [in] unit Unit number
 * \param [in] pport Device physical port.
 * \param [out] vco_select VCO rate calculated based on the speed config.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_pmgr_pm_vco_rate_calculate_f)(int unit, bcmpc_pport_t pport,
                                      pm_vco_setting_t *vco_select);

/*!
 * \brief This function invokes the port Macro driver call
 * for initializing the SERDES core.
 *
 * \param [in] unit Unit number
 * \param [in] lport Device logical port.
 * \param [in] vco_cfg VCO configuration.
 * \param [in] pcfg Logical port  configuration.
 * \param [in] ccfg Port Macro core configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_pmgr_pm_core_attach_f)(int unit, bcmpc_pport_t pport,
                               pm_vco_setting_t *vco_cfg,
                            bcmpc_port_cfg_t *pcfg,
                            bcmpc_pm_core_cfg_t *ccfg);

/*!
 * \brief Set logical port loopback mode.
 *
 * \param [in] unit Unit number
 * \param [in] pport Device physical port.
 * \param [in] pcfg Logical port configuration.
 *
 * \retval SHR_E_NONE No errors
 */
typedef int
(*bcmpc_pmgr_port_loopback_set_f)(int unit, bcmpc_pport_t pport,
                                  bcmpc_port_cfg_t *pcfg);

/*!
 * \brief Set logical port enable set.
 *
 * \param [in] unit Unit number
 * \param [in] pport Device physical port.
 * \param [in] flags Flags to specify PHY or MAC settings.
 * \param [in] pcfg Logical port configuration.
 *
 * \retval SHR_E_NONE No errors
 */
typedef int
(*bcmpc_pmgr_port_enable_set_f)(int unit, bcmpc_pport_t pport,
                                uint32_t flags, bcmpc_port_cfg_t *pcfg);

/*! \} */

/*!
 * \brief Get PM core polarity flip and lane map settings..
 *
 * \param [in] unit Unit number
 * \param [in] pport Device physical port.
 * \param [out] ccfg TSC Core configuration data structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_pmgr_pm_core_config_get_f)(int unit, bcmpc_pport_t pport,
                                   bcmpc_pm_core_cfg_t *ccfg);

/*!
 * \brief Get port TX packet timestamp from FIFO.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Device logical port.
 * \param [in] pport Device physical port.
 * \param [out] timestamp_info Packet timestamp data structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_EMPTY No timestamps in the FIFO.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
typedef int
(*bcmpc_pmgr_port_tx_timestamp_info_get_f)(int unit,
                             bcmpc_lport_t lport, bcmpc_pport_t pport,
                             bcmpc_pmgr_port_timestamp_t *timestamp_info);

/*!
 * \brief Get egress 1588 timestamp mode.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Device physical port.
 * \param [out] timestamp_mode Timestamp mode enumeration
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpc_pmgr_egress_timestamp_mode_get_f)(int unit, bcmpc_pport_t pport,
                              bcmpc_pmgr_egr_timestamp_mode_t *timestamp_mode);

/*!
 * \brief Set egress 1588 timestamp mode.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Device physical port.
 * \param [in] timestamp_mode Timestamp mode enumeration
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpc_pmgr_egress_timestamp_mode_set_f)(int unit, bcmpc_pport_t pport,
                              bcmpc_pmgr_egr_timestamp_mode_t timestamp_mode);

/*!
 * \brief Get the FDR configuration control of the given port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] pport Device physical port.
 * \param [out] fdr_cfg FDR control configuration information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_pmgr_fdr_control_get_f)(int unit, bcmpc_lport_t lport,
                                bcmpc_pport_t pport,
                                bcmpc_fdr_port_control_t *fdr_cfg);

/*!
 * \brief Set the FDR configuration control of the given port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] pport Device physical port.
 * \param [in] fdr_cfg FDR control configuration information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_pmgr_fdr_control_set_f)(int unit, bcmpc_lport_t lport,
                                bcmpc_pport_t pport,
                                bcmpc_fdr_port_control_t *fdr_cfg);

/*!
 * \brief Get the FDR statistics for a port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] pport Device physical port.
 * \param [out] fdr_stats FDR statistics structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_pmgr_fdr_stats_get_f)(int unit, bcmpc_lport_t lport,
                              bcmpc_pport_t pport,
                              bcmpc_fdr_port_stats_t *fdr_stats);

/*!
 * \brief Get the diag statistics for a port.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Device physical port.
 * \param [out] diag_stats Diag statistics structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_pmgr_port_diag_stats_get_f)(int unit,
                                    bcmpc_pport_t pport,
                                    bcmpc_pmgr_port_diag_stats_t *diag_stats);

/*!
 * \brief Process interrupts on a port
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] pport Device physical port.
 * \param [in] intr_type Interrupt type to be processed.
 * \param [out] is_handled 1 - Indicates the interrupt is handled.
 *                         0 - Interrupt processing failed.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_pmgr_port_interrupt_process_f)(int unit, bcmpc_lport_t lport,
                                       bcmpc_pport_t pport,
                                       bcmpc_intr_type_t intr_type,
                                       uint32_t *is_handled);

/*******************************************************************************
 * PMGR driver object
 */

/*!
 * \brief Port manager driver.
 *
 * A port manager is a driver which controls complex operations on a logical
 * port. The port manager is the component, which knows how a port, MAC and
 * PHY work together. Several logical port operations require that PHY APIs
 * and PMAC APIs are called in a particular order.
 */
typedef struct bcmpc_pmgr_drv_s {

    /*! PM port enable set. */
    bcmpc_pmgr_pm_port_enable_f pm_port_enable;

    /*! Port configuration set. */
    bcmpc_port_cfg_set_f port_cfg_set;

    /*! Port status get. */
    bcmpc_pmgr_port_status_get_f port_status_get;

    /*! PHY link information get. */
    bcmpc_pmgr_phy_link_info_get_f phy_link_info_get;

    /*! PHY link state get. */
    bcmpc_pmgr_phy_link_state_get_f phy_link_state_get;

    /*! Fault state get. */
    bcmpc_pmgr_fault_state_get_f fault_state_get;

    /*! PM mode set. */
    bcmpc_pmgr_core_mode_set_f core_mode_set;

    /*! PHY initialization. */
    bcmpc_pmgr_phy_init_f phy_init;

    /*! PHY operation execute. */
    bcmpc_op_exec_f phy_op_exec;

    /*! PMAC operation execute. */
    bcmpc_op_exec_f pmac_op_exec;

    /*! Failover status get function. */
    bcmpc_pmgr_failover_loopback_get_f failover_loopback_get;

    /*! Failover set function. */
    bcmpc_pmgr_failover_enable_set_f failover_enable_set;

    /*! Failover get function. */
    bcmpc_pmgr_failover_enable_get_f failover_enable_get;

    /*! Failover loopback bring out function. */
    bcmpc_pmgr_failover_loopback_remove_f failover_loopback_remove;

    /*! PFC configuration set. */
    bcmpc_pmgr_pfc_set_f pm_pfc_set;

    /*! PFC configuration get. */
    bcmpc_pmgr_pfc_get_f pm_pfc_get;

    /* NEW FUNCTIONS */

    /*! PM port attach. */
    bcmpc_pmgr_pm_port_attach_f port_attach;

    /*! PM Port configuration get. */
    bcmpc_pmgr_pm_port_speed_get_f port_speed_get;

    /*! PM config validate */
    bcmpc_pmgr_pm_port_config_validate_f pm_port_config_validate;

    /*! PM port auto-negotiation ability set. */
    bcmpc_pmgr_pm_port_autoneg_advert_set_f port_autoneg_advert_set;

    /*! Mac control set. */
    bcmpc_pmgr_mac_control_set_f pm_mac_control_set;

    /*! Mac status get. */
    bcmpc_pmgr_mac_control_get_f pm_mac_control_get;

    /*! Mac control set. */
    bcmpc_pmgr_phy_control_set_f pm_phy_control_set;

    /*! Phy status get. */
    bcmpc_pmgr_phy_status_get_f pm_phy_status_get;

    /*! TX taps control set. */
    bcmpc_pmgr_tx_taps_control_set_f pm_tx_taps_config_set;

    /*! TX taps status get. */
    bcmpc_pmgr_tx_taps_status_get_f pm_tx_taps_status_get;

    /*! PMD firmware control set. */
    bcmpc_pmgr_pmd_firmware_control_set_f pm_pmd_firmware_config_set;

    /*! PMD firmware status get. */
    bcmpc_pmgr_pmd_firmware_status_get_f pm_pmd_firmware_status_get;

    /*! Number of port abilities. */
    bcmpc_pmgr_port_abilities_get_f pm_port_abilities_get;

    /*! phymod port detach. */
    bcmpc_pmgr_pm_port_detach_f pm_port_detach;

    /*! PM TSC power on. */
    bcmpc_pmgr_pm_tsc_power_on_f pm_tsc_power_on;

    /*! PM vco rate get. */
    bcmpc_pmgr_pm_vco_rate_get_f pm_vco_rate_get;

    /*! Timesync configuration set. */
    bcmpc_pmgr_timesync_set_f pm_timesync_set;

    /*! PM port SW state update */
    bcmpc_pmgr_port_sw_state_update_f pm_port_sw_state_update;

    /*! PORT_INFO table update. */
    bcmpc_port_info_table_update_f port_info_table_update;

    /*! PM initialization */
    bcmpc_pmgr_pm_init_f pm_init;

    /*! Port Reduce Lane Mode status get. */
    bcmpc_pmgr_port_rlm_status_get_f port_rlm_status_get;

    /*! Port Macro SERDES core attach. */
    bcmpc_pmgr_pm_core_attach_f pm_core_attach;

    /*! PM VCO rate calclulate. */
    bcmpc_pmgr_pm_vco_rate_calculate_f pm_vco_rate_calculate;

    /*! Port loopback set. */
    bcmpc_pmgr_port_loopback_set_f port_loopback_set;

    /*! Port enable set. */
    bcmpc_pmgr_port_enable_set_f port_enable_set;

    /*! PM Core polarity and lane map configuration get. */
    bcmpc_pmgr_pm_core_config_get_f pm_core_config_get;

    /*! Get port TX packet timestamp from FIFO. */
    bcmpc_pmgr_port_tx_timestamp_info_get_f port_tx_timestamp_info_get;

    /*! Port Macro egress 1588 timestamping mode get. */
    bcmpc_pmgr_egress_timestamp_mode_get_f pm_egress_timestamp_mode_get;

    /*! Port Macro egress 1588 timestamping mode set. */
    bcmpc_pmgr_egress_timestamp_mode_set_f pm_egress_timestamp_mode_set;

    /*! Port FDR configuration control get. */
    bcmpc_pmgr_fdr_control_get_f pm_port_fdr_control_get;

    /*! Port FDR configuration control set. */
    bcmpc_pmgr_fdr_control_set_f pm_port_fdr_control_set;

    /*! Port FDR stats get. */
    bcmpc_pmgr_fdr_stats_get_f pm_port_fdr_stats_get;

    /*! Port diag stats get. */
    bcmpc_pmgr_port_diag_stats_get_f pm_port_diag_stats_get;

    /*! Port interrupt process. */
    bcmpc_pmgr_port_interrupt_process_f pm_port_interrupt_process;

} bcmpc_pmgr_drv_t;


/*******************************************************************************
 * PMGR driver instance
 */

/*! The default port manager driver. */
extern bcmpc_pmgr_drv_t bcmpc_pmgr_drv_default;

#endif /* BCMPC_PMGR_DRV_INTERNAL_H */
