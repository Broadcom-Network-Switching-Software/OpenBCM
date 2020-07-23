

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INGRCS

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_ingress_cs.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>
#include <soc/dnx/dbal/dbal_structures.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dram.h>








static shr_error_e
jr2_a0_dnx_data_ingress_cs_properties_cs_buffer_nof_bits_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingress_cs;
    int submodule_index = dnx_data_ingress_cs_submodule_properties;
    int define_index = dnx_data_ingress_cs_properties_define_cs_buffer_nof_bits_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 200;

    
    define->data = 200;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_ingress_cs_properties_nof_bits_entry_size_prefix_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ingress_cs;
    int submodule_index = dnx_data_ingress_cs_submodule_properties;
    int define_index = dnx_data_ingress_cs_properties_define_nof_bits_entry_size_prefix;
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
jr2_a0_dnx_data_ingress_cs_properties_per_stage_set(
    int unit)
{
    int stage_index;
    dnx_data_ingress_cs_properties_per_stage_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_ingress_cs;
    int submodule_index = dnx_data_ingress_cs_submodule_properties;
    int table_index = dnx_data_ingress_cs_properties_table_per_stage;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DBAL_NOF_STAGES;
    table->info_get.key_size[0] = DBAL_NOF_STAGES;

    
    table->values[0].default_val = "FALSE";
    table->values[1].default_val = "FALSE";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_ingress_cs_properties_per_stage_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_ingress_cs_properties_table_per_stage");

    
    default_data = (dnx_data_ingress_cs_properties_per_stage_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->valid = FALSE;
    default_data->supports_half_entries = FALSE;
    
    for (stage_index = 0; stage_index < table->keys[0].size; stage_index++)
    {
        data = (dnx_data_ingress_cs_properties_per_stage_t *) dnxc_data_mgmt_table_data_get(unit, table, stage_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_STAGE_PRT < table->keys[0].size)
    {
        data = (dnx_data_ingress_cs_properties_per_stage_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_STAGE_PRT, 0);
        data->valid = TRUE;
        data->supports_half_entries = TRUE;
    }
    if (DBAL_STAGE_LLR < table->keys[0].size)
    {
        data = (dnx_data_ingress_cs_properties_per_stage_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_STAGE_LLR, 0);
        data->valid = TRUE;
        data->supports_half_entries = FALSE;
    }
    if (DBAL_STAGE_VT1 < table->keys[0].size)
    {
        data = (dnx_data_ingress_cs_properties_per_stage_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_STAGE_VT1, 0);
        data->valid = TRUE;
        data->supports_half_entries = FALSE;
    }
    if (DBAL_STAGE_VT2 < table->keys[0].size)
    {
        data = (dnx_data_ingress_cs_properties_per_stage_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_STAGE_VT2, 0);
        data->valid = TRUE;
        data->supports_half_entries = FALSE;
    }
    if (DBAL_STAGE_VT3 < table->keys[0].size)
    {
        data = (dnx_data_ingress_cs_properties_per_stage_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_STAGE_VT3, 0);
        data->valid = TRUE;
        data->supports_half_entries = FALSE;
    }
    if (DBAL_STAGE_VT4 < table->keys[0].size)
    {
        data = (dnx_data_ingress_cs_properties_per_stage_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_STAGE_VT4, 0);
        data->valid = TRUE;
        data->supports_half_entries = FALSE;
    }
    if (DBAL_STAGE_VT5 < table->keys[0].size)
    {
        data = (dnx_data_ingress_cs_properties_per_stage_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_STAGE_VT5, 0);
        data->valid = TRUE;
        data->supports_half_entries = FALSE;
    }
    if (DBAL_STAGE_FWD1 < table->keys[0].size)
    {
        data = (dnx_data_ingress_cs_properties_per_stage_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_STAGE_FWD1, 0);
        data->valid = TRUE;
        data->supports_half_entries = FALSE;
    }
    if (DBAL_STAGE_FWD2 < table->keys[0].size)
    {
        data = (dnx_data_ingress_cs_properties_per_stage_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_STAGE_FWD2, 0);
        data->valid = TRUE;
        data->supports_half_entries = FALSE;
    }
    if (DBAL_STAGE_IPMF1 < table->keys[0].size)
    {
        data = (dnx_data_ingress_cs_properties_per_stage_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_STAGE_IPMF1, 0);
        data->valid = TRUE;
        data->supports_half_entries = FALSE;
    }
    if (DBAL_STAGE_IPMF2 < table->keys[0].size)
    {
        data = (dnx_data_ingress_cs_properties_per_stage_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_STAGE_IPMF2, 0);
        data->valid = TRUE;
        data->supports_half_entries = FALSE;
    }
    if (DBAL_STAGE_IPMF3 < table->keys[0].size)
    {
        data = (dnx_data_ingress_cs_properties_per_stage_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_STAGE_IPMF3, 0);
        data->valid = TRUE;
        data->supports_half_entries = FALSE;
    }
    if (DBAL_STAGE_EPMF < table->keys[0].size)
    {
        data = (dnx_data_ingress_cs_properties_per_stage_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_STAGE_EPMF, 0);
        data->valid = TRUE;
        data->supports_half_entries = FALSE;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
jr2_a0_dnx_data_ingress_cs_features_disable_ecc_fix_en_before_read_write_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_ingress_cs;
    int submodule_index = dnx_data_ingress_cs_submodule_features;
    int feature_index = dnx_data_ingress_cs_features_disable_ecc_fix_en_before_read_write;
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
jr2_a0_dnx_data_ingress_cs_parser_parsing_context_map_enable_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_ingress_cs;
    int submodule_index = dnx_data_ingress_cs_submodule_parser;
    int feature_index = dnx_data_ingress_cs_parser_parsing_context_map_enable;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}





shr_error_e
jr2_a0_data_ingress_cs_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_ingress_cs;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_ingress_cs_submodule_properties;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_ingress_cs_properties_define_cs_buffer_nof_bits_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingress_cs_properties_cs_buffer_nof_bits_max_set;
    data_index = dnx_data_ingress_cs_properties_define_nof_bits_entry_size_prefix;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_ingress_cs_properties_nof_bits_entry_size_prefix_set;

    

    
    data_index = dnx_data_ingress_cs_properties_table_per_stage;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_ingress_cs_properties_per_stage_set;
    
    submodule_index = dnx_data_ingress_cs_submodule_features;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_ingress_cs_features_disable_ecc_fix_en_before_read_write;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_ingress_cs_features_disable_ecc_fix_en_before_read_write_set;

    
    
    submodule_index = dnx_data_ingress_cs_submodule_parser;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_ingress_cs_parser_parsing_context_map_enable;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_ingress_cs_parser_parsing_context_map_enable_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

