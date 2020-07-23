/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/







#include <shared/bsl.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/TMC/tmc_api_tdm.h>





















void
  SOC_TMC_TDM_FTMH_OPT_UC_clear(
    SOC_SAND_OUT SOC_TMC_TDM_FTMH_OPT_UC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_TDM_FTMH_OPT_UC));
  info->dest_if = 0;
  info->dest_fap_id = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_TDM_FTMH_OPT_MC_clear(
    SOC_SAND_OUT SOC_TMC_TDM_FTMH_OPT_MC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_TDM_FTMH_OPT_MC));
  info->mc_id = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_TDM_FTMH_STANDARD_UC_clear(
    SOC_SAND_OUT SOC_TMC_TDM_FTMH_STANDARD_UC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_TDM_FTMH_STANDARD_UC));
  info->user_def = 0;
  info->sys_phy_port = 0;
  info->user_def_2 = 0;
  info->dest_fap_port = 0;
  info->dest_fap_id = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_TDM_FTMH_STANDARD_MC_clear(
    SOC_SAND_OUT SOC_TMC_TDM_FTMH_STANDARD_MC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_TDM_FTMH_STANDARD_MC));
  info->user_def = 0;
  info->mc_id = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_TDM_FTMH_clear(
    SOC_SAND_OUT SOC_TMC_TDM_FTMH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_TDM_FTMH));
  SOC_TMC_TDM_FTMH_OPT_UC_clear(&(info->opt_uc));
  SOC_TMC_TDM_FTMH_OPT_MC_clear(&(info->opt_mc));
  SOC_TMC_TDM_FTMH_STANDARD_UC_clear(&(info->standard_uc));
  SOC_TMC_TDM_FTMH_STANDARD_MC_clear(&(info->standard_mc));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_TDM_FTMH_INFO_clear(
    SOC_SAND_OUT SOC_TMC_TDM_FTMH_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_TDM_FTMH_INFO));
  info->action_ing = SOC_TMC_TDM_NOF_ING_ACTIONS;
  SOC_TMC_TDM_FTMH_clear(&(info->ftmh));
  info->action_eg = SOC_TMC_TDM_NOF_EG_ACTIONS;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_TDM_MC_STATIC_ROUTE_INFO_clear(
    SOC_SAND_OUT SOC_TMC_TDM_MC_STATIC_ROUTE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_TDM_MC_STATIC_ROUTE_INFO));
  soc_sand_u64_clear(&(info->link_bitmap));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_TDM_DIRECT_ROUTING_INFO_clear(
    SOC_SAND_OUT SOC_TMC_TDM_DIRECT_ROUTING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_TDM_DIRECT_ROUTING_INFO));
  soc_sand_u64_clear(&(info->link_bitmap));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_TMC_DEBUG_IS_LVL1

const char*
  SOC_TMC_TDM_ING_ACTION_to_string(
    SOC_SAND_IN  SOC_TMC_TDM_ING_ACTION enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_TDM_ING_ACTION_ADD:
    str = "add";
  break;
  case SOC_TMC_TDM_ING_ACTION_NO_CHANGE:
    str = "no_change";
  break;
  case SOC_TMC_TDM_ING_ACTION_CUSTOMER_EMBED:
    str = "customer_embed";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_TDM_EG_ACTION_to_string(
    SOC_SAND_IN  SOC_TMC_TDM_EG_ACTION enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_TDM_EG_ACTION_REMOVE:
    str = "remove";
  break;
  case SOC_TMC_TDM_EG_ACTION_NO_CHANGE:
    str = "no_change";
  break;
  case SOC_TMC_TDM_EG_ACTION_CUSTOMER_EXTRACT:
    str = "customer_extract";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_TMC_TDM_FTMH_OPT_UC_print(
    SOC_SAND_IN  SOC_TMC_TDM_FTMH_OPT_UC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "dest_if: %u\n\r"),info->dest_if));
  LOG_CLI((BSL_META_U(unit,
                      "dest_fap_id: %u\n\r"),info->dest_fap_id));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_TDM_FTMH_OPT_MC_print(
    SOC_SAND_IN  SOC_TMC_TDM_FTMH_OPT_MC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "mc_id: %u\n\r"),info->mc_id));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_TDM_FTMH_STANDARD_UC_print(
    SOC_SAND_IN  SOC_TMC_TDM_FTMH_STANDARD_UC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "user_def: %u\n\r"),info->user_def));
  LOG_CLI((BSL_META_U(unit,
                      "sys_phy_port: %u\n\r"),info->sys_phy_port));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_TDM_FTMH_STANDARD_MC_print(
    SOC_SAND_IN  SOC_TMC_TDM_FTMH_STANDARD_MC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "user_def: %u\n\r"),info->user_def));
  LOG_CLI((BSL_META_U(unit,
                      "mc_id: %u\n\r"),info->mc_id));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_TDM_FTMH_print(
    SOC_SAND_IN  SOC_TMC_TDM_FTMH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "\n\ropt_uc:\n\r")));
  SOC_TMC_TDM_FTMH_OPT_UC_print(&(info->opt_uc));
  LOG_CLI((BSL_META_U(unit,
                      "opt_mc:\n\r")));
  SOC_TMC_TDM_FTMH_OPT_MC_print(&(info->opt_mc));
  LOG_CLI((BSL_META_U(unit,
                      "standard_uc:\n\r")));
  SOC_TMC_TDM_FTMH_STANDARD_UC_print(&(info->standard_uc));
  LOG_CLI((BSL_META_U(unit,
                      "standard_mc:\n\r")));
  SOC_TMC_TDM_FTMH_STANDARD_MC_print(&(info->standard_mc));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_TDM_FTMH_INFO_print(
    SOC_SAND_IN  SOC_TMC_TDM_FTMH_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "action_ing %s "), SOC_TMC_TDM_ING_ACTION_to_string(info->action_ing)));
  LOG_CLI((BSL_META_U(unit,
                      "ftmh:")));
  SOC_TMC_TDM_FTMH_print(&(info->ftmh));
  LOG_CLI((BSL_META_U(unit,
                      "is_mc: %u\n\r"),info->is_mc));
  LOG_CLI((BSL_META_U(unit,
                      "action_eg %s "), SOC_TMC_TDM_EG_ACTION_to_string(info->action_eg)));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void
  SOC_TMC_TDM_MC_STATIC_ROUTE_INFO_print(
    SOC_SAND_IN  SOC_TMC_TDM_MC_STATIC_ROUTE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "link_bitmap:")));
  soc_sand_u64_print(&(info->link_bitmap), 1, 0);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

