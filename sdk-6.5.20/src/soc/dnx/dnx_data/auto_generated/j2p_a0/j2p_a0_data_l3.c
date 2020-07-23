

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_L3

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_l3.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>








static shr_error_e
j2p_a0_dnx_data_l3_fwd_lpm_strength_profile_support_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fwd;
    int define_index = dnx_data_l3_fwd_define_lpm_strength_profile_support;
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
j2p_a0_dnx_data_l3_fwd_nof_lpm_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fwd;
    int define_index = dnx_data_l3_fwd_define_nof_lpm_profiles;
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
j2p_a0_dnx_data_l3_fwd_lpm_profile_to_entry_strength_set(
    int unit)
{
    int lpm_profile_index;
    int kaps_intf_index;
    dnx_data_l3_fwd_lpm_profile_to_entry_strength_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fwd;
    int table_index = dnx_data_l3_fwd_table_lpm_profile_to_entry_strength;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 8;
    table->info_get.key_size[0] = 8;
    table->keys[1].size = 4;
    table->info_get.key_size[1] = 4;

    
    table->values[0].default_val = "-1,-1,-1,-1,-1,-1,-1";
    table->values[1].default_val = "-1,-1,-1,-1,-1,-1,-1";
    table->values[2].default_val = "1";
    table->values[3].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_l3_fwd_lpm_profile_to_entry_strength_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_l3_fwd_table_lpm_profile_to_entry_strength");

    
    default_data = (dnx_data_l3_fwd_lpm_profile_to_entry_strength_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->prefix_len_non_def[0] = -1;
    default_data->prefix_len_non_def[1] = -1;
    default_data->prefix_len_non_def[2] = -1;
    default_data->prefix_len_non_def[3] = -1;
    default_data->prefix_len_non_def[4] = -1;
    default_data->prefix_len_non_def[5] = -1;
    default_data->prefix_len_non_def[6] = -1;
    default_data->prefix_len_def[0] = -1;
    default_data->prefix_len_def[1] = -1;
    default_data->prefix_len_def[2] = -1;
    default_data->prefix_len_def[3] = -1;
    default_data->prefix_len_def[4] = -1;
    default_data->prefix_len_def[5] = -1;
    default_data->prefix_len_def[6] = -1;
    default_data->entry_strength_non_def[0] = 1;
    default_data->entry_strength_def[0] = -1;
    
    for (lpm_profile_index = 0; lpm_profile_index < table->keys[0].size; lpm_profile_index++)
    {
        for (kaps_intf_index = 0; kaps_intf_index < table->keys[1].size; kaps_intf_index++)
        {
            data = (dnx_data_l3_fwd_lpm_profile_to_entry_strength_t *) dnxc_data_mgmt_table_data_get(unit, table, lpm_profile_index, kaps_intf_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    if (0 < table->keys[0].size && DBAL_ENUM_FVAL_KAPS_INTERFACE_INTERFACE_0 < table->keys[1].size)
    {
        data = (dnx_data_l3_fwd_lpm_profile_to_entry_strength_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, DBAL_ENUM_FVAL_KAPS_INTERFACE_INTERFACE_0);
        data->prefix_len_non_def[0] = 0;
        data->prefix_len_non_def[1] = 160;
        data->prefix_len_non_def[2] = -1;
        data->prefix_len_non_def[3] = -1;
        data->prefix_len_non_def[4] = -1;
        data->prefix_len_non_def[5] = -1;
        data->prefix_len_non_def[6] = -1;
        data->prefix_len_non_def[7] = -1;
        data->prefix_len_def[0] = 0;
        data->prefix_len_def[1] = 160;
        data->prefix_len_def[2] = -1;
        data->prefix_len_def[3] = -1;
        data->prefix_len_def[4] = -1;
        data->prefix_len_def[5] = -1;
        data->prefix_len_def[6] = -1;
        data->prefix_len_def[7] = -1;
        data->entry_strength_non_def[0] = 1;
        data->entry_strength_non_def[1] = -1;
        data->entry_strength_non_def[2] = -1;
        data->entry_strength_non_def[3] = -1;
        data->entry_strength_non_def[4] = -1;
        data->entry_strength_non_def[5] = -1;
        data->entry_strength_non_def[6] = -1;
        data->entry_strength_non_def[7] = -1;
        data->entry_strength_def[0] = 0;
        data->entry_strength_def[1] = -1;
        data->entry_strength_def[2] = -1;
        data->entry_strength_def[3] = -1;
        data->entry_strength_def[4] = -1;
        data->entry_strength_def[5] = -1;
        data->entry_strength_def[6] = -1;
        data->entry_strength_def[7] = -1;
    }
    if (0 < table->keys[0].size && DBAL_ENUM_FVAL_KAPS_INTERFACE_INTERFACE_1 < table->keys[1].size)
    {
        data = (dnx_data_l3_fwd_lpm_profile_to_entry_strength_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, DBAL_ENUM_FVAL_KAPS_INTERFACE_INTERFACE_1);
        data->prefix_len_non_def[0] = 0;
        data->prefix_len_non_def[1] = 51;
        data->prefix_len_non_def[2] = 63;
        data->prefix_len_non_def[3] = 64;
        data->prefix_len_non_def[4] = 96;
        data->prefix_len_non_def[5] = 160;
        data->prefix_len_non_def[6] = -1;
        data->prefix_len_non_def[7] = -1;
        data->prefix_len_def[0] = 0;
        data->prefix_len_def[1] = 160;
        data->prefix_len_def[2] = -1;
        data->prefix_len_def[3] = -1;
        data->prefix_len_def[4] = -1;
        data->prefix_len_def[5] = -1;
        data->prefix_len_def[6] = -1;
        data->prefix_len_def[7] = -1;
        data->entry_strength_non_def[0] = 1;
        data->entry_strength_non_def[1] = 2;
        data->entry_strength_non_def[2] = 3;
        data->entry_strength_non_def[3] = 4;
        data->entry_strength_non_def[4] = 5;
        data->entry_strength_non_def[5] = -1;
        data->entry_strength_non_def[6] = -1;
        data->entry_strength_non_def[7] = -1;
        data->entry_strength_def[0] = 0;
        data->entry_strength_def[1] = -1;
        data->entry_strength_def[2] = -1;
        data->entry_strength_def[3] = -1;
        data->entry_strength_def[4] = -1;
        data->entry_strength_def[5] = -1;
        data->entry_strength_def[6] = -1;
        data->entry_strength_def[7] = -1;
    }
    if (0 < table->keys[0].size && DBAL_ENUM_FVAL_KAPS_INTERFACE_INTERFACE_2 < table->keys[1].size)
    {
        data = (dnx_data_l3_fwd_lpm_profile_to_entry_strength_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, DBAL_ENUM_FVAL_KAPS_INTERFACE_INTERFACE_2);
        data->prefix_len_non_def[0] = 0;
        data->prefix_len_non_def[1] = 160;
        data->prefix_len_non_def[2] = -1;
        data->prefix_len_non_def[3] = -1;
        data->prefix_len_non_def[4] = -1;
        data->prefix_len_non_def[5] = -1;
        data->prefix_len_non_def[6] = -1;
        data->prefix_len_non_def[7] = -1;
        data->prefix_len_def[0] = 0;
        data->prefix_len_def[1] = 160;
        data->prefix_len_def[2] = -1;
        data->prefix_len_def[3] = -1;
        data->prefix_len_def[4] = -1;
        data->prefix_len_def[5] = -1;
        data->prefix_len_def[6] = -1;
        data->prefix_len_def[7] = -1;
        data->entry_strength_non_def[0] = 1;
        data->entry_strength_non_def[1] = -1;
        data->entry_strength_non_def[2] = -1;
        data->entry_strength_non_def[3] = -1;
        data->entry_strength_non_def[4] = -1;
        data->entry_strength_non_def[5] = -1;
        data->entry_strength_non_def[6] = -1;
        data->entry_strength_non_def[7] = -1;
        data->entry_strength_def[0] = 0;
        data->entry_strength_def[1] = -1;
        data->entry_strength_def[2] = -1;
        data->entry_strength_def[3] = -1;
        data->entry_strength_def[4] = -1;
        data->entry_strength_def[5] = -1;
        data->entry_strength_def[6] = -1;
        data->entry_strength_def[7] = -1;
    }
    if (0 < table->keys[0].size && DBAL_ENUM_FVAL_KAPS_INTERFACE_INTERFACE_3 < table->keys[1].size)
    {
        data = (dnx_data_l3_fwd_lpm_profile_to_entry_strength_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, DBAL_ENUM_FVAL_KAPS_INTERFACE_INTERFACE_3);
        data->prefix_len_non_def[0] = 0;
        data->prefix_len_non_def[1] = 160;
        data->prefix_len_non_def[2] = -1;
        data->prefix_len_non_def[3] = -1;
        data->prefix_len_non_def[4] = -1;
        data->prefix_len_non_def[5] = -1;
        data->prefix_len_non_def[6] = -1;
        data->prefix_len_non_def[7] = -1;
        data->prefix_len_def[0] = 0;
        data->prefix_len_def[1] = 160;
        data->prefix_len_def[2] = -1;
        data->prefix_len_def[3] = -1;
        data->prefix_len_def[4] = -1;
        data->prefix_len_def[5] = -1;
        data->prefix_len_def[6] = -1;
        data->prefix_len_def[7] = -1;
        data->entry_strength_non_def[0] = 1;
        data->entry_strength_non_def[1] = -1;
        data->entry_strength_non_def[2] = -1;
        data->entry_strength_non_def[3] = -1;
        data->entry_strength_non_def[4] = -1;
        data->entry_strength_non_def[5] = -1;
        data->entry_strength_non_def[6] = -1;
        data->entry_strength_non_def[7] = -1;
        data->entry_strength_def[0] = 0;
        data->entry_strength_def[1] = -1;
        data->entry_strength_def[2] = -1;
        data->entry_strength_def[3] = -1;
        data->entry_strength_def[4] = -1;
        data->entry_strength_def[5] = -1;
        data->entry_strength_def[6] = -1;
        data->entry_strength_def[7] = -1;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
j2p_a0_dnx_data_l3_ecmp_group_size_multiply_and_divide_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_ecmp;
    int define_index = dnx_data_l3_ecmp_define_group_size_multiply_and_divide_nof_bits;
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
j2p_a0_dnx_data_l3_ecmp_member_table_nof_rows_size_in_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_ecmp;
    int define_index = dnx_data_l3_ecmp_define_member_table_nof_rows_size_in_bits;
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
j2p_a0_dnx_data_l3_ecmp_member_table_address_size_in_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_ecmp;
    int define_index = dnx_data_l3_ecmp_define_member_table_address_size_in_bits;
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
j2p_a0_dnx_data_l3_ecmp_tunnel_priority_support_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_ecmp;
    int define_index = dnx_data_l3_ecmp_define_tunnel_priority_support;
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
j2p_a0_dnx_data_l3_ecmp_nof_tunnel_priority_map_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_ecmp;
    int define_index = dnx_data_l3_ecmp_define_nof_tunnel_priority_map_profiles;
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
j2p_a0_dnx_data_l3_ecmp_tunnel_priority_field_width_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_ecmp;
    int define_index = dnx_data_l3_ecmp_define_tunnel_priority_field_width;
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
j2p_a0_dnx_data_l3_ecmp_tunnel_priority_index_field_width_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_ecmp;
    int define_index = dnx_data_l3_ecmp_define_tunnel_priority_index_field_width;
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
j2p_a0_dnx_data_l3_feature_ipv6_mc_compatible_dmac_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_feature;
    int feature_index = dnx_data_l3_feature_ipv6_mc_compatible_dmac;
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
j2p_a0_dnx_data_l3_feature_fec_hit_bit_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_feature;
    int feature_index = dnx_data_l3_feature_fec_hit_bit;
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
j2p_a0_dnx_data_l3_feature_mc_bridge_fallback_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_feature;
    int feature_index = dnx_data_l3_feature_mc_bridge_fallback;
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
j2p_a0_dnx_data_l3_feature_nat_on_a_stick_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_feature;
    int feature_index = dnx_data_l3_feature_nat_on_a_stick;
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
j2p_a0_data_l3_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_l3;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_l3_submodule_fwd;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_l3_fwd_define_lpm_strength_profile_support;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_l3_fwd_lpm_strength_profile_support_set;
    data_index = dnx_data_l3_fwd_define_nof_lpm_profiles;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_l3_fwd_nof_lpm_profiles_set;

    

    
    data_index = dnx_data_l3_fwd_table_lpm_profile_to_entry_strength;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_l3_fwd_lpm_profile_to_entry_strength_set;
    
    submodule_index = dnx_data_l3_submodule_ecmp;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_l3_ecmp_define_group_size_multiply_and_divide_nof_bits;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_l3_ecmp_group_size_multiply_and_divide_nof_bits_set;
    data_index = dnx_data_l3_ecmp_define_member_table_nof_rows_size_in_bits;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_l3_ecmp_member_table_nof_rows_size_in_bits_set;
    data_index = dnx_data_l3_ecmp_define_member_table_address_size_in_bits;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_l3_ecmp_member_table_address_size_in_bits_set;
    data_index = dnx_data_l3_ecmp_define_tunnel_priority_support;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_l3_ecmp_tunnel_priority_support_set;
    data_index = dnx_data_l3_ecmp_define_nof_tunnel_priority_map_profiles;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_l3_ecmp_nof_tunnel_priority_map_profiles_set;
    data_index = dnx_data_l3_ecmp_define_tunnel_priority_field_width;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_l3_ecmp_tunnel_priority_field_width_set;
    data_index = dnx_data_l3_ecmp_define_tunnel_priority_index_field_width;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_l3_ecmp_tunnel_priority_index_field_width_set;

    

    
    
    submodule_index = dnx_data_l3_submodule_feature;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_l3_feature_ipv6_mc_compatible_dmac;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_l3_feature_ipv6_mc_compatible_dmac_set;
    data_index = dnx_data_l3_feature_fec_hit_bit;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_l3_feature_fec_hit_bit_set;
    data_index = dnx_data_l3_feature_mc_bridge_fallback;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_l3_feature_mc_bridge_fallback_set;
    data_index = dnx_data_l3_feature_nat_on_a_stick;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_l3_feature_nat_on_a_stick_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

