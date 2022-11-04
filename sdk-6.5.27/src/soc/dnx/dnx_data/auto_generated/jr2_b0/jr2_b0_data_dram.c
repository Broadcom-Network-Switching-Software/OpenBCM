
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_DRAM

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_dram.h>
#include <soc/dnx/shmoo_hbm16.h>
#include <bcm_int/dnx/dram/hbmc/hbmc.h>
#include <bcm_int/dnx/dram/dram.h>







static shr_error_e
jr2_b0_dnx_data_dram_hbm_dword_alignment_check_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_hbm;
    int feature_index = dnx_data_dram_hbm_dword_alignment_check;
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
jr2_b0_dnx_data_dram_hbm_is_delete_bdb_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_hbm;
    int feature_index = dnx_data_dram_hbm_is_delete_bdb_supported;
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
jr2_b0_dnx_data_dram_hbm_vendor_info_disable_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_hbm;
    int feature_index = dnx_data_dram_hbm_vendor_info_disable;
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
jr2_b0_dnx_data_dram_general_info_frequency_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_frequency;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1000;

    
    define->data = 1000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_EXT_RAM_FREQ;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 500;
    define->property.range_max = 1400;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_b0_dnx_data_dram_general_info_supported_dram_bitmap_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_supported_dram_bitmap;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x3;

    
    define->data = 0x3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_b0_dnx_data_dram_general_info_dram_bitmap_internal_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int define_index = dnx_data_dram_general_info_define_dram_bitmap_internal;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x3;

    
    define->data = 0x3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "ext_ram_enabled_bitmap";
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_custom;
    define->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnx_data_property_dram_general_info_dram_bitmap_internal_read(unit, (uint32 *) &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_b0_dnx_data_dram_general_info_dram_info_set(
    int unit)
{
    dnx_data_dram_general_info_dram_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int table_index = dnx_data_dram_general_info_table_dram_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "SHMOO_HBM16_DRAM_TYPE_GEN2";
    table->values[1].default_val = "DATA(dram,general_info,dram_bitmap_internal)";
    table->values[2].default_val = "32";
    table->values[3].default_val = "16384";
    table->values[4].default_val = "32";
    table->values[5].default_val = "DATA(dram, general_info, frequency) * 2";
    table->values[6].default_val = "100";
    table->values[7].default_val = "SHMOO_HBM16_CTL_TYPE_1";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_general_info_dram_info_t, (1 + 1  ), "data of dnx_data_dram_general_info_table_dram_info");

    
    default_data = (dnx_data_dram_general_info_dram_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dram_type = SHMOO_HBM16_DRAM_TYPE_GEN2;
    default_data->dram_bitmap = dnx_data_dram.general_info.dram_bitmap_internal_get(unit);
    default_data->nof_columns = 32;
    default_data->nof_rows = 16384;
    default_data->nof_banks = 32;
    default_data->data_rate = dnx_data_dram.general_info.frequency_get(unit) * 2;
    default_data->ref_clock = 100;
    default_data->ctl_type = SHMOO_HBM16_CTL_TYPE_1;
    
    data = (dnx_data_dram_general_info_dram_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);
    
    table->values[2].property.name = spn_EXT_RAM_COLUMNS;
    table->values[2].property.doc =
        "\n"
    ;
    table->values[2].property.method = dnxc_data_property_method_direct_map;
    table->values[2].property.method_str = "direct_map";
    table->values[2].property.nof_mapping = 1;
    DNXC_DATA_ALLOC(table->values[2].property.mapping, dnxc_data_property_map_t, table->values[2].property.nof_mapping, "dnx_data_dram_general_info_dram_info_t property mapping");

    table->values[2].property.mapping[0].name = "32";
    table->values[2].property.mapping[0].val = 32;
    table->values[2].property.mapping[0].is_default = 1 ;
    
    table->values[3].property.name = spn_EXT_RAM_ROWS;
    table->values[3].property.doc =
        "\n"
    ;
    table->values[3].property.method = dnxc_data_property_method_direct_map;
    table->values[3].property.method_str = "direct_map";
    table->values[3].property.nof_mapping = 1;
    DNXC_DATA_ALLOC(table->values[3].property.mapping, dnxc_data_property_map_t, table->values[3].property.nof_mapping, "dnx_data_dram_general_info_dram_info_t property mapping");

    table->values[3].property.mapping[0].name = "16384";
    table->values[3].property.mapping[0].val = 16384;
    table->values[3].property.mapping[0].is_default = 1 ;
    
    table->values[4].property.name = spn_EXT_RAM_BANKS;
    table->values[4].property.doc =
        "\n"
    ;
    table->values[4].property.method = dnxc_data_property_method_direct_map;
    table->values[4].property.method_str = "direct_map";
    table->values[4].property.nof_mapping = 1;
    DNXC_DATA_ALLOC(table->values[4].property.mapping, dnxc_data_property_map_t, table->values[4].property.nof_mapping, "dnx_data_dram_general_info_dram_info_t property mapping");

    table->values[4].property.mapping[0].name = "32";
    table->values[4].property.mapping[0].val = 32;
    table->values[4].property.mapping[0].is_default = 1 ;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    data = (dnx_data_dram_general_info_dram_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[2].property, -1, &data->nof_columns));
    data = (dnx_data_dram_general_info_dram_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[3].property, -1, &data->nof_rows));
    data = (dnx_data_dram_general_info_dram_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[4].property, -1, &data->nof_banks));
    
    table->flags |= (DNXC_DATA_F_PROPERTY);
    table->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);

exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_b0_dnx_data_dram_dram_block_dram_block_params_set(
    int unit)
{
    int mechanism_index;
    dnx_data_dram_dram_block_dram_block_params_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dram_block;
    int table_index = dnx_data_dram_dram_block_table_dram_block_params;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DNX_DRAM_BLOCK_MECHANISM_NOF;
    table->info_get.key_size[0] = DNX_DRAM_BLOCK_MECHANISM_NOF;

    
    table->values[0].default_val = "FALSE";
    table->values[1].default_val = "TRUE";
    table->values[2].default_val = "FALSE";
    table->values[3].default_val = "DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID";
    table->values[4].default_val = "DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID";
    table->values[5].default_val = "DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID";
    table->values[6].default_val = "DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID";
    table->values[7].default_val = "DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID";
    table->values[8].default_val = "DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID";
    table->values[9].default_val = "DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID";
    table->values[10].default_val = "DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID";
    table->values[11].default_val = "DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID";
    table->values[12].default_val = "DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID";
    table->values[13].default_val = "DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID";
    table->values[14].default_val = "DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID";
    table->values[15].default_val = "DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID";
    table->values[16].default_val = "DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID";
    table->values[17].default_val = "DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID";
    table->values[18].default_val = "DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID";
    table->values[19].default_val = "DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID";
    table->values[20].default_val = "TRUE";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_dram_block_dram_block_params_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_dram_dram_block_table_dram_block_params");

    
    default_data = (dnx_data_dram_dram_block_dram_block_params_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->is_valid = FALSE;
    default_data->is_used = TRUE;
    default_data->is_disable_supported = FALSE;
    default_data->dbal_table = DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID;
    default_data->increment_threshold_0 = DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID;
    default_data->increment_threshold_1 = DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID;
    default_data->increment_threshold_2 = DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID;
    default_data->decrement_threshold_0 = DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID;
    default_data->decrement_threshold_1 = DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID;
    default_data->decrement_threshold_2 = DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID;
    default_data->increment_size_0 = DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID;
    default_data->increment_size_1 = DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID;
    default_data->increment_size_2 = DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID;
    default_data->decrement_size_0 = DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID;
    default_data->decrement_size_1 = DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID;
    default_data->decrement_size_2 = DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID;
    default_data->bucket_full_size = DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID;
    default_data->bucket_assert_threshold = DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID;
    default_data->bucket_deassert_threshold = DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID;
    default_data->log2_window_size = DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID;
    default_data->reset_on_deassert = TRUE;
    
    for (mechanism_index = 0; mechanism_index < table->keys[0].size; mechanism_index++)
    {
        data = (dnx_data_dram_dram_block_dram_block_params_t *) dnxc_data_mgmt_table_data_get(unit, table, mechanism_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DNX_DRAM_BLOCK_MECHANISM_WRITE_MINUS_READ < table->keys[0].size)
    {
        data = (dnx_data_dram_dram_block_dram_block_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_DRAM_BLOCK_MECHANISM_WRITE_MINUS_READ, 0);
        data->is_valid = TRUE;
        data->is_used = FALSE;
        data->is_disable_supported = TRUE;
        data->dbal_table = DBAL_TABLE_DRAM_COMMON_TDU_DRAM_BLOCK_WRITE_MINUS_READ_LEAKY_BUCKET_CONFIG;
        data->increment_threshold_0 = 0;
        data->increment_threshold_1 = 2000;
        data->increment_threshold_2 = 4000;
        data->decrement_threshold_0 = 0;
        data->decrement_threshold_1 = 5 * dnx_data_dram.dram_block.wmr_decrement_thr_factor_get(unit) / 100;
        data->decrement_threshold_2 = 10 * dnx_data_dram.dram_block.wmr_decrement_thr_factor_get(unit) / 100;
        data->increment_size_0 = 1;
        data->increment_size_1 = 2;
        data->increment_size_2 = 4;
        data->decrement_size_0 = 0;
        data->decrement_size_1 = 0;
        data->decrement_size_2 = 0;
        data->bucket_full_size = 50;
        data->bucket_assert_threshold = 0;
        data->bucket_deassert_threshold = 0;
    }
    if (DNX_DRAM_BLOCK_MECHANISM_WRITE_PLUS_READ < table->keys[0].size)
    {
        data = (dnx_data_dram_dram_block_dram_block_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_DRAM_BLOCK_MECHANISM_WRITE_PLUS_READ, 0);
        data->is_valid = TRUE;
        data->dbal_table = DBAL_TABLE_DRAM_COMMON_TDU_DRAM_BLOCK_WRITE_PLUS_READ_LEAKY_BUCKET_CONFIG;
        data->increment_threshold_0 = 91 * dnx_data_dram.dram_block.wpr_increment_thr_factor_get(unit) / 100;
        data->increment_threshold_1 = 93 * dnx_data_dram.dram_block.wpr_increment_thr_factor_get(unit) / 100;
        data->increment_threshold_2 = 95 * dnx_data_dram.dram_block.wpr_increment_thr_factor_get(unit) / 100;
        data->increment_size_0 = 2;
        data->increment_size_1 = 3;
        data->increment_size_2 = 4;
        data->decrement_size_0 = 1;
        data->bucket_full_size = 50;
        data->bucket_assert_threshold = 16;
        data->bucket_deassert_threshold = 4;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}



shr_error_e
jr2_b0_data_dram_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_dram;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_dram_submodule_hbm;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_dram_hbm_dword_alignment_check;
    feature = &submodule->features[data_index];
    feature->set = jr2_b0_dnx_data_dram_hbm_dword_alignment_check_set;
    data_index = dnx_data_dram_hbm_is_delete_bdb_supported;
    feature = &submodule->features[data_index];
    feature->set = jr2_b0_dnx_data_dram_hbm_is_delete_bdb_supported_set;
    data_index = dnx_data_dram_hbm_vendor_info_disable;
    feature = &submodule->features[data_index];
    feature->set = jr2_b0_dnx_data_dram_hbm_vendor_info_disable_set;

    
    
    submodule_index = dnx_data_dram_submodule_general_info;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_dram_general_info_define_frequency;
    define = &submodule->defines[data_index];
    define->set = jr2_b0_dnx_data_dram_general_info_frequency_set;
    data_index = dnx_data_dram_general_info_define_supported_dram_bitmap;
    define = &submodule->defines[data_index];
    define->set = jr2_b0_dnx_data_dram_general_info_supported_dram_bitmap_set;
    data_index = dnx_data_dram_general_info_define_dram_bitmap_internal;
    define = &submodule->defines[data_index];
    define->set = jr2_b0_dnx_data_dram_general_info_dram_bitmap_internal_set;

    

    
    data_index = dnx_data_dram_general_info_table_dram_info;
    table = &submodule->tables[data_index];
    table->set = jr2_b0_dnx_data_dram_general_info_dram_info_set;
    
    submodule_index = dnx_data_dram_submodule_dram_block;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnx_data_dram_dram_block_table_dram_block_params;
    table = &submodule->tables[data_index];
    table->set = jr2_b0_dnx_data_dram_dram_block_dram_block_params_set;

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

