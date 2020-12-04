/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __SOC_SAND_PP_GENERAL_INCLUDED__

#define __SOC_SAND_PP_GENERAL_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/SAND/Utils/sand_pp_mac.h>







#define SOC_SAND_PP_IPV4_SUBNET_PREF_MAX_LEN 32
#define SOC_SAND_PP_IPV4_MAX_IP_STRING 18

#define SOC_SAND_PP_IPV4_MC_ADDR_PREFIX 0xE0000000




#define SOC_SAND_PP_PORT_L2_TYPE_CEP_VAL 4
#define SOC_SAND_PP_PORT_L2_TYPE_PCEP_VAL 5




#define  SOC_SAND_PP_LOCAL_PORT_CEP_SET(port_id) \
          (SOC_SAND_SET_BITS_RANGE(SOC_SAND_PP_PORT_L2_TYPE_CEP_VAL,31,26) | SOC_SAND_SET_BITS_RANGE(port_id,6,0) )




  

 



#define  SOC_SAND_PP_LOCAL_PORT_PEP_SET(port_id,s_vid) \
          (SOC_SAND_SET_BITS_RANGE(SOC_SAND_PP_PORT_L2_TYPE_PCEP_VAL,31,26) | SOC_SAND_SET_BITS_RANGE(s_vid,21,10) | SOC_SAND_SET_BITS_RANGE(port_id,6,0) )



#define SOC_SAND_PP_IPV4_ADDRESS_NOF_UINT32S 1
#define SOC_SAND_PP_IPV6_ADDRESS_NOF_UINT32S 4


#define  SOC_SAND_PP_IPV4_ADDRESS_NOF_BITS (SOC_SAND_PP_IPV4_ADDRESS_NOF_UINT32S * SOC_SAND_NOF_BITS_IN_UINT32)
#define  SOC_SAND_PP_IPV6_ADDRESS_NOF_BITS (SOC_SAND_PP_IPV6_ADDRESS_NOF_UINT32S * SOC_SAND_NOF_BITS_IN_UINT32)


#define  SOC_SAND_PP_IPV4_ADDRESS_STRING_LEN 8
#define  SOC_SAND_PP_IPV6_ADDRESS_STRING_LEN 32



#define SOC_SAND_PP_NOF_BITS_IN_EXP                                (8)

#define SOC_SAND_PP_VLAN_ID_MAX                                    (4*1024-1)

#define SOC_SAND_PP_BVID_ID_MAX_JERICHO_ONLY                       (32*1024-1)
#define SOC_SAND_PP_BVID_ID_MIN_JERICHO_ONLY                       (28*1024-1)
#define SOC_SAND_PP_TC_MAX                                         (7)
#define SOC_SAND_PP_DP_MAX                                         (3)
#define SOC_SAND_PP_DEI_CFI_MAX                                    (1)
#define SOC_SAND_PP_PCP_UP_MAX                                     (7)
#define SOC_SAND_PP_ETHER_TYPE_MAX                                 (0xffff)
#define SOC_SAND_PP_TPID_MAX                                       (0xffff)
#define SOC_SAND_PP_IP_TTL_MAX                                     (255)
#define SOC_SAND_PP_IPV4_TOS_MAX                                   (255)
#define SOC_SAND_PP_IPV6_TC_MAX                                    (255)
#define SOC_SAND_PP_MPLS_LABEL_MAX                                 ((1<<20)-1)
#define SOC_SAND_PP_MPLS_EXP_MAX                                   (7)
#define SOC_SAND_PP_ISID_MAX                                       (0xffffff)
#define SOC_SAND_PP_L3_DSCP_MAX                                    (SOC_SAND_PP_IPV4_TOS_MAX) 
#define SOC_SAND_PP_MPLS_DSCP_MAX                                  (SOC_SAND_PP_MPLS_EXP_MAX)
#define SOC_SAND_PON_TUNNEL_ID_MAX                                 (0x7ff)
#define SOC_SAND_PP_IP_TTL_NOF_BITS                          (8)

#define SOC_SAND_PP_ETHER_TYPE_NOF_BITS                      (16)
#define SOC_SAND_PP_TPID_NOF_BITS                            (16)
#define SOC_SAND_PP_UP_NOF_BITS                              (3)
#define SOC_SAND_PP_VID_NOF_BITS                             (12)
#define SOC_SAND_PP_CFI_NOF_BITS                             (1)
#define SOC_SAND_PP_PCP_NOF_BITS                             (3)

#define SOC_SAND_PP_NOF_TC                                   (8)



#define SOC_SAND_PP_ETHERNET_ETHERTYPE_IPV4       (0x0800)
#define SOC_SAND_PP_ETHERNET_ETHERTYPE_TRILL      (0x22F3)
#define SOC_SAND_PP_ETHERNET_ETHERTYPE_IPV6       (0x86DD)
#define SOC_SAND_PP_ETHERNET_ETHERTYPE_MPLS       (0x8847)



#define SOC_SAND_PP_TRILL_NICK_NAME_NOF_BITS      (16)
#define SOC_SAND_PP_TRILL_M_NOF_BITS              (1)



#define SOC_SAND_PP_IP_PROTOCOL_NOF_BITS (8)

#define SOC_SAND_PP_IP_PROTOCOL_HOPOPT (0x00)
#define SOC_SAND_PP_IP_PROTOCOL_ICMP (0x01)
#define SOC_SAND_PP_IP_PROTOCOL_IGMP (0x02)
#define SOC_SAND_PP_IP_PROTOCOL_GGP (0x03)
#define SOC_SAND_PP_IP_PROTOCOL_IP_in_IP (0x04)
#define SOC_SAND_PP_IP_PROTOCOL_ST (0x05)
#define SOC_SAND_PP_IP_PROTOCOL_TCP (0x06)
#define SOC_SAND_PP_IP_PROTOCOL_CBT (0x07)
#define SOC_SAND_PP_IP_PROTOCOL_EGP (0x08)
#define SOC_SAND_PP_IP_PROTOCOL_IGP (0x09)
#define SOC_SAND_PP_IP_PROTOCOL_BBN_RCC_MON (0x0A)
#define SOC_SAND_PP_IP_PROTOCOL_NVP_II (0x0B)
#define SOC_SAND_PP_IP_PROTOCOL_PUP (0x0C)
#define SOC_SAND_PP_IP_PROTOCOL_ARGUS (0x0D)
#define SOC_SAND_PP_IP_PROTOCOL_EMCON (0x0E)
#define SOC_SAND_PP_IP_PROTOCOL_XNET (0x0F)
#define SOC_SAND_PP_IP_PROTOCOL_CHAOS (0x10)
#define SOC_SAND_PP_IP_PROTOCOL_UDP (0x11)
#define SOC_SAND_PP_IP_PROTOCOL_MUX (0x12)
#define SOC_SAND_PP_IP_PROTOCOL_DCN_MEAS (0x13)
#define SOC_SAND_PP_IP_PROTOCOL_HMP (0x14)
#define SOC_SAND_PP_IP_PROTOCOL_PRM (0x15)
#define SOC_SAND_PP_IP_PROTOCOL_XNS_IDP (0x16)
#define SOC_SAND_PP_IP_PROTOCOL_TRUNK_1 (0x17)
#define SOC_SAND_PP_IP_PROTOCOL_TRUNK_2 (0x18)
#define SOC_SAND_PP_IP_PROTOCOL_LEAF_1 (0x19)
#define SOC_SAND_PP_IP_PROTOCOL_LEAF_2 (0x1A)
#define SOC_SAND_PP_IP_PROTOCOL_RDP (0x1B)
#define SOC_SAND_PP_IP_PROTOCOL_IRTP (0x1C)
#define SOC_SAND_PP_IP_PROTOCOL_ISO_TP4 (0x1D)
#define SOC_SAND_PP_IP_PROTOCOL_NETBLT (0x1E)
#define SOC_SAND_PP_IP_PROTOCOL_MFE_NSP (0x1F)
#define SOC_SAND_PP_IP_PROTOCOL_MERIT_INP (0x20)
#define SOC_SAND_PP_IP_PROTOCOL_DCCP (0x21)
#define SOC_SAND_PP_IP_PROTOCOL_3PC (0x22)
#define SOC_SAND_PP_IP_PROTOCOL_IDPR (0x23)
#define SOC_SAND_PP_IP_PROTOCOL_XTP (0x24)
#define SOC_SAND_PP_IP_PROTOCOL_DDP (0x25)
#define SOC_SAND_PP_IP_PROTOCOL_IDPR_CMTP (0x26)
#define SOC_SAND_PP_IP_PROTOCOL_TP_PLUS_PLUS (0x27)
#define SOC_SAND_PP_IP_PROTOCOL_IL (0x28)
#define SOC_SAND_PP_IP_PROTOCOL_IPv6 (0x29)
#define SOC_SAND_PP_IP_PROTOCOL_SDRP (0x2A)
#define SOC_SAND_PP_IP_PROTOCOL_IPv6_Route (0x2B)
#define SOC_SAND_PP_IP_PROTOCOL_IPv6_Frag (0x2C)
#define SOC_SAND_PP_IP_PROTOCOL_IDRP (0x2D)
#define SOC_SAND_PP_IP_PROTOCOL_RSVP (0x2E)
#define SOC_SAND_PP_IP_PROTOCOL_GRE (0x2F)
#define SOC_SAND_PP_IP_PROTOCOL_MHRP (0x30)
#define SOC_SAND_PP_IP_PROTOCOL_BNA (0x31)
#define SOC_SAND_PP_IP_PROTOCOL_ESP (0x32)
#define SOC_SAND_PP_IP_PROTOCOL_AH (0x33)
#define SOC_SAND_PP_IP_PROTOCOL_I_NLSP (0x34)
#define SOC_SAND_PP_IP_PROTOCOL_SWIPE (0x35)
#define SOC_SAND_PP_IP_PROTOCOL_NARP (0x36)
#define SOC_SAND_PP_IP_PROTOCOL_MOBILE (0x37)
#define SOC_SAND_PP_IP_PROTOCOL_TLSP (0x38)
#define SOC_SAND_PP_IP_PROTOCOL_SKIP (0x39)
#define SOC_SAND_PP_IP_PROTOCOL_IPv6_ICMP (0x3A)
#define SOC_SAND_PP_IP_PROTOCOL_IPv6_NoNxt (0x3B)
#define SOC_SAND_PP_IP_PROTOCOL_IPv6_Opts (0x3C)
#define SOC_SAND_PP_IP_PROTOCOL_CFTP (0x3E)
#define SOC_SAND_PP_IP_PROTOCOL_SAT_EXPAK (0x40)
#define SOC_SAND_PP_IP_PROTOCOL_KRYPTOLAN (0x41)
#define SOC_SAND_PP_IP_PROTOCOL_RVD (0x42)
#define SOC_SAND_PP_IP_PROTOCOL_IPPC (0x43)
#define SOC_SAND_PP_IP_PROTOCOL_SAT_MON (0x45)
#define SOC_SAND_PP_IP_PROTOCOL_VISA (0x46)
#define SOC_SAND_PP_IP_PROTOCOL_IPCU (0x47)
#define SOC_SAND_PP_IP_PROTOCOL_CPNX (0x48)
#define SOC_SAND_PP_IP_PROTOCOL_CPHB (0x49)
#define SOC_SAND_PP_IP_PROTOCOL_WSN (0x4A)
#define SOC_SAND_PP_IP_PROTOCOL_PVP (0x4B)
#define SOC_SAND_PP_IP_PROTOCOL_BR_SAT_MON (0x4C)
#define SOC_SAND_PP_IP_PROTOCOL_SUN_ND (0x4D)
#define SOC_SAND_PP_IP_PROTOCOL_WB_MON (0x4E)
#define SOC_SAND_PP_IP_PROTOCOL_WB_EXPAK (0x4F)
#define SOC_SAND_PP_IP_PROTOCOL_ISO_IP (0x50)
#define SOC_SAND_PP_IP_PROTOCOL_VMTP (0x51)
#define SOC_SAND_PP_IP_PROTOCOL_SECURE_VMTP (0x52)
#define SOC_SAND_PP_IP_PROTOCOL_VINES (0x53)
#define SOC_SAND_PP_IP_PROTOCOL_TTP (0x54)
#define SOC_SAND_PP_IP_PROTOCOL_IPTM (0x54)
#define SOC_SAND_PP_IP_PROTOCOL_NSFNET_IGP (0x55)
#define SOC_SAND_PP_IP_PROTOCOL_DGP (0x56)
#define SOC_SAND_PP_IP_PROTOCOL_TCF (0x57)
#define SOC_SAND_PP_IP_PROTOCOL_EIGRP (0x58)
#define SOC_SAND_PP_IP_PROTOCOL_OSPF (0x59)
#define SOC_SAND_PP_IP_PROTOCOL_Sprite_RPC (0x5A)
#define SOC_SAND_PP_IP_PROTOCOL_LARP (0x5B)
#define SOC_SAND_PP_IP_PROTOCOL_MTP (0x5C)
#define SOC_SAND_PP_IP_PROTOCOL_AX_25 (0x5D)
#define SOC_SAND_PP_IP_PROTOCOL_IPIP (0x5E)
#define SOC_SAND_PP_IP_PROTOCOL_MICP (0x5F)
#define SOC_SAND_PP_IP_PROTOCOL_SCC_SP (0x60)
#define SOC_SAND_PP_IP_PROTOCOL_ETHERIP (0x61)
#define SOC_SAND_PP_IP_PROTOCOL_ENCAP (0x62)
#define SOC_SAND_PP_IP_PROTOCOL_GMTP (0x64)
#define SOC_SAND_PP_IP_PROTOCOL_IFMP (0x65)
#define SOC_SAND_PP_IP_PROTOCOL_PNNI (0x66)
#define SOC_SAND_PP_IP_PROTOCOL_PIM (0x67)
#define SOC_SAND_PP_IP_PROTOCOL_ARIS (0x68)
#define SOC_SAND_PP_IP_PROTOCOL_SCPS (0x69)
#define SOC_SAND_PP_IP_PROTOCOL_QNX (0x6A)
#define SOC_SAND_PP_IP_PROTOCOL_A_N (0x6B)
#define SOC_SAND_PP_IP_PROTOCOL_IPComp (0x6C)
#define SOC_SAND_PP_IP_PROTOCOL_SNP (0x6D)
#define SOC_SAND_PP_IP_PROTOCOL_Compaq_Peer (0x6E)
#define SOC_SAND_PP_IP_PROTOCOL_IPX_in_IP (0x6F)
#define SOC_SAND_PP_IP_PROTOCOL_VRRP (0x70)
#define SOC_SAND_PP_IP_PROTOCOL_PGM (0x71)
#define SOC_SAND_PP_IP_PROTOCOL_L2TP (0x73)
#define SOC_SAND_PP_IP_PROTOCOL_DDX (0x74)
#define SOC_SAND_PP_IP_PROTOCOL_IATP (0x75)
#define SOC_SAND_PP_IP_PROTOCOL_STP (0x76)
#define SOC_SAND_PP_IP_PROTOCOL_SRP (0x77)
#define SOC_SAND_PP_IP_PROTOCOL_UTI (0x78)
#define SOC_SAND_PP_IP_PROTOCOL_SMP (0x79)
#define SOC_SAND_PP_IP_PROTOCOL_SM (0x7A)
#define SOC_SAND_PP_IP_PROTOCOL_PTP (0x7B)
#define SOC_SAND_PP_IP_PROTOCOL_IS_IS over IPv4 (0x7C)
#define SOC_SAND_PP_IP_PROTOCOL_FIRE (0x7D)
#define SOC_SAND_PP_IP_PROTOCOL_CRTP (0x7E)
#define SOC_SAND_PP_IP_PROTOCOL_CRUDP (0x7F)
#define SOC_SAND_PP_IP_PROTOCOL_SSCOPMCE (0x80)
#define SOC_SAND_PP_IP_PROTOCOL_IPLT (0x81)
#define SOC_SAND_PP_IP_PROTOCOL_SPS (0x82)
#define SOC_SAND_PP_IP_PROTOCOL_PIPE (0x83)
#define SOC_SAND_PP_IP_PROTOCOL_SCTP (0x84)
#define SOC_SAND_PP_IP_PROTOCOL_FC (0x85)
#define SOC_SAND_PP_IP_PROTOCOL_RSVP_E2E_IGNORE (0x86)
#define SOC_SAND_PP_IP_PROTOCOL_Mobility Header (0x87)
#define SOC_SAND_PP_IP_PROTOCOL_UDPLite (0x88)
#define SOC_SAND_PP_IP_PROTOCOL_MPLS_in_IP (0x89)
#define SOC_SAND_PP_IP_PROTOCOL_manet (0x8A)
#define SOC_SAND_PP_IP_PROTOCOL_HIP (0x8B)
#define SOC_SAND_PP_IP_PROTOCOL_Shim6 (0x8C)
#define SOC_SAND_PP_IP_PROTOCOL_WESP (0x8D)
#define SOC_SAND_PP_IP_PROTOCOL_ROHC (0x8E)


#define SOC_SAND_PP_IP_NO_OPTION_NOF_BITS (20 * SOC_SAND_NOF_BITS_IN_BYTE)

#define SOC_SAND_PP_UDP_NOF_BITS (8 * SOC_SAND_NOF_BITS_IN_BYTE)

#define SOC_SAND_PP_VXLAN_NOF_BITS (8 * SOC_SAND_NOF_BITS_IN_BYTE)

#define SOC_SAND_PP_GRE2_NOF_BITS (2 * SOC_SAND_NOF_BITS_IN_BYTE)

#define SOC_SAND_PP_GRE4_NOF_BITS (4 * SOC_SAND_NOF_BITS_IN_BYTE)

#define SOC_SAND_PP_GRE8_NOF_BITS (8 * SOC_SAND_NOF_BITS_IN_BYTE)

#define SOC_SAND_PP_ERSPAN_TYPEII_NOF_BITS  (8 * SOC_SAND_NOF_BITS_IN_BYTE) 






#define SOC_SAND_PP_NOF_TAGS_IN_VLAN_FORMAT(tag_format) \
      (tag_format == SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE)? 0: \
      (tag_format == SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_TAG || tag_format == SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_C_TAG || \
       tag_format == SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_I_TAG || tag_format == SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_PRIORITY_TAG)?1:2;

#define SOC_SAND_PP_TOS_SET(__tos, __tos_val, __is_uniform) \
        (__tos) =  ((__is_uniform << 8) | (__tos_val))

#define SOC_SAND_PP_TOS_VAL_GET(__tos) \
         (0xff & (__tos))

#define SOC_SAND_PP_TOS_IS_UNIFORM_GET(__tos) \
         ((__tos) >> 8)


#define SOC_SAND_PP_TTL_SET(__ttl, __ttl_val, __is_uniform) \
        (__ttl) =  ((__is_uniform << 8) | (__ttl_val))

#define SOC_SAND_PP_TTL_VAL_GET(__ttl) \
         (0xff & (__ttl))

#define SOC_SAND_PP_TTL_IS_UNIFORM_GET(__ttl) \
         ((__ttl) >> 8)








typedef uint32 SOC_SAND_PP_LOCAL_PORT_ID;


typedef uint32 SOC_SAND_PP_VLAN_ID; 


typedef uint32 SOC_SAND_PON_TUNNEL_ID;


typedef uint8 SOC_SAND_PP_TC; 



typedef uint8 SOC_SAND_PP_DP; 



typedef uint8 SOC_SAND_PP_DEI_CFI; 



typedef uint8 SOC_SAND_PP_PCP_UP; 



typedef uint16 SOC_SAND_PP_ETHER_TYPE; 



typedef uint16 SOC_SAND_PP_TPID; 



typedef uint16 SOC_SAND_PP_IP_TTL; 



typedef uint16 SOC_SAND_PP_IPV4_TOS; 



typedef uint8 SOC_SAND_PP_IPV6_TC; 



typedef uint32 SOC_SAND_PP_MPLS_LABEL; 



typedef uint8 SOC_SAND_PP_MPLS_EXP; 



typedef uint32 SOC_SAND_PP_ISID; 



typedef enum
{
  
  SOC_SAND_PP_L4_PORT_TYPE_UDP = 0,
  
  SOC_SAND_PP_L4_PORT_TYPE_TCP = 1,
  
  SOC_SAND_PP_L4_PORT_TYPE_LAST
}SOC_SAND_PP_L4_PORT_TYPE;





typedef enum
{
  
  SOC_SAND_PP_FRWRD_ACTION_TYPE_NONE=0,
  
  SOC_SAND_PP_FRWRD_ACTION_TYPE_NORMAL=1,
  
  SOC_SAND_PP_FRWRD_ACTION_TYPE_INTERCEPT=2,
  
  SOC_SAND_PP_FRWRD_ACTION_TYPE_CONTROL=3,
  
  SOC_SAND_PP_NOF_FRWRD_ACTION_TYPES
}SOC_SAND_PP_FRWRD_ACTION_TYPE;


typedef enum
{
  
  SOC_SAND_PP_VLAN_TAG_TYPE_NONE = 0,
  
  SOC_SAND_PP_VLAN_TAG_TYPE_CTAG = 1,
  
  SOC_SAND_PP_VLAN_TAG_TYPE_STAG = 2,
  
  SOC_SAND_PP_VLAN_TAG_TYPE_PRIORITY = 3,
  
  SOC_SAND_PP_VLAN_TAG_TYPE_ANY = 4,
  
  SOC_SAND_PP_VLAN_TAG_TYPE_ITAG = 5,
  
  SOC_SAND_PP_NOF_VLAN_TAG_TYPES = 6
}SOC_SAND_PP_VLAN_TAG_TYPE;

typedef enum
{
   
    SOC_SAND_PP_ETHERNET_DA_TYPE_TYPE_INVALID = -1,
  
  SOC_SAND_PP_ETHERNET_DA_TYPE_TYPE_MC = 0,
  
  SOC_SAND_PP_ETHERNET_DA_TYPE_TYPE_BC = 1,
  
  SOC_SAND_PP_ETHERNET_DA_TYPE_TYPE_UC = 2,
  
  SOC_SAND_PP_NOF_ETHERNET_DA_TYPES = 3
}SOC_SAND_PP_ETHERNET_DA_TYPE;

typedef enum
{
  
  SOC_SAND_PP_PORT_L2_TYPE_VBP = 0,
  
  SOC_SAND_PP_PORT_L2_TYPE_CNP = 1,
  
  SOC_SAND_PP_PORT_L2_TYPE_CEP = 2,
  
  SOC_SAND_PP_PORT_L2_TYPE_PNP = 3,
  
  SOC_SAND_PP_NOF_PORT_L2_TYPES = 4
}SOC_SAND_PP_PORT_L2_TYPE;
typedef enum
{
  
  SOC_SAND_PP_SYS_PORT_TYPE_SINGLE_PORT = 0,
  
  SOC_SAND_PP_SYS_PORT_TYPE_LAG = 1,
  
  SOC_SAND_PP_NOF_SYS_PORT_TYPES = 2
}SOC_SAND_PP_SYS_PORT_TYPE;
typedef enum
{
  
  SOC_SAND_PP_L4_PRTCL_TYPE_UDP = 0x1,
  
  SOC_SAND_PP_L4_PRTCL_TYPE_TCP = 0x2,
  
  SOC_SAND_PP_L4_PRTCL_TYPE_TCP_UDP = 0x3,
  
  SOC_SAND_PP_NOF_L4_PRTCL_TYPES
}SOC_SAND_PP_L4_PRTCL_TYPE;
typedef enum
{
  
  SOC_SAND_PP_IP_TYPE_NONE = 0x0,
  
  SOC_SAND_PP_IP_TYPE_IPV4_UC = 0x1,
  
  SOC_SAND_PP_IP_TYPE_IPV4_MC = 0x2,
  
  SOC_SAND_PP_IP_TYPE_IPV4 = 0x3,
  
  SOC_SAND_PP_IP_TYPE_IPV6_UC = 0x4,
  
  SOC_SAND_PP_IP_TYPE_IPV6_MC = 0x8,
  
  SOC_SAND_PP_IP_TYPE_IPV6 = 0xC,
  
  SOC_SAND_PP_IP_TYPE_ALL = (int)0xFFFFFFFF,
  
  SOC_SAND_PP_NOF_IP_TYPES = 7
}SOC_SAND_PP_IP_TYPE;

typedef enum
{
  
  SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB = 0,
  
  SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE = 1,
  
  SOC_SAND_PP_NOF_HUB_SPOKE_ORIENTATIONS = 2
}SOC_SAND_PP_HUB_SPOKE_ORIENTATION;
typedef enum
{
  
  SOC_SAND_PP_SPLIT_HORIZON_ORIENTATION_SPLIT = 0,
  
  SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE_HORIZON = 1,
  
  SOC_SAND_PP_NOF_SPLIT_HORIZON_ORIENTATIONS = 2
}SOC_SAND_PP_SPLIT_HORIZON_ORIENTATION;
typedef enum
{
  
  SOC_SAND_PP_DEST_TYPE_DROP = 0,
  
  SOC_SAND_PP_DEST_TYPE_ROUTER = 1,
  
  SOC_SAND_PP_DEST_SINGLE_PORT = 2,
  
  SOC_SAND_PP_DEST_EXPLICIT_FLOW = 3,
  
  SOC_SAND_PP_DEST_LAG = 4,
  
  SOC_SAND_PP_DEST_MULTICAST = 5,
  
  SOC_SAND_PP_DEST_FEC = 6,
  
  SOC_SAND_PP_DEST_TRAP = 7,
  
  SOC_SAND_PP_NOF_DEST_TYPES = 8
}SOC_SAND_PP_DEST_TYPE;
typedef enum
{
  
  SOC_SAND_PP_MPLS_TUNNEL_MODEL_PIPE = 0,
  
  SOC_SAND_PP_MPLS_TUNNEL_MODEL_UNIFORM = 1,

  
  SOC_SAND_PP_MPLS_TUNNEL_MODEL_SET = 2,
  
  SOC_SAND_PP_MPLS_TUNNEL_MODEL_COPY = 3,
  
  SOC_SAND_PP_NOF_MPLS_TUNNEL_MODELS = 4
}SOC_SAND_PP_MPLS_TUNNEL_MODEL;
typedef enum
{
  
  SOC_SAND_PP_FEC_COMMAND_TYPE_ROUTE = 0,
  
  SOC_SAND_PP_FEC_COMMAND_TYPE_TRAP = 1,
  
  SOC_SAND_PP_FEC_COMMAND_TYPE_DROP = 2,
  
  SOC_SAND_PP_NOF_FEC_COMMAND_TYPES = 3
}SOC_SAND_PP_FEC_COMMAND_TYPE;

typedef enum
{
  
  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE = 0,
  
  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_C_TAG = 1,
  
  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_TAG = 2,
  
  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_PRIORITY_TAG = 3,
  
  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_C_C_TAG = 5,
  
  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_PRIORITY_C_TAG = 7,
  
  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_C_TAG = 6,
  
  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_C_S_TAG = 9,
  
  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_S_TAG = 10,
  
  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_PRIORITY_S_TAG = 11,
  
  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_I_TAG = 4,
  
  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_ANY = (int)0xFFFFFFFF,
  
  SOC_SAND_PP_NOF_ETHERNET_FRAME_VLAN_FORMATS = 16
}SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT;

typedef enum
{
  
  SOC_SAND_PP_ETH_ENCAP_TYPE_ETH2 = 0,
  
  SOC_SAND_PP_ETH_ENCAP_TYPE_LLC = 1,
  
  SOC_SAND_PP_ETH_ENCAP_TYPE_LLC_SNAP = 2,
  
  SOC_SAND_PP_ETH_ENCAP_TYPE_OTHER = 3,
  
  SOC_SAND_PP_NOF_ETH_ENCAP_TYPES = 4
}SOC_SAND_PP_ETH_ENCAP_TYPE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_SAND_PP_DEST_TYPE dest_type;
  
  uint32 dest_val;

} SOC_SAND_PP_DESTINATION_ID;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 address[SOC_SAND_PP_IPV4_ADDRESS_NOF_UINT32S];

} SOC_SAND_PP_IPV4_ADDRESS;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 user_priority;
  
  uint32 traffic_class;
  
  uint32 drop_precedence;
}SOC_SAND_PP_COS_PARAMS;


 
typedef uint32 SOC_SAND_PP_SYSTEM_FEC_ID;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 ip_address;
  
  uint8 prefix_len;

} SOC_SAND_PP_IPV4_SUBNET;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 address[SOC_SAND_PP_IPV6_ADDRESS_NOF_UINT32S];

} SOC_SAND_PP_IPV6_ADDRESS;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_SAND_PP_IPV6_ADDRESS ipv6_address;
  
  uint8 prefix_len;

} SOC_SAND_PP_IPV6_SUBNET;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_SAND_PP_SYS_PORT_TYPE sys_port_type;
  
  uint32 sys_id;

} SOC_SAND_PP_SYS_PORT_ID;

typedef struct
{
  
  uint32 val;
  
  uint32 mask;
}SOC_SAND_PP_MASKED_VAL;
typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_SAND_PP_TPID tpid;
  
  SOC_SAND_PP_VLAN_ID vid;
  
  SOC_SAND_PP_PCP_UP pcp;
  
  SOC_SAND_PP_DEI_CFI dei;

} SOC_SAND_PP_VLAN_TAG;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_SAND_PP_IPV4_SUBNET source;
  
  SOC_SAND_PP_IPV4_SUBNET group;
  
  SOC_SAND_PP_MASKED_VAL vid;
  
  SOC_SAND_PP_MASKED_VAL port;
}SOC_SAND_PP_IPV4_MC_ROUTE_KEY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
 
  SOC_SAND_PP_IPV6_SUBNET dest;

  SOC_SAND_PP_MASKED_VAL vid;
  
  SOC_SAND_PP_MASKED_VAL port;

}SOC_SAND_PP_IPV6_ROUTE_KEY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 is_multicast;
  
  uint32 dest_nick;

} SOC_SAND_PP_TRILL_DEST;









uint32
  soc_sand_pp_ipv4_subnet_verify(
    SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET *subnet
  );

uint32
  SOC_SAND_PP_TRILL_DEST_verify(
    SOC_SAND_IN  SOC_SAND_PP_TRILL_DEST *info
  );

uint32
  soc_sand_pp_ipv4_address_string_parse(
    SOC_SAND_IN char               ipv4_string[SOC_SAND_PP_IPV4_ADDRESS_STRING_LEN],
    SOC_SAND_OUT SOC_SAND_PP_IPV4_ADDRESS   *ipv4_addr
  );

uint32
  soc_sand_pp_ipv6_address_string_parse(
    SOC_SAND_IN char               ipv6_string[SOC_SAND_PP_IPV6_ADDRESS_STRING_LEN],
    SOC_SAND_OUT SOC_SAND_PP_IPV6_ADDRESS   *ipv6_addr
  );

void
  soc_sand_SAND_PP_DESTINATION_ID_clear(
    SOC_SAND_OUT SOC_SAND_PP_DESTINATION_ID *info
  );

void
  soc_sand_SAND_PP_COS_PARAMS_clear(
    SOC_SAND_OUT SOC_SAND_PP_COS_PARAMS *info
  );

void
  soc_sand_SAND_PP_SYS_PORT_ID_clear(
    SOC_SAND_OUT SOC_SAND_PP_SYS_PORT_ID *info
  );

void
  soc_sand_SAND_PP_IPV4_SUBNET_clear(
    SOC_SAND_OUT SOC_SAND_PP_IPV4_SUBNET *info
  );

void
  soc_sand_SAND_PP_IPV4_ADDRESS_clear(
    SOC_SAND_OUT SOC_SAND_PP_IPV4_ADDRESS *info
  );

void
  soc_sand_SAND_PP_IPV6_ADDRESS_clear(
    SOC_SAND_OUT SOC_SAND_PP_IPV6_ADDRESS *info
  );

void
  soc_sand_SAND_PP_IPV6_SUBNET_clear(
    SOC_SAND_OUT SOC_SAND_PP_IPV6_SUBNET *info
  );

void
  soc_sand_SAND_PP_TRILL_DEST_clear(
    SOC_SAND_OUT SOC_SAND_PP_TRILL_DEST *info
  );

void
  SOC_SAND_PP_VLAN_TAG_clear(
    SOC_SAND_OUT SOC_SAND_PP_VLAN_TAG *info
  );


void
  soc_sand_SAND_PP_IPV4_MC_ROUTE_KEY_clear(
    SOC_SAND_OUT SOC_SAND_PP_IPV4_MC_ROUTE_KEY *info
  );

void
soc_sand_SAND_PP_IPV6_ROUTE_KEY_clear(
  SOC_SAND_OUT SOC_SAND_PP_IPV6_ROUTE_KEY *info
  );

uint32
  SOC_SAND_PP_DESTINATION_ID_encode(
    SOC_SAND_IN  SOC_SAND_PP_DESTINATION_ID               *dest_info,
    SOC_SAND_OUT uint32                             *encoded_dest_val
  );

uint32
  SOC_SAND_PP_DESTINATION_ID_decode(
    SOC_SAND_IN  uint32                             encoded_dest_val,
    SOC_SAND_OUT SOC_SAND_PP_DESTINATION_ID               *dest_info
  );


#if SOC_SAND_DEBUG

const char*
  soc_sand_SAND_PP_COS_PARAMS_to_string(
    SOC_SAND_IN SOC_SAND_PP_COS_PARAMS enum_val
  );

const char*
  soc_sand_SAND_PP_DEST_TYPE_to_string(
    SOC_SAND_IN  SOC_SAND_PP_DEST_TYPE enum_val
  );

const char*
  soc_sand_SAND_PP_FRWRD_ACTION_TYPE_to_string(
    SOC_SAND_IN SOC_SAND_PP_FRWRD_ACTION_TYPE enum_val
  );

const char*
  soc_sand_SAND_PP_FRWRD_ACTION_TYPE_to_string_short(
    SOC_SAND_IN SOC_SAND_PP_FRWRD_ACTION_TYPE enum_val
  );

const char*
  soc_sand_SAND_PP_PORT_L2_TYPE_to_string(
    SOC_SAND_IN  SOC_SAND_PP_PORT_L2_TYPE enum_val
  );

const char*
  soc_sand_SAND_PP_PORT_L2_TYPE_to_string_short(
    SOC_SAND_IN SOC_SAND_PP_PORT_L2_TYPE enum_val
  );

const char*
  soc_sand_SAND_PP_VLAN_TAG_TYPE_to_string(
    SOC_SAND_IN  SOC_SAND_PP_VLAN_TAG_TYPE enum_val
  );

void
  soc_sand_SAND_PP_IPV4_SUBNET_print_short(
    SOC_SAND_IN SOC_SAND_PP_IPV4_SUBNET *info
  );

const char*
  soc_sand_SAND_PP_ETHERNET_DA_TYPE_to_string(
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_DA_TYPE enum_val
  );

const char*
  soc_sand_SAND_PP_SYS_PORT_TYPE_to_string(
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_TYPE enum_val
  );

const char*
  soc_sand_SAND_PP_L4_PORT_TYPE_to_string(
    SOC_SAND_IN  SOC_SAND_PP_L4_PORT_TYPE enum_val
  );

const char*
  soc_sand_pp_ip_long_to_string(
    SOC_SAND_IN   uint32    ip_addr,
    SOC_SAND_IN   uint8     short_format,
    SOC_SAND_OUT   char  decimal_ip[SOC_SAND_PP_IPV4_MAX_IP_STRING]
  );

const char*
  soc_sand_SAND_PP_FEC_COMMAND_TYPE_to_string(
    SOC_SAND_IN  SOC_SAND_PP_FEC_COMMAND_TYPE enum_val
  );

void
  soc_sand_SAND_PP_SYS_PORT_ID_table_format_print(
    SOC_SAND_IN SOC_SAND_PP_SYS_PORT_ID *info
  );

const char*
  soc_sand_SAND_PP_L4_PRTCL_TYPE_to_string(
    SOC_SAND_IN  SOC_SAND_PP_L4_PRTCL_TYPE enum_val
  );

const char*
  SOC_SAND_PP_IP_TYPE_to_string(
    SOC_SAND_IN  SOC_SAND_PP_IP_TYPE enum_val
  );
void
  soc_sand_SAND_PP_COS_PARAMS_print(
    SOC_SAND_IN SOC_SAND_PP_COS_PARAMS *info
  );

const char*
  soc_sand_SAND_PP_HUB_SPOKE_ORIENTATION_to_string(
    SOC_SAND_IN  SOC_SAND_PP_HUB_SPOKE_ORIENTATION enum_val
  );

const char*
  soc_sand_SAND_PP_SPLIT_HORIZON_ORIENTATION_to_string(
    SOC_SAND_IN  SOC_SAND_PP_SPLIT_HORIZON_ORIENTATION enum_val
  );

const char*
  soc_sand_SAND_PP_MPLS_TUNNEL_MODEL_to_string(
    SOC_SAND_IN  SOC_SAND_PP_MPLS_TUNNEL_MODEL enum_val
  );

const char*
  soc_sand_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_to_string(
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT enum_val
  );

const char*
  SOC_SAND_PP_ETH_ENCAP_TYPE_to_string(
    SOC_SAND_IN  SOC_SAND_PP_ETH_ENCAP_TYPE enum_val
  );


void
  soc_sand_SAND_PP_DESTINATION_ID_table_format_print(
    SOC_SAND_IN SOC_SAND_PP_DESTINATION_ID *info
  );

void
  soc_sand_SAND_PP_DESTINATION_ID_print(
    SOC_SAND_IN  SOC_SAND_PP_DESTINATION_ID *info
  );

void
  soc_sand_SAND_PP_IPV4_SUBNET_print(
    SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET *info
  );

void
  soc_sand_SAND_PP_IPV4_ADDRESS_print(
    SOC_SAND_IN  SOC_SAND_PP_IPV4_ADDRESS *info
  );

void
  soc_sand_SAND_PP_IPV6_ADDRESS_print(
    SOC_SAND_IN  SOC_SAND_PP_IPV6_ADDRESS *info
  );

void
  soc_sand_SAND_PP_IPV6_SUBNET_print(
    SOC_SAND_IN  SOC_SAND_PP_IPV6_SUBNET *info
  );

void
  soc_sand_SAND_PP_TRILL_DEST_print(
    SOC_SAND_IN  SOC_SAND_PP_TRILL_DEST *info
  );

void
  soc_sand_SAND_PP_IPV4_MC_ROUTE_KEY_print(
    SOC_SAND_IN SOC_SAND_PP_IPV4_MC_ROUTE_KEY *info
  );

void
  soc_sand_SAND_PP_IPV6_ROUTE_KEY_print(
    SOC_SAND_IN SOC_SAND_PP_IPV6_ROUTE_KEY *info
  );

void
  soc_sand_SAND_PP_SYS_PORT_ID_print(
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID *info
  );

void
  SOC_SAND_PP_VLAN_TAG_print(
    SOC_SAND_IN  SOC_SAND_PP_VLAN_TAG *info
  );


#endif 


#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
