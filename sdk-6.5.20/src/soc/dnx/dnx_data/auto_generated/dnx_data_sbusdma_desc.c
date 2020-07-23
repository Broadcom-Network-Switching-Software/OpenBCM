

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



extern shr_error_e jr2_a0_data_sbusdma_desc_attach(
    int unit);



static shr_error_e
dnx_data_sbusdma_desc_global_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "global";
    submodule_data->doc = "Descriptor DMA global defines";
    
    submodule_data->nof_features = _dnx_data_sbusdma_desc_global_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data sbusdma_desc global features");

    submodule_data->features[dnx_data_sbusdma_desc_global_desc_dma].name = "desc_dma";
    submodule_data->features[dnx_data_sbusdma_desc_global_desc_dma].doc = "Descriptor DMA aggregation and async commit to HW.";
    submodule_data->features[dnx_data_sbusdma_desc_global_desc_dma].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_sbusdma_desc_global_force_enable].name = "force_enable";
    submodule_data->features[dnx_data_sbusdma_desc_global_force_enable].doc = "Force enable descriptor DMA to ignore image limitations.";
    submodule_data->features[dnx_data_sbusdma_desc_global_force_enable].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_sbusdma_desc_global_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data sbusdma_desc global defines");

    submodule_data->defines[dnx_data_sbusdma_desc_global_define_dma_desc_aggregator_chain_length_max].name = "dma_desc_aggregator_chain_length_max";
    submodule_data->defines[dnx_data_sbusdma_desc_global_define_dma_desc_aggregator_chain_length_max].doc = "Maximal number of descriptors in a single chain.";
    
    submodule_data->defines[dnx_data_sbusdma_desc_global_define_dma_desc_aggregator_chain_length_max].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_sbusdma_desc_global_define_dma_desc_aggregator_buff_size_kb].name = "dma_desc_aggregator_buff_size_kb";
    submodule_data->defines[dnx_data_sbusdma_desc_global_define_dma_desc_aggregator_buff_size_kb].doc = "Total size in KB of dmmable memory allocated in favor of the descriptor DMA double-buffer.";
    
    submodule_data->defines[dnx_data_sbusdma_desc_global_define_dma_desc_aggregator_buff_size_kb].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_sbusdma_desc_global_define_dma_desc_aggregator_timeout_usec].name = "dma_desc_aggregator_timeout_usec";
    submodule_data->defines[dnx_data_sbusdma_desc_global_define_dma_desc_aggregator_timeout_usec].doc = "Timeout between the creation of a descriptor chain and its commit to HW";
    
    submodule_data->defines[dnx_data_sbusdma_desc_global_define_dma_desc_aggregator_timeout_usec].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_sbusdma_desc_global_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data sbusdma_desc global tables");

    
    submodule_data->tables[dnx_data_sbusdma_desc_global_table_enable_module_desc_dma].name = "enable_module_desc_dma";
    submodule_data->tables[dnx_data_sbusdma_desc_global_table_enable_module_desc_dma].doc = "Enable descriptor DMA per module";
    submodule_data->tables[dnx_data_sbusdma_desc_global_table_enable_module_desc_dma].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_sbusdma_desc_global_table_enable_module_desc_dma].size_of_values = sizeof(dnx_data_sbusdma_desc_global_enable_module_desc_dma_t);
    submodule_data->tables[dnx_data_sbusdma_desc_global_table_enable_module_desc_dma].entry_get = dnx_data_sbusdma_desc_global_enable_module_desc_dma_entry_str_get;

    
    submodule_data->tables[dnx_data_sbusdma_desc_global_table_enable_module_desc_dma].nof_keys = 1;
    submodule_data->tables[dnx_data_sbusdma_desc_global_table_enable_module_desc_dma].keys[0].name = "module_enum_val";
    submodule_data->tables[dnx_data_sbusdma_desc_global_table_enable_module_desc_dma].keys[0].doc = "The module name.";

    
    submodule_data->tables[dnx_data_sbusdma_desc_global_table_enable_module_desc_dma].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_sbusdma_desc_global_table_enable_module_desc_dma].values, dnxc_data_value_t, submodule_data->tables[dnx_data_sbusdma_desc_global_table_enable_module_desc_dma].nof_values, "_dnx_data_sbusdma_desc_global_table_enable_module_desc_dma table values");
    submodule_data->tables[dnx_data_sbusdma_desc_global_table_enable_module_desc_dma].values[0].name = "enable";
    submodule_data->tables[dnx_data_sbusdma_desc_global_table_enable_module_desc_dma].values[0].type = "uint32";
    submodule_data->tables[dnx_data_sbusdma_desc_global_table_enable_module_desc_dma].values[0].doc = "Enable descriptor DMA for the given module.";
    submodule_data->tables[dnx_data_sbusdma_desc_global_table_enable_module_desc_dma].values[0].offset = UTILEX_OFFSETOF(dnx_data_sbusdma_desc_global_enable_module_desc_dma_t, enable);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_sbusdma_desc_global_feature_get(
    int unit,
    dnx_data_sbusdma_desc_global_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_sbusdma_desc, dnx_data_sbusdma_desc_submodule_global, feature);
}


uint32
dnx_data_sbusdma_desc_global_dma_desc_aggregator_chain_length_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sbusdma_desc, dnx_data_sbusdma_desc_submodule_global, dnx_data_sbusdma_desc_global_define_dma_desc_aggregator_chain_length_max);
}

uint32
dnx_data_sbusdma_desc_global_dma_desc_aggregator_buff_size_kb_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sbusdma_desc, dnx_data_sbusdma_desc_submodule_global, dnx_data_sbusdma_desc_global_define_dma_desc_aggregator_buff_size_kb);
}

uint32
dnx_data_sbusdma_desc_global_dma_desc_aggregator_timeout_usec_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_sbusdma_desc, dnx_data_sbusdma_desc_submodule_global, dnx_data_sbusdma_desc_global_define_dma_desc_aggregator_timeout_usec);
}



const dnx_data_sbusdma_desc_global_enable_module_desc_dma_t *
dnx_data_sbusdma_desc_global_enable_module_desc_dma_get(
    int unit,
    int module_enum_val)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_sbusdma_desc, dnx_data_sbusdma_desc_submodule_global, dnx_data_sbusdma_desc_global_table_enable_module_desc_dma);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, module_enum_val, 0);
    return (const dnx_data_sbusdma_desc_global_enable_module_desc_dma_t *) data;

}


shr_error_e
dnx_data_sbusdma_desc_global_enable_module_desc_dma_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_sbusdma_desc_global_enable_module_desc_dma_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_sbusdma_desc, dnx_data_sbusdma_desc_submodule_global, dnx_data_sbusdma_desc_global_table_enable_module_desc_dma);
    data = (const dnx_data_sbusdma_desc_global_enable_module_desc_dma_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->enable);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_sbusdma_desc_global_enable_module_desc_dma_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_sbusdma_desc, dnx_data_sbusdma_desc_submodule_global, dnx_data_sbusdma_desc_global_table_enable_module_desc_dma);

}



shr_error_e
dnx_data_sbusdma_desc_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "sbusdma_desc";
    module_data->nof_submodules = _dnx_data_sbusdma_desc_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data sbusdma_desc submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_sbusdma_desc_global_init(unit, &module_data->submodules[dnx_data_sbusdma_desc_submodule_global]));
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_sbusdma_desc_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_sbusdma_desc_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_sbusdma_desc_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_sbusdma_desc_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_sbusdma_desc_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_sbusdma_desc_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_sbusdma_desc_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_sbusdma_desc_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_sbusdma_desc_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

