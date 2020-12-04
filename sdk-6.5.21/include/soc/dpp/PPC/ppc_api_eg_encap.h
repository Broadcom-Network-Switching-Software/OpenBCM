/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPC_API_EG_ENCAP_INCLUDED__

#define __SOC_PPC_API_EG_ENCAP_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_general.h>

#include <soc/dpp/dpp_config_defs.h>






#define  SOC_PPC_EG_ENCAP_MPLS_MAX_NOF_TUNNELS                      (2)


#define  SOC_PPC_EG_ENCAP_DATA_INFO_MAX_SIZE                        (SOC_DPP_DEFS_MAX(NOF_EEDB_PAYLOADS) / SOC_SAND_NOF_BITS_IN_UINT32 + 1)


#define  SOC_PPC_EG_ENCAP_DATA_PROG_ENTRY_MAX_SIZE                  (4)

 
#define SOC_PPC_EG_ENCAP_NOF_BANKS(unit)                           (SOC_DPP_DEFS_GET(unit,eg_encap_nof_banks)) 
#define SOC_PPC_EG_ENCAP_BANK_NDX_MAX(unit)                        (SOC_PPC_EG_ENCAP_NOF_BANKS(unit)-1) 


#define SOC_PPC_EG_ENCAP_NEXT_EEP_INVALID                        (0xffffffff)


typedef enum
{
    SOC_PPC_EG_ENCAP_ENTRY_UPDATE_INVALID = 0,
    
    SOC_PPC_EG_ENCAP_ENTRY_UPDATE_NEXT_LIF = 0x1,
    
    SOC_PPC_EG_ENCAP_ENTRY_UPDATE_DROP = 0x2,
    
    SOC_PPC_EG_ENCAP_ENTRY_UPDATE_OUT_LIF_PROFILE = 0x4,
    SOC_PPC_EG_ENCAP_ENTRY_UPDATE_TYPE_MAX
    
}SOC_PPC_EG_ENCAP_ENTRY_UPDATE_TYPE;

#define SOC_PPC_PRGE_DATA_ENTRY_LSBS_ERSPAN        (2)
#define SOC_PPC_PRGE_DATA_ENTRY_LSBS_RSPAN         (1)
#define SOC_PPC_PRGE_DATA_ENTRY_LSBS_PON_TUNNEL    (1)
#define SOC_PPC_PRGE_DATA_ENTRY_LSBS_L2_SRC_ENCAP  (1)



#define SOC_PPC_EG_ENCAP_ERSPAN_PRIO_VAL      (0)

#define SOC_PPC_EG_ENCAP_ERSPAN_TRUNC_VAL     (0)


#define PPC_API_EG_ENCAP_PER_PKT_HDR_COMP_NOF_VALUE_LSBS    (6)





  
#define SOC_PPC_EG_ENCAP_EEDB_ERSPAN_FORMAT_SET(__unit, __prio, __trunc, __span_id, __data) \
            (__data)[1] = (((__prio << 21)) | ((__trunc) << 18) | ((__span_id) << 8));                                                    \
          (__data)[0] = SOC_PPC_PRGE_DATA_ENTRY_LSBS_ERSPAN;

#define SOC_PPC_EG_ENCAP_EEDB_DATA_ERSPAN_FORMAT_SET(__unit, __prio, __trunc, __span_id,__eg_encap_data_info)  \
    SOC_PPC_EG_ENCAP_EEDB_ERSPAN_FORMAT_SET(__unit, __prio, __trunc, __span_id, (__eg_encap_data_info)->data_entry)

#define SOC_PPC_EG_ENCAP_DATA_ERSPAN_FORMAT_SPAN_ID_GET(__unit, __eg_encap_data_info)  (((__eg_encap_data_info)->data_entry[1] >> 8)& 0x3ff)


#define SOC_PPC_EG_ENCAP_EEDB_IS_FORMAT_ERSPAN(__eg_encap_data_info)  (((__eg_encap_data_info)->data_entry[0]  & 0xff) == SOC_PPC_PRGE_DATA_ENTRY_LSBS_ERSPAN)






typedef enum
{
  
  SOC_PPC_EG_ENCAP_EEP_TYPE_LIF_EEP = 0,
  
  SOC_PPC_EG_ENCAP_EEP_TYPE_TUNNEL_EEP = 1,
  
  SOC_PPC_EG_ENCAP_EEP_TYPE_LL = 2,
  
  SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_PB = 3,
   
  SOC_PPC_EG_ENCAP_EEP_TYPE_VSI = SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_PB,
  
  SOC_PPC_EG_ENCAP_EEP_TYPE_DATA = 5,
   
  SOC_PPC_EG_ENCAP_EEP_TYPE_ROO_LL = 6, 
   
  SOC_PPC_EG_ENCAP_EEP_TYPE_TRILL = 7, 
  
  SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_ARAD = 8
}SOC_PPC_EG_ENCAP_EEP_TYPE;

#define SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX      SOC_SAND_MAX(SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_PB, SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_ARAD)

typedef enum
{
  
  SOC_PPC_EG_ENCAP_EXP_MARK_MODE_FROM_PUSH_PROFILE = 0,
  
  SOC_PPC_EG_ENCAP_EXP_MARK_MODE_MAP_TC_DP = 1,
  
  SOC_PPC_NOF_EG_ENCAP_EXP_MARK_MODES = 2
}SOC_PPC_EG_ENCAP_EXP_MARK_MODE;

typedef enum
{
  
  SOC_PPC_EG_ENCAP_ENTRY_TYPE_VSI = 0,
  
  SOC_PPC_EG_ENCAP_ENTRY_TYPE_AC = 1,
  
  SOC_PPC_EG_ENCAP_ENTRY_TYPE_SWAP_CMND = 2,
  
  SOC_PPC_EG_ENCAP_ENTRY_TYPE_PWE = 3,
  
  SOC_PPC_EG_ENCAP_ENTRY_TYPE_POP_CMND = 4,
  
  SOC_PPC_EG_ENCAP_ENTRY_TYPE_MPLS_ENCAP = 5,
  
  SOC_PPC_EG_ENCAP_ENTRY_TYPE_IPV4_ENCAP = 6,
  
  SOC_PPC_EG_ENCAP_ENTRY_TYPE_LL_ENCAP = 7,
  
  SOC_PPC_EG_ENCAP_ENTRY_TYPE_NULL = 8,
  
  SOC_PPC_NOF_EG_ENCAP_ENTRY_TYPES_PB = 9,
  
  SOC_PPC_EG_ENCAP_ENTRY_TYPE_DATA = 9,
  
  SOC_PPC_EG_ENCAP_ENTRY_TYPE_IPV6_ENCAP = 10,
  
  SOC_PPC_EG_ENCAP_ENTRY_TYPE_MIRROR_ENCAP = 11,
  
  SOC_PPC_EG_ENCAP_ENTRY_TYPE_OVERLAY_ARP_ENCAP = 12, 
  
    SOC_PPC_EG_ENCAP_ENTRY_TYPE_ROO_LL_ENCAP = 13, 
  
    SOC_PPC_EG_ENCAP_ENTRY_TYPE_TRILL_ENCAP = 14, 
  
  SOC_PPC_NOF_EG_ENCAP_ENTRY_TYPES_ARAD = 15 
}SOC_PPC_EG_ENCAP_ENTRY_TYPE;

typedef enum
{
  
  SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_NONE = 0,
  
  SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_ETHER_IP= 1,
  
      SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_BASIC_GRE = 2,
  
  SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_ENHANCE_GRE = 3,
  
  SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_VXLAN = 4,
  
  SOC_PPC_NOF_EG_ENCAP_ENCAPSULATION_MODE_TYPES = 5

}SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE;

typedef enum {
    
    SOC_PPC_EG_ENCAP_ACCESS_PHASE_TYPE_TUNNEL_1,
    
    SOC_PPC_EG_ENCAP_ACCESS_PHASE_TYPE_TUNNEL_2,
    
    SOC_PPC_EG_ENCAP_ACCESS_PHASE_TYPE_LL_ARP,
    
    SOC_PPC_EG_ENCAP_ACCESS_PHASE_TYPE_AC,
    
    SOC_PPC_EG_ENCAP_ACCESS_PHASE_TYPE_NATIVE_LL_ARP,
    
    SOC_PPC_EG_ENCAP_ACCESS_PHASE_TYPE_NATIVE_AC,
    
    SOC_PPC_EG_ENCAP_ACCESS_PHASE_TYPE_COUNT
} SOC_PPC_EG_ENCAP_ACCESS_PHASE_TYPE;

typedef enum
{
  
  SOC_PPC_EG_ENCAP_ACCESS_PHASE_ONE = 0,
   
  SOC_PPC_EG_ENCAP_ACCESS_PHASE_TWO = 1,
  
  SOC_PPC_EG_ENCAP_ACCESS_PHASE_THREE = 2,
  
  SOC_PPC_EG_ENCAP_ACCESS_PHASE_FOUR = 3,
  
  SOC_PPC_EG_ENCAP_ACCESS_PHASE_FIVE = 4,
  
  SOC_PPC_EG_ENCAP_ACCESS_PHASE_SIX = 5,
  
  SOC_PPC_EG_ENCAP_ACCESS_PHASE_COUNT = 6
}SOC_PPC_EG_ENCAP_ACCESS_PHASE;


#define SOC_PPC_NOF_EG_ENCAP_ACCESS_PHASE_TYPES(unit)  (SOC_DPP_DEFS_GET(unit, eg_encap_nof_phases))

typedef enum
{
  
  SOC_PPC_EG_ENCAP_DATA_TYPE_MIRROR_ERSPAN = 0,
  
  SOC_PPC_EG_ENCAP_DATA_TYPE_IPV6_TUNNEL = 1,
  
  SOC_PPC_EG_ENCAP_DATA_TYPE_IPV6_TUNNEL_DIP = 2,
  
  SOC_PPC_EG_ENCAP_DATA_TYPE_IPV6_TUNNEL_SIP = 3,
  
  SOC_PPC_NOF_EG_ENCAP_DATA_TYPES = 4
}SOC_PPC_EG_ENCAP_DATA_TYPE;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 ll_limit;

  
  uint32 ip_tnl_limit;

   
  SOC_PPC_EG_ENCAP_ACCESS_PHASE bank_access_phase[SOC_DPP_DEFS_MAX(EG_ENCAP_NOF_BANKS)];

} SOC_PPC_EG_ENCAP_RANGE_INFO;


typedef struct
{
    SOC_SAND_MAGIC_NUM_VAR
     
    uint32 is_protection_valid; 
     
    uint32 protection_pointer; 
     
    uint8 protection_pass_value; 
} SOC_PPC_EG_ENCAP_PROTECTION_INFO;



typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_SAND_PP_VLAN_ID vid;
  
  SOC_SAND_PP_PCP_UP pcp;
  
  SOC_SAND_PP_DEI_CFI dei;

} SOC_PPC_EG_VLAN_EDIT_VLAN_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_EG_VLAN_EDIT_VLAN_INFO vlan_tags[SOC_PPC_VLAN_TAGS_MAX];
  
  uint32 nof_tags;
  
  uint32 edit_profile;
  
  uint32 pcp_profile;

  uint32 lif_profile;

   
  uint32 oam_lif_set;
#ifdef BCM_88660_A0
  
  uint32 use_as_data_entry;

  
  uint32 data[SOC_PPC_EG_ENCAP_DATA_INFO_MAX_SIZE];
#endif
} SOC_PPC_EG_AC_VLAN_EDIT_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_EG_AC_VLAN_EDIT_INFO edit_info;
   
  SOC_PPC_EG_ENCAP_PROTECTION_INFO protection_info;

} SOC_PPC_EG_AC_INFO;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 swap_label;
  
  SOC_PPC_VSI_ID out_vsi;
   
  uint32 oam_lif_set;
   
  uint32 drop;
  
  uint32 outlif_profile;
  
  uint32 remark_profile;
} SOC_PPC_EG_ENCAP_SWAP_INFO;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 tpid_profile;
  
  uint8 has_cw;

} SOC_PPC_EG_ENCAP_POP_INTO_ETH_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_MPLS_COMMAND_TYPE pop_type;
  
  SOC_PPC_EG_ENCAP_POP_INTO_ETH_INFO ethernet_info;
  
  SOC_PPC_PKT_FRWRD_TYPE pop_next_header;
  
  SOC_SAND_PP_MPLS_TUNNEL_MODEL model;
   
  uint32 oam_lif_set;
   
  uint32 drop;
  
  uint32 outlif_profile;
  
  uint32 out_vsi;
} SOC_PPC_EG_ENCAP_POP_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 tunnel_label;
  
  uint32 push_profile;
} SOC_PPC_EG_ENCAP_MPLS_TUNNEL_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_EG_ENCAP_MPLS_TUNNEL_INFO tunnels[SOC_PPC_EG_ENCAP_MPLS_MAX_NOF_TUNNELS];
  
  uint32 nof_tunnels;
  
  SOC_SAND_PP_HUB_SPOKE_ORIENTATION orientation;
  
  SOC_PPC_VSI_ID out_vsi;
   
  uint32 oam_lif_set;
   
  uint32 drop;
  
  uint32 outlif_profile;
   
  SOC_PPC_EG_ENCAP_PROTECTION_INFO protection_info;
  
  uint32 addition_label;

} SOC_PPC_EG_ENCAP_MPLS_ENCAP_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 label;
  
  uint32 push_profile;
  
  SOC_SAND_PP_HUB_SPOKE_ORIENTATION orientation;
   
  uint32 oam_lif_set;
   
  uint32 drop;
   
  uint32 outlif_profile;
   
  SOC_PPC_EG_ENCAP_PROTECTION_INFO protection_info;

  
  SOC_PPC_EG_ENCAP_MPLS_ENCAP_INFO egress_tunnel_label_info;

} SOC_PPC_EG_ENCAP_PWE_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 dest;
  
  uint32 src_index;
  
  uint8 ttl_index;
  
  uint8 tos_index;
  
  uint8 enable_gre;
   
  SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE encapsulation_mode;
  
   int encapsulation_mode_index; 

} SOC_PPC_EG_ENCAP_IPV4_TUNNEL_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_EG_ENCAP_IPV4_TUNNEL_INFO dest;
  
  SOC_PPC_VSI_ID out_vsi;
   
  uint32 oam_lif_set;
   
  uint32 drop;
  
  uint32 outlif_profile;

} SOC_PPC_EG_ENCAP_IPV4_ENCAP_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_SAND_PP_MAC_ADDRESS dest_mac;
  
  uint8 out_vid_valid;
  
  SOC_SAND_PP_VLAN_ID out_vid;
  
  uint8 pcp_dei_valid;
  
  uint8 pcp_dei;
  
  uint8 tpid_index;
   
  uint8 ll_remark_profile; 
  
  uint8 out_ac_valid;
  
  uint32  out_ac_lsb;
   
  uint32 oam_lif_set;
   
  uint32 drop;
  
  uint32 outlif_profile; 
  
  uint32 native_ll; 
}SOC_PPC_EG_ENCAP_LL_INFO;




typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_SAND_PP_MPLS_TUNNEL_MODEL model;
  
  uint8 has_cw;
  
  SOC_SAND_PP_IP_TTL ttl;
  
  SOC_PPC_EG_ENCAP_EXP_MARK_MODE exp_mark_mode;
  
  SOC_SAND_PP_MPLS_EXP exp;
  
  uint8 remark_profile;
  
  uint8 add_entropy_label;
  
  uint8 add_entropy_label_indicator;
  
  SOC_SAND_PP_MPLS_TUNNEL_MODEL ttl_model;
  
  SOC_SAND_PP_MPLS_TUNNEL_MODEL exp_model;
  
  uint8 rsv_mpls_label_bmp;
} SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO;

typedef struct 
{ 
  SOC_SAND_MAGIC_NUM_VAR 
   
  uint8 remark_profile; 
   
  SOC_PPC_VSI_ID out_vsi; 
   
  uint32 oam_lif_set;
   
  uint32 drop;
  
  uint32 outlif_profile;

  
  uint32 outrif_profile_index;

} SOC_PPC_EG_ENCAP_VSI_ENCAP_INFO; 



typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 data_entry[SOC_PPC_EG_ENCAP_DATA_INFO_MAX_SIZE];
   
  uint32 oam_lif_set;
   
  uint32 drop;
  
  uint32 outlif_profile;
  
  uint32 rsv_bits;
} SOC_PPC_EG_ENCAP_DATA_INFO;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  
  SOC_SAND_PP_MAC_ADDRESS dest_mac;

  
  SOC_SAND_PP_MAC_ADDRESS src_mac;

  
  uint16 outer_tpid; 

  
  uint8 pcp_dei;

  
  SOC_SAND_PP_VLAN_ID out_vid;

   
  uint16 ether_type;

   
  uint8 ll_vsi_pointer; 

  
  SOC_SAND_PP_VLAN_ID inner_vid;

  
  uint8 inner_pcp_dei;

  
  uint8 pcp_dei_profile; 

  
  uint8 remark_profile;

  
  int eth_type_index; 

  
  int nof_tags; 

}SOC_PPC_EG_ENCAP_OVERLAY_ARP_ENCAP_INFO;


typedef struct 
{
  uint16 ether_type; 
  
  uint16 tpid_0;
   
  uint16 tpid_1; 
}SOC_PPC_EG_ENCAP_ETHER_TYPE_INDEX_INFO; 


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_SAND_PP_IPV6_ADDRESS dest;
  
  SOC_SAND_PP_IPV6_ADDRESS src;
  
  uint16 hop_limit;
  
  uint16 flow_label;
  
  uint8 next_header;
  
  uint8 sip_index;
  
  uint8 traffic_class;
} SOC_PPC_EG_ENCAP_IPV6_TUNNEL_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
   SOC_PPC_EG_ENCAP_IPV6_TUNNEL_INFO tunnel;

} SOC_PPC_EG_ENCAP_IPV6_ENCAP_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_SAND_PP_MAC_ADDRESS dest;
  
  SOC_SAND_PP_MAC_ADDRESS src;
  
  uint16 tpid;
  
  uint16 vid;
  
  uint8 pcp;
  
  uint16 ether_type;
  
  uint16 erspan_id;
   
  uint16 encap_id;

} SOC_PPC_EG_ENCAP_MIRROR_TUNNEL_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
   SOC_PPC_EG_ENCAP_MIRROR_TUNNEL_INFO tunnel;

} SOC_PPC_EG_ENCAP_MIRROR_ENCAP_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 data_entry[SOC_PPC_EG_ENCAP_DATA_PROG_ENTRY_MAX_SIZE];

} SOC_PPC_EG_ENCAP_DATA_PROG_ENTRY_INFO;



typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 push_profile;
  
  SOC_SAND_PP_TC tc;
  
  SOC_SAND_PP_DP dp;

} SOC_PPC_EG_ENCAP_PUSH_EXP_KEY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 cw;

} SOC_PPC_EG_ENCAP_PWE_GLBL_INFO;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint16 l2_gre_prtcl_type;
  
  uint16 unkown_gre_prtcl_type;
  
  uint16 unkown_ip_next_prtcl_type;
  
  uint16 eth_ip_val;
  
  uint16 unkown_ll_ether_type;
  
  
  uint16 vxlan_udp_dest_port;
  
  uint16 mpls_entry_label_msbs;
  
  uint16 custom1_ether_type;
  
  uint16 custom2_ether_type;

} SOC_PPC_EG_ENCAP_GLBL_INFO;



typedef struct 
{
    SOC_SAND_MAGIC_NUM_VAR

    
    uint8 my_nickname_index; 
    
    uint8 m; 
    
    uint16 nick; 
    
    uint32 outlif_profile;
    
    uint32                       ll_eep_ndx;
    
    uint8                        ll_eep_is_valid;
    
    uint8 remark_profile;

} SOC_PPC_EG_ENCAP_TRILL_INFO; 


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_VSI_ID vsi;
  
  SOC_PPC_EG_AC_INFO out_ac;
  
  SOC_PPC_EG_ENCAP_SWAP_INFO swap_info;
  
  SOC_PPC_EG_ENCAP_PWE_INFO pwe_info;
  
  SOC_PPC_EG_ENCAP_POP_INFO pop_info;
  
  SOC_PPC_EG_ENCAP_MPLS_ENCAP_INFO mpls_encap_info;
  
  SOC_PPC_EG_ENCAP_IPV4_ENCAP_INFO ipv4_encap_info;
  
  SOC_PPC_EG_ENCAP_IPV6_ENCAP_INFO ipv6_encap_info;
  
  SOC_PPC_EG_ENCAP_MIRROR_ENCAP_INFO mirror_encap_info;
  
  SOC_PPC_EG_ENCAP_LL_INFO ll_info;
  
  SOC_PPC_EG_ENCAP_VSI_ENCAP_INFO vsi_info;
  
  SOC_PPC_EG_ENCAP_DATA_INFO data_info;
   
  SOC_PPC_EG_ENCAP_OVERLAY_ARP_ENCAP_INFO overlay_arp_encap_info;
  
  SOC_PPC_EG_ENCAP_TRILL_INFO trill_encap_info; 
} SOC_PPC_EG_ENCAP_ENTRY_VALUE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_EG_ENCAP_ENTRY_TYPE entry_type;
  
  SOC_PPC_EG_ENCAP_ENTRY_VALUE entry_val;

} SOC_PPC_EG_ENCAP_ENTRY_INFO;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 int_pri;   
  
  uint32 int_pri_valid;   
        
  bcm_color_t color;         
        
  uint32 color_valid;  
} SOC_PPC_EG_MAP_ENCAP_INTPRI_COLOR_INFO;


typedef struct 
{
     
    SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE ip_tunnel_template; 
    
    uint32 encapsulation_size; 
    
    uint8 protocol; 
    
    uint8 protocol_enable; 
} SOC_PPC_EG_ENCAP_IP_TUNNEL_SIZE_PROTOCOL_TEMPLATE_INFO; 


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_SAND_PP_IP_TTL ttl;
  
  SOC_SAND_PP_MPLS_EXP exp;
  
  SOC_SAND_PP_MPLS_TUNNEL_MODEL ttl_model;
  
  SOC_SAND_PP_MPLS_TUNNEL_MODEL exp_model;
  
  uint8 label_type;
  
  bcm_mpls_label_t label;
} SOC_PPC_EG_ENCAP_ADDITIONAL_LABEL_PROFILE_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 v6;
  
  SOC_SAND_PP_IPV6_ADDRESS src_ip;
} SOC_PPC_EG_ENCAP_IP_TUNNEL_SRC_IP;









void
  SOC_PPC_EG_ENCAP_RANGE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_RANGE_INFO *info
  );

void
  SOC_PPC_EG_ENCAP_PROTECTION_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_PROTECTION_INFO   *info
  );

void
  SOC_PPC_EG_ENCAP_SWAP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_SWAP_INFO *info
  );

void
  SOC_PPC_EG_ENCAP_PWE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_PWE_INFO *info
  );

void
  SOC_PPC_EG_ENCAP_POP_INTO_ETH_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_POP_INTO_ETH_INFO *info
  );

void
  SOC_PPC_EG_ENCAP_POP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_POP_INFO *info
  );

void
  SOC_PPC_EG_ENCAP_MPLS_TUNNEL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_MPLS_TUNNEL_INFO *info
  );

void
  SOC_PPC_EG_ENCAP_MPLS_ENCAP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_MPLS_ENCAP_INFO *info
  );

void
  SOC_PPC_EG_ENCAP_IPV4_TUNNEL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_IPV4_TUNNEL_INFO *info
  );

void
  SOC_PPC_EG_ENCAP_IPV4_ENCAP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_IPV4_ENCAP_INFO *info
  );

void
  SOC_PPC_EG_ENCAP_IPV6_TUNNEL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_IPV6_TUNNEL_INFO *info
  );

void
  SOC_PPC_EG_ENCAP_IPV6_ENCAP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_IPV6_ENCAP_INFO *info
  );

void
  SOC_PPC_EG_ENCAP_MIRROR_TUNNEL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_MIRROR_TUNNEL_INFO *info
  );

void
  SOC_PPC_EG_ENCAP_MIRROR_ENCAP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_MIRROR_ENCAP_INFO *info
  );

void
  SOC_PPC_EG_ENCAP_LL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_LL_INFO *info
  );

void
  SOC_PPC_EG_ENCAP_ENTRY_VALUE_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_ENTRY_VALUE *info
  );

void
  SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO *info
  );

void
  SOC_PPC_EG_ENCAP_ADDITIONAL_LABEL_PROFILE_INFO_clear(
    SOC_PPC_EG_ENCAP_ADDITIONAL_LABEL_PROFILE_INFO *info
  );

void 
  SOC_PPC_EG_ENCAP_VSI_ENCAP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_VSI_ENCAP_INFO *info
  );

void 
  SOC_PPC_EG_ENCAP_DATA_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_DATA_INFO *info
  );

void 
  SOC_PPC_EG_ENCAP_DATA_PROG_ENTRY_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_DATA_PROG_ENTRY_INFO *info
  );

void
  SOC_PPC_EG_ENCAP_PUSH_EXP_KEY_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_PUSH_EXP_KEY *info
  );

void
  SOC_PPC_EG_ENCAP_PWE_GLBL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_PWE_GLBL_INFO *info
  );

void
  SOC_PPC_EG_ENCAP_GLBL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_GLBL_INFO *info
  );

void
  SOC_PPC_EG_ENCAP_ENTRY_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_ENTRY_INFO *info
  );

void
  SOC_PPC_EG_ENCAP_OVERLAY_ARP_ENCAP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_OVERLAY_ARP_ENCAP_INFO *ll_encap_info
  );

void 
  SOC_PPC_EG_ENCAP_TRILL_INFO_clear(
     SOC_SAND_OUT SOC_PPC_EG_ENCAP_TRILL_INFO *info
  ); 

void 
SOC_PPC_EG_ENCAP_IP_TUNNEL_SIZE_PROTOCOL_TEMPLATE_INFO_clear(
   SOC_PPC_EG_ENCAP_IP_TUNNEL_SIZE_PROTOCOL_TEMPLATE_INFO *info
   ); 

#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_EG_ENCAP_EEP_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_EEP_TYPE enum_val
  );

const char*
  SOC_PPC_EG_ENCAP_ACCESS_PHASE_to_string(
    SOC_SAND_IN SOC_PPC_EG_ENCAP_ACCESS_PHASE enum_val
  );

const char*
  SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_to_string(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE enum_val
  );

const char*
  SOC_PPC_EG_ENCAP_EXP_MARK_MODE_to_string(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_EXP_MARK_MODE enum_val
  );

const char*
  SOC_PPC_EG_ENCAP_ENTRY_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_ENTRY_TYPE enum_val
  );

void
  SOC_PPC_EG_ENCAP_RANGE_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_RANGE_INFO *info
  );

void
  SOC_PPC_EG_ENCAP_PROTECTION_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PROTECTION_INFO *info
  );

void
  SOC_PPC_EG_ENCAP_SWAP_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_SWAP_INFO *info
  );

void
  SOC_PPC_EG_ENCAP_VSI_ENCAP_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_VSI_ENCAP_INFO *info
  );

void
  SOC_PPC_EG_ENCAP_DATA_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_DATA_INFO *info
  );

void
  SOC_PPC_EG_ENCAP_DATA_PROG_ENTRY_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_DATA_PROG_ENTRY_INFO *info
  );

void
  SOC_PPC_EG_ENCAP_PWE_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PWE_INFO *info
  );

void
  SOC_PPC_EG_ENCAP_POP_INTO_ETH_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_POP_INTO_ETH_INFO *info
  );

void
  SOC_PPC_EG_ENCAP_POP_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_POP_INFO *info
  );

void
  SOC_PPC_EG_ENCAP_MPLS_TUNNEL_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_MPLS_TUNNEL_INFO *info
  );

void
  SOC_PPC_EG_ENCAP_MPLS_ENCAP_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_MPLS_ENCAP_INFO *info
  );

void
  SOC_PPC_EG_ENCAP_IPV4_TUNNEL_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_IPV4_TUNNEL_INFO *info
  );

void
  SOC_PPC_EG_ENCAP_IPV4_ENCAP_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_IPV4_ENCAP_INFO *info
  );

void
  SOC_PPC_EG_ENCAP_IPV6_TUNNEL_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_IPV6_TUNNEL_INFO *info
  );

void
  SOC_PPC_EG_ENCAP_IPV6_ENCAP_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_IPV6_ENCAP_INFO *info
  );

void
  SOC_PPC_EG_ENCAP_MIRROR_TUNNEL_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_MIRROR_TUNNEL_INFO *info
  );

void
  SOC_PPC_EG_ENCAP_MIRROR_ENCAP_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_MIRROR_ENCAP_INFO *info
  );

void
  SOC_PPC_EG_ENCAP_LL_INFO_print(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_LL_INFO *info
  );

void
  SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO *info
  );

void
  SOC_PPC_EG_ENCAP_ADDITIONAL_LABEL_PROFILE_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_ADDITIONAL_LABEL_PROFILE_INFO *info
  );

void
  SOC_PPC_EG_ENCAP_PUSH_EXP_KEY_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PUSH_EXP_KEY *info
  );

void
  SOC_PPC_EG_ENCAP_PWE_GLBL_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PWE_GLBL_INFO *info
  );

void
  SOC_PPC_EG_ENCAP_GLBL_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_GLBL_INFO *info
  );

void
  SOC_PPC_EG_ENCAP_ENTRY_VALUE_print(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_ENTRY_VALUE *info
  );

void
  SOC_PPC_EG_ENCAP_ENTRY_INFO_print(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_ENTRY_INFO *info
  );

void
  SOC_PPC_EG_ENCAP_TRILL_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_TRILL_INFO *info
  );


void
  SOC_PPC_EG_VLAN_EDIT_VLAN_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_VLAN_INFO *info
  );

void
  SOC_PPC_EG_AC_VLAN_EDIT_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_AC_VLAN_EDIT_INFO *info
  );

void
  SOC_PPC_EG_AC_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_AC_INFO *info
  );


#endif 

void
  SOC_PPC_EG_MAP_ENCAP_INTPRI_COLOR_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_MAP_ENCAP_INTPRI_COLOR_INFO *info
  );

void 
  SOC_PPC_EG_ENCAP_IP_TUNNEL_SRC_IP_clear(
   SOC_SAND_OUT SOC_PPC_EG_ENCAP_IP_TUNNEL_SRC_IP *info
   );


#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
