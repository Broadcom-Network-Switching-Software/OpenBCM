

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TRUNK

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_trunk.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>








static shr_error_e
jr2_a0_dnx_data_trunk_parameters_nof_pools_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trunk;
    int submodule_index = dnx_data_trunk_submodule_parameters;
    int define_index = dnx_data_trunk_parameters_define_nof_pools;
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
jr2_a0_dnx_data_trunk_parameters_max_nof_members_in_pool_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trunk;
    int submodule_index = dnx_data_trunk_submodule_parameters;
    int define_index = dnx_data_trunk_parameters_define_max_nof_members_in_pool;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 16384;

    
    define->data = 16384;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trunk_parameters_max_nof_groups_in_pool_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trunk;
    int submodule_index = dnx_data_trunk_submodule_parameters;
    int define_index = dnx_data_trunk_parameters_define_max_nof_groups_in_pool;
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
jr2_a0_dnx_data_trunk_parameters_spa_pool_shift_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trunk;
    int submodule_index = dnx_data_trunk_submodule_parameters;
    int define_index = dnx_data_trunk_parameters_define_spa_pool_shift;
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
jr2_a0_dnx_data_trunk_parameters_spa_type_shift_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trunk;
    int submodule_index = dnx_data_trunk_submodule_parameters;
    int define_index = dnx_data_trunk_parameters_define_spa_type_shift;
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
jr2_a0_dnx_data_trunk_parameters_pool_info_set(
    int unit)
{
    int pool_index_index;
    dnx_data_trunk_parameters_pool_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_trunk;
    int submodule_index = dnx_data_trunk_submodule_parameters;
    int table_index = dnx_data_trunk_parameters_table_pool_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 2;
    table->info_get.key_size[0] = 2;

    
    table->values[0].default_val = "64";
    table->values[1].default_val = "256";
    table->values[2].default_val = "0x5";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_trunk_parameters_pool_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_trunk_parameters_table_pool_info");

    
    default_data = (dnx_data_trunk_parameters_pool_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->max_nof_members_in_group = 64;
    default_data->max_nof_groups_in_pool = 256;
    default_data->pool_hw_mode = 0x5;
    
    for (pool_index_index = 0; pool_index_index < table->keys[0].size; pool_index_index++)
    {
        data = (dnx_data_trunk_parameters_pool_info_t *) dnxc_data_mgmt_table_data_get(unit, table, pool_index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->values[0].property.name = spn_TRUNK_GROUP_MAX_MEMBERS;
    table->values[0].property.doc =
        "SOC property to configure the max amount of members in each trunk in bank. This also determines the amount of trunk IDs in bank\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_suffix_direct_map;
    table->values[0].property.method_str = "suffix_direct_map";
    table->values[0].property.suffix = "pool";
    table->values[0].property.nof_mapping = 8;
    DNXC_DATA_ALLOC(table->values[0].property.mapping, dnxc_data_property_map_t, table->values[0].property.nof_mapping, "dnx_data_trunk_parameters_pool_info_t property mapping");

    table->values[0].property.mapping[0].name = "2";
    table->values[0].property.mapping[0].val = 2;
    table->values[0].property.mapping[1].name = "4";
    table->values[0].property.mapping[1].val = 4;
    table->values[0].property.mapping[2].name = "8";
    table->values[0].property.mapping[2].val = 8;
    table->values[0].property.mapping[3].name = "16";
    table->values[0].property.mapping[3].val = 16;
    table->values[0].property.mapping[4].name = "32";
    table->values[0].property.mapping[4].val = 32;
    table->values[0].property.mapping[5].name = "64";
    table->values[0].property.mapping[5].val = 64;
    table->values[0].property.mapping[5].is_default = 1 ;
    table->values[0].property.mapping[6].name = "128";
    table->values[0].property.mapping[6].val = 128;
    table->values[0].property.mapping[7].name = "256";
    table->values[0].property.mapping[7].val = 256;
    
    table->values[1].property.name = spn_TRUNK_GROUP_MAX_MEMBERS;
    table->values[1].property.doc = NULL;
    table->values[1].property.method = dnxc_data_property_method_suffix_direct_map;
    table->values[1].property.method_str = "suffix_direct_map";
    table->values[1].property.suffix = "pool";
    table->values[1].property.nof_mapping = 8;
    DNXC_DATA_ALLOC(table->values[1].property.mapping, dnxc_data_property_map_t, table->values[1].property.nof_mapping, "dnx_data_trunk_parameters_pool_info_t property mapping");

    table->values[1].property.mapping[0].name = "2";
    table->values[1].property.mapping[0].val = 8192;
    table->values[1].property.mapping[1].name = "4";
    table->values[1].property.mapping[1].val = 4096;
    table->values[1].property.mapping[2].name = "8";
    table->values[1].property.mapping[2].val = 2048;
    table->values[1].property.mapping[3].name = "16";
    table->values[1].property.mapping[3].val = 1024;
    table->values[1].property.mapping[4].name = "32";
    table->values[1].property.mapping[4].val = 512;
    table->values[1].property.mapping[5].name = "64";
    table->values[1].property.mapping[5].val = 256;
    table->values[1].property.mapping[5].is_default = 1 ;
    table->values[1].property.mapping[6].name = "128";
    table->values[1].property.mapping[6].val = 128;
    table->values[1].property.mapping[7].name = "256";
    table->values[1].property.mapping[7].val = 64;
    
    table->values[2].property.name = spn_TRUNK_GROUP_MAX_MEMBERS;
    table->values[2].property.doc = NULL;
    table->values[2].property.method = dnxc_data_property_method_suffix_direct_map;
    table->values[2].property.method_str = "suffix_direct_map";
    table->values[2].property.suffix = "pool";
    table->values[2].property.nof_mapping = 8;
    DNXC_DATA_ALLOC(table->values[2].property.mapping, dnxc_data_property_map_t, table->values[2].property.nof_mapping, "dnx_data_trunk_parameters_pool_info_t property mapping");

    table->values[2].property.mapping[0].name = "2";
    table->values[2].property.mapping[0].val = 0;
    table->values[2].property.mapping[1].name = "4";
    table->values[2].property.mapping[1].val = 1;
    table->values[2].property.mapping[2].name = "8";
    table->values[2].property.mapping[2].val = 2;
    table->values[2].property.mapping[3].name = "16";
    table->values[2].property.mapping[3].val = 3;
    table->values[2].property.mapping[4].name = "32";
    table->values[2].property.mapping[4].val = 4;
    table->values[2].property.mapping[5].name = "64";
    table->values[2].property.mapping[5].val = 5;
    table->values[2].property.mapping[6].name = "128";
    table->values[2].property.mapping[6].val = 6;
    table->values[2].property.mapping[7].name = "256";
    table->values[2].property.mapping[7].val = 7;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (pool_index_index = 0; pool_index_index < table->keys[0].size; pool_index_index++)
    {
        data = (dnx_data_trunk_parameters_pool_info_t *) dnxc_data_mgmt_table_data_get(unit, table, pool_index_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[0].property, pool_index_index, &data->max_nof_members_in_group));
    }
    for (pool_index_index = 0; pool_index_index < table->keys[0].size; pool_index_index++)
    {
        data = (dnx_data_trunk_parameters_pool_info_t *) dnxc_data_mgmt_table_data_get(unit, table, pool_index_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[1].property, pool_index_index, &data->max_nof_groups_in_pool));
    }
    for (pool_index_index = 0; pool_index_index < table->keys[0].size; pool_index_index++)
    {
        data = (dnx_data_trunk_parameters_pool_info_t *) dnxc_data_mgmt_table_data_get(unit, table, pool_index_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[2].property, pool_index_index, &data->pool_hw_mode));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
jr2_a0_dnx_data_trunk_psc_multiply_and_divide_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_trunk;
    int submodule_index = dnx_data_trunk_submodule_psc;
    int feature_index = dnx_data_trunk_psc_multiply_and_divide;
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
jr2_a0_dnx_data_trunk_psc_smooth_division_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_trunk;
    int submodule_index = dnx_data_trunk_submodule_psc;
    int feature_index = dnx_data_trunk_psc_smooth_division;
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
jr2_a0_dnx_data_trunk_psc_consistant_hashing_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_trunk;
    int submodule_index = dnx_data_trunk_submodule_psc;
    int feature_index = dnx_data_trunk_psc_consistant_hashing;
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
jr2_a0_dnx_data_trunk_psc_consistant_hashing_small_group_size_in_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trunk;
    int submodule_index = dnx_data_trunk_submodule_psc;
    int define_index = dnx_data_trunk_psc_define_consistant_hashing_small_group_size_in_bits;
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
jr2_a0_dnx_data_trunk_psc_smooth_division_max_nof_member_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trunk;
    int submodule_index = dnx_data_trunk_submodule_psc;
    int define_index = dnx_data_trunk_psc_define_smooth_division_max_nof_member;
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
jr2_a0_dnx_data_trunk_psc_smooth_division_entries_per_profile_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trunk;
    int submodule_index = dnx_data_trunk_submodule_psc;
    int define_index = dnx_data_trunk_psc_define_smooth_division_entries_per_profile;
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
jr2_a0_dnx_data_trunk_egress_trunk_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trunk;
    int submodule_index = dnx_data_trunk_submodule_egress_trunk;
    int define_index = dnx_data_trunk_egress_trunk_define_nof;
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
jr2_a0_dnx_data_trunk_egress_trunk_nof_lb_keys_per_profile_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trunk;
    int submodule_index = dnx_data_trunk_submodule_egress_trunk;
    int define_index = dnx_data_trunk_egress_trunk_define_nof_lb_keys_per_profile;
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
jr2_a0_dnx_data_trunk_egress_trunk_dbal_egress_trunk_index_multiplier_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trunk;
    int submodule_index = dnx_data_trunk_submodule_egress_trunk;
    int define_index = dnx_data_trunk_egress_trunk_define_dbal_egress_trunk_index_multiplier;
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
jr2_a0_dnx_data_trunk_egress_trunk_dbal_entry_index_divider_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trunk;
    int submodule_index = dnx_data_trunk_submodule_egress_trunk;
    int define_index = dnx_data_trunk_egress_trunk_define_dbal_entry_index_divider;
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
jr2_a0_dnx_data_trunk_egress_trunk_dbal_entry_index_msb_multiplier_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trunk;
    int submodule_index = dnx_data_trunk_submodule_egress_trunk;
    int define_index = dnx_data_trunk_egress_trunk_define_dbal_entry_index_msb_multiplier;
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
jr2_a0_dnx_data_trunk_egress_trunk_invalid_pp_dsp_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trunk;
    int submodule_index = dnx_data_trunk_submodule_egress_trunk;
    int define_index = dnx_data_trunk_egress_trunk_define_invalid_pp_dsp;
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
jr2_a0_dnx_data_trunk_graceful_allow_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_trunk;
    int submodule_index = dnx_data_trunk_submodule_graceful;
    int feature_index = dnx_data_trunk_graceful_allow;
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
jr2_a0_dnx_data_trunk_graceful_first_hw_configuration_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trunk;
    int submodule_index = dnx_data_trunk_submodule_graceful;
    int define_index = dnx_data_trunk_graceful_define_first_hw_configuration;
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
jr2_a0_dnx_data_trunk_graceful_second_hw_configuration_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trunk;
    int submodule_index = dnx_data_trunk_submodule_graceful;
    int define_index = dnx_data_trunk_graceful_define_second_hw_configuration;
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
jr2_a0_data_trunk_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_trunk;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_trunk_submodule_parameters;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_trunk_parameters_define_nof_pools;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trunk_parameters_nof_pools_set;
    data_index = dnx_data_trunk_parameters_define_max_nof_members_in_pool;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trunk_parameters_max_nof_members_in_pool_set;
    data_index = dnx_data_trunk_parameters_define_max_nof_groups_in_pool;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trunk_parameters_max_nof_groups_in_pool_set;
    data_index = dnx_data_trunk_parameters_define_spa_pool_shift;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trunk_parameters_spa_pool_shift_set;
    data_index = dnx_data_trunk_parameters_define_spa_type_shift;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trunk_parameters_spa_type_shift_set;

    

    
    data_index = dnx_data_trunk_parameters_table_pool_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_trunk_parameters_pool_info_set;
    
    submodule_index = dnx_data_trunk_submodule_psc;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_trunk_psc_define_consistant_hashing_small_group_size_in_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trunk_psc_consistant_hashing_small_group_size_in_bits_set;
    data_index = dnx_data_trunk_psc_define_smooth_division_max_nof_member;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trunk_psc_smooth_division_max_nof_member_set;
    data_index = dnx_data_trunk_psc_define_smooth_division_entries_per_profile;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trunk_psc_smooth_division_entries_per_profile_set;

    
    data_index = dnx_data_trunk_psc_multiply_and_divide;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_trunk_psc_multiply_and_divide_set;
    data_index = dnx_data_trunk_psc_smooth_division;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_trunk_psc_smooth_division_set;
    data_index = dnx_data_trunk_psc_consistant_hashing;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_trunk_psc_consistant_hashing_set;

    
    
    submodule_index = dnx_data_trunk_submodule_egress_trunk;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_trunk_egress_trunk_define_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trunk_egress_trunk_nof_set;
    data_index = dnx_data_trunk_egress_trunk_define_nof_lb_keys_per_profile;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trunk_egress_trunk_nof_lb_keys_per_profile_set;
    data_index = dnx_data_trunk_egress_trunk_define_dbal_egress_trunk_index_multiplier;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trunk_egress_trunk_dbal_egress_trunk_index_multiplier_set;
    data_index = dnx_data_trunk_egress_trunk_define_dbal_entry_index_divider;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trunk_egress_trunk_dbal_entry_index_divider_set;
    data_index = dnx_data_trunk_egress_trunk_define_dbal_entry_index_msb_multiplier;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trunk_egress_trunk_dbal_entry_index_msb_multiplier_set;
    data_index = dnx_data_trunk_egress_trunk_define_invalid_pp_dsp;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trunk_egress_trunk_invalid_pp_dsp_set;

    

    
    
    submodule_index = dnx_data_trunk_submodule_graceful;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_trunk_graceful_define_first_hw_configuration;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trunk_graceful_first_hw_configuration_set;
    data_index = dnx_data_trunk_graceful_define_second_hw_configuration;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trunk_graceful_second_hw_configuration_set;

    
    data_index = dnx_data_trunk_graceful_allow;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_trunk_graceful_allow_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

