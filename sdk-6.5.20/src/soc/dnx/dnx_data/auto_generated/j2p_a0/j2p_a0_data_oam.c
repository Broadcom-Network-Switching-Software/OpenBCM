

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
j2p_a0_dnx_data_oam_general_max_oam_offset_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_general;
    int feature_index = dnx_data_oam_general_max_oam_offset_support;
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
j2p_a0_dnx_data_oam_general_oam_force_interface_for_meps_without_counter_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_general;
    int define_index = dnx_data_oam_general_define_oam_force_interface_for_meps_without_counter;
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
j2p_a0_dnx_data_oam_general_oam_my_cfm_mac_by_range_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_general;
    int define_index = dnx_data_oam_general_define_oam_my_cfm_mac_by_range;
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
j2p_a0_dnx_data_oam_general_oam_lm_read_per_sub_enable_field_exists_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_general;
    int define_index = dnx_data_oam_general_define_oam_lm_read_per_sub_enable_field_exists;
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
j2p_a0_dnx_data_oam_general_oam_lm_read_per_sub_disable_field_exists_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_general;
    int define_index = dnx_data_oam_general_define_oam_lm_read_per_sub_disable_field_exists;
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
j2p_a0_dnx_data_oam_general_oam_identification_appdb_1_field_exists_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_general;
    int define_index = dnx_data_oam_general_define_oam_identification_appdb_1_field_exists;
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
j2p_a0_dnx_data_oam_general_oam_nof_labels_for_sd_on_p_node_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_general;
    int define_index = dnx_data_oam_general_define_oam_nof_labels_for_sd_on_p_node;
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
j2p_a0_dnx_data_oam_general_oam_tod_config_in_eci_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_general;
    int define_index = dnx_data_oam_general_define_oam_tod_config_in_eci;
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
j2p_a0_dnx_data_oam_oamp_mep_db_da_is_mc_for_short_entries_support_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_mep_db_da_is_mc_for_short_entries_support;
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
j2p_a0_dnx_data_oam_oamp_rmep_db_entries_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_rmep_db_entries_nof_bits;
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
j2p_a0_dnx_data_oam_oamp_mep_db_version_limitation_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_mep_db_version_limitation;
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
j2p_a0_dnx_data_oam_oamp_oamp_mep_db_threshold_version_limitation_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_oamp_mep_db_threshold_version_limitation;
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
j2p_a0_dnx_data_oam_oamp_rmep_db_non_zero_lifetime_units_limitation_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_rmep_db_non_zero_lifetime_units_limitation;
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
j2p_a0_dnx_data_oam_oamp_crc_calculation_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_crc_calculation_supported;
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
j2p_a0_dnx_data_oam_oamp_modified_mep_db_structure_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_modified_mep_db_structure;
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
j2p_a0_dnx_data_oam_oamp_local_port_2_system_port_bits_set(
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
j2p_a0_dnx_data_oam_oamp_punt_packet_destination_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_punt_packet_destination_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 21;

    
    define->data = 21;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_oam_oamp_punt_packet_itmh_destination_full_encoding_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_punt_packet_itmh_destination_full_encoding_supported;
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
j2p_a0_dnx_data_oam_oamp_core_nof_bits_set(
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
j2p_a0_dnx_data_oam_oamp_punt_packet_pph_ttl_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_punt_packet_pph_ttl_supported;
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
j2p_a0_dnx_data_oam_oamp_lmm_dmm_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_lmm_dmm_supported;
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
j2p_a0_dnx_data_oam_oamp_slm_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_slm_supported;
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
j2p_a0_dnx_data_oam_oamp_mdb_tx_rate_lsb_field_present_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_mdb_tx_rate_lsb_field_present;
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
j2p_a0_dnx_data_oam_oamp_mdb_reserved_field_size_calculation_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_oamp;
    int define_index = dnx_data_oam_oamp_define_mdb_reserved_field_size_calculation;
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
j2p_a0_dnx_data_oam_feature_slr_scan_mashine_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_feature;
    int feature_index = dnx_data_oam_feature_slr_scan_mashine_support;
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
j2p_a0_dnx_data_oam_feature_oam_offset_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_feature;
    int feature_index = dnx_data_oam_feature_oam_offset_supported;
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
j2p_a0_dnx_data_oam_feature_oam_statistics_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_feature;
    int feature_index = dnx_data_oam_feature_oam_statistics_supported;
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
j2p_a0_dnx_data_oam_feature_lm_read_index_limitation_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_feature;
    int feature_index = dnx_data_oam_feature_lm_read_index_limitation;
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
j2p_a0_dnx_data_oam_feature_oamp_scanner_limitation_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_feature;
    int feature_index = dnx_data_oam_feature_oamp_scanner_limitation;
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
j2p_a0_dnx_data_oam_feature_oamp_tx_rate_limitation_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_feature;
    int feature_index = dnx_data_oam_feature_oamp_tx_rate_limitation;
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
j2p_a0_dnx_data_oam_feature_oamp_protection_rmep_id_limitation_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_feature;
    int feature_index = dnx_data_oam_feature_oamp_protection_rmep_id_limitation;
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
j2p_a0_dnx_data_oam_feature_oamp_generic_response_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_feature;
    int feature_index = dnx_data_oam_feature_oamp_generic_response_supported;
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
j2p_a0_dnx_data_oam_feature_oamp_txm_bank_limitation_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_oam;
    int submodule_index = dnx_data_oam_submodule_feature;
    int feature_index = dnx_data_oam_feature_oamp_txm_bank_limitation;
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
j2p_a0_data_oam_attach(
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

    
    data_index = dnx_data_oam_general_define_oam_force_interface_for_meps_without_counter;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_oam_general_oam_force_interface_for_meps_without_counter_set;
    data_index = dnx_data_oam_general_define_oam_my_cfm_mac_by_range;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_oam_general_oam_my_cfm_mac_by_range_set;
    data_index = dnx_data_oam_general_define_oam_lm_read_per_sub_enable_field_exists;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_oam_general_oam_lm_read_per_sub_enable_field_exists_set;
    data_index = dnx_data_oam_general_define_oam_lm_read_per_sub_disable_field_exists;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_oam_general_oam_lm_read_per_sub_disable_field_exists_set;
    data_index = dnx_data_oam_general_define_oam_identification_appdb_1_field_exists;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_oam_general_oam_identification_appdb_1_field_exists_set;
    data_index = dnx_data_oam_general_define_oam_nof_labels_for_sd_on_p_node;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_oam_general_oam_nof_labels_for_sd_on_p_node_set;
    data_index = dnx_data_oam_general_define_oam_tod_config_in_eci;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_oam_general_oam_tod_config_in_eci_set;

    
    data_index = dnx_data_oam_general_max_oam_offset_support;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_oam_general_max_oam_offset_support_set;

    
    
    submodule_index = dnx_data_oam_submodule_oamp;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_oam_oamp_define_mep_db_da_is_mc_for_short_entries_support;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_oam_oamp_mep_db_da_is_mc_for_short_entries_support_set;
    data_index = dnx_data_oam_oamp_define_rmep_db_entries_nof_bits;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_oam_oamp_rmep_db_entries_nof_bits_set;
    data_index = dnx_data_oam_oamp_define_mep_db_version_limitation;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_oam_oamp_mep_db_version_limitation_set;
    data_index = dnx_data_oam_oamp_define_oamp_mep_db_threshold_version_limitation;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_oam_oamp_oamp_mep_db_threshold_version_limitation_set;
    data_index = dnx_data_oam_oamp_define_rmep_db_non_zero_lifetime_units_limitation;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_oam_oamp_rmep_db_non_zero_lifetime_units_limitation_set;
    data_index = dnx_data_oam_oamp_define_crc_calculation_supported;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_oam_oamp_crc_calculation_supported_set;
    data_index = dnx_data_oam_oamp_define_modified_mep_db_structure;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_oam_oamp_modified_mep_db_structure_set;
    data_index = dnx_data_oam_oamp_define_local_port_2_system_port_bits;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_oam_oamp_local_port_2_system_port_bits_set;
    data_index = dnx_data_oam_oamp_define_punt_packet_destination_size;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_oam_oamp_punt_packet_destination_size_set;
    data_index = dnx_data_oam_oamp_define_punt_packet_itmh_destination_full_encoding_supported;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_oam_oamp_punt_packet_itmh_destination_full_encoding_supported_set;
    data_index = dnx_data_oam_oamp_define_core_nof_bits;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_oam_oamp_core_nof_bits_set;
    data_index = dnx_data_oam_oamp_define_punt_packet_pph_ttl_supported;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_oam_oamp_punt_packet_pph_ttl_supported_set;
    data_index = dnx_data_oam_oamp_define_lmm_dmm_supported;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_oam_oamp_lmm_dmm_supported_set;
    data_index = dnx_data_oam_oamp_define_slm_supported;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_oam_oamp_slm_supported_set;
    data_index = dnx_data_oam_oamp_define_mdb_tx_rate_lsb_field_present;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_oam_oamp_mdb_tx_rate_lsb_field_present_set;
    data_index = dnx_data_oam_oamp_define_mdb_reserved_field_size_calculation;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_oam_oamp_mdb_reserved_field_size_calculation_set;

    

    
    
    submodule_index = dnx_data_oam_submodule_feature;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_oam_feature_slr_scan_mashine_support;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_oam_feature_slr_scan_mashine_support_set;
    data_index = dnx_data_oam_feature_oam_offset_supported;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_oam_feature_oam_offset_supported_set;
    data_index = dnx_data_oam_feature_oam_statistics_supported;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_oam_feature_oam_statistics_supported_set;
    data_index = dnx_data_oam_feature_lm_read_index_limitation;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_oam_feature_lm_read_index_limitation_set;
    data_index = dnx_data_oam_feature_oamp_scanner_limitation;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_oam_feature_oamp_scanner_limitation_set;
    data_index = dnx_data_oam_feature_oamp_tx_rate_limitation;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_oam_feature_oamp_tx_rate_limitation_set;
    data_index = dnx_data_oam_feature_oamp_protection_rmep_id_limitation;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_oam_feature_oamp_protection_rmep_id_limitation_set;
    data_index = dnx_data_oam_feature_oamp_generic_response_supported;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_oam_feature_oamp_generic_response_supported_set;
    data_index = dnx_data_oam_feature_oamp_txm_bank_limitation;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_oam_feature_oamp_txm_bank_limitation_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

