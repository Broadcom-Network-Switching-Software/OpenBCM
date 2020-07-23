

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_EGRESSDNX

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_tdm.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>
#include <bcm_int/dnx/tdm/tdm.h>







static shr_error_e
j2c_a0_dnx_data_tdm_params_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_tdm;
    int submodule_index = dnx_data_tdm_submodule_params;
    int feature_index = dnx_data_tdm_params_is_supported;
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
j2c_a0_dnx_data_tdm_params_context_map_base_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tdm;
    int submodule_index = dnx_data_tdm_submodule_params;
    int define_index = dnx_data_tdm_params_define_context_map_base_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 14;

    
    define->data = 14;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_tdm_params_context_map_base_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tdm;
    int submodule_index = dnx_data_tdm_submodule_params;
    int define_index = dnx_data_tdm_params_define_context_map_base_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1 << dnx_data_tdm.params.context_map_base_size_get(unit);

    
    define->data = 1 << dnx_data_tdm.params.context_map_base_size_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_tdm_params_sid_min_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tdm;
    int submodule_index = dnx_data_tdm_submodule_params;
    int define_index = dnx_data_tdm_params_define_sid_min_size;
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
j2c_a0_dnx_data_tdm_params_sid_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tdm;
    int submodule_index = dnx_data_tdm_submodule_params;
    int define_index = dnx_data_tdm_params_define_sid_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 14;

    
    define->data = 14;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_tdm_params_nof_stream_ids_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tdm;
    int submodule_index = dnx_data_tdm_submodule_params;
    int define_index = dnx_data_tdm_params_define_nof_stream_ids;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1 << dnx_data_tdm.params.sid_size_get(unit);

    
    define->data = 1 << dnx_data_tdm.params.sid_size_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_tdm_params_global_sid_offset_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tdm;
    int submodule_index = dnx_data_tdm_submodule_params;
    int define_index = dnx_data_tdm_params_define_global_sid_offset_size;
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
j2c_a0_dnx_data_tdm_params_global_sid_offset_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tdm;
    int submodule_index = dnx_data_tdm_submodule_params;
    int define_index = dnx_data_tdm_params_define_global_sid_offset_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1 << dnx_data_tdm.params.global_sid_offset_size_get(unit);

    
    define->data = 1 << dnx_data_tdm.params.global_sid_offset_size_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_tdm_params_sid_offset_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tdm;
    int submodule_index = dnx_data_tdm_submodule_params;
    int define_index = dnx_data_tdm_params_define_sid_offset_size;
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
j2c_a0_dnx_data_tdm_params_sid_offset_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tdm;
    int submodule_index = dnx_data_tdm_submodule_params;
    int define_index = dnx_data_tdm_params_define_sid_offset_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (1 << (dnx_data_tdm.params.sid_offset_size_get(unit)-1))+1;

    
    define->data = (1 << (dnx_data_tdm.params.sid_offset_size_get(unit)-1))+1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_tdm_params_sid_offset_nof_in_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tdm;
    int submodule_index = dnx_data_tdm_submodule_params;
    int define_index = dnx_data_tdm_params_define_sid_offset_nof_in_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = ((dnx_data_tdm.params.sid_offset_nof_get(unit)-1)*8)+1;

    
    define->data = ((dnx_data_tdm.params.sid_offset_nof_get(unit)-1)*8)+1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_tdm_params_context_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tdm;
    int submodule_index = dnx_data_tdm_submodule_params;
    int define_index = dnx_data_tdm_params_define_context_size;
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
j2c_a0_dnx_data_tdm_params_nof_contexts_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tdm;
    int submodule_index = dnx_data_tdm_submodule_params;
    int define_index = dnx_data_tdm_params_define_nof_contexts;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1 << dnx_data_tdm.params.context_size_get(unit);

    
    define->data = 1 << dnx_data_tdm.params.context_size_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_tdm_params_pkt_size_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tdm;
    int submodule_index = dnx_data_tdm_submodule_params;
    int define_index = dnx_data_tdm_params_define_pkt_size_size;
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
j2c_a0_dnx_data_tdm_params_pkt_size_lower_limit_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tdm;
    int submodule_index = dnx_data_tdm_submodule_params;
    int define_index = dnx_data_tdm_params_define_pkt_size_lower_limit;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 64;

    
    define->data = 64;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_tdm_params_pkt_size_upper_limit_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tdm;
    int submodule_index = dnx_data_tdm_submodule_params;
    int define_index = dnx_data_tdm_params_define_pkt_size_upper_limit;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 511;

    
    define->data = 511;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_tdm_params_link_mask_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tdm;
    int submodule_index = dnx_data_tdm_submodule_params;
    int define_index = dnx_data_tdm_params_define_link_mask_size;
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
j2c_a0_dnx_data_tdm_params_link_mask_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tdm;
    int submodule_index = dnx_data_tdm_submodule_params;
    int define_index = dnx_data_tdm_params_define_link_mask_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 36;

    
    define->data = 36;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_tdm_params_tdm_tc_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tdm;
    int submodule_index = dnx_data_tdm_submodule_params;
    int define_index = dnx_data_tdm_params_define_tdm_tc;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_TDM_EGRESS_PRIORITY;
    define->property.doc = 
        "TDM tc\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 7;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_tdm_params_tdm_dp_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tdm;
    int submodule_index = dnx_data_tdm_submodule_params;
    int define_index = dnx_data_tdm_params_define_tdm_dp;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_TDM_EGRESS_DP;
    define->property.doc = 
        "TDM dp\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 3;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_tdm_params_mode_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_tdm;
    int submodule_index = dnx_data_tdm_submodule_params;
    int define_index = dnx_data_tdm_params_define_mode;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = TDM_MODE_NONE;

    
    define->data = TDM_MODE_NONE;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_TDM_MODE;
    define->property.doc = 
        "TDM system mode\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 4;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "TDM_NONE";
    define->property.mapping[0].val = TDM_MODE_NONE;
    define->property.mapping[0].is_default = 1 ;
    define->property.mapping[1].name = "TDM_OPTIMIZED";
    define->property.mapping[1].val = TDM_MODE_OPTIMIZED;
    define->property.mapping[2].name = "TDM_STANDARD";
    define->property.mapping[2].val = TDM_MODE_STANDARD;
    define->property.mapping[3].name = "TDM_PACKET";
    define->property.mapping[3].val = TDM_MODE_PACKET;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}




shr_error_e
j2c_a0_data_tdm_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_tdm;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_tdm_submodule_params;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_tdm_params_define_context_map_base_size;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_tdm_params_context_map_base_size_set;
    data_index = dnx_data_tdm_params_define_context_map_base_nof;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_tdm_params_context_map_base_nof_set;
    data_index = dnx_data_tdm_params_define_sid_min_size;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_tdm_params_sid_min_size_set;
    data_index = dnx_data_tdm_params_define_sid_size;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_tdm_params_sid_size_set;
    data_index = dnx_data_tdm_params_define_nof_stream_ids;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_tdm_params_nof_stream_ids_set;
    data_index = dnx_data_tdm_params_define_global_sid_offset_size;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_tdm_params_global_sid_offset_size_set;
    data_index = dnx_data_tdm_params_define_global_sid_offset_nof;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_tdm_params_global_sid_offset_nof_set;
    data_index = dnx_data_tdm_params_define_sid_offset_size;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_tdm_params_sid_offset_size_set;
    data_index = dnx_data_tdm_params_define_sid_offset_nof;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_tdm_params_sid_offset_nof_set;
    data_index = dnx_data_tdm_params_define_sid_offset_nof_in_bits;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_tdm_params_sid_offset_nof_in_bits_set;
    data_index = dnx_data_tdm_params_define_context_size;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_tdm_params_context_size_set;
    data_index = dnx_data_tdm_params_define_nof_contexts;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_tdm_params_nof_contexts_set;
    data_index = dnx_data_tdm_params_define_pkt_size_size;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_tdm_params_pkt_size_size_set;
    data_index = dnx_data_tdm_params_define_pkt_size_lower_limit;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_tdm_params_pkt_size_lower_limit_set;
    data_index = dnx_data_tdm_params_define_pkt_size_upper_limit;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_tdm_params_pkt_size_upper_limit_set;
    data_index = dnx_data_tdm_params_define_link_mask_size;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_tdm_params_link_mask_size_set;
    data_index = dnx_data_tdm_params_define_link_mask_nof;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_tdm_params_link_mask_nof_set;
    data_index = dnx_data_tdm_params_define_tdm_tc;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_tdm_params_tdm_tc_set;
    data_index = dnx_data_tdm_params_define_tdm_dp;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_tdm_params_tdm_dp_set;
    data_index = dnx_data_tdm_params_define_mode;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_tdm_params_mode_set;

    
    data_index = dnx_data_tdm_params_is_supported;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_tdm_params_is_supported_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

