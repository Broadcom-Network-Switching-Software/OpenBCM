/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPC_API_RIF_INCLUDED__

#define __SOC_PPC_API_RIF_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_general.h>
#include <soc/dpp/PPC/ppc_api_mpls_term.h>
#include <soc/dpp/PPC/ppc_api_frwrd_fec.h>







#define  SOC_PPC_RIF_IS_NOT_UPDATED (0)


#define  SOC_PPC_RIF_MPLS_LABEL_FRR         (0x1)
#define  SOC_PPC_RIF_MPLS_LABEL_ELI         (0x4)
#define  SOC_PPC_RIF_MPLS_LABEL_EXPECT_BOS  (0x8)


#define  SOC_PPC_RIF_MPLS_LABEL_LOOKUP_NONE             (0x10)

#define  SOC_PPC_RIF_MPLS_LABEL_EVPN_IML                (0x20)


#define  SOC_PPC_RIF_MPLS_LABEL_TERM_BASED_PORT          (0x40)

#define  SOC_PPC_RIF_MAX_NOF_ROUTING_ENABLERS_BITMAPS       16


#define  SOC_PPC_RIF_PROFILE_TO_ROUTING_ENABLERS_VECTOR_BEFORE_INIT         (SOC_IS_JERICHO_PLUS(unit) ? 0x30FFFFD : 0x30FFFFC);

#define  SOC_PPC_ROUTING_ENABLE_MIM_UC_BIT                  18
#define  SOC_PPC_ROUTING_ENABLE_MIM_MC_BIT                  19
#define  SOC_PPC_ROUTING_ENABLE_IPV4UC_BIT                  26
#define  SOC_PPC_ROUTING_ENABLE_IPV4MC_BIT                  27
#define  SOC_PPC_ROUTING_ENABLE_IPV6UC_BIT                  28
#define  SOC_PPC_ROUTING_ENABLE_IPV6MC_BIT                  29
#define  SOC_PPC_ROUTING_ENABLE_MPLS_UC_BIT                 30
#define  SOC_PPC_ROUTING_ENABLE_MPLS_MC_BIT                 31


#define  SOC_PPC_RIF_IP_TERM_FLAG_USE_DIP       0x1
#define  SOC_PPC_RIF_IP_TERM_FLAG_USE_SIP       0x2
#define  SOC_PPC_RIF_IP_TERM_FLAG_IPV6          0x4
#define  SOC_PPC_RIF_IP_TERM_FLAG_KEY_ONLY      0x8
#define  SOC_PPC_RIF_IP_TERM_IPMC_BIDIR         0x10
#define  SOC_PPC_RIF_IP_TERM_FLAG_USE_DIP_DUMMY 0x20
#define  SOC_PPC_RIF_IP_TERM_FLAG_VRF_IS_VALID  0x40
#define  SOC_PPC_RIF_IP_TERM_FLAG_MY_VTEP_INDEX_IS_VALID 0x80
#define  SOC_PPC_RIF_IP_TERM_FLAG_IP_GRE_TUNNEL_IS_VALID 0x100
#define  SOC_PPC_RIF_IP_TERM_FLAG_BUD                    0x200
#define  SOC_PPC_RIF_IP_TERM_FLAG_USE_DIP_EXPLICIT_NULL_DUMMY 0x400










typedef enum
{
  
  SOC_PPC_RIF_ROUTE_ENABLE_TYPE_NONE = 0x0,
  
  SOC_PPC_RIF_ROUTE_ENABLE_TYPE_IPV4_UC = 0x1,
  SOC_PPC_RIF_ROUTE_ENABLE_TYPE_IPV6_UC = 0x2,  

  
  SOC_PPC_RIF_ROUTE_ENABLE_TYPE_IPV4_MC = 0x4,
  SOC_PPC_RIF_ROUTE_ENABLE_TYPE_IPV6_MC = 0x8,  

  
  SOC_PPC_RIF_ROUTE_ENABLE_TYPE_MPLS = 0x10
    
} SOC_PPC_RIF_ROUTE_ENABLE_TYPE;

#define SOC_PPC_RIF_ROUTE_ENABLE_TYPE_IP_UC   (SOC_PPC_RIF_ROUTE_ENABLE_TYPE_IPV4_UC + SOC_PPC_RIF_ROUTE_ENABLE_TYPE_IPV6_UC)
#define SOC_PPC_RIF_ROUTE_ENABLE_TYPE_IP_MC   (SOC_PPC_RIF_ROUTE_ENABLE_TYPE_IPV4_MC + SOC_PPC_RIF_ROUTE_ENABLE_TYPE_IPV6_MC)
#define SOC_PPC_RIF_ROUTE_ENABLE_TYPE_ALL     (SOC_PPC_RIF_ROUTE_ENABLE_TYPE_IP_MC + SOC_PPC_RIF_ROUTE_ENABLE_TYPE_IP_UC + SOC_PPC_RIF_ROUTE_ENABLE_TYPE_MPLS)




#define SOC_PPC_RIF_IP_TUNNEL_TERM_ADD_LIF_ID_IS_LIF_INDEX (0x0)
 
#define SOC_PPC_RIF_IP_TUNNEL_TERM_ADD_LIF_ID_IS_FEC (0x1)

#define SOC_PPC_RIF_IP_TUNNEL_TERM_ADD_LIF_ID_IS_MY_VTEP_INDEX (0x2)


typedef enum 
{ 
   
  SOC_PPC_MPLS_LABEL_INDEX_FIRST = 0x1, 
   
  SOC_PPC_MPLS_LABEL_INDEX_SECOND = 0x2, 
   
  SOC_PPC_MPLS_LABEL_INDEX_THIRD = 0x3, 
  
   
  SOC_PPC_MPLS_LABEL_INDEX_ZERO = 0x4, 
 
  SOC_PPC_MPLS_LABEL_INDEX_ALL = 0x0, 
   
  SOC_PPC_NOF_MPLS_LABEL_INDEX = 5 
} SOC_PPC_MPLS_LABEL_INDEX; 



typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 first_label;
  
  uint32 last_label;

} SOC_PPC_RIF_MPLS_LABELS_RANGE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_SAND_PP_MPLS_LABEL label_id;
  
  SOC_PPC_VSI_ID vsid;

   
  SOC_PPC_MPLS_LABEL_INDEX label_index; 

  
  SOC_SAND_PP_MPLS_LABEL label_id_second;

  
  uint32 vlan_domain;

  
  uint32 flags;

} SOC_PPC_MPLS_LABEL_RIF_KEY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 uc_rpf_enable;
  
  uint8 custom_rif_bit;

  
  SOC_PPC_VRF_ID vrf_id;
  
  uint32 cos_profile;
  
  uint32 ttl_scope_index;
  
  uint32 routing_enablers_bm;

  
  uint8  routing_enablers_bm_id;

  
  uint32  routing_enablers_bm_rif_profile;

  
  int  is_configure_enabler_needed;

  
  uint8 enable_default_routing;
#ifdef BCM_88660_A0
  
	SOC_PPC_FRWRD_FEC_RPF_MODE uc_rpf_mode;
#endif

  
  uint8 intf_class;

} SOC_PPC_RIF_INFO;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 flags;
  
  uint32 dip;
  
  uint32 dip_prefix_len;
  
  uint32 sip;
  
  uint32 sip_prefix_len;

  
  SOC_SAND_PP_IPV6_SUBNET dip6;
  
  SOC_SAND_PP_IPV6_SUBNET sip6;
  
  uint32 vrf;
  
  uint16 ipv4_next_protocol;
  
  uint16 ipv4_next_protocol_prefix_len;
  
  uint32 port_property;
  
  uint8 port_property_en;
  
  uint8 my_vtep_index; 
  
  uint16 gre_ethertype; 
  
  uint16 gre_ethertype_prefix_len; 
  
  uint8 ip_gre_tunnel; 
  
  uint16 qualifier_next_protocol; 
  
  uint16 qualifier_next_protocol_prefix_len; 
  
  uint16 ttl;
} SOC_PPC_RIF_IP_TERM_KEY;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_RIF_ID rif;
  
  uint32 cos_profile;

  
  uint32 lif_profile;

  
  uint32 tpid_profile;

  
  uint32 default_forward_profile;

  
  SOC_PPC_VSI_ID vsi;

  
  SOC_PPC_FRWRD_DECISION_INFO learn_record;
  
  uint8 learn_enable;

  
  uint32 vsi_assignment_mode;

  
  uint32 protection_pointer;
  
  uint8 protection_pass_value;

  
  uint8 oam_valid;
  
  SOC_PPC_MPLS_TERM_MODEL_TYPE processing_type;
  
  SOC_SAND_PP_HUB_SPOKE_ORIENTATION orientation;

   
  uint8 skip_ethernet;
  
  uint8 is_extended;

  
  uint64 additional_data;

  
  int ext_lif_id;

  
  int global_lif;

} SOC_PPC_RIF_IP_TERM_INFO;












void
  SOC_PPC_RIF_MPLS_LABELS_RANGE_clear(
    SOC_SAND_OUT SOC_PPC_RIF_MPLS_LABELS_RANGE *info
  );

void
  SOC_PPC_MPLS_LABEL_RIF_KEY_clear(
    SOC_SAND_OUT SOC_PPC_MPLS_LABEL_RIF_KEY *info
  );

void
  SOC_PPC_RIF_INFO_clear(
    SOC_SAND_OUT SOC_PPC_RIF_INFO *info
  );

void
  SOC_PPC_RIF_IP_TERM_INFO_clear(
    SOC_SAND_OUT SOC_PPC_RIF_IP_TERM_INFO *info
  );

void
  SOC_PPC_RIF_IP_TERM_KEY_clear(
    SOC_SAND_OUT SOC_PPC_RIF_IP_TERM_KEY *key
  );

#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_RIF_ROUTE_ENABLE_TYPE_to_string(
    SOC_SAND_IN  uint32 enum_val
  );

void
  SOC_PPC_RIF_MPLS_LABELS_RANGE_print(
    SOC_SAND_IN  SOC_PPC_RIF_MPLS_LABELS_RANGE *info
  );

void
  SOC_PPC_MPLS_LABEL_RIF_KEY_print(
    SOC_SAND_IN  SOC_PPC_MPLS_LABEL_RIF_KEY *info
  );

void
  SOC_PPC_RIF_INFO_print(
    SOC_SAND_IN  SOC_PPC_RIF_INFO *info
  );

void
  SOC_PPC_RIF_IP_TERM_INFO_print(
    SOC_SAND_IN  SOC_PPC_RIF_IP_TERM_INFO *info
  );

void
  SOC_PPC_RIF_IP_TERM_KEY_print(
    SOC_SAND_IN  SOC_PPC_RIF_IP_TERM_KEY *key
  );

const char*
  SOC_PPC_MPLS_LABEL_INDEX_to_string(
    SOC_SAND_IN  SOC_PPC_MPLS_LABEL_INDEX enum_val
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
