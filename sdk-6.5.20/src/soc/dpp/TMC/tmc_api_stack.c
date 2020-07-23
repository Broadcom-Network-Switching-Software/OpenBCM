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

#include <soc/dpp/TMC/tmc_api_stack.h>





















void
  SOC_TMC_STACK_GLBL_INFO_clear(
    SOC_SAND_OUT SOC_TMC_STACK_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_STACK_GLBL_INFO));
  info->max_nof_tm_domains = SOC_TMC_STACK_MAX_NOF_TM_DOMAINS_1;
  info->my_tm_domain = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_STACK_PORT_DISTR_INFO_clear(
    SOC_SAND_OUT SOC_TMC_STACK_PORT_DISTR_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_STACK_PORT_DISTR_INFO));
  info->peer_tm_domain = 0;
  for (ind = 0; ind < SOC_TMC_STACK_PRUN_BMP_LEN; ++ind)
  {
    info->prun_bmp[ind] = 0;
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_TMC_DEBUG_IS_LVL1

const char*
  SOC_TMC_STACK_MAX_NOF_TM_DOMAINS_to_string(
    SOC_SAND_IN  SOC_TMC_STACK_MAX_NOF_TM_DOMAINS enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_STACK_MAX_NOF_TM_DOMAINS_1:
    str = "1";
  break;
  case SOC_TMC_STACK_MAX_NOF_TM_DOMAINS_8:
    str = "8";
  break;
  case SOC_TMC_STACK_MAX_NOF_TM_DOMAINS_16:
    str = "16";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_TMC_STACK_GLBL_INFO_print(
    SOC_SAND_IN  SOC_TMC_STACK_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "max_nof_tm_domains %s "), SOC_TMC_STACK_MAX_NOF_TM_DOMAINS_to_string(info->max_nof_tm_domains)));
  LOG_CLI((BSL_META_U(unit,
                      "my_tm_domain: %u\n\r"),info->my_tm_domain));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_STACK_PORT_DISTR_INFO_print(
    SOC_SAND_IN  SOC_TMC_STACK_PORT_DISTR_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "peer_tm_domain: %u\n\r"),info->peer_tm_domain));
  for (ind = 0; ind < SOC_TMC_STACK_PRUN_BMP_LEN; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "prun_bmp[%u]: %u\n\r"),ind,info->prun_bmp[ind]));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

