/*! \file bcmpc_pm_drv_pm8x50_common.h
 *
 * PM8x50 PortMacro Manager common definitions.
 *
 * Declaration of the common structures, enumerations, and functions for
 * the PM8x50 PortMacro Managers.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPC_PM_DRV_PM8X50_COMMON_H
#define BCMPC_PM_DRV_PM8X50_COMMON_H

/*!
 * Port Macro Receive Statistics Vector (RSV) definitions.
 */
#define PM8X50_COMMON_RSV_MASK_WRONG_SA             (0x1 << (0))
#define PM8X50_COMMON_RSV_MASK_STACK_VLAN_DETECT    (0x1 << (1))
/* unsupported PFC DA*/
#define PM8X50_COMMON_RSV_MASK_PFC_DA_ERR           (0x1 << (2))
/* same bit as PFC */
#define PM8X50_COMMON_RSV_MASK_PAUSE_DA_ERR         (0x1 << (2))
#define PM8X50_COMMON_RSV_MASK_RCV_TERM_OR_CODE_ERR (0x1 << (3))
#define PM8X50_COMMON_RSV_MASK_CRC_ERR              (0x1 << (4))
/* IEEE length check fail */
#define PM8X50_COMMON_RSV_MASK_FRAME_LEN_ERR        (0x1 << (5))
/* truncated/out of range */
#define PM8X50_COMMON_RSV_MASK_TRUNCATED_FRAME      (0x1 << (6))
#define PM8X50_COMMON_RSV_MASK_FRAME_RCV_OK         (0x1 << (7))
#define PM8X50_COMMON_RSV_MASK_MCAST_FRAME          (0x1 << (8))
#define PM8X50_COMMON_RSV_MASK_BCAST_FRAME          (0x1 << (9))
#define PM8X50_COMMON_RSV_MASK_PROMISCUOUS_FRAME    (0x1 << (10))
#define PM8X50_COMMON_RSV_MASK_CONTROL_FRAME        (0x1 << (11))
#define PM8X50_COMMON_RSV_MASK_PAUSE_FRAME          (0x1 << (12))
#define PM8X50_COMMON_RSV_MASK_OPCODE_ERR           (0x1 << (13))
#define PM8X50_COMMON_RSV_MASK_VLAN_TAG_DETECT      (0x1 << (14))
#define PM8X50_COMMON_RSV_MASK_UCAST_FRAME          (0x1 << (15))
#define PM8X50_COMMON_RSV_MASK_RUNT_FRAME           (0x1 << (16))
#define PM8X50_COMMON_RSV_MASK_RESERVED_1           (0x1 << (17))
#define PM8X50_COMMON_RSV_MASK_PFC_FRAME            (0x1 << (18))

#define PM8X50_COMMON_RSV_MASK_MIN   PM8X50_COMMON_RSV_MASK_WRONG_SA
#define PM8X50_COMMON_RSV_MASK_MAX   PM8X50_COMMON_RSV_MASK_PFC_FRAME
#define PM8X50_COMMON_RSV_MASK_ALL   ((PM8X50_COMMON_RSV_MASK_PFC_FRAME) | \
                                     ((PM8X50_COMMON_RSV_MASK_PFC_FRAME) - 1))


/* !  RLM state definitions.
 * Defines:
 *     pm8x50_common_port_rlm_fsm_t *
 * Purpose:
 *     Valid values Reduced Lane Mode state
 */
typedef enum pm8x50_common_port_rlm_fsm_e {
    PM8X50_COMMON_PORT_RLM_SEND_PACKET_DATA,  /*entry state for RLM */
    PM8X50_COMMON_PORT_RLM_STOP_MAC,          /* STALL TX */
    PM8X50_COMMON_PORT_RLM_SEND_LI,           /* SEND link interrupt protocol */
    PM8X50_COMMON_PORT_RLM_SEND_PRBS,         /* SEND prbs data  */
    PM8X50_COMMON_PORT_RLM_SEND_IDLE,         /* send idle */
    PM8X50_COMMON_PORT_RLM_DONE,              /* RLM transition done */
    PM8X50_COMMON_PORT_RLM_ERROR,             /* RLM error state */
    PM8X50_COMMON_PORT_RLM_FSM_COUNT
} pm8x50_common_port_rlm_fsm_t;

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
int
pm8x50_common_pm_destroy(int unit, pm_info_t *pm_info);

/*!
 * \brief Port speed get function.
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
int
pm8x50_common_port_speed_config_get(int unit, const pm_access_t *pm_acc,
                                    pm_oper_status_t *op_st, pm_info_t *pm_info,
                                    pm_speed_config_t *speed_config);

/*!
 * \brief Port looback set function.
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
int
pm8x50_common_port_loopback_set(int unit, const pm_access_t *pm_acc,
                                pm_oper_status_t *op_st, pm_info_t *pm_info,
                                pm_loopback_mode_t loopback_type, int enable);

/*!
 * \brief Port looback get function.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [in] loopback_type Supported looback type.
 * \param [out] enable Enable/disable loopback function.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
int
pm8x50_common_port_loopback_get(int unit, const pm_access_t *pm_acc,
                                pm_oper_status_t *op_st, pm_info_t *pm_info,
                                pm_loopback_mode_t loopback_type, int *enable);

/*!
 * \brief Port encapsulation set function.
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
int
pm8x50_common_port_encap_set(int unit, const pm_access_t *pm_acc,
                             pm_oper_status_t *op_st, pm_info_t *pm_info,
                             pm_encap_t encap);

/*!
 * \brief Port encapsulation get function.
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
int
pm8x50_common_port_encap_get(int unit, const pm_access_t *pm_acc,
                             pm_oper_status_t *op_st, pm_info_t *pm_info,
                             pm_encap_t *encap);

/*!
 * \brief Port pause set function.
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
int
pm8x50_common_port_pause_ctrl_set(int unit, const pm_access_t *pm_acc,
                                  pm_oper_status_t *op_st, pm_info_t *pm_info,
                                  pm_pause_ctrl_t *pause_ctrl);

/*!
 * \brief Port pause get function.
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
int
pm8x50_common_port_pause_ctrl_get(int unit, const pm_access_t *pm_acc,
                                  pm_oper_status_t *op_st,
                                  pm_info_t *pm_info,
                                  pm_pause_ctrl_t *pause_ctrl);

/*!
 * \brief Port pause address set function.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [in] pause_addr Pause MAC address.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
int
pm8x50_common_port_pause_addr_set(int unit, const pm_access_t *pm_acc,
                                  pm_oper_status_t *op_st,
                                  pm_info_t *pm_info,
                                  pm_mac_t pause_addr);

/*!
 * \brief Port pause address get function.
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
int
pm8x50_common_port_pause_addr_get(int unit, const pm_access_t *pm_acc,
                                  pm_oper_status_t *op_st,
                                  pm_info_t *pm_info, pm_mac_t pause_addr);

/*!
 * \brief Port max frame size set function.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [in] max_frame_size Maximum frame size.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
int
pm8x50_common_port_frame_max_set(int unit, const pm_access_t *pm_acc,
                                 pm_oper_status_t *op_st,
                                 pm_info_t *pm_info, uint32_t max_frame_size);

/*!
 * \brief Port max frame size get function.
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
int
pm8x50_common_port_frame_max_get(int unit, const pm_access_t *pm_acc,
                                 pm_oper_status_t *op_st,
                                 pm_info_t *pm_info, uint32_t *max_frame_size);

/*!
 * \brief Port enable status set function.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [in] flags. Indicator to check port enable status
 * \param [in] enable Port enable value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
int
pm8x50_common_port_enable_set(int unit, const pm_access_t *pm_acc,
                              pm_oper_status_t *op_st, pm_info_t *pm_info,
                              uint32_t flags, uint32_t enable);

/*!
 * \brief Port enable status get function.
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
int
pm8x50_common_port_enable_get(int unit, const pm_access_t *pm_acc,
                              pm_oper_status_t *op_st, pm_info_t *pm_info,
                              uint32_t flags, uint32_t *enable);

/*!
 * \brief Port lag failover lookback get function.
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
int
pm8x50_common_port_lag_failover_loopback_get(int unit, const pm_access_t *pm_acc,
                                         pm_oper_status_t *op_st,
                                         pm_info_t *pm_info, uint32_t *enable);

/*!
 * \brief Port tx lane config set function.
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
int
pm8x50_common_port_tx_set(int unit, const pm_access_t *pm_acc,
                          pm_oper_status_t *op_st, pm_info_t *pm_info,
                          phymod_tx_t *tx_config);

/*!
 * \brief Port tx lane config get function.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [out] tx_config Tx lane config.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
int
pm8x50_common_port_tx_get(int unit, const pm_access_t *pm_acc,
                          pm_oper_status_t *op_st, pm_info_t *pm_info,
                          phymod_tx_t *tx_config);

/*!
 * \brief Port PMD lane config get function.
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
int
pm8x50_common_port_pmd_lane_cfg_get(int unit, const pm_access_t *pm_acc,
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
int
pm8x50_common_port_failover_enable_set(int unit, const pm_access_t *pm_acc,
                                       pm_oper_status_t *op_st,
                                       pm_info_t *pm_info, uint32_t enale);

/*!
 * \brief Get LAG failover enable state.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [out] enable 1 for enable, otherwise disabled.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
int
pm8x50_common_port_failover_enable_get(int unit, const pm_access_t *pm_acc,
                                       pm_oper_status_t *op_st,
                                       pm_info_t *pm_info, uint32_t *enale);

/*!
 * \brief Set port link training enabled.
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
int
pm8x50_common_port_link_training_set(int unit, const pm_access_t *pm_acc,
                                     pm_oper_status_t *op_st,
                                     pm_info_t *pm_info, uint32_t enale);

/*!
 * \brief Get port link training enabled.
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
int
pm8x50_common_port_link_training_get(int unit, const pm_access_t *pm_acc,
                                     pm_oper_status_t *op_st,
                                     pm_info_t *pm_info, uint32_t *enale);

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
int
pm8x50_common_avg_ipg_set(int unit, const pm_access_t *pm_acc,
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
int
pm8x50_common_avg_ipg_get(int unit, const pm_access_t *pm_acc,
                              pm_oper_status_t *op_st, pm_info_t *pm_info,
                              uint8_t *ipg_size);

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
int
pm8x50_common_phy_status_get(int unit, const pm_access_t *pm_acc,
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
 * \param [in] pm_ctrl PM phy control structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
int
pm8x50_common_phy_control_set(int unit, const pm_access_t *pm_acc,
                              pm_oper_status_t *op_st, pm_info_t *pm_info,
                              uint32_t flags, pm_phy_control_t *pm_ctrl);

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
int
pm8x50_common_port_synce_clk_ctrl_set(int unit, const pm_access_t *pm_acc,
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
int
pm8x50_common_port_synce_clk_ctrl_get(int unit, const pm_access_t *pm_acc,
                                      pm_oper_status_t *op_st,
                                      pm_info_t *pm_info,
                                      pm_port_synce_clk_ctrl_t *cfg);

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
int
pm8x50_common_port_link_training_status_get(int unit, const pm_access_t *pm_acc,
                                            pm_oper_status_t *op_st,
                                            pm_info_t *pm_info,
                                            uint32_t *locked);

/*!
 * \brief Get port default tx taps configurations.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [in] speed_config Port speed configuration structure.
 * \param [out] tx_taps_config default tx taps configurations.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
int
pm8x50_common_port_default_tx_taps_config_get(int unit, const pm_access_t *pm_acc,
                                              pm_oper_status_t *op_st, pm_info_t *pm_info,
                                              pm_speed_config_t *speed_config,
                                              phymod_tx_t *tx_taps_config);

/*!
 * \brief Get PM core polarity flip configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [out] polarity Polarity flip configuration structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
int
pm8x50_common_phy_polarity_get(int unit, const pm_access_t *pm_acc,
                               pm_oper_status_t *op_st, pm_info_t *pm_info,
                               phymod_polarity_t *polarity);

/*!
 * \brief Get PM core lane map configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [out] lane_map Lane map configuration structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
int
pm8x50_common_core_lane_map_get(int unit, const pm_access_t *pm_acc,
                                pm_oper_status_t *op_st, pm_info_t *pm_info,
                                phymod_lane_map_t *lane_map);

/*!
 * \brief Get a port TX packet timestamp from FIFO.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_acc MAC and PHY access information structure.
 * \param [out] op_st Operation status for this function call.
 * \param [in] pm_info PM state structure.
 * \param [out] timestamp_info Packet timestamp dtata structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
int
pm8x50_common_port_tx_timestamp_info_get(int unit, const pm_access_t *pm_acc,
                                  pm_oper_status_t *op_st,
                                  pm_info_t *pm_info,
                                  pm_tx_timestamp_info_t *timestamp_info);

/*!
 * \brief Get FEC uncorrectable counter for a port
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
int
pm8x50_common_phy_rsfec_uncorrectable_counter_get(int unit,
                                                  const pm_access_t *pm_acc,
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
int
pm8x50_common_phy_rsfec_correctable_counter_get(int unit,
                                                const pm_access_t *pm_acc,
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
int
pm8x50_common_phy_rsfec_symbol_error_counter_get(int unit,
                                                 const pm_access_t *pm_acc,
                                                 pm_info_t *pm_info,
                                                 uint64_t *count);

#endif /* BCMPC_PM_DRV_PM8X50_COMMON_H */
