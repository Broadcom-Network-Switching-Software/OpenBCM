
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COMMON

#include <soc/dnx/dnx_data/auto_generated/dnx2/dnx_data_internal_dnx2_aod_sizes.h>




extern shr_error_e jr2_a0_data_dnx2_aod_sizes_attach(
    int unit);


extern shr_error_e jr2_b0_data_dnx2_aod_sizes_attach(
    int unit);


extern shr_error_e j2c_a0_data_dnx2_aod_sizes_attach(
    int unit);


extern shr_error_e q2a_a0_data_dnx2_aod_sizes_attach(
    int unit);


extern shr_error_e q2a_b0_data_dnx2_aod_sizes_attach(
    int unit);


extern shr_error_e j2p_a0_data_dnx2_aod_sizes_attach(
    int unit);


extern shr_error_e j2x_a0_data_dnx2_aod_sizes_attach(
    int unit);




static shr_error_e
dnx_data_dnx2_aod_sizes_AOD_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "AOD";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_dnx2_aod_sizes_AOD_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data dnx2_aod_sizes AOD features");

    
    submodule_data->nof_defines = _dnx_data_dnx2_aod_sizes_AOD_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data dnx2_aod_sizes AOD defines");

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_AOD_define_ETPP_TRAP_CONTEXT_PORT_PROFILE].name = "ETPP_TRAP_CONTEXT_PORT_PROFILE";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_AOD_define_ETPP_TRAP_CONTEXT_PORT_PROFILE].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_AOD_define_ETPP_TRAP_CONTEXT_PORT_PROFILE].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_AOD_define_TDM_TRAP_CANDIDATE_HEADER].name = "TDM_TRAP_CANDIDATE_HEADER";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_AOD_define_TDM_TRAP_CANDIDATE_HEADER].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_AOD_define_TDM_TRAP_CANDIDATE_HEADER].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_AOD_define_TDM_TRAP_CANDIDATE_HEADER___PP_DSP_AG].name = "TDM_TRAP_CANDIDATE_HEADER___PP_DSP_AG";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_AOD_define_TDM_TRAP_CANDIDATE_HEADER___PP_DSP_AG].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_AOD_define_TDM_TRAP_CANDIDATE_HEADER___PP_DSP_AG].flags |= DNXC_DATA_F_NUMERIC;
    }

    
    submodule_data->nof_tables = _dnx_data_dnx2_aod_sizes_AOD_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data dnx2_aod_sizes AOD tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_dnx2_aod_sizes_AOD_feature_get(
    int unit,
    dnx_data_dnx2_aod_sizes_AOD_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_AOD, feature);
}


uint32
dnx_data_dnx2_aod_sizes_AOD_ETPP_TRAP_CONTEXT_PORT_PROFILE_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPP_TRAP_CONTEXT_PORT_PROFILE", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_AOD, dnx_data_dnx2_aod_sizes_AOD_define_ETPP_TRAP_CONTEXT_PORT_PROFILE);
}

uint32
dnx_data_dnx2_aod_sizes_AOD_TDM_TRAP_CANDIDATE_HEADER_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "TDM_TRAP_CANDIDATE_HEADER", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_AOD, dnx_data_dnx2_aod_sizes_AOD_define_TDM_TRAP_CANDIDATE_HEADER);
}

uint32
dnx_data_dnx2_aod_sizes_AOD_TDM_TRAP_CANDIDATE_HEADER___PP_DSP_AG_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "TDM_TRAP_CANDIDATE_HEADER___PP_DSP_AG", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_AOD, dnx_data_dnx2_aod_sizes_AOD_define_TDM_TRAP_CANDIDATE_HEADER___PP_DSP_AG);
}










static shr_error_e
dnx_data_dnx2_aod_sizes_dynamic_AOD_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "dynamic_AOD";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_dnx2_aod_sizes_dynamic_AOD_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data dnx2_aod_sizes dynamic_AOD features");

    
    submodule_data->nof_defines = _dnx_data_dnx2_aod_sizes_dynamic_AOD_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data dnx2_aod_sizes dynamic_AOD defines");

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_0].name = "aod_0";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_0].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_0].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_1].name = "aod_1";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_1].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_1].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_2].name = "aod_2";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_2].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_2].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_3].name = "aod_3";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_3].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_3].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_4].name = "aod_4";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_4].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_4].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_5].name = "aod_5";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_5].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_5].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_6].name = "aod_6";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_6].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_6].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_7].name = "aod_7";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_7].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_7].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_8].name = "aod_8";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_8].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_8].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_9].name = "aod_9";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_9].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_9].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_10].name = "aod_10";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_10].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_10].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_11].name = "aod_11";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_11].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_11].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_12].name = "aod_12";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_12].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_12].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_13].name = "aod_13";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_13].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_13].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_14].name = "aod_14";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_14].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_14].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_15].name = "aod_15";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_15].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_15].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_16].name = "aod_16";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_16].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_16].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_17].name = "aod_17";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_17].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_17].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_18].name = "aod_18";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_18].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_18].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_19].name = "aod_19";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_19].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_19].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_20].name = "aod_20";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_20].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_20].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_21].name = "aod_21";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_21].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_21].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_22].name = "aod_22";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_22].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_22].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_23].name = "aod_23";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_23].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_23].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_24].name = "aod_24";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_24].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_24].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_25].name = "aod_25";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_25].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_25].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_26].name = "aod_26";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_26].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_26].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_27].name = "aod_27";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_27].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_27].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_28].name = "aod_28";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_28].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_28].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_29].name = "aod_29";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_29].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_29].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_30].name = "aod_30";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_30].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_30].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_31].name = "aod_31";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_31].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_31].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_32].name = "aod_32";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_32].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_32].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_33].name = "aod_33";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_33].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_33].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_34].name = "aod_34";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_34].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_34].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_35].name = "aod_35";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_35].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_35].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_36].name = "aod_36";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_36].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_36].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_37].name = "aod_37";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_37].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_37].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_38].name = "aod_38";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_38].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_38].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_39].name = "aod_39";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_39].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_39].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_40].name = "aod_40";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_40].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_40].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_41].name = "aod_41";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_41].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_41].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_42].name = "aod_42";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_42].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_42].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_43].name = "aod_43";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_43].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_43].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_44].name = "aod_44";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_44].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_44].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_45].name = "aod_45";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_45].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_45].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_46].name = "aod_46";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_46].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_46].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_47].name = "aod_47";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_47].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_47].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_48].name = "aod_48";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_48].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_48].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_49].name = "aod_49";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_49].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_49].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_50].name = "aod_50";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_50].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_50].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_51].name = "aod_51";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_51].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_51].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_52].name = "aod_52";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_52].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_52].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_53].name = "aod_53";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_53].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_53].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_54].name = "aod_54";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_54].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_54].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_55].name = "aod_55";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_55].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_55].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_56].name = "aod_56";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_56].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_56].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_57].name = "aod_57";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_57].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_57].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_58].name = "aod_58";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_58].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_58].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_59].name = "aod_59";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_59].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_59].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_60].name = "aod_60";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_60].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_60].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_61].name = "aod_61";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_61].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_61].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_62].name = "aod_62";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_62].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_62].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_63].name = "aod_63";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_63].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_63].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_64].name = "aod_64";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_64].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_64].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_65].name = "aod_65";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_65].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_65].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_66].name = "aod_66";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_66].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_66].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_67].name = "aod_67";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_67].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_67].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_68].name = "aod_68";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_68].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_68].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_69].name = "aod_69";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_69].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_69].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_70].name = "aod_70";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_70].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_70].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_71].name = "aod_71";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_71].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_71].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_72].name = "aod_72";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_72].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_72].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_73].name = "aod_73";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_73].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_73].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_74].name = "aod_74";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_74].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_74].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_75].name = "aod_75";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_75].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_75].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_76].name = "aod_76";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_76].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_76].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_77].name = "aod_77";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_77].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_77].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_78].name = "aod_78";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_78].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_78].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_79].name = "aod_79";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_79].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_79].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_80].name = "aod_80";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_80].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_80].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_81].name = "aod_81";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_81].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_81].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_82].name = "aod_82";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_82].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_82].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_83].name = "aod_83";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_83].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_83].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_84].name = "aod_84";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_84].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_84].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_85].name = "aod_85";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_85].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_85].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_86].name = "aod_86";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_86].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_86].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_87].name = "aod_87";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_87].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_87].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_88].name = "aod_88";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_88].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_88].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_89].name = "aod_89";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_89].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_89].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_90].name = "aod_90";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_90].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_90].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_91].name = "aod_91";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_91].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_91].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_92].name = "aod_92";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_92].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_92].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_93].name = "aod_93";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_93].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_93].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_94].name = "aod_94";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_94].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_94].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_95].name = "aod_95";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_95].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_95].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_96].name = "aod_96";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_96].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_96].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_97].name = "aod_97";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_97].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_97].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_98].name = "aod_98";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_98].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_98].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_99].name = "aod_99";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_99].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_99].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_100].name = "aod_100";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_100].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_100].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_101].name = "aod_101";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_101].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_101].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_102].name = "aod_102";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_102].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_102].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_103].name = "aod_103";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_103].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_103].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_104].name = "aod_104";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_104].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_104].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_105].name = "aod_105";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_105].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_105].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_106].name = "aod_106";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_106].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_106].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_107].name = "aod_107";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_107].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_107].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_108].name = "aod_108";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_108].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_108].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_109].name = "aod_109";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_109].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_109].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_110].name = "aod_110";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_110].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_110].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_111].name = "aod_111";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_111].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_111].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_112].name = "aod_112";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_112].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_112].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_113].name = "aod_113";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_113].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_113].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_114].name = "aod_114";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_114].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_114].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_115].name = "aod_115";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_115].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_115].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_116].name = "aod_116";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_116].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_116].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_117].name = "aod_117";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_117].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_117].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_118].name = "aod_118";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_118].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_118].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_119].name = "aod_119";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_119].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_119].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_120].name = "aod_120";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_120].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_120].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_121].name = "aod_121";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_121].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_121].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_122].name = "aod_122";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_122].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_122].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_123].name = "aod_123";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_123].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_123].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_124].name = "aod_124";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_124].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_124].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_125].name = "aod_125";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_125].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_125].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_126].name = "aod_126";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_126].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_126].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_127].name = "aod_127";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_127].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_127].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_128].name = "aod_128";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_128].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_128].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_129].name = "aod_129";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_129].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_129].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_130].name = "aod_130";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_130].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_130].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_131].name = "aod_131";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_131].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_131].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_132].name = "aod_132";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_132].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_132].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_133].name = "aod_133";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_133].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_133].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_134].name = "aod_134";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_134].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_134].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_135].name = "aod_135";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_135].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_135].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_136].name = "aod_136";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_136].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_136].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_137].name = "aod_137";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_137].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_137].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_138].name = "aod_138";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_138].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_138].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_139].name = "aod_139";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_139].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_139].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_140].name = "aod_140";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_140].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_140].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_141].name = "aod_141";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_141].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_141].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_142].name = "aod_142";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_142].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_142].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_143].name = "aod_143";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_143].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_143].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_144].name = "aod_144";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_144].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_144].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_145].name = "aod_145";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_145].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_145].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_146].name = "aod_146";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_146].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_146].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_147].name = "aod_147";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_147].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_147].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_148].name = "aod_148";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_148].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_148].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_149].name = "aod_149";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_149].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_149].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_150].name = "aod_150";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_150].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_150].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_151].name = "aod_151";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_151].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_151].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_152].name = "aod_152";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_152].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_152].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_153].name = "aod_153";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_153].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_153].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_154].name = "aod_154";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_154].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_154].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_155].name = "aod_155";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_155].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_155].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_156].name = "aod_156";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_156].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_156].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_157].name = "aod_157";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_157].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_157].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_158].name = "aod_158";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_158].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_158].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_159].name = "aod_159";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_159].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_159].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_160].name = "aod_160";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_160].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_160].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_161].name = "aod_161";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_161].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_161].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_162].name = "aod_162";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_162].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_162].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_163].name = "aod_163";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_163].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_163].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_164].name = "aod_164";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_164].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_164].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_165].name = "aod_165";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_165].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_165].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_166].name = "aod_166";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_166].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_166].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_167].name = "aod_167";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_167].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_167].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_168].name = "aod_168";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_168].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_168].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_169].name = "aod_169";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_169].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_169].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_170].name = "aod_170";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_170].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_170].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_171].name = "aod_171";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_171].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_171].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_172].name = "aod_172";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_172].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_172].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_173].name = "aod_173";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_173].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_173].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_174].name = "aod_174";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_174].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_174].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_175].name = "aod_175";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_175].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_175].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_176].name = "aod_176";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_176].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_176].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_177].name = "aod_177";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_177].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_177].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_178].name = "aod_178";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_178].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_178].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_179].name = "aod_179";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_179].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_179].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_180].name = "aod_180";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_180].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_180].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_181].name = "aod_181";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_181].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_181].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_182].name = "aod_182";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_182].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_182].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_183].name = "aod_183";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_183].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_183].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_184].name = "aod_184";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_184].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_184].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_185].name = "aod_185";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_185].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_185].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_186].name = "aod_186";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_186].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_186].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_187].name = "aod_187";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_187].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_187].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_188].name = "aod_188";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_188].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_188].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_189].name = "aod_189";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_189].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_189].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_190].name = "aod_190";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_190].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_190].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_191].name = "aod_191";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_191].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_191].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_192].name = "aod_192";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_192].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_192].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_193].name = "aod_193";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_193].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_193].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_194].name = "aod_194";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_194].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_194].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_195].name = "aod_195";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_195].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_195].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_196].name = "aod_196";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_196].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_196].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_197].name = "aod_197";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_197].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_197].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_198].name = "aod_198";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_198].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_198].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_199].name = "aod_199";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_199].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_199].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_200].name = "aod_200";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_200].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_200].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_201].name = "aod_201";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_201].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_201].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_202].name = "aod_202";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_202].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_202].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_203].name = "aod_203";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_203].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_203].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_204].name = "aod_204";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_204].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_204].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_205].name = "aod_205";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_205].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_205].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_206].name = "aod_206";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_206].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_206].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_207].name = "aod_207";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_207].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_207].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_208].name = "aod_208";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_208].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_208].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_209].name = "aod_209";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_209].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_209].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_210].name = "aod_210";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_210].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_210].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_211].name = "aod_211";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_211].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_211].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_212].name = "aod_212";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_212].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_212].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_213].name = "aod_213";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_213].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_213].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_214].name = "aod_214";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_214].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_214].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_215].name = "aod_215";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_215].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_215].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_216].name = "aod_216";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_216].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_216].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_217].name = "aod_217";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_217].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_217].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_218].name = "aod_218";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_218].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_218].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_219].name = "aod_219";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_219].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_219].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_220].name = "aod_220";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_220].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_220].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_221].name = "aod_221";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_221].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_221].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_222].name = "aod_222";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_222].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_222].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_223].name = "aod_223";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_223].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_223].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_224].name = "aod_224";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_224].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_224].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_225].name = "aod_225";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_225].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_225].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_226].name = "aod_226";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_226].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_226].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_227].name = "aod_227";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_227].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_227].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_228].name = "aod_228";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_228].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_228].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_229].name = "aod_229";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_229].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_229].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_230].name = "aod_230";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_230].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_230].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_231].name = "aod_231";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_231].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_231].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_232].name = "aod_232";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_232].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_232].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_233].name = "aod_233";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_233].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_233].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_234].name = "aod_234";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_234].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_234].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_235].name = "aod_235";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_235].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_235].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_236].name = "aod_236";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_236].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_236].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_237].name = "aod_237";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_237].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_237].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_238].name = "aod_238";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_238].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_238].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_239].name = "aod_239";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_239].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_239].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_240].name = "aod_240";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_240].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_240].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_241].name = "aod_241";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_241].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_241].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_242].name = "aod_242";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_242].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_242].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_243].name = "aod_243";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_243].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_243].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_244].name = "aod_244";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_244].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_244].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_245].name = "aod_245";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_245].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_245].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_246].name = "aod_246";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_246].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_246].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_247].name = "aod_247";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_247].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_247].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_248].name = "aod_248";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_248].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_248].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_249].name = "aod_249";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_249].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_249].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_250].name = "aod_250";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_250].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_250].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_251].name = "aod_251";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_251].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_251].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_252].name = "aod_252";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_252].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_252].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_253].name = "aod_253";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_253].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_253].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_254].name = "aod_254";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_254].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_254].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_255].name = "aod_255";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_255].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_255].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_256].name = "aod_256";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_256].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_256].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_257].name = "aod_257";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_257].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_257].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_258].name = "aod_258";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_258].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_258].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_259].name = "aod_259";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_259].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_259].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_260].name = "aod_260";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_260].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_260].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_261].name = "aod_261";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_261].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_261].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_262].name = "aod_262";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_262].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_262].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_263].name = "aod_263";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_263].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_263].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_264].name = "aod_264";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_264].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_264].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_265].name = "aod_265";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_265].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_265].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_266].name = "aod_266";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_266].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_266].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_267].name = "aod_267";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_267].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_267].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_268].name = "aod_268";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_268].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_268].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_269].name = "aod_269";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_269].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_269].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_270].name = "aod_270";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_270].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_270].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_271].name = "aod_271";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_271].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_271].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_272].name = "aod_272";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_272].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_272].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_273].name = "aod_273";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_273].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_273].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_274].name = "aod_274";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_274].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_274].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_275].name = "aod_275";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_275].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_275].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_276].name = "aod_276";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_276].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_276].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_277].name = "aod_277";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_277].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_277].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_278].name = "aod_278";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_278].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_278].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_279].name = "aod_279";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_279].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_279].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_280].name = "aod_280";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_280].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_280].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_281].name = "aod_281";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_281].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_281].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_282].name = "aod_282";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_282].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_282].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_283].name = "aod_283";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_283].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_283].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_284].name = "aod_284";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_284].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_284].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_285].name = "aod_285";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_285].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_285].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_286].name = "aod_286";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_286].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_286].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_287].name = "aod_287";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_287].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_287].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_288].name = "aod_288";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_288].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_288].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_289].name = "aod_289";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_289].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_289].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_290].name = "aod_290";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_290].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_290].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_291].name = "aod_291";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_291].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_291].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_292].name = "aod_292";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_292].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_292].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_293].name = "aod_293";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_293].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_293].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_294].name = "aod_294";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_294].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_294].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_295].name = "aod_295";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_295].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_295].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_296].name = "aod_296";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_296].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_296].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_297].name = "aod_297";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_297].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_297].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_298].name = "aod_298";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_298].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_298].flags |= DNXC_DATA_F_NUMERIC;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_299].name = "aod_299";
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_299].doc = "";
        
        submodule_data->defines[dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_299].flags |= DNXC_DATA_F_NUMERIC;
    }

    
    submodule_data->nof_tables = _dnx_data_dnx2_aod_sizes_dynamic_AOD_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data dnx2_aod_sizes dynamic_AOD tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_dnx2_aod_sizes_dynamic_AOD_feature_get(
    int unit,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, feature);
}


uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_0_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_0", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_0);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_1_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_1", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_1);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_2_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_2", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_2);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_3_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_3", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_3);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_4_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_4", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_4);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_5_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_5", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_5);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_6_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_6", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_6);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_7_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_7", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_7);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_8_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_8", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_8);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_9_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_9", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_9);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_10_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_10", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_10);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_11_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_11", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_11);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_12_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_12", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_12);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_13_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_13", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_13);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_14_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_14", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_14);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_15_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_15", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_15);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_16_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_16", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_16);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_17_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_17", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_17);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_18_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_18", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_18);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_19_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_19", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_19);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_20_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_20", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_20);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_21_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_21", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_21);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_22_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_22", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_22);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_23_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_23", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_23);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_24_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_24", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_24);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_25_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_25", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_25);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_26_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_26", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_26);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_27_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_27", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_27);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_28_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_28", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_28);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_29_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_29", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_29);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_30_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_30", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_30);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_31_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_31", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_31);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_32_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_32", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_32);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_33_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_33", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_33);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_34_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_34", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_34);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_35_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_35", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_35);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_36_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_36", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_36);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_37_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_37", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_37);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_38_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_38", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_38);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_39_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_39", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_39);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_40_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_40", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_40);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_41_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_41", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_41);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_42_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_42", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_42);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_43_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_43", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_43);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_44_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_44", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_44);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_45_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_45", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_45);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_46_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_46", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_46);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_47_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_47", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_47);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_48_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_48", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_48);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_49_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_49", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_49);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_50_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_50", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_50);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_51_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_51", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_51);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_52_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_52", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_52);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_53_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_53", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_53);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_54_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_54", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_54);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_55_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_55", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_55);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_56_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_56", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_56);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_57_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_57", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_57);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_58_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_58", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_58);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_59_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_59", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_59);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_60_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_60", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_60);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_61_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_61", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_61);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_62_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_62", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_62);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_63_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_63", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_63);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_64_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_64", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_64);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_65_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_65", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_65);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_66_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_66", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_66);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_67_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_67", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_67);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_68_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_68", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_68);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_69_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_69", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_69);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_70_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_70", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_70);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_71_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_71", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_71);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_72_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_72", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_72);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_73_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_73", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_73);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_74_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_74", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_74);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_75_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_75", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_75);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_76_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_76", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_76);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_77_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_77", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_77);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_78_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_78", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_78);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_79_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_79", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_79);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_80_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_80", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_80);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_81_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_81", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_81);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_82_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_82", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_82);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_83_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_83", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_83);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_84_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_84", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_84);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_85_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_85", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_85);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_86_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_86", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_86);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_87_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_87", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_87);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_88_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_88", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_88);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_89_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_89", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_89);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_90_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_90", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_90);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_91_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_91", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_91);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_92_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_92", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_92);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_93_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_93", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_93);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_94_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_94", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_94);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_95_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_95", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_95);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_96_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_96", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_96);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_97_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_97", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_97);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_98_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_98", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_98);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_99_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_99", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_99);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_100_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_100", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_100);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_101_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_101", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_101);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_102_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_102", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_102);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_103_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_103", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_103);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_104_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_104", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_104);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_105_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_105", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_105);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_106_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_106", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_106);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_107_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_107", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_107);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_108_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_108", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_108);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_109_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_109", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_109);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_110_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_110", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_110);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_111_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_111", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_111);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_112_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_112", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_112);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_113_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_113", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_113);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_114_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_114", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_114);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_115_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_115", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_115);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_116_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_116", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_116);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_117_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_117", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_117);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_118_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_118", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_118);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_119_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_119", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_119);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_120_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_120", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_120);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_121_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_121", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_121);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_122_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_122", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_122);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_123_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_123", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_123);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_124_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_124", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_124);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_125_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_125", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_125);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_126_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_126", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_126);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_127_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_127", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_127);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_128_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_128", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_128);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_129_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_129", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_129);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_130_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_130", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_130);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_131_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_131", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_131);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_132_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_132", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_132);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_133_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_133", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_133);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_134_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_134", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_134);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_135_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_135", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_135);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_136_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_136", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_136);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_137_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_137", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_137);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_138_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_138", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_138);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_139_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_139", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_139);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_140_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_140", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_140);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_141_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_141", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_141);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_142_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_142", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_142);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_143_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_143", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_143);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_144_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_144", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_144);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_145_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_145", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_145);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_146_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_146", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_146);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_147_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_147", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_147);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_148_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_148", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_148);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_149_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_149", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_149);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_150_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_150", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_150);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_151_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_151", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_151);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_152_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_152", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_152);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_153_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_153", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_153);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_154_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_154", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_154);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_155_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_155", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_155);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_156_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_156", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_156);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_157_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_157", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_157);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_158_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_158", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_158);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_159_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_159", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_159);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_160_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_160", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_160);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_161_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_161", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_161);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_162_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_162", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_162);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_163_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_163", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_163);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_164_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_164", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_164);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_165_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_165", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_165);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_166_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_166", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_166);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_167_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_167", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_167);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_168_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_168", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_168);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_169_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_169", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_169);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_170_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_170", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_170);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_171_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_171", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_171);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_172_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_172", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_172);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_173_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_173", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_173);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_174_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_174", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_174);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_175_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_175", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_175);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_176_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_176", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_176);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_177_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_177", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_177);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_178_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_178", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_178);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_179_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_179", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_179);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_180_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_180", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_180);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_181_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_181", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_181);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_182_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_182", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_182);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_183_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_183", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_183);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_184_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_184", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_184);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_185_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_185", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_185);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_186_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_186", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_186);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_187_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_187", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_187);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_188_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_188", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_188);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_189_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_189", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_189);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_190_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_190", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_190);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_191_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_191", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_191);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_192_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_192", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_192);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_193_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_193", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_193);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_194_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_194", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_194);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_195_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_195", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_195);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_196_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_196", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_196);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_197_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_197", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_197);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_198_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_198", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_198);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_199_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_199", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_199);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_200_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_200", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_200);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_201_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_201", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_201);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_202_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_202", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_202);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_203_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_203", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_203);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_204_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_204", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_204);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_205_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_205", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_205);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_206_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_206", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_206);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_207_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_207", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_207);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_208_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_208", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_208);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_209_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_209", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_209);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_210_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_210", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_210);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_211_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_211", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_211);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_212_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_212", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_212);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_213_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_213", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_213);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_214_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_214", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_214);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_215_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_215", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_215);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_216_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_216", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_216);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_217_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_217", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_217);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_218_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_218", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_218);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_219_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_219", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_219);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_220_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_220", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_220);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_221_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_221", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_221);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_222_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_222", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_222);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_223_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_223", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_223);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_224_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_224", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_224);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_225_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_225", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_225);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_226_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_226", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_226);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_227_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_227", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_227);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_228_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_228", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_228);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_229_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_229", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_229);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_230_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_230", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_230);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_231_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_231", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_231);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_232_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_232", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_232);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_233_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_233", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_233);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_234_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_234", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_234);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_235_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_235", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_235);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_236_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_236", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_236);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_237_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_237", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_237);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_238_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_238", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_238);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_239_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_239", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_239);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_240_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_240", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_240);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_241_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_241", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_241);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_242_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_242", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_242);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_243_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_243", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_243);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_244_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_244", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_244);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_245_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_245", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_245);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_246_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_246", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_246);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_247_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_247", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_247);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_248_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_248", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_248);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_249_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_249", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_249);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_250_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_250", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_250);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_251_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_251", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_251);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_252_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_252", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_252);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_253_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_253", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_253);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_254_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_254", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_254);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_255_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_255", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_255);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_256_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_256", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_256);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_257_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_257", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_257);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_258_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_258", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_258);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_259_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_259", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_259);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_260_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_260", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_260);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_261_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_261", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_261);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_262_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_262", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_262);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_263_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_263", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_263);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_264_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_264", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_264);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_265_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_265", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_265);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_266_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_266", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_266);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_267_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_267", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_267);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_268_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_268", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_268);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_269_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_269", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_269);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_270_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_270", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_270);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_271_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_271", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_271);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_272_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_272", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_272);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_273_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_273", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_273);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_274_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_274", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_274);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_275_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_275", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_275);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_276_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_276", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_276);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_277_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_277", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_277);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_278_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_278", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_278);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_279_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_279", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_279);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_280_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_280", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_280);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_281_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_281", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_281);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_282_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_282", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_282);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_283_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_283", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_283);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_284_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_284", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_284);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_285_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_285", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_285);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_286_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_286", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_286);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_287_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_287", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_287);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_288_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_288", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_288);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_289_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_289", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_289);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_290_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_290", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_290);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_291_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_291", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_291);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_292_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_292", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_292);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_293_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_293", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_293);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_294_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_294", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_294);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_295_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_295", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_295);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_296_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_296", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_296);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_297_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_297", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_297);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_298_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_298", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_298);
}

uint32
dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_299_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "aod_299", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_aod_sizes, dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD, dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_299);
}







shr_error_e
dnx_data_dnx2_aod_sizes_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "dnx2_aod_sizes";
    module_data->nof_submodules = _dnx_data_dnx2_aod_sizes_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data dnx2_aod_sizes submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_dnx2_aod_sizes_AOD_init(unit, &module_data->submodules[dnx_data_dnx2_aod_sizes_submodule_AOD]));
    SHR_IF_ERR_EXIT(dnx_data_dnx2_aod_sizes_dynamic_AOD_init(unit, &module_data->submodules[dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD]));
    

    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_dnx2_aod_sizes_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_dnx2_aod_sizes_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_dnx2_aod_sizes_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_dnx2_aod_sizes_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_dnx2_aod_sizes_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_dnx2_aod_sizes_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_dnx2_aod_sizes_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_dnx2_aod_sizes_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_dnx2_aod_sizes_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_dnx2_aod_sizes_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_dnx2_aod_sizes_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_dnx2_aod_sizes_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_dnx2_aod_sizes_attach(unit));
        SHR_IF_ERR_EXIT(q2a_b0_data_dnx2_aod_sizes_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_dnx2_aod_sizes_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_dnx2_aod_sizes_attach(unit));
        SHR_IF_ERR_EXIT(q2a_b0_data_dnx2_aod_sizes_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_dnx2_aod_sizes_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_dnx2_aod_sizes_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_dnx2_aod_sizes_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_dnx2_aod_sizes_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a2(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_dnx2_aod_sizes_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_dnx2_aod_sizes_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2x_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_dnx2_aod_sizes_attach(unit));
        SHR_IF_ERR_EXIT(j2x_a0_data_dnx2_aod_sizes_attach(unit));
    }
    else

    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

