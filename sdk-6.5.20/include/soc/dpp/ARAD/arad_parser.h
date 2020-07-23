/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_PARSER_INCLUDED__

#define __ARAD_PARSER_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/arad_framework.h>
#include <soc/dpp/PPC/ppc_api_general.h>
#include <soc/dpp/TMC/tmc_api_pmf_low_level_db.h>








#define ARAD_PARSER_NOF_PARSER_PROGRAMS_REPLICATIONS       (3)

#define ARAD_ITMH_DEST_ISQ_FLOW_ID_PREFIX                  (0x4) 
#define ARAD_ITMH_DEST_OUT_LIF_ID_PREFIX                   (0x5) 
#define ARAD_ITMH_DEST_MC_FLOW_ID_PREFIX                   (0xD) 


#define DPP_PARSER_IP_NEXT_PROTOCOL_UDP_L2TP            6


#define DPP_PARSER_IP_NEXT_PROTOCOL_TCP                 8
#define DPP_PARSER_IP_NEXT_PROTOCOL_UDP                 9
#define DPP_PARSER_IP_NEXT_PROTOCOL_IGMP                10
#define DPP_PARSER_IP_NEXT_PROTOCOL_ICMP                11
#define DPP_PARSER_IP_NEXT_PROTOCOL_ICMPv6              12
#define DPP_PARSER_IP_NEXT_PROTOCOL_IPv4                13
#define DPP_PARSER_IP_NEXT_PROTOCOL_IPv6                14
#define DPP_PARSER_IP_NEXT_PROTOCOL_MPLS                15


#define DPP_PARSER_ETH_NEXT_PROTOCOL_TRILL              8
#define DPP_PARSER_ETH_NEXT_PROTOCOL_MinM               9
#define DPP_PARSER_ETH_NEXT_PROTOCOL_ARP                10
#define DPP_PARSER_ETH_NEXT_PROTOCOL_CFM                11
#define DPP_PARSER_ETH_NEXT_PROTOCOL_FCoE               12
#define DPP_PARSER_ETH_NEXT_PROTOCOL_IPv4               13
#define DPP_PARSER_ETH_NEXT_PROTOCOL_IPv6               14
#define DPP_PARSER_ETH_NEXT_PROTOCOL_MPLS               15


#define DPP_PARSER_NEXT_PROTOCOL_NOF                    16

#define ARAD_PARSER_CUSTOM_MACRO_PROTO_1                   (0x0) 
#define ARAD_PARSER_CUSTOM_MACRO_PROTO_2                   (0x1) 
#define ARAD_PARSER_CUSTOM_MACRO_PROTO_3                   (0x2) 
#define ARAD_PARSER_CUSTOM_MACRO_PROTO_4                   (0x3) 
#define ARAD_PARSER_CUSTOM_MACRO_PROTO_5                   (0x4) 
#define ARAD_PARSER_CUSTOM_MACRO_PROTO_6                   (0x5) 
#define ARAD_PARSER_CUSTOM_MACRO_PROTO_7                   (0x6) 
#define ARAD_PARSER_CUSTOM_MACRO_PROTO_8                   (0x7) 
#define ARAD_PARSER_CUSTOM_MACRO_PROTO_9                   (0x8) 
#define ARAD_PARSER_CUSTOM_MACRO_PROTO_10                  (0x9) 
#define ARAD_PARSER_CUSTOM_MACRO_PROTO_11                  (0xa) 
#define ARAD_PARSER_CUSTOM_MACRO_PROTO_12                  (0xb) 
#define ARAD_PARSER_CUSTOM_MACRO_PROTO_13                  (0xc) 
#define ARAD_PARSER_CUSTOM_MACRO_PROTO_14                  (0xd) 
#define ARAD_PARSER_CUSTOM_MACRO_PROTO_15                  (0xe) 

#define ARAD_PARSER_CUSTOM_MACRO_PROTO_RANGE_5_6           ((ARAD_PARSER_CUSTOM_MACRO_PROTO_5 + 1) << 1)  + 1
#define ARAD_PARSER_CUSTOM_MACRO_PROTO_RANGE_7_8           ((ARAD_PARSER_CUSTOM_MACRO_PROTO_7 + 1) << 1)  + 1
#define ARAD_PARSER_CUSTOM_MACRO_PROTO_RANGE_9_10          ((ARAD_PARSER_CUSTOM_MACRO_PROTO_9 + 1) << 1)  + 1
#define ARAD_PARSER_CUSTOM_MACRO_PROTO_RANGE_AFT_15        ((ARAD_PARSER_CUSTOM_MACRO_PROTO_15 + 1) << 1) + 1


#define ARAD_PARSER_CUSTOM_PROTO_NOF_ENTRIES                    15

#define ARAD_PARSER_PROG_RAW_ADDR_START                          (0x0000)
#define ARAD_PARSER_PROG_ETH_ADDR_START                          (0x0001)
#define ARAD_PARSER_PROG_TM_ADDR_START                           (0x0002)
#define ARAD_PARSER_PROG_PPH_TM_ADDR_START                       (0x0003)
#define ARAD_PARSER_PROG_FTMH_ADDR_START                         (0x0004)
#define ARAD_PARSER_PROG_RAW_MPLS_ADDR_START                     (0x0005)


#define ARAD_PP_UDP_DUMMY_DST_PORT      (0x10000)

 
#define ARAD_PARSER_MACRO_SEL_TM_BASE                      (0x0004)


#define IS_ETH      0x01
#define IS_IPV4     0x02
#define IS_IPV6     0x04
#define IS_MPLS     0x08
#define IS_TRILL    0x10
#define IS_UD       0x20

typedef enum
{
    
    DPP_PLC_NA              = -1,
    
    DPP_PLC_FIRST, 
    DPP_PLC_DONTCARE = DPP_PLC_FIRST, 
    DPP_PLC_TM, 
    DPP_PLC_TM_IS, 
    DPP_PLC_TM_MC_FLOW, 
    DPP_PLC_TM_OUT_LIF, 
    DPP_PLC_RAW, 
    DPP_PLC_FTMH, 
    DPP_PLC_VXLAN, 
    DPP_PLC_BFD_SINGLE_HOP, 
    DPP_PLC_IP_UDP_GTP1, 
    DPP_PLC_IP_UDP_GTP2, 
    DPP_PLC_PP_L4, 
    DPP_PLC_PP, 
    DPP_PLC_MPLS_5, 
    DPP_PLC_GAL_GACH_BFD, 
    DPP_PLC_FCOE, 
    DPP_PLC_FCOE_VFT, 
    
    DPP_PLC_ANY, 
    DPP_PLC_ANY_PP, 
    DPP_PLC_ANY_PP_AND_BFD, 
    DPP_PLC_PP_AND_PP_L4, 
    DPP_PLC_ANY_FCOE, 
    
    
    DPP_PLC_IPv4isH3, 
    DPP_PLC_IPv6isH3, 
    DPP_PLC_IPv4isH4, 
    DPP_PLC_IPv6isH4, 
    DPP_PLC_IPv4isH5, 
    DPP_PLC_IPv6isH5, 
    DPP_PLC_L4_IPv4isH3, 
    DPP_PLC_L4_IPv6isH3, 
    DPP_PLC_L4_IPv4isH4, 
    DPP_PLC_L4_IPv6isH4, 
    DPP_PLC_L4_IPv4isH5, 
    DPP_PLC_L4_IPv6isH5, 
    DPP_PLC_ETHisH3, 
    DPP_PLC_ETHisH4, 
    
    
    DPP_PLC_IPv4isHAny, 
    
    DPP_PLC_IPv6isHAny, 
    
    DPP_PLC_Any_IPv4isH3, 
    
    DPP_PLC_Any_IPv6isH3, 

    
    DPP_PLC_Any_IPv4isH4, 
    
    DPP_PLC_Any_IPv6isH4, 
    
    DPP_PLC_Any_IPv4isH5, 
    
    DPP_PLC_Any_IPv6isH5, 
    
    DPP_PLC_EthisHAny, 
    DPP_PLC_NOF 
} DPP_PLC_E;

typedef enum
{
    DPP_PLC_PROFILE_NA    = -1,
    DPP_PLC_PROFILE_FIRST =  0,
    DPP_PLC_PROFILE_TM    = DPP_PLC_PROFILE_FIRST,
    DPP_PLC_PROFILE_TM_IS,
    DPP_PLC_PROFILE_TM_MC_FLOW,
    DPP_PLC_PROFILE_TM_OUT_LIF,
    DPP_PLC_PROFILE_RAW_FTMH,
    DPP_PLC_PROFILE_PP,
    DPP_PLC_PROFILE_MPLS,
    DPP_PLC_PROFILE_MPLS_EXT,
    DPP_PLC_PROFILE_FCOE,
    DPP_PLC_PROFILE_GLOBAL,
    DPP_PLC_PROFILE_NOF
} DPP_PLC_PROFILE_E;


#define ARAD_PARSER_PLC_TM                     (0x0)
#define ARAD_PARSER_PLC_RAW                    (0x7)
#define ARAD_PARSER_PLC_FTMH                   (0x8)
#define ARAD_PARSER_PLC_TM_OUT_LIF             (0xC)
#define ARAD_PARSER_PLC_TM_MC_FLOW             (0xD)
#define ARAD_PARSER_PLC_TM_IS                  (0xD)

#define ARAD_PARSER_PLC_FCOE                   (0x4) 
#define ARAD_PARSER_PLC_FCOE_VFT               (0x5) 

#define ARAD_PARSER_PLC_VXLAN                  (0x1)
#define ARAD_PARSER_PLC_BFD_SINGLE_HOP         (0xa)
#define ARAD_PARSER_PLC_IP_UDP_GTP1            (0xC)
#define ARAD_PARSER_PLC_IP_UDP_GTP2            (0xD)
#define ARAD_PARSER_PLC_PP_L4                  (0xE) 
#define ARAD_PARSER_PLC_PP                     (0xF)

#define ARAD_PARSER_PLC_MPLS_5                 (0xC)    
#define ARAD_PARSER_PLC_GAL_GACH_BFD           (0xD)

#define ARAD_PARSER_PLC_NOF                    (16)
#define ARAD_PARSER_PLC_MAX                    (ARAD_PARSER_PLC_NOF - 1)

#define ARAD_PARSER_PLC_MATCH_ONE              0x0
#define ARAD_PARSER_PLC_MATCH_ANY_FCOE         0x1
#define ARAD_PARSER_PLC_MATCH_PP_AND_PP_L4     0x1

#define ARAD_PARSER_PLC_MATCH_ANY_PP           0x3
#define ARAD_PARSER_PLC_MATCH_ANY_PP_BFD       0x7
#define ARAD_PARSER_PLC_MATCH_ANY              0xF


#define ARAD_PARSER_ETHER_PROTO_NOF_ENTRIES                    7

#define ARAD_PARSER_ETHER_PROTO_2_PPPoES                   (0x1)
#define ARAD_PARSER_ETHER_PROTO_2_PPPoES_ETHER_TYPE        (0x8864) 
#define ARAD_PARSER_ETHER_PROTO_5_PPPoES_CONTROL           (0x5) 


#define ARAD_PARSER_ETHER_PROTO_4_ADD_TPID                 (0x3)
#define ARAD_PARSER_ETHER_PROTO_4_ADD_TPID_TYPE            (0x88a8) 
 
#define ARAD_PARSER_ETHER_PROTO_5_EoE                      (0x4)
#define ARAD_PARSER_ETHER_PROTO_5_EoE_TPID_TYPE            (0xE0E0)

#define ARAD_PARSER_ETHER_PROTO_3_EoE                      (0x2)
#define ARAD_PARSER_ETHER_PROTO_3_EoE_TPID_TYPE            (0xE0EC)

#define ARAD_PARSER_ETHER_PROTO_1_8021x                    (0x0)
#define ARAD_PARSER_ETHER_PROTO_1_8021x_TPID_TYPE          (0x888E)


#define ARAD_PARSER_ETHER_PROTO_6_1588                     (0x5) 
#define ARAD_PARSER_ETHER_PROTO_6_1588_ETHER_TYPE          (0x88f7)  
#define ARAD_PARSER_ETHER_PROTO_7_MPLS_MC                  (0x6) 
#define ARAD_PARSER_ETHER_PROTO_7_MPLS_MC_ETHER_TYPE       (0x8848)  



#define ARAD_PARSER_IP_PROTO_NOF_ENTRIES                    7


#define ARAD_PARSER_IP_PROTO_IPV6_EXT_HOP_BY_HOP            0       
#define ARAD_PARSER_IP_PROTO_IPV6_EXT_DEST_OPTIONS          60      
#define ARAD_PARSER_IP_PROTO_IPV6_EXT_ROUTING               43      
#define ARAD_PARSER_IP_PROTO_IPV6_EXT_FRAGMENT              44      
#define ARAD_PARSER_IP_PROTO_IPV6_EXT_MOBILITY              135     
#define ARAD_PARSER_IP_PROTO_IPV6_EXT_HIP                   139     
#define ARAD_PARSER_IP_PROTO_IPV6_EXT_SHIM6                 140     

typedef enum
{
  
  DPP_PFC_NA             = -1,
  
  DPP_PFC_FIRST, 
  
  DPP_PFC_ETH = DPP_PFC_FIRST, 
  
  DPP_PFC_ETH_ETH, 
  
  DPP_PFC_IPV4_ETH, 
  
  DPP_PFC_IPV6_ETH, 
  
  DPP_PFC_MPLS1_ETH, 
  
  DPP_PFC_MPLS2_ETH, 
  
  DPP_PFC_MPLS3_ETH, 
  
  DPP_PFC_FCOE_STD_ETH, 
  
  DPP_PFC_FCOE_ENCAP_ETH, 
  
  DPP_PFC_ETH_IPV4_ETH, 
  
  DPP_PFC_ETH_TRILL_ETH, 
  
  DPP_PFC_ETH_MPLS1_ETH, 
  
  DPP_PFC_ETH_MPLS2_ETH, 
  
  DPP_PFC_ETH_MPLS3_ETH, 
  
  DPP_PFC_IPV4_IPV4_ETH, 
  
  DPP_PFC_IPV4_IPV6_ETH, 
  
  DPP_PFC_IPV4_MPLS1_ETH, 
  
  DPP_PFC_IPV4_MPLS2_ETH, 
  
  DPP_PFC_IPV4_MPLS3_ETH, 
  
  DPP_PFC_IPV6_IPV4_ETH, 
  
  DPP_PFC_IPV6_IPV6_ETH, 
  
  DPP_PFC_IPV6_MPLS1_ETH, 
  
  DPP_PFC_IPV6_MPLS2_ETH, 
  
  DPP_PFC_IPV6_MPLS3_ETH, 
  
  DPP_PFC_RAW_AND_FTMH, 
  
  DPP_PFC_TM, 
  
  DPP_PFC_TM_IS, 
  
  DPP_PFC_TM_MC_FLOW, 
  
  DPP_PFC_TM_OUT_LIF, 
  DPP_PFC_LAST_OLD = DPP_PFC_TM_OUT_LIF, 
  
  DPP_PFC_ANY, 
  
  DPP_PFC_ANY_ETH, 
  
  DPP_PFC_ANY_TM, 
  
  DPP_PFC_ANY_IP, 
  
  DPP_PFC_ANY_IPV4, 
  
  DPP_PFC_ANY_IPV6, 
  
  DPP_PFC_ANY_MPLS1, 
  
  DPP_PFC_ANY_MPLS2, 
  
  DPP_PFC_ANY_MPLS3, 
  
  DPP_PFC_ANY_MPLS, 
  
  DPP_PFC_ANY_FCOE, 
  
  
  DPP_PFC_MPLS4P_ETH, 
  
  DPP_PFC_ETH_IPV6_ETH, 

  
  DPP_PFC_IPV4isH3, 
  DPP_PFC_IPV6isH3, 
  DPP_PFC_L4_IPV4isH3, 
  DPP_PFC_L4_IPV6isH3, 
  DPP_PFC_ETHisH3, 
  DPP_PFC_IPV4isH4_ETHisH3, 
  DPP_PFC_IPV6isH4_ETHisH3, 
  
  DPP_PFC_ANY_EXT_MPLS, 
  DPP_PFC_NOF 
} DPP_PFC_E;

typedef enum {
    
    DPP_PLC_TYPE_PRIMARY    = 0,
    
    DPP_PLC_TYPE_GROUP      = 2
} DPP_PLC_TYPE_E;

typedef struct
{
    DPP_PLC_E       sw;
    char*           name;
    int             hw;
    int             mask;
    DPP_PLC_TYPE_E  type;
    int             lb_set;
}  dpp_parser_plc_info_t;

typedef struct
{
    DPP_PLC_PROFILE_E       id;
    char*                   name;
    dpp_parser_plc_info_t*  plc_info;
    int                     nof_plc;
}  dpp_parser_plc_profile_t;

typedef enum {
    
    DPP_PFC_TYPE_PRIMARY    = 0,
    
    DPP_PFC_TYPE_SECONDARY  = 1,
    
    DPP_PFC_TYPE_GROUP      = 2
} DPP_PFC_TYPE_E;

typedef struct
{
    DPP_PFC_E   sw;
    char*       name;
    uint32      hw;
    uint32      mask;
    int         is_proto;
    int         l4_location;
    int         type;
    uint32      hdr_type_map;
    uint32      vtt;
    uint32      pmf;
    DPP_PLC_PROFILE_E plc_profile_id;
    DPP_PLC_PROFILE_E jer_plc_profile_id;
}  dpp_parser_pfc_info_t;

extern dpp_parser_pfc_info_t dpp_parser_pfc_info[];
extern dpp_parser_plc_profile_t dpp_parser_plc_profiles[];
extern int dpp_parser_plc_profile_nof;

extern char *parser_segment_name[];

#define ARAD_PARSER_PFC_NOF_BITS            6

#define ARAD_PARSER_PFC_NOF                 (1 << ARAD_PARSER_PFC_NOF_BITS)

typedef struct {
    DPP_PFC_E pfc_map[ARAD_PARSER_PFC_NOF];
} dpp_parser_info_t;

#define ARAD_PARSER_PFC_ETH                 0x00
#define ARAD_PARSER_PFC_ETH_ETH             0x01
#define ARAD_PARSER_PFC_IPV4_ETH            0x02
#define ARAD_PARSER_PFC_IPV6_ETH            0x03
#define ARAD_PARSER_PFC_MPLS4P_ETH          0x04
#define ARAD_PARSER_PFC_MPLS1_ETH           0x05
#define ARAD_PARSER_PFC_MPLS2_ETH           0x06
#define ARAD_PARSER_PFC_MPLS3_ETH           0x07
#define ARAD_PARSER_PFC_FCOE_STD_ETH        0x08
#define ARAD_PARSER_PFC_PPPOE_ETH	        0x08 
#define ARAD_PARSER_PFC_FCOE_ENCAP_ETH      0x09
#define ARAD_PARSER_PFC_ETH_IPV4_ETH        0x0A
#define ARAD_PARSER_PFC_ETH_IPV6_ETH        0x0B
#define ARAD_PARSER_PFC_ETH_TRILL_ETH       0x0C
#define ARAD_PARSER_PFC_ETH_MPLS1_ETH       0x0D
#define ARAD_PARSER_PFC_ETH_MPLS2_ETH       0x0E
#define ARAD_PARSER_PFC_ETH_MPLS3_ETH       0x0F
#define ARAD_PARSER_PFC_IPV4_IPV4_ETH       0x12
#define ARAD_PARSER_PFC_IPV4_IPV6_ETH       0x13
#define ARAD_PARSER_PFC_IPV4_MPLS1_ETH      0x15
#define ARAD_PARSER_PFC_IPV4_MPLS2_ETH      0x16
#define ARAD_PARSER_PFC_IPV4_MPLS3_ETH      0x17
#define ARAD_PARSER_PFC_IPV6_IPV4_ETH       0x1A
#define ARAD_PARSER_PFC_IPV6_IPV6_ETH       0x1B
#define ARAD_PARSER_PFC_IPV6_MPLS1_ETH      0x1D
#define ARAD_PARSER_PFC_IPV6_MPLS2_ETH      0x1E
#define ARAD_PARSER_PFC_IPV6_MPLS3_ETH      0x1F
#define ARAD_PARSER_PFC_RAW_AND_FTMH        0x20 
#define ARAD_PARSER_PFC_TM                  0x30 
#define ARAD_PARSER_PFC_TM_IS               0x31 
#define ARAD_PARSER_PFC_TM_MC_FLOW          0x3A 
#define ARAD_PARSER_PFC_TM_OUT_LIF          0x3C 


#define ARAD_PARSER_1_PFC_ETH_MPLS_ETH ARAD_PARSER_PFC_IPV4_MPLS2_ETH


#define ARAD_PARSER_PFC_MATCH_ONE                        0x00
#define ARAD_PARSER_PFC_MATCH_ANY_FCOE                   0x01
#define ARAD_PARSER_PFC_MATCH_IPVx                       0x01
#define ARAD_PARSER_PFC_MATCH_MPLS_ENCAP_IPVx            0x08
#define ARAD_PARSER_PFC_MATCH_ANY_ENCAP                  0x18
#define ARAD_PARSER_PFC_MATCH_ANY_MPLS                   0x1B
#define ARAD_PARSER_PFC_MATCH_ANY_ENCAP_ANY_IP           0x19
#define ARAD_PARSER_PFC_MATCH_AT_LEAST_MPLS2             0x19
#define ARAD_PARSER_PFC_MATCH_ANY_ENCAP_ANY_TYPE         0x38
#define ARAD_PARSER_PFC_MATCH_ANY_ENCAP_ANY_TYPE_ANY_IP  0x39
#define ARAD_PARSER_PFC_MATCH_TYPE                       0x1F
#define ARAD_PARSER_PFC_MATCH_ANY                        0x3F





typedef enum
{
   
   ARAD_PARSER_UDP_TUNNEL_NEXT_PRTCL_TYPE_IPV4 = 0,
    
   ARAD_PARSER_UDP_TUNNEL_NEXT_PRTCL_TYPE_IPV6 = 1,
   
   ARAD_PARSER_UDP_TUNNEL_NEXT_PRTCL_TYPE_MPLS = 2
}ARAD_PARSER_UDP_TUNNEL_NEXT_PRTCL_TYPE;









char *arad_parser_get_macro_str(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 macro_sel
  );


uint32
dpp_parser_pfc_l4_location_init(int unit);


uint32
dpp_parser_pfc_map_init(int unit);

dpp_parser_pfc_info_t *parser_pfc_info_get_by_sw(
    int unit,
    DPP_PFC_E pfc_sw);


soc_error_t dpp_parser_pfc_get_hw_by_sw(
    int       unit,
    DPP_PFC_E pfc_sw,
    uint32 *pfc_hw_p,
    uint32 *pfc_hw_mask_p);


soc_error_t dpp_parser_pfc_get_sw_by_hw(
        int unit,
        uint32 pfc_hw,
        DPP_PFC_E *pfc_sw_p);


soc_error_t dpp_parser_pfc_activate_hw_by_sw(
    int         unit,
    DPP_PFC_E   pfc_sw,
    uint32*     pfc_hw_p);


soc_error_t dpp_parser_pfc_get_vtt_by_sw(
    int         unit,
    DPP_PFC_E   pfc_sw,
    uint32*     pfc_vtt_p,
    uint32*     pfc_vtt_mask_p);


soc_error_t dpp_parser_pfc_get_acl_by_sw(
    int         unit,
    DPP_PFC_E   pfc_sw,
    uint32*     pfc_pmf_p,
    uint32*     pfc_pmf_mask_p);


soc_error_t dpp_parser_pfc_get_sw_by_acl(
    int         unit,
    uint32      pfc_pmf,
    uint32      pfc_pmf_mask,
    DPP_PFC_E*  pfc_sw_p);


soc_error_t dpp_parser_pfc_get_pmf_by_sw(
    int         unit,
    DPP_PFC_E   pfc_sw,
    uint32*     pfc_pmf_p,
    uint32*     pfc_pmf_mask_p);


char *dpp_parser_pfc_string_by_hw(
    int         unit,
    int         pfc_hw);


char *dpp_parser_pfc_string_by_sw(
    int unit,
    DPP_PFC_E   pfc_sw);


soc_error_t dpp_parser_plc_hw_by_sw(
    int         unit,
    DPP_PFC_E   pfc_sw,
    DPP_PLC_E   plc_sw,
    uint32*     plc_hw_p,
    uint32*     plc_hw_mask_p);


soc_error_t dpp_parser_plc_acl_by_sw(
    int         unit,
    DPP_PFC_E   pfc_sw,
    DPP_PLC_E   plc_sw,
    uint32*     plc_acl_p,
    uint32*     plc_acl_mask_p);


soc_error_t dpp_parser_plc_sw_by_acl(
    int         unit,
    DPP_PFC_E   pfc_sw,
    uint32      plc_acl,
    uint32      plc_acl_mask,
    DPP_PLC_E*  plc_sw_p);


char *dpp_parser_plc_string_by_sw(
    int         unit,
    DPP_PFC_E   pfc_sw,
    DPP_PLC_E   plc_sw);

char *dpp_parser_plc_string_by_hw(
    int         unit,
    int         pfc_hw,
    int         plc_hw);

char *dpp_parser_plc_profile_string_by_sw(
    int         unit,
    DPP_PFC_E   pfc_sw);

SOC_PPC_PKT_HDR_TYPE dpp_parser_pfc_hdr_type_at_index(
    int         pfc_hw,
    uint32      hdr_index);

uint32
  arad_parser_init(
    SOC_SAND_IN  int                                 unit
  );

uint32
  arad_parser_ingress_shape_state_set(
     SOC_SAND_IN int                                 unit,
     SOC_SAND_IN uint8                                 enable,
     SOC_SAND_IN uint32                                  q_low,
     SOC_SAND_IN uint32                                  q_high
  );

int
  arad_parser_nof_bytes_to_remove_set(
    SOC_SAND_IN int         unit,
    SOC_SAND_IN int         core,
    SOC_SAND_IN uint32      tm_port,
    SOC_SAND_IN uint32      nof_bytes_to_skip
  );


uint32
arad_parser_vxlan_program_info_set(
   SOC_SAND_IN int unit,
   SOC_SAND_IN uint16 udp_dest_port
 );

uint32
arad_parser_vxlan_program_info_get(
   SOC_SAND_IN int unit,
   SOC_SAND_OUT uint16 *udp_dest_port
 );

int
  arad_parser_nof_bytes_to_remove_get(
    SOC_SAND_IN int         unit,
    SOC_SAND_IN int         core,
    SOC_SAND_IN uint32      tm_port,
    SOC_SAND_OUT uint32     *nof_bytes_to_skip
  );

uint32
  arad_parser_pp_port_nof_bytes_to_remove_set(
    SOC_SAND_IN int      unit,
    SOC_SAND_IN int      core,
    SOC_SAND_IN uint32      pp_port_ndx,
    SOC_SAND_IN uint32      nof_bytes_to_skip
  );

uint32
arad_parser_udp_tunnel_dst_port_set(
    SOC_SAND_IN int                                      unit,
    SOC_SAND_IN ARAD_PARSER_UDP_TUNNEL_NEXT_PRTCL_TYPE   udp_dst_port_type,
    SOC_SAND_IN int                                      udp_dst_port_val
  );

uint32
arad_parser_udp_tunnel_dst_port_get(
    SOC_SAND_IN  int                                        unit,
    SOC_SAND_IN  ARAD_PARSER_UDP_TUNNEL_NEXT_PRTCL_TYPE     udp_dst_port_type,
    SOC_SAND_OUT int                                        *udp_dst_port_val
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
