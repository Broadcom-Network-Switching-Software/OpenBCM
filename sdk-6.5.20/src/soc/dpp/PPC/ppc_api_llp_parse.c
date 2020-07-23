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

#include <soc/dpp/PPC/ppc_api_llp_parse.h>





















void
  SOC_PPC_LLP_PARSE_TPID_VALUES_clear(
    SOC_SAND_OUT SOC_PPC_LLP_PARSE_TPID_VALUES *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LLP_PARSE_TPID_VALUES));
  for (ind = 0; ind < SOC_PPC_LLP_PARSE_NOF_TPID_VALS; ++ind)
  {
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_PARSE_TPID_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_PARSE_TPID_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LLP_PARSE_TPID_INFO));
  info->index = 0;
  info->type = SOC_SAND_PP_VLAN_TAG_TYPE_ANY;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO));
  SOC_PPC_LLP_PARSE_TPID_INFO_clear(&(info->tpid1));
  SOC_PPC_LLP_PARSE_TPID_INFO_clear(&(info->tpid2));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_PARSE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_PARSE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LLP_PARSE_INFO));
  info->outer_tpid = SOC_PPC_NOF_LLP_PARSE_TPID_INDEXS;
  info->inner_tpid = SOC_PPC_NOF_LLP_PARSE_TPID_INDEXS;
  info->is_outer_prio = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO));
  info->flags = 0;
  info->action_trap_code = SOC_PPC_NOF_TRAP_CODES;
  info->tag_format = SOC_SAND_PP_NOF_ETHERNET_FRAME_VLAN_FORMATS;
  info->priority_tag_type = SOC_SAND_PP_NOF_VLAN_TAG_TYPES;
  info->dlft_edit_command_id = 0;
  info->dflt_edit_pcp_profile = 0;
  info->initial_c_tag = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_LLP_PARSE_TPID_INDEX_to_string(
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_TPID_INDEX enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_LLP_PARSE_TPID_INDEX_NONE:
    str = "none";
  break;
  case SOC_PPC_LLP_PARSE_TPID_INDEX_TPID1:
    str = "tpid1";
  break;
  case SOC_PPC_LLP_PARSE_TPID_INDEX_TPID2:
    str = "tpid2";
  break;
  case SOC_PPC_LLP_PARSE_TPID_INDEX_ISID_TPID:
    str = "isid_tpid";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_PPC_LLP_PARSE_TPID_VALUES_print(
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_TPID_VALUES *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < SOC_PPC_LLP_PARSE_NOF_TPID_VALS; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "tpid_vals[%u]: 0x%04x\n\r"), ind,info->tpid_vals[ind]));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_PARSE_TPID_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_TPID_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "index: %u\n\r"), info->index));
  LOG_CLI((BSL_META_U(unit,
                      "type %s "), soc_sand_SAND_PP_VLAN_TAG_TYPE_to_string(info->type)));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "tpid1:\n\r")));
  SOC_PPC_LLP_PARSE_TPID_INFO_print(&(info->tpid1));
  LOG_CLI((BSL_META_U(unit,
                      "\n\r tpid2:\n\r")));
  SOC_PPC_LLP_PARSE_TPID_INFO_print(&(info->tpid2));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_PARSE_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "outer_tpid %s "), SOC_PPC_LLP_PARSE_TPID_INDEX_to_string(info->outer_tpid)));
  LOG_CLI((BSL_META_U(unit,
                      "inner_tpid %s "), SOC_PPC_LLP_PARSE_TPID_INDEX_to_string(info->inner_tpid)));
  LOG_CLI((BSL_META_U(unit,
                      "is_outer_prio: %u\n\r"),info->is_outer_prio));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "action_trap_code %s, BCM API: %s\n\r"), SOC_PPC_TRAP_CODE_to_string(info->action_trap_code), SOC_PPC_TRAP_CODE_to_api_string(info->action_trap_code)));
  LOG_CLI((BSL_META_U(unit,
                      "tag_format %s \n\r"), soc_sand_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_to_string(info->tag_format)));
  LOG_CLI((BSL_META_U(unit,
                      "priority_tag_type %s \n\r"), soc_sand_SAND_PP_VLAN_TAG_TYPE_to_string(info->priority_tag_type)));
  LOG_CLI((BSL_META_U(unit,
                      "dlft_edit_command_id: %u\n\r"),info->dlft_edit_command_id));
  LOG_CLI((BSL_META_U(unit,
                      "dflt_edit_pcp_profile: %u\n\r"),info->dflt_edit_pcp_profile));
  LOG_CLI((BSL_META_U(unit,
                      "initial_c_tag: %u\n\r"),info->initial_c_tag));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

