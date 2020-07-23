

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_LIF

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_lif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>







static shr_error_e
q2a_a0_dnx_data_lif_global_lif_prevent_tunnel_update_rif_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_lif;
    int submodule_index = dnx_data_lif_submodule_global_lif;
    int feature_index = dnx_data_lif_global_lif_prevent_tunnel_update_rif;
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
q2a_a0_dnx_data_lif_global_lif_glem_rif_optimization_enabled_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_lif;
    int submodule_index = dnx_data_lif_submodule_global_lif;
    int define_index = dnx_data_lif_global_lif_define_glem_rif_optimization_enabled;
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
q2a_a0_dnx_data_lif_global_lif_null_lif_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_lif;
    int submodule_index = dnx_data_lif_submodule_global_lif;
    int define_index = dnx_data_lif_global_lif_define_null_lif;
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
q2a_a0_dnx_data_lif_global_lif_egress_in_lif_null_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_lif;
    int submodule_index = dnx_data_lif_submodule_global_lif;
    int define_index = dnx_data_lif_global_lif_define_egress_in_lif_null_value;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x0;

    
    define->data = 0x0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_SYSTEM_HEADERS_MODE;
    define->property.doc = 
        "IN_LIF null value in egress stage\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 2;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "0";
    define->property.mapping[0].val = 0x0;
    define->property.mapping[0].is_default = 1 ;
    define->property.mapping[1].name = "1";
    define->property.mapping[1].val = 0x0;
    define->property.mapping[1].is_default = 1 ;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
q2a_a0_dnx_data_lif_in_lif_mapped_in_lif_profile_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_lif;
    int submodule_index = dnx_data_lif_submodule_in_lif;
    int feature_index = dnx_data_lif_in_lif_mapped_in_lif_profile;
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
q2a_a0_dnx_data_lif_in_lif_in_lif_profile_iop_mode_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_lif;
    int submodule_index = dnx_data_lif_submodule_in_lif;
    int feature_index = dnx_data_lif_in_lif_in_lif_profile_iop_mode;
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
q2a_a0_dnx_data_lif_out_lif_glem_result_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_lif;
    int submodule_index = dnx_data_lif_submodule_out_lif;
    int define_index = dnx_data_lif_out_lif_define_glem_result;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 19;

    
    define->data = 19;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_lif_out_lif_outlif_eedb_banks_pointer_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_lif;
    int submodule_index = dnx_data_lif_submodule_out_lif;
    int define_index = dnx_data_lif_out_lif_define_outlif_eedb_banks_pointer_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 18;

    
    define->data = 18;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_lif_out_lif_outlif_pointer_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_lif;
    int submodule_index = dnx_data_lif_submodule_out_lif;
    int define_index = dnx_data_lif_out_lif_define_outlif_pointer_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 18;

    
    define->data = 18;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_OUTLIF_NOF_BITS;
    define->property.doc = NULL;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 2;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "18";
    define->property.mapping[0].val = 18;
    define->property.mapping[0].is_default = 1 ;
    define->property.mapping[1].name = "19";
    define->property.mapping[1].val = 19;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_lif_out_lif_allocation_bank_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_lif;
    int submodule_index = dnx_data_lif_submodule_out_lif;
    int define_index = dnx_data_lif_out_lif_define_allocation_bank_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4096;

    
    define->data = 4096;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_OUTLIF_NOF_BITS;
    define->property.doc = 
        "8192 + (DATA(lif,out_lif,outlif_pointer_size)-19)*8192\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 2;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "18";
    define->property.mapping[0].val = 4096;
    define->property.mapping[0].is_default = 1 ;
    define->property.mapping[1].name = "19";
    define->property.mapping[1].val = 8192;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_lif_out_lif_local_outlif_width_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_lif;
    int submodule_index = dnx_data_lif_submodule_out_lif;
    int define_index = dnx_data_lif_out_lif_define_local_outlif_width;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 18;

    
    define->data = 18;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_OUTLIF_NOF_BITS;
    define->property.doc = 
        "DATA(lif,out_lif,outlif_pointer_size)\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 2;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "18";
    define->property.mapping[0].val = 18;
    define->property.mapping[0].is_default = 1 ;
    define->property.mapping[1].name = "19";
    define->property.mapping[1].val = 19;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_lif_out_lif_outlif_profile_width_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_lif;
    int submodule_index = dnx_data_lif_submodule_out_lif;
    int define_index = dnx_data_lif_out_lif_define_outlif_profile_width;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_OUTLIF_NOF_BITS;
    define->property.doc = 
        "DATA(lif,out_lif,glem_result)-DATA(lif,out_lif,local_outlif_width)\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 2;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "18";
    define->property.mapping[0].val = 1;
    define->property.mapping[0].is_default = 1 ;
    define->property.mapping[1].name = "19";
    define->property.mapping[1].val = 0;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_lif_out_lif_nof_local_out_lifs_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_lif;
    int submodule_index = dnx_data_lif_submodule_out_lif;
    int define_index = dnx_data_lif_out_lif_define_nof_local_out_lifs;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x40000;

    
    define->data = 0x40000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_OUTLIF_NOF_BITS;
    define->property.doc = 
        "1 << DATA(lif,out_lif,outlif_pointer_size)\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 2;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "18";
    define->property.mapping[0].val = 0x40000;
    define->property.mapping[0].is_default = 1 ;
    define->property.mapping[1].name = "19";
    define->property.mapping[1].val = 0x80000;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_lif_out_lif_physical_bank_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_lif;
    int submodule_index = dnx_data_lif_submodule_out_lif;
    int define_index = dnx_data_lif_out_lif_define_physical_bank_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x800;

    
    define->data = 0x800;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_OUTLIF_NOF_BITS;
    define->property.doc = 
        "1 << (DATA(lif,out_lif,outlif_pointer_size) - DATA(lif,out_lif,physical_bank_pointer_size))\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 2;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "18";
    define->property.mapping[0].val = 0x800;
    define->property.mapping[0].is_default = 1 ;
    define->property.mapping[1].name = "19";
    define->property.mapping[1].val = 0x1000;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
q2a_a0_dnx_data_lif_feature_iop_mode_orientation_selection_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_lif;
    int submodule_index = dnx_data_lif_submodule_feature;
    int feature_index = dnx_data_lif_feature_iop_mode_orientation_selection;
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
q2a_a0_dnx_data_lif_feature_in_lif_tunnel_wide_data_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_lif;
    int submodule_index = dnx_data_lif_submodule_feature;
    int feature_index = dnx_data_lif_feature_in_lif_tunnel_wide_data;
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
q2a_a0_data_lif_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_lif;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_lif_submodule_global_lif;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_lif_global_lif_define_glem_rif_optimization_enabled;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_lif_global_lif_glem_rif_optimization_enabled_set;
    data_index = dnx_data_lif_global_lif_define_null_lif;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_lif_global_lif_null_lif_set;
    data_index = dnx_data_lif_global_lif_define_egress_in_lif_null_value;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_lif_global_lif_egress_in_lif_null_value_set;

    
    data_index = dnx_data_lif_global_lif_prevent_tunnel_update_rif;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_lif_global_lif_prevent_tunnel_update_rif_set;

    
    
    submodule_index = dnx_data_lif_submodule_in_lif;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_lif_in_lif_mapped_in_lif_profile;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_lif_in_lif_mapped_in_lif_profile_set;
    data_index = dnx_data_lif_in_lif_in_lif_profile_iop_mode;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_lif_in_lif_in_lif_profile_iop_mode_set;

    
    
    submodule_index = dnx_data_lif_submodule_out_lif;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_lif_out_lif_define_glem_result;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_lif_out_lif_glem_result_set;
    data_index = dnx_data_lif_out_lif_define_outlif_eedb_banks_pointer_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_lif_out_lif_outlif_eedb_banks_pointer_size_set;
    data_index = dnx_data_lif_out_lif_define_outlif_pointer_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_lif_out_lif_outlif_pointer_size_set;
    data_index = dnx_data_lif_out_lif_define_allocation_bank_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_lif_out_lif_allocation_bank_size_set;
    data_index = dnx_data_lif_out_lif_define_local_outlif_width;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_lif_out_lif_local_outlif_width_set;
    data_index = dnx_data_lif_out_lif_define_outlif_profile_width;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_lif_out_lif_outlif_profile_width_set;
    data_index = dnx_data_lif_out_lif_define_nof_local_out_lifs;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_lif_out_lif_nof_local_out_lifs_set;
    data_index = dnx_data_lif_out_lif_define_physical_bank_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_lif_out_lif_physical_bank_size_set;

    

    
    
    submodule_index = dnx_data_lif_submodule_feature;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_lif_feature_iop_mode_orientation_selection;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_lif_feature_iop_mode_orientation_selection_set;
    data_index = dnx_data_lif_feature_in_lif_tunnel_wide_data;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_lif_feature_in_lif_tunnel_wide_data_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

