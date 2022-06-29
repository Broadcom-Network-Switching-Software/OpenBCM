
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_L2

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_l2.h>







static shr_error_e
j2x_a0_dnx_data_l2_general_aging_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_general;
    int feature_index = dnx_data_l2_general_aging_support;
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
j2x_a0_dnx_data_l2_general_ivl_feature_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_general;
    int feature_index = dnx_data_l2_general_ivl_feature_support;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;
    
    feature->property.name = spn_SYSTEM_HEADERS_MODE;
    feature->property.doc = 
        "\n"
    ;
    feature->property.method = dnxc_data_property_method_direct_map;
    feature->property.method_str = "direct_map";
    feature->property.nof_mapping = 2;
    DNXC_DATA_ALLOC(feature->property.mapping , dnxc_data_property_map_t, feature->property.nof_mapping, "ivl_feature_support property mapping");

    feature->property.mapping[0].name = "0";
    feature->property.mapping[0].val = 0;
    feature->property.mapping[1].name = "1";
    feature->property.mapping[1].val = 1;
    feature->property.mapping[1].is_default = 1 ;
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &feature->property, -1, &feature->data));
    
    feature->flags |= (DNXC_DATA_F_PROPERTY);


    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
j2x_a0_dnx_data_l2_general_l2_egress_max_additional_termination_size_bytes_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_general;
    int define_index = dnx_data_l2_general_define_l2_egress_max_additional_termination_size_bytes;
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
j2x_a0_dnx_data_l2_general_mact_mngmnt_fid_exceed_limit_int_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_general;
    int define_index = dnx_data_l2_general_define_mact_mngmnt_fid_exceed_limit_int;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = J2P_INT_MACT_LARGE_EM_LARGE_EM_MNGMNT_REQ_FID_EXCEED_LIMIT;

    
    define->data = J2P_INT_MACT_LARGE_EM_LARGE_EM_MNGMNT_REQ_FID_EXCEED_LIMIT;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_l2_general_mact_lela_fid_exceed_limit_int_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_general;
    int define_index = dnx_data_l2_general_define_mact_lela_fid_exceed_limit_int;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = J2P_INT_MACT_LARGE_EM_LARGE_EM_LELA_FID_EXCEED_LIMIT;

    
    define->data = J2P_INT_MACT_LARGE_EM_LARGE_EM_LELA_FID_EXCEED_LIMIT;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
j2x_a0_dnx_data_l2_feature_age_out_and_refresh_profile_selection_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_feature;
    int feature_index = dnx_data_l2_feature_age_out_and_refresh_profile_selection;
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
j2x_a0_dnx_data_l2_feature_age_machine_pause_after_flush_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_feature;
    int feature_index = dnx_data_l2_feature_age_machine_pause_after_flush;
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
j2x_a0_dnx_data_l2_feature_age_state_not_updated_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_feature;
    int feature_index = dnx_data_l2_feature_age_state_not_updated;
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
j2x_a0_dnx_data_l2_feature_eth_qual_is_mc_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_feature;
    int feature_index = dnx_data_l2_feature_eth_qual_is_mc;
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
j2x_a0_dnx_data_l2_feature_bc_same_as_unknown_mc_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_feature;
    int feature_index = dnx_data_l2_feature_bc_same_as_unknown_mc;
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
j2x_a0_dnx_data_l2_feature_wrong_limit_interrupt_handling_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_feature;
    int feature_index = dnx_data_l2_feature_wrong_limit_interrupt_handling;
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
j2x_a0_dnx_data_l2_feature_transplant_instead_of_refresh_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_feature;
    int feature_index = dnx_data_l2_feature_transplant_instead_of_refresh;
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
j2x_a0_dnx_data_l2_feature_static_mac_age_out_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_feature;
    int feature_index = dnx_data_l2_feature_static_mac_age_out;
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
j2x_a0_dnx_data_l2_feature_vmv_for_limit_in_wrong_location_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_feature;
    int feature_index = dnx_data_l2_feature_vmv_for_limit_in_wrong_location;
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
j2x_a0_dnx_data_l2_feature_learn_limit_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_feature;
    int feature_index = dnx_data_l2_feature_learn_limit;
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
j2x_a0_dnx_data_l2_feature_learn_events_wrong_command_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_feature;
    int feature_index = dnx_data_l2_feature_learn_events_wrong_command;
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
j2x_a0_dnx_data_l2_feature_opportunistic_learning_always_transplant_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_feature;
    int feature_index = dnx_data_l2_feature_opportunistic_learning_always_transplant;
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
j2x_a0_dnx_data_l2_feature_exceed_limit_interrupt_by_insert_cmd_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_feature;
    int feature_index = dnx_data_l2_feature_exceed_limit_interrupt_by_insert_cmd;
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
j2x_a0_dnx_data_l2_feature_ignore_limit_check_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_feature;
    int feature_index = dnx_data_l2_feature_ignore_limit_check;
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
j2x_a0_dnx_data_l2_feature_no_trap_for_unknown_destination_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_feature;
    int feature_index = dnx_data_l2_feature_no_trap_for_unknown_destination;
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
j2x_a0_dnx_data_l2_feature_mact_access_by_opportunistic_learning_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_feature;
    int feature_index = dnx_data_l2_feature_mact_access_by_opportunistic_learning;
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
j2x_a0_dnx_data_l2_feature_opportunistic_rejected_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_feature;
    int feature_index = dnx_data_l2_feature_opportunistic_rejected;
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
j2x_a0_dnx_data_l2_feature_flush_drop_stuck_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_feature;
    int feature_index = dnx_data_l2_feature_flush_drop_stuck;
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
j2x_a0_dnx_data_l2_feature_lif_flood_profile_always_update_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_feature;
    int feature_index = dnx_data_l2_feature_lif_flood_profile_always_update;
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
j2x_a0_dnx_data_l2_feature_olp_always_enters_source_cpu_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_feature;
    int feature_index = dnx_data_l2_feature_olp_always_enters_source_cpu;
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
j2x_a0_dnx_data_l2_feature_learn_payload_native_update_enable_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_feature;
    int feature_index = dnx_data_l2_feature_learn_payload_native_update_enable;
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
j2x_a0_dnx_data_l2_feature_accepted_data_with_vmv_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_feature;
    int feature_index = dnx_data_l2_feature_accepted_data_with_vmv;
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
j2x_a0_dnx_data_l2_vlan_domain_nof_vlan_domains_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_vlan_domain;
    int define_index = dnx_data_l2_vlan_domain_define_nof_vlan_domains;
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
j2x_a0_dnx_data_l2_age_and_flush_machine_flush_pulse_set(
    int unit)
{
    int l2_action_type_index;
    dnx_data_l2_age_and_flush_machine_flush_pulse_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_age_and_flush_machine;
    int table_index = dnx_data_l2_age_and_flush_machine_table_flush_pulse;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 1;
    table->info_get.key_size[0] = 1;

    
    table->values[0].default_val = "INVALIDr";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_l2_age_and_flush_machine_flush_pulse_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_l2_age_and_flush_machine_table_flush_pulse");

    
    default_data = (dnx_data_l2_age_and_flush_machine_flush_pulse_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->flush_pulse_reg = INVALIDr;
    
    for (l2_action_type_index = 0; l2_action_type_index < table->keys[0].size; l2_action_type_index++)
    {
        data = (dnx_data_l2_age_and_flush_machine_flush_pulse_t *) dnxc_data_mgmt_table_data_get(unit, table, l2_action_type_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if ((uint8)l2ActionTypeFwdAndNA < table->keys[0].size)
    {
        data = (dnx_data_l2_age_and_flush_machine_flush_pulse_t *) dnxc_data_mgmt_table_data_get(unit, table, (uint8)l2ActionTypeFwdAndNA, 0);
        data->flush_pulse_reg = MDB_REG_301Fr;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
j2x_a0_dnx_data_l2_olp_jr_mode_enhanced_performance_enable_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_olp;
    int define_index = dnx_data_l2_olp_define_jr_mode_enhanced_performance_enable;
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
j2x_a0_data_l2_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_l2;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_l2_submodule_general;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_l2_general_define_l2_egress_max_additional_termination_size_bytes;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_l2_general_l2_egress_max_additional_termination_size_bytes_set;
    data_index = dnx_data_l2_general_define_mact_mngmnt_fid_exceed_limit_int;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_l2_general_mact_mngmnt_fid_exceed_limit_int_set;
    data_index = dnx_data_l2_general_define_mact_lela_fid_exceed_limit_int;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_l2_general_mact_lela_fid_exceed_limit_int_set;

    
    data_index = dnx_data_l2_general_aging_support;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_l2_general_aging_support_set;
    data_index = dnx_data_l2_general_ivl_feature_support;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_l2_general_ivl_feature_support_set;

    
    
    submodule_index = dnx_data_l2_submodule_feature;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_l2_feature_age_out_and_refresh_profile_selection;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_l2_feature_age_out_and_refresh_profile_selection_set;
    data_index = dnx_data_l2_feature_age_machine_pause_after_flush;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_l2_feature_age_machine_pause_after_flush_set;
    data_index = dnx_data_l2_feature_age_state_not_updated;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_l2_feature_age_state_not_updated_set;
    data_index = dnx_data_l2_feature_eth_qual_is_mc;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_l2_feature_eth_qual_is_mc_set;
    data_index = dnx_data_l2_feature_bc_same_as_unknown_mc;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_l2_feature_bc_same_as_unknown_mc_set;
    data_index = dnx_data_l2_feature_wrong_limit_interrupt_handling;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_l2_feature_wrong_limit_interrupt_handling_set;
    data_index = dnx_data_l2_feature_transplant_instead_of_refresh;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_l2_feature_transplant_instead_of_refresh_set;
    data_index = dnx_data_l2_feature_static_mac_age_out;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_l2_feature_static_mac_age_out_set;
    data_index = dnx_data_l2_feature_vmv_for_limit_in_wrong_location;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_l2_feature_vmv_for_limit_in_wrong_location_set;
    data_index = dnx_data_l2_feature_learn_limit;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_l2_feature_learn_limit_set;
    data_index = dnx_data_l2_feature_learn_events_wrong_command;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_l2_feature_learn_events_wrong_command_set;
    data_index = dnx_data_l2_feature_opportunistic_learning_always_transplant;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_l2_feature_opportunistic_learning_always_transplant_set;
    data_index = dnx_data_l2_feature_exceed_limit_interrupt_by_insert_cmd;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_l2_feature_exceed_limit_interrupt_by_insert_cmd_set;
    data_index = dnx_data_l2_feature_ignore_limit_check;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_l2_feature_ignore_limit_check_set;
    data_index = dnx_data_l2_feature_no_trap_for_unknown_destination;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_l2_feature_no_trap_for_unknown_destination_set;
    data_index = dnx_data_l2_feature_mact_access_by_opportunistic_learning;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_l2_feature_mact_access_by_opportunistic_learning_set;
    data_index = dnx_data_l2_feature_opportunistic_rejected;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_l2_feature_opportunistic_rejected_set;
    data_index = dnx_data_l2_feature_flush_drop_stuck;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_l2_feature_flush_drop_stuck_set;
    data_index = dnx_data_l2_feature_lif_flood_profile_always_update;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_l2_feature_lif_flood_profile_always_update_set;
    data_index = dnx_data_l2_feature_olp_always_enters_source_cpu;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_l2_feature_olp_always_enters_source_cpu_set;
    data_index = dnx_data_l2_feature_learn_payload_native_update_enable;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_l2_feature_learn_payload_native_update_enable_set;
    data_index = dnx_data_l2_feature_accepted_data_with_vmv;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_l2_feature_accepted_data_with_vmv_set;

    
    
    submodule_index = dnx_data_l2_submodule_vlan_domain;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_l2_vlan_domain_define_nof_vlan_domains;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_l2_vlan_domain_nof_vlan_domains_set;

    

    
    
    submodule_index = dnx_data_l2_submodule_age_and_flush_machine;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnx_data_l2_age_and_flush_machine_table_flush_pulse;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_l2_age_and_flush_machine_flush_pulse_set;
    
    submodule_index = dnx_data_l2_submodule_olp;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_l2_olp_define_jr_mode_enhanced_performance_enable;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_l2_olp_jr_mode_enhanced_performance_enable_set;

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

