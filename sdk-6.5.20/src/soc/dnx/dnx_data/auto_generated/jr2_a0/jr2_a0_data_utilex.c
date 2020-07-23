

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SHAREDSWDNX_UTILSDNX

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_utilex.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>








static shr_error_e
jr2_a0_dnx_data_utilex_common_max_nof_dss_per_core_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_utilex;
    int submodule_index = dnx_data_utilex_submodule_common;
    int define_index = dnx_data_utilex_common_define_max_nof_dss_per_core;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 500;

    
    define->data = 500;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_utilex_common_max_nof_hashs_per_core_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_utilex;
    int submodule_index = dnx_data_utilex_submodule_common;
    int define_index = dnx_data_utilex_common_define_max_nof_hashs_per_core;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 200;

    
    define->data = 200;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_utilex_common_max_nof_lists_per_core_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_utilex;
    int submodule_index = dnx_data_utilex_submodule_common;
    int define_index = dnx_data_utilex_common_define_max_nof_lists_per_core;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 200;

    
    define->data = 200;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_utilex_common_max_nof_multis_per_core_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_utilex;
    int submodule_index = dnx_data_utilex_submodule_common;
    int define_index = dnx_data_utilex_common_define_max_nof_multis_per_core;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 200;

    
    define->data = 200;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_utilex_common_max_nof_res_tag_bitmaps_per_core_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_utilex;
    int submodule_index = dnx_data_utilex_submodule_common;
    int define_index = dnx_data_utilex_common_define_max_nof_res_tag_bitmaps_per_core;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 800;

    
    define->data = 800;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_utilex_common_max_nof_defragmented_chunks_per_core_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_utilex;
    int submodule_index = dnx_data_utilex_submodule_common;
    int define_index = dnx_data_utilex_common_define_max_nof_defragmented_chunks_per_core;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}




shr_error_e
jr2_a0_data_utilex_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_utilex;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_utilex_submodule_common;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_utilex_common_define_max_nof_dss_per_core;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_utilex_common_max_nof_dss_per_core_set;
    data_index = dnx_data_utilex_common_define_max_nof_hashs_per_core;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_utilex_common_max_nof_hashs_per_core_set;
    data_index = dnx_data_utilex_common_define_max_nof_lists_per_core;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_utilex_common_max_nof_lists_per_core_set;
    data_index = dnx_data_utilex_common_define_max_nof_multis_per_core;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_utilex_common_max_nof_multis_per_core_set;
    data_index = dnx_data_utilex_common_define_max_nof_res_tag_bitmaps_per_core;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_utilex_common_max_nof_res_tag_bitmaps_per_core_set;
    data_index = dnx_data_utilex_common_define_max_nof_defragmented_chunks_per_core;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_utilex_common_max_nof_defragmented_chunks_per_core_set;

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

