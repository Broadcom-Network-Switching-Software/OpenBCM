/**
 *
 * \file dnx_port_nif_ofr_prd.h
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * NIF OFR procedures for DNX.
 *
 */

#ifndef _NIF_OFR_PRD_H_
#define _NIF_OFR_PRD_H_

#include <bcm/types.h>

int dnx_ofr_prd_tpid_set(
    int unit,
    bcm_port_t port,
    uint32 tpid_index,
    uint32 tpid_value);

int dnx_ofr_prd_tpid_get(
    int unit,
    bcm_port_t port,
    uint32 tpid_index,
    uint32 *tpid_value);

int dnx_ofr_prd_map_set(
    int unit,
    bcm_port_t port,
    bcm_cosq_ingress_port_drop_map_t map,
    uint32 key,
    uint32 priority,
    uint32 is_tdm);

int dnx_ofr_prd_map_get(
    int unit,
    bcm_port_t port,
    bcm_cosq_ingress_port_drop_map_t map,
    uint32 key,
    uint32 *priority,
    uint32 *is_tdm);

int dnx_ofr_prd_properties_set(
    int unit,
    bcm_port_t port,
    imb_prd_properties_t property,
    uint32 val);

int dnx_ofr_prd_properties_get(
    int unit,
    bcm_port_t port,
    imb_prd_properties_t property,
    uint32 *val);

int dnx_ofr_prd_custom_ether_type_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 ether_type_val);

int dnx_ofr_prd_custom_ether_type_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 *ether_type_val);

int dnx_ofr_prd_mpls_special_label_set(
    int unit,
    bcm_port_t port,
    uint32 label_index,
    const bcm_cosq_ingress_port_drop_mpls_special_label_config_t * label_config);

int dnx_ofr_prd_mpls_special_label_get(
    int unit,
    bcm_port_t port,
    uint32 label_index,
    bcm_cosq_ingress_port_drop_mpls_special_label_config_t * label_config);

int dnx_ofr_prd_threshold_set(
    int unit,
    bcm_port_t port,
    uint32 priority,
    uint32 threshold);

int dnx_ofr_prd_threshold_get(
    int unit,
    bcm_port_t port,
    uint32 priority,
    uint32 *threshold);

int dnx_ofr_prd_threshold_max_get(
    int unit,
    bcm_port_t port,
    uint32 *threshold);

int dnx_ofr_prd_flex_key_entry_set(
    int unit,
    bcm_port_t port,
    uint32 key_index,
    const dnx_cosq_prd_tcam_entry_info_t * entry_info);

int dnx_ofr_prd_flex_key_entry_get(
    int unit,
    bcm_port_t port,
    uint32 key_index,
    dnx_cosq_prd_tcam_entry_info_t * entry_info);

int dnx_ofr_prd_tcam_entry_key_offset_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 offset_index,
    uint32 offset_value);

int dnx_ofr_prd_tcam_entry_key_offset_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 offset_index,
    uint32 *offset_value);

int dnx_ofr_prd_tcam_entry_key_offset_base_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 offset_base);

int dnx_ofr_prd_tcam_entry_key_offset_base_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 *offset_base);

int dnx_ofr_prd_ether_type_size_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 ether_type_size);

int dnx_ofr_prd_ether_type_size_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 *ether_type_size);

int dnx_ofr_prd_control_frame_set(
    int unit,
    bcm_port_t port,
    uint32 control_frame_index,
    const bcm_cosq_ingress_drop_control_frame_config_t * control_frame_config);

int dnx_ofr_prd_control_frame_get(
    int unit,
    bcm_port_t port,
    uint32 control_frame_index,
    bcm_cosq_ingress_drop_control_frame_config_t * control_frame_config);

int dnx_ofr_prd_port_profile_map_set(
    int unit,
    bcm_port_t port,
    uint32 prd_profile);

int dnx_ofr_prd_port_profile_map_get(
    int unit,
    bcm_port_t port,
    uint32 *prd_profile);

int dnx_ofr_prd_hard_stage_enable_set(
    int unit,
    bcm_port_t port,
    uint32 enable);

int dnx_ofr_prd_hard_stage_enable_get(
    int unit,
    bcm_port_t port,
    uint32 *enable);

int dnx_ofr_prd_enable_set(
    int unit,
    bcm_port_t port,
    int enable_mode);

int dnx_ofr_prd_enable_get(
    int unit,
    bcm_port_t port,
    int *enable_mode);

int dnx_ofr_prd_global_drop_count_get(
    int unit,
    uint32 priority,
    uint32 priority_group,
    uint64 *drop_count);

int dnx_ofr_prd_port_counter_map_set(
    int unit,
    uint32 counter,
    uint32 port,
    uint32 priority_group);

int dnx_ofr_prd_rmc_drop_count_get(
    int unit,
    uint32 counter,
    uint64 *drop_count);

#endif /* _NIF_OFR_PRD_H_ */
