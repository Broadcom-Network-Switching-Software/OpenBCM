

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_QOS

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_qos.h>



extern shr_error_e jr2_a0_data_qos_attach(
    int unit);
extern shr_error_e jr2_b0_data_qos_attach(
    int unit);
extern shr_error_e j2c_a0_data_qos_attach(
    int unit);
extern shr_error_e q2a_a0_data_qos_attach(
    int unit);
extern shr_error_e j2p_a0_data_qos_attach(
    int unit);



static shr_error_e
dnx_data_qos_qos_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "qos";
    submodule_data->doc = "QOS data";
    
    submodule_data->nof_features = _dnx_data_qos_qos_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data qos qos features");

    submodule_data->features[dnx_data_qos_qos_ecn_L4S].name = "ecn_L4S";
    submodule_data->features[dnx_data_qos_qos_ecn_L4S].doc = "ecn 1 bit for L4S";
    submodule_data->features[dnx_data_qos_qos_ecn_L4S].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_qos_qos_qos_var_type_map_enable].name = "qos_var_type_map_enable";
    submodule_data->features[dnx_data_qos_qos_qos_var_type_map_enable].doc = "Enable configuration of QOS var type map";
    submodule_data->features[dnx_data_qos_qos_qos_var_type_map_enable].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_qos_qos_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data qos qos defines");

    submodule_data->defines[dnx_data_qos_qos_define_nof_ingress_profiles].name = "nof_ingress_profiles";
    submodule_data->defines[dnx_data_qos_qos_define_nof_ingress_profiles].doc = "Number of ingress qos prfiles";
    
    submodule_data->defines[dnx_data_qos_qos_define_nof_ingress_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_qos_qos_define_nof_ingress_opcode_profiles].name = "nof_ingress_opcode_profiles";
    submodule_data->defines[dnx_data_qos_qos_define_nof_ingress_opcode_profiles].doc = "Number of ingress opcode qos prfiles";
    
    submodule_data->defines[dnx_data_qos_qos_define_nof_ingress_opcode_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_qos_qos_define_nof_ingress_ecn_profiles].name = "nof_ingress_ecn_profiles";
    submodule_data->defines[dnx_data_qos_qos_define_nof_ingress_ecn_profiles].doc = "Number of ingress ecn qos prfiles";
    
    submodule_data->defines[dnx_data_qos_qos_define_nof_ingress_ecn_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_qos_qos_define_nof_ingress_policer_profiles].name = "nof_ingress_policer_profiles";
    submodule_data->defines[dnx_data_qos_qos_define_nof_ingress_policer_profiles].doc = "Number of ingress policer qos prfiles";
    
    submodule_data->defines[dnx_data_qos_qos_define_nof_ingress_policer_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_qos_qos_define_nof_ingress_vlan_pcp_profiles].name = "nof_ingress_vlan_pcp_profiles";
    submodule_data->defines[dnx_data_qos_qos_define_nof_ingress_vlan_pcp_profiles].doc = "Number of ingress vlan pcp qos prfiles";
    
    submodule_data->defines[dnx_data_qos_qos_define_nof_ingress_vlan_pcp_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_qos_qos_define_nof_egress_profiles].name = "nof_egress_profiles";
    submodule_data->defines[dnx_data_qos_qos_define_nof_egress_profiles].doc = "Number of egress qos prfiles";
    
    submodule_data->defines[dnx_data_qos_qos_define_nof_egress_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_qos_qos_define_nof_egress_policer_profiles].name = "nof_egress_policer_profiles";
    submodule_data->defines[dnx_data_qos_qos_define_nof_egress_policer_profiles].doc = "Number of egress policer qos prfiles";
    
    submodule_data->defines[dnx_data_qos_qos_define_nof_egress_policer_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_qos_qos_define_nof_egress_mpls_php_profiles].name = "nof_egress_mpls_php_profiles";
    submodule_data->defines[dnx_data_qos_qos_define_nof_egress_mpls_php_profiles].doc = "Number of egress mpls php qos prfiles";
    
    submodule_data->defines[dnx_data_qos_qos_define_nof_egress_mpls_php_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_qos_qos_define_nof_egress_opcode_profiles].name = "nof_egress_opcode_profiles";
    submodule_data->defines[dnx_data_qos_qos_define_nof_egress_opcode_profiles].doc = "Number of egress opcode qos prfiles";
    
    submodule_data->defines[dnx_data_qos_qos_define_nof_egress_opcode_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_qos_qos_define_nof_egress_php_opcode_profiles].name = "nof_egress_php_opcode_profiles";
    submodule_data->defines[dnx_data_qos_qos_define_nof_egress_php_opcode_profiles].doc = "Number of egress php opcode qos prfiles";
    
    submodule_data->defines[dnx_data_qos_qos_define_nof_egress_php_opcode_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_qos_qos_define_nof_egress_ttl_pipe_profiles].name = "nof_egress_ttl_pipe_profiles";
    submodule_data->defines[dnx_data_qos_qos_define_nof_egress_ttl_pipe_profiles].doc = "Number of egress ttl qos pipe profiles";
    
    submodule_data->defines[dnx_data_qos_qos_define_nof_egress_ttl_pipe_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_qos_qos_define_nof_ingress_propagation_profiles].name = "nof_ingress_propagation_profiles";
    submodule_data->defines[dnx_data_qos_qos_define_nof_ingress_propagation_profiles].doc = "Number of propagation profiles";
    
    submodule_data->defines[dnx_data_qos_qos_define_nof_ingress_propagation_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_qos_qos_define_nof_ingress_ecn_opcode_profiles].name = "nof_ingress_ecn_opcode_profiles";
    submodule_data->defines[dnx_data_qos_qos_define_nof_ingress_ecn_opcode_profiles].doc = "Number of ingress ecn opcode profiles";
    
    submodule_data->defines[dnx_data_qos_qos_define_nof_ingress_ecn_opcode_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_qos_qos_define_nof_egress_network_qos_pipe_profiles].name = "nof_egress_network_qos_pipe_profiles";
    submodule_data->defines[dnx_data_qos_qos_define_nof_egress_network_qos_pipe_profiles].doc = "Number of network_qos profiles";
    
    submodule_data->defines[dnx_data_qos_qos_define_nof_egress_network_qos_pipe_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_qos_qos_define_nof_egress_cos_profiles].name = "nof_egress_cos_profiles";
    submodule_data->defines[dnx_data_qos_qos_define_nof_egress_cos_profiles].doc = "Number of egress cos profiles";
    
    submodule_data->defines[dnx_data_qos_qos_define_nof_egress_cos_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_qos_qos_define_packet_max_priority].name = "packet_max_priority";
    submodule_data->defines[dnx_data_qos_qos_define_packet_max_priority].doc = "QOS Max priority for packet";
    
    submodule_data->defines[dnx_data_qos_qos_define_packet_max_priority].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_qos_qos_define_packet_max_exp].name = "packet_max_exp";
    submodule_data->defines[dnx_data_qos_qos_define_packet_max_exp].doc = "QOS Max exp for packet";
    
    submodule_data->defines[dnx_data_qos_qos_define_packet_max_exp].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_qos_qos_define_packet_max_dscp].name = "packet_max_dscp";
    submodule_data->defines[dnx_data_qos_qos_define_packet_max_dscp].doc = "QOS Max dscp for packet";
    
    submodule_data->defines[dnx_data_qos_qos_define_packet_max_dscp].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_qos_qos_define_packet_max_cfi].name = "packet_max_cfi";
    submodule_data->defines[dnx_data_qos_qos_define_packet_max_cfi].doc = "QOS Max cfi for packet";
    
    submodule_data->defines[dnx_data_qos_qos_define_packet_max_cfi].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_qos_qos_define_packet_max_tc].name = "packet_max_tc";
    submodule_data->defines[dnx_data_qos_qos_define_packet_max_tc].doc = "QOS Max tc for packet";
    
    submodule_data->defines[dnx_data_qos_qos_define_packet_max_tc].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_qos_qos_define_packet_max_dp].name = "packet_max_dp";
    submodule_data->defines[dnx_data_qos_qos_define_packet_max_dp].doc = "QOS Max dp for packet";
    
    submodule_data->defines[dnx_data_qos_qos_define_packet_max_dp].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_qos_qos_define_packet_max_ecn].name = "packet_max_ecn";
    submodule_data->defines[dnx_data_qos_qos_define_packet_max_ecn].doc = "QOS Max ecn for packet";
    
    submodule_data->defines[dnx_data_qos_qos_define_packet_max_ecn].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_qos_qos_define_packet_max_egress_policer_offset].name = "packet_max_egress_policer_offset";
    submodule_data->defines[dnx_data_qos_qos_define_packet_max_egress_policer_offset].doc = "QOS egress policer max offset";
    
    submodule_data->defines[dnx_data_qos_qos_define_packet_max_egress_policer_offset].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_qos_qos_define_max_int_map_id].name = "max_int_map_id";
    submodule_data->defines[dnx_data_qos_qos_define_max_int_map_id].doc = "QOS Max int map id";
    
    submodule_data->defines[dnx_data_qos_qos_define_max_int_map_id].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_qos_qos_define_max_egress_var].name = "max_egress_var";
    submodule_data->defines[dnx_data_qos_qos_define_max_egress_var].doc = "QOS Max egress var";
    
    submodule_data->defines[dnx_data_qos_qos_define_max_egress_var].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_qos_qos_define_ecn_bits_width].name = "ecn_bits_width";
    submodule_data->defines[dnx_data_qos_qos_define_ecn_bits_width].doc = "QOS ecn bits width";
    
    submodule_data->defines[dnx_data_qos_qos_define_ecn_bits_width].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_qos_qos_define_eei_mpls_ttl_profile].name = "eei_mpls_ttl_profile";
    submodule_data->defines[dnx_data_qos_qos_define_eei_mpls_ttl_profile].doc = "TTL profile used by EEI.MPLS when TTL model is pipe";
    
    submodule_data->defines[dnx_data_qos_qos_define_eei_mpls_ttl_profile].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_qos_qos_define_routing_dscp_preserve_mode].name = "routing_dscp_preserve_mode";
    submodule_data->defines[dnx_data_qos_qos_define_routing_dscp_preserve_mode].doc = "DSCP preserve modes in routing case.";
    
    submodule_data->defines[dnx_data_qos_qos_define_routing_dscp_preserve_mode].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_qos_qos_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data qos qos tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_qos_qos_feature_get(
    int unit,
    dnx_data_qos_qos_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_qos, dnx_data_qos_submodule_qos, feature);
}


uint32
dnx_data_qos_qos_nof_ingress_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_qos, dnx_data_qos_submodule_qos, dnx_data_qos_qos_define_nof_ingress_profiles);
}

uint32
dnx_data_qos_qos_nof_ingress_opcode_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_qos, dnx_data_qos_submodule_qos, dnx_data_qos_qos_define_nof_ingress_opcode_profiles);
}

uint32
dnx_data_qos_qos_nof_ingress_ecn_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_qos, dnx_data_qos_submodule_qos, dnx_data_qos_qos_define_nof_ingress_ecn_profiles);
}

uint32
dnx_data_qos_qos_nof_ingress_policer_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_qos, dnx_data_qos_submodule_qos, dnx_data_qos_qos_define_nof_ingress_policer_profiles);
}

uint32
dnx_data_qos_qos_nof_ingress_vlan_pcp_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_qos, dnx_data_qos_submodule_qos, dnx_data_qos_qos_define_nof_ingress_vlan_pcp_profiles);
}

uint32
dnx_data_qos_qos_nof_egress_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_qos, dnx_data_qos_submodule_qos, dnx_data_qos_qos_define_nof_egress_profiles);
}

uint32
dnx_data_qos_qos_nof_egress_policer_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_qos, dnx_data_qos_submodule_qos, dnx_data_qos_qos_define_nof_egress_policer_profiles);
}

uint32
dnx_data_qos_qos_nof_egress_mpls_php_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_qos, dnx_data_qos_submodule_qos, dnx_data_qos_qos_define_nof_egress_mpls_php_profiles);
}

uint32
dnx_data_qos_qos_nof_egress_opcode_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_qos, dnx_data_qos_submodule_qos, dnx_data_qos_qos_define_nof_egress_opcode_profiles);
}

uint32
dnx_data_qos_qos_nof_egress_php_opcode_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_qos, dnx_data_qos_submodule_qos, dnx_data_qos_qos_define_nof_egress_php_opcode_profiles);
}

uint32
dnx_data_qos_qos_nof_egress_ttl_pipe_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_qos, dnx_data_qos_submodule_qos, dnx_data_qos_qos_define_nof_egress_ttl_pipe_profiles);
}

uint32
dnx_data_qos_qos_nof_ingress_propagation_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_qos, dnx_data_qos_submodule_qos, dnx_data_qos_qos_define_nof_ingress_propagation_profiles);
}

uint32
dnx_data_qos_qos_nof_ingress_ecn_opcode_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_qos, dnx_data_qos_submodule_qos, dnx_data_qos_qos_define_nof_ingress_ecn_opcode_profiles);
}

uint32
dnx_data_qos_qos_nof_egress_network_qos_pipe_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_qos, dnx_data_qos_submodule_qos, dnx_data_qos_qos_define_nof_egress_network_qos_pipe_profiles);
}

uint32
dnx_data_qos_qos_nof_egress_cos_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_qos, dnx_data_qos_submodule_qos, dnx_data_qos_qos_define_nof_egress_cos_profiles);
}

uint32
dnx_data_qos_qos_packet_max_priority_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_qos, dnx_data_qos_submodule_qos, dnx_data_qos_qos_define_packet_max_priority);
}

uint32
dnx_data_qos_qos_packet_max_exp_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_qos, dnx_data_qos_submodule_qos, dnx_data_qos_qos_define_packet_max_exp);
}

uint32
dnx_data_qos_qos_packet_max_dscp_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_qos, dnx_data_qos_submodule_qos, dnx_data_qos_qos_define_packet_max_dscp);
}

uint32
dnx_data_qos_qos_packet_max_cfi_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_qos, dnx_data_qos_submodule_qos, dnx_data_qos_qos_define_packet_max_cfi);
}

uint32
dnx_data_qos_qos_packet_max_tc_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_qos, dnx_data_qos_submodule_qos, dnx_data_qos_qos_define_packet_max_tc);
}

uint32
dnx_data_qos_qos_packet_max_dp_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_qos, dnx_data_qos_submodule_qos, dnx_data_qos_qos_define_packet_max_dp);
}

uint32
dnx_data_qos_qos_packet_max_ecn_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_qos, dnx_data_qos_submodule_qos, dnx_data_qos_qos_define_packet_max_ecn);
}

uint32
dnx_data_qos_qos_packet_max_egress_policer_offset_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_qos, dnx_data_qos_submodule_qos, dnx_data_qos_qos_define_packet_max_egress_policer_offset);
}

uint32
dnx_data_qos_qos_max_int_map_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_qos, dnx_data_qos_submodule_qos, dnx_data_qos_qos_define_max_int_map_id);
}

uint32
dnx_data_qos_qos_max_egress_var_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_qos, dnx_data_qos_submodule_qos, dnx_data_qos_qos_define_max_egress_var);
}

uint32
dnx_data_qos_qos_ecn_bits_width_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_qos, dnx_data_qos_submodule_qos, dnx_data_qos_qos_define_ecn_bits_width);
}

uint32
dnx_data_qos_qos_eei_mpls_ttl_profile_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_qos, dnx_data_qos_submodule_qos, dnx_data_qos_qos_define_eei_mpls_ttl_profile);
}

uint32
dnx_data_qos_qos_routing_dscp_preserve_mode_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_qos, dnx_data_qos_submodule_qos, dnx_data_qos_qos_define_routing_dscp_preserve_mode);
}










static shr_error_e
dnx_data_qos_feature_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "feature";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_qos_feature_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data qos feature features");

    submodule_data->features[dnx_data_qos_feature_egress_remark_or_keep_enabler].name = "egress_remark_or_keep_enabler";
    submodule_data->features[dnx_data_qos_feature_egress_remark_or_keep_enabler].doc = "enabler map table for remarking egress forward header";
    submodule_data->features[dnx_data_qos_feature_egress_remark_or_keep_enabler].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_qos_feature_fwd_eve_pcp_diff_encap].name = "fwd_eve_pcp_diff_encap";
    submodule_data->features[dnx_data_qos_feature_fwd_eve_pcp_diff_encap].doc = "eve pcp source at fwd stage is different with encap stage";
    submodule_data->features[dnx_data_qos_feature_fwd_eve_pcp_diff_encap].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_qos_feature_phb_rm_ffc_extend].name = "phb_rm_ffc_extend";
    submodule_data->features[dnx_data_qos_feature_phb_rm_ffc_extend].doc = "phb and remark FFCs extend 1 bit each for field_size and key_offset";
    submodule_data->features[dnx_data_qos_feature_phb_rm_ffc_extend].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_qos_feature_hdr_qual_after_mpls_available].name = "hdr_qual_after_mpls_available";
    submodule_data->features[dnx_data_qos_feature_hdr_qual_after_mpls_available].doc = "If the header qualifier after mpls layer is available in QoS context selection";
    submodule_data->features[dnx_data_qos_feature_hdr_qual_after_mpls_available].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_qos_feature_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data qos feature defines");

    submodule_data->defines[dnx_data_qos_feature_define_phb_rm_ffc_bits_width].name = "phb_rm_ffc_bits_width";
    submodule_data->defines[dnx_data_qos_feature_define_phb_rm_ffc_bits_width].doc = "phb and remark ffc bits width";
    
    submodule_data->defines[dnx_data_qos_feature_define_phb_rm_ffc_bits_width].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_qos_feature_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data qos feature tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_qos_feature_feature_get(
    int unit,
    dnx_data_qos_feature_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_qos, dnx_data_qos_submodule_feature, feature);
}


uint32
dnx_data_qos_feature_phb_rm_ffc_bits_width_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_qos, dnx_data_qos_submodule_feature, dnx_data_qos_feature_define_phb_rm_ffc_bits_width);
}







shr_error_e
dnx_data_qos_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "qos";
    module_data->nof_submodules = _dnx_data_qos_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data qos submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_qos_qos_init(unit, &module_data->submodules[dnx_data_qos_submodule_qos]));
    SHR_IF_ERR_EXIT(dnx_data_qos_feature_init(unit, &module_data->submodules[dnx_data_qos_submodule_feature]));
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_qos_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_qos_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_qos_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_qos_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_qos_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_qos_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_qos_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_qos_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_qos_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_qos_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_qos_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_qos_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_qos_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_qos_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_qos_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_qos_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_qos_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

