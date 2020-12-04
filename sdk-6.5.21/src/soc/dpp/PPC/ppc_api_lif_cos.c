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

#include <soc/dpp/PPC/ppc_api_lif_cos.h>





















void
  SOC_PPC_LIF_COS_AC_PROFILE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LIF_COS_AC_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LIF_COS_AC_PROFILE_INFO));
  info->type = SOC_PPC_NOF_LIF_COS_AC_PROFILE_TYPES;
  info->tc = 0;
  info->dp = 0;
  info->map_table = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LIF_COS_PWE_PROFILE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LIF_COS_PWE_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LIF_COS_PWE_PROFILE_INFO));
  info->type = SOC_PPC_NOF_LIF_COS_PWE_PROFILE_TYPES;
  info->tc = 0;
  info->dp = 0;
  info->map_table = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LIF_COS_PROFILE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LIF_COS_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LIF_COS_PROFILE_INFO));
  info->map_when_ip = 0;
  info->map_when_mpls = 0;
  info->map_when_l2 = 0;
  info->map_from_tc_dp = 0;
  info->forced_tc = 0;
  info->forced_dp = 0;
  info->use_layer2_pcp = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY_clear(
    SOC_SAND_OUT SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY));
  info->tc = 0;
  info->dp = 0;
  info->dscp = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_IP_KEY_clear(
    SOC_SAND_OUT SOC_PPC_LIF_COS_PROFILE_MAP_TBL_IP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LIF_COS_PROFILE_MAP_TBL_IP_KEY));
  info->ip_type = SOC_SAND_PP_NOF_IP_TYPES;
  info->tos = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY_clear(
    SOC_SAND_OUT SOC_PPC_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY));
  info->in_exp = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_L2_KEY_clear(
    SOC_SAND_OUT SOC_PPC_LIF_COS_PROFILE_MAP_TBL_L2_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LIF_COS_PROFILE_MAP_TBL_L2_KEY));
  info->outer_tpid = 0;
  info->incoming_up = 0;
  info->incoming_dei = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY_clear(
    SOC_SAND_OUT SOC_PPC_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY));
  info->tc = 0;
  info->dp = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LIF_COS_OPCODE_ACTION_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LIF_COS_OPCODE_ACTION_INFO));
  info->is_packet_valid = 0;
  info->is_qos_only = 0;
  info->ac_offset = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_LIF_COS_AC_PROFILE_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_LIF_COS_AC_PROFILE_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_LIF_COS_AC_PROFILE_TYPE_NONE:
    str = "none";
  break;
  case SOC_PPC_LIF_COS_AC_PROFILE_TYPE_FORCE_ALWAYS:
    str = "force_always";
  break;
  case SOC_PPC_LIF_COS_AC_PROFILE_TYPE_MAP_IF_TAG_ELSE_FORCE:
    str = "map_if_tag_else_force";
  break;
  case SOC_PPC_LIF_COS_AC_PROFILE_TYPE_MAP_IF_IP_ELSE_FORCE:
    str = "map_if_ip_else_force";
  break;
  case SOC_PPC_LIF_COS_AC_PROFILE_TYPE_MAP_IF_TAG_ELSE_NONE:
    str = "map_if_tag_else_none";
  break;
  case SOC_PPC_LIF_COS_AC_PROFILE_TYPE_MAP_IF_IP_ELSE_NONE:
    str = "map_if_ip_else_none";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_LIF_COS_PWE_PROFILE_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_LIF_COS_PWE_PROFILE_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_LIF_COS_PWE_PROFILE_TYPE_NONE:
    str = "none";
  break;
  case SOC_PPC_LIF_COS_PWE_PROFILE_TYPE_FORCE_ALWAYS:
    str = "force_always";
  break;
  case SOC_PPC_LIF_COS_PWE_PROFILE_TYPE_MAP:
    str = "map";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_LIF_COS_OPCODE_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_LIF_COS_OPCODE_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_LIF_COS_OPCODE_TYPE_L3:
    str = "l3";
  break;
  case SOC_PPC_LIF_COS_OPCODE_TYPE_L2:
    str = "l2";
  break;
  case SOC_PPC_LIF_COS_OPCODE_TYPE_TC_DP:
    str = "tc_dp";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_PPC_LIF_COS_AC_PROFILE_INFO_print(
    SOC_SAND_IN  SOC_PPC_LIF_COS_AC_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "type %s "), SOC_PPC_LIF_COS_AC_PROFILE_TYPE_to_string(info->type)));
  LOG_CLI((BSL_META_U(unit,
                      "tc: %u\n\r"), info->tc));
  LOG_CLI((BSL_META_U(unit,
                      "dp: %u\n\r"), info->dp));
  LOG_CLI((BSL_META_U(unit,
                      "map_table: %u\n\r"),info->map_table));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LIF_COS_PWE_PROFILE_INFO_print(
    SOC_SAND_IN  SOC_PPC_LIF_COS_PWE_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "type %s "), SOC_PPC_LIF_COS_PWE_PROFILE_TYPE_to_string(info->type)));
  LOG_CLI((BSL_META_U(unit,
                      "tc: %u\n\r"), info->tc));
  LOG_CLI((BSL_META_U(unit,
                      "dp: %u\n\r"), info->dp));
  LOG_CLI((BSL_META_U(unit,
                      "map_table: %u\n\r"),info->map_table));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LIF_COS_PROFILE_INFO_print(
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "map_when_ip: %u\n\r"),info->map_when_ip));
  LOG_CLI((BSL_META_U(unit,
                      "map_when_mpls: %u\n\r"),info->map_when_mpls));
  LOG_CLI((BSL_META_U(unit,
                      "map_when_l2: %u\n\r"),info->map_when_l2));
  LOG_CLI((BSL_META_U(unit,
                      "map_from_tc_dp: %u\n\r"),info->map_from_tc_dp));
  LOG_CLI((BSL_META_U(unit,
                      "forced_tc: %u\n\r"), info->forced_tc));
  LOG_CLI((BSL_META_U(unit,
                      "forced_dp: %u\n\r"), info->forced_dp));
#ifdef BCM_88660_A0
  LOG_CLI((BSL_META_U(unit,
                      "use_layer2_pcp: %u\n\r"), info->use_layer2_pcp));
#endif 
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY_print(
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "tc: %u\n\r"), info->tc));
  LOG_CLI((BSL_META_U(unit,
                      "dp: %u\n\r"), info->dp));
  LOG_CLI((BSL_META_U(unit,
                      "dscp: %u\n\r"), info->dscp));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_IP_KEY_print(
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_IP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "ip_type %s "), SOC_SAND_PP_IP_TYPE_to_string(info->ip_type)));
  LOG_CLI((BSL_META_U(unit,
                      "tos: %u\n\r"), info->tos));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY_print(
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "in_exp: %u\n\r"), info->in_exp));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_L2_KEY_print(
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_L2_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "outer_tpid: %u\n\r"), info->outer_tpid));
  LOG_CLI((BSL_META_U(unit,
                      "incoming_up: %u\n\r"), info->incoming_up));
  LOG_CLI((BSL_META_U(unit,
                      "incoming_dei: %u\n\r"), info->incoming_dei));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY_print(
    SOC_SAND_IN  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "tc: %u\n\r"), info->tc));
  LOG_CLI((BSL_META_U(unit,
                      "dp: %u\n\r"), info->dp));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO_print(
    SOC_SAND_IN  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "is_packet_valid: %u\n\r"),info->is_packet_valid));
  LOG_CLI((BSL_META_U(unit,
                      "is_qos_only: %u\n\r"),info->is_qos_only));
  LOG_CLI((BSL_META_U(unit,
                      "ac_offset: %u\n\r"), info->ac_offset));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

