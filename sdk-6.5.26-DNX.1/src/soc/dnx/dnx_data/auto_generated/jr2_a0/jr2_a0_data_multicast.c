
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MULTICAST

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_multicast.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <bcm_int/dnx/algo/multicast/algo_multicast.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>







static shr_error_e
jr2_a0_dnx_data_multicast_general_is_used_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_multicast;
    int submodule_index = dnx_data_multicast_submodule_general;
    int feature_index = dnx_data_multicast_general_is_used;
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
jr2_a0_dnx_data_multicast_params_mcdb_formats_v2_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_multicast;
    int submodule_index = dnx_data_multicast_submodule_params;
    int feature_index = dnx_data_multicast_params_mcdb_formats_v2;
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
jr2_a0_dnx_data_multicast_params_mc_pruning_actions_not_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_multicast;
    int submodule_index = dnx_data_multicast_submodule_params;
    int feature_index = dnx_data_multicast_params_mc_pruning_actions_not_supported;
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
jr2_a0_dnx_data_multicast_params_max_ing_mc_groups_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_multicast;
    int submodule_index = dnx_data_multicast_submodule_params;
    int define_index = dnx_data_multicast_params_define_max_ing_mc_groups;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_multicast.params.nof_mcdb_entries_get(unit)/4-1;

    
    define->data = dnx_data_multicast.params.nof_mcdb_entries_get(unit)/4-1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_MULTICAST_INGRESS_GROUP_ID_RANGE_MAX;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range_signed;
    define->property.method_str = "range_signed";
    define->property.range_min = -1;
    define->property.range_max = dnx_data_multicast.params.nof_mcdb_entries_get(unit)-1;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_multicast_params_max_egr_mc_groups_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_multicast;
    int submodule_index = dnx_data_multicast_submodule_params;
    int define_index = dnx_data_multicast_params_define_max_egr_mc_groups;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_multicast.params.nof_mcdb_entries_get(unit)/4-1;

    
    define->data = dnx_data_multicast.params.nof_mcdb_entries_get(unit)/4-1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_MULTICAST_EGRESS_GROUP_ID_RANGE_MAX;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range_signed;
    define->property.method_str = "range_signed";
    define->property.range_min = -1;
    define->property.range_max = dnx_data_multicast.params.nof_mcdb_entries_get(unit)-1;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_multicast_params_nof_ing_mc_bitmaps_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_multicast;
    int submodule_index = dnx_data_multicast_submodule_params;
    int define_index = dnx_data_multicast_params_define_nof_ing_mc_bitmaps;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_MULTICAST_NOF_INGRESS_BITMAP;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = dnx_data_multicast.params.nof_mcdb_entries_get(unit)-1;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_multicast_params_nof_egr_mc_bitmaps_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_multicast;
    int submodule_index = dnx_data_multicast_submodule_params;
    int define_index = dnx_data_multicast_params_define_nof_egr_mc_bitmaps;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8192;

    
    define->data = 8192;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_MULTICAST_NOF_EGRESS_BITMAP;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = dnx_data_multicast.params.nof_mcdb_entries_get(unit)-1;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_multicast_params_mcdb_allocation_method_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_multicast;
    int submodule_index = dnx_data_multicast_submodule_params;
    int define_index = dnx_data_multicast_params_define_mcdb_allocation_method;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = DNX_ALGO_MULTICAST_ALLOCATION_BASIC;

    
    define->data = DNX_ALGO_MULTICAST_ALLOCATION_BASIC;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_multicast_params_nof_mcdb_entries_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_multicast;
    int submodule_index = dnx_data_multicast_submodule_params;
    int define_index = dnx_data_multicast_params_define_nof_mcdb_entries_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = utilex_log2_round_up(dnx_data_multicast.params.nof_mcdb_entries_get(unit));

    
    define->data = utilex_log2_round_up(dnx_data_multicast.params.nof_mcdb_entries_get(unit));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_multicast_params_ingr_dest_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_multicast;
    int submodule_index = dnx_data_multicast_submodule_params;
    int define_index = dnx_data_multicast_params_define_ingr_dest_bits;
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
jr2_a0_dnx_data_multicast_params_mcdb_block_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_multicast;
    int submodule_index = dnx_data_multicast_submodule_params;
    int define_index = dnx_data_multicast_params_define_mcdb_block_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8192;

    
    define->data = 8192;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_multicast_params_nof_mcdb_entries_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_multicast;
    int submodule_index = dnx_data_multicast_submodule_params;
    int define_index = dnx_data_multicast_params_define_nof_mcdb_entries;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8192*32;

    
    define->data = 8192*32;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_multicast_mcdb_hash_table_max_key_size_in_words_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_multicast;
    int submodule_index = dnx_data_multicast_submodule_mcdb_hash_table;
    int define_index = dnx_data_multicast_mcdb_hash_table_define_max_key_size_in_words;
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
jr2_a0_dnx_data_multicast_mcdb_hash_table_destination_start_bit_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_multicast;
    int submodule_index = dnx_data_multicast_submodule_mcdb_hash_table;
    int define_index = dnx_data_multicast_mcdb_hash_table_define_destination_start_bit;
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
jr2_a0_dnx_data_multicast_mcdb_hash_table_destination_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_multicast;
    int submodule_index = dnx_data_multicast_submodule_mcdb_hash_table;
    int define_index = dnx_data_multicast_mcdb_hash_table_define_destination_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 19;

    
    define->data = 19;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_multicast_mcdb_hash_table_reference_num_start_bit_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_multicast;
    int submodule_index = dnx_data_multicast_submodule_mcdb_hash_table;
    int define_index = dnx_data_multicast_mcdb_hash_table_define_reference_num_start_bit;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 19;

    
    define->data = 19;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_multicast_mcdb_hash_table_reference_num_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_multicast;
    int submodule_index = dnx_data_multicast_submodule_mcdb_hash_table;
    int define_index = dnx_data_multicast_mcdb_hash_table_define_reference_num_nof_bits;
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
jr2_a0_dnx_data_multicast_mcdb_hash_table_destination_type_start_bit_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_multicast;
    int submodule_index = dnx_data_multicast_submodule_mcdb_hash_table;
    int define_index = dnx_data_multicast_mcdb_hash_table_define_destination_type_start_bit;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 24;

    
    define->data = 24;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_multicast_mcdb_hash_table_destination_type_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_multicast;
    int submodule_index = dnx_data_multicast_submodule_mcdb_hash_table;
    int define_index = dnx_data_multicast_mcdb_hash_table_define_destination_type_nof_bits;
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
jr2_a0_dnx_data_multicast_mcdb_hash_table_info_start_bit_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_multicast;
    int submodule_index = dnx_data_multicast_submodule_mcdb_hash_table;
    int define_index = dnx_data_multicast_mcdb_hash_table_define_info_start_bit;
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
jr2_a0_dnx_data_multicast_mcdb_hash_table_info_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_multicast;
    int submodule_index = dnx_data_multicast_submodule_mcdb_hash_table;
    int define_index = dnx_data_multicast_mcdb_hash_table_define_info_nof_bits;
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
jr2_a0_dnx_data_multicast_mcdb_hash_table_group_id_start_bit_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_multicast;
    int submodule_index = dnx_data_multicast_submodule_mcdb_hash_table;
    int define_index = dnx_data_multicast_mcdb_hash_table_define_group_id_start_bit;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 58;

    
    define->data = 58;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_multicast_mcdb_hash_table_group_id_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_multicast;
    int submodule_index = dnx_data_multicast_submodule_mcdb_hash_table;
    int define_index = dnx_data_multicast_mcdb_hash_table_define_group_id_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 19;

    
    define->data = 19;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_multicast_mcdb_hash_table_is_ingress_start_bit_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_multicast;
    int submodule_index = dnx_data_multicast_submodule_mcdb_hash_table;
    int define_index = dnx_data_multicast_mcdb_hash_table_define_is_ingress_start_bit;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 77;

    
    define->data = 77;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_multicast_mcdb_hash_table_is_ingress_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_multicast;
    int submodule_index = dnx_data_multicast_submodule_mcdb_hash_table;
    int define_index = dnx_data_multicast_mcdb_hash_table_define_is_ingress_nof_bits;
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
jr2_a0_dnx_data_multicast_mcdb_hash_table_core_id_start_bit_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_multicast;
    int submodule_index = dnx_data_multicast_submodule_mcdb_hash_table;
    int define_index = dnx_data_multicast_mcdb_hash_table_define_core_id_start_bit;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 78;

    
    define->data = 78;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_multicast_mcdb_hash_table_core_id_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_multicast;
    int submodule_index = dnx_data_multicast_submodule_mcdb_hash_table;
    int define_index = dnx_data_multicast_mcdb_hash_table_define_core_id_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}




shr_error_e
jr2_a0_data_multicast_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_multicast;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_multicast_submodule_general;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_multicast_general_is_used;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_multicast_general_is_used_set;

    
    
    submodule_index = dnx_data_multicast_submodule_params;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_multicast_params_define_max_ing_mc_groups;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_multicast_params_max_ing_mc_groups_set;
    data_index = dnx_data_multicast_params_define_max_egr_mc_groups;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_multicast_params_max_egr_mc_groups_set;
    data_index = dnx_data_multicast_params_define_nof_ing_mc_bitmaps;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_multicast_params_nof_ing_mc_bitmaps_set;
    data_index = dnx_data_multicast_params_define_nof_egr_mc_bitmaps;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_multicast_params_nof_egr_mc_bitmaps_set;
    data_index = dnx_data_multicast_params_define_mcdb_allocation_method;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_multicast_params_mcdb_allocation_method_set;
    data_index = dnx_data_multicast_params_define_nof_mcdb_entries_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_multicast_params_nof_mcdb_entries_bits_set;
    data_index = dnx_data_multicast_params_define_ingr_dest_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_multicast_params_ingr_dest_bits_set;
    data_index = dnx_data_multicast_params_define_mcdb_block_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_multicast_params_mcdb_block_size_set;
    data_index = dnx_data_multicast_params_define_nof_mcdb_entries;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_multicast_params_nof_mcdb_entries_set;

    
    data_index = dnx_data_multicast_params_mcdb_formats_v2;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_multicast_params_mcdb_formats_v2_set;
    data_index = dnx_data_multicast_params_mc_pruning_actions_not_supported;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_multicast_params_mc_pruning_actions_not_supported_set;

    
    
    submodule_index = dnx_data_multicast_submodule_mcdb_hash_table;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_multicast_mcdb_hash_table_define_max_key_size_in_words;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_multicast_mcdb_hash_table_max_key_size_in_words_set;
    data_index = dnx_data_multicast_mcdb_hash_table_define_destination_start_bit;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_multicast_mcdb_hash_table_destination_start_bit_set;
    data_index = dnx_data_multicast_mcdb_hash_table_define_destination_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_multicast_mcdb_hash_table_destination_nof_bits_set;
    data_index = dnx_data_multicast_mcdb_hash_table_define_reference_num_start_bit;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_multicast_mcdb_hash_table_reference_num_start_bit_set;
    data_index = dnx_data_multicast_mcdb_hash_table_define_reference_num_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_multicast_mcdb_hash_table_reference_num_nof_bits_set;
    data_index = dnx_data_multicast_mcdb_hash_table_define_destination_type_start_bit;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_multicast_mcdb_hash_table_destination_type_start_bit_set;
    data_index = dnx_data_multicast_mcdb_hash_table_define_destination_type_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_multicast_mcdb_hash_table_destination_type_nof_bits_set;
    data_index = dnx_data_multicast_mcdb_hash_table_define_info_start_bit;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_multicast_mcdb_hash_table_info_start_bit_set;
    data_index = dnx_data_multicast_mcdb_hash_table_define_info_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_multicast_mcdb_hash_table_info_nof_bits_set;
    data_index = dnx_data_multicast_mcdb_hash_table_define_group_id_start_bit;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_multicast_mcdb_hash_table_group_id_start_bit_set;
    data_index = dnx_data_multicast_mcdb_hash_table_define_group_id_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_multicast_mcdb_hash_table_group_id_nof_bits_set;
    data_index = dnx_data_multicast_mcdb_hash_table_define_is_ingress_start_bit;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_multicast_mcdb_hash_table_is_ingress_start_bit_set;
    data_index = dnx_data_multicast_mcdb_hash_table_define_is_ingress_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_multicast_mcdb_hash_table_is_ingress_nof_bits_set;
    data_index = dnx_data_multicast_mcdb_hash_table_define_core_id_start_bit;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_multicast_mcdb_hash_table_core_id_start_bit_set;
    data_index = dnx_data_multicast_mcdb_hash_table_define_core_id_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_multicast_mcdb_hash_table_core_id_nof_bits_set;

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

