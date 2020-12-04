/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPC_API_LIF_INCLUDED__

#define __SOC_PPC_API_LIF_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_general.h>
#include <soc/dpp/PPC/ppc_api_llp_parse.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_occupation_mgmt.h>





#define  SOC_PPC_LIF_IGNORE_INNER_VID (0xFFFFFFFF)


#define  SOC_PPC_LIF_IGNORE_OUTER_VID (0xFFFFFFFF)


#define  SOC_PPC_LIF_IGNORE_TUNNEL_VID (0xFFFFFFFF)


#define  SOC_PPC_LIF_IGNORE_ETHER_TYPE (0xFFFF)


#define  SOC_PPC_LIF_IGNORE_OUTER_PCP (0xFF)


#define  SOC_PPC_LIF_VLAN_RANGE_MAX_SIZE (32)


#define  SOC_PPC_LIF_AC_GROUP_MAX_SIZE (8)


#define  SOC_PPC_ISID_DM_DISABLE          (0xFFFF)


#define SOC_PPC_INVALID_GLOBAL_LIF      (0x3ffff)


#define  SOC_PPC_VSI_MAX_NOF_ARAD           (32768)


#define  SOC_PPC_VSI_P2P_SERVICE            (16 * 1024 - 2)
#define  SOC_PPC_VSI_P2P_SERVICE_ARAD       (SOC_PPC_VSI_MAX_NOF_ARAD - 2)


#define  SOC_PPC_VSI_EQ_IN_VID              (SOC_PPC_VSI_MAX_NOF_ARAD - 1)





#define  SOC_PPC_VSI_EQ_VSI_BASE          (0x0)

#define  SOC_PPC_VSI_EQ_IN_LIF          (0x3)

#define  SOC_PPC_VSI_EQ_IN_VID_PLUS_VSI_BASE (0x1)




#define  SOC_PPC_VC_LABEL_INDEXED_LABEL_INDEX_SHIFT                      20
#define  SOC_PPC_VC_LABEL_INDEXED_LABEL_INDEX_MASK                       0x3
#define  SOC_PPC_VC_LABEL_INDEXED_LABEL_VALUE_SHIFT                      0
#define  SOC_PPC_VC_LABEL_INDEXED_LABEL_VALUE_MASK                       0xFFFFF


#define  SOC_PPC_LIF_AC_SPOOF_MODE_ENABLE  (1)
#define  SOC_PPC_FLP_COS_PROFILE_ANTI_SPOOFING       (0x1 << 5)
#define  SOC_PPC_LIF_AC_SPOOF_MODE_ENABLE_SET                   (SOC_PPC_LIF_AC_SPOOF_MODE_ENABLE << 5)
#define  SOC_PPC_LIF_AC_SPOOF_MODE_DISABLE_SET                  (~SOC_PPC_LIF_AC_SPOOF_MODE_ENABLE_SET)


#define SOC_PPC_LIF_AC_VAMC_COS_PROFILE_DISABLE         0
#define SOC_PPC_LIF_AC_VAMC_COS_PROFILE_ENABLE          1
#define SOC_PPC_LIF_AC_VAMC_COS_PROFILE_LSB             5
#define SOC_PPC_LIF_AC_VAMC_COS_PROFILE_MSB             5
#define SOC_PPC_LIF_AC_VAMC_COS_PROFILE_LSB_NOF_BITS    (SOC_PPC_LIF_AC_VAMC_COS_PROFILE_MSB - \
                                                         SOC_PPC_LIF_AC_VAMC_COS_PROFILE_LSB + 1)
#define SOC_PPC_FLP_COS_PROFILE_VMAC                    (0x1 << 5)


#define  SOC_PPC_LIF_AC_LOCAL_SWITCHING_COS_PROFILE  (0x1 << 5)


#define  SOC_PPC_LIF_AC_L2_SSM_IPMC_COS_PROFILE  (0x1 << 5)


#define SOC_PPC_LIF_NULL_LOCAL_OUTLIF_ID (0)







#define SOC_PPC_VC_LABEL_INDEXED_SET(label, vc_label_value, vc_label_index)               \
          ((label) = (((vc_label_index) & SOC_PPC_VC_LABEL_INDEXED_LABEL_INDEX_MASK)  << SOC_PPC_VC_LABEL_INDEXED_LABEL_INDEX_SHIFT)  | \
        (((vc_label_value) & SOC_PPC_VC_LABEL_INDEXED_LABEL_VALUE_MASK) << SOC_PPC_VC_LABEL_INDEXED_LABEL_VALUE_SHIFT))

#define SOC_PPC_VC_LABEL_INDEXED_LABEL_VALUE_GET(label)   \
        (((label) >> SOC_PPC_VC_LABEL_INDEXED_LABEL_VALUE_SHIFT) & SOC_PPC_VC_LABEL_INDEXED_LABEL_VALUE_MASK)

#define SOC_PPC_VC_LABEL_INDEXED_LABEL_INDEX_GET(label)   \
        (((label) >> SOC_PPC_VC_LABEL_INDEXED_LABEL_INDEX_SHIFT) & SOC_PPC_VC_LABEL_INDEXED_LABEL_INDEX_MASK)




typedef enum
{
  
  SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT = 0,
  
  SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN = 1,
  
  SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN_VLAN = 2,
  
  SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_RAW = 3,
  
  SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_RAW_VLAN = 4,
  
  SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_RAW_VLAN_VLAN = 5,
  
  SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_COMP_VLAN = 6,
  
  SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_COMP_VLAN_COMP_VLAN = 7,
   
  SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_DEFAULT = 8,
  
  SOC_PPC_NOF_L2_LIF_AC_MAP_KEY_TYPES_PB = 8,
  
  SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_INITIAL_VLAN = 9,
  
  SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_PCP_VLAN = 10,
  
  SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_PCP_VLAN_VLAN = 11,

  
  SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_TUNNEL = 12,

  
  SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_TUNNEL_COMP_VLAN = 13,

  
  SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_TUNNEL_COMP_VLAN_COMP_VLAN = 14,

  
  SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_TUNNEL_PCP_ETHER_TYPE_COMP_VLAN_COMP_VLAN = 15,

  
  SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_TLS_PORT_TUNNEL = 16,

  

  SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_NONE = 17,

  
  SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_TRILL_DESIGNATED_VLAN = 18,
   
  SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_FLEXIBLE_Q_IN_Q_PORT_INITIAL_VID = 19,

   
  SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_FLEXIBLE_Q_IN_Q_PORT_INITIAL_VID_VID = 20,
  
  SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_UNTAG = 21,
  SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_TEST2 = 22,

  
  SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN_ETH = 23,

  
  SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN_VLAN_ETH = 24,
  
  SOC_PPC_NOF_L2_LIF_AC_MAP_KEY_TYPES_ARAD = 24
}SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE;

typedef enum
{
  
  SOC_PPC_L2_LIF_DFLT_FRWRD_SRC_VSI = 0,
  
  SOC_PPC_L2_LIF_DFLT_FRWRD_SRC_LIF = 1,
  
  SOC_PPC_NOF_L2_LIF_DFLT_FRWRD_SRCS = 2
}SOC_PPC_L2_LIF_DFLT_FRWRD_SRC;

typedef enum
{
  
  SOC_PPC_L2_LIF_AC_SERVICE_TYPE_MP = 0,
  
  SOC_PPC_L2_LIF_AC_SERVICE_TYPE_AC2AC = 1,
  
  SOC_PPC_L2_LIF_AC_SERVICE_TYPE_AC2PWE = 2,
  
  SOC_PPC_L2_LIF_AC_SERVICE_TYPE_AC2PBB = 3,
  
  SOC_PPC_NOF_L2_LIF_AC_SERVICE_TYPES = 4
}SOC_PPC_L2_LIF_AC_SERVICE_TYPE;

typedef enum
{
  
  SOC_PPC_L2_LIF_PWE_SERVICE_TYPE_MP = 0,
  
  SOC_PPC_L2_LIF_PWE_SERVICE_TYPE_P2P = 1,
  
  SOC_PPC_L2_LIF_PWE_SERVICE_TYPE_P2P_CUSTOM = 2,
  
  SOC_PPC_NOF_L2_LIF_PWE_SERVICE_TYPES = 3
}SOC_PPC_L2_LIF_PWE_SERVICE_TYPE;

typedef enum
{
  
  SOC_PPC_L2_LIF_ISID_SERVICE_TYPE_P2P = 0,
  
  SOC_PPC_L2_LIF_ISID_SERVICE_TYPE_MP = 1,
  
  SOC_PPC_NOF_L2_LIF_ISID_SERVICE_TYPES = 2
}SOC_PPC_L2_LIF_ISID_SERVICE_TYPE;

typedef enum
{
  
  SOC_PPC_L2_LIF_AC_LEARN_DISABLE = 0,
  
  SOC_PPC_L2_LIF_AC_LEARN_SYS_PORT = 1,
  
  SOC_PPC_L2_LIF_AC_LEARN_INFO = 2,
  
  SOC_PPC_L2_LIF_AC_LEARN_SYS_PORT_LIF = 3,
  
  SOC_PPC_NOF_L2_LIF_AC_LEARN_TYPES = 4
}SOC_PPC_L2_LIF_AC_LEARN_TYPE;

typedef enum
{
  
  SOC_PPC_L2_LIF_L2CP_HANDLE_TYPE_NORMAL = 0,
  
  SOC_PPC_L2_LIF_L2CP_HANDLE_TYPE_TUNNEL = 1,
  
  SOC_PPC_L2_LIF_L2CP_HANDLE_TYPE_PEER = 2,
  
  SOC_PPC_L2_LIF_L2CP_HANDLE_TYPE_DROP = 3,
  
  SOC_PPC_NOF_L2_LIF_L2CP_HANDLE_TYPES = 4
} SOC_PPC_L2_LIF_L2CP_HANDLE_TYPE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_SAND_PP_MPLS_LABEL first_label;
  
  SOC_SAND_PP_MPLS_LABEL last_label;

} SOC_PPC_L2_LIF_IN_VC_RANGE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 port_profile;
  
  SOC_PPC_LLP_PARSE_INFO pkt_parse_info;

} SOC_PPC_L2_LIF_AC_KEY_QUALIFIER;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_L2_LIF_DFLT_FRWRD_SRC default_frwd_type;
  
  SOC_PPC_FRWRD_DECISION_INFO default_forwarding;

} SOC_PPC_L2_LIF_DEFAULT_FORWARDING_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 enable_learning;
  
  SOC_PPC_FRWRD_DECISION_INFO learn_info;

} SOC_PPC_L2_LIF_PWE_LEARN_RECORD;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 inner_vid_valid;
  
  uint16 inner_vid;
  
  uint8 outer_vid_valid;
  
  uint16 outer_vid;
   
uint16  vlan_domain;
   
uint16  vlan_domain_valid;
} SOC_PPC_L2_LIF_PWE_ADDITIONAL_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_VSI_ID vsid;
  
  SOC_PPC_L2_LIF_DEFAULT_FORWARDING_INFO default_frwrd;
  
  SOC_PPC_L2_LIF_PWE_LEARN_RECORD learn_record;
  
  SOC_SAND_PP_HUB_SPOKE_ORIENTATION orientation;
  
  uint32 tpid_profile_index;
  
  uint32 cos_profile;
  
  uint8 has_cw;
  
  SOC_PPC_L2_LIF_PWE_SERVICE_TYPE service_type;
   
  uint32 vsi_assignment_mode;
   
  uint8 use_lif; 
   
  uint32 lif_profile; 
    
  uint32 protection_pointer; 
   
  uint8 protection_pass_value; 
   
  uint32 default_forward_profile; 
   
  uint8 oam_valid;
   
  uint8 term_profile; 
   
  uint8 action_profile;
    
  SOC_SAND_PP_MPLS_TUNNEL_MODEL model;
  
  uint8 has_gal;
  
  uint32 lif_table_entry_in_lif_field;

  
  uint8 is_extended;

  
  int ext_lif_id;

  
  int global_lif_id;

  
  int network_group;
} SOC_PPC_L2_LIF_PWE_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_PORT local_port_ndx;
  
  uint8 is_outer;
  
  SOC_SAND_PP_VLAN_ID first_vid;
  
  SOC_SAND_PP_VLAN_ID last_vid;
  
  int           core_id;


} SOC_PPC_L2_VLAN_RANGE_KEY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_SAND_PP_VLAN_ID first_vid;
  
  SOC_SAND_PP_VLAN_ID last_vid;

} SOC_PPC_L2_VLAN_RANGE_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_L2_VLAN_RANGE_INFO outer_vlan_range[SOC_PPC_LIF_VLAN_RANGE_MAX_SIZE];

  
  SOC_PPC_L2_VLAN_RANGE_INFO inner_vlan_range[SOC_PPC_LIF_VLAN_RANGE_MAX_SIZE];
  
} SOC_PPC_L2_PORT_VLAN_RANGE_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE key_type;
  
  uint32 vlan_domain;
  
  SOC_SAND_PP_VLAN_ID outer_vid;
  
  SOC_SAND_PP_VLAN_ID inner_vid;
  
  SOC_SAND_PON_TUNNEL_ID tunnel;
  
  SOC_SAND_PP_ETHER_TYPE ethertype;
 
  uint32 raw_key;
  
  SOC_SAND_PP_PCP_UP outer_pcp;
  
  int  core_id;



} SOC_PPC_L2_LIF_AC_KEY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_L2_LIF_AC_LEARN_TYPE learn_type;
  
  SOC_PPC_FRWRD_DECISION_INFO learn_info;

} SOC_PPC_L2_LIF_AC_LEARN_RECORD;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 ing_vlan_edit_profile;
  
  SOC_SAND_PP_VLAN_ID vid;
     
  SOC_SAND_PP_VLAN_ID vid2;
  
  uint32 edit_pcp_profile;

} SOC_PPC_L2_LIF_AC_ING_EDIT_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_L2_LIF_AC_SERVICE_TYPE service_type;
  
  SOC_PPC_VSI_ID vsid;
  
  SOC_PPC_L2_LIF_AC_LEARN_RECORD learn_record;
  
  SOC_PPC_L2_LIF_AC_ING_EDIT_INFO ing_edit_info;
  
  SOC_PPC_L2_LIF_DEFAULT_FORWARDING_INFO default_frwrd;
  
  SOC_SAND_PP_HUB_SPOKE_ORIENTATION orientation;
  
  uint32 cos_profile;
  
  uint32 l2cp_profile;
    
  uint32 lif_profile;
    
  uint32 protection_pointer; 
   
  uint8 protection_pass_value; 
   
  uint8 oam_valid;
   
  uint32 vsi_assignment_mode;
   
  uint32 default_forward_profile;
   
  uint8 use_lif;      

  
  uint8 is_extended;

  
  int ext_lif_id;

  
  int global_lif;

  
  int network_group;

} SOC_PPC_L2_LIF_AC_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_MP_LEVEL mp_level;
  
  uint8 is_valid;

} SOC_PPC_L2_LIF_AC_MP_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 l2cp_profile;
  
  uint32 da_mac_address_lsb;

} SOC_PPC_L2_LIF_L2CP_KEY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_L2_LIF_AC_INFO acs_info[SOC_PPC_LIF_AC_GROUP_MAX_SIZE];
  
  uint32 opcode_id;
  
  uint32 nof_lif_entries;

} SOC_PPC_L2_LIF_AC_GROUP_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_SAND_PP_ISID isid_id;
  
  uint32 isid_domain;
  
  uint32 bsa_nickname;

  
  uint8 bsa_nickname_valid;

} SOC_PPC_L2_LIF_ISID_KEY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_L2_LIF_DEFAULT_FORWARDING_INFO default_frwrd;
  
  uint8 learn_enable;
  
  uint32 tpid_profile_index;
  
  uint32 cos_profile;
   
  SOC_SAND_PP_HUB_SPOKE_ORIENTATION orientation;
  
  SOC_PPC_L2_LIF_ISID_SERVICE_TYPE service_type;
   
  uint32 vsi_assignment_mode; 
   
  uint8 use_lif; 
   
  uint32 protection_pointer; 
   
  uint8 protection_pass_value;
   
  uint8 oam_valid; 
   
  uint32 default_forward_profile; 

  
  uint8 is_extended;

  
  int ext_lif_id;

  
  int global_lif;

  
  uint32 lif_profile;

} SOC_PPC_L2_LIF_ISID_INFO;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 vpn_key;
  
  int match_port_class;
} SOC_PPC_L2_LIF_GRE_KEY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_VSI_ID vsid;

  
  uint8 is_extended;

  
  int ext_lif_id;

} SOC_PPC_L2_LIF_GRE_INFO;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 vpn_key;
  
  int match_port_class;
} SOC_PPC_L2_LIF_VXLAN_KEY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_VSI_ID vsid;

  
  uint8 is_extended;

  
  int ext_lif_id;

} SOC_PPC_L2_LIF_VXLAN_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 port_da_not_found_profile;
  
  SOC_SAND_PP_ETHERNET_DA_TYPE da_type;
   
  uint32 lif_default_forward_profile;

} SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_FRWRD_DECISION_INFO frwrd_dest;

  
  uint32 offset;

  
  uint8 add_vsi;


} SOC_PPC_L2_LIF_DEFAULT_FRWRD_ACTION;

typedef struct { 
   SOC_SAND_MAGIC_NUM_VAR

  
  SOC_PPC_FEC_ID fec_id;

  
  uint32 learn_enable;

   
  SOC_PPC_FRWRD_DECISION_INFO learn_info; 

  uint32 vsi_id;

   
  uint32 protection_pointer; 
   
  uint8 protection_pass_value;

    
  uint8 is_extended;

    
  int global_lif_id;

  
  SOC_SAND_PP_HUB_SPOKE_ORIENTATION orientation;

} SOC_PPC_L2_LIF_TRILL_INFO; 



typedef struct
{
    SOC_SAND_MAGIC_NUM_VAR

    
    SOC_SAND_PP_ECID extender_port_vid;

    
    SOC_SAND_PP_VLAN_ID vid;

    
    uint32 name_space;

    
    uint32 is_tagged;
} SOC_PPC_L2_LIF_EXTENDER_KEY;


typedef struct
{
    SOC_SAND_MAGIC_NUM_VAR

    
    SOC_PPC_L2_LIF_AC_SERVICE_TYPE service_type;

    
    SOC_PPC_VSI_ID vsid;

    
    SOC_PPC_L2_LIF_AC_LEARN_RECORD learn_record;

    
    SOC_PPC_L2_LIF_DEFAULT_FORWARDING_INFO default_frwrd;

    
    uint32 cos_profile;

     
    uint32 lif_profile;

     
    uint32 protection_pointer; 

     
    uint8 protection_pass_value; 

     
    uint8 oam_valid;

     
    uint32 vsi_assignment_mode;

     
    uint32 default_forward_profile;

     
    uint8 use_lif;      

    
    uint8 is_extended;

    
    int ext_lif_id;

    
    int global_lif;

} SOC_PPC_L2_LIF_EXTENDER_INFO;












void
  SOC_PPC_L2_LIF_AC_KEY_QUALIFIER_clear(
    SOC_SAND_OUT SOC_PPC_L2_LIF_AC_KEY_QUALIFIER *info
  );

void
  SOC_PPC_L2_LIF_IN_VC_RANGE_clear(
    SOC_SAND_OUT SOC_PPC_L2_LIF_IN_VC_RANGE *info
  );

void
  SOC_PPC_L2_LIF_DEFAULT_FORWARDING_INFO_clear(
    SOC_SAND_OUT SOC_PPC_L2_LIF_DEFAULT_FORWARDING_INFO *info
  );

void
  SOC_PPC_L2_LIF_PWE_LEARN_RECORD_clear(
    SOC_SAND_OUT SOC_PPC_L2_LIF_PWE_LEARN_RECORD *info
  );

void
  SOC_PPC_L2_LIF_PWE_ADDITIONAL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_L2_LIF_PWE_ADDITIONAL_INFO *info
  );

void
  SOC_PPC_L2_LIF_PWE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_L2_LIF_PWE_INFO *info
  );

void
  SOC_PPC_L2_VLAN_RANGE_KEY_clear(
    SOC_SAND_OUT SOC_PPC_L2_VLAN_RANGE_KEY *info
  );

void
  SOC_PPC_L2_PORT_VLAN_RANGE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_L2_PORT_VLAN_RANGE_INFO *info
  );

void
  SOC_PPC_L2_LIF_AC_KEY_clear(
    SOC_SAND_OUT SOC_PPC_L2_LIF_AC_KEY *info
  );

void
  SOC_PPC_L2_LIF_AC_LEARN_RECORD_clear(
    SOC_SAND_OUT SOC_PPC_L2_LIF_AC_LEARN_RECORD *info
  );

void
  SOC_PPC_L2_LIF_AC_ING_EDIT_INFO_clear(
    SOC_SAND_OUT SOC_PPC_L2_LIF_AC_ING_EDIT_INFO *info
  );

void
  SOC_PPC_L2_LIF_AC_INFO_clear(
    SOC_SAND_OUT SOC_PPC_L2_LIF_AC_INFO *info
  );

void
  SOC_PPC_L2_LIF_AC_MP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_L2_LIF_AC_MP_INFO *info
  );

void
  SOC_PPC_L2_LIF_L2CP_KEY_clear(
    SOC_SAND_OUT SOC_PPC_L2_LIF_L2CP_KEY *info
  );

void
  SOC_PPC_L2_LIF_AC_GROUP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_L2_LIF_AC_GROUP_INFO *info
  );

void
  SOC_PPC_L2_LIF_ISID_KEY_clear(
    SOC_SAND_OUT SOC_PPC_L2_LIF_ISID_KEY *info
  );

void
  SOC_PPC_L2_LIF_ISID_INFO_clear(
    SOC_SAND_OUT SOC_PPC_L2_LIF_ISID_INFO *info
  );

void 
  SOC_PPC_L2_LIF_TRILL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_L2_LIF_TRILL_INFO *info
  );

void
  SOC_PPC_L2_LIF_TRILL_INFO_print(
    SOC_SAND_IN  SOC_PPC_L2_LIF_TRILL_INFO *info
  );

void
  SOC_PPC_L2_LIF_GRE_KEY_clear(
    SOC_SAND_OUT SOC_PPC_L2_LIF_GRE_KEY *info
  );

void
  SOC_PPC_L2_LIF_GRE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_L2_LIF_GRE_INFO *info
  );

void
  SOC_PPC_L2_LIF_VXLAN_KEY_clear(
    SOC_SAND_OUT SOC_PPC_L2_LIF_VXLAN_KEY *info
  );

void
  SOC_PPC_L2_LIF_VXLAN_INFO_clear(
    SOC_SAND_OUT SOC_PPC_L2_LIF_VXLAN_INFO *info
  );

void
  SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY_clear(
    SOC_SAND_OUT SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY *info
  );

void
  SOC_PPC_L2_LIF_DEFAULT_FRWRD_ACTION_clear(
    SOC_SAND_OUT SOC_PPC_L2_LIF_DEFAULT_FRWRD_ACTION *info
  );

void 
  SOC_PPC_L2_LIF_EXTENDER_KEY_clear(
    SOC_SAND_OUT SOC_PPC_L2_LIF_EXTENDER_KEY *info
  );

void 
  SOC_PPC_L2_LIF_EXTENDER_INFO_clear(
    SOC_SAND_OUT SOC_PPC_L2_LIF_EXTENDER_INFO *info
  );

#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE enum_val
  );

const char*
  SOC_PPC_L2_LIF_DFLT_FRWRD_SRC_to_string(
    SOC_SAND_IN  SOC_PPC_L2_LIF_DFLT_FRWRD_SRC enum_val
  );

const char*
  SOC_PPC_L2_LIF_AC_SERVICE_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_SERVICE_TYPE enum_val
  );

const char*
  SOC_PPC_L2_LIF_PWE_SERVICE_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_L2_LIF_PWE_SERVICE_TYPE enum_val
  );

const char*
  SOC_PPC_L2_LIF_ISID_SERVICE_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_L2_LIF_ISID_SERVICE_TYPE enum_val
  );

const char*
  SOC_PPC_L2_LIF_AC_LEARN_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_LEARN_TYPE enum_val
  );

const char*
  SOC_PPC_L2_LIF_VSI_ASSIGNMENT_MODE_to_string(
    SOC_SAND_IN  uint32 val
  );

const char*
  SOC_PPC_L2_LIF_L2CP_HANDLE_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_L2_LIF_L2CP_HANDLE_TYPE enum_val
  );


void
  SOC_PPC_L2_LIF_AC_KEY_QUALIFIER_print(
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY_QUALIFIER *info
  );

void
  SOC_PPC_L2_LIF_IN_VC_RANGE_print(
    SOC_SAND_IN  SOC_PPC_L2_LIF_IN_VC_RANGE *info
  );

void
  SOC_PPC_L2_LIF_DEFAULT_FORWARDING_INFO_print(
    SOC_SAND_IN  SOC_PPC_L2_LIF_DEFAULT_FORWARDING_INFO *info
  );

void
  SOC_PPC_L2_LIF_PWE_LEARN_RECORD_print(
    SOC_SAND_IN  SOC_PPC_L2_LIF_PWE_LEARN_RECORD *info
  );

void
  SOC_PPC_L2_LIF_PWE_INFO_print(
    SOC_SAND_IN  SOC_PPC_L2_LIF_PWE_INFO *info
  );

void
  SOC_PPC_L2_VLAN_RANGE_KEY_print(
    SOC_SAND_IN  SOC_PPC_L2_VLAN_RANGE_KEY *info
  );

void
  SOC_PPC_L2_PORT_VLAN_RANGE_INFO_print(
    SOC_SAND_IN  SOC_PPC_L2_PORT_VLAN_RANGE_INFO *info
  );

void
  SOC_PPC_L2_LIF_AC_KEY_print(
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY *info
  );

void
  SOC_PPC_L2_LIF_AC_LEARN_RECORD_print(
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_LEARN_RECORD *info
  );

void
  SOC_PPC_L2_LIF_AC_ING_EDIT_INFO_print(
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_ING_EDIT_INFO *info
  );

void
  SOC_PPC_L2_LIF_AC_INFO_print(
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_INFO *info
  );

void
  SOC_PPC_L2_LIF_AC_MP_INFO_print(
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_MP_INFO *info
  );

void
  SOC_PPC_L2_LIF_L2CP_KEY_print(
    SOC_SAND_IN  SOC_PPC_L2_LIF_L2CP_KEY *info
  );

void
  SOC_PPC_L2_LIF_AC_GROUP_INFO_print(
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_GROUP_INFO *info
  );

void
  SOC_PPC_L2_LIF_ISID_KEY_print(
    SOC_SAND_IN  SOC_PPC_L2_LIF_ISID_KEY *info
  );

void
  SOC_PPC_L2_LIF_ISID_INFO_print(
    SOC_SAND_IN  SOC_PPC_L2_LIF_ISID_INFO *info
  );

void
  SOC_PPC_L2_LIF_GRE_KEY_print(
    SOC_SAND_IN  SOC_PPC_L2_LIF_GRE_KEY *info
  );

void
  SOC_PPC_L2_LIF_GRE_INFO_print(
    SOC_SAND_IN  SOC_PPC_L2_LIF_GRE_INFO *info
  );

void
  SOC_PPC_L2_LIF_VXLAN_KEY_print(
    SOC_SAND_IN  SOC_PPC_L2_LIF_VXLAN_KEY *info
  );

void
  SOC_PPC_L2_LIF_VXLAN_INFO_print(
    SOC_SAND_IN  SOC_PPC_L2_LIF_VXLAN_INFO *info
  );

void
  SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY_print(
    SOC_SAND_IN SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY *info
  );

void
  SOC_PPC_L2_LIF_DEFAULT_FRWRD_ACTION_print(
    SOC_SAND_IN SOC_PPC_L2_LIF_DEFAULT_FRWRD_ACTION *info
  );

int
  SOC_PPC_OCC_PROFILE_USAGE_print(
    SOC_SAND_IN int unit,
    SOC_SAND_IN SOC_OCC_MGMT_TYPE profile_type,
    SOC_SAND_IN int size
  );

int
  SOC_PPC_OCC_ALL_PROFILES_print(
    SOC_SAND_IN int unit
  );

void
  SOC_PPC_L2_LIF_EXTENDER_KEY_print(
    SOC_SAND_IN  SOC_PPC_L2_LIF_EXTENDER_KEY *info
  );

void
  SOC_PPC_L2_LIF_EXTENDER_INFO_print(
    SOC_SAND_IN  SOC_PPC_L2_LIF_EXTENDER_INFO *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
