

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_QOS_H_

#define _DNX_DATA_INTERNAL_QOS_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_qos.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_qos_submodule_qos,
    dnx_data_qos_submodule_feature,

    
    _dnx_data_qos_submodule_nof
} dnx_data_qos_submodule_e;








int dnx_data_qos_qos_feature_get(
    int unit,
    dnx_data_qos_qos_feature_e feature);



typedef enum
{
    dnx_data_qos_qos_define_nof_ingress_profiles,
    dnx_data_qos_qos_define_nof_ingress_opcode_profiles,
    dnx_data_qos_qos_define_nof_ingress_ecn_profiles,
    dnx_data_qos_qos_define_nof_ingress_policer_profiles,
    dnx_data_qos_qos_define_nof_ingress_vlan_pcp_profiles,
    dnx_data_qos_qos_define_nof_egress_profiles,
    dnx_data_qos_qos_define_nof_egress_policer_profiles,
    dnx_data_qos_qos_define_nof_egress_mpls_php_profiles,
    dnx_data_qos_qos_define_nof_egress_opcode_profiles,
    dnx_data_qos_qos_define_nof_egress_php_opcode_profiles,
    dnx_data_qos_qos_define_nof_egress_ttl_pipe_profiles,
    dnx_data_qos_qos_define_nof_ingress_propagation_profiles,
    dnx_data_qos_qos_define_nof_ingress_ecn_opcode_profiles,
    dnx_data_qos_qos_define_nof_egress_network_qos_pipe_profiles,
    dnx_data_qos_qos_define_nof_egress_cos_profiles,
    dnx_data_qos_qos_define_packet_max_priority,
    dnx_data_qos_qos_define_packet_max_exp,
    dnx_data_qos_qos_define_packet_max_dscp,
    dnx_data_qos_qos_define_packet_max_cfi,
    dnx_data_qos_qos_define_packet_max_tc,
    dnx_data_qos_qos_define_packet_max_dp,
    dnx_data_qos_qos_define_packet_max_ecn,
    dnx_data_qos_qos_define_packet_max_egress_policer_offset,
    dnx_data_qos_qos_define_max_int_map_id,
    dnx_data_qos_qos_define_max_egress_var,
    dnx_data_qos_qos_define_ecn_bits_width,
    dnx_data_qos_qos_define_eei_mpls_ttl_profile,
    dnx_data_qos_qos_define_routing_dscp_preserve_mode,

    
    _dnx_data_qos_qos_define_nof
} dnx_data_qos_qos_define_e;



uint32 dnx_data_qos_qos_nof_ingress_profiles_get(
    int unit);


uint32 dnx_data_qos_qos_nof_ingress_opcode_profiles_get(
    int unit);


uint32 dnx_data_qos_qos_nof_ingress_ecn_profiles_get(
    int unit);


uint32 dnx_data_qos_qos_nof_ingress_policer_profiles_get(
    int unit);


uint32 dnx_data_qos_qos_nof_ingress_vlan_pcp_profiles_get(
    int unit);


uint32 dnx_data_qos_qos_nof_egress_profiles_get(
    int unit);


uint32 dnx_data_qos_qos_nof_egress_policer_profiles_get(
    int unit);


uint32 dnx_data_qos_qos_nof_egress_mpls_php_profiles_get(
    int unit);


uint32 dnx_data_qos_qos_nof_egress_opcode_profiles_get(
    int unit);


uint32 dnx_data_qos_qos_nof_egress_php_opcode_profiles_get(
    int unit);


uint32 dnx_data_qos_qos_nof_egress_ttl_pipe_profiles_get(
    int unit);


uint32 dnx_data_qos_qos_nof_ingress_propagation_profiles_get(
    int unit);


uint32 dnx_data_qos_qos_nof_ingress_ecn_opcode_profiles_get(
    int unit);


uint32 dnx_data_qos_qos_nof_egress_network_qos_pipe_profiles_get(
    int unit);


uint32 dnx_data_qos_qos_nof_egress_cos_profiles_get(
    int unit);


uint32 dnx_data_qos_qos_packet_max_priority_get(
    int unit);


uint32 dnx_data_qos_qos_packet_max_exp_get(
    int unit);


uint32 dnx_data_qos_qos_packet_max_dscp_get(
    int unit);


uint32 dnx_data_qos_qos_packet_max_cfi_get(
    int unit);


uint32 dnx_data_qos_qos_packet_max_tc_get(
    int unit);


uint32 dnx_data_qos_qos_packet_max_dp_get(
    int unit);


uint32 dnx_data_qos_qos_packet_max_ecn_get(
    int unit);


uint32 dnx_data_qos_qos_packet_max_egress_policer_offset_get(
    int unit);


uint32 dnx_data_qos_qos_max_int_map_id_get(
    int unit);


uint32 dnx_data_qos_qos_max_egress_var_get(
    int unit);


uint32 dnx_data_qos_qos_ecn_bits_width_get(
    int unit);


uint32 dnx_data_qos_qos_eei_mpls_ttl_profile_get(
    int unit);


uint32 dnx_data_qos_qos_routing_dscp_preserve_mode_get(
    int unit);



typedef enum
{

    
    _dnx_data_qos_qos_table_nof
} dnx_data_qos_qos_table_e;









int dnx_data_qos_feature_feature_get(
    int unit,
    dnx_data_qos_feature_feature_e feature);



typedef enum
{
    dnx_data_qos_feature_define_phb_rm_ffc_bits_width,

    
    _dnx_data_qos_feature_define_nof
} dnx_data_qos_feature_define_e;



uint32 dnx_data_qos_feature_phb_rm_ffc_bits_width_get(
    int unit);



typedef enum
{

    
    _dnx_data_qos_feature_table_nof
} dnx_data_qos_feature_table_e;






shr_error_e dnx_data_qos_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

