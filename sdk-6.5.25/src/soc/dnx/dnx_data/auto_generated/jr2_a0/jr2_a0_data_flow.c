
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







static shr_error_e
jr2_a0_dnx_data_flow_general_is_tests_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_flow;
    int submodule_index = dnx_data_flow_submodule_general;
    int feature_index = dnx_data_flow_general_is_tests_supported;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_flow_general_is_inlif_profile_dedicated_logic_enabled_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_flow;
    int submodule_index = dnx_data_flow_submodule_general;
    int feature_index = dnx_data_flow_general_is_inlif_profile_dedicated_logic_enabled;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}








static shr_error_e
jr2_a0_dnx_data_flow_ipv4_use_flow_lif_init_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_flow;
    int submodule_index = dnx_data_flow_submodule_ipv4;
    int define_index = dnx_data_flow_ipv4_define_use_flow_lif_init;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_CUSTOM_FEATURE;
    define->property.doc = 
        "\n"
        "Enable/Disable flow lif usage\n"
        "Default: 0\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_enable;
    define->property.method_str = "suffix_enable";
    define->property.suffix = "use_flow_lif_tunnel_init";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_flow_ipv4_use_flow_lif_term_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_flow;
    int submodule_index = dnx_data_flow_submodule_ipv4;
    int define_index = dnx_data_flow_ipv4_define_use_flow_lif_term;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_CUSTOM_FEATURE;
    define->property.doc = 
        "\n"
        "Enable/Disable flow lif usage\n"
        "Default: 0\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_enable;
    define->property.method_str = "suffix_enable";
    define->property.suffix = "use_flow_lif_tunnel_term";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_flow_srv6_use_flow_lif_init_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_flow;
    int submodule_index = dnx_data_flow_submodule_srv6;
    int feature_index = dnx_data_flow_srv6_use_flow_lif_init;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_flow_srv6_use_flow_lif_term_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_flow;
    int submodule_index = dnx_data_flow_submodule_srv6;
    int feature_index = dnx_data_flow_srv6_use_flow_lif_term;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_flow_in_lif_flow_in_use_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_flow;
    int submodule_index = dnx_data_flow_submodule_in_lif;
    int feature_index = dnx_data_flow_in_lif_flow_in_use;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_flow_mpls_ingress_flow_in_use_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_flow;
    int submodule_index = dnx_data_flow_submodule_mpls;
    int feature_index = dnx_data_flow_mpls_ingress_flow_in_use;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_flow_mpls_egress_flow_in_use_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_flow;
    int submodule_index = dnx_data_flow_submodule_mpls;
    int feature_index = dnx_data_flow_mpls_egress_flow_in_use;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_flow_mpls_port_flow_in_use_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_flow;
    int submodule_index = dnx_data_flow_submodule_mpls_port;
    int feature_index = dnx_data_flow_mpls_port_flow_in_use;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_flow_arp_flow_in_use_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_flow;
    int submodule_index = dnx_data_flow_submodule_arp;
    int feature_index = dnx_data_flow_arp_flow_in_use;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_flow_arp_virtual_lif_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_flow;
    int submodule_index = dnx_data_flow_submodule_arp;
    int feature_index = dnx_data_flow_arp_virtual_lif_support;
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
jr2_a0_dnx_data_flow_vlan_port_flow_in_use_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_flow;
    int submodule_index = dnx_data_flow_submodule_vlan_port;
    int feature_index = dnx_data_flow_vlan_port_flow_in_use;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;
    
    feature->property.name = spn_CUSTOM_FEATURE;
    feature->property.doc = 
        "\n"
        "Enable/Disable flow lif usage\n"
        "Default: 0\n"
        "\n"
    ;
    feature->property.method = dnxc_data_property_method_suffix_enable;
    feature->property.method_str = "suffix_enable";
    feature->property.suffix = "vlan_port_flow_in_use";
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &feature->property, -1, &feature->data));
    
    feature->flags |= (DNXC_DATA_F_PROPERTY);


    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}








static shr_error_e
jr2_a0_dnx_data_flow_algo_gpm_flow_gport_in_use_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_flow;
    int submodule_index = dnx_data_flow_submodule_algo_gpm;
    int define_index = dnx_data_flow_algo_gpm_define_flow_gport_in_use;
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
jr2_a0_dnx_data_flow_wide_data_flow_in_use_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_flow;
    int submodule_index = dnx_data_flow_submodule_wide_data;
    int feature_index = dnx_data_flow_wide_data_flow_in_use;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_flow_stat_pp_flow_in_use_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_flow;
    int submodule_index = dnx_data_flow_submodule_stat_pp;
    int feature_index = dnx_data_flow_stat_pp_flow_in_use;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_flow_rch_flow_in_use_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_flow;
    int submodule_index = dnx_data_flow_submodule_rch;
    int feature_index = dnx_data_flow_rch_flow_in_use;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_flow_trap_flow_in_use_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_flow;
    int submodule_index = dnx_data_flow_submodule_trap;
    int feature_index = dnx_data_flow_trap_flow_in_use;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_flow_reflector_flow_in_use_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_flow;
    int submodule_index = dnx_data_flow_submodule_reflector;
    int feature_index = dnx_data_flow_reflector_flow_in_use;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}





shr_error_e
jr2_a0_data_flow_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_flow;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_flow_submodule_general;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_flow_general_is_tests_supported;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_flow_general_is_tests_supported_set;
    data_index = dnx_data_flow_general_is_inlif_profile_dedicated_logic_enabled;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_flow_general_is_inlif_profile_dedicated_logic_enabled_set;

    
    
    submodule_index = dnx_data_flow_submodule_ipv4;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_flow_ipv4_define_use_flow_lif_init;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_flow_ipv4_use_flow_lif_init_set;
    data_index = dnx_data_flow_ipv4_define_use_flow_lif_term;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_flow_ipv4_use_flow_lif_term_set;

    

    
    
    submodule_index = dnx_data_flow_submodule_srv6;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_flow_srv6_use_flow_lif_init;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_flow_srv6_use_flow_lif_init_set;
    data_index = dnx_data_flow_srv6_use_flow_lif_term;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_flow_srv6_use_flow_lif_term_set;

    
    
    submodule_index = dnx_data_flow_submodule_in_lif;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_flow_in_lif_flow_in_use;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_flow_in_lif_flow_in_use_set;

    
    
    submodule_index = dnx_data_flow_submodule_mpls;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_flow_mpls_ingress_flow_in_use;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_flow_mpls_ingress_flow_in_use_set;
    data_index = dnx_data_flow_mpls_egress_flow_in_use;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_flow_mpls_egress_flow_in_use_set;

    
    
    submodule_index = dnx_data_flow_submodule_mpls_port;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_flow_mpls_port_flow_in_use;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_flow_mpls_port_flow_in_use_set;

    
    
    submodule_index = dnx_data_flow_submodule_arp;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_flow_arp_flow_in_use;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_flow_arp_flow_in_use_set;
    data_index = dnx_data_flow_arp_virtual_lif_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_flow_arp_virtual_lif_support_set;

    
    
    submodule_index = dnx_data_flow_submodule_vlan_port;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_flow_vlan_port_flow_in_use;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_flow_vlan_port_flow_in_use_set;

    
    
    submodule_index = dnx_data_flow_submodule_algo_gpm;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_flow_algo_gpm_define_flow_gport_in_use;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_flow_algo_gpm_flow_gport_in_use_set;

    

    
    
    submodule_index = dnx_data_flow_submodule_wide_data;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_flow_wide_data_flow_in_use;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_flow_wide_data_flow_in_use_set;

    
    
    submodule_index = dnx_data_flow_submodule_stat_pp;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_flow_stat_pp_flow_in_use;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_flow_stat_pp_flow_in_use_set;

    
    
    submodule_index = dnx_data_flow_submodule_rch;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_flow_rch_flow_in_use;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_flow_rch_flow_in_use_set;

    
    
    submodule_index = dnx_data_flow_submodule_trap;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_flow_trap_flow_in_use;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_flow_trap_flow_in_use_set;

    
    
    submodule_index = dnx_data_flow_submodule_reflector;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_flow_reflector_flow_in_use;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_flow_reflector_flow_in_use_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

