

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_QOS_H_

#define _DNX_DATA_QOS_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_qos.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_qos_init(
    int unit);







typedef enum
{
    
    dnx_data_qos_qos_ecn_L4S,
    
    dnx_data_qos_qos_qos_var_type_map_enable,

    
    _dnx_data_qos_qos_feature_nof
} dnx_data_qos_qos_feature_e;



typedef int(
    *dnx_data_qos_qos_feature_get_f) (
    int unit,
    dnx_data_qos_qos_feature_e feature);


typedef uint32(
    *dnx_data_qos_qos_nof_ingress_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_qos_qos_nof_ingress_opcode_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_qos_qos_nof_ingress_ecn_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_qos_qos_nof_ingress_policer_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_qos_qos_nof_ingress_vlan_pcp_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_qos_qos_nof_egress_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_qos_qos_nof_egress_policer_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_qos_qos_nof_egress_mpls_php_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_qos_qos_nof_egress_opcode_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_qos_qos_nof_egress_php_opcode_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_qos_qos_nof_egress_ttl_pipe_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_qos_qos_nof_ingress_propagation_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_qos_qos_nof_ingress_ecn_opcode_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_qos_qos_nof_egress_network_qos_pipe_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_qos_qos_nof_egress_cos_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_qos_qos_packet_max_priority_get_f) (
    int unit);


typedef uint32(
    *dnx_data_qos_qos_packet_max_exp_get_f) (
    int unit);


typedef uint32(
    *dnx_data_qos_qos_packet_max_dscp_get_f) (
    int unit);


typedef uint32(
    *dnx_data_qos_qos_packet_max_cfi_get_f) (
    int unit);


typedef uint32(
    *dnx_data_qos_qos_packet_max_tc_get_f) (
    int unit);


typedef uint32(
    *dnx_data_qos_qos_packet_max_dp_get_f) (
    int unit);


typedef uint32(
    *dnx_data_qos_qos_packet_max_ecn_get_f) (
    int unit);


typedef uint32(
    *dnx_data_qos_qos_packet_max_egress_policer_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_qos_qos_max_int_map_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_qos_qos_max_egress_var_get_f) (
    int unit);


typedef uint32(
    *dnx_data_qos_qos_ecn_bits_width_get_f) (
    int unit);


typedef uint32(
    *dnx_data_qos_qos_eei_mpls_ttl_profile_get_f) (
    int unit);


typedef uint32(
    *dnx_data_qos_qos_routing_dscp_preserve_mode_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_qos_qos_feature_get_f feature_get;
    
    dnx_data_qos_qos_nof_ingress_profiles_get_f nof_ingress_profiles_get;
    
    dnx_data_qos_qos_nof_ingress_opcode_profiles_get_f nof_ingress_opcode_profiles_get;
    
    dnx_data_qos_qos_nof_ingress_ecn_profiles_get_f nof_ingress_ecn_profiles_get;
    
    dnx_data_qos_qos_nof_ingress_policer_profiles_get_f nof_ingress_policer_profiles_get;
    
    dnx_data_qos_qos_nof_ingress_vlan_pcp_profiles_get_f nof_ingress_vlan_pcp_profiles_get;
    
    dnx_data_qos_qos_nof_egress_profiles_get_f nof_egress_profiles_get;
    
    dnx_data_qos_qos_nof_egress_policer_profiles_get_f nof_egress_policer_profiles_get;
    
    dnx_data_qos_qos_nof_egress_mpls_php_profiles_get_f nof_egress_mpls_php_profiles_get;
    
    dnx_data_qos_qos_nof_egress_opcode_profiles_get_f nof_egress_opcode_profiles_get;
    
    dnx_data_qos_qos_nof_egress_php_opcode_profiles_get_f nof_egress_php_opcode_profiles_get;
    
    dnx_data_qos_qos_nof_egress_ttl_pipe_profiles_get_f nof_egress_ttl_pipe_profiles_get;
    
    dnx_data_qos_qos_nof_ingress_propagation_profiles_get_f nof_ingress_propagation_profiles_get;
    
    dnx_data_qos_qos_nof_ingress_ecn_opcode_profiles_get_f nof_ingress_ecn_opcode_profiles_get;
    
    dnx_data_qos_qos_nof_egress_network_qos_pipe_profiles_get_f nof_egress_network_qos_pipe_profiles_get;
    
    dnx_data_qos_qos_nof_egress_cos_profiles_get_f nof_egress_cos_profiles_get;
    
    dnx_data_qos_qos_packet_max_priority_get_f packet_max_priority_get;
    
    dnx_data_qos_qos_packet_max_exp_get_f packet_max_exp_get;
    
    dnx_data_qos_qos_packet_max_dscp_get_f packet_max_dscp_get;
    
    dnx_data_qos_qos_packet_max_cfi_get_f packet_max_cfi_get;
    
    dnx_data_qos_qos_packet_max_tc_get_f packet_max_tc_get;
    
    dnx_data_qos_qos_packet_max_dp_get_f packet_max_dp_get;
    
    dnx_data_qos_qos_packet_max_ecn_get_f packet_max_ecn_get;
    
    dnx_data_qos_qos_packet_max_egress_policer_offset_get_f packet_max_egress_policer_offset_get;
    
    dnx_data_qos_qos_max_int_map_id_get_f max_int_map_id_get;
    
    dnx_data_qos_qos_max_egress_var_get_f max_egress_var_get;
    
    dnx_data_qos_qos_ecn_bits_width_get_f ecn_bits_width_get;
    
    dnx_data_qos_qos_eei_mpls_ttl_profile_get_f eei_mpls_ttl_profile_get;
    
    dnx_data_qos_qos_routing_dscp_preserve_mode_get_f routing_dscp_preserve_mode_get;
} dnx_data_if_qos_qos_t;







typedef enum
{
    
    dnx_data_qos_feature_egress_remark_or_keep_enabler,
    
    dnx_data_qos_feature_fwd_eve_pcp_diff_encap,
    
    dnx_data_qos_feature_phb_rm_ffc_extend,
    
    dnx_data_qos_feature_hdr_qual_after_mpls_available,

    
    _dnx_data_qos_feature_feature_nof
} dnx_data_qos_feature_feature_e;



typedef int(
    *dnx_data_qos_feature_feature_get_f) (
    int unit,
    dnx_data_qos_feature_feature_e feature);


typedef uint32(
    *dnx_data_qos_feature_phb_rm_ffc_bits_width_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_qos_feature_feature_get_f feature_get;
    
    dnx_data_qos_feature_phb_rm_ffc_bits_width_get_f phb_rm_ffc_bits_width_get;
} dnx_data_if_qos_feature_t;





typedef struct
{
    
    dnx_data_if_qos_qos_t qos;
    
    dnx_data_if_qos_feature_t feature;
} dnx_data_if_qos_t;




extern dnx_data_if_qos_t dnx_data_qos;


#endif 

