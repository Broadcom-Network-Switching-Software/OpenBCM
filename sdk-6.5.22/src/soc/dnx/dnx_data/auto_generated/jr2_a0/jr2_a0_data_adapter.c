
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TX

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_adapter.h>
#include <src/bcm/dnx/rx/auto_generated/rx_internal_jr2_a0.h>








static shr_error_e
jr2_a0_dnx_data_adapter_tx_loopback_enable_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_adapter;
    int submodule_index = dnx_data_adapter_submodule_tx;
    int define_index = dnx_data_adapter_tx_define_loopback_enable;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "adapter_loopback_enable";
    define->property.doc = 
        "\n"
        "Use loopback in the adapter server when receiving packets.\n"
        "Default: 0\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_enable;
    define->property.method_str = "enable";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);
    define->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);

exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_adapter_rx_packet_header_signal_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_adapter;
    int submodule_index = dnx_data_adapter_submodule_rx;
    int define_index = dnx_data_adapter_rx_define_packet_header_signal_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = PACKET_HEADER_SIGNAL_ID_JR2_A0;

    
    define->data = PACKET_HEADER_SIGNAL_ID_JR2_A0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_adapter_rx_constant_header_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_adapter;
    int submodule_index = dnx_data_adapter_submodule_rx;
    int define_index = dnx_data_adapter_rx_define_constant_header_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 28;

    
    define->data = 28;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_adapter_general_lib_ver_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_adapter;
    int submodule_index = dnx_data_adapter_submodule_general;
    int define_index = dnx_data_adapter_general_define_lib_ver;
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
jr2_a0_dnx_data_adapter_reg_mem_access_do_collect_enable_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_adapter;
    int submodule_index = dnx_data_adapter_submodule_reg_mem_access;
    int define_index = dnx_data_adapter_reg_mem_access_define_do_collect_enable;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->flags |= DNXC_DATA_F_INIT_ONLY;
    
    define->property.name = "custom_feature";
    define->property.doc = 
        "\n"
        "Use do_collect in adapter to collect writes into one bulk write.\n"
        "Default: 0\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_enable;
    define->property.method_str = "suffix_enable";
    define->property.suffix = "adapter_do_collect_enable";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_adapter_reg_mem_access_cmic_block_index_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_adapter;
    int submodule_index = dnx_data_adapter_submodule_reg_mem_access;
    int define_index = dnx_data_adapter_reg_mem_access_define_cmic_block_index;
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
jr2_a0_dnx_data_adapter_reg_mem_access_iproc_block_index_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_adapter;
    int submodule_index = dnx_data_adapter_submodule_reg_mem_access;
    int define_index = dnx_data_adapter_reg_mem_access_define_iproc_block_index;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 6;

    
    define->data = 6;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_adapter_reg_mem_access_clear_on_read_mems_set(
    int unit)
{
    int index_index;
    dnx_data_adapter_reg_mem_access_clear_on_read_mems_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_adapter;
    int submodule_index = dnx_data_adapter_submodule_reg_mem_access;
    int table_index = dnx_data_adapter_reg_mem_access_table_clear_on_read_mems;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 0;
    table->info_get.key_size[0] = 0;

    
    table->values[0].default_val = "INVALIDm";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_adapter_reg_mem_access_clear_on_read_mems_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_adapter_reg_mem_access_table_clear_on_read_mems");

    
    default_data = (dnx_data_adapter_reg_mem_access_clear_on_read_mems_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->mem = INVALIDm;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_adapter_reg_mem_access_clear_on_read_mems_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_adapter_reg_mem_access_clear_on_read_regs_set(
    int unit)
{
    int index_index;
    dnx_data_adapter_reg_mem_access_clear_on_read_regs_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_adapter;
    int submodule_index = dnx_data_adapter_submodule_reg_mem_access;
    int table_index = dnx_data_adapter_reg_mem_access_table_clear_on_read_regs;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 0;
    table->info_get.key_size[0] = 0;

    
    table->values[0].default_val = "INVALIDr";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_adapter_reg_mem_access_clear_on_read_regs_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_adapter_reg_mem_access_table_clear_on_read_regs");

    
    default_data = (dnx_data_adapter_reg_mem_access_clear_on_read_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->reg = INVALIDr;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_adapter_reg_mem_access_clear_on_read_regs_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}



shr_error_e
jr2_a0_data_adapter_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_adapter;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_adapter_submodule_tx;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_adapter_tx_define_loopback_enable;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_adapter_tx_loopback_enable_set;

    

    
    
    submodule_index = dnx_data_adapter_submodule_rx;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_adapter_rx_define_packet_header_signal_id;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_adapter_rx_packet_header_signal_id_set;
    data_index = dnx_data_adapter_rx_define_constant_header_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_adapter_rx_constant_header_size_set;

    

    
    
    submodule_index = dnx_data_adapter_submodule_general;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_adapter_general_define_lib_ver;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_adapter_general_lib_ver_set;

    

    
    
    submodule_index = dnx_data_adapter_submodule_reg_mem_access;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_adapter_reg_mem_access_define_do_collect_enable;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_adapter_reg_mem_access_do_collect_enable_set;
    data_index = dnx_data_adapter_reg_mem_access_define_cmic_block_index;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_adapter_reg_mem_access_cmic_block_index_set;
    data_index = dnx_data_adapter_reg_mem_access_define_iproc_block_index;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_adapter_reg_mem_access_iproc_block_index_set;

    

    
    data_index = dnx_data_adapter_reg_mem_access_table_clear_on_read_mems;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_adapter_reg_mem_access_clear_on_read_mems_set;
    data_index = dnx_data_adapter_reg_mem_access_table_clear_on_read_regs;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_adapter_reg_mem_access_clear_on_read_regs_set;

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

