
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_ACCESSDNX

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_access.h>








static shr_error_e
jr2_a0_dnx_data_access_mdio_nof_pms_per_ring_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_access;
    int submodule_index = dnx_data_access_submodule_mdio;
    int define_index = dnx_data_access_mdio_define_nof_pms_per_ring;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_fabric.blocks.nof_pms_get(unit);

    
    define->data = dnx_data_fabric.blocks.nof_pms_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_access_mdio_ring_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_access;
    int submodule_index = dnx_data_access_submodule_mdio;
    int define_index = dnx_data_access_mdio_define_ring_offset;
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
jr2_a0_dnx_data_access_mdio_offset_in_ring_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_access;
    int submodule_index = dnx_data_access_submodule_mdio;
    int define_index = dnx_data_access_mdio_define_offset_in_ring;
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
jr2_a0_dnx_data_access_mdio_ext_divisor_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_access;
    int submodule_index = dnx_data_access_submodule_mdio;
    int define_index = dnx_data_access_mdio_define_ext_divisor;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 14;

    
    define->data = 14;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_RATE_EXT_MDIO_DIVISOR;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 255;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_access_mdio_int_divisor_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_access;
    int submodule_index = dnx_data_access_submodule_mdio;
    int define_index = dnx_data_access_mdio_define_int_divisor;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 50;

    
    define->data = 50;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_RATE_INT_MDIO_DIVISOR;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 255;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_access_mdio_int_div_out_delay_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_access;
    int submodule_index = dnx_data_access_submodule_mdio;
    int define_index = dnx_data_access_mdio_define_int_div_out_delay;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 25;

    
    define->data = 25;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_MDIO_OUTPUT_DELAY;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_range;
    define->property.method_str = "suffix_range";
    define->property.suffix = "int";
    define->property.range_min = 0;
    define->property.range_max = 255;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_access_mdio_ext_div_out_delay_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_access;
    int submodule_index = dnx_data_access_submodule_mdio;
    int define_index = dnx_data_access_mdio_define_ext_div_out_delay;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 14;

    
    define->data = 14;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_MDIO_OUTPUT_DELAY;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_range;
    define->property.method_str = "suffix_range";
    define->property.suffix = "ext";
    define->property.range_min = 0;
    define->property.range_max = 255;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}








static shr_error_e
jr2_a0_dnx_data_access_rcpu_rx_set(
    int unit)
{
    dnx_data_access_rcpu_rx_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_access;
    int submodule_index = dnx_data_access_submodule_rcpu;
    int table_index = dnx_data_access_rcpu_table_rx;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "0";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_access_rcpu_rx_t, (1 + 1  ), "data of dnx_data_access_rcpu_table_rx");

    
    default_data = (dnx_data_access_rcpu_rx_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    _SHR_PBMP_WORD_SET(default_data->pbmp, 0, 0);
    
    data = (dnx_data_access_rcpu_rx_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);
    
    table->values[0].property.name = spn_RCPU_RX_PBMP;
    table->values[0].property.doc =
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_pbmp;
    table->values[0].property.method_str = "pbmp";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    data = (dnx_data_access_rcpu_rx_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[0].property, -1, &data->pbmp));
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_access_blocks_override_set(
    int unit)
{
    int index_index;
    dnx_data_access_blocks_override_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_access;
    int submodule_index = dnx_data_access_submodule_blocks;
    int table_index = dnx_data_access_blocks_table_override;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 20;
    table->info_get.key_size[0] = 20;

    
    table->values[0].default_val = "NULL";
    table->values[1].default_val = "-1";
    table->values[2].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_access_blocks_override_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_access_blocks_table_override");

    
    default_data = (dnx_data_access_blocks_override_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->block_type = NULL;
    default_data->block_instance = -1;
    default_data->value = -1;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_access_blocks_override_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}



shr_error_e
jr2_a0_data_access_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_access;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_access_submodule_mdio;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_access_mdio_define_nof_pms_per_ring;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_access_mdio_nof_pms_per_ring_set;
    data_index = dnx_data_access_mdio_define_ring_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_access_mdio_ring_offset_set;
    data_index = dnx_data_access_mdio_define_offset_in_ring;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_access_mdio_offset_in_ring_set;
    data_index = dnx_data_access_mdio_define_ext_divisor;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_access_mdio_ext_divisor_set;
    data_index = dnx_data_access_mdio_define_int_divisor;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_access_mdio_int_divisor_set;
    data_index = dnx_data_access_mdio_define_int_div_out_delay;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_access_mdio_int_div_out_delay_set;
    data_index = dnx_data_access_mdio_define_ext_div_out_delay;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_access_mdio_ext_div_out_delay_set;

    

    
    
    submodule_index = dnx_data_access_submodule_rcpu;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnx_data_access_rcpu_table_rx;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_access_rcpu_rx_set;
    
    submodule_index = dnx_data_access_submodule_blocks;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnx_data_access_blocks_table_override;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_access_blocks_override_set;

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

