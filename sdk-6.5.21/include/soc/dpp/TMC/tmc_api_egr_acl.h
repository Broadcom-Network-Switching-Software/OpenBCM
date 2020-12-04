/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __SOC_TMC_API_EGR_ACL_INCLUDED__

#define __SOC_TMC_API_EGR_ACL_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/TMC/tmc_api_general.h>
#include <soc/dpp/TMC/tmc_api_tcam.h>
#include <soc/dpp/TMC/tmc_api_tcam_key.h>






#define  SOC_TMC_EGR_ACL_NOF_DP_VALUES (2)









typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 val[SOC_TMC_EGR_ACL_NOF_DP_VALUES];

} SOC_TMC_EGR_ACL_DP_VALUES;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 profile;
  
  uint32 acl_data;

} SOC_TMC_EGR_ACL_PORT_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 trap_en;
  
  uint32 trap_code;
  
  uint8 ofp_ov;
  
  uint32 ofp;
  
  uint8 dp_tc_ov;
  
  uint32 tc;
  
  uint32 dp;
  
  uint32 cud;

} SOC_TMC_EGR_ACL_ACTION_VAL;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_EGR_ACL_DB_TYPE type;
  
  SOC_TMC_TCAM_KEY key;
  
  uint16 priority;
  
  SOC_TMC_EGR_ACL_ACTION_VAL action_val;

} SOC_TMC_EGR_ACL_ENTRY_INFO;










void
  SOC_TMC_EGR_ACL_DP_VALUES_clear(
    SOC_SAND_OUT SOC_TMC_EGR_ACL_DP_VALUES *info
  );

void
  SOC_TMC_EGR_ACL_PORT_INFO_clear(
    SOC_SAND_OUT SOC_TMC_EGR_ACL_PORT_INFO *info
  );

void
  SOC_TMC_EGR_ACL_ACTION_VAL_clear(
    SOC_SAND_OUT SOC_TMC_EGR_ACL_ACTION_VAL *info
  );

void
  SOC_TMC_EGR_ACL_ENTRY_INFO_clear(
    SOC_SAND_OUT SOC_TMC_EGR_ACL_ENTRY_INFO *info
  );

#if SOC_TMC_DEBUG_IS_LVL1

void
  SOC_TMC_EGR_ACL_DP_VALUES_print(
    SOC_SAND_IN  SOC_TMC_EGR_ACL_DP_VALUES *info
  );

void
  SOC_TMC_EGR_ACL_PORT_INFO_print(
    SOC_SAND_IN  SOC_TMC_EGR_ACL_PORT_INFO *info
  );

void
  SOC_TMC_EGR_ACL_ACTION_VAL_print(
    SOC_SAND_IN  SOC_TMC_EGR_ACL_ACTION_VAL *info
  );

void
  SOC_TMC_EGR_ACL_ENTRY_INFO_print(
    SOC_SAND_IN  SOC_TMC_EGR_ACL_ENTRY_INFO *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

