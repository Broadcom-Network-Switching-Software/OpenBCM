

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_QOS

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_qos.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>







static shr_error_e
q2a_a0_dnx_data_qos_qos_ecn_L4S_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_qos;
    int submodule_index = dnx_data_qos_submodule_qos;
    int feature_index = dnx_data_qos_qos_ecn_L4S;
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
q2a_a0_dnx_data_qos_qos_ecn_bits_width_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_qos;
    int submodule_index = dnx_data_qos_submodule_qos;
    int define_index = dnx_data_qos_qos_define_ecn_bits_width;
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
q2a_a0_dnx_data_qos_feature_egress_remark_or_keep_enabler_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_qos;
    int submodule_index = dnx_data_qos_submodule_feature;
    int feature_index = dnx_data_qos_feature_egress_remark_or_keep_enabler;
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
q2a_a0_dnx_data_qos_feature_fwd_eve_pcp_diff_encap_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_qos;
    int submodule_index = dnx_data_qos_submodule_feature;
    int feature_index = dnx_data_qos_feature_fwd_eve_pcp_diff_encap;
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
q2a_a0_dnx_data_qos_feature_phb_rm_ffc_extend_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_qos;
    int submodule_index = dnx_data_qos_submodule_feature;
    int feature_index = dnx_data_qos_feature_phb_rm_ffc_extend;
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
q2a_a0_dnx_data_qos_feature_phb_rm_ffc_bits_width_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_qos;
    int submodule_index = dnx_data_qos_submodule_feature;
    int define_index = dnx_data_qos_feature_define_phb_rm_ffc_bits_width;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 23;

    
    define->data = 23;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}




shr_error_e
q2a_a0_data_qos_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_qos;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_qos_submodule_qos;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_qos_qos_define_ecn_bits_width;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_qos_qos_ecn_bits_width_set;

    
    data_index = dnx_data_qos_qos_ecn_L4S;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_qos_qos_ecn_L4S_set;

    
    
    submodule_index = dnx_data_qos_submodule_feature;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_qos_feature_define_phb_rm_ffc_bits_width;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_qos_feature_phb_rm_ffc_bits_width_set;

    
    data_index = dnx_data_qos_feature_egress_remark_or_keep_enabler;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_qos_feature_egress_remark_or_keep_enabler_set;
    data_index = dnx_data_qos_feature_fwd_eve_pcp_diff_encap;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_qos_feature_fwd_eve_pcp_diff_encap_set;
    data_index = dnx_data_qos_feature_phb_rm_ffc_extend;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_qos_feature_phb_rm_ffc_extend_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

