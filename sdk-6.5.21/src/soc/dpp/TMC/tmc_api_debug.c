/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/






#include <shared/bsl.h>

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/TMC/tmc_api_debug.h>


























void
  SOC_TMC_DBG_AUTOCREDIT_INFO_clear(
    SOC_SAND_OUT SOC_TMC_DBG_AUTOCREDIT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_DBG_AUTOCREDIT_INFO));
  info->first_queue = 0;
  info->last_queue = 0;
  info->rate = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_TMC_DEBUG_IS_LVL1

const char*
  SOC_TMC_DBG_FORCE_MODE_to_string(
    SOC_SAND_IN SOC_TMC_DBG_FORCE_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_DBG_FORCE_MODE_NONE:
    str = "none";
  break;
  case SOC_TMC_DBG_FORCE_MODE_LOCAL:
    str = "local";
  break;
  case SOC_TMC_DBG_FORCE_MODE_FABRIC:
    str = "fabric";
  break;
  case SOC_TMC_DBG_NOF_FORCE_MODES:
    str = " Not initialized";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_DBG_FLUSH_MODE_to_string(
    SOC_SAND_IN  SOC_TMC_DBG_FLUSH_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_DBG_FLUSH_MODE_DEQUEUE:
    str = "dequeue";
  break;
  case SOC_TMC_DBG_FLUSH_MODE_DELETE:
    str = "delete";
  break;
  case SOC_TMC_DBG_NOF_FLUSH_MODES:
    str = " Not initialized";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_TMC_DBG_AUTOCREDIT_INFO_print(
    SOC_SAND_IN SOC_TMC_DBG_AUTOCREDIT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "first_queue: %u\n\r"),info->first_queue));
  LOG_CLI((BSL_META_U(unit,
                      "last_queue: %u\n\r"),info->last_queue));
  LOG_CLI((BSL_META_U(unit,
                      "rate: %u[Mbps]\n\r"),info->rate));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

