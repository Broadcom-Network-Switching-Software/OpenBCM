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

#include <soc/dpp/PPC/ppc_api_llp_cos.h>





















void
  SOC_PPC_LLP_COS_GLBL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_COS_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LLP_COS_GLBL_INFO));
  info->default_dp = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_COS_TC_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_COS_TC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LLP_COS_TC_INFO));
  info->tc = 0;
  info->valid = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_COS_UP_USE_clear(
    SOC_SAND_OUT SOC_PPC_LLP_COS_UP_USE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LLP_COS_UP_USE));
  info->use_for_tc = 0;
  info->use_for_out_up = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_COS_PORT_L2_TABLE_INDEXES_clear(
    SOC_SAND_OUT SOC_PPC_LLP_COS_PORT_L2_TABLE_INDEXES *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LLP_COS_PORT_L2_TABLE_INDEXES));
  info->in_up_to_tc_and_de_index = 0;
  info->tc_to_up_index = 0;
  info->up_to_dp_index = 0;
  info->tc_to_dp_index = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_COS_PORT_L2_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_COS_PORT_L2_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LLP_COS_PORT_L2_INFO));
  SOC_PPC_LLP_COS_UP_USE_clear(&(info->up_use));
  info->use_l2_protocol = 0;
  info->ignore_pkt_pcp_for_tc = 0;
  info->use_dei = 0;
  SOC_PPC_LLP_COS_PORT_L2_TABLE_INDEXES_clear(&(info->tbls_select));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_COS_PORT_L3_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_COS_PORT_L3_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LLP_COS_PORT_L3_INFO));
  info->use_ip_qos = 0;
  info->ip_qos_to_tc_index = 0;
  info->use_ip_subnet = 0;
  info->use_mpls_term_lbl = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_COS_PORT_L4_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_COS_PORT_L4_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LLP_COS_PORT_L4_INFO));
  info->use_l4_prtcl = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_COS_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_COS_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LLP_COS_PORT_INFO));
  SOC_PPC_LLP_COS_PORT_L2_INFO_clear(&(info->l2_info));
  SOC_PPC_LLP_COS_PORT_L3_INFO_clear(&(info->l3_info));
  SOC_PPC_LLP_COS_PORT_L4_INFO_clear(&(info->l4_info));
  info->default_tc = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO));
  info->value1 = 0;
  info->value2 = 0;
  info->valid = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_COS_IPV4_SUBNET_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_COS_IPV4_SUBNET_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LLP_COS_IPV4_SUBNET_INFO));
  info->tc = 0;
  info->tc_is_valid = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_COS_PRTCL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_COS_PRTCL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LLP_COS_PRTCL_INFO));
  info->tc = 0;
  info->tc_is_valid = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO));
  info->start = 0;
  info->end = 0;
  info->in_range = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_LLP_COS_MAPPING_TABLE_to_string(
    SOC_SAND_IN  SOC_PPC_LLP_COS_MAPPING_TABLE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_LLP_COS_MAPPING_TABLE_UP_TO_DE_TC:
    str = "up_to_de_tc";
  break;
  case SOC_PPC_LLP_COS_MAPPING_TABLE_IPV4_TOS_TO_DP_TC_VALID:
    str = "ipv4_tos_to_dp_tc_valid";
  break;
  case SOC_PPC_LLP_COS_MAPPING_TABLE_IPV6_TC_TO_DP_TC_VALID:
    str = "ipv6_tc_to_dp_tc_valid";
  break;
  case SOC_PPC_LLP_COS_MAPPING_TABLE_TC_TO_DP:
    str = "tc_to_dp";
  break;
  case SOC_PPC_LLP_COS_MAPPING_TABLE_INCOMING_UP_TO_UP:
    str = "incoming_up_to_up";
  break;
  case SOC_PPC_LLP_COS_MAPPING_TABLE_TC_TO_UP:
    str = "tc_to_up";
  break;
  case SOC_PPC_LLP_COS_MAPPING_TABLE_DE_TO_DP:
    str = "de_to_dp";
  break;
  case SOC_PPC_LLP_COS_MAPPING_TABLE_UP_TO_DP:
    str = "up_to_dp";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_PPC_LLP_COS_GLBL_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_COS_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "default_dp: %u\n\r"), info->default_dp));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_COS_TC_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_COS_TC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "tc: %u\n\r"), info->tc));
  LOG_CLI((BSL_META_U(unit,
                      "valid: %u\n\r"),info->valid));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_COS_UP_USE_print(
    SOC_SAND_IN  SOC_PPC_LLP_COS_UP_USE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "use_for_tc: %u\n\r"),info->use_for_tc));
  LOG_CLI((BSL_META_U(unit,
                      "use_for_out_up: %u\n\r"),info->use_for_out_up));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_COS_PORT_L2_TABLE_INDEXES_print(
    SOC_SAND_IN  SOC_PPC_LLP_COS_PORT_L2_TABLE_INDEXES *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "in_up_to_tc_and_de_index: %u\n\r"),info->in_up_to_tc_and_de_index));
  LOG_CLI((BSL_META_U(unit,
                      "tc_to_up_index: %u\n\r"),info->tc_to_up_index));
  LOG_CLI((BSL_META_U(unit,
                      "up_to_dp_index: %u\n\r"),info->up_to_dp_index));
  LOG_CLI((BSL_META_U(unit,
                      "tc_to_dp_index: %u\n\r"),info->tc_to_dp_index));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_COS_PORT_L2_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_COS_PORT_L2_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "up_use:")));
  SOC_PPC_LLP_COS_UP_USE_print(&(info->up_use));
  LOG_CLI((BSL_META_U(unit,
                      "use_l2_protocol: %u\n\r"),info->use_l2_protocol));
  LOG_CLI((BSL_META_U(unit,
                      "ignore_pkt_pcp_for_tc: %u\n\r"),info->ignore_pkt_pcp_for_tc));
  LOG_CLI((BSL_META_U(unit,
                      "use_dei: %u\n\r"),info->use_dei));
  LOG_CLI((BSL_META_U(unit,
                      "tbls_select:")));
  SOC_PPC_LLP_COS_PORT_L2_TABLE_INDEXES_print(&(info->tbls_select));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_COS_PORT_L3_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_COS_PORT_L3_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "use_ip_qos: %u\n\r"),info->use_ip_qos));
  LOG_CLI((BSL_META_U(unit,
                      "ip_qos_to_tc_index: %u\n\r"),info->ip_qos_to_tc_index));
  LOG_CLI((BSL_META_U(unit,
                      "use_ip_subnet: %u\n\r"),info->use_ip_subnet));
  LOG_CLI((BSL_META_U(unit,
                      "use_mpls_term_lbl: %u\n\r"),info->use_mpls_term_lbl));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_COS_PORT_L4_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_COS_PORT_L4_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "use_l4_prtcl: %u\n\r"),info->use_l4_prtcl));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_COS_PORT_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_COS_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "l2_info:")));
  SOC_PPC_LLP_COS_PORT_L2_INFO_print(&(info->l2_info));
  LOG_CLI((BSL_META_U(unit,
                      "l3_info:")));
  SOC_PPC_LLP_COS_PORT_L3_INFO_print(&(info->l3_info));
  LOG_CLI((BSL_META_U(unit,
                      "l4_info:")));
  SOC_PPC_LLP_COS_PORT_L4_INFO_print(&(info->l4_info));
  LOG_CLI((BSL_META_U(unit,
                      "default_tc: %u\n\r"), info->default_tc));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "value1: %u\n\r"),info->value1));
  LOG_CLI((BSL_META_U(unit,
                      "value2: %u\n\r"),info->value2));
  LOG_CLI((BSL_META_U(unit,
                      "valid: %u\n\r"),info->valid));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_COS_IPV4_SUBNET_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_COS_IPV4_SUBNET_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "tc: %u\n\r"),info->tc));
  LOG_CLI((BSL_META_U(unit,
                      "tc_is_valid: %u\n\r"),info->tc_is_valid));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_COS_PRTCL_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_COS_PRTCL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "tc: %u\n\r"),info->tc));
  LOG_CLI((BSL_META_U(unit,
                      "tc_is_valid: %u\n\r"),info->tc_is_valid));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "start: %u\n\r"),info->start));
  LOG_CLI((BSL_META_U(unit,
                      "end: %u\n\r"),info->end));
  LOG_CLI((BSL_META_U(unit,
                      "in_range: %u\n\r"),info->in_range));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

