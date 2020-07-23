

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_BIERDNX

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_headers.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>







static shr_error_e
j2c_a0_dnx_data_headers_optimized_ftmh_ftmh_opt_parse_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_optimized_ftmh;
    int feature_index = dnx_data_headers_optimized_ftmh_ftmh_opt_parse;
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
j2c_a0_dnx_data_headers_internal_header_visibilty_mode_global_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_internal;
    int feature_index = dnx_data_headers_internal_header_visibilty_mode_global;
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
j2c_a0_dnx_data_headers_internal_cfg_fai_unknown_address_enable_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_internal;
    int define_index = dnx_data_headers_internal_define_cfg_fai_unknown_address_enable;
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
j2c_a0_dnx_data_headers_system_headers_jr_mode_ftmh_oam_ts_right_shift_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_system_headers;
    int define_index = dnx_data_headers_system_headers_define_jr_mode_ftmh_oam_ts_right_shift;
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
j2c_a0_dnx_data_headers_otmh_otmh_enable_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_otmh;
    int feature_index = dnx_data_headers_otmh_otmh_enable;
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
j2c_a0_dnx_data_headers_general_fwd_header_offset_calc_with_egress_pars_idx_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_general;
    int feature_index = dnx_data_headers_general_fwd_header_offset_calc_with_egress_pars_idx;
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
j2c_a0_dnx_data_headers_feature_unknown_address_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_feature;
    int feature_index = dnx_data_headers_feature_unknown_address;
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
j2c_a0_dnx_data_headers_feature_tsh_ext_with_flow_id_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_headers;
    int submodule_index = dnx_data_headers_submodule_feature;
    int feature_index = dnx_data_headers_feature_tsh_ext_with_flow_id;
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
j2c_a0_data_headers_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_headers;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_headers_submodule_optimized_ftmh;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_headers_optimized_ftmh_ftmh_opt_parse;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_headers_optimized_ftmh_ftmh_opt_parse_set;

    
    
    submodule_index = dnx_data_headers_submodule_internal;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_headers_internal_define_cfg_fai_unknown_address_enable;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_headers_internal_cfg_fai_unknown_address_enable_set;

    
    data_index = dnx_data_headers_internal_header_visibilty_mode_global;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_headers_internal_header_visibilty_mode_global_set;

    
    
    submodule_index = dnx_data_headers_submodule_system_headers;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_headers_system_headers_define_jr_mode_ftmh_oam_ts_right_shift;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_headers_system_headers_jr_mode_ftmh_oam_ts_right_shift_set;

    

    
    
    submodule_index = dnx_data_headers_submodule_otmh;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_headers_otmh_otmh_enable;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_headers_otmh_otmh_enable_set;

    
    
    submodule_index = dnx_data_headers_submodule_general;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_headers_general_fwd_header_offset_calc_with_egress_pars_idx;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_headers_general_fwd_header_offset_calc_with_egress_pars_idx_set;

    
    
    submodule_index = dnx_data_headers_submodule_feature;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_headers_feature_unknown_address;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_headers_feature_unknown_address_set;
    data_index = dnx_data_headers_feature_tsh_ext_with_flow_id;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_headers_feature_tsh_ext_with_flow_id_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

