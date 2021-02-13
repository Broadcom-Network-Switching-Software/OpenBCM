/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
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

#include <soc/dpp/PPC/ppc_api_eg_vlan_edit.h>





















void
  SOC_PPC_EG_VLAN_EDIT_PEP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_VLAN_EDIT_PEP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_EG_VLAN_EDIT_PEP_INFO));
  info->pvid = 0;
  info->up = 0;
  info->pep_edit_profile = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_VLAN_EDIT_COMMAND_KEY_clear(
    SOC_SAND_OUT SOC_PPC_EG_VLAN_EDIT_COMMAND_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_EG_VLAN_EDIT_COMMAND_KEY));
  info->tag_format = SOC_SAND_PP_NOF_ETHERNET_FRAME_VLAN_FORMATS;
  info->edit_profile = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_VLAN_EDIT_COMMAND_TAG_BUILD_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_VLAN_EDIT_COMMAND_TAG_BUILD_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_EG_VLAN_EDIT_COMMAND_TAG_BUILD_INFO));
  info->tpid_index = 0;
  info->vid_source = SOC_PPC_NOF_EG_VLAN_EDIT_TAG_VID_SRCS_ARAD;
  info->pcp_dei_source = SOC_PPC_NOF_EG_VLAN_EDIT_TAG_PCP_DEI_SRCS;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO));
  info->tags_to_remove = 0;
  SOC_PPC_EG_VLAN_EDIT_COMMAND_TAG_BUILD_INFO_clear(&(info->inner_tag));
  SOC_PPC_EG_VLAN_EDIT_COMMAND_TAG_BUILD_INFO_clear(&(info->outer_tag));
  info->cep_editing = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_EG_VLAN_EDIT_TAG_VID_SRC_to_string(
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_TAG_VID_SRC enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY:
    str = "empty";
  break;
  case SOC_PPC_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG:
    str = "outer_tag";
  break;
  case SOC_PPC_EG_VLAN_EDIT_TAG_VID_SRC_INNER_TAG:
    str = "inner_tag";
  break;
  case SOC_PPC_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_0:
    str = "vid_edit_outer";
  break;
  case SOC_PPC_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_1:
    str = "vid_edit_inner";
  break;
  case SOC_PPC_EG_VLAN_EDIT_TAG_VID_SRC_ENCAP_INFO:
    str = "encap_info";
  break;
  case SOC_PPC_EG_VLAN_EDIT_TAG_VID_SRC_CEP_EDIT_INFO:
    str = "cep_edit_info";
  break;
  case SOC_PPC_EG_VLAN_EDIT_TAG_VID_SRC_PEP_EDIT:
    str = "pep_edit";
  break;
  case SOC_PPC_EG_VLAN_EDIT_TAG_VID_SRC_VSI:
    str = "vsi";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_SRC_to_string(
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_SRC enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_NONE:
    str = "none";
  break;
  case SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_SRC_MAP:
    str = "map";
  break;
  case SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_SRC_OUTER_TAG:
    str = "outer_tag";
  break;
  case SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_SRC_INNER_TAG:
    str = "inner_tag";
  break;
  case SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_SRC_EDIT_INFO_0:
    str = "edit_info_0";
  break;
  case SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_SRC_EDIT_INFO_1:
    str = "edit_info_1";
  break;
  case SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_SRC_CEP_EDIT_INFO:
    str = "cep_edit_info";
  break;
  case SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_SRC_PEP_EDIT:
    str = "pep_edit";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY_to_string(
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY_TC_DP:
    str = "tc_dp";
  break;
  case SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY_PCP:
    str = "pcp";
  break;
  case SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY_DSCP_EXP:
    str = "dscp_exp";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_PPC_EG_VLAN_EDIT_PEP_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_PEP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "pvid: %u\n\r"),info->pvid));
  LOG_CLI((BSL_META_U(unit,
                      "up: %u\n\r"), info->up));
  LOG_CLI((BSL_META_U(unit,
                      "pep_edit_profile: %u\n\r"),info->pep_edit_profile));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_VLAN_EDIT_COMMAND_KEY_print(
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_COMMAND_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);
  LOG_CLI((BSL_META_U(unit,
                      "tag_format %d (%s) "), info->tag_format, soc_sand_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_to_string(info->tag_format)));
  LOG_CLI((BSL_META_U(unit,
                      "edit_profile: %u\n\r"),info->edit_profile));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_VLAN_EDIT_COMMAND_TAG_BUILD_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_COMMAND_TAG_BUILD_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "  tpid_index: %u\n\r"),info->tpid_index));
  LOG_CLI((BSL_META_U(unit,
                      "  vid_source: %s \n\r"), SOC_PPC_EG_VLAN_EDIT_TAG_VID_SRC_to_string(info->vid_source)));
  LOG_CLI((BSL_META_U(unit,
                      "  pcp_dei_source: %s \n\r"), SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_SRC_to_string(info->pcp_dei_source)));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  
  if (info->is_extender) {
    LOG_CLI((BSL_META_U(unit,
                        "\n\rEdit profile: ETAG format\n\r")));
  }

  LOG_CLI((BSL_META_U(unit,
                      "tags_to_remove: %u\n\r"), info->tags_to_remove));
  LOG_CLI((BSL_META_U(unit,
                      "cep_editing: %u\n\r"),info->cep_editing));
  LOG_CLI((BSL_META_U(unit,
                      "outer_tag:\n\r")));
  SOC_PPC_EG_VLAN_EDIT_COMMAND_TAG_BUILD_INFO_print(&(info->outer_tag));
  LOG_CLI((BSL_META_U(unit,
                      "inner_tag:\n\r")));
  SOC_PPC_EG_VLAN_EDIT_COMMAND_TAG_BUILD_INFO_print(&(info->inner_tag));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

