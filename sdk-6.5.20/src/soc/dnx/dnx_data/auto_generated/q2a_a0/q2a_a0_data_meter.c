

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_POLICER

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_meter.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>







static shr_error_e
q2a_a0_dnx_data_meter_diag_rate_measurements_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_diag;
    int feature_index = dnx_data_meter_diag_rate_measurements_support;
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
q2a_a0_dnx_data_meter_meter_db_tcsm_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_meter_db;
    int feature_index = dnx_data_meter_meter_db_tcsm_support;
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
q2a_a0_dnx_data_meter_mem_mgmt_base_big_engine_for_meter_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_mem_mgmt;
    int define_index = dnx_data_meter_mem_mgmt_define_base_big_engine_for_meter;
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
q2a_a0_dnx_data_meter_mem_mgmt_meter_pointer_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_mem_mgmt;
    int define_index = dnx_data_meter_mem_mgmt_define_meter_pointer_size;
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
q2a_a0_dnx_data_meter_mem_mgmt_meter_pointer_map_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_mem_mgmt;
    int define_index = dnx_data_meter_mem_mgmt_define_meter_pointer_map_size;
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
q2a_a0_dnx_data_meter_mem_mgmt_invalid_bank_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_mem_mgmt;
    int define_index = dnx_data_meter_mem_mgmt_define_invalid_bank_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 63;

    
    define->data = 63;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_meter_mem_mgmt_ptr_map_table_resolution_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_mem_mgmt;
    int define_index = dnx_data_meter_mem_mgmt_define_ptr_map_table_resolution;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1024;

    
    define->data = 1024;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_meter_mem_mgmt_bank_id_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_mem_mgmt;
    int define_index = dnx_data_meter_mem_mgmt_define_bank_id_size;
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
q2a_a0_dnx_data_meter_mem_mgmt_sections_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_mem_mgmt;
    int define_index = dnx_data_meter_mem_mgmt_define_sections_offset;
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
q2a_a0_dnx_data_meter_expansion_expansion_based_on_tc_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_expansion;
    int feature_index = dnx_data_meter_expansion_expansion_based_on_tc_support;
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
q2a_a0_dnx_data_meter_expansion_max_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_meter;
    int submodule_index = dnx_data_meter_submodule_expansion;
    int define_index = dnx_data_meter_expansion_define_max_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 7;

    
    define->data = 7;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}




shr_error_e
q2a_a0_data_meter_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_meter;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_meter_submodule_diag;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_meter_diag_rate_measurements_support;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_meter_diag_rate_measurements_support_set;

    
    
    submodule_index = dnx_data_meter_submodule_meter_db;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_meter_meter_db_tcsm_support;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_meter_meter_db_tcsm_support_set;

    
    
    submodule_index = dnx_data_meter_submodule_mem_mgmt;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_meter_mem_mgmt_define_base_big_engine_for_meter;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_meter_mem_mgmt_base_big_engine_for_meter_set;
    data_index = dnx_data_meter_mem_mgmt_define_meter_pointer_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_meter_mem_mgmt_meter_pointer_size_set;
    data_index = dnx_data_meter_mem_mgmt_define_meter_pointer_map_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_meter_mem_mgmt_meter_pointer_map_size_set;
    data_index = dnx_data_meter_mem_mgmt_define_invalid_bank_id;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_meter_mem_mgmt_invalid_bank_id_set;
    data_index = dnx_data_meter_mem_mgmt_define_ptr_map_table_resolution;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_meter_mem_mgmt_ptr_map_table_resolution_set;
    data_index = dnx_data_meter_mem_mgmt_define_bank_id_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_meter_mem_mgmt_bank_id_size_set;
    data_index = dnx_data_meter_mem_mgmt_define_sections_offset;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_meter_mem_mgmt_sections_offset_set;

    

    
    
    submodule_index = dnx_data_meter_submodule_expansion;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_meter_expansion_define_max_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_meter_expansion_max_size_set;

    
    data_index = dnx_data_meter_expansion_expansion_based_on_tc_support;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_meter_expansion_expansion_based_on_tc_support_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

