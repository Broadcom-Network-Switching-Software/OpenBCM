/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPC_API_PORT_INCLUDED__

#define __SOC_PPC_API_PORT_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_general.h>






#define SOC_PPC_PORT_PROPERTY_VLAN_TRANSLATION_MAX      (0xf)
#define SOC_PPC_PORT_PROPERTY_VLAN_TRANSLATION_SHIFT    (0)
#define SOC_PPC_PORT_PROPERTY_VLAN_TRANSLATION_MASK     (0xf)

#define SOC_PPC_PORT_PROPERTY_TUNNEL_TERMINATION_MAX    (4095)
#define SOC_PPC_PORT_PROPERTY_TUNNEL_TERMINATION_SHIFT  (0)
#define SOC_PPC_PORT_PROPERTY_TUNNEL_TERMINATION_MASK   (0xfff)





#define SOC_PPC_PORT_PROPERTY_VLAN_TRANSLATION_SET(_port_property, _value)  \
    ((_port_property) = ((_port_property & ~(SOC_PPC_PORT_PROPERTY_VLAN_TRANSLATION_MASK << SOC_PPC_PORT_PROPERTY_VLAN_TRANSLATION_SHIFT)) \
    | (_value & (SOC_PPC_PORT_PROPERTY_VLAN_TRANSLATION_MASK << SOC_PPC_PORT_PROPERTY_VLAN_TRANSLATION_SHIFT))))

#define SOC_PPC_PORT_PROPERTY_VLAN_TRANSLATION_GET(_port_property)  \
    ((_port_property & (SOC_PPC_PORT_PROPERTY_VLAN_TRANSLATION_MASK << SOC_PPC_PORT_PROPERTY_VLAN_TRANSLATION_SHIFT)) \
    >> SOC_PPC_PORT_PROPERTY_VLAN_TRANSLATION_SHIFT)

#define SOC_PPC_PORT_PROPERTY_TUNNEL_TERMINATION_SET(_port_property, _value)  \
    ((_port_property) = ((_port_property & ~(SOC_PPC_PORT_PROPERTY_TUNNEL_TERMINATION_MASK << SOC_PPC_PORT_PROPERTY_TUNNEL_TERMINATION_SHIFT)) \
    | (_value & (SOC_PPC_PORT_PROPERTY_TUNNEL_TERMINATION_MASK << SOC_PPC_PORT_PROPERTY_TUNNEL_TERMINATION_SHIFT))))

#define SOC_PPC_PORT_PROPERTY_TUNNEL_TERMINATION_GET(_port_property)  \
    ((_port_property & (SOC_PPC_PORT_PROPERTY_TUNNEL_TERMINATION_MASK << SOC_PPC_PORT_PROPERTY_TUNNEL_TERMINATION_SHIFT)) \
    >> SOC_PPC_PORT_PROPERTY_TUNNEL_TERMINATION_SHIFT)





typedef enum
{
  
  SOC_PPC_PORT_DIRECTION_INCOMING = 1,
  
  SOC_PPC_PORT_DIRECTION_OUTGOING = 2,
  
  SOC_PPC_PORT_DIRECTION_BOTH = 3,
  
  SOC_PPC_PORT_NOF_DIRECTIONS = 4
}SOC_PPC_PORT_DIRECTION;

#define SOC_PPC_IS_DIRECTION_INCOMING(dir) \
  SOC_SAND_NUM2BOOL(((dir) == SOC_PPC_PORT_DIRECTION_INCOMING  ) || ((dir) == SOC_PPC_PORT_DIRECTION_BOTH))

#define SOC_PPC_IS_DIRECTION_OUTGOING(dir) \
  SOC_SAND_NUM2BOOL(((dir) == SOC_PPC_PORT_DIRECTION_OUTGOING  ) || ((dir) == SOC_PPC_PORT_DIRECTION_BOTH))


typedef enum
{
  
  SOC_PPC_PORT_STP_STATE_BLOCK = 0,
  
  SOC_PPC_PORT_STP_STATE_LEARN = 1,
  
  SOC_PPC_PORT_STP_STATE_FORWARD = 2,
  
  SOC_PPC_NOF_PORT_STP_STATES = 3
}SOC_PPC_PORT_STP_STATE;

typedef enum
{
  SOC_PPC_PORT_STP_STATE_FLD_VAL_BLOCK = 2,
  SOC_PPC_PORT_STP_STATE_FLD_VAL_LEARN = 1,
  SOC_PPC_PORT_STP_STATE_FLD_VAL_FORWARD = 0
} SOC_PPC_PORT_STP_STATE_FLD_VAL;


typedef enum
{
  

  
  SOC_PPC_PORT_DEFINED_VT_PROFILE_DEFAULT = 0,
  
  SOC_PPC_PORT_DEFINED_VT_PROFILE_DOUBLE_TAG_PRIORITY_INITIAL_VID = 3,  
  
  SOC_PPC_PORT_DEFINED_VT_PROFILE_FRR_COUPLING_USE_INITIAL_VID = 5,
  
  SOC_PPC_PORT_DEFINED_VT_PROFILE_FRR_COUPLING_DOUBLE_TAG_PRIORITY_USE_INITIAL_VID = 7,
  
  SOC_PPC_PORT_DEFINED_VT_PROFILE_USE_INITIAL_VID = 1,
  
  SOC_PPC_PORT_DEFINED_VT_PROFILE_TRILL_USE_INITIAL_VID = 5,
  
  SOC_PPC_PORT_DEFINED_VT_PROFILE_FRR_COUPLING = 4,
  
  SOC_PPC_PORT_DEFINED_VT_PROFILE_EXPLICIT_NULL = 4,
  
  SOC_PPC_PORT_DEFINED_VT_PROFILE_DOUBLE_TAG_PRIORITY = 2,  
  
  SOC_PPC_PORT_DEFINED_VT_PROFILE_TRILL = 4,
  
  
  SOC_PPC_PORT_DEFINED_VT_PROFILE_PON_DEFAULT = 4,
  
  SOC_PPC_PORT_DEFINED_VT_PROFILE_PON_TUNNEL_ID = 5,
  
  SOC_PPC_PORT_DEFINED_VT_PROFILE_EXTENDER_PE = 5,
  
  SOC_PPC_PORT_DEFINED_VT_PROFILE_IGNORE_2ND_TAG = 6,
  
  SOC_PPC_PORT_DEFINED_VT_PROFILE_IGNORE_2ND_TAG_EXPLICIT_NULL = 7,
  
  SOC_PPC_PORT_DEFINED_VT_PROFILE_EVB = 7,  
  
  SOC_PPC_PORT_DEFINED_VT_PROFILE_FLEXIBLE_Q_IN_Q  = 7,  
  
  SOC_PPC_PORT_DEFINED_VT_PROFILE_PCP_ENABLE       = 7,

  SOC_PPC_PORT_DEFINED_VT_PROFILE_VLAN_DOMAIN_MPLS       = 7,

   

  

  SOC_PPC_PORT_DEFINED_VT_PORT_TERMINATION = 8,
  
  SOC_PPC_PORT_DEFINED_VT_PROFILE_GENERALIZED_RCH0 = 9,
  
  SOC_PPC_PORT_DEFINED_VT_PROFILE_GENERALIZED_RCH1 = 10,

  

  SOC_PPC_PORT_DEFINED_VT_PORT_TERMINATION_CASCADE = 16,
  
  

  SOC_PPC_PORT_DEFINED_VT_PROFILE_3MTSE_PLUS_FRR = 17,

   

  SOC_PPC_NOF_PORT_DEFINED_VT_PROFILES = 17
}SOC_PPC_PORT_DEFINED_VT_PROFILE;

typedef enum
{
  
  SOC_PPC_PORT_DEFINED_TT_PROFILE_DEFAULT = 0,
  
  SOC_PPC_PORT_DEFINED_TT_PROFILE_FRR_COUPLING = 1,
  
  SOC_PPC_PORT_DEFINED_TT_PROFILE_EXPLICIT_NULL = 1,
  
  SOC_PPC_PORT_DEFINED_TT_PROFILE_FORCE_MY_MAC = 2,  
   
  SOC_PPC_PORT_DEFINED_TT_PROFILE_PORT_EXTENDER_CB = 3,
  
  SOC_PPC_PORT_DEFINED_TT_PROFILE_PORT_EXTENDER_UNTAG_CB = 4,

  SOC_PPC_PORT_DEFINED_TT_PROFILE_VLAN_DOMAIN_MPLS       = 5,

   
   
  SOC_PPC_PORT_DEFINED_TT_PROFILE_PORT_TERMINATION = 6,
  
   
   
  SOC_PPC_PORT_DEFINED_TT_PROFILE_GENERALIZED_RCH = 7,

  
  SOC_PPC_PORT_DEFINED_TT_PROFILE_TERMINATION_2MTSE_PLUS_FRR = 8,

  
  SOC_PPC_NOF_PORT_DEFINED_TT_PROFILES
}SOC_PPC_PORT_DEFINED_TT_PROFILE;



typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 enable_l2_vpn;
  
  SOC_SAND_PP_DEST_TYPE learn_dest_type;

} SOC_PPC_PORT_L2_VPN_INFO;

#define SOC_PPC_PORT_IHP_PINFO_LLR_TBL                   (0x1 << 0)
#define SOC_PPC_PORT_IHP_PP_PORT_INFO_TBL                (0x1 << 1)
#define SOC_PPC_PORT_IHP_VTT_PP_PORT_CONFIG_TBL          (0x1 << 2)
#define SOC_PPC_PORT_IHB_PINFO_FLP_TBL                   (0x1 << 3)
#define SOC_PPC_PORT_EGQ_PCT_TBL                         (0x1 << 4)
#define SOC_PPC_PORT_EPNI_PP_PCT_TBL                     (0x1 << 5)
#define SOC_PPC_PORT_EGQ_PP_PPCT_TBL                     (0x1 << 6)
#define SOC_PPC_PORT_IHP_VTT_PP_PORT_VSI_PROFILE_TBL     (0x1 << 7)
#define SOC_PPC_PORT_ALL_TBL                             (0xff)

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 port_profile;
  
  
  uint32 port_profile_eg;
  
  uint32 tpid_profile;
  
  uint32 vlan_domain;
  
  SOC_SAND_PP_PORT_L2_TYPE port_type;
  
  SOC_PPC_ACTION_PROFILE initial_action_profile;
  
  uint8 is_pbp;
  
  uint8 is_n_port;
  
  uint8 enable_learning;
  
  uint8 enable_outgoing_learning;
  
  uint8 enable_same_interfac_filter;
  
  uint8 same_interface_filter_skip;
  
  uint8 is_learn_ac;
  
  SOC_SAND_PP_HUB_SPOKE_ORIENTATION orientation;
  
  uint32 ether_type_based_profile;
  
  uint8 not_authorized_802_1x;
  
  uint32 mtu;
  
  uint32 mtu_encapsulated;
   
  uint32 da_not_found_profile;
  
  SOC_PPC_AC_ID dflt_egress_ac;
  
  SOC_PPC_PORT_DEFINED_VT_PROFILE vlan_translation_profile;
  
  SOC_PPC_PORT_DEFINED_TT_PROFILE tunnel_termination_profile;

  
  uint32 ac_p2p_to_pbb_vsi_profile;
  
  uint32 label_pwe_p2p_vsi_profile;

  
  uint8 enable_pon_double_lookup;

  
  uint8 enable_mpls_speculative_parsing;
  
  uint32 dflt_port_lif;

  
  uint32 flags;

} SOC_PPC_PORT_INFO;

typedef enum {
    soc_ppc_port_property_vlan_translation,
    soc_ppc_port_property_tunnel_termination
} SOC_PPC_PORT_PROPERTY;











void
  SOC_PPC_PORT_L2_VPN_INFO_clear(
    SOC_SAND_OUT SOC_PPC_PORT_L2_VPN_INFO *info
  );

void
  SOC_PPC_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PPC_PORT_INFO *info
  );

#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_PORT_DIRECTION_to_string(
    SOC_SAND_IN  SOC_PPC_PORT_DIRECTION enum_val
  );

const char*
  SOC_PPC_PORT_STP_STATE_to_string(
    SOC_SAND_IN  SOC_PPC_PORT_STP_STATE enum_val
  );


void
  SOC_PPC_PORT_L2_VPN_INFO_print(
    SOC_SAND_IN  SOC_PPC_PORT_L2_VPN_INFO *info
  );

void
  SOC_PPC_PORT_INFO_print(
    SOC_SAND_IN  SOC_PPC_PORT_INFO *info
  );

#endif 




#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

