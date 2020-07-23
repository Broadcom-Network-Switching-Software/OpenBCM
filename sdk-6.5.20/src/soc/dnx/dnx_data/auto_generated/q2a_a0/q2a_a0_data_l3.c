

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_L3

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_l3.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>








static shr_error_e
q2a_a0_dnx_data_l3_fec_nof_fecs_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fec;
    int define_index = dnx_data_l3_fec_define_nof_fecs;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x60000;

    
    define->data = 0x60000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_l3_fec_max_fec_id_for_single_dhb_cluster_pair_granularity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fec;
    int define_index = dnx_data_l3_fec_define_max_fec_id_for_single_dhb_cluster_pair_granularity;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = UTILEX_MIN(dnx_data_l3.fec.nof_fecs_get(unit),0x40000)-1;

    
    define->data = UTILEX_MIN(dnx_data_l3.fec.nof_fecs_get(unit),0x40000)-1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_l3_fec_bank_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fec;
    int define_index = dnx_data_l3_fec_define_bank_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 13106;

    
    define->data = 13106;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_l3_fec_nof_physical_fecs_per_bank_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fec;
    int define_index = dnx_data_l3_fec_define_nof_physical_fecs_per_bank;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4096;

    
    define->data = 4096;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_l3_fec_fer_mdb_granularity_ratio_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fec;
    int define_index = dnx_data_l3_fec_define_fer_mdb_granularity_ratio;
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
q2a_a0_dnx_data_l3_source_address_custom_sa_use_dual_homing_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_source_address;
    int define_index = dnx_data_l3_source_address_define_custom_sa_use_dual_homing;
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
q2a_a0_dnx_data_l3_fwd_flp_fragment_support_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fwd;
    int define_index = dnx_data_l3_fwd_define_flp_fragment_support;
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
q2a_a0_dnx_data_l3_feature_fec_hit_bit_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_feature;
    int feature_index = dnx_data_l3_feature_fec_hit_bit;
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
q2a_a0_dnx_data_l3_feature_fer_fec_granularity_double_size_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_feature;
    int feature_index = dnx_data_l3_feature_fer_fec_granularity_double_size;
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
q2a_a0_dnx_data_l3_feature_nat_on_a_stick_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_feature;
    int feature_index = dnx_data_l3_feature_nat_on_a_stick;
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
q2a_a0_data_l3_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_l3;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_l3_submodule_fec;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_l3_fec_define_nof_fecs;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_l3_fec_nof_fecs_set;
    data_index = dnx_data_l3_fec_define_max_fec_id_for_single_dhb_cluster_pair_granularity;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_l3_fec_max_fec_id_for_single_dhb_cluster_pair_granularity_set;
    data_index = dnx_data_l3_fec_define_bank_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_l3_fec_bank_size_set;
    data_index = dnx_data_l3_fec_define_nof_physical_fecs_per_bank;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_l3_fec_nof_physical_fecs_per_bank_set;
    data_index = dnx_data_l3_fec_define_fer_mdb_granularity_ratio;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_l3_fec_fer_mdb_granularity_ratio_set;

    

    
    
    submodule_index = dnx_data_l3_submodule_source_address;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_l3_source_address_define_custom_sa_use_dual_homing;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_l3_source_address_custom_sa_use_dual_homing_set;

    

    
    
    submodule_index = dnx_data_l3_submodule_fwd;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_l3_fwd_define_flp_fragment_support;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_l3_fwd_flp_fragment_support_set;

    

    
    
    submodule_index = dnx_data_l3_submodule_feature;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_l3_feature_fec_hit_bit;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_l3_feature_fec_hit_bit_set;
    data_index = dnx_data_l3_feature_fer_fec_granularity_double_size;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_l3_feature_fer_fec_granularity_double_size_set;
    data_index = dnx_data_l3_feature_nat_on_a_stick;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_l3_feature_nat_on_a_stick_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

