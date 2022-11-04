/** \file imb_cdu_internal.h
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef IMB_CDU_INTERNAL_H_INCLUDED
/*
 * {
 */
#define IMB_CDU_INTERNAL_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

/*
 * INCLUDE FILES:
 * {
 */

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
 *
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
 *
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
 *
 */
int imb_cdu_ilkn_logic_power_down_set(
    int unit,
    bcm_core_t core_id,
    int cdu_id,
    int power_down);

/**
 * \brief - Configure power down of  CDU TX
 *
 */
int imb_cdu_tx_power_down_set(
    int unit,
    bcm_core_t core_id,
    int cdu_id,
    int power_down);

/**
 * \brief - Set PM's refclk termination correction.
 *
 */
int imb_cdu_refclk_termination_set(
    int unit,
    int core,
    int cdu_id,
    int is_sw_correction);

/**
 * \brief - Set cdmac(port belong to) block valid/invalid .
 *
 */
int imb_cdu_port_cdmac_valid_enable(
    int unit,
    bcm_port_t port,
    int enable);

/**
 * \brief - Judge the xpmd is supported or not.
 *
 */
int imb_cdu_port_xpmd_is_enabled(
    int unit,
    bcm_port_t port,
    uint8 *is_xpmd_enabled);

/**
 * \brief - Enable the xpmd lanes.
 *
 */
int imb_cdu_port_xpmd_lane_enable(
    int unit,
    bcm_port_t port,
    uint32 flags,
    int enable);

/**
 * \brief - Enable 400Gbit Ethernet for the CDU
 *
 * \param [in] unit - chip unit id
 * \param [in] core - device core
 * \param [in] cdu_id - cdu index inside the core
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
int imb_cdu_400ge_enable_set(
    int unit,
    bcm_core_t core,
    int cdu_id,
    int enable);

/**
 * \brief - Enable Frame Preemption for the CDU
 *
 * \param [in] unit - chip unit id
 * \param [in] core - device core
 * \param [in] cdu_id - cdu index inside the core
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
int imb_cdu_frame_preemption_enable_set(
    int unit,
    bcm_core_t core,
    int cdu_id,
    int enable);

/**
* \brief - Reset per port instrumentation counters for PFC deadlock detection mechanism
* and/or instrumentatino traffic counters.
*
* \param [in] unit - chip unit id
* \param [in] pm_index - PM index
* \param [in] port_index_base - Base port index
* \param [in] nof_port_indexes - Number of indexes for port
* \return
*   int - see _SHR_E_ *
*
* \remarks
*   * None
* \see
*   * None
*/
int imb_cdu_internal_instru_traffic_pfc_deadlock_counters_ofr_reset(
    int unit,
    uint32 pm_index,
    uint32 port_index_base,
    uint32 nof_port_indexes);

/**
* \brief - Get per IMB instrumentation sync counters last record written index.
*
* \param [in] unit - chip unit id
* \param [in] port_index_base - Base port index
* \param [in] nof_port_indexes - Number of indexes for port
* \param [in] nof_tcs - Number of TCs to return for port
* \param [in] record_index - The record to read
* \param [in] nof_counters - Number of counters to be read. Number of elements in dbal_fields and counter_values.
* \param [in] dbal_fields - An array (with nof_counters elements) of the DBAL fields to collect.
* \param [out] counter_values - An array (with nof_counters elements) of the values of the counters.

* \return
*   int - see _SHR_E_ *
*
* \remarks
*   * None
* \see
*   * None
*/
int imb_cdu_internal_instru_synced_counters_per_record_ofr_get(
    int unit,
    uint32 port_index_base,
    uint32 nof_port_indexes,
    uint32 nof_tcs,
    int record_index,
    int nof_counters,
    const int dbal_fields[],
    uint64 counter_values[]);

/**
 * \brief - Return the link mux to its default value
 *
 * \param [in] unit - chip unit id
 * \param [in] core - device core
 * \param [in] cdu_id - cdu index inside the core
 * \param [in] lane_id_cdu - lane ID CDU
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_hw_linkscan_link_mux_clear(
    int unit,
    bcm_core_t core,
    uint32 cdu_id,
    int lane_id_cdu);

/**
 * \brief - Configure the link mux in CDU for HW linkscan
 *    For OTN port, the value is 0 ~ 7.
 *    For FlexE port, the value is 8.
 *    For ETH port, the value is 9.
 *
 * \param [in] unit - chip unit id
 * \param [in] core - device core
 * \param [in] cdu_id - cdu index inside the core
 * \param [in] lane_id_cdu - lane ID CDU
 * \param [in] link_sel - the link status selection value
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_hw_linkscan_link_mux_set(
    int unit,
    bcm_core_t core,
    uint32 cdu_id,
    int lane_id_cdu,
    uint32 link_sel);

/**
 * \brief - Get the CDU PM reset status
 *
 * \param [in] unit - chip unit id
 * \param [in] core - core id
 * \param [in] ethu_id_in_core - ethu id in core
 * \param [out] in_reset - indicates if the priority and TDM is valid
 *
 * \return
 *   int - seee _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_internal_pm_reset_get(
    int unit,
    bcm_core_t core,
    int ethu_id_in_core,
    int *in_reset);

/**
 * \brief - Set the per lane mask for the link status change interrupt
 *    1 - interrupt enabled, 0 - interrupt disabled
 *
 * \param [in] unit - chip unit id
 * \param [in] core - device core
 * \param [in] ethu_id_in_core - ethu index inside the core
 * \param [in] lane_id_cdu - lane ID CDU
 * \param [in] mask - the link status selection value
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_hw_per_lane_link_status_change_intr_mask_set(
    int unit,
    bcm_core_t core,
    uint32 ethu_id_in_core,
    int lane_id_cdu,
    uint32 mask);

/**
 * \brief - Set the per cdu instance mask for the link status change interrupt
 *    1 - interrupt enabled, 0 - interrupt disabled
 *
 * \param [in] unit - chip unit id
 * \param [in] core - device core
 * \param [in] ethu_id_in_core - ethu index inside the core
 * \param [in] mask - the link status selection value
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_cdu_hw_per_cdu_instance_link_status_change_intr_mask_set(
    int unit,
    bcm_core_t core,
    uint32 ethu_id_in_core,
    uint32 mask);

/*
  * }
  */
#endif/*_IMB_CDU_INTERNAL_H_INCLUDED_*/
