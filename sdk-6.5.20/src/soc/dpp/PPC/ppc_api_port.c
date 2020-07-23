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

#include <soc/dpp/PPC/ppc_api_port.h>





















void
  SOC_PPC_PORT_L2_VPN_INFO_clear(
    SOC_SAND_OUT SOC_PPC_PORT_L2_VPN_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_PORT_L2_VPN_INFO));
  info->enable_l2_vpn = 0;
  info->learn_dest_type = SOC_SAND_PP_NOF_DEST_TYPES;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PPC_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_PORT_INFO));
  info->port_profile = 0;
  info->vlan_domain = 0;
  info->port_type = SOC_SAND_PP_NOF_PORT_L2_TYPES;
  SOC_PPC_ACTION_PROFILE_clear(&(info->initial_action_profile));
  info->is_pbp = 0;
  info->is_n_port = 0;
  info->enable_learning = 0;
  info->enable_outgoing_learning = 1;
  info->enable_same_interfac_filter = 0;
  info->is_learn_ac = 0;
  info->orientation = SOC_SAND_PP_NOF_HUB_SPOKE_ORIENTATIONS;
  info->ether_type_based_profile = 0;
  info->not_authorized_802_1x = 0;
  info->mtu = 0;
  info->mtu_encapsulated = 0;
  info->da_not_found_profile = 0;
  info->dflt_egress_ac = 0;
  info->ac_p2p_to_pbb_vsi_profile = 0;
  info->label_pwe_p2p_vsi_profile = 0;
  info->dflt_port_lif = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_PORT_DIRECTION_to_string(
    SOC_SAND_IN  SOC_PPC_PORT_DIRECTION enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_PORT_DIRECTION_INCOMING:
    str = "incoming";
  break;
  case SOC_PPC_PORT_DIRECTION_OUTGOING:
    str = "outgoing";
  break;
  case SOC_PPC_PORT_DIRECTION_BOTH:
    str = "both";
  break;
  case SOC_PPC_PORT_NOF_DIRECTIONS:
    str = "nof_directions";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_PORT_STP_STATE_to_string(
    SOC_SAND_IN  SOC_PPC_PORT_STP_STATE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_PORT_STP_STATE_BLOCK:
    str = "block";
  break;
  case SOC_PPC_PORT_STP_STATE_LEARN:
    str = "learn";
  break;
  case SOC_PPC_PORT_STP_STATE_FORWARD:
    str = "forward";
  break;
  default:
    str = " Unknown";
  }
  return str;
}


void
  SOC_PPC_PORT_L2_VPN_INFO_print(
    SOC_SAND_IN  SOC_PPC_PORT_L2_VPN_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "enable_l2_vpn: %u\n\r"),info->enable_l2_vpn));
  LOG_CLI((BSL_META_U(unit,
                      "learn_dest_type %s "), soc_sand_SAND_PP_DEST_TYPE_to_string(info->learn_dest_type)));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_PORT_INFO_print(
    SOC_SAND_IN  SOC_PPC_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "port_profile: %u\n\r"),info->port_profile));
  LOG_CLI((BSL_META_U(unit,
                      "tpid_profile: %u\n\r"),info->tpid_profile));
  LOG_CLI((BSL_META_U(unit,
                      "vlan_domain: %u\n\r"),info->vlan_domain));
  LOG_CLI((BSL_META_U(unit,
                      "port_type %s "), soc_sand_SAND_PP_PORT_L2_TYPE_to_string(info->port_type)));
  LOG_CLI((BSL_META_U(unit,
                      "initial_action_profile:")));
  SOC_PPC_ACTION_PROFILE_print(&(info->initial_action_profile));
  LOG_CLI((BSL_META_U(unit,
                      "is_pbp: %u\n\r"),info->is_pbp));
  LOG_CLI((BSL_META_U(unit,
                      "is_n_port: %u\n\r"),info->is_n_port));
  LOG_CLI((BSL_META_U(unit,
                      "enable_learning: %u\n\r"),info->enable_learning));
  LOG_CLI((BSL_META_U(unit,
                      "enable_outgoing_learning: %u\n\r"),info->enable_outgoing_learning));
  LOG_CLI((BSL_META_U(unit,
                      "enable_same_interfac_filter: %u\n\r"),info->enable_same_interfac_filter));
  LOG_CLI((BSL_META_U(unit,
                      "is_learn_ac: %u\n\r"),info->is_learn_ac));
  LOG_CLI((BSL_META_U(unit,
                      "orientation %s "), soc_sand_SAND_PP_HUB_SPOKE_ORIENTATION_to_string(info->orientation)));
  LOG_CLI((BSL_META_U(unit,
                      "ether_type_based_profile: %u\n\r"),info->ether_type_based_profile));
  LOG_CLI((BSL_META_U(unit,
                      "extend_p2p_info:")));
  LOG_CLI((BSL_META_U(unit,
                      "not_authorized_802_1x: %u\n\r"),info->not_authorized_802_1x));
  LOG_CLI((BSL_META_U(unit,
                      "mtu: %u\n\r"),info->mtu));
  LOG_CLI((BSL_META_U(unit,
                      "mtu_encapsulated: %u\n\r"),info->mtu_encapsulated));
  LOG_CLI((BSL_META_U(unit,
                      "da_not_found_profile: %u\n\r"),info->da_not_found_profile));
  LOG_CLI((BSL_META_U(unit,
                      "vlan_translation_profile: %u\n\r"),info->vlan_translation_profile));
  LOG_CLI((BSL_META_U(unit,
                      "tunnel_termination_profile: %u\n\r"),info->tunnel_termination_profile));
  LOG_CLI((BSL_META_U(unit,
                      "ac_p2p_to_pbb_vsi_profile: %u\n\r"),info->ac_p2p_to_pbb_vsi_profile));
  LOG_CLI((BSL_META_U(unit,
                      "label_pwe_p2p_vsi_profile: %u\n\r"),info->label_pwe_p2p_vsi_profile));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

