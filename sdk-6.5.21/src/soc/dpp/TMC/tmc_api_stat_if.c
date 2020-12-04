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

#include <soc/dpp/TMC/tmc_api_stat_if.h>





















void
  SOC_TMC_STAT_IF_INFO_clear(
    SOC_SAND_OUT SOC_TMC_STAT_IF_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_STAT_IF_INFO));
  info->enable = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_STAT_IF_BILLING_INFO_clear(
    SOC_SAND_OUT SOC_TMC_STAT_IF_BILLING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_STAT_IF_BILLING_INFO));
  info->mode = SOC_TMC_STAT_NOF_IF_BILLING_MODES;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_STAT_IF_FAP20V_INFO_clear(
    SOC_SAND_OUT SOC_TMC_STAT_IF_FAP20V_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_STAT_IF_FAP20V_INFO));
  info->mode = SOC_TMC_STAT_NOF_IF_FAP20V_MODES;
  info->fabric_mc = SOC_TMC_STAT_NOF_IF_MC_MODES;
  info->ing_mc = SOC_TMC_STAT_NOF_IF_MC_MODES;
  info->is_original_pkt_size = 0;
  info->ing_mc_is_report_single = 0;
  info->count_snoop = 0;
  info->cnm_report = SOC_TMC_STAT_NOF_IF_CNM_MODES;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_STAT_IF_REPORT_MODE_INFO_clear(
    SOC_SAND_OUT SOC_TMC_STAT_IF_REPORT_MODE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_STAT_IF_REPORT_MODE_INFO));
  SOC_TMC_STAT_IF_BILLING_INFO_clear(&(info->billing));
  SOC_TMC_STAT_IF_FAP20V_INFO_clear(&(info->fap20v));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_STAT_IF_REPORT_INFO_clear(
    SOC_SAND_OUT SOC_TMC_STAT_IF_REPORT_INFO *info
  )
{
  int i;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_STAT_IF_REPORT_INFO));
  info->mode = SOC_TMC_STAT_NOF_IF_REPORT_MODES;
  SOC_TMC_STAT_IF_REPORT_MODE_INFO_clear(&(info->format));
  info->sync_rate = 0;
  info->parity_enable = 0;
  info->is_idle_reports_present = 0;
  info->if_report_original_pkt_size = 0;
  info->if_pkt_size = 0;
  info->if_scrubber_queue_min = 0;
  info->if_scrubber_queue_max = 0;
  info->if_scrubber_rate_min  = 0;
  info->if_scrubber_rate_max  = 0;
  info->if_report_mc_once = 0;
  for(i = 0; i < SOC_TMC_STAT_NOF_THRESHOLD_RANGES; i++ ) {
      info->if_scrubber_buffer_descr_th[i] = 0;
      info->if_scrubber_bdb_th[i] = 0;
      info->if_scrubber_uc_dram_buffer_th[i] = 0;
  }

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_STAT_IF_BIST_PATTERN_clear(
    SOC_SAND_OUT SOC_TMC_STAT_IF_BIST_PATTERN *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_STAT_IF_BIST_PATTERN));
  info->word1 = 0;
  info->word2 = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_STAT_IF_BIST_INFO_clear(
    SOC_SAND_OUT SOC_TMC_STAT_IF_BIST_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_STAT_IF_BIST_INFO));
  info->en_mode = SOC_TMC_STAT_NOF_IF_BIST_EN_MODES;
  SOC_TMC_STAT_IF_BIST_PATTERN_clear(&(info->pattern));
  info->nof_duplications = 0;
  info->nof_idle_clocks = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_TMC_DEBUG_IS_LVL1

const char*
  SOC_TMC_STAT_IF_PHASE_to_string(
    SOC_SAND_IN  SOC_TMC_STAT_IF_PHASE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_STAT_IF_PHASE_000:
    str = "phase_000";
  break;
  case SOC_TMC_STAT_IF_PHASE_090:
    str = "phase_090";
  break;
  case SOC_TMC_STAT_IF_PHASE_180:
    str = "phase_180";
  break;
  case SOC_TMC_STAT_IF_PHASE_270:
    str = "phase_270";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_STAT_IF_REPORT_MODE_to_string(
    SOC_SAND_IN  SOC_TMC_STAT_IF_REPORT_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_STAT_IF_REPORT_MODE_BILLING:
    str = "billing";
  break;
  case SOC_TMC_STAT_IF_REPORT_MODE_FAP20V:
    str = "fap20v";
  break;
  case SOC_TMC_STAT_NOF_IF_REPORT_MODES:
    str = " Not initialized";
  break;
  case SOC_TMC_STAT_IF_REPORT_MODE_BILLING_QUEUE_NUMBER:
      str = "billing_queue_number";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_STAT_IF_BILLING_MODE_to_string(
    SOC_SAND_IN  SOC_TMC_STAT_IF_BILLING_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_STAT_IF_BILLING_MODE_EGR_Q_NUM:
    str = "egr_q_num";
  break;
  case SOC_TMC_STAT_IF_BILLING_MODE_CUD:
    str = "cud";
  break;
  case SOC_TMC_STAT_IF_BILLING_MODE_VSI_VLAN:
    str = "vsi_vlan";
  break;
  case SOC_TMC_STAT_IF_BILLING_MODE_BOTH_LIFS:
    str = "both_lifs";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_STAT_IF_FAP20V_MODE_to_string(
    SOC_SAND_IN  SOC_TMC_STAT_IF_FAP20V_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_STAT_IF_FAP20V_MODE_Q_SIZE:
    str = "q_size";
  break;
  case SOC_TMC_STAT_IF_FAP20V_MODE_PKT_SIZE:
    str = "pkt_size";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_STAT_IF_MC_MODE_to_string(
    SOC_SAND_IN  SOC_TMC_STAT_IF_MC_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_STAT_IF_MC_MODE_Q_NUM:
    str = "q_num";
  break;
  case SOC_TMC_STAT_IF_MC_MODE_MC_ID:
    str = "mc_id";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_STAT_IF_CNM_MODE_to_string(
    SOC_SAND_IN  SOC_TMC_STAT_IF_CNM_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_STAT_IF_CNM_MODE_DIS:
    str = "dis";
  break;
  case SOC_TMC_STAT_IF_CNM_MODE_EN:
    str = "en";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_STAT_IF_PARITY_MODE_to_string(
    SOC_SAND_IN  SOC_TMC_STAT_IF_PARITY_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_STAT_IF_PARITY_MODE_DIS:
    str = "dis";
  break;
  case SOC_TMC_STAT_IF_PARITY_MODE_EN:
    str = "en";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_STAT_IF_BIST_EN_MODE_to_string(
    SOC_SAND_IN  SOC_TMC_STAT_IF_BIST_EN_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_STAT_IF_BIST_EN_MODE_DIS:
    str = "dis";
  break;
  case SOC_TMC_STAT_IF_BIST_EN_MODE_PATTERN:
    str = "pattern";
  break;
  case SOC_TMC_STAT_IF_BIST_EN_MODE_WALKING_ONE:
    str = "walking_one";
  break;
  case SOC_TMC_STAT_IF_BIST_EN_MODE_PRBS:
    str = "prbs";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_TMC_STAT_IF_INFO_print(
    SOC_SAND_IN  SOC_TMC_STAT_IF_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "enable: %u\n\r"),info->enable));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_STAT_IF_BILLING_INFO_print(
    SOC_SAND_IN  SOC_TMC_STAT_IF_BILLING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "mode %s "), SOC_TMC_STAT_IF_BILLING_MODE_to_string(info->mode)));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_STAT_IF_FAP20V_INFO_print(
    SOC_SAND_IN  SOC_TMC_STAT_IF_FAP20V_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "mode %s "), SOC_TMC_STAT_IF_FAP20V_MODE_to_string(info->mode)));
  LOG_CLI((BSL_META_U(unit,
                      "fabric_mc %s "), SOC_TMC_STAT_IF_MC_MODE_to_string(info->fabric_mc)));
  LOG_CLI((BSL_META_U(unit,
                      "ing_mc %s "), SOC_TMC_STAT_IF_MC_MODE_to_string(info->ing_mc)));
  LOG_CLI((BSL_META_U(unit,
                      "is_original_pkt_size: %u\n\r"),info->is_original_pkt_size));
  LOG_CLI((BSL_META_U(unit,
                      "ing_mc_is_report_single: %u\n\r"),info->ing_mc_is_report_single));
  LOG_CLI((BSL_META_U(unit,
                      "count_snoop: %u\n\r"),info->count_snoop));
  LOG_CLI((BSL_META_U(unit,
                      "cnm_report %s "), SOC_TMC_STAT_IF_CNM_MODE_to_string(info->cnm_report)));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_STAT_IF_REPORT_MODE_INFO_print(
    SOC_SAND_IN  SOC_TMC_STAT_IF_REPORT_MODE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "billing:")));
  SOC_TMC_STAT_IF_BILLING_INFO_print(&(info->billing));
  LOG_CLI((BSL_META_U(unit,
                      "fap20v:")));
  SOC_TMC_STAT_IF_FAP20V_INFO_print(&(info->fap20v));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_STAT_IF_REPORT_INFO_print(
    SOC_SAND_IN  SOC_TMC_STAT_IF_REPORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "mode %s "), SOC_TMC_STAT_IF_REPORT_MODE_to_string(info->mode)));
  LOG_CLI((BSL_META_U(unit,
                      "format:")));
  SOC_TMC_STAT_IF_REPORT_MODE_INFO_print(&(info->format));
  LOG_CLI((BSL_META_U(unit,
                      "sync_rate: %u[Nanoseconds]\n\r"),info->sync_rate));
  LOG_CLI((BSL_META_U(unit,
                      "parity_enable: %u\n\r"),info->parity_enable));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_STAT_IF_BIST_PATTERN_print(
    SOC_SAND_IN  SOC_TMC_STAT_IF_BIST_PATTERN *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "word1: %u\n\r"),info->word1));
  LOG_CLI((BSL_META_U(unit,
                      "word2: %u\n\r"),info->word2));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_STAT_IF_BIST_INFO_print(
    SOC_SAND_IN  SOC_TMC_STAT_IF_BIST_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "en_mode %s "), SOC_TMC_STAT_IF_BIST_EN_MODE_to_string(info->en_mode)));
  LOG_CLI((BSL_META_U(unit,
                      "pattern:")));
  SOC_TMC_STAT_IF_BIST_PATTERN_print(&(info->pattern));
  LOG_CLI((BSL_META_U(unit,
                      "nof_duplications: %u\n\r"),info->nof_duplications));
  LOG_CLI((BSL_META_U(unit,
                      "nof_idle_clocks: %u\n\r"),info->nof_idle_clocks));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

