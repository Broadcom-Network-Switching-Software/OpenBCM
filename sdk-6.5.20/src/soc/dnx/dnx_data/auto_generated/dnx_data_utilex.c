

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



extern shr_error_e jr2_a0_data_utilex_attach(
    int unit);



static shr_error_e
dnx_data_utilex_common_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "common";
    submodule_data->doc = "Definitions common to all the various utilex services";
    
    submodule_data->nof_features = _dnx_data_utilex_common_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data utilex common features");

    
    submodule_data->nof_defines = _dnx_data_utilex_common_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data utilex common defines");

    submodule_data->defines[dnx_data_utilex_common_define_max_nof_dss_per_core].name = "max_nof_dss_per_core";
    submodule_data->defines[dnx_data_utilex_common_define_max_nof_dss_per_core].doc = "Maximal number of bit map arrays to assign at init.";
    
    submodule_data->defines[dnx_data_utilex_common_define_max_nof_dss_per_core].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_utilex_common_define_max_nof_hashs_per_core].name = "max_nof_hashs_per_core";
    submodule_data->defines[dnx_data_utilex_common_define_max_nof_hashs_per_core].doc = "Maximal number of hash table entries to assign at init.";
    
    submodule_data->defines[dnx_data_utilex_common_define_max_nof_hashs_per_core].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_utilex_common_define_max_nof_lists_per_core].name = "max_nof_lists_per_core";
    submodule_data->defines[dnx_data_utilex_common_define_max_nof_lists_per_core].doc = "Maximal number of sorted list entries to assign at init.";
    
    submodule_data->defines[dnx_data_utilex_common_define_max_nof_lists_per_core].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_utilex_common_define_max_nof_multis_per_core].name = "max_nof_multis_per_core";
    submodule_data->defines[dnx_data_utilex_common_define_max_nof_multis_per_core].doc = "Maximal number of multi set entries to assign at init.";
    
    submodule_data->defines[dnx_data_utilex_common_define_max_nof_multis_per_core].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_utilex_common_define_max_nof_res_tag_bitmaps_per_core].name = "max_nof_res_tag_bitmaps_per_core";
    submodule_data->defines[dnx_data_utilex_common_define_max_nof_res_tag_bitmaps_per_core].doc = "Maximal number of res tag bitmaps to assign at init.";
    
    submodule_data->defines[dnx_data_utilex_common_define_max_nof_res_tag_bitmaps_per_core].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_utilex_common_define_max_nof_defragmented_chunks_per_core].name = "max_nof_defragmented_chunks_per_core";
    submodule_data->defines[dnx_data_utilex_common_define_max_nof_defragmented_chunks_per_core].doc = "Maximal number of defragmented chunks to assign at init.";
    
    submodule_data->defines[dnx_data_utilex_common_define_max_nof_defragmented_chunks_per_core].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_utilex_common_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data utilex common tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_utilex_common_feature_get(
    int unit,
    dnx_data_utilex_common_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_utilex, dnx_data_utilex_submodule_common, feature);
}


uint32
dnx_data_utilex_common_max_nof_dss_per_core_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_utilex, dnx_data_utilex_submodule_common, dnx_data_utilex_common_define_max_nof_dss_per_core);
}

uint32
dnx_data_utilex_common_max_nof_hashs_per_core_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_utilex, dnx_data_utilex_submodule_common, dnx_data_utilex_common_define_max_nof_hashs_per_core);
}

uint32
dnx_data_utilex_common_max_nof_lists_per_core_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_utilex, dnx_data_utilex_submodule_common, dnx_data_utilex_common_define_max_nof_lists_per_core);
}

uint32
dnx_data_utilex_common_max_nof_multis_per_core_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_utilex, dnx_data_utilex_submodule_common, dnx_data_utilex_common_define_max_nof_multis_per_core);
}

uint32
dnx_data_utilex_common_max_nof_res_tag_bitmaps_per_core_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_utilex, dnx_data_utilex_submodule_common, dnx_data_utilex_common_define_max_nof_res_tag_bitmaps_per_core);
}

uint32
dnx_data_utilex_common_max_nof_defragmented_chunks_per_core_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_utilex, dnx_data_utilex_submodule_common, dnx_data_utilex_common_define_max_nof_defragmented_chunks_per_core);
}







shr_error_e
dnx_data_utilex_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "utilex";
    module_data->nof_submodules = _dnx_data_utilex_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data utilex submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_utilex_common_init(unit, &module_data->submodules[dnx_data_utilex_submodule_common]));
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_utilex_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_utilex_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_utilex_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_utilex_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_utilex_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_utilex_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_utilex_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_utilex_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_utilex_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

