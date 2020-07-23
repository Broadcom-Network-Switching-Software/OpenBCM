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
#include <soc/dpp/PPC/ppc_api_trap_mgmt.h>





















void
  SOC_PPC_TRAP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_TRAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_TRAP_INFO));
  SOC_PPC_ACTION_PROFILE_clear(&(info->action_profile));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_OUTLIF_clear(
    SOC_SAND_OUT SOC_PPC_OUTLIF *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_OUTLIF));
  info->type = SOC_PPC_OUTLIF_ENCODE_TYPE_NONE;
  info->val = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_MPLS_COMMAND_clear(
    SOC_SAND_OUT SOC_PPC_MPLS_COMMAND *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_MPLS_COMMAND));
  info->command = SOC_PPC_NOF_MPLS_COMMAND_TYPES;
  info->label = 0;
  info->push_profile = 0;
  info->tpid_profile = 0;
  info->model = SOC_SAND_PP_NOF_MPLS_TUNNEL_MODELS;
  info->has_cw = FALSE;
  info->pop_next_header = SOC_PPC_NOF_PKT_FRWRD_TYPES;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EEI_VAL_clear(
    SOC_SAND_OUT SOC_PPC_EEI_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_EEI_VAL));
  soc_sand_SAND_PP_TRILL_DEST_clear(&(info->trill_dest));
  SOC_PPC_MPLS_COMMAND_clear(&(info->mpls_command));
  info->isid = 0;
  info->raw = 0;
  info->outlif = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EEI_clear(
    SOC_SAND_OUT SOC_PPC_EEI *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_EEI));
  info->type = SOC_PPC_EEI_TYPE_EMPTY;
  SOC_PPC_EEI_VAL_clear(&(info->val));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_DECISION_TYPE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_DECISION_TYPE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_DECISION_TYPE_INFO));
  SOC_PPC_EEI_clear(&(info->eei));
  SOC_PPC_OUTLIF_clear(&(info->outlif));
  SOC_PPC_TRAP_INFO_clear(&(info->trap_info));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_DECISION_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_DECISION_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_DECISION_INFO));
  info->type = SOC_PPC_NOF_FRWRD_DECISION_TYPES;
  info->dest_id = 0;
  SOC_PPC_FRWRD_DECISION_TYPE_INFO_clear(&(info->additional_info));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_ACTION_PROFILE_clear(
    SOC_SAND_OUT SOC_PPC_ACTION_PROFILE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_ACTION_PROFILE));
  info->trap_code = SOC_PPC_TRAP_CODE_RESERVED_MC_0;
  info->frwrd_action_strength = 0;
  info->snoop_action_strength = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_TPID_PROFILE_clear(
    SOC_SAND_OUT SOC_PPC_TPID_PROFILE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_TPID_PROFILE));
  info->tpid1_index = 0;
  info->tpid2_index = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_PEP_KEY_clear(
    SOC_SAND_OUT SOC_PPC_PEP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_PEP_KEY));
  info->port = 0;
  info->vsi = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_IP_ROUTING_TABLE_ITER_clear(
    SOC_SAND_OUT SOC_PPC_IP_ROUTING_TABLE_ITER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_IP_ROUTING_TABLE_ITER));
  info->type = SOC_PPC_NOF_IP_ROUTING_TABLE_ITER_TYPES;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_IP_ROUTING_TABLE_RANGE_clear(
    SOC_SAND_OUT SOC_PPC_IP_ROUTING_TABLE_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_IP_ROUTING_TABLE_RANGE));
  SOC_PPC_IP_ROUTING_TABLE_ITER_clear(&(info->start));
  info->entries_to_scan = 0;
  info->entries_to_act = 0;
  info->entries_from_lem = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_ADDITIONAL_TPID_VALUES_clear(
    SOC_SAND_OUT SOC_PPC_ADDITIONAL_TPID_VALUES *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_ADDITIONAL_TPID_VALUES));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MATCH_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_MATCH_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_MATCH_INFO));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_EEI_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_EEI_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_EEI_TYPE_EMPTY:
    str = "empty";
  break;
  case SOC_PPC_EEI_TYPE_TRILL:
    str = "trill";
  break;
  case SOC_PPC_EEI_TYPE_MPLS:
    str = "mpls";
  break;
  case SOC_PPC_EEI_TYPE_MIM:
    str = "mim";
  break;
  case SOC_PPC_EEI_TYPE_OUTLIF:
    str = "outlif";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_OUTLIF_ENCODE_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_OUTLIF_ENCODE_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_OUTLIF_ENCODE_TYPE_NONE:
    str = "none";
  break;
  case SOC_PPC_OUTLIF_ENCODE_TYPE_RAW:
    str = "raw";
  break;
  case SOC_PPC_OUTLIF_ENCODE_TYPE_AC:
    str = "ac";
  break;
  case SOC_PPC_OUTLIF_ENCODE_TYPE_EEP:
    str = "eep";
  break;
  case SOC_PPC_OUTLIF_ENCODE_TYPE_VSI:
    str = "vsi";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_FRWRD_DECISION_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_FRWRD_DECISION_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_FRWRD_DECISION_TYPE_DROP:
    str = "drop";
  break;
  case SOC_PPC_FRWRD_DECISION_TYPE_UC_FLOW:
    str = "uc_flow";
  break;
  case SOC_PPC_FRWRD_DECISION_TYPE_UC_LAG:
    str = "uc_lag";
  break;
  case SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT:
    str = "uc_port";
  break;
  case SOC_PPC_FRWRD_DECISION_TYPE_MC:
    str = "mc";
  break;
  case SOC_PPC_FRWRD_DECISION_TYPE_FEC:
    str = "fec";
  break;
  case SOC_PPC_FRWRD_DECISION_TYPE_TRAP:
    str = "trap";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_MPLS_COMMAND_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_MPLS_COMMAND_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_MPLS_COMMAND_TYPE_PUSH:
    str = "push";
  break;
  case SOC_PPC_MPLS_COMMAND_TYPE_POP:
    str = "pop";
  break;
  case SOC_PPC_MPLS_COMMAND_TYPE_POP_INTO_MPLS_PIPE:
    str = "pop_into_mpls_pipe";
  break;
  case SOC_PPC_MPLS_COMMAND_TYPE_POP_INTO_MPLS_UNIFORM:
    str = "pop_into_mpls_uniform";
  break;
  case SOC_PPC_MPLS_COMMAND_TYPE_POP_INTO_IPV4_PIPE:
    str = "pop_into_ipv4_pipe";
  break;
  case SOC_PPC_MPLS_COMMAND_TYPE_POP_INTO_IPV4_UNIFORM:
    str = "pop_into_ipv4_uniform";
  break;
  case SOC_PPC_MPLS_COMMAND_TYPE_POP_INTO_IPV6_PIPE:
    str = "pop_into_ipv6_pipe";
  break;
  case SOC_PPC_MPLS_COMMAND_TYPE_POP_INTO_IPV6_UNIFORM:
    str = "pop_into_ipv6_uniform";
  break;
  case SOC_PPC_MPLS_COMMAND_TYPE_POP_INTO_ETHERNET:
    str = "pop_into_ethernet";
  break;
  case SOC_PPC_MPLS_COMMAND_TYPE_SWAP:
    str = "swap";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_L2_NEXT_PRTCL_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_L2_NEXT_PRTCL_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_L2_NEXT_PRTCL_TYPE_MAC_IN_MAC:
    str = "mac_in_mac";
  break;
  case SOC_PPC_L2_NEXT_PRTCL_TYPE_TRILL:
    str = "trill";
  break;
  case SOC_PPC_L2_NEXT_PRTCL_TYPE_IPV4:
    str = "ipv4";
  break;
  case SOC_PPC_L2_NEXT_PRTCL_TYPE_IPV6:
    str = "ipv6";
  break;
  case SOC_PPC_L2_NEXT_PRTCL_TYPE_ARP:
    str = "arp";
  break;
  case SOC_PPC_L2_NEXT_PRTCL_TYPE_CFM:
    str = "cfm";
  break;
  case SOC_PPC_L2_NEXT_PRTCL_TYPE_MPLS:
    str = "mpls";
  break;
  case SOC_PPC_L2_NEXT_PRTCL_TYPE_FC_ETH:
    str = "FC_ETH";
  break;
  case SOC_PPC_L2_NEXT_PRTCL_TYPE_OTHER:
    str = "other";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_L3_NEXT_PRTCL_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_L3_NEXT_PRTCL_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_L3_NEXT_PRTCL_TYPE_NONE:
    str = "none";
  break;
  case SOC_PPC_L3_NEXT_PRTCL_TYPE_TCP:
    str = "tcp";
  break;
  case SOC_PPC_L3_NEXT_PRTCL_TYPE_UDP:
    str = "udp";
  break;
  case SOC_PPC_L3_NEXT_PRTCL_TYPE_IGMP:
    str = "igmp";
  break;
  case SOC_PPC_L3_NEXT_PRTCL_TYPE_ICMP:
    str = "icmp";
  break;
  case SOC_PPC_L3_NEXT_PRTCL_TYPE_ICMPV6:
    str = "icmpv6";
  break;
  case SOC_PPC_L3_NEXT_PRTCL_TYPE_IPV4:
    str = "ipv4";
  break;
  case SOC_PPC_L3_NEXT_PRTCL_TYPE_IPV6:
    str = "ipv6";
  break;
  case SOC_PPC_L3_NEXT_PRTCL_TYPE_MPLS:
    str = "mpls";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_PKT_TERM_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_PKT_TERM_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_PKT_TERM_TYPE_NONE:
    str = "none";
  break;
  case SOC_PPC_PKT_TERM_TYPE_ETH:
    str = "eth";
  break;
  case SOC_PPC_PKT_TERM_TYPE_IPV4_ETH:
    str = "ipv4_eth";
  break;
  case SOC_PPC_PKT_TERM_TYPE_IPV6_ETH:
    str = "ipv6_eth";
  break;
  case SOC_PPC_PKT_TERM_TYPE_MPLS_ETH:
    str = "mpls_eth";
  break;
  case SOC_PPC_PKT_TERM_TYPE_CW_MPLS_ETH:
    str = "cw_mpls_eth";
  break;
  case SOC_PPC_PKT_TERM_TYPE_MPLS2_ETH:
    str = "mpls2_eth";
  break;
  case SOC_PPC_PKT_TERM_TYPE_CW_MPLS2_ETH:
    str = "cw_mpls2_eth";
  break;
  case SOC_PPC_PKT_TERM_TYPE_MPLS3_ETH:
    str = "mpls3_eth";
  break;
  case SOC_PPC_PKT_TERM_TYPE_CW_MPLS3_ETH:
    str = "cw_mpls3_eth";
  break;
  case SOC_PPC_PKT_TERM_TYPE_TRILL:
    str = "trill";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_PKT_FRWRD_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_PKT_FRWRD_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_PKT_FRWRD_TYPE_BRIDGE:
    str = "bridge";
  break;
  case SOC_PPC_PKT_FRWRD_TYPE_IPV4_UC:
    str = "ipv4_uc";
  break;
  case SOC_PPC_PKT_FRWRD_TYPE_IPV4_MC:
    str = "ipv4_mc";
  break;
  case SOC_PPC_PKT_FRWRD_TYPE_IPV6_UC:
    str = "ipv6_uc";
  break;
  case SOC_PPC_PKT_FRWRD_TYPE_IPV6_MC:
    str = "ipv6_mc";
  break;
  case SOC_PPC_PKT_FRWRD_TYPE_MPLS:
    str = "mpls";
  break;
  case SOC_PPC_PKT_FRWRD_TYPE_TRILL:
    str = "trill";
  break;
  case SOC_PPC_PKT_FRWRD_TYPE_CPU_TRAP:
    str = "cpu_trap";
  break;
  case SOC_PPC_PKT_FRWRD_TYPE_BRIDGE_AFTER_TERM:
    str = "bridge_after_term";
  break;
  case SOC_PPC_PKT_FRWRD_TYPE_CUSTOM1:
    str = "FCF";
  break;
  case SOC_PPC_PKT_FRWRD_TYPE_TM:
    str = "tm";
    break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_PKT_HDR_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_PKT_HDR_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_PKT_HDR_TYPE_NONE:
    str = "none";
  break;
  case SOC_PPC_PKT_HDR_TYPE_ETH:
    str = "eth";
  break;
  case SOC_PPC_PKT_HDR_TYPE_IPV4:
    str = "ipv4";
  break;
  case SOC_PPC_PKT_HDR_TYPE_IPV6:
    str = "ipv6";
  break;
  case SOC_PPC_PKT_HDR_TYPE_MPLS:
    str = "mpls";
  break;
  case SOC_PPC_PKT_HDR_TYPE_TRILL:
    str = "trill";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_IP_ROUTING_TABLE_ITER_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_IP_ROUTING_TABLE_ITER_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_IP_ROUTING_TABLE_ITER_TYPE_FAST:
    str = "fast";
  break;
  case SOC_PPC_IP_ROUTING_TABLE_ITER_TYPE_IP_PREFIX_ORDERED:
    str = "ip_prefix_ordered";
  break;
  case SOC_PPC_IP_ROUTING_TABLE_ITER_TYPE_PREFIX_IP_ORDERED:
    str = "prefix_ip_ordered";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_HASH_MASKS_to_string(
    SOC_SAND_IN  SOC_PPC_HASH_MASKS enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_HASH_MASKS_MAC_SA:
    str = "mac_sa";
  break;
  case SOC_PPC_HASH_MASKS_MAC_DA:
    str = "mac_da";
  break;
  case SOC_PPC_HASH_MASKS_VSI:
    str = "vsi";
  break;
  case SOC_PPC_HASH_MASKS_ETH_TYPE_CODE:
    str = "eth_type_code";
  break;
  case SOC_PPC_HASH_MASKS_MPLS_LABEL_1:
    str = "mpls_label_1";
  break;
  case SOC_PPC_HASH_MASKS_MPLS_LABEL_2:
    str = "mpls_label_2";
  break;
  case SOC_PPC_HASH_MASKS_MPLS_LABEL_3:
    str = "mpls_label_3";
  break;
  case SOC_PPC_HASH_MASKS_MPLS_LABEL_4:
    str = "mpls_label_4";
  break;
  case SOC_PPC_HASH_MASKS_MPLS_LABEL_5:
    str = "mpls_label_5";
  break;
  case SOC_PPC_HASH_MASKS_IPV4_SIP:
    str = "ipv4_sip";
  break;
  case SOC_PPC_HASH_MASKS_IPV4_DIP:
    str = "ipv4_dip";
  break;
  case SOC_PPC_HASH_MASKS_IPV4_PROTOCOL:
    str = "ipv4_protocol";
  break;
  case SOC_PPC_HASH_MASKS_IPV6_SIP:
    str = "ipv6_sip";
  break;
  case SOC_PPC_HASH_MASKS_IPV6_DIP:
    str = "ipv6_dip";
  break;
  case SOC_PPC_HASH_MASKS_IPV6_PROTOCOL:
    str = "ipv6_protocol";
  break;
  case SOC_PPC_HASH_MASKS_L4_SRC_PORT:
    str = "l4_src_port";
  break;
  case SOC_PPC_HASH_MASKS_L4_DEST_PORT:
    str = "l4_dest_port";
  break;
  case SOC_PPC_HASH_MASKS_FC_DEST_ID:
    str = "fc_dest_id";
  break;
  case SOC_PPC_HASH_MASKS_FC_SRC_ID:
    str = "fc_src_id";
  break;
  case SOC_PPC_HASH_MASKS_FC_SEQ_ID:
    str = "fc_seq_id";
  break;
  case SOC_PPC_HASH_MASKS_FC_ORG_EX_ID:
    str = "fc_org_ex_id";
  break;
  case SOC_PPC_HASH_MASKS_FC_RES_EX_ID:
    str = "fc_res_ex_id";
  break;
  case SOC_PPC_HASH_MASKS_FC_VFI:
    str = "fc_vfi";
  break;
  case SOC_PPC_HASH_MASKS_TRILL_EG_NICK:
    str = "trill_eg_nick";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_PPC_TRAP_INFO_print(
    SOC_SAND_IN  SOC_PPC_TRAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "action_profile:")));
  SOC_PPC_ACTION_PROFILE_print(&(info->action_profile));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_OUTLIF_print(
    SOC_SAND_IN  SOC_PPC_OUTLIF *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "type %s "), SOC_PPC_OUTLIF_ENCODE_TYPE_to_string(info->type)));
  LOG_CLI((BSL_META_U(unit,
                      "val: %u\n\r"),info->val));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_MPLS_COMMAND_print(
    SOC_SAND_IN  SOC_PPC_MPLS_COMMAND *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "command %s "), SOC_PPC_MPLS_COMMAND_TYPE_to_string(info->command)));
  LOG_CLI((BSL_META_U(unit,
                      "label: %u\n\r"),info->label));
  LOG_CLI((BSL_META_U(unit,
                      "push_profile: %u\n\r"),info->push_profile));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EEI_VAL_print(
    SOC_SAND_IN  SOC_PPC_EEI_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "trill_dest:")));
  soc_sand_SAND_PP_TRILL_DEST_print(&(info->trill_dest));
  LOG_CLI((BSL_META_U(unit,
                      "mpls_command:")));
  SOC_PPC_MPLS_COMMAND_print(&(info->mpls_command));
  LOG_CLI((BSL_META_U(unit,
                      "isid: %u\n\r"),info->isid));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EEI_print(
    SOC_SAND_IN  SOC_PPC_EEI *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "type %s "), SOC_PPC_EEI_TYPE_to_string(info->type)));
  switch(info->type)
  {
  case SOC_PPC_EEI_TYPE_MPLS:
    SOC_PPC_MPLS_COMMAND_print(&(info->val.mpls_command));
    break;
  case SOC_PPC_EEI_TYPE_TRILL:
    soc_sand_SAND_PP_TRILL_DEST_print(&(info->val.trill_dest));
    break;
  case SOC_PPC_EEI_TYPE_MIM:
    LOG_CLI((BSL_META_U(unit,
                        "isid: %u\n\r"),info->val.isid));
    break;
  default:
    break;
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_DECISION_TYPE_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_DECISION_TYPE      frwrd_type,
    SOC_SAND_IN  SOC_PPC_FRWRD_DECISION_TYPE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  switch(frwrd_type)
  {
  case SOC_PPC_FRWRD_DECISION_TYPE_FEC:
  case SOC_PPC_FRWRD_DECISION_TYPE_MC:
    LOG_CLI((BSL_META_U(unit,
                        "eei:")));
    SOC_PPC_EEI_print(&(info->eei));
  break;
  case SOC_PPC_FRWRD_DECISION_TYPE_UC_FLOW:
  case SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT:
  case SOC_PPC_FRWRD_DECISION_TYPE_UC_LAG:
    LOG_CLI((BSL_META_U(unit,
                        "outlif:")));
    SOC_PPC_OUTLIF_print(&(info->outlif));
  break;
  case SOC_PPC_FRWRD_DECISION_TYPE_TRAP:
    LOG_CLI((BSL_META_U(unit,
                        "trap_info:")));
    SOC_PPC_TRAP_INFO_print(&(info->trap_info));
  break;
  default:
    break;
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_DECISION_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_DECISION_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "type %s "), SOC_PPC_FRWRD_DECISION_TYPE_to_string(info->type)));
  LOG_CLI((BSL_META_U(unit,
                      "dest_id: %u  "),info->dest_id));
  SOC_PPC_FRWRD_DECISION_TYPE_INFO_print(info->type, &(info->additional_info));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_DECISION_INFO_print_table_format(
  SOC_SAND_IN  char                *line_pref,
  SOC_SAND_IN  SOC_PPC_FRWRD_DECISION_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "%-7s|"),
           SOC_PPC_FRWRD_DECISION_TYPE_to_string(info->type)
           ));

  if (info->type == SOC_PPC_FRWRD_DECISION_TYPE_DROP)
  {
    LOG_CLI((BSL_META_U(unit,
                        "%-6s|"),"   -" ));
    LOG_CLI((BSL_META_U(unit,
                        "%-6s|"),"   -" ));
    LOG_CLI((BSL_META_U(unit,
                        "%10s%-10s ")," ","-"));
  }
  else
  {
    LOG_CLI((BSL_META_U(unit,
                        "%-6d|"),info->dest_id));
  }

  switch(info->type)
  {  
    case SOC_PPC_FRWRD_DECISION_TYPE_UC_FLOW:
    case SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT:
    case SOC_PPC_FRWRD_DECISION_TYPE_UC_LAG:
        if(info->additional_info.eei.type == SOC_PPC_EEI_TYPE_EMPTY)
        {
            LOG_CLI((BSL_META_U(unit,
                                "%-6s|"),
                     "OUTLIF"
                     ));

            LOG_CLI((BSL_META_U(unit,
                                "%-7s|   %-10u"),
                     SOC_PPC_OUTLIF_ENCODE_TYPE_to_string(info->additional_info.outlif.type),
                     info->additional_info.outlif.val
                     ));
            break;
        }
        
    case SOC_PPC_FRWRD_DECISION_TYPE_FEC:
    case SOC_PPC_FRWRD_DECISION_TYPE_MC:
        LOG_CLI((BSL_META_U(unit,
                            "%-6s|"),
                 "  EEI"
                 ));
       
        switch(info->additional_info.eei.type)
        {
          case SOC_PPC_EEI_TYPE_EMPTY:
            LOG_CLI((BSL_META_U(unit,
                                "%-6s| %-13s"),
                     "EMPTY", "      -"
                     ));
            break;
        case SOC_PPC_EEI_TYPE_OUTLIF:
            LOG_CLI((BSL_META_U(unit,
                                "%-6s| %-13u"),
                     "OUTLIF", info->additional_info.eei.val.outlif
                     ));
        break;
          case SOC_PPC_EEI_TYPE_MIM:
            LOG_CLI((BSL_META_U(unit,
                                "%-6s| %-13u"),
                     " ISID", info->additional_info.eei.val.isid
                     ));
            break;
          case SOC_PPC_EEI_TYPE_MPLS:
            if (info->additional_info.eei.val.mpls_command.command == SOC_PPC_MPLS_COMMAND_TYPE_PUSH)
            {
              LOG_CLI((BSL_META_U(unit,
                                  "Push  L:%-7u Pr:%-2u"),
                       info->additional_info.eei.val.mpls_command.label,
                       info->additional_info.eei.val.mpls_command.push_profile
                       ));
            }
            else if (info->additional_info.eei.val.mpls_command.command == SOC_PPC_MPLS_COMMAND_TYPE_SWAP)
            {
              LOG_CLI((BSL_META_U(unit,
                                  "Swap  L:%-7u      "),
                       info->additional_info.eei.val.mpls_command.label
                       ));
            }
            else 
            {
              LOG_CLI((BSL_META_U(unit,
                                  "%-20s"),
                       SOC_PPC_MPLS_COMMAND_TYPE_to_string(info->additional_info.eei.val.mpls_command.command)
                       ));
            }
            break;
          case SOC_PPC_EEI_TYPE_TRILL:
            LOG_CLI((BSL_META_U(unit,
                                "%s%-6u %s"),
                     "TRILL  Dest:",
                     info->additional_info.eei.val.trill_dest.dest_nick,
                     (info->additional_info.eei.val.trill_dest.is_multicast)?"MC":"UC"
                     ));
            break;
          default:
            break;
        }

        break;

  case SOC_PPC_FRWRD_DECISION_TYPE_TRAP:
    LOG_CLI((BSL_META_U(unit,
                        "%-7s|"),
             "TRAP"
             ));
    LOG_CLI((BSL_META_U(unit,
                        "%-17s| %-5u"),
             SOC_PPC_TRAP_CODE_to_string(info->additional_info.trap_info.action_profile.trap_code),
             info->additional_info.trap_info.action_profile.frwrd_action_strength
             ));
  break;
  default:
    break;
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_ACTION_PROFILE_print(
    SOC_SAND_IN  SOC_PPC_ACTION_PROFILE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "trap_code: %s, BCM API: %s\n"), SOC_PPC_TRAP_CODE_to_string(info->trap_code), SOC_PPC_TRAP_CODE_to_api_string(info->trap_code)));
  LOG_CLI((BSL_META_U(unit,
                      "frwrd_action_strength: %u\n\r"),info->frwrd_action_strength));
  LOG_CLI((BSL_META_U(unit,
                      "snoop_action_strength: %u\n\r"),info->snoop_action_strength));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_TPID_PROFILE_print(
    SOC_SAND_IN  SOC_PPC_TPID_PROFILE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "tpid1_index: %u\n\r"), info->tpid1_index));
  LOG_CLI((BSL_META_U(unit,
                      "tpid2_index: %u\n\r"), info->tpid2_index));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_PEP_KEY_print(
    SOC_SAND_IN  SOC_PPC_PEP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "port: %u\n\r"),info->port));
  LOG_CLI((BSL_META_U(unit,
                      "vsi: %u\n\r"),info->vsi));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_IP_ROUTING_TABLE_ITER_print(
    SOC_SAND_IN  SOC_PPC_IP_ROUTING_TABLE_ITER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "type %s "), SOC_PPC_IP_ROUTING_TABLE_ITER_TYPE_to_string(info->type)));
  LOG_CLI((BSL_META_U(unit,
                      "payload:")));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_IP_ROUTING_TABLE_RANGE_print(
    SOC_SAND_IN  SOC_PPC_IP_ROUTING_TABLE_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "start:")));
  SOC_PPC_IP_ROUTING_TABLE_ITER_print(&(info->start));
  LOG_CLI((BSL_META_U(unit,
                      "entries_to_scan: %u\n\r"),info->entries_to_scan));
  LOG_CLI((BSL_META_U(unit,
                      "entries_to_act: %u\n\r"),info->entries_to_act));
  LOG_CLI((BSL_META_U(unit,
                      "entries_from_lem: %u\n\r"),info->entries_from_lem));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_ADDITIONAL_TPID_VALUES_print(
    SOC_SAND_IN  SOC_PPC_ADDITIONAL_TPID_VALUES *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < SOC_PPC_MAX_NOF_ADDITIONAL_TPID_VALS; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "tpid_vals[%u]: 0x%04x\n\r"), ind,info->tpid_vals[ind]));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MATCH_INFO_print(
    SOC_SAND_IN SOC_PPC_FRWRD_MATCH_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "in_lif: %u\n\r"), info->in_lif));
  LOG_CLI((BSL_META_U(unit,
                      "inner_tag: %u\n\r"), info->inner_tag));
  LOG_CLI((BSL_META_U(unit,
                      "outer_tag: %u\n\r"), info->outer_tag));
  LOG_CLI((BSL_META_U(unit,
                      "nof_tags: %u\n\r"), info->nof_tags));
  exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 


#include <soc/dpp/SAND/Utils/sand_footer.h>

