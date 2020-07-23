/*! \file bcmpmac_drv.h
 *
 * BCM PortMAC Driver.
 *
 * Declaration of the functions which are supported by PortMAC driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPMAC_DRV_H
#define BCMPMAC_DRV_H

#include <bcmpmac/bcmpmac_acc.h>

/******************************************************************************
 * PortMAC driver functions
 */

/*!
 * \brief Initialize a port.
 *
 * This function is used to initialize or clean up a port. It is called when a
 * port is added or deleted.
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
(*bcmpmac_port_init_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                       uint32_t init);

/*!
 * \brief Set Port reset state.
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
(*bcmpmac_port_reset_set_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                            uint32_t reset);

/*!
 * \brief Get port reset state.
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
(*bcmpmac_port_reset_get_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                            uint32_t *reset);

/*!
 * \brief Set port enable state.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] enable 1 to enable; 0 to disable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_port_enable_set_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                             uint32_t enable);

/*!
 * \brief Get port enable state.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] enable 1 to enable; 0 to disable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_port_enable_get_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                             uint32_t *enable);

/*!
 * \brief Set MAC reset state.
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
(*bcmpmac_mac_reset_set_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                           uint32_t reset);

/*!
 * \brief Get MAC reset state.
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
(*bcmpmac_mac_reset_get_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                           uint32_t *reset);

/*!
 * \brief Set Rx enable state.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] enable 1 to enable; 0 to disable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_rx_enable_set_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                           uint32_t enable);

/*!
 * \brief Get Rx enable state.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] enable 1 to enable; 0 to disable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_rx_enable_get_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                           uint32_t *enable);

/*!
 * \brief Set Tx enable state.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] enable 1 to enable; 0 to disable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_tx_enable_set_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                           uint32_t enable);

/*!
 * \brief Get Tx enable state.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] enable 1 to enable; 0 to disable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_tx_enable_get_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                           uint32_t *enable);

/*!
 * \brief Set speed.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] speed Speed in Mbps.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_speed_set_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                       uint32_t speed);

/*!
 * \brief Get speed.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] speed Speed in Mbps.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_speed_get_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                       uint32_t *speed);

/*!
 * \brief Set encap mode.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] encap Encap mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_encap_set_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                       bcmpmac_encap_mode_t encap);

/*!
 * \brief Get encap mode.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] encap Encap mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_encap_get_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                       bcmpmac_encap_mode_t *encap);

/*!
 * \brief Set MAC loopback.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] en 1 to enable MAC loopback; 0 to disable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_lpbk_set_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                      uint32_t en);

/*!
 * \brief Get MAC loopback.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] en 1 to enable MAC loopback; 0 to disable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_lpbk_get_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                      uint32_t *en);

/*!
 * \brief Set pause flow control configuration.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] pause_ctrl Pause flow control configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_pause_set_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                       bcmpmac_pause_ctrl_t *pause_ctrl);

/*!
 * \brief Get pause flow control configuration.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] pause_ctrl Pause flow control configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_pause_get_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                       bcmpmac_pause_ctrl_t *pause_ctrl);

/*!
 * \brief Set PAUSE frame source address.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] mac MAC address used for PAUSE transmission.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_pause_addr_set_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                            bcmpmac_mac_t mac);

/*!
 * \brief Get PAUSE frame source address.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] mac MAC address used for PAUSE transmission.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_pause_addr_get_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                            bcmpmac_mac_t mac);

/*!
 * \brief Set maximum receive frame size.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] size Maximum frame size in bytes.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_frame_max_set_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                           uint32_t size);

/*!
 * \brief Get maximum receive frame size.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] size Maximum frame size in bytes.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_frame_max_get_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                           uint32_t *size);


/*!
 * \brief Get fault status.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] st Fault status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_fault_status_get_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                              bcmpmac_fault_status_t *st);

/*!
 * \brief Set PM port mode.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] mode PM port mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_core_mode_set_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                           bcmpmac_core_port_mode_t *mode);

/*!
 * \brief Execute port operation.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] op Operation string.
 * \param [in] op_param Operation parameter.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_op_exec_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                     const char *op, uint32_t op_param);

/*!
 * \brief Get LAG failover loopback status.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] failover_lpbk Set when in lag failover loopback.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_failover_loopback_get_f)(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                                   uint32_t *failover_lpbk);

/*!
 * \brief Get LAG failover enable state.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] en 1 for enabled, otherwise disabled.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_failover_enable_get_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                                 uint32_t *en);

/*!
 * \brief Enable/Disable LAG failover.
 *
 * If LAG failover mode is enabled, the MAC will route all TX packets to the RX
 * path when the link is down. The state is called failover loopback.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] en 1 for enabled, otherwise disabled.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_failover_enable_set_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                                 uint32_t en);

/*!
 * \brief Bring port out of LAG failover loopback.
 *
 * Change the port back to normal operation no matter the port is in failover
 * loopback state or not.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_failover_loopback_remove_f)(bcmpmac_access_t *pa,
                                      bcmpmac_pport_t pm_pport);

/*!
 * \brief Set PFC configuration.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] pfc_cfg PFC configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_pfc_set_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                     bcmpmac_pfc_t *pfc_cfg);

/*!
 * \brief Get PFC configuration.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] pfc_cfg PFC configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_pfc_get_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                     bcmpmac_pfc_t *pfc_cfg);

/*!
 * \brief Set cdport mode.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] flags Flags for port mode setting.
 * \param [out] lane_mask Port lane mask.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_port_mode_set_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                           uint32_t flags, uint32_t lane_mask);

/*!
 * \brief Set fault disable.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] st Fault disable status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_fault_disable_set_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                              bcmpmac_fault_disable_t *st);

/*!
 * \brief Get fault disable status.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] st Fault disable status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_fault_disable_get_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                              bcmpmac_fault_disable_t *st);

/*!
 * \brief Set IPG (inter-packet gap).
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] ipg_size IPG size.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_avg_ipg_set_f)(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                         uint8_t ipg_size);

/*!
 * \brief Get IPG (inter-packet gap) size.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] ipg_size IPG size.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_avg_ipg_get_f)(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                         uint8_t *ipg_size);

/*!
 * \brief Enable CDC interrupt.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] enable 1 to enable; 0 to disable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_interrupt_enable_set_f)(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                                  uint32_t enable);

/*!
 * \brief Get CDC interrupt enable status.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] enable 1 is enabled; 0 is disabled.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_interrupt_enable_get_f)(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                                  uint32_t *enable);

/*!
 * \brief Set MIB frame oversize value.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] size Oversize value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_mib_oversize_set_f)(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                              uint32_t size);

/*!
 * \brief Get MIB frame oversize value.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] size Oversize value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_mib_oversize_get_f)(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                              uint32_t *size);

/*!
 * \brief Set pause frame handling in the switch.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] enable  1 - Pause frames passed to system side.
 *                     0 - Pause frames dropped in MAC.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_pass_pause_frame_set_f)(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                                  uint32_t enable);

/*!
 * \brief Get pause frame handling in the switch.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] enable  1 - Pause frames passed to system side.
 *                      0 - Pause frames dropped in MAC.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_pass_pause_frame_get_f)(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                                  uint32_t *enable);

/*!
 * \brief Set pfc frame handling in the switch.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] enable  1 - PFC frames passed to system side.
 *                     0 - PFC frames dropped in MAC.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_pass_pfc_frame_set_f)(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                                uint32_t enable);

/*!
 * \brief Get pfc frame handling in the switch.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] enable  1 - PFC frames passed to system side.
 *                      0 - PFC frames dropped in MAC.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_pass_pfc_frame_get_f)(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                                uint32_t *enable);

/*!
 * \brief Set control frames(ethertype 0x8808) handling in the switch.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] enable  1 - Control frames passed to system side.
 *                     0 - Control frames dropped in MAC.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_pass_control_frame_set_f)(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                                    uint32_t enable);

/*!
 * \brief Get control frame handling in the switch.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] enable  1 - Control frames passed to system side.
 *                      0 - Control frames dropped in MAC.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_pass_control_frame_get_f)(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                                    uint32_t *enable);

/*!
 * \brief Set frame handling from egress pipeline to MAC.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] discard  1 - Frames from egress pipeline to
 *                          MAC dropped on line side.
 *                      0 - Frames not dropped.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_discard_set_f)(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                         uint32_t discard);

/*!
 * \brief Get frame handling from egress pipeline to MAC.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] discard  1 - Frames from egress pipeline to
 *                          MAC dropped on line side.
 *                      0 - Frames not dropped.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_discard_get_f)(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                         uint32_t *discard);

/*!
 * \brief Set link status indication from Software.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] link  1 indicates link is active, otherwise not active.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_sw_link_status_set_f)(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                                int link);

/*!
 * \brief Get software link status.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] link  1 indicates link is active, otherwise not active.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_sw_link_status_get_f)(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                                int *link);

/*!
 * \brief Set configuration to choose between link status
 * indication from software or the hardware.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] enable  1 indicates hardware link status is used,
 *                     otherwise software link status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_link_status_select_set_f)(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                                    int enable);

/*!
 * \brief Get configuration to choose between link status
 * indication from software or the hardware.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] enable  1 indicates hardware link status is used,
 *                     otherwise software link status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_link_status_select_get_f)(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                                    int *enable);

/*!
 * \brief Set sending of link interruption ordered-sets in the Tx direction.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] enable  1 enable sending of link interruptions in TX direction.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_link_interrupt_ordered_set_f)(bcmpmac_access_t *pa,
                                        bcmpmac_pport_t port, uint32_t enable);

/*!
 * \brief Get sending of link interruption ordered-sets seeting in the
 * Tx direction.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] enable  1 enable sending of link interruptions in TX direction.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_link_interrupt_ordered_get_f)(bcmpmac_access_t *pa,
                                        bcmpmac_pport_t port, uint32_t *enable);

/*!
 * \brief Get link interruption state.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] status  1 when link interruption state is detected as per
 *                        RS layer state machine. Live status
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_link_interrupt_live_status_get_f)(bcmpmac_access_t *pa,
                                            bcmpmac_pport_t port,
                                            uint32_t *status);

/*!
 * \brief Set MAC TX traffic stall.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] enable  1 stop transmitting any more packets in
 *                       MAC in TX direction.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_stall_tx_enable_get_f)(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                                 bool *enable);

/*!
 * \brief Set MAC TX traffic stall.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] enable  1 stop transmitting any more packets in
 *                       MAC in TX direction.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_stall_tx_enable_set_f)(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                                 bool enable);

/*!
 * \brief Get link status.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] clear_on_read  1 clear the link_latch_down after read.
 * \param [in] start_lane  starting lane number of theport.
 * \param [out] link  1 indicates link was down since last clear.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_port_link_status_get_f)(bcmpmac_access_t *pa,
                                  bcmpmac_pport_t port,
                                  uint32_t clear_on_read,
                                  uint32_t start_lane,
                                  int *link);

/*!
 * \brief CDMAC control set.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] reset 1 Put CDMAC in hard reset (active High).
 *                    0 Bring CDMAC out of hard reset.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_port_mac_control_set_f)(bcmpmac_access_t *pa,
                                  bcmpmac_pport_t port,
                                  uint32_t reset);

/*!
 * \brief CDMAC control get.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] reset 1 CDMAC in hard reset.
 *                    0 CDMAC out of hard reset.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_port_mac_control_get_f)(bcmpmac_access_t *pa,
                                  bcmpmac_pport_t port,
                                  uint32_t *reset);

/*!
 * \brief Get tsc control configuration.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] tsc_cfg TSC control configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_port_tsc_ctrl_get_f)(bcmpmac_access_t *pa,
                               bcmpmac_pport_t port,
                               bcmpmac_tsc_ctrl_t *tsc_cfg);

/*!
 * \brief Set tsc control configuration.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] tsc_pwr_on 1 to power on TSC, 0 to power off TSC.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_port_tsc_ctrl_set_f)(bcmpmac_access_t *pa,
                              bcmpmac_pport_t port,
                              int tsc_pwr_on);

/*!
 * \brief Set runt threshold on a port.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] value  Size of packet below which it is marked
 *                    and run and dropped.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_runt_threshold_set_f)(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                                uint32_t value);

/*!
 * \brief Get runt threshold on a port.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] value  Size of packet below which it is marked
 *                    and run and dropped.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_runt_threshold_get_f)(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                                uint32_t *value);

/*!
 * \brief Set force PFC XON on a port.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] value  1 - Forces MAC to generate an XON to MMU.
 *                    0 - stop the XON packet generation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_force_pfc_set_f)(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                           uint32_t value);

/*!
 * \brief Get force PFC XON on a port.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] value  1 - Forces MAC to generate an XON to MMU.
 *                     0 - stop the XON packet generation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_force_pfc_get_f)(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                           uint32_t *value);

/*!
 * \brief Selective set for MAC Receive Statistic Vector (RSV).
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] flags To indicate the RSV to set.
 * \param [in] value  Bitmask
 *                    1 - Enable (Pass the packet).
 *                        Value in mask is set to 0.
 *                    0 - Disable (Purge the packet).
 *                        Value in mask is set to 1.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_rsv_selective_mask_set_f)(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                                    uint32_t flags, uint32_t value);

/*!
 * \brief Selective get for MAC Receive Statistic Vector (RSV).
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] flags To indicate the RSV to set.
 * \param [out] value  Bitmask
 *                    1 - Indicates enable (Pass the packet).
 *                        Value in mask is set to 0.
 *                    0 - Indicates disable (Purge the packet).
 *                        Value in mask is set to 1.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_rsv_selective_mask_get_f)(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                                    uint32_t flags, uint32_t *value);

/*!
 * \brief Set MAC Receive Statistic Vector (RSV).
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] value  Bitmask
 *                    1 - Enable (Pass the packet).
 *                        Value in mask is set to 0.
 *                    0 - Disable (Purge the packet).
 *                        Value in mask is set to 1.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_rsv_mask_set_f)(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                          uint32_t value);

/*!
 * \brief Get MAC Receive Statistic Vector (RSV).
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] value  Bitmask
 *                    1 - Indicates enable (Pass the packet).
 *                        Value in mask is set to 0.
 *                    0 - Indicates disable (Purge the packet).
 *                        Value in mask is set to 1.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_rsv_mask_get_f)(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                          uint32_t *value);

/*!
 * \brief Get port TX FIFO timestamp information.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] timestamp_info Timestamp information structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_port_tx_timestamp_get_f)(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                                 bcmpmac_tx_timestamp_info_t *timestamp_info);

/*!
 * \brief Get egress 1588 timestamp mode.
 *
 * \param [in] pa PMAC access info.
 * \param [in] port Port macro physical port number.
 * \param [out] timestamp_mode Timestamp mode enumeration
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_egress_timestamp_mode_get_f)(bcmpmac_access_t *pa,
                                 bcmpmac_pport_t port,
                                 bcmpmac_egr_timestamp_mode_t *timestamp_mode);

/*!
 * \brief Set egress 1588 timestamp mode.
 *
 * \param [in] pa PMAC access info.
 * \param [in] port Port macro physical port number.
 * \param [in] timestamp_mode Timestamp mode enumeration
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_egress_timestamp_mode_set_f)(bcmpmac_access_t *pa,
                                 bcmpmac_pport_t port,
                                 bcmpmac_egr_timestamp_mode_t timestamp_mode);

/*!
 * \brief Get 16B credit granualarity setting.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] enable  1 - 16Byte credit granularity enabled.
 *                      0 - 64byte credit granualrity.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_16b_credit_granularity_get_f)(bcmpmac_access_t *pa,
                                        bcmpmac_pport_t port, bool *enable);

/*!
 * \brief Set 16B credit granualarity setting.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] enable  1 - 16Byte credit granularity.
 *                     0 - 64byte credit granualrity.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_16b_credit_granularity_set_f)(bcmpmac_access_t *pa,
                                        bcmpmac_pport_t port, bool enable);
/*!
 * \brief Set cdport cell mode.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] enable 1 to enable; 0 to disable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_cell_mode_set_f)(bcmpmac_access_t *pa, bcmpmac_pport_t pm_pport,
                           uint32_t enable);

/*!
 * \brief Get MAC RX strip CRC.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] enable  1 - CRC is stripped from the received packet.
 *                      0 - CRC is not stripped from the received packet.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_strip_crc_get_f)(bcmpmac_access_t *pa,
                           bcmpmac_pport_t port, uint32_t *enable);

/*!
 * \brief Set MAC RX strip CRC.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] enable  1 - CRC is stripped from the received packet.
 *                     0 - CRC is not stripped from the received packet.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_strip_crc_set_f)(bcmpmac_access_t *pa,
                           bcmpmac_pport_t port, uint32_t enable);

/*!
 * \brief Get MAC TX threshold value.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] threshold TX threshold value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_tx_threshold_get_f)(bcmpmac_access_t *pa,
                              bcmpmac_pport_t port, uint32_t *threshold);

/*!
 * \brief Set MAC TX CRC mode.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] crc_mode TX crc mode setting.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_tx_threshold_set_f)(bcmpmac_access_t *pa,
                              bcmpmac_pport_t port, uint32_t threshold);

/*!
 * \brief Get MAC TX CRC mode setting.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [out] crc_mode TX CRC mode setting.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_tx_crc_mode_get_f)(bcmpmac_access_t *pa,
                             bcmpmac_pport_t port, uint32_t *crc_mode);

/*!
 * \brief Set MAC TX CRC mode setting.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] crc_mode TX CRC mode setting.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
typedef int
(*bcmpmac_tx_crc_mode_set_f)(bcmpmac_access_t *pa,
                             bcmpmac_pport_t port, uint32_t crc_mode);

/******************************************************************************
 * PortMAC driver object
 */

/*!
 * \brief PortMAC driver
 *
 * PortMAC driver function vector.
 *
 * There are two common parameters cross the functions of the driver.
 *
 * The first is \c pa (PAMC access info) which provides the necessary
 * infomation for hardware access. And the second is \c pm_pport (Port macro
 * physical port number) which indicates the target port of the operation.
 *
 * The physical port macro port is the physical port within a port macro. It is
 * sometimes referred to as the "block-port number" or the "port index".
 */
typedef struct bcmpmac_drv_s {

    /*! Driver name. */
    const char *name;

    /*! Port init function. */
    bcmpmac_port_init_f port_init;

    /*! Port reset set function. */
    bcmpmac_port_reset_set_f port_reset_set;

    /*! Port reset get function. */
    bcmpmac_port_reset_get_f port_reset_get;

    /*! Port enable set function. */
    bcmpmac_port_enable_set_f port_enable_set;

    /*! Port enable get function. */
    bcmpmac_port_enable_get_f port_enable_get;

    /*! MAC reset set function. */
    bcmpmac_mac_reset_set_f mac_reset_set;

    /*! MAC reset get function. */
    bcmpmac_mac_reset_get_f mac_reset_get;

    /*! Rx enable set function. */
    bcmpmac_rx_enable_set_f rx_enable_set;

    /*! Rx enable get function. */
    bcmpmac_rx_enable_get_f rx_enable_get;

    /*! Tx enable set function. */
    bcmpmac_tx_enable_set_f tx_enable_set;

    /*! Tx enable get function. */
    bcmpmac_tx_enable_get_f tx_enable_get;

    /*! Speed set function. */
    bcmpmac_speed_set_f speed_set;

    /*! Speed get function. */
    bcmpmac_speed_get_f speed_get;

    /*! Encap mode set function. */
    bcmpmac_encap_set_f encap_set;

    /*! Encap mode get function. */
    bcmpmac_encap_get_f encap_get;

    /*! MAC loopback set function. */
    bcmpmac_lpbk_set_f lpbk_set;

    /*! MAC loopback get function. */
    bcmpmac_lpbk_get_f lpbk_get;

    /*! Pause set function. */
    bcmpmac_pause_set_f pause_set;

    /*! Pause get function. */
    bcmpmac_pause_get_f pause_get;

    /*! PAUSE frame source address set function. */
    bcmpmac_pause_addr_set_f pause_addr_set;

    /*! PAUSE frame source address get function. */
    bcmpmac_pause_addr_get_f pause_addr_get;

    /*! Max receive frame size set function. */
    bcmpmac_frame_max_set_f frame_max_set;

    /*! Max receive frame size get function. */
    bcmpmac_frame_max_get_f frame_max_get;

    /*! Local/remote fault get function. */
    bcmpmac_fault_status_get_f fault_status_get;

    /*! Core port mode set function. */
    bcmpmac_core_mode_set_f core_mode_set;

    /*! Excute operation. */
    bcmpmac_op_exec_f op_exec;

    /*! Failover set function. */
    bcmpmac_failover_enable_set_f failover_enable_set;

    /*! Failover get function. */
    bcmpmac_failover_enable_get_f failover_enable_get;

    /*! Failover loopback status get function. */
    bcmpmac_failover_loopback_get_f failover_loopback_get;

    /*! Failover loopback bring out function. */
    bcmpmac_failover_loopback_remove_f failover_loopback_remove;

    /*! PFC set function. */
    bcmpmac_pfc_set_f pfc_set;

    /*! PFC get function. */
    bcmpmac_pfc_get_f pfc_get;

    /*! Port mode set function. */
    bcmpmac_port_mode_set_f port_mode_set;

    /*! Local/remote fault disable get function. */
    bcmpmac_fault_disable_set_f fault_disable_set;

    /*! Local/remote fault disable get function. */
    bcmpmac_fault_disable_get_f fault_disable_get;

    /*! IPG size set function. */
    bcmpmac_avg_ipg_set_f avg_ipg_set;

    /*! IPG size get function. */
    bcmpmac_avg_ipg_get_f avg_ipg_get;

    /*! IPG size set function. */
    bcmpmac_interrupt_enable_set_f interrupt_enable_set;

    /*! IPG size get function. */
    bcmpmac_interrupt_enable_get_f interrupt_enable_get;

    /*! MIB oversize set function. */
    bcmpmac_mib_oversize_set_f mib_oversize_set;

    /*! MIB oversize get function. */
    bcmpmac_mib_oversize_get_f mib_oversize_get;

    /*! Pause frame handling set function. */
    bcmpmac_pass_pause_frame_set_f  pass_pause_set;

    /*! Pause frame handling get function. */
    bcmpmac_pass_pause_frame_get_f  pass_pause_get;

    /*! PFC frame handling set function. */
    bcmpmac_pass_pfc_frame_set_f  pass_pfc_set;

    /*! PFC frame handling get function. */
    bcmpmac_pass_pfc_frame_get_f  pass_pfc_get;

    /*! Control frames handling set function. */
    bcmpmac_pass_control_frame_set_f  pass_control_set;

    /*! Control frames handling get function. */
    bcmpmac_pass_control_frame_get_f  pass_control_get;

    /*! Set Frames handling from egress pipeline to MAC. */
    bcmpmac_discard_set_f  discard_set;

    /*! Get Frames handling from egress pipeline to MAC. */
    bcmpmac_discard_get_f  discard_get;

    /*! Set software link status control. */
    bcmpmac_sw_link_status_set_f  sw_link_status_set;

    /*! Get software link status control. */
    bcmpmac_sw_link_status_get_f  sw_link_status_get;

    /*! Set link status indication select. */
    bcmpmac_link_status_select_set_f  link_status_select_set;

    /*! Set link status indication select. */
    bcmpmac_link_status_select_get_f  link_status_select_get;

    /*! Set force link interrupt control. */
    bcmpmac_link_interrupt_ordered_set_f  link_interrupt_ordered_set;

    /*! Get force link interrupt control. */
    bcmpmac_link_interrupt_ordered_get_f  link_interrupt_ordered_get;

    /*! Get link interruption live status. */
    bcmpmac_link_interrupt_live_status_get_f  link_interrupt_live_status_get;

    /*! Set TX stall. */
    bcmpmac_stall_tx_enable_set_f  stall_tx_enable_set;

    /*! Set TX stall. */
    bcmpmac_stall_tx_enable_get_f  stall_tx_enable_get;

    /*! Get link latch down status. */
    bcmpmac_port_link_status_get_f  port_link_status_get;

    /*! Port macro mac control set. */
    bcmpmac_port_mac_control_set_f  port_mac_control_set;

    /*! Port macro mac control get. */
    bcmpmac_port_mac_control_get_f  port_mac_control_get;

    /*! Port macro tsc control reset. */
    bcmpmac_port_tsc_ctrl_set_f  tsc_ctrl_set;

    /*! Port macro tsc control get. */
    bcmpmac_port_tsc_ctrl_get_f  tsc_ctrl_get;

    /*! RUNT threshold set function. */
    bcmpmac_runt_threshold_set_f  runt_threshold_set;

    /*! RUNT threshold get function. */
    bcmpmac_runt_threshold_get_f  runt_threshold_get;

    /*! Force PFC XON set function. */
    bcmpmac_force_pfc_set_f force_pfc_xon_set;

    /*! Force PFC XON get function. */
    bcmpmac_force_pfc_get_f force_pfc_xon_get;

    /*! MAC selective Receive Statistic Vector (RSV) set function. */
    bcmpmac_rsv_selective_mask_set_f rsv_selective_mask_set;

    /*! MAC selective Receive Statistic Vector (RSV) get function. */
    bcmpmac_rsv_selective_mask_get_f rsv_selective_mask_get;

    /*! MAC Receive Statistic Vector (RSV) set function. */
    bcmpmac_rsv_mask_set_f rsv_mask_set;

    /*! MAC Receive Statistic Vector (RSV) get function. */
    bcmpmac_rsv_mask_get_f rsv_mask_get;

    /*! Port TX timestamp FIFO get. */
    bcmpmac_port_tx_timestamp_get_f tx_timestamp_info_get;

    /*! Port Macro egress 1588 timestamping mode get. */
    bcmpmac_egress_timestamp_mode_get_f egress_timestamp_mode_get;

    /*! Port Macro egress 1588 timestamping mode set. */
    bcmpmac_egress_timestamp_mode_set_f egress_timestamp_mode_set;

    /*! Port Macro 16B credit granularity get. */
    bcmpmac_16b_credit_granularity_get_f credit_granularity_get;

    /*! Port Macro 16B credit granularity set. */
    bcmpmac_16b_credit_granularity_set_f credit_granularity_set ;

    /*! cdport cell mode set. */
    bcmpmac_cell_mode_set_f cell_mode_set;

    /*! Strip CRC get. */
    bcmpmac_strip_crc_get_f  strip_crc_get;

    /*! Strip CRC set. */
    bcmpmac_strip_crc_set_f  strip_crc_set;

    /*! MAC TX threshold get. */
    bcmpmac_tx_threshold_get_f  tx_threshold_get;

    /*! MAC TX threshold set. */
    bcmpmac_tx_threshold_set_f  tx_threshold_set;

    /*! MAC TX CRC mode get. */
    bcmpmac_tx_crc_mode_get_f  tx_crc_mode_get;

    /*! MAC TX CRC mode set. */
    bcmpmac_tx_crc_mode_set_f  tx_crc_mode_set;

} bcmpmac_drv_t;

#endif /* BCMPMAC_DRV_H */
