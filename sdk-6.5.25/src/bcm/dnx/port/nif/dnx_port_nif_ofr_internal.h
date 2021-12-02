/** \file dnx_port_nif_ofr_internal.h
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifndef _PORT_NIF_OFR_INTERNAL_H_
#define _PORT_NIF_OFR_INTERNAL_H_

#include <shared/shrextend/shrextend_error.h>
#include <bcm/types.h>
#include <bcm/port.h>
#include <bcm/cosq.h>
#include <bcm_int/dnx/cosq/ingress/cosq_ingress_port.h>
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
    bcm_port_t port);

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

#endif /* _PORT_NIF_OFR_INTERNAL_H_ */
