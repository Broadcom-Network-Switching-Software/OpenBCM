/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPC_API_L3_SRC_BIND_INCLUDED__

#define __SOC_PPC_API_L3_SRC_BIND_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_general.h>













#define SOC_PPC_IP6_COMPRESSION_DIP 0x01 
#define SOC_PPC_IP6_COMPRESSION_SIP 0x02 
#define SOC_PPC_IP6_COMPRESSION_TCAM 0x04 
#define SOC_PPC_IP6_COMPRESSION_LEM  0x08 

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_LIF_ID lif_ndx;
  uint16         pppoe_session_id;
} SOC_PPC_SRC_BIND_PPPOE_ENTRY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_SAND_PP_MAC_ADDRESS smac;
  
  uint8 smac_valid;
  
  uint32 sip;
  
  SOC_PPC_LIF_ID lif_ndx;
  
  uint8 prefix_len;
  
  uint8 is_network;
  
  SOC_PPC_VSI_ID  vsi_id;
} SOC_PPC_SRC_BIND_IPV4_ENTRY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_SAND_PP_MAC_ADDRESS smac;
  
  uint8 smac_valid;
  
  SOC_SAND_PP_IPV6_ADDRESS sip6;

  
  SOC_PPC_LIF_ID lif_ndx;
  
  uint8 prefix_len;
  
  uint8 is_network;
} SOC_PPC_SRC_BIND_IPV6_ENTRY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 flags;
  
  SOC_SAND_PP_IPV6_SUBNET ip6;
  
  
  SOC_PPC_LIF_ID lif_ndx;

  
  uint16 ip6_tt_compressed_result;

  
  uint32 ip6_compressed_result;
  
  uint8 is_spoof;
  
  SOC_PPC_VSI_ID  vsi_id;
} SOC_PPC_IPV6_COMPRESSED_ENTRY;









void
  SOC_PPC_SRC_BIND_IPV4_ENTRY_clear(
    SOC_SAND_OUT SOC_PPC_SRC_BIND_IPV4_ENTRY     *info
  );

void
  SOC_PPC_SRC_BIND_IPV6_ENTRY_clear(
    SOC_SAND_OUT SOC_PPC_SRC_BIND_IPV6_ENTRY     *info
  );

void
  SOC_PPC_SRC_BIND_IPV4_ENTRY_print(
    SOC_SAND_OUT SOC_PPC_SRC_BIND_IPV4_ENTRY     *info
  );

void
  SOC_PPC_SRC_BIND_IPV6_ENTRY_print(
    SOC_SAND_OUT SOC_PPC_SRC_BIND_IPV6_ENTRY     *info
  );

void
  SOC_PPC_SRC_BIND_PPPOE_ENTRY_clear(
    SOC_SAND_OUT SOC_PPC_SRC_BIND_PPPOE_ENTRY     *info
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

