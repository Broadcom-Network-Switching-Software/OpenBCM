/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPC_API_VSI_INCLUDED__

#define __SOC_PPC_API_VSI_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_general.h>





#define SOC_PPC_VSI_FID_IS_VSID                                (0xffffffff)

#define SOC_PPC_VSI_LOW_CNFG_VALUE                          (1)

#define SOC_PPC_VSI_L2CP_KEY_L2CP_PROFILE_SIZE              (64)


#define SOC_PPC_VSI_L2CP_KEY_ENTRY_OFFSET(l2cp_profile, da_mac_address_lsb)  \
                                         (l2cp_profile*SOC_PPC_VSI_L2CP_KEY_L2CP_PROFILE_SIZE + da_mac_address_lsb)


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 l2cp_profile_ingress;
  
  uint32 l2cp_profile_egress;
  
  uint32 da_mac_address_lsb;

} SOC_PPC_VSI_L2CP_KEY;

typedef enum
{
  
  SOC_PPC_VSI_L2CP_HANDLE_TYPE_NORMAL = 0,
  
  SOC_PPC_VSI_L2CP_HANDLE_TYPE_TUNNEL = 1,
  
  SOC_PPC_VSI_L2CP_HANDLE_TYPE_PEER = 2,
  
  SOC_PPC_VSI_L2CP_HANDLE_TYPE_DROP = 3,
  
  SOC_PPC_VSI_L2CP_HANDLE_TYPES = 4
}SOC_PPC_VSI_L2CP_HANDLE_TYPE;










typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_FRWRD_DECISION_INFO default_forwarding;
  
  uint32 default_forward_profile;
  
  uint32 stp_topology_id;
  
  uint8 enable_my_mac;
  
  uint32 fid_profile_id;
  
  uint32 mac_learn_profile_id;

   
  uint32 profile_ingress;
   
  uint32 profile_egress;
  
  uint8 clear_on_destroy;
} SOC_PPC_VSI_INFO;










void
  SOC_PPC_VSI_L2CP_KEY_clear(
    SOC_SAND_OUT SOC_PPC_VSI_L2CP_KEY *info
  );

void
  SOC_PPC_VSI_INFO_clear(
    SOC_SAND_OUT SOC_PPC_VSI_INFO *info
  );

#if SOC_PPC_DEBUG_IS_LVL1

void
  SOC_PPC_VSI_INFO_print(
    SOC_SAND_IN  SOC_PPC_VSI_INFO *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

