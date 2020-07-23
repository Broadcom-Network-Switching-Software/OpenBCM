

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_ESEM

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_esem.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>








static shr_error_e
jr2_a0_dnx_data_esem_access_cmd_nof_esem_accesses_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_esem;
    int submodule_index = dnx_data_esem_submodule_access_cmd;
    int define_index = dnx_data_esem_access_cmd_define_nof_esem_accesses;
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
jr2_a0_dnx_data_esem_access_cmd_app_di_id_size_in_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_esem;
    int submodule_index = dnx_data_esem_submodule_access_cmd;
    int define_index = dnx_data_esem_access_cmd_define_app_di_id_size_in_bits;
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
jr2_a0_dnx_data_esem_access_cmd_no_action_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_esem;
    int submodule_index = dnx_data_esem_submodule_access_cmd;
    int define_index = dnx_data_esem_access_cmd_define_no_action;
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
jr2_a0_dnx_data_esem_access_cmd_default_native_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_esem;
    int submodule_index = dnx_data_esem_submodule_access_cmd;
    int define_index = dnx_data_esem_access_cmd_define_default_native;
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
jr2_a0_dnx_data_esem_access_cmd_default_ac_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_esem;
    int submodule_index = dnx_data_esem_submodule_access_cmd;
    int define_index = dnx_data_esem_access_cmd_define_default_ac;
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
jr2_a0_dnx_data_esem_access_cmd_sflow_sample_interface_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_esem;
    int submodule_index = dnx_data_esem_submodule_access_cmd;
    int define_index = dnx_data_esem_access_cmd_define_sflow_sample_interface;
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
jr2_a0_dnx_data_esem_access_cmd_ip_tunnel_no_action_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_esem;
    int submodule_index = dnx_data_esem_submodule_access_cmd;
    int define_index = dnx_data_esem_access_cmd_define_ip_tunnel_no_action;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 60;

    
    define->data = 60;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_esem_access_cmd_nof_esem_cmd_default_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_esem;
    int submodule_index = dnx_data_esem_submodule_access_cmd;
    int define_index = dnx_data_esem_access_cmd_define_nof_esem_cmd_default_profiles;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 5;

    
    define->data = 5;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_esem_access_cmd_nof_cmds_size_in_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_esem;
    int submodule_index = dnx_data_esem_submodule_access_cmd;
    int define_index = dnx_data_esem_access_cmd_define_nof_cmds_size_in_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 6;

    
    define->data = 6;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_esem_access_cmd_nof_cmds_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_esem;
    int submodule_index = dnx_data_esem_submodule_access_cmd;
    int define_index = dnx_data_esem_access_cmd_define_nof_cmds;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1 << dnx_data_esem.access_cmd.nof_cmds_size_in_bits_get(unit);

    
    define->data = 1 << dnx_data_esem.access_cmd.nof_cmds_size_in_bits_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_esem_access_cmd_min_size_for_esem_cmd_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_esem;
    int submodule_index = dnx_data_esem_submodule_access_cmd;
    int define_index = dnx_data_esem_access_cmd_define_min_size_for_esem_cmd;
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
jr2_a0_dnx_data_esem_access_cmd_esem_cmd_arr_prefix_gre_any_ipv4_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_esem;
    int submodule_index = dnx_data_esem_submodule_access_cmd;
    int define_index = dnx_data_esem_access_cmd_define_esem_cmd_arr_prefix_gre_any_ipv4;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x30;

    
    define->data = 0x30;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_esem_access_cmd_esem_cmd_arr_prefix_vxlan_gpe_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_esem;
    int submodule_index = dnx_data_esem_submodule_access_cmd;
    int define_index = dnx_data_esem_access_cmd_define_esem_cmd_arr_prefix_vxlan_gpe;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x34;

    
    define->data = 0x34;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_esem_access_cmd_esem_cmd_arr_prefix_ip_tunnel_no_action_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_esem;
    int submodule_index = dnx_data_esem_submodule_access_cmd;
    int define_index = dnx_data_esem_access_cmd_define_esem_cmd_arr_prefix_ip_tunnel_no_action;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x3C;

    
    define->data = 0x3C;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_esem_default_result_profile_default_native_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_esem;
    int submodule_index = dnx_data_esem_submodule_default_result_profile;
    int define_index = dnx_data_esem_default_result_profile_define_default_native;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 12;

    
    define->data = 12;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_esem_default_result_profile_default_ac_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_esem;
    int submodule_index = dnx_data_esem_submodule_default_result_profile;
    int define_index = dnx_data_esem_default_result_profile_define_default_ac;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 13;

    
    define->data = 13;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_esem_default_result_profile_sflow_sample_interface_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_esem;
    int submodule_index = dnx_data_esem_submodule_default_result_profile;
    int define_index = dnx_data_esem_default_result_profile_define_sflow_sample_interface;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 14;

    
    define->data = 14;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_esem_default_result_profile_default_dual_homing_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_esem;
    int submodule_index = dnx_data_esem_submodule_default_result_profile;
    int define_index = dnx_data_esem_default_result_profile_define_default_dual_homing;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 15;

    
    define->data = 15;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_esem_default_result_profile_vxlan_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_esem;
    int submodule_index = dnx_data_esem_submodule_default_result_profile;
    int define_index = dnx_data_esem_default_result_profile_define_vxlan;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 11;

    
    define->data = 11;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_esem_default_result_profile_nof_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_esem;
    int submodule_index = dnx_data_esem_submodule_default_result_profile;
    int define_index = dnx_data_esem_default_result_profile_define_nof_profiles;
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
jr2_a0_dnx_data_esem_default_result_profile_nof_allocable_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_esem;
    int submodule_index = dnx_data_esem_submodule_default_result_profile;
    int define_index = dnx_data_esem_default_result_profile_define_nof_allocable_profiles;
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
jr2_a0_dnx_data_esem_feature_etps_properties_assignment_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_esem;
    int submodule_index = dnx_data_esem_submodule_feature;
    int feature_index = dnx_data_esem_feature_etps_properties_assignment;
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
jr2_a0_dnx_data_esem_feature_key_with_sspa_contains_member_id_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_esem;
    int submodule_index = dnx_data_esem_submodule_feature;
    int feature_index = dnx_data_esem_feature_key_with_sspa_contains_member_id;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}





shr_error_e
jr2_a0_data_esem_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_esem;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_esem_submodule_access_cmd;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_esem_access_cmd_define_nof_esem_accesses;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_esem_access_cmd_nof_esem_accesses_set;
    data_index = dnx_data_esem_access_cmd_define_app_di_id_size_in_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_esem_access_cmd_app_di_id_size_in_bits_set;
    data_index = dnx_data_esem_access_cmd_define_no_action;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_esem_access_cmd_no_action_set;
    data_index = dnx_data_esem_access_cmd_define_default_native;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_esem_access_cmd_default_native_set;
    data_index = dnx_data_esem_access_cmd_define_default_ac;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_esem_access_cmd_default_ac_set;
    data_index = dnx_data_esem_access_cmd_define_sflow_sample_interface;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_esem_access_cmd_sflow_sample_interface_set;
    data_index = dnx_data_esem_access_cmd_define_ip_tunnel_no_action;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_esem_access_cmd_ip_tunnel_no_action_set;
    data_index = dnx_data_esem_access_cmd_define_nof_esem_cmd_default_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_esem_access_cmd_nof_esem_cmd_default_profiles_set;
    data_index = dnx_data_esem_access_cmd_define_nof_cmds_size_in_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_esem_access_cmd_nof_cmds_size_in_bits_set;
    data_index = dnx_data_esem_access_cmd_define_nof_cmds;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_esem_access_cmd_nof_cmds_set;
    data_index = dnx_data_esem_access_cmd_define_min_size_for_esem_cmd;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_esem_access_cmd_min_size_for_esem_cmd_set;
    data_index = dnx_data_esem_access_cmd_define_esem_cmd_arr_prefix_gre_any_ipv4;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_esem_access_cmd_esem_cmd_arr_prefix_gre_any_ipv4_set;
    data_index = dnx_data_esem_access_cmd_define_esem_cmd_arr_prefix_vxlan_gpe;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_esem_access_cmd_esem_cmd_arr_prefix_vxlan_gpe_set;
    data_index = dnx_data_esem_access_cmd_define_esem_cmd_arr_prefix_ip_tunnel_no_action;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_esem_access_cmd_esem_cmd_arr_prefix_ip_tunnel_no_action_set;

    

    
    
    submodule_index = dnx_data_esem_submodule_default_result_profile;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_esem_default_result_profile_define_default_native;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_esem_default_result_profile_default_native_set;
    data_index = dnx_data_esem_default_result_profile_define_default_ac;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_esem_default_result_profile_default_ac_set;
    data_index = dnx_data_esem_default_result_profile_define_sflow_sample_interface;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_esem_default_result_profile_sflow_sample_interface_set;
    data_index = dnx_data_esem_default_result_profile_define_default_dual_homing;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_esem_default_result_profile_default_dual_homing_set;
    data_index = dnx_data_esem_default_result_profile_define_vxlan;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_esem_default_result_profile_vxlan_set;
    data_index = dnx_data_esem_default_result_profile_define_nof_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_esem_default_result_profile_nof_profiles_set;
    data_index = dnx_data_esem_default_result_profile_define_nof_allocable_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_esem_default_result_profile_nof_allocable_profiles_set;

    

    
    
    submodule_index = dnx_data_esem_submodule_feature;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_esem_feature_etps_properties_assignment;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_esem_feature_etps_properties_assignment_set;
    data_index = dnx_data_esem_feature_key_with_sspa_contains_member_id;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_esem_feature_key_with_sspa_contains_member_id_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

