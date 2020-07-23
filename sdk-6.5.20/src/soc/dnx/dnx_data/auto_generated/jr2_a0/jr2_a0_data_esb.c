

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_EGRESSDNX

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_esb.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>







static shr_error_e
jr2_a0_dnx_data_esb_general_esb_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_esb;
    int submodule_index = dnx_data_esb_submodule_general;
    int feature_index = dnx_data_esb_general_esb_support;
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
jr2_a0_dnx_data_esb_general_nof_nif_ilkn_queues_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_esb;
    int submodule_index = dnx_data_esb_submodule_general;
    int define_index = dnx_data_esb_general_define_nof_nif_ilkn_queues;
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
jr2_a0_dnx_data_esb_general_nof_fpc_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_esb;
    int submodule_index = dnx_data_esb_submodule_general;
    int define_index = dnx_data_esb_general_define_nof_fpc;
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
jr2_a0_dnx_data_esb_general_total_nof_buffers_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_esb;
    int submodule_index = dnx_data_esb_submodule_general;
    int define_index = dnx_data_esb_general_define_total_nof_buffers;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_esb.general.nof_fpc_get(unit)*dnx_data_esb.general.nof_buffers_per_fpc_get(unit);

    
    define->data = dnx_data_esb.general.nof_fpc_get(unit)*dnx_data_esb.general.nof_buffers_per_fpc_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_esb_general_fpc_ptr_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_esb;
    int submodule_index = dnx_data_esb_submodule_general;
    int define_index = dnx_data_esb_general_define_fpc_ptr_size;
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
jr2_a0_dnx_data_esb_general_nof_buffers_per_fpc_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_esb;
    int submodule_index = dnx_data_esb_submodule_general;
    int define_index = dnx_data_esb_general_define_nof_buffers_per_fpc;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1 << dnx_data_esb.general.fpc_ptr_size_get(unit);

    
    define->data = 1 << dnx_data_esb.general.fpc_ptr_size_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}




shr_error_e
jr2_a0_data_esb_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_esb;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_esb_submodule_general;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_esb_general_define_nof_nif_ilkn_queues;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_esb_general_nof_nif_ilkn_queues_set;
    data_index = dnx_data_esb_general_define_nof_fpc;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_esb_general_nof_fpc_set;
    data_index = dnx_data_esb_general_define_total_nof_buffers;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_esb_general_total_nof_buffers_set;
    data_index = dnx_data_esb_general_define_fpc_ptr_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_esb_general_fpc_ptr_size_set;
    data_index = dnx_data_esb_general_define_nof_buffers_per_fpc;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_esb_general_nof_buffers_per_fpc_set;

    
    data_index = dnx_data_esb_general_esb_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_esb_general_esb_support_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

