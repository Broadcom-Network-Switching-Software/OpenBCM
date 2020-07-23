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

#include <soc/dpp/PPC/ppc_api_llp_sa_auth.h>





















void
  SOC_PPC_LLP_SA_AUTH_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_SA_AUTH_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LLP_SA_AUTH_PORT_INFO));
  info->sa_auth_enable = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_SA_AUTH_MAC_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_SA_AUTH_MAC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LLP_SA_AUTH_MAC_INFO));
  info->tagged_only = 0;
  info->expect_tag_vid = SOC_PPC_LLP_SA_AUTH_ACCEPT_ALL_VIDS;
  soc_sand_SAND_PP_SYS_PORT_ID_clear(&(info->expect_system_port));
  info->expect_system_port.sys_id = SOC_PPC_LLP_SA_AUTH_ACCEPT_ALL_PORTS;
     
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_SA_AUTH_MATCH_RULE_clear(
    SOC_SAND_OUT SOC_PPC_LLP_SA_AUTH_MATCH_RULE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LLP_SA_AUTH_MATCH_RULE));
  info->rule_type = SOC_PPC_NOF_LLP_SA_AUTH_MATCH_RULE_TYPES;
  soc_sand_SAND_PP_SYS_PORT_ID_clear(&(info->port));
  info->vid = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_LLP_SA_AUTH_MATCH_RULE_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_LLP_SA_AUTH_MATCH_RULE_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_LLP_SA_AUTH_MATCH_RULE_TYPE_PORT:
    str = "port";
  break;
  case SOC_PPC_LLP_SA_AUTH_MATCH_RULE_TYPE_VID:
    str = "vid";
  break;
  case SOC_PPC_LLP_SA_MATCH_RULE_TYPE_ALL:
    str = "soc_ppc_llp_sa_match_rule_type_all";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_PPC_LLP_SA_AUTH_PORT_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_SA_AUTH_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "sa_auth_enable: %u\n\r"),info->sa_auth_enable));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_SA_AUTH_MAC_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_SA_AUTH_MAC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "tagged_only: %u\n\r"),info->tagged_only));
  if (info->expect_tag_vid == SOC_PPC_LLP_SA_AUTH_ACCEPT_ALL_VIDS)
  {
    LOG_CLI((BSL_META_U(unit,
                        "expect_tag_vid: ANY\n\r")));
  }
  else
  {
    LOG_CLI((BSL_META_U(unit,
                        "expect_tag_vid: %u\n\r"),info->expect_tag_vid));
  }
  
  if (info->expect_system_port.sys_id == SOC_PPC_LLP_SA_AUTH_ACCEPT_ALL_PORTS)
  {
    LOG_CLI((BSL_META_U(unit,
                        "expect_system_port: ANY\n\r")));
  }
  else
  {
    LOG_CLI((BSL_META_U(unit,
                        "expect_system_port: ")));
  soc_sand_SAND_PP_SYS_PORT_ID_print(&(info->expect_system_port));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_SA_AUTH_MATCH_RULE_print(
    SOC_SAND_IN  SOC_PPC_LLP_SA_AUTH_MATCH_RULE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "rule_type %s "), SOC_PPC_LLP_SA_AUTH_MATCH_RULE_TYPE_to_string(info->rule_type)));
  LOG_CLI((BSL_META_U(unit,
                      "port:")));
  soc_sand_SAND_PP_SYS_PORT_ID_print(&(info->port));
  LOG_CLI((BSL_META_U(unit,
                      "vid: %u\n\r"),info->vid));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

