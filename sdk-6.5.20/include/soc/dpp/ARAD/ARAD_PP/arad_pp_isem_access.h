/* 
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $Copyright: (c) 2013 Broadcom Corporation All Rights Reserved.$
 * $
*/

#ifndef __ARAD_PP_ISEM_ACCESS_INCLUDED__

#define __ARAD_PP_ISEM_ACCESS_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_ip_tcam.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_dbal.h>

#include <soc/dpp/ARAD/arad_tcam_mgmt.h>








#define ARAD_PP_ISEM_ACCESS_LSB_PREFIX                              (SOC_IS_JERICHO(unit)? 44: 37)
#define ARAD_PP_ISEM_ACCESS_MSB_PREFIX                              (SOC_IS_JERICHO(unit)? 49: 40)
#define ARAD_PP_ISEM_ACCESS_NOF_BITS_PREFIX                         (ARAD_PP_ISEM_ACCESS_MSB_PREFIX - ARAD_PP_ISEM_ACCESS_LSB_PREFIX + 1)

#define ARAD_PP_ISEM_ACCESS_KEY_SIZE                                  (2)

#define ARAD_PP_ISEM_ACCESS_NOF_TABLES                                (3)
#define ARAD_PP_ISEM_ACCESS_ID_ISEM_A                                 (0)
#define ARAD_PP_ISEM_ACCESS_ID_ISEM_B                                 (1)
#define ARAD_PP_ISEM_ACCESS_ID_TCAM                                   (2)

#define DPP_PP_ISEM_PROG_SEL_1ST_CAM_DEF_OFFSET(unit)                (SOC_DPP_DEFS_GET(unit, nof_vtt_program_selection_lines) - 6)
#define DPP_PP_ISEM_PROG_SEL_1ST_CAM_USR_LAST_ENTRY(unit)            (DPP_PP_ISEM_PROG_SEL_1ST_CAM_DEF_OFFSET(unit) -1)


#define ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_NONE                             (0)
#define ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_COUPLING                         (1)
#define ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_VRRP                             (2)
#define ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_IPV6_CASCADE                     (3)
#define ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_3MTSE_PLUS_FRR_TCAM              (4)
#define ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_3MTSE_PLUS_FRR_L4B_AND_L5G       (5)
#define ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_3MTSE_PLUS_FRR_NO_MPLS           (6)
#define ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_2MTSE_PLUS_FRR_TCAM              (7)


#define ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_NONE                             (0)
#define ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_MAC_IN_MAC                       (1)
#define ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_MAC_IN_MAC_MC_FALLBACK           (2)
#define ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_FC                               (3)
#define ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_L2TP                             (3) 
#define ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_PWEoGRE                          (4)
#define ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_PPPoE                            (5)
#define ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_MYMAC_IP_DISABLED_TRAP           (6)
#define ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_MPLS_EXP_NULL                    (7)



#define ARAD_PP_ISEM_ACCESS_TCAM_DEF_PREFIX_SIZE  (4)
#define ARAD_PP_ISEM_ACCESS_TCAM_DEF_MIN_BANKS    (0)



#define ARAD_PP_ISEM_ACCESS_L2_PON_TLS                                         (0x1)


#define ARAD_PP_ISEM_ACCESS_TRILL_VSI_VL                                       (0x1)
#define ARAD_PP_ISEM_ACCESS_TRILL_VSI_FGL                                      (0x2)
#define ARAD_PP_ISEM_ACCESS_TRILL_VSI_TTS                                      (0x4)


#define ARAD_PP_ISEM_ACCESS_NAMESPACE_TO_PREFIX_WO_ELI(mpls_namespace) \
  ((mpls_namespace == SOC_PPC_MPLS_TERM_NAMESPACE_L1) ? ARAD_PP_ISEM_ACCESS_MPLS_L1_PREFIX: \
  (mpls_namespace == SOC_PPC_MPLS_TERM_NAMESPACE_L2) ? ARAD_PP_ISEM_ACCESS_MPLS_L2_PREFIX: \
  (mpls_namespace == SOC_PPC_MPLS_TERM_NAMESPACE_L3) ? ARAD_PP_ISEM_ACCESS_MPLS_L3_PREFIX: \
  (mpls_namespace == SOC_PPC_MPLS_TERM_NAMESPACE_L1_L3) ? ARAD_PP_ISEM_ACCESS_MPLS_L1L3_PREFIX:ARAD_PP_ISEM_ACCESS_MPLS_L1L2_PREFIX)

#define ARAD_PP_ISEM_ACCESS_NAMESPACE_TO_PREFIX_WO_ELI_4_bit(mpls_namespace) \
  ((mpls_namespace == SOC_PPC_MPLS_TERM_NAMESPACE_L1) ? ARAD_PP_ISEM_ACCESS_MPLS_L1_PREFIX_4bit: \
  (mpls_namespace == SOC_PPC_MPLS_TERM_NAMESPACE_L2) ? ARAD_PP_ISEM_ACCESS_MPLS_L2_PREFIX_4bit: \
  (mpls_namespace == SOC_PPC_MPLS_TERM_NAMESPACE_L3) ? ARAD_PP_ISEM_ACCESS_MPLS_L3_PREFIX_4bit: \
  (mpls_namespace == SOC_PPC_MPLS_TERM_NAMESPACE_L1_L3) ? ARAD_PP_ISEM_ACCESS_MPLS_L1L3_PREFIX_4bit:ARAD_PP_ISEM_ACCESS_MPLS_L1L2_PREFIX_4bit)

#define ARAD_PP_ISEM_ACCESS_NAMESPACE_TO_PREFIX_ELI(mpls_namespace) \
  ((mpls_namespace == SOC_PPC_MPLS_TERM_NAMESPACE_L1) ? ARAD_PP_ISEM_ACCESS_MPLS_L1ELI_PREFIX: \
  (mpls_namespace == SOC_PPC_MPLS_TERM_NAMESPACE_L2) ? ARAD_PP_ISEM_ACCESS_MPLS_L2ELI_PREFIX: \
  (mpls_namespace == SOC_PPC_MPLS_TERM_NAMESPACE_L1_L3) ? ARAD_PP_ISEM_ACCESS_MPLS_L1L3ELI_PREFIX:ARAD_PP_ISEM_ACCESS_MPLS_L1L2ELI_PREFIX)

#define ARAD_PP_ISEM_ACCESS_NAMESPACE_TO_PREFIX(mpls_namespace,is_eli) \
  ((is_eli) ? ARAD_PP_ISEM_ACCESS_NAMESPACE_TO_PREFIX_ELI(mpls_namespace):ARAD_PP_ISEM_ACCESS_NAMESPACE_TO_PREFIX_WO_ELI(mpls_namespace))

#define ARAD_PP_VTT_FIND_NAMESPACE_L1(namespace) \
  (namespace == SOC_PPC_MPLS_TERM_NAMESPACE_L1 || namespace == SOC_PPC_MPLS_TERM_NAMESPACE_L1_L3 \
   || namespace == SOC_PPC_MPLS_TERM_NAMESPACE_L1_L2)

#define ARAD_PP_VTT_FIND_NAMESPACE_L2(namespace) \
  (namespace == SOC_PPC_MPLS_TERM_NAMESPACE_L2 || namespace == SOC_PPC_MPLS_TERM_NAMESPACE_L1_L2)

#define ARAD_PP_VTT_FIND_NAMESPACE_L3(namespace) \
  (namespace == SOC_PPC_MPLS_TERM_NAMESPACE_L3 || namespace == SOC_PPC_MPLS_TERM_NAMESPACE_L1_L3)


typedef enum
{
    
    PROG_VT_TM_PROG,
     
    PROG_VT_VDxINITIALVID,
    
    PROG_VT_VDxOUTERVID,
    
    PROG_VT_VDxINITIALVID_L1,
    
    PROG_VT_VDxOUTERVID_L1,
    
    PROG_VT_VD_OUTER_INNER,
    
    PROG_VT_HIGH_VD_OUTER_INNER_OR_LOW_VD_OUTER,
    
    PROG_VT_VDxINITIALVID_L1_L2ELI,
    
    PROG_VT_VDxOUTERVID_L1_L2ELI,
    
    PROG_VT_VD_INITIALVID_TRILL,
    PROG_VT_VD_OUTER_TRILL,
    PROG_VT_VD_OUTER_INNER_TRILL,
    
    PROG_VT_VD_INITIALVID_L1FRR,
    
    PROG_VT_VD_OUTERVID_L1FRR,
    
    PROG_VT_VD_OUTER_OR_VD_OUTER_PCP,
    
    PROG_VT_VD_OUTER_INNER_OR_VD_OUTER_INNER_OUTERPCP,
    
    PROG_VT_HIGH_VD_OUTER_INNER_OUTERPCP_OR_VD_OUTER_INNER_OR_VD_OUTER,
    
    PROG_VT_VD_INITIALVID_OR_VD_INITIALVID,
    
    PROG_VT_VD_OUTER_OR_VD_OUTER,
    
    PROG_VT_VD_OUTER_INNER_OR_VD_OUTER_INNER,
    
    PROG_VT_INDX_VD_INITIALVID_L1,
    
    PROG_VT_INDX_VD_OUTERVID_L1,
    
    PROG_VT_INDX_MPLS_PORT_L1,
    
    PROG_VT_INDX_VD_INITIALVID_FRR_L1,
    
    PROG_VT_INDX_VD_OUTERVID_FRR_L1,
    
    PROG_VT_INDX_VD_INITIALVID_L3,
    
    PROG_VT_INDX_VD_OUTERVID_L3,
    
    PROG_VT_PON_UNTAGGED,
    
    PROG_VT_PON_ONE_TAG,
    
    PROG_VT_PON_TWO_TAGS,
    
    PROG_VT_PON_TWO_TAGS_VS_TUNNEL_ID,
    
    PROG_VT_IPV4_INITIAL_VID,
    
    PROG_VT_IPV4_OUTER_VID,
    
    PROG_VT_IPV4_PORT,
    
    PROG_VT_EVB_S_TAG,
    
    PROG_VT_EVB_UN_C_TAG,
    
    PROG_VT_DOUBLE_TAG_INITIAL_VID,
    
    PROG_VT_DOUBLE_TAG_PRIORITY_INITIAL_VID,
    
    PROG_VT_VD_DESIGNATED_VID_TRILL,
    
    PROG_VT_DOUBLE_TAG_5_IP_TUPLE_Q_IN_Q,
    
    PROG_VT_SINGLE_TAG_5_IP_TUPLE_Q_IN_Q,
    
    PROG_VT_IPV4_INITIAL_VID_AFTER_RECYCLE,
    
    PROG_VT_IPV4_OUTER_VID_AFTER_RECYCLE,
	
    PROG_VT_VD_INITIALVID,
	
    PROG_VT_VD_SINGLE_TAG,
	
    PROG_VT_VD_DOUBLE_TAG,
    
    PROG_VT_VD_INITIALVID_VRRP,
    
    PROG_VT_VD_SINGLE_TAG_VRRP,
    
    PROG_VT_VD_DOUBLE_TAG_VRRP,
	
    PROG_VT_VD_INITIALVID_EXPLICIT_NULL,
	 
    PROG_VT_VD_SINGLE_TAG_EXPLICIT_NULL,
	
    PROG_VT_VD_DOUBLE_TAG_EXPLICIT_NULL,

    PROG_VT_OUTER_INNER_PCP_1_TST2,
    PROG_VT_OUTER_INNER_PCP_2_TST2,
    PROG_VT_OUTER_PCP_1_TST2,
    PROG_VT_OUTER_PCP_2_TST2,
    PROG_VT_OUTER_INNER_1_TST2,
    PROG_VT_OUTER_INNER_2_TST2,
    PROG_VT_OUTER_1_TST2,
    PROG_VT_OUTER_2_TST2,
    PROG_VT_UNTAGGED_TST2,
    PROG_VT_TEST2,
    PROG_VT_EXTENDER_PE,
    PROG_VT_EXTENDER_PE_UT,
    PROG_VT_CUSTOM_PP_PORT_TUNNEL,
    PROG_VT_VD_INITIALVID_VLAN_DOMAIN_MPLS_L1, 
    
    PROG_VT_SECTION_OAM_ONE_TAG,
    
    PROG_VT_SECTION_OAM_TWO_TAGS,

    PROG_VT_INDX_VD_MPLS4_INITIALVID_L1L2,
    PROG_VT_INDX_VD_MPLS4_OUTERVID_L1L2,
    
    PROG_VT_IPV6_INITIAL_VID,
    
    PROG_VT_IPV6_OUTER_VID,
    
    PROG_VT_IPV6_INITIAL_VID_AFTER_RECYCLE,
    
    PROG_VT_IPV6_OUTER_VID_AFTER_RECYCLE,
    
    PROG_VT_GENERALIZED_RCH0,
    
    PROG_VT_GENERALIZED_RCH1,
    
	PROG_VT_3MTSE_PLUS_FRR_INITIALVID_L2_L1,
	
	PROG_VT_3MTSE_PLUS_FRR_OUTERVID_L2_L1,
	
	PROG_VT_3MTSE_PLUS_FRR_VDxINITIALVID_L2_L4B_L5GAL,
	
	PROG_VT_3MTSE_PLUS_FRR_VDxOUTERVID_L2_L4B_L5GAL,
	
	PROG_VT_2MTSE_PLUS_FRR_VDxINITIALVID_FRR,
	
	PROG_VT_2MTSE_PLUS_FRR_VDxOUTERVID_FRR,
	
	PROG_VT_2MTSE_PLUS_FRR_VDxOUTER_INNER_VID_FRR,
	
	PROG_VT_2MTSE_PLUS_FRR_VDxOUTER_INNER_PCP_VID_FRR,
    
    PROG_VT_NOF_PROGS
} PROG_VT_PROGRAMS;

typedef enum
{
    
    PROG_TT_TM_PROG,
    
    PROG_TT_L2,
    
    PROG_TT_L3,
    
    PROG_TT_L2_L3ELI,
    
    PROG_TT_ARP_2ND_PROG,
    
    PROG_TT_BRIDGE_STAR_2ND_PROG,
    
    PROG_TT_IPV4_ROUTER_DIP_FOUND_PROG,
    
    PROG_TT_IPV4_ROUTER_PORT_VXLAN_PROG,
    
    PROG_TT_IPV4_ROUTER_PORT_L2_GRE_PROG,
    
    PROG_TT_IPV4_ROUTER_PWE_GRE_DIP_FOUND_PROG,
    
    PROG_TT_IPV4_ROUTER_MC_DIP_NOT_FOUND_PROG,
    
    PROG_TT_IPV4_ROUTER_UC_DIP_NOT_FOUND_PROG,
    
    PROG_TT_IPV4_ROUTER_DIP_FOUND_PROG_IPMC_RCY,
    
    PROG_TT_IP_ROUTER_2ND_PROG,
    
    PROG_TT_IPV6_ROUTER_2ND_PROG,
    
    PROG_TT_IP_ROUTER_CompatibleMc_2ND_PROG,
    
    PROG_TT_UNKNOWN_L3_PROG,
    
    PROG_TT_MAC_IN_MAC_WITH_BTAG_PROG,
    
    PROG_TT_MAC_IN_MAC_MC_PROG,
    
    PROG_TT_FC_WITH_VFT_PROG,
    
    PROG_TT_FC_PROG,
    
    
    PROG_TT_INDX_L1_L2_INRIF,
    
    PROG_TT_INDX_L1_L2_L3ELI_INRIF,
    
    PROG_TT_INDX_L1_L2ELI_INRIF,
    
    
    PROG_TT_INDX_L3_L2,
     
    PROG_TT_INDX_MPLS_PORT_L3_L2, 
    
    PROG_TT_INDX_L3_L4,
    
    PROG_TT_INDX_L2_L3ELI,
    
    
    PROG_TT_INDX_L1_L1,
    
    PROG_TT_INDX_L1_L1_GAL,
    
    PROG_TT_INDX_L1_L2,
    
    PROG_TT_INDX_L1_L2_GAL,
    
    PROG_TT_INDX_L2_L2,
    
    PROG_TT_INDX_L2_L3_GAL,
    
    PROG_TT_MLDP_AFTER_RECYCLE,
    
    PROG_TT_INDX_AS_ORDER_L2_L3,
    
    PROG_TT_INDX_AS_ORDER_L2_L2,
    
    PROG_TT_INDX_AS_ORDER_L1_L2,
    
    PROG_TT_INDX_AS_ORDER_L1_L1,
    
    PROG_TT_INDX_AS_ORDER_NOP,
    
    PROG_TT_INDX_MPLS4_L3_L4,
    
    PROG_TT_INDX_MPLS4_NOP,

    
    
    PROG_TT_TRILL_2ND_PROG,
    PROG_TT_TRILL_2ND_PROG_TWO_VLANS_AT_NATIVE_ETH,
    PROG_TT_TRILL_2ND_PROG_TRILL_TRAP_PROG,
    PROG_TT_DIP6_COMPRESSION_PROG,
    
    PROG_TT_OAM_STAT_PROG,
    PROG_TT_BFD_STAT_PROG,
    
    PROG_TT_EXTENDER_UNTAG_CHECK_IP_MC,
    
    PROG_TT_EXTENDER_UNTAG_CHECK_IP_UC,
    
    PROG_TT_EXTENDER_UNTAG_CHECK,
    
    PROG_TT_IPV4_ROUTER_L3_GRE_PORT_PROG,
    
    PROG_TT_IPV4_ROUTER_DIP_SIP_VRF_PROG,
    
    PROG_TT_IPV4_FOR_EXPLICIT_NULL_PROG,
    
    PROG_TT_TUNNEL_FOR_TWO_EXPLICIT_NULL_PROG,
    
    PROG_TT_IP_DISABLE_FOR_EXPLICIT_NULL_PROG,
    
    PROG_TT_IPV4_DISABLE_FOR_EXPLICIT_NULL_PROG,
    
    PROG_TT_IPV6_DISABLE_FOR_EXPLICIT_NULL_PROG,
    
    PROG_TT_EXTENDER_IP_MC_PROG,
    
    PROG_TT_EXTENDER_IP_UC_PROG,
    
    PROG_TT_EXTENDER_PROG,
    
    PROG_TT_INDX_L1_GAL_ONLY,
    
    PROG_TT_INDX_L2_GAL_ONLY,
    
    PROG_TT_VLAN_DOMAIN_MPLS_L2,
    
    PROG_TT_IPV6_ROUTER_COMP_DEST_FOUND,
    
    PROG_TT_IPV6_ROUTER_2ND_PROG_AFTER_RECYCLE,
    
    PROG_TT_IPV6_ROUTER_COMP_DEST_FOUND_AFTER_RECYCLE,
    
    PROG_TT_PPPOE_PROG,
	
	PROG_TT_L2TP,
    
    PROG_TT_GENERALIZED_RCH_MPLS_PROG,
    
    PROG_TT_MYMAC_IP_DISABLED_TRAP_PROG,
	
	PROG_TT_3MTSE_PLUS_FRR_L1_L1,
	
    PROG_TT_3MTSE_PLUS_FRR_TFL_L3_L3,
	
	PROG_TT_3MTSE_PLUS_FRR_L3_L1,
	
    PROG_TT_3MTSE_PLUS_FRR_TFL_L4_L3,
	
	PROG_TT_3MTSE_PLUS_FRR_L1,
	
	PROG_TT_3MTSE_PLUS_FRR_L2_L1,
	
	PROG_TT_2MTSE_PLUS_FRR_L1_L1,
	
	PROG_TT_2MTSE_PLUS_FRR_TFL_L2_L2,
	
	PROG_TT_2MTSE_PLUS_FRR_L2_L1,
	
	PROG_TT_2MTSE_PLUS_FRR_TFL_L3_L2,
    
    PROG_TT_NOF_PROGS
} PROG_TT_PROGRAMS;


#define ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES 20
#define ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_LONGS (ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES / sizeof(uint32))









typedef enum
{
  
  ARAD_PP_ISEM_ACCESS_GET_PROCS_PTR = ARAD_PP_PROC_DESC_BASE_ISEM_ACCESS_FIRST,
  ARAD_PP_ISEM_ACCESS_GET_ERRS_PTR,
  

  
  ARAD_PP_ISEM_ACCESS_PROCEDURE_DESC_LAST
} ARAD_PP_ISEM_ACCESS_PROCEDURE_DESC;

typedef enum
{
  
  
  ARAD_PP_ISEM_ACCESS_UNKNOWN_KEY_TYPE_ERR = ARAD_PP_ERR_DESC_BASE_ISEM_ACCESS_FIRST,
  ARAD_PP_ISEM_ACCESS_NOT_READY_ERR,
  ARAD_PP_ISEM_ACCESS_MPLS_IN_RIF_NOT_SUPPORTED_ERR,  
  ARAD_PP_ISEM_ACCESS_LABEL_INDEX_NOT_SUPPORTED_ERR,
  ARAD_PP_ISEM_ACCESS_LABEL_INDEX_OUT_OF_RANGE_ERR,
  ARAD_PP_ISEM_ACCESS_PROG_SEL_INCORRECT_DEF_ALLOCATION_ERR,
  ARAD_PP_ISEM_ACCESS_PROG_SEL_USR_INDEX_OUT_OF_RANGE_ERR,
  ARAD_PP_ISEM_ACCESS_PROG_SEL_INVALID_LIF_KEY_RANGE_ERR,
  ARAD_PP_ISEM_ACCESS_AC_KEY_SET_NOT_SUPPORTED_ERR,
  ARAD_PP_ISEM_ACCESS_SOC_PROPERTIES_ERR,
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_TCAM_INVALID_ERR,
  ARAD_PP_ISEM_ACCESS_PROGRAMS_FULL_ERR,

  
  ARAD_PP_ISEM_ACCESS_ERR_LAST
} ARAD_PP_ISEM_ACCESS_ERR;

typedef enum
{
  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD = 0,                                
  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD_VID,                             
  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD_VID_VID,                         
  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD_INITIAL_VID,                     
  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD_PCP_VID,                         
  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD_PCP_VID_VID,   
  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_PON_VD_TUNNEL,
  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_PON_VD_TUNNEL_VID,
  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_PON_VD_TUNNEL_VID_VID,
  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_PON_VD_ETHERTYPE_TUNNEL_PCP_VID_VID,
  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS,                               
  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L1,                            
  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L1_IN_RIF,                     
  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L2,                            
  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L3,                            
  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_FRR,                           
  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_ELI,                           
  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L1_ELI,                        
  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L2_ELI,                       
  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L1_IN_RIF_ELI,                
  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_PBB,                               
  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_IP_TUNNEL_SIP,
  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_IP_TUNNEL_DIP,                     
  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_IP_TUNNEL_SIP_DIP,
  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_TRILL_NICK,  
  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_TRILL_APPOINTED_FORWARDER,
  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_TRILL_NATIVE_INNER_TPID,
  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_TRILL_VSI,
  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_FCOE,                              
  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_SPOOF_IPV4,                        
  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_SPOOF_IPV6,                         
  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_EoIP,                               
  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_GRE,                                
  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_IPV6_TUNNEL_DIP, 
  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_IPV6_TUNNEL_CASCADE_DIP,
  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_IPV6_TUNNEL_CASCADE_SIP,
  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_IPV6_TUNNEL_DIP_AFTER_RECYCLE, 
  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_IPV6_TUNNEL_CASCADE_DIP_AFTER_RECYCLE,
  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_VNI,  
  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_RPA,  
  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_TRILL_DESIGNATED_VID,
  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_FLEXIBLE_Q_IN_Q_SINGLE_TAG, 
  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_FLEXIBLE_Q_IN_Q_DOUBLE_TAG, 
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_TST_INITIAL_TAG, 
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_TST_COMPRESSED_TAG, 
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_TST_UNTAG, 
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_TST_DOUBLE_TAG,                          
  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_PWE_L2_GAL,   
      
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_EXPLICIT_NULL,
  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_DIP6_TCAM_COMPRESSION,
  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_SPOOF_SIP6_TCAM_COMPRESSION,
  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_OAM_STAT_TT,
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_OAM_STAT_VT,
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_BFD_STAT_VT,
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_BFD_STAT_TT_ISA,
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_BFD_STAT_TT_ISB,
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_SRC_PORT_DA_DB_TCAM,
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_VSI_DA_DB_VRRP_TCAM,
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_AC_TST2,
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_TST2_TCAM,
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_TEST2_TCAM,
  ARAD_PP_ISEM_ACCESS_KEY_TEST2,
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_EXTENDER_UNTAG_CHECK,
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_EXTENDER_PE,
  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_IP_TUNNEL_SIP_DIP_VRF,
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_IP_TUNNEL_MY_VTEP_INDEX_SIP_VRF,
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_IPV4_MATCH_VT_DB_PROFILE,
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_EFP,
  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_CUSTOM_PP_PORT_TUNNEL,
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_CUSTOM_PP_LSP_ECMP,
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_VLAN_DOMAIN_MPLS,
  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_SECTION_OAM_PORT_ETH_VID,
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_SECTION_OAM_PORT_ETH_VID_VID,

   
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L0,
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_L4B_L5G,

  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_L2TP_TUNNEL_SESSION,

  
  ARAD_PP_ISEM_ACCESS_KEY_TYPE_PPPOE_SESSION,

  ARAD_PP_ISEM_ACCESS_NOF_KEY_TYPES
} ARAD_PP_ISEM_ACCESS_KEY_TYPE;


  typedef struct arad_pp_isem_access_l2_eth_s
  {
    uint32 vlan_domain;
    uint32 outer_vid; 
    uint32 inner_vid;
    uint32 outer_pcp;
    uint32 outer_dei; 
    int    core_id;
  } arad_pp_isem_access_l2_eth_t;

  typedef struct arad_pp_isem_access_mpls_s
  {
    uint32 in_rif;  
    uint32 label;
    uint32 label2; 
    uint32 is_bos; 
    uint32 flags;  
    uint32 vlan_domain;
	uint32 ttl;
  } arad_pp_isem_access_mpls_t;

  typedef struct arad_pp_isem_access_ip_tunnel_s
  {
    uint32 dip;
    uint32 sip;    
    uint32 sip_prefix_len;
    uint32 dip_prefix_len;    
    SOC_SAND_PP_IPV6_SUBNET dip6;
    SOC_SAND_PP_IPV6_SUBNET sip6;
    uint32 ipv4_next_protocol;
    uint32 ipv4_next_protocol_prefix_len;
    uint32 port_property; 
    uint8  port_property_en; 
    uint32 flags; 
    uint32 vrf; 
    uint32 vrf_prefix_len; 
    uint32 my_vtep_index; 
    uint32 gre_ethertype;  
    uint32 gre_ethertype_len; 
    uint32 ip_gre_tunnel; 
    uint32 ip_gre_tunnel_en; 
    uint32 qualifier_next_protocol; 
    uint32 qualifier_next_protocol_prefix_len; 
  } arad_pp_isem_access_ip_tunnel_t;

  typedef struct arad_pp_isem_access_trill_s
  {
    uint32 nick_name;
    uint32 ing_nick;
    uint32 dist_tree;
    uint32 link_adj_id;
    uint32 native_inner_tpid;
    uint32 port;
    uint32 vsi;
    uint32 high_vid;
    uint32 low_vid;
    uint32 flags;
  } arad_pp_isem_access_trill_t;

  typedef struct arad_pp_isem_access_pbb_s
  {
    uint8  bsa_nickname_valid;
    uint32 bsa_nickname;
    uint32 isid_domain;
    uint32 isid;
  } arad_pp_isem_access_pbb_t;

  typedef struct arad_pp_isem_access_l2_gre_s
  {
    uint32 gre_key;
    uint32 match_port_class;
  } arad_pp_isem_access_l2_gre_t;

  typedef struct arad_pp_isem_access_l2_vni_s
  {
    uint32 vni_key;
    uint32 match_port_class;
  } arad_pp_isem_access_l2_vni_t;

  typedef struct arad_pp_isem_access_fcoe_s
  {
    uint32 d_id; 
    uint32 vsan; 
  } arad_pp_isem_access_fcoe_t;

  
  typedef struct arad_pp_isem_access_l2_pon_s
  {
    uint32 port; 
    uint8  port_valid;
    uint32 outer_vid; 
    uint8  outer_vid_valid;
    uint32 inner_vid;
    uint8  inner_vid_valid;
    uint32 outer_pcp;
    uint8  outer_pcp_valid;
    uint32 outer_dei; 
    uint8  outer_dei_valid; 
    uint32 ether_type;
    uint8  ether_type_valid;
    uint32 tunnel_id;
    uint8  tunnel_id_valid;
    uint32 flags;
	int    core;
  } arad_pp_isem_access_l2_pon_t;
  
  
  typedef struct arad_pp_isem_access_section_oam_s
  {
    uint32 port; 
    uint8  port_valid;
    uint32 outer_vid;
    uint8  outer_vid_valid;
    uint32 inner_vid;
    uint8  inner_vid_valid;
    uint32 ether_type;
    uint8  ether_type_valid;
    uint32 flags;
    int    core;
  } arad_pp_isem_access_section_oam_t;

  typedef struct arad_pp_isem_access_spoof_v4_s
  {
    SOC_SAND_PP_MAC_ADDRESS smac; 
    uint32 sip;     
  } arad_pp_isem_access_spoof_v4_t;

  typedef struct arad_pp_isem_access_spoof_v6_s
  {
    SOC_SAND_PP_MAC_ADDRESS smac; 
    SOC_SAND_PP_IPV6_SUBNET sip6;
    uint32 vsi;
  } arad_pp_isem_access_spoof_v6_t;

  typedef struct arad_pp_isem_access_rpa_s
  {
    uint32 dip; 
    uint32 vrf; 
  } arad_pp_isem_access_rpa_t;

  typedef struct arad_pp_isem_access_ip6_compression_s
  {
    SOC_SAND_PP_IPV6_SUBNET ip6;
  } arad_pp_isem_access_ip6_compression_t;

  typedef struct arad_pp_isem_access_oam_stat_s
  {
    uint32 cfm_eth_type;
    uint32 opaque;
    uint32 bfd_format;
    uint32 pph_type;
  } arad_pp_isem_access_oam_stat_t;
   
   typedef struct arad_pp_isem_access_pppoe_s
   {
   	 uint32 in_lif;
	 uint16 pppoe_session;
   } arad_pp_isem_access_pppoe_t;
	
	typedef struct arad_pp_isem_access_l2tp_s
	{
	  uint32 vrf;
	  uint32 l2tp_tunnel_session;
	} arad_pp_isem_access_l2tp_t;
	 
typedef union
{
  
  arad_pp_isem_access_l2_eth_t l2_eth;
  arad_pp_isem_access_mpls_t mpls;
  arad_pp_isem_access_ip_tunnel_t ip_tunnel;
  arad_pp_isem_access_trill_t trill;
  arad_pp_isem_access_pbb_t pbb;
  arad_pp_isem_access_l2_gre_t l2_gre;
  arad_pp_isem_access_l2_vni_t l2_vni;
  arad_pp_isem_access_fcoe_t fcoe;
  arad_pp_isem_access_l2_pon_t l2_pon;
  arad_pp_isem_access_spoof_v4_t spoof_v4;
  arad_pp_isem_access_spoof_v6_t spoof_v6;
  arad_pp_isem_access_rpa_t rpa;
  arad_pp_isem_access_ip6_compression_t ip6_compression;
  arad_pp_isem_access_oam_stat_t oam_stat;
  arad_pp_isem_access_section_oam_t section_oam;
  arad_pp_isem_access_pppoe_t pppoe;
  arad_pp_isem_access_l2tp_t l2tp;
} ARAD_PP_ISEM_ACCESS_KEY_INFO;

typedef struct
{
  ARAD_PP_ISEM_ACCESS_KEY_TYPE key_type;
  ARAD_PP_ISEM_ACCESS_KEY_INFO key_info;
} ARAD_PP_ISEM_ACCESS_KEY;

typedef struct
{
  uint32 sem_result_ndx;

} ARAD_PP_ISEM_ACCESS_ENTRY;

#define ARAD_PP_ISEM_ACCESS_NOF_LABEL_HANDLES 3

#define ARAD_PP_SW_DB_VLAN_DOMAINS_NOF_U32  (SOC_DPP_DEFS_NOF_VLAN_DOMAINS_MAX/32)

typedef struct
{
  SHR_BITDCL pon_double_lookup_enable[ARAD_PP_SW_DB_VLAN_DOMAINS_NOF_U32];
} ARAD_PON_DOUBLE_LOOKUP;

typedef struct
{
  uint8 mpls_use_in_rif;
  uint8 port_vlan_pcp_lookup;
  uint8 match_port_vlan_critiria_64K;
  uint8 mpls_index;
  uint8 spoof_enable;
  uint8 trill_mode; 
  uint8 trill_appointed_fwd; 
  uint8 ipv6_term_enable;
  uint8 ipv4_term_enable;
  uint8 ipv4_term_dip_sip_enable; 
  uint8 ipv6_term_dip_sip_enable; 
  uint8 e_o_ip_enable;
  uint8 vxlan_enable;
  uint8 nvgre_enable;
  uint8 spoof_ipv6_enable;
  uint8 fast_reroute_labels_enable;
  uint8 is_bos_in_key_enable;
  uint8 pon_enable; 
  uint8 use_pon_tcam_lkup; 
  uint8 tls_db_enable; 
  uint8 custom_pon_enable; 
  uint8 tls_in_tcam_enable; 
  uint8 evb_enable; 
  uint8 fcoe_enable; 
  uint8 ipv4mc_bidir_enable; 
  uint8 mim_enable; 
  uint8 eli_enable; 
  uint8 mpls_1_namespace;  
  uint8 mpls_2_namespace;  
  uint8 mpls_3_namespace;  
  uint8 mpls_1_database;   
  uint8 mpls_2_database;   
  uint8 mpls_3_database;   
  uint8 mpls_tp_mac_address; 
  uint8 trill_disable_designated_vlan_check; 
  uint8 designated_vlan_inlif_enable;
  uint8 q_in_q_ip_5_tuple; 
  uint8 custom_feature_vt_tst1; 
  uint8 custom_feature_vt_tst2; 
  uint8 custom_feature_vrrp_scaling_tcam; 
  uint8 vrrp_scaling_tcam; 
  uint8 ingress_full_mymac_1; 
  uint8 tunnel_termination_in_tt_only; 
  uint8 tunnel_termination_ordered_by_index; 
  uint8 pwe_gal_support; 
  uint8 trill_transparent_service; 
  uint8 explicit_null_arad_plus_support; 
  uint8 port_raw_mpls; 
  uint8 compression_spoof_ip6_enable;
  uint8 compression_ip6_enable;
  uint8 test1;
  uint8 extender_cb_enable;     
  uint8 evpn_enable;
  uint8 custom_pon_ipmc; 
  uint8 oam_enable; 
  uint8 oam_statistics_enable; 
  uint8 section_oam_enable; 
}ARAD_PP_ISEM_ACCESS_PROGRAMS_SOC_PROPERTIES;

typedef struct
{
  SOC_SAND_HASH_TABLE_PTR isem_key_to_entry_id;
  int vt_programs[PROG_VT_NOF_PROGS];
  int tt_programs[PROG_TT_NOF_PROGS];
} ARAD_VTT;





extern ARAD_PP_ISEM_ACCESS_PROGRAMS_SOC_PROPERTIES g_prog_soc_prop[BCM_MAX_NUM_UNITS];
extern int   is_g_prog_soc_prop_initilized[BCM_MAX_NUM_UNITS];

extern int    is_db_in_use[SOC_DPP_DBAL_SW_TABLE_ID_VTT_LAST - SOC_DPP_DBAL_SW_TABLE_ID_VTT_FIRST + 1];



uint32 arad_pp_isem_access_programs_soc_properties_get(int unit);
uint32 arad_pp_isem_prog_programs_init(int unit);

uint32 arad_pp_vtt_1st_lookup_program_swap_sem(
    int unit,
    uint32 swap,
    ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION_TBL_DATA* tbl_data
);

void 
arad_pp_isem_access_deinit(int unit);
uint32
  arad_pp_isem_access_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );

uint32
  arad_pp_isem_access_entry_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_ISEM_ACCESS_KEY             *isem_key,
    SOC_SAND_OUT ARAD_PP_ISEM_ACCESS_ENTRY           *isem_entry,
    SOC_SAND_OUT uint8                               *success
  );

uint32
  arad_pp_isem_access_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_ISEM_ACCESS_KEY             *isem_key,
    SOC_SAND_IN  ARAD_PP_ISEM_ACCESS_ENTRY           *isem_entry,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE            *success
  );

uint32
  arad_pp_isem_access_isem_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 *key_buffer,
    SOC_SAND_IN  uint32                                 *entry_buffer,
    SOC_SAND_IN  uint32                                 tables_access_id,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE               *success
  );

uint32 
    arad_pp_isem_access_isem_entry_remove_unsafe(int unit, uint32 key_buffer[ARAD_PP_ISEM_ACCESS_KEY_SIZE], uint32 tables_access_id);

uint32 
    arad_pp_isem_access_isem_entry_get_unsafe(int unit, uint32 key_buffer[ARAD_PP_ISEM_ACCESS_KEY_SIZE], uint32 entry_buffer[SOC_DPP_DEFS_MAX(ISEM_PAYLOAD_NOF_UINT32)], uint32 tbl_access_id, uint8* found);

uint32
  arad_pp_isem_access_entry_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_ISEM_ACCESS_KEY             *isem_key
  );

uint32 arad_pp_isem_prefix_from_buffer(SOC_SAND_IN  int       unit,
                                       SOC_SAND_IN  uint32   *buffer,
                                       SOC_SAND_IN  uint32    lookup_num,
                                       SOC_SAND_IN  uint32    tables_access_id,
                                       SOC_SAND_OUT uint32   *prefix );

uint32
  arad_pp_isem_access_prefix_to_key_type(SOC_SAND_IN int                           unit,
                                         SOC_SAND_IN uint32                        isem_prefix,
                                         SOC_SAND_IN uint32                        lookup_num,
                                         SOC_SAND_IN uint32                        tables_access_id,
                                         SOC_SAND_IN uint32                        *buffer,
                                         SOC_SAND_OUT ARAD_PP_ISEM_ACCESS_KEY_TYPE *isem_key_in);

uint32
   arad_pp_isem_access_tcam_callback(
      SOC_SAND_IN int unit,
      SOC_SAND_IN uint32  user_data
    );

uint32
  arad_pp_isem_access_key_from_buffer(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 *buffer,
    SOC_SAND_IN  uint32                                 lookup_num,
    SOC_SAND_IN  uint32                                 tables_access_id,
    SOC_SAND_OUT ARAD_PP_ISEM_ACCESS_KEY                *isem_key
  );

uint32
  arad_pp_isem_access_entry_from_buffer(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                    *buffer,
    SOC_SAND_OUT ARAD_PP_ISEM_ACCESS_ENTRY *isem_entry
  );

uint32
    arad_pp_isem_access_tcam_db_id_get(
      SOC_SAND_IN ARAD_PP_ISEM_ACCESS_KEY_TYPE key_type);


uint32
  arad_pp_isem_access_tcam_rewrite_entry(
     SOC_SAND_IN  int                        unit,
     SOC_SAND_IN  uint8                      entry_exists,
     SOC_SAND_IN  ARAD_TCAM_GLOBAL_LOCATION  *global_location,
     SOC_SAND_IN  ARAD_TCAM_LOCATION         *location
  );

uint32
  arad_pp_isem_access_key_to_buffer(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_ISEM_ACCESS_KEY                   *isem_key,
    SOC_SAND_IN  uint32                                 table_access_id,
    SOC_SAND_IN  uint32                                 insert_index, 
    SOC_SAND_OUT uint32                                  *buffer
  );

uint32
  arad_pp_l2_lif_ac_key_to_sem_key_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY                         *ac_key,
    SOC_SAND_OUT ARAD_PP_ISEM_ACCESS_KEY                       *isem_key
  );

uint32
  arad_pp_isem_access_sem_tables_get(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  ARAD_PP_ISEM_ACCESS_KEY                 *isem_key,
    SOC_SAND_OUT uint32                                  *nof_tables,
    SOC_SAND_OUT uint32                                  *tables_access_ids,
    SOC_SAND_OUT uint8                                   *is_duplicate_entry
  );


uint32
  arad_pp_isem_access_prog_sel_in_rif_key_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT uint8                                 *with_in_rif
  );


uint32
  arad_pp_isem_access_prog_sel_ac_key_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 entry_ndx,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY_QUALIFIER           *qual_key,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE            *key_mapping
  );

uint32
  arad_pp_isem_access_prog_sel_ac_key_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 entry_ndx,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY_QUALIFIER           *qual_key,
    SOC_SAND_OUT SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE            *key_mapping    
  );

void
  arad_pp_isem_access_print_all_programs_data(
    SOC_SAND_IN  int                                 unit
  );

char*
  arad_pp_isem_access_print_tt_program_data(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 program_id,
    SOC_SAND_IN  uint32 to_print
  );

uint32 arad_pp_isem_access_print_vt_program_look_info(int unit, int prog_id);

uint32 arad_pp_isem_access_print_tt_program_look_info(int unit, int prog_id);

char* arad_pp_isem_access_print_vt_program_data(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 program_id,
    SOC_SAND_IN  uint32 to_print
  );


void
  arad_pp_isem_access_program_sel_line_to_program_id(
    SOC_SAND_IN   int                 unit,
    SOC_SAND_IN   int                 line,    
    SOC_SAND_IN   int                 is_vt,
    SOC_SAND_OUT  uint8               *prog_id
  ) ;

uint32
  arad_pp_isem_access_print_last_vtt_program_data(
    SOC_SAND_IN   int                 unit,
    SOC_SAND_IN   int                 core_id,
    SOC_SAND_IN   int                 to_print,
    SOC_SAND_OUT  int                 *prog_id_vt,
    SOC_SAND_OUT  int                 *prog_id_tt,
    SOC_SAND_OUT  int                 *num_of_progs_vt,
    SOC_SAND_OUT  int                 *num_of_progs_tt
  );



CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_isem_access_get_procs_ptr(void);


CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_isem_access_get_errs_ptr(void);


#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif


