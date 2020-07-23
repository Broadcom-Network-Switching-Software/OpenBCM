

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_SWITCH

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_spb.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>







static shr_error_e
j2p_a0_dnx_data_spb_ocb_blocks_in_ring_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_spb;
    int submodule_index = dnx_data_spb_submodule_ocb;
    int feature_index = dnx_data_spb_ocb_blocks_in_ring;
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
j2p_a0_dnx_data_spb_ocb_nof_blocks_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_spb;
    int submodule_index = dnx_data_spb_submodule_ocb;
    int define_index = dnx_data_spb_ocb_define_nof_blocks;
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
j2p_a0_dnx_data_spb_ocb_last_in_chain_set(
    int unit)
{
    int ocb_index_index;
    dnx_data_spb_ocb_last_in_chain_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_spb;
    int submodule_index = dnx_data_spb_submodule_ocb;
    int table_index = dnx_data_spb_ocb_table_last_in_chain;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_spb.ocb.nof_blocks_get(unit);
    table->info_get.key_size[0] = dnx_data_spb.ocb.nof_blocks_get(unit);

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_spb_ocb_last_in_chain_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_spb_ocb_table_last_in_chain");

    
    default_data = (dnx_data_spb_ocb_last_in_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->is_last_in_chain = 0;
    
    for (ocb_index_index = 0; ocb_index_index < table->keys[0].size; ocb_index_index++)
    {
        data = (dnx_data_spb_ocb_last_in_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, ocb_index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_spb_ocb_last_in_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->is_last_in_chain = 1;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_spb_ocb_last_in_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->is_last_in_chain = 1;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_spb_ocb_last_in_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->is_last_in_chain = 1;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_spb_ocb_last_in_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->is_last_in_chain = 1;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}



shr_error_e
j2p_a0_data_spb_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_spb;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_spb_submodule_ocb;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_spb_ocb_define_nof_blocks;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_spb_ocb_nof_blocks_set;

    
    data_index = dnx_data_spb_ocb_blocks_in_ring;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_spb_ocb_blocks_in_ring_set;

    
    data_index = dnx_data_spb_ocb_table_last_in_chain;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_spb_ocb_last_in_chain_set;

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

