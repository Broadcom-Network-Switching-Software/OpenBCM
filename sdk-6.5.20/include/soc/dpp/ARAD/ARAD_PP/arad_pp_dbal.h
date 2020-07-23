/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
*/

#ifndef __ARAD_PP_DBAL_INCLUDED__
#define __ARAD_PP_DBAL_INCLUDED__


#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/ARAD/arad_pmf_low_level_ce.h>
#include <soc/dpp/ARAD/arad_pmf_low_level_db.h>
#include <soc/dpp/ARAD/arad_pmf_low_level.h>
#include <soc/dpp/ARAD/arad_pmf_low_level_pgm.h>
#include <shared/swstate/sw_state.h>
#include <soc/dpp/ARAD/arad_api_general.h>
#include <soc/dpp/ARAD/arad_api_end2end_scheduler.h>
#include <soc/dpp/ARAD/arad_api_ingress_packet_queuing.h>
#include <soc/dpp/ARAD/arad_api_mgmt.h>
#include <soc/dpp/ARAD/arad_egr_prog_editor.h>
#include <soc/dpp/ARAD/arad_pmf_low_level_fem_tag.h>
#include <soc/dpp/ARAD/arad_pmf_prog_select.h>
#include <soc/dpp/ARAD/arad_tcam.h>
#include <soc/dpp/ARAD/arad_tdm.h>
#include <soc/dpp/ARAD/arad_cell.h>
#include <soc/dpp/ARAD/arad_dram.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_l3_src_bind.h>
#include <soc/dpp/SAND/Utils/sand_occupation_bitmap.h>
#include <soc/dpp/SAND/Utils/sand_hashtable.h>
#include <soc/dpp/SAND/Utils/sand_sorted_list.h>
#include <soc/dpp/ARAD/arad_tcam_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_fp_key.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_fp_fem.h>
#include <soc/dpp/ARAD/arad_ingress_traffic_mgmt.h>
#include <soc/dpp/SAND/Utils/sand_multi_set.h>
#include <soc/dpp/ARAD/arad_sim_em_block.h>
#include <soc/dpp/ARAD/arad_interrupts.h>
#include <soc/dpp/ARAD/arad_mgmt.h>
#include <appl/diag/parse.h>


#define DBAL_MAX_PROGRAMS_PER_TABLE                         10
#define DBAL_MAX_NAME_LENGTH                                128 
#define DBAL_DEFAULT_NOF_ENTRIES_FOR_TABLE_BLOCK_GET        50

#define DBAL_KAPS_2ND_LKP_DIASBLE 0XFE


#define DBAL_PREFIX_NOT_DEFINED             (-1)

#define SOC_DPP_DBAL_USE_COMPLETE_KEY       255

#define SOC_DPP_DBAL_ZERO_VRF_IN_KEY(is_dynamic)        (is_dynamic ? 35 : 36)


#define SOC_DPP_DBAL_ZERO_INRIF_IN_KEY        (35)


#define SOC_DPP_DBAL_HITBIT_FLAG_CLEAR        (1 << 7) 
#define SOC_DPP_DBAL_HITBIT_FLAG_HIT          (1 << 0) 

typedef enum
{
    SOC_DPP_HW_KEY_LOOKUP_DISABLED,
    SOC_DPP_HW_KEY_LOOKUP_IN_LEM_1ST,
    SOC_DPP_HW_KEY_LOOKUP_IN_LEM_2ND,
    SOC_DPP_HW_KEY_LOOKUP_IN_LEARN_KEY
}SOC_DPP_HW_KEY_LOOKUP;

typedef enum
{
    SOC_DPP_DBAL_DIAG_ENTRY_MANAGE_ADD,
    SOC_DPP_DBAL_DIAG_ENTRY_MANAGE_DELETE,
    SOC_DPP_DBAL_DIAG_ENTRY_MANAGE_NUM_OF_MODE
}SOC_DPP_DBAL_DIAG_ENTRY_MANAGE_MODE;

#define DBAL_QUAL_INFO_CLEAR(qual_info, nof_elements)   sal_memset(qual_info, 0, sizeof(SOC_DPP_DBAL_QUAL_INFO)*nof_elements)

#define SOC_DPP_DBAL_NOF_DYNAMIC_TABLES         50

typedef enum
{
    SOC_DPP_DBAL_SW_TABLE_ID_INVALID = -1,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_LEM,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_RPF_LEM,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_CUSTOM_LEM,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_LEM_DEFAULT,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_MPLS_EXTENDED,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_EXTENDED,
    SOC_DPP_DBAL_SW_TABLE_ID_P2P_EXTENDED,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_FLEXIBLE,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KAPS,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_RPF_KAPS,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KAPS,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_BRIDGE_FID,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_BRIDGE_VLAN_FID_UNTAGGED,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_BRIDGE_VLAN_FID,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_LEM,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_LEARN_LEM,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_MASTER_KBP,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_FWD_KBP,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_RPF_KBP,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_RPF_DUMMY_KBP,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_PUBLIC_FWD_KBP,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_PUBLIC_RPF_KBP,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_PUBLIC_RPF_DUMMY_KBP,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_FWD_KBP,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_RPF_KBP,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_DUMMY_KBP,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_RPF_KAPS,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_MASTER_KBP,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_FWD_KBP,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_FWD_KBP_SIP_SHARING,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_RPF_KBP,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_RPF_DUMMY_KBP,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_PUBLIC_FWD_KBP,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_PUBLIC_RPF_KBP,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_PUBLIC_RPF_DUMMY_KBP,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC_FWD_KBP,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC_RPF_KBP,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC_DUMMY_KBP,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KBP_DEFAULT_ROUTE_KAPS,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KBP_DEFAULT_ROUTE_KAPS,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV4DC_KBP_DEFAULT_ROUTE_KAPS,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_KBP_DEFAULT_ROUTE_KAPS,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC_KBP_DEFAULT_ROUTE_KAPS,
    SOC_DPP_DBAL_SW_TABLE_ID_FCOE_KAPS,
    SOC_DPP_DBAL_SW_TABLE_ID_FCOE_NPORT_KAPS,
    SOC_DPP_DBAL_SW_TABLE_ID_OAM_STATISTICS,
    SOC_DPP_DBAL_SW_TABLE_ID_BFD_STATISTICS,
    SOC_DPP_DBAL_SW_TABLE_ID_BFD_MPLS_STATISTICS,
    SOC_DPP_DBAL_SW_TABLE_ID_BFD_PWE_STATISTICS,
    SOC_DPP_DBAL_SW_TABLE_ID_OAM_DOWN_UNTAGGED_STATISTICS,
    SOC_DPP_DBAL_SW_TABLE_ID_OAM_SINGLE_TAG_STATISTICS,
    SOC_DPP_DBAL_SW_TABLE_ID_OAM_DOUBLE_TAG_STATISTICS,
    SOC_DPP_DBAL_SW_TABLE_ID_IVL_LEARN_LEM,
    SOC_DPP_DBAL_SW_TABLE_ID_IVL_FWD_LEM,
    SOC_DPP_DBAL_SW_TABLE_ID_BFD_ECHO_LEM,
    SOC_DPP_DBAL_SW_TABLE_ID_IVL_INNER_LEARN_LEM,           
    SOC_DPP_DBAL_SW_TABLE_ID_IVL_INNER_FWD_LEM,
    SOC_DPP_DBAL_SW_TABLE_ID_IVL_FWD_OUTER_LEARN_LEM,           
    SOC_DPP_DBAL_SW_TABLE_ID_IVL_FWD_OUTER_FWD_LEM,
    SOC_DPP_DBAL_SW_TABLE_ID_LSR_CNT_LEM,
    SOC_DPP_DBAL_SW_TABLE_ID_LSR_LEM,
    SOC_DPP_DBAL_SW_TABLE_ID_LSR_KBP,
    SOC_DPP_DBAL_SW_TABLE_ID_ETHERNET_TK_EPON_UNI_V4_STATIC_LEM,
    SOC_DPP_DBAL_SW_TABLE_ID_ETHERNET_TK_EPON_UNI_V4_STATIC_KAPS,
    SOC_DPP_DBAL_SW_TABLE_ID_ETHERNET_TK_EPON_UNI_V6_STATIC_LEM,
    SOC_DPP_DBAL_SW_TABLE_ID_ETHERNET_TK_EPON_UNI_V6_STATIC_KAPS,
    SOC_DPP_DBAL_SW_TABLE_ID_ETHERNET_TK_EPON_UNI_LEM_DEFAULT_FORWARD,
    SOC_DPP_DBAL_SW_TABLE_ID_TRILL_FLP_TCAM,
	SOC_DPP_DBAL_SW_TABLE_ID_IPV4_DC_MASTER,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV4_DC,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV4_DC_DUMMY_KBP,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_ROUTE_SCALE_LEM,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_ROUTE_SCALE_SHORT_KAPS,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_ROUTE_SCALE_LONG_KAPS,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE_SCALE_SHORT_LEM,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE_SCALE_LONG_LEM,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE_SCALE_SHORT_KAPS,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE_SCALE_LONG_KAPS,
    SOC_DPP_DBAL_SW_TABLE_ID_VPWS_TAGGED_SINGLE_TAG,
    SOC_DPP_DBAL_SW_TABLE_ID_VPWS_TAGGED_DOUBLE_TAG,
    SOC_DPP_DBAL_SW_TABLE_ID_ETHERNET_TK_EPON_UNI_V4_STATIC_LEM_ARP_REQ,
    SOC_DPP_DBAL_SW_TABLE_ID_ETHERNET_TK_EPON_UNI_V4_STATIC_LEM_ARP_RPY,
    SOC_DPP_DBAL_SW_TABLE_ID_ETHERNET_TK_EPON_UNI_V4_STATIC_KAPS_ARP_REQ,
    SOC_DPP_DBAL_SW_TABLE_ID_ETHERNET_TK_EPON_UNI_V4_STATIC_KAPS_ARP_RPY,
    SOC_DPP_DBAL_SW_TABLE_ID_ETHERNET_TK_EPON_UNI_LEM_DEFAULT_LEARN,
    SOC_DPP_DBAL_SW_TABLE_ID_PPPOE_IPV4_ANTI_SPOOFING,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV4_ANTI_SPOOFING_KAPS,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV6_ANTI_SPOOFING_KAPS,
    SOC_DPP_DBAL_SW_TABLE_ID_OUTLIF_L2G_LEM,
    SOC_DPP_DBAL_SW_TABLE_ID_L3_MAC_LEARNING_CUSTOM_LEM,
    SOC_DPP_DBAL_SW_TABLE_ID_ENCAP_TO_DEST_LEM,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC_SSM_COMPRESS_SIP_KAPS,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC_SSM_KAPS,


    SOC_DPP_DBAL_SW_TABLE_ID_VTT_FIRST,
    SOC_DPP_DBAL_SW_TABLE_ID_VDxINITIALVID_SEM_A = SOC_DPP_DBAL_SW_TABLE_ID_VTT_FIRST,
    SOC_DPP_DBAL_SW_TABLE_ID_VDxINITIALVID_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTER_INNER_VID_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTER_INNER_VID_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTER_INITIAL_VID_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTER_INITIAL_VID_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_PCP_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_EVB_SINGLE_TAG_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_EVB_DOUBLE_TAG_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_INITIAL_VID_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_OUTER_VID_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_INNER_OUTER_VID_PCP_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_SPOOFv4_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_DIPv6_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_DIPv6_COMPRESSED_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_SPOOFv6_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_SPOOFv6_COMPRESSED_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_DUMMY_UNINDEXED_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_UNINDEXED_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_UNINDEXED_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_UNINDEXED_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_UNINDEXED_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_UNINDEXED_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_UNINDEXED_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_GAL_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_GAL_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_GAL_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_PORT_L1_L1IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_PORT_L3_L1IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_PORT_L1_L1IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_PORT_L3_L1IDX_SEM_A,

    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L1IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L1IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L2IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L2IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L3IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L3IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L13IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L13IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L12IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L12IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L1IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L1IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L2IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L2IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L3IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L3IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L13IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L13IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L12IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L12IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_L1IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_L1IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_L2IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_L2IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_L3IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_L3IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_L13IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_L13IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_L12IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_L12IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_L1IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_L1IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_L2IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_L2IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_L3IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_L3IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_L13IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_L13IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_L12IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_L12IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_BOS_L1IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_BOS_L1IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_BOS_L2IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_BOS_L2IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_BOS_L3IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_BOS_L3IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_BOS_L13IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_BOS_L13IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_BOS_L12IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_BOS_L12IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_BOS_L1IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_BOS_L1IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_BOS_L2IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_BOS_L2IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_BOS_L3IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_BOS_L3IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_BOS_L13IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_BOS_L13IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_BOS_L12IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_BOS_L12IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_BOS_L1IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_BOS_L1IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_BOS_L2IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_BOS_L2IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_BOS_L3IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_BOS_L3IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_BOS_L13IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_BOS_L13IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_BOS_L12IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_BOS_L12IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_BOS_L1IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_BOS_L1IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_BOS_L2IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_BOS_L2IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_BOS_L3IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_BOS_L3IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_BOS_L13IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_BOS_L13IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_BOS_L12IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_BOS_L12IDX_SEM_B,

    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_ELI_UNINDEXED_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_ELI_UNINDEXED_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_ELI_UNINDEXED_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_ELI_UNINDEXED_SEM_B,

    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_ELI_L1IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_ELI_L1IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_ELI_L2IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_ELI_L2IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_ELI_L3IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_ELI_L3IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_ELI_L13IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_ELI_L13IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_ELI_L12IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_ELI_L12IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_ELI_L1IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_ELI_L1IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_ELI_L2IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_ELI_L2IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_ELI_L3IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_ELI_L3IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_ELI_L13IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_ELI_L13IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_ELI_L12IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_ELI_L12IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_L1IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_L1IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_L2IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_L2IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_L3IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_L3IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_L13IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_L13IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_L12IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_L12IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_ELI_L1IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_ELI_L1IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_ELI_L2IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_ELI_L2IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_ELI_L3IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_ELI_L3IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_ELI_L13IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_ELI_L13IDX_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_ELI_L12IDX_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_ELI_L12IDX_SEM_B,

    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L2_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_FRR_INITIAL_VID_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_FRR_OUTER_VID_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_MPLS4_INITIAL_VID_L1_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_MPLS4_OUTER_VID_L1_TCAM,
	SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_TCAM,
	SOC_DPP_DBAL_SW_TABLE_ID_MPLS_INITIAL_VID_L4B_L5G_TCAM,
	SOC_DPP_DBAL_SW_TABLE_ID_MPLS_OUTER_VID_L4B_L5G_TCAM,
	SOC_DPP_DBAL_SW_TABLE_ID_VDxINNERVID_SEM_A,
	SOC_DPP_DBAL_SW_TABLE_ID_VDxINNERVID_SEM_B,
	SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTER_INNER_PCP_VID_SEM_A,
	SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTER_INNER_PCP_VID_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_TRILL_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_DESIGNATED_VID_TRILL_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_TRILL_ING_NICK_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_TRILL_UC_ONE_TAG_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_TRILL_UC_TWO_TAG_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_TRILL_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_DIPv4_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_DIPv4_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_DIPv4_DUMMY_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_DIPv4_DUMMY_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_SIPv4_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_DIP_SIP_INITIAL_VID_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_DIP_SIP_OUTER_VID_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_ETHERNET_HEADER_ISID_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_ETHERNET_HEADER_BSA_ISID_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MC_DIPv4_RIF_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_TUNNEL_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_VXLAN_ID_TUNNEL_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_L2GRE_ID_TUNNEL_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_SINGLE_TAG_VD_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_DOUBLE_TAG_VD_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_5_TUPPLE_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_VD_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_INITIAL_VD_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_OUTER_VD_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_OUTER_INNER_VD_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_UNTAGGED_TST1_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_ONE_TAG_TST1_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_DOUBLE_TAG_TST1_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_UNTAGGED_VRRP_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_ONE_TAG_VRRP_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_DOUBLE_TAG_VRRP_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_TST1_MPLS_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_QINQ_COMPRESSED_TPID1_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_QINQ_COMPRESSED_TPID2_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_QINANY_TPID1_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_QINANY_TPID2_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_QINANY_PCP_TPID1_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_QINANY_PCP_TPID2_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_1Q_TPID1_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_1Q_TPID2_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_1Q_COMPRESSED_TPID1_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_1Q_COMPRESSED_TPID2_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_1Q_PCP_COMPRESSED_TPID1_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_1Q_PCP_COMPRESSED_TPID2_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_UNTAGGED_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_EXTENDER_UNTAG_CHECK_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_EXTENDER_PE_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_EXTENDER_PE_UT_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_EXTENDER_CHANNEL_REG_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_TST2_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_TEST2_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_TEST2_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_SRC_PORT_DA_OUTER_VID_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_SRC_PORT_DA_INITIAL_VID_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_DIP_SIP_VRF_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_GRE_DUMMY_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_UNINDEXED_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_UNINDEXED_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_MY_VTEP_INDEX_SIP_VRF_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_CUSTOM_PP_PORT_TUNNEL_SEMA,
    SOC_DPP_DBAL_SW_TABLE_ID_CUSTOM_PP_PORT_LSP_ECMP_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_VLAN_DOMAIN_MPLS_L1_SEMA,
    SOC_DPP_DBAL_SW_TABLE_ID_VLAN_DOMAIN_MPLS_L1_SEMB,
    SOC_DPP_DBAL_SW_TABLE_ID_VLAN_DOMAIN_MPLS_L2_SEMA,
    SOC_DPP_DBAL_SW_TABLE_ID_VLAN_DOMAIN_MPLS_L2_SEMB,
    SOC_DPP_DBAL_SW_TABLE_ID_OAM_STAT_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_OAM_STAT_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_BFD_STAT_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_BFD_STAT_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_TM_STAT_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_TM_STAT_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_FC_WITH_VFT_VSAN_VFT_SEMB,
    SOC_DPP_DBAL_SW_TABLE_ID_FC_WITH_VFT_VSAN_VSI_SEMB,
    SOC_DPP_DBAL_SW_TABLE_ID_FC_VSAN_VFT_SEMB,
    SOC_DPP_DBAL_SW_TABLE_ID_FC_VSAN_VSI_SEMB,
    SOC_DPP_DBAL_SW_TABLE_ID_PON_VDxTUNNEL_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_PON_VDxTUNNEL_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_PON_VDxTUNNELxETHTYPE_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_PON_VDxTUNNELxOTUER_VID_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_PON_VDxTUNNELxETHTYPExPCPxOUTER_VID_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_PON_VDxTUNNELxOTUER_VIDxINNER_VID_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_PON_VDxTUNNELxOTUER_VID_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_PON_VDxTUNNELxOTUER_VIDxINNER_VID_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_PON_VDxTUNNELxETHTYPExPCPxOUTER_VIDxINNER_VID_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_SECTION_OAM_VDxOTUER_VIDxETH_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_SECTION_OAM_VDxOTUER_VIDxINNER_VIDxETH_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV6_DIP_NP_ET_INITIAL_VID_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV6_DIP_NP_ET_OUTER_VID_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV6_SIP_VRF_COMP_DEST_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV6_DIP_NP_ET_INITIAL_VID_AFTER_RECYCLE_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_IPV6_DIP_NP_ET_OUTER_VID_AFTER_RECYCLE_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_DIPv6_AFTER_RECYCLE_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_GENERALIZED_RCH0_TCAM,
    SOC_DPP_DBAL_SW_TABLE_ID_GENERALIZED_RCH1_TCAM,
	SOC_DPP_DBAL_SW_TABLE_ID_L2TP_ISEM_A,
	SOC_DPP_DBAL_SW_TABLE_ID_PPPOE_ISEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_DIPv4_EXPLICIT_NULL_DUMMY_SEM_A,
    SOC_DPP_DBAL_SW_TABLE_ID_DIPv4_EXPLICIT_NULL_DUMMY_SEM_B,
    SOC_DPP_DBAL_SW_TABLE_ID_TST1_TCAM_SKELETON, 

    SOC_DPP_DBAL_SW_TABLE_ID_VTT_LAST = SOC_DPP_DBAL_SW_TABLE_ID_TST1_TCAM_SKELETON,

    

    SOC_DPP_DBAL_SW_TABLE_DYNAMIC_BASE_ID, 
    SOC_DPP_DBAL_SW_TABLE_DYNAMIC_END_ID = SOC_DPP_DBAL_SW_TABLE_DYNAMIC_BASE_ID + SOC_DPP_DBAL_NOF_DYNAMIC_TABLES,


    SOC_DPP_DBAL_SW_TABLE_ID_GPON_L2_IP4MC,
    SOC_DPP_DBAL_SW_TABLE_ID_GPON_L2_IP4MC_SSM,
    SOC_DPP_DBAL_SW_TABLE_ID_GPON_L2_IP6MC_SSM_EUI,
    
    SOC_DPP_DBAL_SW_NOF_TABLES

}SOC_DPP_DBAL_SW_TABLE_IDS;


typedef enum
{
    SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM,
    SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM,
    SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP,
    SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A,
    SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B,
    SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS,
	
    SOC_DPP_DBAL_PHYSICAL_DB_TYPE_ESEM,
    SOC_DPP_DBAL_PHYSICAL_DB_TYPE_OAM1,
    SOC_DPP_DBAL_PHYSICAL_DB_TYPE_OAM2,
    SOC_DPP_DBAL_PHYSICAL_DB_TYPE_RMAP,
    SOC_DPP_DBAL_PHYSICAL_DB_TYPE_GLEM,

    SOC_DPP_DBAL_PHYSICAL_DB_NOF_TYPES

}SOC_DPP_DBAL_PHYSICAL_DB_TYPES;

typedef enum
{
    SOC_DPP_DBAL_PROGRAM_KEY_A,
    SOC_DPP_DBAL_PROGRAM_KEY_B,
    SOC_DPP_DBAL_PROGRAM_KEY_C,
    SOC_DPP_DBAL_PROGRAM_KEY_D,

    SOC_DPP_DBAL_PROGRAM_KEY_A_MSB,
    SOC_DPP_DBAL_PROGRAM_KEY_B_MSB,
    SOC_DPP_DBAL_PROGRAM_KEY_C_MSB,
    SOC_DPP_DBAL_PROGRAM_KEY_D_MSB,

    SOC_DPP_DBAL_PROGRAM_NOF_KEYS,

    SOC_DPP_DBAL_PROGRAM_KBP_MULTI_KEY 
}SOC_DPP_DBAL_PROGRAM_KEYS;

#define SOC_DPP_DBAL_PROGRAM_NOF_COMPLETE_KEYS      SOC_DPP_DBAL_PROGRAM_NOF_KEYS/2


#define SOC_DPP_DBAL_ATI_NONE                           0
#define SOC_DPP_DBAL_ATI_TCAM_DUMMY                     1
#define SOC_DPP_DBAL_ATI_TCAM_STATIC                    2
#define SOC_DPP_DBAL_ATI_KBP_MASTER_KEY_INDICATION      3
#define SOC_DPP_DBAL_ATI_KBP_LOOKUP_ONLY                4
#define SOC_DPP_DBAL_ATI_KBP_NO_DB_UPDATE               5
#define SOC_DPP_DBAL_ATI_KBP_DUMMY_TABLE                6
#define SOC_DPP_DBAL_ATI_KBP_FLP_CE_ALLOC_ONLY          7

#define SOC_DPP_DBAL_PROGRAM_NOF_COMPLETE_KEYS      SOC_DPP_DBAL_PROGRAM_NOF_KEYS/2


#define MEM_TYPE_TO_KEY(mem_type)  (mem_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A) ? SOC_DPP_DBAL_PROGRAM_KEY_A : \
                                   (mem_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B) ? SOC_DPP_DBAL_PROGRAM_KEY_B : SOC_DPP_DBAL_PROGRAM_KEY_C 

typedef struct
{
    SOC_PPC_FP_QUAL_TYPE    qual_type;      
    uint8                   qual_offset;    
    uint8                   ignore_qual_offset_for_entry_mngmnt;
    uint8                   qual_nof_bits;  
    uint8                   qual_full_size; 
    uint8                   qual_is_in_hdr; 
}SOC_DPP_DBAL_QUAL_INFO;



typedef struct
{
    uint8                       lookup_number; 
    SOC_DPP_DBAL_PROGRAM_KEYS   key_id; 
    SOC_DPP_DBAL_SW_TABLE_IDS   sw_table_id;
    uint32                      public_lpm_lookup_size; 
}SOC_DPP_DBAL_KEY_TO_TABLE;

typedef struct
{
    SOC_PPC_FP_DATABASE_STAGE  stage;
    uint32                  program_id;
    uint8                   lookup_number; 
    uint32                  key_id;
    int                     nof_bits_used_in_key;
    uint8                   ce_assigned[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX]; 
    uint32                  public_lpm_lookup_size; 
}DBAL_PROGRAM_INFO;

typedef void *DBAL_ITERATOR;


typedef struct
{
    uint8                           is_table_initiated;
    SOC_DPP_DBAL_PHYSICAL_DB_TYPES  physical_db_type;
    uint32                          db_prefix; 
    uint32                          db_prefix_len; 
    SOC_DPP_DBAL_QUAL_INFO          qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int                             nof_qualifiers;
    
    uint32                          additional_table_info;
    char                            table_name[DBAL_MAX_NAME_LENGTH];
    DBAL_PROGRAM_INFO               table_programs[DBAL_MAX_PROGRAMS_PER_TABLE];
    int                             nof_table_programs;
    uint32                          nof_entries_added_to_table;
    SOC_PPC_FP_ACTION_TYPE          action[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX];
    int                             nof_actions;
    DBAL_ITERATOR                   table_iterator;
} SOC_DPP_DBAL_TABLE_INFO;


#define ARAD_PP_ISEM_ACCESS_MAX_PREFIX_SIZE_JER                     6

typedef struct SOC_DPP_DBAL_INFO_s
{
  SOC_DPP_DBAL_TABLE_INFO dbal_tables[SOC_DPP_DBAL_SW_NOF_TABLES];
  SOC_DPP_DBAL_SW_TABLE_IDS sem_a_prefix_mapping[(1 << ARAD_PP_ISEM_ACCESS_MAX_PREFIX_SIZE_JER)];
  SOC_DPP_DBAL_SW_TABLE_IDS sem_b_prefix_mapping[(1 << ARAD_PP_ISEM_ACCESS_MAX_PREFIX_SIZE_JER)];
} SOC_DPP_DBAL_INFO;


uint32 arad_pp_dbal_init(int unit);
uint32 arad_pp_dbal_deinit(int unit);
uint32 arad_pp_dbal_ce_info_get(int unit, uint32 table_id, SOC_PPC_FP_DATABASE_STAGE stage, ARAD_PMF_CE *ce_array, uint8 *nof_ces, uint8 *is_key_320b, uint8 *ces_ids);
uint32 arad_pp_dbal_db_predfix_get(int unit, uint32 table_id, uint32* db_prefix);
uint32 arad_pp_dbal_table_is_initiated(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, int* is_table_initiated);
uint32 arad_pp_dbal_qualifier_full_size_get(int unit, SOC_PPC_FP_DATABASE_STAGE stage, SOC_PPC_FP_QUAL_TYPE qual_type, uint8* qual_full_size, uint8* qual_is_in_hdr);
const char* arad_pp_dbal_key_id_to_string(int unit, uint8 key_id);





uint32 arad_pp_dbal_table_create(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint32 db_prefix, uint32 db_prefix_len,
                                  SOC_DPP_DBAL_PHYSICAL_DB_TYPES physical_db, int nof_qualifiers, uint32 additional_info,
                                  SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], char* table_name);

uint32 arad_pp_dbal_dynamic_table_create(int unit, SOC_DPP_DBAL_PHYSICAL_DB_TYPES physical_db, int nof_qualifiers,
                                  SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], char* table_name, SOC_DPP_DBAL_SW_TABLE_IDS* table_id);


uint32 arad_pp_dbal_table_clear(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id);
uint32 arad_pp_dbal_table_print(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id);
uint32 arad_pp_dbal_table_stage_create(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, SOC_PPC_FP_DATABASE_STAGE stage, int lookup_number, int program_id );
uint32 arad_pp_dbal_flp_lookup_update(int unit, DBAL_PROGRAM_INFO table_program, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint8 is_to_remove, uint8* lookup_used, int is_msb);
uint32 arad_pp_dbal_table_actions_set(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, SOC_PPC_FP_ACTION_TYPE action[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX]);
uint32 arad_pp_dbal_table_destroy(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id);
uint32 arad_pp_dbal_table_physical_db_get(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, SOC_DPP_DBAL_PHYSICAL_DB_TYPES* physical_db_type );
uint32 arad_pp_dbal_table_disassociate_all(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id);

uint32 arad_pp_dbal_table_kaps_hitbit_set(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint8 enable);
uint32 arad_pp_dbal_table_kaps_hitbit_get(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint8 *enable);


uint32 arad_pp_dbal_key_inst_set(int unit, int program_id, SOC_PPC_FP_DATABASE_STAGE stage, int key_id, SOC_DPP_DBAL_QUAL_INFO qual_info, int is_msb, int ce_id);
uint32 arad_pp_dbal_program_to_tables_associate(int unit, int program_id, SOC_PPC_FP_DATABASE_STAGE stage,
                                                SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS], uint8 use_32_bit_ce[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] [SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], int nof_valid_keys);

uint32 arad_pp_dbal_program_to_tables_associate_implicit(int unit, int program_id, SOC_PPC_FP_DATABASE_STAGE stage,
                                                         SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS],
                                                         uint8 qualifier_to_ce_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS][SOC_PPC_FP_NOF_QUALS_PER_DB_MAX],
                                                         int nof_valid_keys);
uint32 arad_pp_dbal_program_table_disassociate(int unit, int program_id, SOC_PPC_FP_DATABASE_STAGE stage, SOC_DPP_DBAL_SW_TABLE_IDS table_id);

uint32 arad_pp_dbal_entry_add(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], uint32 priority, void* payload, SOC_SAND_SUCCESS_FAILURE *success);
uint32 arad_pp_dbal_entry_get(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], void* payload, uint32* priority, uint8* hit_bit, uint8 *found);
uint32 arad_pp_dbal_entry_delete(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], SOC_SAND_SUCCESS_FAILURE *success);


uint32 arad_pp_dbal_iterator_init(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, SOC_DPP_DBAL_PHYSICAL_DB_TYPES* physical_db_type);
uint32 arad_pp_dbal_iterator_get_next(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint32 flags, SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], void* payload, uint32* priority, uint8* hit_bit, uint8 *found);
uint32 arad_pp_dbal_iterator_deinit(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id);

#if defined(INCLUDE_KBP)
uint32 arad_pp_dbal_entry_key_to_kbp_buffer(int unit, SOC_DPP_DBAL_TABLE_INFO *dbal_table, uint32 table_size_in_bytes, SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], uint32 *prefix_len, uint8 *data_bytes);
#endif

uint32 arad_pp_dbal_block_get(int unit, SOC_PPC_IP_ROUTING_TABLE_RANGE *block_range_key, SOC_DPP_DBAL_SW_TABLE_IDS table_id, SOC_PPC_FP_QUAL_VAL *qual_vals_array, void* payload, uint32 *nof_entries);


uint32 arad_pp_dbal_entry_add_id(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint32 entry_id, SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], uint32 priority, ARAD_TCAM_ACTION* payload, uint8 is_for_update, SOC_SAND_SUCCESS_FAILURE *success);
uint32 arad_pp_dbal_entry_get_id(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint32 entry_id, SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], void* payload, uint32* priority, uint8* hit_bit, uint8 *found);
uint32 arad_pp_dbal_entry_delete_id(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint32 entry_id, SOC_SAND_SUCCESS_FAILURE *success);



uint32 arad_pp_dbal_last_packet_dump(int unit, int core_id);
uint32 arad_pp_dbal_ce_per_program_dump(int unit, int program_id, SOC_PPC_FP_DATABASE_STAGE stage);
uint32 arad_pp_dbal_table_info_dump(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id);
uint32 arad_pp_dbal_tables_dump(int unit, int is_full_info);
uint32 arad_pp_dbal_entry_dump(int unit, SOC_PPC_FP_DATABASE_STAGE stage, SOC_DPP_DBAL_SW_TABLE_IDS table_id, SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], uint32 val[ARAD_PP_FP_TCAM_ENTRY_SIZE]);
uint32 arad_pp_dbal_entry_input_validate(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, SOC_PPC_FP_QUAL_VAL in_qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX]);
uint32 arad_pp_dbal_diag_entry_manage(int unit, SOC_DPP_DBAL_DIAG_ENTRY_MANAGE_MODE mode, int table_id, args_t* arg);
uint32 arad_pp_dbal_isem_prefix_table_dump(int unit);
uint32 arad_pp_dbal_tcam_prefix_table_dump(int unit);
uint32 arad_pp_dbal_lem_prefix_table_dump(int unit);
const char* arad_pp_dbal_physical_db_to_string(SOC_DPP_DBAL_PHYSICAL_DB_TYPES physical_db_type);
int arad_pp_dbal_program_to_string(int unit, SOC_PPC_FP_DATABASE_STAGE stage, int cam_line, const char**str, uint8* prog_id);
uint32 arad_pp_dbal_phisycal_db_dump(int unit, int mode);
uint32 arad_pp_dbal_qualifier_to_instruction(int unit, SOC_PPC_FP_DATABASE_STAGE stage, uint8 is_32b_ce, uint32 nof_bits, SOC_PPC_FP_QUAL_TYPE qual_type, int is_msb, uint32* ce_instr_encoding);
uint32 arad_pp_dbal_qualifier_to_instruction_dump(int unit,SOC_PPC_FP_DATABASE_STAGE stage, int is_32b_ce, int nof_bits, SOC_PPC_FP_QUAL_TYPE qual_type);



uint32 arad_pp_dbal_kbp_buffer_to_entry_key(int unit, const SOC_DPP_DBAL_TABLE_INFO *dbal_table, uint32 prefix_len, uint8 *data_bytes,
                                            SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX]);


#define DBAL_QUAL_VALS_CLEAR(qual_vals)                                                                                      \
    {uint32 qual_index;                                                                                                      \
        sal_memset(qual_vals, 0x0, sizeof(SOC_PPC_FP_QUAL_VAL)*SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);                             \
        for (qual_index = 0; qual_index < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; ++qual_index)                                     \
        {                                                                                                                    \
            (qual_vals)[qual_index].type = BCM_FIELD_ENTRY_INVALID;                                                          \
        }                                                                                                                    \
    }

#define DBAL_QUAL_VAL_ENCODE_FID(qual_val, fid_ndx)                                                                          \
    *qual_val.type = SOC_PPC_FP_QUAL_FID;                                                                                    \
    *qual_val.val.arr[0] = fid_ndx;                                                                                          \
    *qual_val.is_valid.arr[0] = SOC_SAND_U32_MAX;

#define DBAL_QUAL_VAL_ENCODE_VRF(qual_val, vrf_ndx, mask)                                                                    \
    *qual_val.type = SOC_PPC_FP_QUAL_IRPP_VRF;                                                                               \
    *qual_val.val.arr[0] = vrf_ndx;                                                                                          \
    *qual_val.is_valid.arr[0] = mask;

#define DBAL_QUAL_VAL_ENCODE_IPV4(qual_val, ip_address, prefix_len)                                                          \
    *qual_val.val.arr[0] = ip_address;                                                                                       \
                                                               \
    *qual_val.is_valid.arr[0] = prefix_len ? (SOC_SAND_U32_MAX << (SOC_SAND_PP_IPV4_ADDRESS_NOF_BITS - prefix_len)) : 0;

#define DBAL_QUAL_VAL_ENCODE_AFTER_FWD_IPV4_DIP(qual_val, ip_address, prefix_len)                                            \
    *qual_val.type = SOC_PPC_FP_QUAL_HDR_AFTER_FWD_IPV4_DIP;                                                                 \
    DBAL_QUAL_VAL_ENCODE_IPV4(qual_val, ip_address, prefix_len);

#define DBAL_QUAL_VAL_ENCODE_AFTER_FWD_IPV4_SIP(qual_val, ip_address, prefix_len)                                            \
    *qual_val.type = SOC_PPC_FP_QUAL_HDR_AFTER_FWD_IPV4_SIP;                                                                 \
    DBAL_QUAL_VAL_ENCODE_IPV4(qual_val, ip_address, prefix_len);

#define DBAL_QUAL_VAL_ENCODE_FWD_IPV4_DIP(qual_val, ip_address, prefix_len)                                                  \
    *qual_val.type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP;                                                                       \
    DBAL_QUAL_VAL_ENCODE_IPV4(qual_val, ip_address, prefix_len);

#define DBAL_QUAL_VAL_ENCODE_FWD_IPV4_SIP(qual_val, ip_address, prefix_len)                                                  \
    *qual_val.type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_SIP;                                                                       \
    DBAL_QUAL_VAL_ENCODE_IPV4(qual_val, ip_address, prefix_len);

#define DBAL_QUAL_VAL_ENCODE_IPV4_DIP(qual_val, ip_address, prefix_len)                                                      \
    *qual_val.type = SOC_PPC_FP_QUAL_HDR_IPV4_DIP;                                                                           \
    DBAL_QUAL_VAL_ENCODE_IPV4(qual_val, ip_address, prefix_len);

#define DBAL_QUAL_VAL_ENCODE_IPV4_SIP(qual_val, ip_address, prefix_len)                                                      \
    *qual_val.type = SOC_PPC_FP_QUAL_HDR_IPV4_SIP;                                                                           \
    DBAL_QUAL_VAL_ENCODE_IPV4(qual_val, ip_address, prefix_len);


#define DBAL_QUAL_VAL_ENCODE_IN_RIF(qual_val, inrif, mask)                                                                   \
    *qual_val.type = SOC_PPC_FP_QUAL_IRPP_IN_RIF;                                                                            \
    *qual_val.val.arr[0] = inrif;                                                                                            \
    *qual_val.is_valid.arr[0] = mask;

#define DBAL_QUAL_VAL_ENCODE_IPV6(qual_val, address, mask)                                                                   \
    *qual_val.val.arr[0] = *(address + 0);                                                                                   \
    *qual_val.is_valid.arr[0] = *(mask + 0);                                                                                 \
    *qual_val.val.arr[1] = *(address + 1);                                                                                   \
    *qual_val.is_valid.arr[1] = *(mask + 1);

#define DBAL_QUAL_VAL_ENCODE_IPV6_DIP_LOW(qual_val, address, mask)                                                           \
    *qual_val.type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_LOW;                                                                   \
    DBAL_QUAL_VAL_ENCODE_IPV6(qual_val, address, mask)

#define DBAL_QUAL_VAL_ENCODE_IPV6_DIP_HIGH(qual_val, address, mask)                                                          \
    *qual_val.type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;                                                                  \
    DBAL_QUAL_VAL_ENCODE_IPV6(qual_val, (address + 2), (mask + 2)) 

#define DBAL_QUAL_VAL_ENCODE_IPV6_SIP_LOW(qual_val, address, mask)                                                           \
    *qual_val.type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_LOW;                                                                   \
    DBAL_QUAL_VAL_ENCODE_IPV6(qual_val, address, mask)

#define DBAL_QUAL_VAL_ENCODE_IPV6_SIP_HIGH(qual_val, address, mask)                                                          \
    *qual_val.type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_HIGH;                                                                  \
    DBAL_QUAL_VAL_ENCODE_IPV6(qual_val, (address + 2), (mask + 2)) 

#define DBAL_QUAL_VAL_ENCODE_VLAN_INNERMOST(qual_val, value_)                                                                    \
    *qual_val.type = SOC_PPC_FP_QUAL_HDR_FWD_INNERMOST_VLAN_TAG_ID;                                                                               \
    *qual_val.val.arr[0] = value_;                                                                                          \
    *qual_val.is_valid.arr[0] = SOC_SAND_U32_MAX;

#define DBAL_QUAL_VAL_ENCODE_COMPRESSED_SIP(qual_val, compressed_sip, mask)                                                                    \
    *qual_val.type = SOC_PPC_FP_QUAL_IPV6_COMPRESSED_SIP;                                                                               \
    *qual_val.val.arr[0] = compressed_sip;                                                                                          \
    *qual_val.is_valid.arr[0] = mask;



typedef struct
{
    uint8  is_valid;
    uint32 prm_blk;
    uint16 addr_high;
    uint16 addr_low;
    uint32 msb;
    uint32 lsb;
    int signal_length_in_bits;
    int signal_offset;        

} SOC_DPP_SIGNAL_MNGR_SIGNAL_INFO;

typedef enum
{
    
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_LEM1_KEY,
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_LEM2_KEY,
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_LEM1_2_FLP_RES,
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_LEM2_2_FLP_RES,
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_LEM1_2_FLP_HIT,
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_LEM2_2_FLP_HIT,

    
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_LPM1_KEY,
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_LPM1_KEY_2,
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_LPM2_KEY,
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_LPM2_KEY_2,
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_LPM1_2_FLP_RES,
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_LPM2_2_FLP_RES,
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_LPM1_2_FLP_HIT,
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_LPM2_2_FLP_HIT,

    
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_TCAM_KEY,
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_TCAM_2_FLP_RES,
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_TCAM_2_FLP_HIT,

    
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_KBP_OPCODE,
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_KBP_KEY_PART1,
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_KBP_KEY_PART2,
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_KBP_KEY_PART3,
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_KBP_KEY_PART4,
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_KBP_KEY_PART5,
	SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_KBP_2_FLP_RES_LSB_PART1,
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_KBP_2_FLP_RES_LSB_PART2,
	SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_KBP_2_FLP_RES,
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_KBP_2_FLP_ERR,

    
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_VT_2_SEM_A_KEY,
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_VT_2_SEM_A_RES,
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_VT_2_SEM_A_HIT,

    
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_VT_2_SEM_B_KEY,
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_VT_2_SEM_B_RES,
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_VT_2_SEM_B_HIT,

    
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_VT_2_TCAM_KEY_LSB,
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_VT_2_TCAM_KEY_MSB,
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_VT_2_TCAM_RES,
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_VT_2_TCAM_HIT,

    
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_TT_2_SEM_A_KEY,
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_TT_2_SEM_A_RES,
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_TT_2_SEM_A_HIT,

    
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_TT_2_SEM_B_KEY,
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_TT_2_SEM_B_RES,
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_TT_2_SEM_B_HIT,

    
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_TT_2_TCAM_KEY,
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_TT_2_TCAM_RES,
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_TT_2_TCAM_HIT,

    
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_RIF,
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_RIF_PROFILE,

    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_NOF_TYPES

}SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE;

uint32 arad_pp_signal_mngr_signal_get(int unit, int core_id, SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE signal_id, uint32 val[8], int* size_of_signal_in_bits);

uint32 arad_pp_signal_mngr_signal_print(int unit, int core_id, SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE signal_id);

uint32 arad_pp_signal_mngr_signal_ids_get(
           int unit,
           SOC_DPP_DBAL_PHYSICAL_DB_TYPES physical_db_type,
           int lookup_number,
           SOC_PPC_FP_DATABASE_STAGE  stage,
           SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE key_signal_id[5],
           SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE* res_signal_id,
           SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE* hit_signal_id,
           SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE* opcode_signal_id,
           int* nof_key_signals);

#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif
