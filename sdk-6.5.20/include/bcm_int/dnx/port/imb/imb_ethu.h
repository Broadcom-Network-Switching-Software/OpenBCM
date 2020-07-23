/** \file imb_ethu.h
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef IMB_ETHU_H_INCLUDED
/*
 * {
 */
#define IMB_ETHU_H_INCLUDED

#include <soc/portmod/portmod.h>
#include <bcm/port.h>
#include <bcm/cosq.h>
#include <bcm_int/dnx/cosq/ingress/cosq_ingress_port.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>

#include <bcm_int/dnx/port/imb/imb.h>

/**
 * \brief - Disable traffic for all lanes that belong to the given ETHU :
 *
 * \param [in] unit - chip unit id
 * \param [in] core - core id
 * \param [in] ethu_id - ethu id to be disabled
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_tx_data_disable_traffic(
    int unit,
    bcm_core_t core,
    int ethu_id);

/**
 * \brief - Configure the ethu in the specified Scheduler
 *
 * see .h file
 */
int imb_ethu_scheduler_config_set(
    int unit,
    bcm_core_t core,
    uint32 ethu_id,
    bcm_port_nif_scheduler_t sch_prio,
    int weight);

/**
 * \brief - set the requested weight in the low priority RMC
 *        scheduler. this scheduler is performing RR on all RMCs
 *        with traffic, and each rMC has more than one bit in
 *        the RR. the weight is actually the number of bits in
 *        the RR.
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] rmc_id - rmc id to set the weight to
 * \param [in] weight - weight value
 *
 * \return
 *   int - see _SHR_E_ *
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_rmc_low_prio_scheduler_config_set(
    int unit,
    bcm_port_t port,
    int rmc_id,
    int weight);

/*
 * Functions
 * {
 */
 /**
 * \brief - get port ETHU active indication in the specified
 *        scheduler.
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] sch_prio - Scheduler to configure. see
 *        bcm_port_nif_scheduler_t
 * \param [out] active - active indication. if the ETHU has any
 *        BW in the scheduler returns true, otherwise returns
 *        false.
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_port_scheduler_active_get(
    int unit,
    bcm_port_t port,
    bcm_port_nif_scheduler_t sch_prio,
    uint32 *active);

/**
 * \brief - Set the logical FIFO threshold after overflow.
 * after the fifo reaches overflow, it will not resume writing
 * until fifo level will get below this value.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] rmc - logical fifo info
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_port_thr_after_ovf_set(
    int unit,
    bcm_port_t port,
    dnx_algo_port_rmc_info_t * rmc);

/**
 * \brief - map RMC (logical FIFO) to a physical lane in the ETHU
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] rmc - logical fifo info.
 * \param [in] is_map_valid - indication whether to map /unmap
 *        the rmc to lane. this value should be 0 when the port
 *        is on the process of being removed.
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_port_rmc_to_lane_map(
    int unit,
    bcm_port_t port,
    dnx_algo_port_rmc_info_t * rmc,
    int is_map_valid);

/**
 * \brief - Enable / disable egress flush. Egress flush means
 *        continuously granting credits to the port in order to
 *        free occupied resources.
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] flush_enable - enable flush indication
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_port_tx_egress_flush_set(
    int unit,
    bcm_port_t port,
    int flush_enable);

/**
 * \brief - Reset priority groups in the scheduler.
 *          Configure the specific sch_priority, or all sch_priorities
 *          of the port in case sch_priority=IMB_COSQ_ALL_SCH_PRIO.
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] sch_priority - Scheduler to configure. see
 *        bcm_port_nif_scheduler_t.
 *        Use IMB_COSQ_ALL_SCH_PRIO to reset all sch_prio of the port.
 * \param [in] in_reset - reset indication
 *        Can use one of the following:
 *          IMB_COMMON_IN_RESET - put the scheduler in reset.
 *          IMB_COMMON_OUT_OF_RESET - get the scheduler out of reset.
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_port_rx_nmg_reset_sch_prio_set(
    int unit,
    bcm_port_t port,
    uint32 sch_priority,
    int in_reset);

/*Enable / disable a port*/
int imb_ethu_port_enable_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    int enable);

/**
 * \brief - Get port enable indication
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] enable - enable indication
 *
 * \return
 *   int - see _SHR_E_ *
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_port_enable_get(
    int unit,
    bcm_port_t port,
    int *enable);

/**
 * \brief - Enable / disable Tx data to the PM
 *
 * \param [in] unit - chip unit id
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
int imb_ethu_port_tx_data_to_pm_enable_set(
    int unit,
    bcm_port_t port,
    int enable);

/**
 * \brief - Enable / disable port loopback
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] loopback - loopback type
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_port_loopback_set(
    int unit,
    bcm_port_t port,
    int loopback);

/**
 * \brief - Get loopback type on the port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] loopback - loopback type
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_port_loopback_get(
    int unit,
    bcm_port_t port,
    int *loopback);

/**
 * \brief - Enable / disable a port's RMCs to produce PFC for the specified priority
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] priority - PFC priority
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
int imb_ethu_port_gen_pfc_from_rmc_enable_set(
    int unit,
    bcm_port_t port,
    uint32 priority,
    uint32 enable);

/**
 * \brief - Get Enable / disable indication to produce PFC for the specified priority from a port's RMCs
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] priority - PFC priority
 * \param [out] enable - enable indication
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_port_gen_pfc_from_rmc_enable_get(
    int unit,
    bcm_port_t port,
    uint32 priority,
    uint32 *enable);

/**
 * \brief - Enable / disable port's RMCs(all lanes) to generate LLFC
 *
 * \param [in] unit - chip unit id
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
int imb_ethu_port_gen_llfc_from_rmc_enable_set(
    int unit,
    bcm_port_t port,
    uint32 enable);

/**
 * \brief - Enable / disable PM to stop TX on all of a port's lanes according to a LLFC signal from CFC
 *
 * \param [in] unit - chip unit id
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
int imb_ethu_port_stop_pm_from_cfc_llfc_enable_set(
    int unit,
    bcm_port_t port,
    uint32 enable);

/**
 * \brief - Set enable / disable PM to stop TX on a port according to received PFC signal.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] pfc - PFC for which to enable to stop TX on PM
 * \param [in] enable - TRUE or FALSE
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_port_stop_pm_from_pfc_enable_set(
    int unit,
    bcm_port_t port,
    int pfc,
    uint32 enable);

/**
 * \brief - Get enable / disable PM to stop TX on a port according to received PFC signal.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] pfc - PFC for which to get enable status
 * \param [out] enable - enable / disable indication
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_port_stop_pm_from_pfc_enable_get(
    int unit,
    bcm_port_t port,
    int pfc,
    uint32 *enable);

/**
 * \brief - Set enable/disable LLFC RMC threshold to generate PFC on the specified port.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable - TRUE or FALSE
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_port_gen_pfc_from_llfc_thresh_enable_set(
    int unit,
    bcm_port_t port,
    uint32 enable);

/**
 * \brief - Get enable / disable indication for LLFC RMC threshold to generate PFC on the specified port.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] enable - enable / disable indication
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_port_gen_pfc_from_llfc_thresh_enable_get(
    int unit,
    bcm_port_t port,
    uint32 *enable);

/**
 * \brief - Set a port's Rx Quad MAC Lane FIFO (QMLF) Flow Control generation thresholds.
*
* \param [in] unit - chip unit id
* \param [in] port - logical port
* \param [in] flags - flags
* \param [in] threshold - threshold value
*
* \return
*   int - see _SHR_E_*
*
* \remarks
*   * None
* \see
*   * None
*/
int imb_ethu_port_fc_rx_qmlf_threshold_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 threshold);

/**
 * \brief - Get a port's Rx Quad MAC Lane FIFO (QMLF) Flow Control generation thresholds.
*
* \param [in] unit - chip unit id
* \param [in] port - logical port
* \param [in] flags - flags
* \param [out] threshold - threshold value
*
* \return
*   int - see _SHR_E_*
*
* \remarks
*   * None
* \see
*   * None
*/
int imb_ethu_port_fc_rx_qmlf_threshold_get(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 *threshold);

/**
 * \brief - Set the Flow Control Interface in/out of reset for a specified ETHU.
 *
 * \param [in] unit - chip unit id
 * \param [in] imb_info - IMB info
 * \param [in] in_reset - reset indication
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_fc_reset_set(
    int unit,
    const imb_create_info_t * imb_info,
    uint32 in_reset);

/**
 * \brief - Enable / disable the generation of a LLFC based on a signal from Global Resources thresholds for a specified ETHU
 *
 * \param [in] unit - chip unit id
 * \param [in] imb_info - IMB info
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
int imb_ethu_llfc_from_glb_rsc_enable_set(
    int unit,
    const imb_create_info_t * imb_info,
    uint32 enable);

/**
 * \brief - Per ETHU map the received PFC - each priority received by NIF should be mapped to a priority that will be sent to EGQ in order to stop a Q-pair
 *
 * \param [in] unit - chip unit id
 * \param [in] imb_id - IMB ID
 * \param [in] nif_priority - NIF priority to be mapped
 * \param [in] egq_priority - EGQ priority to be mapped
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_pfc_rec_priority_map(
    int unit,
    int imb_id,
    uint32 nif_priority,
    uint32 egq_priority);

/**
 * \brief - Per ETHU unmap all priorities of the received PFC
 *
 * \param [in] unit - chip unit id
 * \param [in] imb_id - IMB ID
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_pfc_rec_priority_unmap(
    int unit,
    int imb_id);

/**
 * \brief - Set local fault enable value per port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable - enable value (1 for enabled)
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_port_local_fault_enable_set(
    int unit,
    bcm_port_t port,
    int enable);

/**
 * \brief - Get local fault enable value per port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] enable - enable value (1 for enabled)
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_port_local_fault_enable_get(
    int unit,
    bcm_port_t port,
    int *enable);

/**
 * \brief - Set remote fault enable value per port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable - enable value (1 for enabled)
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_port_remote_fault_enable_set(
    int unit,
    bcm_port_t port,
    int enable);

/**
 * \brief - Get remote fault enable value per port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] enable - enable value (1 for enabled)
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_port_remote_fault_enable_get(
    int unit,
    bcm_port_t port,
    int *enable);

/**
 * \brief - Get port link state
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] clear_status - indication to clear latch down
 *        status
 * \param [out] link_state - link state info
 *
 * \return
 *   int - see_SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_port_link_state_get(
    int unit,
    bcm_port_t port,
    int clear_status,
    bcm_port_link_state_t * link_state);

/**
 * \brief - get master ports of the PM in ETHU
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 *
 * \param [out] ethu_ports - ports of the ETHU
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_pm_master_ports_get(
    int unit,
    bcm_port_t port,
    bcm_pbmp_t * ethu_ports);

/**
 * \brief - get number of ports on a port's ETHU
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] ethu_nof_ports - number of ports on the ETHU
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_pm_nof_master_ports_get(
    int unit,
    bcm_port_t port,
    int *ethu_nof_ports);

/**
 * \brief - map SIF instance (port) into NIF port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port. if port==BCM_PORT_INVALID, disconnect the sif instance from the NIF
 * \param [in] instace_id - statistics interface instance (port) id
 * \param [in] instace_core - the core that the sif instance belong to.
 * \param [in] connect - TRUE to connect stif instance to port, FALSE to disconnect
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_port_to_stif_instance_map_set(
    int unit,
    bcm_port_t port,
    int instace_id,
    int instace_core,
    int connect);

/**
 * \brief - Configure ETHU scheduler
 *
 * \param [in] unit   - chip unit id
 * \param [in] port   - logical port
 * \param [in] enable - 0:disable port, 1:enable port
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *  * None
 */
int imb_ethu_port_scheduler_config_set(
    int unit,
    bcm_port_t port,
    int enable);

/**
 * \brief - helper function to unmap the RMCs
 *        from the physical lanes.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_port_remove_rmc_lane_unmap(
    int unit,
    bcm_port_t port);

/**
 * \brief - Set the given priority groups configuration
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] priority_config - priority groups configuration
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_port_priority_config_set(
    int unit,
    bcm_port_t port,
    const bcm_port_prio_config_t * priority_config);

/**
 * \brief - Get the given priority groups configuration
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] priority_config - priority groups configuration
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_port_priority_config_get(
    int unit,
    bcm_port_t port,
    bcm_port_prio_config_t * priority_config);

/**
 * \brief - Get ETHU current bandwidth
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] is_port_include - indication whether to include /
 *        exclude specified port in the BW calculation
 * \param [out] bandwidth - BW calculated
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_bandwidth_get(
    int unit,
    bcm_port_t port,
    int is_port_include,
    int *bandwidth);

/**
 * \brief - Get prd drop counter according to priority
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] sch_priority - scheduler priority. the PRD
 *        counter is per RMC, so the user of this function gives
 *        the scheduler priority in order to find which RMC to
 *        read the counter from
 * \param [out] count - counter value
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_drop_count_get(
    int unit,
    bcm_port_t port,
    bcm_port_nif_scheduler_t sch_priority,
    uint64 *count);

/**
 * \brief - Get NIF RX fifo status
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] sch_priority - nif schedule priority
 * \param [out] max_occupancy - indicate the MAX fullness level of the fifo
 * \param [out] fifo_level - indicate the current fullness level of the fifo
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_port_rx_fifo_status_get(
    int unit,
    bcm_port_t port,
    bcm_port_nif_scheduler_t sch_priority,
    uint32 *max_occupancy,
    uint32 *fifo_level);

/**
 * \brief - Get NIF TX fifo status
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] max_occupancy - indicate the MAX fullness level of the fifo
 * \param [out] fifo_level - indicate the current fullness level of the fifo
 * \param [out] pm_credits - indicate the credits from PM tx buffer
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_port_tx_fifo_status_get(
    int unit,
    bcm_port_t port,
    uint32 *max_occupancy,
    uint32 *fifo_level,
    uint32 *pm_credits);

/*Configure PRD priority maps.*/
int imb_ethu_prd_map_set(
    int unit,
    bcm_port_t port,
    bcm_cosq_ingress_port_drop_map_t map,
    uint32 key,
    uint32 priority,
    uint32 is_tdm);
int imb_ethu_prd_map_get(
    int unit,
    bcm_port_t port,
    bcm_cosq_ingress_port_drop_map_t map,
    uint32 key,
    uint32 *priority,
    uint32 *is_tdm);

/*Set/Get PRD Configurable Ether types.*/
int imb_ethu_prd_custom_ether_type_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 ether_type_val);
int imb_ethu_prd_custom_ether_type_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 *ether_type_val);

/*Set/Get PRD TCAM entries (for the soft stage).*/
int imb_ethu_prd_flex_key_entry_set(
    int unit,
    bcm_port_t port,
    uint32 key_index,
    const dnx_cosq_prd_tcam_entry_info_t * entry_info);
int imb_ethu_prd_flex_key_entry_get(
    int unit,
    bcm_port_t port,
    uint32 key_index,
    dnx_cosq_prd_tcam_entry_info_t * entry_info);

/*Set/Get PRD Control Frame properties. if a packet is identified as control frame, it will automatically get the highest priority.*/
int imb_ethu_prd_control_frame_set(
    int unit,
    bcm_port_t port,
    uint32 control_frame_index,
    const bcm_cosq_ingress_drop_control_frame_config_t * control_frame_config);
int imb_ethu_prd_control_frame_get(
    int unit,
    bcm_port_t port,
    uint32 control_frame_index,
    bcm_cosq_ingress_drop_control_frame_config_t * control_frame_config);

/*Set/Get PRD MPLS special label properties. if a packet is identified as MPLS special label and the label value matches to one of the saved special labels, the priority and TDM indication will be taken from the label properties.*/
int imb_ethu_prd_mpls_special_label_set(
    int unit,
    bcm_port_t port,
    uint32 label_index,
    const bcm_cosq_ingress_port_drop_mpls_special_label_config_t * label_config);
int imb_ethu_prd_mpls_special_label_get(
    int unit,
    bcm_port_t port,
    uint32 label_index,
    bcm_cosq_ingress_port_drop_mpls_special_label_config_t * label_config);
int imb_ethu_prd_hard_stage_enable_set(
    int unit,
    bcm_port_t port,
    uint32 enable);
int imb_ethu_prd_hard_stage_enable_get(
    int unit,
    bcm_port_t port,
    uint32 *enable);

/**
 * \brief - set TPID value for the port profile
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] tpid_index - tpid index (0-3). per port there can
 *        be 4 identified TPIDs
 * \param [in] tpid_value - TPID value
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_tpid_set(
    int unit,
    bcm_port_t port,
    uint32 tpid_index,
    uint32 tpid_value);
/**
 * \brief - get TPID value for the port profile
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] tpid_index - tpid index (0-3). per port there can
 *        be 4 identified TPIDs
 * \param [in] tpid_value - TPID value
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_tpid_get(
    int unit,
    bcm_port_t port,
    uint32 tpid_index,
    uint32 *tpid_value);
/**
 * \brief - set different PRD properties
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] property - PRD property to set:
 *  see enum imb_prd_properties_t.
 * \param [in] val - value of the property to set.
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_properties_set(
    int unit,
    bcm_port_t port,
    imb_prd_properties_t property,
    uint32 val);
/**
 * \brief - get PRD property value
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] property - PRD property to get.
 * see enum imb_prd_properties_t.
 * \param [out] val - returned property value
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_properties_get(
    int unit,
    bcm_port_t port,
    imb_prd_properties_t property,
    uint32 *val);
/**
 * \brief - enable/disable PRD for the port. this call should be
 *        last, after the PRD is already configured.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable_mode - enable modes for the PRD:
 * bcmCosqIngressPortDropDisable
 * bcmCosqIngressPortDropEnableHardStage
 * bcmCosqIngressPortDropEnableHardAndSoftStage
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * it is required to call header type set before enabling
 *     the PRD.
 * \see
 *   * None
 */
int imb_ethu_prd_enable_set(
    int unit,
    bcm_port_t port,
    int enable_mode);
/**
 * \brief - get PRD enable mode per port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable_mode - PRD enable modes:
 *  bcmCosqIngressPortDropDisable
 *  bcmCosqIngressPortDropEnableHardStage
 *  bcmCosqIngressPortDropEnableHardAndSoftStage
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_prd_enable_get(
    int unit,
    bcm_port_t port,
    int *enable_mode);

int imb_ethu_prd_ether_type_size_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 ether_type_size);
int imb_ethu_prd_ether_type_size_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 *ether_type_size);
int imb_ethu_prd_tcam_entry_key_offset_base_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 offset_base);
int imb_ethu_prd_tcam_entry_key_offset_base_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 *offset_base);
/*Set/Get PRD TCAM key offset value per Ether type (for the soft stage).*/
int imb_ethu_prd_tcam_entry_key_offset_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 offset_index,
    uint32 offset_value);
int imb_ethu_prd_tcam_entry_key_offset_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 offset_index,
    uint32 *offset_value);
/**
 * \brief
 *   Validate if the FEC + Speed combination is supported.
 *   This function only validates part of FEC + Speed combinations.
 *   portmod will validate the rest FEC + Speed combinations.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] fec_type - fec type
 * \param [in] speed - speed
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 */
int imb_ethu_port_fec_speed_validate(
    int unit,
    bcm_port_t port,
    bcm_port_phy_fec_t fec_type,
    int speed);

/**
 * \brief - Reset per ETHU instrumentation counters
 *
 * \param [in] unit - chip unit id
 * \param [in] imb_info - IMB info
 * \return
 *   int - see _SHR_E_ *
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_instru_counters_reset(
    int unit,
    const imb_create_info_t * imb_info);

/**
 * \brief - Reset per IMB instrumentation counters for PFC deadlock detection mechanism
 *
 * \param [in] unit - chip unit id
 * \param [in] imb_info - IMB info
 * \return
 *   int - see _SHR_E_ *
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_pfc_deadlock_counters_reset(
    int unit,
    const imb_create_info_t * imb_info);

/**
 * \brief - Reset per port instrumentation counters for PFC deadlock detection mechanism
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \return
 *   int - see _SHR_E_ *
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_port_pfc_deadlock_counters_reset(
    int unit,
    bcm_port_t port);

/**
 * \brief - Get the debug info for PFC deadlock detection mechanism
 *
 * \param [in] unit - chip unit id
 * \param [in] ethu_id - ETHU ID
 * \param [out] ethu_port_num - port number internal for the ETHU ID
 *              of the port that triggered the PFC deadlock detection
 * \param [out] pfc_num - PFC that triggered the PFC deadlock detection
 * \return
 *   int - see _SHR_E_ *
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_pfc_deadlock_debug_info_get(
    int unit,
    int ethu_id,
    int *ethu_port_num,
    int *pfc_num);

/**
 * \brief - Set the max PFC duration for PFC deadlock detection mechanism
 *
 * \param [in] unit - chip unit id
 * \param [in] ethu_id - ETHU ID
 * \param [in] max_duration - max duration of the PFC
 *             that will trigger the PFC deadlock recovery
 * \return
 *   int - see _SHR_E_ *
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_pfc_deadlock_max_duration_set(
    int unit,
    int ethu_id,
    uint32 max_duration);

/**
 * \brief - Get the max PFC duration for PFC deadlock detection mechanism
 *
 * \param [in] unit - chip unit id
 * \param [in] ethu_id - ETHU ID
 * \param [out] max_duration - max duration of the PFC
 *             that will trigger the PFC deadlock recovery
 * \return
 *   int - see _SHR_E_ *
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_pfc_deadlock_max_duration_get(
    int unit,
    int ethu_id,
    uint32 *max_duration);

/**
 * \brief - Set enable/disable indication of the interrupt for PFC Deadlock recovery mechanism
 *
 * \param [in] unit - chip unit id
 * \param [in] ethu_id - ETHU ID
 * \param [in] enable - enable indication
 * \return
 *   int - see _SHR_E_ *
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_pfc_deadlock_int_enable_set(
    int unit,
    int ethu_id,
    uint32 enable);

/**
 * \brief - Get enable/disable indication of the interrupt for PFC Deadlock recovery mechanism
 *
 * \param [in] unit - chip unit id
 * \param [in] ethu_id - ETHU ID
 * \param [out] enable - enable indication
 * \return
 *   int - see _SHR_E_ *
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_pfc_deadlock_int_enable_get(
    int unit,
    int ethu_id,
    uint32 *enable);

/**
 * \brief - Configure L1 mismatch rate in RMC
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] tx_speed - The speed for target TMC
 * \param [in] enable - Enable or disable the configuration
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_port_l1_mismatch_rate_rx_config_set(
    int unit,
    bcm_port_t port,
    int tx_speed,
    int enable);

/**
 * \brief - power down / up port's ETHU
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] power_down - power_down indication
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_port_power_down_set(
    int unit,
    bcm_port_t port,
    int power_down);

/**
 * \brief - get all master logical ports on a ETHU
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] ethu_ports - bitmap of master logical ports on the ETHU
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ethu_master_ports_get(
    int unit,
    bcm_port_t port,
    bcm_pbmp_t * ethu_ports);

#endif /* IMB_ETHU_H_INCLUDED */
