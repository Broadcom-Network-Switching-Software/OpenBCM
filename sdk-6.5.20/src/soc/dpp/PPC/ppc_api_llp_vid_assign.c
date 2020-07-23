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

#include <soc/dpp/PPC/ppc_api_llp_vid_assign.h>





















void
  SOC_PPC_LLP_VID_ASSIGN_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_VID_ASSIGN_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LLP_VID_ASSIGN_PORT_INFO));
  info->pvid = 0;
  info->ignore_incoming_tag = 0;
  info->enable_subnet_ip = 0;
  info->enable_protocol = 0;
  info->enable_sa_based = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_VID_ASSIGN_MAC_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_VID_ASSIGN_MAC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LLP_VID_ASSIGN_MAC_INFO));
  info->vid = 0;
  info->use_for_untagged = 0;
  info->override_tagged = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_VID_ASSIGN_IPV4_SUBNET_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_VID_ASSIGN_IPV4_SUBNET_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LLP_VID_ASSIGN_IPV4_SUBNET_INFO));
  info->vid = 0;
  info->vid_is_valid = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_VID_ASSIGN_PRTCL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_VID_ASSIGN_PRTCL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LLP_VID_ASSIGN_PRTCL_INFO));
  info->vid = 0;
  info->vid_is_valid = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE_clear(
    SOC_SAND_OUT SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE));
  info->rule_type = SOC_PPC_NOF_LLP_VID_ASSIGN_MATCH_RULE_TYPES;
  info->vid = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE_TYPE_TAGGED:
    str = "tagged";
  break;
  case SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE_TYPE_UNTAGGED:
    str = "untagged";
  break;
  case SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE_TYPE_ALL:
    str = "all";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_PPC_LLP_VID_ASSIGN_PORT_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_VID_ASSIGN_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "pvid: %u\n\r"),info->pvid));
  LOG_CLI((BSL_META_U(unit,
                      "ignore_incoming_tag: %u\n\r"),info->ignore_incoming_tag));
  LOG_CLI((BSL_META_U(unit,
                      "enable_subnet_ip: %u\n\r"),info->enable_subnet_ip));
  LOG_CLI((BSL_META_U(unit,
                      "enable_protocol: %u\n\r"),info->enable_protocol));
  LOG_CLI((BSL_META_U(unit,
                      "enable_sa_based: %u\n\r"),info->enable_sa_based));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_VID_ASSIGN_MAC_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_VID_ASSIGN_MAC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "vid: %u\n\r"),info->vid));
  LOG_CLI((BSL_META_U(unit,
                      "use_for_untagged: %u\n\r"),info->use_for_untagged));
  LOG_CLI((BSL_META_U(unit,
                      "override_tagged: %u\n\r"),info->override_tagged));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_VID_ASSIGN_IPV4_SUBNET_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_VID_ASSIGN_IPV4_SUBNET_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "vid: %u\n\r"),info->vid));
  LOG_CLI((BSL_META_U(unit,
                      "vid_is_valid: %u\n\r"),info->vid_is_valid));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_VID_ASSIGN_PRTCL_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_VID_ASSIGN_PRTCL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "vid: %u\n\r"),info->vid));
  LOG_CLI((BSL_META_U(unit,
                      "vid_is_valid: %u\n\r"),info->vid_is_valid));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE_print(
    SOC_SAND_IN  SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "rule_type %s "), SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE_TYPE_to_string(info->rule_type)));
  LOG_CLI((BSL_META_U(unit,
                      "vid: %u\n\r"),info->vid));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

