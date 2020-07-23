

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_DRAM

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_dram.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>
#include <soc/shmoo_hbm16.h>
#include <bcm_int/dnx/dram/hbmc/hbmc.h>







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
        "This is an internal soc property that should be used only for debug purposes\n"
        "dram Frequency in MHZ\n"
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
    table->values[1].default_val = "0x3";
    table->values[2].default_val = "32";
    table->values[3].default_val = "16384";
    table->values[4].default_val = "32";
    table->values[5].default_val = "DATA(dram, general_info, frequency) * 2";
    table->values[6].default_val = "100";
    table->values[7].default_val = "SHMOO_HBM16_CTL_TYPE_1";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_dram_general_info_dram_info_t, (1 + 1  ), "data of dnx_data_dram_general_info_table_dram_info");

    
    default_data = (dnx_data_dram_general_info_dram_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dram_type = SHMOO_HBM16_DRAM_TYPE_GEN2;
    default_data->dram_bitmap = 0x3;
    default_data->nof_columns = 32;
    default_data->nof_rows = 16384;
    default_data->nof_banks = 32;
    default_data->data_rate = dnx_data_dram.general_info.frequency_get(unit) * 2;
    default_data->ref_clock = 100;
    default_data->ctl_type = SHMOO_HBM16_CTL_TYPE_1;
    
    data = (dnx_data_dram_general_info_dram_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);
    
    table->values[1].property.name = "ext_ram_enabled_bitmap";
    table->values[1].property.doc =
        "\n"
        "This is an internal soc property that should be used only for debug purposes\n"
        "A bitmap that indicates which HBMs are used.\n"
        "\n"
    ;
    table->values[1].property.method = dnxc_data_property_method_custom;
    table->values[1].property.method_str = "custom";
    
    table->values[2].property.name = spn_EXT_RAM_COLUMNS;
    table->values[2].property.doc =
        "\n"
        "This is an internal soc property that should be used only for debug purposes\n"
        "Number of columns in the dram\n"
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
        "This is an internal soc property that should be used only for debug purposes\n"
        "Number of rows in the dram\n"
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
        "This is an internal soc property that should be used only for debug purposes\n"
        "Number of banks in the dram\n"
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
    SHR_IF_ERR_EXIT(dnx_data_property_dram_general_info_dram_info_dram_bitmap_read(unit, &data->dram_bitmap));
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
jr2_b0_dnx_data_dram_dram_block_write_minus_read_leaky_bucket_disable_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_dram_block;
    int feature_index = dnx_data_dram_dram_block_write_minus_read_leaky_bucket_disable;
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

    
    
    submodule_index = dnx_data_dram_submodule_general_info;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_dram_general_info_define_frequency;
    define = &submodule->defines[data_index];
    define->set = jr2_b0_dnx_data_dram_general_info_frequency_set;

    

    
    data_index = dnx_data_dram_general_info_table_dram_info;
    table = &submodule->tables[data_index];
    table->set = jr2_b0_dnx_data_dram_general_info_dram_info_set;
    
    submodule_index = dnx_data_dram_submodule_dram_block;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_dram_dram_block_write_minus_read_leaky_bucket_disable;
    feature = &submodule->features[data_index];
    feature->set = jr2_b0_dnx_data_dram_dram_block_write_minus_read_leaky_bucket_disable_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

