/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __SOC_TMC_API_MULTICAST_EGRESS_INCLUDED__

#define __SOC_TMC_API_MULTICAST_EGRESS_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/TMC/tmc_api_general.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>






#define SOC_TMC_MULT_EG_NOF_UINT32S_IN_BITMAP_PETRA   SOC_SAND_DIV_ROUND_UP(SOC_TMC_NOF_FAP_PORTS_PETRA, SOC_SAND_REG_SIZE_BITS)
#define SOC_TMC_MULT_EG_NOF_UINT32S_IN_BITMAP_ARAD    SOC_SAND_DIV_ROUND_UP(SOC_TMC_NOF_FAP_PORTS_ARAD, SOC_SAND_REG_SIZE_BITS)
#define SOC_TMC_MULT_EG_NOF_UINT32S_IN_BITMAP_MAX     SOC_SAND_DIV_ROUND_UP(SOC_TMC_NOF_FAP_PORTS_JERICHO, SOC_SAND_REG_SIZE_BITS)


#define ARAD_MC_EGR_IS_BITMAP_BIT 0x80000000



#define SOC_TMC_MULT_EG_VLAN_NOF_IDS_MAX (4 * 1024)




#define SOC_TMC_MULT_EG_VLAN_NOF_IDS_MAX_ARAD (8 * 1024)











typedef enum
{
  
  SOC_TMC_MULT_EG_ENTRY_TYPE_OFP = 0,
  
  SOC_TMC_MULT_EG_ENTRY_TYPE_VLAN_PTR = 1,
  
  SOC_TMC_MULT_EG_ENTRY_NOF_TYPES = 2
}SOC_TMC_MULT_EG_ENTRY_TYPE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 mc_id_low;
  
  int32 mc_id_high;

}SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_MULT_EG_ENTRY_TYPE type;
  
  SOC_TMC_FAP_PORT_ID port;
  
  uint32 vlan_mc_id;
  
  uint32 cud;

}SOC_TMC_MULT_EG_ENTRY;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 bitmap[SOC_TMC_MULT_EG_NOF_UINT32S_IN_BITMAP_MAX + 1]; 
 
}SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP;











void
  SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_clear(
    SOC_SAND_OUT SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE *info
  );

void
  SOC_TMC_MULT_EG_ENTRY_clear(
    SOC_SAND_OUT SOC_TMC_MULT_EG_ENTRY *info
  );

void
  SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_clear(
    SOC_SAND_OUT SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP *info
  );

#if SOC_TMC_DEBUG_IS_LVL1

const char*
  SOC_TMC_MULT_EG_ENTRY_TYPE_to_string(
    SOC_SAND_IN  SOC_TMC_MULT_EG_ENTRY_TYPE enum_val
  );

void
  SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_print(
    SOC_SAND_IN SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE *info
  );

void
  SOC_TMC_MULT_EG_ENTRY_print(
    SOC_SAND_IN SOC_TMC_MULT_EG_ENTRY *info
  );

void
  SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_print(
    SOC_SAND_IN SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
