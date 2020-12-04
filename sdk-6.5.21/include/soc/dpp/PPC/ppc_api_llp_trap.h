/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPC_API_LLP_TRAP_INCLUDED__

#define __SOC_PPC_API_LLP_TRAP_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_general.h>






#define  SOC_PPC_TRAP_NOF_MY_IPS (2)









typedef enum
{
  
  SOC_PPC_LLP_TRAP_PORT_ENABLE_NONE = 0,
  
  SOC_PPC_LLP_TRAP_PORT_ENABLE_ARP = 1,
  
  SOC_PPC_LLP_TRAP_PORT_ENABLE_IGMP = 2,
  
  SOC_PPC_LLP_TRAP_PORT_ENABLE_MLD = 4,
  
  SOC_PPC_LLP_TRAP_PORT_ENABLE_DHCP = 8,
  
  SOC_PPC_LLP_TRAP_PORT_ENABLE_PROG_TRAP_0 = 0x10,
  
  SOC_PPC_LLP_TRAP_PORT_ENABLE_PROG_TRAP_1 = 0x20,
  
  SOC_PPC_LLP_TRAP_PORT_ENABLE_PROG_TRAP_2 = 0x40,
  
  SOC_PPC_LLP_TRAP_PORT_ENABLE_PROG_TRAP_3 = 0x80,
   
  SOC_PPC_LLP_TRAP_PORT_ENABLE_ING_VLAN_MEMBERSHIP = 0x100, 
  
  SOC_PPC_LLP_TRAP_PORT_ENABLE_SAME_INTERFACE = 0x200,
  
  SOC_PPC_LLP_TRAP_PORT_ENABLE_ICMP_REDIRECT = 0x400,
  
  SOC_PPC_LLP_TRAP_PORT_ENABLE_ALL = (int)0xFFFFFFFF,
  
  SOC_PPC_NOF_LLP_TRAP_PORT_ENABLES = 11
}SOC_PPC_LLP_TRAP_PORT_ENABLE;

typedef enum
{
  
  SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_NONE = 0,
  
  SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_DA = 1,
  
  SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_ETHER_TYPE = 2,
  
  SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_SUB_TYPE = 4,
  
  SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_IP_PRTCL = 8,
  
  SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_L4_PORTS = 16,
  
  SOC_PPC_NOF_LLP_TRAP_PROG_TRAP_COND_SELECTS = 6
}SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 reserved_mc_profile;
  
  uint32 trap_enable_mask;

} SOC_PPC_LLP_TRAP_PORT_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 my_ips[SOC_PPC_TRAP_NOF_MY_IPS];
  
  uint8 ignore_da;

} SOC_PPC_LLP_TRAP_ARP_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 reserved_mc_profile;
  
  uint32 da_mac_address_lsb;

} SOC_PPC_LLP_TRAP_RESERVED_MC_KEY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_SAND_PP_MAC_ADDRESS dest_mac;
  
  uint8 dest_mac_nof_bits;
  
  SOC_SAND_PP_ETHER_TYPE ether_type;
  
  uint8 sub_type;
  
  uint8 sub_type_bitmap;

} SOC_PPC_LLP_TRAP_PROG_TRAP_L2_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 ip_protocol;

} SOC_PPC_LLP_TRAP_PROG_TRAP_L3_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint16 src_port;
  
  uint16 src_port_bitmap;
  
  uint16 dest_port;
  
  uint16 dest_port_bitmap;

} SOC_PPC_LLP_TRAP_PROG_TRAP_L4_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_LLP_TRAP_PROG_TRAP_L2_INFO l2_info;
  
  SOC_PPC_LLP_TRAP_PROG_TRAP_L3_INFO l3_info;
  
  SOC_PPC_LLP_TRAP_PROG_TRAP_L4_INFO l4_info;
  
  uint32 enable_bitmap;
  
  uint32 inverse_bitmap;

} SOC_PPC_LLP_TRAP_PROG_TRAP_QUALIFIER;










void
  SOC_PPC_LLP_TRAP_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_TRAP_PORT_INFO *info
  );

void
  SOC_PPC_LLP_TRAP_ARP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_TRAP_ARP_INFO *info
  );

void
  SOC_PPC_LLP_TRAP_RESERVED_MC_KEY_clear(
    SOC_SAND_OUT SOC_PPC_LLP_TRAP_RESERVED_MC_KEY *info
  );

void
  SOC_PPC_LLP_TRAP_PROG_TRAP_L2_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_TRAP_PROG_TRAP_L2_INFO *info
  );

void
  SOC_PPC_LLP_TRAP_PROG_TRAP_L3_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_TRAP_PROG_TRAP_L3_INFO *info
  );

void
  SOC_PPC_LLP_TRAP_PROG_TRAP_L4_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_TRAP_PROG_TRAP_L4_INFO *info
  );

void
  SOC_PPC_LLP_TRAP_PROG_TRAP_QUALIFIER_clear(
    SOC_SAND_OUT SOC_PPC_LLP_TRAP_PROG_TRAP_QUALIFIER *info
  );

#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_LLP_TRAP_PORT_ENABLE_to_string(
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_PORT_ENABLE enum_val
  );

const char*
  SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_to_string(
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT enum_val
  );

void
  SOC_PPC_LLP_TRAP_PORT_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_PORT_INFO *info
  );

void
  SOC_PPC_LLP_TRAP_ARP_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_ARP_INFO *info
  );

void
  SOC_PPC_LLP_TRAP_RESERVED_MC_KEY_print(
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_RESERVED_MC_KEY *info
  );

void
  SOC_PPC_LLP_TRAP_PROG_TRAP_L2_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_PROG_TRAP_L2_INFO *info
  );

void
  SOC_PPC_LLP_TRAP_PROG_TRAP_L3_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_PROG_TRAP_L3_INFO *info
  );

void
  SOC_PPC_LLP_TRAP_PROG_TRAP_L4_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_PROG_TRAP_L4_INFO *info
  );

void
  SOC_PPC_LLP_TRAP_PROG_TRAP_QUALIFIER_print(
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_PROG_TRAP_QUALIFIER *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

