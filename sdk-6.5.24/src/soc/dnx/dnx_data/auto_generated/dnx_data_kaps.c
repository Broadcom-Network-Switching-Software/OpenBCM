
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_KAPSDNX

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_kaps.h>



#ifdef BCM_DNX2_SUPPORT

extern shr_error_e jr2_a0_data_kaps_attach(
    int unit);

#endif 



static shr_error_e
dnx_data_kaps_flist_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "flist";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_kaps_flist_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data kaps flist features");

    submodule_data->features[dnx_data_kaps_flist_advanced_ad].name = "advanced_ad";
    submodule_data->features[dnx_data_kaps_flist_advanced_ad].doc = "";
    submodule_data->features[dnx_data_kaps_flist_advanced_ad].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_kaps_flist_zero_bit_ad].name = "zero_bit_ad";
    submodule_data->features[dnx_data_kaps_flist_zero_bit_ad].doc = "";
    submodule_data->features[dnx_data_kaps_flist_zero_bit_ad].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_kaps_flist_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data kaps flist defines");

    
    submodule_data->nof_tables = _dnx_data_kaps_flist_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data kaps flist tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_kaps_flist_feature_get(
    int unit,
    dnx_data_kaps_flist_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_kaps, dnx_data_kaps_submodule_flist, feature);
}








shr_error_e
dnx_data_kaps_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "kaps";
    module_data->nof_submodules = _dnx_data_kaps_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data kaps submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_kaps_flist_init(unit, &module_data->submodules[dnx_data_kaps_submodule_flist]));
    
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_kaps_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_kaps_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_kaps_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_kaps_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_kaps_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_kaps_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_kaps_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_kaps_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_kaps_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2p_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_kaps_attach(unit));
    }
    else

#endif 
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

