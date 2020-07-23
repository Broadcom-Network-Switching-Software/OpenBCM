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

#include <soc/dpp/PPC/ppc_api_frwrd_mact_mgmt.h>





















void
  SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO));
  info->enable_ipv4_mc_compatible = 0;
  info->mask_fid = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_AGING_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_AGING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_MACT_AGING_INFO));
  info->enable_aging = 0;
  SOC_PPC_FRWRD_MACT_TIME_clear(&(info->aging_time));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



void
  SOC_PPC_FRWRD_MACT_AGING_ONE_PASS_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_AGING_ONE_PASS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY));
  info->event_type = SOC_PPC_NOF_FRWRD_MACT_EVENT_TYPES;
  info->is_lag = 0;
  info->vsi_event_handle_profile = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO));
  info->self_learning = 0;
  info->send_to_learning_fifo = 0;
  info->send_to_shadow_fifo = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE));
  info->delete_internally = 0;
  info->event_when_aged_out = 0;
  info->event_when_refreshed = 0;
  info->clear_hit_bit = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO));
  info->olp_receive_header_type = SOC_PPC_NOF_FRWRD_MACT_MSG_HDR_TYPES;
  info->header_type = SOC_PPC_NOF_FRWRD_MACT_MSG_HDR_TYPES;
  for (ind = 0; ind < SOC_PPC_FRWRD_MACT_MSG_HDR_SIZE; ++ind)
  {
    info->header[ind] = 0;
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_EVENT_PROCESSING_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_EVENT_PROCESSING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_MACT_EVENT_PROCESSING_INFO));
  info->type = SOC_PPC_NOF_FRWRD_MACT_EVENT_PATH_TYPES;
  SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO_clear(&(info->info));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_OPER_MODE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_OPER_MODE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_MACT_OPER_MODE_INFO));
  info->learning_mode = SOC_PPC_NOF_FRWRD_MACT_LEARNING_MODES;
  info->soc_petra_a_compatible = 0;
  SOC_PPC_FRWRD_MACT_EVENT_PROCESSING_INFO_clear(&(info->learn_msgs_info));
  info->shadow_mode = SOC_PPC_NOF_FRWRD_MACT_SHADOW_MODES;
  SOC_PPC_FRWRD_MACT_EVENT_PROCESSING_INFO_clear(&(info->shadow_msgs_info));
  info->learn_in_elk = 0;
  info->disable_learning = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_MAC_LIMIT_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_MAC_LIMIT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_MACT_MAC_LIMIT_INFO));
  info->is_limited = 0;
  info->nof_entries = 0;
  info->action_when_exceed = SOC_PPC_NOF_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPES;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO));
  info->fid_fail = 0;
  info->fid_allowed = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO));
  info->enable = 0;
  info->static_may_exceed = 0;
  info->generate_event = 0;
  info->fid_base = 0;
  info->glbl_limit = SOC_PPC_FRWRD_MACT_NO_GLOBAL_LIMIT;
  info->cpu_may_exceed = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_MAC_LIMIT_RANGE_MAP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_MAC_LIMIT_RANGE_MAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_MACT_MAC_LIMIT_RANGE_MAP_INFO));
  info->zero_msbs = 0;
  info->shift_right_bits = 0;
  info->base_add_val = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_MAC_LIMIT_MAPPING_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_MAC_LIMIT_MAPPING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_MACT_MAC_LIMIT_MAPPING_INFO));
  info->invalid_ptr = 0;
  info->range_end[0] = 0x8000;
  info->range_end[1] = 0xC000;
  info->range_end[2] = 0xD000;

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_MACT_PORT_INFO));
  info->sa_drop_action_profile = 0;
  info->sa_unknown_action_profile = 0;
  info->da_unknown_action_profile = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_EVENT_LAG_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_EVENT_LAG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_MACT_EVENT_LAG_INFO));
  info->is_lag = 0;
  info->id = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_EVENT_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_EVENT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_MACT_EVENT_INFO));
  info->type = SOC_PPC_NOF_FRWRD_MACT_EVENT_TYPES;
  SOC_PPC_FRWRD_MACT_ENTRY_KEY_clear(&(info->key));
  SOC_PPC_FRWRD_MACT_ENTRY_VALUE_clear(&(info->value));
  SOC_PPC_FRWRD_MACT_EVENT_LAG_INFO_clear(&(info->lag));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_EVENT_BUFFER_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_EVENT_BUFFER *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_MACT_EVENT_BUFFER));
  for (ind = 0; ind < SOC_PPC_FRWRD_MACT_EVENT_BUFF_MAX_SIZE; ++ind)
  {
    info->buff[ind] = 0;
  }
  info->buff_len = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void
  SOC_PPC_FRWRD_MACT_LEARN_MSG_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_LEARN_MSG *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_MACT_LEARN_MSG));
  info->max_nof_events = 0;
  info->msg_buffer = 0;
  info->msg_len = 0;

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_PPC_FRWRD_MACT_LEARN_MSG_CONF_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_LEARN_MSG_CONF *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_MACT_LEARN_MSG_CONF));
  info->recv_mode = 0;
  info->header_size = 0;

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_PPC_FRWRD_MACT_LEARN_MSG_PARSE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_LEARN_MSG_PARSE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  info->source_fap = 0;
  info->nof_events_in_msg = 0;
  info->nof_parsed_events = 0;

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_FRWRD_MACT_LEARNING_MODE_to_string(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_LEARNING_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_FRWRD_MACT_LEARNING_MODE_INGRESS_DISTRIBUTED:
    str = "ingress_distributed";
  break;
  case SOC_PPC_FRWRD_MACT_LEARNING_MODE_INGRESS_CENTRALIZED:
    str = "ingress_centralized";
  break;
  case SOC_PPC_FRWRD_MACT_LEARNING_MODE_EGRESS_DISTRIBUTED:
    str = "egress_distributed";
  break;
  case SOC_PPC_FRWRD_MACT_LEARNING_MODE_EGRESS_CENTRALIZED:
    str = "egress_centralized";
  break;
  case SOC_PPC_FRWRD_MACT_LEARNING_MODE_EGRESS_INDEPENDENT:
    str = "egress_independent";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_FRWRD_MACT_SHADOW_MODE_to_string(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_SHADOW_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_FRWRD_MACT_SHADOW_MODE_NONE:
    str = "none";
  break;
  case SOC_PPC_FRWRD_MACT_SHADOW_MODE_ARP:
    str = "arp";
  break;
  case SOC_PPC_FRWRD_MACT_SHADOW_MODE_LAG:
    str = "lag";
  break;
  case SOC_PPC_FRWRD_MACT_SHADOW_MODE_ALL:
    str = "all";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_RAW:
    str = "raw";
  break;
  case SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_ITMH:
    str = "itmh";
  break;
  case SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_ETH_O_ITMH:
    str = "eth_o_itmh";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE_NONE:
    str = "none";
  break;
  case SOC_PPC_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE_INTERRUPT:
    str = "interrupt";
  break;
  case SOC_PPC_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE_MSG:
    str = "msg";
  break;
  case SOC_PPC_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE_EVENT:
    str = "event";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_FRWRD_MACT_TRAP_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAP_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_FRWRD_MACT_TRAP_TYPE_SA_DROP:
    str = "sa_drop";
  break;
  case SOC_PPC_FRWRD_MACT_TRAP_TYPE_SA_UNKNOWN:
    str = "sa_unknown";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_FRWRD_MACT_EVENT_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_EVENT_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_FRWRD_MACT_EVENT_TYPE_NONE:
    str = "none";
  break;
  case SOC_PPC_FRWRD_MACT_EVENT_TYPE_AGED_OUT:
    str = "aged_out";
  break;
  case SOC_PPC_FRWRD_MACT_EVENT_TYPE_LEARN:
    str = "learn";
  break;
  case SOC_PPC_FRWRD_MACT_EVENT_TYPE_TRANSPLANT:
    str = "transplant";
  break;
  case SOC_PPC_FRWRD_MACT_EVENT_TYPE_REFRESH:
    str = "refresh";
  break;
  case SOC_PPC_FRWRD_MACT_EVENT_TYPE_RETRIEVE:
    str = "retrieve";
  break;
  case SOC_PPC_FRWRD_MACT_EVENT_TYPE_LIMIT_EXCEED:
    str = "limit_exceed";
  break;
  case SOC_PPC_FRWRD_MACT_EVENT_TYPE_ACK:
    str = "ack";
  break;
  case SOC_PPC_FRWRD_MACT_EVENT_TYPE_REQ_FAIL:
    str = "req_fail";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_FRWRD_MACT_EVENT_PATH_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_EVENT_PATH_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_FRWRD_MACT_EVENT_PATH_TYPE_MSG_SINGLE:
    str = "msg_single";
  break;
  case SOC_PPC_FRWRD_MACT_EVENT_PATH_TYPE_MSG_AGGR:
    str = "msg_aggr";
  break;
  case SOC_PPC_FRWRD_MACT_EVENT_PATH_TYPE_DIRECT_ACCESS:
    str = "direct_access";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "enable_ipv4_mc_compatible: %u\n\r"),info->enable_ipv4_mc_compatible));
  LOG_CLI((BSL_META_U(unit,
                      "mask_fid: %u\n\r"),info->mask_fid));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_AGING_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_AGING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "enable_aging: %u\n\r"),info->enable_aging));
  LOG_CLI((BSL_META_U(unit,
                      "aging_time:")));
  SOC_PPC_FRWRD_MACT_TIME_print(&(info->aging_time));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_PPC_FRWRD_MACT_AGING_ONE_PASS_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_AGING_ONE_PASS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "event_type %s, "), SOC_PPC_FRWRD_MACT_EVENT_TYPE_to_string(info->event_type)));
  LOG_CLI((BSL_META_U(unit,
                      "is_lag: %u, "),info->is_lag));
  LOG_CLI((BSL_META_U(unit,
                      "vsi_event_handle_profile: %u\n\r"),info->vsi_event_handle_profile));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "self_learning: %u\n\r"),info->self_learning));
  LOG_CLI((BSL_META_U(unit,
                      "send_to_learning_fifo: %u\n\r"),info->send_to_learning_fifo));
  LOG_CLI((BSL_META_U(unit,
                      "send_to_shadow_fifo: %u\n\r"),info->send_to_shadow_fifo));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "delete_internally: %u\n\r"),info->delete_internally));
  LOG_CLI((BSL_META_U(unit,
                      "event_when_aged_out: %u\n\r"),info->event_when_aged_out));
  LOG_CLI((BSL_META_U(unit,
                      "event_when_refreshed: %u\n\r"),info->event_when_refreshed));
  LOG_CLI((BSL_META_U(unit,
                      "clear_hit_bit: %u\n\r"),info->clear_hit_bit));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "olp_receive_header_type %s "), SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_to_string(info->olp_receive_header_type)));

  LOG_CLI((BSL_META_U(unit,
                      "header_type %s "), SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_to_string(info->header_type)));
  for (ind = 0; ind < SOC_PPC_FRWRD_MACT_MSG_HDR_SIZE; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "header[%u]: 0x%08x\n\r"),ind,info->header[ind]));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_EVENT_PROCESSING_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_EVENT_PROCESSING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "type %s "), SOC_PPC_FRWRD_MACT_EVENT_PATH_TYPE_to_string(info->type)));
  LOG_CLI((BSL_META_U(unit,
                      "info:")));
  SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO_print(&(info->info));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_OPER_MODE_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_OPER_MODE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "learning_mode %s "), SOC_PPC_FRWRD_MACT_LEARNING_MODE_to_string(info->learning_mode)));
  LOG_CLI((BSL_META_U(unit,
                      "soc_petra_a_compatible: %u\n\r"),info->soc_petra_a_compatible));
  LOG_CLI((BSL_META_U(unit,
                      "learn_msgs_info:")));
  SOC_PPC_FRWRD_MACT_EVENT_PROCESSING_INFO_print(&(info->learn_msgs_info));
  LOG_CLI((BSL_META_U(unit,
                      "shadow_mode %s \n\r"), SOC_PPC_FRWRD_MACT_SHADOW_MODE_to_string(info->shadow_mode)));
  LOG_CLI((BSL_META_U(unit,
                      "shadow_msgs_info:")));
  SOC_PPC_FRWRD_MACT_EVENT_PROCESSING_INFO_print(&(info->shadow_msgs_info));
  LOG_CLI((BSL_META_U(unit,
                      "disable_learning: %u\n\r"),info->disable_learning));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_MAC_LIMIT_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_MAC_LIMIT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "is_limited: %u\n\r"),info->is_limited));
  LOG_CLI((BSL_META_U(unit,
                      "nof_entries: %u\n\r"),info->nof_entries));
  LOG_CLI((BSL_META_U(unit,
                      "action_when_exceed %s "), SOC_PPC_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE_to_string(info->action_when_exceed)));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "fid_fail: %u\n\r"),info->fid_fail));
  LOG_CLI((BSL_META_U(unit,
                      "fid_allowed: %u\n\r"),info->fid_allowed));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "enable: %u\n\r"),info->enable));
  LOG_CLI((BSL_META_U(unit,
                      "static_may_exceed: %u\n\r"),info->static_may_exceed));
  LOG_CLI((BSL_META_U(unit,
                      "generate_event: %u\n\r"),info->generate_event));
  LOG_CLI((BSL_META_U(unit,
                      "fid_base: %u\n\r"),info->fid_base));
  LOG_CLI((BSL_META_U(unit,
                      "glbl_limit: %u\n\r"),info->glbl_limit));
  LOG_CLI((BSL_META_U(unit,
                      "cpu_may_exceed: %u\n\r"),info->cpu_may_exceed));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_MAC_LIMIT_RANGE_MAP_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_MAC_LIMIT_RANGE_MAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "zero_msbs: %u\n\r"), info->zero_msbs));
  LOG_CLI((BSL_META_U(unit,
                      "shift_right_bits: %u\n\r"), info->shift_right_bits));
  LOG_CLI((BSL_META_U(unit,
                      "base_add_val: %u\n\r"), info->base_add_val));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_MAC_LIMIT_MAPPING_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_MAC_LIMIT_MAPPING_INFO *info
  )
{
  uint32 range_idx;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "invalid_ptr: %u\n\r"),info->invalid_ptr));

  for (range_idx = 0; range_idx < (SOC_PPC_MAX_NOF_MACT_LIMIT_LIF_RANGES - 1); range_idx++) {
      LOG_CLI((BSL_META_U(unit,
                          "range_end for range #%d: %u\n\r"), range_idx, info->range_end[range_idx]));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_PORT_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "sa_drop_action_profile: %u\n\r"),info->sa_drop_action_profile));
  LOG_CLI((BSL_META_U(unit,
                      "sa_unknown_action_profile: %u\n\r"),info->sa_unknown_action_profile));
  LOG_CLI((BSL_META_U(unit,
                      "da_unknown_action_profile: %u\n\r"),info->da_unknown_action_profile));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_EVENT_LAG_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_EVENT_LAG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "is_lag: %u, "),info->is_lag));
  LOG_CLI((BSL_META_U(unit,
                      "id: %u\n\r"),info->id));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_EVENT_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_EVENT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "type %s "), SOC_PPC_FRWRD_MACT_EVENT_TYPE_to_string(info->type)));
  LOG_CLI((BSL_META_U(unit,
                      "key:")));
  SOC_PPC_FRWRD_MACT_ENTRY_KEY_print(&(info->key));
  LOG_CLI((BSL_META_U(unit,
                      "value:")));
  SOC_PPC_FRWRD_MACT_ENTRY_VALUE_print(&(info->value));
  LOG_CLI((BSL_META_U(unit,
                      "lag:")));
  SOC_PPC_FRWRD_MACT_EVENT_LAG_INFO_print(&(info->lag));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_EVENT_BUFFER_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_EVENT_BUFFER *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < SOC_PPC_FRWRD_MACT_EVENT_BUFF_MAX_SIZE; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "buff[%u]: %08x, "),ind,info->buff[ind]));
  }
  LOG_CLI((BSL_META_U(unit,
                      "buff_len: %u\n\r"),info->buff_len));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



void
  SOC_PPC_FRWRD_MACT_LEARN_MSG_print(
    SOC_SAND_IN SOC_PPC_FRWRD_MACT_LEARN_MSG *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "max_nof_events: %u\n\r"),info->max_nof_events));
  LOG_CLI((BSL_META_U(unit,
                      "msg_len: %u\n\r"),info->msg_len));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_PPC_FRWRD_MACT_LEARN_MSG_CONF_print(
    SOC_SAND_IN SOC_PPC_FRWRD_MACT_LEARN_MSG_CONF *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  
  LOG_CLI((BSL_META_U(unit,
                      "recv mode %s "), SOC_TMC_PKT_PACKET_RECV_MODE_to_string(info->recv_mode)));

  LOG_CLI((BSL_META_U(unit,
                      "msg_len: %u\n\r"),info->header_size));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_LEARN_MSG_PARSE_INFO_print(
    SOC_SAND_IN SOC_PPC_FRWRD_MACT_LEARN_MSG_PARSE_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "source_fap: %u\n\r"),info->source_fap));
  LOG_CLI((BSL_META_U(unit,
                      "nof_events_in_msg: %u\n\r"),info->nof_events_in_msg));
  LOG_CLI((BSL_META_U(unit,
                      "nof_parsed_events: %u\n\r"),info->nof_parsed_events));

  for (ind = 0; ind < info->nof_parsed_events; ++ind)
  {
    SOC_PPC_FRWRD_MACT_EVENT_INFO_print(&(info->events[ind]));
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

