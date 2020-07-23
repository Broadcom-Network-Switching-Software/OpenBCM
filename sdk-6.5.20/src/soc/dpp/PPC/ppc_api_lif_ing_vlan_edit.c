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

#include <soc/dpp/PPC/ppc_api_lif_ing_vlan_edit.h>





















void
  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_TAG_BUILD_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_TAG_BUILD_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_TAG_BUILD_INFO));
  info->tpid_index = 0;
  info->vid_source = SOC_PPC_NOF_LIF_ING_VLAN_EDIT_TAG_VID_SRCS_ARAD; 
  info->pcp_dei_source = SOC_PPC_NOF_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRCS;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_KEY_clear(
    SOC_SAND_OUT SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_KEY));
  info->tag_format = SOC_SAND_PP_NOF_ETHERNET_FRAME_VLAN_FORMATS;
  info->edit_profile = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO));
  info->tags_to_remove = 0;
  info->tpid_profile = 0;           
  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_TAG_BUILD_INFO_clear(&(info->inner_tag));
  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_TAG_BUILD_INFO_clear(&(info->outer_tag));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_LIF_ING_VLAN_EDIT_TAG_VID_SRC_to_string(
    SOC_SAND_IN  SOC_PPC_LIF_ING_VLAN_EDIT_TAG_VID_SRC enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY:
    str = "empty";
  break;
  case SOC_PPC_LIF_ING_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG:
    str = "outer_tag";
  break;
  case SOC_PPC_LIF_ING_VLAN_EDIT_TAG_VID_SRC_INNER_TAG:
    str = "inner_tag";
  break;
  case SOC_PPC_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO:
    str = "vid_edit_outer";
  break;
  case SOC_PPC_LIF_ING_VLAN_EDIT_TAG_VID_SRC_AC_EDIT_INFO_2:
    str = "vid_edit_inner";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC_to_string(
    SOC_SAND_IN  SOC_PPC_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC_NONE:
    str = "none";
  break;
  case SOC_PPC_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC_OUTER_TAG:
    str = "outer_tag";
  break;
  case SOC_PPC_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC_INNER_TAG:
    str = "inner_tag";
  break;
  case SOC_PPC_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC_NEW:
    str = "new";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_TAG_BUILD_INFO_print(
    SOC_SAND_IN  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_TAG_BUILD_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "tpid_index %d\n\r"), info->tpid_index));
  LOG_CLI((BSL_META_U(unit,
                      "vid_source %s\n\r"), SOC_PPC_LIF_ING_VLAN_EDIT_TAG_VID_SRC_to_string(info->vid_source)));
  LOG_CLI((BSL_META_U(unit,
                      "pcp_dei_source %s\n\r"), SOC_PPC_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC_to_string(info->pcp_dei_source)));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_KEY_print(
    SOC_SAND_IN  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "tag_format %s "), soc_sand_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_to_string(info->tag_format)));
  LOG_CLI((BSL_META_U(unit,
                      "edit_profile: %u\n\r"),info->edit_profile));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO_print(
    SOC_SAND_IN  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "tags_to_remove: %u\n\r"), info->tags_to_remove));
  LOG_CLI((BSL_META_U(unit,
                      "tpid_profile: %u\n\r"),info->tpid_profile));
  LOG_CLI((BSL_META_U(unit,
                      "inner_tag:\n\r")));
  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_TAG_BUILD_INFO_print(&(info->inner_tag));
  LOG_CLI((BSL_META_U(unit,
                      "\n\router_tag:\n\r")));
  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_TAG_BUILD_INFO_print(&(info->outer_tag));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

