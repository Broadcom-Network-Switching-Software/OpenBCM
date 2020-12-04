/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_PPC

#include <shared/bsl.h>




#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/PPC/ppc_api_general.h>
#include <soc/dpp/PPC/ppc_api_ptp.h>





















void
  SOC_PPC_PTP_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PPC_PTP_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_PTP_PORT_INFO));
  info->ptp_enabled         = 0;
  info->flags               = 0;
  info->pkt_drop            = 0;
  info->pkt_tocpu           = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_PTP_IN_PP_PORT_PROFILE_to_string(
    SOC_SAND_IN  SOC_PPC_PTP_IN_PP_PORT_PROFILE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_PTP_IN_PP_PORT_PROFILE_0:
    str = "profile 0";
  break;
  case SOC_PPC_PTP_IN_PP_PORT_PROFILE_1:
    str = "profile 1";
  break;
  case SOC_PPC_PTP_IN_PP_PORT_PROFILE_2:
    str = "profile 2";
  break;
  case SOC_PPC_PTP_IN_PP_PORT_PROFILE_3:
    str = "profile 3";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_PPC_PTP_PORT_INFO_print(
    SOC_SAND_IN  SOC_PPC_PTP_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "ptp enabled: %u\n\r"), info->ptp_enabled));
  LOG_CLI((BSL_META_U(unit,
                      "flags:     0x%x\n\r"), info->flags));
  LOG_CLI((BSL_META_U(unit,
                      "pkt_drop:  0x%x\n\r"), info->pkt_drop));
  LOG_CLI((BSL_META_U(unit,
                      "pkt_tocpu: 0x%x\n\r"), info->pkt_tocpu));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 


#include <soc/dpp/SAND/Utils/sand_footer.h>

