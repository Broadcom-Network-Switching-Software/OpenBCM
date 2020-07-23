

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_BFD

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_bfd.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>







static shr_error_e
jr2_a0_dnx_data_bfd_general_configurable_single_hop_tos_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_bfd;
    int submodule_index = dnx_data_bfd_submodule_general;
    int feature_index = dnx_data_bfd_general_configurable_single_hop_tos;
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
jr2_a0_dnx_data_bfd_general_extended_sip_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_bfd;
    int submodule_index = dnx_data_bfd_submodule_general;
    int feature_index = dnx_data_bfd_general_extended_sip_support;
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
jr2_a0_dnx_data_bfd_general_vxlan_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_bfd;
    int submodule_index = dnx_data_bfd_submodule_general;
    int feature_index = dnx_data_bfd_general_vxlan_support;
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
jr2_a0_dnx_data_bfd_general_nof_bits_bfd_endpoints_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_bfd;
    int submodule_index = dnx_data_bfd_submodule_general;
    int define_index = dnx_data_bfd_general_define_nof_bits_bfd_endpoints;
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
jr2_a0_dnx_data_bfd_general_nof_endpoints_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_bfd;
    int submodule_index = dnx_data_bfd_submodule_general;
    int define_index = dnx_data_bfd_general_define_nof_endpoints;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x10000;

    
    define->data = 0x10000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_bfd_general_nof_dips_for_bfd_multihop_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_bfd;
    int submodule_index = dnx_data_bfd_submodule_general;
    int define_index = dnx_data_bfd_general_define_nof_dips_for_bfd_multihop;
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
jr2_a0_dnx_data_bfd_general_nof_bits_dips_for_bfd_multihop_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_bfd;
    int submodule_index = dnx_data_bfd_submodule_general;
    int define_index = dnx_data_bfd_general_define_nof_bits_dips_for_bfd_multihop;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_bfd_general_nof_bfd_server_trap_codes_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_bfd;
    int submodule_index = dnx_data_bfd_submodule_general;
    int define_index = dnx_data_bfd_general_define_nof_bfd_server_trap_codes;
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
jr2_a0_dnx_data_bfd_general_nof_profiles_for_tx_rate_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_bfd;
    int submodule_index = dnx_data_bfd_submodule_general;
    int define_index = dnx_data_bfd_general_define_nof_profiles_for_tx_rate;
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
jr2_a0_dnx_data_bfd_general_nof_bits_bfd_server_trap_codes_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_bfd;
    int submodule_index = dnx_data_bfd_submodule_general;
    int define_index = dnx_data_bfd_general_define_nof_bits_bfd_server_trap_codes;
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
jr2_a0_dnx_data_bfd_general_rmep_index_db_ipv4_const_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_bfd;
    int submodule_index = dnx_data_bfd_submodule_general;
    int define_index = dnx_data_bfd_general_define_rmep_index_db_ipv4_const;
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
jr2_a0_dnx_data_bfd_general_rmep_index_db_mpls_const_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_bfd;
    int submodule_index = dnx_data_bfd_submodule_general;
    int define_index = dnx_data_bfd_general_define_rmep_index_db_mpls_const;
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
jr2_a0_dnx_data_bfd_general_rmep_index_db_pwe_const_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_bfd;
    int submodule_index = dnx_data_bfd_submodule_general;
    int define_index = dnx_data_bfd_general_define_rmep_index_db_pwe_const;
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
jr2_a0_dnx_data_bfd_general_nof_req_int_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_bfd;
    int submodule_index = dnx_data_bfd_submodule_general;
    int define_index = dnx_data_bfd_general_define_nof_req_int_profiles;
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
jr2_a0_dnx_data_bfd_general_nof_bits_your_discr_to_lif_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_bfd;
    int submodule_index = dnx_data_bfd_submodule_general;
    int define_index = dnx_data_bfd_general_define_nof_bits_your_discr_to_lif;
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
jr2_a0_dnx_data_bfd_general_oamp_bfd_cw_valid_dis_supp_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_bfd;
    int submodule_index = dnx_data_bfd_submodule_general;
    int define_index = dnx_data_bfd_general_define_oamp_bfd_cw_valid_dis_supp;
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
jr2_a0_dnx_data_bfd_feature_detect_multiplier_limitation_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_bfd;
    int submodule_index = dnx_data_bfd_submodule_feature;
    int feature_index = dnx_data_bfd_feature_detect_multiplier_limitation;
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
jr2_a0_dnx_data_bfd_feature_ptch_oam_offset_limitation_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_bfd;
    int submodule_index = dnx_data_bfd_submodule_feature;
    int feature_index = dnx_data_bfd_feature_ptch_oam_offset_limitation;
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
jr2_a0_data_bfd_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_bfd;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_bfd_submodule_general;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_bfd_general_define_nof_bits_bfd_endpoints;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_bfd_general_nof_bits_bfd_endpoints_set;
    data_index = dnx_data_bfd_general_define_nof_endpoints;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_bfd_general_nof_endpoints_set;
    data_index = dnx_data_bfd_general_define_nof_dips_for_bfd_multihop;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_bfd_general_nof_dips_for_bfd_multihop_set;
    data_index = dnx_data_bfd_general_define_nof_bits_dips_for_bfd_multihop;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_bfd_general_nof_bits_dips_for_bfd_multihop_set;
    data_index = dnx_data_bfd_general_define_nof_bfd_server_trap_codes;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_bfd_general_nof_bfd_server_trap_codes_set;
    data_index = dnx_data_bfd_general_define_nof_profiles_for_tx_rate;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_bfd_general_nof_profiles_for_tx_rate_set;
    data_index = dnx_data_bfd_general_define_nof_bits_bfd_server_trap_codes;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_bfd_general_nof_bits_bfd_server_trap_codes_set;
    data_index = dnx_data_bfd_general_define_rmep_index_db_ipv4_const;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_bfd_general_rmep_index_db_ipv4_const_set;
    data_index = dnx_data_bfd_general_define_rmep_index_db_mpls_const;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_bfd_general_rmep_index_db_mpls_const_set;
    data_index = dnx_data_bfd_general_define_rmep_index_db_pwe_const;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_bfd_general_rmep_index_db_pwe_const_set;
    data_index = dnx_data_bfd_general_define_nof_req_int_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_bfd_general_nof_req_int_profiles_set;
    data_index = dnx_data_bfd_general_define_nof_bits_your_discr_to_lif;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_bfd_general_nof_bits_your_discr_to_lif_set;
    data_index = dnx_data_bfd_general_define_oamp_bfd_cw_valid_dis_supp;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_bfd_general_oamp_bfd_cw_valid_dis_supp_set;

    
    data_index = dnx_data_bfd_general_configurable_single_hop_tos;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_bfd_general_configurable_single_hop_tos_set;
    data_index = dnx_data_bfd_general_extended_sip_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_bfd_general_extended_sip_support_set;
    data_index = dnx_data_bfd_general_vxlan_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_bfd_general_vxlan_support_set;

    
    
    submodule_index = dnx_data_bfd_submodule_feature;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_bfd_feature_detect_multiplier_limitation;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_bfd_feature_detect_multiplier_limitation_set;
    data_index = dnx_data_bfd_feature_ptch_oam_offset_limitation;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_bfd_feature_ptch_oam_offset_limitation_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

