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

#include <soc/dpp/PPC/ppc_api_frwrd_mact.h>





















void
  SOC_PPC_FRWRD_MACT_ENTRY_KEY_MAC_ADDR_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_ENTRY_KEY_MAC_ADDR *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_MACT_ENTRY_KEY_MAC_ADDR));
  soc_sand_SAND_PP_MAC_ADDRESS_clear(&(info->mac));
  info->fid = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_ENTRY_KEY_IPV4_MC_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_ENTRY_KEY_IPV4_MC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_MACT_ENTRY_KEY_IPV4_MC));
  info->dip = 0;
  info->fid = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_ENTRY_KEY_VAL_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_ENTRY_KEY_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_MACT_ENTRY_KEY_VAL));
  SOC_PPC_FRWRD_MACT_ENTRY_KEY_MAC_ADDR_clear(&(info->mac));
  SOC_PPC_FRWRD_MACT_ENTRY_KEY_IPV4_MC_clear(&(info->ipv4_mc));
#ifdef BCM_88660_A0
  SOC_PPC_FRWRD_MACT_ENTRY_KEY_SLB_clear(&(info->slb));
#endif 
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_ENTRY_KEY_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_ENTRY_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_MACT_ENTRY_KEY));
  info->key_type = SOC_PPC_NOF_FRWRD_MACT_KEY_TYPES;
  
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_ENTRY_AGING_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_ENTRY_AGING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_MACT_ENTRY_AGING_INFO));
  info->is_dynamic = 0;
  info->age_status = 6;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_ENTRY_FRWRD_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_ENTRY_FRWRD_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_MACT_ENTRY_FRWRD_INFO));
  SOC_PPC_FRWRD_DECISION_INFO_clear(&(info->forward_decision));
  info->drop_when_sa_is_known = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_ENTRY_VALUE_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_ENTRY_VALUE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_MACT_ENTRY_VALUE));
  SOC_PPC_FRWRD_MACT_ENTRY_FRWRD_INFO_clear(&(info->frwrd_info));
  SOC_PPC_FRWRD_MACT_ENTRY_AGING_INFO_clear(&(info->aging_info));
  info->accessed = FALSE;
  info->group = 0;

#ifdef BCM_88660_A0
  SOC_PPC_FRWRD_MACT_ENTRY_VALUE_SLB_clear(&(info->slb_info));
#endif 

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_TIME_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_TIME *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_MACT_TIME));
  info->sec = 0;
  info->mili_sec = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_MAC_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_MAC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_MAC));
  info->fid = 0;
  info->fid_mask = 0;
  soc_sand_SAND_PP_MAC_ADDRESS_clear(&(info->mac));
  soc_sand_SAND_PP_MAC_ADDRESS_clear(&(info->mac_mask));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_IPV4_MC_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_IPV4_MC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_IPV4_MC));
  info->dip = 0;
  info->dip_mask = 0;
  info->fid = 0;
  info->fid_mask = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE));
  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_MAC_clear(&(info->mac));
  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_IPV4_MC_clear(&(info->ipv4_mc));
#ifdef BCM_88660_A0
  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_SLB_clear(&info->slb);
#endif 

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE));
  SOC_PPC_FRWRD_MACT_ENTRY_VALUE_clear(&(info->val));
  info->compare_mask = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE));
  info->key_type = SOC_PPC_NOF_FRWRD_MACT_KEY_TYPES;
  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_clear(&(info->key_rule));
  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE_clear(&(info->value_rule));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION));
  info->type = SOC_PPC_NOF_FRWRD_MACT_TRAVERSE_ACTION_TYPES;
  SOC_PPC_FRWRD_MACT_ENTRY_VALUE_clear(&(info->updated_val));
  SOC_PPC_FRWRD_MACT_ENTRY_VALUE_clear(&(info->updated_val_mask));
  info->update_mask = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_TRAVERSE_STATUS_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_TRAVERSE_STATUS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_MACT_TRAVERSE_STATUS_INFO));
  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE_clear(&(info->rule));
  SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_clear(&(info->action));
  SOC_PPC_FRWRD_MACT_TIME_clear(&(info->time_to_finish));
  info->nof_matched_entries = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_ENTRY_KEY_SLB_clear(
    SOC_SAND_OUT  SOC_PPC_FRWRD_MACT_ENTRY_KEY_SLB *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(*info));
  info->is_destination_fec = 0;
  info->destination = 0;
  info->flow_label[0] = 0;
  info->flow_label[1] = 0;
  
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_ENTRY_VALUE_SLB_clear(
    SOC_SAND_OUT  SOC_PPC_FRWRD_MACT_ENTRY_VALUE_SLB *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(*info));
  info->match_fec_entries = 0;
  info->match_lag_entries = 0;
  info->lag_group_and_member = 0;
  info->fec = 0;
  
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_SLB_clear(
    SOC_SAND_OUT  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_SLB *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(*info));
  SOC_PPC_FRWRD_MACT_ENTRY_KEY_SLB_clear(&info->value);
  SOC_PPC_FRWRD_MACT_ENTRY_KEY_SLB_clear(&info->mask);
  
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_FRWRD_MACT_KEY_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_KEY_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_FRWRD_MACT_KEY_TYPE_MAC_ADDR:
    str = "mac_addr";
  break;
  case SOC_PPC_FRWRD_MACT_KEY_TYPE_IPV4_MC:
    str = "ipv4_mc";
  break;
#ifdef BCM_88660_A0
  case SOC_PPC_FRWRD_MACT_KEY_TYPE_SLB:
    str = "slb_entry";
  break;
#endif 
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_FRWRD_MACT_ADD_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ADD_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_FRWRD_MACT_ADD_TYPE_INSERT:
    str = "insert";
  break;
  case SOC_PPC_FRWRD_MACT_ADD_TYPE_LEARN:
    str = "learn";
  break;
  case SOC_PPC_FRWRD_MACT_ADD_TYPE_REFRESH:
    str = "refresh";
  break;
  case SOC_PPC_FRWRD_MACT_ADD_TYPE_TRANSPLANT:
    str = "transplant";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_NONE:
    str = "none";
  break;
  case SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_REMOVE:
    str = "remove";
  break;
  case SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_UPDATE:
    str = "update";
  break;
  case SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_COUNT:
    str = "count";
  break;
  case SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_RETRIEVE:
    str = "retrieve";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_to_string(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_NONE:
    str = "none";
  break;
  case SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DEST_TYPE:
    str = "dest_type";
  break;
  case SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DEST_VAL:
    str = "dest_val";
  break;
  case SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_ADDITIONAL_INFO:
    str = "additional_info";
  break;
  case SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_IS_DYNAMIC:
    str = "is_dynamic";
  break;
  case SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DROP_SA:
    str = "drop_sa";
    break;
  case SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_ALL:
    str = "all";
  break;
  case SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_ACCESSED:
    str = "accessed";
  break;
#ifdef BCM_88660_A0
  case SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_COMPARE_SLB:
    str = "compare_slb";
    break;
  case SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_REPLACE_SLB:
    str = "replace_slb";
    break;
#endif 
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_PPC_FRWRD_MACT_ENTRY_KEY_MAC_ADDR_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY_MAC_ADDR *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "mac:")));
  soc_sand_SAND_PP_MAC_ADDRESS_print(&(info->mac));
  LOG_CLI((BSL_META_U(unit,
                      "  fid: %u\n\r"),info->fid));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_ENTRY_KEY_IPV4_MC_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY_IPV4_MC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "dip: %u\n\r"),info->dip));
  LOG_CLI((BSL_META_U(unit,
                      "fid: %u\n\r"),info->fid));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_ENTRY_KEY_VAL_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "mac:")));
  SOC_PPC_FRWRD_MACT_ENTRY_KEY_MAC_ADDR_print(&(info->mac));
  LOG_CLI((BSL_META_U(unit,
                      "ipv4_mc:")));
  SOC_PPC_FRWRD_MACT_ENTRY_KEY_IPV4_MC_print(&(info->ipv4_mc));
#ifdef BCM_88660_A0
  LOG_CLI((BSL_META_U(unit,
                      "slb:")));
  SOC_PPC_FRWRD_MACT_ENTRY_KEY_SLB_print(&(info->slb));
#endif 
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_ENTRY_KEY_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  switch(info->key_type)
  {
  case SOC_PPC_FRWRD_MACT_KEY_TYPE_IPV4_MC:
    SOC_PPC_FRWRD_MACT_ENTRY_KEY_IPV4_MC_print(&(info->key_val.ipv4_mc));
  break;
  case SOC_PPC_FRWRD_MACT_KEY_TYPE_MAC_ADDR:
  default:
    SOC_PPC_FRWRD_MACT_ENTRY_KEY_MAC_ADDR_print(&(info->key_val.mac));
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_ENTRY_AGING_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_AGING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "is_dynamic: %u\n\r"),info->is_dynamic));
  LOG_CLI((BSL_META_U(unit,
                      "age_status: %u\n\r"), info->age_status));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_ENTRY_FRWRD_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_FRWRD_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "forward_decision:")));
  SOC_PPC_FRWRD_DECISION_INFO_print(&(info->forward_decision));
  LOG_CLI((BSL_META_U(unit,
                      "drop_when_sa_is_known: %u\n\r"),info->drop_when_sa_is_known));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_ENTRY_VALUE_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_VALUE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "frwrd_info:")));
  SOC_PPC_FRWRD_MACT_ENTRY_FRWRD_INFO_print(&(info->frwrd_info));
  LOG_CLI((BSL_META_U(unit,
                      "aging_info:")));
  SOC_PPC_FRWRD_MACT_ENTRY_AGING_INFO_print(&(info->aging_info));
  LOG_CLI((BSL_META_U(unit,
                      "accessed: %u\n\r"),info->accessed));
  LOG_CLI((BSL_META_U(unit,
                      "group: %u\n\r"),info->group));

#ifdef BCM_88660_A0
  LOG_CLI((BSL_META_U(unit,
                      "slb_info:")));
  SOC_PPC_FRWRD_MACT_ENTRY_VALUE_SLB_print(&(info->slb_info));
#endif 

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_TIME_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TIME *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "sec: %u\n\r"),info->sec));
  LOG_CLI((BSL_META_U(unit,
                      "mili_sec: %u\n\r"),info->mili_sec));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_MAC_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_MAC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "fid: %u\n\r"),info->fid));
  LOG_CLI((BSL_META_U(unit,
                      "fid_mask: %u\n\r"),info->fid_mask));
  LOG_CLI((BSL_META_U(unit,
                      "mac:")));
  soc_sand_SAND_PP_MAC_ADDRESS_print(&(info->mac));
  LOG_CLI((BSL_META_U(unit,
                      "mac_mask:")));
  soc_sand_SAND_PP_MAC_ADDRESS_print(&(info->mac_mask));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_IPV4_MC_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_IPV4_MC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "dip: %u\n\r"),info->dip));
  LOG_CLI((BSL_META_U(unit,
                      "dip_mask: %u\n\r"),info->dip_mask));
  LOG_CLI((BSL_META_U(unit,
                      "fid: %u\n\r"),info->fid));
  LOG_CLI((BSL_META_U(unit,
                      "fid_mask: %u\n\r"),info->fid_mask));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "mac:")));
  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_MAC_print(&(info->mac));
  LOG_CLI((BSL_META_U(unit,
                      "ipv4_mc:")));
  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_IPV4_MC_print(&(info->ipv4_mc));

#ifdef BCM_88660_A0
  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_SLB_print(&(info->slb));
#endif 
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "val:")));
  SOC_PPC_FRWRD_MACT_ENTRY_VALUE_print(&(info->val));
  LOG_CLI((BSL_META_U(unit,
                      "compare_mask: %u\n\r"),info->compare_mask));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "key_type %s "), SOC_PPC_FRWRD_MACT_KEY_TYPE_to_string(info->key_type)));
  LOG_CLI((BSL_META_U(unit,
                      "key_rule:")));
  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_print(&(info->key_rule));
  LOG_CLI((BSL_META_U(unit,
                      "value_rule:")));
  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE_print(&(info->value_rule));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "type %s "), SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_to_string(info->type)));
  LOG_CLI((BSL_META_U(unit,
                      "updated_val:")));
  SOC_PPC_FRWRD_MACT_ENTRY_VALUE_print(&(info->updated_val));
  LOG_CLI((BSL_META_U(unit,
                      "updated_val_mask:")));
  SOC_PPC_FRWRD_MACT_ENTRY_VALUE_print(&(info->updated_val_mask));
  LOG_CLI((BSL_META_U(unit,
                      "update_mask: %u\n\r"),info->update_mask));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_TRAVERSE_STATUS_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_STATUS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "rule:")));
  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE_print(&(info->rule));
  LOG_CLI((BSL_META_U(unit,
                      "action:")));
  SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_print(&(info->action));
  LOG_CLI((BSL_META_U(unit,
                      "time_to_finish:")));
  SOC_PPC_FRWRD_MACT_TIME_print(&(info->time_to_finish));
  LOG_CLI((BSL_META_U(unit,
                      "nof_matched_entries: %u\n\r"),info->nof_matched_entries));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_ENTRY_KEY_SLB_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_KEY_SLB *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "is_destination_fec = %d.\n"), info->is_destination_fec));
  LOG_CLI((BSL_META_U(unit,
                      "destination = 0x%08x.\n"), info->destination));
  LOG_CLI((BSL_META_U(unit,
                      "flow_label = 0x%08x%08x.\n"), info->flow_label[1], info->flow_label[0])); 

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_ENTRY_VALUE_SLB_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_ENTRY_VALUE_SLB *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "match_fec_entries = %d.\n"), info->match_fec_entries));
  LOG_CLI((BSL_META_U(unit,
                      "match_lag_entries = %d.\n"), info->match_lag_entries));
  LOG_CLI((BSL_META_U(unit,
                      "lag_group_and_member = 0x%08x.\n"), info->lag_group_and_member));
  LOG_CLI((BSL_META_U(unit,
                      "fec = 0x%08x.\n"), info->fec));
  
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_SLB_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_SLB *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "value: ")));
  SOC_PPC_FRWRD_MACT_ENTRY_KEY_SLB_print(&info->value);
  LOG_CLI((BSL_META_U(unit,
                      "mask: ")));
  SOC_PPC_FRWRD_MACT_ENTRY_KEY_SLB_print(&info->mask);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

