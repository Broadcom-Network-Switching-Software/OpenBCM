

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COUNTER

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_gtimer.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>







static shr_error_e
q2a_a0_dnx_data_gtimer_rtp_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_rtp;
    int feature_index = dnx_data_gtimer_rtp_is_supported;
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
q2a_a0_dnx_data_gtimer_fdt_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_fdt;
    int feature_index = dnx_data_gtimer_fdt_is_supported;
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
q2a_a0_dnx_data_gtimer_fdtl_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_fdtl;
    int feature_index = dnx_data_gtimer_fdtl_is_supported;
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
q2a_a0_dnx_data_gtimer_pem_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_pem;
    int feature_index = dnx_data_gtimer_pem_is_supported;
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
q2a_a0_dnx_data_gtimer_pem_block_index_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_pem;
    int define_index = dnx_data_gtimer_pem_define_block_index_size;
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
q2a_a0_dnx_data_gtimer_pem_block_index_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_pem;
    int define_index = dnx_data_gtimer_pem_define_block_index_max_value;
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
q2a_a0_dnx_data_gtimer_dhc_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_dhc;
    int feature_index = dnx_data_gtimer_dhc_is_supported;
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
q2a_a0_dnx_data_gtimer_cdu_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_cdu;
    int feature_index = dnx_data_gtimer_cdu_is_supported;
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
q2a_a0_dnx_data_gtimer_cdu_block_index_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_cdu;
    int define_index = dnx_data_gtimer_cdu_define_block_index_size;
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
q2a_a0_dnx_data_gtimer_cdu_block_index_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_cdu;
    int define_index = dnx_data_gtimer_cdu_define_block_index_max_value;
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
q2a_a0_dnx_data_gtimer_esb_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_esb;
    int feature_index = dnx_data_gtimer_esb_is_supported;
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
q2a_a0_dnx_data_gtimer_esb_block_index_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_esb;
    int define_index = dnx_data_gtimer_esb_define_block_index_size;
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
q2a_a0_dnx_data_gtimer_esb_block_index_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_esb;
    int define_index = dnx_data_gtimer_esb_define_block_index_max_value;
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
q2a_a0_dnx_data_gtimer_cdum_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_cdum;
    int feature_index = dnx_data_gtimer_cdum_is_supported;
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
q2a_a0_dnx_data_gtimer_mesh_topology_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_mesh_topology;
    int feature_index = dnx_data_gtimer_mesh_topology_is_supported;
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
q2a_a0_dnx_data_gtimer_bdm_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_bdm;
    int feature_index = dnx_data_gtimer_bdm_is_supported;
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
q2a_a0_dnx_data_gtimer_bdm_block_index_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_bdm;
    int define_index = dnx_data_gtimer_bdm_define_block_index_size;
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
q2a_a0_dnx_data_gtimer_bdm_block_index_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_bdm;
    int define_index = dnx_data_gtimer_bdm_define_block_index_max_value;
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
q2a_a0_dnx_data_gtimer_ecgm_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_ecgm;
    int feature_index = dnx_data_gtimer_ecgm_is_supported;
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
q2a_a0_dnx_data_gtimer_ecgm_block_index_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_ecgm;
    int define_index = dnx_data_gtimer_ecgm_define_block_index_size;
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
q2a_a0_dnx_data_gtimer_ecgm_block_index_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_ecgm;
    int define_index = dnx_data_gtimer_ecgm_define_block_index_max_value;
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
q2a_a0_dnx_data_gtimer_cfc_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_cfc;
    int feature_index = dnx_data_gtimer_cfc_is_supported;
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
q2a_a0_dnx_data_gtimer_cfc_block_index_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_cfc;
    int define_index = dnx_data_gtimer_cfc_define_block_index_size;
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
q2a_a0_dnx_data_gtimer_cfc_block_index_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_cfc;
    int define_index = dnx_data_gtimer_cfc_define_block_index_max_value;
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
q2a_a0_dnx_data_gtimer_cgm_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_cgm;
    int feature_index = dnx_data_gtimer_cgm_is_supported;
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
q2a_a0_dnx_data_gtimer_cgm_block_index_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_cgm;
    int define_index = dnx_data_gtimer_cgm_define_block_index_size;
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
q2a_a0_dnx_data_gtimer_cgm_block_index_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_cgm;
    int define_index = dnx_data_gtimer_cgm_define_block_index_max_value;
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
q2a_a0_dnx_data_gtimer_crps_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_crps;
    int feature_index = dnx_data_gtimer_crps_is_supported;
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
q2a_a0_dnx_data_gtimer_crps_block_index_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_crps;
    int define_index = dnx_data_gtimer_crps_define_block_index_size;
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
q2a_a0_dnx_data_gtimer_crps_block_index_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_crps;
    int define_index = dnx_data_gtimer_crps_define_block_index_max_value;
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
q2a_a0_dnx_data_gtimer_ddha_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_ddha;
    int feature_index = dnx_data_gtimer_ddha_is_supported;
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
q2a_a0_dnx_data_gtimer_ddha_block_index_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_ddha;
    int define_index = dnx_data_gtimer_ddha_define_block_index_size;
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
q2a_a0_dnx_data_gtimer_ddha_block_index_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_ddha;
    int define_index = dnx_data_gtimer_ddha_define_block_index_max_value;
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
q2a_a0_dnx_data_gtimer_ddp_block_index_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_ddp;
    int define_index = dnx_data_gtimer_ddp_define_block_index_size;
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
q2a_a0_dnx_data_gtimer_ddp_block_index_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_ddp;
    int define_index = dnx_data_gtimer_ddp_define_block_index_max_value;
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
q2a_a0_dnx_data_gtimer_dqm_block_index_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_dqm;
    int define_index = dnx_data_gtimer_dqm_define_block_index_size;
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
q2a_a0_dnx_data_gtimer_dqm_block_index_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_dqm;
    int define_index = dnx_data_gtimer_dqm_define_block_index_max_value;
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
q2a_a0_dnx_data_gtimer_edb_block_index_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_edb;
    int define_index = dnx_data_gtimer_edb_define_block_index_size;
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
q2a_a0_dnx_data_gtimer_edb_block_index_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_edb;
    int define_index = dnx_data_gtimer_edb_define_block_index_max_value;
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
q2a_a0_dnx_data_gtimer_epni_block_index_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_epni;
    int define_index = dnx_data_gtimer_epni_define_block_index_size;
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
q2a_a0_dnx_data_gtimer_epni_block_index_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_epni;
    int define_index = dnx_data_gtimer_epni_define_block_index_max_value;
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
q2a_a0_dnx_data_gtimer_epre_block_index_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_epre;
    int define_index = dnx_data_gtimer_epre_define_block_index_size;
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
q2a_a0_dnx_data_gtimer_epre_block_index_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_epre;
    int define_index = dnx_data_gtimer_epre_define_block_index_max_value;
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
q2a_a0_dnx_data_gtimer_eps_block_index_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_eps;
    int define_index = dnx_data_gtimer_eps_define_block_index_size;
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
q2a_a0_dnx_data_gtimer_eps_block_index_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_eps;
    int define_index = dnx_data_gtimer_eps_define_block_index_max_value;
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
q2a_a0_dnx_data_gtimer_erpp_block_index_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_erpp;
    int define_index = dnx_data_gtimer_erpp_define_block_index_size;
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
q2a_a0_dnx_data_gtimer_erpp_block_index_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_erpp;
    int define_index = dnx_data_gtimer_erpp_define_block_index_max_value;
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
q2a_a0_dnx_data_gtimer_etppa_block_index_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_etppa;
    int define_index = dnx_data_gtimer_etppa_define_block_index_size;
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
q2a_a0_dnx_data_gtimer_etppa_block_index_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_etppa;
    int define_index = dnx_data_gtimer_etppa_define_block_index_max_value;
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
q2a_a0_dnx_data_gtimer_etppb_block_index_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_etppb;
    int define_index = dnx_data_gtimer_etppb_define_block_index_size;
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
q2a_a0_dnx_data_gtimer_etppb_block_index_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_etppb;
    int define_index = dnx_data_gtimer_etppb_define_block_index_max_value;
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
q2a_a0_dnx_data_gtimer_etppc_block_index_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_etppc;
    int define_index = dnx_data_gtimer_etppc_define_block_index_size;
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
q2a_a0_dnx_data_gtimer_etppc_block_index_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_etppc;
    int define_index = dnx_data_gtimer_etppc_define_block_index_max_value;
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
q2a_a0_dnx_data_gtimer_fqp_block_index_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_fqp;
    int define_index = dnx_data_gtimer_fqp_define_block_index_size;
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
q2a_a0_dnx_data_gtimer_fqp_block_index_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_fqp;
    int define_index = dnx_data_gtimer_fqp_define_block_index_max_value;
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
q2a_a0_dnx_data_gtimer_ile_block_index_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_ile;
    int define_index = dnx_data_gtimer_ile_define_block_index_size;
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
q2a_a0_dnx_data_gtimer_ile_block_index_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_ile;
    int define_index = dnx_data_gtimer_ile_define_block_index_max_value;
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
q2a_a0_dnx_data_gtimer_ippa_block_index_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_ippa;
    int define_index = dnx_data_gtimer_ippa_define_block_index_size;
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
q2a_a0_dnx_data_gtimer_ippa_block_index_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_ippa;
    int define_index = dnx_data_gtimer_ippa_define_block_index_max_value;
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
q2a_a0_dnx_data_gtimer_ippb_block_index_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_ippb;
    int define_index = dnx_data_gtimer_ippb_define_block_index_size;
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
q2a_a0_dnx_data_gtimer_ippb_block_index_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_ippb;
    int define_index = dnx_data_gtimer_ippb_define_block_index_max_value;
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
q2a_a0_dnx_data_gtimer_ippc_block_index_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_ippc;
    int define_index = dnx_data_gtimer_ippc_define_block_index_size;
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
q2a_a0_dnx_data_gtimer_ippc_block_index_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_ippc;
    int define_index = dnx_data_gtimer_ippc_define_block_index_max_value;
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
q2a_a0_dnx_data_gtimer_ippd_block_index_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_ippd;
    int define_index = dnx_data_gtimer_ippd_define_block_index_size;
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
q2a_a0_dnx_data_gtimer_ippd_block_index_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_ippd;
    int define_index = dnx_data_gtimer_ippd_define_block_index_max_value;
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
q2a_a0_dnx_data_gtimer_ippe_block_index_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_ippe;
    int define_index = dnx_data_gtimer_ippe_define_block_index_size;
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
q2a_a0_dnx_data_gtimer_ippe_block_index_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_ippe;
    int define_index = dnx_data_gtimer_ippe_define_block_index_max_value;
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
q2a_a0_dnx_data_gtimer_ippf_block_index_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_ippf;
    int define_index = dnx_data_gtimer_ippf_define_block_index_size;
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
q2a_a0_dnx_data_gtimer_ippf_block_index_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_ippf;
    int define_index = dnx_data_gtimer_ippf_define_block_index_max_value;
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
q2a_a0_dnx_data_gtimer_ips_block_index_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_ips;
    int define_index = dnx_data_gtimer_ips_define_block_index_size;
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
q2a_a0_dnx_data_gtimer_ips_block_index_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_ips;
    int define_index = dnx_data_gtimer_ips_define_block_index_max_value;
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
q2a_a0_dnx_data_gtimer_ipt_block_index_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_ipt;
    int define_index = dnx_data_gtimer_ipt_define_block_index_size;
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
q2a_a0_dnx_data_gtimer_ipt_block_index_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_ipt;
    int define_index = dnx_data_gtimer_ipt_define_block_index_max_value;
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
q2a_a0_dnx_data_gtimer_iqm_block_index_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_iqm;
    int define_index = dnx_data_gtimer_iqm_define_block_index_size;
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
q2a_a0_dnx_data_gtimer_iqm_block_index_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_iqm;
    int define_index = dnx_data_gtimer_iqm_define_block_index_max_value;
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
q2a_a0_dnx_data_gtimer_ire_block_index_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_ire;
    int define_index = dnx_data_gtimer_ire_define_block_index_size;
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
q2a_a0_dnx_data_gtimer_ire_block_index_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_ire;
    int define_index = dnx_data_gtimer_ire_define_block_index_max_value;
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
q2a_a0_dnx_data_gtimer_itpp_block_index_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_itpp;
    int define_index = dnx_data_gtimer_itpp_define_block_index_size;
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
q2a_a0_dnx_data_gtimer_itpp_block_index_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_itpp;
    int define_index = dnx_data_gtimer_itpp_define_block_index_max_value;
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
q2a_a0_dnx_data_gtimer_itppd_block_index_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_itppd;
    int define_index = dnx_data_gtimer_itppd_define_block_index_size;
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
q2a_a0_dnx_data_gtimer_itppd_block_index_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_itppd;
    int define_index = dnx_data_gtimer_itppd_define_block_index_max_value;
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
q2a_a0_dnx_data_gtimer_kaps_block_index_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_kaps;
    int define_index = dnx_data_gtimer_kaps_define_block_index_size;
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
q2a_a0_dnx_data_gtimer_kaps_block_index_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_kaps;
    int define_index = dnx_data_gtimer_kaps_define_block_index_max_value;
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
q2a_a0_dnx_data_gtimer_mcp_block_index_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_mcp;
    int define_index = dnx_data_gtimer_mcp_define_block_index_size;
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
q2a_a0_dnx_data_gtimer_mcp_block_index_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_mcp;
    int define_index = dnx_data_gtimer_mcp_define_block_index_max_value;
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
q2a_a0_dnx_data_gtimer_mrps_block_index_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_mrps;
    int define_index = dnx_data_gtimer_mrps_define_block_index_size;
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
q2a_a0_dnx_data_gtimer_mrps_block_index_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_mrps;
    int define_index = dnx_data_gtimer_mrps_define_block_index_max_value;
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
q2a_a0_dnx_data_gtimer_nmg_block_index_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_nmg;
    int define_index = dnx_data_gtimer_nmg_define_block_index_size;
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
q2a_a0_dnx_data_gtimer_nmg_block_index_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_nmg;
    int define_index = dnx_data_gtimer_nmg_define_block_index_max_value;
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
q2a_a0_dnx_data_gtimer_ocb_block_index_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_ocb;
    int define_index = dnx_data_gtimer_ocb_define_block_index_size;
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
q2a_a0_dnx_data_gtimer_ocb_block_index_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_ocb;
    int define_index = dnx_data_gtimer_ocb_define_block_index_max_value;
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
q2a_a0_dnx_data_gtimer_pdm_block_index_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_pdm;
    int define_index = dnx_data_gtimer_pdm_define_block_index_size;
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
q2a_a0_dnx_data_gtimer_pdm_block_index_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_pdm;
    int define_index = dnx_data_gtimer_pdm_define_block_index_max_value;
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
q2a_a0_dnx_data_gtimer_pqp_block_index_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_pqp;
    int define_index = dnx_data_gtimer_pqp_define_block_index_size;
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
q2a_a0_dnx_data_gtimer_pqp_block_index_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_pqp;
    int define_index = dnx_data_gtimer_pqp_define_block_index_max_value;
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
q2a_a0_dnx_data_gtimer_rqp_block_index_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_rqp;
    int define_index = dnx_data_gtimer_rqp_define_block_index_size;
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
q2a_a0_dnx_data_gtimer_rqp_block_index_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_rqp;
    int define_index = dnx_data_gtimer_rqp_define_block_index_max_value;
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
q2a_a0_dnx_data_gtimer_sif_block_index_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_sif;
    int define_index = dnx_data_gtimer_sif_define_block_index_size;
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
q2a_a0_dnx_data_gtimer_sif_block_index_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_sif;
    int define_index = dnx_data_gtimer_sif_define_block_index_max_value;
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
q2a_a0_dnx_data_gtimer_spb_block_index_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_spb;
    int define_index = dnx_data_gtimer_spb_define_block_index_size;
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
q2a_a0_dnx_data_gtimer_spb_block_index_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_spb;
    int define_index = dnx_data_gtimer_spb_define_block_index_max_value;
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
q2a_a0_dnx_data_gtimer_sqm_block_index_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_sqm;
    int define_index = dnx_data_gtimer_sqm_define_block_index_size;
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
q2a_a0_dnx_data_gtimer_sqm_block_index_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_sqm;
    int define_index = dnx_data_gtimer_sqm_define_block_index_max_value;
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
q2a_a0_dnx_data_gtimer_tcam_block_index_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_tcam;
    int define_index = dnx_data_gtimer_tcam_define_block_index_size;
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
q2a_a0_dnx_data_gtimer_tcam_block_index_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_tcam;
    int define_index = dnx_data_gtimer_tcam_define_block_index_max_value;
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
q2a_a0_dnx_data_gtimer_tdu_block_index_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_tdu;
    int define_index = dnx_data_gtimer_tdu_define_block_index_size;
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
q2a_a0_dnx_data_gtimer_tdu_block_index_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_tdu;
    int define_index = dnx_data_gtimer_tdu_define_block_index_max_value;
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
q2a_a0_dnx_data_gtimer_dcc_block_index_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_dcc;
    int define_index = dnx_data_gtimer_dcc_define_block_index_size;
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
q2a_a0_dnx_data_gtimer_dcc_block_index_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = dnx_data_gtimer_submodule_dcc;
    int define_index = dnx_data_gtimer_dcc_define_block_index_max_value;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3;

    
    define->data = 3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}




shr_error_e
q2a_a0_data_gtimer_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_gtimer;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_gtimer_submodule_rtp;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_gtimer_rtp_is_supported;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_gtimer_rtp_is_supported_set;

    
    
    submodule_index = dnx_data_gtimer_submodule_fdt;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_gtimer_fdt_is_supported;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_gtimer_fdt_is_supported_set;

    
    
    submodule_index = dnx_data_gtimer_submodule_fdtl;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_gtimer_fdtl_is_supported;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_gtimer_fdtl_is_supported_set;

    
    
    submodule_index = dnx_data_gtimer_submodule_pem;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_gtimer_pem_define_block_index_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_pem_block_index_size_set;
    data_index = dnx_data_gtimer_pem_define_block_index_max_value;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_pem_block_index_max_value_set;

    
    data_index = dnx_data_gtimer_pem_is_supported;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_gtimer_pem_is_supported_set;

    
    
    submodule_index = dnx_data_gtimer_submodule_dhc;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_gtimer_dhc_is_supported;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_gtimer_dhc_is_supported_set;

    
    
    submodule_index = dnx_data_gtimer_submodule_cdu;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_gtimer_cdu_define_block_index_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_cdu_block_index_size_set;
    data_index = dnx_data_gtimer_cdu_define_block_index_max_value;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_cdu_block_index_max_value_set;

    
    data_index = dnx_data_gtimer_cdu_is_supported;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_gtimer_cdu_is_supported_set;

    
    
    submodule_index = dnx_data_gtimer_submodule_esb;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_gtimer_esb_define_block_index_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_esb_block_index_size_set;
    data_index = dnx_data_gtimer_esb_define_block_index_max_value;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_esb_block_index_max_value_set;

    
    data_index = dnx_data_gtimer_esb_is_supported;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_gtimer_esb_is_supported_set;

    
    
    submodule_index = dnx_data_gtimer_submodule_cdum;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_gtimer_cdum_is_supported;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_gtimer_cdum_is_supported_set;

    
    
    submodule_index = dnx_data_gtimer_submodule_mesh_topology;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_gtimer_mesh_topology_is_supported;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_gtimer_mesh_topology_is_supported_set;

    
    
    submodule_index = dnx_data_gtimer_submodule_bdm;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_gtimer_bdm_define_block_index_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_bdm_block_index_size_set;
    data_index = dnx_data_gtimer_bdm_define_block_index_max_value;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_bdm_block_index_max_value_set;

    
    data_index = dnx_data_gtimer_bdm_is_supported;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_gtimer_bdm_is_supported_set;

    
    
    submodule_index = dnx_data_gtimer_submodule_ecgm;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_gtimer_ecgm_define_block_index_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_ecgm_block_index_size_set;
    data_index = dnx_data_gtimer_ecgm_define_block_index_max_value;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_ecgm_block_index_max_value_set;

    
    data_index = dnx_data_gtimer_ecgm_is_supported;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_gtimer_ecgm_is_supported_set;

    
    
    submodule_index = dnx_data_gtimer_submodule_cfc;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_gtimer_cfc_define_block_index_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_cfc_block_index_size_set;
    data_index = dnx_data_gtimer_cfc_define_block_index_max_value;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_cfc_block_index_max_value_set;

    
    data_index = dnx_data_gtimer_cfc_is_supported;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_gtimer_cfc_is_supported_set;

    
    
    submodule_index = dnx_data_gtimer_submodule_cgm;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_gtimer_cgm_define_block_index_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_cgm_block_index_size_set;
    data_index = dnx_data_gtimer_cgm_define_block_index_max_value;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_cgm_block_index_max_value_set;

    
    data_index = dnx_data_gtimer_cgm_is_supported;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_gtimer_cgm_is_supported_set;

    
    
    submodule_index = dnx_data_gtimer_submodule_crps;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_gtimer_crps_define_block_index_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_crps_block_index_size_set;
    data_index = dnx_data_gtimer_crps_define_block_index_max_value;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_crps_block_index_max_value_set;

    
    data_index = dnx_data_gtimer_crps_is_supported;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_gtimer_crps_is_supported_set;

    
    
    submodule_index = dnx_data_gtimer_submodule_ddha;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_gtimer_ddha_define_block_index_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_ddha_block_index_size_set;
    data_index = dnx_data_gtimer_ddha_define_block_index_max_value;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_ddha_block_index_max_value_set;

    
    data_index = dnx_data_gtimer_ddha_is_supported;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_gtimer_ddha_is_supported_set;

    
    
    submodule_index = dnx_data_gtimer_submodule_ddp;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_gtimer_ddp_define_block_index_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_ddp_block_index_size_set;
    data_index = dnx_data_gtimer_ddp_define_block_index_max_value;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_ddp_block_index_max_value_set;

    

    
    
    submodule_index = dnx_data_gtimer_submodule_dqm;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_gtimer_dqm_define_block_index_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_dqm_block_index_size_set;
    data_index = dnx_data_gtimer_dqm_define_block_index_max_value;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_dqm_block_index_max_value_set;

    

    
    
    submodule_index = dnx_data_gtimer_submodule_edb;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_gtimer_edb_define_block_index_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_edb_block_index_size_set;
    data_index = dnx_data_gtimer_edb_define_block_index_max_value;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_edb_block_index_max_value_set;

    

    
    
    submodule_index = dnx_data_gtimer_submodule_epni;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_gtimer_epni_define_block_index_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_epni_block_index_size_set;
    data_index = dnx_data_gtimer_epni_define_block_index_max_value;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_epni_block_index_max_value_set;

    

    
    
    submodule_index = dnx_data_gtimer_submodule_epre;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_gtimer_epre_define_block_index_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_epre_block_index_size_set;
    data_index = dnx_data_gtimer_epre_define_block_index_max_value;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_epre_block_index_max_value_set;

    

    
    
    submodule_index = dnx_data_gtimer_submodule_eps;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_gtimer_eps_define_block_index_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_eps_block_index_size_set;
    data_index = dnx_data_gtimer_eps_define_block_index_max_value;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_eps_block_index_max_value_set;

    

    
    
    submodule_index = dnx_data_gtimer_submodule_erpp;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_gtimer_erpp_define_block_index_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_erpp_block_index_size_set;
    data_index = dnx_data_gtimer_erpp_define_block_index_max_value;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_erpp_block_index_max_value_set;

    

    
    
    submodule_index = dnx_data_gtimer_submodule_etppa;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_gtimer_etppa_define_block_index_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_etppa_block_index_size_set;
    data_index = dnx_data_gtimer_etppa_define_block_index_max_value;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_etppa_block_index_max_value_set;

    

    
    
    submodule_index = dnx_data_gtimer_submodule_etppb;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_gtimer_etppb_define_block_index_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_etppb_block_index_size_set;
    data_index = dnx_data_gtimer_etppb_define_block_index_max_value;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_etppb_block_index_max_value_set;

    

    
    
    submodule_index = dnx_data_gtimer_submodule_etppc;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_gtimer_etppc_define_block_index_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_etppc_block_index_size_set;
    data_index = dnx_data_gtimer_etppc_define_block_index_max_value;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_etppc_block_index_max_value_set;

    

    
    
    submodule_index = dnx_data_gtimer_submodule_fqp;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_gtimer_fqp_define_block_index_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_fqp_block_index_size_set;
    data_index = dnx_data_gtimer_fqp_define_block_index_max_value;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_fqp_block_index_max_value_set;

    

    
    
    submodule_index = dnx_data_gtimer_submodule_ile;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_gtimer_ile_define_block_index_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_ile_block_index_size_set;
    data_index = dnx_data_gtimer_ile_define_block_index_max_value;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_ile_block_index_max_value_set;

    

    
    
    submodule_index = dnx_data_gtimer_submodule_ippa;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_gtimer_ippa_define_block_index_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_ippa_block_index_size_set;
    data_index = dnx_data_gtimer_ippa_define_block_index_max_value;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_ippa_block_index_max_value_set;

    

    
    
    submodule_index = dnx_data_gtimer_submodule_ippb;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_gtimer_ippb_define_block_index_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_ippb_block_index_size_set;
    data_index = dnx_data_gtimer_ippb_define_block_index_max_value;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_ippb_block_index_max_value_set;

    

    
    
    submodule_index = dnx_data_gtimer_submodule_ippc;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_gtimer_ippc_define_block_index_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_ippc_block_index_size_set;
    data_index = dnx_data_gtimer_ippc_define_block_index_max_value;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_ippc_block_index_max_value_set;

    

    
    
    submodule_index = dnx_data_gtimer_submodule_ippd;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_gtimer_ippd_define_block_index_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_ippd_block_index_size_set;
    data_index = dnx_data_gtimer_ippd_define_block_index_max_value;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_ippd_block_index_max_value_set;

    

    
    
    submodule_index = dnx_data_gtimer_submodule_ippe;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_gtimer_ippe_define_block_index_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_ippe_block_index_size_set;
    data_index = dnx_data_gtimer_ippe_define_block_index_max_value;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_ippe_block_index_max_value_set;

    

    
    
    submodule_index = dnx_data_gtimer_submodule_ippf;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_gtimer_ippf_define_block_index_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_ippf_block_index_size_set;
    data_index = dnx_data_gtimer_ippf_define_block_index_max_value;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_ippf_block_index_max_value_set;

    

    
    
    submodule_index = dnx_data_gtimer_submodule_ips;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_gtimer_ips_define_block_index_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_ips_block_index_size_set;
    data_index = dnx_data_gtimer_ips_define_block_index_max_value;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_ips_block_index_max_value_set;

    

    
    
    submodule_index = dnx_data_gtimer_submodule_ipt;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_gtimer_ipt_define_block_index_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_ipt_block_index_size_set;
    data_index = dnx_data_gtimer_ipt_define_block_index_max_value;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_ipt_block_index_max_value_set;

    

    
    
    submodule_index = dnx_data_gtimer_submodule_iqm;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_gtimer_iqm_define_block_index_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_iqm_block_index_size_set;
    data_index = dnx_data_gtimer_iqm_define_block_index_max_value;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_iqm_block_index_max_value_set;

    

    
    
    submodule_index = dnx_data_gtimer_submodule_ire;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_gtimer_ire_define_block_index_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_ire_block_index_size_set;
    data_index = dnx_data_gtimer_ire_define_block_index_max_value;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_ire_block_index_max_value_set;

    

    
    
    submodule_index = dnx_data_gtimer_submodule_itpp;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_gtimer_itpp_define_block_index_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_itpp_block_index_size_set;
    data_index = dnx_data_gtimer_itpp_define_block_index_max_value;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_itpp_block_index_max_value_set;

    

    
    
    submodule_index = dnx_data_gtimer_submodule_itppd;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_gtimer_itppd_define_block_index_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_itppd_block_index_size_set;
    data_index = dnx_data_gtimer_itppd_define_block_index_max_value;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_itppd_block_index_max_value_set;

    

    
    
    submodule_index = dnx_data_gtimer_submodule_kaps;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_gtimer_kaps_define_block_index_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_kaps_block_index_size_set;
    data_index = dnx_data_gtimer_kaps_define_block_index_max_value;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_kaps_block_index_max_value_set;

    

    
    
    submodule_index = dnx_data_gtimer_submodule_mcp;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_gtimer_mcp_define_block_index_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_mcp_block_index_size_set;
    data_index = dnx_data_gtimer_mcp_define_block_index_max_value;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_mcp_block_index_max_value_set;

    

    
    
    submodule_index = dnx_data_gtimer_submodule_mrps;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_gtimer_mrps_define_block_index_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_mrps_block_index_size_set;
    data_index = dnx_data_gtimer_mrps_define_block_index_max_value;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_mrps_block_index_max_value_set;

    

    
    
    submodule_index = dnx_data_gtimer_submodule_nmg;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_gtimer_nmg_define_block_index_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_nmg_block_index_size_set;
    data_index = dnx_data_gtimer_nmg_define_block_index_max_value;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_nmg_block_index_max_value_set;

    

    
    
    submodule_index = dnx_data_gtimer_submodule_ocb;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_gtimer_ocb_define_block_index_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_ocb_block_index_size_set;
    data_index = dnx_data_gtimer_ocb_define_block_index_max_value;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_ocb_block_index_max_value_set;

    

    
    
    submodule_index = dnx_data_gtimer_submodule_pdm;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_gtimer_pdm_define_block_index_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_pdm_block_index_size_set;
    data_index = dnx_data_gtimer_pdm_define_block_index_max_value;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_pdm_block_index_max_value_set;

    

    
    
    submodule_index = dnx_data_gtimer_submodule_pqp;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_gtimer_pqp_define_block_index_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_pqp_block_index_size_set;
    data_index = dnx_data_gtimer_pqp_define_block_index_max_value;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_pqp_block_index_max_value_set;

    

    
    
    submodule_index = dnx_data_gtimer_submodule_rqp;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_gtimer_rqp_define_block_index_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_rqp_block_index_size_set;
    data_index = dnx_data_gtimer_rqp_define_block_index_max_value;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_rqp_block_index_max_value_set;

    

    
    
    submodule_index = dnx_data_gtimer_submodule_sif;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_gtimer_sif_define_block_index_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_sif_block_index_size_set;
    data_index = dnx_data_gtimer_sif_define_block_index_max_value;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_sif_block_index_max_value_set;

    

    
    
    submodule_index = dnx_data_gtimer_submodule_spb;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_gtimer_spb_define_block_index_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_spb_block_index_size_set;
    data_index = dnx_data_gtimer_spb_define_block_index_max_value;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_spb_block_index_max_value_set;

    

    
    
    submodule_index = dnx_data_gtimer_submodule_sqm;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_gtimer_sqm_define_block_index_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_sqm_block_index_size_set;
    data_index = dnx_data_gtimer_sqm_define_block_index_max_value;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_sqm_block_index_max_value_set;

    

    
    
    submodule_index = dnx_data_gtimer_submodule_tcam;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_gtimer_tcam_define_block_index_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_tcam_block_index_size_set;
    data_index = dnx_data_gtimer_tcam_define_block_index_max_value;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_tcam_block_index_max_value_set;

    

    
    
    submodule_index = dnx_data_gtimer_submodule_tdu;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_gtimer_tdu_define_block_index_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_tdu_block_index_size_set;
    data_index = dnx_data_gtimer_tdu_define_block_index_max_value;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_tdu_block_index_max_value_set;

    

    
    
    submodule_index = dnx_data_gtimer_submodule_dcc;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_gtimer_dcc_define_block_index_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_dcc_block_index_size_set;
    data_index = dnx_data_gtimer_dcc_define_block_index_max_value;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_gtimer_dcc_block_index_max_value_set;

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

