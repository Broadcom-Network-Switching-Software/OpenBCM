/** \file dnx_port_nif_ofr_internal.h
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef _PORT_NIF_OFR_INTERNAL_H_
#define _PORT_NIF_OFR_INTERNAL_H_

#include <soc/sand/shrextend/shrextend_error.h>
#include <bcm/types.h>
#include <bcm/port.h>
#include <bcm/cosq.h>
#include <bcm_int/dnx/cosq/ingress/cosq_ingress_port.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_enum_field_types.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>

#define DNX_PORT_NIF_OFR_PRIORITY_IS_LOW(priority_group) \
        (priority_group.sch_priority == bcmPortNifSchedulerLow)

#define DNX_PORT_NIF_OFR_PRIORITY_IS_HIGH(priority_group) \
        (priority_group.sch_priority == bcmPortNifSchedulerHigh)

#define DNX_PORT_NIF_OFR_PRIORITY_IS_LOW_OR_HIGH(priority_group) \
        ((DNX_PORT_NIF_OFR_PRIORITY_IS_LOW(priority_group)) || (DNX_PORT_NIF_OFR_PRIORITY_IS_HIGH(priority_group)))

#define DNX_PORT_NIF_OFR_PRIORITY_IS_TDM(priority_group) \
        (priority_group.sch_priority == bcmPortNifSchedulerTDM)

shr_error_e dnx_port_ofr_info_init(
    int unit);

shr_error_e dnx_port_ofr_high_priority_to_ire_channel_set(
    int unit);

shr_error_e dnx_port_ofr_power_down_set(
    int unit,
    int enable);

shr_error_e dnx_port_ofr_sar_scheduler_request_enable_set(
    int unit,
    int enable);

shr_error_e dnx_port_ofr_sar_scheduler_transaction_cnt_reset_set(
    int unit,
    int in_reset);

shr_error_e dnx_port_ofr_async_unit_enable(
    int unit,
    int enable);

shr_error_e dnx_port_ofr_prd_bypass_hw_set(
    int unit,
    int bypass_enable);

shr_error_e dnx_port_ofr_port_params_set(
    int unit,
    bcm_port_t port);

shr_error_e dnx_port_ofr_rmc_machine_id_internal_get(
    int unit,
    bcm_port_t port,
    bcm_port_nif_scheduler_t sch_prio,
    int *priority_id);

shr_error_e dnx_port_ofr_rmc_id_get(
    int unit,
    bcm_port_t port,
    const bcm_port_prio_config_t * priority_config,
    int priority_group,
    int *rmc_id);

shr_error_e dnx_port_ofr_rmc_config_set(
    int unit,
    bcm_port_t port,
    const bcm_port_prio_config_t * priority_config);

shr_error_e dnx_port_ofr_rmc_thresholds_set(
    int unit,
    bcm_port_t port,
    const bcm_port_prio_config_t * priority_config);

shr_error_e dnx_port_ofr_port_priority_config_internal_get(
    int unit,
    bcm_port_t port,
    bcm_port_prio_config_t * priority_config);

shr_error_e dnx_port_ofr_transaction_counter_reset(
    int unit,
    bcm_port_t port,
    int in_reset);

shr_error_e dnx_port_ofr_rx_sch_context_reset(
    int unit,
    bcm_port_t port,
    const bcm_port_prio_config_t * priority_config,
    int in_reset);

shr_error_e dnx_port_ofr_gear_shift_reset(
    int unit,
    bcm_port_t port,
    int in_reset);

shr_error_e dnx_port_ofr_unpack_logic_reset(
    int unit,
    bcm_port_t port,
    int in_reset);

shr_error_e dnx_port_ofr_sar_datapath_enable(
    int unit,
    int enable);

shr_error_e dnx_port_ofr_rx_fifo_reset(
    int unit,
    bcm_port_t port,
    const bcm_port_prio_config_t * priority_config,
    int in_reset);

shr_error_e dnx_port_ofr_quantum_counter_reset(
    int unit,
    bcm_port_t port,
    const bcm_port_prio_config_t * priority_config,
    int in_reset);

shr_error_e dnx_port_ofr_quantum_load_enable(
    int unit,
    bcm_port_t port,
    const bcm_port_prio_config_t * priority_config,
    int enable);

shr_error_e dnx_port_ofr_deficit_rr_config_set(
    int unit,
    bcm_port_t port,
    const bcm_port_prio_config_t * priority_config,
    int enable);

shr_error_e dnx_port_ofr_rx_sch_context_quantum_value_set(
    int unit,
    bcm_port_t port,
    int value);

shr_error_e dnx_port_ofr_rx_sch_config_set(
    int unit,
    bcm_port_t port,
    const bcm_port_prio_config_t * priority_config,
    int enable);

int dnx_ofr_prd_tpid_hw_set(
    int unit,
    uint32 prd_profile,
    uint32 tpid_index,
    uint32 tpid_value);

int dnx_ofr_prd_tpid_hw_get(
    int unit,
    uint32 prd_profile,
    uint32 tpid_index,
    uint32 *tpid_value);

int dnx_ofr_prd_map_tm_tc_dp_hw_set(
    int unit,
    uint32 tc,
    uint32 dp,
    uint32 priority,
    uint32 is_tdm);

int dnx_ofr_prd_map_tm_tc_dp_hw_get(
    int unit,
    uint32 tc,
    uint32 dp,
    uint32 *priority,
    uint32 *is_tdm);

int dnx_ofr_prd_map_ip_dscp_hw_set(
    int unit,
    uint32 dscp,
    uint32 priority,
    uint32 is_tdm);

int dnx_ofr_prd_map_ip_dscp_hw_get(
    int unit,
    uint32 dscp,
    uint32 *priority,
    uint32 *is_tdm);

int dnx_ofr_prd_map_mpls_exp_hw_set(
    int unit,
    uint32 exp,
    uint32 priority,
    uint32 is_tdm);

int dnx_ofr_prd_map_mpls_exp_hw_get(
    int unit,
    uint32 exp,
    uint32 *priority,
    uint32 *is_tdm);

int dnx_ofr_prd_map_pcp_dei_hw_set(
    int unit,
    uint32 pcp,
    uint32 dei,
    uint32 priority,
    uint32 is_tdm);

int dnx_ofr_prd_map_pcp_dei_hw_get(
    int unit,
    uint32 pcp,
    uint32 dei,
    uint32 *priority,
    uint32 *is_tdm);

int dnx_ofr_prd_ignore_ip_dscp_hw_set(
    int unit,
    int prd_profile,
    uint32 ignore_ip_dscp);

int dnx_ofr_prd_ignore_ip_dscp_hw_get(
    int unit,
    int prd_profile,
    uint32 *ignore_ip_dscp);

int dnx_ofr_prd_ignore_mpls_exp_hw_set(
    int unit,
    int prd_profile,
    uint32 ignore_mpls_exp);

int dnx_ofr_prd_ignore_mpls_exp_hw_get(
    int unit,
    int prd_profile,
    uint32 *ignore_mpls_exp);

int dnx_ofr_prd_ignore_inner_tag_hw_set(
    int unit,
    int prd_profile,
    uint32 ignore_inner_tag);

int dnx_ofr_prd_ignore_inner_tag_hw_get(
    int unit,
    int prd_profile,
    uint32 *ignore_inner_tag);

int dnx_ofr_prd_ignore_outer_tag_hw_set(
    int unit,
    int prd_profile,
    uint32 ignore_outer_tag);

int dnx_ofr_prd_ignore_outer_tag_hw_get(
    int unit,
    int prd_profile,
    uint32 *ignore_outer_tag);

int dnx_ofr_prd_default_priority_hw_set(
    int unit,
    int prd_profile,
    uint32 default_priority);

int dnx_ofr_prd_default_priority_hw_get(
    int unit,
    int prd_profile,
    uint32 *default_priority);

int dnx_ofr_prd_custom_ether_type_hw_set(
    int unit,
    uint32 ether_type_code,
    uint32 ether_type_val);

int dnx_ofr_prd_custom_ether_type_hw_get(
    int unit,
    uint32 ether_type_code,
    uint32 *ether_type_val);

int dnx_ofr_prd_mpls_special_label_hw_set(
    int unit,
    uint32 special_label_index,
    const bcm_cosq_ingress_port_drop_mpls_special_label_config_t * label_config);

int dnx_ofr_prd_mpls_special_label_hw_get(
    int unit,
    uint32 special_label_index,
    bcm_cosq_ingress_port_drop_mpls_special_label_config_t * label_config);

int dnx_ofr_prd_threshold_hw_set(
    int unit,
    uint32 rmc,
    uint32 priority,
    uint32 threshold);

int dnx_ofr_prd_threshold_hw_get(
    int unit,
    uint32 rmc,
    uint32 priority,
    uint32 *threshold);

int dnx_ofr_prd_tcam_entry_hw_set(
    int unit,
    uint32 entry_index,
    const dnx_cosq_prd_tcam_entry_info_t * entry_info);

int dnx_ofr_prd_tcam_entry_hw_get(
    int unit,
    uint32 entry_index,
    dnx_cosq_prd_tcam_entry_info_t * entry_info);

int dnx_ofr_prd_tcam_entry_key_offset_hw_set(
    int unit,
    uint32 ether_type_code,
    uint32 offset_index,
    uint32 offset_value);

int dnx_ofr_prd_tcam_entry_key_offset_hw_get(
    int unit,
    uint32 ether_type_code,
    uint32 offset_index,
    uint32 *offset_value);

int dnx_ofr_prd_tcam_entry_key_offset_base_hw_set(
    int unit,
    uint32 ether_type_code,
    uint32 offset_base);

int dnx_ofr_prd_tcam_entry_key_offset_base_hw_get(
    int unit,
    uint32 ether_type_code,
    uint32 *offset_base);

int dnx_ofr_prd_ether_type_size_hw_set(
    int unit,
    uint32 ether_type_code,
    uint32 ether_type_size);

int dnx_ofr_prd_ether_type_size_hw_get(
    int unit,
    uint32 ether_type_code,
    uint32 *ether_type_size);

int dnx_ofr_prd_control_frame_hw_set(
    int unit,
    uint32 control_frame_index,
    const bcm_cosq_ingress_drop_control_frame_config_t * control_frame_config);

int dnx_ofr_prd_control_frame_hw_get(
    int unit,
    uint32 control_frame_index,
    bcm_cosq_ingress_drop_control_frame_config_t * control_frame_config);

int dnx_ofr_rx_prd_port_profile_map_hw_set(
    int unit,
    int nif_interface_id,
    int prd_profile);

int dnx_ofr_prd_hard_stage_enable_hw_set(
    int unit,
    uint32 rmc,
    uint32 enable);

int dnx_ofr_prd_hard_stage_enable_hw_get(
    int unit,
    uint32 rmc,
    uint32 *enable);

int dnx_ofr_prd_soft_stage_enable_hw_set(
    int unit,
    int prd_profile,
    uint32 enable_eth,
    uint32 enable_tm);

int dnx_ofr_prd_soft_stage_enable_hw_get(
    int unit,
    int prd_profile,
    uint32 *enable_eth,
    uint32 *enable_tm);

shr_error_e dnx_port_ofr_enable_dynamic_memories_set(
    int unit,
    int enable);

int dnx_ofr_prd_itmh_offsets_hw_set(
    int unit,
    uint32 tc_offset,
    uint32 dp_offset);

int dnx_ofr_prd_ftmh_offsets_hw_set(
    int unit,
    uint32 tc_offset,
    uint32 dp_offset);

int dnx_ofr_prd_outer_tag_size_hw_set(
    int unit,
    int prd_profile,
    uint32 outer_tag_size);

int dnx_ofr_prd_port_type_hw_set(
    int unit,
    int prd_profile,
    uint32 prd_port_type);

int dnx_ofr_prd_is_single_port_on_profile_get(
    int unit,
    bcm_port_t port,
    uint8 *is_single_port_on_profile);

int dnx_ofr_profile_rx_priority_to_rmc_map_get(
    int unit,
    int prd_profile,
    uint32 *rmc_group);

int dnx_ofr_internal_port_rx_priority_to_rmc_map(
    int unit,
    bcm_port_t port,
    dnx_algo_port_rmc_info_t * rmc);

/**
 * \brief - Configure the data queue of ILU port to be high / low priority
 *
 * \param [in] unit - chip unit id.
 * \param [in] ilkn_id - ilkn_interface_id
 * \param [in] dbal_sch_prio - priority (low/high/tdm)
 * \param [in] is_enable - TRUE = enable, FALSE = disable
 *
 * \return
 *   error
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_ofr_internal_ilkn_rx_sched_request_enable_set(
    int unit,
    uint32 ilkn_id,
    dbal_enum_value_field_nif_sch_priority_e dbal_sch_prio,
    uint32 is_enable);

/**
 * \brief - Configure the data queue of ILU port to be high / low priority
 *
 * \param [in] unit - chip unit id.
 * \param [in] ilkn_id - ilkn_interface_id
 * \param [in] dbal_sch_prio - priority (low/high/tdm)
 * \param [out] is_enabled - TRUE = enabled, FALSE = disabled
 *
 * \return
 *   error
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_ofr_internal_ilkn_rx_sched_request_enable_get(
    int unit,
    uint32 ilkn_id,
    dbal_enum_value_field_nif_sch_priority_e dbal_sch_prio,
    uint32 *is_enabled);

/**
 * \brief - Configure the data queue of ILU port to be high / low priority
 *
 * \param [in] unit - chip unit id.
 * \param [in] ilkn_id - ilkn_interface_id
 * \param [in] is_priority_high - value to set
 *
 * \return
 *   error
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_ofr_internal_ilkn_rx_is_high_priority_config_set(
    int unit,
    uint32 ilkn_id,
    uint32 is_priority_high);

/**
 * \brief - Configure the data queue of ILU port to be high / low priority
 *
 * \param [in] unit - chip unit id.
 * \param [in] ilkn_id - ilkn_interface_id
 * \param [in] is_tdm - set tdm counter or data counter?
 * \param [in] in_reset - put in_reset or out of reset?
 *
 * \return
 *   error
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_ofr_internal_ilkn_rx_counter_reset_set(
    int unit,
    uint32 ilkn_id,
    int is_tdm,
    uint32 in_reset);

/**
 * \brief - retrieves the drop count of a global counter based on a priority and priority group
 *
 * \param [in] unit - chip unit id
 * \param [in] priority - global port drop counter priority
 * \param [in] priority_group - global port drop counter priority group
 * \param [out] drop_count - drop count of counter
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int dnx_ofr_prd_global_drop_count_hw_get(
    int unit,
    uint32 priority,
    uint32 priority_group,
    uint64 *drop_count);

/**
 * \brief - maps a counter to a port's RMC(priority group)
 *
 * \param [in] unit - chip unit id
 * \param [in] counter - counter to map port to
 * \param [in] nif_port - nif port id
 * \param [in] priority_group - priority group to count
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int dnx_ofr_prd_port_counter_hw_set(
    int unit,
    uint32 counter,
    uint32 nif_port,
    uint32 priority_group);

/**
 * \brief - retrieves a specific priority drop count that is mapped to a specific counter
 *
 * \param [in] unit - chip unit id
 * \param [in] counter - the counter id to retrieve drop count from
 * \param [out] prio_0_drop_count - drop count of a priority 0
 * \param [out] prio_1_drop_count - drop count of a priority 1
 * \param [out] prio_2_drop_count - drop count of a priority 2
 * \param [out] prio_3_drop_count - drop count of a priority 3
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int dnx_ofr_prd_counter_drop_count_hw_get(
    int unit,
    uint32 counter,
    uint64 *prio_0_drop_count,
    uint64 *prio_1_drop_count,
    uint64 *prio_2_drop_count,
    uint64 *prio_3_drop_count);

/**
 * \brief - Calculate how many free section OFR link list has per memory group
 *
 * \param [in] unit - chip unit id.
 * \param [in] logical_port - logical port
 * \param [out] total_free_sections - return value array
 *
 * \return
 *   error
*/
shr_error_e dnx_port_ofr_double_link_list_total_free_sections_get(
    int unit,
    bcm_port_t logical_port,
    int *total_free_sections);

shr_error_e dnx_port_ofr_rx_fifo_status_counter_hw_map(
    int unit,
    uint32 counter,
    uint32 nif_port);
shr_error_e dnx_port_ofr_rx_fifo_status_hw_get(
    int unit,
    uint32 counter,
    uint32 priority_group,
    uint32 *max_occupancy,
    uint32 *fifo_level);

shr_error_e dnx_port_ofr_tx_fifo_status_counter_hw_map(
    int unit,
    uint32 counter,
    uint32 nif_port);

shr_error_e dnx_port_ofr_tx_fifo_status_hw_get(
    int unit,
    uint32 counter,
    uint32 *max_occupancy,
    uint32 *fifo_level);

#endif /* _PORT_NIF_OFR_INTERNAL_H_ */
