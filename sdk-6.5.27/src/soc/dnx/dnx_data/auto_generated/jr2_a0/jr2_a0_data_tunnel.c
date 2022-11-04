
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TUNNEL

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_tunnel.h>








static shr_error_e
jr2_a0_dnx_data_tunnel_udp_nof_udp_ports_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tunnel;
    int submodule_index = dnx_data_tunnel_submodule_udp;
    int define_index = dnx_data_tunnel_udp_define_nof_udp_ports_profiles;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8;

    
    define->data = 8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_tunnel_udp_first_udp_ports_profile_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tunnel;
    int submodule_index = dnx_data_tunnel_submodule_udp;
    int define_index = dnx_data_tunnel_udp_define_first_udp_ports_profile;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_tunnel_tunnel_type_udp_tunnel_type_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tunnel;
    int submodule_index = dnx_data_tunnel_submodule_tunnel_type;
    int define_index = dnx_data_tunnel_tunnel_type_define_udp_tunnel_type;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_tunnel_tunnel_type_gre_tunnel_type_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tunnel;
    int submodule_index = dnx_data_tunnel_submodule_tunnel_type;
    int define_index = dnx_data_tunnel_tunnel_type_define_gre_tunnel_type;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_tunnel_tunnel_type_geneve_tunnel_type_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tunnel;
    int submodule_index = dnx_data_tunnel_submodule_tunnel_type;
    int define_index = dnx_data_tunnel_tunnel_type_define_geneve_tunnel_type_supported;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_tunnel_roo_terminate_only_8B_in_ip_at_term_block_using_pmf_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tunnel;
    int submodule_index = dnx_data_tunnel_submodule_roo;
    int define_index = dnx_data_tunnel_roo_define_terminate_only_8B_in_ip_at_term_block_using_pmf;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_tunnel_feature_ipvx_tunnel_over_mpls_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_tunnel;
    int submodule_index = dnx_data_tunnel_submodule_feature;
    int feature_index = dnx_data_tunnel_feature_ipvx_tunnel_over_mpls_supported;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_tunnel_feature_ipvx_tunnel_over_mpls_use_vtt_tcam_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_tunnel;
    int submodule_index = dnx_data_tunnel_submodule_feature;
    int feature_index = dnx_data_tunnel_feature_ipvx_tunnel_over_mpls_use_vtt_tcam;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_tunnel_feature_tunnel_route_disable_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tunnel;
    int submodule_index = dnx_data_tunnel_submodule_feature;
    int define_index = dnx_data_tunnel_feature_define_tunnel_route_disable;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_tunnel_feature_dedicated_fodo_to_vni_table_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tunnel;
    int submodule_index = dnx_data_tunnel_submodule_feature;
    int define_index = dnx_data_tunnel_feature_define_dedicated_fodo_to_vni_table;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_tunnel_feature_dip_idx_type_is_lif_field_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tunnel;
    int submodule_index = dnx_data_tunnel_submodule_feature;
    int define_index = dnx_data_tunnel_feature_define_dip_idx_type_is_lif_field;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_tunnel_feature_fodo_to_vni_entry_created_by_vlan_api_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tunnel;
    int submodule_index = dnx_data_tunnel_submodule_feature;
    int define_index = dnx_data_tunnel_feature_define_fodo_to_vni_entry_created_by_vlan_api;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_tunnel_feature_has_flow_gport_to_tunnel_type_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tunnel;
    int submodule_index = dnx_data_tunnel_submodule_feature;
    int define_index = dnx_data_tunnel_feature_define_has_flow_gport_to_tunnel_type;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_tunnel_feature_support_learn_mode_async_lif_and_fec_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tunnel;
    int submodule_index = dnx_data_tunnel_submodule_feature;
    int define_index = dnx_data_tunnel_feature_define_support_learn_mode_async_lif_and_fec;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_tunnel_feature_l3_fodo_result_type_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tunnel;
    int submodule_index = dnx_data_tunnel_submodule_feature;
    int define_index = dnx_data_tunnel_feature_define_l3_fodo_result_type_supported;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_tunnel_feature_udp_tunnel_nat_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tunnel;
    int submodule_index = dnx_data_tunnel_submodule_feature;
    int define_index = dnx_data_tunnel_feature_define_udp_tunnel_nat;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_tunnel_feature_short_network_domain_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tunnel;
    int submodule_index = dnx_data_tunnel_submodule_feature;
    int define_index = dnx_data_tunnel_feature_define_short_network_domain;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}




shr_error_e
jr2_a0_data_tunnel_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_tunnel;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_tunnel_submodule_udp;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_tunnel_udp_define_nof_udp_ports_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_tunnel_udp_nof_udp_ports_profiles_set;
    data_index = dnx_data_tunnel_udp_define_first_udp_ports_profile;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_tunnel_udp_first_udp_ports_profile_set;

    

    
    
    submodule_index = dnx_data_tunnel_submodule_tunnel_type;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_tunnel_tunnel_type_define_udp_tunnel_type;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_tunnel_tunnel_type_udp_tunnel_type_set;
    data_index = dnx_data_tunnel_tunnel_type_define_gre_tunnel_type;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_tunnel_tunnel_type_gre_tunnel_type_set;
    data_index = dnx_data_tunnel_tunnel_type_define_geneve_tunnel_type_supported;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_tunnel_tunnel_type_geneve_tunnel_type_supported_set;

    

    
    
    submodule_index = dnx_data_tunnel_submodule_roo;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_tunnel_roo_define_terminate_only_8B_in_ip_at_term_block_using_pmf;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_tunnel_roo_terminate_only_8B_in_ip_at_term_block_using_pmf_set;

    

    
    
    submodule_index = dnx_data_tunnel_submodule_feature;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_tunnel_feature_define_tunnel_route_disable;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_tunnel_feature_tunnel_route_disable_set;
    data_index = dnx_data_tunnel_feature_define_dedicated_fodo_to_vni_table;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_tunnel_feature_dedicated_fodo_to_vni_table_set;
    data_index = dnx_data_tunnel_feature_define_dip_idx_type_is_lif_field;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_tunnel_feature_dip_idx_type_is_lif_field_set;
    data_index = dnx_data_tunnel_feature_define_fodo_to_vni_entry_created_by_vlan_api;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_tunnel_feature_fodo_to_vni_entry_created_by_vlan_api_set;
    data_index = dnx_data_tunnel_feature_define_has_flow_gport_to_tunnel_type;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_tunnel_feature_has_flow_gport_to_tunnel_type_set;
    data_index = dnx_data_tunnel_feature_define_support_learn_mode_async_lif_and_fec;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_tunnel_feature_support_learn_mode_async_lif_and_fec_set;
    data_index = dnx_data_tunnel_feature_define_l3_fodo_result_type_supported;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_tunnel_feature_l3_fodo_result_type_supported_set;
    data_index = dnx_data_tunnel_feature_define_udp_tunnel_nat;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_tunnel_feature_udp_tunnel_nat_set;
    data_index = dnx_data_tunnel_feature_define_short_network_domain;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_tunnel_feature_short_network_domain_set;

    
    data_index = dnx_data_tunnel_feature_ipvx_tunnel_over_mpls_supported;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_tunnel_feature_ipvx_tunnel_over_mpls_supported_set;
    data_index = dnx_data_tunnel_feature_ipvx_tunnel_over_mpls_use_vtt_tcam;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_tunnel_feature_ipvx_tunnel_over_mpls_use_vtt_tcam_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

