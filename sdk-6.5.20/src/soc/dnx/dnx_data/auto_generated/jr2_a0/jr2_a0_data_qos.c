

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
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>







static shr_error_e
jr2_a0_dnx_data_qos_qos_qos_var_type_map_enable_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_qos;
    int submodule_index = dnx_data_qos_submodule_qos;
    int feature_index = dnx_data_qos_qos_qos_var_type_map_enable;
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
jr2_a0_dnx_data_qos_qos_nof_ingress_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_qos;
    int submodule_index = dnx_data_qos_submodule_qos;
    int define_index = dnx_data_qos_qos_define_nof_ingress_profiles;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1024;

    
    define->data = 1024;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_qos_qos_nof_ingress_opcode_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_qos;
    int submodule_index = dnx_data_qos_submodule_qos;
    int define_index = dnx_data_qos_qos_define_nof_ingress_opcode_profiles;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 256;

    
    define->data = 256;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_qos_qos_nof_ingress_ecn_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_qos;
    int submodule_index = dnx_data_qos_submodule_qos;
    int define_index = dnx_data_qos_qos_define_nof_ingress_ecn_profiles;
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
jr2_a0_dnx_data_qos_qos_nof_ingress_policer_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_qos;
    int submodule_index = dnx_data_qos_submodule_qos;
    int define_index = dnx_data_qos_qos_define_nof_ingress_policer_profiles;
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
jr2_a0_dnx_data_qos_qos_nof_ingress_vlan_pcp_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_qos;
    int submodule_index = dnx_data_qos_submodule_qos;
    int define_index = dnx_data_qos_qos_define_nof_ingress_vlan_pcp_profiles;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 16;

    
    define->data = 16;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_qos_qos_nof_egress_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_qos;
    int submodule_index = dnx_data_qos_submodule_qos;
    int define_index = dnx_data_qos_qos_define_nof_egress_profiles;
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
jr2_a0_dnx_data_qos_qos_nof_egress_policer_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_qos;
    int submodule_index = dnx_data_qos_submodule_qos;
    int define_index = dnx_data_qos_qos_define_nof_egress_policer_profiles;
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
jr2_a0_dnx_data_qos_qos_nof_egress_mpls_php_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_qos;
    int submodule_index = dnx_data_qos_submodule_qos;
    int define_index = dnx_data_qos_qos_define_nof_egress_mpls_php_profiles;
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
jr2_a0_dnx_data_qos_qos_nof_egress_opcode_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_qos;
    int submodule_index = dnx_data_qos_submodule_qos;
    int define_index = dnx_data_qos_qos_define_nof_egress_opcode_profiles;
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
jr2_a0_dnx_data_qos_qos_nof_egress_php_opcode_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_qos;
    int submodule_index = dnx_data_qos_submodule_qos;
    int define_index = dnx_data_qos_qos_define_nof_egress_php_opcode_profiles;
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
jr2_a0_dnx_data_qos_qos_nof_egress_ttl_pipe_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_qos;
    int submodule_index = dnx_data_qos_submodule_qos;
    int define_index = dnx_data_qos_qos_define_nof_egress_ttl_pipe_profiles;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 16;

    
    define->data = 16;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_qos_qos_nof_ingress_propagation_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_qos;
    int submodule_index = dnx_data_qos_submodule_qos;
    int define_index = dnx_data_qos_qos_define_nof_ingress_propagation_profiles;
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
jr2_a0_dnx_data_qos_qos_nof_ingress_ecn_opcode_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_qos;
    int submodule_index = dnx_data_qos_submodule_qos;
    int define_index = dnx_data_qos_qos_define_nof_ingress_ecn_opcode_profiles;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 16;

    
    define->data = 16;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_qos_qos_nof_egress_network_qos_pipe_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_qos;
    int submodule_index = dnx_data_qos_submodule_qos;
    int define_index = dnx_data_qos_qos_define_nof_egress_network_qos_pipe_profiles;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 64;

    
    define->data = 64;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_qos_qos_nof_egress_cos_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_qos;
    int submodule_index = dnx_data_qos_submodule_qos;
    int define_index = dnx_data_qos_qos_define_nof_egress_cos_profiles;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 16;

    
    define->data = 16;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_qos_qos_packet_max_priority_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_qos;
    int submodule_index = dnx_data_qos_submodule_qos;
    int define_index = dnx_data_qos_qos_define_packet_max_priority;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 7;

    
    define->data = 7;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_qos_qos_packet_max_exp_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_qos;
    int submodule_index = dnx_data_qos_submodule_qos;
    int define_index = dnx_data_qos_qos_define_packet_max_exp;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 7;

    
    define->data = 7;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_qos_qos_packet_max_dscp_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_qos;
    int submodule_index = dnx_data_qos_submodule_qos;
    int define_index = dnx_data_qos_qos_define_packet_max_dscp;
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
jr2_a0_dnx_data_qos_qos_packet_max_cfi_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_qos;
    int submodule_index = dnx_data_qos_submodule_qos;
    int define_index = dnx_data_qos_qos_define_packet_max_cfi;
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
jr2_a0_dnx_data_qos_qos_packet_max_tc_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_qos;
    int submodule_index = dnx_data_qos_submodule_qos;
    int define_index = dnx_data_qos_qos_define_packet_max_tc;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 7;

    
    define->data = 7;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_qos_qos_packet_max_dp_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_qos;
    int submodule_index = dnx_data_qos_submodule_qos;
    int define_index = dnx_data_qos_qos_define_packet_max_dp;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3;

    
    define->data = 3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_qos_qos_packet_max_ecn_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_qos;
    int submodule_index = dnx_data_qos_submodule_qos;
    int define_index = dnx_data_qos_qos_define_packet_max_ecn;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3;

    
    define->data = 3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_qos_qos_packet_max_egress_policer_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_qos;
    int submodule_index = dnx_data_qos_submodule_qos;
    int define_index = dnx_data_qos_qos_define_packet_max_egress_policer_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 7;

    
    define->data = 7;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_qos_qos_max_int_map_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_qos;
    int submodule_index = dnx_data_qos_submodule_qos;
    int define_index = dnx_data_qos_qos_define_max_int_map_id;
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
jr2_a0_dnx_data_qos_qos_max_egress_var_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_qos;
    int submodule_index = dnx_data_qos_submodule_qos;
    int define_index = dnx_data_qos_qos_define_max_egress_var;
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
jr2_a0_dnx_data_qos_qos_ecn_bits_width_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_qos;
    int submodule_index = dnx_data_qos_submodule_qos;
    int define_index = dnx_data_qos_qos_define_ecn_bits_width;
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
jr2_a0_dnx_data_qos_qos_eei_mpls_ttl_profile_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_qos;
    int submodule_index = dnx_data_qos_submodule_qos;
    int define_index = dnx_data_qos_qos_define_eei_mpls_ttl_profile;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 30;

    
    define->data = 30;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_qos_qos_routing_dscp_preserve_mode_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_qos;
    int submodule_index = dnx_data_qos_submodule_qos;
    int define_index = dnx_data_qos_qos_define_routing_dscp_preserve_mode;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_LOGICAL_PORT_ROUTING_PRESERVE_DSCP;
    define->property.doc = 
        "\n"
        "DSCP preserve modes in routing case.\n"
        "1: DSCP preserve per inlif and outlif;\n"
        "2: DSCP preserve per outlif.(Default mode)\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 2;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "1";
    define->property.mapping[0].val = 1;
    define->property.mapping[1].name = "2";
    define->property.mapping[1].val = 2;
    define->property.mapping[1].is_default = 1 ;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_qos_feature_egress_remark_or_keep_enabler_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_qos;
    int submodule_index = dnx_data_qos_submodule_feature;
    int feature_index = dnx_data_qos_feature_egress_remark_or_keep_enabler;
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
jr2_a0_dnx_data_qos_feature_fwd_eve_pcp_diff_encap_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_qos;
    int submodule_index = dnx_data_qos_submodule_feature;
    int feature_index = dnx_data_qos_feature_fwd_eve_pcp_diff_encap;
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
jr2_a0_dnx_data_qos_feature_phb_rm_ffc_extend_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_qos;
    int submodule_index = dnx_data_qos_submodule_feature;
    int feature_index = dnx_data_qos_feature_phb_rm_ffc_extend;
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
jr2_a0_dnx_data_qos_feature_hdr_qual_after_mpls_available_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_qos;
    int submodule_index = dnx_data_qos_submodule_feature;
    int feature_index = dnx_data_qos_feature_hdr_qual_after_mpls_available;
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
jr2_a0_dnx_data_qos_feature_phb_rm_ffc_bits_width_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_qos;
    int submodule_index = dnx_data_qos_submodule_feature;
    int define_index = dnx_data_qos_feature_define_phb_rm_ffc_bits_width;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 21;

    
    define->data = 21;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}




shr_error_e
jr2_a0_data_qos_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_qos;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_qos_submodule_qos;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_qos_qos_define_nof_ingress_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_qos_qos_nof_ingress_profiles_set;
    data_index = dnx_data_qos_qos_define_nof_ingress_opcode_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_qos_qos_nof_ingress_opcode_profiles_set;
    data_index = dnx_data_qos_qos_define_nof_ingress_ecn_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_qos_qos_nof_ingress_ecn_profiles_set;
    data_index = dnx_data_qos_qos_define_nof_ingress_policer_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_qos_qos_nof_ingress_policer_profiles_set;
    data_index = dnx_data_qos_qos_define_nof_ingress_vlan_pcp_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_qos_qos_nof_ingress_vlan_pcp_profiles_set;
    data_index = dnx_data_qos_qos_define_nof_egress_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_qos_qos_nof_egress_profiles_set;
    data_index = dnx_data_qos_qos_define_nof_egress_policer_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_qos_qos_nof_egress_policer_profiles_set;
    data_index = dnx_data_qos_qos_define_nof_egress_mpls_php_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_qos_qos_nof_egress_mpls_php_profiles_set;
    data_index = dnx_data_qos_qos_define_nof_egress_opcode_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_qos_qos_nof_egress_opcode_profiles_set;
    data_index = dnx_data_qos_qos_define_nof_egress_php_opcode_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_qos_qos_nof_egress_php_opcode_profiles_set;
    data_index = dnx_data_qos_qos_define_nof_egress_ttl_pipe_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_qos_qos_nof_egress_ttl_pipe_profiles_set;
    data_index = dnx_data_qos_qos_define_nof_ingress_propagation_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_qos_qos_nof_ingress_propagation_profiles_set;
    data_index = dnx_data_qos_qos_define_nof_ingress_ecn_opcode_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_qos_qos_nof_ingress_ecn_opcode_profiles_set;
    data_index = dnx_data_qos_qos_define_nof_egress_network_qos_pipe_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_qos_qos_nof_egress_network_qos_pipe_profiles_set;
    data_index = dnx_data_qos_qos_define_nof_egress_cos_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_qos_qos_nof_egress_cos_profiles_set;
    data_index = dnx_data_qos_qos_define_packet_max_priority;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_qos_qos_packet_max_priority_set;
    data_index = dnx_data_qos_qos_define_packet_max_exp;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_qos_qos_packet_max_exp_set;
    data_index = dnx_data_qos_qos_define_packet_max_dscp;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_qos_qos_packet_max_dscp_set;
    data_index = dnx_data_qos_qos_define_packet_max_cfi;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_qos_qos_packet_max_cfi_set;
    data_index = dnx_data_qos_qos_define_packet_max_tc;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_qos_qos_packet_max_tc_set;
    data_index = dnx_data_qos_qos_define_packet_max_dp;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_qos_qos_packet_max_dp_set;
    data_index = dnx_data_qos_qos_define_packet_max_ecn;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_qos_qos_packet_max_ecn_set;
    data_index = dnx_data_qos_qos_define_packet_max_egress_policer_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_qos_qos_packet_max_egress_policer_offset_set;
    data_index = dnx_data_qos_qos_define_max_int_map_id;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_qos_qos_max_int_map_id_set;
    data_index = dnx_data_qos_qos_define_max_egress_var;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_qos_qos_max_egress_var_set;
    data_index = dnx_data_qos_qos_define_ecn_bits_width;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_qos_qos_ecn_bits_width_set;
    data_index = dnx_data_qos_qos_define_eei_mpls_ttl_profile;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_qos_qos_eei_mpls_ttl_profile_set;
    data_index = dnx_data_qos_qos_define_routing_dscp_preserve_mode;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_qos_qos_routing_dscp_preserve_mode_set;

    
    data_index = dnx_data_qos_qos_qos_var_type_map_enable;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_qos_qos_qos_var_type_map_enable_set;

    
    
    submodule_index = dnx_data_qos_submodule_feature;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_qos_feature_define_phb_rm_ffc_bits_width;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_qos_feature_phb_rm_ffc_bits_width_set;

    
    data_index = dnx_data_qos_feature_egress_remark_or_keep_enabler;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_qos_feature_egress_remark_or_keep_enabler_set;
    data_index = dnx_data_qos_feature_fwd_eve_pcp_diff_encap;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_qos_feature_fwd_eve_pcp_diff_encap_set;
    data_index = dnx_data_qos_feature_phb_rm_ffc_extend;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_qos_feature_phb_rm_ffc_extend_set;
    data_index = dnx_data_qos_feature_hdr_qual_after_mpls_available;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_qos_feature_hdr_qual_after_mpls_available_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

