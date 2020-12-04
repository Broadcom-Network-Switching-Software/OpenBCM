/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPC_API_EG_FILTER_INCLUDED__

#define __SOC_PPC_API_EG_FILTER_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_general.h>













typedef enum
{
  
  SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_PROMISCUOUS = 0,
  
  SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_ISOLATED = 1,
  
  SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_COMMUNITY = 2,
  
  SOC_PPC_NOF_EG_FILTER_PVLAN_PORT_TYPES = 3
}SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE;

typedef enum
{
  
  SOC_PPC_EG_FILTER_PORT_ENABLE_NONE = 0x0,
  
  SOC_PPC_EG_FILTER_PORT_ENABLE_VSI_MEMBERSHIP = 0x1,
  
  SOC_PPC_EG_FILTER_PORT_ENABLE_SAME_INTERFACE = 0x2,
  
  SOC_PPC_EG_FILTER_PORT_ENABLE_UC_UNKNOW_DA = 0x4,
  
  SOC_PPC_EG_FILTER_PORT_ENABLE_MC_UNKNOW_DA = 0x8,
  
  SOC_PPC_EG_FILTER_PORT_ENABLE_BC_UNKNOW_DA = 0x10,
  
  SOC_PPC_EG_FILTER_PORT_ENABLE_MTU = 0x20,
  
  SOC_PPC_EG_FILTER_PORT_ENABLE_STP = 0x40,
  
  SOC_PPC_EG_FILTER_PORT_ENABLE_PEP_ACCEPTABLE_FRM_TYPES = 0x80,
  
  SOC_PPC_EG_FILTER_PORT_ENABLE_SPLIT_HORIZON = 0x100,
  
  SOC_PPC_EG_EGQ_FILTER_PORT_ENABLE_NONE = 0x200,
  
  SOC_PPC_EG_EPNI_FILTER_PORT_ENABLE_NONE = 0x400,

  
  SOC_PPC_EG_FILTER_PORT_ENABLE_ALL = (int)0xffffffff,
  
  SOC_PPC_NOF_EG_FILTER_PORT_ENABLES = 11
}SOC_PPC_EG_FILTER_PORT_ENABLE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 filter_mask;
  
  uint32 acceptable_frames_profile;

} SOC_PPC_EG_FILTER_PORT_INFO;

typedef struct 
{
  SOC_SAND_MAGIC_NUM_VAR
  
  
  uint32 in_lif_profile_disable_same_interface_filter_bitmap;


uint32 in_lif_profile_enable_logical_intf_same_filter_bitmap;

} SOC_PPC_EG_FILTER_GLOBAL_INFO;









void
  SOC_PPC_EG_FILTER_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_FILTER_PORT_INFO *info
  );

#ifdef BCM_88660
void
  SOC_PPC_EG_FILTER_GLOBAL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_FILTER_GLOBAL_INFO *info
  );
#endif 

#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE enum_val
  );

const char*
  SOC_PPC_EG_FILTER_PORT_ENABLE_to_string(
    SOC_SAND_IN  SOC_PPC_EG_FILTER_PORT_ENABLE enum_val
  );

void
  SOC_PPC_EG_FILTER_PORT_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_FILTER_PORT_INFO *info
  );

#ifdef BCM_88660

void 
  SOC_PPC_EG_FILTER_GLOBAL_INFO_print(
    SOC_SAND_IN SOC_PPC_EG_FILTER_GLOBAL_INFO *info
  );
#endif 

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

