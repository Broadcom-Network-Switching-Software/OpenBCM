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

#include <soc/dpp/PPC/ppc_api_frwrd_ipv6.h>





















void
  SOC_PPC_FRWRD_IPV6_ROUTER_DEFAULT_ACTION_VAL_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV6_ROUTER_DEFAULT_ACTION_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_IPV6_ROUTER_DEFAULT_ACTION_VAL));
  info->fec_id = 0;
  SOC_PPC_ACTION_PROFILE_clear(&(info->action_profile));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_IPV6_ROUTER_DEFAULT_ACTION_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV6_ROUTER_DEFAULT_ACTION *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_IPV6_ROUTER_DEFAULT_ACTION));
  info->type = SOC_PPC_FRWRD_IPV6_ROUTER_DEFAULT_ACTION_TYPE_ACTION_PROFILE;
  SOC_PPC_FRWRD_IPV6_ROUTER_DEFAULT_ACTION_VAL_clear(&(info->value));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_IPV6_ROUTER_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV6_ROUTER_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_IPV6_ROUTER_INFO));
  SOC_PPC_FRWRD_IPV6_ROUTER_DEFAULT_ACTION_clear(&(info->uc_default_action));
  info->uc_default_action.value.action_profile.trap_code = SOC_PPC_TRAP_CODE_DEFAULT_UCV6;
  SOC_PPC_FRWRD_IPV6_ROUTER_DEFAULT_ACTION_clear(&(info->mc_default_action));
  info->mc_default_action.value.action_profile.trap_code = SOC_PPC_TRAP_CODE_DEFAULT_MCV6;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_IPV6_VRF_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV6_VRF_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_IPV6_VRF_INFO));
  SOC_PPC_FRWRD_IPV6_ROUTER_INFO_clear(&(info->router_info));
  info->router_info.uc_default_action.type = SOC_PPC_FRWRD_IPV6_ROUTER_DEFAULT_ACTION_TYPE_FEC;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY));
  soc_sand_SAND_PP_IPV6_SUBNET_clear(&(info->subnet));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY));
  soc_sand_SAND_PP_IPV6_SUBNET_clear(&(info->group));
  soc_sand_SAND_PP_IPV6_SUBNET_clear(&(info->source));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_IPV6_VPN_ROUTE_KEY_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV6_VPN_ROUTE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_IPV6_VPN_ROUTE_KEY));
  soc_sand_SAND_PP_IPV6_SUBNET_clear(&(info->subnet));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_IPV6_MC_ROUTE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV6_MC_ROUTE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_IPV6_MC_ROUTE_INFO));
  soc_sand_SAND_PP_DESTINATION_ID_clear(&(info->dest_id));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_FRWRD_IPV6_ROUTER_DEFAULT_ACTION_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_ROUTER_DEFAULT_ACTION_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_FRWRD_IPV6_ROUTER_DEFAULT_ACTION_TYPE_FEC:
    str = "fec";
  break;
  case SOC_PPC_FRWRD_IPV6_ROUTER_DEFAULT_ACTION_TYPE_ACTION_PROFILE:
    str = "action_profile";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_PPC_FRWRD_IPV6_ROUTER_DEFAULT_ACTION_VAL_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_ROUTER_DEFAULT_ACTION_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "fec_id: %u\n\r"),info->fec_id));
  LOG_CLI((BSL_META_U(unit,
                      "action_profile:")));
  SOC_PPC_ACTION_PROFILE_print(&(info->action_profile));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_IPV6_ROUTER_DEFAULT_ACTION_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_ROUTER_DEFAULT_ACTION *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "type %s "), SOC_PPC_FRWRD_IPV6_ROUTER_DEFAULT_ACTION_TYPE_to_string(info->type)));
  LOG_CLI((BSL_META_U(unit,
                      "value:")));
  SOC_PPC_FRWRD_IPV6_ROUTER_DEFAULT_ACTION_VAL_print(&(info->value));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_IPV6_ROUTER_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_ROUTER_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "uc_default_action:")));
  SOC_PPC_FRWRD_IPV6_ROUTER_DEFAULT_ACTION_print(&(info->uc_default_action));
  LOG_CLI((BSL_META_U(unit,
                      "mc_default_action:")));
  SOC_PPC_FRWRD_IPV6_ROUTER_DEFAULT_ACTION_print(&(info->mc_default_action));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_IPV6_VRF_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_VRF_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "router_info:")));
  SOC_PPC_FRWRD_IPV6_ROUTER_INFO_print(&(info->router_info));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "subnet:")));
  soc_sand_SAND_PP_IPV6_SUBNET_print(&(info->subnet));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "vrf: %u \n"), info->vrf_ndx));
  LOG_CLI((BSL_META_U(unit,
                      "group:")));
  soc_sand_SAND_PP_IPV6_SUBNET_print(&(info->group));
  if (info->inrif.mask)
  {
    LOG_CLI((BSL_META_U(unit,
                        "in-rif: %u      "),info->inrif.val));
    LOG_CLI((BSL_META_U(unit,
                        "in-rif mask: 0x%03X\n\r"),info->inrif.mask));
  }
  if (info->source.prefix_len) {
      LOG_CLI((BSL_META_U(unit,
                          "source:")));
      soc_sand_SAND_PP_IPV6_SUBNET_print(&(info->source));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_IPV6_VPN_ROUTE_KEY_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_VPN_ROUTE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "subnet:")));
  soc_sand_SAND_PP_IPV6_SUBNET_print(&(info->subnet));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_IPV6_MC_ROUTE_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV6_MC_ROUTE_INFO *info
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

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

