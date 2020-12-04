

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_AOD_H_

#define _DNX_DATA_AOD_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_aod.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_aod_init(
    int unit);






typedef struct
{
    
    char *image;
} dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER___image_t;


typedef struct
{
    
    uint32 valid;
    
    char *doc;
    
    char *Egress_FWD_Code;
    
    uint32 Egress_FWD_Code_mask;
    
    uint32 Egress_FWD_Code_valid;
    
    uint32 DSS_Stacking;
    
    uint32 DSS_Stacking_valid;
    
    uint32 Exclude_Source;
    
    uint32 Exclude_Source_valid;
    
    uint32 GLEM_PP_or_NON_PP;
    
    uint32 GLEM_PP_or_NON_PP_valid;
    
    uint32 Hairpin_or_Same_Interface;
    
    uint32 Hairpin_or_Same_Interface_valid;
    
    uint32 IPv4_Filters;
    
    uint32 IPv4_Filters_valid;
    
    uint32 IPv4_Filters_Dip_Zero;
    
    uint32 IPv4_Filters_Dip_Zero_valid;
    
    uint32 IPv4_Filters_Header_Checksum;
    
    uint32 IPv4_Filters_Header_Checksum_valid;
    
    uint32 IPv4_Filters_Header_Length;
    
    uint32 IPv4_Filters_Header_Length_valid;
    
    uint32 IPv4_Filters_Options;
    
    uint32 IPv4_Filters_Options_valid;
    
    uint32 IPv4_Filters_Sip_Equal_Dip;
    
    uint32 IPv4_Filters_Sip_Equal_Dip_valid;
    
    uint32 IPv4_Filters_Sip_Multicast;
    
    uint32 IPv4_Filters_Sip_Multicast_valid;
    
    uint32 IPv4_Filters_Total_Length;
    
    uint32 IPv4_Filters_Total_Length_valid;
    
    uint32 IPv4_Filters_Version;
    
    uint32 IPv4_Filters_Version_valid;
    
    uint32 IPv6_Filters;
    
    uint32 IPv6_Filters_valid;
    
    uint32 IPv6_Filters_Dip_Is_All;
    
    uint32 IPv6_Filters_Dip_Is_All_valid;
    
    uint32 IPv6_Filters_Ipv4_Cmp_Dst;
    
    uint32 IPv6_Filters_Ipv4_Cmp_Dst_valid;
    
    uint32 IPv6_Filters_Ipv4_Mapped_Dst;
    
    uint32 IPv6_Filters_Ipv4_Mapped_Dst_valid;
    
    uint32 IPv6_Filters_Link_Local_Dst;
    
    uint32 IPv6_Filters_Link_Local_Dst_valid;
    
    uint32 IPv6_Filters_Link_Local_Src;
    
    uint32 IPv6_Filters_Link_Local_Src_valid;
    
    uint32 IPv6_Filters_Loopback;
    
    uint32 IPv6_Filters_Loopback_valid;
    
    uint32 IPv6_Filters_Mc_Dst;
    
    uint32 IPv6_Filters_Mc_Dst_valid;
    
    uint32 IPv6_Filters_Next_Header_Is_Zeros;
    
    uint32 IPv6_Filters_Next_Header_Is_Zeros_valid;
    
    uint32 IPv6_Filters_Sip_Is_All;
    
    uint32 IPv6_Filters_Sip_Is_All_valid;
    
    uint32 IPv6_Filters_Sip_Is_Mc;
    
    uint32 IPv6_Filters_Sip_Is_Mc_valid;
    
    uint32 IPv6_Filters_Site_Local_Dst;
    
    uint32 IPv6_Filters_Site_Local_Dst_valid;
    
    uint32 IPv6_Filters_Site_Local_Src;
    
    uint32 IPv6_Filters_Site_Local_Src_valid;
    
    uint32 IPv6_Filters_Version;
    
    uint32 IPv6_Filters_Version_valid;
    
    uint32 Invalid_OTM;
    
    uint32 Invalid_OTM_valid;
    
    uint32 L4_Filters;
    
    uint32 L4_Filters_valid;
    
    uint32 L4_Filters_Tcp_Fragment_Incomplete_Hdr;
    
    uint32 L4_Filters_Tcp_Fragment_Incomplete_Hdr_valid;
    
    uint32 L4_Filters_Tcp_Fragment_Offset_Less_Than_8;
    
    uint32 L4_Filters_Tcp_Fragment_Offset_Less_Than_8_valid;
    
    uint32 L4_Filters_Tcp_Seq_Num_Zero_Flags_Set;
    
    uint32 L4_Filters_Tcp_Seq_Num_Zero_Flags_Set_valid;
    
    uint32 L4_Filters_Tcp_Seq_Num_Zero_Flags_Zero;
    
    uint32 L4_Filters_Tcp_Seq_Num_Zero_Flags_Zero_valid;
    
    uint32 L4_Filters_Tcp_Src_Port_Equals_Dst_Port;
    
    uint32 L4_Filters_Tcp_Src_Port_Equals_Dst_Port_valid;
    
    uint32 L4_Filters_Tcp_Syn_And_Fin_Are_Set;
    
    uint32 L4_Filters_Tcp_Syn_And_Fin_Are_Set_valid;
    
    uint32 L4_Filters_Udp_Src_Port_Equals_Dst_Port;
    
    uint32 L4_Filters_Udp_Src_Port_Equals_Dst_Port_valid;
    
    uint32 LAG_Multicast;
    
    uint32 LAG_Multicast_valid;
    
    uint32 MTU;
    
    uint32 MTU_valid;
    
    uint32 RQP_Discard;
    
    uint32 RQP_Discard_valid;
    
    uint32 Split_Horizon;
    
    uint32 Split_Horizon_valid;
    
    uint32 TDM_Indication;
    
    uint32 TDM_Indication_valid;
    
    uint32 TTL_Scoping;
    
    uint32 TTL_Scoping_valid;
    
    uint32 TTL_eq_0;
    
    uint32 TTL_eq_0_valid;
    
    uint32 TTL_eq_1;
    
    uint32 TTL_eq_1_valid;
    
    uint32 Unacceptable_Frame_Types;
    
    uint32 Unacceptable_Frame_Types_valid;
    
    uint32 Unknown_DA;
    
    uint32 Unknown_DA_valid;
} dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t;


typedef struct
{
    
    char *image;
} dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE___image_t;


typedef struct
{
    
    uint32 valid;
    
    char *doc;
    
    char *Egress_FWD_Code;
    
    uint32 Egress_FWD_Code_mask;
    
    uint32 Egress_FWD_Code_valid;
    
    uint32 Acceptable_Frame_Type_Enable;
    
    uint32 Acceptable_Frame_Type_Enable_valid;
    
    uint32 FWD_Context_is_PP_Enable;
    
    uint32 FWD_Context_is_PP_Enable_valid;
    
    uint32 Forward_Code_IPv4_or_IPv6;
    
    uint32 Forward_Code_IPv4_or_IPv6_valid;
    
    uint32 Learn_Enable;
    
    uint32 Learn_Enable_valid;
    
    uint32 Protocol_Ethernet_Enable;
    
    uint32 Protocol_Ethernet_Enable_valid;
    
    uint32 Protocol_IPv4_MC_Enable;
    
    uint32 Protocol_IPv4_MC_Enable_valid;
    
    uint32 Protocol_IPv4_UC_Enable;
    
    uint32 Protocol_IPv4_UC_Enable_valid;
    
    uint32 Protocol_IPv6_MC_Enable;
    
    uint32 Protocol_IPv6_MC_Enable_valid;
    
    uint32 Protocol_IPv6_UC_Enable;
    
    uint32 Protocol_IPv6_UC_Enable_valid;
    
    uint32 Protocol_MPLS_Enable;
    
    uint32 Protocol_MPLS_Enable_valid;
    
    uint32 STP_Enable;
    
    uint32 STP_Enable_valid;
    
    uint32 Split_Horizon_Enable;
    
    uint32 Split_Horizon_Enable_valid;
    
    uint32 Termination_Enable;
    
    uint32 Termination_Enable_valid;
    
    uint32 Vlan_Membership_Enable;
    
    uint32 Vlan_Membership_Enable_valid;
} dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE_t;


typedef struct
{
    
    char *image;
} dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION___image_t;


typedef struct
{
    
    uint32 valid;
    
    char *doc;
    
    uint32 FAI;
    
    uint32 FAI_mask;
    
    uint32 FAI_valid;
    
    uint32 FWD_Layer_Qualifier;
    
    uint32 FWD_Layer_Qualifier_mask;
    
    uint32 FWD_Layer_Qualifier_valid;
    
    char *FWD_Layer_Type;
    
    uint32 FWD_Layer_Type_mask;
    
    uint32 FWD_Layer_Type_valid;
    
    char *FWD_code_port_profile;
    
    uint32 FWD_code_port_profile_mask;
    
    uint32 FWD_code_port_profile_valid;
    
    uint32 TM_Action_Type;
    
    uint32 TM_Action_Type_mask;
    
    uint32 TM_Action_Type_valid;
    
    uint32 ingress_trapped;
    
    uint32 ingress_trapped_mask;
    
    uint32 ingress_trapped_valid;
    
    uint32 is_Applet;
    
    uint32 is_Applet_mask;
    
    uint32 is_Applet_valid;
    
    uint32 is_TDM;
    
    uint32 is_TDM_mask;
    
    uint32 is_TDM_valid;
    
    char *Egress_FWD_Code;
    
    uint32 Egress_FWD_Code_valid;
} dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION_t;


typedef struct
{
    
    char *image;
} dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION___image_t;


typedef struct
{
    
    uint32 valid;
    
    char *doc;
    
    char *ACE_Value;
    
    uint32 ACE_Value_mask;
    
    uint32 ACE_Value_valid;
    
    uint32 FAI;
    
    uint32 FAI_mask;
    
    uint32 FAI_valid;
    
    uint32 FWD_Layer_Qualifier;
    
    uint32 FWD_Layer_Qualifier_mask;
    
    uint32 FWD_Layer_Qualifier_valid;
    
    char *FWD_Layer_Type;
    
    uint32 FWD_Layer_Type_mask;
    
    uint32 FWD_Layer_Type_valid;
    
    char *FWD_code_port_profile;
    
    uint32 FWD_code_port_profile_mask;
    
    uint32 FWD_code_port_profile_valid;
    
    uint32 TM_Action_Type;
    
    uint32 TM_Action_Type_mask;
    
    uint32 TM_Action_Type_valid;
    
    uint32 ingress_trapped;
    
    uint32 ingress_trapped_mask;
    
    uint32 ingress_trapped_valid;
    
    uint32 is_Applet;
    
    uint32 is_Applet_mask;
    
    uint32 is_Applet_valid;
    
    uint32 is_TDM;
    
    uint32 is_TDM_mask;
    
    uint32 is_TDM_valid;
    
    char *Egress_FWD_Code;
    
    uint32 Egress_FWD_Code_valid;
} dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION_t;


typedef struct
{
    
    char *image;
} dnx_data_aod_tables_EGRESS_PARSING_START_TYPE_CONTEXT___image_t;


typedef struct
{
    
    uint32 valid;
    
    char *doc;
    
    char *Parsing_Start_Type;
    
    uint32 Parsing_Start_Type_mask;
    
    uint32 Parsing_Start_Type_valid;
    
    char *ERPP_1st_Parser_Parser_Context;
    
    uint32 ERPP_1st_Parser_Parser_Context_valid;
} dnx_data_aod_tables_EGRESS_PARSING_START_TYPE_CONTEXT_t;


typedef struct
{
    
    char *image;
} dnx_data_aod_tables_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE___image_t;


typedef struct
{
    
    uint32 valid;
    
    char *doc;
    
    char *JR_FWD_CODE;
    
    uint32 JR_FWD_CODE_mask;
    
    uint32 JR_FWD_CODE_valid;
    
    char *ERPP_1st_Parser_Parser_Context;
    
    uint32 ERPP_1st_Parser_Parser_Context_valid;
} dnx_data_aod_tables_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE_t;


typedef struct
{
    
    char *image;
} dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES___image_t;


typedef struct
{
    
    uint32 valid;
    
    char *doc;
    
    char *VTT_MPLS_Special_Label_Profile;
    
    uint32 VTT_MPLS_Special_Label_Profile_mask;
    
    uint32 VTT_MPLS_Special_Label_Profile_valid;
    
    uint32 Check_Bos;
    
    uint32 Check_Bos_valid;
    
    uint32 ECN_Compare_Strength;
    
    uint32 ECN_Compare_Strength_valid;
    
    uint32 ECN_Mapping_Profile;
    
    uint32 ECN_Mapping_Profile_valid;
    
    uint32 ECN_Propagation_Strength;
    
    uint32 ECN_Propagation_Strength_valid;
    
    uint32 Expected_Bos;
    
    uint32 Expected_Bos_valid;
    
    uint32 Force_Has_CW;
    
    uint32 Force_Has_CW_valid;
    
    uint32 Force_Is_OAM;
    
    uint32 Force_Is_OAM_valid;
    
    uint32 Force_Parser_Context;
    
    uint32 Force_Parser_Context_valid;
    
    char *Force_Parser_Context_Value;
    
    uint32 Force_Parser_Context_Value_valid;
    
    uint32 Labels_To_Terminate;
    
    uint32 Labels_To_Terminate_valid;
    
    uint32 PHB_Compare_Strength;
    
    uint32 PHB_Compare_Strength_valid;
    
    uint32 PHB_Propagation_Strength;
    
    uint32 PHB_Propagation_Strength_valid;
    
    uint32 Qos_Mapping_Profile;
    
    uint32 Qos_Mapping_Profile_valid;
    
    uint32 Remark_Compare_Strength;
    
    uint32 Remark_Compare_Strength_valid;
    
    uint32 Remark_Propagation_Strength;
    
    uint32 Remark_Propagation_Strength_valid;
    
    uint32 Special_After_Tunnel;
    
    uint32 Special_After_Tunnel_valid;
    
    uint32 Special_Before_Tunnel;
    
    uint32 Special_Before_Tunnel_valid;
    
    uint32 TTL_Compare_Strength;
    
    uint32 TTL_Compare_Strength_valid;
    
    uint32 TTL_Propagation_Strength;
    
    uint32 TTL_Propagation_Strength_valid;
} dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_t;



typedef enum
{

    
    _dnx_data_aod_tables_feature_nof
} dnx_data_aod_tables_feature_e;



typedef int(
    *dnx_data_aod_tables_feature_get_f) (
    int unit,
    dnx_data_aod_tables_feature_e feature);


typedef const dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER___image_t *(
    *dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER___image_get_f) (
    int unit);


typedef const dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t *(
    *dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_get_f) (
    int unit,
    int index);


typedef const dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE___image_t *(
    *dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE___image_get_f) (
    int unit);


typedef const dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE_t *(
    *dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE_get_f) (
    int unit,
    int index,
    int soc_value);


typedef const dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION___image_t *(
    *dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION___image_get_f) (
    int unit);


typedef const dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION_t *(
    *dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION_get_f) (
    int unit,
    int index,
    int soc_value);


typedef const dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION___image_t *(
    *dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION___image_get_f) (
    int unit);


typedef const dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION_t *(
    *dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION_get_f) (
    int unit,
    int index,
    int soc_value);


typedef const dnx_data_aod_tables_EGRESS_PARSING_START_TYPE_CONTEXT___image_t *(
    *dnx_data_aod_tables_EGRESS_PARSING_START_TYPE_CONTEXT___image_get_f) (
    int unit);


typedef const dnx_data_aod_tables_EGRESS_PARSING_START_TYPE_CONTEXT_t *(
    *dnx_data_aod_tables_EGRESS_PARSING_START_TYPE_CONTEXT_get_f) (
    int unit,
    int index,
    int soc_value);


typedef const dnx_data_aod_tables_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE___image_t *(
    *dnx_data_aod_tables_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE___image_get_f) (
    int unit);


typedef const dnx_data_aod_tables_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE_t *(
    *dnx_data_aod_tables_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE_get_f) (
    int unit,
    int index,
    int soc_value);


typedef const dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES___image_t *(
    *dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES___image_get_f) (
    int unit);


typedef const dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_t *(
    *dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_get_f) (
    int unit,
    int index);



typedef struct
{
    
    dnx_data_aod_tables_feature_get_f feature_get;
    
    dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER___image_get_f ERPP_FWD_CONTEXT_FILTER___image_get;
    
    dnxc_data_table_info_get_f ERPP_FWD_CONTEXT_FILTER___image_info_get;
    
    dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_get_f ERPP_FWD_CONTEXT_FILTER_get;
    
    dnxc_data_table_info_get_f ERPP_FWD_CONTEXT_FILTER_info_get;
    
    dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE___image_get_f EGRESS_PER_FORWARD_CODE___image_get;
    
    dnxc_data_table_info_get_f EGRESS_PER_FORWARD_CODE___image_info_get;
    
    dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE_get_f EGRESS_PER_FORWARD_CODE_get;
    
    dnxc_data_table_info_get_f EGRESS_PER_FORWARD_CODE_info_get;
    
    dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION___image_get_f ERPP_FORWARD_CODE_SELECTION___image_get;
    
    dnxc_data_table_info_get_f ERPP_FORWARD_CODE_SELECTION___image_info_get;
    
    dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION_get_f ERPP_FORWARD_CODE_SELECTION_get;
    
    dnxc_data_table_info_get_f ERPP_FORWARD_CODE_SELECTION_info_get;
    
    dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION___image_get_f ETPP_FORWARD_CODE_SELECTION___image_get;
    
    dnxc_data_table_info_get_f ETPP_FORWARD_CODE_SELECTION___image_info_get;
    
    dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION_get_f ETPP_FORWARD_CODE_SELECTION_get;
    
    dnxc_data_table_info_get_f ETPP_FORWARD_CODE_SELECTION_info_get;
    
    dnx_data_aod_tables_EGRESS_PARSING_START_TYPE_CONTEXT___image_get_f EGRESS_PARSING_START_TYPE_CONTEXT___image_get;
    
    dnxc_data_table_info_get_f EGRESS_PARSING_START_TYPE_CONTEXT___image_info_get;
    
    dnx_data_aod_tables_EGRESS_PARSING_START_TYPE_CONTEXT_get_f EGRESS_PARSING_START_TYPE_CONTEXT_get;
    
    dnxc_data_table_info_get_f EGRESS_PARSING_START_TYPE_CONTEXT_info_get;
    
    dnx_data_aod_tables_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE___image_get_f EGRESS_PARSING_CONTEXT_MAP_JR1_MODE___image_get;
    
    dnxc_data_table_info_get_f EGRESS_PARSING_CONTEXT_MAP_JR1_MODE___image_info_get;
    
    dnx_data_aod_tables_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE_get_f EGRESS_PARSING_CONTEXT_MAP_JR1_MODE_get;
    
    dnxc_data_table_info_get_f EGRESS_PARSING_CONTEXT_MAP_JR1_MODE_info_get;
    
    dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES___image_get_f MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES___image_get;
    
    dnxc_data_table_info_get_f MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES___image_info_get;
    
    dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_get_f MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_get;
    
    dnxc_data_table_info_get_f MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_info_get;
} dnx_data_if_aod_tables_t;





typedef struct
{
    
    dnx_data_if_aod_tables_t tables;
} dnx_data_if_aod_t;




extern dnx_data_if_aod_t dnx_data_aod;


#endif 

