/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __ARAD_API_PORTS_INCLUDED__

#define __ARAD_API_PORTS_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/arad_api_general.h>
#include <soc/dpp/TMC/tmc_api_stack.h>
#include <soc/dpp/TMC/tmc_api_ports.h>
#include <soc/dpp/PPC/ppc_api_lag.h>






#define  ARAD_PORTS_SYS_PHYS_PORT_ID_MAX (SOC_TMC_PORTS_SYS_PHYS_PORT_ID_MAX)


#define  ARAD_PORT_LAGS_MAX (ARAD_MAX_LAG_GROUP_ID)


#define  ARAD_PORTS_LAG_OUT_MEMBERS_MAX (ARAD_MAX_LAG_ENTRY_ID)


#define  ARAD_PORTS_LAG_IN_MEMBERS_MAX (SOC_TMC_PORTS_LAG_IN_MEMBERS_MAX_ARAD)


#define  ARAD_PORTS_LAG_MEMBERS_MAX (SOC_TMC_PORTS_LAG_MEMBERS_MAX)
#define ARAD_PORTS_LAG_MEMBER_ID_MAX (ARAD_MAX_LAG_ENTRY_ID)

#define ARAD_PORT_NOF_PP_PORTS                       (256)

#define ARAD_PORTS_PP_PORT_RCY_OVERLAY_PTCH (SOC_TMC_PORT_PP_PORT_RCY_OVERLAY_PTCH)


#define ARAD_PORTS_TEST1_PORT_PP_PORT (SOC_TEST1_PORT_PP_PORT)



#define ARAD_PORTS_PRESERVING_DSCP_PORT_PP_PORT (SOC_PRESERVING_DSCP_PORT_PP_PORT)

#define ARAD_PORTS_MIM_SPB_PORT_PP_PORT (SOC_MIM_SPB_PORT)










#define ARAD_PORT_DIRECTION_INCOMING                      SOC_TMC_PORT_DIRECTION_INCOMING
#define ARAD_PORT_DIRECTION_OUTGOING                      SOC_TMC_PORT_DIRECTION_OUTGOING
#define ARAD_PORT_DIRECTION_BOTH                          SOC_TMC_PORT_DIRECTION_BOTH
#define ARAD_PORT_NOF_DIRECTIONS                          SOC_TMC_PORT_NOF_DIRECTIONS
typedef SOC_TMC_PORT_DIRECTION                                 ARAD_PORT_DIRECTION;

#define ARAD_PORT_HEADER_TYPE_NONE                        SOC_TMC_PORT_HEADER_TYPE_NONE
#define ARAD_PORT_HEADER_TYPE_ETH                         SOC_TMC_PORT_HEADER_TYPE_ETH
#define ARAD_PORT_HEADER_TYPE_RAW                         SOC_TMC_PORT_HEADER_TYPE_RAW
#define ARAD_PORT_HEADER_TYPE_TM                          SOC_TMC_PORT_HEADER_TYPE_TM
#define ARAD_PORT_HEADER_TYPE_PROG                        SOC_TMC_PORT_HEADER_TYPE_PROG
#define ARAD_PORT_HEADER_TYPE_CPU                         SOC_TMC_PORT_HEADER_TYPE_CPU
#define ARAD_PORT_HEADER_TYPE_STACKING                    SOC_TMC_PORT_HEADER_TYPE_STACKING
#define ARAD_PORT_HEADER_TYPE_TDM                         SOC_TMC_PORT_HEADER_TYPE_TDM
#define ARAD_PORT_HEADER_TYPE_TDM_RAW                     SOC_TMC_PORT_HEADER_TYPE_TDM_RAW
#define ARAD_PORT_HEADER_TYPE_INJECTED                    SOC_TMC_PORT_HEADER_TYPE_INJECTED
#define ARAD_PORT_HEADER_TYPE_INJECTED_PP                 SOC_TMC_PORT_HEADER_TYPE_INJECTED_PP
#define ARAD_PORT_HEADER_TYPE_INJECTED_2                  SOC_TMC_PORT_HEADER_TYPE_INJECTED_2
#define ARAD_PORT_HEADER_TYPE_INJECTED_2_PP               SOC_TMC_PORT_HEADER_TYPE_INJECTED_2_PP
#define ARAD_PORT_HEADER_TYPE_XGS_HQoS                    SOC_TMC_PORT_HEADER_TYPE_XGS_HQoS
#define ARAD_PORT_HEADER_TYPE_XGS_DiffServ                SOC_TMC_PORT_HEADER_TYPE_XGS_DiffServ
#define ARAD_PORT_HEADER_TYPE_XGS_MAC_EXT                 SOC_TMC_PORT_HEADER_TYPE_XGS_MAC_EXT
#define ARAD_PORT_HEADER_TYPE_TDM_PMM                     SOC_TMC_PORT_HEADER_TYPE_TDM_PMM
#define ARAD_PORT_HEADER_TYPE_L2_REMOTE_CPU               SOC_TMC_PORT_HEADER_TYPE_REMOTE_CPU
#define ARAD_PORT_HEADER_TYPE_UDH_ETH                     SOC_TMC_PORT_HEADER_TYPE_UDH_ETH
#define ARAD_PORT_HEADER_TYPE_MPLS_RAW                    SOC_TMC_PORT_HEADER_TYPE_MPLS_RAW
#define ARAD_PORT_HEADER_TYPE_FCOE_N_PORT                 SOC_TMC_PORT_HEADER_TYPE_FCOE_N_PORT
#define ARAD_PORT_HEADER_TYPE_L2_ENCAP_EXTERNAL_CPU	  SOC_TMC_PORT_HEADER_TYPE_L2_ENCAP_EXTERNAL_CPU
#define ARAD_PORT_HEADER_TYPE_MIRROR_RAW                  SOC_TMC_PORT_HEADER_TYPE_MIRROR_RAW
#define ARAD_PORT_HEADER_TYPE_DSA_RAW                     SOC_TMC_PORT_HEADER_TYPE_DSA_RAW
#define ARAD_PORT_HEADER_TYPE_RAW_DSA                     SOC_TMC_PORT_HEADER_TYPE_RAW_DSA
#define ARAD_PORT_HEADER_TYPE_RCH_0                       SOC_TMC_PORT_HEADER_TYPE_RCH_0
#define ARAD_PORT_HEADER_TYPE_RCH_1                       SOC_TMC_PORT_HEADER_TYPE_RCH_1

#define ARAD_PORT_HEADER_TYPE_COE                         SOC_TMC_PORT_HEADER_TYPE_COE

#define ARAD_PORT_NOF_HEADER_TYPES                        SOC_TMC_PORT_NOF_HEADER_TYPES
typedef SOC_TMC_PORT_HEADER_TYPE                               ARAD_PORT_HEADER_TYPE;

#define ARAD_PORTS_SNOOP_SIZE_BYTES_64                    SOC_TMC_PORTS_SNOOP_SIZE_BYTES_64
#define ARAD_PORTS_SNOOP_SIZE_BYTES_192                   SOC_TMC_PORTS_SNOOP_SIZE_BYTES_192
#define ARAD_PORTS_SNOOP_SIZE_ALL                         SOC_TMC_PORTS_SNOOP_SIZE_ALL
#define ARAD_PORTS_NOF_SNOOP_SIZES                        SOC_TMC_PORTS_NOF_SNOOP_SIZES
typedef SOC_TMC_PORTS_SNOOP_SIZE                               ARAD_PORTS_SNOOP_SIZE;

#define ARAD_PORTS_FTMH_EXT_OUTLIF_NEVER                  SOC_TMC_PORTS_FTMH_EXT_OUTLIF_NEVER
#define ARAD_PORTS_FTMH_EXT_OUTLIF_IF_MC                  SOC_TMC_PORTS_FTMH_EXT_OUTLIF_IF_MC
#define ARAD_PORTS_FTMH_EXT_OUTLIF_ALWAYS                 SOC_TMC_PORTS_FTMH_EXT_OUTLIF_ALWAYS
#define ARAD_PORTS_FTMH_EXT_OUTLIF_DOUBLE_TAG             SOC_TMC_PORTS_FTMH_EXT_OUTLIF_DOUBLE_TAG
#define ARAD_PORTS_FTMH_NOF_EXT_OUTLIFS                   SOC_TMC_PORTS_FTMH_NOF_EXT_OUTLIFS
typedef SOC_TMC_PORTS_FTMH_EXT_OUTLIF                          ARAD_PORTS_FTMH_EXT_OUTLIF;

#define ARAD_PORT_EGR_HDR_CR_DISCOUNT_TYPE_A              SOC_TMC_PORT_EGR_HDR_CR_DISCOUNT_TYPE_A
#define ARAD_PORT_EGR_HDR_CR_DISCOUNT_TYPE_B              SOC_TMC_PORT_EGR_HDR_CR_DISCOUNT_TYPE_B
#define ARAD_PORT_NOF_EGR_HDR_CR_DISCOUNT_TYPES           SOC_TMC_PORT_NOF_EGR_HDR_CR_DISCOUNT_TYPES
typedef SOC_TMC_PORT_EGR_HDR_CR_DISCOUNT_TYPE                  ARAD_PORT_EGR_HDR_CR_DISCOUNT_TYPE;

typedef SOC_TMC_PORT2IF_MAPPING_INFO                           ARAD_PORT2IF_MAPPING_INFO;
typedef SOC_TMC_PORTS_LAG_MEMBER                               ARAD_PORTS_LAG_MEMBER;
typedef SOC_TMC_PORTS_OVERRIDE_INFO                            ARAD_PORTS_OVERRIDE_INFO;
typedef SOC_TMC_PORT_INBOUND_MIRROR_INFO                       ARAD_PORT_INBOUND_MIRROR_INFO;
typedef SOC_TMC_PORT_OUTBOUND_MIRROR_INFO                      ARAD_PORT_OUTBOUND_MIRROR_INFO;
typedef SOC_TMC_PORT_SNOOP_INFO                                ARAD_PORT_SNOOP_INFO;
typedef SOC_TMC_PORTS_ITMH_BASE                                ARAD_PORTS_ITMH_BASE;
typedef SOC_TMC_PORTS_ITMH_EXT_SRC_PORT                        ARAD_PORTS_ITMH_EXT_SRC_PORT;
typedef SOC_TMC_PORTS_ITMH                                     ARAD_PORTS_ITMH;
typedef SOC_TMC_PORTS_ISP_INFO                                 ARAD_PORTS_ISP_INFO;
typedef SOC_TMC_PORTS_STAG_FIELDS                              ARAD_PORTS_STAG_FIELDS;
typedef SOC_TMC_PORTS_OTMH_EXTENSIONS_EN                       ARAD_PORTS_OTMH_EXTENSIONS_EN;
typedef SOC_TMC_PORT_EGR_HDR_CR_DISCOUNT_INFO                  ARAD_PORT_EGR_HDR_CR_DISCOUNT_INFO;

typedef SOC_TMC_PORT_LAG_SYS_PORT_INFO                         ARAD_PORT_LAG_SYS_PORT_INFO;

#define ARAD_PORTS_FC_TYPE_NONE                              SOC_TMC_PORTS_FC_TYPE_NONE
#define ARAD_PORTS_FC_TYPE_LL                                SOC_TMC_PORTS_FC_TYPE_LL
#define ARAD_PORTS_FC_TYPE_CB2                               SOC_TMC_PORTS_FC_TYPE_CB2
#define ARAD_PORTS_FC_TYPE_CB8                               SOC_TMC_PORTS_FC_TYPE_CB8
#define ARAD_PORTS_NOF_FC_TYPES                              SOC_TMC_PORTS_NOF_FC_TYPES
typedef SOC_TMC_PORTS_FC_TYPE                                  ARAD_PORTS_FC_TYPE;

typedef SOC_TMC_PORT_PP_PORT_INFO                              ARAD_PORT_PP_PORT_INFO;
typedef SOC_TMC_PORT_COUNTER_INFO                              ARAD_PORT_COUNTER_INFO;
typedef SOC_TMC_PORTS_FORWARDING_HEADER_INFO                   ARAD_PORTS_FORWARDING_HEADER_INFO;

#define ARAD_PORT_LAG_MODE_1K_16                   SOC_TMC_PORT_LAG_MODE_1K_16     
#define ARAD_PORT_LAG_MODE_512_32                  SOC_TMC_PORT_LAG_MODE_512_32     
#define ARAD_PORT_LAG_MODE_256_64                  SOC_TMC_PORT_LAG_MODE_256_64    
#define ARAD_PORT_LAG_MODE_128_128                 SOC_TMC_PORT_LAG_MODE_128_128   
#define ARAD_PORT_LAG_MODE_64_256                  SOC_TMC_PORT_LAG_MODE_64_256      
#define ARAD_NOF_PORT_LAG_MODES                    SOC_TMC_NOF_PORT_LAG_MODES      
typedef SOC_TMC_PORT_LAG_MODE                            ARAD_PORT_LAG_MODE;

typedef SOC_TMC_PORTS_SWAP_INFO                    ARAD_PORTS_SWAP_INFO;

typedef SOC_TMC_PORTS_PON_TUNNEL_INFO              ARAD_PORTS_PON_TUNNEL_INFO;

typedef SOC_TMC_L2_ENCAP_INFO                      ARAD_L2_ENCAP_INFO;

#define ARAD_PORTS_VT_PROFILE_NONE                 SOC_TMC_PORTS_VT_PROFILE_NONE
#define ARAD_PORTS_VT_PROFILE_OVERLAY_RCY          SOC_TMC_PORTS_VT_PROFILE_OVERLAY_RCY
#define ARAD_PORTS_VT_PROFILE_CUSTOM_PP            SOC_TMC_PORTS_VT_PROFILE_CUSTOM_PP
#define ARAD_PORTS_NOF_VT_PROFILES                 SOC_TMC_PORTS_NOF_VT_PROFILES
typedef SOC_TMC_PORTS_VT_PROFILE                   ARAD_PORTS_VT_PROFILE;

#define ARAD_PORTS_TT_PROFILE_NONE                 SOC_TMC_PORTS_TT_PROFILE_NONE
#define ARAD_PORTS_TT_PROFILE_OVERLAY_RCY          SOC_TMC_PORTS_TT_PROFILE_OVERLAY_RCY
#define ARAD_PORTS_TT_PROFILE_PON                  SOC_TMC_PORTS_TT_PROFILE_PON
#define ARAD_PORTS_TT_PROFILE_FORCE_BRIDGE_FWD     SOC_TMC_PORTS_TT_PROFILE_FORCE_BRIDGE_FWD
#define ARAD_PORTS_NOF_TT_PROFILES                 SOC_TMC_PORTS_NOF_TT_PROFILES
typedef SOC_TMC_PORTS_TT_PROFILE                   ARAD_PORTS_TT_PROFILE;

#define ARAD_PORTS_FLP_PROFILE_NONE                SOC_TMC_PORTS_FLP_PROFILE_NONE
#define ARAD_PORTS_FLP_PROFILE_OVERLAY_RCY         SOC_TMC_PORTS_FLP_PROFILE_OVERLAY_RCY
#define ARAD_PORTS_FLP_PROFILE_NON_BFD_FRWD        SOC_TMC_PORTS_FLP_PROFILE_NON_BFD_FRWD
#define ARAD_PORTS_NOF_FLP_PROFILES                SOC_TMC_PORTS_NOF_FLP_PROFILES


typedef SOC_TMC_PORTS_FLP_PROFILE                  ARAD_PORTS_FLP_PROFILE;

typedef SOC_TMC_PORTS_PROGRAMS_INFO                ARAD_PORTS_PROGRAMS_INFO;

#define ARAD_PORTS_APPLICATION_MAPPING_TYPE_XGS_MAC_EXTENDER SOC_TMC_PORTS_APPLICATION_MAPPING_TYPE_XGS_MAC_EXTENDER
#define ARAD_PORTS_APPLICATION_MAPPING_TYPE_PP_PORT_EXTENDER SOC_TMC_PORTS_APPLICATION_MAPPING_TYPE_PP_PORT_EXTENDER
#define ARAD_PORTS_APPLICATION_MAPPING_NOF_TYPES             SOC_TMC_PORTS_APPLICATION_MAPPING_NOF_TYPES
typedef SOC_TMC_PORTS_APPLICATION_MAPPING_INFO               ARAD_PORTS_APPLICATION_MAPPING_INFO;

typedef SOC_TMC_PORTS_ILKN_CONFIG                  ARAD_PORTS_ILKN_CONFIG;                     
typedef SOC_TMC_PORTS_CAUI_CONFIG                  ARAD_PORTS_CAUI_CONFIG;                     













uint32
  arad_sys_phys_to_local_port_map_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 sys_phys_port_ndx,
    SOC_SAND_IN  uint32                 mapped_fap_id,
    SOC_SAND_IN  uint32                 mapped_fap_port_id
  );


uint32
  arad_sys_phys_to_local_port_map_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 sys_phys_port_ndx,
    SOC_SAND_OUT uint32                 *mapped_fap_id,
    SOC_SAND_OUT uint32                 *mapped_fap_port_id
  );


uint32
  arad_local_to_sys_phys_port_map_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 fap_ndx,
    SOC_SAND_IN  uint32                 fap_local_port_ndx,
    SOC_SAND_OUT uint32                 *sys_phys_port_id
  );


uint32
  arad_modport_to_sys_phys_port_map_get(
    SOC_SAND_IN  int     unit,
    SOC_SAND_IN  uint32  fap_id,            
    SOC_SAND_IN  uint32  tm_port,           
    SOC_SAND_OUT uint32  *sys_phys_port_id  
  );


uint32
  arad_port_to_dynamic_interface_map_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_IN  ARAD_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  ARAD_PORT2IF_MAPPING_INFO *info,
    SOC_SAND_IN  uint8                    is_init
  );

int
  arad_port_to_interface_map_set(
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  soc_port_t          port,
    SOC_SAND_IN  int                 unmap,
    SOC_SAND_IN  int                 is_dynamic_port
  );

 
   
uint32  
    arad_ports_is_port_lag_member( 
              SOC_SAND_IN  int                                 unit, 
              SOC_SAND_IN  int                                 core_id,
              SOC_SAND_IN  uint32                                 port_id, 
              SOC_SAND_OUT uint8                                 *is_in_lag, 
              SOC_SAND_OUT uint32                                 *lag_id); 


uint32
  arad_ports_lag_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  uint32                 lag_ndx,
    SOC_SAND_IN  SOC_PPC_LAG_INFO      *info
  );


uint32
  arad_ports_lag_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 lag_ndx,
    SOC_SAND_OUT SOC_PPC_LAG_INFO      *info_incoming,
    SOC_SAND_OUT SOC_PPC_LAG_INFO      *info_outgoing
  );


uint32
  arad_ports_lag_member_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PORT_DIRECTION                      direction_ndx,
    SOC_SAND_IN  uint32                                 lag_ndx,
    SOC_SAND_IN  ARAD_PORTS_LAG_MEMBER                    *lag_member,
    SOC_SAND_OUT uint8                                 *success
  );


uint32
  arad_ports_lag_sys_port_remove(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  uint32                 lag_ndx,
    SOC_SAND_IN  ARAD_PORTS_LAG_MEMBER  *lag_member
  );


uint32
  arad_ports_lag_order_preserve_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  lag_ndx,
    SOC_SAND_IN  uint8                 is_order_preserving
  );


uint32
  arad_ports_lag_order_preserve_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  lag_ndx,
    SOC_SAND_OUT uint8                 *is_order_preserving
  );

uint32 arad_ports_lag_lb_key_range_set(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  SOC_PPC_LAG_INFO      *info);


uint32
  arad_port_header_type_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_IN  ARAD_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  ARAD_PORT_HEADER_TYPE    header_type
  );


uint32
  arad_port_header_type_get(
    SOC_SAND_IN  int                 unit,
     SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_OUT ARAD_PORT_HEADER_TYPE    *header_type_incoming,
    SOC_SAND_OUT ARAD_PORT_HEADER_TYPE    *header_type_outgoing
  );


uint32
  arad_ports_mirror_inbound_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 ifp_ndx,
    SOC_SAND_IN  ARAD_PORT_INBOUND_MIRROR_INFO *info
  );


uint32
  arad_ports_mirror_inbound_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 ifp_ndx,
    SOC_SAND_OUT ARAD_PORT_INBOUND_MIRROR_INFO *info
  );


uint32
  arad_ports_mirror_outbound_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 ofp_ndx,
    SOC_SAND_IN  ARAD_PORT_OUTBOUND_MIRROR_INFO *info
  );


uint32
  arad_ports_mirror_outbound_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 ofp_ndx,
    SOC_SAND_OUT ARAD_PORT_OUTBOUND_MIRROR_INFO *info
  );


uint32
  arad_ports_snoop_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 snoop_cmd_ndx,
    SOC_SAND_IN  ARAD_PORT_SNOOP_INFO     *info
  );


uint32
  arad_ports_snoop_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 snoop_cmd_ndx,
    SOC_SAND_OUT ARAD_PORT_SNOOP_INFO     *info
  );


uint32
  arad_ports_shaping_header_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  ARAD_PORTS_ISP_INFO      *info
  );


uint32
  arad_ports_shaping_header_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_OUT ARAD_PORTS_ISP_INFO      *info
  );


uint32
  arad_ports_forwarding_header_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  ARAD_PORTS_ITMH          *info
  );


uint32
  arad_ports_forwarding_header_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_OUT ARAD_PORTS_ITMH          *info
  );


uint32
  arad_ports_stag_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  ARAD_PORTS_STAG_FIELDS   *info
  );


uint32
  arad_ports_stag_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_OUT ARAD_PORTS_STAG_FIELDS   *info
  );


uint32
  arad_ports_ftmh_extension_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_PORTS_FTMH_EXT_OUTLIF ext_option
  );


uint32
  arad_ports_ftmh_extension_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT ARAD_PORTS_FTMH_EXT_OUTLIF *ext_option
  );


uint32
  arad_ports_otmh_extension_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_OUT ARAD_PORTS_OTMH_EXTENSIONS_EN *info
  );



uint32
  arad_port_egr_hdr_credit_discount_type_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_PORT_HEADER_TYPE    port_hdr_type_ndx,
    SOC_SAND_IN  ARAD_PORT_EGR_HDR_CR_DISCOUNT_TYPE cr_discnt_type_ndx,
    SOC_SAND_IN  ARAD_PORT_EGR_HDR_CR_DISCOUNT_INFO *info
  );



uint32
  arad_port_egr_hdr_credit_discount_type_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_PORT_HEADER_TYPE    port_hdr_type_ndx,
    SOC_SAND_IN  ARAD_PORT_EGR_HDR_CR_DISCOUNT_TYPE cr_discnt_type_ndx,
    SOC_SAND_OUT ARAD_PORT_EGR_HDR_CR_DISCOUNT_INFO *info
  );



uint32
  arad_port_egr_hdr_credit_discount_select_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_IN  ARAD_PORT_EGR_HDR_CR_DISCOUNT_TYPE cr_discnt_type
  );



uint32
  arad_port_egr_hdr_credit_discount_select_get(
    SOC_SAND_IN  int                    unit, 
    SOC_SAND_IN  int                    core, 
    SOC_SAND_IN  uint32                 tm_port, 
    SOC_SAND_OUT ARAD_PORT_EGR_HDR_CR_DISCOUNT_TYPE *cr_discnt_type
  );

uint32 arad_port_stacking_info_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  int                              core_id,
    SOC_SAND_IN  uint32                              local_port_ndx,
    SOC_SAND_IN  uint32                              is_stacking,
    SOC_SAND_IN  uint32                              peer_tmd);

uint32 arad_port_stacking_info_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  int                              core_id,
    SOC_SAND_IN  uint32                              local_port_ndx,
    SOC_SAND_OUT  uint32                          *is_stacking,
    SOC_SAND_OUT  uint32                          *peer_tmd);

uint32 arad_port_stacking_route_history_bitmap_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  int                              core_id,
    SOC_SAND_IN  uint32                           tm_port,
    SOC_SAND_IN  SOC_TMC_STACK_EGR_PROG_TM_PORT_PROFILE_STACK tm_port_profile_stack,
    SOC_SAND_IN  uint32                              bitmap);


uint32
  arad_port_pp_port_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 pp_port_ndx,
    SOC_SAND_IN  ARAD_PORT_PP_PORT_INFO         *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE         *success
  );


uint32
  arad_port_pp_port_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                     core_id,
    SOC_SAND_IN  uint32                    pp_port_ndx,
    SOC_SAND_OUT ARAD_PORT_PP_PORT_INFO     *info
  );


uint32
  arad_port_to_pp_port_map_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  soc_port_t             port,
    SOC_SAND_IN  ARAD_PORT_DIRECTION    direction_ndx
  );


uint32
  arad_port_to_pp_port_map_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    port_ndx,
    SOC_SAND_OUT uint32                    *pp_port_in,
    SOC_SAND_OUT uint32                    *pp_port_out
  );
  


uint32 
  arad_port_direct_lb_key_min_set(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN int core_id,
    SOC_SAND_IN uint32  local_port,
    SOC_SAND_IN uint32 min_lb_key
   );

uint32 
  arad_port_direct_lb_key_max_set(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN int core_id, 
    SOC_SAND_IN uint32  local_port,
    SOC_SAND_IN uint32 max_lb_key
   );

uint32 
  arad_port_direct_lb_key_min_get(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN int core_id, 
    SOC_SAND_IN uint32  local_port,
    uint32* min_lb_key
    );

uint32 
  arad_port_direct_lb_key_max_get(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN int core_id, 
    SOC_SAND_IN uint32  local_port,
    uint32* max_lb_key
    );

#ifdef BCM_88660_A0

uint32
   arad_port_synchronize_lb_key_tables_at_egress(
   SOC_SAND_IN int unit
    );


uint32
  arad_port_switch_lb_key_tables(
     SOC_SAND_IN int unit
    );

#endif 

uint32
  arad_port_encap_config_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core_id,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  ARAD_L2_ENCAP_INFO       *info
  );
uint32
  arad_port_encap_config_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core_id,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_OUT ARAD_L2_ENCAP_INFO       *info
  );


uint32
  arad_ports_swap_set(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID           port_ndx,
    SOC_SAND_IN  ARAD_PORTS_SWAP_INFO       *info
  );


uint32
  arad_ports_swap_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID           port_ndx,
    SOC_SAND_OUT ARAD_PORTS_SWAP_INFO       *info
  );



uint32
  arad_ports_pon_tunnel_info_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  ARAD_PON_TUNNEL_ID       tunnel,
    SOC_SAND_IN  ARAD_PORTS_PON_TUNNEL_INFO *info
  );



uint32
  arad_ports_pon_tunnel_info_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  ARAD_PON_TUNNEL_ID       tunnel,
    SOC_SAND_OUT ARAD_PORTS_PON_TUNNEL_INFO *info
  );



uint32
  arad_ports_programs_info_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  ARAD_PORTS_PROGRAMS_INFO *info
  );



uint32
  arad_ports_programs_info_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_OUT ARAD_PORTS_PROGRAMS_INFO *info
  );

void
  ARAD_PORT_PP_PORT_INFO_clear(
    SOC_SAND_OUT ARAD_PORT_PP_PORT_INFO *info
  );

void
  ARAD_PORT_COUNTER_INFO_clear(
    SOC_SAND_OUT ARAD_PORT_COUNTER_INFO *info
  );


void
  arad_ARAD_PORT2IF_MAPPING_INFO_clear(
    SOC_SAND_OUT ARAD_PORT2IF_MAPPING_INFO *info
  );
void
  arad_ARAD_PORTS_LAG_MEMBER_clear(
    SOC_SAND_OUT ARAD_PORTS_LAG_MEMBER *info
  );




uint32 
  arad_ports_application_mapping_info_set (
    SOC_SAND_IN int unit, 
    SOC_SAND_IN ARAD_FAP_PORT_ID port_ndx, 
    SOC_SAND_IN ARAD_PORTS_APPLICATION_MAPPING_INFO *info    
    );


uint32 
  arad_ports_application_mapping_info_get (
    SOC_SAND_IN int unit, 
    SOC_SAND_IN ARAD_FAP_PORT_ID port_ndx, 
    SOC_SAND_INOUT ARAD_PORTS_APPLICATION_MAPPING_INFO *info    
    );

void
  arad_ARAD_PORTS_ITMH_clear(
    SOC_SAND_OUT ARAD_PORTS_ITMH *info
  );
void
  arad_ARAD_PORT_LAG_SYS_PORT_INFO_clear(
    SOC_SAND_OUT ARAD_PORT_LAG_SYS_PORT_INFO *info
  );

void
  arad_ARAD_PORTS_OTMH_EXTENSIONS_EN_clear(
    SOC_SAND_OUT ARAD_PORTS_OTMH_EXTENSIONS_EN *info
  );

void
  arad_ARAD_PORTS_SWAP_INFO_clear(
    SOC_SAND_OUT ARAD_PORTS_SWAP_INFO *info
  );

void
  arad_ARAD_PORTS_PON_TUNNEL_INFO_clear(
    SOC_SAND_OUT ARAD_PORTS_PON_TUNNEL_INFO *info
  );

void
  arad_ARAD_PORTS_PROGRAMS_INFO_clear(
    SOC_SAND_OUT ARAD_PORTS_PROGRAMS_INFO *info
  );

#if ARAD_DEBUG_IS_LVL1
void
  ARAD_PORT_PP_PORT_INFO_print(
    SOC_SAND_IN  ARAD_PORT_PP_PORT_INFO *info
  );
const char*
  arad_ARAD_PORT_HEADER_TYPE_to_string(
    SOC_SAND_IN ARAD_PORT_HEADER_TYPE enum_val
  );

const char*
  arad_ARAD_PORTS_FTMH_EXT_OUTLIF_to_string(
    SOC_SAND_IN ARAD_PORTS_FTMH_EXT_OUTLIF enum_val
  );
void
  arad_ARAD_PORT2IF_MAPPING_INFO_print(
    SOC_SAND_IN ARAD_PORT2IF_MAPPING_INFO *info
  );

void
  arad_ARAD_PORTS_SWAP_INFO_print(
    SOC_SAND_IN ARAD_PORTS_SWAP_INFO *info
  );


#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif


