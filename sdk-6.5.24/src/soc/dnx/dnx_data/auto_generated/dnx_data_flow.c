
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLOW

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_flow.h>



#ifdef BCM_DNX2_SUPPORT

extern shr_error_e jr2_a0_data_flow_attach(
    int unit);

#endif 
#ifdef BCM_DNX2_SUPPORT

extern shr_error_e j2p_a0_data_flow_attach(
    int unit);

#endif 



static shr_error_e
dnx_data_flow_general_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "general";
    submodule_data->doc = "general info";
    
    submodule_data->nof_features = _dnx_data_flow_general_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data flow general features");

    submodule_data->features[dnx_data_flow_general_is_tests_supported].name = "is_tests_supported";
    submodule_data->features[dnx_data_flow_general_is_tests_supported].doc = "";
    submodule_data->features[dnx_data_flow_general_is_tests_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_flow_general_is_inlif_profile_dedicated_logic_enabled].name = "is_inlif_profile_dedicated_logic_enabled";
    submodule_data->features[dnx_data_flow_general_is_inlif_profile_dedicated_logic_enabled].doc = "Flow Module is used to determine if service_type will use uts cb or will use a dedicated logic";
    submodule_data->features[dnx_data_flow_general_is_inlif_profile_dedicated_logic_enabled].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_flow_general_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data flow general defines");

    
    submodule_data->nof_tables = _dnx_data_flow_general_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data flow general tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_flow_general_feature_get(
    int unit,
    dnx_data_flow_general_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_flow, dnx_data_flow_submodule_general, feature);
}











static shr_error_e
dnx_data_flow_ipv4_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "ipv4";
    submodule_data->doc = "IPV4 tunnel data";
    
    submodule_data->nof_features = _dnx_data_flow_ipv4_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data flow ipv4 features");

    
    submodule_data->nof_defines = _dnx_data_flow_ipv4_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data flow ipv4 defines");

    submodule_data->defines[dnx_data_flow_ipv4_define_use_flow_lif_init].name = "use_flow_lif_init";
    submodule_data->defines[dnx_data_flow_ipv4_define_use_flow_lif_init].doc = "Use flow lif to configure init tunnels";
    
    submodule_data->defines[dnx_data_flow_ipv4_define_use_flow_lif_init].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_flow_ipv4_define_use_flow_lif_term].name = "use_flow_lif_term";
    submodule_data->defines[dnx_data_flow_ipv4_define_use_flow_lif_term].doc = "Use flow lif to configure term tunnels";
    
    submodule_data->defines[dnx_data_flow_ipv4_define_use_flow_lif_term].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_flow_ipv4_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data flow ipv4 tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_flow_ipv4_feature_get(
    int unit,
    dnx_data_flow_ipv4_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_flow, dnx_data_flow_submodule_ipv4, feature);
}


uint32
dnx_data_flow_ipv4_use_flow_lif_init_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_flow, dnx_data_flow_submodule_ipv4, dnx_data_flow_ipv4_define_use_flow_lif_init);
}

uint32
dnx_data_flow_ipv4_use_flow_lif_term_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_flow, dnx_data_flow_submodule_ipv4, dnx_data_flow_ipv4_define_use_flow_lif_term);
}










static shr_error_e
dnx_data_flow_in_lif_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "in_lif";
    submodule_data->doc = "In LIF data";
    
    submodule_data->nof_features = _dnx_data_flow_in_lif_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data flow in_lif features");

    submodule_data->features[dnx_data_flow_in_lif_flow_in_use].name = "flow_in_use";
    submodule_data->features[dnx_data_flow_in_lif_flow_in_use].doc = "Use flow to configure In LIF.";
    submodule_data->features[dnx_data_flow_in_lif_flow_in_use].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_flow_in_lif_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data flow in_lif defines");

    
    submodule_data->nof_tables = _dnx_data_flow_in_lif_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data flow in_lif tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_flow_in_lif_feature_get(
    int unit,
    dnx_data_flow_in_lif_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_flow, dnx_data_flow_submodule_in_lif, feature);
}











static shr_error_e
dnx_data_flow_mpls_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "mpls";
    submodule_data->doc = "mpls flow data";
    
    submodule_data->nof_features = _dnx_data_flow_mpls_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data flow mpls features");

    submodule_data->features[dnx_data_flow_mpls_ingress_flow_in_use].name = "ingress_flow_in_use";
    submodule_data->features[dnx_data_flow_mpls_ingress_flow_in_use].doc = "flow Module is used for allocating InLIFs";
    submodule_data->features[dnx_data_flow_mpls_ingress_flow_in_use].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_flow_mpls_egress_flow_in_use].name = "egress_flow_in_use";
    submodule_data->features[dnx_data_flow_mpls_egress_flow_in_use].doc = "flow Module is used for allocating OutLIFs";
    submodule_data->features[dnx_data_flow_mpls_egress_flow_in_use].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_flow_mpls_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data flow mpls defines");

    
    submodule_data->nof_tables = _dnx_data_flow_mpls_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data flow mpls tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_flow_mpls_feature_get(
    int unit,
    dnx_data_flow_mpls_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_flow, dnx_data_flow_submodule_mpls, feature);
}











static shr_error_e
dnx_data_flow_mpls_port_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "mpls_port";
    submodule_data->doc = "mpls port (vpls) flow data";
    
    submodule_data->nof_features = _dnx_data_flow_mpls_port_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data flow mpls_port features");

    submodule_data->features[dnx_data_flow_mpls_port_flow_in_use].name = "flow_in_use";
    submodule_data->features[dnx_data_flow_mpls_port_flow_in_use].doc = "flow Module is used for allocating LIFs";
    submodule_data->features[dnx_data_flow_mpls_port_flow_in_use].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_flow_mpls_port_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data flow mpls_port defines");

    
    submodule_data->nof_tables = _dnx_data_flow_mpls_port_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data flow mpls_port tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_flow_mpls_port_feature_get(
    int unit,
    dnx_data_flow_mpls_port_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_flow, dnx_data_flow_submodule_mpls_port, feature);
}











static shr_error_e
dnx_data_flow_arp_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "arp";
    submodule_data->doc = "arp flow data";
    
    submodule_data->nof_features = _dnx_data_flow_arp_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data flow arp features");

    submodule_data->features[dnx_data_flow_arp_flow_in_use].name = "flow_in_use";
    submodule_data->features[dnx_data_flow_arp_flow_in_use].doc = "flow Module is used for allocating LIFs";
    submodule_data->features[dnx_data_flow_arp_flow_in_use].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_flow_arp_virtual_lif_support].name = "virtual_lif_support";
    submodule_data->features[dnx_data_flow_arp_virtual_lif_support].doc = "Indication if virtual LIFs are supported for ARP";
    submodule_data->features[dnx_data_flow_arp_virtual_lif_support].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_flow_arp_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data flow arp defines");

    
    submodule_data->nof_tables = _dnx_data_flow_arp_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data flow arp tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_flow_arp_feature_get(
    int unit,
    dnx_data_flow_arp_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_flow, dnx_data_flow_submodule_arp, feature);
}











static shr_error_e
dnx_data_flow_vlan_port_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "vlan_port";
    submodule_data->doc = "VLAN PORT data";
    
    submodule_data->nof_features = _dnx_data_flow_vlan_port_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data flow vlan_port features");

    submodule_data->features[dnx_data_flow_vlan_port_flow_in_use].name = "flow_in_use";
    submodule_data->features[dnx_data_flow_vlan_port_flow_in_use].doc = "Flow Module is used for allocating LIFs";
    submodule_data->features[dnx_data_flow_vlan_port_flow_in_use].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_flow_vlan_port_inlif_result_type_missing].name = "inlif_result_type_missing";
    submodule_data->features[dnx_data_flow_vlan_port_inlif_result_type_missing].doc = "Some INLIF result types are missing";
    submodule_data->features[dnx_data_flow_vlan_port_inlif_result_type_missing].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_flow_vlan_port_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data flow vlan_port defines");

    
    submodule_data->nof_tables = _dnx_data_flow_vlan_port_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data flow vlan_port tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_flow_vlan_port_feature_get(
    int unit,
    dnx_data_flow_vlan_port_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_flow, dnx_data_flow_submodule_vlan_port, feature);
}











static shr_error_e
dnx_data_flow_algo_gpm_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "algo_gpm";
    submodule_data->doc = "algo gpm definitions related to flow";
    
    submodule_data->nof_features = _dnx_data_flow_algo_gpm_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data flow algo_gpm features");

    
    submodule_data->nof_defines = _dnx_data_flow_algo_gpm_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data flow algo_gpm defines");

    submodule_data->defines[dnx_data_flow_algo_gpm_define_flow_gport_in_use].name = "flow_gport_in_use";
    submodule_data->defines[dnx_data_flow_algo_gpm_define_flow_gport_in_use].doc = "if true, all algo gpm db holds flow gport type and not legacy gport types";
    
    submodule_data->defines[dnx_data_flow_algo_gpm_define_flow_gport_in_use].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_flow_algo_gpm_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data flow algo_gpm tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_flow_algo_gpm_feature_get(
    int unit,
    dnx_data_flow_algo_gpm_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_flow, dnx_data_flow_submodule_algo_gpm, feature);
}


uint32
dnx_data_flow_algo_gpm_flow_gport_in_use_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_flow, dnx_data_flow_submodule_algo_gpm, dnx_data_flow_algo_gpm_define_flow_gport_in_use);
}










static shr_error_e
dnx_data_flow_wide_data_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "wide_data";
    submodule_data->doc = "wide data definitions related to flow";
    
    submodule_data->nof_features = _dnx_data_flow_wide_data_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data flow wide_data features");

    submodule_data->features[dnx_data_flow_wide_data_flow_in_use].name = "flow_in_use";
    submodule_data->features[dnx_data_flow_wide_data_flow_in_use].doc = "Flow Module is used for setting wide data";
    submodule_data->features[dnx_data_flow_wide_data_flow_in_use].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_flow_wide_data_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data flow wide_data defines");

    
    submodule_data->nof_tables = _dnx_data_flow_wide_data_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data flow wide_data tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_flow_wide_data_feature_get(
    int unit,
    dnx_data_flow_wide_data_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_flow, dnx_data_flow_submodule_wide_data, feature);
}











static shr_error_e
dnx_data_flow_stat_pp_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "stat_pp";
    submodule_data->doc = "stat pp definitions related to flow";
    
    submodule_data->nof_features = _dnx_data_flow_stat_pp_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data flow stat_pp features");

    submodule_data->features[dnx_data_flow_stat_pp_flow_in_use].name = "flow_in_use";
    submodule_data->features[dnx_data_flow_stat_pp_flow_in_use].doc = "Flow Module is used for setting stat pp using flow";
    submodule_data->features[dnx_data_flow_stat_pp_flow_in_use].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_flow_stat_pp_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data flow stat_pp defines");

    
    submodule_data->nof_tables = _dnx_data_flow_stat_pp_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data flow stat_pp tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_flow_stat_pp_feature_get(
    int unit,
    dnx_data_flow_stat_pp_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_flow, dnx_data_flow_submodule_stat_pp, feature);
}











static shr_error_e
dnx_data_flow_rch_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "rch";
    submodule_data->doc = "rch definitions related to flow";
    
    submodule_data->nof_features = _dnx_data_flow_rch_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data flow rch features");

    submodule_data->features[dnx_data_flow_rch_flow_in_use].name = "flow_in_use";
    submodule_data->features[dnx_data_flow_rch_flow_in_use].doc = "Flow Module is used for setting rch (l2_egress) using flow";
    submodule_data->features[dnx_data_flow_rch_flow_in_use].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_flow_rch_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data flow rch defines");

    
    submodule_data->nof_tables = _dnx_data_flow_rch_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data flow rch tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_flow_rch_feature_get(
    int unit,
    dnx_data_flow_rch_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_flow, dnx_data_flow_submodule_rch, feature);
}











static shr_error_e
dnx_data_flow_trap_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "trap";
    submodule_data->doc = "trap definitions related to flow";
    
    submodule_data->nof_features = _dnx_data_flow_trap_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data flow trap features");

    submodule_data->features[dnx_data_flow_trap_flow_in_use].name = "flow_in_use";
    submodule_data->features[dnx_data_flow_trap_flow_in_use].doc = "Flow Module is used for setting trap lif configuration";
    submodule_data->features[dnx_data_flow_trap_flow_in_use].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_flow_trap_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data flow trap defines");

    
    submodule_data->nof_tables = _dnx_data_flow_trap_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data flow trap tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_flow_trap_feature_get(
    int unit,
    dnx_data_flow_trap_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_flow, dnx_data_flow_submodule_trap, feature);
}











static shr_error_e
dnx_data_flow_reflector_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "reflector";
    submodule_data->doc = "reflector definitions related to flow";
    
    submodule_data->nof_features = _dnx_data_flow_reflector_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data flow reflector features");

    submodule_data->features[dnx_data_flow_reflector_flow_in_use].name = "flow_in_use";
    submodule_data->features[dnx_data_flow_reflector_flow_in_use].doc = "Flow Module is used for setting reflector lif configuration";
    submodule_data->features[dnx_data_flow_reflector_flow_in_use].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_flow_reflector_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data flow reflector defines");

    
    submodule_data->nof_tables = _dnx_data_flow_reflector_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data flow reflector tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_flow_reflector_feature_get(
    int unit,
    dnx_data_flow_reflector_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_flow, dnx_data_flow_submodule_reflector, feature);
}








shr_error_e
dnx_data_flow_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "flow";
    module_data->nof_submodules = _dnx_data_flow_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data flow submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_flow_general_init(unit, &module_data->submodules[dnx_data_flow_submodule_general]));
    SHR_IF_ERR_EXIT(dnx_data_flow_ipv4_init(unit, &module_data->submodules[dnx_data_flow_submodule_ipv4]));
    SHR_IF_ERR_EXIT(dnx_data_flow_in_lif_init(unit, &module_data->submodules[dnx_data_flow_submodule_in_lif]));
    SHR_IF_ERR_EXIT(dnx_data_flow_mpls_init(unit, &module_data->submodules[dnx_data_flow_submodule_mpls]));
    SHR_IF_ERR_EXIT(dnx_data_flow_mpls_port_init(unit, &module_data->submodules[dnx_data_flow_submodule_mpls_port]));
    SHR_IF_ERR_EXIT(dnx_data_flow_arp_init(unit, &module_data->submodules[dnx_data_flow_submodule_arp]));
    SHR_IF_ERR_EXIT(dnx_data_flow_vlan_port_init(unit, &module_data->submodules[dnx_data_flow_submodule_vlan_port]));
    SHR_IF_ERR_EXIT(dnx_data_flow_algo_gpm_init(unit, &module_data->submodules[dnx_data_flow_submodule_algo_gpm]));
    SHR_IF_ERR_EXIT(dnx_data_flow_wide_data_init(unit, &module_data->submodules[dnx_data_flow_submodule_wide_data]));
    SHR_IF_ERR_EXIT(dnx_data_flow_stat_pp_init(unit, &module_data->submodules[dnx_data_flow_submodule_stat_pp]));
    SHR_IF_ERR_EXIT(dnx_data_flow_rch_init(unit, &module_data->submodules[dnx_data_flow_submodule_rch]));
    SHR_IF_ERR_EXIT(dnx_data_flow_trap_init(unit, &module_data->submodules[dnx_data_flow_submodule_trap]));
    SHR_IF_ERR_EXIT(dnx_data_flow_reflector_init(unit, &module_data->submodules[dnx_data_flow_submodule_reflector]));
    
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_flow_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_flow_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_flow_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_flow_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_flow_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_flow_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_flow_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_flow_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_flow_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_flow_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2p_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_flow_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_flow_attach(unit));
    }
    else

#endif 
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

