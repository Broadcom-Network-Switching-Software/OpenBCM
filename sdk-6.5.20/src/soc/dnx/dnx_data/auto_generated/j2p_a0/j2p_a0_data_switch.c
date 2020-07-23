

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_SWITCH

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_switch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>








static shr_error_e
j2p_a0_dnx_data_switch_load_balancing_reserved_next_label_valid_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_switch;
    int submodule_index = dnx_data_switch_submodule_load_balancing;
    int define_index = dnx_data_switch_load_balancing_define_reserved_next_label_valid;
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
j2p_a0_dnx_data_switch_load_balancing_hashing_selection_per_layer_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_switch;
    int submodule_index = dnx_data_switch_submodule_load_balancing;
    int define_index = dnx_data_switch_load_balancing_define_hashing_selection_per_layer;
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
j2p_a0_dnx_data_switch_load_balancing_mpls_split_stack_config_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_switch;
    int submodule_index = dnx_data_switch_submodule_load_balancing;
    int define_index = dnx_data_switch_load_balancing_define_mpls_split_stack_config;
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
j2p_a0_dnx_data_switch_load_balancing_mpls_cam_next_label_valid_field_exists_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_switch;
    int submodule_index = dnx_data_switch_submodule_load_balancing;
    int define_index = dnx_data_switch_load_balancing_define_mpls_cam_next_label_valid_field_exists;
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
j2p_a0_dnx_data_switch_svtag_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_switch;
    int submodule_index = dnx_data_switch_submodule_svtag;
    int define_index = dnx_data_switch_svtag_define_supported;
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
j2p_a0_dnx_data_switch_svtag_svtag_label_size_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_switch;
    int submodule_index = dnx_data_switch_submodule_svtag;
    int define_index = dnx_data_switch_svtag_define_svtag_label_size_bits;
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
j2p_a0_dnx_data_switch_svtag_egress_svtag_offset_addr_size_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_switch;
    int submodule_index = dnx_data_switch_submodule_svtag;
    int define_index = dnx_data_switch_svtag_define_egress_svtag_offset_addr_size_bits;
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
j2p_a0_dnx_data_switch_svtag_egress_svtag_offset_addr_position_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_switch;
    int submodule_index = dnx_data_switch_submodule_svtag;
    int define_index = dnx_data_switch_svtag_define_egress_svtag_offset_addr_position_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 16;

    
    define->data = 16;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_switch_svtag_egress_svtag_signature_size_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_switch;
    int submodule_index = dnx_data_switch_submodule_svtag;
    int define_index = dnx_data_switch_svtag_define_egress_svtag_signature_size_bits;
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
j2p_a0_dnx_data_switch_svtag_egress_svtag_signature_position_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_switch;
    int submodule_index = dnx_data_switch_submodule_svtag;
    int define_index = dnx_data_switch_svtag_define_egress_svtag_signature_position_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 24;

    
    define->data = 24;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_switch_svtag_egress_svtag_sci_size_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_switch;
    int submodule_index = dnx_data_switch_submodule_svtag;
    int define_index = dnx_data_switch_svtag_define_egress_svtag_sci_size_bits;
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
j2p_a0_dnx_data_switch_svtag_egress_svtag_sci_position_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_switch;
    int submodule_index = dnx_data_switch_submodule_svtag;
    int define_index = dnx_data_switch_svtag_define_egress_svtag_sci_position_bits;
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
j2p_a0_dnx_data_switch_svtag_egress_svtag_pkt_type_size_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_switch;
    int submodule_index = dnx_data_switch_submodule_svtag;
    int define_index = dnx_data_switch_svtag_define_egress_svtag_pkt_type_size_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_switch_svtag_egress_svtag_pkt_type_position_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_switch;
    int submodule_index = dnx_data_switch_submodule_svtag;
    int define_index = dnx_data_switch_svtag_define_egress_svtag_pkt_type_position_bits;
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
j2p_a0_dnx_data_switch_svtag_egress_svtag_ipv6_indication_position_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_switch;
    int submodule_index = dnx_data_switch_submodule_svtag;
    int define_index = dnx_data_switch_svtag_define_egress_svtag_ipv6_indication_position_bits;
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
j2p_a0_dnx_data_switch_svtag_egress_svtag_hw_field_position_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_switch;
    int submodule_index = dnx_data_switch_submodule_svtag;
    int define_index = dnx_data_switch_svtag_define_egress_svtag_hw_field_position_bits;
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
j2p_a0_dnx_data_switch_svtag_egress_svtag_accumulation_indication_hw_field_position_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_switch;
    int submodule_index = dnx_data_switch_submodule_svtag;
    int define_index = dnx_data_switch_svtag_define_egress_svtag_accumulation_indication_hw_field_position_bits;
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
j2p_a0_dnx_data_switch_svtag_ingress_svtag_position_bytes_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_switch;
    int submodule_index = dnx_data_switch_submodule_svtag;
    int define_index = dnx_data_switch_svtag_define_ingress_svtag_position_bytes;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 12;

    
    define->data = 12;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
j2p_a0_dnx_data_switch_feature_mpls_labels_included_in_hash_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_switch;
    int submodule_index = dnx_data_switch_submodule_feature;
    int feature_index = dnx_data_switch_feature_mpls_labels_included_in_hash;
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
j2p_a0_dnx_data_switch_feature_excluded_header_bits_from_hash_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_switch;
    int submodule_index = dnx_data_switch_submodule_feature;
    int feature_index = dnx_data_switch_feature_excluded_header_bits_from_hash;
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
j2p_a0_data_switch_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_switch;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_switch_submodule_load_balancing;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_switch_load_balancing_define_reserved_next_label_valid;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_switch_load_balancing_reserved_next_label_valid_set;
    data_index = dnx_data_switch_load_balancing_define_hashing_selection_per_layer;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_switch_load_balancing_hashing_selection_per_layer_set;
    data_index = dnx_data_switch_load_balancing_define_mpls_split_stack_config;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_switch_load_balancing_mpls_split_stack_config_set;
    data_index = dnx_data_switch_load_balancing_define_mpls_cam_next_label_valid_field_exists;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_switch_load_balancing_mpls_cam_next_label_valid_field_exists_set;

    

    
    
    submodule_index = dnx_data_switch_submodule_svtag;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_switch_svtag_define_supported;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_switch_svtag_supported_set;
    data_index = dnx_data_switch_svtag_define_svtag_label_size_bits;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_switch_svtag_svtag_label_size_bits_set;
    data_index = dnx_data_switch_svtag_define_egress_svtag_offset_addr_size_bits;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_switch_svtag_egress_svtag_offset_addr_size_bits_set;
    data_index = dnx_data_switch_svtag_define_egress_svtag_offset_addr_position_bits;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_switch_svtag_egress_svtag_offset_addr_position_bits_set;
    data_index = dnx_data_switch_svtag_define_egress_svtag_signature_size_bits;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_switch_svtag_egress_svtag_signature_size_bits_set;
    data_index = dnx_data_switch_svtag_define_egress_svtag_signature_position_bits;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_switch_svtag_egress_svtag_signature_position_bits_set;
    data_index = dnx_data_switch_svtag_define_egress_svtag_sci_size_bits;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_switch_svtag_egress_svtag_sci_size_bits_set;
    data_index = dnx_data_switch_svtag_define_egress_svtag_sci_position_bits;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_switch_svtag_egress_svtag_sci_position_bits_set;
    data_index = dnx_data_switch_svtag_define_egress_svtag_pkt_type_size_bits;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_switch_svtag_egress_svtag_pkt_type_size_bits_set;
    data_index = dnx_data_switch_svtag_define_egress_svtag_pkt_type_position_bits;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_switch_svtag_egress_svtag_pkt_type_position_bits_set;
    data_index = dnx_data_switch_svtag_define_egress_svtag_ipv6_indication_position_bits;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_switch_svtag_egress_svtag_ipv6_indication_position_bits_set;
    data_index = dnx_data_switch_svtag_define_egress_svtag_hw_field_position_bits;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_switch_svtag_egress_svtag_hw_field_position_bits_set;
    data_index = dnx_data_switch_svtag_define_egress_svtag_accumulation_indication_hw_field_position_bits;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_switch_svtag_egress_svtag_accumulation_indication_hw_field_position_bits_set;
    data_index = dnx_data_switch_svtag_define_ingress_svtag_position_bytes;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_switch_svtag_ingress_svtag_position_bytes_set;

    

    
    
    submodule_index = dnx_data_switch_submodule_feature;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_switch_feature_mpls_labels_included_in_hash;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_switch_feature_mpls_labels_included_in_hash_set;
    data_index = dnx_data_switch_feature_excluded_header_bits_from_hash;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_switch_feature_excluded_header_bits_from_hash_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

