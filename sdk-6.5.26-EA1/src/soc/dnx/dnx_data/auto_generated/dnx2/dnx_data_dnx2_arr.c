
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

#include <soc/dnx/dnx_data/auto_generated/dnx2/dnx_data_internal_dnx2_arr.h>



#ifdef BCM_DNX2_SUPPORT

extern shr_error_e jr2_a0_data_dnx2_arr_attach(
    int unit);

#endif 
#ifdef BCM_DNX2_SUPPORT

extern shr_error_e jr2_b0_data_dnx2_arr_attach(
    int unit);

#endif 
#ifdef BCM_DNX2_SUPPORT

extern shr_error_e j2c_a0_data_dnx2_arr_attach(
    int unit);

#endif 
#ifdef BCM_DNX2_SUPPORT

extern shr_error_e q2a_a0_data_dnx2_arr_attach(
    int unit);

#endif 
#ifdef BCM_DNX2_SUPPORT

extern shr_error_e q2a_b0_data_dnx2_arr_attach(
    int unit);

#endif 
#ifdef BCM_DNX2_SUPPORT

extern shr_error_e j2p_a0_data_dnx2_arr_attach(
    int unit);

#endif 
#ifdef BCM_DNX2_SUPPORT

extern shr_error_e j2x_a0_data_dnx2_arr_attach(
    int unit);

#endif 



static shr_error_e
dnx_data_dnx2_arr_prefix_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "prefix";
    submodule_data->doc = "ARR prefix table values";
    
    submodule_data->nof_features = _dnx_data_dnx2_arr_prefix_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data dnx2_arr prefix features");

    
    submodule_data->nof_defines = _dnx_data_dnx2_arr_prefix_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data dnx2_arr prefix defines");

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_Empty].name = "ETPP_ETPS_FORMATS___prefix_Empty";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_Empty].doc = "prefix of format ETPP_ETPS_FORMATS at index 0";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_Empty].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_FORWARD_DOMAIN_VRF_Encoding].name = "ETPP_ETPS_FORMATS___prefix_FORWARD_DOMAIN_VRF_Encoding";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_FORWARD_DOMAIN_VRF_Encoding].doc = "prefix of format ETPP_ETPS_FORMATS at index 1";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_FORWARD_DOMAIN_VRF_Encoding].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_ENCAP_TTL_IDX_MPLS_FHEI].name = "ETPP_ETPS_FORMATS___prefix_ENCAP_TTL_IDX_MPLS_FHEI";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_ENCAP_TTL_IDX_MPLS_FHEI].doc = "prefix of format ETPP_ETPS_FORMATS at index 2";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_ENCAP_TTL_IDX_MPLS_FHEI].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_ESEM_COMMAND_IPvX_TUNNEL_ESEM_Command].name = "ETPP_ETPS_FORMATS___prefix_ESEM_COMMAND_IPvX_TUNNEL_ESEM_Command";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_ESEM_COMMAND_IPvX_TUNNEL_ESEM_Command].doc = "prefix of format ETPP_ETPS_FORMATS at index 3";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_ESEM_COMMAND_IPvX_TUNNEL_ESEM_Command].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_Always_Last_Layer].name = "ETPP_ETPS_FORMATS___prefix_Always_Last_Layer";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_Always_Last_Layer].doc = "prefix of format ETPP_ETPS_FORMATS at index 4";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_Always_Last_Layer].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_LIF_AHP_MPLS_Prefix].name = "ETPP_ETPS_FORMATS___prefix_LIF_AHP_MPLS_Prefix";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_LIF_AHP_MPLS_Prefix].doc = "prefix of format ETPP_ETPS_FORMATS at index 5";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_LIF_AHP_MPLS_Prefix].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_ENCAP_QOS_MODEL_MPLS_FHEI].name = "ETPP_ETPS_FORMATS___prefix_ENCAP_QOS_MODEL_MPLS_FHEI";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_ENCAP_QOS_MODEL_MPLS_FHEI].doc = "prefix of format ETPP_ETPS_FORMATS at index 6";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_ENCAP_QOS_MODEL_MPLS_FHEI].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_ENCAP_QOS_IDX_MPLS_FHEI].name = "ETPP_ETPS_FORMATS___prefix_ENCAP_QOS_IDX_MPLS_FHEI";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_ENCAP_QOS_IDX_MPLS_FHEI].doc = "prefix of format ETPP_ETPS_FORMATS at index 7";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_ENCAP_QOS_IDX_MPLS_FHEI].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_OutLIF_Profile_Orientation_Only].name = "ETPP_ETPS_FORMATS___prefix_OutLIF_Profile_Orientation_Only";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_OutLIF_Profile_Orientation_Only].doc = "prefix of format ETPP_ETPS_FORMATS at index 8";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_OutLIF_Profile_Orientation_Only].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_FORWARD_DOMAIN_VRF_IPvX_Tunnel].name = "ETPP_ETPS_FORMATS___prefix_FORWARD_DOMAIN_VRF_IPvX_Tunnel";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_FORWARD_DOMAIN_VRF_IPvX_Tunnel].doc = "prefix of format ETPP_ETPS_FORMATS at index 9";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_FORWARD_DOMAIN_VRF_IPvX_Tunnel].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_QOS_MODEL_1].name = "ETPP_ETPS_FORMATS___prefix_QOS_MODEL_1";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_QOS_MODEL_1].doc = "prefix of format ETPP_ETPS_FORMATS at index 10";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_QOS_MODEL_1].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_ESEM_COMMAND_IPv6_TUNNELS_FODO_Namespace].name = "ETPP_ETPS_FORMATS___prefix_ESEM_COMMAND_IPv6_TUNNELS_FODO_Namespace";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_ESEM_COMMAND_IPv6_TUNNELS_FODO_Namespace].doc = "prefix of format ETPP_ETPS_FORMATS at index 11";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_ESEM_COMMAND_IPv6_TUNNELS_FODO_Namespace].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_ENCAP_TTL_MODEL_MPLS_FHEI].name = "ETPP_ETPS_FORMATS___prefix_ENCAP_TTL_MODEL_MPLS_FHEI";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_ENCAP_TTL_MODEL_MPLS_FHEI].doc = "prefix of format ETPP_ETPS_FORMATS at index 12";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_ENCAP_TTL_MODEL_MPLS_FHEI].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_ESEM_COMMAND_IPv4_VxLAN_OR_VXLAN_GPE_FODO_Namespace].name = "ETPP_ETPS_FORMATS___prefix_ESEM_COMMAND_IPv4_VxLAN_OR_VXLAN_GPE_FODO_Namespace";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_ESEM_COMMAND_IPv4_VxLAN_OR_VXLAN_GPE_FODO_Namespace].doc = "prefix of format ETPP_ETPS_FORMATS at index 13";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_ESEM_COMMAND_IPv4_VxLAN_OR_VXLAN_GPE_FODO_Namespace].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_FORWARD_QOS_MODEL_NWK_QOS].name = "ETPP_ETPS_FORMATS___prefix_FORWARD_QOS_MODEL_NWK_QOS";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_FORWARD_QOS_MODEL_NWK_QOS].doc = "prefix of format ETPP_ETPS_FORMATS at index 14";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_FORWARD_QOS_MODEL_NWK_QOS].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_ESEM_COMMAND_PWE].name = "ETPP_ETPS_FORMATS___prefix_ESEM_COMMAND_PWE";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_ESEM_COMMAND_PWE].doc = "prefix of format ETPP_ETPS_FORMATS at index 15";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_ESEM_COMMAND_PWE].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_FEC_ENTRY_FORMAT___prefix_Empty].name = "IRPP_FEC_ENTRY_FORMAT___prefix_Empty";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_FEC_ENTRY_FORMAT___prefix_Empty].doc = "prefix of format IRPP_FEC_ENTRY_FORMAT at index 0";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_FEC_ENTRY_FORMAT___prefix_Empty].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_FEC_ENTRY_FORMAT___prefix_EEI_W_OUTLIF].name = "IRPP_FEC_ENTRY_FORMAT___prefix_EEI_W_OUTLIF";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_FEC_ENTRY_FORMAT___prefix_EEI_W_OUTLIF].doc = "prefix of format IRPP_FEC_ENTRY_FORMAT at index 2";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_FEC_ENTRY_FORMAT___prefix_EEI_W_OUTLIF].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_FEC_ENTRY_FORMAT___prefix_OUTLIF_ETH_RIF].name = "IRPP_FEC_ENTRY_FORMAT___prefix_OUTLIF_ETH_RIF";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_FEC_ENTRY_FORMAT___prefix_OUTLIF_ETH_RIF].doc = "prefix of format IRPP_FEC_ENTRY_FORMAT at index 3";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_FEC_ENTRY_FORMAT___prefix_OUTLIF_ETH_RIF].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_FEC_ENTRY_FORMAT___prefix_EEI_W_POP_Command].name = "IRPP_FEC_ENTRY_FORMAT___prefix_EEI_W_POP_Command";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_FEC_ENTRY_FORMAT___prefix_EEI_W_POP_Command].doc = "prefix of format IRPP_FEC_ENTRY_FORMAT at index 4";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_FEC_ENTRY_FORMAT___prefix_EEI_W_POP_Command].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_FEC_ENTRY_FORMAT___prefix_EEI_W_SWAP_Command].name = "IRPP_FEC_ENTRY_FORMAT___prefix_EEI_W_SWAP_Command";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_FEC_ENTRY_FORMAT___prefix_EEI_W_SWAP_Command].doc = "prefix of format IRPP_FEC_ENTRY_FORMAT at index 5";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_FEC_ENTRY_FORMAT___prefix_EEI_W_SWAP_Command].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_FEC_ENTRY_FORMAT___prefix_MC_RPF_MODE_MC_RPF_MODE_EXPLICIT].name = "IRPP_FEC_ENTRY_FORMAT___prefix_MC_RPF_MODE_MC_RPF_MODE_EXPLICIT";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_FEC_ENTRY_FORMAT___prefix_MC_RPF_MODE_MC_RPF_MODE_EXPLICIT].doc = "prefix of format IRPP_FEC_ENTRY_FORMAT at index 6";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_FEC_ENTRY_FORMAT___prefix_MC_RPF_MODE_MC_RPF_MODE_EXPLICIT].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_FEC_SUPER_ENTRY_FORMAT___prefix_Empty].name = "IRPP_FEC_SUPER_ENTRY_FORMAT___prefix_Empty";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_FEC_SUPER_ENTRY_FORMAT___prefix_Empty].doc = "prefix of format IRPP_FEC_SUPER_ENTRY_FORMAT at index 0";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_FEC_SUPER_ENTRY_FORMAT___prefix_Empty].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_FWD_STAGE_FORMATS___prefix_Empty].name = "IRPP_FWD_STAGE_FORMATS___prefix_Empty";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_FWD_STAGE_FORMATS___prefix_Empty].doc = "prefix of format IRPP_FWD_STAGE_FORMATS at index 0";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_FWD_STAGE_FORMATS___prefix_Empty].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b11].name = "IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b11";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b11].doc = "prefix of format IRPP_FWD_STAGE_FORMATS at index 1";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b11].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b0001].name = "IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b0001";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b0001].doc = "prefix of format IRPP_FWD_STAGE_FORMATS at index 2";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b0001].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b0010].name = "IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b0010";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b0010].doc = "prefix of format IRPP_FWD_STAGE_FORMATS at index 3";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b0010].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b0011].name = "IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b0011";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b0011].doc = "prefix of format IRPP_FWD_STAGE_FORMATS at index 4";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b0011].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b0100].name = "IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b0100";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b0100].doc = "prefix of format IRPP_FWD_STAGE_FORMATS at index 5";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b0100].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b0101].name = "IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b0101";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b0101].doc = "prefix of format IRPP_FWD_STAGE_FORMATS at index 6";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b0101].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b01111].name = "IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b01111";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b01111].doc = "prefix of format IRPP_FWD_STAGE_FORMATS at index 7";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b01111].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_Empty].name = "IRPP_IN_LIF_FORMATS___prefix_Empty";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_Empty].doc = "prefix of format IRPP_IN_LIF_FORMATS at index 0";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_Empty].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_Service_Type_Extended_P2P_W_Learning_Prefix].name = "IRPP_IN_LIF_FORMATS___prefix_Service_Type_Extended_P2P_W_Learning_Prefix";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_Service_Type_Extended_P2P_W_Learning_Prefix].doc = "prefix of format IRPP_IN_LIF_FORMATS at index 1";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_Service_Type_Extended_P2P_W_Learning_Prefix].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_Parser_Context_Eth_B1].name = "IRPP_IN_LIF_FORMATS___prefix_Parser_Context_Eth_B1";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_Parser_Context_Eth_B1].doc = "prefix of format IRPP_IN_LIF_FORMATS at index 2";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_Parser_Context_Eth_B1].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_Parser_Context_IPvX_DEMUX_B1].name = "IRPP_IN_LIF_FORMATS___prefix_Parser_Context_IPvX_DEMUX_B1";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_Parser_Context_IPvX_DEMUX_B1].doc = "prefix of format IRPP_IN_LIF_FORMATS at index 3";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_Parser_Context_IPvX_DEMUX_B1].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_Service_Type_P2P_Prefix].name = "IRPP_IN_LIF_FORMATS___prefix_Service_Type_P2P_Prefix";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_Service_Type_P2P_Prefix].doc = "prefix of format IRPP_IN_LIF_FORMATS at index 4";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_Service_Type_P2P_Prefix].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_Free].name = "IRPP_IN_LIF_FORMATS___prefix_Free";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_Free].doc = "prefix of format IRPP_IN_LIF_FORMATS at index 5";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_Free].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_VRF_VSI_Not_Found].name = "IRPP_IN_LIF_FORMATS___prefix_VRF_VSI_Not_Found";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_VRF_VSI_Not_Found].doc = "prefix of format IRPP_IN_LIF_FORMATS at index 6";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_VRF_VSI_Not_Found].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_Forward_Domain_Assignment_Mode_LIF_or_VLAN].name = "IRPP_IN_LIF_FORMATS___prefix_Forward_Domain_Assignment_Mode_LIF_or_VLAN";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_Forward_Domain_Assignment_Mode_LIF_or_VLAN].doc = "prefix of format IRPP_IN_LIF_FORMATS at index 7";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_Forward_Domain_Assignment_Mode_LIF_or_VLAN].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_Forward_Domain_VRF].name = "IRPP_IN_LIF_FORMATS___prefix_Forward_Domain_VRF";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_Forward_Domain_VRF].doc = "prefix of format IRPP_IN_LIF_FORMATS at index 8";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_Forward_Domain_VRF].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_NEXT_LAYER_NETWORK_DOMAIN_TNIorVNI].name = "IRPP_IN_LIF_FORMATS___prefix_NEXT_LAYER_NETWORK_DOMAIN_TNIorVNI";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_NEXT_LAYER_NETWORK_DOMAIN_TNIorVNI].doc = "prefix of format IRPP_IN_LIF_FORMATS at index 10";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_NEXT_LAYER_NETWORK_DOMAIN_TNIorVNI].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_NEXT_LAYER_NETWORK_DOMAIN_PWE_WO_SD_TAGS].name = "IRPP_IN_LIF_FORMATS___prefix_NEXT_LAYER_NETWORK_DOMAIN_PWE_WO_SD_TAGS";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_NEXT_LAYER_NETWORK_DOMAIN_PWE_WO_SD_TAGS].doc = "prefix of format IRPP_IN_LIF_FORMATS at index 11";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_NEXT_LAYER_NETWORK_DOMAIN_PWE_WO_SD_TAGS].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF_OPTIMIZED].name = "IRPP_IN_LIF_FORMATS___prefix_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF_OPTIMIZED";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF_OPTIMIZED].doc = "prefix of format IRPP_IN_LIF_FORMATS at index 12";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF_OPTIMIZED].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_VSI_P2P_MSBS].name = "IRPP_IN_LIF_FORMATS___prefix_VSI_P2P_MSBS";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_VSI_P2P_MSBS].doc = "prefix of format IRPP_IN_LIF_FORMATS at index 13";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_VSI_P2P_MSBS].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_QoS_Profile_short_Prefix].name = "IRPP_IN_LIF_FORMATS___prefix_QoS_Profile_short_Prefix";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_QoS_Profile_short_Prefix].doc = "prefix of format IRPP_IN_LIF_FORMATS at index 14";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_QoS_Profile_short_Prefix].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_Learn_info_destination_port_encoding].name = "IRPP_IN_LIF_FORMATS___prefix_Learn_info_destination_port_encoding";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_Learn_info_destination_port_encoding].doc = "prefix of format IRPP_IN_LIF_FORMATS at index 15";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_Learn_info_destination_port_encoding].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_VSI_FORMATS___prefix_Empty].name = "IRPP_VSI_FORMATS___prefix_Empty";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_VSI_FORMATS___prefix_Empty].doc = "prefix of format IRPP_VSI_FORMATS at index 0";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_VSI_FORMATS___prefix_Empty].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_VSI_FORMATS___prefix_VSI_Profile_Prefix].name = "IRPP_VSI_FORMATS___prefix_VSI_Profile_Prefix";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_VSI_FORMATS___prefix_VSI_Profile_Prefix].doc = "prefix of format IRPP_VSI_FORMATS at index 1";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_VSI_FORMATS___prefix_VSI_Profile_Prefix].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_VSI_FORMATS___prefix_Dest_MC_ID_Prefix].name = "IRPP_VSI_FORMATS___prefix_Dest_MC_ID_Prefix";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_VSI_FORMATS___prefix_Dest_MC_ID_Prefix].doc = "prefix of format IRPP_VSI_FORMATS at index 2";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_VSI_FORMATS___prefix_Dest_MC_ID_Prefix].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_VSI_FORMATS___prefix_Dest_FEC_Port_Trap_Prefix].name = "IRPP_VSI_FORMATS___prefix_Dest_FEC_Port_Trap_Prefix";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_VSI_FORMATS___prefix_Dest_FEC_Port_Trap_Prefix].doc = "prefix of format IRPP_VSI_FORMATS at index 3";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_VSI_FORMATS___prefix_Dest_FEC_Port_Trap_Prefix].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_VSI_FORMATS___prefix_Topology_ID_Prefix].name = "IRPP_VSI_FORMATS___prefix_Topology_ID_Prefix";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_VSI_FORMATS___prefix_Topology_ID_Prefix].doc = "prefix of format IRPP_VSI_FORMATS at index 4";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_VSI_FORMATS___prefix_Topology_ID_Prefix].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_VSI_FORMATS___prefix_QOS_Profile_Prefix].name = "IRPP_VSI_FORMATS___prefix_QOS_Profile_Prefix";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_VSI_FORMATS___prefix_QOS_Profile_Prefix].doc = "prefix of format IRPP_VSI_FORMATS at index 5";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_VSI_FORMATS___prefix_QOS_Profile_Prefix].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_VSI_FORMATS___prefix_My_Mac_Prefix_Prefix].name = "IRPP_VSI_FORMATS___prefix_My_Mac_Prefix_Prefix";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_VSI_FORMATS___prefix_My_Mac_Prefix_Prefix].doc = "prefix of format IRPP_VSI_FORMATS at index 6";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_VSI_FORMATS___prefix_My_Mac_Prefix_Prefix].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_VSI_FORMATS___prefix_VSI_Stat_Obj_Cmd_Prefix].name = "IRPP_VSI_FORMATS___prefix_VSI_Stat_Obj_Cmd_Prefix";
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_VSI_FORMATS___prefix_VSI_Stat_Obj_Cmd_Prefix].doc = "prefix of format IRPP_VSI_FORMATS at index 7";
        
        submodule_data->defines[dnx_data_dnx2_arr_prefix_define_IRPP_VSI_FORMATS___prefix_VSI_Stat_Obj_Cmd_Prefix].flags |= DNXC_DATA_F_DEFINE;
    }

    
    submodule_data->nof_tables = _dnx_data_dnx2_arr_prefix_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data dnx2_arr prefix tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_dnx2_arr_prefix_feature_get(
    int unit,
    dnx_data_dnx2_arr_prefix_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, feature);
}


uint32
dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_Empty_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPP_ETPS_FORMATS___prefix_Empty", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_Empty);
}

uint32
dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_FORWARD_DOMAIN_VRF_Encoding_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPP_ETPS_FORMATS___prefix_FORWARD_DOMAIN_VRF_Encoding", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_FORWARD_DOMAIN_VRF_Encoding);
}

uint32
dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_ENCAP_TTL_IDX_MPLS_FHEI_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPP_ETPS_FORMATS___prefix_ENCAP_TTL_IDX_MPLS_FHEI", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_ENCAP_TTL_IDX_MPLS_FHEI);
}

uint32
dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_ESEM_COMMAND_IPvX_TUNNEL_ESEM_Command_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPP_ETPS_FORMATS___prefix_ESEM_COMMAND_IPvX_TUNNEL_ESEM_Command", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_ESEM_COMMAND_IPvX_TUNNEL_ESEM_Command);
}

uint32
dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_Always_Last_Layer_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPP_ETPS_FORMATS___prefix_Always_Last_Layer", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_Always_Last_Layer);
}

uint32
dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_LIF_AHP_MPLS_Prefix_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPP_ETPS_FORMATS___prefix_LIF_AHP_MPLS_Prefix", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_LIF_AHP_MPLS_Prefix);
}

uint32
dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_ENCAP_QOS_MODEL_MPLS_FHEI_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPP_ETPS_FORMATS___prefix_ENCAP_QOS_MODEL_MPLS_FHEI", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_ENCAP_QOS_MODEL_MPLS_FHEI);
}

uint32
dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_ENCAP_QOS_IDX_MPLS_FHEI_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPP_ETPS_FORMATS___prefix_ENCAP_QOS_IDX_MPLS_FHEI", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_ENCAP_QOS_IDX_MPLS_FHEI);
}

uint32
dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_OutLIF_Profile_Orientation_Only_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPP_ETPS_FORMATS___prefix_OutLIF_Profile_Orientation_Only", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_OutLIF_Profile_Orientation_Only);
}

uint32
dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_FORWARD_DOMAIN_VRF_IPvX_Tunnel_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPP_ETPS_FORMATS___prefix_FORWARD_DOMAIN_VRF_IPvX_Tunnel", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_FORWARD_DOMAIN_VRF_IPvX_Tunnel);
}

uint32
dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_QOS_MODEL_1_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPP_ETPS_FORMATS___prefix_QOS_MODEL_1", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_QOS_MODEL_1);
}

uint32
dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_ESEM_COMMAND_IPv6_TUNNELS_FODO_Namespace_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPP_ETPS_FORMATS___prefix_ESEM_COMMAND_IPv6_TUNNELS_FODO_Namespace", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_ESEM_COMMAND_IPv6_TUNNELS_FODO_Namespace);
}

uint32
dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_ENCAP_TTL_MODEL_MPLS_FHEI_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPP_ETPS_FORMATS___prefix_ENCAP_TTL_MODEL_MPLS_FHEI", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_ENCAP_TTL_MODEL_MPLS_FHEI);
}

uint32
dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_ESEM_COMMAND_IPv4_VxLAN_OR_VXLAN_GPE_FODO_Namespace_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPP_ETPS_FORMATS___prefix_ESEM_COMMAND_IPv4_VxLAN_OR_VXLAN_GPE_FODO_Namespace", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_ESEM_COMMAND_IPv4_VxLAN_OR_VXLAN_GPE_FODO_Namespace);
}

uint32
dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_FORWARD_QOS_MODEL_NWK_QOS_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPP_ETPS_FORMATS___prefix_FORWARD_QOS_MODEL_NWK_QOS", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_FORWARD_QOS_MODEL_NWK_QOS);
}

uint32
dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_ESEM_COMMAND_PWE_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPP_ETPS_FORMATS___prefix_ESEM_COMMAND_PWE", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_ETPP_ETPS_FORMATS___prefix_ESEM_COMMAND_PWE);
}

uint32
dnx_data_dnx2_arr_prefix_IRPP_FEC_ENTRY_FORMAT___prefix_Empty_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IRPP_FEC_ENTRY_FORMAT___prefix_Empty", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_IRPP_FEC_ENTRY_FORMAT___prefix_Empty);
}

uint32
dnx_data_dnx2_arr_prefix_IRPP_FEC_ENTRY_FORMAT___prefix_EEI_W_OUTLIF_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IRPP_FEC_ENTRY_FORMAT___prefix_EEI_W_OUTLIF", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_IRPP_FEC_ENTRY_FORMAT___prefix_EEI_W_OUTLIF);
}

uint32
dnx_data_dnx2_arr_prefix_IRPP_FEC_ENTRY_FORMAT___prefix_OUTLIF_ETH_RIF_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IRPP_FEC_ENTRY_FORMAT___prefix_OUTLIF_ETH_RIF", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_IRPP_FEC_ENTRY_FORMAT___prefix_OUTLIF_ETH_RIF);
}

uint32
dnx_data_dnx2_arr_prefix_IRPP_FEC_ENTRY_FORMAT___prefix_EEI_W_POP_Command_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IRPP_FEC_ENTRY_FORMAT___prefix_EEI_W_POP_Command", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_IRPP_FEC_ENTRY_FORMAT___prefix_EEI_W_POP_Command);
}

uint32
dnx_data_dnx2_arr_prefix_IRPP_FEC_ENTRY_FORMAT___prefix_EEI_W_SWAP_Command_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IRPP_FEC_ENTRY_FORMAT___prefix_EEI_W_SWAP_Command", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_IRPP_FEC_ENTRY_FORMAT___prefix_EEI_W_SWAP_Command);
}

uint32
dnx_data_dnx2_arr_prefix_IRPP_FEC_ENTRY_FORMAT___prefix_MC_RPF_MODE_MC_RPF_MODE_EXPLICIT_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IRPP_FEC_ENTRY_FORMAT___prefix_MC_RPF_MODE_MC_RPF_MODE_EXPLICIT", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_IRPP_FEC_ENTRY_FORMAT___prefix_MC_RPF_MODE_MC_RPF_MODE_EXPLICIT);
}

uint32
dnx_data_dnx2_arr_prefix_IRPP_FEC_SUPER_ENTRY_FORMAT___prefix_Empty_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IRPP_FEC_SUPER_ENTRY_FORMAT___prefix_Empty", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_IRPP_FEC_SUPER_ENTRY_FORMAT___prefix_Empty);
}

uint32
dnx_data_dnx2_arr_prefix_IRPP_FWD_STAGE_FORMATS___prefix_Empty_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IRPP_FWD_STAGE_FORMATS___prefix_Empty", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_IRPP_FWD_STAGE_FORMATS___prefix_Empty);
}

uint32
dnx_data_dnx2_arr_prefix_IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b11_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b11", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b11);
}

uint32
dnx_data_dnx2_arr_prefix_IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b0001_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b0001", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b0001);
}

uint32
dnx_data_dnx2_arr_prefix_IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b0010_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b0010", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b0010);
}

uint32
dnx_data_dnx2_arr_prefix_IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b0011_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b0011", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b0011);
}

uint32
dnx_data_dnx2_arr_prefix_IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b0100_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b0100", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b0100);
}

uint32
dnx_data_dnx2_arr_prefix_IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b0101_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b0101", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b0101);
}

uint32
dnx_data_dnx2_arr_prefix_IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b01111_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b01111", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b01111);
}

uint32
dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_Empty_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IRPP_IN_LIF_FORMATS___prefix_Empty", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_Empty);
}

uint32
dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_Service_Type_Extended_P2P_W_Learning_Prefix_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IRPP_IN_LIF_FORMATS___prefix_Service_Type_Extended_P2P_W_Learning_Prefix", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_Service_Type_Extended_P2P_W_Learning_Prefix);
}

uint32
dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_Parser_Context_Eth_B1_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IRPP_IN_LIF_FORMATS___prefix_Parser_Context_Eth_B1", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_Parser_Context_Eth_B1);
}

uint32
dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_Parser_Context_IPvX_DEMUX_B1_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IRPP_IN_LIF_FORMATS___prefix_Parser_Context_IPvX_DEMUX_B1", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_Parser_Context_IPvX_DEMUX_B1);
}

uint32
dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_Service_Type_P2P_Prefix_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IRPP_IN_LIF_FORMATS___prefix_Service_Type_P2P_Prefix", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_Service_Type_P2P_Prefix);
}

uint32
dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_Free_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IRPP_IN_LIF_FORMATS___prefix_Free", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_Free);
}

uint32
dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_VRF_VSI_Not_Found_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IRPP_IN_LIF_FORMATS___prefix_VRF_VSI_Not_Found", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_VRF_VSI_Not_Found);
}

uint32
dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_Forward_Domain_Assignment_Mode_LIF_or_VLAN_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IRPP_IN_LIF_FORMATS___prefix_Forward_Domain_Assignment_Mode_LIF_or_VLAN", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_Forward_Domain_Assignment_Mode_LIF_or_VLAN);
}

uint32
dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_Forward_Domain_VRF_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IRPP_IN_LIF_FORMATS___prefix_Forward_Domain_VRF", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_Forward_Domain_VRF);
}

uint32
dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_NEXT_LAYER_NETWORK_DOMAIN_TNIorVNI_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IRPP_IN_LIF_FORMATS___prefix_NEXT_LAYER_NETWORK_DOMAIN_TNIorVNI", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_NEXT_LAYER_NETWORK_DOMAIN_TNIorVNI);
}

uint32
dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_NEXT_LAYER_NETWORK_DOMAIN_PWE_WO_SD_TAGS_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IRPP_IN_LIF_FORMATS___prefix_NEXT_LAYER_NETWORK_DOMAIN_PWE_WO_SD_TAGS", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_NEXT_LAYER_NETWORK_DOMAIN_PWE_WO_SD_TAGS);
}

uint32
dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF_OPTIMIZED_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IRPP_IN_LIF_FORMATS___prefix_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF_OPTIMIZED", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF_OPTIMIZED);
}

uint32
dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_VSI_P2P_MSBS_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IRPP_IN_LIF_FORMATS___prefix_VSI_P2P_MSBS", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_VSI_P2P_MSBS);
}

uint32
dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_QoS_Profile_short_Prefix_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IRPP_IN_LIF_FORMATS___prefix_QoS_Profile_short_Prefix", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_QoS_Profile_short_Prefix);
}

uint32
dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_Learn_info_destination_port_encoding_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IRPP_IN_LIF_FORMATS___prefix_Learn_info_destination_port_encoding", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_IRPP_IN_LIF_FORMATS___prefix_Learn_info_destination_port_encoding);
}

uint32
dnx_data_dnx2_arr_prefix_IRPP_VSI_FORMATS___prefix_Empty_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IRPP_VSI_FORMATS___prefix_Empty", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_IRPP_VSI_FORMATS___prefix_Empty);
}

uint32
dnx_data_dnx2_arr_prefix_IRPP_VSI_FORMATS___prefix_VSI_Profile_Prefix_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IRPP_VSI_FORMATS___prefix_VSI_Profile_Prefix", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_IRPP_VSI_FORMATS___prefix_VSI_Profile_Prefix);
}

uint32
dnx_data_dnx2_arr_prefix_IRPP_VSI_FORMATS___prefix_Dest_MC_ID_Prefix_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IRPP_VSI_FORMATS___prefix_Dest_MC_ID_Prefix", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_IRPP_VSI_FORMATS___prefix_Dest_MC_ID_Prefix);
}

uint32
dnx_data_dnx2_arr_prefix_IRPP_VSI_FORMATS___prefix_Dest_FEC_Port_Trap_Prefix_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IRPP_VSI_FORMATS___prefix_Dest_FEC_Port_Trap_Prefix", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_IRPP_VSI_FORMATS___prefix_Dest_FEC_Port_Trap_Prefix);
}

uint32
dnx_data_dnx2_arr_prefix_IRPP_VSI_FORMATS___prefix_Topology_ID_Prefix_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IRPP_VSI_FORMATS___prefix_Topology_ID_Prefix", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_IRPP_VSI_FORMATS___prefix_Topology_ID_Prefix);
}

uint32
dnx_data_dnx2_arr_prefix_IRPP_VSI_FORMATS___prefix_QOS_Profile_Prefix_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IRPP_VSI_FORMATS___prefix_QOS_Profile_Prefix", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_IRPP_VSI_FORMATS___prefix_QOS_Profile_Prefix);
}

uint32
dnx_data_dnx2_arr_prefix_IRPP_VSI_FORMATS___prefix_My_Mac_Prefix_Prefix_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IRPP_VSI_FORMATS___prefix_My_Mac_Prefix_Prefix", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_IRPP_VSI_FORMATS___prefix_My_Mac_Prefix_Prefix);
}

uint32
dnx_data_dnx2_arr_prefix_IRPP_VSI_FORMATS___prefix_VSI_Stat_Obj_Cmd_Prefix_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IRPP_VSI_FORMATS___prefix_VSI_Stat_Obj_Cmd_Prefix", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_prefix, dnx_data_dnx2_arr_prefix_define_IRPP_VSI_FORMATS___prefix_VSI_Stat_Obj_Cmd_Prefix);
}










static shr_error_e
dnx_data_dnx2_arr_max_nof_entries_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "max_nof_entries";
    submodule_data->doc = "ARR max_nof_entries values";
    
    submodule_data->nof_features = _dnx_data_dnx2_arr_max_nof_entries_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data dnx2_arr max_nof_entries features");

    
    submodule_data->nof_defines = _dnx_data_dnx2_arr_max_nof_entries_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data dnx2_arr max_nof_entries defines");

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_AGE_PROFILE_ARR].name = "AGE_PROFILE_ARR";
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_AGE_PROFILE_ARR].doc = "max_nof_entries of AGE_PROFILE_ARR";
        
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_AGE_PROFILE_ARR].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_ETPP_ENCAP_1_ARR_ToS].name = "ETPP_ENCAP_1_ARR_ToS";
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_ETPP_ENCAP_1_ARR_ToS].doc = "max_nof_entries of ETPP_ENCAP_1_ARR_ToS";
        
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_ETPP_ENCAP_1_ARR_ToS].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_ETPP_ENCAP_2_ARR_ToS].name = "ETPP_ENCAP_2_ARR_ToS";
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_ETPP_ENCAP_2_ARR_ToS].doc = "max_nof_entries of ETPP_ENCAP_2_ARR_ToS";
        
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_ETPP_ENCAP_2_ARR_ToS].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_ETPP_ENCAP_3_ARR_ToS].name = "ETPP_ENCAP_3_ARR_ToS";
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_ETPP_ENCAP_3_ARR_ToS].doc = "max_nof_entries of ETPP_ENCAP_3_ARR_ToS";
        
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_ETPP_ENCAP_3_ARR_ToS].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_ETPP_ENCAP_4_ARR_ToS].name = "ETPP_ENCAP_4_ARR_ToS";
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_ETPP_ENCAP_4_ARR_ToS].doc = "max_nof_entries of ETPP_ENCAP_4_ARR_ToS";
        
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_ETPP_ENCAP_4_ARR_ToS].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_ETPP_ENCAP_5_ARR_ToS].name = "ETPP_ENCAP_5_ARR_ToS";
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_ETPP_ENCAP_5_ARR_ToS].doc = "max_nof_entries of ETPP_ENCAP_5_ARR_ToS";
        
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_ETPP_ENCAP_5_ARR_ToS].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_ETPP_ENCAP_ARR_ToS_plus_2].name = "ETPP_ENCAP_ARR_ToS_plus_2";
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_ETPP_ENCAP_ARR_ToS_plus_2].doc = "max_nof_entries of ETPP_ENCAP_ARR_ToS_plus_2";
        
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_ETPP_ENCAP_ARR_ToS_plus_2].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_ETPP_FWD_ARR_ToS].name = "ETPP_FWD_ARR_ToS";
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_ETPP_FWD_ARR_ToS].doc = "max_nof_entries of ETPP_FWD_ARR_ToS";
        
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_ETPP_FWD_ARR_ToS].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_ETPP_FWD_ARR_ToS_plus_1].name = "ETPP_FWD_ARR_ToS_plus_1";
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_ETPP_FWD_ARR_ToS_plus_1].doc = "max_nof_entries of ETPP_FWD_ARR_ToS_plus_1";
        
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_ETPP_FWD_ARR_ToS_plus_1].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_ETPP_Preprocessing_ARR].name = "ETPP_Preprocessing_ARR";
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_ETPP_Preprocessing_ARR].doc = "max_nof_entries of ETPP_Preprocessing_ARR";
        
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_ETPP_Preprocessing_ARR].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_ETPP_Termination_1_ARR].name = "ETPP_Termination_1_ARR";
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_ETPP_Termination_1_ARR].doc = "max_nof_entries of ETPP_Termination_1_ARR";
        
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_ETPP_Termination_1_ARR].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_ETPP_Termination_2_ARR].name = "ETPP_Termination_2_ARR";
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_ETPP_Termination_2_ARR].doc = "max_nof_entries of ETPP_Termination_2_ARR";
        
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_ETPP_Termination_2_ARR].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_IPPB_ACCEPTED_ARR].name = "IPPB_ACCEPTED_ARR";
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_IPPB_ACCEPTED_ARR].doc = "max_nof_entries of IPPB_ACCEPTED_ARR";
        
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_IPPB_ACCEPTED_ARR].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_IPPB_EXPECTED_ACCEPTED_STRENGTH_ARR].name = "IPPB_EXPECTED_ACCEPTED_STRENGTH_ARR";
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_IPPB_EXPECTED_ACCEPTED_STRENGTH_ARR].doc = "max_nof_entries of IPPB_EXPECTED_ACCEPTED_STRENGTH_ARR";
        
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_IPPB_EXPECTED_ACCEPTED_STRENGTH_ARR].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_IPPB_EXPECTED_ARR].name = "IPPB_EXPECTED_ARR";
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_IPPB_EXPECTED_ARR].doc = "max_nof_entries of IPPB_EXPECTED_ARR";
        
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_IPPB_EXPECTED_ARR].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_IRPP_FEC_ARR].name = "IRPP_FEC_ARR";
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_IRPP_FEC_ARR].doc = "max_nof_entries of IRPP_FEC_ARR";
        
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_IRPP_FEC_ARR].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_IRPP_FWD1_FWD_ARR].name = "IRPP_FWD1_FWD_ARR";
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_IRPP_FWD1_FWD_ARR].doc = "max_nof_entries of IRPP_FWD1_FWD_ARR";
        
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_IRPP_FWD1_FWD_ARR].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_IRPP_FWD1_RPF_ARR].name = "IRPP_FWD1_RPF_ARR";
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_IRPP_FWD1_RPF_ARR].doc = "max_nof_entries of IRPP_FWD1_RPF_ARR";
        
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_IRPP_FWD1_RPF_ARR].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_IRPP_SUPER_FEC_ARR].name = "IRPP_SUPER_FEC_ARR";
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_IRPP_SUPER_FEC_ARR].doc = "max_nof_entries of IRPP_SUPER_FEC_ARR";
        
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_IRPP_SUPER_FEC_ARR].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_IRPP_VSI_ARR].name = "IRPP_VSI_ARR";
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_IRPP_VSI_ARR].doc = "max_nof_entries of IRPP_VSI_ARR";
        
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_IRPP_VSI_ARR].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_IRPP_VTT_ARR].name = "IRPP_VTT_ARR";
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_IRPP_VTT_ARR].doc = "max_nof_entries of IRPP_VTT_ARR";
        
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_IRPP_VTT_ARR].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_ITPP_MACT_LEARN_JR1_TRANSLATE_ARR].name = "ITPP_MACT_LEARN_JR1_TRANSLATE_ARR";
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_ITPP_MACT_LEARN_JR1_TRANSLATE_ARR].doc = "max_nof_entries of ITPP_MACT_LEARN_JR1_TRANSLATE_ARR";
        
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_ITPP_MACT_LEARN_JR1_TRANSLATE_ARR].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_PPMC_ERPP_ARR].name = "PPMC_ERPP_ARR";
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_PPMC_ERPP_ARR].doc = "max_nof_entries of PPMC_ERPP_ARR";
        
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_PPMC_ERPP_ARR].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_USER_ALLOCATION].name = "USER_ALLOCATION";
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_USER_ALLOCATION].doc = "max_nof_entries of USER_ALLOCATION";
        
        submodule_data->defines[dnx_data_dnx2_arr_max_nof_entries_define_USER_ALLOCATION].flags |= DNXC_DATA_F_DEFINE;
    }

    
    submodule_data->nof_tables = _dnx_data_dnx2_arr_max_nof_entries_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data dnx2_arr max_nof_entries tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_dnx2_arr_max_nof_entries_feature_get(
    int unit,
    dnx_data_dnx2_arr_max_nof_entries_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_max_nof_entries, feature);
}


uint32
dnx_data_dnx2_arr_max_nof_entries_AGE_PROFILE_ARR_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "AGE_PROFILE_ARR", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_max_nof_entries, dnx_data_dnx2_arr_max_nof_entries_define_AGE_PROFILE_ARR);
}

uint32
dnx_data_dnx2_arr_max_nof_entries_ETPP_ENCAP_1_ARR_ToS_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPP_ENCAP_1_ARR_ToS", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_max_nof_entries, dnx_data_dnx2_arr_max_nof_entries_define_ETPP_ENCAP_1_ARR_ToS);
}

uint32
dnx_data_dnx2_arr_max_nof_entries_ETPP_ENCAP_2_ARR_ToS_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPP_ENCAP_2_ARR_ToS", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_max_nof_entries, dnx_data_dnx2_arr_max_nof_entries_define_ETPP_ENCAP_2_ARR_ToS);
}

uint32
dnx_data_dnx2_arr_max_nof_entries_ETPP_ENCAP_3_ARR_ToS_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPP_ENCAP_3_ARR_ToS", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_max_nof_entries, dnx_data_dnx2_arr_max_nof_entries_define_ETPP_ENCAP_3_ARR_ToS);
}

uint32
dnx_data_dnx2_arr_max_nof_entries_ETPP_ENCAP_4_ARR_ToS_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPP_ENCAP_4_ARR_ToS", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_max_nof_entries, dnx_data_dnx2_arr_max_nof_entries_define_ETPP_ENCAP_4_ARR_ToS);
}

uint32
dnx_data_dnx2_arr_max_nof_entries_ETPP_ENCAP_5_ARR_ToS_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPP_ENCAP_5_ARR_ToS", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_max_nof_entries, dnx_data_dnx2_arr_max_nof_entries_define_ETPP_ENCAP_5_ARR_ToS);
}

uint32
dnx_data_dnx2_arr_max_nof_entries_ETPP_ENCAP_ARR_ToS_plus_2_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPP_ENCAP_ARR_ToS_plus_2", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_max_nof_entries, dnx_data_dnx2_arr_max_nof_entries_define_ETPP_ENCAP_ARR_ToS_plus_2);
}

uint32
dnx_data_dnx2_arr_max_nof_entries_ETPP_FWD_ARR_ToS_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPP_FWD_ARR_ToS", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_max_nof_entries, dnx_data_dnx2_arr_max_nof_entries_define_ETPP_FWD_ARR_ToS);
}

uint32
dnx_data_dnx2_arr_max_nof_entries_ETPP_FWD_ARR_ToS_plus_1_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPP_FWD_ARR_ToS_plus_1", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_max_nof_entries, dnx_data_dnx2_arr_max_nof_entries_define_ETPP_FWD_ARR_ToS_plus_1);
}

uint32
dnx_data_dnx2_arr_max_nof_entries_ETPP_Preprocessing_ARR_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPP_Preprocessing_ARR", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_max_nof_entries, dnx_data_dnx2_arr_max_nof_entries_define_ETPP_Preprocessing_ARR);
}

uint32
dnx_data_dnx2_arr_max_nof_entries_ETPP_Termination_1_ARR_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPP_Termination_1_ARR", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_max_nof_entries, dnx_data_dnx2_arr_max_nof_entries_define_ETPP_Termination_1_ARR);
}

uint32
dnx_data_dnx2_arr_max_nof_entries_ETPP_Termination_2_ARR_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPP_Termination_2_ARR", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_max_nof_entries, dnx_data_dnx2_arr_max_nof_entries_define_ETPP_Termination_2_ARR);
}

uint32
dnx_data_dnx2_arr_max_nof_entries_IPPB_ACCEPTED_ARR_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IPPB_ACCEPTED_ARR", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_max_nof_entries, dnx_data_dnx2_arr_max_nof_entries_define_IPPB_ACCEPTED_ARR);
}

uint32
dnx_data_dnx2_arr_max_nof_entries_IPPB_EXPECTED_ACCEPTED_STRENGTH_ARR_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IPPB_EXPECTED_ACCEPTED_STRENGTH_ARR", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_max_nof_entries, dnx_data_dnx2_arr_max_nof_entries_define_IPPB_EXPECTED_ACCEPTED_STRENGTH_ARR);
}

uint32
dnx_data_dnx2_arr_max_nof_entries_IPPB_EXPECTED_ARR_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IPPB_EXPECTED_ARR", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_max_nof_entries, dnx_data_dnx2_arr_max_nof_entries_define_IPPB_EXPECTED_ARR);
}

uint32
dnx_data_dnx2_arr_max_nof_entries_IRPP_FEC_ARR_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IRPP_FEC_ARR", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_max_nof_entries, dnx_data_dnx2_arr_max_nof_entries_define_IRPP_FEC_ARR);
}

uint32
dnx_data_dnx2_arr_max_nof_entries_IRPP_FWD1_FWD_ARR_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IRPP_FWD1_FWD_ARR", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_max_nof_entries, dnx_data_dnx2_arr_max_nof_entries_define_IRPP_FWD1_FWD_ARR);
}

uint32
dnx_data_dnx2_arr_max_nof_entries_IRPP_FWD1_RPF_ARR_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IRPP_FWD1_RPF_ARR", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_max_nof_entries, dnx_data_dnx2_arr_max_nof_entries_define_IRPP_FWD1_RPF_ARR);
}

uint32
dnx_data_dnx2_arr_max_nof_entries_IRPP_SUPER_FEC_ARR_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IRPP_SUPER_FEC_ARR", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_max_nof_entries, dnx_data_dnx2_arr_max_nof_entries_define_IRPP_SUPER_FEC_ARR);
}

uint32
dnx_data_dnx2_arr_max_nof_entries_IRPP_VSI_ARR_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IRPP_VSI_ARR", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_max_nof_entries, dnx_data_dnx2_arr_max_nof_entries_define_IRPP_VSI_ARR);
}

uint32
dnx_data_dnx2_arr_max_nof_entries_IRPP_VTT_ARR_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IRPP_VTT_ARR", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_max_nof_entries, dnx_data_dnx2_arr_max_nof_entries_define_IRPP_VTT_ARR);
}

uint32
dnx_data_dnx2_arr_max_nof_entries_ITPP_MACT_LEARN_JR1_TRANSLATE_ARR_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ITPP_MACT_LEARN_JR1_TRANSLATE_ARR", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_max_nof_entries, dnx_data_dnx2_arr_max_nof_entries_define_ITPP_MACT_LEARN_JR1_TRANSLATE_ARR);
}

uint32
dnx_data_dnx2_arr_max_nof_entries_PPMC_ERPP_ARR_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "PPMC_ERPP_ARR", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_max_nof_entries, dnx_data_dnx2_arr_max_nof_entries_define_PPMC_ERPP_ARR);
}

uint32
dnx_data_dnx2_arr_max_nof_entries_USER_ALLOCATION_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "USER_ALLOCATION", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_max_nof_entries, dnx_data_dnx2_arr_max_nof_entries_define_USER_ALLOCATION);
}










static shr_error_e
dnx_data_dnx2_arr_formats_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "formats";
    submodule_data->doc = "ARR formats table values";
    
    submodule_data->nof_features = _dnx_data_dnx2_arr_formats_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data dnx2_arr formats features");

    
    submodule_data->nof_defines = _dnx_data_dnx2_arr_formats_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data dnx2_arr formats defines");

    
    submodule_data->nof_tables = _dnx_data_dnx2_arr_formats_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data dnx2_arr formats tables");

    if (soc_is(unit, DNX2_DEVICE))
    {
        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_files].name = "files";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_files].doc = "file locations";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_files].flags |= DNXC_DATA_F_TABLE;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_files].size_of_values = sizeof(dnx_data_dnx2_arr_formats_files_t);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_files].entry_get = dnx_data_dnx2_arr_formats_files_entry_str_get;

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_files].nof_keys = 0;

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_files].nof_values = 1;
        DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dnx2_arr_formats_table_files].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dnx2_arr_formats_table_files].nof_values, "_dnx_data_dnx2_arr_formats_table_files table values");
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_files].values[0].name = "arr_formats_file";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_files].values[0].type = "char *";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_files].values[0].doc = "location of arr formats xml file";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_files].values[0].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_files_t, arr_formats_file);
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_AGE_PROFILE_ARR_CFG].name = "MACT_AGE_PROFILE_ARR_CFG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_AGE_PROFILE_ARR_CFG].doc = "MACT_AGE_PROFILE_ARR_CFG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_AGE_PROFILE_ARR_CFG].flags |= DNXC_DATA_F_TABLE;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_AGE_PROFILE_ARR_CFG].size_of_values = sizeof(dnx_data_dnx2_arr_formats_MACT_AGE_PROFILE_ARR_CFG_t);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_AGE_PROFILE_ARR_CFG].entry_get = dnx_data_dnx2_arr_formats_MACT_AGE_PROFILE_ARR_CFG_entry_str_get;

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_AGE_PROFILE_ARR_CFG].nof_keys = 1;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_AGE_PROFILE_ARR_CFG].keys[0].name = "index";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_AGE_PROFILE_ARR_CFG].keys[0].doc = "entry index";

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_AGE_PROFILE_ARR_CFG].nof_values = 4;
        DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_AGE_PROFILE_ARR_CFG].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_AGE_PROFILE_ARR_CFG].nof_values, "_dnx_data_dnx2_arr_formats_table_MACT_AGE_PROFILE_ARR_CFG table values");
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_AGE_PROFILE_ARR_CFG].values[0].name = "valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_AGE_PROFILE_ARR_CFG].values[0].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_AGE_PROFILE_ARR_CFG].values[0].doc = "indicates whether entry is valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_AGE_PROFILE_ARR_CFG].values[0].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_MACT_AGE_PROFILE_ARR_CFG_t, valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_AGE_PROFILE_ARR_CFG].values[1].name = "doc";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_AGE_PROFILE_ARR_CFG].values[1].type = "char *";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_AGE_PROFILE_ARR_CFG].values[1].doc = "entry documentation";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_AGE_PROFILE_ARR_CFG].values[1].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_MACT_AGE_PROFILE_ARR_CFG_t, doc);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_AGE_PROFILE_ARR_CFG].values[2].name = "AGE_PROFILE";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_AGE_PROFILE_ARR_CFG].values[2].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_AGE_PROFILE_ARR_CFG].values[2].doc = "AGE_PROFILE";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_AGE_PROFILE_ARR_CFG].values[2].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_MACT_AGE_PROFILE_ARR_CFG_t, AGE_PROFILE);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_AGE_PROFILE_ARR_CFG].values[3].name = "AGE_PROFILE_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_AGE_PROFILE_ARR_CFG].values[3].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_AGE_PROFILE_ARR_CFG].values[3].doc = "AGE_PROFILE_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_AGE_PROFILE_ARR_CFG].values[3].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_MACT_AGE_PROFILE_ARR_CFG_t, AGE_PROFILE_valid);
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].name = "ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].doc = "ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].flags |= DNXC_DATA_F_TABLE;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].size_of_values = sizeof(dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_t);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].entry_get = dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_entry_str_get;

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].nof_keys = 1;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].keys[0].name = "index";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].keys[0].doc = "entry index";

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].nof_values = 36;
        DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].nof_values, "_dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE table values");
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[0].name = "valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[0].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[0].doc = "indicates whether entry is valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[0].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_t, valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[1].name = "doc";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[1].type = "char *";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[1].doc = "entry documentation";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[1].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_t, doc);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[2].name = "DUAL_HOMING_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[2].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[2].doc = "DUAL_HOMING_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[2].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_t, DUAL_HOMING_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[3].name = "DUAL_HOMING_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[3].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[3].doc = "DUAL_HOMING_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[3].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_t, DUAL_HOMING_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[4].name = "EGRESS_LAST_LAYER_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[4].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[4].doc = "EGRESS_LAST_LAYER_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[4].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_t, EGRESS_LAST_LAYER_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[5].name = "EGRESS_LAST_LAYER_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[5].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[5].doc = "EGRESS_LAST_LAYER_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[5].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_t, EGRESS_LAST_LAYER_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[6].name = "VLAN_MEMBERSHIP_INTERFACE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[6].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[6].doc = "VLAN_MEMBERSHIP_INTERFACE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[6].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_t, VLAN_MEMBERSHIP_INTERFACE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[7].name = "VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[7].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[7].doc = "VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[7].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_t, VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[8].name = "ENCAP_DESTINATION_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[8].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[8].doc = "ENCAP_DESTINATION_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[8].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_t, ENCAP_DESTINATION_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[9].name = "ENCAP_DESTINATION_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[9].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[9].doc = "ENCAP_DESTINATION_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[9].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_t, ENCAP_DESTINATION_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[10].name = "QOS_MODEL_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[10].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[10].doc = "QOS_MODEL_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[10].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_t, QOS_MODEL_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[11].name = "QOS_MODEL_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[11].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[11].doc = "QOS_MODEL_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[11].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_t, QOS_MODEL_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[12].name = "FORWARD_DOMAIN_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[12].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[12].doc = "FORWARD_DOMAIN_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[12].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_t, FORWARD_DOMAIN_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[13].name = "FORWARD_DOMAIN_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[13].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[13].doc = "FORWARD_DOMAIN_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[13].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_t, FORWARD_DOMAIN_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[14].name = "FORWARD_DOMAIN_NETWORK_NAME_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[14].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[14].doc = "FORWARD_DOMAIN_NETWORK_NAME_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[14].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_t, FORWARD_DOMAIN_NETWORK_NAME_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[15].name = "FORWARD_DOMAIN_NETWORK_NAME_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[15].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[15].doc = "FORWARD_DOMAIN_NETWORK_NAME_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[15].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_t, FORWARD_DOMAIN_NETWORK_NAME_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[16].name = "LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[16].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[16].doc = "LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[16].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_t, LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[17].name = "LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[17].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[17].doc = "LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[17].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_t, LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[18].name = "MTU_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[18].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[18].doc = "MTU_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[18].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_t, MTU_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[19].name = "MTU_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[19].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[19].doc = "MTU_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[19].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_t, MTU_PROFILE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[20].name = "NWK_QOS_IDX_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[20].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[20].doc = "NWK_QOS_IDX_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[20].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_t, NWK_QOS_IDX_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[21].name = "NWK_QOS_IDX_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[21].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[21].doc = "NWK_QOS_IDX_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[21].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_t, NWK_QOS_IDX_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[22].name = "REMARK_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[22].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[22].doc = "REMARK_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[22].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_t, REMARK_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[23].name = "REMARK_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[23].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[23].doc = "REMARK_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[23].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_t, REMARK_PROFILE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[24].name = "SOURCE_IDX_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[24].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[24].doc = "SOURCE_IDX_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[24].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_t, SOURCE_IDX_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[25].name = "SOURCE_IDX_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[25].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[25].doc = "SOURCE_IDX_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[25].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_t, SOURCE_IDX_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[26].name = "LIF_STATISTICS_OBJECT_COMMAND_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[26].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[26].doc = "LIF_STATISTICS_OBJECT_COMMAND_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[26].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_t, LIF_STATISTICS_OBJECT_COMMAND_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[27].name = "LIF_STATISTICS_OBJECT_COMMAND_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[27].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[27].doc = "LIF_STATISTICS_OBJECT_COMMAND_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[27].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_t, LIF_STATISTICS_OBJECT_COMMAND_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[28].name = "LIF_STATISTICS_OBJECT_ID_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[28].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[28].doc = "LIF_STATISTICS_OBJECT_ID_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[28].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_t, LIF_STATISTICS_OBJECT_ID_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[29].name = "LIF_STATISTICS_OBJECT_ID_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[29].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[29].doc = "LIF_STATISTICS_OBJECT_ID_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[29].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_t, LIF_STATISTICS_OBJECT_ID_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[30].name = "TTL_MODEL_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[30].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[30].doc = "TTL_MODEL_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[30].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_t, TTL_MODEL_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[31].name = "TTL_MODEL_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[31].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[31].doc = "TTL_MODEL_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[31].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_t, TTL_MODEL_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[32].name = "TTL_IDX_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[32].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[32].doc = "TTL_IDX_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[32].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_t, TTL_IDX_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[33].name = "TTL_IDX_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[33].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[33].doc = "TTL_IDX_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[33].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_t, TTL_IDX_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[34].name = "VLAN_EDIT_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[34].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[34].doc = "VLAN_EDIT_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[34].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_t, VLAN_EDIT_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[35].name = "VLAN_EDIT_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[35].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[35].doc = "VLAN_EDIT_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE].values[35].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_t, VLAN_EDIT_PROFILE_CONFIG_valid);
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].name = "ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].doc = "ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].flags |= DNXC_DATA_F_TABLE;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].size_of_values = sizeof(dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_t);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].entry_get = dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_entry_str_get;

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].nof_keys = 1;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].keys[0].name = "index";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].keys[0].doc = "entry index";

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].nof_values = 36;
        DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].nof_values, "_dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE table values");
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[0].name = "valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[0].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[0].doc = "indicates whether entry is valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[0].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_t, valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[1].name = "doc";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[1].type = "char *";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[1].doc = "entry documentation";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[1].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_t, doc);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[2].name = "DUAL_HOMING_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[2].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[2].doc = "DUAL_HOMING_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[2].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_t, DUAL_HOMING_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[3].name = "DUAL_HOMING_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[3].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[3].doc = "DUAL_HOMING_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[3].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_t, DUAL_HOMING_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[4].name = "EGRESS_LAST_LAYER_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[4].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[4].doc = "EGRESS_LAST_LAYER_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[4].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_t, EGRESS_LAST_LAYER_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[5].name = "EGRESS_LAST_LAYER_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[5].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[5].doc = "EGRESS_LAST_LAYER_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[5].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_t, EGRESS_LAST_LAYER_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[6].name = "VLAN_MEMBERSHIP_INTERFACE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[6].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[6].doc = "VLAN_MEMBERSHIP_INTERFACE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[6].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_t, VLAN_MEMBERSHIP_INTERFACE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[7].name = "VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[7].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[7].doc = "VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[7].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_t, VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[8].name = "ENCAP_DESTINATION_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[8].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[8].doc = "ENCAP_DESTINATION_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[8].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_t, ENCAP_DESTINATION_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[9].name = "ENCAP_DESTINATION_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[9].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[9].doc = "ENCAP_DESTINATION_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[9].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_t, ENCAP_DESTINATION_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[10].name = "QOS_MODEL_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[10].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[10].doc = "QOS_MODEL_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[10].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_t, QOS_MODEL_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[11].name = "QOS_MODEL_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[11].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[11].doc = "QOS_MODEL_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[11].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_t, QOS_MODEL_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[12].name = "FORWARD_DOMAIN_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[12].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[12].doc = "FORWARD_DOMAIN_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[12].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_t, FORWARD_DOMAIN_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[13].name = "FORWARD_DOMAIN_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[13].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[13].doc = "FORWARD_DOMAIN_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[13].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_t, FORWARD_DOMAIN_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[14].name = "FORWARD_DOMAIN_NETWORK_NAME_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[14].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[14].doc = "FORWARD_DOMAIN_NETWORK_NAME_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[14].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_t, FORWARD_DOMAIN_NETWORK_NAME_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[15].name = "FORWARD_DOMAIN_NETWORK_NAME_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[15].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[15].doc = "FORWARD_DOMAIN_NETWORK_NAME_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[15].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_t, FORWARD_DOMAIN_NETWORK_NAME_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[16].name = "LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[16].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[16].doc = "LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[16].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_t, LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[17].name = "LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[17].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[17].doc = "LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[17].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_t, LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[18].name = "MTU_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[18].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[18].doc = "MTU_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[18].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_t, MTU_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[19].name = "MTU_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[19].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[19].doc = "MTU_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[19].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_t, MTU_PROFILE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[20].name = "NWK_QOS_IDX_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[20].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[20].doc = "NWK_QOS_IDX_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[20].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_t, NWK_QOS_IDX_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[21].name = "NWK_QOS_IDX_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[21].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[21].doc = "NWK_QOS_IDX_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[21].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_t, NWK_QOS_IDX_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[22].name = "REMARK_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[22].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[22].doc = "REMARK_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[22].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_t, REMARK_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[23].name = "REMARK_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[23].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[23].doc = "REMARK_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[23].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_t, REMARK_PROFILE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[24].name = "SOURCE_IDX_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[24].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[24].doc = "SOURCE_IDX_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[24].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_t, SOURCE_IDX_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[25].name = "SOURCE_IDX_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[25].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[25].doc = "SOURCE_IDX_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[25].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_t, SOURCE_IDX_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[26].name = "LIF_STATISTICS_OBJECT_COMMAND_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[26].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[26].doc = "LIF_STATISTICS_OBJECT_COMMAND_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[26].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_t, LIF_STATISTICS_OBJECT_COMMAND_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[27].name = "LIF_STATISTICS_OBJECT_COMMAND_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[27].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[27].doc = "LIF_STATISTICS_OBJECT_COMMAND_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[27].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_t, LIF_STATISTICS_OBJECT_COMMAND_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[28].name = "LIF_STATISTICS_OBJECT_ID_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[28].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[28].doc = "LIF_STATISTICS_OBJECT_ID_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[28].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_t, LIF_STATISTICS_OBJECT_ID_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[29].name = "LIF_STATISTICS_OBJECT_ID_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[29].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[29].doc = "LIF_STATISTICS_OBJECT_ID_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[29].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_t, LIF_STATISTICS_OBJECT_ID_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[30].name = "TTL_MODEL_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[30].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[30].doc = "TTL_MODEL_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[30].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_t, TTL_MODEL_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[31].name = "TTL_MODEL_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[31].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[31].doc = "TTL_MODEL_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[31].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_t, TTL_MODEL_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[32].name = "TTL_IDX_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[32].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[32].doc = "TTL_IDX_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[32].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_t, TTL_IDX_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[33].name = "TTL_IDX_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[33].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[33].doc = "TTL_IDX_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[33].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_t, TTL_IDX_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[34].name = "VLAN_EDIT_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[34].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[34].doc = "VLAN_EDIT_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[34].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_t, VLAN_EDIT_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[35].name = "VLAN_EDIT_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[35].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[35].doc = "VLAN_EDIT_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE].values[35].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_t, VLAN_EDIT_PROFILE_CONFIG_valid);
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].name = "ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].doc = "ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].flags |= DNXC_DATA_F_TABLE;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].size_of_values = sizeof(dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_t);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].entry_get = dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_entry_str_get;

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].nof_keys = 1;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].keys[0].name = "index";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].keys[0].doc = "entry index";

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].nof_values = 36;
        DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].nof_values, "_dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE table values");
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[0].name = "valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[0].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[0].doc = "indicates whether entry is valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[0].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_t, valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[1].name = "doc";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[1].type = "char *";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[1].doc = "entry documentation";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[1].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_t, doc);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[2].name = "DUAL_HOMING_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[2].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[2].doc = "DUAL_HOMING_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[2].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_t, DUAL_HOMING_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[3].name = "DUAL_HOMING_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[3].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[3].doc = "DUAL_HOMING_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[3].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_t, DUAL_HOMING_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[4].name = "EGRESS_LAST_LAYER_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[4].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[4].doc = "EGRESS_LAST_LAYER_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[4].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_t, EGRESS_LAST_LAYER_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[5].name = "EGRESS_LAST_LAYER_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[5].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[5].doc = "EGRESS_LAST_LAYER_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[5].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_t, EGRESS_LAST_LAYER_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[6].name = "VLAN_MEMBERSHIP_INTERFACE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[6].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[6].doc = "VLAN_MEMBERSHIP_INTERFACE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[6].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_t, VLAN_MEMBERSHIP_INTERFACE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[7].name = "VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[7].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[7].doc = "VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[7].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_t, VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[8].name = "ENCAP_DESTINATION_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[8].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[8].doc = "ENCAP_DESTINATION_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[8].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_t, ENCAP_DESTINATION_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[9].name = "ENCAP_DESTINATION_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[9].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[9].doc = "ENCAP_DESTINATION_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[9].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_t, ENCAP_DESTINATION_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[10].name = "QOS_MODEL_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[10].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[10].doc = "QOS_MODEL_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[10].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_t, QOS_MODEL_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[11].name = "QOS_MODEL_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[11].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[11].doc = "QOS_MODEL_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[11].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_t, QOS_MODEL_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[12].name = "FORWARD_DOMAIN_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[12].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[12].doc = "FORWARD_DOMAIN_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[12].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_t, FORWARD_DOMAIN_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[13].name = "FORWARD_DOMAIN_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[13].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[13].doc = "FORWARD_DOMAIN_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[13].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_t, FORWARD_DOMAIN_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[14].name = "FORWARD_DOMAIN_NETWORK_NAME_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[14].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[14].doc = "FORWARD_DOMAIN_NETWORK_NAME_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[14].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_t, FORWARD_DOMAIN_NETWORK_NAME_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[15].name = "FORWARD_DOMAIN_NETWORK_NAME_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[15].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[15].doc = "FORWARD_DOMAIN_NETWORK_NAME_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[15].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_t, FORWARD_DOMAIN_NETWORK_NAME_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[16].name = "LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[16].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[16].doc = "LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[16].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_t, LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[17].name = "LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[17].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[17].doc = "LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[17].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_t, LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[18].name = "MTU_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[18].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[18].doc = "MTU_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[18].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_t, MTU_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[19].name = "MTU_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[19].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[19].doc = "MTU_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[19].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_t, MTU_PROFILE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[20].name = "NWK_QOS_IDX_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[20].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[20].doc = "NWK_QOS_IDX_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[20].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_t, NWK_QOS_IDX_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[21].name = "NWK_QOS_IDX_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[21].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[21].doc = "NWK_QOS_IDX_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[21].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_t, NWK_QOS_IDX_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[22].name = "REMARK_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[22].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[22].doc = "REMARK_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[22].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_t, REMARK_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[23].name = "REMARK_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[23].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[23].doc = "REMARK_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[23].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_t, REMARK_PROFILE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[24].name = "SOURCE_IDX_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[24].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[24].doc = "SOURCE_IDX_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[24].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_t, SOURCE_IDX_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[25].name = "SOURCE_IDX_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[25].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[25].doc = "SOURCE_IDX_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[25].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_t, SOURCE_IDX_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[26].name = "LIF_STATISTICS_OBJECT_COMMAND_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[26].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[26].doc = "LIF_STATISTICS_OBJECT_COMMAND_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[26].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_t, LIF_STATISTICS_OBJECT_COMMAND_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[27].name = "LIF_STATISTICS_OBJECT_COMMAND_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[27].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[27].doc = "LIF_STATISTICS_OBJECT_COMMAND_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[27].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_t, LIF_STATISTICS_OBJECT_COMMAND_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[28].name = "LIF_STATISTICS_OBJECT_ID_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[28].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[28].doc = "LIF_STATISTICS_OBJECT_ID_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[28].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_t, LIF_STATISTICS_OBJECT_ID_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[29].name = "LIF_STATISTICS_OBJECT_ID_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[29].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[29].doc = "LIF_STATISTICS_OBJECT_ID_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[29].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_t, LIF_STATISTICS_OBJECT_ID_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[30].name = "TTL_MODEL_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[30].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[30].doc = "TTL_MODEL_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[30].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_t, TTL_MODEL_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[31].name = "TTL_MODEL_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[31].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[31].doc = "TTL_MODEL_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[31].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_t, TTL_MODEL_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[32].name = "TTL_IDX_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[32].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[32].doc = "TTL_IDX_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[32].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_t, TTL_IDX_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[33].name = "TTL_IDX_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[33].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[33].doc = "TTL_IDX_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[33].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_t, TTL_IDX_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[34].name = "VLAN_EDIT_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[34].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[34].doc = "VLAN_EDIT_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[34].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_t, VLAN_EDIT_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[35].name = "VLAN_EDIT_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[35].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[35].doc = "VLAN_EDIT_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE].values[35].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_t, VLAN_EDIT_PROFILE_CONFIG_valid);
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].name = "ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].doc = "ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].flags |= DNXC_DATA_F_TABLE;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].size_of_values = sizeof(dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_t);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].entry_get = dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_entry_str_get;

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].nof_keys = 1;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].keys[0].name = "index";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].keys[0].doc = "entry index";

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].nof_values = 36;
        DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].nof_values, "_dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE table values");
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[0].name = "valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[0].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[0].doc = "indicates whether entry is valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[0].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_t, valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[1].name = "doc";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[1].type = "char *";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[1].doc = "entry documentation";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[1].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_t, doc);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[2].name = "DUAL_HOMING_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[2].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[2].doc = "DUAL_HOMING_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[2].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_t, DUAL_HOMING_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[3].name = "DUAL_HOMING_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[3].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[3].doc = "DUAL_HOMING_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[3].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_t, DUAL_HOMING_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[4].name = "EGRESS_LAST_LAYER_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[4].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[4].doc = "EGRESS_LAST_LAYER_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[4].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_t, EGRESS_LAST_LAYER_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[5].name = "EGRESS_LAST_LAYER_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[5].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[5].doc = "EGRESS_LAST_LAYER_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[5].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_t, EGRESS_LAST_LAYER_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[6].name = "VLAN_MEMBERSHIP_INTERFACE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[6].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[6].doc = "VLAN_MEMBERSHIP_INTERFACE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[6].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_t, VLAN_MEMBERSHIP_INTERFACE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[7].name = "VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[7].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[7].doc = "VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[7].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_t, VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[8].name = "ENCAP_DESTINATION_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[8].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[8].doc = "ENCAP_DESTINATION_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[8].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_t, ENCAP_DESTINATION_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[9].name = "ENCAP_DESTINATION_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[9].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[9].doc = "ENCAP_DESTINATION_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[9].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_t, ENCAP_DESTINATION_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[10].name = "QOS_MODEL_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[10].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[10].doc = "QOS_MODEL_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[10].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_t, QOS_MODEL_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[11].name = "QOS_MODEL_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[11].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[11].doc = "QOS_MODEL_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[11].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_t, QOS_MODEL_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[12].name = "FORWARD_DOMAIN_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[12].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[12].doc = "FORWARD_DOMAIN_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[12].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_t, FORWARD_DOMAIN_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[13].name = "FORWARD_DOMAIN_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[13].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[13].doc = "FORWARD_DOMAIN_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[13].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_t, FORWARD_DOMAIN_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[14].name = "FORWARD_DOMAIN_NETWORK_NAME_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[14].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[14].doc = "FORWARD_DOMAIN_NETWORK_NAME_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[14].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_t, FORWARD_DOMAIN_NETWORK_NAME_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[15].name = "FORWARD_DOMAIN_NETWORK_NAME_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[15].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[15].doc = "FORWARD_DOMAIN_NETWORK_NAME_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[15].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_t, FORWARD_DOMAIN_NETWORK_NAME_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[16].name = "LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[16].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[16].doc = "LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[16].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_t, LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[17].name = "LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[17].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[17].doc = "LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[17].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_t, LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[18].name = "MTU_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[18].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[18].doc = "MTU_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[18].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_t, MTU_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[19].name = "MTU_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[19].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[19].doc = "MTU_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[19].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_t, MTU_PROFILE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[20].name = "NWK_QOS_IDX_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[20].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[20].doc = "NWK_QOS_IDX_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[20].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_t, NWK_QOS_IDX_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[21].name = "NWK_QOS_IDX_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[21].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[21].doc = "NWK_QOS_IDX_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[21].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_t, NWK_QOS_IDX_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[22].name = "REMARK_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[22].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[22].doc = "REMARK_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[22].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_t, REMARK_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[23].name = "REMARK_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[23].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[23].doc = "REMARK_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[23].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_t, REMARK_PROFILE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[24].name = "SOURCE_IDX_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[24].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[24].doc = "SOURCE_IDX_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[24].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_t, SOURCE_IDX_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[25].name = "SOURCE_IDX_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[25].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[25].doc = "SOURCE_IDX_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[25].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_t, SOURCE_IDX_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[26].name = "LIF_STATISTICS_OBJECT_COMMAND_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[26].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[26].doc = "LIF_STATISTICS_OBJECT_COMMAND_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[26].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_t, LIF_STATISTICS_OBJECT_COMMAND_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[27].name = "LIF_STATISTICS_OBJECT_COMMAND_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[27].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[27].doc = "LIF_STATISTICS_OBJECT_COMMAND_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[27].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_t, LIF_STATISTICS_OBJECT_COMMAND_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[28].name = "LIF_STATISTICS_OBJECT_ID_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[28].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[28].doc = "LIF_STATISTICS_OBJECT_ID_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[28].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_t, LIF_STATISTICS_OBJECT_ID_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[29].name = "LIF_STATISTICS_OBJECT_ID_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[29].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[29].doc = "LIF_STATISTICS_OBJECT_ID_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[29].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_t, LIF_STATISTICS_OBJECT_ID_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[30].name = "TTL_MODEL_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[30].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[30].doc = "TTL_MODEL_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[30].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_t, TTL_MODEL_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[31].name = "TTL_MODEL_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[31].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[31].doc = "TTL_MODEL_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[31].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_t, TTL_MODEL_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[32].name = "TTL_IDX_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[32].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[32].doc = "TTL_IDX_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[32].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_t, TTL_IDX_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[33].name = "TTL_IDX_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[33].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[33].doc = "TTL_IDX_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[33].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_t, TTL_IDX_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[34].name = "VLAN_EDIT_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[34].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[34].doc = "VLAN_EDIT_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[34].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_t, VLAN_EDIT_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[35].name = "VLAN_EDIT_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[35].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[35].doc = "VLAN_EDIT_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE].values[35].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_t, VLAN_EDIT_PROFILE_CONFIG_valid);
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].name = "ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].doc = "ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].flags |= DNXC_DATA_F_TABLE;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].size_of_values = sizeof(dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_t);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].entry_get = dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_entry_str_get;

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].nof_keys = 1;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].keys[0].name = "index";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].keys[0].doc = "entry index";

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].nof_values = 36;
        DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].nof_values, "_dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE table values");
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[0].name = "valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[0].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[0].doc = "indicates whether entry is valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[0].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_t, valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[1].name = "doc";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[1].type = "char *";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[1].doc = "entry documentation";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[1].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_t, doc);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[2].name = "DUAL_HOMING_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[2].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[2].doc = "DUAL_HOMING_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[2].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_t, DUAL_HOMING_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[3].name = "DUAL_HOMING_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[3].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[3].doc = "DUAL_HOMING_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[3].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_t, DUAL_HOMING_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[4].name = "EGRESS_LAST_LAYER_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[4].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[4].doc = "EGRESS_LAST_LAYER_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[4].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_t, EGRESS_LAST_LAYER_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[5].name = "EGRESS_LAST_LAYER_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[5].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[5].doc = "EGRESS_LAST_LAYER_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[5].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_t, EGRESS_LAST_LAYER_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[6].name = "VLAN_MEMBERSHIP_INTERFACE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[6].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[6].doc = "VLAN_MEMBERSHIP_INTERFACE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[6].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_t, VLAN_MEMBERSHIP_INTERFACE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[7].name = "VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[7].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[7].doc = "VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[7].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_t, VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[8].name = "ENCAP_DESTINATION_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[8].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[8].doc = "ENCAP_DESTINATION_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[8].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_t, ENCAP_DESTINATION_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[9].name = "ENCAP_DESTINATION_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[9].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[9].doc = "ENCAP_DESTINATION_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[9].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_t, ENCAP_DESTINATION_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[10].name = "QOS_MODEL_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[10].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[10].doc = "QOS_MODEL_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[10].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_t, QOS_MODEL_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[11].name = "QOS_MODEL_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[11].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[11].doc = "QOS_MODEL_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[11].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_t, QOS_MODEL_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[12].name = "FORWARD_DOMAIN_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[12].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[12].doc = "FORWARD_DOMAIN_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[12].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_t, FORWARD_DOMAIN_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[13].name = "FORWARD_DOMAIN_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[13].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[13].doc = "FORWARD_DOMAIN_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[13].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_t, FORWARD_DOMAIN_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[14].name = "FORWARD_DOMAIN_NETWORK_NAME_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[14].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[14].doc = "FORWARD_DOMAIN_NETWORK_NAME_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[14].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_t, FORWARD_DOMAIN_NETWORK_NAME_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[15].name = "FORWARD_DOMAIN_NETWORK_NAME_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[15].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[15].doc = "FORWARD_DOMAIN_NETWORK_NAME_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[15].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_t, FORWARD_DOMAIN_NETWORK_NAME_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[16].name = "LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[16].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[16].doc = "LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[16].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_t, LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[17].name = "LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[17].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[17].doc = "LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[17].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_t, LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[18].name = "MTU_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[18].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[18].doc = "MTU_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[18].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_t, MTU_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[19].name = "MTU_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[19].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[19].doc = "MTU_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[19].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_t, MTU_PROFILE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[20].name = "NWK_QOS_IDX_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[20].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[20].doc = "NWK_QOS_IDX_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[20].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_t, NWK_QOS_IDX_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[21].name = "NWK_QOS_IDX_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[21].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[21].doc = "NWK_QOS_IDX_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[21].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_t, NWK_QOS_IDX_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[22].name = "REMARK_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[22].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[22].doc = "REMARK_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[22].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_t, REMARK_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[23].name = "REMARK_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[23].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[23].doc = "REMARK_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[23].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_t, REMARK_PROFILE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[24].name = "SOURCE_IDX_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[24].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[24].doc = "SOURCE_IDX_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[24].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_t, SOURCE_IDX_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[25].name = "SOURCE_IDX_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[25].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[25].doc = "SOURCE_IDX_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[25].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_t, SOURCE_IDX_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[26].name = "LIF_STATISTICS_OBJECT_COMMAND_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[26].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[26].doc = "LIF_STATISTICS_OBJECT_COMMAND_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[26].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_t, LIF_STATISTICS_OBJECT_COMMAND_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[27].name = "LIF_STATISTICS_OBJECT_COMMAND_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[27].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[27].doc = "LIF_STATISTICS_OBJECT_COMMAND_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[27].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_t, LIF_STATISTICS_OBJECT_COMMAND_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[28].name = "LIF_STATISTICS_OBJECT_ID_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[28].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[28].doc = "LIF_STATISTICS_OBJECT_ID_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[28].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_t, LIF_STATISTICS_OBJECT_ID_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[29].name = "LIF_STATISTICS_OBJECT_ID_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[29].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[29].doc = "LIF_STATISTICS_OBJECT_ID_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[29].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_t, LIF_STATISTICS_OBJECT_ID_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[30].name = "TTL_MODEL_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[30].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[30].doc = "TTL_MODEL_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[30].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_t, TTL_MODEL_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[31].name = "TTL_MODEL_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[31].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[31].doc = "TTL_MODEL_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[31].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_t, TTL_MODEL_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[32].name = "TTL_IDX_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[32].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[32].doc = "TTL_IDX_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[32].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_t, TTL_IDX_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[33].name = "TTL_IDX_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[33].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[33].doc = "TTL_IDX_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[33].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_t, TTL_IDX_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[34].name = "VLAN_EDIT_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[34].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[34].doc = "VLAN_EDIT_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[34].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_t, VLAN_EDIT_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[35].name = "VLAN_EDIT_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[35].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[35].doc = "VLAN_EDIT_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE].values[35].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_t, VLAN_EDIT_PROFILE_CONFIG_valid);
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE].name = "ETPPB_ENC_FORMAT_CONFIGURATION_TABLE";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE].doc = "ETPPB_ENC_FORMAT_CONFIGURATION_TABLE";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE].flags |= DNXC_DATA_F_TABLE;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE].size_of_values = sizeof(dnx_data_dnx2_arr_formats_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE_t);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE].entry_get = dnx_data_dnx2_arr_formats_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE_entry_str_get;

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE].nof_keys = 1;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE].keys[0].name = "index";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE].keys[0].doc = "entry index";

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE].nof_values = 8;
        DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE].nof_values, "_dnx_data_dnx2_arr_formats_table_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE table values");
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE].values[0].name = "valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE].values[0].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE].values[0].doc = "indicates whether entry is valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE].values[0].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE_t, valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE].values[1].name = "doc";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE].values[1].type = "char *";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE].values[1].doc = "entry documentation";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE].values[1].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE_t, doc);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE].values[2].name = "VLAN_MEMBERSHIP_INTERFACE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE].values[2].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE].values[2].doc = "VLAN_MEMBERSHIP_INTERFACE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE].values[2].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE_t, VLAN_MEMBERSHIP_INTERFACE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE].values[3].name = "VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE].values[3].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE].values[3].doc = "VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE].values[3].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE_t, VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE].values[4].name = "ENCAP_DESTINATION_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE].values[4].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE].values[4].doc = "ENCAP_DESTINATION_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE].values[4].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE_t, ENCAP_DESTINATION_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE].values[5].name = "ENCAP_DESTINATION_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE].values[5].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE].values[5].doc = "ENCAP_DESTINATION_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE].values[5].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE_t, ENCAP_DESTINATION_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE].values[6].name = "REMARK_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE].values[6].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE].values[6].doc = "REMARK_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE].values[6].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE_t, REMARK_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE].values[7].name = "REMARK_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE].values[7].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE].values[7].doc = "REMARK_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE].values[7].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE_t, REMARK_PROFILE_CONFIG_valid);
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE].name = "ETPPC_ENC_FORMAT_CONFIGURATION_TABLE";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE].doc = "ETPPC_ENC_FORMAT_CONFIGURATION_TABLE";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE].flags |= DNXC_DATA_F_TABLE;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE].size_of_values = sizeof(dnx_data_dnx2_arr_formats_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE_t);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE].entry_get = dnx_data_dnx2_arr_formats_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE_entry_str_get;

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE].nof_keys = 1;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE].keys[0].name = "index";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE].keys[0].doc = "entry index";

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE].nof_values = 8;
        DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE].nof_values, "_dnx_data_dnx2_arr_formats_table_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE table values");
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE].values[0].name = "valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE].values[0].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE].values[0].doc = "indicates whether entry is valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE].values[0].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE_t, valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE].values[1].name = "doc";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE].values[1].type = "char *";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE].values[1].doc = "entry documentation";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE].values[1].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE_t, doc);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE].values[2].name = "VLAN_MEMBERSHIP_INTERFACE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE].values[2].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE].values[2].doc = "VLAN_MEMBERSHIP_INTERFACE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE].values[2].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE_t, VLAN_MEMBERSHIP_INTERFACE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE].values[3].name = "VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE].values[3].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE].values[3].doc = "VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE].values[3].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE_t, VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE].values[4].name = "ENCAP_DESTINATION_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE].values[4].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE].values[4].doc = "ENCAP_DESTINATION_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE].values[4].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE_t, ENCAP_DESTINATION_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE].values[5].name = "ENCAP_DESTINATION_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE].values[5].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE].values[5].doc = "ENCAP_DESTINATION_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE].values[5].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE_t, ENCAP_DESTINATION_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE].values[6].name = "REMARK_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE].values[6].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE].values[6].doc = "REMARK_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE].values[6].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE_t, REMARK_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE].values[7].name = "REMARK_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE].values[7].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE].values[7].doc = "REMARK_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE].values[7].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE_t, REMARK_PROFILE_CONFIG_valid);
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE].name = "ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE].doc = "ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE].flags |= DNXC_DATA_F_TABLE;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE].size_of_values = sizeof(dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE_t);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE].entry_get = dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE_entry_str_get;

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE].nof_keys = 1;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE].keys[0].name = "index";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE].keys[0].doc = "entry index";

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE].nof_values = 8;
        DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE].nof_values, "_dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE table values");
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE].values[0].name = "valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE].values[0].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE].values[0].doc = "indicates whether entry is valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE].values[0].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE_t, valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE].values[1].name = "doc";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE].values[1].type = "char *";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE].values[1].doc = "entry documentation";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE].values[1].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE_t, doc);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE].values[2].name = "VLAN_MEMBERSHIP_INTERFACE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE].values[2].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE].values[2].doc = "VLAN_MEMBERSHIP_INTERFACE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE].values[2].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE_t, VLAN_MEMBERSHIP_INTERFACE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE].values[3].name = "VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE].values[3].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE].values[3].doc = "VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE].values[3].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE_t, VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE].values[4].name = "ENCAP_DESTINATION_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE].values[4].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE].values[4].doc = "ENCAP_DESTINATION_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE].values[4].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE_t, ENCAP_DESTINATION_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE].values[5].name = "ENCAP_DESTINATION_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE].values[5].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE].values[5].doc = "ENCAP_DESTINATION_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE].values[5].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE_t, ENCAP_DESTINATION_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE].values[6].name = "REMARK_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE].values[6].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE].values[6].doc = "REMARK_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE].values[6].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE_t, REMARK_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE].values[7].name = "REMARK_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE].values[7].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE].values[7].doc = "REMARK_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE].values[7].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE_t, REMARK_PROFILE_CONFIG_valid);
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE].name = "ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE].doc = "ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE].flags |= DNXC_DATA_F_TABLE;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE].size_of_values = sizeof(dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE_t);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE].entry_get = dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE_entry_str_get;

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE].nof_keys = 1;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE].keys[0].name = "index";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE].keys[0].doc = "entry index";

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE].nof_values = 8;
        DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE].nof_values, "_dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE table values");
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE].values[0].name = "valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE].values[0].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE].values[0].doc = "indicates whether entry is valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE].values[0].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE_t, valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE].values[1].name = "doc";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE].values[1].type = "char *";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE].values[1].doc = "entry documentation";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE].values[1].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE_t, doc);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE].values[2].name = "VLAN_MEMBERSHIP_INTERFACE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE].values[2].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE].values[2].doc = "VLAN_MEMBERSHIP_INTERFACE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE].values[2].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE_t, VLAN_MEMBERSHIP_INTERFACE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE].values[3].name = "VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE].values[3].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE].values[3].doc = "VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE].values[3].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE_t, VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE].values[4].name = "ENCAP_DESTINATION_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE].values[4].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE].values[4].doc = "ENCAP_DESTINATION_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE].values[4].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE_t, ENCAP_DESTINATION_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE].values[5].name = "ENCAP_DESTINATION_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE].values[5].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE].values[5].doc = "ENCAP_DESTINATION_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE].values[5].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE_t, ENCAP_DESTINATION_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE].values[6].name = "REMARK_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE].values[6].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE].values[6].doc = "REMARK_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE].values[6].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE_t, REMARK_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE].values[7].name = "REMARK_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE].values[7].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE].values[7].doc = "REMARK_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE].values[7].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE_t, REMARK_PROFILE_CONFIG_valid);
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE].name = "ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE].doc = "ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE].flags |= DNXC_DATA_F_TABLE;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE].size_of_values = sizeof(dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE_t);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE].entry_get = dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE_entry_str_get;

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE].nof_keys = 1;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE].keys[0].name = "index";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE].keys[0].doc = "entry index";

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE].nof_values = 8;
        DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE].nof_values, "_dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE table values");
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE].values[0].name = "valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE].values[0].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE].values[0].doc = "indicates whether entry is valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE].values[0].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE_t, valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE].values[1].name = "doc";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE].values[1].type = "char *";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE].values[1].doc = "entry documentation";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE].values[1].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE_t, doc);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE].values[2].name = "VLAN_MEMBERSHIP_INTERFACE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE].values[2].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE].values[2].doc = "VLAN_MEMBERSHIP_INTERFACE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE].values[2].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE_t, VLAN_MEMBERSHIP_INTERFACE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE].values[3].name = "VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE].values[3].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE].values[3].doc = "VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE].values[3].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE_t, VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE].values[4].name = "ENCAP_DESTINATION_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE].values[4].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE].values[4].doc = "ENCAP_DESTINATION_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE].values[4].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE_t, ENCAP_DESTINATION_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE].values[5].name = "ENCAP_DESTINATION_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE].values[5].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE].values[5].doc = "ENCAP_DESTINATION_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE].values[5].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE_t, ENCAP_DESTINATION_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE].values[6].name = "REMARK_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE].values[6].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE].values[6].doc = "REMARK_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE].values[6].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE_t, REMARK_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE].values[7].name = "REMARK_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE].values[7].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE].values[7].doc = "REMARK_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE].values[7].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE_t, REMARK_PROFILE_CONFIG_valid);
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE].name = "ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE].doc = "ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE].flags |= DNXC_DATA_F_TABLE;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE].size_of_values = sizeof(dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE_t);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE].entry_get = dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE_entry_str_get;

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE].nof_keys = 1;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE].keys[0].name = "index";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE].keys[0].doc = "entry index";

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE].nof_values = 8;
        DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE].nof_values, "_dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE table values");
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE].values[0].name = "valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE].values[0].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE].values[0].doc = "indicates whether entry is valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE].values[0].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE_t, valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE].values[1].name = "doc";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE].values[1].type = "char *";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE].values[1].doc = "entry documentation";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE].values[1].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE_t, doc);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE].values[2].name = "VLAN_MEMBERSHIP_INTERFACE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE].values[2].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE].values[2].doc = "VLAN_MEMBERSHIP_INTERFACE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE].values[2].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE_t, VLAN_MEMBERSHIP_INTERFACE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE].values[3].name = "VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE].values[3].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE].values[3].doc = "VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE].values[3].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE_t, VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE].values[4].name = "ENCAP_DESTINATION_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE].values[4].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE].values[4].doc = "ENCAP_DESTINATION_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE].values[4].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE_t, ENCAP_DESTINATION_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE].values[5].name = "ENCAP_DESTINATION_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE].values[5].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE].values[5].doc = "ENCAP_DESTINATION_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE].values[5].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE_t, ENCAP_DESTINATION_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE].values[6].name = "REMARK_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE].values[6].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE].values[6].doc = "REMARK_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE].values[6].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE_t, REMARK_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE].values[7].name = "REMARK_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE].values[7].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE].values[7].doc = "REMARK_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE].values[7].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE_t, REMARK_PROFILE_CONFIG_valid);
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE].name = "ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE].doc = "ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE].flags |= DNXC_DATA_F_TABLE;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE].size_of_values = sizeof(dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE_t);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE].entry_get = dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE_entry_str_get;

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE].nof_keys = 1;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE].keys[0].name = "index";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE].keys[0].doc = "entry index";

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE].nof_values = 8;
        DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE].nof_values, "_dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE table values");
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE].values[0].name = "valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE].values[0].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE].values[0].doc = "indicates whether entry is valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE].values[0].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE_t, valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE].values[1].name = "doc";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE].values[1].type = "char *";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE].values[1].doc = "entry documentation";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE].values[1].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE_t, doc);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE].values[2].name = "VLAN_MEMBERSHIP_INTERFACE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE].values[2].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE].values[2].doc = "VLAN_MEMBERSHIP_INTERFACE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE].values[2].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE_t, VLAN_MEMBERSHIP_INTERFACE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE].values[3].name = "VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE].values[3].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE].values[3].doc = "VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE].values[3].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE_t, VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE].values[4].name = "ENCAP_DESTINATION_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE].values[4].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE].values[4].doc = "ENCAP_DESTINATION_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE].values[4].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE_t, ENCAP_DESTINATION_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE].values[5].name = "ENCAP_DESTINATION_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE].values[5].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE].values[5].doc = "ENCAP_DESTINATION_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE].values[5].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE_t, ENCAP_DESTINATION_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE].values[6].name = "REMARK_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE].values[6].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE].values[6].doc = "REMARK_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE].values[6].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE_t, REMARK_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE].values[7].name = "REMARK_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE].values[7].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE].values[7].doc = "REMARK_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE].values[7].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE_t, REMARK_PROFILE_CONFIG_valid);
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].name = "ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].doc = "ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].flags |= DNXC_DATA_F_TABLE;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].size_of_values = sizeof(dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_t);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].entry_get = dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_entry_str_get;

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].nof_keys = 1;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].keys[0].name = "index";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].keys[0].doc = "entry index";

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].nof_values = 28;
        DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].nof_values, "_dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0 table values");
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[0].name = "valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[0].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[0].doc = "indicates whether entry is valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[0].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_t, valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[1].name = "doc";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[1].type = "char *";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[1].doc = "entry documentation";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[1].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_t, doc);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[2].name = "ETHERNET_EGRESS_LAST_LAYER_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[2].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[2].doc = "ETHERNET_EGRESS_LAST_LAYER_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[2].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_t, ETHERNET_EGRESS_LAST_LAYER_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[3].name = "ETHERNET_EGRESS_LAST_LAYER_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[3].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[3].doc = "ETHERNET_EGRESS_LAST_LAYER_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[3].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_t, ETHERNET_EGRESS_LAST_LAYER_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[4].name = "ETHERNET_EGRESS_VLAN_MEMBERSHIP_INTERFACE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[4].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[4].doc = "ETHERNET_EGRESS_VLAN_MEMBERSHIP_INTERFACE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[4].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_t, ETHERNET_EGRESS_VLAN_MEMBERSHIP_INTERFACE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[5].name = "ETHERNET_EGRESS_VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[5].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[5].doc = "ETHERNET_EGRESS_VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[5].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_t, ETHERNET_EGRESS_VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[6].name = "ENCAP_DESTINATION_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[6].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[6].doc = "ENCAP_DESTINATION_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[6].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_t, ENCAP_DESTINATION_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[7].name = "ENCAP_DESTINATION_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[7].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[7].doc = "ENCAP_DESTINATION_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[7].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_t, ENCAP_DESTINATION_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[8].name = "FORWARDING_DOMAIN_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[8].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[8].doc = "FORWARDING_DOMAIN_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[8].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_t, FORWARDING_DOMAIN_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[9].name = "FORWARDING_DOMAIN_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[9].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[9].doc = "FORWARDING_DOMAIN_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[9].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_t, FORWARDING_DOMAIN_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[10].name = "QOS_MODEL_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[10].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[10].doc = "QOS_MODEL_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[10].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_t, QOS_MODEL_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[11].name = "QOS_MODEL_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[11].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[11].doc = "QOS_MODEL_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[11].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_t, QOS_MODEL_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[12].name = "MTU_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[12].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[12].doc = "MTU_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[12].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_t, MTU_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[13].name = "MTU_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[13].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[13].doc = "MTU_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[13].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_t, MTU_PROFILE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[14].name = "NWK_QOS_INDEX_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[14].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[14].doc = "NWK_QOS_INDEX_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[14].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_t, NWK_QOS_INDEX_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[15].name = "NWK_QOS_INDEX_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[15].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[15].doc = "NWK_QOS_INDEX_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[15].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_t, NWK_QOS_INDEX_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[16].name = "ETHERNET_OUTLIF_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[16].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[16].doc = "ETHERNET_OUTLIF_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[16].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_t, ETHERNET_OUTLIF_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[17].name = "ETHERNET_OUTLIF_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[17].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[17].doc = "ETHERNET_OUTLIF_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[17].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_t, ETHERNET_OUTLIF_PROFILE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[18].name = "REMARK_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[18].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[18].doc = "REMARK_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[18].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_t, REMARK_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[19].name = "REMARK_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[19].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[19].doc = "REMARK_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[19].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_t, REMARK_PROFILE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[20].name = "LIF_STATISTICS_OBJECT_COMMAND_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[20].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[20].doc = "LIF_STATISTICS_OBJECT_COMMAND_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[20].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_t, LIF_STATISTICS_OBJECT_COMMAND_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[21].name = "LIF_STATISTICS_OBJECT_COMMAND_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[21].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[21].doc = "LIF_STATISTICS_OBJECT_COMMAND_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[21].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_t, LIF_STATISTICS_OBJECT_COMMAND_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[22].name = "LIF_STATISTICS_OBJECT_ID_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[22].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[22].doc = "LIF_STATISTICS_OBJECT_ID_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[22].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_t, LIF_STATISTICS_OBJECT_ID_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[23].name = "LIF_STATISTICS_OBJECT_ID_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[23].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[23].doc = "LIF_STATISTICS_OBJECT_ID_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[23].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_t, LIF_STATISTICS_OBJECT_ID_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[24].name = "TTL_MODEL_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[24].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[24].doc = "TTL_MODEL_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[24].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_t, TTL_MODEL_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[25].name = "TTL_MODEL_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[25].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[25].doc = "TTL_MODEL_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[25].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_t, TTL_MODEL_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[26].name = "ETHERNET_EGRESS_VLAN_EDIT_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[26].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[26].doc = "ETHERNET_EGRESS_VLAN_EDIT_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[26].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_t, ETHERNET_EGRESS_VLAN_EDIT_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[27].name = "ETHERNET_EGRESS_VLAN_EDIT_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[27].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[27].doc = "ETHERNET_EGRESS_VLAN_EDIT_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0].values[27].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_t, ETHERNET_EGRESS_VLAN_EDIT_PROFILE_CONFIG_valid);
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1].name = "ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1].doc = "ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1].flags |= DNXC_DATA_F_TABLE;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1].size_of_values = sizeof(dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1_t);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1].entry_get = dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1_entry_str_get;

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1].nof_keys = 1;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1].keys[0].name = "index";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1].keys[0].doc = "entry index";

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1].nof_values = 8;
        DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1].nof_values, "_dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1 table values");
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1].values[0].name = "valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1].values[0].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1].values[0].doc = "indicates whether entry is valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1].values[0].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1_t, valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1].values[1].name = "doc";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1].values[1].type = "char *";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1].values[1].doc = "entry documentation";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1].values[1].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1_t, doc);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1].values[2].name = "ETHERNET_EGRESS_VLAN_MEMBERSHIP_INTERFACE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1].values[2].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1].values[2].doc = "ETHERNET_EGRESS_VLAN_MEMBERSHIP_INTERFACE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1].values[2].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1_t, ETHERNET_EGRESS_VLAN_MEMBERSHIP_INTERFACE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1].values[3].name = "ETHERNET_EGRESS_VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1].values[3].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1].values[3].doc = "ETHERNET_EGRESS_VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1].values[3].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1_t, ETHERNET_EGRESS_VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1].values[4].name = "ENCAP_DESTINATION_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1].values[4].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1].values[4].doc = "ENCAP_DESTINATION_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1].values[4].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1_t, ENCAP_DESTINATION_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1].values[5].name = "ENCAP_DESTINATION_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1].values[5].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1].values[5].doc = "ENCAP_DESTINATION_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1].values[5].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1_t, ENCAP_DESTINATION_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1].values[6].name = "REMARK_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1].values[6].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1].values[6].doc = "REMARK_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1].values[6].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1_t, REMARK_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1].values[7].name = "REMARK_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1].values[7].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1].values[7].doc = "REMARK_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1].values[7].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1_t, REMARK_PROFILE_CONFIG_valid);
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].name = "ETPPA_PRP_EES_ARR_CONFIGURATION";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].doc = "ETPPA_PRP_EES_ARR_CONFIGURATION";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].flags |= DNXC_DATA_F_TABLE;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].size_of_values = sizeof(dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION_t);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].entry_get = dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION_entry_str_get;

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].nof_keys = 1;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].keys[0].name = "index";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].keys[0].doc = "entry index";

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].nof_values = 14;
        DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].nof_values, "_dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION table values");
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[0].name = "valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[0].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[0].doc = "indicates whether entry is valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[0].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION_t, valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[1].name = "doc";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[1].type = "char *";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[1].doc = "entry documentation";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[1].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION_t, doc);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[2].name = "LLVP_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[2].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[2].doc = "LLVP_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[2].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION_t, LLVP_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[3].name = "LLVP_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[3].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[3].doc = "LLVP_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[3].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION_t, LLVP_PROFILE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[4].name = "EM_ACCESS_CMD_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[4].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[4].doc = "EM_ACCESS_CMD_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[4].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION_t, EM_ACCESS_CMD_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[5].name = "EM_ACCESS_CMD_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[5].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[5].doc = "EM_ACCESS_CMD_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[5].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION_t, EM_ACCESS_CMD_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[6].name = "NAME_SPACE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[6].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[6].doc = "NAME_SPACE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[6].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION_t, NAME_SPACE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[7].name = "NAME_SPACE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[7].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[7].doc = "NAME_SPACE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[7].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION_t, NAME_SPACE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[8].name = "FWD_DOMAIN_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[8].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[8].doc = "FWD_DOMAIN_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[8].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION_t, FWD_DOMAIN_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[9].name = "FWD_DOMAIN_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[9].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[9].doc = "FWD_DOMAIN_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[9].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION_t, FWD_DOMAIN_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[10].name = "OAM_LIF_SET_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[10].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[10].doc = "OAM_LIF_SET_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[10].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION_t, OAM_LIF_SET_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[11].name = "OAM_LIF_SET_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[11].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[11].doc = "OAM_LIF_SET_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[11].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION_t, OAM_LIF_SET_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[12].name = "OUTLIF_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[12].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[12].doc = "OUTLIF_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[12].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION_t, OUTLIF_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[13].name = "OUTLIF_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[13].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[13].doc = "OUTLIF_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION].values[13].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION_t, OUTLIF_PROFILE_CONFIG_valid);
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].name = "ETPPA_PRP_EES_ARR_CONFIGURATION___array";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].doc = "ETPPA_PRP_EES_ARR_CONFIGURATION";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].flags |= DNXC_DATA_F_TABLE;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].size_of_values = sizeof(dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION___array_t);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].entry_get = dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION___array_entry_str_get;

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].nof_keys = 2;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].keys[0].name = "table_index";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].keys[0].doc = "table index";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].keys[1].name = "index";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].keys[1].doc = "entry index";

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].nof_values = 14;
        DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].nof_values, "_dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array table values");
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[0].name = "valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[0].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[0].doc = "indicates whether entry is valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[0].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION___array_t, valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[1].name = "doc";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[1].type = "char *";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[1].doc = "entry documentation";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[1].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION___array_t, doc);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[2].name = "LLVP_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[2].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[2].doc = "LLVP_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[2].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION___array_t, LLVP_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[3].name = "LLVP_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[3].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[3].doc = "LLVP_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[3].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION___array_t, LLVP_PROFILE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[4].name = "EM_ACCESS_CMD_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[4].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[4].doc = "EM_ACCESS_CMD_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[4].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION___array_t, EM_ACCESS_CMD_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[5].name = "EM_ACCESS_CMD_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[5].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[5].doc = "EM_ACCESS_CMD_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[5].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION___array_t, EM_ACCESS_CMD_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[6].name = "NAME_SPACE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[6].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[6].doc = "NAME_SPACE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[6].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION___array_t, NAME_SPACE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[7].name = "NAME_SPACE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[7].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[7].doc = "NAME_SPACE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[7].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION___array_t, NAME_SPACE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[8].name = "FWD_DOMAIN_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[8].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[8].doc = "FWD_DOMAIN_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[8].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION___array_t, FWD_DOMAIN_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[9].name = "FWD_DOMAIN_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[9].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[9].doc = "FWD_DOMAIN_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[9].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION___array_t, FWD_DOMAIN_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[10].name = "OAM_LIF_SET_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[10].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[10].doc = "OAM_LIF_SET_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[10].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION___array_t, OAM_LIF_SET_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[11].name = "OAM_LIF_SET_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[11].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[11].doc = "OAM_LIF_SET_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[11].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION___array_t, OAM_LIF_SET_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[12].name = "OUTLIF_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[12].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[12].doc = "OUTLIF_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[12].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION___array_t, OUTLIF_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[13].name = "OUTLIF_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[13].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[13].doc = "OUTLIF_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array].values[13].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION___array_t, OUTLIF_PROFILE_CONFIG_valid);
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].name = "ETPPC_TERM_ETPS_ARR_1_CONFIGURATION";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].doc = "ETPPC_TERM_ETPS_ARR_1_CONFIGURATION";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].flags |= DNXC_DATA_F_TABLE;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].size_of_values = sizeof(dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION_t);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].entry_get = dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION_entry_str_get;

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].nof_keys = 1;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].keys[0].name = "index";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].keys[0].doc = "entry index";

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].nof_values = 12;
        DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].nof_values, "_dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION table values");
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].values[0].name = "valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].values[0].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].values[0].doc = "indicates whether entry is valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].values[0].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION_t, valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].values[1].name = "doc";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].values[1].type = "char *";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].values[1].doc = "entry documentation";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].values[1].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION_t, doc);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].values[2].name = "ACTION_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].values[2].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].values[2].doc = "ACTION_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].values[2].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION_t, ACTION_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].values[3].name = "ACTION_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].values[3].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].values[3].doc = "ACTION_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].values[3].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION_t, ACTION_PROFILE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].values[4].name = "PROTECTION_PATH_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].values[4].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].values[4].doc = "PROTECTION_PATH_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].values[4].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION_t, PROTECTION_PATH_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].values[5].name = "PROTECTION_PATH_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].values[5].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].values[5].doc = "PROTECTION_PATH_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].values[5].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION_t, PROTECTION_PATH_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].values[6].name = "PROTECTION_PTR_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].values[6].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].values[6].doc = "PROTECTION_PTR_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].values[6].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION_t, PROTECTION_PTR_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].values[7].name = "PROTECTION_PTR_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].values[7].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].values[7].doc = "PROTECTION_PTR_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].values[7].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION_t, PROTECTION_PTR_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].values[8].name = "STAT_OBJ_CMD_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].values[8].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].values[8].doc = "STAT_OBJ_CMD_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].values[8].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION_t, STAT_OBJ_CMD_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].values[9].name = "STAT_OBJ_CMD_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].values[9].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].values[9].doc = "STAT_OBJ_CMD_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].values[9].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION_t, STAT_OBJ_CMD_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].values[10].name = "STAT_OBJ_ID_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].values[10].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].values[10].doc = "STAT_OBJ_ID_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].values[10].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION_t, STAT_OBJ_ID_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].values[11].name = "STAT_OBJ_ID_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].values[11].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].values[11].doc = "STAT_OBJ_ID_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION].values[11].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION_t, STAT_OBJ_ID_CONFIG_valid);
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].name = "ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].doc = "ETPPC_TERM_ETPS_ARR_1_CONFIGURATION";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].flags |= DNXC_DATA_F_TABLE;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].size_of_values = sizeof(dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_t);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].entry_get = dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_entry_str_get;

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].nof_keys = 2;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].keys[0].name = "table_index";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].keys[0].doc = "table index";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].keys[1].name = "index";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].keys[1].doc = "entry index";

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].nof_values = 12;
        DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].nof_values, "_dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array table values");
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].values[0].name = "valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].values[0].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].values[0].doc = "indicates whether entry is valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].values[0].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_t, valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].values[1].name = "doc";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].values[1].type = "char *";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].values[1].doc = "entry documentation";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].values[1].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_t, doc);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].values[2].name = "ACTION_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].values[2].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].values[2].doc = "ACTION_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].values[2].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_t, ACTION_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].values[3].name = "ACTION_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].values[3].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].values[3].doc = "ACTION_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].values[3].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_t, ACTION_PROFILE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].values[4].name = "PROTECTION_PATH_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].values[4].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].values[4].doc = "PROTECTION_PATH_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].values[4].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_t, PROTECTION_PATH_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].values[5].name = "PROTECTION_PATH_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].values[5].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].values[5].doc = "PROTECTION_PATH_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].values[5].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_t, PROTECTION_PATH_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].values[6].name = "PROTECTION_PTR_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].values[6].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].values[6].doc = "PROTECTION_PTR_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].values[6].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_t, PROTECTION_PTR_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].values[7].name = "PROTECTION_PTR_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].values[7].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].values[7].doc = "PROTECTION_PTR_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].values[7].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_t, PROTECTION_PTR_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].values[8].name = "STAT_OBJ_CMD_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].values[8].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].values[8].doc = "STAT_OBJ_CMD_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].values[8].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_t, STAT_OBJ_CMD_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].values[9].name = "STAT_OBJ_CMD_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].values[9].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].values[9].doc = "STAT_OBJ_CMD_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].values[9].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_t, STAT_OBJ_CMD_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].values[10].name = "STAT_OBJ_ID_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].values[10].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].values[10].doc = "STAT_OBJ_ID_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].values[10].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_t, STAT_OBJ_ID_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].values[11].name = "STAT_OBJ_ID_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].values[11].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].values[11].doc = "STAT_OBJ_ID_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array].values[11].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_t, STAT_OBJ_ID_CONFIG_valid);
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].name = "ETPPC_TERM_ETPS_ARR_2_CONFIGURATION";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].doc = "ETPPC_TERM_ETPS_ARR_2_CONFIGURATION";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].flags |= DNXC_DATA_F_TABLE;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].size_of_values = sizeof(dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION_t);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].entry_get = dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION_entry_str_get;

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].nof_keys = 1;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].keys[0].name = "index";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].keys[0].doc = "entry index";

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].nof_values = 12;
        DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].nof_values, "_dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION table values");
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].values[0].name = "valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].values[0].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].values[0].doc = "indicates whether entry is valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].values[0].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION_t, valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].values[1].name = "doc";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].values[1].type = "char *";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].values[1].doc = "entry documentation";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].values[1].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION_t, doc);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].values[2].name = "QOS_MODEL_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].values[2].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].values[2].doc = "QOS_MODEL_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].values[2].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION_t, QOS_MODEL_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].values[3].name = "QOS_MODEL_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].values[3].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].values[3].doc = "QOS_MODEL_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].values[3].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION_t, QOS_MODEL_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].values[4].name = "NWK_QOS_IDX_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].values[4].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].values[4].doc = "NWK_QOS_IDX_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].values[4].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION_t, NWK_QOS_IDX_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].values[5].name = "NWK_QOS_IDX_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].values[5].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].values[5].doc = "NWK_QOS_IDX_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].values[5].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION_t, NWK_QOS_IDX_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].values[6].name = "OUTLIF_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].values[6].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].values[6].doc = "OUTLIF_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].values[6].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION_t, OUTLIF_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].values[7].name = "OUTLIF_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].values[7].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].values[7].doc = "OUTLIF_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].values[7].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION_t, OUTLIF_PROFILE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].values[8].name = "REMARK_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].values[8].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].values[8].doc = "REMARK_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].values[8].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION_t, REMARK_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].values[9].name = "REMARK_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].values[9].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].values[9].doc = "REMARK_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].values[9].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION_t, REMARK_PROFILE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].values[10].name = "TTL_MODEL_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].values[10].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].values[10].doc = "TTL_MODEL_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].values[10].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION_t, TTL_MODEL_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].values[11].name = "TTL_MODEL_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].values[11].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].values[11].doc = "TTL_MODEL_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION].values[11].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION_t, TTL_MODEL_CONFIG_valid);
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].name = "IPPB_ACCEPTED_ARR_FORMAT_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].doc = "IPPB_ACCEPTED_ARR_FORMAT_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].flags |= DNXC_DATA_F_TABLE;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].size_of_values = sizeof(dnx_data_dnx2_arr_formats_IPPB_ACCEPTED_ARR_FORMAT_CONFIG_t);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].entry_get = dnx_data_dnx2_arr_formats_IPPB_ACCEPTED_ARR_FORMAT_CONFIG_entry_str_get;

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].nof_keys = 1;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].keys[0].name = "index";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].keys[0].doc = "entry index";

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].nof_values = 12;
        DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].nof_values, "_dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG table values");
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].values[0].name = "valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].values[0].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].values[0].doc = "indicates whether entry is valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].values[0].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_ACCEPTED_ARR_FORMAT_CONFIG_t, valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].values[1].name = "doc";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].values[1].type = "char *";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].values[1].doc = "entry documentation";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].values[1].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_ACCEPTED_ARR_FORMAT_CONFIG_t, doc);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].values[2].name = "DATA_1";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].values[2].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].values[2].doc = "DATA_1";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].values[2].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_ACCEPTED_ARR_FORMAT_CONFIG_t, DATA_1);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].values[3].name = "DATA_1_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].values[3].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].values[3].doc = "DATA_1_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].values[3].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_ACCEPTED_ARR_FORMAT_CONFIG_t, DATA_1_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].values[4].name = "DATA_2";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].values[4].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].values[4].doc = "DATA_2";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].values[4].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_ACCEPTED_ARR_FORMAT_CONFIG_t, DATA_2);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].values[5].name = "DATA_2_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].values[5].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].values[5].doc = "DATA_2_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].values[5].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_ACCEPTED_ARR_FORMAT_CONFIG_t, DATA_2_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].values[6].name = "TYPE";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].values[6].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].values[6].doc = "TYPE";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].values[6].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_ACCEPTED_ARR_FORMAT_CONFIG_t, TYPE);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].values[7].name = "TYPE_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].values[7].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].values[7].doc = "TYPE_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].values[7].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_ACCEPTED_ARR_FORMAT_CONFIG_t, TYPE_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].values[8].name = "DESTINATION";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].values[8].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].values[8].doc = "DESTINATION";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].values[8].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_ACCEPTED_ARR_FORMAT_CONFIG_t, DESTINATION);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].values[9].name = "DESTINATION_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].values[9].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].values[9].doc = "DESTINATION_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].values[9].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_ACCEPTED_ARR_FORMAT_CONFIG_t, DESTINATION_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].values[10].name = "STRENGTH";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].values[10].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].values[10].doc = "STRENGTH";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].values[10].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_ACCEPTED_ARR_FORMAT_CONFIG_t, STRENGTH);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].values[11].name = "STRENGTH_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].values[11].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].values[11].doc = "STRENGTH_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG].values[11].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_ACCEPTED_ARR_FORMAT_CONFIG_t, STRENGTH_valid);
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_MRQ_ARR_CFG].name = "MACT_MRQ_ARR_CFG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_MRQ_ARR_CFG].doc = "MACT_MRQ_ARR_CFG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_MRQ_ARR_CFG].flags |= DNXC_DATA_F_TABLE;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_MRQ_ARR_CFG].size_of_values = sizeof(dnx_data_dnx2_arr_formats_MACT_MRQ_ARR_CFG_t);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_MRQ_ARR_CFG].entry_get = dnx_data_dnx2_arr_formats_MACT_MRQ_ARR_CFG_entry_str_get;

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_MRQ_ARR_CFG].nof_keys = 1;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_MRQ_ARR_CFG].keys[0].name = "index";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_MRQ_ARR_CFG].keys[0].doc = "entry index";

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_MRQ_ARR_CFG].nof_values = 4;
        DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_MRQ_ARR_CFG].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_MRQ_ARR_CFG].nof_values, "_dnx_data_dnx2_arr_formats_table_MACT_MRQ_ARR_CFG table values");
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_MRQ_ARR_CFG].values[0].name = "valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_MRQ_ARR_CFG].values[0].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_MRQ_ARR_CFG].values[0].doc = "indicates whether entry is valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_MRQ_ARR_CFG].values[0].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_MACT_MRQ_ARR_CFG_t, valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_MRQ_ARR_CFG].values[1].name = "doc";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_MRQ_ARR_CFG].values[1].type = "char *";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_MRQ_ARR_CFG].values[1].doc = "entry documentation";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_MRQ_ARR_CFG].values[1].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_MACT_MRQ_ARR_CFG_t, doc);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_MRQ_ARR_CFG].values[2].name = "STRENGTH";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_MRQ_ARR_CFG].values[2].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_MRQ_ARR_CFG].values[2].doc = "STRENGTH";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_MRQ_ARR_CFG].values[2].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_MACT_MRQ_ARR_CFG_t, STRENGTH);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_MRQ_ARR_CFG].values[3].name = "STRENGTH_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_MRQ_ARR_CFG].values[3].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_MRQ_ARR_CFG].values[3].doc = "STRENGTH_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_MACT_MRQ_ARR_CFG].values[3].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_MACT_MRQ_ARR_CFG_t, STRENGTH_valid);
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].name = "IPPB_EXPECTED_ARR_FORMAT_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].doc = "IPPB_EXPECTED_ARR_FORMAT_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].flags |= DNXC_DATA_F_TABLE;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].size_of_values = sizeof(dnx_data_dnx2_arr_formats_IPPB_EXPECTED_ARR_FORMAT_CONFIG_t);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].entry_get = dnx_data_dnx2_arr_formats_IPPB_EXPECTED_ARR_FORMAT_CONFIG_entry_str_get;

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].nof_keys = 1;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].keys[0].name = "index";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].keys[0].doc = "entry index";

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].nof_values = 12;
        DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].nof_values, "_dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG table values");
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].values[0].name = "valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].values[0].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].values[0].doc = "indicates whether entry is valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].values[0].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_EXPECTED_ARR_FORMAT_CONFIG_t, valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].values[1].name = "doc";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].values[1].type = "char *";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].values[1].doc = "entry documentation";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].values[1].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_EXPECTED_ARR_FORMAT_CONFIG_t, doc);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].values[2].name = "DESTINATION";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].values[2].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].values[2].doc = "DESTINATION";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].values[2].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_EXPECTED_ARR_FORMAT_CONFIG_t, DESTINATION);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].values[3].name = "DESTINATION_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].values[3].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].values[3].doc = "DESTINATION_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].values[3].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_EXPECTED_ARR_FORMAT_CONFIG_t, DESTINATION_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].values[4].name = "DATA_1";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].values[4].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].values[4].doc = "DATA_1";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].values[4].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_EXPECTED_ARR_FORMAT_CONFIG_t, DATA_1);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].values[5].name = "DATA_1_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].values[5].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].values[5].doc = "DATA_1_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].values[5].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_EXPECTED_ARR_FORMAT_CONFIG_t, DATA_1_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].values[6].name = "DATA_2";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].values[6].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].values[6].doc = "DATA_2";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].values[6].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_EXPECTED_ARR_FORMAT_CONFIG_t, DATA_2);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].values[7].name = "DATA_2_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].values[7].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].values[7].doc = "DATA_2_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].values[7].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_EXPECTED_ARR_FORMAT_CONFIG_t, DATA_2_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].values[8].name = "TYPE";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].values[8].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].values[8].doc = "TYPE";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].values[8].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_EXPECTED_ARR_FORMAT_CONFIG_t, TYPE);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].values[9].name = "TYPE_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].values[9].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].values[9].doc = "TYPE_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].values[9].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_EXPECTED_ARR_FORMAT_CONFIG_t, TYPE_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].values[10].name = "STRENGTH";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].values[10].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].values[10].doc = "STRENGTH";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].values[10].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_EXPECTED_ARR_FORMAT_CONFIG_t, STRENGTH);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].values[11].name = "STRENGTH_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].values[11].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].values[11].doc = "STRENGTH_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG].values[11].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_EXPECTED_ARR_FORMAT_CONFIG_t, STRENGTH_valid);
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].name = "IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].doc = "IPPD_FEC_FORMAT_CONFIGURATION_TABLE";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].flags |= DNXC_DATA_F_TABLE;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].size_of_values = sizeof(dnx_data_dnx2_arr_formats_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array_t);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].entry_get = dnx_data_dnx2_arr_formats_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array_entry_str_get;

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].nof_keys = 2;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].keys[0].name = "table_index";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].keys[0].doc = "table index";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].keys[1].name = "index";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].keys[1].doc = "entry index";

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].nof_values = 14;
        DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].nof_values, "_dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array table values");
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[0].name = "valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[0].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[0].doc = "indicates whether entry is valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[0].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array_t, valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[1].name = "doc";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[1].type = "char *";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[1].doc = "entry documentation";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[1].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array_t, doc);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[2].name = "DESTINATION_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[2].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[2].doc = "DESTINATION_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[2].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array_t, DESTINATION_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[3].name = "DESTINATION_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[3].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[3].doc = "DESTINATION_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[3].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array_t, DESTINATION_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[4].name = "EEI_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[4].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[4].doc = "EEI_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[4].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array_t, EEI_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[5].name = "EEI_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[5].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[5].doc = "EEI_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[5].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array_t, EEI_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[6].name = "OUT_LIF_0_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[6].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[6].doc = "OUT_LIF_0_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[6].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array_t, OUT_LIF_0_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[7].name = "OUT_LIF_0_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[7].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[7].doc = "OUT_LIF_0_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[7].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array_t, OUT_LIF_0_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[8].name = "OUT_LIF_1_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[8].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[8].doc = "OUT_LIF_1_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[8].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array_t, OUT_LIF_1_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[9].name = "OUT_LIF_1_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[9].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[9].doc = "OUT_LIF_1_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[9].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array_t, OUT_LIF_1_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[10].name = "WEAK_TM_FLOW_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[10].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[10].doc = "WEAK_TM_FLOW_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[10].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array_t, WEAK_TM_FLOW_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[11].name = "WEAK_TM_FLOW_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[11].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[11].doc = "WEAK_TM_FLOW_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[11].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array_t, WEAK_TM_FLOW_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[12].name = "MC_RPF_MODE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[12].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[12].doc = "MC_RPF_MODE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[12].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array_t, MC_RPF_MODE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[13].name = "MC_RPF_MODE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[13].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[13].doc = "MC_RPF_MODE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array].values[13].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array_t, MC_RPF_MODE_CONFIG_valid);
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].name = "IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].doc = "IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].flags |= DNXC_DATA_F_TABLE;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].size_of_values = sizeof(dnx_data_dnx2_arr_formats_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array_t);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].entry_get = dnx_data_dnx2_arr_formats_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array_entry_str_get;

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].nof_keys = 2;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].keys[0].name = "table_index";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].keys[0].doc = "table index";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].keys[1].name = "index";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].keys[1].doc = "entry index";

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].nof_values = 18;
        DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].nof_values, "_dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array table values");
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[0].name = "valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[0].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[0].doc = "indicates whether entry is valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[0].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array_t, valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[1].name = "doc";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[1].type = "char *";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[1].doc = "entry documentation";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[1].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array_t, doc);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[2].name = "DESTINATION_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[2].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[2].doc = "DESTINATION_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[2].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array_t, DESTINATION_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[3].name = "DESTINATION_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[3].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[3].doc = "DESTINATION_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[3].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array_t, DESTINATION_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[4].name = "EEI_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[4].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[4].doc = "EEI_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[4].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array_t, EEI_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[5].name = "EEI_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[5].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[5].doc = "EEI_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[5].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array_t, EEI_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[6].name = "OUTLIF_0_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[6].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[6].doc = "OUTLIF_0_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[6].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array_t, OUTLIF_0_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[7].name = "OUTLIF_0_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[7].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[7].doc = "OUTLIF_0_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[7].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array_t, OUTLIF_0_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[8].name = "OUTLIF_1_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[8].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[8].doc = "OUTLIF_1_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[8].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array_t, OUTLIF_1_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[9].name = "OUTLIF_1_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[9].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[9].doc = "OUTLIF_1_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[9].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array_t, OUTLIF_1_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[10].name = "IRPP_FWD_RAW_DATA";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[10].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[10].doc = "IRPP_FWD_RAW_DATA";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[10].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array_t, IRPP_FWD_RAW_DATA);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[11].name = "IRPP_FWD_RAW_DATA_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[11].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[11].doc = "IRPP_FWD_RAW_DATA_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[11].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array_t, IRPP_FWD_RAW_DATA_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[12].name = "DEFAULT_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[12].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[12].doc = "DEFAULT_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[12].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array_t, DEFAULT_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[13].name = "DEFAULT_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[13].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[13].doc = "DEFAULT_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[13].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array_t, DEFAULT_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[14].name = "STAT_OBJ_CMD_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[14].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[14].doc = "STAT_OBJ_CMD_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[14].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array_t, STAT_OBJ_CMD_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[15].name = "STAT_OBJ_CMD_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[15].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[15].doc = "STAT_OBJ_CMD_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[15].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array_t, STAT_OBJ_CMD_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[16].name = "STAT_OBJ_ID_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[16].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[16].doc = "STAT_OBJ_ID_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[16].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array_t, STAT_OBJ_ID_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[17].name = "STAT_OBJ_ID_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[17].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[17].doc = "STAT_OBJ_ID_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array].values[17].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array_t, STAT_OBJ_ID_CONFIG_valid);
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].name = "IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].doc = "IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].flags |= DNXC_DATA_F_TABLE;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].size_of_values = sizeof(dnx_data_dnx2_arr_formats_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array_t);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].entry_get = dnx_data_dnx2_arr_formats_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array_entry_str_get;

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].nof_keys = 2;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].keys[0].name = "table_index";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].keys[0].doc = "table index";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].keys[1].name = "index";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].keys[1].doc = "entry index";

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].nof_values = 14;
        DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].nof_values, "_dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array table values");
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[0].name = "valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[0].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[0].doc = "indicates whether entry is valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[0].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array_t, valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[1].name = "doc";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[1].type = "char *";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[1].doc = "entry documentation";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[1].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array_t, doc);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[2].name = "DESTINATION_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[2].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[2].doc = "DESTINATION_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[2].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array_t, DESTINATION_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[3].name = "DESTINATION_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[3].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[3].doc = "DESTINATION_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[3].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array_t, DESTINATION_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[4].name = "OUTLIF_0_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[4].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[4].doc = "OUTLIF_0_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[4].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array_t, OUTLIF_0_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[5].name = "OUTLIF_0_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[5].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[5].doc = "OUTLIF_0_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[5].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array_t, OUTLIF_0_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[6].name = "DEFAULT_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[6].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[6].doc = "DEFAULT_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[6].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array_t, DEFAULT_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[7].name = "DEFAULT_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[7].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[7].doc = "DEFAULT_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[7].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array_t, DEFAULT_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[8].name = "SA_DROP_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[8].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[8].doc = "SA_DROP_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[8].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array_t, SA_DROP_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[9].name = "SA_DROP_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[9].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[9].doc = "SA_DROP_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[9].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array_t, SA_DROP_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[10].name = "STAT_OBJ_CMD_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[10].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[10].doc = "STAT_OBJ_CMD_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[10].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array_t, STAT_OBJ_CMD_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[11].name = "STAT_OBJ_CMD_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[11].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[11].doc = "STAT_OBJ_CMD_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[11].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array_t, STAT_OBJ_CMD_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[12].name = "STAT_OBJ_ID_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[12].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[12].doc = "STAT_OBJ_ID_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[12].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array_t, STAT_OBJ_ID_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[13].name = "STAT_OBJ_ID_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[13].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[13].doc = "STAT_OBJ_ID_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array].values[13].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array_t, STAT_OBJ_ID_CONFIG_valid);
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].name = "IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].doc = "IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].flags |= DNXC_DATA_F_TABLE;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].size_of_values = sizeof(dnx_data_dnx2_arr_formats_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array_t);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].entry_get = dnx_data_dnx2_arr_formats_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array_entry_str_get;

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].nof_keys = 2;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].keys[0].name = "table_index";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].keys[0].doc = "table index";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].keys[1].name = "index";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].keys[1].doc = "entry index";

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].nof_values = 16;
        DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].nof_values, "_dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array table values");
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[0].name = "valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[0].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[0].doc = "indicates whether entry is valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[0].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array_t, valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[1].name = "doc";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[1].type = "char *";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[1].doc = "entry documentation";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[1].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array_t, doc);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[2].name = "LEFT_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[2].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[2].doc = "LEFT_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[2].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array_t, LEFT_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[3].name = "LEFT_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[3].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[3].doc = "LEFT_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[3].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array_t, LEFT_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[4].name = "LEFT_STAT_OBJECT_CMD_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[4].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[4].doc = "LEFT_STAT_OBJECT_CMD_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[4].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array_t, LEFT_STAT_OBJECT_CMD_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[5].name = "LEFT_STAT_OBJECT_CMD_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[5].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[5].doc = "LEFT_STAT_OBJECT_CMD_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[5].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array_t, LEFT_STAT_OBJECT_CMD_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[6].name = "LEFT_STAT_OBJECT_ID_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[6].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[6].doc = "LEFT_STAT_OBJECT_ID_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[6].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array_t, LEFT_STAT_OBJECT_ID_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[7].name = "LEFT_STAT_OBJECT_ID_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[7].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[7].doc = "LEFT_STAT_OBJECT_ID_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[7].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array_t, LEFT_STAT_OBJECT_ID_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[8].name = "PROTECTION_PTR_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[8].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[8].doc = "PROTECTION_PTR_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[8].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array_t, PROTECTION_PTR_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[9].name = "PROTECTION_PTR_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[9].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[9].doc = "PROTECTION_PTR_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[9].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array_t, PROTECTION_PTR_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[10].name = "RIGHT_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[10].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[10].doc = "RIGHT_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[10].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array_t, RIGHT_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[11].name = "RIGHT_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[11].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[11].doc = "RIGHT_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[11].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array_t, RIGHT_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[12].name = "RIGHT_STAT_OBJECT_CMD_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[12].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[12].doc = "RIGHT_STAT_OBJECT_CMD_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[12].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array_t, RIGHT_STAT_OBJECT_CMD_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[13].name = "RIGHT_STAT_OBJECT_CMD_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[13].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[13].doc = "RIGHT_STAT_OBJECT_CMD_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[13].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array_t, RIGHT_STAT_OBJECT_CMD_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[14].name = "RIGHT_STAT_OBJECT_ID_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[14].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[14].doc = "RIGHT_STAT_OBJECT_ID_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[14].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array_t, RIGHT_STAT_OBJECT_ID_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[15].name = "RIGHT_STAT_OBJECT_ID_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[15].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[15].doc = "RIGHT_STAT_OBJECT_ID_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array].values[15].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array_t, RIGHT_STAT_OBJECT_ID_CONFIG_valid);
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].name = "IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].doc = "IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].flags |= DNXC_DATA_F_TABLE;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].size_of_values = sizeof(dnx_data_dnx2_arr_formats_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1_t);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].entry_get = dnx_data_dnx2_arr_formats_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1_entry_str_get;

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].nof_keys = 1;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].keys[0].name = "index";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].keys[0].doc = "entry index";

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].nof_values = 26;
        DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].nof_values, "_dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1 table values");
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[0].name = "valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[0].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[0].doc = "indicates whether entry is valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[0].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1_t, valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[1].name = "doc";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[1].type = "char *";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[1].doc = "entry documentation";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[1].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1_t, doc);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[2].name = "DA_NOT_FOUND_DESTINATION_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[2].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[2].doc = "DA_NOT_FOUND_DESTINATION_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[2].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1_t, DA_NOT_FOUND_DESTINATION_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[3].name = "DA_NOT_FOUND_DESTINATION_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[3].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[3].doc = "DA_NOT_FOUND_DESTINATION_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[3].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1_t, DA_NOT_FOUND_DESTINATION_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[4].name = "ECN_MAPPING_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[4].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[4].doc = "ECN_MAPPING_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[4].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1_t, ECN_MAPPING_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[5].name = "ECN_MAPPING_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[5].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[5].doc = "ECN_MAPPING_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[5].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1_t, ECN_MAPPING_PROFILE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[6].name = "MY_MAC_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[6].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[6].doc = "MY_MAC_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[6].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1_t, MY_MAC_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[7].name = "MY_MAC_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[7].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[7].doc = "MY_MAC_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[7].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1_t, MY_MAC_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[8].name = "MY_MAC_PREFIX_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[8].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[8].doc = "MY_MAC_PREFIX_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[8].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1_t, MY_MAC_PREFIX_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[9].name = "MY_MAC_PREFIX_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[9].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[9].doc = "MY_MAC_PREFIX_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[9].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1_t, MY_MAC_PREFIX_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[10].name = "PROPAGATION_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[10].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[10].doc = "PROPAGATION_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[10].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1_t, PROPAGATION_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[11].name = "PROPAGATION_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[11].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[11].doc = "PROPAGATION_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[11].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1_t, PROPAGATION_PROFILE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[12].name = "QOS_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[12].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[12].doc = "QOS_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[12].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1_t, QOS_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[13].name = "QOS_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[13].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[13].doc = "QOS_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[13].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1_t, QOS_PROFILE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[14].name = "STAT_OBJ_CMD_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[14].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[14].doc = "STAT_OBJ_CMD_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[14].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1_t, STAT_OBJ_CMD_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[15].name = "STAT_OBJ_CMD_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[15].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[15].doc = "STAT_OBJ_CMD_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[15].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1_t, STAT_OBJ_CMD_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[16].name = "STAT_OBJ_ID_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[16].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[16].doc = "STAT_OBJ_ID_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[16].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1_t, STAT_OBJ_ID_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[17].name = "STAT_OBJ_ID_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[17].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[17].doc = "STAT_OBJ_ID_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[17].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1_t, STAT_OBJ_ID_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[18].name = "TOPOLOGY_ID_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[18].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[18].doc = "TOPOLOGY_ID_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[18].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1_t, TOPOLOGY_ID_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[19].name = "TOPOLOGY_ID_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[19].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[19].doc = "TOPOLOGY_ID_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[19].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1_t, TOPOLOGY_ID_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[20].name = "VRF_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[20].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[20].doc = "VRF_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[20].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1_t, VRF_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[21].name = "VRF_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[21].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[21].doc = "VRF_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[21].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1_t, VRF_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[22].name = "VRID_BITMAP_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[22].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[22].doc = "VRID_BITMAP_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[22].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1_t, VRID_BITMAP_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[23].name = "VRID_BITMAP_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[23].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[23].doc = "VRID_BITMAP_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[23].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1_t, VRID_BITMAP_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[24].name = "VSI_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[24].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[24].doc = "VSI_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[24].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1_t, VSI_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[25].name = "VSI_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[25].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[25].doc = "VSI_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1].values[25].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1_t, VSI_PROFILE_CONFIG_valid);
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].name = "IPPF_VSI_FORMAT_CONFIGURATION_TABLE";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].doc = "IPPF_VSI_FORMAT_CONFIGURATION_TABLE";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].flags |= DNXC_DATA_F_TABLE;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].size_of_values = sizeof(dnx_data_dnx2_arr_formats_IPPF_VSI_FORMAT_CONFIGURATION_TABLE_t);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].entry_get = dnx_data_dnx2_arr_formats_IPPF_VSI_FORMAT_CONFIGURATION_TABLE_entry_str_get;

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].nof_keys = 1;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].keys[0].name = "index";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].keys[0].doc = "entry index";

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].nof_values = 26;
        DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].nof_values, "_dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE table values");
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[0].name = "valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[0].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[0].doc = "indicates whether entry is valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[0].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VSI_FORMAT_CONFIGURATION_TABLE_t, valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[1].name = "doc";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[1].type = "char *";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[1].doc = "entry documentation";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[1].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VSI_FORMAT_CONFIGURATION_TABLE_t, doc);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[2].name = "DA_NOT_FOUND_DESTINATION_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[2].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[2].doc = "DA_NOT_FOUND_DESTINATION_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[2].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VSI_FORMAT_CONFIGURATION_TABLE_t, DA_NOT_FOUND_DESTINATION_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[3].name = "DA_NOT_FOUND_DESTINATION_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[3].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[3].doc = "DA_NOT_FOUND_DESTINATION_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[3].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VSI_FORMAT_CONFIGURATION_TABLE_t, DA_NOT_FOUND_DESTINATION_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[4].name = "ECN_MAPPING_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[4].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[4].doc = "ECN_MAPPING_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[4].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VSI_FORMAT_CONFIGURATION_TABLE_t, ECN_MAPPING_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[5].name = "ECN_MAPPING_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[5].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[5].doc = "ECN_MAPPING_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[5].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VSI_FORMAT_CONFIGURATION_TABLE_t, ECN_MAPPING_PROFILE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[6].name = "MY_MAC_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[6].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[6].doc = "MY_MAC_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[6].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VSI_FORMAT_CONFIGURATION_TABLE_t, MY_MAC_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[7].name = "MY_MAC_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[7].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[7].doc = "MY_MAC_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[7].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VSI_FORMAT_CONFIGURATION_TABLE_t, MY_MAC_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[8].name = "MY_MAC_PREFIX_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[8].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[8].doc = "MY_MAC_PREFIX_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[8].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VSI_FORMAT_CONFIGURATION_TABLE_t, MY_MAC_PREFIX_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[9].name = "MY_MAC_PREFIX_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[9].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[9].doc = "MY_MAC_PREFIX_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[9].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VSI_FORMAT_CONFIGURATION_TABLE_t, MY_MAC_PREFIX_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[10].name = "PROPAGATION_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[10].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[10].doc = "PROPAGATION_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[10].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VSI_FORMAT_CONFIGURATION_TABLE_t, PROPAGATION_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[11].name = "PROPAGATION_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[11].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[11].doc = "PROPAGATION_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[11].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VSI_FORMAT_CONFIGURATION_TABLE_t, PROPAGATION_PROFILE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[12].name = "QOS_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[12].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[12].doc = "QOS_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[12].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VSI_FORMAT_CONFIGURATION_TABLE_t, QOS_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[13].name = "QOS_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[13].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[13].doc = "QOS_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[13].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VSI_FORMAT_CONFIGURATION_TABLE_t, QOS_PROFILE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[14].name = "STAT_OBJ_CMD_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[14].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[14].doc = "STAT_OBJ_CMD_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[14].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VSI_FORMAT_CONFIGURATION_TABLE_t, STAT_OBJ_CMD_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[15].name = "STAT_OBJ_CMD_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[15].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[15].doc = "STAT_OBJ_CMD_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[15].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VSI_FORMAT_CONFIGURATION_TABLE_t, STAT_OBJ_CMD_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[16].name = "STAT_OBJ_ID_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[16].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[16].doc = "STAT_OBJ_ID_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[16].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VSI_FORMAT_CONFIGURATION_TABLE_t, STAT_OBJ_ID_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[17].name = "STAT_OBJ_ID_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[17].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[17].doc = "STAT_OBJ_ID_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[17].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VSI_FORMAT_CONFIGURATION_TABLE_t, STAT_OBJ_ID_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[18].name = "TOPOLOGY_ID_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[18].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[18].doc = "TOPOLOGY_ID_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[18].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VSI_FORMAT_CONFIGURATION_TABLE_t, TOPOLOGY_ID_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[19].name = "TOPOLOGY_ID_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[19].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[19].doc = "TOPOLOGY_ID_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[19].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VSI_FORMAT_CONFIGURATION_TABLE_t, TOPOLOGY_ID_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[20].name = "VRF_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[20].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[20].doc = "VRF_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[20].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VSI_FORMAT_CONFIGURATION_TABLE_t, VRF_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[21].name = "VRF_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[21].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[21].doc = "VRF_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[21].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VSI_FORMAT_CONFIGURATION_TABLE_t, VRF_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[22].name = "VRID_BITMAP_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[22].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[22].doc = "VRID_BITMAP_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[22].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VSI_FORMAT_CONFIGURATION_TABLE_t, VRID_BITMAP_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[23].name = "VRID_BITMAP_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[23].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[23].doc = "VRID_BITMAP_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[23].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VSI_FORMAT_CONFIGURATION_TABLE_t, VRID_BITMAP_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[24].name = "VSI_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[24].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[24].doc = "VSI_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[24].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VSI_FORMAT_CONFIGURATION_TABLE_t, VSI_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[25].name = "VSI_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[25].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[25].doc = "VSI_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE].values[25].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VSI_FORMAT_CONFIGURATION_TABLE_t, VSI_PROFILE_CONFIG_valid);
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].name = "IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].doc = "IPPA_VTT_FORMAT_CONFIGURATION_TABLE";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].flags |= DNXC_DATA_F_TABLE;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].size_of_values = sizeof(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].entry_get = dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_entry_str_get;

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].nof_keys = 2;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].keys[0].name = "table_index";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].keys[0].doc = "table index";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].keys[1].name = "index";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].keys[1].doc = "entry index";

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].nof_values = 62;
        DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].nof_values, "_dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array table values");
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[0].name = "valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[0].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[0].doc = "indicates whether entry is valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[0].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[1].name = "doc";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[1].type = "char *";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[1].doc = "entry documentation";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[1].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, doc);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[2].name = "ACTION_PROFILE_IDX_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[2].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[2].doc = "ACTION_PROFILE_IDX_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[2].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, ACTION_PROFILE_IDX_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[3].name = "ACTION_PROFILE_IDX_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[3].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[3].doc = "ACTION_PROFILE_IDX_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[3].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, ACTION_PROFILE_IDX_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[4].name = "DESTINATION_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[4].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[4].doc = "DESTINATION_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[4].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, DESTINATION_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[5].name = "DESTINATION_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[5].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[5].doc = "DESTINATION_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[5].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, DESTINATION_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[6].name = "ECN_MAPPING_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[6].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[6].doc = "ECN_MAPPING_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[6].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, ECN_MAPPING_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[7].name = "ECN_MAPPING_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[7].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[7].doc = "ECN_MAPPING_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[7].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, ECN_MAPPING_PROFILE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[8].name = "EEI_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[8].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[8].doc = "EEI_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[8].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, EEI_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[9].name = "EEI_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[9].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[9].doc = "EEI_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[9].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, EEI_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[10].name = "FORWARD_DOMAIN_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[10].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[10].doc = "FORWARD_DOMAIN_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[10].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, FORWARD_DOMAIN_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[11].name = "FORWARD_DOMAIN_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[11].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[11].doc = "FORWARD_DOMAIN_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[11].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, FORWARD_DOMAIN_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[12].name = "FORWARD_DOMAIN_ASSIGNMENT_MODE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[12].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[12].doc = "FORWARD_DOMAIN_ASSIGNMENT_MODE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[12].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, FORWARD_DOMAIN_ASSIGNMENT_MODE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[13].name = "FORWARD_DOMAIN_ASSIGNMENT_MODE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[13].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[13].doc = "FORWARD_DOMAIN_ASSIGNMENT_MODE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[13].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, FORWARD_DOMAIN_ASSIGNMENT_MODE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[14].name = "SYS_INLIF_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[14].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[14].doc = "SYS_INLIF_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[14].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, SYS_INLIF_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[15].name = "SYS_INLIF_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[15].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[15].doc = "SYS_INLIF_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[15].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, SYS_INLIF_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[16].name = "OUT_LIF_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[16].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[16].doc = "OUT_LIF_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[16].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, OUT_LIF_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[17].name = "OUT_LIF_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[17].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[17].doc = "OUT_LIF_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[17].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, OUT_LIF_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[18].name = "LIF_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[18].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[18].doc = "LIF_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[18].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, LIF_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[19].name = "LIF_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[19].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[19].doc = "LIF_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[19].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, LIF_PROFILE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[20].name = "LEARN_EN_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[20].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[20].doc = "LEARN_EN_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[20].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, LEARN_EN_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[21].name = "LEARN_EN_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[21].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[21].doc = "LEARN_EN_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[21].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, LEARN_EN_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[22].name = "LEARN_INFO_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[22].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[22].doc = "LEARN_INFO_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[22].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, LEARN_INFO_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[23].name = "LEARN_INFO_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[23].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[23].doc = "LEARN_INFO_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[23].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, LEARN_INFO_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[24].name = "LEARN_PAYLOAD_CONTEXT_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[24].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[24].doc = "LEARN_PAYLOAD_CONTEXT_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[24].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, LEARN_PAYLOAD_CONTEXT_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[25].name = "LEARN_PAYLOAD_CONTEXT_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[25].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[25].doc = "LEARN_PAYLOAD_CONTEXT_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[25].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, LEARN_PAYLOAD_CONTEXT_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[26].name = "LIF_GENERIC_DATA_0_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[26].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[26].doc = "LIF_GENERIC_DATA_0_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[26].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, LIF_GENERIC_DATA_0_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[27].name = "LIF_GENERIC_DATA_0_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[27].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[27].doc = "LIF_GENERIC_DATA_0_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[27].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, LIF_GENERIC_DATA_0_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[28].name = "LIF_GENERIC_DATA_1_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[28].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[28].doc = "LIF_GENERIC_DATA_1_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[28].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, LIF_GENERIC_DATA_1_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[29].name = "LIF_GENERIC_DATA_1_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[29].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[29].doc = "LIF_GENERIC_DATA_1_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[29].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, LIF_GENERIC_DATA_1_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[30].name = "PROTECTION_PTR_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[30].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[30].doc = "PROTECTION_PTR_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[30].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, PROTECTION_PTR_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[31].name = "PROTECTION_PTR_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[31].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[31].doc = "PROTECTION_PTR_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[31].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, PROTECTION_PTR_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[32].name = "NEXT_LAYER_NETWORK_DOMAIN_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[32].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[32].doc = "NEXT_LAYER_NETWORK_DOMAIN_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[32].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, NEXT_LAYER_NETWORK_DOMAIN_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[33].name = "NEXT_LAYER_NETWORK_DOMAIN_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[33].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[33].doc = "NEXT_LAYER_NETWORK_DOMAIN_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[33].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, NEXT_LAYER_NETWORK_DOMAIN_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[34].name = "NEXT_PARSER_CONTEXT_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[34].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[34].doc = "NEXT_PARSER_CONTEXT_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[34].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, NEXT_PARSER_CONTEXT_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[35].name = "NEXT_PARSER_CONTEXT_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[35].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[35].doc = "NEXT_PARSER_CONTEXT_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[35].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, NEXT_PARSER_CONTEXT_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[36].name = "OAM_LIF_SET_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[36].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[36].doc = "OAM_LIF_SET_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[36].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, OAM_LIF_SET_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[37].name = "OAM_LIF_SET_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[37].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[37].doc = "OAM_LIF_SET_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[37].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, OAM_LIF_SET_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[38].name = "PROPAGATION_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[38].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[38].doc = "PROPAGATION_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[38].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, PROPAGATION_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[39].name = "PROPAGATION_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[39].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[39].doc = "PROPAGATION_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[39].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, PROPAGATION_PROFILE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[40].name = "PROTECTION_PATH_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[40].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[40].doc = "PROTECTION_PATH_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[40].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, PROTECTION_PATH_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[41].name = "PROTECTION_PATH_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[41].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[41].doc = "PROTECTION_PATH_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[41].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, PROTECTION_PATH_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[42].name = "QOS_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[42].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[42].doc = "QOS_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[42].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, QOS_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[43].name = "QOS_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[43].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[43].doc = "QOS_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[43].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, QOS_PROFILE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[44].name = "SERVICE_TYPE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[44].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[44].doc = "SERVICE_TYPE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[44].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, SERVICE_TYPE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[45].name = "SERVICE_TYPE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[45].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[45].doc = "SERVICE_TYPE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[45].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, SERVICE_TYPE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[46].name = "STAT_OBJ_CMD_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[46].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[46].doc = "STAT_OBJ_CMD_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[46].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, STAT_OBJ_CMD_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[47].name = "STAT_OBJ_CMD_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[47].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[47].doc = "STAT_OBJ_CMD_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[47].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, STAT_OBJ_CMD_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[48].name = "STAT_OBJ_ID_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[48].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[48].doc = "STAT_OBJ_ID_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[48].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, STAT_OBJ_ID_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[49].name = "STAT_OBJ_ID_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[49].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[49].doc = "STAT_OBJ_ID_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[49].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, STAT_OBJ_ID_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[50].name = "TERMINATION_TYPE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[50].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[50].doc = "TERMINATION_TYPE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[50].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, TERMINATION_TYPE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[51].name = "TERMINATION_TYPE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[51].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[51].doc = "TERMINATION_TYPE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[51].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, TERMINATION_TYPE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[52].name = "VLAN_EDIT_PCP_DEI_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[52].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[52].doc = "VLAN_EDIT_PCP_DEI_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[52].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, VLAN_EDIT_PCP_DEI_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[53].name = "VLAN_EDIT_PCP_DEI_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[53].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[53].doc = "VLAN_EDIT_PCP_DEI_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[53].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, VLAN_EDIT_PCP_DEI_PROFILE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[54].name = "VLAN_EDIT_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[54].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[54].doc = "VLAN_EDIT_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[54].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, VLAN_EDIT_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[55].name = "VLAN_EDIT_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[55].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[55].doc = "VLAN_EDIT_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[55].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, VLAN_EDIT_PROFILE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[56].name = "VLAN_EDIT_VID_1_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[56].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[56].doc = "VLAN_EDIT_VID_1_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[56].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, VLAN_EDIT_VID_1_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[57].name = "VLAN_EDIT_VID_1_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[57].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[57].doc = "VLAN_EDIT_VID_1_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[57].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, VLAN_EDIT_VID_1_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[58].name = "VLAN_EDIT_VID_2_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[58].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[58].doc = "VLAN_EDIT_VID_2_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[58].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, VLAN_EDIT_VID_2_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[59].name = "VLAN_EDIT_VID_2_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[59].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[59].doc = "VLAN_EDIT_VID_2_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[59].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, VLAN_EDIT_VID_2_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[60].name = "VLAN_MEMBER_IF_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[60].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[60].doc = "VLAN_MEMBER_IF_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[60].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, VLAN_MEMBER_IF_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[61].name = "VLAN_MEMBER_IF_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[61].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[61].doc = "VLAN_MEMBER_IF_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array].values[61].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t, VLAN_MEMBER_IF_CONFIG_valid);
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].name = "IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].doc = "IPPF_VTT_FORMAT_CONFIGURATION_TABLE";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].flags |= DNXC_DATA_F_TABLE;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].size_of_values = sizeof(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].entry_get = dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_entry_str_get;

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].nof_keys = 2;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].keys[0].name = "table_index";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].keys[0].doc = "table index";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].keys[1].name = "index";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].keys[1].doc = "entry index";

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].nof_values = 62;
        DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].nof_values, "_dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array table values");
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[0].name = "valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[0].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[0].doc = "indicates whether entry is valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[0].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[1].name = "doc";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[1].type = "char *";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[1].doc = "entry documentation";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[1].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, doc);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[2].name = "ACTION_PROFILE_IDX_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[2].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[2].doc = "ACTION_PROFILE_IDX_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[2].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, ACTION_PROFILE_IDX_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[3].name = "ACTION_PROFILE_IDX_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[3].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[3].doc = "ACTION_PROFILE_IDX_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[3].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, ACTION_PROFILE_IDX_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[4].name = "DESTINATION_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[4].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[4].doc = "DESTINATION_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[4].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, DESTINATION_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[5].name = "DESTINATION_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[5].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[5].doc = "DESTINATION_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[5].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, DESTINATION_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[6].name = "ECN_MAPPING_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[6].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[6].doc = "ECN_MAPPING_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[6].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, ECN_MAPPING_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[7].name = "ECN_MAPPING_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[7].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[7].doc = "ECN_MAPPING_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[7].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, ECN_MAPPING_PROFILE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[8].name = "EEI_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[8].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[8].doc = "EEI_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[8].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, EEI_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[9].name = "EEI_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[9].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[9].doc = "EEI_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[9].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, EEI_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[10].name = "FORWARD_DOMAIN_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[10].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[10].doc = "FORWARD_DOMAIN_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[10].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, FORWARD_DOMAIN_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[11].name = "FORWARD_DOMAIN_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[11].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[11].doc = "FORWARD_DOMAIN_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[11].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, FORWARD_DOMAIN_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[12].name = "FORWARD_DOMAIN_ASSIGNMENT_MODE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[12].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[12].doc = "FORWARD_DOMAIN_ASSIGNMENT_MODE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[12].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, FORWARD_DOMAIN_ASSIGNMENT_MODE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[13].name = "FORWARD_DOMAIN_ASSIGNMENT_MODE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[13].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[13].doc = "FORWARD_DOMAIN_ASSIGNMENT_MODE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[13].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, FORWARD_DOMAIN_ASSIGNMENT_MODE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[14].name = "SYS_INLIF_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[14].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[14].doc = "SYS_INLIF_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[14].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, SYS_INLIF_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[15].name = "SYS_INLIF_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[15].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[15].doc = "SYS_INLIF_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[15].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, SYS_INLIF_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[16].name = "OUT_LIF_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[16].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[16].doc = "OUT_LIF_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[16].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, OUT_LIF_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[17].name = "OUT_LIF_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[17].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[17].doc = "OUT_LIF_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[17].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, OUT_LIF_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[18].name = "LIF_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[18].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[18].doc = "LIF_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[18].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, LIF_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[19].name = "LIF_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[19].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[19].doc = "LIF_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[19].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, LIF_PROFILE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[20].name = "LEARN_EN_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[20].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[20].doc = "LEARN_EN_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[20].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, LEARN_EN_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[21].name = "LEARN_EN_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[21].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[21].doc = "LEARN_EN_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[21].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, LEARN_EN_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[22].name = "LEARN_INFO_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[22].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[22].doc = "LEARN_INFO_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[22].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, LEARN_INFO_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[23].name = "LEARN_INFO_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[23].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[23].doc = "LEARN_INFO_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[23].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, LEARN_INFO_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[24].name = "LEARN_PAYLOAD_CONTEXT_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[24].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[24].doc = "LEARN_PAYLOAD_CONTEXT_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[24].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, LEARN_PAYLOAD_CONTEXT_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[25].name = "LEARN_PAYLOAD_CONTEXT_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[25].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[25].doc = "LEARN_PAYLOAD_CONTEXT_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[25].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, LEARN_PAYLOAD_CONTEXT_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[26].name = "LIF_GENERIC_DATA_0_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[26].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[26].doc = "LIF_GENERIC_DATA_0_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[26].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, LIF_GENERIC_DATA_0_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[27].name = "LIF_GENERIC_DATA_0_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[27].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[27].doc = "LIF_GENERIC_DATA_0_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[27].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, LIF_GENERIC_DATA_0_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[28].name = "LIF_GENERIC_DATA_1_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[28].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[28].doc = "LIF_GENERIC_DATA_1_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[28].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, LIF_GENERIC_DATA_1_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[29].name = "LIF_GENERIC_DATA_1_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[29].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[29].doc = "LIF_GENERIC_DATA_1_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[29].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, LIF_GENERIC_DATA_1_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[30].name = "PROTECTION_PTR_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[30].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[30].doc = "PROTECTION_PTR_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[30].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, PROTECTION_PTR_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[31].name = "PROTECTION_PTR_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[31].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[31].doc = "PROTECTION_PTR_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[31].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, PROTECTION_PTR_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[32].name = "NEXT_LAYER_NETWORK_DOMAIN_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[32].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[32].doc = "NEXT_LAYER_NETWORK_DOMAIN_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[32].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, NEXT_LAYER_NETWORK_DOMAIN_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[33].name = "NEXT_LAYER_NETWORK_DOMAIN_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[33].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[33].doc = "NEXT_LAYER_NETWORK_DOMAIN_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[33].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, NEXT_LAYER_NETWORK_DOMAIN_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[34].name = "NEXT_PARSER_CONTEXT_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[34].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[34].doc = "NEXT_PARSER_CONTEXT_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[34].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, NEXT_PARSER_CONTEXT_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[35].name = "NEXT_PARSER_CONTEXT_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[35].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[35].doc = "NEXT_PARSER_CONTEXT_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[35].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, NEXT_PARSER_CONTEXT_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[36].name = "OAM_LIF_SET_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[36].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[36].doc = "OAM_LIF_SET_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[36].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, OAM_LIF_SET_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[37].name = "OAM_LIF_SET_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[37].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[37].doc = "OAM_LIF_SET_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[37].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, OAM_LIF_SET_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[38].name = "PROPAGATION_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[38].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[38].doc = "PROPAGATION_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[38].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, PROPAGATION_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[39].name = "PROPAGATION_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[39].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[39].doc = "PROPAGATION_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[39].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, PROPAGATION_PROFILE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[40].name = "PROTECTION_PATH_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[40].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[40].doc = "PROTECTION_PATH_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[40].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, PROTECTION_PATH_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[41].name = "PROTECTION_PATH_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[41].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[41].doc = "PROTECTION_PATH_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[41].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, PROTECTION_PATH_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[42].name = "QOS_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[42].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[42].doc = "QOS_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[42].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, QOS_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[43].name = "QOS_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[43].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[43].doc = "QOS_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[43].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, QOS_PROFILE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[44].name = "SERVICE_TYPE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[44].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[44].doc = "SERVICE_TYPE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[44].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, SERVICE_TYPE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[45].name = "SERVICE_TYPE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[45].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[45].doc = "SERVICE_TYPE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[45].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, SERVICE_TYPE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[46].name = "STAT_OBJ_CMD_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[46].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[46].doc = "STAT_OBJ_CMD_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[46].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, STAT_OBJ_CMD_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[47].name = "STAT_OBJ_CMD_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[47].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[47].doc = "STAT_OBJ_CMD_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[47].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, STAT_OBJ_CMD_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[48].name = "STAT_OBJ_ID_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[48].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[48].doc = "STAT_OBJ_ID_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[48].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, STAT_OBJ_ID_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[49].name = "STAT_OBJ_ID_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[49].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[49].doc = "STAT_OBJ_ID_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[49].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, STAT_OBJ_ID_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[50].name = "TERMINATION_TYPE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[50].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[50].doc = "TERMINATION_TYPE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[50].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, TERMINATION_TYPE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[51].name = "TERMINATION_TYPE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[51].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[51].doc = "TERMINATION_TYPE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[51].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, TERMINATION_TYPE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[52].name = "VLAN_EDIT_PCP_DEI_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[52].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[52].doc = "VLAN_EDIT_PCP_DEI_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[52].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, VLAN_EDIT_PCP_DEI_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[53].name = "VLAN_EDIT_PCP_DEI_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[53].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[53].doc = "VLAN_EDIT_PCP_DEI_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[53].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, VLAN_EDIT_PCP_DEI_PROFILE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[54].name = "VLAN_EDIT_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[54].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[54].doc = "VLAN_EDIT_PROFILE_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[54].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, VLAN_EDIT_PROFILE_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[55].name = "VLAN_EDIT_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[55].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[55].doc = "VLAN_EDIT_PROFILE_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[55].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, VLAN_EDIT_PROFILE_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[56].name = "VLAN_EDIT_VID_1_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[56].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[56].doc = "VLAN_EDIT_VID_1_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[56].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, VLAN_EDIT_VID_1_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[57].name = "VLAN_EDIT_VID_1_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[57].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[57].doc = "VLAN_EDIT_VID_1_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[57].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, VLAN_EDIT_VID_1_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[58].name = "VLAN_EDIT_VID_2_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[58].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[58].doc = "VLAN_EDIT_VID_2_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[58].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, VLAN_EDIT_VID_2_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[59].name = "VLAN_EDIT_VID_2_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[59].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[59].doc = "VLAN_EDIT_VID_2_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[59].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, VLAN_EDIT_VID_2_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[60].name = "VLAN_MEMBER_IF_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[60].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[60].doc = "VLAN_MEMBER_IF_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[60].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, VLAN_MEMBER_IF_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[61].name = "VLAN_MEMBER_IF_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[61].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[61].doc = "VLAN_MEMBER_IF_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array].values[61].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t, VLAN_MEMBER_IF_CONFIG_valid);
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_LEARN_ARR_CONFIGURATION_TABLE].name = "IPPD_LEARN_ARR_CONFIGURATION_TABLE";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_LEARN_ARR_CONFIGURATION_TABLE].doc = "IPPD_LEARN_ARR_CONFIGURATION_TABLE";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_LEARN_ARR_CONFIGURATION_TABLE].flags |= DNXC_DATA_F_TABLE;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_LEARN_ARR_CONFIGURATION_TABLE].size_of_values = sizeof(dnx_data_dnx2_arr_formats_IPPD_LEARN_ARR_CONFIGURATION_TABLE_t);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_LEARN_ARR_CONFIGURATION_TABLE].entry_get = dnx_data_dnx2_arr_formats_IPPD_LEARN_ARR_CONFIGURATION_TABLE_entry_str_get;

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_LEARN_ARR_CONFIGURATION_TABLE].nof_keys = 1;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_LEARN_ARR_CONFIGURATION_TABLE].keys[0].name = "index";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_LEARN_ARR_CONFIGURATION_TABLE].keys[0].doc = "entry index";

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_LEARN_ARR_CONFIGURATION_TABLE].nof_values = 8;
        DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_LEARN_ARR_CONFIGURATION_TABLE].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_LEARN_ARR_CONFIGURATION_TABLE].nof_values, "_dnx_data_dnx2_arr_formats_table_IPPD_LEARN_ARR_CONFIGURATION_TABLE table values");
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_LEARN_ARR_CONFIGURATION_TABLE].values[0].name = "valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_LEARN_ARR_CONFIGURATION_TABLE].values[0].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_LEARN_ARR_CONFIGURATION_TABLE].values[0].doc = "indicates whether entry is valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_LEARN_ARR_CONFIGURATION_TABLE].values[0].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPD_LEARN_ARR_CONFIGURATION_TABLE_t, valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_LEARN_ARR_CONFIGURATION_TABLE].values[1].name = "doc";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_LEARN_ARR_CONFIGURATION_TABLE].values[1].type = "char *";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_LEARN_ARR_CONFIGURATION_TABLE].values[1].doc = "entry documentation";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_LEARN_ARR_CONFIGURATION_TABLE].values[1].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPD_LEARN_ARR_CONFIGURATION_TABLE_t, doc);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_LEARN_ARR_CONFIGURATION_TABLE].values[2].name = "DESTINATION_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_LEARN_ARR_CONFIGURATION_TABLE].values[2].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_LEARN_ARR_CONFIGURATION_TABLE].values[2].doc = "DESTINATION_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_LEARN_ARR_CONFIGURATION_TABLE].values[2].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPD_LEARN_ARR_CONFIGURATION_TABLE_t, DESTINATION_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_LEARN_ARR_CONFIGURATION_TABLE].values[3].name = "DESTINATION_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_LEARN_ARR_CONFIGURATION_TABLE].values[3].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_LEARN_ARR_CONFIGURATION_TABLE].values[3].doc = "DESTINATION_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_LEARN_ARR_CONFIGURATION_TABLE].values[3].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPD_LEARN_ARR_CONFIGURATION_TABLE_t, DESTINATION_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_LEARN_ARR_CONFIGURATION_TABLE].values[4].name = "EEI_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_LEARN_ARR_CONFIGURATION_TABLE].values[4].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_LEARN_ARR_CONFIGURATION_TABLE].values[4].doc = "EEI_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_LEARN_ARR_CONFIGURATION_TABLE].values[4].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPD_LEARN_ARR_CONFIGURATION_TABLE_t, EEI_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_LEARN_ARR_CONFIGURATION_TABLE].values[5].name = "EEI_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_LEARN_ARR_CONFIGURATION_TABLE].values[5].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_LEARN_ARR_CONFIGURATION_TABLE].values[5].doc = "EEI_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_LEARN_ARR_CONFIGURATION_TABLE].values[5].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPD_LEARN_ARR_CONFIGURATION_TABLE_t, EEI_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_LEARN_ARR_CONFIGURATION_TABLE].values[6].name = "OUTLIF_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_LEARN_ARR_CONFIGURATION_TABLE].values[6].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_LEARN_ARR_CONFIGURATION_TABLE].values[6].doc = "OUTLIF_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_LEARN_ARR_CONFIGURATION_TABLE].values[6].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPD_LEARN_ARR_CONFIGURATION_TABLE_t, OUTLIF_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_LEARN_ARR_CONFIGURATION_TABLE].values[7].name = "OUTLIF_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_LEARN_ARR_CONFIGURATION_TABLE].values[7].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_LEARN_ARR_CONFIGURATION_TABLE].values[7].doc = "OUTLIF_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_IPPD_LEARN_ARR_CONFIGURATION_TABLE].values[7].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_IPPD_LEARN_ARR_CONFIGURATION_TABLE_t, OUTLIF_CONFIG_valid);
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_OLP_LEARN_ARR_CONFIGURATION_TABLE].name = "OLP_LEARN_ARR_CONFIGURATION_TABLE";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_OLP_LEARN_ARR_CONFIGURATION_TABLE].doc = "OLP_LEARN_ARR_CONFIGURATION_TABLE";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_OLP_LEARN_ARR_CONFIGURATION_TABLE].flags |= DNXC_DATA_F_TABLE;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_OLP_LEARN_ARR_CONFIGURATION_TABLE].size_of_values = sizeof(dnx_data_dnx2_arr_formats_OLP_LEARN_ARR_CONFIGURATION_TABLE_t);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_OLP_LEARN_ARR_CONFIGURATION_TABLE].entry_get = dnx_data_dnx2_arr_formats_OLP_LEARN_ARR_CONFIGURATION_TABLE_entry_str_get;

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_OLP_LEARN_ARR_CONFIGURATION_TABLE].nof_keys = 1;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_OLP_LEARN_ARR_CONFIGURATION_TABLE].keys[0].name = "index";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_OLP_LEARN_ARR_CONFIGURATION_TABLE].keys[0].doc = "entry index";

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_OLP_LEARN_ARR_CONFIGURATION_TABLE].nof_values = 8;
        DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dnx2_arr_formats_table_OLP_LEARN_ARR_CONFIGURATION_TABLE].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dnx2_arr_formats_table_OLP_LEARN_ARR_CONFIGURATION_TABLE].nof_values, "_dnx_data_dnx2_arr_formats_table_OLP_LEARN_ARR_CONFIGURATION_TABLE table values");
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_OLP_LEARN_ARR_CONFIGURATION_TABLE].values[0].name = "valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_OLP_LEARN_ARR_CONFIGURATION_TABLE].values[0].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_OLP_LEARN_ARR_CONFIGURATION_TABLE].values[0].doc = "indicates whether entry is valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_OLP_LEARN_ARR_CONFIGURATION_TABLE].values[0].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_OLP_LEARN_ARR_CONFIGURATION_TABLE_t, valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_OLP_LEARN_ARR_CONFIGURATION_TABLE].values[1].name = "doc";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_OLP_LEARN_ARR_CONFIGURATION_TABLE].values[1].type = "char *";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_OLP_LEARN_ARR_CONFIGURATION_TABLE].values[1].doc = "entry documentation";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_OLP_LEARN_ARR_CONFIGURATION_TABLE].values[1].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_OLP_LEARN_ARR_CONFIGURATION_TABLE_t, doc);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_OLP_LEARN_ARR_CONFIGURATION_TABLE].values[2].name = "DESTINATION_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_OLP_LEARN_ARR_CONFIGURATION_TABLE].values[2].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_OLP_LEARN_ARR_CONFIGURATION_TABLE].values[2].doc = "DESTINATION_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_OLP_LEARN_ARR_CONFIGURATION_TABLE].values[2].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_OLP_LEARN_ARR_CONFIGURATION_TABLE_t, DESTINATION_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_OLP_LEARN_ARR_CONFIGURATION_TABLE].values[3].name = "DESTINATION_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_OLP_LEARN_ARR_CONFIGURATION_TABLE].values[3].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_OLP_LEARN_ARR_CONFIGURATION_TABLE].values[3].doc = "DESTINATION_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_OLP_LEARN_ARR_CONFIGURATION_TABLE].values[3].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_OLP_LEARN_ARR_CONFIGURATION_TABLE_t, DESTINATION_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_OLP_LEARN_ARR_CONFIGURATION_TABLE].values[4].name = "EEI_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_OLP_LEARN_ARR_CONFIGURATION_TABLE].values[4].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_OLP_LEARN_ARR_CONFIGURATION_TABLE].values[4].doc = "EEI_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_OLP_LEARN_ARR_CONFIGURATION_TABLE].values[4].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_OLP_LEARN_ARR_CONFIGURATION_TABLE_t, EEI_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_OLP_LEARN_ARR_CONFIGURATION_TABLE].values[5].name = "EEI_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_OLP_LEARN_ARR_CONFIGURATION_TABLE].values[5].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_OLP_LEARN_ARR_CONFIGURATION_TABLE].values[5].doc = "EEI_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_OLP_LEARN_ARR_CONFIGURATION_TABLE].values[5].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_OLP_LEARN_ARR_CONFIGURATION_TABLE_t, EEI_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_OLP_LEARN_ARR_CONFIGURATION_TABLE].values[6].name = "OUTLIF_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_OLP_LEARN_ARR_CONFIGURATION_TABLE].values[6].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_OLP_LEARN_ARR_CONFIGURATION_TABLE].values[6].doc = "OUTLIF_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_OLP_LEARN_ARR_CONFIGURATION_TABLE].values[6].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_OLP_LEARN_ARR_CONFIGURATION_TABLE_t, OUTLIF_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_OLP_LEARN_ARR_CONFIGURATION_TABLE].values[7].name = "OUTLIF_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_OLP_LEARN_ARR_CONFIGURATION_TABLE].values[7].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_OLP_LEARN_ARR_CONFIGURATION_TABLE].values[7].doc = "OUTLIF_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_OLP_LEARN_ARR_CONFIGURATION_TABLE].values[7].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_OLP_LEARN_ARR_CONFIGURATION_TABLE_t, OUTLIF_CONFIG_valid);
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ERPP_EGRESS_MCDB_FORMAT_CFG].name = "ERPP_EGRESS_MCDB_FORMAT_CFG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ERPP_EGRESS_MCDB_FORMAT_CFG].doc = "ERPP_EGRESS_MCDB_FORMAT_CFG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ERPP_EGRESS_MCDB_FORMAT_CFG].flags |= DNXC_DATA_F_TABLE;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ERPP_EGRESS_MCDB_FORMAT_CFG].size_of_values = sizeof(dnx_data_dnx2_arr_formats_ERPP_EGRESS_MCDB_FORMAT_CFG_t);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ERPP_EGRESS_MCDB_FORMAT_CFG].entry_get = dnx_data_dnx2_arr_formats_ERPP_EGRESS_MCDB_FORMAT_CFG_entry_str_get;

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ERPP_EGRESS_MCDB_FORMAT_CFG].nof_keys = 1;
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ERPP_EGRESS_MCDB_FORMAT_CFG].keys[0].name = "index";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ERPP_EGRESS_MCDB_FORMAT_CFG].keys[0].doc = "entry index";

        
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ERPP_EGRESS_MCDB_FORMAT_CFG].nof_values = 8;
        DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_dnx2_arr_formats_table_ERPP_EGRESS_MCDB_FORMAT_CFG].values, dnxc_data_value_t, submodule_data->tables[dnx_data_dnx2_arr_formats_table_ERPP_EGRESS_MCDB_FORMAT_CFG].nof_values, "_dnx_data_dnx2_arr_formats_table_ERPP_EGRESS_MCDB_FORMAT_CFG table values");
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ERPP_EGRESS_MCDB_FORMAT_CFG].values[0].name = "valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ERPP_EGRESS_MCDB_FORMAT_CFG].values[0].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ERPP_EGRESS_MCDB_FORMAT_CFG].values[0].doc = "indicates whether entry is valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ERPP_EGRESS_MCDB_FORMAT_CFG].values[0].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ERPP_EGRESS_MCDB_FORMAT_CFG_t, valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ERPP_EGRESS_MCDB_FORMAT_CFG].values[1].name = "doc";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ERPP_EGRESS_MCDB_FORMAT_CFG].values[1].type = "char *";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ERPP_EGRESS_MCDB_FORMAT_CFG].values[1].doc = "entry documentation";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ERPP_EGRESS_MCDB_FORMAT_CFG].values[1].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ERPP_EGRESS_MCDB_FORMAT_CFG_t, doc);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ERPP_EGRESS_MCDB_FORMAT_CFG].values[2].name = "MCDB_OUTLIF_0_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ERPP_EGRESS_MCDB_FORMAT_CFG].values[2].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ERPP_EGRESS_MCDB_FORMAT_CFG].values[2].doc = "MCDB_OUTLIF_0_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ERPP_EGRESS_MCDB_FORMAT_CFG].values[2].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ERPP_EGRESS_MCDB_FORMAT_CFG_t, MCDB_OUTLIF_0_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ERPP_EGRESS_MCDB_FORMAT_CFG].values[3].name = "MCDB_OUTLIF_0_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ERPP_EGRESS_MCDB_FORMAT_CFG].values[3].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ERPP_EGRESS_MCDB_FORMAT_CFG].values[3].doc = "MCDB_OUTLIF_0_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ERPP_EGRESS_MCDB_FORMAT_CFG].values[3].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ERPP_EGRESS_MCDB_FORMAT_CFG_t, MCDB_OUTLIF_0_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ERPP_EGRESS_MCDB_FORMAT_CFG].values[4].name = "MCDB_OUTLIF_1_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ERPP_EGRESS_MCDB_FORMAT_CFG].values[4].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ERPP_EGRESS_MCDB_FORMAT_CFG].values[4].doc = "MCDB_OUTLIF_1_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ERPP_EGRESS_MCDB_FORMAT_CFG].values[4].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ERPP_EGRESS_MCDB_FORMAT_CFG_t, MCDB_OUTLIF_1_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ERPP_EGRESS_MCDB_FORMAT_CFG].values[5].name = "MCDB_OUTLIF_1_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ERPP_EGRESS_MCDB_FORMAT_CFG].values[5].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ERPP_EGRESS_MCDB_FORMAT_CFG].values[5].doc = "MCDB_OUTLIF_1_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ERPP_EGRESS_MCDB_FORMAT_CFG].values[5].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ERPP_EGRESS_MCDB_FORMAT_CFG_t, MCDB_OUTLIF_1_CONFIG_valid);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ERPP_EGRESS_MCDB_FORMAT_CFG].values[6].name = "MCDB_OUTLIF_2_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ERPP_EGRESS_MCDB_FORMAT_CFG].values[6].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ERPP_EGRESS_MCDB_FORMAT_CFG].values[6].doc = "MCDB_OUTLIF_2_CONFIG";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ERPP_EGRESS_MCDB_FORMAT_CFG].values[6].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ERPP_EGRESS_MCDB_FORMAT_CFG_t, MCDB_OUTLIF_2_CONFIG);
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ERPP_EGRESS_MCDB_FORMAT_CFG].values[7].name = "MCDB_OUTLIF_2_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ERPP_EGRESS_MCDB_FORMAT_CFG].values[7].type = "uint32";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ERPP_EGRESS_MCDB_FORMAT_CFG].values[7].doc = "MCDB_OUTLIF_2_CONFIG_valid";
        submodule_data->tables[dnx_data_dnx2_arr_formats_table_ERPP_EGRESS_MCDB_FORMAT_CFG].values[7].offset = UTILEX_OFFSETOF(dnx_data_dnx2_arr_formats_ERPP_EGRESS_MCDB_FORMAT_CFG_t, MCDB_OUTLIF_2_CONFIG_valid);
    }


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_dnx2_arr_formats_feature_get(
    int unit,
    dnx_data_dnx2_arr_formats_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, feature);
}




const dnx_data_dnx2_arr_formats_files_t *
dnx_data_dnx2_arr_formats_files_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "files", "DNX2");
    }
    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_files);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_dnx2_arr_formats_files_t *) data;

}

const dnx_data_dnx2_arr_formats_MACT_AGE_PROFILE_ARR_CFG_t *
dnx_data_dnx2_arr_formats_MACT_AGE_PROFILE_ARR_CFG_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "MACT_AGE_PROFILE_ARR_CFG", "DNX2");
    }
    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_MACT_AGE_PROFILE_ARR_CFG);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dnx2_arr_formats_MACT_AGE_PROFILE_ARR_CFG_t *) data;

}

const dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_t *
dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE", "DNX2");
    }
    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_t *) data;

}

const dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_t *
dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE", "DNX2");
    }
    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_t *) data;

}

const dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_t *
dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE", "DNX2");
    }
    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_t *) data;

}

const dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_t *
dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE", "DNX2");
    }
    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_t *) data;

}

const dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_t *
dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE", "DNX2");
    }
    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_t *) data;

}

const dnx_data_dnx2_arr_formats_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE_t *
dnx_data_dnx2_arr_formats_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPPB_ENC_FORMAT_CONFIGURATION_TABLE", "DNX2");
    }
    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dnx2_arr_formats_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE_t *) data;

}

const dnx_data_dnx2_arr_formats_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE_t *
dnx_data_dnx2_arr_formats_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPPC_ENC_FORMAT_CONFIGURATION_TABLE", "DNX2");
    }
    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dnx2_arr_formats_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE_t *) data;

}

const dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE_t *
dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE", "DNX2");
    }
    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE_t *) data;

}

const dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE_t *
dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE", "DNX2");
    }
    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE_t *) data;

}

const dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE_t *
dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE", "DNX2");
    }
    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE_t *) data;

}

const dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE_t *
dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE", "DNX2");
    }
    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE_t *) data;

}

const dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE_t *
dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE", "DNX2");
    }
    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE_t *) data;

}

const dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_t *
dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0", "DNX2");
    }
    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_t *) data;

}

const dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1_t *
dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1", "DNX2");
    }
    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1_t *) data;

}

const dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION_t *
dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPPA_PRP_EES_ARR_CONFIGURATION", "DNX2");
    }
    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION_t *) data;

}

const dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION___array_t *
dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION___array_get(
    int unit,
    int table_index,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPPA_PRP_EES_ARR_CONFIGURATION___array", "DNX2");
    }
    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, table_index, index);
    return (const dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION___array_t *) data;

}

const dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION_t *
dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPPC_TERM_ETPS_ARR_1_CONFIGURATION", "DNX2");
    }
    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION_t *) data;

}

const dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_t *
dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_get(
    int unit,
    int table_index,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array", "DNX2");
    }
    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, table_index, index);
    return (const dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_t *) data;

}

const dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION_t *
dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPPC_TERM_ETPS_ARR_2_CONFIGURATION", "DNX2");
    }
    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION_t *) data;

}

const dnx_data_dnx2_arr_formats_IPPB_ACCEPTED_ARR_FORMAT_CONFIG_t *
dnx_data_dnx2_arr_formats_IPPB_ACCEPTED_ARR_FORMAT_CONFIG_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IPPB_ACCEPTED_ARR_FORMAT_CONFIG", "DNX2");
    }
    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dnx2_arr_formats_IPPB_ACCEPTED_ARR_FORMAT_CONFIG_t *) data;

}

const dnx_data_dnx2_arr_formats_MACT_MRQ_ARR_CFG_t *
dnx_data_dnx2_arr_formats_MACT_MRQ_ARR_CFG_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "MACT_MRQ_ARR_CFG", "DNX2");
    }
    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_MACT_MRQ_ARR_CFG);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dnx2_arr_formats_MACT_MRQ_ARR_CFG_t *) data;

}

const dnx_data_dnx2_arr_formats_IPPB_EXPECTED_ARR_FORMAT_CONFIG_t *
dnx_data_dnx2_arr_formats_IPPB_EXPECTED_ARR_FORMAT_CONFIG_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IPPB_EXPECTED_ARR_FORMAT_CONFIG", "DNX2");
    }
    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dnx2_arr_formats_IPPB_EXPECTED_ARR_FORMAT_CONFIG_t *) data;

}

const dnx_data_dnx2_arr_formats_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array_t *
dnx_data_dnx2_arr_formats_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array_get(
    int unit,
    int table_index,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array", "DNX2");
    }
    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, table_index, index);
    return (const dnx_data_dnx2_arr_formats_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array_t *) data;

}

const dnx_data_dnx2_arr_formats_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array_t *
dnx_data_dnx2_arr_formats_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array_get(
    int unit,
    int table_index,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array", "DNX2");
    }
    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, table_index, index);
    return (const dnx_data_dnx2_arr_formats_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array_t *) data;

}

const dnx_data_dnx2_arr_formats_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array_t *
dnx_data_dnx2_arr_formats_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array_get(
    int unit,
    int table_index,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array", "DNX2");
    }
    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, table_index, index);
    return (const dnx_data_dnx2_arr_formats_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array_t *) data;

}

const dnx_data_dnx2_arr_formats_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array_t *
dnx_data_dnx2_arr_formats_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array_get(
    int unit,
    int table_index,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array", "DNX2");
    }
    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, table_index, index);
    return (const dnx_data_dnx2_arr_formats_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array_t *) data;

}

const dnx_data_dnx2_arr_formats_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1_t *
dnx_data_dnx2_arr_formats_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1", "DNX2");
    }
    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dnx2_arr_formats_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1_t *) data;

}

const dnx_data_dnx2_arr_formats_IPPF_VSI_FORMAT_CONFIGURATION_TABLE_t *
dnx_data_dnx2_arr_formats_IPPF_VSI_FORMAT_CONFIGURATION_TABLE_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IPPF_VSI_FORMAT_CONFIGURATION_TABLE", "DNX2");
    }
    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dnx2_arr_formats_IPPF_VSI_FORMAT_CONFIGURATION_TABLE_t *) data;

}

const dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t *
dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_get(
    int unit,
    int table_index,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array", "DNX2");
    }
    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, table_index, index);
    return (const dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t *) data;

}

const dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t *
dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_get(
    int unit,
    int table_index,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array", "DNX2");
    }
    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, table_index, index);
    return (const dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t *) data;

}

const dnx_data_dnx2_arr_formats_IPPD_LEARN_ARR_CONFIGURATION_TABLE_t *
dnx_data_dnx2_arr_formats_IPPD_LEARN_ARR_CONFIGURATION_TABLE_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IPPD_LEARN_ARR_CONFIGURATION_TABLE", "DNX2");
    }
    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_IPPD_LEARN_ARR_CONFIGURATION_TABLE);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dnx2_arr_formats_IPPD_LEARN_ARR_CONFIGURATION_TABLE_t *) data;

}

const dnx_data_dnx2_arr_formats_OLP_LEARN_ARR_CONFIGURATION_TABLE_t *
dnx_data_dnx2_arr_formats_OLP_LEARN_ARR_CONFIGURATION_TABLE_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "OLP_LEARN_ARR_CONFIGURATION_TABLE", "DNX2");
    }
    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_OLP_LEARN_ARR_CONFIGURATION_TABLE);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dnx2_arr_formats_OLP_LEARN_ARR_CONFIGURATION_TABLE_t *) data;

}

const dnx_data_dnx2_arr_formats_ERPP_EGRESS_MCDB_FORMAT_CFG_t *
dnx_data_dnx2_arr_formats_ERPP_EGRESS_MCDB_FORMAT_CFG_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ERPP_EGRESS_MCDB_FORMAT_CFG", "DNX2");
    }
    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ERPP_EGRESS_MCDB_FORMAT_CFG);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_dnx2_arr_formats_ERPP_EGRESS_MCDB_FORMAT_CFG_t *) data;

}


shr_error_e
dnx_data_dnx2_arr_formats_files_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dnx2_arr_formats_files_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_files);
    data = (const dnx_data_dnx2_arr_formats_files_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->arr_formats_file == NULL ? "" : data->arr_formats_file);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dnx2_arr_formats_MACT_AGE_PROFILE_ARR_CFG_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dnx2_arr_formats_MACT_AGE_PROFILE_ARR_CFG_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_MACT_AGE_PROFILE_ARR_CFG);
    data = (const dnx_data_dnx2_arr_formats_MACT_AGE_PROFILE_ARR_CFG_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->doc == NULL ? "" : data->doc);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->AGE_PROFILE);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->AGE_PROFILE_valid);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE);
    data = (const dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->doc == NULL ? "" : data->doc);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->DUAL_HOMING_CONFIG);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->DUAL_HOMING_CONFIG_valid);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->EGRESS_LAST_LAYER_CONFIG);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->EGRESS_LAST_LAYER_CONFIG_valid);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_MEMBERSHIP_INTERFACE_CONFIG);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ENCAP_DESTINATION_CONFIG);
            break;
        case 9:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ENCAP_DESTINATION_CONFIG_valid);
            break;
        case 10:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->QOS_MODEL_CONFIG);
            break;
        case 11:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->QOS_MODEL_CONFIG_valid);
            break;
        case 12:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FORWARD_DOMAIN_CONFIG);
            break;
        case 13:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FORWARD_DOMAIN_CONFIG_valid);
            break;
        case 14:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FORWARD_DOMAIN_NETWORK_NAME_CONFIG);
            break;
        case 15:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FORWARD_DOMAIN_NETWORK_NAME_CONFIG_valid);
            break;
        case 16:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG);
            break;
        case 17:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG_valid);
            break;
        case 18:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->MTU_PROFILE_CONFIG);
            break;
        case 19:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->MTU_PROFILE_CONFIG_valid);
            break;
        case 20:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->NWK_QOS_IDX_CONFIG);
            break;
        case 21:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->NWK_QOS_IDX_CONFIG_valid);
            break;
        case 22:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->REMARK_PROFILE_CONFIG);
            break;
        case 23:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->REMARK_PROFILE_CONFIG_valid);
            break;
        case 24:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->SOURCE_IDX_CONFIG);
            break;
        case 25:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->SOURCE_IDX_CONFIG_valid);
            break;
        case 26:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LIF_STATISTICS_OBJECT_COMMAND_CONFIG);
            break;
        case 27:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LIF_STATISTICS_OBJECT_COMMAND_CONFIG_valid);
            break;
        case 28:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LIF_STATISTICS_OBJECT_ID_CONFIG);
            break;
        case 29:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LIF_STATISTICS_OBJECT_ID_CONFIG_valid);
            break;
        case 30:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TTL_MODEL_CONFIG);
            break;
        case 31:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TTL_MODEL_CONFIG_valid);
            break;
        case 32:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TTL_IDX_CONFIG);
            break;
        case 33:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TTL_IDX_CONFIG_valid);
            break;
        case 34:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_EDIT_PROFILE_CONFIG);
            break;
        case 35:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_EDIT_PROFILE_CONFIG_valid);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE);
    data = (const dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->doc == NULL ? "" : data->doc);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->DUAL_HOMING_CONFIG);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->DUAL_HOMING_CONFIG_valid);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->EGRESS_LAST_LAYER_CONFIG);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->EGRESS_LAST_LAYER_CONFIG_valid);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_MEMBERSHIP_INTERFACE_CONFIG);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ENCAP_DESTINATION_CONFIG);
            break;
        case 9:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ENCAP_DESTINATION_CONFIG_valid);
            break;
        case 10:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->QOS_MODEL_CONFIG);
            break;
        case 11:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->QOS_MODEL_CONFIG_valid);
            break;
        case 12:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FORWARD_DOMAIN_CONFIG);
            break;
        case 13:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FORWARD_DOMAIN_CONFIG_valid);
            break;
        case 14:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FORWARD_DOMAIN_NETWORK_NAME_CONFIG);
            break;
        case 15:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FORWARD_DOMAIN_NETWORK_NAME_CONFIG_valid);
            break;
        case 16:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG);
            break;
        case 17:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG_valid);
            break;
        case 18:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->MTU_PROFILE_CONFIG);
            break;
        case 19:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->MTU_PROFILE_CONFIG_valid);
            break;
        case 20:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->NWK_QOS_IDX_CONFIG);
            break;
        case 21:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->NWK_QOS_IDX_CONFIG_valid);
            break;
        case 22:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->REMARK_PROFILE_CONFIG);
            break;
        case 23:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->REMARK_PROFILE_CONFIG_valid);
            break;
        case 24:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->SOURCE_IDX_CONFIG);
            break;
        case 25:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->SOURCE_IDX_CONFIG_valid);
            break;
        case 26:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LIF_STATISTICS_OBJECT_COMMAND_CONFIG);
            break;
        case 27:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LIF_STATISTICS_OBJECT_COMMAND_CONFIG_valid);
            break;
        case 28:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LIF_STATISTICS_OBJECT_ID_CONFIG);
            break;
        case 29:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LIF_STATISTICS_OBJECT_ID_CONFIG_valid);
            break;
        case 30:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TTL_MODEL_CONFIG);
            break;
        case 31:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TTL_MODEL_CONFIG_valid);
            break;
        case 32:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TTL_IDX_CONFIG);
            break;
        case 33:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TTL_IDX_CONFIG_valid);
            break;
        case 34:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_EDIT_PROFILE_CONFIG);
            break;
        case 35:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_EDIT_PROFILE_CONFIG_valid);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE);
    data = (const dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->doc == NULL ? "" : data->doc);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->DUAL_HOMING_CONFIG);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->DUAL_HOMING_CONFIG_valid);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->EGRESS_LAST_LAYER_CONFIG);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->EGRESS_LAST_LAYER_CONFIG_valid);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_MEMBERSHIP_INTERFACE_CONFIG);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ENCAP_DESTINATION_CONFIG);
            break;
        case 9:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ENCAP_DESTINATION_CONFIG_valid);
            break;
        case 10:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->QOS_MODEL_CONFIG);
            break;
        case 11:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->QOS_MODEL_CONFIG_valid);
            break;
        case 12:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FORWARD_DOMAIN_CONFIG);
            break;
        case 13:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FORWARD_DOMAIN_CONFIG_valid);
            break;
        case 14:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FORWARD_DOMAIN_NETWORK_NAME_CONFIG);
            break;
        case 15:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FORWARD_DOMAIN_NETWORK_NAME_CONFIG_valid);
            break;
        case 16:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG);
            break;
        case 17:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG_valid);
            break;
        case 18:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->MTU_PROFILE_CONFIG);
            break;
        case 19:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->MTU_PROFILE_CONFIG_valid);
            break;
        case 20:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->NWK_QOS_IDX_CONFIG);
            break;
        case 21:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->NWK_QOS_IDX_CONFIG_valid);
            break;
        case 22:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->REMARK_PROFILE_CONFIG);
            break;
        case 23:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->REMARK_PROFILE_CONFIG_valid);
            break;
        case 24:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->SOURCE_IDX_CONFIG);
            break;
        case 25:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->SOURCE_IDX_CONFIG_valid);
            break;
        case 26:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LIF_STATISTICS_OBJECT_COMMAND_CONFIG);
            break;
        case 27:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LIF_STATISTICS_OBJECT_COMMAND_CONFIG_valid);
            break;
        case 28:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LIF_STATISTICS_OBJECT_ID_CONFIG);
            break;
        case 29:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LIF_STATISTICS_OBJECT_ID_CONFIG_valid);
            break;
        case 30:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TTL_MODEL_CONFIG);
            break;
        case 31:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TTL_MODEL_CONFIG_valid);
            break;
        case 32:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TTL_IDX_CONFIG);
            break;
        case 33:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TTL_IDX_CONFIG_valid);
            break;
        case 34:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_EDIT_PROFILE_CONFIG);
            break;
        case 35:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_EDIT_PROFILE_CONFIG_valid);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE);
    data = (const dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->doc == NULL ? "" : data->doc);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->DUAL_HOMING_CONFIG);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->DUAL_HOMING_CONFIG_valid);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->EGRESS_LAST_LAYER_CONFIG);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->EGRESS_LAST_LAYER_CONFIG_valid);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_MEMBERSHIP_INTERFACE_CONFIG);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ENCAP_DESTINATION_CONFIG);
            break;
        case 9:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ENCAP_DESTINATION_CONFIG_valid);
            break;
        case 10:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->QOS_MODEL_CONFIG);
            break;
        case 11:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->QOS_MODEL_CONFIG_valid);
            break;
        case 12:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FORWARD_DOMAIN_CONFIG);
            break;
        case 13:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FORWARD_DOMAIN_CONFIG_valid);
            break;
        case 14:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FORWARD_DOMAIN_NETWORK_NAME_CONFIG);
            break;
        case 15:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FORWARD_DOMAIN_NETWORK_NAME_CONFIG_valid);
            break;
        case 16:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG);
            break;
        case 17:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG_valid);
            break;
        case 18:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->MTU_PROFILE_CONFIG);
            break;
        case 19:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->MTU_PROFILE_CONFIG_valid);
            break;
        case 20:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->NWK_QOS_IDX_CONFIG);
            break;
        case 21:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->NWK_QOS_IDX_CONFIG_valid);
            break;
        case 22:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->REMARK_PROFILE_CONFIG);
            break;
        case 23:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->REMARK_PROFILE_CONFIG_valid);
            break;
        case 24:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->SOURCE_IDX_CONFIG);
            break;
        case 25:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->SOURCE_IDX_CONFIG_valid);
            break;
        case 26:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LIF_STATISTICS_OBJECT_COMMAND_CONFIG);
            break;
        case 27:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LIF_STATISTICS_OBJECT_COMMAND_CONFIG_valid);
            break;
        case 28:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LIF_STATISTICS_OBJECT_ID_CONFIG);
            break;
        case 29:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LIF_STATISTICS_OBJECT_ID_CONFIG_valid);
            break;
        case 30:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TTL_MODEL_CONFIG);
            break;
        case 31:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TTL_MODEL_CONFIG_valid);
            break;
        case 32:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TTL_IDX_CONFIG);
            break;
        case 33:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TTL_IDX_CONFIG_valid);
            break;
        case 34:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_EDIT_PROFILE_CONFIG);
            break;
        case 35:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_EDIT_PROFILE_CONFIG_valid);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE);
    data = (const dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->doc == NULL ? "" : data->doc);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->DUAL_HOMING_CONFIG);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->DUAL_HOMING_CONFIG_valid);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->EGRESS_LAST_LAYER_CONFIG);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->EGRESS_LAST_LAYER_CONFIG_valid);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_MEMBERSHIP_INTERFACE_CONFIG);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ENCAP_DESTINATION_CONFIG);
            break;
        case 9:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ENCAP_DESTINATION_CONFIG_valid);
            break;
        case 10:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->QOS_MODEL_CONFIG);
            break;
        case 11:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->QOS_MODEL_CONFIG_valid);
            break;
        case 12:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FORWARD_DOMAIN_CONFIG);
            break;
        case 13:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FORWARD_DOMAIN_CONFIG_valid);
            break;
        case 14:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FORWARD_DOMAIN_NETWORK_NAME_CONFIG);
            break;
        case 15:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FORWARD_DOMAIN_NETWORK_NAME_CONFIG_valid);
            break;
        case 16:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG);
            break;
        case 17:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG_valid);
            break;
        case 18:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->MTU_PROFILE_CONFIG);
            break;
        case 19:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->MTU_PROFILE_CONFIG_valid);
            break;
        case 20:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->NWK_QOS_IDX_CONFIG);
            break;
        case 21:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->NWK_QOS_IDX_CONFIG_valid);
            break;
        case 22:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->REMARK_PROFILE_CONFIG);
            break;
        case 23:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->REMARK_PROFILE_CONFIG_valid);
            break;
        case 24:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->SOURCE_IDX_CONFIG);
            break;
        case 25:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->SOURCE_IDX_CONFIG_valid);
            break;
        case 26:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LIF_STATISTICS_OBJECT_COMMAND_CONFIG);
            break;
        case 27:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LIF_STATISTICS_OBJECT_COMMAND_CONFIG_valid);
            break;
        case 28:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LIF_STATISTICS_OBJECT_ID_CONFIG);
            break;
        case 29:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LIF_STATISTICS_OBJECT_ID_CONFIG_valid);
            break;
        case 30:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TTL_MODEL_CONFIG);
            break;
        case 31:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TTL_MODEL_CONFIG_valid);
            break;
        case 32:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TTL_IDX_CONFIG);
            break;
        case 33:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TTL_IDX_CONFIG_valid);
            break;
        case 34:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_EDIT_PROFILE_CONFIG);
            break;
        case 35:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_EDIT_PROFILE_CONFIG_valid);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dnx2_arr_formats_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dnx2_arr_formats_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE);
    data = (const dnx_data_dnx2_arr_formats_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->doc == NULL ? "" : data->doc);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_MEMBERSHIP_INTERFACE_CONFIG);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ENCAP_DESTINATION_CONFIG);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ENCAP_DESTINATION_CONFIG_valid);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->REMARK_PROFILE_CONFIG);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->REMARK_PROFILE_CONFIG_valid);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dnx2_arr_formats_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dnx2_arr_formats_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE);
    data = (const dnx_data_dnx2_arr_formats_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->doc == NULL ? "" : data->doc);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_MEMBERSHIP_INTERFACE_CONFIG);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ENCAP_DESTINATION_CONFIG);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ENCAP_DESTINATION_CONFIG_valid);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->REMARK_PROFILE_CONFIG);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->REMARK_PROFILE_CONFIG_valid);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE);
    data = (const dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->doc == NULL ? "" : data->doc);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_MEMBERSHIP_INTERFACE_CONFIG);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ENCAP_DESTINATION_CONFIG);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ENCAP_DESTINATION_CONFIG_valid);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->REMARK_PROFILE_CONFIG);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->REMARK_PROFILE_CONFIG_valid);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE);
    data = (const dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->doc == NULL ? "" : data->doc);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_MEMBERSHIP_INTERFACE_CONFIG);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ENCAP_DESTINATION_CONFIG);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ENCAP_DESTINATION_CONFIG_valid);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->REMARK_PROFILE_CONFIG);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->REMARK_PROFILE_CONFIG_valid);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE);
    data = (const dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->doc == NULL ? "" : data->doc);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_MEMBERSHIP_INTERFACE_CONFIG);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ENCAP_DESTINATION_CONFIG);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ENCAP_DESTINATION_CONFIG_valid);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->REMARK_PROFILE_CONFIG);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->REMARK_PROFILE_CONFIG_valid);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE);
    data = (const dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->doc == NULL ? "" : data->doc);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_MEMBERSHIP_INTERFACE_CONFIG);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ENCAP_DESTINATION_CONFIG);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ENCAP_DESTINATION_CONFIG_valid);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->REMARK_PROFILE_CONFIG);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->REMARK_PROFILE_CONFIG_valid);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE);
    data = (const dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->doc == NULL ? "" : data->doc);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_MEMBERSHIP_INTERFACE_CONFIG);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ENCAP_DESTINATION_CONFIG);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ENCAP_DESTINATION_CONFIG_valid);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->REMARK_PROFILE_CONFIG);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->REMARK_PROFILE_CONFIG_valid);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0);
    data = (const dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->doc == NULL ? "" : data->doc);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ETHERNET_EGRESS_LAST_LAYER_CONFIG);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ETHERNET_EGRESS_LAST_LAYER_CONFIG_valid);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ETHERNET_EGRESS_VLAN_MEMBERSHIP_INTERFACE_CONFIG);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ETHERNET_EGRESS_VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ENCAP_DESTINATION_CONFIG);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ENCAP_DESTINATION_CONFIG_valid);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FORWARDING_DOMAIN_CONFIG);
            break;
        case 9:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FORWARDING_DOMAIN_CONFIG_valid);
            break;
        case 10:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->QOS_MODEL_CONFIG);
            break;
        case 11:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->QOS_MODEL_CONFIG_valid);
            break;
        case 12:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->MTU_PROFILE_CONFIG);
            break;
        case 13:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->MTU_PROFILE_CONFIG_valid);
            break;
        case 14:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->NWK_QOS_INDEX_CONFIG);
            break;
        case 15:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->NWK_QOS_INDEX_CONFIG_valid);
            break;
        case 16:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ETHERNET_OUTLIF_PROFILE_CONFIG);
            break;
        case 17:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ETHERNET_OUTLIF_PROFILE_CONFIG_valid);
            break;
        case 18:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->REMARK_PROFILE_CONFIG);
            break;
        case 19:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->REMARK_PROFILE_CONFIG_valid);
            break;
        case 20:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LIF_STATISTICS_OBJECT_COMMAND_CONFIG);
            break;
        case 21:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LIF_STATISTICS_OBJECT_COMMAND_CONFIG_valid);
            break;
        case 22:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LIF_STATISTICS_OBJECT_ID_CONFIG);
            break;
        case 23:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LIF_STATISTICS_OBJECT_ID_CONFIG_valid);
            break;
        case 24:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TTL_MODEL_CONFIG);
            break;
        case 25:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TTL_MODEL_CONFIG_valid);
            break;
        case 26:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ETHERNET_EGRESS_VLAN_EDIT_PROFILE_CONFIG);
            break;
        case 27:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ETHERNET_EGRESS_VLAN_EDIT_PROFILE_CONFIG_valid);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1);
    data = (const dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->doc == NULL ? "" : data->doc);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ETHERNET_EGRESS_VLAN_MEMBERSHIP_INTERFACE_CONFIG);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ETHERNET_EGRESS_VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ENCAP_DESTINATION_CONFIG);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ENCAP_DESTINATION_CONFIG_valid);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->REMARK_PROFILE_CONFIG);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->REMARK_PROFILE_CONFIG_valid);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION);
    data = (const dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->doc == NULL ? "" : data->doc);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LLVP_PROFILE_CONFIG);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LLVP_PROFILE_CONFIG_valid);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->EM_ACCESS_CMD_CONFIG);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->EM_ACCESS_CMD_CONFIG_valid);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->NAME_SPACE_CONFIG);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->NAME_SPACE_CONFIG_valid);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FWD_DOMAIN_CONFIG);
            break;
        case 9:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FWD_DOMAIN_CONFIG_valid);
            break;
        case 10:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->OAM_LIF_SET_CONFIG);
            break;
        case 11:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->OAM_LIF_SET_CONFIG_valid);
            break;
        case 12:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->OUTLIF_PROFILE_CONFIG);
            break;
        case 13:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->OUTLIF_PROFILE_CONFIG_valid);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION___array_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION___array_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array);
    data = (const dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION___array_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->doc == NULL ? "" : data->doc);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LLVP_PROFILE_CONFIG);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LLVP_PROFILE_CONFIG_valid);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->EM_ACCESS_CMD_CONFIG);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->EM_ACCESS_CMD_CONFIG_valid);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->NAME_SPACE_CONFIG);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->NAME_SPACE_CONFIG_valid);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FWD_DOMAIN_CONFIG);
            break;
        case 9:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FWD_DOMAIN_CONFIG_valid);
            break;
        case 10:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->OAM_LIF_SET_CONFIG);
            break;
        case 11:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->OAM_LIF_SET_CONFIG_valid);
            break;
        case 12:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->OUTLIF_PROFILE_CONFIG);
            break;
        case 13:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->OUTLIF_PROFILE_CONFIG_valid);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION);
    data = (const dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->doc == NULL ? "" : data->doc);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ACTION_PROFILE_CONFIG);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ACTION_PROFILE_CONFIG_valid);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->PROTECTION_PATH_CONFIG);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->PROTECTION_PATH_CONFIG_valid);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->PROTECTION_PTR_CONFIG);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->PROTECTION_PTR_CONFIG_valid);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->STAT_OBJ_CMD_CONFIG);
            break;
        case 9:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->STAT_OBJ_CMD_CONFIG_valid);
            break;
        case 10:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->STAT_OBJ_ID_CONFIG);
            break;
        case 11:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->STAT_OBJ_ID_CONFIG_valid);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array);
    data = (const dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->doc == NULL ? "" : data->doc);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ACTION_PROFILE_CONFIG);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ACTION_PROFILE_CONFIG_valid);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->PROTECTION_PATH_CONFIG);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->PROTECTION_PATH_CONFIG_valid);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->PROTECTION_PTR_CONFIG);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->PROTECTION_PTR_CONFIG_valid);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->STAT_OBJ_CMD_CONFIG);
            break;
        case 9:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->STAT_OBJ_CMD_CONFIG_valid);
            break;
        case 10:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->STAT_OBJ_ID_CONFIG);
            break;
        case 11:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->STAT_OBJ_ID_CONFIG_valid);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION);
    data = (const dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->doc == NULL ? "" : data->doc);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->QOS_MODEL_CONFIG);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->QOS_MODEL_CONFIG_valid);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->NWK_QOS_IDX_CONFIG);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->NWK_QOS_IDX_CONFIG_valid);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->OUTLIF_PROFILE_CONFIG);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->OUTLIF_PROFILE_CONFIG_valid);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->REMARK_PROFILE_CONFIG);
            break;
        case 9:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->REMARK_PROFILE_CONFIG_valid);
            break;
        case 10:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TTL_MODEL_CONFIG);
            break;
        case 11:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TTL_MODEL_CONFIG_valid);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dnx2_arr_formats_IPPB_ACCEPTED_ARR_FORMAT_CONFIG_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dnx2_arr_formats_IPPB_ACCEPTED_ARR_FORMAT_CONFIG_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG);
    data = (const dnx_data_dnx2_arr_formats_IPPB_ACCEPTED_ARR_FORMAT_CONFIG_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->doc == NULL ? "" : data->doc);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->DATA_1);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->DATA_1_valid);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->DATA_2);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->DATA_2_valid);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TYPE);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TYPE_valid);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->DESTINATION);
            break;
        case 9:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->DESTINATION_valid);
            break;
        case 10:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->STRENGTH);
            break;
        case 11:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->STRENGTH_valid);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dnx2_arr_formats_MACT_MRQ_ARR_CFG_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dnx2_arr_formats_MACT_MRQ_ARR_CFG_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_MACT_MRQ_ARR_CFG);
    data = (const dnx_data_dnx2_arr_formats_MACT_MRQ_ARR_CFG_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->doc == NULL ? "" : data->doc);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->STRENGTH);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->STRENGTH_valid);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dnx2_arr_formats_IPPB_EXPECTED_ARR_FORMAT_CONFIG_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dnx2_arr_formats_IPPB_EXPECTED_ARR_FORMAT_CONFIG_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG);
    data = (const dnx_data_dnx2_arr_formats_IPPB_EXPECTED_ARR_FORMAT_CONFIG_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->doc == NULL ? "" : data->doc);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->DESTINATION);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->DESTINATION_valid);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->DATA_1);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->DATA_1_valid);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->DATA_2);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->DATA_2_valid);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TYPE);
            break;
        case 9:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TYPE_valid);
            break;
        case 10:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->STRENGTH);
            break;
        case 11:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->STRENGTH_valid);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dnx2_arr_formats_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dnx2_arr_formats_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array);
    data = (const dnx_data_dnx2_arr_formats_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->doc == NULL ? "" : data->doc);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->DESTINATION_CONFIG);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->DESTINATION_CONFIG_valid);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->EEI_CONFIG);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->EEI_CONFIG_valid);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->OUT_LIF_0_CONFIG);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->OUT_LIF_0_CONFIG_valid);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->OUT_LIF_1_CONFIG);
            break;
        case 9:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->OUT_LIF_1_CONFIG_valid);
            break;
        case 10:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->WEAK_TM_FLOW_CONFIG);
            break;
        case 11:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->WEAK_TM_FLOW_CONFIG_valid);
            break;
        case 12:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->MC_RPF_MODE_CONFIG);
            break;
        case 13:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->MC_RPF_MODE_CONFIG_valid);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dnx2_arr_formats_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dnx2_arr_formats_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array);
    data = (const dnx_data_dnx2_arr_formats_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->doc == NULL ? "" : data->doc);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->DESTINATION_CONFIG);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->DESTINATION_CONFIG_valid);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->EEI_CONFIG);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->EEI_CONFIG_valid);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->OUTLIF_0_CONFIG);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->OUTLIF_0_CONFIG_valid);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->OUTLIF_1_CONFIG);
            break;
        case 9:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->OUTLIF_1_CONFIG_valid);
            break;
        case 10:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->IRPP_FWD_RAW_DATA);
            break;
        case 11:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->IRPP_FWD_RAW_DATA_valid);
            break;
        case 12:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->DEFAULT_CONFIG);
            break;
        case 13:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->DEFAULT_CONFIG_valid);
            break;
        case 14:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->STAT_OBJ_CMD_CONFIG);
            break;
        case 15:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->STAT_OBJ_CMD_CONFIG_valid);
            break;
        case 16:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->STAT_OBJ_ID_CONFIG);
            break;
        case 17:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->STAT_OBJ_ID_CONFIG_valid);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dnx2_arr_formats_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dnx2_arr_formats_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array);
    data = (const dnx_data_dnx2_arr_formats_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->doc == NULL ? "" : data->doc);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->DESTINATION_CONFIG);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->DESTINATION_CONFIG_valid);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->OUTLIF_0_CONFIG);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->OUTLIF_0_CONFIG_valid);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->DEFAULT_CONFIG);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->DEFAULT_CONFIG_valid);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->SA_DROP_CONFIG);
            break;
        case 9:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->SA_DROP_CONFIG_valid);
            break;
        case 10:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->STAT_OBJ_CMD_CONFIG);
            break;
        case 11:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->STAT_OBJ_CMD_CONFIG_valid);
            break;
        case 12:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->STAT_OBJ_ID_CONFIG);
            break;
        case 13:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->STAT_OBJ_ID_CONFIG_valid);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dnx2_arr_formats_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dnx2_arr_formats_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array);
    data = (const dnx_data_dnx2_arr_formats_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->doc == NULL ? "" : data->doc);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LEFT_CONFIG);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LEFT_CONFIG_valid);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LEFT_STAT_OBJECT_CMD_CONFIG);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LEFT_STAT_OBJECT_CMD_CONFIG_valid);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LEFT_STAT_OBJECT_ID_CONFIG);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LEFT_STAT_OBJECT_ID_CONFIG_valid);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->PROTECTION_PTR_CONFIG);
            break;
        case 9:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->PROTECTION_PTR_CONFIG_valid);
            break;
        case 10:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->RIGHT_CONFIG);
            break;
        case 11:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->RIGHT_CONFIG_valid);
            break;
        case 12:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->RIGHT_STAT_OBJECT_CMD_CONFIG);
            break;
        case 13:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->RIGHT_STAT_OBJECT_CMD_CONFIG_valid);
            break;
        case 14:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->RIGHT_STAT_OBJECT_ID_CONFIG);
            break;
        case 15:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->RIGHT_STAT_OBJECT_ID_CONFIG_valid);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dnx2_arr_formats_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dnx2_arr_formats_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1);
    data = (const dnx_data_dnx2_arr_formats_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->doc == NULL ? "" : data->doc);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->DA_NOT_FOUND_DESTINATION_CONFIG);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->DA_NOT_FOUND_DESTINATION_CONFIG_valid);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ECN_MAPPING_PROFILE_CONFIG);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ECN_MAPPING_PROFILE_CONFIG_valid);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->MY_MAC_CONFIG);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->MY_MAC_CONFIG_valid);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->MY_MAC_PREFIX_CONFIG);
            break;
        case 9:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->MY_MAC_PREFIX_CONFIG_valid);
            break;
        case 10:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->PROPAGATION_PROFILE_CONFIG);
            break;
        case 11:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->PROPAGATION_PROFILE_CONFIG_valid);
            break;
        case 12:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->QOS_PROFILE_CONFIG);
            break;
        case 13:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->QOS_PROFILE_CONFIG_valid);
            break;
        case 14:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->STAT_OBJ_CMD_CONFIG);
            break;
        case 15:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->STAT_OBJ_CMD_CONFIG_valid);
            break;
        case 16:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->STAT_OBJ_ID_CONFIG);
            break;
        case 17:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->STAT_OBJ_ID_CONFIG_valid);
            break;
        case 18:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TOPOLOGY_ID_CONFIG);
            break;
        case 19:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TOPOLOGY_ID_CONFIG_valid);
            break;
        case 20:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VRF_CONFIG);
            break;
        case 21:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VRF_CONFIG_valid);
            break;
        case 22:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VRID_BITMAP_CONFIG);
            break;
        case 23:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VRID_BITMAP_CONFIG_valid);
            break;
        case 24:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VSI_PROFILE_CONFIG);
            break;
        case 25:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VSI_PROFILE_CONFIG_valid);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dnx2_arr_formats_IPPF_VSI_FORMAT_CONFIGURATION_TABLE_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dnx2_arr_formats_IPPF_VSI_FORMAT_CONFIGURATION_TABLE_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE);
    data = (const dnx_data_dnx2_arr_formats_IPPF_VSI_FORMAT_CONFIGURATION_TABLE_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->doc == NULL ? "" : data->doc);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->DA_NOT_FOUND_DESTINATION_CONFIG);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->DA_NOT_FOUND_DESTINATION_CONFIG_valid);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ECN_MAPPING_PROFILE_CONFIG);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ECN_MAPPING_PROFILE_CONFIG_valid);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->MY_MAC_CONFIG);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->MY_MAC_CONFIG_valid);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->MY_MAC_PREFIX_CONFIG);
            break;
        case 9:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->MY_MAC_PREFIX_CONFIG_valid);
            break;
        case 10:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->PROPAGATION_PROFILE_CONFIG);
            break;
        case 11:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->PROPAGATION_PROFILE_CONFIG_valid);
            break;
        case 12:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->QOS_PROFILE_CONFIG);
            break;
        case 13:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->QOS_PROFILE_CONFIG_valid);
            break;
        case 14:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->STAT_OBJ_CMD_CONFIG);
            break;
        case 15:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->STAT_OBJ_CMD_CONFIG_valid);
            break;
        case 16:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->STAT_OBJ_ID_CONFIG);
            break;
        case 17:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->STAT_OBJ_ID_CONFIG_valid);
            break;
        case 18:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TOPOLOGY_ID_CONFIG);
            break;
        case 19:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TOPOLOGY_ID_CONFIG_valid);
            break;
        case 20:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VRF_CONFIG);
            break;
        case 21:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VRF_CONFIG_valid);
            break;
        case 22:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VRID_BITMAP_CONFIG);
            break;
        case 23:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VRID_BITMAP_CONFIG_valid);
            break;
        case 24:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VSI_PROFILE_CONFIG);
            break;
        case 25:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VSI_PROFILE_CONFIG_valid);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array);
    data = (const dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->doc == NULL ? "" : data->doc);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ACTION_PROFILE_IDX_CONFIG);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ACTION_PROFILE_IDX_CONFIG_valid);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->DESTINATION_CONFIG);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->DESTINATION_CONFIG_valid);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ECN_MAPPING_PROFILE_CONFIG);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ECN_MAPPING_PROFILE_CONFIG_valid);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->EEI_CONFIG);
            break;
        case 9:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->EEI_CONFIG_valid);
            break;
        case 10:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FORWARD_DOMAIN_CONFIG);
            break;
        case 11:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FORWARD_DOMAIN_CONFIG_valid);
            break;
        case 12:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FORWARD_DOMAIN_ASSIGNMENT_MODE_CONFIG);
            break;
        case 13:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FORWARD_DOMAIN_ASSIGNMENT_MODE_CONFIG_valid);
            break;
        case 14:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->SYS_INLIF_CONFIG);
            break;
        case 15:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->SYS_INLIF_CONFIG_valid);
            break;
        case 16:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->OUT_LIF_CONFIG);
            break;
        case 17:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->OUT_LIF_CONFIG_valid);
            break;
        case 18:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LIF_PROFILE_CONFIG);
            break;
        case 19:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LIF_PROFILE_CONFIG_valid);
            break;
        case 20:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LEARN_EN_CONFIG);
            break;
        case 21:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LEARN_EN_CONFIG_valid);
            break;
        case 22:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LEARN_INFO_CONFIG);
            break;
        case 23:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LEARN_INFO_CONFIG_valid);
            break;
        case 24:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LEARN_PAYLOAD_CONTEXT_CONFIG);
            break;
        case 25:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LEARN_PAYLOAD_CONTEXT_CONFIG_valid);
            break;
        case 26:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LIF_GENERIC_DATA_0_CONFIG);
            break;
        case 27:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LIF_GENERIC_DATA_0_CONFIG_valid);
            break;
        case 28:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LIF_GENERIC_DATA_1_CONFIG);
            break;
        case 29:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LIF_GENERIC_DATA_1_CONFIG_valid);
            break;
        case 30:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->PROTECTION_PTR_CONFIG);
            break;
        case 31:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->PROTECTION_PTR_CONFIG_valid);
            break;
        case 32:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->NEXT_LAYER_NETWORK_DOMAIN_CONFIG);
            break;
        case 33:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->NEXT_LAYER_NETWORK_DOMAIN_CONFIG_valid);
            break;
        case 34:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->NEXT_PARSER_CONTEXT_CONFIG);
            break;
        case 35:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->NEXT_PARSER_CONTEXT_CONFIG_valid);
            break;
        case 36:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->OAM_LIF_SET_CONFIG);
            break;
        case 37:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->OAM_LIF_SET_CONFIG_valid);
            break;
        case 38:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->PROPAGATION_PROFILE_CONFIG);
            break;
        case 39:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->PROPAGATION_PROFILE_CONFIG_valid);
            break;
        case 40:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->PROTECTION_PATH_CONFIG);
            break;
        case 41:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->PROTECTION_PATH_CONFIG_valid);
            break;
        case 42:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->QOS_PROFILE_CONFIG);
            break;
        case 43:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->QOS_PROFILE_CONFIG_valid);
            break;
        case 44:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->SERVICE_TYPE_CONFIG);
            break;
        case 45:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->SERVICE_TYPE_CONFIG_valid);
            break;
        case 46:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->STAT_OBJ_CMD_CONFIG);
            break;
        case 47:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->STAT_OBJ_CMD_CONFIG_valid);
            break;
        case 48:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->STAT_OBJ_ID_CONFIG);
            break;
        case 49:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->STAT_OBJ_ID_CONFIG_valid);
            break;
        case 50:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TERMINATION_TYPE_CONFIG);
            break;
        case 51:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TERMINATION_TYPE_CONFIG_valid);
            break;
        case 52:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_EDIT_PCP_DEI_PROFILE_CONFIG);
            break;
        case 53:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_EDIT_PCP_DEI_PROFILE_CONFIG_valid);
            break;
        case 54:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_EDIT_PROFILE_CONFIG);
            break;
        case 55:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_EDIT_PROFILE_CONFIG_valid);
            break;
        case 56:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_EDIT_VID_1_CONFIG);
            break;
        case 57:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_EDIT_VID_1_CONFIG_valid);
            break;
        case 58:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_EDIT_VID_2_CONFIG);
            break;
        case 59:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_EDIT_VID_2_CONFIG_valid);
            break;
        case 60:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_MEMBER_IF_CONFIG);
            break;
        case 61:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_MEMBER_IF_CONFIG_valid);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array);
    data = (const dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->doc == NULL ? "" : data->doc);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ACTION_PROFILE_IDX_CONFIG);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ACTION_PROFILE_IDX_CONFIG_valid);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->DESTINATION_CONFIG);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->DESTINATION_CONFIG_valid);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ECN_MAPPING_PROFILE_CONFIG);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ECN_MAPPING_PROFILE_CONFIG_valid);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->EEI_CONFIG);
            break;
        case 9:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->EEI_CONFIG_valid);
            break;
        case 10:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FORWARD_DOMAIN_CONFIG);
            break;
        case 11:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FORWARD_DOMAIN_CONFIG_valid);
            break;
        case 12:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FORWARD_DOMAIN_ASSIGNMENT_MODE_CONFIG);
            break;
        case 13:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FORWARD_DOMAIN_ASSIGNMENT_MODE_CONFIG_valid);
            break;
        case 14:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->SYS_INLIF_CONFIG);
            break;
        case 15:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->SYS_INLIF_CONFIG_valid);
            break;
        case 16:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->OUT_LIF_CONFIG);
            break;
        case 17:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->OUT_LIF_CONFIG_valid);
            break;
        case 18:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LIF_PROFILE_CONFIG);
            break;
        case 19:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LIF_PROFILE_CONFIG_valid);
            break;
        case 20:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LEARN_EN_CONFIG);
            break;
        case 21:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LEARN_EN_CONFIG_valid);
            break;
        case 22:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LEARN_INFO_CONFIG);
            break;
        case 23:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LEARN_INFO_CONFIG_valid);
            break;
        case 24:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LEARN_PAYLOAD_CONTEXT_CONFIG);
            break;
        case 25:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LEARN_PAYLOAD_CONTEXT_CONFIG_valid);
            break;
        case 26:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LIF_GENERIC_DATA_0_CONFIG);
            break;
        case 27:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LIF_GENERIC_DATA_0_CONFIG_valid);
            break;
        case 28:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LIF_GENERIC_DATA_1_CONFIG);
            break;
        case 29:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LIF_GENERIC_DATA_1_CONFIG_valid);
            break;
        case 30:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->PROTECTION_PTR_CONFIG);
            break;
        case 31:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->PROTECTION_PTR_CONFIG_valid);
            break;
        case 32:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->NEXT_LAYER_NETWORK_DOMAIN_CONFIG);
            break;
        case 33:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->NEXT_LAYER_NETWORK_DOMAIN_CONFIG_valid);
            break;
        case 34:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->NEXT_PARSER_CONTEXT_CONFIG);
            break;
        case 35:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->NEXT_PARSER_CONTEXT_CONFIG_valid);
            break;
        case 36:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->OAM_LIF_SET_CONFIG);
            break;
        case 37:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->OAM_LIF_SET_CONFIG_valid);
            break;
        case 38:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->PROPAGATION_PROFILE_CONFIG);
            break;
        case 39:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->PROPAGATION_PROFILE_CONFIG_valid);
            break;
        case 40:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->PROTECTION_PATH_CONFIG);
            break;
        case 41:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->PROTECTION_PATH_CONFIG_valid);
            break;
        case 42:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->QOS_PROFILE_CONFIG);
            break;
        case 43:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->QOS_PROFILE_CONFIG_valid);
            break;
        case 44:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->SERVICE_TYPE_CONFIG);
            break;
        case 45:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->SERVICE_TYPE_CONFIG_valid);
            break;
        case 46:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->STAT_OBJ_CMD_CONFIG);
            break;
        case 47:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->STAT_OBJ_CMD_CONFIG_valid);
            break;
        case 48:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->STAT_OBJ_ID_CONFIG);
            break;
        case 49:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->STAT_OBJ_ID_CONFIG_valid);
            break;
        case 50:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TERMINATION_TYPE_CONFIG);
            break;
        case 51:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TERMINATION_TYPE_CONFIG_valid);
            break;
        case 52:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_EDIT_PCP_DEI_PROFILE_CONFIG);
            break;
        case 53:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_EDIT_PCP_DEI_PROFILE_CONFIG_valid);
            break;
        case 54:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_EDIT_PROFILE_CONFIG);
            break;
        case 55:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_EDIT_PROFILE_CONFIG_valid);
            break;
        case 56:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_EDIT_VID_1_CONFIG);
            break;
        case 57:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_EDIT_VID_1_CONFIG_valid);
            break;
        case 58:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_EDIT_VID_2_CONFIG);
            break;
        case 59:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_EDIT_VID_2_CONFIG_valid);
            break;
        case 60:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_MEMBER_IF_CONFIG);
            break;
        case 61:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VLAN_MEMBER_IF_CONFIG_valid);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dnx2_arr_formats_IPPD_LEARN_ARR_CONFIGURATION_TABLE_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dnx2_arr_formats_IPPD_LEARN_ARR_CONFIGURATION_TABLE_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_IPPD_LEARN_ARR_CONFIGURATION_TABLE);
    data = (const dnx_data_dnx2_arr_formats_IPPD_LEARN_ARR_CONFIGURATION_TABLE_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->doc == NULL ? "" : data->doc);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->DESTINATION_CONFIG);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->DESTINATION_CONFIG_valid);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->EEI_CONFIG);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->EEI_CONFIG_valid);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->OUTLIF_CONFIG);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->OUTLIF_CONFIG_valid);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dnx2_arr_formats_OLP_LEARN_ARR_CONFIGURATION_TABLE_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dnx2_arr_formats_OLP_LEARN_ARR_CONFIGURATION_TABLE_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_OLP_LEARN_ARR_CONFIGURATION_TABLE);
    data = (const dnx_data_dnx2_arr_formats_OLP_LEARN_ARR_CONFIGURATION_TABLE_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->doc == NULL ? "" : data->doc);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->DESTINATION_CONFIG);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->DESTINATION_CONFIG_valid);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->EEI_CONFIG);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->EEI_CONFIG_valid);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->OUTLIF_CONFIG);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->OUTLIF_CONFIG_valid);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_dnx2_arr_formats_ERPP_EGRESS_MCDB_FORMAT_CFG_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_dnx2_arr_formats_ERPP_EGRESS_MCDB_FORMAT_CFG_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ERPP_EGRESS_MCDB_FORMAT_CFG);
    data = (const dnx_data_dnx2_arr_formats_ERPP_EGRESS_MCDB_FORMAT_CFG_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->doc == NULL ? "" : data->doc);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->MCDB_OUTLIF_0_CONFIG);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->MCDB_OUTLIF_0_CONFIG_valid);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->MCDB_OUTLIF_1_CONFIG);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->MCDB_OUTLIF_1_CONFIG_valid);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->MCDB_OUTLIF_2_CONFIG);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->MCDB_OUTLIF_2_CONFIG_valid);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_dnx2_arr_formats_files_info_get(
    int unit)
{

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "files", "DNX2");
    }
    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_files);

}

const dnxc_data_table_info_t *
dnx_data_dnx2_arr_formats_MACT_AGE_PROFILE_ARR_CFG_info_get(
    int unit)
{

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "MACT_AGE_PROFILE_ARR_CFG", "DNX2");
    }
    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_MACT_AGE_PROFILE_ARR_CFG);

}

const dnxc_data_table_info_t *
dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_info_get(
    int unit)
{

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE", "DNX2");
    }
    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE);

}

const dnxc_data_table_info_t *
dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_info_get(
    int unit)
{

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE", "DNX2");
    }
    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE);

}

const dnxc_data_table_info_t *
dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_info_get(
    int unit)
{

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE", "DNX2");
    }
    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE);

}

const dnxc_data_table_info_t *
dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_info_get(
    int unit)
{

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE", "DNX2");
    }
    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE);

}

const dnxc_data_table_info_t *
dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_info_get(
    int unit)
{

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE", "DNX2");
    }
    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE);

}

const dnxc_data_table_info_t *
dnx_data_dnx2_arr_formats_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE_info_get(
    int unit)
{

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPPB_ENC_FORMAT_CONFIGURATION_TABLE", "DNX2");
    }
    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE);

}

const dnxc_data_table_info_t *
dnx_data_dnx2_arr_formats_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE_info_get(
    int unit)
{

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPPC_ENC_FORMAT_CONFIGURATION_TABLE", "DNX2");
    }
    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE);

}

const dnxc_data_table_info_t *
dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE_info_get(
    int unit)
{

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE", "DNX2");
    }
    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE);

}

const dnxc_data_table_info_t *
dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE_info_get(
    int unit)
{

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE", "DNX2");
    }
    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE);

}

const dnxc_data_table_info_t *
dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE_info_get(
    int unit)
{

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE", "DNX2");
    }
    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE);

}

const dnxc_data_table_info_t *
dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE_info_get(
    int unit)
{

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE", "DNX2");
    }
    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE);

}

const dnxc_data_table_info_t *
dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE_info_get(
    int unit)
{

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE", "DNX2");
    }
    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE);

}

const dnxc_data_table_info_t *
dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_info_get(
    int unit)
{

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0", "DNX2");
    }
    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0);

}

const dnxc_data_table_info_t *
dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1_info_get(
    int unit)
{

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1", "DNX2");
    }
    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1);

}

const dnxc_data_table_info_t *
dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION_info_get(
    int unit)
{

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPPA_PRP_EES_ARR_CONFIGURATION", "DNX2");
    }
    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION);

}

const dnxc_data_table_info_t *
dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION___array_info_get(
    int unit)
{

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPPA_PRP_EES_ARR_CONFIGURATION___array", "DNX2");
    }
    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPA_PRP_EES_ARR_CONFIGURATION___array);

}

const dnxc_data_table_info_t *
dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION_info_get(
    int unit)
{

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPPC_TERM_ETPS_ARR_1_CONFIGURATION", "DNX2");
    }
    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION);

}

const dnxc_data_table_info_t *
dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_info_get(
    int unit)
{

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array", "DNX2");
    }
    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array);

}

const dnxc_data_table_info_t *
dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION_info_get(
    int unit)
{

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ETPPC_TERM_ETPS_ARR_2_CONFIGURATION", "DNX2");
    }
    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION);

}

const dnxc_data_table_info_t *
dnx_data_dnx2_arr_formats_IPPB_ACCEPTED_ARR_FORMAT_CONFIG_info_get(
    int unit)
{

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IPPB_ACCEPTED_ARR_FORMAT_CONFIG", "DNX2");
    }
    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_IPPB_ACCEPTED_ARR_FORMAT_CONFIG);

}

const dnxc_data_table_info_t *
dnx_data_dnx2_arr_formats_MACT_MRQ_ARR_CFG_info_get(
    int unit)
{

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "MACT_MRQ_ARR_CFG", "DNX2");
    }
    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_MACT_MRQ_ARR_CFG);

}

const dnxc_data_table_info_t *
dnx_data_dnx2_arr_formats_IPPB_EXPECTED_ARR_FORMAT_CONFIG_info_get(
    int unit)
{

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IPPB_EXPECTED_ARR_FORMAT_CONFIG", "DNX2");
    }
    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_IPPB_EXPECTED_ARR_FORMAT_CONFIG);

}

const dnxc_data_table_info_t *
dnx_data_dnx2_arr_formats_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array_info_get(
    int unit)
{

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array", "DNX2");
    }
    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array);

}

const dnxc_data_table_info_t *
dnx_data_dnx2_arr_formats_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array_info_get(
    int unit)
{

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array", "DNX2");
    }
    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array);

}

const dnxc_data_table_info_t *
dnx_data_dnx2_arr_formats_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array_info_get(
    int unit)
{

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array", "DNX2");
    }
    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array);

}

const dnxc_data_table_info_t *
dnx_data_dnx2_arr_formats_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array_info_get(
    int unit)
{

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array", "DNX2");
    }
    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array);

}

const dnxc_data_table_info_t *
dnx_data_dnx2_arr_formats_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1_info_get(
    int unit)
{

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1", "DNX2");
    }
    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1);

}

const dnxc_data_table_info_t *
dnx_data_dnx2_arr_formats_IPPF_VSI_FORMAT_CONFIGURATION_TABLE_info_get(
    int unit)
{

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IPPF_VSI_FORMAT_CONFIGURATION_TABLE", "DNX2");
    }
    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_IPPF_VSI_FORMAT_CONFIGURATION_TABLE);

}

const dnxc_data_table_info_t *
dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_info_get(
    int unit)
{

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array", "DNX2");
    }
    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array);

}

const dnxc_data_table_info_t *
dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_info_get(
    int unit)
{

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array", "DNX2");
    }
    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array);

}

const dnxc_data_table_info_t *
dnx_data_dnx2_arr_formats_IPPD_LEARN_ARR_CONFIGURATION_TABLE_info_get(
    int unit)
{

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "IPPD_LEARN_ARR_CONFIGURATION_TABLE", "DNX2");
    }
    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_IPPD_LEARN_ARR_CONFIGURATION_TABLE);

}

const dnxc_data_table_info_t *
dnx_data_dnx2_arr_formats_OLP_LEARN_ARR_CONFIGURATION_TABLE_info_get(
    int unit)
{

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "OLP_LEARN_ARR_CONFIGURATION_TABLE", "DNX2");
    }
    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_OLP_LEARN_ARR_CONFIGURATION_TABLE);

}

const dnxc_data_table_info_t *
dnx_data_dnx2_arr_formats_ERPP_EGRESS_MCDB_FORMAT_CFG_info_get(
    int unit)
{

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "ERPP_EGRESS_MCDB_FORMAT_CFG", "DNX2");
    }
    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_dnx2_arr, dnx_data_dnx2_arr_submodule_formats, dnx_data_dnx2_arr_formats_table_ERPP_EGRESS_MCDB_FORMAT_CFG);

}



shr_error_e
dnx_data_dnx2_arr_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "dnx2_arr";
    module_data->nof_submodules = _dnx_data_dnx2_arr_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data dnx2_arr submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_dnx2_arr_prefix_init(unit, &module_data->submodules[dnx_data_dnx2_arr_submodule_prefix]));
    SHR_IF_ERR_EXIT(dnx_data_dnx2_arr_max_nof_entries_init(unit, &module_data->submodules[dnx_data_dnx2_arr_submodule_max_nof_entries]));
    SHR_IF_ERR_EXIT(dnx_data_dnx2_arr_formats_init(unit, &module_data->submodules[dnx_data_dnx2_arr_submodule_formats]));
    
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_dnx2_arr_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_dnx2_arr_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_dnx2_arr_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_dnx2_arr_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_dnx2_arr_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_dnx2_arr_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_dnx2_arr_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_dnx2_arr_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_dnx2_arr_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_dnx2_arr_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_dnx2_arr_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_dnx2_arr_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_dnx2_arr_attach(unit));
        SHR_IF_ERR_EXIT(q2a_b0_data_dnx2_arr_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_dnx2_arr_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_dnx2_arr_attach(unit));
        SHR_IF_ERR_EXIT(q2a_b0_data_dnx2_arr_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_dnx2_arr_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_dnx2_arr_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2p_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_dnx2_arr_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_dnx2_arr_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2p_a2(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_dnx2_arr_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_dnx2_arr_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2x_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_dnx2_arr_attach(unit));
        SHR_IF_ERR_EXIT(j2x_a0_data_dnx2_arr_attach(unit));
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

