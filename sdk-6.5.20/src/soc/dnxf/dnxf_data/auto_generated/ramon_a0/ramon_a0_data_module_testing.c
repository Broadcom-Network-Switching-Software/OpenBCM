

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COMMON

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_internal_module_testing.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_property.h>
#include <bcm/fabric.h>







static shr_error_e
ramon_a0_dnxf_data_module_testing_unsupported_supported_feature_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnxf_data_module_module_testing;
    int submodule_index = dnxf_data_module_testing_submodule_unsupported;
    int feature_index = dnxf_data_module_testing_unsupported_supported_feature;
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
ramon_a0_dnxf_data_module_testing_unsupported_supported_def_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_module_testing;
    int submodule_index = dnxf_data_module_testing_submodule_unsupported;
    int define_index = dnxf_data_module_testing_unsupported_define_supported_def;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x12345678;

    
    define->data = 0x12345678;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
ramon_a0_dnxf_data_module_testing_unsupported_supported_table_set(
    int unit)
{
    dnxf_data_module_testing_unsupported_supported_table_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnxf_data_module_module_testing;
    int submodule_index = dnxf_data_module_testing_submodule_unsupported;
    int table_index = dnxf_data_module_testing_unsupported_table_supported_table;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "0x87654321";
    
    DNXC_DATA_ALLOC(table->data, dnxf_data_module_testing_unsupported_supported_table_t, (1 + 1  ), "data of dnxf_data_module_testing_unsupported_table_supported_table");

    
    default_data = (dnxf_data_module_testing_unsupported_supported_table_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->num = 0x87654321;
    
    data = (dnxf_data_module_testing_unsupported_supported_table_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
ramon_a0_dnxf_data_module_testing_types_all_set(
    int unit)
{
    dnxf_data_module_testing_types_all_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnxf_data_module_module_testing;
    int submodule_index = dnxf_data_module_testing_submodule_types;
    int table_index = dnxf_data_module_testing_types_table_all;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "-1";
    table->values[1].default_val = "0x12";
    table->values[2].default_val = "'1'";
    table->values[3].default_val = "0x1234";
    table->values[4].default_val = "bcmFabricDeviceTypeFE2";
    table->values[5].default_val = "1, 2, 3, 4, 5, 6, 7, 8, 9, 10";
    table->values[6].default_val = "0x1, 0x2, 0x3, 0x4";
    table->values[7].default_val = "dnxf data";
    table->values[8].default_val = "'a', 'b', 'c'";
    
    DNXC_DATA_ALLOC(table->data, dnxf_data_module_testing_types_all_t, (1 + 1  ), "data of dnxf_data_module_testing_types_table_all");

    
    default_data = (dnxf_data_module_testing_types_all_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->intval = -1;
    default_data->uint8val = 0x12;
    default_data->charval = '1';
    default_data->uint16val = 0x1234;
    default_data->enumval = bcmFabricDeviceTypeFE2;
    default_data->arrval[0] = 1;
    default_data->arrval[1] = 2;
    default_data->arrval[2] = 3;
    default_data->arrval[3] = 4;
    default_data->arrval[4] = 5;
    default_data->arrval[5] = 6;
    default_data->arrval[6] = 7;
    default_data->arrval[7] = 8;
    default_data->arrval[8] = 9;
    default_data->arrval[9] = 10;
    #if 4 > _SHR_PBMP_WORD_MAX
    #error "out of bound access to array"
    #endif
    _SHR_PBMP_WORD_SET(default_data->pbmpval, 0, 0x1);
    _SHR_PBMP_WORD_SET(default_data->pbmpval, 1, 0x2);
    _SHR_PBMP_WORD_SET(default_data->pbmpval, 2, 0x3);
    _SHR_PBMP_WORD_SET(default_data->pbmpval, 3, 0x4);
    default_data->strval = "dnxf data";
    default_data->bufferval[0] = 'a';
    default_data->bufferval[1] = 'b';
    default_data->bufferval[2] = 'c';
    
    data = (dnxf_data_module_testing_types_all_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
ramon_a0_dnxf_data_module_testing_property_methods_feature_enable_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnxf_data_module_module_testing;
    int submodule_index = dnxf_data_module_testing_submodule_property_methods;
    int feature_index = dnxf_data_module_testing_property_methods_feature_enable;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;
    
    feature->flags |= DNXC_DATA_F_INIT_ONLY;
    
    feature->property.name = "dnxf_data_feature_enable";
    feature->property.doc = 
        "\n"
        "feature enable description\n"
        "\n"
    ;
    feature->property.method = dnxc_data_property_method_enable;
    feature->property.method_str = "enable";
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &feature->property, -1, &feature->data));
    
    feature->flags |= (DNXC_DATA_F_PROPERTY);
    feature->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);


    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
ramon_a0_dnxf_data_module_testing_property_methods_numeric_range_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_module_testing;
    int submodule_index = dnxf_data_module_testing_submodule_property_methods;
    int define_index = dnxf_data_module_testing_property_methods_define_numeric_range;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = -1;

    
    define->data = -1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->flags |= DNXC_DATA_F_INIT_ONLY;
    
    define->property.name = "dnxf_data_numeric_range";
    define->property.doc = 
        "\n"
        "numeric range description\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 5;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);
    define->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
ramon_a0_dnxf_data_module_testing_property_methods_enable_set(
    int unit)
{
    dnxf_data_module_testing_property_methods_enable_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnxf_data_module_module_testing;
    int submodule_index = dnxf_data_module_testing_submodule_property_methods;
    int table_index = dnxf_data_module_testing_property_methods_table_enable;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->flags |= DNXC_DATA_F_INIT_ONLY;

    
    table->values[0].default_val = "-1";
    
    DNXC_DATA_ALLOC(table->data, dnxf_data_module_testing_property_methods_enable_t, (1 + 1  ), "data of dnxf_data_module_testing_property_methods_table_enable");

    
    default_data = (dnxf_data_module_testing_property_methods_enable_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->val = -1;
    
    data = (dnxf_data_module_testing_property_methods_enable_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);
    
    table->property.name = "dnxf_data_enable";
    table->property.doc =
        "SoC property to test dnx data enable method\n"
    ;
    table->property.method = dnxc_data_property_method_enable;
    table->property.method_str = "enable";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    data = (dnxf_data_module_testing_property_methods_enable_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->property, -1, &data->val));
    
    table->flags |= (DNXC_DATA_F_PROPERTY);
    table->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_module_testing_property_methods_port_enable_set(
    int unit)
{
    int port_index;
    dnxf_data_module_testing_property_methods_port_enable_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnxf_data_module_module_testing;
    int submodule_index = dnxf_data_module_testing_submodule_property_methods;
    int table_index = dnxf_data_module_testing_property_methods_table_port_enable;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 10;
    table->info_get.key_size[0] = 10;

    
    table->values[0].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnxf_data_module_testing_property_methods_port_enable_t, (1 * (table->keys[0].size) + 1  ), "data of dnxf_data_module_testing_property_methods_table_port_enable");

    
    default_data = (dnxf_data_module_testing_property_methods_port_enable_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->val = -1;
    
    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnxf_data_module_testing_property_methods_port_enable_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->property.name = "dnxf_data_port_enable";
    table->property.doc =
        "SoC property to test dnx data port enable method\n"
    ;
    table->property.method = dnxc_data_property_method_port_enable;
    table->property.method_str = "port_enable";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnxf_data_module_testing_property_methods_port_enable_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->property, port_index, &data->val));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);
    table->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_module_testing_property_methods_suffix_enable_set(
    int unit)
{
    int link_index;
    dnxf_data_module_testing_property_methods_suffix_enable_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnxf_data_module_module_testing;
    int submodule_index = dnxf_data_module_testing_submodule_property_methods;
    int table_index = dnxf_data_module_testing_property_methods_table_suffix_enable;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 10;
    table->info_get.key_size[0] = 10;

    
    table->values[0].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnxf_data_module_testing_property_methods_suffix_enable_t, (1 * (table->keys[0].size) + 1  ), "data of dnxf_data_module_testing_property_methods_table_suffix_enable");

    
    default_data = (dnxf_data_module_testing_property_methods_suffix_enable_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->val = -1;
    
    for (link_index = 0; link_index < table->keys[0].size; link_index++)
    {
        data = (dnxf_data_module_testing_property_methods_suffix_enable_t *) dnxc_data_mgmt_table_data_get(unit, table, link_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->property.name = "dnxf_data_suffix_enable";
    table->property.doc =
        "SoC property to test dnx data suffix enable method\n"
    ;
    table->property.method = dnxc_data_property_method_suffix_enable;
    table->property.method_str = "suffix_enable";
    table->property.suffix = "link";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (link_index = 0; link_index < table->keys[0].size; link_index++)
    {
        data = (dnxf_data_module_testing_property_methods_suffix_enable_t *) dnxc_data_mgmt_table_data_get(unit, table, link_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->property, link_index, &data->val));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);
    table->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_module_testing_property_methods_range_set(
    int unit)
{
    dnxf_data_module_testing_property_methods_range_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnxf_data_module_module_testing;
    int submodule_index = dnxf_data_module_testing_submodule_property_methods;
    int table_index = dnxf_data_module_testing_property_methods_table_range;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "-1";
    
    DNXC_DATA_ALLOC(table->data, dnxf_data_module_testing_property_methods_range_t, (1 + 1  ), "data of dnxf_data_module_testing_property_methods_table_range");

    
    default_data = (dnxf_data_module_testing_property_methods_range_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->val = -1;
    
    data = (dnxf_data_module_testing_property_methods_range_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);
    
    table->property.name = "dnxf_data_range";
    table->property.doc =
        "SoC property to test dnx data range method\n"
    ;
    table->property.method = dnxc_data_property_method_range;
    table->property.method_str = "range";
    table->property.range_min = 3;
    table->property.range_max = 5;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    data = (dnxf_data_module_testing_property_methods_range_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->property, -1, &data->val));
    
    table->flags |= (DNXC_DATA_F_PROPERTY);
    table->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_module_testing_property_methods_port_range_set(
    int unit)
{
    int port_index;
    dnxf_data_module_testing_property_methods_port_range_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnxf_data_module_module_testing;
    int submodule_index = dnxf_data_module_testing_submodule_property_methods;
    int table_index = dnxf_data_module_testing_property_methods_table_port_range;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 10;
    table->info_get.key_size[0] = 10;

    
    table->values[0].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnxf_data_module_testing_property_methods_port_range_t, (1 * (table->keys[0].size) + 1  ), "data of dnxf_data_module_testing_property_methods_table_port_range");

    
    default_data = (dnxf_data_module_testing_property_methods_port_range_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->val = -1;
    
    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnxf_data_module_testing_property_methods_port_range_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->property.name = "dnxf_data_port_range";
    table->property.doc =
        "SoC property to test dnx data port range method\n"
    ;
    table->property.method = dnxc_data_property_method_port_range;
    table->property.method_str = "port_range";
    table->property.range_min = 3;
    table->property.range_max = 5;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnxf_data_module_testing_property_methods_port_range_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->property, port_index, &data->val));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);
    table->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_module_testing_property_methods_suffix_range_set(
    int unit)
{
    int link_index;
    dnxf_data_module_testing_property_methods_suffix_range_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnxf_data_module_module_testing;
    int submodule_index = dnxf_data_module_testing_submodule_property_methods;
    int table_index = dnxf_data_module_testing_property_methods_table_suffix_range;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 10;
    table->info_get.key_size[0] = 10;

    
    table->values[0].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnxf_data_module_testing_property_methods_suffix_range_t, (1 * (table->keys[0].size) + 1  ), "data of dnxf_data_module_testing_property_methods_table_suffix_range");

    
    default_data = (dnxf_data_module_testing_property_methods_suffix_range_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->val = -1;
    
    for (link_index = 0; link_index < table->keys[0].size; link_index++)
    {
        data = (dnxf_data_module_testing_property_methods_suffix_range_t *) dnxc_data_mgmt_table_data_get(unit, table, link_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->property.name = "dnxf_data_suffix_range";
    table->property.doc =
        "SoC property to test dnx data suffix range method\n"
    ;
    table->property.method = dnxc_data_property_method_suffix_range;
    table->property.method_str = "suffix_range";
    table->property.suffix = "link";
    table->property.range_min = 3;
    table->property.range_max = 5;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (link_index = 0; link_index < table->keys[0].size; link_index++)
    {
        data = (dnxf_data_module_testing_property_methods_suffix_range_t *) dnxc_data_mgmt_table_data_get(unit, table, link_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->property, link_index, &data->val));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);
    table->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_module_testing_property_methods_direct_map_set(
    int unit)
{
    dnxf_data_module_testing_property_methods_direct_map_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnxf_data_module_module_testing;
    int submodule_index = dnxf_data_module_testing_submodule_property_methods;
    int table_index = dnxf_data_module_testing_property_methods_table_direct_map;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "-1";
    
    DNXC_DATA_ALLOC(table->data, dnxf_data_module_testing_property_methods_direct_map_t, (1 + 1  ), "data of dnxf_data_module_testing_property_methods_table_direct_map");

    
    default_data = (dnxf_data_module_testing_property_methods_direct_map_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->val = -1;
    
    data = (dnxf_data_module_testing_property_methods_direct_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);
    
    table->property.name = "dnxf_data_direct_map";
    table->property.doc =
        "SoC property to test dnx data direct map method\n"
    ;
    table->property.method = dnxc_data_property_method_direct_map;
    table->property.method_str = "direct_map";
    table->property.nof_mapping = 3;
    DNXC_DATA_ALLOC(table->property.mapping, dnxc_data_property_map_t, table->property.nof_mapping, "dnxf_data_module_testing_property_methods_direct_map_t property mapping");

    table->property.mapping[0].name = "NORMAL";
    table->property.mapping[0].val = 0;
    table->property.mapping[1].name = "HIGH";
    table->property.mapping[1].val = 1;
    table->property.mapping[2].name = "LOW";
    table->property.mapping[2].val = 2;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    data = (dnxf_data_module_testing_property_methods_direct_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->property, -1, &data->val));
    
    table->flags |= (DNXC_DATA_F_PROPERTY);
    table->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_module_testing_property_methods_port_direct_map_set(
    int unit)
{
    int port_index;
    dnxf_data_module_testing_property_methods_port_direct_map_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnxf_data_module_module_testing;
    int submodule_index = dnxf_data_module_testing_submodule_property_methods;
    int table_index = dnxf_data_module_testing_property_methods_table_port_direct_map;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 10;
    table->info_get.key_size[0] = 10;

    
    table->values[0].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnxf_data_module_testing_property_methods_port_direct_map_t, (1 * (table->keys[0].size) + 1  ), "data of dnxf_data_module_testing_property_methods_table_port_direct_map");

    
    default_data = (dnxf_data_module_testing_property_methods_port_direct_map_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->val = -1;
    
    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnxf_data_module_testing_property_methods_port_direct_map_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->property.name = "dnxf_data_port_direct_map";
    table->property.doc =
        "SoC property to test dnx data port direct map method\n"
    ;
    table->property.method = dnxc_data_property_method_port_direct_map;
    table->property.method_str = "port_direct_map";
    table->property.nof_mapping = 3;
    DNXC_DATA_ALLOC(table->property.mapping, dnxc_data_property_map_t, table->property.nof_mapping, "dnxf_data_module_testing_property_methods_port_direct_map_t property mapping");

    table->property.mapping[0].name = "NORMAL";
    table->property.mapping[0].val = 0;
    table->property.mapping[1].name = "HIGH";
    table->property.mapping[1].val = 1;
    table->property.mapping[2].name = "LOW";
    table->property.mapping[2].val = 2;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnxf_data_module_testing_property_methods_port_direct_map_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->property, port_index, &data->val));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);
    table->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_module_testing_property_methods_suffix_direct_map_set(
    int unit)
{
    int link_index;
    dnxf_data_module_testing_property_methods_suffix_direct_map_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnxf_data_module_module_testing;
    int submodule_index = dnxf_data_module_testing_submodule_property_methods;
    int table_index = dnxf_data_module_testing_property_methods_table_suffix_direct_map;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 10;
    table->info_get.key_size[0] = 10;

    
    table->values[0].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnxf_data_module_testing_property_methods_suffix_direct_map_t, (1 * (table->keys[0].size) + 1  ), "data of dnxf_data_module_testing_property_methods_table_suffix_direct_map");

    
    default_data = (dnxf_data_module_testing_property_methods_suffix_direct_map_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->val = -1;
    
    for (link_index = 0; link_index < table->keys[0].size; link_index++)
    {
        data = (dnxf_data_module_testing_property_methods_suffix_direct_map_t *) dnxc_data_mgmt_table_data_get(unit, table, link_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->property.name = "dnxf_data_suffix_direct_map";
    table->property.doc =
        "SoC property to test dnx data suffix direct map method\n"
    ;
    table->property.method = dnxc_data_property_method_suffix_direct_map;
    table->property.method_str = "suffix_direct_map";
    table->property.suffix = "link";
    table->property.nof_mapping = 3;
    DNXC_DATA_ALLOC(table->property.mapping, dnxc_data_property_map_t, table->property.nof_mapping, "dnxf_data_module_testing_property_methods_suffix_direct_map_t property mapping");

    table->property.mapping[0].name = "NORMAL";
    table->property.mapping[0].val = 0;
    table->property.mapping[1].name = "HIGH";
    table->property.mapping[1].val = 1;
    table->property.mapping[2].name = "LOW";
    table->property.mapping[2].val = 2;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (link_index = 0; link_index < table->keys[0].size; link_index++)
    {
        data = (dnxf_data_module_testing_property_methods_suffix_direct_map_t *) dnxc_data_mgmt_table_data_get(unit, table, link_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->property, link_index, &data->val));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);
    table->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_module_testing_property_methods_custom_set(
    int unit)
{
    int link_index;
    int pipe_index;
    dnxf_data_module_testing_property_methods_custom_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnxf_data_module_module_testing;
    int submodule_index = dnxf_data_module_testing_submodule_property_methods;
    int table_index = dnxf_data_module_testing_property_methods_table_custom;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 5;
    table->info_get.key_size[0] = 5;
    table->keys[1].size = 2;
    table->info_get.key_size[1] = 2;

    
    table->values[0].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnxf_data_module_testing_property_methods_custom_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnxf_data_module_testing_property_methods_table_custom");

    
    default_data = (dnxf_data_module_testing_property_methods_custom_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->val = -1;
    
    for (link_index = 0; link_index < table->keys[0].size; link_index++)
    {
        for (pipe_index = 0; pipe_index < table->keys[1].size; pipe_index++)
        {
            data = (dnxf_data_module_testing_property_methods_custom_t *) dnxc_data_mgmt_table_data_get(unit, table, link_index, pipe_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    table->values[0].property.name = "dnxf_data_custom";
    table->values[0].property.doc =
        "SoC property to test dnx data custom method\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_custom;
    table->values[0].property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (link_index = 0; link_index < table->keys[0].size; link_index++)
    {
        for (pipe_index = 0; pipe_index < table->keys[1].size; pipe_index++)
        {
            data = (dnxf_data_module_testing_property_methods_custom_t *) dnxc_data_mgmt_table_data_get(unit, table, link_index, pipe_index);
            SHR_IF_ERR_EXIT(dnxf_data_property_module_testing_property_methods_custom_val_read(unit, link_index, pipe_index, &data->val));
        }
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);
    table->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);

exit:
    SHR_FUNC_EXIT;
}



shr_error_e
ramon_a0_data_module_testing_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnxf_data_module_module_testing;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnxf_data_module_testing_submodule_unsupported;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnxf_data_module_testing_unsupported_define_supported_def;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_module_testing_unsupported_supported_def_set;

    
    data_index = dnxf_data_module_testing_unsupported_supported_feature;
    feature = &submodule->features[data_index];
    feature->set = ramon_a0_dnxf_data_module_testing_unsupported_supported_feature_set;

    
    data_index = dnxf_data_module_testing_unsupported_table_supported_table;
    table = &submodule->tables[data_index];
    table->set = ramon_a0_dnxf_data_module_testing_unsupported_supported_table_set;
    
    submodule_index = dnxf_data_module_testing_submodule_types;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnxf_data_module_testing_types_table_all;
    table = &submodule->tables[data_index];
    table->set = ramon_a0_dnxf_data_module_testing_types_all_set;
    
    submodule_index = dnxf_data_module_testing_submodule_property_methods;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnxf_data_module_testing_property_methods_define_numeric_range;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_module_testing_property_methods_numeric_range_set;

    
    data_index = dnxf_data_module_testing_property_methods_feature_enable;
    feature = &submodule->features[data_index];
    feature->set = ramon_a0_dnxf_data_module_testing_property_methods_feature_enable_set;

    
    data_index = dnxf_data_module_testing_property_methods_table_enable;
    table = &submodule->tables[data_index];
    table->set = ramon_a0_dnxf_data_module_testing_property_methods_enable_set;
    data_index = dnxf_data_module_testing_property_methods_table_port_enable;
    table = &submodule->tables[data_index];
    table->set = ramon_a0_dnxf_data_module_testing_property_methods_port_enable_set;
    data_index = dnxf_data_module_testing_property_methods_table_suffix_enable;
    table = &submodule->tables[data_index];
    table->set = ramon_a0_dnxf_data_module_testing_property_methods_suffix_enable_set;
    data_index = dnxf_data_module_testing_property_methods_table_range;
    table = &submodule->tables[data_index];
    table->set = ramon_a0_dnxf_data_module_testing_property_methods_range_set;
    data_index = dnxf_data_module_testing_property_methods_table_port_range;
    table = &submodule->tables[data_index];
    table->set = ramon_a0_dnxf_data_module_testing_property_methods_port_range_set;
    data_index = dnxf_data_module_testing_property_methods_table_suffix_range;
    table = &submodule->tables[data_index];
    table->set = ramon_a0_dnxf_data_module_testing_property_methods_suffix_range_set;
    data_index = dnxf_data_module_testing_property_methods_table_direct_map;
    table = &submodule->tables[data_index];
    table->set = ramon_a0_dnxf_data_module_testing_property_methods_direct_map_set;
    data_index = dnxf_data_module_testing_property_methods_table_port_direct_map;
    table = &submodule->tables[data_index];
    table->set = ramon_a0_dnxf_data_module_testing_property_methods_port_direct_map_set;
    data_index = dnxf_data_module_testing_property_methods_table_suffix_direct_map;
    table = &submodule->tables[data_index];
    table->set = ramon_a0_dnxf_data_module_testing_property_methods_suffix_direct_map_set;
    data_index = dnxf_data_module_testing_property_methods_table_custom;
    table = &submodule->tables[data_index];
    table->set = ramon_a0_dnxf_data_module_testing_property_methods_custom_set;

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

