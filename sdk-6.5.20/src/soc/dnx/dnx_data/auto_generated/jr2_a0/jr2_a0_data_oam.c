

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_OAM

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_oam.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>







static shr_error_e
jr2_a0_dnx_data_oam_general_oam_event_fifo_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_general;
    int feature_index = dnx_data_oam_general_oam_event_fifo_support;
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
jr2_a0_dnx_data_oam_general_low_scaling_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_general;
    int feature_index = dnx_data_oam_general_low_scaling;
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
jr2_a0_dnx_data_oam_general_max_oam_offset_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_general;
    int feature_index = dnx_data_oam_general_max_oam_offset_support;
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
jr2_a0_dnx_data_oam_general_oam_nof_non_acc_action_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_general;
    int define_index = dnx_data_oam_general_define_oam_nof_non_acc_action_profiles;
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
jr2_a0_dnx_data_oam_general_oam_nof_acc_action_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_general;
    int define_index = dnx_data_oam_general_define_oam_nof_acc_action_profiles;
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
jr2_a0_dnx_data_oam_general_oam_nof_reserved_egr_acc_action_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_general;
    int define_index = dnx_data_oam_general_define_oam_nof_reserved_egr_acc_action_profiles;
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
jr2_a0_dnx_data_oam_general_oam_egr_acc_action_profile_id_for_inject_mpls_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_general;
    int define_index = dnx_data_oam_general_define_oam_egr_acc_action_profile_id_for_inject_mpls;
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
jr2_a0_dnx_data_oam_general_oam_nof_bits_non_acc_action_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_general;
    int define_index = dnx_data_oam_general_define_oam_nof_bits_non_acc_action_profiles;
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
jr2_a0_dnx_data_oam_general_oam_nof_bits_acc_action_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_general;
    int define_index = dnx_data_oam_general_define_oam_nof_bits_acc_action_profiles;
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
jr2_a0_dnx_data_oam_general_oam_nof_bits_oam_lif_db_key_base_ingress_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_general;
    int define_index = dnx_data_oam_general_define_oam_nof_bits_oam_lif_db_key_base_ingress;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 22;

    
    define->data = 22;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_oam_general_oam_nof_bits_oam_lif_db_key_base_egress_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_general;
    int define_index = dnx_data_oam_general_define_oam_nof_bits_oam_lif_db_key_base_egress;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 20;

    
    define->data = 20;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_oam_general_oam_nof_endpoints_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_general;
    int define_index = dnx_data_oam_general_define_oam_nof_endpoints;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8*dnx_data_oam.general.oam_nof_entries_oam_lif_db_get(unit);

    
    define->data = 8*dnx_data_oam.general.oam_nof_entries_oam_lif_db_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_oam_general_oam_nof_oamp_meps_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_general;
    int define_index = dnx_data_oam_general_define_oam_nof_oamp_meps;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 65536;

    
    define->data = 65536;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_oam_general_oam_nof_groups_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_general;
    int define_index = dnx_data_oam_general_define_oam_nof_groups;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_oam.general.oam_nof_endpoints_get(unit);

    
    define->data = dnx_data_oam.general.oam_nof_endpoints_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_oam_general_oam_nof_entries_oam_lif_db_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_general;
    int define_index = dnx_data_oam_general_define_oam_nof_entries_oam_lif_db;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4096;

    
    define->data = 4096;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_oam_general_oam_nof_bits_counter_base_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_general;
    int define_index = dnx_data_oam_general_define_oam_nof_bits_counter_base;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 20;

    
    define->data = 20;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_oam_general_oam_nof_punt_error_codes_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_general;
    int define_index = dnx_data_oam_general_define_oam_nof_punt_error_codes;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 18;

    
    define->data = 18;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_oam_general_oam_nof_priority_map_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_general;
    int define_index = dnx_data_oam_general_define_oam_nof_priority_map_profiles;
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
jr2_a0_dnx_data_oam_general_oam_force_interface_for_meps_without_counter_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_general;
    int define_index = dnx_data_oam_general_define_oam_force_interface_for_meps_without_counter;
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
jr2_a0_dnx_data_oam_general_oam_my_cfm_mac_by_range_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_general;
    int define_index = dnx_data_oam_general_define_oam_my_cfm_mac_by_range;
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
jr2_a0_dnx_data_oam_general_oam_nof_pemla_channel_types_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_general;
    int define_index = dnx_data_oam_general_define_oam_nof_pemla_channel_types;
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
jr2_a0_dnx_data_oam_general_oam_mdb_rmep_db_profile_limitation_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_general;
    int define_index = dnx_data_oam_general_define_oam_mdb_rmep_db_profile_limitation;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1024*256;

    
    define->data = 1024*256;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_oam_general_oam_lm_read_index_field_exists_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_general;
    int define_index = dnx_data_oam_general_define_oam_lm_read_index_field_exists;
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
jr2_a0_dnx_data_oam_general_oam_lm_read_per_sub_enable_field_exists_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_general;
    int define_index = dnx_data_oam_general_define_oam_lm_read_per_sub_enable_field_exists;
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
jr2_a0_dnx_data_oam_general_oam_lm_read_per_sub_disable_field_exists_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_general;
    int define_index = dnx_data_oam_general_define_oam_lm_read_per_sub_disable_field_exists;
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
jr2_a0_dnx_data_oam_general_oam_identification_appdb_1_field_exists_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_general;
    int define_index = dnx_data_oam_general_define_oam_identification_appdb_1_field_exists;
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
jr2_a0_dnx_data_oam_general_oam_ntp_freq_control_word_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_general;
    int define_index = dnx_data_oam_general_define_oam_ntp_freq_control_word;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x44b83;

    
    define->data = 0x44b83;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_oam_general_oam_ntp_time_freq_control_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_general;
    int define_index = dnx_data_oam_general_define_oam_ntp_time_freq_control;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x44b82fa1;

    
    define->data = 0x44b82fa1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_oam_general_oam_IEEE_1588_time_freq_control_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_general;
    int define_index = dnx_data_oam_general_define_oam_IEEE_1588_time_freq_control;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x10000000;

    
    define->data = 0x10000000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_oam_general_oam_nse_nco_freq_control_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_general;
    int define_index = dnx_data_oam_general_define_oam_nse_nco_freq_control;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x10000000;

    
    define->data = 0x10000000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_oam_general_oam_nof_labels_for_sd_on_p_node_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_general;
    int define_index = dnx_data_oam_general_define_oam_nof_labels_for_sd_on_p_node;
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
jr2_a0_dnx_data_oam_general_oam_tod_config_in_eci_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_general;
    int define_index = dnx_data_oam_general_define_oam_tod_config_in_eci;
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
jr2_a0_dnx_data_oam_oamp_oamp_pe_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int feature_index = dnx_data_oam_oamp_oamp_pe_support;
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
jr2_a0_dnx_data_oam_oamp_oamp_pe_tx_machine_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int feature_index = dnx_data_oam_oamp_oamp_pe_tx_machine_support;
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
jr2_a0_dnx_data_oam_oamp_nof_mep_db_entry_banks_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_nof_mep_db_entry_banks;
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
jr2_a0_dnx_data_oam_oamp_nof_mep_db_entries_per_bank_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_nof_mep_db_entries_per_bank;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2048;

    
    define->data = 2048;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_oam_oamp_scan_rate_bank_exist_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_scan_rate_bank_exist;
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
jr2_a0_dnx_data_oam_oamp_nof_mep_db_entries_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_nof_mep_db_entries;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_oam.oamp.nof_mep_db_entry_banks_get(unit)*dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit);

    
    define->data = dnx_data_oam.oamp.nof_mep_db_entry_banks_get(unit)*dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_oam_oamp_max_nof_endpoint_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_max_nof_endpoint_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8*dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit)*dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit);

    
    define->data = 8*dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit)*dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_oam_oamp_max_nof_mep_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_max_nof_mep_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_oam.oamp.nof_mep_db_entry_banks_get(unit)*dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit)*dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit);

    
    define->data = dnx_data_oam.oamp.nof_mep_db_entry_banks_get(unit)*dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit)*dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_oam_oamp_max_value_of_mep_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_max_value_of_mep_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_oam.oamp.max_nof_mep_id_get(unit)-1;

    
    define->data = dnx_data_oam.oamp.max_nof_mep_id_get(unit)-1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_oam_oamp_nof_umc_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_nof_umc;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2*1024;

    
    define->data = 2*1024;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_oam_oamp_mep_id_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_mep_id_nof_bits;
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
jr2_a0_dnx_data_oam_oamp_nof_mep_db_entry_banks_for_endpoints_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_nof_mep_db_entry_banks_for_endpoints;
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
jr2_a0_dnx_data_oam_oamp_nof_mep_db_endpoint_entries_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_nof_mep_db_endpoint_entries;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_oam.oamp.nof_mep_db_entry_banks_for_endpoints_get(unit)*dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit);

    
    define->data = dnx_data_oam.oamp.nof_mep_db_entry_banks_for_endpoints_get(unit)*dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_oam_oamp_mep_db_da_is_mc_for_short_entries_support_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_mep_db_da_is_mc_for_short_entries_support;
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
jr2_a0_dnx_data_oam_oamp_nof_icc_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_nof_icc_profiles;
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
jr2_a0_dnx_data_oam_oamp_local_port_2_system_port_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_local_port_2_system_port_size;
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
jr2_a0_dnx_data_oam_oamp_oam_nof_sa_mac_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_oam_nof_sa_mac_profiles;
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
jr2_a0_dnx_data_oam_oamp_oam_nof_da_mac_msb_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_oam_nof_da_mac_msb_profiles;
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
jr2_a0_dnx_data_oam_oamp_oam_nof_da_mac_lsb_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_oam_nof_da_mac_lsb_profiles;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 128;

    
    define->data = 128;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_oam_oamp_oam_nof_tpid_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_oam_nof_tpid_profiles;
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
jr2_a0_dnx_data_oam_oamp_oamp_pe_default_instruction_index_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_oamp_pe_default_instruction_index;
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
jr2_a0_dnx_data_oam_oamp_oamp_pe_max_nof_instructions_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_oamp_pe_max_nof_instructions;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4096;

    
    define->data = 4096;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_oam_oamp_oamp_pe_nof_fem_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_oamp_pe_nof_fem_bits;
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
jr2_a0_dnx_data_oam_oamp_mep_db_sub_entry_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_mep_db_sub_entry_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 54;

    
    define->data = 54;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_oam_oamp_mep_db_nof_sub_entries_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_mep_db_nof_sub_entries;
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
jr2_a0_dnx_data_oam_oamp_mep_db_full_entry_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_mep_db_full_entry_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_oam.oamp.mep_db_sub_entry_size_get(unit)*dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit);

    
    define->data = dnx_data_oam.oamp.mep_db_sub_entry_size_get(unit)*dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_oam_oamp_mep_pe_profile_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_mep_pe_profile_nof_bits;
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
jr2_a0_dnx_data_oam_oamp_rmep_db_entries_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_rmep_db_entries_nof_bits;
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
jr2_a0_dnx_data_oam_oamp_rmep_db_rmep_index_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_rmep_db_rmep_index_nof_bits;
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
jr2_a0_dnx_data_oam_oamp_rmep_max_self_contained_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_rmep_max_self_contained;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32*1024;

    
    define->data = 32*1024;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_oam_oamp_rmep_sys_clocks_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_rmep_sys_clocks;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 10;

    
    define->data = 10;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_oam_oamp_mep_sys_clocks_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_mep_sys_clocks;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 40;

    
    define->data = 40;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_oam_oamp_mep_db_version_limitation_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_mep_db_version_limitation;
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
jr2_a0_dnx_data_oam_oamp_oamp_mep_db_threshold_version_limitation_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_oamp_mep_db_threshold_version_limitation;
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
jr2_a0_dnx_data_oam_oamp_rmep_db_non_zero_lifetime_units_limitation_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_rmep_db_non_zero_lifetime_units_limitation;
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
jr2_a0_dnx_data_oam_oamp_crc_calculation_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_crc_calculation_supported;
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
jr2_a0_dnx_data_oam_oamp_modified_mep_db_structure_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_modified_mep_db_structure;
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
jr2_a0_dnx_data_oam_oamp_local_port_2_system_port_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_local_port_2_system_port_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 9;

    
    define->data = 9;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_oam_oamp_mdb_nof_access_ids_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_mdb_nof_access_ids;
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
jr2_a0_dnx_data_oam_oamp_bfd_mpls_short_mdb_reserved_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_bfd_mpls_short_mdb_reserved_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 12-dnx_data_oam.oamp.local_port_2_system_port_bits_get(unit);

    
    define->data = 12-dnx_data_oam.oamp.local_port_2_system_port_bits_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_oam_oamp_bfd_m_hop_short_mdb_reserved_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_bfd_m_hop_short_mdb_reserved_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 12-dnx_data_oam.oamp.local_port_2_system_port_bits_get(unit);

    
    define->data = 12-dnx_data_oam.oamp.local_port_2_system_port_bits_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_oam_oamp_bfd_pwe_short_mdb_reserved_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_bfd_pwe_short_mdb_reserved_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 11-dnx_data_oam.oamp.local_port_2_system_port_bits_get(unit);

    
    define->data = 11-dnx_data_oam.oamp.local_port_2_system_port_bits_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_oam_oamp_ccm_eth_offload_mdb_reserved_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_ccm_eth_offload_mdb_reserved_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 15-dnx_data_oam.oamp.local_port_2_system_port_bits_get(unit);

    
    define->data = 15-dnx_data_oam.oamp.local_port_2_system_port_bits_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_oam_oamp_bfd_1_hop_short_mdb_reserved_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_bfd_1_hop_short_mdb_reserved_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 11-dnx_data_oam.oamp.local_port_2_system_port_bits_get(unit);

    
    define->data = 11-dnx_data_oam.oamp.local_port_2_system_port_bits_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_oam_oamp_ccm_mpls_tp_pwe_offload_mdb_reserved_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_ccm_mpls_tp_pwe_offload_mdb_reserved_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 15-dnx_data_oam.oamp.local_port_2_system_port_bits_get(unit);

    
    define->data = 15-dnx_data_oam.oamp.local_port_2_system_port_bits_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_oam_oamp_punt_packet_destination_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_punt_packet_destination_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 20;

    
    define->data = 20;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_oam_oamp_punt_packet_itmh_destination_full_encoding_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_punt_packet_itmh_destination_full_encoding_supported;
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
jr2_a0_dnx_data_oam_oamp_core_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_core_nof_bits;
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
jr2_a0_dnx_data_oam_oamp_punt_packet_pph_ttl_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_punt_packet_pph_ttl_supported;
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
jr2_a0_dnx_data_oam_oamp_lmm_dmm_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_lmm_dmm_supported;
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
jr2_a0_dnx_data_oam_oamp_intr_oamp_pending_event_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_intr_oamp_pending_event;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 923;

    
    define->data = 923;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_oam_oamp_intr_oamp_stat_pending_event_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_intr_oamp_stat_pending_event;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 924;

    
    define->data = 924;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_oam_oamp_slm_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_slm_supported;
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
jr2_a0_dnx_data_oam_oamp_mdb_tx_rate_lsb_field_present_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_mdb_tx_rate_lsb_field_present;
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
jr2_a0_dnx_data_oam_oamp_nof_jumbo_dm_tlv_entries_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_nof_jumbo_dm_tlv_entries;
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
jr2_a0_dnx_data_oam_oamp_mdb_reserved_field_size_calculation_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_mdb_reserved_field_size_calculation;
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
jr2_a0_dnx_data_oam_oamp_oamp_nof_scan_rmep_db_entries_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_oamp_nof_scan_rmep_db_entries;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 64*1024-1;

    
    define->data = 64*1024-1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "num_of_scan_rmep_db_entries";
    define->property.doc = 
        "\n"
        "Configure the number of RMEP DB entries to be scanned.\n"
        "Default: 64K-1(all entries are scanned)\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 64*1024-1;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_oam_oamp_oamp_mep_full_entry_threshold_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_oamp_mep_full_entry_threshold;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_OAMP_MEP_DB_FULL_ENTRY_THRESHOLD;
    define->property.doc = 
        "\n"
        "Configure the mep db full entry threshold.\n"
        "MEP IDs up to and including the theshold will be utilized by 1/4 entries\n"
        "MEP IDs above the theshold will be utilized by full entries\n"
        "Default: 0\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 64*1024;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_oam_oamp_oamp_rmep_full_entry_threshold_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_oamp_rmep_full_entry_threshold;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0*1024;

    
    define->data = 0*1024;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_OAMP_RMEP_DB_FULL_ENTRY_THRESHOLD;
    define->property.doc = 
        "\n"
        "Configure the rmep db full entry threshold.\n"
        "RMEP IDs up to and including the theshold will be utilized by 1/2 entries\n"
        "RMEP IDs above the theshold will be utilized by full entries\n"
        "Default: 0\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 64*1024;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_oam_oamp_mep_db_jr1_mode_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_mep_db_jr1_mode;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_SYSTEM_HEADERS_MODE;
    define->property.doc = NULL;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 2;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "0";
    define->property.mapping[0].val = 1;
    define->property.mapping[1].name = "1";
    define->property.mapping[1].val = 0;
    define->property.mapping[1].is_default = 1 ;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_oam_oamp_oamp_fifo_dma_event_interface_enable_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_oamp_fifo_dma_event_interface_enable;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_OAMP_FIFO_DMA_EVENT_INTERFACE_ENABLE;
    define->property.doc = 
        "\n"
        "Enables OAMP DMA event interrupts interface.\n"
        "Default: 0\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 1;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_oam_oamp_oamp_fifo_dma_event_interface_buffer_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_oamp_fifo_dma_event_interface_buffer_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_OAMP_FIFO_DMA_EVENT_INTERFACE_BUFFER_SIZE;
    define->property.doc = 
        "\n"
        "Configure the size of OAMP event interrupts host memory in bytes.\n"
        "The valid range of memory size is 80-413200 bytes.\n"
        "Default: 0\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 80;
    define->property.range_max = 413200;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_oam_oamp_oamp_fifo_dma_event_interface_timeout_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_oamp_fifo_dma_event_interface_timeout;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_OAMP_FIFO_DMA_EVENT_INTERFACE_TIMEOUT;
    define->property.doc = 
        "\n"
        "Configures the OAMP fifo dma timeout in microseconds.\n"
        "The valid range is 0-65535.\n"
        "Default: 0\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 65535;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_oam_oamp_oamp_fifo_dma_event_interface_threshold_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_oamp_fifo_dma_event_interface_threshold;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32;

    
    define->data = 32;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_OAMP_FIFO_DMA_EVENT_INTERFACE_THRESHOLD;
    define->property.doc = 
        "\n"
        "Configures the OAMP fifo dma threshold.\n"
        "The valid range is 1-16384.\n"
        "Default: 32\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 1;
    define->property.range_max = 16384;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_oam_oamp_oamp_fifo_dma_report_interface_enable_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_oamp_fifo_dma_report_interface_enable;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_OAMP_FIFO_DMA_REPORT_INTERFACE_ENABLE;
    define->property.doc = 
        "\n"
        "Enables OAMP DMA statistic interrupts interface.\n"
        "Default: 0\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 1;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_oam_oamp_oamp_fifo_dma_report_interface_buffer_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_oamp_fifo_dma_report_interface_buffer_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_OAMP_FIFO_DMA_REPORT_INTERFACE_BUFFER_SIZE;
    define->property.doc = 
        "\n"
        "Configure the size of OAMP statistic interrupts host memory in bytes.\n"
        "The valid range of memory size is 80-413200 bytes.\n"
        "Default: 0\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 80;
    define->property.range_max = 413200;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_oam_oamp_oamp_fifo_dma_report_interface_timeout_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_oamp_fifo_dma_report_interface_timeout;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_OAMP_FIFO_DMA_REPORT_INTERFACE_TIMEOUT;
    define->property.doc = 
        "\n"
        "Configures the OAMP fifo dma report timeout in microseconds.\n"
        "The valid range is 0-65535.\n"
        "Default: 0\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 65535;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_oam_oamp_oamp_fifo_dma_report_interface_threshold_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_oamp_fifo_dma_report_interface_threshold;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32;

    
    define->data = 32;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_OAMP_FIFO_DMA_REPORT_INTERFACE_THRESHOLD;
    define->property.doc = 
        "\n"
        "Configures the OAMP fifo dma report threshold.\n"
        "The valid range is 1-16384.\n"
        "Default: 32\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 1;
    define->property.range_max = 16384;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_oam_oamp_server_destination_msb_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_server_destination_msb_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 14;

    
    define->data = 14;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_SYSTEM_HEADERS_MODE;
    define->property.doc = 
        "Different field lengths for regular and inter-op mode\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 2;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "0";
    define->property.mapping[0].val = 11;
    define->property.mapping[1].name = "1";
    define->property.mapping[1].val = 14;
    define->property.mapping[1].is_default = 1 ;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_oam_oamp_server_destination_lsb_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_server_destination_lsb_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 7;

    
    define->data = 7;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_SYSTEM_HEADERS_MODE;
    define->property.doc = 
        "Different field lengths for regular and inter-op mode\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 2;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "0";
    define->property.mapping[0].val = 8;
    define->property.mapping[1].name = "1";
    define->property.mapping[1].val = 7;
    define->property.mapping[1].is_default = 1 ;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_oam_property_oam_injected_over_lsp_cnt_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_property;
    int define_index = dnx_data_oam_property_define_oam_injected_over_lsp_cnt;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_CUSTOM_FEATURE;
    define->property.doc = 
        "\n"
        "Default is 0.\n"
        "Used for Count injected CCM packet over LSP\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_enable;
    define->property.method_str = "suffix_enable";
    define->property.suffix = "oam_injected_over_lsp_cnt";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_oam_feature_slr_scan_mashine_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_feature;
    int feature_index = dnx_data_oam_feature_slr_scan_mashine_support;
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
jr2_a0_dnx_data_oam_feature_oam_offset_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_feature;
    int feature_index = dnx_data_oam_feature_oam_offset_supported;
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
jr2_a0_dnx_data_oam_feature_oam_statistics_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_feature;
    int feature_index = dnx_data_oam_feature_oam_statistics_supported;
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
jr2_a0_dnx_data_oam_feature_lm_read_index_limitation_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_feature;
    int feature_index = dnx_data_oam_feature_lm_read_index_limitation;
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
jr2_a0_dnx_data_oam_feature_oamp_scanner_limitation_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_feature;
    int feature_index = dnx_data_oam_feature_oamp_scanner_limitation;
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
jr2_a0_dnx_data_oam_feature_oamp_tx_rate_limitation_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_feature;
    int feature_index = dnx_data_oam_feature_oamp_tx_rate_limitation;
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
jr2_a0_dnx_data_oam_feature_oamp_protection_rmep_id_limitation_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_feature;
    int feature_index = dnx_data_oam_feature_oamp_protection_rmep_id_limitation;
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
jr2_a0_dnx_data_oam_feature_oamp_generic_response_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_feature;
    int feature_index = dnx_data_oam_feature_oamp_generic_response_supported;
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
jr2_a0_dnx_data_oam_feature_oamp_txm_bank_limitation_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_feature;
    int feature_index = dnx_data_oam_feature_oamp_txm_bank_limitation;
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
jr2_a0_dnx_data_oam_feature_oamp_rmep_scale_limitataion_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_feature;
    int feature_index = dnx_data_oam_feature_oamp_rmep_scale_limitataion;
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
jr2_a0_data_oam_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_oam;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_oam_submodule_general;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_oam_general_define_oam_nof_non_acc_action_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_general_oam_nof_non_acc_action_profiles_set;
    data_index = dnx_data_oam_general_define_oam_nof_acc_action_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_general_oam_nof_acc_action_profiles_set;
    data_index = dnx_data_oam_general_define_oam_nof_reserved_egr_acc_action_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_general_oam_nof_reserved_egr_acc_action_profiles_set;
    data_index = dnx_data_oam_general_define_oam_egr_acc_action_profile_id_for_inject_mpls;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_general_oam_egr_acc_action_profile_id_for_inject_mpls_set;
    data_index = dnx_data_oam_general_define_oam_nof_bits_non_acc_action_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_general_oam_nof_bits_non_acc_action_profiles_set;
    data_index = dnx_data_oam_general_define_oam_nof_bits_acc_action_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_general_oam_nof_bits_acc_action_profiles_set;
    data_index = dnx_data_oam_general_define_oam_nof_bits_oam_lif_db_key_base_ingress;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_general_oam_nof_bits_oam_lif_db_key_base_ingress_set;
    data_index = dnx_data_oam_general_define_oam_nof_bits_oam_lif_db_key_base_egress;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_general_oam_nof_bits_oam_lif_db_key_base_egress_set;
    data_index = dnx_data_oam_general_define_oam_nof_endpoints;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_general_oam_nof_endpoints_set;
    data_index = dnx_data_oam_general_define_oam_nof_oamp_meps;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_general_oam_nof_oamp_meps_set;
    data_index = dnx_data_oam_general_define_oam_nof_groups;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_general_oam_nof_groups_set;
    data_index = dnx_data_oam_general_define_oam_nof_entries_oam_lif_db;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_general_oam_nof_entries_oam_lif_db_set;
    data_index = dnx_data_oam_general_define_oam_nof_bits_counter_base;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_general_oam_nof_bits_counter_base_set;
    data_index = dnx_data_oam_general_define_oam_nof_punt_error_codes;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_general_oam_nof_punt_error_codes_set;
    data_index = dnx_data_oam_general_define_oam_nof_priority_map_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_general_oam_nof_priority_map_profiles_set;
    data_index = dnx_data_oam_general_define_oam_force_interface_for_meps_without_counter;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_general_oam_force_interface_for_meps_without_counter_set;
    data_index = dnx_data_oam_general_define_oam_my_cfm_mac_by_range;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_general_oam_my_cfm_mac_by_range_set;
    data_index = dnx_data_oam_general_define_oam_nof_pemla_channel_types;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_general_oam_nof_pemla_channel_types_set;
    data_index = dnx_data_oam_general_define_oam_mdb_rmep_db_profile_limitation;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_general_oam_mdb_rmep_db_profile_limitation_set;
    data_index = dnx_data_oam_general_define_oam_lm_read_index_field_exists;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_general_oam_lm_read_index_field_exists_set;
    data_index = dnx_data_oam_general_define_oam_lm_read_per_sub_enable_field_exists;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_general_oam_lm_read_per_sub_enable_field_exists_set;
    data_index = dnx_data_oam_general_define_oam_lm_read_per_sub_disable_field_exists;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_general_oam_lm_read_per_sub_disable_field_exists_set;
    data_index = dnx_data_oam_general_define_oam_identification_appdb_1_field_exists;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_general_oam_identification_appdb_1_field_exists_set;
    data_index = dnx_data_oam_general_define_oam_ntp_freq_control_word;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_general_oam_ntp_freq_control_word_set;
    data_index = dnx_data_oam_general_define_oam_ntp_time_freq_control;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_general_oam_ntp_time_freq_control_set;
    data_index = dnx_data_oam_general_define_oam_IEEE_1588_time_freq_control;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_general_oam_IEEE_1588_time_freq_control_set;
    data_index = dnx_data_oam_general_define_oam_nse_nco_freq_control;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_general_oam_nse_nco_freq_control_set;
    data_index = dnx_data_oam_general_define_oam_nof_labels_for_sd_on_p_node;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_general_oam_nof_labels_for_sd_on_p_node_set;
    data_index = dnx_data_oam_general_define_oam_tod_config_in_eci;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_general_oam_tod_config_in_eci_set;

    
    data_index = dnx_data_oam_general_oam_event_fifo_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_oam_general_oam_event_fifo_support_set;
    data_index = dnx_data_oam_general_low_scaling;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_oam_general_low_scaling_set;
    data_index = dnx_data_oam_general_max_oam_offset_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_oam_general_max_oam_offset_support_set;

    
    
    submodule_index = dnx_data_oam_submodule_oamp;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_oam_oamp_define_nof_mep_db_entry_banks;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_nof_mep_db_entry_banks_set;
    data_index = dnx_data_oam_oamp_define_nof_mep_db_entries_per_bank;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_nof_mep_db_entries_per_bank_set;
    data_index = dnx_data_oam_oamp_define_scan_rate_bank_exist;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_scan_rate_bank_exist_set;
    data_index = dnx_data_oam_oamp_define_nof_mep_db_entries;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_nof_mep_db_entries_set;
    data_index = dnx_data_oam_oamp_define_max_nof_endpoint_id;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_max_nof_endpoint_id_set;
    data_index = dnx_data_oam_oamp_define_max_nof_mep_id;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_max_nof_mep_id_set;
    data_index = dnx_data_oam_oamp_define_max_value_of_mep_id;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_max_value_of_mep_id_set;
    data_index = dnx_data_oam_oamp_define_nof_umc;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_nof_umc_set;
    data_index = dnx_data_oam_oamp_define_mep_id_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_mep_id_nof_bits_set;
    data_index = dnx_data_oam_oamp_define_nof_mep_db_entry_banks_for_endpoints;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_nof_mep_db_entry_banks_for_endpoints_set;
    data_index = dnx_data_oam_oamp_define_nof_mep_db_endpoint_entries;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_nof_mep_db_endpoint_entries_set;
    data_index = dnx_data_oam_oamp_define_mep_db_da_is_mc_for_short_entries_support;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_mep_db_da_is_mc_for_short_entries_support_set;
    data_index = dnx_data_oam_oamp_define_nof_icc_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_nof_icc_profiles_set;
    data_index = dnx_data_oam_oamp_define_local_port_2_system_port_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_local_port_2_system_port_size_set;
    data_index = dnx_data_oam_oamp_define_oam_nof_sa_mac_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_oam_nof_sa_mac_profiles_set;
    data_index = dnx_data_oam_oamp_define_oam_nof_da_mac_msb_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_oam_nof_da_mac_msb_profiles_set;
    data_index = dnx_data_oam_oamp_define_oam_nof_da_mac_lsb_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_oam_nof_da_mac_lsb_profiles_set;
    data_index = dnx_data_oam_oamp_define_oam_nof_tpid_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_oam_nof_tpid_profiles_set;
    data_index = dnx_data_oam_oamp_define_oamp_pe_default_instruction_index;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_oamp_pe_default_instruction_index_set;
    data_index = dnx_data_oam_oamp_define_oamp_pe_max_nof_instructions;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_oamp_pe_max_nof_instructions_set;
    data_index = dnx_data_oam_oamp_define_oamp_pe_nof_fem_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_oamp_pe_nof_fem_bits_set;
    data_index = dnx_data_oam_oamp_define_mep_db_sub_entry_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_mep_db_sub_entry_size_set;
    data_index = dnx_data_oam_oamp_define_mep_db_nof_sub_entries;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_mep_db_nof_sub_entries_set;
    data_index = dnx_data_oam_oamp_define_mep_db_full_entry_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_mep_db_full_entry_size_set;
    data_index = dnx_data_oam_oamp_define_mep_pe_profile_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_mep_pe_profile_nof_bits_set;
    data_index = dnx_data_oam_oamp_define_rmep_db_entries_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_rmep_db_entries_nof_bits_set;
    data_index = dnx_data_oam_oamp_define_rmep_db_rmep_index_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_rmep_db_rmep_index_nof_bits_set;
    data_index = dnx_data_oam_oamp_define_rmep_max_self_contained;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_rmep_max_self_contained_set;
    data_index = dnx_data_oam_oamp_define_rmep_sys_clocks;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_rmep_sys_clocks_set;
    data_index = dnx_data_oam_oamp_define_mep_sys_clocks;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_mep_sys_clocks_set;
    data_index = dnx_data_oam_oamp_define_mep_db_version_limitation;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_mep_db_version_limitation_set;
    data_index = dnx_data_oam_oamp_define_oamp_mep_db_threshold_version_limitation;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_oamp_mep_db_threshold_version_limitation_set;
    data_index = dnx_data_oam_oamp_define_rmep_db_non_zero_lifetime_units_limitation;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_rmep_db_non_zero_lifetime_units_limitation_set;
    data_index = dnx_data_oam_oamp_define_crc_calculation_supported;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_crc_calculation_supported_set;
    data_index = dnx_data_oam_oamp_define_modified_mep_db_structure;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_modified_mep_db_structure_set;
    data_index = dnx_data_oam_oamp_define_local_port_2_system_port_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_local_port_2_system_port_bits_set;
    data_index = dnx_data_oam_oamp_define_mdb_nof_access_ids;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_mdb_nof_access_ids_set;
    data_index = dnx_data_oam_oamp_define_bfd_mpls_short_mdb_reserved_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_bfd_mpls_short_mdb_reserved_bits_set;
    data_index = dnx_data_oam_oamp_define_bfd_m_hop_short_mdb_reserved_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_bfd_m_hop_short_mdb_reserved_bits_set;
    data_index = dnx_data_oam_oamp_define_bfd_pwe_short_mdb_reserved_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_bfd_pwe_short_mdb_reserved_bits_set;
    data_index = dnx_data_oam_oamp_define_ccm_eth_offload_mdb_reserved_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_ccm_eth_offload_mdb_reserved_bits_set;
    data_index = dnx_data_oam_oamp_define_bfd_1_hop_short_mdb_reserved_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_bfd_1_hop_short_mdb_reserved_bits_set;
    data_index = dnx_data_oam_oamp_define_ccm_mpls_tp_pwe_offload_mdb_reserved_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_ccm_mpls_tp_pwe_offload_mdb_reserved_bits_set;
    data_index = dnx_data_oam_oamp_define_punt_packet_destination_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_punt_packet_destination_size_set;
    data_index = dnx_data_oam_oamp_define_punt_packet_itmh_destination_full_encoding_supported;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_punt_packet_itmh_destination_full_encoding_supported_set;
    data_index = dnx_data_oam_oamp_define_core_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_core_nof_bits_set;
    data_index = dnx_data_oam_oamp_define_punt_packet_pph_ttl_supported;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_punt_packet_pph_ttl_supported_set;
    data_index = dnx_data_oam_oamp_define_lmm_dmm_supported;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_lmm_dmm_supported_set;
    data_index = dnx_data_oam_oamp_define_intr_oamp_pending_event;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_intr_oamp_pending_event_set;
    data_index = dnx_data_oam_oamp_define_intr_oamp_stat_pending_event;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_intr_oamp_stat_pending_event_set;
    data_index = dnx_data_oam_oamp_define_slm_supported;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_slm_supported_set;
    data_index = dnx_data_oam_oamp_define_mdb_tx_rate_lsb_field_present;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_mdb_tx_rate_lsb_field_present_set;
    data_index = dnx_data_oam_oamp_define_nof_jumbo_dm_tlv_entries;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_nof_jumbo_dm_tlv_entries_set;
    data_index = dnx_data_oam_oamp_define_mdb_reserved_field_size_calculation;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_mdb_reserved_field_size_calculation_set;
    data_index = dnx_data_oam_oamp_define_oamp_nof_scan_rmep_db_entries;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_oamp_nof_scan_rmep_db_entries_set;
    data_index = dnx_data_oam_oamp_define_oamp_mep_full_entry_threshold;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_oamp_mep_full_entry_threshold_set;
    data_index = dnx_data_oam_oamp_define_oamp_rmep_full_entry_threshold;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_oamp_rmep_full_entry_threshold_set;
    data_index = dnx_data_oam_oamp_define_mep_db_jr1_mode;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_mep_db_jr1_mode_set;
    data_index = dnx_data_oam_oamp_define_oamp_fifo_dma_event_interface_enable;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_oamp_fifo_dma_event_interface_enable_set;
    data_index = dnx_data_oam_oamp_define_oamp_fifo_dma_event_interface_buffer_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_oamp_fifo_dma_event_interface_buffer_size_set;
    data_index = dnx_data_oam_oamp_define_oamp_fifo_dma_event_interface_timeout;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_oamp_fifo_dma_event_interface_timeout_set;
    data_index = dnx_data_oam_oamp_define_oamp_fifo_dma_event_interface_threshold;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_oamp_fifo_dma_event_interface_threshold_set;
    data_index = dnx_data_oam_oamp_define_oamp_fifo_dma_report_interface_enable;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_oamp_fifo_dma_report_interface_enable_set;
    data_index = dnx_data_oam_oamp_define_oamp_fifo_dma_report_interface_buffer_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_oamp_fifo_dma_report_interface_buffer_size_set;
    data_index = dnx_data_oam_oamp_define_oamp_fifo_dma_report_interface_timeout;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_oamp_fifo_dma_report_interface_timeout_set;
    data_index = dnx_data_oam_oamp_define_oamp_fifo_dma_report_interface_threshold;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_oamp_fifo_dma_report_interface_threshold_set;
    data_index = dnx_data_oam_oamp_define_server_destination_msb_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_server_destination_msb_bits_set;
    data_index = dnx_data_oam_oamp_define_server_destination_lsb_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_oamp_server_destination_lsb_bits_set;

    
    data_index = dnx_data_oam_oamp_oamp_pe_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_oam_oamp_oamp_pe_support_set;
    data_index = dnx_data_oam_oamp_oamp_pe_tx_machine_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_oam_oamp_oamp_pe_tx_machine_support_set;

    
    
    submodule_index = dnx_data_oam_submodule_property;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_oam_property_define_oam_injected_over_lsp_cnt;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_oam_property_oam_injected_over_lsp_cnt_set;

    

    
    
    submodule_index = dnx_data_oam_submodule_feature;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_oam_feature_slr_scan_mashine_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_oam_feature_slr_scan_mashine_support_set;
    data_index = dnx_data_oam_feature_oam_offset_supported;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_oam_feature_oam_offset_supported_set;
    data_index = dnx_data_oam_feature_oam_statistics_supported;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_oam_feature_oam_statistics_supported_set;
    data_index = dnx_data_oam_feature_lm_read_index_limitation;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_oam_feature_lm_read_index_limitation_set;
    data_index = dnx_data_oam_feature_oamp_scanner_limitation;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_oam_feature_oamp_scanner_limitation_set;
    data_index = dnx_data_oam_feature_oamp_tx_rate_limitation;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_oam_feature_oamp_tx_rate_limitation_set;
    data_index = dnx_data_oam_feature_oamp_protection_rmep_id_limitation;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_oam_feature_oamp_protection_rmep_id_limitation_set;
    data_index = dnx_data_oam_feature_oamp_generic_response_supported;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_oam_feature_oamp_generic_response_supported_set;
    data_index = dnx_data_oam_feature_oamp_txm_bank_limitation;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_oam_feature_oamp_txm_bank_limitation_set;
    data_index = dnx_data_oam_feature_oamp_rmep_scale_limitataion;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_oam_feature_oamp_rmep_scale_limitataion_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

