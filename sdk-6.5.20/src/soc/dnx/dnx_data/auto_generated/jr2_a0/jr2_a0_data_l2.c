

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_L2

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_l2.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>







static shr_error_e
jr2_a0_dnx_data_l2_general_learning_use_insert_cmd_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_general;
    int feature_index = dnx_data_l2_general_learning_use_insert_cmd;
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
jr2_a0_dnx_data_l2_general_flush_machine_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_general;
    int feature_index = dnx_data_l2_general_flush_machine_support;
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
jr2_a0_dnx_data_l2_general_dynamic_entries_iteration_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_general;
    int feature_index = dnx_data_l2_general_dynamic_entries_iteration_support;
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
jr2_a0_dnx_data_l2_general_counters_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_general;
    int feature_index = dnx_data_l2_general_counters_support;
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
jr2_a0_dnx_data_l2_general_aging_support_set(
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
jr2_a0_dnx_data_l2_general_vsi_offset_shift_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_general;
    int define_index = dnx_data_l2_general_define_vsi_offset_shift;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 70;

    
    define->data = 70;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l2_general_lif_offset_shift_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_general;
    int define_index = dnx_data_l2_general_define_lif_offset_shift;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = -74;

    
    define->data = -74;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l2_general_l2_learn_limit_mode_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_general;
    int define_index = dnx_data_l2_general_define_l2_learn_limit_mode;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_L2_LEARN_LIMIT_MODE;
    define->property.doc = 
        "MACT learn limit mode\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 3;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "VLAN";
    define->property.mapping[0].val = 0;
    define->property.mapping[0].is_default = 1 ;
    define->property.mapping[1].name = "VLAN_PORT";
    define->property.mapping[1].val = 1;
    define->property.mapping[2].name = "TUNNEL";
    define->property.mapping[2].val = 2;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l2_general_jr_mode_nof_fec_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_general;
    int define_index = dnx_data_l2_general_define_jr_mode_nof_fec_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 17;

    
    define->data = 17;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l2_general_arad_plus_mode_nof_fec_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_general;
    int define_index = dnx_data_l2_general_define_arad_plus_mode_nof_fec_bits;
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
jr2_a0_dnx_data_l2_general_l2_egress_max_extention_size_bytes_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_general;
    int define_index = dnx_data_l2_general_define_l2_egress_max_extention_size_bytes;
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
jr2_a0_dnx_data_l2_general_l2_egress_max_additional_termination_size_bytes_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_general;
    int define_index = dnx_data_l2_general_define_l2_egress_max_additional_termination_size_bytes;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 192;

    
    define->data = 192;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_l2_feature_age_out_and_refresh_profile_selection_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_feature;
    int feature_index = dnx_data_l2_feature_age_out_and_refresh_profile_selection;
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
jr2_a0_dnx_data_l2_feature_age_machine_pause_after_flush_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_feature;
    int feature_index = dnx_data_l2_feature_age_machine_pause_after_flush;
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
jr2_a0_dnx_data_l2_feature_age_state_not_updated_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_feature;
    int feature_index = dnx_data_l2_feature_age_state_not_updated;
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
jr2_a0_dnx_data_l2_feature_appdb_id_for_olp_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_feature;
    int feature_index = dnx_data_l2_feature_appdb_id_for_olp;
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
jr2_a0_dnx_data_l2_feature_eth_qual_is_mc_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_feature;
    int feature_index = dnx_data_l2_feature_eth_qual_is_mc;
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
jr2_a0_dnx_data_l2_feature_bc_same_as_unknown_mc_set(
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
jr2_a0_dnx_data_l2_feature_fid_mgmt_ecc_error_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_feature;
    int feature_index = dnx_data_l2_feature_fid_mgmt_ecc_error;
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
jr2_a0_dnx_data_l2_feature_wrong_limit_interrupt_handling_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_feature;
    int feature_index = dnx_data_l2_feature_wrong_limit_interrupt_handling;
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
jr2_a0_dnx_data_l2_feature_transplant_instead_of_refresh_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_feature;
    int feature_index = dnx_data_l2_feature_transplant_instead_of_refresh;
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
jr2_a0_dnx_data_l2_feature_static_mac_age_out_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_feature;
    int feature_index = dnx_data_l2_feature_static_mac_age_out;
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
jr2_a0_dnx_data_l2_feature_vmv_for_limit_in_wrong_location_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_feature;
    int feature_index = dnx_data_l2_feature_vmv_for_limit_in_wrong_location;
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
jr2_a0_dnx_data_l2_feature_learn_limit_set(
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
jr2_a0_dnx_data_l2_feature_limit_per_lif_counters_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_feature;
    int feature_index = dnx_data_l2_feature_limit_per_lif_counters;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;
    
    feature->property.name = spn_L2_LEARN_LIMIT_MODE;
    feature->property.doc = NULL;
    feature->property.method = dnxc_data_property_method_direct_map;
    feature->property.method_str = "direct_map";
    feature->property.nof_mapping = 3;
    DNXC_DATA_ALLOC(feature->property.mapping , dnxc_data_property_map_t, feature->property.nof_mapping, "limit_per_lif_counters property mapping");

    feature->property.mapping[0].name = "VLAN";
    feature->property.mapping[0].val = 0;
    feature->property.mapping[0].is_default = 1 ;
    feature->property.mapping[1].name = "VLAN_PORT";
    feature->property.mapping[1].val = 1;
    feature->property.mapping[2].name = "TUNNEL";
    feature->property.mapping[2].val = 0;
    feature->property.mapping[2].is_default = 1 ;
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &feature->property, -1, &feature->data));
    
    feature->flags |= (DNXC_DATA_F_PROPERTY);


    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l2_feature_learn_events_wrong_command_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_feature;
    int feature_index = dnx_data_l2_feature_learn_events_wrong_command;
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
jr2_a0_dnx_data_l2_feature_opportunistic_learning_always_transplant_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_feature;
    int feature_index = dnx_data_l2_feature_opportunistic_learning_always_transplant;
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
jr2_a0_dnx_data_l2_feature_exceed_limit_interrupt_by_insert_cmd_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_feature;
    int feature_index = dnx_data_l2_feature_exceed_limit_interrupt_by_insert_cmd;
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
jr2_a0_dnx_data_l2_feature_refresh_events_wrong_key_msbs_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_feature;
    int feature_index = dnx_data_l2_feature_refresh_events_wrong_key_msbs;
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
jr2_a0_dnx_data_l2_feature_ignore_limit_check_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_feature;
    int feature_index = dnx_data_l2_feature_ignore_limit_check;
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
jr2_a0_dnx_data_l2_vsi_nof_vsi_aging_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_vsi;
    int define_index = dnx_data_l2_vsi_define_nof_vsi_aging_profiles;
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
jr2_a0_dnx_data_l2_vsi_nof_event_forwarding_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_vsi;
    int define_index = dnx_data_l2_vsi_define_nof_event_forwarding_profiles;
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
jr2_a0_dnx_data_l2_vsi_nof_vsi_learning_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_vsi;
    int define_index = dnx_data_l2_vsi_define_nof_vsi_learning_profiles;
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
jr2_a0_dnx_data_l2_vlan_domain_nof_vlan_domains_set(
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
jr2_a0_dnx_data_l2_vlan_domain_nof_bits_next_layer_network_domain_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_vlan_domain;
    int define_index = dnx_data_l2_vlan_domain_define_nof_bits_next_layer_network_domain;
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
jr2_a0_dnx_data_l2_dma_flush_nof_dma_entries_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_dma;
    int define_index = dnx_data_l2_dma_define_flush_nof_dma_entries;
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
jr2_a0_dnx_data_l2_dma_flush_db_nof_dma_rules_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_dma;
    int define_index = dnx_data_l2_dma_define_flush_db_nof_dma_rules;
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
jr2_a0_dnx_data_l2_dma_flush_db_nof_dma_rules_per_table_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_dma;
    int define_index = dnx_data_l2_dma_define_flush_db_nof_dma_rules_per_table;
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
jr2_a0_dnx_data_l2_dma_flush_db_rule_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_dma;
    int define_index = dnx_data_l2_dma_define_flush_db_rule_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 67;

    
    define->data = 67;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l2_dma_flush_db_data_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_dma;
    int define_index = dnx_data_l2_dma_define_flush_db_data_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 25;

    
    define->data = 25;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l2_dma_flush_group_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_dma;
    int define_index = dnx_data_l2_dma_define_flush_group_size;
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
jr2_a0_dnx_data_l2_dma_learning_fifo_dma_buffer_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_dma;
    int define_index = dnx_data_l2_dma_define_learning_fifo_dma_buffer_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 200000;

    
    define->data = 200000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_LEARNING_FIFO_DMA_BUFFER_SIZE;
    define->property.doc = 
        "\n"
        "Configure the size of the host memory in bytes.\n"
        "The valid range of memory size is 20-327680 bytes.\n"
        "learning_fifo_dma_buffer_size=20-327680\n"
        "Default: 200000\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 20;
    define->property.range_max = 327680;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l2_dma_learning_fifo_dma_timeout_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_dma;
    int define_index = dnx_data_l2_dma_define_learning_fifo_dma_timeout;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32767;

    
    define->data = 32767;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_LEARNING_FIFO_DMA_TIMEOUT;
    define->property.doc = 
        "\n"
        "Configures the learning fifo dma timeout in microseconds.\n"
        "The valid range is 0-65535.\n"
        "learning_fifo_dma_timeout=0-65535\n"
        "Default: 32767\n"
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
jr2_a0_dnx_data_l2_dma_learning_fifo_dma_threshold_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_dma;
    int define_index = dnx_data_l2_dma_define_learning_fifo_dma_threshold;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_LEARNING_FIFO_DMA_THRESHOLD;
    define->property.doc = 
        "\n"
        "Configures the learning fifo dma threshold.\n"
        "The valid range is 1-16384.\n"
        "learning_fifo_dma_threshold=1-16384\n"
        "Default: 4\n"
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
jr2_a0_dnx_data_l2_age_and_flush_machine_max_age_states_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_age_and_flush_machine;
    int define_index = dnx_data_l2_age_and_flush_machine_define_max_age_states;
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
jr2_a0_dnx_data_l2_age_and_flush_machine_filter_rules_set(
    int unit)
{
    dnx_data_l2_age_and_flush_machine_filter_rules_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_age_and_flush_machine;
    int table_index = dnx_data_l2_age_and_flush_machine_table_filter_rules;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "MDB_MEM_23000m";
    table->values[1].default_val = "ITEM_0_0f";
    table->values[2].default_val = "ITEM_8_264f";
    table->values[3].default_val = "ITEM_275_531f";
    table->values[4].default_val = "ITEM_265_267f";
    table->values[5].default_val = "ITEM_532_534f";
    table->values[6].default_val = "ITEM_2_7f";
    table->values[7].default_val = "ITEM_269_274f";
    table->values[8].default_val = "ITEM_1_1f";
    table->values[9].default_val = "ITEM_268_268f";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_l2_age_and_flush_machine_filter_rules_t, (1 + 1  ), "data of dnx_data_l2_age_and_flush_machine_table_filter_rules");

    
    default_data = (dnx_data_l2_age_and_flush_machine_filter_rules_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->table_name = MDB_MEM_23000m;
    default_data->valid = ITEM_0_0f;
    default_data->entry_mask = ITEM_8_264f;
    default_data->entry_filter = ITEM_275_531f;
    default_data->src_mask = ITEM_265_267f;
    default_data->src_filter = ITEM_532_534f;
    default_data->appdb_id_mask = ITEM_2_7f;
    default_data->appdb_id_filter = ITEM_269_274f;
    default_data->accessed_mask = ITEM_1_1f;
    default_data->accessed_filter = ITEM_268_268f;
    
    data = (dnx_data_l2_age_and_flush_machine_filter_rules_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l2_age_and_flush_machine_data_rules_set(
    int unit)
{
    dnx_data_l2_age_and_flush_machine_data_rules_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_age_and_flush_machine;
    int table_index = dnx_data_l2_age_and_flush_machine_table_data_rules;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "MDB_MEM_24000m";
    table->values[1].default_val = "ITEM_0_3f";
    table->values[2].default_val = "ITEM_4_99f";
    table->values[3].default_val = "ITEM_100_195f";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_l2_age_and_flush_machine_data_rules_t, (1 + 1  ), "data of dnx_data_l2_age_and_flush_machine_table_data_rules");

    
    default_data = (dnx_data_l2_age_and_flush_machine_data_rules_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->table_name = MDB_MEM_24000m;
    default_data->command = ITEM_0_3f;
    default_data->payload_mask = ITEM_4_99f;
    default_data->payload = ITEM_100_195f;
    
    data = (dnx_data_l2_age_and_flush_machine_data_rules_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l2_age_and_flush_machine_flush_set(
    int unit)
{
    dnx_data_l2_age_and_flush_machine_flush_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_age_and_flush_machine;
    int table_index = dnx_data_l2_age_and_flush_machine_table_flush;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "MDB_REG_3021r";
    table->values[1].default_val = "50";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_l2_age_and_flush_machine_flush_t, (1 + 1  ), "data of dnx_data_l2_age_and_flush_machine_table_flush");

    
    default_data = (dnx_data_l2_age_and_flush_machine_flush_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->flush_pulse = MDB_REG_3021r;
    default_data->traverse_thread_priority = 50;
    
    data = (dnx_data_l2_age_and_flush_machine_flush_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);
    
    table->values[1].property.name = spn_L2_FLUSH_TRAVERSE_THREAD_PRIORITY;
    table->values[1].property.doc =
        "L2 flush match traverse non-blocking thread priority\n"
    ;
    table->values[1].property.method = dnxc_data_property_method_range;
    table->values[1].property.method_str = "range";
    table->values[1].property.range_min = 0;
    table->values[1].property.range_max = 0xFFFF;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    data = (dnx_data_l2_age_and_flush_machine_flush_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[1].property, -1, &data->traverse_thread_priority));
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l2_age_and_flush_machine_age_set(
    int unit)
{
    dnx_data_l2_age_and_flush_machine_age_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_age_and_flush_machine;
    int table_index = dnx_data_l2_age_and_flush_machine_table_age;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "MDB_REG_181r";
    table->values[1].default_val = "ITEM_360_367f";
    table->values[2].default_val = "MDB_REG_3020r";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_l2_age_and_flush_machine_age_t, (1 + 1  ), "data of dnx_data_l2_age_and_flush_machine_table_age");

    
    default_data = (dnx_data_l2_age_and_flush_machine_age_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->age_config = MDB_REG_181r;
    default_data->disable_aging = ITEM_360_367f;
    default_data->scan_pulse = MDB_REG_3020r;
    
    data = (dnx_data_l2_age_and_flush_machine_age_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
jr2_a0_dnx_data_l2_olp_refresh_events_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_olp;
    int feature_index = dnx_data_l2_olp_refresh_events_support;
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
jr2_a0_dnx_data_l2_olp_olp_learn_payload_initial_value_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_olp;
    int feature_index = dnx_data_l2_olp_olp_learn_payload_initial_value_supported;
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
jr2_a0_dnx_data_l2_olp_dsp_messages_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_olp;
    int feature_index = dnx_data_l2_olp_dsp_messages_support;
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
jr2_a0_dnx_data_l2_olp_lpkgv_shift_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_olp;
    int define_index = dnx_data_l2_olp_define_lpkgv_shift;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 26;

    
    define->data = 26;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l2_olp_lpkgv_mask_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_olp;
    int define_index = dnx_data_l2_olp_define_lpkgv_mask;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x3f;

    
    define->data = 0x3f;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l2_olp_lpkgv_with_outlif_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_olp;
    int define_index = dnx_data_l2_olp_define_lpkgv_with_outlif;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x20;

    
    define->data = 0x20;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l2_olp_lpkgv_wo_outlif_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_olp;
    int define_index = dnx_data_l2_olp_define_lpkgv_wo_outlif;
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
jr2_a0_dnx_data_l2_olp_destination_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_olp;
    int define_index = dnx_data_l2_olp_define_destination_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 31;

    
    define->data = 31;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l2_olp_outlif_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_olp;
    int define_index = dnx_data_l2_olp_define_outlif_offset;
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
jr2_a0_dnx_data_l2_olp_eei_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_olp;
    int define_index = dnx_data_l2_olp_define_eei_offset;
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
jr2_a0_dnx_data_l2_olp_fec_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_olp;
    int define_index = dnx_data_l2_olp_define_fec_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 31;

    
    define->data = 31;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l2_olp_jr_mode_enhanced_performance_enable_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_olp;
    int define_index = dnx_data_l2_olp_define_jr_mode_enhanced_performance_enable;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}




shr_error_e
jr2_a0_data_l2_attach(
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

    
    data_index = dnx_data_l2_general_define_vsi_offset_shift;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l2_general_vsi_offset_shift_set;
    data_index = dnx_data_l2_general_define_lif_offset_shift;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l2_general_lif_offset_shift_set;
    data_index = dnx_data_l2_general_define_l2_learn_limit_mode;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l2_general_l2_learn_limit_mode_set;
    data_index = dnx_data_l2_general_define_jr_mode_nof_fec_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l2_general_jr_mode_nof_fec_bits_set;
    data_index = dnx_data_l2_general_define_arad_plus_mode_nof_fec_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l2_general_arad_plus_mode_nof_fec_bits_set;
    data_index = dnx_data_l2_general_define_l2_egress_max_extention_size_bytes;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l2_general_l2_egress_max_extention_size_bytes_set;
    data_index = dnx_data_l2_general_define_l2_egress_max_additional_termination_size_bytes;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l2_general_l2_egress_max_additional_termination_size_bytes_set;

    
    data_index = dnx_data_l2_general_learning_use_insert_cmd;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l2_general_learning_use_insert_cmd_set;
    data_index = dnx_data_l2_general_flush_machine_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l2_general_flush_machine_support_set;
    data_index = dnx_data_l2_general_dynamic_entries_iteration_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l2_general_dynamic_entries_iteration_support_set;
    data_index = dnx_data_l2_general_counters_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l2_general_counters_support_set;
    data_index = dnx_data_l2_general_aging_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l2_general_aging_support_set;

    
    
    submodule_index = dnx_data_l2_submodule_feature;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_l2_feature_age_out_and_refresh_profile_selection;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l2_feature_age_out_and_refresh_profile_selection_set;
    data_index = dnx_data_l2_feature_age_machine_pause_after_flush;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l2_feature_age_machine_pause_after_flush_set;
    data_index = dnx_data_l2_feature_age_state_not_updated;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l2_feature_age_state_not_updated_set;
    data_index = dnx_data_l2_feature_appdb_id_for_olp;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l2_feature_appdb_id_for_olp_set;
    data_index = dnx_data_l2_feature_eth_qual_is_mc;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l2_feature_eth_qual_is_mc_set;
    data_index = dnx_data_l2_feature_bc_same_as_unknown_mc;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l2_feature_bc_same_as_unknown_mc_set;
    data_index = dnx_data_l2_feature_fid_mgmt_ecc_error;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l2_feature_fid_mgmt_ecc_error_set;
    data_index = dnx_data_l2_feature_wrong_limit_interrupt_handling;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l2_feature_wrong_limit_interrupt_handling_set;
    data_index = dnx_data_l2_feature_transplant_instead_of_refresh;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l2_feature_transplant_instead_of_refresh_set;
    data_index = dnx_data_l2_feature_static_mac_age_out;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l2_feature_static_mac_age_out_set;
    data_index = dnx_data_l2_feature_vmv_for_limit_in_wrong_location;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l2_feature_vmv_for_limit_in_wrong_location_set;
    data_index = dnx_data_l2_feature_learn_limit;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l2_feature_learn_limit_set;
    data_index = dnx_data_l2_feature_limit_per_lif_counters;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l2_feature_limit_per_lif_counters_set;
    data_index = dnx_data_l2_feature_learn_events_wrong_command;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l2_feature_learn_events_wrong_command_set;
    data_index = dnx_data_l2_feature_opportunistic_learning_always_transplant;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l2_feature_opportunistic_learning_always_transplant_set;
    data_index = dnx_data_l2_feature_exceed_limit_interrupt_by_insert_cmd;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l2_feature_exceed_limit_interrupt_by_insert_cmd_set;
    data_index = dnx_data_l2_feature_refresh_events_wrong_key_msbs;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l2_feature_refresh_events_wrong_key_msbs_set;
    data_index = dnx_data_l2_feature_ignore_limit_check;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l2_feature_ignore_limit_check_set;

    
    
    submodule_index = dnx_data_l2_submodule_vsi;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_l2_vsi_define_nof_vsi_aging_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l2_vsi_nof_vsi_aging_profiles_set;
    data_index = dnx_data_l2_vsi_define_nof_event_forwarding_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l2_vsi_nof_event_forwarding_profiles_set;
    data_index = dnx_data_l2_vsi_define_nof_vsi_learning_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l2_vsi_nof_vsi_learning_profiles_set;

    

    
    
    submodule_index = dnx_data_l2_submodule_vlan_domain;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_l2_vlan_domain_define_nof_vlan_domains;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l2_vlan_domain_nof_vlan_domains_set;
    data_index = dnx_data_l2_vlan_domain_define_nof_bits_next_layer_network_domain;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l2_vlan_domain_nof_bits_next_layer_network_domain_set;

    

    
    
    submodule_index = dnx_data_l2_submodule_dma;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_l2_dma_define_flush_nof_dma_entries;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l2_dma_flush_nof_dma_entries_set;
    data_index = dnx_data_l2_dma_define_flush_db_nof_dma_rules;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l2_dma_flush_db_nof_dma_rules_set;
    data_index = dnx_data_l2_dma_define_flush_db_nof_dma_rules_per_table;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l2_dma_flush_db_nof_dma_rules_per_table_set;
    data_index = dnx_data_l2_dma_define_flush_db_rule_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l2_dma_flush_db_rule_size_set;
    data_index = dnx_data_l2_dma_define_flush_db_data_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l2_dma_flush_db_data_size_set;
    data_index = dnx_data_l2_dma_define_flush_group_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l2_dma_flush_group_size_set;
    data_index = dnx_data_l2_dma_define_learning_fifo_dma_buffer_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l2_dma_learning_fifo_dma_buffer_size_set;
    data_index = dnx_data_l2_dma_define_learning_fifo_dma_timeout;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l2_dma_learning_fifo_dma_timeout_set;
    data_index = dnx_data_l2_dma_define_learning_fifo_dma_threshold;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l2_dma_learning_fifo_dma_threshold_set;

    

    
    
    submodule_index = dnx_data_l2_submodule_age_and_flush_machine;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_l2_age_and_flush_machine_define_max_age_states;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l2_age_and_flush_machine_max_age_states_set;

    

    
    data_index = dnx_data_l2_age_and_flush_machine_table_filter_rules;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_l2_age_and_flush_machine_filter_rules_set;
    data_index = dnx_data_l2_age_and_flush_machine_table_data_rules;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_l2_age_and_flush_machine_data_rules_set;
    data_index = dnx_data_l2_age_and_flush_machine_table_flush;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_l2_age_and_flush_machine_flush_set;
    data_index = dnx_data_l2_age_and_flush_machine_table_age;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_l2_age_and_flush_machine_age_set;
    
    submodule_index = dnx_data_l2_submodule_olp;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_l2_olp_define_lpkgv_shift;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l2_olp_lpkgv_shift_set;
    data_index = dnx_data_l2_olp_define_lpkgv_mask;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l2_olp_lpkgv_mask_set;
    data_index = dnx_data_l2_olp_define_lpkgv_with_outlif;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l2_olp_lpkgv_with_outlif_set;
    data_index = dnx_data_l2_olp_define_lpkgv_wo_outlif;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l2_olp_lpkgv_wo_outlif_set;
    data_index = dnx_data_l2_olp_define_destination_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l2_olp_destination_offset_set;
    data_index = dnx_data_l2_olp_define_outlif_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l2_olp_outlif_offset_set;
    data_index = dnx_data_l2_olp_define_eei_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l2_olp_eei_offset_set;
    data_index = dnx_data_l2_olp_define_fec_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l2_olp_fec_offset_set;
    data_index = dnx_data_l2_olp_define_jr_mode_enhanced_performance_enable;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l2_olp_jr_mode_enhanced_performance_enable_set;

    
    data_index = dnx_data_l2_olp_refresh_events_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l2_olp_refresh_events_support_set;
    data_index = dnx_data_l2_olp_olp_learn_payload_initial_value_supported;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l2_olp_olp_learn_payload_initial_value_supported_set;
    data_index = dnx_data_l2_olp_dsp_messages_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l2_olp_dsp_messages_support_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

