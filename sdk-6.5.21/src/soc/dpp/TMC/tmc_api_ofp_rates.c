/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/






#include <shared/bsl.h>

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/TMC/tmc_api_ofp_rates.h>
#include <soc/dpp/TMC/tmc_api_general.h>


























void
  SOC_TMC_OFP_RATES_MAL_SHPR_clear(
    SOC_SAND_OUT SOC_TMC_OFP_RATES_MAL_SHPR *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_OFP_RATES_MAL_SHPR));
  info->rate_update_mode = SOC_TMC_OFP_SHPR_UPDATE_MODE_SUM_OF_PORTS;
  info->rate = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_OFP_RATES_MAL_SHPR_INFO_clear(
    SOC_SAND_OUT SOC_TMC_OFP_RATES_MAL_SHPR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_OFP_RATES_MAL_SHPR_INFO));
  SOC_TMC_OFP_RATES_MAL_SHPR_clear(&(info->sch_shaper));
  SOC_TMC_OFP_RATES_MAL_SHPR_clear(&(info->egq_shaper));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_OFP_RATES_INTERFACE_SHPR_clear(
    SOC_SAND_OUT SOC_TMC_OFP_RATES_INTERFACE_SHPR *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_OFP_RATES_INTERFACE_SHPR));
  info->rate_update_mode = SOC_TMC_OFP_SHPR_UPDATE_MODE_SUM_OF_PORTS;
  info->rate = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_OFP_RATES_INTERFACE_SHPR_INFO_clear(
    SOC_SAND_OUT SOC_TMC_OFP_RATES_INTERFACE_SHPR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_OFP_RATES_INTERFACE_SHPR_INFO));
  SOC_TMC_OFP_RATES_INTERFACE_SHPR_clear(&(info->sch_shaper));
  SOC_TMC_OFP_RATES_INTERFACE_SHPR_clear(&(info->egq_shaper));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_OFP_RATE_INFO_clear(
    SOC_SAND_OUT SOC_TMC_OFP_RATE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_OFP_RATE_INFO));
  info->port_id = SOC_TMC_OFP_RATES_ILLEGAL_PORT_ID;
  info->sch_rate = 0;
  info->egq_rate = 0;
  info->max_burst = SOC_TMC_OFP_RATES_BURST_LIMIT_MAX;
  info->port_priority = 0;
  info->tcg_ndx = 0;
  info->sch_max_burst = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_OFP_RATES_TBL_INFO_clear(
    SOC_SAND_OUT SOC_TMC_OFP_RATES_TBL_INFO *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_OFP_RATES_TBL_INFO));
  info->nof_valid_entries = 0;
  for (ind=0; ind<SOC_TMC_NOF_FAP_PORTS; ++ind)
  {
    SOC_TMC_OFP_RATE_INFO_clear(&(info->rates[ind]));
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_OFP_FAT_PIPE_RATE_INFO_clear(
    SOC_SAND_OUT SOC_TMC_OFP_FAT_PIPE_RATE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_OFP_FAT_PIPE_RATE_INFO));
  info->sch_rate = 0;
  info->egq_rate = 0;
  info->max_burst = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_OFP_RATES_PORT_PRIORITY_SHPR_INFO_clear(
    SOC_SAND_OUT SOC_TMC_OFP_RATES_PORT_PRIORITY_SHPR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_OFP_RATES_PORT_PRIORITY_SHPR_INFO));
  SOC_TMC_OFP_RATE_SHPR_INFO_clear(&info->sch_shaper);
  info->sch_shaper.max_burst = SOC_TMC_OFP_RATES_SCH_BURST_LIMIT_MAX;
  SOC_TMC_OFP_RATE_SHPR_INFO_clear(&info->egq_shaper);
  info->egq_shaper.max_burst = SOC_TMC_OFP_RATES_BURST_LIMIT_MAX;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_OFP_RATE_SHPR_INFO_clear(
    SOC_SAND_OUT SOC_TMC_OFP_RATE_SHPR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_OFP_RATE_SHPR_INFO));
  info->rate = 0;  
  info->max_burst = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_OFP_RATES_TCG_SHPR_INFO_clear(
    SOC_SAND_OUT SOC_TMC_OFP_RATES_TCG_SHPR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_OFP_RATES_TCG_SHPR_INFO));
  SOC_TMC_OFP_RATE_SHPR_INFO_clear(&info->sch_shaper);
  info->sch_shaper.max_burst = SOC_TMC_OFP_RATES_SCH_BURST_LIMIT_MAX;
  SOC_TMC_OFP_RATE_SHPR_INFO_clear(&info->egq_shaper); 
  info->egq_shaper.max_burst = SOC_TMC_OFP_RATES_BURST_LIMIT_MAX;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_TMC_DEBUG_IS_LVL1

const char*
  SOC_TMC_OFP_RATES_CAL_SET_to_string(
    SOC_SAND_IN SOC_TMC_OFP_RATES_CAL_SET enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_OFP_RATES_CAL_SET_A:
    str = "a";
  break;
  case SOC_TMC_OFP_RATES_CAL_SET_B:
    str = "b";
  break;
  case SOC_TMC_OFP_NOF_RATES_CAL_SETS:
    str = " Not initialized";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_OFP_SHPR_UPDATE_MODE_to_string(
    SOC_SAND_IN SOC_TMC_OFP_SHPR_UPDATE_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_OFP_SHPR_UPDATE_MODE_SUM_OF_PORTS:
    str = "sum_of_ports";
  break;
  case SOC_TMC_OFP_SHPR_UPDATE_MODE_OVERRIDE:
    str = "override";
  break;
  case SOC_TMC_OFP_SHPR_UPDATE_MODE_DONT_TUCH:
    str = "dont_tuch";
  break;
  case SOC_TMC_OFP_NOF_SHPR_UPDATE_MODES:
    str = " Not initialized";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_TMC_OFP_RATES_MAL_SHPR_print(
    SOC_SAND_IN SOC_TMC_OFP_RATES_MAL_SHPR *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "  Rate_update_mode: %s \n\r"),
           SOC_TMC_OFP_SHPR_UPDATE_MODE_to_string(info->rate_update_mode)
           ));
  LOG_CLI((BSL_META_U(unit,
                      "  Rate: %u[Kbps]\n\r"),info->rate));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_OFP_RATES_MAL_SHPR_INFO_print(
    SOC_SAND_IN SOC_TMC_OFP_RATES_MAL_SHPR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Sch_shaper:\n\r")));
  SOC_TMC_OFP_RATES_MAL_SHPR_print(&(info->sch_shaper));
  LOG_CLI((BSL_META_U(unit,
                      "Egq_shaper:\n\r")));
  SOC_TMC_OFP_RATES_MAL_SHPR_print(&(info->egq_shaper));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_OFP_RATES_INTERFACE_SHPR_print(
    SOC_SAND_IN SOC_TMC_OFP_RATES_INTERFACE_SHPR *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "  Rate_update_mode: %s \n\r"),
           SOC_TMC_OFP_SHPR_UPDATE_MODE_to_string(info->rate_update_mode)
           ));
  LOG_CLI((BSL_META_U(unit,
                      "  Rate: %u[Kbps]\n\r"),info->rate));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_OFP_RATES_INTERFACE_SHPR_INFO_print(
    SOC_SAND_IN SOC_TMC_OFP_RATES_INTERFACE_SHPR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Sch_shaper:\n\r")));
  SOC_TMC_OFP_RATES_INTERFACE_SHPR_print(&(info->sch_shaper));
  LOG_CLI((BSL_META_U(unit,
                      "Egq_shaper:\n\r")));
  SOC_TMC_OFP_RATES_INTERFACE_SHPR_print(&(info->egq_shaper));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_OFP_RATE_INFO_print(
    SOC_SAND_IN SOC_TMC_OFP_RATE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Port[%-2u] rate: "),info->port_id));
  LOG_CLI((BSL_META_U(unit,
                      "SCH: %-8u[Kbps], "),info->sch_rate));
  LOG_CLI((BSL_META_U(unit,
                      "EGQ: %-8u[Kbps], "),info->egq_rate));

  if (info->max_burst == SOC_TMC_OFP_RATES_BURST_LIMIT_MAX)
  {
    LOG_CLI((BSL_META_U(unit,
                        "Max Burst: No Limit.\n\r")));
  }
  else
  {
    LOG_CLI((BSL_META_U(unit,
                        "Max Burst: %-6u[Byte].\n\r"),info->max_burst));
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_OFP_RATES_TBL_INFO_print(
    SOC_SAND_IN SOC_TMC_OFP_RATES_TBL_INFO *info
  )
{
  uint32
    ind=0,
    nof_zero_rate=0;
  uint8
    zero_rate;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Nof_valid_entries: %u[Entries]\n\r"),info->nof_valid_entries));
  LOG_CLI((BSL_META_U(unit,
                      "Rates:\n\r")));
  for (ind=0; ind<info->nof_valid_entries; ++ind)
  {
    zero_rate = SOC_SAND_NUM2BOOL((info->rates[ind].egq_rate == 0) && (info->rates[ind].sch_rate == 0));
    if(!zero_rate)
    {
      SOC_TMC_OFP_RATE_INFO_print(&(info->rates[ind]));
    }
    else
    {
      nof_zero_rate++;
    }
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_OFP_FAT_PIPE_RATE_INFO_print(
    SOC_SAND_IN SOC_TMC_OFP_FAT_PIPE_RATE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Sch_rate:  %u[Kbps]\n\r"),info->sch_rate));
  LOG_CLI((BSL_META_U(unit,
                      "Egq_rate:  %u[Kbps]\n\r"),info->egq_rate));
  LOG_CLI((BSL_META_U(unit,
                      "Max_burst: %u[Bytes]\n\r"),info->max_burst));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_OFP_RATES_PORT_PRIORITY_SHPR_INFO_print(
    SOC_SAND_OUT SOC_TMC_OFP_RATES_PORT_PRIORITY_SHPR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "SCH: ")));
  SOC_TMC_OFP_RATE_SHPR_INFO_print(&info->sch_shaper);
  LOG_CLI((BSL_META_U(unit,
                      "EGQ: ")));
  SOC_TMC_OFP_RATE_SHPR_INFO_print(&info->egq_shaper);  
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_OFP_RATE_SHPR_INFO_print(
    SOC_SAND_OUT SOC_TMC_OFP_RATE_SHPR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "rate:  %u[Kbps]\n\r"),info->rate));  
  LOG_CLI((BSL_META_U(unit,
                      "Max_burst: %u[Bytes]\n\r"),info->max_burst));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_OFP_RATES_TCG_SHPR_INFO_print(
    SOC_SAND_OUT SOC_TMC_OFP_RATES_TCG_SHPR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "SCH: ")));
  SOC_TMC_OFP_RATE_SHPR_INFO_print(&info->sch_shaper);
  LOG_CLI((BSL_META_U(unit,
                      "EGQ: ")));
  SOC_TMC_OFP_RATE_SHPR_INFO_print(&info->egq_shaper);  
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

