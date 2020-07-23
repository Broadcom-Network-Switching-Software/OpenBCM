

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_VLAN

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_vlan.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>








static shr_error_e
jr2_a0_dnx_data_vlan_vlan_translate_nof_vlan_tag_formats_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_vlan;
    int submodule_index = dnx_data_vlan_submodule_vlan_translate;
    int define_index = dnx_data_vlan_vlan_translate_define_nof_vlan_tag_formats;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32;

    
    define->data = 32;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_vlan_vlan_translate_nof_ingress_vlan_edit_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_vlan;
    int submodule_index = dnx_data_vlan_submodule_vlan_translate;
    int define_index = dnx_data_vlan_vlan_translate_define_nof_ingress_vlan_edit_profiles;
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
jr2_a0_dnx_data_vlan_vlan_translate_nof_egress_vlan_edit_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_vlan;
    int submodule_index = dnx_data_vlan_submodule_vlan_translate;
    int define_index = dnx_data_vlan_vlan_translate_define_nof_egress_vlan_edit_profiles;
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
jr2_a0_dnx_data_vlan_vlan_translate_nof_vlan_range_entries_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_vlan;
    int submodule_index = dnx_data_vlan_submodule_vlan_translate;
    int define_index = dnx_data_vlan_vlan_translate_define_nof_vlan_range_entries;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32;

    
    define->data = 32;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_vlan_vlan_translate_nof_vlan_ranges_per_entry_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_vlan;
    int submodule_index = dnx_data_vlan_submodule_vlan_translate;
    int define_index = dnx_data_vlan_vlan_translate_define_nof_vlan_ranges_per_entry;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32;

    
    define->data = 32;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_vlan_vlan_translate_egress_tpid_outer_packet_index_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_vlan;
    int submodule_index = dnx_data_vlan_submodule_vlan_translate;
    int define_index = dnx_data_vlan_vlan_translate_define_egress_tpid_outer_packet_index;
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
jr2_a0_dnx_data_vlan_vlan_translate_egress_tpid_inner_packet_index_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_vlan;
    int submodule_index = dnx_data_vlan_submodule_vlan_translate;
    int define_index = dnx_data_vlan_vlan_translate_define_egress_tpid_inner_packet_index;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 15;

    
    define->data = 15;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_vlan_vlan_translate_nof_ingress_vlan_edit_action_ids_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_vlan;
    int submodule_index = dnx_data_vlan_submodule_vlan_translate;
    int define_index = dnx_data_vlan_vlan_translate_define_nof_ingress_vlan_edit_action_ids;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 128;

    
    define->data = 128;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_SYSTEM_HEADERS_MODE;
    define->property.doc = 
        "Different ingress VLAN edit action_ids number for regular and inter-op mode\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 2;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "0";
    define->property.mapping[0].val = 64;
    define->property.mapping[1].name = "1";
    define->property.mapping[1].val = 128;
    define->property.mapping[1].is_default = 1 ;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_vlan_vlan_translate_nof_egress_vlan_edit_action_ids_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_vlan;
    int submodule_index = dnx_data_vlan_submodule_vlan_translate;
    int define_index = dnx_data_vlan_vlan_translate_define_nof_egress_vlan_edit_action_ids;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 128;

    
    define->data = 128;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_vlan_llvp_ext_vcp_en_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_vlan;
    int submodule_index = dnx_data_vlan_submodule_llvp;
    int define_index = dnx_data_vlan_llvp_define_ext_vcp_en;
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
jr2_a0_data_vlan_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_vlan;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_vlan_submodule_vlan_translate;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_vlan_vlan_translate_define_nof_vlan_tag_formats;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_vlan_vlan_translate_nof_vlan_tag_formats_set;
    data_index = dnx_data_vlan_vlan_translate_define_nof_ingress_vlan_edit_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_vlan_vlan_translate_nof_ingress_vlan_edit_profiles_set;
    data_index = dnx_data_vlan_vlan_translate_define_nof_egress_vlan_edit_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_vlan_vlan_translate_nof_egress_vlan_edit_profiles_set;
    data_index = dnx_data_vlan_vlan_translate_define_nof_vlan_range_entries;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_vlan_vlan_translate_nof_vlan_range_entries_set;
    data_index = dnx_data_vlan_vlan_translate_define_nof_vlan_ranges_per_entry;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_vlan_vlan_translate_nof_vlan_ranges_per_entry_set;
    data_index = dnx_data_vlan_vlan_translate_define_egress_tpid_outer_packet_index;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_vlan_vlan_translate_egress_tpid_outer_packet_index_set;
    data_index = dnx_data_vlan_vlan_translate_define_egress_tpid_inner_packet_index;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_vlan_vlan_translate_egress_tpid_inner_packet_index_set;
    data_index = dnx_data_vlan_vlan_translate_define_nof_ingress_vlan_edit_action_ids;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_vlan_vlan_translate_nof_ingress_vlan_edit_action_ids_set;
    data_index = dnx_data_vlan_vlan_translate_define_nof_egress_vlan_edit_action_ids;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_vlan_vlan_translate_nof_egress_vlan_edit_action_ids_set;

    

    
    
    submodule_index = dnx_data_vlan_submodule_llvp;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_vlan_llvp_define_ext_vcp_en;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_vlan_llvp_ext_vcp_en_set;

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

