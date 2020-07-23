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

#include <soc/dpp/PPC/ppc_api_frwrd_ipv4.h>





















void
  SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_VAL_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_VAL));
  info->route_val = 0;
  SOC_PPC_ACTION_PROFILE_clear(&(info->action_profile));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION));
  info->type = SOC_PPC_NOF_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_TYPES;
  SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_VAL_clear(&(info->value));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_IPV4_ROUTER_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_ROUTER_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_IPV4_ROUTER_INFO));
  SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_clear(&(info->uc_default_action));
  SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_clear(&(info->mc_default_action));
  info->flood_unknown_mc = 0;
  info->ipv4_mc_l2_lookup = 0;
  info->rpf_default_route = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_IPV4_VRF_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_VRF_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_IPV4_VRF_INFO));
  SOC_PPC_FRWRD_IPV4_ROUTER_INFO_clear(&(info->router_info));
  info->use_dflt_non_vrf_routing = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_IPV4_GLBL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_IPV4_GLBL_INFO));
  SOC_PPC_FRWRD_IPV4_ROUTER_INFO_clear(&(info->router_info));
  info->uc_table_resouces = SOC_PPC_NOF_FRWRD_IPV4_HOST_TABLE_RESOURCES;
  info->mc_table_resouces = SOC_PPC_NOF_FRWRD_IPV4_MC_HOST_TABLE_RESOURCES;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY));
  soc_sand_SAND_PP_IPV4_SUBNET_clear(&(info->subnet));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY));
  info->group = 0;
  info->group_prefix_len = 0;
  soc_sand_SAND_PP_IPV4_SUBNET_clear(&(info->source));
  info->inrif = 0;
  info->inrif_valid = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_PPC_FRWRD_IP_MC_RP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_MC_RP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_IP_MC_RP_INFO));

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY));
  soc_sand_SAND_PP_IPV4_SUBNET_clear(&(info->subnet));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_IPV4_HOST_KEY_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_HOST_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_IPV4_HOST_KEY));
  info->vrf_ndx = 0;
  info->ip_address = 0;
  info->route_scale = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO));
  SOC_PPC_FRWRD_DECISION_INFO_clear(&info->frwrd_decision);
  info->frwrd_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_FEC;
  info->fec_id = 0;
  info->eep = 0;
  info->mac_lsb = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO));
  soc_sand_SAND_PP_DESTINATION_ID_clear(&(info->dest_id));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_IPV4_MEM_STATUS_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_MEM_STATUS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_IPV4_MEM_STATUS));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_IPV4_MEM_DEFRAG_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_MEM_DEFRAG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_IPV4_MEM_DEFRAG_INFO));
  soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&(info->range));
  info->range.entries_to_act = SOC_SAND_TBL_ITER_SCAN_ALL;
  info->range.entries_to_scan = SOC_SAND_TBL_ITER_SCAN_ALL;

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_TYPE_FEC:
    str = "fec";
  break;
  case SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_TYPE_ACTION_PROFILE:
    str = "action_profile";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_FRWRD_IPV4_HOST_TABLE_RESOURCE_to_string(
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_HOST_TABLE_RESOURCE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_FRWRD_IPV4_HOST_TABLE_RESOURCE_LPM_THEN_LEM:
    str = "lpm_then_lem";
  break;
  case SOC_PPC_FRWRD_IPV4_HOST_TABLE_RESOURCE_LPM_ONLY:
    str = "lpm_only";
  break;
  case SOC_PPC_FRWRD_IPV4_HOST_TABLE_RESOURCE_LEM_THEN_LPM:
    str = "lem_then_lpm";
  break;
  case SOC_PPC_FRWRD_IPV4_HOST_TABLE_RESOURCE_LEM_ONLY:
    str = "lem_only";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_FRWRD_IPV4_MC_HOST_TABLE_RESOURCE_to_string(
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_HOST_TABLE_RESOURCE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_FRWRD_IPV4_MC_HOST_TABLE_RESOURCE_TCAM_ONLY:
    str = "tcam_only";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_FRWRD_IP_CACHE_MODE_to_string(
    SOC_SAND_IN  SOC_PPC_FRWRD_IP_CACHE_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_FRWRD_IP_CACHE_MODE_NONE:
    str = "none";
  break;
  case SOC_PPC_FRWRD_IP_CACHE_MODE_IPV4_UC_LPM:
    str = "lpm";
  break;
  default:
    str = " Unknown";
  }
  return str;
}


const char*
  SOC_PPC_FRWRD_IP_ROUTE_STATUS_to_string(
    SOC_SAND_IN  SOC_PPC_FRWRD_IP_ROUTE_STATUS enum_val
  )
{
  const char* str = " Unknown";
  if(enum_val & SOC_PPC_FRWRD_IP_ROUTE_STATUS_COMMITED)
    str = "commited";
  if(enum_val & SOC_PPC_FRWRD_IP_ROUTE_STATUS_PEND_ADD)
    str = "pend_add";
  if(enum_val & SOC_PPC_FRWRD_IP_ROUTE_STATUS_PEND_REMOVE)
    str = "pend_remove";
  if(enum_val & SOC_PPC_FRWRD_IP_ROUTE_STATUS_ACCESSED)
    str = "accessed";
  return str;
}

const char*
  SOC_PPC_FRWRD_IP_ROUTE_LOCATION_to_string(
    SOC_SAND_IN  SOC_PPC_FRWRD_IP_ROUTE_LOCATION enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_FRWRD_IP_ROUTE_LOCATION_HOST:
    str = "host";
  break;
  case SOC_PPC_FRWRD_IP_ROUTE_LOCATION_LPM:
    str = "lpm";
  break;
  case SOC_PPC_FRWRD_IP_ROUTE_LOCATION_TCAM:
    str = "tcam";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_VAL_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "route_val: %u\n\r"),info->route_val));
  LOG_CLI((BSL_META_U(unit,
                      "action_profile:")));
  SOC_PPC_ACTION_PROFILE_print(&(info->action_profile));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "type %s "), SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_TYPE_to_string(info->type)));
  LOG_CLI((BSL_META_U(unit,
                      "value:")));
  SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_VAL_print(&(info->value));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_IPV4_ROUTER_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_ROUTER_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "uc_default_action:")));
  SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_print(&(info->uc_default_action));
  LOG_CLI((BSL_META_U(unit,
                      "mc_default_action:")));
  SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_print(&(info->mc_default_action));
  LOG_CLI((BSL_META_U(unit,
                      "flood_unknown_mc: %u\n\r"),info->flood_unknown_mc));
  LOG_CLI((BSL_META_U(unit,
                      "ipv4_mc_l2_lookup: %u\n\r"),info->ipv4_mc_l2_lookup));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_IPV4_VRF_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_VRF_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "router_info:")));
  SOC_PPC_FRWRD_IPV4_ROUTER_INFO_print(&(info->router_info));
  LOG_CLI((BSL_META_U(unit,
                      "use_dflt_non_vrf_routing: %u\n\r"),info->use_dflt_non_vrf_routing));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_IPV4_GLBL_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "router_info:")));
  SOC_PPC_FRWRD_IPV4_ROUTER_INFO_print(&(info->router_info));
  LOG_CLI((BSL_META_U(unit,
                      "uc_table_resouces %s "), SOC_PPC_FRWRD_IPV4_HOST_TABLE_RESOURCE_to_string(info->uc_table_resouces)));
  LOG_CLI((BSL_META_U(unit,
                      "mc_table_resouces %s "), SOC_PPC_FRWRD_IPV4_MC_HOST_TABLE_RESOURCE_to_string(info->mc_table_resouces)));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "subnet:")));
  soc_sand_SAND_PP_IPV4_SUBNET_print(&(info->subnet));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "group: %u\n\r"),info->group));
  LOG_CLI((BSL_META_U(unit,
                      "source:")));
  soc_sand_SAND_PP_IPV4_SUBNET_print(&(info->source));
  LOG_CLI((BSL_META_U(unit,
                      "inrif: %u\n\r"),info->inrif));
  LOG_CLI((BSL_META_U(unit,
                      "inrif_valid: %u\n\r"),info->inrif_valid));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_IP_MC_RP_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_IP_MC_RP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "flags: %u\n\r"),info->flags));
  LOG_CLI((BSL_META_U(unit,
                      "rp_id: %u\n\r"),info->rp_id));
  LOG_CLI((BSL_META_U(unit,
                      "inrif: %u\n\r"),info->inrif));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "subnet:")));
  soc_sand_SAND_PP_IPV4_SUBNET_print(&(info->subnet));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_IPV4_HOST_KEY_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_HOST_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "vrf_ndx: %u\n\r"),info->vrf_ndx));
  LOG_CLI((BSL_META_U(unit,
                      "ip_address: %u\n\r"),info->ip_address));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "fec_id: %u\n\r"),info->fec_id));
  SOC_PPC_FRWRD_DECISION_INFO_print(&info->frwrd_decision);
  if (info->eep != SOC_PPC_EEP_NULL)
  {
    LOG_CLI((BSL_META_U(unit,
                        "eep: %u\n\r"),info->eep));
  }
  else
  {
    LOG_CLI((BSL_META_U(unit,
                        "eep: NULL \n\r")));
  }
  
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "dest_id:")));
  soc_sand_SAND_PP_DESTINATION_ID_print(&(info->dest_id));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_IPV4_MEM_STATUS_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MEM_STATUS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);
  soc_sand_SAND_ARR_MEM_ALLOCATOR_MEM_STATUS_print(&(info->mem_stat));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_IPV4_MEM_DEFRAG_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MEM_DEFRAG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "range:")));
  soc_sand_SAND_TABLE_BLOCK_RANGE_print(&(info->range));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

