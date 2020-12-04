#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_STAT


#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/arad_api_framework.h>
#include <soc/dpp/ARAD/arad_framework.h>
#include <soc/dpp/ARAD/arad_api_stat_if.h>
#include <soc/dpp/ARAD/arad_stat_if.h>

#include <soc/dpp/ARAD/arad_general.h>



























uint32
  arad_stat_if_report_info_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_STAT_IF_REPORT_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_STAT_IF_REPORT_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_CHECK_NULL_INPUT(info);

  res = arad_stat_if_report_info_verify(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_stat_if_report_info_set_unsafe(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_stat_if_report_info_set()",0,0);
}


uint32
  arad_stat_if_report_info_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_STAT_IF_REPORT_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_STAT_IF_REPORT_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_CHECK_NULL_INPUT(info);
 
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_stat_if_report_info_get_unsafe(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_stat_if_report_info_get()",0,0);
}

void
  arad_ARAD_STAT_IF_INFO_clear(
    SOC_SAND_OUT ARAD_STAT_IF_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_STAT_IF_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_STAT_IF_REPORT_MODE_BILLING_INFO_clear(
    SOC_SAND_OUT ARAD_STAT_IF_REPORT_MODE_BILLING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_STAT_IF_BILLING_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_STAT_IF_REPORT_MODE_FAP20V_INFO_clear(
    SOC_SAND_OUT ARAD_STAT_IF_REPORT_MODE_FAP20V_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_STAT_IF_FAP20V_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_STAT_IF_REPORT_MODE_INFO_clear(
    SOC_SAND_OUT ARAD_STAT_IF_REPORT_MODE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_STAT_IF_REPORT_MODE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_STAT_IF_REPORT_INFO_clear(
    SOC_SAND_OUT ARAD_STAT_IF_REPORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_STAT_IF_REPORT_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_STAT_IF_BIST_PATTERN_clear(
    SOC_SAND_OUT ARAD_STAT_IF_BIST_PATTERN *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_STAT_IF_BIST_PATTERN_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_STAT_IF_BIST_INFO_clear(
    SOC_SAND_OUT ARAD_STAT_IF_BIST_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_STAT_IF_BIST_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if ARAD_DEBUG_IS_LVL1

const char*
  arad_ARAD_STAT_IF_PHASE_to_string(
    SOC_SAND_IN  ARAD_STAT_IF_PHASE enum_val
  )
{
  return SOC_TMC_STAT_IF_PHASE_to_string(enum_val);
}

const char*
  arad_ARAD_STAT_IF_REPORT_MODE_to_string(
    SOC_SAND_IN  ARAD_STAT_IF_REPORT_MODE enum_val
  )
{
  return SOC_TMC_STAT_IF_REPORT_MODE_to_string(enum_val);
}

const char*
  arad_ARAD_STAT_IF_BILLING_MODE_to_string(
    SOC_SAND_IN  ARAD_STAT_IF_BILLING_MODE enum_val
  )
{
  return SOC_TMC_STAT_IF_BILLING_MODE_to_string(enum_val);
}

const char*
  arad_ARAD_STAT_IF_FAP20V_MODE_to_string(
    SOC_SAND_IN  ARAD_STAT_IF_FAP20V_MODE enum_val
  )
{
  return SOC_TMC_STAT_IF_FAP20V_MODE_to_string(enum_val);
}

const char*
  arad_ARAD_STAT_IF_MC_MODE_to_string(
    SOC_SAND_IN  ARAD_STAT_IF_MC_MODE enum_val
  )
{
  return SOC_TMC_STAT_IF_MC_MODE_to_string(enum_val);
}

const char*
  arad_ARAD_STAT_IF_CNM_MODE_to_string(
    SOC_SAND_IN  ARAD_STAT_IF_CNM_MODE enum_val
  )
{
  return SOC_TMC_STAT_IF_CNM_MODE_to_string(enum_val);
}

const char*
  arad_ARAD_STAT_IF_PARITY_MODE_to_string(
    SOC_SAND_IN  ARAD_STAT_IF_PARITY_MODE enum_val
  )
{
  return SOC_TMC_STAT_IF_PARITY_MODE_to_string(enum_val);
}

const char*
  arad_ARAD_STAT_IF_BIST_EN_MODE_to_string(
    SOC_SAND_IN  ARAD_STAT_IF_BIST_EN_MODE enum_val
  )
{
  return SOC_TMC_STAT_IF_BIST_EN_MODE_to_string(enum_val);
}

void
  arad_ARAD_STAT_IF_INFO_print(
    SOC_SAND_IN  ARAD_STAT_IF_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_STAT_IF_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_STAT_IF_REPORT_MODE_BILLING_INFO_print(
    SOC_SAND_IN ARAD_STAT_IF_REPORT_MODE_BILLING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_STAT_IF_BILLING_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_STAT_IF_REPORT_MODE_FAP20V_INFO_print(
    SOC_SAND_IN ARAD_STAT_IF_REPORT_MODE_FAP20V_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_STAT_IF_FAP20V_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_STAT_IF_REPORT_MODE_INFO_print(
    SOC_SAND_IN  ARAD_STAT_IF_REPORT_MODE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_STAT_IF_REPORT_MODE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_STAT_IF_REPORT_INFO_print(
    SOC_SAND_IN  ARAD_STAT_IF_REPORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_STAT_IF_REPORT_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_STAT_IF_BIST_PATTERN_print(
    SOC_SAND_IN  ARAD_STAT_IF_BIST_PATTERN *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_STAT_IF_BIST_PATTERN_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_STAT_IF_BIST_INFO_print(
    SOC_SAND_IN  ARAD_STAT_IF_BIST_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_STAT_IF_BIST_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 




#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 
