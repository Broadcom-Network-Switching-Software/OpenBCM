/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_TMC_API_TDM_INCLUDED__

#define __SOC_TMC_API_TDM_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Utils/sand_u64.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/TMC/tmc_api_general.h>












typedef enum
{
  
  SOC_TMC_TDM_ING_ACTION_ADD = 0,
  
  SOC_TMC_TDM_ING_ACTION_NO_CHANGE = 1,
  
  SOC_TMC_TDM_ING_ACTION_CUSTOMER_EMBED = 2,
  
  SOC_TMC_TDM_NOF_ING_ACTIONS = 3
}SOC_TMC_TDM_ING_ACTION;

typedef enum
{
  
  SOC_TMC_TDM_EG_ACTION_REMOVE = 0,
  
  SOC_TMC_TDM_EG_ACTION_NO_CHANGE = 1,
  
  SOC_TMC_TDM_EG_ACTION_CUSTOMER_EXTRACT = 2,
  
  SOC_TMC_TDM_NOF_EG_ACTIONS = 3
}SOC_TMC_TDM_EG_ACTION;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 dest_if;
  
  uint32 dest_fap_id;

} SOC_TMC_TDM_FTMH_OPT_UC;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 mc_id;

} SOC_TMC_TDM_FTMH_OPT_MC;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 user_def;  
  
  uint32 sys_phy_port;
  
  uint32 dest_fap_id;
  
  uint32 dest_fap_port;
  
  uint32 user_def_2;

} SOC_TMC_TDM_FTMH_STANDARD_UC;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 user_def;
  
  uint32 mc_id;

} SOC_TMC_TDM_FTMH_STANDARD_MC;

typedef union
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_TDM_FTMH_OPT_UC opt_uc;
  
  SOC_TMC_TDM_FTMH_OPT_MC opt_mc;
  
  SOC_TMC_TDM_FTMH_STANDARD_UC standard_uc;
  
  SOC_TMC_TDM_FTMH_STANDARD_MC standard_mc;

} SOC_TMC_TDM_FTMH;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_TDM_ING_ACTION action_ing;
  
  uint8 is_mc;
  
  SOC_TMC_TDM_FTMH ftmh;
  
  SOC_TMC_TDM_EG_ACTION action_eg;
  
} SOC_TMC_TDM_FTMH_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_SAND_U64 link_bitmap;

} SOC_TMC_TDM_MC_STATIC_ROUTE_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_SAND_U64 link_bitmap;

} SOC_TMC_TDM_DIRECT_ROUTING_INFO;









void
  SOC_TMC_TDM_FTMH_OPT_UC_clear(
    SOC_SAND_OUT SOC_TMC_TDM_FTMH_OPT_UC *info
  );

void
  SOC_TMC_TDM_FTMH_OPT_MC_clear(
    SOC_SAND_OUT SOC_TMC_TDM_FTMH_OPT_MC *info
  );

void
  SOC_TMC_TDM_FTMH_STANDARD_UC_clear(
    SOC_SAND_OUT SOC_TMC_TDM_FTMH_STANDARD_UC *info
  );

void
  SOC_TMC_TDM_FTMH_STANDARD_MC_clear(
    SOC_SAND_OUT SOC_TMC_TDM_FTMH_STANDARD_MC *info
  );

void
  SOC_TMC_TDM_FTMH_clear(
    SOC_SAND_OUT SOC_TMC_TDM_FTMH *info
  );

void
  SOC_TMC_TDM_FTMH_INFO_clear(
    SOC_SAND_OUT SOC_TMC_TDM_FTMH_INFO *info
  );
void
  SOC_TMC_TDM_MC_STATIC_ROUTE_INFO_clear(
    SOC_SAND_OUT SOC_TMC_TDM_MC_STATIC_ROUTE_INFO *info
  );

void
  SOC_TMC_TDM_DIRECT_ROUTING_INFO_clear(
    SOC_SAND_OUT SOC_TMC_TDM_DIRECT_ROUTING_INFO *info
  );

#if SOC_TMC_DEBUG_IS_LVL1

const char*
  SOC_TMC_TDM_ING_ACTION_to_string(
    SOC_SAND_IN  SOC_TMC_TDM_ING_ACTION enum_val
  );

const char*
  SOC_TMC_TDM_EG_ACTION_to_string(
    SOC_SAND_IN  SOC_TMC_TDM_EG_ACTION enum_val
  );

void
  SOC_TMC_TDM_FTMH_OPT_UC_print(
    SOC_SAND_IN  SOC_TMC_TDM_FTMH_OPT_UC *info
  );

void
  SOC_TMC_TDM_FTMH_OPT_MC_print(
    SOC_SAND_IN  SOC_TMC_TDM_FTMH_OPT_MC *info
  );

void
  SOC_TMC_TDM_FTMH_STANDARD_UC_print(
    SOC_SAND_IN  SOC_TMC_TDM_FTMH_STANDARD_UC *info
  );

void
  SOC_TMC_TDM_FTMH_STANDARD_MC_print(
    SOC_SAND_IN  SOC_TMC_TDM_FTMH_STANDARD_MC *info
  );

void
  SOC_TMC_TDM_FTMH_print(
    SOC_SAND_IN  SOC_TMC_TDM_FTMH *info
  );

void
  SOC_TMC_TDM_FTMH_INFO_print(
    SOC_SAND_IN  SOC_TMC_TDM_FTMH_INFO *info
  );
void
  SOC_TMC_TDM_MC_STATIC_ROUTE_INFO_print(
    SOC_SAND_IN  SOC_TMC_TDM_MC_STATIC_ROUTE_INFO *info
  );

void
  SOC_TMC_TDM_DIRECT_ROUTING_INFO_print(
    SOC_SAND_IN  SOC_TMC_TDM_DIRECT_ROUTING_INFO *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
