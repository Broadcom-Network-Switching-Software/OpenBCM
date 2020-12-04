/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPC_API_MYMAC_INCLUDED__

#define __SOC_PPC_API_MYMAC_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/dpp_config_defs.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_general.h>




#define SOC_PPC_MYMAC_VIRTUAL_NICK_NAMES_SIZE 3

#define SOC_PPC_MYMAC_NOF_MY_NICKNAMES        (SOC_PPC_MYMAC_VIRTUAL_NICK_NAMES_SIZE + 1)

#define SOC_PPC_VRRP_CAM_MAX_NOF_ENTRIES(_unit)         (SOC_DPP_DEFS_GET(unit, vrrp_mymac_cam_size))

#define SOC_PPC_VRRP_MAX_VSI(_unit)                     (SOC_DPP_DEFS_GET(unit, vrrp_mymac_map_size))

#define SOC_PPC_L3_NOF_PROTOCOL_GROUPS(unit)            (SOC_DPP_DEFS_GET(unit, vrrp_nof_protocol_groups))

#define SOC_PPC_L3_VRRP_PROTOCOL_GROUP_NONE             (0)
#define SOC_PPC_L3_VRRP_PROTOCOL_GROUP_IPV4             (1 << soc_ppc_parsed_l2_next_protcol_ipv4)
#define SOC_PPC_L3_VRRP_PROTOCOL_GROUP_IPV6             (1 << soc_ppc_parsed_l2_next_protcol_ipv6)
#define SOC_PPC_L3_VRRP_PROTOCOL_GROUP_ARP              (1 << soc_ppc_parsed_l2_next_protcol_arp)
#define SOC_PPC_L3_VRRP_PROTOCOL_GROUP_MPLS             (1 << soc_ppc_parsed_l2_next_protcol_mpls)
#define SOC_PPC_L3_VRRP_PROTOCOL_GROUP_MIM              (1 << soc_ppc_parsed_l2_next_protcol_mim)
#define SOC_PPC_L3_VRRP_PROTOCOL_GROUP_TRILL            (1 << soc_ppc_parsed_l2_next_protcol_trill)
#define SOC_PPC_L3_VRRP_PROTOCOL_GROUP_FCOE             (1 << soc_ppc_parsed_l2_next_protcol_fcoe)
#define SOC_PPC_L3_VRRP_PROTOCOL_GROUP_INVALID          ((uint32) -1)

#define SOC_PPC_L3_VRRP_PROTOCOL_GROUP_ALL_VALID\
        (SOC_PPC_L3_VRRP_PROTOCOL_GROUP_NONE    \
         |SOC_PPC_L3_VRRP_PROTOCOL_GROUP_IPV4   \
         |SOC_PPC_L3_VRRP_PROTOCOL_GROUP_IPV6   \
         |SOC_PPC_L3_VRRP_PROTOCOL_GROUP_ARP    \
         |SOC_PPC_L3_VRRP_PROTOCOL_GROUP_MPLS   \
         |SOC_PPC_L3_VRRP_PROTOCOL_GROUP_MIM    \
         |SOC_PPC_L3_VRRP_PROTOCOL_GROUP_TRILL  \
         |SOC_PPC_L3_VRRP_PROTOCOL_GROUP_FCOE)    






#define SOC_PPC_PARSED_L2_NEXT_PROTOCOL_TO_L3_PROTOCOL_FLAG(_l2_parsed)  \
        (1 << (_l2_parsed))






typedef enum
{
  
  SOC_PPC_MYMAC_VRRP_MODE_ALL_VSI_BASED = 0,
  
  SOC_PPC_MYMAC_VRRP_MODE_256_VSI_BASED = 1,
  
  SOC_PPC_MYMAC_VRRP_MODE_512_VSI_BASED = 2,
  
  SOC_PPC_MYMAC_VRRP_MODE_1K_VSI_BASED = 3,
  
  SOC_PPC_MYMAC_VRRP_MODE_2K_VSI_BASED = 4,
  
  SOC_PPC_NOF_MYMAC_VRRP_MODES = 5
}SOC_PPC_MYMAC_VRRP_MODE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 enable;
  
  SOC_PPC_MYMAC_VRRP_MODE mode;
  
  SOC_SAND_PP_MAC_ADDRESS vrid_my_mac_msb;
  
  SOC_SAND_PP_MAC_ADDRESS ipv6_vrid_my_mac_msb;
  
  uint8 ipv6_enable;

} SOC_PPC_MYMAC_VRRP_INFO;



typedef struct 
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_SAND_PP_MAC_ADDRESS mac_addr;
  
  SOC_SAND_PP_MAC_ADDRESS mac_mask;
  
  uint32 protocol_group;
  
  uint32 protocol_group_mask;
  
  uint8 is_ipv4_entry;
  
  uint8 vrrp_cam_index;
  
  uint32 flags;

} SOC_PPC_VRRP_CAM_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 my_nick_name;
  
  uint32 virtual_nick_names[SOC_PPC_MYMAC_VIRTUAL_NICK_NAMES_SIZE];

  
  SOC_SAND_PP_MAC_ADDRESS all_rbridges_mac;
  
  SOC_SAND_PP_MAC_ADDRESS all_esadi_rbridges;

  
  uint32 skip_ethernet_my_nickname; 

  
  uint32 skip_ethernet_virtual_nick_names[SOC_PPC_MYMAC_VIRTUAL_NICK_NAMES_SIZE]; 

} SOC_PPC_MYMAC_TRILL_INFO;

typedef enum {
    soc_ppc_parsed_l2_next_protcol_none            =   0 ,
    soc_ppc_parsed_l2_next_protcol_user_def_first  =   1 ,
    soc_ppc_parsed_l2_next_protcol_user_def_last   =   7 ,
    soc_ppc_parsed_l2_next_protcol_trill           =   8 ,
    soc_ppc_parsed_l2_next_protcol_mim             =   9 ,
    soc_ppc_parsed_l2_next_protcol_arp             =   10,
    soc_ppc_parsed_l2_next_protcol_cfm             =   11,
    soc_ppc_parsed_l2_next_protcol_fcoe            =   12,
    soc_ppc_parsed_l2_next_protcol_ipv4            =   13,
    soc_ppc_parsed_l2_next_protcol_ipv6            =   14,
    soc_ppc_parsed_l2_next_protcol_mpls            =   15,
    soc_ppc_parsed_l2_next_protcol_count           =   16
} soc_ppc_parsed_l2_next_prtocol_t;









void
  SOC_PPC_MYMAC_VRRP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_MYMAC_VRRP_INFO *info
  );

void
  SOC_PPC_MYMAC_TRILL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_MYMAC_TRILL_INFO *info
  );

#ifdef BCM_88660_A0

void 
  SOC_PPC_VRRP_CAM_INFO_clear(
   SOC_SAND_OUT SOC_PPC_VRRP_CAM_INFO *info
  );

#endif 

#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_MYMAC_VRRP_MODE_to_string(
    SOC_SAND_IN  SOC_PPC_MYMAC_VRRP_MODE enum_val
  );

void
  SOC_PPC_MYMAC_VRRP_INFO_print(
    SOC_SAND_IN  SOC_PPC_MYMAC_VRRP_INFO *info
  );

void
  SOC_PPC_MYMAC_TRILL_INFO_print(
    SOC_SAND_IN  SOC_PPC_MYMAC_TRILL_INFO *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

