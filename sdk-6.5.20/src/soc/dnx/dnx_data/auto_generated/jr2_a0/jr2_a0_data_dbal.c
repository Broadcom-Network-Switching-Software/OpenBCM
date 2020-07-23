

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DBALDNX

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>
#include <soc/dnx/dbal/dbal.h>









static shr_error_e
jr2_a0_dnx_data_dbal_diag_loggerInfo_set(
    int unit)
{
    dnx_data_dbal_diag_loggerInfo_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_dbal;
    int submodule_index = dnx_data_dbal_submodule_diag;
    int table_index = dnx_data_dbal_diag_table_loggerInfo;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "NULL";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_dbal_diag_loggerInfo_t, (1 + 1  ), "data of dnx_data_dbal_diag_table_loggerInfo");

    
    default_data = (dnx_data_dbal_diag_loggerInfo_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->file_name = NULL;
    
    data = (dnx_data_dbal_diag_loggerInfo_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);
    
    table->values[0].property.name = spn_CUSTOM_FEATURE;
    table->values[0].property.doc =
        "\n"
        "Dbal logger output file, if NULL log will not dump to file\n"
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_suffix_str;
    table->values[0].property.method_str = "suffix_str";
    table->values[0].property.suffix = "logger_file_name";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    data = (dnx_data_dbal_diag_loggerInfo_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[0].property, -1, &data->file_name));
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
jr2_a0_dnx_data_dbal_db_init_invalid_feature_example_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dbal;
    int submodule_index = dnx_data_dbal_submodule_db_init;
    int feature_index = dnx_data_dbal_db_init_invalid_feature_example;
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
jr2_a0_dnx_data_dbal_db_init_dbal_device_state_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dbal;
    int submodule_index = dnx_data_dbal_submodule_db_init;
    int define_index = dnx_data_dbal_db_init_define_dbal_device_state;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = DBAL_DEVICE_STATE_ON_PRODUCTION;

    
    define->data = DBAL_DEVICE_STATE_ON_PRODUCTION;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_dbal_db_init_run_ltt_after_wm_test_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dbal;
    int submodule_index = dnx_data_dbal_submodule_db_init;
    int define_index = dnx_data_dbal_db_init_define_run_ltt_after_wm_test;
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
jr2_a0_dnx_data_dbal_db_init_dbal_collection_pool_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dbal;
    int submodule_index = dnx_data_dbal_submodule_db_init;
    int define_index = dnx_data_dbal_db_init_define_dbal_collection_pool_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_CUSTOM_FEATURE;
    define->property.doc = 
        "\n"
        "Indicates the DBAL collection mode pool size\n"
        "Default: 0.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_range;
    define->property.method_str = "suffix_range";
    define->property.suffix = "dbal_collect_mode_pool_size";
    define->property.range_min = 0;
    define->property.range_max = 1000;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_dbal_table_nof_dynamic_tables_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dbal;
    int submodule_index = dnx_data_dbal_submodule_table;
    int define_index = dnx_data_dbal_table_define_nof_dynamic_tables;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 100;

    
    define->data = 100;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_dbal_table_nof_dynamic_xml_tables_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dbal;
    int submodule_index = dnx_data_dbal_submodule_table;
    int define_index = dnx_data_dbal_table_define_nof_dynamic_xml_tables;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 100;

    
    define->data = 100;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_dbal_table_nof_dynamic_tables_labels_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dbal;
    int submodule_index = dnx_data_dbal_submodule_table;
    int define_index = dnx_data_dbal_table_define_nof_dynamic_tables_labels;
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
jr2_a0_dnx_data_dbal_table_nof_dynamic_tables_key_fields_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dbal;
    int submodule_index = dnx_data_dbal_submodule_table;
    int define_index = dnx_data_dbal_table_define_nof_dynamic_tables_key_fields;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 35;

    
    define->data = 35;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_dbal_table_nof_dynamic_tables_result_fields_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dbal;
    int submodule_index = dnx_data_dbal_submodule_table;
    int define_index = dnx_data_dbal_table_define_nof_dynamic_tables_result_fields;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 50;

    
    define->data = 50;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_dbal_table_nof_dynamic_tables_multi_result_types_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dbal;
    int submodule_index = dnx_data_dbal_submodule_table;
    int define_index = dnx_data_dbal_table_define_nof_dynamic_tables_multi_result_types;
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
jr2_a0_dnx_data_dbal_table_nof_dynamic_tables_ppmc_multi_result_types_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dbal;
    int submodule_index = dnx_data_dbal_submodule_table;
    int define_index = dnx_data_dbal_table_define_nof_dynamic_tables_ppmc_multi_result_types;
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
jr2_a0_dnx_data_dbal_table_nof_dynamic_and_static_tables_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dbal;
    int submodule_index = dnx_data_dbal_submodule_table;
    int define_index = dnx_data_dbal_table_define_nof_dynamic_and_static_tables;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = DBAL_NOF_TABLES+100;

    
    define->data = DBAL_NOF_TABLES+100;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_dbal_hw_ent_nof_direct_maps_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dbal;
    int submodule_index = dnx_data_dbal_submodule_hw_ent;
    int define_index = dnx_data_dbal_hw_ent_define_nof_direct_maps;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = DBAL_NOF_TABLES * DBAL_MAX_NUMBER_OF_HW_ELEMENTS;

    
    define->data = DBAL_NOF_TABLES * DBAL_MAX_NUMBER_OF_HW_ELEMENTS;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_dbal_hw_ent_nof_groups_maps_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dbal;
    int submodule_index = dnx_data_dbal_submodule_hw_ent;
    int define_index = dnx_data_dbal_hw_ent_define_nof_groups_maps;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = DBAL_NOF_TABLES * DBAL_MAX_NUMBER_OF_HW_ELEMENTS;

    
    define->data = DBAL_NOF_TABLES * DBAL_MAX_NUMBER_OF_HW_ELEMENTS;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}




shr_error_e
jr2_a0_data_dbal_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_dbal;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_dbal_submodule_diag;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnx_data_dbal_diag_table_loggerInfo;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_dbal_diag_loggerInfo_set;
    
    submodule_index = dnx_data_dbal_submodule_db_init;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_dbal_db_init_define_dbal_device_state;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dbal_db_init_dbal_device_state_set;
    data_index = dnx_data_dbal_db_init_define_run_ltt_after_wm_test;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dbal_db_init_run_ltt_after_wm_test_set;
    data_index = dnx_data_dbal_db_init_define_dbal_collection_pool_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dbal_db_init_dbal_collection_pool_size_set;

    
    data_index = dnx_data_dbal_db_init_invalid_feature_example;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_dbal_db_init_invalid_feature_example_set;

    
    
    submodule_index = dnx_data_dbal_submodule_table;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_dbal_table_define_nof_dynamic_tables;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dbal_table_nof_dynamic_tables_set;
    data_index = dnx_data_dbal_table_define_nof_dynamic_xml_tables;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dbal_table_nof_dynamic_xml_tables_set;
    data_index = dnx_data_dbal_table_define_nof_dynamic_tables_labels;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dbal_table_nof_dynamic_tables_labels_set;
    data_index = dnx_data_dbal_table_define_nof_dynamic_tables_key_fields;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dbal_table_nof_dynamic_tables_key_fields_set;
    data_index = dnx_data_dbal_table_define_nof_dynamic_tables_result_fields;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dbal_table_nof_dynamic_tables_result_fields_set;
    data_index = dnx_data_dbal_table_define_nof_dynamic_tables_multi_result_types;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dbal_table_nof_dynamic_tables_multi_result_types_set;
    data_index = dnx_data_dbal_table_define_nof_dynamic_tables_ppmc_multi_result_types;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dbal_table_nof_dynamic_tables_ppmc_multi_result_types_set;
    data_index = dnx_data_dbal_table_define_nof_dynamic_and_static_tables;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dbal_table_nof_dynamic_and_static_tables_set;

    

    
    
    submodule_index = dnx_data_dbal_submodule_hw_ent;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_dbal_hw_ent_define_nof_direct_maps;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dbal_hw_ent_nof_direct_maps_set;
    data_index = dnx_data_dbal_hw_ent_define_nof_groups_maps;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_dbal_hw_ent_nof_groups_maps_set;

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

