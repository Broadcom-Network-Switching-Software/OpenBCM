

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOC_DIAG

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>









static shr_error_e
jr2_a0_dnx_data_debug_mem_params_set(
    int unit)
{
    int block_index;
    dnx_data_debug_mem_params_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_debug;
    int submodule_index = dnx_data_debug_submodule_mem;
    int table_index = dnx_data_debug_mem_table_params;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DBAL_NOF_ENUM_PP_ASIC_BLOCK_VALUES;
    table->info_get.key_size[0] = DBAL_NOF_ENUM_PP_ASIC_BLOCK_VALUES;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_debug_mem_params_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_debug_mem_table_params");

    
    default_data = (dnx_data_debug_mem_params_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->valid = 0;
    default_data->line_nof = 0;
    default_data->width = 0;
    
    for (block_index = 0; block_index < table->keys[0].size; block_index++)
    {
        data = (dnx_data_debug_mem_params_t *) dnxc_data_mgmt_table_data_get(unit, table, block_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPA < table->keys[0].size)
    {
        data = (dnx_data_debug_mem_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPA, 0);
        data->valid = 1;
        data->line_nof = 10;
        data->width = 1509;
    }
    if (DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPB < table->keys[0].size)
    {
        data = (dnx_data_debug_mem_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPB, 0);
        data->valid = 1;
        data->line_nof = 43;
        data->width = 1868;
    }
    if (DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPC < table->keys[0].size)
    {
        data = (dnx_data_debug_mem_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPC, 0);
        data->valid = 1;
        data->line_nof = 5 ;
        data->width = 1883;
    }
    if (DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPD < table->keys[0].size)
    {
        data = (dnx_data_debug_mem_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPD, 0);
        data->valid = 1;
        data->line_nof = 17;
        data->width = 2369;
    }
    if (DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPE < table->keys[0].size)
    {
        data = (dnx_data_debug_mem_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPE, 0);
        data->valid = 1;
        data->line_nof = 3 ;
        data->width = 1218;
    }
    if (DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPF < table->keys[0].size)
    {
        data = (dnx_data_debug_mem_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPF, 0);
        data->valid = 1;
        data->line_nof = 21;
        data->width = 1509;
    }
    if (DBAL_ENUM_FVAL_PP_ASIC_BLOCK_ERPP < table->keys[0].size)
    {
        data = (dnx_data_debug_mem_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_PP_ASIC_BLOCK_ERPP, 0);
        data->valid = 1;
        data->line_nof = 13;
        data->width = 1152;
    }
    if (DBAL_ENUM_FVAL_PP_ASIC_BLOCK_ETPPA < table->keys[0].size)
    {
        data = (dnx_data_debug_mem_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_PP_ASIC_BLOCK_ETPPA, 0);
        data->valid = 1;
        data->line_nof = 3 ;
        data->width = 1152;
    }
    if (DBAL_ENUM_FVAL_PP_ASIC_BLOCK_ETPPB < table->keys[0].size)
    {
        data = (dnx_data_debug_mem_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_PP_ASIC_BLOCK_ETPPB, 0);
        data->valid = 1;
        data->line_nof = 8 ;
        data->width = 1301;
    }
    if (DBAL_ENUM_FVAL_PP_ASIC_BLOCK_ETPPC < table->keys[0].size)
    {
        data = (dnx_data_debug_mem_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_PP_ASIC_BLOCK_ETPPC, 0);
        data->valid = 1;
        data->line_nof = 10;
        data->width = 2219;
    }
    if (DBAL_ENUM_FVAL_PP_ASIC_BLOCK_TO_ENGINE < table->keys[0].size)
    {
        data = (dnx_data_debug_mem_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_PP_ASIC_BLOCK_TO_ENGINE, 0);
        data->valid = 1;
        data->line_nof = 256;
        data->width = 640;
    }
    if (DBAL_ENUM_FVAL_PP_ASIC_BLOCK_FROM_ENGINE < table->keys[0].size)
    {
        data = (dnx_data_debug_mem_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_PP_ASIC_BLOCK_FROM_ENGINE, 0);
        data->valid = 1;
        data->line_nof = 256;
        data->width = 640;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
jr2_a0_dnx_data_debug_feature_no_hit_bit_on_mdb_access_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_debug;
    int submodule_index = dnx_data_debug_submodule_feature;
    int feature_index = dnx_data_debug_feature_no_hit_bit_on_mdb_access;
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
jr2_a0_dnx_data_debug_feature_pem_vt_init_fix_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_debug;
    int submodule_index = dnx_data_debug_submodule_feature;
    int feature_index = dnx_data_debug_feature_pem_vt_init_fix;
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
jr2_a0_dnx_data_debug_feature_iparser_signals_valid_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_debug;
    int submodule_index = dnx_data_debug_submodule_feature;
    int feature_index = dnx_data_debug_feature_iparser_signals_valid;
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
jr2_a0_dnx_data_debug_feature_fwd_action_signals_from_lbp_valid_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_debug;
    int submodule_index = dnx_data_debug_submodule_feature;
    int feature_index = dnx_data_debug_feature_fwd_action_signals_from_lbp_valid;
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
jr2_a0_dnx_data_debug_kleap_fwd1_gen_data_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_debug;
    int submodule_index = dnx_data_debug_submodule_kleap;
    int define_index = dnx_data_debug_kleap_define_fwd1_gen_data_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2738;

    
    define->data = 2738;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_debug_kleap_fwd2_gen_data_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_debug;
    int submodule_index = dnx_data_debug_submodule_kleap;
    int define_index = dnx_data_debug_kleap_define_fwd2_gen_data_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3161;

    
    define->data = 3161;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_debug_general_global_visibility_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_debug;
    int submodule_index = dnx_data_debug_submodule_general;
    int define_index = dnx_data_debug_general_define_global_visibility;
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
jr2_a0_dnx_data_debug_general_etpp_eop_and_sop_dec_above_threshold_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_debug;
    int submodule_index = dnx_data_debug_submodule_general;
    int define_index = dnx_data_debug_general_define_etpp_eop_and_sop_dec_above_threshold;
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
jr2_a0_dnx_data_debug_general_egress_port_mc_visibility_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_debug;
    int submodule_index = dnx_data_debug_submodule_general;
    int define_index = dnx_data_debug_general_define_egress_port_mc_visibility;
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
jr2_a0_dnx_data_debug_general_mdb_debug_signals_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_debug;
    int submodule_index = dnx_data_debug_submodule_general;
    int define_index = dnx_data_debug_general_define_mdb_debug_signals;
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
jr2_a0_dnx_data_debug_general_mem_array_index_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_debug;
    int submodule_index = dnx_data_debug_submodule_general;
    int define_index = dnx_data_debug_general_define_mem_array_index;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}




shr_error_e
jr2_a0_data_debug_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_debug;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_debug_submodule_mem;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnx_data_debug_mem_table_params;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_debug_mem_params_set;
    
    submodule_index = dnx_data_debug_submodule_feature;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_debug_feature_no_hit_bit_on_mdb_access;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_debug_feature_no_hit_bit_on_mdb_access_set;
    data_index = dnx_data_debug_feature_pem_vt_init_fix;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_debug_feature_pem_vt_init_fix_set;
    data_index = dnx_data_debug_feature_iparser_signals_valid;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_debug_feature_iparser_signals_valid_set;
    data_index = dnx_data_debug_feature_fwd_action_signals_from_lbp_valid;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_debug_feature_fwd_action_signals_from_lbp_valid_set;

    
    
    submodule_index = dnx_data_debug_submodule_kleap;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_debug_kleap_define_fwd1_gen_data_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_debug_kleap_fwd1_gen_data_size_set;
    data_index = dnx_data_debug_kleap_define_fwd2_gen_data_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_debug_kleap_fwd2_gen_data_size_set;

    

    
    
    submodule_index = dnx_data_debug_submodule_general;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_debug_general_define_global_visibility;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_debug_general_global_visibility_set;
    data_index = dnx_data_debug_general_define_etpp_eop_and_sop_dec_above_threshold;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_debug_general_etpp_eop_and_sop_dec_above_threshold_set;
    data_index = dnx_data_debug_general_define_egress_port_mc_visibility;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_debug_general_egress_port_mc_visibility_set;
    data_index = dnx_data_debug_general_define_mdb_debug_signals;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_debug_general_mdb_debug_signals_set;
    data_index = dnx_data_debug_general_define_mem_array_index;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_debug_general_mem_array_index_set;

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

