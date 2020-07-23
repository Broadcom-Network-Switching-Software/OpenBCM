

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_SRV6

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_srv6.h>



extern shr_error_e jr2_a0_data_srv6_attach(
    int unit);
extern shr_error_e q2a_b0_data_srv6_attach(
    int unit);
extern shr_error_e j2p_a0_data_srv6_attach(
    int unit);



static shr_error_e
dnx_data_srv6_termination_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "termination";
    submodule_data->doc = "SRv6 tunnel termination in Egress node";
    
    submodule_data->nof_features = _dnx_data_srv6_termination_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data srv6 termination features");

    
    submodule_data->nof_defines = _dnx_data_srv6_termination_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data srv6 termination defines");

    submodule_data->defines[dnx_data_srv6_termination_define_min_nof_terminated_sids_psp].name = "min_nof_terminated_sids_psp";
    submodule_data->defines[dnx_data_srv6_termination_define_min_nof_terminated_sids_psp].doc = "Min number of SIDs that can be termianted in SRv6 PSP Egress node";
    
    submodule_data->defines[dnx_data_srv6_termination_define_min_nof_terminated_sids_psp].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_srv6_termination_define_max_nof_terminated_sids_psp].name = "max_nof_terminated_sids_psp";
    submodule_data->defines[dnx_data_srv6_termination_define_max_nof_terminated_sids_psp].doc = "Max number of SIDs that can be termianted in SRv6 PSP Egress node";
    
    submodule_data->defines[dnx_data_srv6_termination_define_max_nof_terminated_sids_psp].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_srv6_termination_define_min_nof_terminated_sids_usp].name = "min_nof_terminated_sids_usp";
    submodule_data->defines[dnx_data_srv6_termination_define_min_nof_terminated_sids_usp].doc = "Min number of SIDs that can be termianted in SRv6 USP Egress node";
    
    submodule_data->defines[dnx_data_srv6_termination_define_min_nof_terminated_sids_usp].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_srv6_termination_define_max_nof_terminated_sids_usp].name = "max_nof_terminated_sids_usp";
    submodule_data->defines[dnx_data_srv6_termination_define_max_nof_terminated_sids_usp].doc = "Max number of SIDs that can be termianted in SRv6 USP Egress node";
    
    submodule_data->defines[dnx_data_srv6_termination_define_max_nof_terminated_sids_usp].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_srv6_termination_define_max_nof_terminated_sids_usp_1pass].name = "max_nof_terminated_sids_usp_1pass";
    submodule_data->defines[dnx_data_srv6_termination_define_max_nof_terminated_sids_usp_1pass].doc = "Max number of SIDs that can be termianted in SRv6 USP Egress node in 1pass";
    
    submodule_data->defines[dnx_data_srv6_termination_define_max_nof_terminated_sids_usp_1pass].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_srv6_termination_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data srv6 termination tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_srv6_termination_feature_get(
    int unit,
    dnx_data_srv6_termination_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_srv6, dnx_data_srv6_submodule_termination, feature);
}


uint32
dnx_data_srv6_termination_min_nof_terminated_sids_psp_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_srv6, dnx_data_srv6_submodule_termination, dnx_data_srv6_termination_define_min_nof_terminated_sids_psp);
}

uint32
dnx_data_srv6_termination_max_nof_terminated_sids_psp_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_srv6, dnx_data_srv6_submodule_termination, dnx_data_srv6_termination_define_max_nof_terminated_sids_psp);
}

uint32
dnx_data_srv6_termination_min_nof_terminated_sids_usp_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_srv6, dnx_data_srv6_submodule_termination, dnx_data_srv6_termination_define_min_nof_terminated_sids_usp);
}

uint32
dnx_data_srv6_termination_max_nof_terminated_sids_usp_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_srv6, dnx_data_srv6_submodule_termination, dnx_data_srv6_termination_define_max_nof_terminated_sids_usp);
}

uint32
dnx_data_srv6_termination_max_nof_terminated_sids_usp_1pass_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_srv6, dnx_data_srv6_submodule_termination, dnx_data_srv6_termination_define_max_nof_terminated_sids_usp_1pass);
}










static shr_error_e
dnx_data_srv6_encapsulation_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "encapsulation";
    submodule_data->doc = "SRv6 tunnel encapsulation in Ingress node";
    
    submodule_data->nof_features = _dnx_data_srv6_encapsulation_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data srv6 encapsulation features");

    
    submodule_data->nof_defines = _dnx_data_srv6_encapsulation_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data srv6 encapsulation defines");

    submodule_data->defines[dnx_data_srv6_encapsulation_define_max_nof_encap_sids_main_pass].name = "max_nof_encap_sids_main_pass";
    submodule_data->defines[dnx_data_srv6_encapsulation_define_max_nof_encap_sids_main_pass].doc = "Max number of SIDs that can be Encapsulated in SRV6 tunnel, when building main headers (SRH, and IPv6 tunnel)";
    
    submodule_data->defines[dnx_data_srv6_encapsulation_define_max_nof_encap_sids_main_pass].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_srv6_encapsulation_define_nof_encap_sids_extended_pass].name = "nof_encap_sids_extended_pass";
    submodule_data->defines[dnx_data_srv6_encapsulation_define_nof_encap_sids_extended_pass].doc = "Number of SIDs that can be Encapsulated in SRV6 tunnel, extended pass - only SIDs are encapsulated";
    
    submodule_data->defines[dnx_data_srv6_encapsulation_define_nof_encap_sids_extended_pass].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_srv6_encapsulation_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data srv6 encapsulation tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_srv6_encapsulation_feature_get(
    int unit,
    dnx_data_srv6_encapsulation_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_srv6, dnx_data_srv6_submodule_encapsulation, feature);
}


uint32
dnx_data_srv6_encapsulation_max_nof_encap_sids_main_pass_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_srv6, dnx_data_srv6_submodule_encapsulation, dnx_data_srv6_encapsulation_define_max_nof_encap_sids_main_pass);
}

uint32
dnx_data_srv6_encapsulation_nof_encap_sids_extended_pass_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_srv6, dnx_data_srv6_submodule_encapsulation, dnx_data_srv6_encapsulation_define_nof_encap_sids_extended_pass);
}







shr_error_e
dnx_data_srv6_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "srv6";
    module_data->nof_submodules = _dnx_data_srv6_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data srv6 submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_srv6_termination_init(unit, &module_data->submodules[dnx_data_srv6_submodule_termination]));
    SHR_IF_ERR_EXIT(dnx_data_srv6_encapsulation_init(unit, &module_data->submodules[dnx_data_srv6_submodule_encapsulation]));
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_srv6_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_srv6_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_srv6_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_srv6_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_srv6_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_srv6_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_srv6_attach(unit));
        SHR_IF_ERR_EXIT(q2a_b0_data_srv6_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_srv6_attach(unit));
        SHR_IF_ERR_EXIT(q2a_b0_data_srv6_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_srv6_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_srv6_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

