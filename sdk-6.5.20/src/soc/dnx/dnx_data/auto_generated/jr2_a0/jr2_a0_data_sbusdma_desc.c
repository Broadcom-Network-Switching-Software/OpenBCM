

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DMA

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_sbusdma_desc.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>
#include <soc/dnx/utils/dnx_sbusdma_desc.h>







static shr_error_e
jr2_a0_dnx_data_sbusdma_desc_global_desc_dma_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_sbusdma_desc;
    int submodule_index = dnx_data_sbusdma_desc_submodule_global;
    int feature_index = dnx_data_sbusdma_desc_global_desc_dma;
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
jr2_a0_dnx_data_sbusdma_desc_global_force_enable_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_sbusdma_desc;
    int submodule_index = dnx_data_sbusdma_desc_submodule_global;
    int feature_index = dnx_data_sbusdma_desc_global_force_enable;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;
    
    feature->property.name = "custom_feature";
    feature->property.doc = 
        "\n"
        "Enable / Disable image verification when enabling descriptor DMA.\n"
        "custom_feature_dma_desc_aggregator_force_enable = 0 / 1\n"
        "\n"
    ;
    feature->property.method = dnxc_data_property_method_suffix_enable;
    feature->property.method_str = "suffix_enable";
    feature->property.suffix = "dma_desc_aggregator_force_enable";
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &feature->property, -1, &feature->data));
    
    feature->flags |= (DNXC_DATA_F_PROPERTY);
    feature->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);


    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_sbusdma_desc_global_dma_desc_aggregator_chain_length_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sbusdma_desc;
    int submodule_index = dnx_data_sbusdma_desc_submodule_global;
    int define_index = dnx_data_sbusdma_desc_global_define_dma_desc_aggregator_chain_length_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_DMA_DESC_AGGREGATOR_CHAIN_LENGTH_MAX;
    define->property.doc = 
        "\n"
        "Maximal number of descriptors in a single chain.\n"
        "Default: 0 - disabled\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 1000000;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);
    define->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sbusdma_desc_global_dma_desc_aggregator_buff_size_kb_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sbusdma_desc;
    int submodule_index = dnx_data_sbusdma_desc_submodule_global;
    int define_index = dnx_data_sbusdma_desc_global_define_dma_desc_aggregator_buff_size_kb;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_DMA_DESC_AGGREGATOR_BUFF_SIZE_KB;
    define->property.doc = 
        "\n"
        "Total size in KB of dmmable memory allocated in favor of the descriptor DMA double-buffer.\n"
        "Default: 0 - disabled\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 100000;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);
    define->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_sbusdma_desc_global_dma_desc_aggregator_timeout_usec_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_sbusdma_desc;
    int submodule_index = dnx_data_sbusdma_desc_submodule_global;
    int define_index = dnx_data_sbusdma_desc_global_define_dma_desc_aggregator_timeout_usec;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_DMA_DESC_AGGREGATOR_TIMEOUT_USEC;
    define->property.doc = 
        "\n"
        "Timeout between the creation of a descriptor chain and its commit to HW\n"
        "Default: 0 - disabled\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 100000000;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);
    define->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_sbusdma_desc_global_enable_module_desc_dma_set(
    int unit)
{
    int module_enum_val_index;
    dnx_data_sbusdma_desc_global_enable_module_desc_dma_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_sbusdma_desc;
    int submodule_index = dnx_data_sbusdma_desc_submodule_global;
    int table_index = dnx_data_sbusdma_desc_global_table_enable_module_desc_dma;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES;
    table->info_get.key_size[0] = SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES;

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_sbusdma_desc_global_enable_module_desc_dma_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_sbusdma_desc_global_table_enable_module_desc_dma");

    
    default_data = (dnx_data_sbusdma_desc_global_enable_module_desc_dma_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->enable = 0;
    
    for (module_enum_val_index = 0; module_enum_val_index < table->keys[0].size; module_enum_val_index++)
    {
        data = (dnx_data_sbusdma_desc_global_enable_module_desc_dma_t *) dnxc_data_mgmt_table_data_get(unit, table, module_enum_val_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->values[0].property.name = "dma_desc_aggregator_enable_specific";
    table->values[0].property.doc =
        "\n"
        "Enable (1) / Disable (0) descriptor DMA per module.\n"
        "See UM for supported module names.\n"
        "Default: Disabled (0).\n"
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_custom;
    table->values[0].property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (module_enum_val_index = 0; module_enum_val_index < table->keys[0].size; module_enum_val_index++)
    {
        data = (dnx_data_sbusdma_desc_global_enable_module_desc_dma_t *) dnxc_data_mgmt_table_data_get(unit, table, module_enum_val_index, 0);
        SHR_IF_ERR_EXIT(dnx_data_property_sbusdma_desc_global_enable_module_desc_dma_enable_read(unit, module_enum_val_index, &data->enable));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}



shr_error_e
jr2_a0_data_sbusdma_desc_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_sbusdma_desc;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_sbusdma_desc_submodule_global;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_sbusdma_desc_global_define_dma_desc_aggregator_chain_length_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sbusdma_desc_global_dma_desc_aggregator_chain_length_max_set;
    data_index = dnx_data_sbusdma_desc_global_define_dma_desc_aggregator_buff_size_kb;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sbusdma_desc_global_dma_desc_aggregator_buff_size_kb_set;
    data_index = dnx_data_sbusdma_desc_global_define_dma_desc_aggregator_timeout_usec;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_sbusdma_desc_global_dma_desc_aggregator_timeout_usec_set;

    
    data_index = dnx_data_sbusdma_desc_global_desc_dma;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_sbusdma_desc_global_desc_dma_set;
    data_index = dnx_data_sbusdma_desc_global_force_enable;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_sbusdma_desc_global_force_enable_set;

    
    data_index = dnx_data_sbusdma_desc_global_table_enable_module_desc_dma;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_sbusdma_desc_global_enable_module_desc_dma_set;

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

