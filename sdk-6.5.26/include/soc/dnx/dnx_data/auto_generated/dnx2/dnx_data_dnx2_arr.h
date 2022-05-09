
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_DNX2_ARR_H_

#define _DNX_DATA_DNX2_ARR_H_

#include <sal/limits.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/property.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx2/dnx_data_max_dnx2_arr.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



shr_error_e dnx_data_if_dnx2_arr_init(
    int unit);







typedef enum
{

    
    _dnx_data_dnx2_arr_prefix_feature_nof
} dnx_data_dnx2_arr_prefix_feature_e;



typedef int(
    *dnx_data_dnx2_arr_prefix_feature_get_f) (
    int unit,
    dnx_data_dnx2_arr_prefix_feature_e feature);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_Empty_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_FORWARD_DOMAIN_VRF_Encoding_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_ENCAP_TTL_IDX_MPLS_FHEI_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_Empty2_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_Always_Last_Layer_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_LIF_AHP_MPLS_Prefix_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_ENCAP_QOS_MODEL_MPLS_FHEI_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_ENCAP_QOS_IDX_MPLS_FHEI_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_OutLIF_Profile_Orientation_Only_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_FORWARD_DOMAIN_VRF_IPvX_Tunnel_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_QOS_MODEL_1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_ESEM_COMMAND_IPv6_TUNNELS_FODO_Namespace_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_ENCAP_TTL_MODEL_MPLS_FHEI_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_ESEM_COMMAND_IPv4_VxLAN_OR_VXLAN_GPE_FODO_Namespace_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_FORWARD_QOS_MODEL_NWK_QOS_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_ESEM_COMMAND_ARP_PLUS_AC_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_IRPP_FEC_ENTRY_FORMAT___prefix_Empty_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_IRPP_FEC_ENTRY_FORMAT___prefix_EEI_W_OUTLIF_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_IRPP_FEC_ENTRY_FORMAT___prefix_OUTLIF_ETH_RIF_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_IRPP_FEC_ENTRY_FORMAT___prefix_EEI_W_POP_Command_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_IRPP_FEC_ENTRY_FORMAT___prefix_EEI_W_SWAP_Command_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_IRPP_FEC_SUPER_ENTRY_FORMAT___prefix_Empty_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_IRPP_FWD_STAGE_FORMATS___prefix_Empty_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b11_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b0001_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b0010_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b0011_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b0100_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b0101_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b01111_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_Empty_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_Service_Type_Extended_P2P_W_Learning_Prefix_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_Parser_Context_IPvX_DEMUX_or_ETH_B1__4b_Prefix_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_Empty2_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_Free_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_VRF_VSI_Not_Found_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_Forward_Domain_Assignment_Mode_LIF_or_VLAN_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_Forward_Domain_VRF_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_NEXT_LAYER_NETWORK_DOMAIN_TNIorVNI_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_NEXT_LAYER_NETWORK_DOMAIN_PWE_WO_SD_TAGS_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF_OPTIMIZED_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_VSI_P2P_MSBS_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_QoS_Profile_short_Prefix_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_Learn_info_destination_port_encoding_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_IRPP_VSI_FORMATS___prefix_Empty_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_IRPP_VSI_FORMATS___prefix_VSI_Profile_Prefix_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_IRPP_VSI_FORMATS___prefix_Dest_MC_ID_Prefix_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_IRPP_VSI_FORMATS___prefix_Dest_FEC_Port_Trap_Prefix_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_IRPP_VSI_FORMATS___prefix_Topology_ID_Prefix_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_IRPP_VSI_FORMATS___prefix_QOS_Profile_Prefix_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_IRPP_VSI_FORMATS___prefix_My_Mac_Prefix_Prefix_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_prefix_IRPP_VSI_FORMATS___prefix_VSI_Stat_Obj_Cmd_Prefix_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_dnx2_arr_prefix_feature_get_f feature_get;
    
    dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_Empty_get_f ETPP_ETPS_FORMATS___prefix_Empty_get;
    
    dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_FORWARD_DOMAIN_VRF_Encoding_get_f ETPP_ETPS_FORMATS___prefix_FORWARD_DOMAIN_VRF_Encoding_get;
    
    dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_ENCAP_TTL_IDX_MPLS_FHEI_get_f ETPP_ETPS_FORMATS___prefix_ENCAP_TTL_IDX_MPLS_FHEI_get;
    
    dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_Empty2_get_f ETPP_ETPS_FORMATS___prefix_Empty2_get;
    
    dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_Always_Last_Layer_get_f ETPP_ETPS_FORMATS___prefix_Always_Last_Layer_get;
    
    dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_LIF_AHP_MPLS_Prefix_get_f ETPP_ETPS_FORMATS___prefix_LIF_AHP_MPLS_Prefix_get;
    
    dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_ENCAP_QOS_MODEL_MPLS_FHEI_get_f ETPP_ETPS_FORMATS___prefix_ENCAP_QOS_MODEL_MPLS_FHEI_get;
    
    dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_ENCAP_QOS_IDX_MPLS_FHEI_get_f ETPP_ETPS_FORMATS___prefix_ENCAP_QOS_IDX_MPLS_FHEI_get;
    
    dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_OutLIF_Profile_Orientation_Only_get_f ETPP_ETPS_FORMATS___prefix_OutLIF_Profile_Orientation_Only_get;
    
    dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_FORWARD_DOMAIN_VRF_IPvX_Tunnel_get_f ETPP_ETPS_FORMATS___prefix_FORWARD_DOMAIN_VRF_IPvX_Tunnel_get;
    
    dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_QOS_MODEL_1_get_f ETPP_ETPS_FORMATS___prefix_QOS_MODEL_1_get;
    
    dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_ESEM_COMMAND_IPv6_TUNNELS_FODO_Namespace_get_f ETPP_ETPS_FORMATS___prefix_ESEM_COMMAND_IPv6_TUNNELS_FODO_Namespace_get;
    
    dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_ENCAP_TTL_MODEL_MPLS_FHEI_get_f ETPP_ETPS_FORMATS___prefix_ENCAP_TTL_MODEL_MPLS_FHEI_get;
    
    dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_ESEM_COMMAND_IPv4_VxLAN_OR_VXLAN_GPE_FODO_Namespace_get_f ETPP_ETPS_FORMATS___prefix_ESEM_COMMAND_IPv4_VxLAN_OR_VXLAN_GPE_FODO_Namespace_get;
    
    dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_FORWARD_QOS_MODEL_NWK_QOS_get_f ETPP_ETPS_FORMATS___prefix_FORWARD_QOS_MODEL_NWK_QOS_get;
    
    dnx_data_dnx2_arr_prefix_ETPP_ETPS_FORMATS___prefix_ESEM_COMMAND_ARP_PLUS_AC_get_f ETPP_ETPS_FORMATS___prefix_ESEM_COMMAND_ARP_PLUS_AC_get;
    
    dnx_data_dnx2_arr_prefix_IRPP_FEC_ENTRY_FORMAT___prefix_Empty_get_f IRPP_FEC_ENTRY_FORMAT___prefix_Empty_get;
    
    dnx_data_dnx2_arr_prefix_IRPP_FEC_ENTRY_FORMAT___prefix_EEI_W_OUTLIF_get_f IRPP_FEC_ENTRY_FORMAT___prefix_EEI_W_OUTLIF_get;
    
    dnx_data_dnx2_arr_prefix_IRPP_FEC_ENTRY_FORMAT___prefix_OUTLIF_ETH_RIF_get_f IRPP_FEC_ENTRY_FORMAT___prefix_OUTLIF_ETH_RIF_get;
    
    dnx_data_dnx2_arr_prefix_IRPP_FEC_ENTRY_FORMAT___prefix_EEI_W_POP_Command_get_f IRPP_FEC_ENTRY_FORMAT___prefix_EEI_W_POP_Command_get;
    
    dnx_data_dnx2_arr_prefix_IRPP_FEC_ENTRY_FORMAT___prefix_EEI_W_SWAP_Command_get_f IRPP_FEC_ENTRY_FORMAT___prefix_EEI_W_SWAP_Command_get;
    
    dnx_data_dnx2_arr_prefix_IRPP_FEC_SUPER_ENTRY_FORMAT___prefix_Empty_get_f IRPP_FEC_SUPER_ENTRY_FORMAT___prefix_Empty_get;
    
    dnx_data_dnx2_arr_prefix_IRPP_FWD_STAGE_FORMATS___prefix_Empty_get_f IRPP_FWD_STAGE_FORMATS___prefix_Empty_get;
    
    dnx_data_dnx2_arr_prefix_IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b11_get_f IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b11_get;
    
    dnx_data_dnx2_arr_prefix_IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b0001_get_f IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b0001_get;
    
    dnx_data_dnx2_arr_prefix_IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b0010_get_f IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b0010_get;
    
    dnx_data_dnx2_arr_prefix_IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b0011_get_f IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b0011_get;
    
    dnx_data_dnx2_arr_prefix_IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b0100_get_f IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b0100_get;
    
    dnx_data_dnx2_arr_prefix_IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b0101_get_f IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b0101_get;
    
    dnx_data_dnx2_arr_prefix_IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b01111_get_f IRPP_FWD_STAGE_FORMATS___prefix_msb_with_b01111_get;
    
    dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_Empty_get_f IRPP_IN_LIF_FORMATS___prefix_Empty_get;
    
    dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_Service_Type_Extended_P2P_W_Learning_Prefix_get_f IRPP_IN_LIF_FORMATS___prefix_Service_Type_Extended_P2P_W_Learning_Prefix_get;
    
    dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_Parser_Context_IPvX_DEMUX_or_ETH_B1__4b_Prefix_get_f IRPP_IN_LIF_FORMATS___prefix_Parser_Context_IPvX_DEMUX_or_ETH_B1__4b_Prefix_get;
    
    dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_Empty2_get_f IRPP_IN_LIF_FORMATS___prefix_Empty2_get;
    
    dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_Free_get_f IRPP_IN_LIF_FORMATS___prefix_Free_get;
    
    dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_VRF_VSI_Not_Found_get_f IRPP_IN_LIF_FORMATS___prefix_VRF_VSI_Not_Found_get;
    
    dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_Forward_Domain_Assignment_Mode_LIF_or_VLAN_get_f IRPP_IN_LIF_FORMATS___prefix_Forward_Domain_Assignment_Mode_LIF_or_VLAN_get;
    
    dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_Forward_Domain_VRF_get_f IRPP_IN_LIF_FORMATS___prefix_Forward_Domain_VRF_get;
    
    dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_NEXT_LAYER_NETWORK_DOMAIN_TNIorVNI_get_f IRPP_IN_LIF_FORMATS___prefix_NEXT_LAYER_NETWORK_DOMAIN_TNIorVNI_get;
    
    dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_NEXT_LAYER_NETWORK_DOMAIN_PWE_WO_SD_TAGS_get_f IRPP_IN_LIF_FORMATS___prefix_NEXT_LAYER_NETWORK_DOMAIN_PWE_WO_SD_TAGS_get;
    
    dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF_OPTIMIZED_get_f IRPP_IN_LIF_FORMATS___prefix_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF_OPTIMIZED_get;
    
    dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_VSI_P2P_MSBS_get_f IRPP_IN_LIF_FORMATS___prefix_VSI_P2P_MSBS_get;
    
    dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_QoS_Profile_short_Prefix_get_f IRPP_IN_LIF_FORMATS___prefix_QoS_Profile_short_Prefix_get;
    
    dnx_data_dnx2_arr_prefix_IRPP_IN_LIF_FORMATS___prefix_Learn_info_destination_port_encoding_get_f IRPP_IN_LIF_FORMATS___prefix_Learn_info_destination_port_encoding_get;
    
    dnx_data_dnx2_arr_prefix_IRPP_VSI_FORMATS___prefix_Empty_get_f IRPP_VSI_FORMATS___prefix_Empty_get;
    
    dnx_data_dnx2_arr_prefix_IRPP_VSI_FORMATS___prefix_VSI_Profile_Prefix_get_f IRPP_VSI_FORMATS___prefix_VSI_Profile_Prefix_get;
    
    dnx_data_dnx2_arr_prefix_IRPP_VSI_FORMATS___prefix_Dest_MC_ID_Prefix_get_f IRPP_VSI_FORMATS___prefix_Dest_MC_ID_Prefix_get;
    
    dnx_data_dnx2_arr_prefix_IRPP_VSI_FORMATS___prefix_Dest_FEC_Port_Trap_Prefix_get_f IRPP_VSI_FORMATS___prefix_Dest_FEC_Port_Trap_Prefix_get;
    
    dnx_data_dnx2_arr_prefix_IRPP_VSI_FORMATS___prefix_Topology_ID_Prefix_get_f IRPP_VSI_FORMATS___prefix_Topology_ID_Prefix_get;
    
    dnx_data_dnx2_arr_prefix_IRPP_VSI_FORMATS___prefix_QOS_Profile_Prefix_get_f IRPP_VSI_FORMATS___prefix_QOS_Profile_Prefix_get;
    
    dnx_data_dnx2_arr_prefix_IRPP_VSI_FORMATS___prefix_My_Mac_Prefix_Prefix_get_f IRPP_VSI_FORMATS___prefix_My_Mac_Prefix_Prefix_get;
    
    dnx_data_dnx2_arr_prefix_IRPP_VSI_FORMATS___prefix_VSI_Stat_Obj_Cmd_Prefix_get_f IRPP_VSI_FORMATS___prefix_VSI_Stat_Obj_Cmd_Prefix_get;
} dnx_data_if_dnx2_arr_prefix_t;







typedef enum
{

    
    _dnx_data_dnx2_arr_max_nof_entries_feature_nof
} dnx_data_dnx2_arr_max_nof_entries_feature_e;



typedef int(
    *dnx_data_dnx2_arr_max_nof_entries_feature_get_f) (
    int unit,
    dnx_data_dnx2_arr_max_nof_entries_feature_e feature);


typedef uint32(
    *dnx_data_dnx2_arr_max_nof_entries_AGE_PROFILE_ARR_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_max_nof_entries_ETPP_ENCAP_1_ARR_ToS_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_max_nof_entries_ETPP_ENCAP_2_ARR_ToS_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_max_nof_entries_ETPP_ENCAP_3_ARR_ToS_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_max_nof_entries_ETPP_ENCAP_4_ARR_ToS_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_max_nof_entries_ETPP_ENCAP_5_ARR_ToS_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_max_nof_entries_ETPP_ENCAP_ARR_ToS_plus_2_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_max_nof_entries_ETPP_FWD_ARR_ToS_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_max_nof_entries_ETPP_FWD_ARR_ToS_plus_1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_max_nof_entries_ETPP_Preprocessing_ARR_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_max_nof_entries_ETPP_Termination_1_ARR_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_max_nof_entries_ETPP_Termination_2_ARR_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_max_nof_entries_IPPB_ACCEPTED_ARR_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_max_nof_entries_IPPB_EXPECTED_ACCEPTED_STRENGTH_ARR_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_max_nof_entries_IPPB_EXPECTED_ARR_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_max_nof_entries_IRPP_FEC_ARR_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_max_nof_entries_IRPP_FWD1_FWD_ARR_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_max_nof_entries_IRPP_FWD1_RPF_ARR_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_max_nof_entries_IRPP_SUPER_FEC_ARR_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_max_nof_entries_IRPP_VSI_ARR_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_max_nof_entries_IRPP_VTT_ARR_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_max_nof_entries_ITPP_MACT_LEARN_JR1_TRANSLATE_ARR_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_max_nof_entries_PPMC_ERPP_ARR_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_arr_max_nof_entries_USER_ALLOCATION_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_dnx2_arr_max_nof_entries_feature_get_f feature_get;
    
    dnx_data_dnx2_arr_max_nof_entries_AGE_PROFILE_ARR_get_f AGE_PROFILE_ARR_get;
    
    dnx_data_dnx2_arr_max_nof_entries_ETPP_ENCAP_1_ARR_ToS_get_f ETPP_ENCAP_1_ARR_ToS_get;
    
    dnx_data_dnx2_arr_max_nof_entries_ETPP_ENCAP_2_ARR_ToS_get_f ETPP_ENCAP_2_ARR_ToS_get;
    
    dnx_data_dnx2_arr_max_nof_entries_ETPP_ENCAP_3_ARR_ToS_get_f ETPP_ENCAP_3_ARR_ToS_get;
    
    dnx_data_dnx2_arr_max_nof_entries_ETPP_ENCAP_4_ARR_ToS_get_f ETPP_ENCAP_4_ARR_ToS_get;
    
    dnx_data_dnx2_arr_max_nof_entries_ETPP_ENCAP_5_ARR_ToS_get_f ETPP_ENCAP_5_ARR_ToS_get;
    
    dnx_data_dnx2_arr_max_nof_entries_ETPP_ENCAP_ARR_ToS_plus_2_get_f ETPP_ENCAP_ARR_ToS_plus_2_get;
    
    dnx_data_dnx2_arr_max_nof_entries_ETPP_FWD_ARR_ToS_get_f ETPP_FWD_ARR_ToS_get;
    
    dnx_data_dnx2_arr_max_nof_entries_ETPP_FWD_ARR_ToS_plus_1_get_f ETPP_FWD_ARR_ToS_plus_1_get;
    
    dnx_data_dnx2_arr_max_nof_entries_ETPP_Preprocessing_ARR_get_f ETPP_Preprocessing_ARR_get;
    
    dnx_data_dnx2_arr_max_nof_entries_ETPP_Termination_1_ARR_get_f ETPP_Termination_1_ARR_get;
    
    dnx_data_dnx2_arr_max_nof_entries_ETPP_Termination_2_ARR_get_f ETPP_Termination_2_ARR_get;
    
    dnx_data_dnx2_arr_max_nof_entries_IPPB_ACCEPTED_ARR_get_f IPPB_ACCEPTED_ARR_get;
    
    dnx_data_dnx2_arr_max_nof_entries_IPPB_EXPECTED_ACCEPTED_STRENGTH_ARR_get_f IPPB_EXPECTED_ACCEPTED_STRENGTH_ARR_get;
    
    dnx_data_dnx2_arr_max_nof_entries_IPPB_EXPECTED_ARR_get_f IPPB_EXPECTED_ARR_get;
    
    dnx_data_dnx2_arr_max_nof_entries_IRPP_FEC_ARR_get_f IRPP_FEC_ARR_get;
    
    dnx_data_dnx2_arr_max_nof_entries_IRPP_FWD1_FWD_ARR_get_f IRPP_FWD1_FWD_ARR_get;
    
    dnx_data_dnx2_arr_max_nof_entries_IRPP_FWD1_RPF_ARR_get_f IRPP_FWD1_RPF_ARR_get;
    
    dnx_data_dnx2_arr_max_nof_entries_IRPP_SUPER_FEC_ARR_get_f IRPP_SUPER_FEC_ARR_get;
    
    dnx_data_dnx2_arr_max_nof_entries_IRPP_VSI_ARR_get_f IRPP_VSI_ARR_get;
    
    dnx_data_dnx2_arr_max_nof_entries_IRPP_VTT_ARR_get_f IRPP_VTT_ARR_get;
    
    dnx_data_dnx2_arr_max_nof_entries_ITPP_MACT_LEARN_JR1_TRANSLATE_ARR_get_f ITPP_MACT_LEARN_JR1_TRANSLATE_ARR_get;
    
    dnx_data_dnx2_arr_max_nof_entries_PPMC_ERPP_ARR_get_f PPMC_ERPP_ARR_get;
    
    dnx_data_dnx2_arr_max_nof_entries_USER_ALLOCATION_get_f USER_ALLOCATION_get;
} dnx_data_if_dnx2_arr_max_nof_entries_t;






typedef struct
{
    
    char *arr_formats_file;
} dnx_data_dnx2_arr_formats_files_t;


typedef struct
{
    
    uint32 valid;
    
    char *doc;
    
    uint32 AGE_PROFILE;
    
    uint32 AGE_PROFILE_valid;
} dnx_data_dnx2_arr_formats_MACT_AGE_PROFILE_ARR_CFG_t;


typedef struct
{
    
    uint32 valid;
    
    char *doc;
    
    uint32 DUAL_HOMING_CONFIG;
    
    uint32 DUAL_HOMING_CONFIG_valid;
    
    uint32 EGRESS_LAST_LAYER_CONFIG;
    
    uint32 EGRESS_LAST_LAYER_CONFIG_valid;
    
    uint32 VLAN_MEMBERSHIP_INTERFACE_CONFIG;
    
    uint32 VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid;
    
    uint32 ENCAP_DESTINATION_CONFIG;
    
    uint32 ENCAP_DESTINATION_CONFIG_valid;
    
    uint32 QOS_MODEL_CONFIG;
    
    uint32 QOS_MODEL_CONFIG_valid;
    
    uint32 FORWARD_DOMAIN_CONFIG;
    
    uint32 FORWARD_DOMAIN_CONFIG_valid;
    
    uint32 FORWARD_DOMAIN_NETWORK_NAME_CONFIG;
    
    uint32 FORWARD_DOMAIN_NETWORK_NAME_CONFIG_valid;
    
    uint32 LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG;
    
    uint32 LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG_valid;
    
    uint32 MTU_PROFILE_CONFIG;
    
    uint32 MTU_PROFILE_CONFIG_valid;
    
    uint32 NWK_QOS_IDX_CONFIG;
    
    uint32 NWK_QOS_IDX_CONFIG_valid;
    
    uint32 REMARK_PROFILE_CONFIG;
    
    uint32 REMARK_PROFILE_CONFIG_valid;
    
    uint32 SOURCE_IDX_CONFIG;
    
    uint32 SOURCE_IDX_CONFIG_valid;
    
    uint32 LIF_STATISTICS_OBJECT_COMMAND_CONFIG;
    
    uint32 LIF_STATISTICS_OBJECT_COMMAND_CONFIG_valid;
    
    uint32 LIF_STATISTICS_OBJECT_ID_CONFIG;
    
    uint32 LIF_STATISTICS_OBJECT_ID_CONFIG_valid;
    
    uint32 TTL_MODEL_CONFIG;
    
    uint32 TTL_MODEL_CONFIG_valid;
    
    uint32 TTL_IDX_CONFIG;
    
    uint32 TTL_IDX_CONFIG_valid;
    
    uint32 VLAN_EDIT_PROFILE_CONFIG;
    
    uint32 VLAN_EDIT_PROFILE_CONFIG_valid;
} dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_t;


typedef struct
{
    
    uint32 valid;
    
    char *doc;
    
    uint32 DUAL_HOMING_CONFIG;
    
    uint32 DUAL_HOMING_CONFIG_valid;
    
    uint32 EGRESS_LAST_LAYER_CONFIG;
    
    uint32 EGRESS_LAST_LAYER_CONFIG_valid;
    
    uint32 VLAN_MEMBERSHIP_INTERFACE_CONFIG;
    
    uint32 VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid;
    
    uint32 ENCAP_DESTINATION_CONFIG;
    
    uint32 ENCAP_DESTINATION_CONFIG_valid;
    
    uint32 QOS_MODEL_CONFIG;
    
    uint32 QOS_MODEL_CONFIG_valid;
    
    uint32 FORWARD_DOMAIN_CONFIG;
    
    uint32 FORWARD_DOMAIN_CONFIG_valid;
    
    uint32 FORWARD_DOMAIN_NETWORK_NAME_CONFIG;
    
    uint32 FORWARD_DOMAIN_NETWORK_NAME_CONFIG_valid;
    
    uint32 LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG;
    
    uint32 LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG_valid;
    
    uint32 MTU_PROFILE_CONFIG;
    
    uint32 MTU_PROFILE_CONFIG_valid;
    
    uint32 NWK_QOS_IDX_CONFIG;
    
    uint32 NWK_QOS_IDX_CONFIG_valid;
    
    uint32 REMARK_PROFILE_CONFIG;
    
    uint32 REMARK_PROFILE_CONFIG_valid;
    
    uint32 SOURCE_IDX_CONFIG;
    
    uint32 SOURCE_IDX_CONFIG_valid;
    
    uint32 LIF_STATISTICS_OBJECT_COMMAND_CONFIG;
    
    uint32 LIF_STATISTICS_OBJECT_COMMAND_CONFIG_valid;
    
    uint32 LIF_STATISTICS_OBJECT_ID_CONFIG;
    
    uint32 LIF_STATISTICS_OBJECT_ID_CONFIG_valid;
    
    uint32 TTL_MODEL_CONFIG;
    
    uint32 TTL_MODEL_CONFIG_valid;
    
    uint32 TTL_IDX_CONFIG;
    
    uint32 TTL_IDX_CONFIG_valid;
    
    uint32 VLAN_EDIT_PROFILE_CONFIG;
    
    uint32 VLAN_EDIT_PROFILE_CONFIG_valid;
} dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_t;


typedef struct
{
    
    uint32 valid;
    
    char *doc;
    
    uint32 DUAL_HOMING_CONFIG;
    
    uint32 DUAL_HOMING_CONFIG_valid;
    
    uint32 EGRESS_LAST_LAYER_CONFIG;
    
    uint32 EGRESS_LAST_LAYER_CONFIG_valid;
    
    uint32 VLAN_MEMBERSHIP_INTERFACE_CONFIG;
    
    uint32 VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid;
    
    uint32 ENCAP_DESTINATION_CONFIG;
    
    uint32 ENCAP_DESTINATION_CONFIG_valid;
    
    uint32 QOS_MODEL_CONFIG;
    
    uint32 QOS_MODEL_CONFIG_valid;
    
    uint32 FORWARD_DOMAIN_CONFIG;
    
    uint32 FORWARD_DOMAIN_CONFIG_valid;
    
    uint32 FORWARD_DOMAIN_NETWORK_NAME_CONFIG;
    
    uint32 FORWARD_DOMAIN_NETWORK_NAME_CONFIG_valid;
    
    uint32 LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG;
    
    uint32 LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG_valid;
    
    uint32 MTU_PROFILE_CONFIG;
    
    uint32 MTU_PROFILE_CONFIG_valid;
    
    uint32 NWK_QOS_IDX_CONFIG;
    
    uint32 NWK_QOS_IDX_CONFIG_valid;
    
    uint32 REMARK_PROFILE_CONFIG;
    
    uint32 REMARK_PROFILE_CONFIG_valid;
    
    uint32 SOURCE_IDX_CONFIG;
    
    uint32 SOURCE_IDX_CONFIG_valid;
    
    uint32 LIF_STATISTICS_OBJECT_COMMAND_CONFIG;
    
    uint32 LIF_STATISTICS_OBJECT_COMMAND_CONFIG_valid;
    
    uint32 LIF_STATISTICS_OBJECT_ID_CONFIG;
    
    uint32 LIF_STATISTICS_OBJECT_ID_CONFIG_valid;
    
    uint32 TTL_MODEL_CONFIG;
    
    uint32 TTL_MODEL_CONFIG_valid;
    
    uint32 TTL_IDX_CONFIG;
    
    uint32 TTL_IDX_CONFIG_valid;
    
    uint32 VLAN_EDIT_PROFILE_CONFIG;
    
    uint32 VLAN_EDIT_PROFILE_CONFIG_valid;
} dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_t;


typedef struct
{
    
    uint32 valid;
    
    char *doc;
    
    uint32 DUAL_HOMING_CONFIG;
    
    uint32 DUAL_HOMING_CONFIG_valid;
    
    uint32 EGRESS_LAST_LAYER_CONFIG;
    
    uint32 EGRESS_LAST_LAYER_CONFIG_valid;
    
    uint32 VLAN_MEMBERSHIP_INTERFACE_CONFIG;
    
    uint32 VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid;
    
    uint32 ENCAP_DESTINATION_CONFIG;
    
    uint32 ENCAP_DESTINATION_CONFIG_valid;
    
    uint32 QOS_MODEL_CONFIG;
    
    uint32 QOS_MODEL_CONFIG_valid;
    
    uint32 FORWARD_DOMAIN_CONFIG;
    
    uint32 FORWARD_DOMAIN_CONFIG_valid;
    
    uint32 FORWARD_DOMAIN_NETWORK_NAME_CONFIG;
    
    uint32 FORWARD_DOMAIN_NETWORK_NAME_CONFIG_valid;
    
    uint32 LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG;
    
    uint32 LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG_valid;
    
    uint32 MTU_PROFILE_CONFIG;
    
    uint32 MTU_PROFILE_CONFIG_valid;
    
    uint32 NWK_QOS_IDX_CONFIG;
    
    uint32 NWK_QOS_IDX_CONFIG_valid;
    
    uint32 REMARK_PROFILE_CONFIG;
    
    uint32 REMARK_PROFILE_CONFIG_valid;
    
    uint32 SOURCE_IDX_CONFIG;
    
    uint32 SOURCE_IDX_CONFIG_valid;
    
    uint32 LIF_STATISTICS_OBJECT_COMMAND_CONFIG;
    
    uint32 LIF_STATISTICS_OBJECT_COMMAND_CONFIG_valid;
    
    uint32 LIF_STATISTICS_OBJECT_ID_CONFIG;
    
    uint32 LIF_STATISTICS_OBJECT_ID_CONFIG_valid;
    
    uint32 TTL_MODEL_CONFIG;
    
    uint32 TTL_MODEL_CONFIG_valid;
    
    uint32 TTL_IDX_CONFIG;
    
    uint32 TTL_IDX_CONFIG_valid;
    
    uint32 VLAN_EDIT_PROFILE_CONFIG;
    
    uint32 VLAN_EDIT_PROFILE_CONFIG_valid;
} dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_t;


typedef struct
{
    
    uint32 valid;
    
    char *doc;
    
    uint32 DUAL_HOMING_CONFIG;
    
    uint32 DUAL_HOMING_CONFIG_valid;
    
    uint32 EGRESS_LAST_LAYER_CONFIG;
    
    uint32 EGRESS_LAST_LAYER_CONFIG_valid;
    
    uint32 VLAN_MEMBERSHIP_INTERFACE_CONFIG;
    
    uint32 VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid;
    
    uint32 ENCAP_DESTINATION_CONFIG;
    
    uint32 ENCAP_DESTINATION_CONFIG_valid;
    
    uint32 QOS_MODEL_CONFIG;
    
    uint32 QOS_MODEL_CONFIG_valid;
    
    uint32 FORWARD_DOMAIN_CONFIG;
    
    uint32 FORWARD_DOMAIN_CONFIG_valid;
    
    uint32 FORWARD_DOMAIN_NETWORK_NAME_CONFIG;
    
    uint32 FORWARD_DOMAIN_NETWORK_NAME_CONFIG_valid;
    
    uint32 LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG;
    
    uint32 LIF_ADDITIONAL_HEADERS_PROFILE_CONFIG_valid;
    
    uint32 MTU_PROFILE_CONFIG;
    
    uint32 MTU_PROFILE_CONFIG_valid;
    
    uint32 NWK_QOS_IDX_CONFIG;
    
    uint32 NWK_QOS_IDX_CONFIG_valid;
    
    uint32 REMARK_PROFILE_CONFIG;
    
    uint32 REMARK_PROFILE_CONFIG_valid;
    
    uint32 SOURCE_IDX_CONFIG;
    
    uint32 SOURCE_IDX_CONFIG_valid;
    
    uint32 LIF_STATISTICS_OBJECT_COMMAND_CONFIG;
    
    uint32 LIF_STATISTICS_OBJECT_COMMAND_CONFIG_valid;
    
    uint32 LIF_STATISTICS_OBJECT_ID_CONFIG;
    
    uint32 LIF_STATISTICS_OBJECT_ID_CONFIG_valid;
    
    uint32 TTL_MODEL_CONFIG;
    
    uint32 TTL_MODEL_CONFIG_valid;
    
    uint32 TTL_IDX_CONFIG;
    
    uint32 TTL_IDX_CONFIG_valid;
    
    uint32 VLAN_EDIT_PROFILE_CONFIG;
    
    uint32 VLAN_EDIT_PROFILE_CONFIG_valid;
} dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_t;


typedef struct
{
    
    uint32 valid;
    
    char *doc;
    
    uint32 VLAN_MEMBERSHIP_INTERFACE_CONFIG;
    
    uint32 VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid;
    
    uint32 ENCAP_DESTINATION_CONFIG;
    
    uint32 ENCAP_DESTINATION_CONFIG_valid;
    
    uint32 REMARK_PROFILE_CONFIG;
    
    uint32 REMARK_PROFILE_CONFIG_valid;
} dnx_data_dnx2_arr_formats_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE_t;


typedef struct
{
    
    uint32 valid;
    
    char *doc;
    
    uint32 VLAN_MEMBERSHIP_INTERFACE_CONFIG;
    
    uint32 VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid;
    
    uint32 ENCAP_DESTINATION_CONFIG;
    
    uint32 ENCAP_DESTINATION_CONFIG_valid;
    
    uint32 REMARK_PROFILE_CONFIG;
    
    uint32 REMARK_PROFILE_CONFIG_valid;
} dnx_data_dnx2_arr_formats_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE_t;


typedef struct
{
    
    uint32 valid;
    
    char *doc;
    
    uint32 VLAN_MEMBERSHIP_INTERFACE_CONFIG;
    
    uint32 VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid;
    
    uint32 ENCAP_DESTINATION_CONFIG;
    
    uint32 ENCAP_DESTINATION_CONFIG_valid;
    
    uint32 REMARK_PROFILE_CONFIG;
    
    uint32 REMARK_PROFILE_CONFIG_valid;
} dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE_t;


typedef struct
{
    
    uint32 valid;
    
    char *doc;
    
    uint32 VLAN_MEMBERSHIP_INTERFACE_CONFIG;
    
    uint32 VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid;
    
    uint32 ENCAP_DESTINATION_CONFIG;
    
    uint32 ENCAP_DESTINATION_CONFIG_valid;
    
    uint32 REMARK_PROFILE_CONFIG;
    
    uint32 REMARK_PROFILE_CONFIG_valid;
} dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE_t;


typedef struct
{
    
    uint32 valid;
    
    char *doc;
    
    uint32 VLAN_MEMBERSHIP_INTERFACE_CONFIG;
    
    uint32 VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid;
    
    uint32 ENCAP_DESTINATION_CONFIG;
    
    uint32 ENCAP_DESTINATION_CONFIG_valid;
    
    uint32 REMARK_PROFILE_CONFIG;
    
    uint32 REMARK_PROFILE_CONFIG_valid;
} dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE_t;


typedef struct
{
    
    uint32 valid;
    
    char *doc;
    
    uint32 VLAN_MEMBERSHIP_INTERFACE_CONFIG;
    
    uint32 VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid;
    
    uint32 ENCAP_DESTINATION_CONFIG;
    
    uint32 ENCAP_DESTINATION_CONFIG_valid;
    
    uint32 REMARK_PROFILE_CONFIG;
    
    uint32 REMARK_PROFILE_CONFIG_valid;
} dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE_t;


typedef struct
{
    
    uint32 valid;
    
    char *doc;
    
    uint32 VLAN_MEMBERSHIP_INTERFACE_CONFIG;
    
    uint32 VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid;
    
    uint32 ENCAP_DESTINATION_CONFIG;
    
    uint32 ENCAP_DESTINATION_CONFIG_valid;
    
    uint32 REMARK_PROFILE_CONFIG;
    
    uint32 REMARK_PROFILE_CONFIG_valid;
} dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE_t;


typedef struct
{
    
    uint32 valid;
    
    char *doc;
    
    uint32 ETHERNET_EGRESS_LAST_LAYER_CONFIG;
    
    uint32 ETHERNET_EGRESS_LAST_LAYER_CONFIG_valid;
    
    uint32 ETHERNET_EGRESS_VLAN_MEMBERSHIP_INTERFACE_CONFIG;
    
    uint32 ETHERNET_EGRESS_VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid;
    
    uint32 ENCAP_DESTINATION_CONFIG;
    
    uint32 ENCAP_DESTINATION_CONFIG_valid;
    
    uint32 FORWARDING_DOMAIN_CONFIG;
    
    uint32 FORWARDING_DOMAIN_CONFIG_valid;
    
    uint32 QOS_MODEL_CONFIG;
    
    uint32 QOS_MODEL_CONFIG_valid;
    
    uint32 MTU_PROFILE_CONFIG;
    
    uint32 MTU_PROFILE_CONFIG_valid;
    
    uint32 NWK_QOS_INDEX_CONFIG;
    
    uint32 NWK_QOS_INDEX_CONFIG_valid;
    
    uint32 ETHERNET_OUTLIF_PROFILE_CONFIG;
    
    uint32 ETHERNET_OUTLIF_PROFILE_CONFIG_valid;
    
    uint32 REMARK_PROFILE_CONFIG;
    
    uint32 REMARK_PROFILE_CONFIG_valid;
    
    uint32 LIF_STATISTICS_OBJECT_COMMAND_CONFIG;
    
    uint32 LIF_STATISTICS_OBJECT_COMMAND_CONFIG_valid;
    
    uint32 LIF_STATISTICS_OBJECT_ID_CONFIG;
    
    uint32 LIF_STATISTICS_OBJECT_ID_CONFIG_valid;
    
    uint32 TTL_MODEL_CONFIG;
    
    uint32 TTL_MODEL_CONFIG_valid;
    
    uint32 ETHERNET_EGRESS_VLAN_EDIT_PROFILE_CONFIG;
    
    uint32 ETHERNET_EGRESS_VLAN_EDIT_PROFILE_CONFIG_valid;
} dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_t;


typedef struct
{
    
    uint32 valid;
    
    char *doc;
    
    uint32 ETHERNET_EGRESS_VLAN_MEMBERSHIP_INTERFACE_CONFIG;
    
    uint32 ETHERNET_EGRESS_VLAN_MEMBERSHIP_INTERFACE_CONFIG_valid;
    
    uint32 ENCAP_DESTINATION_CONFIG;
    
    uint32 ENCAP_DESTINATION_CONFIG_valid;
    
    uint32 REMARK_PROFILE_CONFIG;
    
    uint32 REMARK_PROFILE_CONFIG_valid;
} dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1_t;


typedef struct
{
    
    uint32 valid;
    
    char *doc;
    
    uint32 LLVP_PROFILE_CONFIG;
    
    uint32 LLVP_PROFILE_CONFIG_valid;
    
    uint32 EM_ACCESS_CMD_CONFIG;
    
    uint32 EM_ACCESS_CMD_CONFIG_valid;
    
    uint32 NAME_SPACE_CONFIG;
    
    uint32 NAME_SPACE_CONFIG_valid;
    
    uint32 FWD_DOMAIN_CONFIG;
    
    uint32 FWD_DOMAIN_CONFIG_valid;
    
    uint32 OAM_LIF_SET_CONFIG;
    
    uint32 OAM_LIF_SET_CONFIG_valid;
    
    uint32 OUTLIF_PROFILE_CONFIG;
    
    uint32 OUTLIF_PROFILE_CONFIG_valid;
} dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION_t;


typedef struct
{
    
    uint32 valid;
    
    char *doc;
    
    uint32 LLVP_PROFILE_CONFIG;
    
    uint32 LLVP_PROFILE_CONFIG_valid;
    
    uint32 EM_ACCESS_CMD_CONFIG;
    
    uint32 EM_ACCESS_CMD_CONFIG_valid;
    
    uint32 NAME_SPACE_CONFIG;
    
    uint32 NAME_SPACE_CONFIG_valid;
    
    uint32 FWD_DOMAIN_CONFIG;
    
    uint32 FWD_DOMAIN_CONFIG_valid;
    
    uint32 OAM_LIF_SET_CONFIG;
    
    uint32 OAM_LIF_SET_CONFIG_valid;
    
    uint32 OUTLIF_PROFILE_CONFIG;
    
    uint32 OUTLIF_PROFILE_CONFIG_valid;
} dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION___array_t;


typedef struct
{
    
    uint32 valid;
    
    char *doc;
    
    uint32 ACTION_PROFILE_CONFIG;
    
    uint32 ACTION_PROFILE_CONFIG_valid;
    
    uint32 PROTECTION_PATH_CONFIG;
    
    uint32 PROTECTION_PATH_CONFIG_valid;
    
    uint32 PROTECTION_PTR_CONFIG;
    
    uint32 PROTECTION_PTR_CONFIG_valid;
    
    uint32 STAT_OBJ_CMD_CONFIG;
    
    uint32 STAT_OBJ_CMD_CONFIG_valid;
    
    uint32 STAT_OBJ_ID_CONFIG;
    
    uint32 STAT_OBJ_ID_CONFIG_valid;
} dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION_t;


typedef struct
{
    
    uint32 valid;
    
    char *doc;
    
    uint32 ACTION_PROFILE_CONFIG;
    
    uint32 ACTION_PROFILE_CONFIG_valid;
    
    uint32 PROTECTION_PATH_CONFIG;
    
    uint32 PROTECTION_PATH_CONFIG_valid;
    
    uint32 PROTECTION_PTR_CONFIG;
    
    uint32 PROTECTION_PTR_CONFIG_valid;
    
    uint32 STAT_OBJ_CMD_CONFIG;
    
    uint32 STAT_OBJ_CMD_CONFIG_valid;
    
    uint32 STAT_OBJ_ID_CONFIG;
    
    uint32 STAT_OBJ_ID_CONFIG_valid;
} dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_t;


typedef struct
{
    
    uint32 valid;
    
    char *doc;
    
    uint32 QOS_MODEL_CONFIG;
    
    uint32 QOS_MODEL_CONFIG_valid;
    
    uint32 NWK_QOS_IDX_CONFIG;
    
    uint32 NWK_QOS_IDX_CONFIG_valid;
    
    uint32 OUTLIF_PROFILE_CONFIG;
    
    uint32 OUTLIF_PROFILE_CONFIG_valid;
    
    uint32 REMARK_PROFILE_CONFIG;
    
    uint32 REMARK_PROFILE_CONFIG_valid;
    
    uint32 TTL_MODEL_CONFIG;
    
    uint32 TTL_MODEL_CONFIG_valid;
} dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION_t;


typedef struct
{
    
    uint32 valid;
    
    char *doc;
    
    uint32 DATA_1;
    
    uint32 DATA_1_valid;
    
    uint32 DATA_2;
    
    uint32 DATA_2_valid;
    
    uint32 TYPE;
    
    uint32 TYPE_valid;
    
    uint32 DESTINATION;
    
    uint32 DESTINATION_valid;
    
    uint32 STRENGTH;
    
    uint32 STRENGTH_valid;
} dnx_data_dnx2_arr_formats_IPPB_ACCEPTED_ARR_FORMAT_CONFIG_t;


typedef struct
{
    
    uint32 valid;
    
    char *doc;
    
    uint32 STRENGTH;
    
    uint32 STRENGTH_valid;
} dnx_data_dnx2_arr_formats_MACT_MRQ_ARR_CFG_t;


typedef struct
{
    
    uint32 valid;
    
    char *doc;
    
    uint32 DESTINATION;
    
    uint32 DESTINATION_valid;
    
    uint32 DATA_1;
    
    uint32 DATA_1_valid;
    
    uint32 DATA_2;
    
    uint32 DATA_2_valid;
    
    uint32 TYPE;
    
    uint32 TYPE_valid;
    
    uint32 STRENGTH;
    
    uint32 STRENGTH_valid;
} dnx_data_dnx2_arr_formats_IPPB_EXPECTED_ARR_FORMAT_CONFIG_t;


typedef struct
{
    
    uint32 valid;
    
    char *doc;
    
    uint32 DESTINATION_CONFIG;
    
    uint32 DESTINATION_CONFIG_valid;
    
    uint32 EEI_CONFIG;
    
    uint32 EEI_CONFIG_valid;
    
    uint32 OUT_LIF_0_CONFIG;
    
    uint32 OUT_LIF_0_CONFIG_valid;
    
    uint32 OUT_LIF_1_CONFIG;
    
    uint32 OUT_LIF_1_CONFIG_valid;
    
    uint32 WEAK_TM_FLOW_CONFIG;
    
    uint32 WEAK_TM_FLOW_CONFIG_valid;
    
    uint32 MC_RPF_MODE_CONFIG;
    
    uint32 MC_RPF_MODE_CONFIG_valid;
} dnx_data_dnx2_arr_formats_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array_t;


typedef struct
{
    
    uint32 valid;
    
    char *doc;
    
    uint32 DESTINATION_CONFIG;
    
    uint32 DESTINATION_CONFIG_valid;
    
    uint32 EEI_CONFIG;
    
    uint32 EEI_CONFIG_valid;
    
    uint32 OUTLIF_0_CONFIG;
    
    uint32 OUTLIF_0_CONFIG_valid;
    
    uint32 OUTLIF_1_CONFIG;
    
    uint32 OUTLIF_1_CONFIG_valid;
    
    uint32 IRPP_FWD_RAW_DATA;
    
    uint32 IRPP_FWD_RAW_DATA_valid;
    
    uint32 DEFAULT_CONFIG;
    
    uint32 DEFAULT_CONFIG_valid;
    
    uint32 STAT_OBJ_CMD_CONFIG;
    
    uint32 STAT_OBJ_CMD_CONFIG_valid;
    
    uint32 STAT_OBJ_ID_CONFIG;
    
    uint32 STAT_OBJ_ID_CONFIG_valid;
} dnx_data_dnx2_arr_formats_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array_t;


typedef struct
{
    
    uint32 valid;
    
    char *doc;
    
    uint32 DESTINATION_CONFIG;
    
    uint32 DESTINATION_CONFIG_valid;
    
    uint32 OUTLIF_0_CONFIG;
    
    uint32 OUTLIF_0_CONFIG_valid;
    
    uint32 DEFAULT_CONFIG;
    
    uint32 DEFAULT_CONFIG_valid;
    
    uint32 SA_DROP_CONFIG;
    
    uint32 SA_DROP_CONFIG_valid;
    
    uint32 STAT_OBJ_CMD_CONFIG;
    
    uint32 STAT_OBJ_CMD_CONFIG_valid;
    
    uint32 STAT_OBJ_ID_CONFIG;
    
    uint32 STAT_OBJ_ID_CONFIG_valid;
} dnx_data_dnx2_arr_formats_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array_t;


typedef struct
{
    
    uint32 valid;
    
    char *doc;
    
    uint32 LEFT_CONFIG;
    
    uint32 LEFT_CONFIG_valid;
    
    uint32 LEFT_STAT_OBJECT_CMD_CONFIG;
    
    uint32 LEFT_STAT_OBJECT_CMD_CONFIG_valid;
    
    uint32 LEFT_STAT_OBJECT_ID_CONFIG;
    
    uint32 LEFT_STAT_OBJECT_ID_CONFIG_valid;
    
    uint32 PROTECTION_PTR_CONFIG;
    
    uint32 PROTECTION_PTR_CONFIG_valid;
    
    uint32 RIGHT_CONFIG;
    
    uint32 RIGHT_CONFIG_valid;
    
    uint32 RIGHT_STAT_OBJECT_CMD_CONFIG;
    
    uint32 RIGHT_STAT_OBJECT_CMD_CONFIG_valid;
    
    uint32 RIGHT_STAT_OBJECT_ID_CONFIG;
    
    uint32 RIGHT_STAT_OBJECT_ID_CONFIG_valid;
} dnx_data_dnx2_arr_formats_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array_t;


typedef struct
{
    
    uint32 valid;
    
    char *doc;
    
    uint32 DA_NOT_FOUND_DESTINATION_CONFIG;
    
    uint32 DA_NOT_FOUND_DESTINATION_CONFIG_valid;
    
    uint32 ECN_MAPPING_PROFILE_CONFIG;
    
    uint32 ECN_MAPPING_PROFILE_CONFIG_valid;
    
    uint32 MY_MAC_CONFIG;
    
    uint32 MY_MAC_CONFIG_valid;
    
    uint32 MY_MAC_PREFIX_CONFIG;
    
    uint32 MY_MAC_PREFIX_CONFIG_valid;
    
    uint32 PROPAGATION_PROFILE_CONFIG;
    
    uint32 PROPAGATION_PROFILE_CONFIG_valid;
    
    uint32 QOS_PROFILE_CONFIG;
    
    uint32 QOS_PROFILE_CONFIG_valid;
    
    uint32 STAT_OBJ_CMD_CONFIG;
    
    uint32 STAT_OBJ_CMD_CONFIG_valid;
    
    uint32 STAT_OBJ_ID_CONFIG;
    
    uint32 STAT_OBJ_ID_CONFIG_valid;
    
    uint32 TOPOLOGY_ID_CONFIG;
    
    uint32 TOPOLOGY_ID_CONFIG_valid;
    
    uint32 VRF_CONFIG;
    
    uint32 VRF_CONFIG_valid;
    
    uint32 VRID_BITMAP_CONFIG;
    
    uint32 VRID_BITMAP_CONFIG_valid;
    
    uint32 VSI_PROFILE_CONFIG;
    
    uint32 VSI_PROFILE_CONFIG_valid;
} dnx_data_dnx2_arr_formats_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1_t;


typedef struct
{
    
    uint32 valid;
    
    char *doc;
    
    uint32 DA_NOT_FOUND_DESTINATION_CONFIG;
    
    uint32 DA_NOT_FOUND_DESTINATION_CONFIG_valid;
    
    uint32 ECN_MAPPING_PROFILE_CONFIG;
    
    uint32 ECN_MAPPING_PROFILE_CONFIG_valid;
    
    uint32 MY_MAC_CONFIG;
    
    uint32 MY_MAC_CONFIG_valid;
    
    uint32 MY_MAC_PREFIX_CONFIG;
    
    uint32 MY_MAC_PREFIX_CONFIG_valid;
    
    uint32 PROPAGATION_PROFILE_CONFIG;
    
    uint32 PROPAGATION_PROFILE_CONFIG_valid;
    
    uint32 QOS_PROFILE_CONFIG;
    
    uint32 QOS_PROFILE_CONFIG_valid;
    
    uint32 STAT_OBJ_CMD_CONFIG;
    
    uint32 STAT_OBJ_CMD_CONFIG_valid;
    
    uint32 STAT_OBJ_ID_CONFIG;
    
    uint32 STAT_OBJ_ID_CONFIG_valid;
    
    uint32 TOPOLOGY_ID_CONFIG;
    
    uint32 TOPOLOGY_ID_CONFIG_valid;
    
    uint32 VRF_CONFIG;
    
    uint32 VRF_CONFIG_valid;
    
    uint32 VRID_BITMAP_CONFIG;
    
    uint32 VRID_BITMAP_CONFIG_valid;
    
    uint32 VSI_PROFILE_CONFIG;
    
    uint32 VSI_PROFILE_CONFIG_valid;
} dnx_data_dnx2_arr_formats_IPPF_VSI_FORMAT_CONFIGURATION_TABLE_t;


typedef struct
{
    
    uint32 valid;
    
    char *doc;
    
    uint32 ACTION_PROFILE_IDX_CONFIG;
    
    uint32 ACTION_PROFILE_IDX_CONFIG_valid;
    
    uint32 DESTINATION_CONFIG;
    
    uint32 DESTINATION_CONFIG_valid;
    
    uint32 ECN_MAPPING_PROFILE_CONFIG;
    
    uint32 ECN_MAPPING_PROFILE_CONFIG_valid;
    
    uint32 EEI_CONFIG;
    
    uint32 EEI_CONFIG_valid;
    
    uint32 FORWARD_DOMAIN_CONFIG;
    
    uint32 FORWARD_DOMAIN_CONFIG_valid;
    
    uint32 FORWARD_DOMAIN_ASSIGNMENT_MODE_CONFIG;
    
    uint32 FORWARD_DOMAIN_ASSIGNMENT_MODE_CONFIG_valid;
    
    uint32 SYS_INLIF_CONFIG;
    
    uint32 SYS_INLIF_CONFIG_valid;
    
    uint32 OUT_LIF_CONFIG;
    
    uint32 OUT_LIF_CONFIG_valid;
    
    uint32 LIF_PROFILE_CONFIG;
    
    uint32 LIF_PROFILE_CONFIG_valid;
    
    uint32 LEARN_EN_CONFIG;
    
    uint32 LEARN_EN_CONFIG_valid;
    
    uint32 LEARN_INFO_CONFIG;
    
    uint32 LEARN_INFO_CONFIG_valid;
    
    uint32 LEARN_PAYLOAD_CONTEXT_CONFIG;
    
    uint32 LEARN_PAYLOAD_CONTEXT_CONFIG_valid;
    
    uint32 LIF_GENERIC_DATA_0_CONFIG;
    
    uint32 LIF_GENERIC_DATA_0_CONFIG_valid;
    
    uint32 LIF_GENERIC_DATA_1_CONFIG;
    
    uint32 LIF_GENERIC_DATA_1_CONFIG_valid;
    
    uint32 PROTECTION_PTR_CONFIG;
    
    uint32 PROTECTION_PTR_CONFIG_valid;
    
    uint32 NEXT_LAYER_NETWORK_DOMAIN_CONFIG;
    
    uint32 NEXT_LAYER_NETWORK_DOMAIN_CONFIG_valid;
    
    uint32 NEXT_PARSER_CONTEXT_CONFIG;
    
    uint32 NEXT_PARSER_CONTEXT_CONFIG_valid;
    
    uint32 OAM_LIF_SET_CONFIG;
    
    uint32 OAM_LIF_SET_CONFIG_valid;
    
    uint32 PROPAGATION_PROFILE_CONFIG;
    
    uint32 PROPAGATION_PROFILE_CONFIG_valid;
    
    uint32 PROTECTION_PATH_CONFIG;
    
    uint32 PROTECTION_PATH_CONFIG_valid;
    
    uint32 QOS_PROFILE_CONFIG;
    
    uint32 QOS_PROFILE_CONFIG_valid;
    
    uint32 SERVICE_TYPE_CONFIG;
    
    uint32 SERVICE_TYPE_CONFIG_valid;
    
    uint32 STAT_OBJ_CMD_CONFIG;
    
    uint32 STAT_OBJ_CMD_CONFIG_valid;
    
    uint32 STAT_OBJ_ID_CONFIG;
    
    uint32 STAT_OBJ_ID_CONFIG_valid;
    
    uint32 TERMINATION_TYPE_CONFIG;
    
    uint32 TERMINATION_TYPE_CONFIG_valid;
    
    uint32 VLAN_EDIT_PCP_DEI_PROFILE_CONFIG;
    
    uint32 VLAN_EDIT_PCP_DEI_PROFILE_CONFIG_valid;
    
    uint32 VLAN_EDIT_PROFILE_CONFIG;
    
    uint32 VLAN_EDIT_PROFILE_CONFIG_valid;
    
    uint32 VLAN_EDIT_VID_1_CONFIG;
    
    uint32 VLAN_EDIT_VID_1_CONFIG_valid;
    
    uint32 VLAN_EDIT_VID_2_CONFIG;
    
    uint32 VLAN_EDIT_VID_2_CONFIG_valid;
    
    uint32 VLAN_MEMBER_IF_CONFIG;
    
    uint32 VLAN_MEMBER_IF_CONFIG_valid;
} dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t;


typedef struct
{
    
    uint32 valid;
    
    char *doc;
    
    uint32 ACTION_PROFILE_IDX_CONFIG;
    
    uint32 ACTION_PROFILE_IDX_CONFIG_valid;
    
    uint32 DESTINATION_CONFIG;
    
    uint32 DESTINATION_CONFIG_valid;
    
    uint32 ECN_MAPPING_PROFILE_CONFIG;
    
    uint32 ECN_MAPPING_PROFILE_CONFIG_valid;
    
    uint32 EEI_CONFIG;
    
    uint32 EEI_CONFIG_valid;
    
    uint32 FORWARD_DOMAIN_CONFIG;
    
    uint32 FORWARD_DOMAIN_CONFIG_valid;
    
    uint32 FORWARD_DOMAIN_ASSIGNMENT_MODE_CONFIG;
    
    uint32 FORWARD_DOMAIN_ASSIGNMENT_MODE_CONFIG_valid;
    
    uint32 SYS_INLIF_CONFIG;
    
    uint32 SYS_INLIF_CONFIG_valid;
    
    uint32 OUT_LIF_CONFIG;
    
    uint32 OUT_LIF_CONFIG_valid;
    
    uint32 LIF_PROFILE_CONFIG;
    
    uint32 LIF_PROFILE_CONFIG_valid;
    
    uint32 LEARN_EN_CONFIG;
    
    uint32 LEARN_EN_CONFIG_valid;
    
    uint32 LEARN_INFO_CONFIG;
    
    uint32 LEARN_INFO_CONFIG_valid;
    
    uint32 LEARN_PAYLOAD_CONTEXT_CONFIG;
    
    uint32 LEARN_PAYLOAD_CONTEXT_CONFIG_valid;
    
    uint32 LIF_GENERIC_DATA_0_CONFIG;
    
    uint32 LIF_GENERIC_DATA_0_CONFIG_valid;
    
    uint32 LIF_GENERIC_DATA_1_CONFIG;
    
    uint32 LIF_GENERIC_DATA_1_CONFIG_valid;
    
    uint32 PROTECTION_PTR_CONFIG;
    
    uint32 PROTECTION_PTR_CONFIG_valid;
    
    uint32 NEXT_LAYER_NETWORK_DOMAIN_CONFIG;
    
    uint32 NEXT_LAYER_NETWORK_DOMAIN_CONFIG_valid;
    
    uint32 NEXT_PARSER_CONTEXT_CONFIG;
    
    uint32 NEXT_PARSER_CONTEXT_CONFIG_valid;
    
    uint32 OAM_LIF_SET_CONFIG;
    
    uint32 OAM_LIF_SET_CONFIG_valid;
    
    uint32 PROPAGATION_PROFILE_CONFIG;
    
    uint32 PROPAGATION_PROFILE_CONFIG_valid;
    
    uint32 PROTECTION_PATH_CONFIG;
    
    uint32 PROTECTION_PATH_CONFIG_valid;
    
    uint32 QOS_PROFILE_CONFIG;
    
    uint32 QOS_PROFILE_CONFIG_valid;
    
    uint32 SERVICE_TYPE_CONFIG;
    
    uint32 SERVICE_TYPE_CONFIG_valid;
    
    uint32 STAT_OBJ_CMD_CONFIG;
    
    uint32 STAT_OBJ_CMD_CONFIG_valid;
    
    uint32 STAT_OBJ_ID_CONFIG;
    
    uint32 STAT_OBJ_ID_CONFIG_valid;
    
    uint32 TERMINATION_TYPE_CONFIG;
    
    uint32 TERMINATION_TYPE_CONFIG_valid;
    
    uint32 VLAN_EDIT_PCP_DEI_PROFILE_CONFIG;
    
    uint32 VLAN_EDIT_PCP_DEI_PROFILE_CONFIG_valid;
    
    uint32 VLAN_EDIT_PROFILE_CONFIG;
    
    uint32 VLAN_EDIT_PROFILE_CONFIG_valid;
    
    uint32 VLAN_EDIT_VID_1_CONFIG;
    
    uint32 VLAN_EDIT_VID_1_CONFIG_valid;
    
    uint32 VLAN_EDIT_VID_2_CONFIG;
    
    uint32 VLAN_EDIT_VID_2_CONFIG_valid;
    
    uint32 VLAN_MEMBER_IF_CONFIG;
    
    uint32 VLAN_MEMBER_IF_CONFIG_valid;
} dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t;


typedef struct
{
    
    uint32 valid;
    
    char *doc;
    
    uint32 DESTINATION_CONFIG;
    
    uint32 DESTINATION_CONFIG_valid;
    
    uint32 EEI_CONFIG;
    
    uint32 EEI_CONFIG_valid;
    
    uint32 OUTLIF_CONFIG;
    
    uint32 OUTLIF_CONFIG_valid;
} dnx_data_dnx2_arr_formats_IPPD_LEARN_ARR_CONFIGURATION_TABLE_t;


typedef struct
{
    
    uint32 valid;
    
    char *doc;
    
    uint32 DESTINATION_CONFIG;
    
    uint32 DESTINATION_CONFIG_valid;
    
    uint32 EEI_CONFIG;
    
    uint32 EEI_CONFIG_valid;
    
    uint32 OUTLIF_CONFIG;
    
    uint32 OUTLIF_CONFIG_valid;
} dnx_data_dnx2_arr_formats_OLP_LEARN_ARR_CONFIGURATION_TABLE_t;


typedef struct
{
    
    uint32 valid;
    
    char *doc;
    
    uint32 MCDB_OUTLIF_0_CONFIG;
    
    uint32 MCDB_OUTLIF_0_CONFIG_valid;
    
    uint32 MCDB_OUTLIF_1_CONFIG;
    
    uint32 MCDB_OUTLIF_1_CONFIG_valid;
    
    uint32 MCDB_OUTLIF_2_CONFIG;
    
    uint32 MCDB_OUTLIF_2_CONFIG_valid;
} dnx_data_dnx2_arr_formats_ERPP_EGRESS_MCDB_FORMAT_CFG_t;



typedef enum
{

    
    _dnx_data_dnx2_arr_formats_feature_nof
} dnx_data_dnx2_arr_formats_feature_e;



typedef int(
    *dnx_data_dnx2_arr_formats_feature_get_f) (
    int unit,
    dnx_data_dnx2_arr_formats_feature_e feature);


typedef const dnx_data_dnx2_arr_formats_files_t *(
    *dnx_data_dnx2_arr_formats_files_get_f) (
    int unit);


typedef const dnx_data_dnx2_arr_formats_MACT_AGE_PROFILE_ARR_CFG_t *(
    *dnx_data_dnx2_arr_formats_MACT_AGE_PROFILE_ARR_CFG_get_f) (
    int unit,
    int index);


typedef const dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_t *(
    *dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_get_f) (
    int unit,
    int index);


typedef const dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_t *(
    *dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_get_f) (
    int unit,
    int index);


typedef const dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_t *(
    *dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_get_f) (
    int unit,
    int index);


typedef const dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_t *(
    *dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_get_f) (
    int unit,
    int index);


typedef const dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_t *(
    *dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_get_f) (
    int unit,
    int index);


typedef const dnx_data_dnx2_arr_formats_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE_t *(
    *dnx_data_dnx2_arr_formats_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE_get_f) (
    int unit,
    int index);


typedef const dnx_data_dnx2_arr_formats_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE_t *(
    *dnx_data_dnx2_arr_formats_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE_get_f) (
    int unit,
    int index);


typedef const dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE_t *(
    *dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE_get_f) (
    int unit,
    int index);


typedef const dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE_t *(
    *dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE_get_f) (
    int unit,
    int index);


typedef const dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE_t *(
    *dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE_get_f) (
    int unit,
    int index);


typedef const dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE_t *(
    *dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE_get_f) (
    int unit,
    int index);


typedef const dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE_t *(
    *dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE_get_f) (
    int unit,
    int index);


typedef const dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_t *(
    *dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_get_f) (
    int unit,
    int index);


typedef const dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1_t *(
    *dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1_get_f) (
    int unit,
    int index);


typedef const dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION_t *(
    *dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION_get_f) (
    int unit,
    int index);


typedef const dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION___array_t *(
    *dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION___array_get_f) (
    int unit,
    int table_index,
    int index);


typedef const dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION_t *(
    *dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION_get_f) (
    int unit,
    int index);


typedef const dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_t *(
    *dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_get_f) (
    int unit,
    int table_index,
    int index);


typedef const dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION_t *(
    *dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION_get_f) (
    int unit,
    int index);


typedef const dnx_data_dnx2_arr_formats_IPPB_ACCEPTED_ARR_FORMAT_CONFIG_t *(
    *dnx_data_dnx2_arr_formats_IPPB_ACCEPTED_ARR_FORMAT_CONFIG_get_f) (
    int unit,
    int index);


typedef const dnx_data_dnx2_arr_formats_MACT_MRQ_ARR_CFG_t *(
    *dnx_data_dnx2_arr_formats_MACT_MRQ_ARR_CFG_get_f) (
    int unit,
    int index);


typedef const dnx_data_dnx2_arr_formats_IPPB_EXPECTED_ARR_FORMAT_CONFIG_t *(
    *dnx_data_dnx2_arr_formats_IPPB_EXPECTED_ARR_FORMAT_CONFIG_get_f) (
    int unit,
    int index);


typedef const dnx_data_dnx2_arr_formats_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array_t *(
    *dnx_data_dnx2_arr_formats_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array_get_f) (
    int unit,
    int table_index,
    int index);


typedef const dnx_data_dnx2_arr_formats_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array_t *(
    *dnx_data_dnx2_arr_formats_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array_get_f) (
    int unit,
    int table_index,
    int index);


typedef const dnx_data_dnx2_arr_formats_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array_t *(
    *dnx_data_dnx2_arr_formats_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array_get_f) (
    int unit,
    int table_index,
    int index);


typedef const dnx_data_dnx2_arr_formats_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array_t *(
    *dnx_data_dnx2_arr_formats_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array_get_f) (
    int unit,
    int table_index,
    int index);


typedef const dnx_data_dnx2_arr_formats_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1_t *(
    *dnx_data_dnx2_arr_formats_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1_get_f) (
    int unit,
    int index);


typedef const dnx_data_dnx2_arr_formats_IPPF_VSI_FORMAT_CONFIGURATION_TABLE_t *(
    *dnx_data_dnx2_arr_formats_IPPF_VSI_FORMAT_CONFIGURATION_TABLE_get_f) (
    int unit,
    int index);


typedef const dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_t *(
    *dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_get_f) (
    int unit,
    int table_index,
    int index);


typedef const dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_t *(
    *dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_get_f) (
    int unit,
    int table_index,
    int index);


typedef const dnx_data_dnx2_arr_formats_IPPD_LEARN_ARR_CONFIGURATION_TABLE_t *(
    *dnx_data_dnx2_arr_formats_IPPD_LEARN_ARR_CONFIGURATION_TABLE_get_f) (
    int unit,
    int index);


typedef const dnx_data_dnx2_arr_formats_OLP_LEARN_ARR_CONFIGURATION_TABLE_t *(
    *dnx_data_dnx2_arr_formats_OLP_LEARN_ARR_CONFIGURATION_TABLE_get_f) (
    int unit,
    int index);


typedef const dnx_data_dnx2_arr_formats_ERPP_EGRESS_MCDB_FORMAT_CFG_t *(
    *dnx_data_dnx2_arr_formats_ERPP_EGRESS_MCDB_FORMAT_CFG_get_f) (
    int unit,
    int index);



typedef struct
{
    
    dnx_data_dnx2_arr_formats_feature_get_f feature_get;
    
    dnx_data_dnx2_arr_formats_files_get_f files_get;
    
    dnxc_data_table_info_get_f files_info_get;
    
    dnx_data_dnx2_arr_formats_MACT_AGE_PROFILE_ARR_CFG_get_f MACT_AGE_PROFILE_ARR_CFG_get;
    
    dnxc_data_table_info_get_f MACT_AGE_PROFILE_ARR_CFG_info_get;
    
    dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_get_f ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_get;
    
    dnxc_data_table_info_get_f ETPPC_ENC_1_ARR_0_FORMAT_CONFIGURATION_TABLE_info_get;
    
    dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_get_f ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_get;
    
    dnxc_data_table_info_get_f ETPPB_ENC_2_ARR_0_FORMAT_CONFIGURATION_TABLE_info_get;
    
    dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_get_f ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_get;
    
    dnxc_data_table_info_get_f ETPPB_ENC_3_ARR_0_FORMAT_CONFIGURATION_TABLE_info_get;
    
    dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_get_f ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_get;
    
    dnxc_data_table_info_get_f ETPPB_ENC_4_ARR_0_FORMAT_CONFIGURATION_TABLE_info_get;
    
    dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_get_f ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_get;
    
    dnxc_data_table_info_get_f ETPPB_ENC_5_ARR_0_FORMAT_CONFIGURATION_TABLE_info_get;
    
    dnx_data_dnx2_arr_formats_ETPPB_ENC_FORMAT_CONFIGURATION_TABLE_get_f ETPPB_ENC_FORMAT_CONFIGURATION_TABLE_get;
    
    dnxc_data_table_info_get_f ETPPB_ENC_FORMAT_CONFIGURATION_TABLE_info_get;
    
    dnx_data_dnx2_arr_formats_ETPPC_ENC_FORMAT_CONFIGURATION_TABLE_get_f ETPPC_ENC_FORMAT_CONFIGURATION_TABLE_get;
    
    dnxc_data_table_info_get_f ETPPC_ENC_FORMAT_CONFIGURATION_TABLE_info_get;
    
    dnx_data_dnx2_arr_formats_ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE_get_f ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE_get;
    
    dnxc_data_table_info_get_f ETPPB_ENC_2_ARR_1_FORMAT_CONFIGURATION_TABLE_info_get;
    
    dnx_data_dnx2_arr_formats_ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE_get_f ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE_get;
    
    dnxc_data_table_info_get_f ETPPB_ENC_3_ARR_1_FORMAT_CONFIGURATION_TABLE_info_get;
    
    dnx_data_dnx2_arr_formats_ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE_get_f ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE_get;
    
    dnxc_data_table_info_get_f ETPPB_ENC_4_ARR_1_FORMAT_CONFIGURATION_TABLE_info_get;
    
    dnx_data_dnx2_arr_formats_ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE_get_f ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE_get;
    
    dnxc_data_table_info_get_f ETPPB_ENC_5_ARR_1_FORMAT_CONFIGURATION_TABLE_info_get;
    
    dnx_data_dnx2_arr_formats_ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE_get_f ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE_get;
    
    dnxc_data_table_info_get_f ETPPC_ENC_1_ARR_1_FORMAT_CONFIGURATION_TABLE_info_get;
    
    dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_get_f ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_get;
    
    dnxc_data_table_info_get_f ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_0_info_get;
    
    dnx_data_dnx2_arr_formats_ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1_get_f ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1_get;
    
    dnxc_data_table_info_get_f ETPPC_FORWARDING_FORMAT_CONFIGURATION_TABLE_1_info_get;
    
    dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION_get_f ETPPA_PRP_EES_ARR_CONFIGURATION_get;
    
    dnxc_data_table_info_get_f ETPPA_PRP_EES_ARR_CONFIGURATION_info_get;
    
    dnx_data_dnx2_arr_formats_ETPPA_PRP_EES_ARR_CONFIGURATION___array_get_f ETPPA_PRP_EES_ARR_CONFIGURATION___array_get;
    
    dnxc_data_table_info_get_f ETPPA_PRP_EES_ARR_CONFIGURATION___array_info_get;
    
    dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION_get_f ETPPC_TERM_ETPS_ARR_1_CONFIGURATION_get;
    
    dnxc_data_table_info_get_f ETPPC_TERM_ETPS_ARR_1_CONFIGURATION_info_get;
    
    dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_get_f ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_get;
    
    dnxc_data_table_info_get_f ETPPC_TERM_ETPS_ARR_1_CONFIGURATION___array_info_get;
    
    dnx_data_dnx2_arr_formats_ETPPC_TERM_ETPS_ARR_2_CONFIGURATION_get_f ETPPC_TERM_ETPS_ARR_2_CONFIGURATION_get;
    
    dnxc_data_table_info_get_f ETPPC_TERM_ETPS_ARR_2_CONFIGURATION_info_get;
    
    dnx_data_dnx2_arr_formats_IPPB_ACCEPTED_ARR_FORMAT_CONFIG_get_f IPPB_ACCEPTED_ARR_FORMAT_CONFIG_get;
    
    dnxc_data_table_info_get_f IPPB_ACCEPTED_ARR_FORMAT_CONFIG_info_get;
    
    dnx_data_dnx2_arr_formats_MACT_MRQ_ARR_CFG_get_f MACT_MRQ_ARR_CFG_get;
    
    dnxc_data_table_info_get_f MACT_MRQ_ARR_CFG_info_get;
    
    dnx_data_dnx2_arr_formats_IPPB_EXPECTED_ARR_FORMAT_CONFIG_get_f IPPB_EXPECTED_ARR_FORMAT_CONFIG_get;
    
    dnxc_data_table_info_get_f IPPB_EXPECTED_ARR_FORMAT_CONFIG_info_get;
    
    dnx_data_dnx2_arr_formats_IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array_get_f IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array_get;
    
    dnxc_data_table_info_get_f IPPD_FEC_FORMAT_CONFIGURATION_TABLE___array_info_get;
    
    dnx_data_dnx2_arr_formats_IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array_get_f IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array_get;
    
    dnxc_data_table_info_get_f IPPB_FLP_FWD_FORMAT_CONFIGURATION_TABLE___array_info_get;
    
    dnx_data_dnx2_arr_formats_IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array_get_f IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array_get;
    
    dnxc_data_table_info_get_f IPPB_FLP_RPF_FORMAT_CONFIGURATION_TABLE___array_info_get;
    
    dnx_data_dnx2_arr_formats_IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array_get_f IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array_get;
    
    dnxc_data_table_info_get_f IPPD_SUPER_ARR_FORMAT_CONFIGURATION_TABLE___array_info_get;
    
    dnx_data_dnx2_arr_formats_IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1_get_f IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1_get;
    
    dnxc_data_table_info_get_f IPPA_VSI_FORMAT_CONFIGURATION_TABLE_1_info_get;
    
    dnx_data_dnx2_arr_formats_IPPF_VSI_FORMAT_CONFIGURATION_TABLE_get_f IPPF_VSI_FORMAT_CONFIGURATION_TABLE_get;
    
    dnxc_data_table_info_get_f IPPF_VSI_FORMAT_CONFIGURATION_TABLE_info_get;
    
    dnx_data_dnx2_arr_formats_IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_get_f IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_get;
    
    dnxc_data_table_info_get_f IPPA_VTT_FORMAT_CONFIGURATION_TABLE___array_info_get;
    
    dnx_data_dnx2_arr_formats_IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_get_f IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_get;
    
    dnxc_data_table_info_get_f IPPF_VTT_FORMAT_CONFIGURATION_TABLE___array_info_get;
    
    dnx_data_dnx2_arr_formats_IPPD_LEARN_ARR_CONFIGURATION_TABLE_get_f IPPD_LEARN_ARR_CONFIGURATION_TABLE_get;
    
    dnxc_data_table_info_get_f IPPD_LEARN_ARR_CONFIGURATION_TABLE_info_get;
    
    dnx_data_dnx2_arr_formats_OLP_LEARN_ARR_CONFIGURATION_TABLE_get_f OLP_LEARN_ARR_CONFIGURATION_TABLE_get;
    
    dnxc_data_table_info_get_f OLP_LEARN_ARR_CONFIGURATION_TABLE_info_get;
    
    dnx_data_dnx2_arr_formats_ERPP_EGRESS_MCDB_FORMAT_CFG_get_f ERPP_EGRESS_MCDB_FORMAT_CFG_get;
    
    dnxc_data_table_info_get_f ERPP_EGRESS_MCDB_FORMAT_CFG_info_get;
} dnx_data_if_dnx2_arr_formats_t;





typedef struct
{
    
    dnx_data_if_dnx2_arr_prefix_t prefix;
    
    dnx_data_if_dnx2_arr_max_nof_entries_t max_nof_entries;
    
    dnx_data_if_dnx2_arr_formats_t formats;
} dnx_data_if_dnx2_arr_t;




extern dnx_data_if_dnx2_arr_t dnx_data_dnx2_arr;


#endif 

