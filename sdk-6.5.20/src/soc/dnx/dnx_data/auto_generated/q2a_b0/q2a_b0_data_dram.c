

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_DRAM

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_dram.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>







static shr_error_e
q2a_b0_dnx_data_dram_gddr6_allow_disable_read_crc_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_gddr6;
    int feature_index = dnx_data_dram_gddr6_allow_disable_read_crc;
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
q2a_b0_dnx_data_dram_gddr6_interleaved_refresh_cycles_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_gddr6;
    int feature_index = dnx_data_dram_gddr6_interleaved_refresh_cycles;
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
q2a_b0_dnx_data_dram_gddr6_refresh_pend_switch_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_gddr6;
    int feature_index = dnx_data_dram_gddr6_refresh_pend_switch;
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
q2a_b0_dnx_data_dram_gddr6_cadt_exit_mode_when_done_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_gddr6;
    int feature_index = dnx_data_dram_gddr6_cadt_exit_mode_when_done;
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
q2a_b0_dnx_data_dram_gddr6_periodic_temp_readout_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_gddr6;
    int feature_index = dnx_data_dram_gddr6_periodic_temp_readout;
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
q2a_b0_dnx_data_dram_gddr6_readout_to_readout_prd_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_gddr6;
    int define_index = dnx_data_dram_gddr6_define_readout_to_readout_prd;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3;

    
    define->data = 3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_b0_dnx_data_dram_gddr6_refresh_to_readout_prd_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_gddr6;
    int define_index = dnx_data_dram_gddr6_define_refresh_to_readout_prd;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 35;

    
    define->data = 35;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_b0_dnx_data_dram_gddr6_readout_done_to_done_prd_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_gddr6;
    int define_index = dnx_data_dram_gddr6_define_readout_done_to_done_prd;
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
q2a_b0_dnx_data_dram_general_info_is_temperature_reading_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_dram;
    int submodule_index = dnx_data_dram_submodule_general_info;
    int feature_index = dnx_data_dram_general_info_is_temperature_reading_supported;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}





shr_error_e
q2a_b0_data_dram_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_dram;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_dram_submodule_gddr6;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_dram_gddr6_define_readout_to_readout_prd;
    define = &submodule->defines[data_index];
    define->set = q2a_b0_dnx_data_dram_gddr6_readout_to_readout_prd_set;
    data_index = dnx_data_dram_gddr6_define_refresh_to_readout_prd;
    define = &submodule->defines[data_index];
    define->set = q2a_b0_dnx_data_dram_gddr6_refresh_to_readout_prd_set;
    data_index = dnx_data_dram_gddr6_define_readout_done_to_done_prd;
    define = &submodule->defines[data_index];
    define->set = q2a_b0_dnx_data_dram_gddr6_readout_done_to_done_prd_set;

    
    data_index = dnx_data_dram_gddr6_allow_disable_read_crc;
    feature = &submodule->features[data_index];
    feature->set = q2a_b0_dnx_data_dram_gddr6_allow_disable_read_crc_set;
    data_index = dnx_data_dram_gddr6_interleaved_refresh_cycles;
    feature = &submodule->features[data_index];
    feature->set = q2a_b0_dnx_data_dram_gddr6_interleaved_refresh_cycles_set;
    data_index = dnx_data_dram_gddr6_refresh_pend_switch;
    feature = &submodule->features[data_index];
    feature->set = q2a_b0_dnx_data_dram_gddr6_refresh_pend_switch_set;
    data_index = dnx_data_dram_gddr6_cadt_exit_mode_when_done;
    feature = &submodule->features[data_index];
    feature->set = q2a_b0_dnx_data_dram_gddr6_cadt_exit_mode_when_done_set;
    data_index = dnx_data_dram_gddr6_periodic_temp_readout;
    feature = &submodule->features[data_index];
    feature->set = q2a_b0_dnx_data_dram_gddr6_periodic_temp_readout_set;

    
    
    submodule_index = dnx_data_dram_submodule_general_info;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_dram_general_info_is_temperature_reading_supported;
    feature = &submodule->features[data_index];
    feature->set = q2a_b0_dnx_data_dram_general_info_is_temperature_reading_supported_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

