

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_ELKDNX

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_elk.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>
#include <soc/dnxc/dnxc_port.h>
#include <soc/kbp/alg_kbp/include/init.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_tables.h>







static shr_error_e
jr2_a0_dnx_data_elk_general_is_egw_support_rr_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_elk;
    int submodule_index = dnx_data_elk_submodule_general;
    int feature_index = dnx_data_elk_general_is_egw_support_rr;
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
jr2_a0_dnx_data_elk_general_is_kbp_traffic_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_elk;
    int submodule_index = dnx_data_elk_submodule_general;
    int feature_index = dnx_data_elk_general_is_kbp_traffic_supported;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = !dnx_data_elk.connectivity.blackhole_get(unit);

    
    feature->data = !dnx_data_elk.connectivity.blackhole_get(unit);

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_elk_general_max_ports_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_elk;
    int submodule_index = dnx_data_elk_submodule_general;
    int define_index = dnx_data_elk_general_define_max_ports;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_elk_general_rop_fifo_depth_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_elk;
    int submodule_index = dnx_data_elk_submodule_general;
    int define_index = dnx_data_elk_general_define_rop_fifo_depth;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 512;

    
    define->data = 512;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_elk_general_flp_full_treshold_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_elk;
    int submodule_index = dnx_data_elk_submodule_general;
    int define_index = dnx_data_elk_general_define_flp_full_treshold;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2000;

    
    define->data = 2000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_elk_general_max_lane_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_elk;
    int submodule_index = dnx_data_elk_submodule_general;
    int define_index = dnx_data_elk_general_define_max_lane_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32;

    
    define->data = 32;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_elk_general_egw_max_opcode_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_elk;
    int submodule_index = dnx_data_elk_submodule_general;
    int define_index = dnx_data_elk_general_define_egw_max_opcode;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 255;

    
    define->data = 255;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_elk_general_max_kbp_instance_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_elk;
    int submodule_index = dnx_data_elk_submodule_general;
    int define_index = dnx_data_elk_general_define_max_kbp_instance;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_elk_general_lookup_timeout_delay_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_elk;
    int submodule_index = dnx_data_elk_submodule_general;
    int define_index = dnx_data_elk_general_define_lookup_timeout_delay;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0xa28;

    
    define->data = 0xa28;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_elk_general_trans_timer_delay_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_elk;
    int submodule_index = dnx_data_elk_submodule_general;
    int define_index = dnx_data_elk_general_define_trans_timer_delay;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x13;

    
    define->data = 0x13;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_elk_general_device_type_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_elk;
    int submodule_index = dnx_data_elk_submodule_general;
    int define_index = dnx_data_elk_general_define_device_type;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_EXT_TCAM_DEV_TYPE;
    define->property.doc = 
        "\n"
        "Specify the external Tcam type.\n"
        "Jericho2 support BCM52311 and beyond.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 2;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "BCM52311";
    define->property.mapping[0].val = 1;
    define->property.mapping[1].name = "BCM52321";
    define->property.mapping[1].val = 2;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_elk_application_ipv4_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_elk;
    int submodule_index = dnx_data_elk_submodule_application;
    int feature_index = dnx_data_elk_application_ipv4;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;
    
    feature->property.name = spn_EXT_IPV4_FWD_ENABLE;
    feature->property.doc = 
        "\n"
        "Enable / Disable KBP IPv4 forwarding application.\n"
        "\n"
    ;
    feature->property.method = dnxc_data_property_method_enable;
    feature->property.method_str = "enable";
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &feature->property, -1, &feature->data));
    
    feature->flags |= (DNXC_DATA_F_PROPERTY);


    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_elk_application_ipv6_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_elk;
    int submodule_index = dnx_data_elk_submodule_application;
    int feature_index = dnx_data_elk_application_ipv6;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;
    
    feature->property.name = spn_EXT_IPV6_FWD_ENABLE;
    feature->property.doc = 
        "\n"
        "Enable / Disable KBP IPv6 forwarding application.\n"
        "\n"
    ;
    feature->property.method = dnxc_data_property_method_enable;
    feature->property.method_str = "enable";
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &feature->property, -1, &feature->data));
    
    feature->flags |= (DNXC_DATA_F_PROPERTY);


    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_elk_application_nat_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_elk;
    int submodule_index = dnx_data_elk_submodule_application;
    int feature_index = dnx_data_elk_application_nat;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;
    
    feature->property.name = "custom_feature";
    feature->property.doc = 
        "\n"
        "Enable / Disable KBP NAT forwarding application.\n"
        "\n"
    ;
    feature->property.method = dnxc_data_property_method_suffix_enable;
    feature->property.method_str = "suffix_enable";
    feature->property.suffix = "ext_nat_fwd_enable";
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &feature->property, -1, &feature->data));
    
    feature->flags |= (DNXC_DATA_F_PROPERTY);


    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_elk_application_sllb_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_elk;
    int submodule_index = dnx_data_elk_submodule_application;
    int feature_index = dnx_data_elk_application_sllb;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;
    
    feature->property.name = spn_EXT_SLLB_FWD_ENABLE;
    feature->property.doc = 
        "\n"
        "Enable / Disable KBP SLLB forwarding application.\n"
        "\n"
    ;
    feature->property.method = dnxc_data_property_method_enable;
    feature->property.method_str = "enable";
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &feature->property, -1, &feature->data));
    
    feature->flags |= (DNXC_DATA_F_PROPERTY);


    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_elk_application_optimized_result_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_elk;
    int submodule_index = dnx_data_elk_submodule_application;
    int define_index = dnx_data_elk_application_define_optimized_result;
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
jr2_a0_dnx_data_elk_application_split_rpf_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_elk;
    int submodule_index = dnx_data_elk_submodule_application;
    int define_index = dnx_data_elk_application_define_split_rpf;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_CUSTOM_FEATURE;
    define->property.doc = 
        "\n"
        "Enable / Disable KBP split RPF.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_enable;
    define->property.method_str = "suffix_enable";
    define->property.suffix = "kbp_split_rpf_enable";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_elk_application_thread_safety_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_elk;
    int submodule_index = dnx_data_elk_submodule_application;
    int define_index = dnx_data_elk_application_define_thread_safety;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_EXT_TCAM_THREAD_SAFETY_ENABLE;
    define->property.doc = 
        "\n"
        "Enable / Disable KBP thread safety.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_enable;
    define->property.method_str = "enable";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_elk_application_ipv4_large_mc_enable_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_elk;
    int submodule_index = dnx_data_elk_submodule_application;
    int define_index = dnx_data_elk_application_define_ipv4_large_mc_enable;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_IPV4_LARGE_MC_ENABLE;
    define->property.doc = 
        "\n"
        "Enable / Disable IPv4 large Multicast modes.\n"
        "Mode 0: Default mode. Disables IPv4 large multicast.\n"
        "Mode 1: Supports higher capacity by enabling ACL algo mode 3.\n"
        "And this modes assumes VRF and DIP are fully specified.\n"
        "Mode 2: Supports higher capacity by enabling ACL algo mode 3.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 2;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_elk_application_ipv6_large_mc_enable_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_elk;
    int submodule_index = dnx_data_elk_submodule_application;
    int define_index = dnx_data_elk_application_define_ipv6_large_mc_enable;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_IPV6_LARGE_MC_ENABLE;
    define->property.doc = 
        "\n"
        "Enable / Disable IPv6 large Multicast modes.\n"
        "Mode 0: Default mode. Disables IPv6 large multicast.\n"
        "Mode 1: Supports higher capacity by enabling ACL algo mode 3.\n"
        "And this modes assumes VRF and DIP are fully specified.\n"
        "Mode 2: Supports higher capacity by enabling ACL algo mode 3.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 2;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_elk_application_ipv4_mc_hash_table_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_elk;
    int submodule_index = dnx_data_elk_submodule_application;
    int define_index = dnx_data_elk_application_define_ipv4_mc_hash_table_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 131072;

    
    define->data = 131072;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_elk_application_ipv6_mc_hash_table_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_elk;
    int submodule_index = dnx_data_elk_submodule_application;
    int define_index = dnx_data_elk_application_define_ipv6_mc_hash_table_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 81920;

    
    define->data = 81920;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_elk_application_ipv4_mc_table_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_elk;
    int submodule_index = dnx_data_elk_submodule_application;
    int define_index = dnx_data_elk_application_define_ipv4_mc_table_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_elk.application.ipv4_large_mc_enable_get(unit) == 0 ? 1024 : 32768;

    
    define->data = dnx_data_elk.application.ipv4_large_mc_enable_get(unit) == 0 ? 1024 : 32768;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_elk_application_ipv6_mc_table_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_elk;
    int submodule_index = dnx_data_elk_submodule_application;
    int define_index = dnx_data_elk_application_define_ipv6_mc_table_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_elk.application.ipv6_large_mc_enable_get(unit) == 0 ? 1024 : 32768;

    
    define->data = dnx_data_elk.application.ipv6_large_mc_enable_get(unit) == 0 ? 1024 : 32768;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_elk_application_db_property_key_map(
    int unit,
    int key0_val,
    int key1_val,
    int *key0_index,
    int *key1_index)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (key0_val)
    {
        case DBAL_TABLE_KBP_IPV4_MULTICAST_TCAM_FORWARD:
            *key0_index = 0;
            break;
        case DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_RPF:
            *key0_index = 1;
            break;
        case DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD:
            *key0_index = 2;
            break;
        case DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_RPF:
            *key0_index = 3;
            break;
        case DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD:
            *key0_index = 4;
            break;
        case DBAL_TABLE_KBP_IPV6_MULTICAST_TCAM_FORWARD:
            *key0_index = 5;
            break;
        default:
            *key0_index = -1;
            break;
    }

    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_elk_application_db_property_key_reverse_map(
    int unit,
    int key0_index,
    int key1_index,
    int *key0_val,
    int *key1_val)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (key0_index)
    {
        case 0:
            *key0_val = DBAL_TABLE_KBP_IPV4_MULTICAST_TCAM_FORWARD;
            break;
        case 1:
            *key0_val = DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_RPF;
            break;
        case 2:
            *key0_val = DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD;
            break;
        case 3:
            *key0_val = DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_RPF;
            break;
        case 4:
            *key0_val = DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD;
            break;
        case 5:
            *key0_val = DBAL_TABLE_KBP_IPV6_MULTICAST_TCAM_FORWARD;
            break;
        default:
            *key0_val = -1;
            break;
    }

    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_elk_application_db_property_set(
    int unit)
{
    int db_index;
    dnx_data_elk_application_db_property_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_elk;
    int submodule_index = dnx_data_elk_submodule_application;
    int table_index = dnx_data_elk_application_table_db_property;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 6;
    table->info_get.key_size[0] = 6;

    
    table->values[0].default_val = "1024";
    table->values[1].default_val = "1024";
    table->values[2].default_val = "0";
    table->values[3].default_val = "0";
    table->values[4].default_val = "0";
    table->values[5].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_elk_application_db_property_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_elk_application_table_db_property");

    
    default_data = (dnx_data_elk_application_db_property_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->initial_regular_capacity = 1024;
    default_data->initial_optimized_capacity = 1024;
    default_data->xl_db = 0;
    default_data->locality = 0;
    default_data->zero_size_ad = 0;
    default_data->dbal_none_direct_optimized = 0;
    
    for (db_index = 0; db_index < table->keys[0].size; db_index++)
    {
        data = (dnx_data_elk_application_db_property_t *) dnxc_data_mgmt_table_data_get(unit, table, db_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    table->key_map = jr2_a0_dnx_data_elk_application_db_property_key_map;
    table->key_map_reverse = jr2_a0_dnx_data_elk_application_db_property_key_reverse_map;
    
    data = (dnx_data_elk_application_db_property_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD, 0);
    data->initial_regular_capacity = 32768;
    data->initial_optimized_capacity = 524288;
    data->xl_db = 1;
    data->locality = 0;
    data->zero_size_ad = 0;
    data->dbal_none_direct_optimized = 1;
    data = (dnx_data_elk_application_db_property_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD, 0);
    data->initial_regular_capacity = 32768;
    data->initial_optimized_capacity = 524288;
    data->xl_db = 1;
    data->locality = 0;
    data->zero_size_ad = 0;
    data->dbal_none_direct_optimized = 1;
    data = (dnx_data_elk_application_db_property_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_RPF, 0);
    data->initial_regular_capacity = 524288;
    data->initial_optimized_capacity = 0;
    data->xl_db = 0;
    data->locality = 2;
    data->zero_size_ad = 1;
    data->dbal_none_direct_optimized = 1;
    data = (dnx_data_elk_application_db_property_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_RPF, 0);
    data->initial_regular_capacity = 131072;
    data->initial_optimized_capacity = 0;
    data->xl_db = 0;
    data->locality = 2;
    data->zero_size_ad = 1;
    data->dbal_none_direct_optimized = 1;
    data = (dnx_data_elk_application_db_property_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_TABLE_KBP_IPV4_MULTICAST_TCAM_FORWARD, 0);
    data->initial_regular_capacity = dnx_data_elk.application.ipv4_mc_table_size_get(unit);
    data->initial_optimized_capacity = 0;
    data->xl_db = 0;
    data->locality = 0;
    data->zero_size_ad = 0;
    data->dbal_none_direct_optimized = 0;
    data = (dnx_data_elk_application_db_property_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_TABLE_KBP_IPV6_MULTICAST_TCAM_FORWARD, 0);
    data->initial_regular_capacity = dnx_data_elk.application.ipv6_mc_table_size_get(unit);
    data->initial_optimized_capacity = 0;
    data->xl_db = 0;
    data->locality = 0;
    data->zero_size_ad = 0;
    data->dbal_none_direct_optimized = 0;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_elk_connectivity_connect_mode_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_elk;
    int submodule_index = dnx_data_elk_submodule_connectivity;
    int define_index = dnx_data_elk_connectivity_define_connect_mode;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_EXT_TCAM_CONNECT_MODE;
    define->property.doc = 
        "\n"
        "External Tcam connection mode.\n"
        "SINGLE: One host connects with one KBP;\n"
        "DUAL_SHARED: Two hosts connect with one KBP via one thread;\n"
        "DUAL_SMT: Two hosts connect with one KBP via two threads.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 5;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "SINGLE";
    define->property.mapping[0].val = 0;
    define->property.mapping[0].is_default = 1 ;
    define->property.mapping[1].name = "SINGLE_HOST";
    define->property.mapping[1].val = 0;
    define->property.mapping[1].is_default = 1 ;
    define->property.mapping[2].name = "DUAL_SHARED";
    define->property.mapping[2].val = 1;
    define->property.mapping[3].name = "DUAL_SMT";
    define->property.mapping[3].val = 2;
    define->property.mapping[4].name = "SINGLE_PORT";
    define->property.mapping[4].val = 3;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_elk_connectivity_blackhole_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_elk;
    int submodule_index = dnx_data_elk_submodule_connectivity;
    int define_index = dnx_data_elk_connectivity_define_blackhole;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_CUSTOM_FEATURE;
    define->property.doc = 
        "\n"
        "Enable / Disable KBP Blackhole mode:\n"
        "0: disabled\n"
        "1: enabled - initialize 1 KBP instance\n"
        "2: enabled - initialize 2 KBP instances\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_range;
    define->property.method_str = "suffix_range";
    define->property.suffix = "kbp_blackhole";
    define->property.range_min = 0;
    define->property.range_max = 2;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_elk_connectivity_mdio_set(
    int unit)
{
    int kbp_inst_index;
    dnx_data_elk_connectivity_mdio_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_elk;
    int submodule_index = dnx_data_elk_submodule_connectivity;
    int table_index = dnx_data_elk_connectivity_table_mdio;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_elk.general.max_kbp_instance_get(unit);
    table->info_get.key_size[0] = dnx_data_elk.general.max_kbp_instance_get(unit);

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_elk_connectivity_mdio_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_elk_connectivity_table_mdio");

    
    default_data = (dnx_data_elk_connectivity_mdio_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->mdio_id = 0;
    
    for (kbp_inst_index = 0; kbp_inst_index < table->keys[0].size; kbp_inst_index++)
    {
        data = (dnx_data_elk_connectivity_mdio_t *) dnxc_data_mgmt_table_data_get(unit, table, kbp_inst_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->values[0].property.name = spn_PORT_PHY_ADDR;
    table->values[0].property.doc =
        "\n"
        "The MDIO ID connecting with the External Tcam.\n"
        "KBP mdio ID format:\n"
        "* bits [5:6,8:9] - bus ID.\n"
        "* bit [7] - Internal select. Set to 0 for external phy access.\n"
        "* bits [0:4] - phy/kbp id ==> 0x1\n"
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_suffix_range;
    table->values[0].property.method_str = "suffix_range";
    table->values[0].property.suffix = "ext_tcam";
    table->values[0].property.range_min = 0;
    table->values[0].property.range_max = 0xFFFFFFFF;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (kbp_inst_index = 0; kbp_inst_index < table->keys[0].size; kbp_inst_index++)
    {
        data = (dnx_data_elk_connectivity_mdio_t *) dnxc_data_mgmt_table_data_get(unit, table, kbp_inst_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[0].property, kbp_inst_index, &data->mdio_id));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_elk_connectivity_ilkn_reverse_set(
    int unit)
{
    int ilkn_id_index;
    dnx_data_elk_connectivity_ilkn_reverse_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_elk;
    int submodule_index = dnx_data_elk_submodule_connectivity;
    int table_index = dnx_data_elk_connectivity_table_ilkn_reverse;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 16;
    table->info_get.key_size[0] = 16;

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_elk_connectivity_ilkn_reverse_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_elk_connectivity_table_ilkn_reverse");

    
    default_data = (dnx_data_elk_connectivity_ilkn_reverse_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->reverse = 0;
    
    for (ilkn_id_index = 0; ilkn_id_index < table->keys[0].size; ilkn_id_index++)
    {
        data = (dnx_data_elk_connectivity_ilkn_reverse_t *) dnxc_data_mgmt_table_data_get(unit, table, ilkn_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->values[0].property.name = spn_EXT_ILKN_REVERSE;
    table->values[0].property.doc =
        "\n"
        "External lookup (elk) ILKN lanes swap.\n"
        "If set, reverse the lanes numbering order on elk device side.\n"
        "This swap is logical and relates to ILKN protocol lane numbering.\n"
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_suffix_enable;
    table->values[0].property.method_str = "suffix_enable";
    table->values[0].property.suffix = "";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (ilkn_id_index = 0; ilkn_id_index < table->keys[0].size; ilkn_id_index++)
    {
        data = (dnx_data_elk_connectivity_ilkn_reverse_t *) dnxc_data_mgmt_table_data_get(unit, table, ilkn_id_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[0].property, ilkn_id_index, &data->reverse));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_elk_connectivity_topology_set(
    int unit)
{
    dnx_data_elk_connectivity_topology_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_elk;
    int submodule_index = dnx_data_elk_submodule_connectivity;
    int table_index = dnx_data_elk_connectivity_table_topology;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "0, 0, 0, 0";
    table->values[1].default_val = "0, 0, 0, 0";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_elk_connectivity_topology_t, (1 + 1  ), "data of dnx_data_elk_connectivity_table_topology");

    
    default_data = (dnx_data_elk_connectivity_topology_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_ELK_GENERAL_MAX_PORTS, _SHR_E_INTERNAL, "out of bound access to array")
    default_data->start_lane[0] = 0;
    default_data->start_lane[1] = 0;
    default_data->start_lane[2] = 0;
    default_data->start_lane[3] = 0;
    SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_ELK_GENERAL_MAX_PORTS, _SHR_E_INTERNAL, "out of bound access to array")
    default_data->port_core_mapping[0] = 0;
    default_data->port_core_mapping[1] = 0;
    default_data->port_core_mapping[2] = 0;
    default_data->port_core_mapping[3] = 0;
    
    data = (dnx_data_elk_connectivity_topology_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);
    
    table->property.name = "ext_tcam_start_lane";
    table->property.doc =
        "\n"
        "1. Define the start lane number for each kbp port.\n"
        "2. Define the KBP port to Jericho2 core mapping.\n"
        "\n"
    ;
    table->property.method = dnxc_data_property_method_custom;
    table->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    data = (dnx_data_elk_connectivity_topology_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnx_data_property_elk_connectivity_topology_read(unit, data));
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_elk_connectivity_kbp_serdes_tx_taps_set(
    int unit)
{
    int kbp_inst_index;
    dnx_data_elk_connectivity_kbp_serdes_tx_taps_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_elk;
    int submodule_index = dnx_data_elk_submodule_connectivity;
    int table_index = dnx_data_elk_connectivity_table_kbp_serdes_tx_taps;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_elk.general.max_kbp_instance_get(unit);
    table->info_get.key_size[0] = dnx_data_elk.general.max_kbp_instance_get(unit);

    
    table->values[0].default_val = "-14";
    table->values[1].default_val = "81";
    table->values[2].default_val = "-28";
    table->values[3].default_val = "4";
    table->values[4].default_val = "0";
    table->values[5].default_val = "0";
    table->values[6].default_val = "NRZ_6TAP";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_elk_connectivity_kbp_serdes_tx_taps_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_elk_connectivity_table_kbp_serdes_tx_taps");

    
    default_data = (dnx_data_elk_connectivity_kbp_serdes_tx_taps_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->pre = -14;
    default_data->main = 81;
    default_data->post = -28;
    default_data->pre2 = 4;
    default_data->post2 = 0;
    default_data->post3 = 0;
    default_data->txfir_tap_enable = NRZ_6TAP;
    
    for (kbp_inst_index = 0; kbp_inst_index < table->keys[0].size; kbp_inst_index++)
    {
        data = (dnx_data_elk_connectivity_kbp_serdes_tx_taps_t *) dnxc_data_mgmt_table_data_get(unit, table, kbp_inst_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->property.name = "ext_tcam_serdes_tx_taps";
    table->property.doc =
        "\n"
        "a set of TX FIR parameters for the lane.\n"
        "serdes_tx_taps_<lane_id>=signalling_mode:pre:main:post:pre2:post2:post3\n"
        "signalling_mode can be either pam4 or nrz.\n"
        "[pre2:post2:post3] part is optional:\n"
        "- if it is given - this is 6-tap mode.\n"
        "- if it is not given - this is 3-tap mode.\n"
        "\n"
    ;
    table->property.method = dnxc_data_property_method_custom;
    table->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (kbp_inst_index = 0; kbp_inst_index < table->keys[0].size; kbp_inst_index++)
    {
        data = (dnx_data_elk_connectivity_kbp_serdes_tx_taps_t *) dnxc_data_mgmt_table_data_get(unit, table, kbp_inst_index, 0);
        SHR_IF_ERR_EXIT(dnx_data_property_elk_connectivity_kbp_serdes_tx_taps_read(unit, kbp_inst_index, data));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_elk_connectivity_kbp_lane_serdes_tx_taps_set(
    int unit)
{
    int lane_id_index;
    dnx_data_elk_connectivity_kbp_lane_serdes_tx_taps_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_elk;
    int submodule_index = dnx_data_elk_submodule_connectivity;
    int table_index = dnx_data_elk_connectivity_table_kbp_lane_serdes_tx_taps;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_elk.general.max_lane_id_get(unit) * dnx_data_elk.general.max_kbp_instance_get(unit);
    table->info_get.key_size[0] = dnx_data_elk.general.max_lane_id_get(unit) * dnx_data_elk.general.max_kbp_instance_get(unit);

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "DNXC_PORT_TX_FIR_INVALID_MAIN_TAP";
    table->values[2].default_val = "0";
    table->values[3].default_val = "0";
    table->values[4].default_val = "0";
    table->values[5].default_val = "0";
    table->values[6].default_val = "1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_elk_connectivity_kbp_lane_serdes_tx_taps_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_elk_connectivity_table_kbp_lane_serdes_tx_taps");

    
    default_data = (dnx_data_elk_connectivity_kbp_lane_serdes_tx_taps_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->pre = 0;
    default_data->main = DNXC_PORT_TX_FIR_INVALID_MAIN_TAP;
    default_data->post = 0;
    default_data->pre2 = 0;
    default_data->post2 = 0;
    default_data->post3 = 0;
    default_data->txfir_tap_enable = 1;
    
    for (lane_id_index = 0; lane_id_index < table->keys[0].size; lane_id_index++)
    {
        data = (dnx_data_elk_connectivity_kbp_lane_serdes_tx_taps_t *) dnxc_data_mgmt_table_data_get(unit, table, lane_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->property.name = "serdes_tx_taps";
    table->property.doc =
        "\n"
        "a set of TX FIR parameters for the external interface kbp device\n"
        "It is configured per stat lane, as follow:\n"
        "serdes_tx_taps_ext_stat_lane<lane_id>=signalling_mode:pre:main:post:pre2:post2:post3\n"
        "signalling_mode can be either pam4 or nrz.\n"
        "[pre2:post2:post3] are mandatory. (Can be set to zero).\n"
        "main is set to DNXC_PORT_TX_FIR_INVALID_MAIN_TAP by default to distinguish between whether the SoC property was set or not.\n"
        "\n"
    ;
    table->property.method = dnxc_data_property_method_custom;
    table->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (lane_id_index = 0; lane_id_index < table->keys[0].size; lane_id_index++)
    {
        data = (dnx_data_elk_connectivity_kbp_lane_serdes_tx_taps_t *) dnxc_data_mgmt_table_data_get(unit, table, lane_id_index, 0);
        SHR_IF_ERR_EXIT(dnx_data_property_elk_connectivity_kbp_lane_serdes_tx_taps_read(unit, lane_id_index, data));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_elk_recovery_enable_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_elk;
    int submodule_index = dnx_data_elk_submodule_recovery;
    int define_index = dnx_data_elk_recovery_define_enable;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_CUSTOM_FEATURE;
    define->property.doc = 
        "\n"
        "1: Enable the recovery sequence.\n"
        "0: Disable the recovery sequence.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_enable;
    define->property.method_str = "suffix_enable";
    define->property.suffix = "kbp_recovery_enable";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_elk_recovery_iteration_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_elk;
    int submodule_index = dnx_data_elk_submodule_recovery;
    int define_index = dnx_data_elk_recovery_define_iteration;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 5;

    
    define->data = 5;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_CUSTOM_FEATURE;
    define->property.doc = 
        "\n"
        "The re-try iteration while executing the recovery sequence.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_range;
    define->property.method_str = "suffix_range";
    define->property.suffix = "kbp_recovery_iter";
    define->property.range_min = 0;
    define->property.range_max = 0xFFFFFFFF;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}




shr_error_e
jr2_a0_data_elk_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_elk;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_elk_submodule_general;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_elk_general_define_max_ports;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_elk_general_max_ports_set;
    data_index = dnx_data_elk_general_define_rop_fifo_depth;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_elk_general_rop_fifo_depth_set;
    data_index = dnx_data_elk_general_define_flp_full_treshold;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_elk_general_flp_full_treshold_set;
    data_index = dnx_data_elk_general_define_max_lane_id;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_elk_general_max_lane_id_set;
    data_index = dnx_data_elk_general_define_egw_max_opcode;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_elk_general_egw_max_opcode_set;
    data_index = dnx_data_elk_general_define_max_kbp_instance;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_elk_general_max_kbp_instance_set;
    data_index = dnx_data_elk_general_define_lookup_timeout_delay;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_elk_general_lookup_timeout_delay_set;
    data_index = dnx_data_elk_general_define_trans_timer_delay;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_elk_general_trans_timer_delay_set;
    data_index = dnx_data_elk_general_define_device_type;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_elk_general_device_type_set;

    
    data_index = dnx_data_elk_general_is_egw_support_rr;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_elk_general_is_egw_support_rr_set;
    data_index = dnx_data_elk_general_is_kbp_traffic_supported;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_elk_general_is_kbp_traffic_supported_set;

    
    
    submodule_index = dnx_data_elk_submodule_application;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_elk_application_define_optimized_result;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_elk_application_optimized_result_set;
    data_index = dnx_data_elk_application_define_split_rpf;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_elk_application_split_rpf_set;
    data_index = dnx_data_elk_application_define_thread_safety;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_elk_application_thread_safety_set;
    data_index = dnx_data_elk_application_define_ipv4_large_mc_enable;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_elk_application_ipv4_large_mc_enable_set;
    data_index = dnx_data_elk_application_define_ipv6_large_mc_enable;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_elk_application_ipv6_large_mc_enable_set;
    data_index = dnx_data_elk_application_define_ipv4_mc_hash_table_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_elk_application_ipv4_mc_hash_table_size_set;
    data_index = dnx_data_elk_application_define_ipv6_mc_hash_table_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_elk_application_ipv6_mc_hash_table_size_set;
    data_index = dnx_data_elk_application_define_ipv4_mc_table_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_elk_application_ipv4_mc_table_size_set;
    data_index = dnx_data_elk_application_define_ipv6_mc_table_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_elk_application_ipv6_mc_table_size_set;

    
    data_index = dnx_data_elk_application_ipv4;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_elk_application_ipv4_set;
    data_index = dnx_data_elk_application_ipv6;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_elk_application_ipv6_set;
    data_index = dnx_data_elk_application_nat;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_elk_application_nat_set;
    data_index = dnx_data_elk_application_sllb;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_elk_application_sllb_set;

    
    data_index = dnx_data_elk_application_table_db_property;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_elk_application_db_property_set;
    
    submodule_index = dnx_data_elk_submodule_connectivity;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_elk_connectivity_define_connect_mode;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_elk_connectivity_connect_mode_set;
    data_index = dnx_data_elk_connectivity_define_blackhole;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_elk_connectivity_blackhole_set;

    

    
    data_index = dnx_data_elk_connectivity_table_mdio;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_elk_connectivity_mdio_set;
    data_index = dnx_data_elk_connectivity_table_ilkn_reverse;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_elk_connectivity_ilkn_reverse_set;
    data_index = dnx_data_elk_connectivity_table_topology;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_elk_connectivity_topology_set;
    data_index = dnx_data_elk_connectivity_table_kbp_serdes_tx_taps;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_elk_connectivity_kbp_serdes_tx_taps_set;
    data_index = dnx_data_elk_connectivity_table_kbp_lane_serdes_tx_taps;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_elk_connectivity_kbp_lane_serdes_tx_taps_set;
    
    submodule_index = dnx_data_elk_submodule_recovery;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_elk_recovery_define_enable;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_elk_recovery_enable_set;
    data_index = dnx_data_elk_recovery_define_iteration;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_elk_recovery_iteration_set;

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

