/** \file imb_cdu_internal.h
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef IMB_CDU_INTERNAL_H_INCLUDED
/*
 * {
 */
#define IMB_CDU_INTERNAL_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * INCLUDE FILES:
 * {
 */

#include <shared/shrextend/shrextend_error.h>
#include <bcm/port.h>
/*
 * }
 */

/*
 * MACROs
 * {
 */
 /*
  * }
  */

/**
 * \brief - Configure the MAC mode on a specific mac_mode_config interface.
 * this method should be called only when loading the first port
 * on the MAC, or removing the last port on the MAC mode config.
 * when MAC is enabled, the MAC mode will be set according to
 * mac_mode, when MAC is disabled, mac_mode argument will be
 * ignored, and MAC mode will be set to default reg value.
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] mac_mode - MAC operation mode. see
 *        DBAL_FIELD_MAC_MODE
 * \param [in] enable - enable indication
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_port_mac_mode_config_set(
    int unit,
    bcm_port_t port,
    int mac_mode,
    int enable);

/**
 * \brief - Get the the MAC mode config in the CDU
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [out] mac_mode - MAC operation mode. see
 *        DBAL_FIELD_MAC_MODE
 */
int imb_cdu_port_mac_mode_config_get(
    int unit,
    bcm_port_t port,
    int *mac_mode);

/**
 * \brief - Enable / disable the MAC in the CDU.
 * this method should be called only when loading the first port
 * on the MAC, or removing the last port on the MAC.
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] enable - enable indication
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_port_mac_enable_set(
    int unit,
    bcm_port_t port,
    int enable);

/**
 * \brief - Get the the MAC mode in the CDU
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [out] enable - enable indication
 */
int imb_cdu_port_mac_enable_get(
    int unit,
    bcm_port_t port,
    int *enable);
/**
 * \brief - Reset the Async unit in the CDU
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] in_reset - reset indication.
 * in_reset = 1 --> put the async unit in reset
 * in_reset = 0 --> take the async unit out of reset
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_port_async_unit_reset(
    int unit,
    bcm_port_t port,
    int in_reset);

/**
 * \brief - Reset the PM for the entire CDU.
 *
 * \param [in] unit - chip unit id
 * \param [in] core - device core
 * \param [in] cdu_id - cdu index inside the core
 * \param [in] in_reset - reset indication.
 * in_reset = TRUE -> put in reset
 * in_reset = FALSE -> take out of reset
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_pm_reset(
    int unit,
    int core,
    int cdu_id,
    int in_reset);

/**
 * \brief - get all ilkn master logical ports on a CDU
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] cdu_ilkn_ports - bitmap of master ports on the CDU
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_ilkn_master_ports_get(
    int unit,
    bcm_port_t port,
    bcm_pbmp_t * cdu_ilkn_ports);

/**
 * \brief - get TX start threshold. this threshold represent the
 *        number of 64B words to be accumulated in the MLF
 *        before transmitting towards the PM. This is to prevent
 *        TX MAC starvation and is important for systems with
 *        oversubscription.
 *
 * \param [in] unit - chip unit id
 * \param [in] core - device core
 * \param [in] cdu_id - cdu index inside the core
 * \param [in] first_lane_in_port - lane inside the CDU
 * \param [out] start_thr - threshold value
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_tx_start_thr_hw_set(
    int unit,
    bcm_core_t core,
    uint32 cdu_id,
    int first_lane_in_port,
    uint32 start_thr);
/**
 * \brief - Enable EEE for the CDU
 *
 * \param [in] unit - chip unit id
 * \param [in] core - device core
 * \param [in] cdu_id - cdu index inside the core
 * \param [in] enable - enable indication
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_eee_enable_set(
    int unit,
    bcm_core_t core,
    int cdu_id,
    int enable);

/**
 * \brief - Enable IDDQ for the CDU
 *
 * \param [in] unit - chip unit id
 * \param [in] core - device core
 * \param [in] cdu_id - cdu index inside the core
 * \param [in] enable - enable indication
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_iddq_enable_set(
    int unit,
    bcm_core_t core,
    int cdu_id,
    int enable);

/**
 * \brief - map NIF port to EGQ interface
 *
 * \param [in] unit   - chip unit id
 * \param [in] core   - device core
 * \param [in] nif_id - NIF ID. range of 0-47.
 * \param [in] egq_if - EQQ interface. range of 0-63.
 * \param [in] enable - 1:enable mapping, 0:disable mapping
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_nif_egq_interface_map(
    int unit,
    bcm_core_t core,
    uint32 nif_id,
    uint32 egq_if,
    int enable);

/**
 * \brief - Set PRD logic in Bypass mode. the PRD parser will be bypassed.
 *
 * \param [in] unit - chip unit id
 * \param [in] core - device core
 * \param [in] ethu_id_in_core - ethu index inside the core
 * \param [out] is_bypass - bypass mode
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_prd_bypass_get(
    int unit,
    int core,
    int ethu_id_in_core,
    uint32 *is_bypass);
/**
 * \brief - Set port-macro MAC in reset.
 *
 * \param [in] unit - chip unit id
 * \param [in] core - device core
 * \param [in] cdu_id - cdu index inside the core
 * \param [in] first_lane_in_port - first_lane_in_port (which extracted from the logical port)
 * \param [in] reset - reset value
 *
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_rx_gear_shift_reset_set(
    int unit,
    bcm_core_t core,
    uint32 cdu_id,
    int first_lane_in_port,
    int reset);

/**
 * \brief - set PRD thresholds per priority. based on the
 *        priority given from the parser, the packet is sent to
 *        the correct RMC. in each RMC there is a thresholds per
 *        priority which is mapped to it.
 */
int imb_cdu_prd_threshold_hw_set(
    int unit,
    bcm_core_t core,
    int cdu_id,
    uint32 rmc,
    uint32 priority,
    uint32 threshold);
/**
 * \brief - get the current threshold for a specific priority in
 *        a specific RMC.
 */
int imb_cdu_prd_threshold_hw_get(
    int unit,
    bcm_core_t core,
    int cdu_id,
    uint32 rmc,
    uint32 priority,
    uint32 *threshold);

/**
 * \brief - Configure power down of ILKN FEC of CDU
 */
int imb_cdu_ilkn_logic_power_down_set(
    int unit,
    bcm_core_t core_id,
    int cdu_id,
    int power_down);

/**
 * \brief - Configure power down of  CDU TX
 */
int imb_cdu_tx_power_down_set(
    int unit,
    bcm_core_t core_id,
    int cdu_id,
    int power_down);

 /*
  * }
  */
#endif/*_IMB_CDU_INTERNAL_H_INCLUDED_*/
