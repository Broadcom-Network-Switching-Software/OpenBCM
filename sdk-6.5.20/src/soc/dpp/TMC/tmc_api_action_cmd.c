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

#include <soc/dpp/TMC/tmc_api_action_cmd.h>





















void
  SOC_TMC_ACTION_CMD_OVERRIDE_clear(
    SOC_SAND_OUT SOC_TMC_ACTION_CMD_OVERRIDE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_ACTION_CMD_OVERRIDE));
  info->value = 0;
  info->enable = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ACTION_CMD_clear(
    SOC_SAND_OUT SOC_TMC_ACTION_CMD *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_ACTION_CMD));
  SOC_TMC_ACTION_CMD_OVERRIDE_clear(&(info->tc));
  SOC_TMC_ACTION_CMD_OVERRIDE_clear(&(info->dp));
  SOC_TMC_ACTION_CMD_OVERRIDE_clear(&(info->meter_ptr_low));
  SOC_TMC_ACTION_CMD_OVERRIDE_clear(&(info->meter_ptr_up));
  SOC_TMC_ACTION_CMD_OVERRIDE_clear(&(info->meter_dp));
  SOC_TMC_ACTION_CMD_OVERRIDE_clear(&(info->counter_ptr_1));
  SOC_TMC_ACTION_CMD_OVERRIDE_clear(&(info->counter_ptr_2));
  SOC_TMC_ACTION_CMD_OVERRIDE_clear(&(info->outlif));
  info->is_ing_mc = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void
    SOC_TMC_ACTION_CMD_SNOOP_MIRROR_STAMPING_INFO_clear( 
       SOC_TMC_ACTION_CMD_SNOOP_MIRROR_STAMPING_INFO *info
       ) 
{
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);
    sal_memset(info, 0x0, sizeof(SOC_TMC_ACTION_CMD_SNOOP_MIRROR_STAMPING_INFO));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void
  SOC_TMC_ACTION_CMD_SNOOP_INFO_clear(
    SOC_SAND_OUT SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO));
  SOC_TMC_ACTION_CMD_clear(&(info->cmd));
  info->size = SOC_TMC_ACTION_NOF_CMD_SIZE_BYTESS;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ACTION_CMD_MIRROR_INFO_clear(
    SOC_SAND_OUT SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO));
  SOC_TMC_ACTION_CMD_clear(&(info->cmd));
  SOC_TMC_ACTION_CMD_SNOOP_MIRROR_STAMPING_INFO_clear(&info->stamping);
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_TMC_DEBUG_IS_LVL1

const char*
  SOC_TMC_ACTION_CMD_SIZE_BYTES_to_string(
    SOC_SAND_IN  SOC_TMC_ACTION_CMD_SIZE_BYTES enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_ACTION_CMD_SIZE_BYTES_64:
    str = "bytes_64";
  break;
  case SOC_TMC_ACTION_CMD_SIZE_BYTES_128:
    str = "bytes_128";
  break;
  case SOC_TMC_ACTION_CMD_SIZE_BYTES_192:
    str = "bytes_192";
  break;
  case SOC_TMC_ACTION_CMD_SIZE_BYTES_256:
    str = "bytes_256";
  break;
  case SOC_TMC_ACTION_CMD_SIZE_BYTES_ALL_PCKT:
    str = "all_pckt";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_TMC_ACTION_CMD_OVERRIDE_print(
    SOC_SAND_IN  SOC_TMC_ACTION_CMD_OVERRIDE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "value: %u\n\r"),info->value));
  LOG_CLI((BSL_META_U(unit,
                      "enable: %u\n\r"),info->enable));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ACTION_CMD_print(
    SOC_SAND_IN  SOC_TMC_ACTION_CMD *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "destination:\n\r")));
  SOC_TMC_DEST_INFO_print(&(info->dest_id));
  LOG_CLI((BSL_META_U(unit,
                      "tc:\n\r")));
  SOC_TMC_ACTION_CMD_OVERRIDE_print(&(info->tc));
  LOG_CLI((BSL_META_U(unit,
                      "dp:\n\r")));
  SOC_TMC_ACTION_CMD_OVERRIDE_print(&(info->dp));
  LOG_CLI((BSL_META_U(unit,
                      "meter_ptr_low:\n\r")));
  SOC_TMC_ACTION_CMD_OVERRIDE_print(&(info->meter_ptr_low));
  LOG_CLI((BSL_META_U(unit,
                      "meter_ptr_up:\n\r")));
  SOC_TMC_ACTION_CMD_OVERRIDE_print(&(info->meter_ptr_up));
  LOG_CLI((BSL_META_U(unit,
                      "meter_dp:\n\r")));
  SOC_TMC_ACTION_CMD_OVERRIDE_print(&(info->meter_dp));
  LOG_CLI((BSL_META_U(unit,
                      "counter_ptr_1:\n\r")));
  SOC_TMC_ACTION_CMD_OVERRIDE_print(&(info->counter_ptr_1));
  LOG_CLI((BSL_META_U(unit,
                      "counter_ptr_2:\n\r")));
  SOC_TMC_ACTION_CMD_OVERRIDE_print(&(info->counter_ptr_2));
  LOG_CLI((BSL_META_U(unit,
                      "is_ing_mc: %u\n\r"),info->is_ing_mc));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ACTION_CMD_SNOOP_INFO_print(
    SOC_SAND_IN  SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "cmd:")));
  SOC_TMC_ACTION_CMD_print(&(info->cmd));
  LOG_CLI((BSL_META_U(unit,
                      "snoop_size %s "), SOC_TMC_ACTION_CMD_SIZE_BYTES_to_string(info->size)));
  LOG_CLI((BSL_META_U(unit,
                      "snoop_prob %u "), info->prob));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_ACTION_CMD_MIRROR_INFO_print(
    SOC_SAND_IN  SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "cmd:")));
  SOC_TMC_ACTION_CMD_print(&(info->cmd));
  LOG_CLI((BSL_META_U(unit,
                      "mirror_prob %u "), info->prob));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

