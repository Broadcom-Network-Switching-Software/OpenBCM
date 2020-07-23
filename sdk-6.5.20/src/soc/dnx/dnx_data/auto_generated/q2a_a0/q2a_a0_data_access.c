

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_ACCESSDNX

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>









static shr_error_e
q2a_a0_dnx_data_access_blocks_soft_init_set(
    int unit)
{
    dnx_data_access_blocks_soft_init_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_access;
    int submodule_index = dnx_data_access_submodule_blocks;
    int table_index = dnx_data_access_blocks_table_soft_init;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "0x0";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_access_blocks_soft_init_t, (1 + 1  ), "data of dnx_data_access_blocks_table_soft_init");

    
    default_data = (dnx_data_access_blocks_soft_init_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    _SHR_PBMP_WORD_SET(default_data->trigger_value, 0, 0x0);
    
    data = (dnx_data_access_blocks_soft_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);
    
    data = (dnx_data_access_blocks_soft_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    #if 8 > _SHR_PBMP_WORD_MAX
    #error "out of bound access to array"
    #endif
    _SHR_PBMP_WORD_SET(data->trigger_value, 0, 0xFDFFFFFF);
    _SHR_PBMP_WORD_SET(data->trigger_value, 1, 0xFFFFFFFF);
    _SHR_PBMP_WORD_SET(data->trigger_value, 2, 0xFFFFFFFF);
    _SHR_PBMP_WORD_SET(data->trigger_value, 3, 0xFFFFFFFF);
    _SHR_PBMP_WORD_SET(data->trigger_value, 4, 0xFFFFFFFF);
    _SHR_PBMP_WORD_SET(data->trigger_value, 5, 0xFFFFFFFF);
    _SHR_PBMP_WORD_SET(data->trigger_value, 6, 0xFFFFFFFF);
    _SHR_PBMP_WORD_SET(data->trigger_value, 7, 0xFFFFFFFF);

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}



shr_error_e
q2a_a0_data_access_attach(
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
    
    submodule_index = dnx_data_access_submodule_blocks;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnx_data_access_blocks_table_soft_init;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_access_blocks_soft_init_set;

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

