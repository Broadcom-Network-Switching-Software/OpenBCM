

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



extern shr_error_e jr2_a0_data_bfd_attach(
    int unit);
extern shr_error_e jr2_b0_data_bfd_attach(
    int unit);
extern shr_error_e j2c_a0_data_bfd_attach(
    int unit);
extern shr_error_e q2a_a0_data_bfd_attach(
    int unit);
extern shr_error_e j2p_a0_data_bfd_attach(
    int unit);



static shr_error_e
dnx_data_bfd_general_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "general";
    submodule_data->doc = "general bfd constants";
    
    submodule_data->nof_features = _dnx_data_bfd_general_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data bfd general features");

    submodule_data->features[dnx_data_bfd_general_configurable_single_hop_tos].name = "configurable_single_hop_tos";
    submodule_data->features[dnx_data_bfd_general_configurable_single_hop_tos].doc = "";
    submodule_data->features[dnx_data_bfd_general_configurable_single_hop_tos].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_bfd_general_extended_sip_support].name = "extended_sip_support";
    submodule_data->features[dnx_data_bfd_general_extended_sip_support].doc = "HW limitation: Extended  BFD SIP is not supported on  - JR2_A0 only!  Works for B0 and up";
    submodule_data->features[dnx_data_bfd_general_extended_sip_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_bfd_general_vxlan_support].name = "vxlan_support";
    submodule_data->features[dnx_data_bfd_general_vxlan_support].doc = "HW limitation: BFD over VXLAN is not supported on  - JR2_A0 only!  Works for B0 and up";
    submodule_data->features[dnx_data_bfd_general_vxlan_support].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_bfd_general_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data bfd general defines");

    submodule_data->defines[dnx_data_bfd_general_define_nof_bits_bfd_endpoints].name = "nof_bits_bfd_endpoints";
    submodule_data->defines[dnx_data_bfd_general_define_nof_bits_bfd_endpoints].doc = "Number of bits needed for BFD endpoints";
    
    submodule_data->defines[dnx_data_bfd_general_define_nof_bits_bfd_endpoints].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_bfd_general_define_nof_endpoints].name = "nof_endpoints";
    submodule_data->defines[dnx_data_bfd_general_define_nof_endpoints].doc = "Maximum number of BFD endpoints";
    
    submodule_data->defines[dnx_data_bfd_general_define_nof_endpoints].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_bfd_general_define_nof_dips_for_bfd_multihop].name = "nof_dips_for_bfd_multihop";
    submodule_data->defines[dnx_data_bfd_general_define_nof_dips_for_bfd_multihop].doc = "Number of usable BFD Destination IPs for multi-hop";
    
    submodule_data->defines[dnx_data_bfd_general_define_nof_dips_for_bfd_multihop].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_bfd_general_define_nof_bits_dips_for_bfd_multihop].name = "nof_bits_dips_for_bfd_multihop";
    submodule_data->defines[dnx_data_bfd_general_define_nof_bits_dips_for_bfd_multihop].doc = "Number of bits needed for BFD Destination IPs";
    
    submodule_data->defines[dnx_data_bfd_general_define_nof_bits_dips_for_bfd_multihop].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_bfd_general_define_nof_bfd_server_trap_codes].name = "nof_bfd_server_trap_codes";
    submodule_data->defines[dnx_data_bfd_general_define_nof_bfd_server_trap_codes].doc = "Number of usable BFD server trap codes";
    
    submodule_data->defines[dnx_data_bfd_general_define_nof_bfd_server_trap_codes].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_bfd_general_define_nof_profiles_for_tx_rate].name = "nof_profiles_for_tx_rate";
    submodule_data->defines[dnx_data_bfd_general_define_nof_profiles_for_tx_rate].doc = "Number of profiles in TX_RATE register";
    
    submodule_data->defines[dnx_data_bfd_general_define_nof_profiles_for_tx_rate].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_bfd_general_define_nof_bits_bfd_server_trap_codes].name = "nof_bits_bfd_server_trap_codes";
    submodule_data->defines[dnx_data_bfd_general_define_nof_bits_bfd_server_trap_codes].doc = "Number of bit needed for BFD server trap codes";
    
    submodule_data->defines[dnx_data_bfd_general_define_nof_bits_bfd_server_trap_codes].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_bfd_general_define_rmep_index_db_ipv4_const].name = "rmep_index_db_ipv4_const";
    submodule_data->defines[dnx_data_bfd_general_define_rmep_index_db_ipv4_const].doc = "RMEP index DB constant for IPv4 MEPs";
    
    submodule_data->defines[dnx_data_bfd_general_define_rmep_index_db_ipv4_const].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_bfd_general_define_rmep_index_db_mpls_const].name = "rmep_index_db_mpls_const";
    submodule_data->defines[dnx_data_bfd_general_define_rmep_index_db_mpls_const].doc = "RMEP index DB constant for MEPs over MPLS";
    
    submodule_data->defines[dnx_data_bfd_general_define_rmep_index_db_mpls_const].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_bfd_general_define_rmep_index_db_pwe_const].name = "rmep_index_db_pwe_const";
    submodule_data->defines[dnx_data_bfd_general_define_rmep_index_db_pwe_const].doc = "RMEP index DB constant for MEPs over PWE";
    
    submodule_data->defines[dnx_data_bfd_general_define_rmep_index_db_pwe_const].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_bfd_general_define_nof_req_int_profiles].name = "nof_req_int_profiles";
    submodule_data->defines[dnx_data_bfd_general_define_nof_req_int_profiles].doc = "Number of entries in required interrupt table";
    
    submodule_data->defines[dnx_data_bfd_general_define_nof_req_int_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_bfd_general_define_nof_bits_your_discr_to_lif].name = "nof_bits_your_discr_to_lif";
    submodule_data->defines[dnx_data_bfd_general_define_nof_bits_your_discr_to_lif].doc = "Number of bits from your discriminator to oam_lif";
    
    submodule_data->defines[dnx_data_bfd_general_define_nof_bits_your_discr_to_lif].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_bfd_general_define_oamp_bfd_cw_valid_dis_supp].name = "oamp_bfd_cw_valid_dis_supp";
    submodule_data->defines[dnx_data_bfd_general_define_oamp_bfd_cw_valid_dis_supp].doc = "Control word validation can be disabled in OAMP";
    
    submodule_data->defines[dnx_data_bfd_general_define_oamp_bfd_cw_valid_dis_supp].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_bfd_general_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data bfd general tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_bfd_general_feature_get(
    int unit,
    dnx_data_bfd_general_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_bfd, dnx_data_bfd_submodule_general, feature);
}


uint32
dnx_data_bfd_general_nof_bits_bfd_endpoints_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_bfd, dnx_data_bfd_submodule_general, dnx_data_bfd_general_define_nof_bits_bfd_endpoints);
}

uint32
dnx_data_bfd_general_nof_endpoints_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_bfd, dnx_data_bfd_submodule_general, dnx_data_bfd_general_define_nof_endpoints);
}

uint32
dnx_data_bfd_general_nof_dips_for_bfd_multihop_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_bfd, dnx_data_bfd_submodule_general, dnx_data_bfd_general_define_nof_dips_for_bfd_multihop);
}

uint32
dnx_data_bfd_general_nof_bits_dips_for_bfd_multihop_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_bfd, dnx_data_bfd_submodule_general, dnx_data_bfd_general_define_nof_bits_dips_for_bfd_multihop);
}

uint32
dnx_data_bfd_general_nof_bfd_server_trap_codes_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_bfd, dnx_data_bfd_submodule_general, dnx_data_bfd_general_define_nof_bfd_server_trap_codes);
}

uint32
dnx_data_bfd_general_nof_profiles_for_tx_rate_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_bfd, dnx_data_bfd_submodule_general, dnx_data_bfd_general_define_nof_profiles_for_tx_rate);
}

uint32
dnx_data_bfd_general_nof_bits_bfd_server_trap_codes_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_bfd, dnx_data_bfd_submodule_general, dnx_data_bfd_general_define_nof_bits_bfd_server_trap_codes);
}

uint32
dnx_data_bfd_general_rmep_index_db_ipv4_const_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_bfd, dnx_data_bfd_submodule_general, dnx_data_bfd_general_define_rmep_index_db_ipv4_const);
}

uint32
dnx_data_bfd_general_rmep_index_db_mpls_const_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_bfd, dnx_data_bfd_submodule_general, dnx_data_bfd_general_define_rmep_index_db_mpls_const);
}

uint32
dnx_data_bfd_general_rmep_index_db_pwe_const_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_bfd, dnx_data_bfd_submodule_general, dnx_data_bfd_general_define_rmep_index_db_pwe_const);
}

uint32
dnx_data_bfd_general_nof_req_int_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_bfd, dnx_data_bfd_submodule_general, dnx_data_bfd_general_define_nof_req_int_profiles);
}

uint32
dnx_data_bfd_general_nof_bits_your_discr_to_lif_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_bfd, dnx_data_bfd_submodule_general, dnx_data_bfd_general_define_nof_bits_your_discr_to_lif);
}

uint32
dnx_data_bfd_general_oamp_bfd_cw_valid_dis_supp_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_bfd, dnx_data_bfd_submodule_general, dnx_data_bfd_general_define_oamp_bfd_cw_valid_dis_supp);
}










static shr_error_e
dnx_data_bfd_feature_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "feature";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_bfd_feature_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data bfd feature features");

    submodule_data->features[dnx_data_bfd_feature_detect_multiplier_limitation].name = "detect_multiplier_limitation";
    submodule_data->features[dnx_data_bfd_feature_detect_multiplier_limitation].doc = "";
    submodule_data->features[dnx_data_bfd_feature_detect_multiplier_limitation].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_bfd_feature_ptch_oam_offset_limitation].name = "ptch_oam_offset_limitation";
    submodule_data->features[dnx_data_bfd_feature_ptch_oam_offset_limitation].doc = "";
    submodule_data->features[dnx_data_bfd_feature_ptch_oam_offset_limitation].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_bfd_feature_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data bfd feature defines");

    
    submodule_data->nof_tables = _dnx_data_bfd_feature_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data bfd feature tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_bfd_feature_feature_get(
    int unit,
    dnx_data_bfd_feature_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_bfd, dnx_data_bfd_submodule_feature, feature);
}








shr_error_e
dnx_data_bfd_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "bfd";
    module_data->nof_submodules = _dnx_data_bfd_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data bfd submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_bfd_general_init(unit, &module_data->submodules[dnx_data_bfd_submodule_general]));
    SHR_IF_ERR_EXIT(dnx_data_bfd_feature_init(unit, &module_data->submodules[dnx_data_bfd_submodule_feature]));
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_bfd_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_bfd_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_bfd_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_bfd_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_bfd_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_bfd_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_bfd_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_bfd_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_bfd_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_bfd_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_bfd_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_bfd_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_bfd_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_bfd_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_bfd_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_bfd_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_bfd_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

