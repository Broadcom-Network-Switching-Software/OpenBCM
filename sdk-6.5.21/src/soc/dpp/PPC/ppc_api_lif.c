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

#include <soc/dpp/PPC/ppc_api_lif.h>





















void
  SOC_PPC_L2_LIF_AC_KEY_QUALIFIER_clear(
    SOC_SAND_OUT SOC_PPC_L2_LIF_AC_KEY_QUALIFIER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_L2_LIF_AC_KEY_QUALIFIER));
  info->port_profile = 0;
  SOC_PPC_LLP_PARSE_INFO_clear(&info->pkt_parse_info);
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_L2_LIF_IN_VC_RANGE_clear(
    SOC_SAND_OUT SOC_PPC_L2_LIF_IN_VC_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_L2_LIF_IN_VC_RANGE));
  info->first_label = 0;
  info->last_label = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_L2_LIF_DEFAULT_FORWARDING_INFO_clear(
    SOC_SAND_OUT SOC_PPC_L2_LIF_DEFAULT_FORWARDING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_L2_LIF_DEFAULT_FORWARDING_INFO));
  info->default_frwd_type = SOC_PPC_NOF_L2_LIF_DFLT_FRWRD_SRCS;
  SOC_PPC_FRWRD_DECISION_INFO_clear(&(info->default_forwarding));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_L2_LIF_PWE_LEARN_RECORD_clear(
    SOC_SAND_OUT SOC_PPC_L2_LIF_PWE_LEARN_RECORD *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_L2_LIF_PWE_LEARN_RECORD));
  info->enable_learning = 0;
  SOC_PPC_FRWRD_DECISION_INFO_clear(&(info->learn_info));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_L2_LIF_PWE_ADDITIONAL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_L2_LIF_PWE_ADDITIONAL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_L2_LIF_PWE_ADDITIONAL_INFO));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_L2_LIF_PWE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_L2_LIF_PWE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_L2_LIF_PWE_INFO));
  info->vsid = 0;
  SOC_PPC_L2_LIF_DEFAULT_FORWARDING_INFO_clear(&(info->default_frwrd));
  SOC_PPC_L2_LIF_PWE_LEARN_RECORD_clear(&(info->learn_record));
  info->orientation = SOC_SAND_PP_NOF_HUB_SPOKE_ORIENTATIONS;
  info->tpid_profile_index = 0;
  info->cos_profile = 0;
  info->has_cw = 0;
  info->service_type = SOC_PPC_NOF_L2_LIF_PWE_SERVICE_TYPES;
  info->action_profile = 0;
  info->cos_profile = 0;
  info->lif_profile = 0;
  info->oam_valid = FALSE;
  info->protection_pass_value = 0;
  info->protection_pointer = 0;
  info->term_profile = 0;
  info->use_lif = TRUE;
  info->vsi_assignment_mode = 0;
  info->default_forward_profile = 0;
  info->model = SOC_SAND_PP_NOF_MPLS_TUNNEL_MODELS;
  info->has_gal = 0;
  info->lif_table_entry_in_lif_field = 0;
  info->network_group = -1;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_L2_VLAN_RANGE_KEY_clear(
    SOC_SAND_OUT SOC_PPC_L2_VLAN_RANGE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_L2_VLAN_RANGE_KEY));
  info->local_port_ndx = 0;
  info->is_outer = 0;
  info->first_vid = 0;
  info->last_vid = 0;
  info->core_id = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_L2_PORT_VLAN_RANGE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_L2_PORT_VLAN_RANGE_INFO *info
  )
{
  uint32 index;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_L2_PORT_VLAN_RANGE_INFO));

  for (index = 0; index < SOC_PPC_LIF_VLAN_RANGE_MAX_SIZE; index++)
  {
    info->outer_vlan_range[index].first_vid = 4096;
    info->outer_vlan_range[index].last_vid = 4096;
    info->inner_vlan_range[index].first_vid = 4096;
    info->inner_vlan_range[index].last_vid = 4096;    
  }

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_L2_LIF_AC_KEY_clear(
    SOC_SAND_OUT SOC_PPC_L2_LIF_AC_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_L2_LIF_AC_KEY));
  info->key_type = SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN_VLAN;
  info->vlan_domain = 0;
  info->outer_vid = SOC_PPC_LIF_IGNORE_OUTER_VID;
  info->inner_vid = SOC_PPC_LIF_IGNORE_INNER_VID;
  info->raw_key = 0;
  info->outer_pcp = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_L2_LIF_AC_LEARN_RECORD_clear(
    SOC_SAND_OUT SOC_PPC_L2_LIF_AC_LEARN_RECORD *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_L2_LIF_AC_LEARN_RECORD));
  info->learn_type = SOC_PPC_NOF_L2_LIF_AC_LEARN_TYPES;
  SOC_PPC_FRWRD_DECISION_INFO_clear(&(info->learn_info));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_L2_LIF_AC_ING_EDIT_INFO_clear(
    SOC_SAND_OUT SOC_PPC_L2_LIF_AC_ING_EDIT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_L2_LIF_AC_ING_EDIT_INFO));
  info->ing_vlan_edit_profile = 0;
  info->vid = 0;
  info->edit_pcp_profile = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_L2_LIF_AC_INFO_clear(
    SOC_SAND_OUT SOC_PPC_L2_LIF_AC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_L2_LIF_AC_INFO));
  info->service_type = SOC_PPC_NOF_L2_LIF_AC_SERVICE_TYPES;
  info->vsid = 0;
  SOC_PPC_L2_LIF_AC_LEARN_RECORD_clear(&(info->learn_record));
  SOC_PPC_L2_LIF_AC_ING_EDIT_INFO_clear(&(info->ing_edit_info));
  SOC_PPC_L2_LIF_DEFAULT_FORWARDING_INFO_clear(&(info->default_frwrd));
  info->orientation = SOC_SAND_PP_NOF_HUB_SPOKE_ORIENTATIONS;
  info->cos_profile = 0;
  info->l2cp_profile = 0;
  info->default_forward_profile = 0;
  info->lif_profile = 0;
  info->oam_valid = FALSE;
  info->protection_pass_value = 0;
  info->protection_pointer = 0;
  info->use_lif = FALSE;
  info->vsi_assignment_mode = 0;
  info->network_group = -1;
  
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_L2_LIF_AC_MP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_L2_LIF_AC_MP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_L2_LIF_AC_MP_INFO));
  info->mp_level = 0;
  info->is_valid = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_L2_LIF_L2CP_KEY_clear(
    SOC_SAND_OUT SOC_PPC_L2_LIF_L2CP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_L2_LIF_L2CP_KEY));
  info->l2cp_profile = 0;
  info->da_mac_address_lsb = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_L2_LIF_AC_GROUP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_L2_LIF_AC_GROUP_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_L2_LIF_AC_GROUP_INFO));
  for (ind = 0; ind < SOC_PPC_LIF_AC_GROUP_MAX_SIZE; ++ind)
  {
    SOC_PPC_L2_LIF_AC_INFO_clear(&(info->acs_info[ind]));
  }
  info->opcode_id = 0;
  info->nof_lif_entries = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_L2_LIF_ISID_KEY_clear(
    SOC_SAND_OUT SOC_PPC_L2_LIF_ISID_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_L2_LIF_ISID_KEY));
  info->isid_id = 0;
  info->isid_domain = 0;
  info->bsa_nickname = 0;
  info->bsa_nickname_valid = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_L2_LIF_ISID_INFO_clear(
    SOC_SAND_OUT SOC_PPC_L2_LIF_ISID_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_L2_LIF_ISID_INFO));
  SOC_PPC_L2_LIF_DEFAULT_FORWARDING_INFO_clear(&(info->default_frwrd));
  info->learn_enable = 0;
  info->tpid_profile_index = 0;
  info->cos_profile = 0;
  info->service_type = SOC_PPC_NOF_L2_LIF_ISID_SERVICE_TYPES;
  info->default_forward_profile = 0;
  info->oam_valid = FALSE;
  info->orientation = SOC_SAND_PP_NOF_HUB_SPOKE_ORIENTATIONS;
  info->protection_pass_value = 0;
  info->protection_pointer = 0;
  info->use_lif = TRUE;
  info->vsi_assignment_mode = 0;

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_L2_LIF_TRILL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_L2_LIF_TRILL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_L2_LIF_TRILL_INFO));
  SOC_PPC_FRWRD_DECISION_INFO_clear(&(info->learn_info)); 
  
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_L2_LIF_GRE_KEY_clear(
    SOC_SAND_OUT SOC_PPC_L2_LIF_GRE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_L2_LIF_GRE_KEY));
  info->vpn_key = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_L2_LIF_GRE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_L2_LIF_GRE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_L2_LIF_GRE_INFO));
  info->vsid = 0;

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_L2_LIF_VXLAN_KEY_clear(
    SOC_SAND_OUT SOC_PPC_L2_LIF_VXLAN_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_L2_LIF_VXLAN_KEY));
  info->vpn_key = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_L2_LIF_VXLAN_INFO_clear(
    SOC_SAND_OUT SOC_PPC_L2_LIF_VXLAN_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_L2_LIF_VXLAN_INFO));
  info->vsid = 0;

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY_clear(
    SOC_SAND_OUT SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY));
  info->da_type = SOC_SAND_PP_NOF_ETHERNET_DA_TYPES;
  info->lif_default_forward_profile = 0;
  info->port_da_not_found_profile = 0;
  
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_L2_LIF_DEFAULT_FRWRD_ACTION_clear(
    SOC_SAND_OUT SOC_PPC_L2_LIF_DEFAULT_FRWRD_ACTION *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_L2_LIF_DEFAULT_FRWRD_ACTION));
  info->add_vsi = TRUE;
  SOC_PPC_FRWRD_DECISION_INFO_clear(&info->frwrd_dest);
  
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_L2_LIF_EXTENDER_KEY_clear(
    SOC_SAND_OUT SOC_PPC_L2_LIF_EXTENDER_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_L2_LIF_EXTENDER_KEY));
  
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_L2_LIF_EXTENDER_INFO_clear(
    SOC_SAND_OUT SOC_PPC_L2_LIF_EXTENDER_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_L2_LIF_EXTENDER_INFO));
  SOC_PPC_L2_LIF_AC_LEARN_RECORD_clear(&(info->learn_record));
  SOC_PPC_L2_LIF_DEFAULT_FORWARDING_INFO_clear(&(info->default_frwrd));
  
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT:
    str = "port";
  break;  
  case SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN:
    str = "port_vlan";
  break;
  case SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN_VLAN:
    str = "port_vlan_vlan";
    break;
 case SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN_ETH:
    str = "port_vlan_eth";
    break;
 case SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN_VLAN_ETH:
    str = "port_vlan_vlan_eth";
    break;
  case SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_RAW:
    str = "raw";
    break;
  case SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_RAW_VLAN:
    str = "raw_vlan";
    break;
  case SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_RAW_VLAN_VLAN:
    str = "raw_vlan_vlan";
    break;
  case SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_COMP_VLAN:
    str = "comp_vlan";
    break;
  case SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_COMP_VLAN_COMP_VLAN:
    str = "comp_vlan_comp_vlan";
    break;
  case SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_INITIAL_VLAN:
    str = "port_initial_vlan";
    break;
  case SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_TRILL_DESIGNATED_VLAN:
    str = "port_designated_vlan";
	break;
  case SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_FLEXIBLE_Q_IN_Q_PORT_INITIAL_VID:
    str = "flexible_q_in_q_single_tag";
    break; 
  case SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_FLEXIBLE_Q_IN_Q_PORT_INITIAL_VID_VID:
    str = "flexible_q_in_q_double_tag";
    break; 
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_L2_LIF_DFLT_FRWRD_SRC_to_string(
    SOC_SAND_IN  SOC_PPC_L2_LIF_DFLT_FRWRD_SRC enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_L2_LIF_DFLT_FRWRD_SRC_VSI:
    str = "vsi";
  break;
  case SOC_PPC_L2_LIF_DFLT_FRWRD_SRC_LIF:
    str = "lif";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_L2_LIF_AC_SERVICE_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_SERVICE_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_L2_LIF_AC_SERVICE_TYPE_MP:
    str = "mp";
  break;
  case SOC_PPC_L2_LIF_AC_SERVICE_TYPE_AC2AC:
    str = "ac2ac";
  break;
  case SOC_PPC_L2_LIF_AC_SERVICE_TYPE_AC2PWE:
    str = "ac2pwe";
  break;
  case SOC_PPC_L2_LIF_AC_SERVICE_TYPE_AC2PBB:
    str = "ac2pbb";
  break;
  default:
    str = " NA";
  }
  return str;
}

const char*
  SOC_PPC_L2_LIF_PWE_SERVICE_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_L2_LIF_PWE_SERVICE_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_L2_LIF_PWE_SERVICE_TYPE_MP:
    str = "mp";
  break;
  case SOC_PPC_L2_LIF_PWE_SERVICE_TYPE_P2P:
    str = "p2p";
  break;
  case SOC_PPC_L2_LIF_PWE_SERVICE_TYPE_P2P_CUSTOM:
    str = "p2p_custom";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_L2_LIF_ISID_SERVICE_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_L2_LIF_ISID_SERVICE_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_L2_LIF_ISID_SERVICE_TYPE_MP:
    str = "mp";
  break;
  case SOC_PPC_L2_LIF_ISID_SERVICE_TYPE_P2P:
    str = "p2p";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_L2_LIF_AC_LEARN_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_LEARN_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_L2_LIF_AC_LEARN_DISABLE:
    str = "learn_disable";
  break;
  case SOC_PPC_L2_LIF_AC_LEARN_SYS_PORT:
    str = "learn_sys_port";
  break;
  case SOC_PPC_L2_LIF_AC_LEARN_INFO:
    str = "learn_info";
  break;
  case SOC_PPC_L2_LIF_AC_LEARN_SYS_PORT_LIF:
    str = "learn_sys_port_lif";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_L2_LIF_VSI_ASSIGNMENT_MODE_to_string(
    SOC_SAND_IN  uint32 val
  )
{
  const char* str = NULL;
  switch(val)
  {
  case SOC_PPC_VSI_EQ_IN_VID:
    str = "vsi_equals_vid";
  break;
  case SOC_PPC_VSI_EQ_VSI_BASE:
    str = "vsi_equals_vsi_lif";
  break;
  case SOC_PPC_VSI_EQ_IN_LIF:
    str = "vsi_equals_lif_index";
  break;
  case SOC_PPC_VSI_EQ_IN_VID_PLUS_VSI_BASE:
      str = "vsi_equals_lif_index_plus_vid";
      break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_L2_LIF_L2CP_HANDLE_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_L2_LIF_L2CP_HANDLE_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_L2_LIF_L2CP_HANDLE_TYPE_NORMAL:
    str = "normal";
  break;
  case SOC_PPC_L2_LIF_L2CP_HANDLE_TYPE_TUNNEL:
    str = "tunnel";
  break;
  case SOC_PPC_L2_LIF_L2CP_HANDLE_TYPE_PEER:
    str = "peer";
  break;
  case SOC_PPC_L2_LIF_L2CP_HANDLE_TYPE_DROP:
    str = "drop";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_PPC_L2_LIF_AC_KEY_QUALIFIER_print(
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY_QUALIFIER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "port_profile: %u\n\r"),info->port_profile));
  SOC_PPC_LLP_PARSE_INFO_print(&(info->pkt_parse_info));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_L2_LIF_IN_VC_RANGE_print(
    SOC_SAND_IN  SOC_PPC_L2_LIF_IN_VC_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "first_label: %u\n\r"),info->first_label));
  LOG_CLI((BSL_META_U(unit,
                      "last_label: %u\n\r"),info->last_label));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_L2_LIF_DEFAULT_FORWARDING_INFO_print(
    SOC_SAND_IN  SOC_PPC_L2_LIF_DEFAULT_FORWARDING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "default_frwd_type %s "), SOC_PPC_L2_LIF_DFLT_FRWRD_SRC_to_string(info->default_frwd_type)));
  if (info->default_frwd_type == SOC_PPC_L2_LIF_DFLT_FRWRD_SRC_LIF)
  {
    LOG_CLI((BSL_META_U(unit,
                        "default_forwarding:")));
    SOC_PPC_FRWRD_DECISION_INFO_print(&(info->default_forwarding));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_L2_LIF_PWE_LEARN_RECORD_print(
    SOC_SAND_IN  SOC_PPC_L2_LIF_PWE_LEARN_RECORD *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "enable_learning: %u\n\r"),info->enable_learning));
  LOG_CLI((BSL_META_U(unit,
                      "learn_info:")));
  SOC_PPC_FRWRD_DECISION_INFO_print(&(info->learn_info));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_L2_LIF_PWE_INFO_print(
    SOC_SAND_IN  SOC_PPC_L2_LIF_PWE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "service_type %s \n\r"), SOC_PPC_L2_LIF_PWE_SERVICE_TYPE_to_string(info->service_type)));
  LOG_CLI((BSL_META_U(unit,
                      "vsid: %u\n\r"),info->vsid));
  if (info->service_type == SOC_PPC_L2_LIF_PWE_SERVICE_TYPE_MP)
  {
	LOG_CLI((BSL_META_U(unit,
                            "info->default_forward_profile: %u\n\r"),info->default_forward_profile));
    LOG_CLI((BSL_META_U(unit,
                        "learn_record:")));
    SOC_PPC_L2_LIF_PWE_LEARN_RECORD_print(&(info->learn_record));
    LOG_CLI((BSL_META_U(unit,
                        "\n\rdefault_frwd_type %s \n\r"), SOC_PPC_L2_LIF_DFLT_FRWRD_SRC_to_string(info->default_frwrd.default_frwd_type)));
  }
  else
  {
	LOG_CLI((BSL_META_U(unit,
                            "use_lif: %u\n\r"),info->use_lif));
    LOG_CLI((BSL_META_U(unit,
                        "default_frwrd:")));
    SOC_PPC_L2_LIF_DEFAULT_FORWARDING_INFO_print(&(info->default_frwrd));
	LOG_CLI((BSL_META_U(unit,
                            "model: %s\n\r"),soc_sand_SAND_PP_MPLS_TUNNEL_MODEL_to_string(info->model)));
  }
  if (info->network_group != -1) {
      LOG_CLI((BSL_META_U(unit,
                          "network group: %d \n\r"), info->network_group));
  } else {
      LOG_CLI((BSL_META_U(unit,
                          "orientation %s \n\r"), soc_sand_SAND_PP_HUB_SPOKE_ORIENTATION_to_string(info->orientation)));
  }
  LOG_CLI((BSL_META_U(unit,
                      "action_profile_index: %u\n\r"),info->action_profile));
  LOG_CLI((BSL_META_U(unit,
                      "termination_profile: %u\n\r"),info->term_profile));
  LOG_CLI((BSL_META_U(unit,
                      "tpid_profile_index: %u\n\r"),info->tpid_profile_index));
  LOG_CLI((BSL_META_U(unit,
                      "cos_profile: %u\n\r"),info->cos_profile));
  LOG_CLI((BSL_META_U(unit,
                      "vsi_assignment_mode: %s\n\r"),SOC_PPC_L2_LIF_VSI_ASSIGNMENT_MODE_to_string(info->vsi_assignment_mode)));
  LOG_CLI((BSL_META_U(unit,
                      "oam_valid: %u\n\r"),info->oam_valid));
  LOG_CLI((BSL_META_U(unit,
                      "oam_instance: %u\n\r"),info->protection_pointer));
  LOG_CLI((BSL_META_U(unit,
                      "protection_pass_val: %u\n\r"),info->protection_pass_value));
  LOG_CLI((BSL_META_U(unit,
                      "has_cw: %u\n\r"),info->has_cw));
  LOG_CLI((BSL_META_U(unit,
                      "has_gal: %u\n\r"),info->has_gal));
  if (info->has_gal) {
      LOG_CLI((BSL_META_U(unit,
                          "lif_table_entry_in_lif_field: %u\n\r"),info->lif_table_entry_in_lif_field));
  }
  
  LOG_CLI((BSL_META_U(unit,
                      "in_lif_profile: %u\n\r"),info->lif_profile));
  
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_L2_VLAN_RANGE_KEY_print(
    SOC_SAND_IN  SOC_PPC_L2_VLAN_RANGE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "local_port_ndx: %u\n\r"),info->local_port_ndx));
  LOG_CLI((BSL_META_U(unit,
                      "is_outer: %u\n\r"), info->is_outer));
  LOG_CLI((BSL_META_U(unit,
                      "first_vid: %u\n\r"),info->first_vid));
  LOG_CLI((BSL_META_U(unit,
                      "last_vid: %u\n\r"),info->last_vid));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_L2_PORT_VLAN_RANGE_INFO_print(
    SOC_SAND_IN  SOC_PPC_L2_PORT_VLAN_RANGE_INFO *info
  )
{
  uint32 index;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (index = 0; index < SOC_PPC_LIF_VLAN_RANGE_MAX_SIZE; index++)
  {
    LOG_CLI((BSL_META_U(unit,
                        "range %d:\n\r"), index));
    LOG_CLI((BSL_META_U(unit,
                        "first_vid: %u\n\r"),info->outer_vlan_range[index].first_vid));
    LOG_CLI((BSL_META_U(unit,
                        "last_vid: %u\n\r"), info->outer_vlan_range[index].last_vid));
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_PPC_L2_LIF_AC_KEY_print(
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "key_type: %s\n\r"),SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_to_string(info->key_type)));

  if (info->key_type != SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_RAW)
  {
    LOG_CLI((BSL_META_U(unit,
                        "vlan_domain: %u\n\r"),info->vlan_domain));
    if (info->outer_vid == SOC_PPC_LIF_IGNORE_OUTER_VID)
    {
      LOG_CLI((BSL_META_U(unit,
                          "outer_vid: ignored\n\r")));
    }
    else
    {
      LOG_CLI((BSL_META_U(unit,
                          "outer_vid: %u\n\r"),info->outer_vid));
    }
    if (info->inner_vid == SOC_PPC_LIF_IGNORE_INNER_VID)
    {
      LOG_CLI((BSL_META_U(unit,
                          "inner_vid: ignored\n\r")));
    }
    else
    {
      LOG_CLI((BSL_META_U(unit,
                          "inner_vid: %u\n\r"),info->inner_vid));
    }
    if (info->key_type != SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_PCP_VLAN
        && info->key_type != SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_PCP_VLAN_VLAN
        && info->key_type != SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_TUNNEL_PCP_ETHER_TYPE_COMP_VLAN_COMP_VLAN)
    {
      LOG_CLI((BSL_META_U(unit,
                          "outer_pcp_vid: ignored\n\r")));
    }
    else
    {
      LOG_CLI((BSL_META_U(unit,
                          "outer_pcp: %u\n\r"),info->outer_pcp));
    }

    if (info->key_type != SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_TUNNEL_PCP_ETHER_TYPE_COMP_VLAN_COMP_VLAN)
    {
      LOG_CLI((BSL_META_U(unit,
                          "ethertype: ignored\n\r")));
    }
    else
    {
      LOG_CLI((BSL_META_U(unit,
                          "ethertype: %u\n\r"),info->ethertype));
    }

    if (info->key_type != SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_TUNNEL 
        && info->key_type != SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_TUNNEL_COMP_VLAN
        && info->key_type != SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_TUNNEL_COMP_VLAN_COMP_VLAN
        && info->key_type != SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_TUNNEL_PCP_ETHER_TYPE_COMP_VLAN_COMP_VLAN) {
      LOG_CLI((BSL_META_U(unit,
                          "tunnel: ignored\n\r")));
    }
    else
    {
      if (info->tunnel == SOC_PPC_LIF_IGNORE_TUNNEL_VID)
      {
        LOG_CLI((BSL_META_U(unit,
                            "tunnel: ignored\n\r")));
      }
      else
      {
        LOG_CLI((BSL_META_U(unit,
                            "tunnel: %u\n\r"),info->tunnel));
      }
    }
  }
  else
  {
    LOG_CLI((BSL_META_U(unit,
                        "raw_key: %u\n\r"),info->raw_key));
  }
  
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_L2_LIF_AC_LEARN_RECORD_print(
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_LEARN_RECORD *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "learn_type %s "), SOC_PPC_L2_LIF_AC_LEARN_TYPE_to_string(info->learn_type)));
  if (info->learn_type == SOC_PPC_L2_LIF_AC_LEARN_INFO)
  {
    LOG_CLI((BSL_META_U(unit,
                        "learn_info:")));
    SOC_PPC_FRWRD_DECISION_INFO_print(&(info->learn_info));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_L2_LIF_AC_ING_EDIT_INFO_print(
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_ING_EDIT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "ing_vlan_edit_profile: %u\n\r"),info->ing_vlan_edit_profile));
  LOG_CLI((BSL_META_U(unit,
                      "vid_1: %u\n\r"),info->vid));
  LOG_CLI((BSL_META_U(unit,
                      "vid_2: %u\n\r"),info->vid2));
  LOG_CLI((BSL_META_U(unit,
                      "edit_pcp_profile: %u\n\r"),info->edit_pcp_profile));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_L2_LIF_AC_INFO_print(
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "service_type %s "), SOC_PPC_L2_LIF_AC_SERVICE_TYPE_to_string(info->service_type)));
  LOG_CLI((BSL_META_U(unit,
                      "vsid: %u\n\r"),info->vsid));
  if (info->service_type ==SOC_PPC_L2_LIF_AC_SERVICE_TYPE_MP)
  {
	LOG_CLI((BSL_META_U(unit,
                            "default_forward_profile: %u\n\r"),info->default_forward_profile));
    LOG_CLI((BSL_META_U(unit,
                        "learn_record:")));
    SOC_PPC_L2_LIF_AC_LEARN_RECORD_print(&(info->learn_record));
    LOG_CLI((BSL_META_U(unit,
                        "default_frwd_type %s \n\r"), SOC_PPC_L2_LIF_DFLT_FRWRD_SRC_to_string(info->default_frwrd.default_frwd_type)));
  }
  else
  {
    SOC_PPC_L2_LIF_DEFAULT_FORWARDING_INFO_print(&(info->default_frwrd));
  }
  LOG_CLI((BSL_META_U(unit,
                      "ing_edit_info:")));
  SOC_PPC_L2_LIF_AC_ING_EDIT_INFO_print(&(info->ing_edit_info));
  if (info->network_group != -1) {
      LOG_CLI((BSL_META_U(unit,
                          "network group: %d \n\r"), info->network_group));
  } else {
      LOG_CLI((BSL_META_U(unit,
                      "orientation %s "), soc_sand_SAND_PP_HUB_SPOKE_ORIENTATION_to_string(info->orientation)));
  }
  LOG_CLI((BSL_META_U(unit,
                      "cos_profile: %u\n\r"),info->cos_profile));
  LOG_CLI((BSL_META_U(unit,
                      "l2cp_profile: %u\n\r"),info->l2cp_profile));
  LOG_CLI((BSL_META_U(unit,
                      "oam_valid: %u\n\r"),info->oam_valid));
  LOG_CLI((BSL_META_U(unit,
                      "oam_instance: %u\n\r"),info->protection_pointer));
  LOG_CLI((BSL_META_U(unit,
                      "protection_pass_val: %u\n\r"),info->protection_pass_value));
  LOG_CLI((BSL_META_U(unit,
                      "use_lif: %u\n\r"),info->use_lif));
  if (info->vsi_assignment_mode == SOC_PPC_VSI_EQ_IN_VID) {
      LOG_CLI((BSL_META_U(unit,
                      "vsi_assignment_mode: VSI Equal In-VLAN\n\r")));
  } else {
      LOG_CLI((BSL_META_U(unit,
                      "vsi_assignment_mode: %u\n\r"),info->vsi_assignment_mode));
  }
  

  
  LOG_CLI((BSL_META_U(unit,
                      "lif_profile: %u\n\r"),info->lif_profile));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_L2_LIF_AC_MP_INFO_print(
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_MP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "mp_level: %u\n\r"),info->mp_level));
  LOG_CLI((BSL_META_U(unit,
                      "is_valid: %u\n\r"),info->is_valid));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_L2_LIF_L2CP_KEY_print(
    SOC_SAND_IN  SOC_PPC_L2_LIF_L2CP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "l2cp_profile: %u\n\r"),info->l2cp_profile));
  LOG_CLI((BSL_META_U(unit,
                      "da_mac_address_lsb: %u\n\r"),info->da_mac_address_lsb));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_L2_LIF_AC_GROUP_INFO_print(
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_GROUP_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < info->nof_lif_entries; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "acs_info[%u]:"),ind));
    SOC_PPC_L2_LIF_AC_INFO_print(&(info->acs_info[ind]));
  }
  LOG_CLI((BSL_META_U(unit,
                      "opcode_id: %u\n\r"),info->opcode_id));
  LOG_CLI((BSL_META_U(unit,
                      "nof_lif_entries: %u\n\r"),info->nof_lif_entries));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_L2_LIF_ISID_KEY_print(
    SOC_SAND_IN  SOC_PPC_L2_LIF_ISID_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "isid_id: %u\n\r"),info->isid_id));
  LOG_CLI((BSL_META_U(unit,
                      "isid_domain: %u\n\r"),info->isid_domain));
  LOG_CLI((BSL_META_U(unit,
                      "bsa_nickname: %u\n\r"),info->bsa_nickname));
  LOG_CLI((BSL_META_U(unit,
                      "bsa_nickname_valid: %u\n\r"),info->bsa_nickname_valid));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_L2_LIF_ISID_INFO_print(
    SOC_SAND_IN  SOC_PPC_L2_LIF_ISID_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "default_frwrd:")));
  SOC_PPC_L2_LIF_DEFAULT_FORWARDING_INFO_print(&(info->default_frwrd));
  LOG_CLI((BSL_META_U(unit,
                      "default_forward_profile: %u\n\r"),info->default_forward_profile));
  LOG_CLI((BSL_META_U(unit,
                      "learn_enable: %u\n\r"),info->learn_enable));
  LOG_CLI((BSL_META_U(unit,
                      "tpid_profile_index: %u\n\r"),info->tpid_profile_index));
  LOG_CLI((BSL_META_U(unit,
                      "cos_profile: %u\n\r"),info->cos_profile));
  LOG_CLI((BSL_META_U(unit,
                      "service_type: %s\n\r"), SOC_PPC_L2_LIF_ISID_SERVICE_TYPE_to_string(info->service_type)));
  LOG_CLI((BSL_META_U(unit,
                      "orientation %s \n\r"), soc_sand_SAND_PP_HUB_SPOKE_ORIENTATION_to_string(info->orientation)));
  LOG_CLI((BSL_META_U(unit,
                      "oam_valid: %u\n\r"),info->oam_valid));
  LOG_CLI((BSL_META_U(unit,
                      "protection_pointer: %u\n\r"),info->protection_pointer));
  LOG_CLI((BSL_META_U(unit,
                      "protection_pass_val: %u\n\r"),info->protection_pass_value));
  LOG_CLI((BSL_META_U(unit,
                      "use_lif: %u\n\r"),info->use_lif));
  LOG_CLI((BSL_META_U(unit,
                      "vsi_assignment_mode: %u\n\r"),info->vsi_assignment_mode));
  LOG_CLI((BSL_META_U(unit,
                      "inlif_profile: %u\n\r"),info->lif_profile));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_L2_LIF_TRILL_INFO_print(
    SOC_SAND_IN  SOC_PPC_L2_LIF_TRILL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);


  SOC_PPC_FRWRD_DECISION_INFO_print(&(info->learn_info)); 

  LOG_CLI((BSL_META_U(unit,
                      "learn_enable: %u\n\r"),info->learn_enable));
  LOG_CLI((BSL_META_U(unit,
                      "vsi-id: %u\n\r"),info->vsi_id));
  LOG_CLI((BSL_META_U(unit,
                      "protection_pass_value: %u\n\r"),info->protection_pass_value));
  LOG_CLI((BSL_META_U(unit,
                      "protection_pointer: %u\n\r"),info->protection_pointer));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_L2_LIF_GRE_KEY_print(
    SOC_SAND_IN  SOC_PPC_L2_LIF_GRE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "vpn_key: %u\n\r"),info->vpn_key));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_L2_LIF_GRE_INFO_print(
    SOC_SAND_IN  SOC_PPC_L2_LIF_GRE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);


  LOG_CLI((BSL_META_U(unit,
                      "vsid: %u\n\r"),info->vsid));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_PPC_L2_LIF_VXLAN_KEY_print(
    SOC_SAND_IN  SOC_PPC_L2_LIF_VXLAN_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "vpn_key: %u\n\r"),info->vpn_key));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_L2_LIF_VXLAN_INFO_print(
    SOC_SAND_IN  SOC_PPC_L2_LIF_VXLAN_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);


  LOG_CLI((BSL_META_U(unit,
                      "vsid: %u\n\r"),info->vsid));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY_print(
    SOC_SAND_IN  SOC_PPC_L2_LIF_DEFAULT_FRWRD_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "default_frwrd key:")));
  LOG_CLI((BSL_META_U(unit,
                      "port_da_not_found_profile: %u\n\r"),info->port_da_not_found_profile));
  LOG_CLI((BSL_META_U(unit,
                      "da type: %s\n\r"), soc_sand_SAND_PP_ETHERNET_DA_TYPE_to_string(info->da_type)));
  LOG_CLI((BSL_META_U(unit,
                      "lif_default_forward_profile: %u\n\r"),info->lif_default_forward_profile));
  
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_L2_LIF_DEFAULT_FRWRD_ACTION_print(
    SOC_SAND_IN  SOC_PPC_L2_LIF_DEFAULT_FRWRD_ACTION *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "default_frwrd action:")));
  SOC_PPC_FRWRD_DECISION_INFO_print(&info->frwrd_dest);
  LOG_CLI((BSL_META_U(unit,
                      "offset: %u\n\r"),info->offset));
  LOG_CLI((BSL_META_U(unit,
                      "add_vsi: %u\n\r"),info->add_vsi));
  
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

int
  SOC_PPC_OCC_PROFILE_USAGE_print(
    SOC_SAND_IN int unit,
    SOC_SAND_IN SOC_OCC_MGMT_TYPE profile_type,
    SOC_SAND_IN int size
  ){
    int i, rv = SOC_SAND_OK;

    char** apps = sal_alloc(sizeof(char*) * (size+1), "print apps");
    rv = arad_pp_occ_mgmt_diag_info_get(unit, profile_type, apps);
    if (soc_sand_get_error_code_from_error_word(rv) != SOC_SAND_OK) 
    { 
        rv = SOC_SAND_ERR;
        goto exit;
    }

    LOG_CLI((BSL_META_U(unit, "%s\n\r"), apps[0]));
    LOG_CLI((BSL_META_U(unit, "%.*s\n\r"), (int)(sal_strlen(apps[0])), "======================")); 
    for (i=1; i <= size; i++) {
        LOG_CLI((BSL_META_U(unit, "\tBit %d : %s\n\r"), i-1, apps[i]));
    }

exit:
    sal_free(apps);
    return rv;
}

int
  SOC_PPC_OCC_ALL_PROFILES_print(
     SOC_SAND_IN int unit
  ){
    int rv;

    rv = SOC_PPC_OCC_PROFILE_USAGE_print(unit, SOC_OCC_MGMT_TYPE_INLIF, SOC_OCC_MGMT_INLIF_SIZE);
    if (soc_sand_get_error_code_from_error_word(rv) != SOC_SAND_OK) { 
        return SOC_SAND_ERR; 
    }

    if (SOC_IS_JERICHO(unit)) {
        rv = SOC_PPC_OCC_PROFILE_USAGE_print(unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_SIZE(unit));
        if (soc_sand_get_error_code_from_error_word(rv) != SOC_SAND_OK) { 
            return SOC_SAND_ERR; 
        }

        
        
    }

    return SOC_SAND_OK;
}

void
  SOC_PPC_L2_LIF_EXTENDER_KEY_print(
    SOC_SAND_IN  SOC_PPC_L2_LIF_EXTENDER_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "extender_port_vid: %u\n\r"),info->extender_port_vid));
  LOG_CLI((BSL_META_U(unit,
                      "vid: %u\n\r"),info->vid));
  LOG_CLI((BSL_META_U(unit,
                      "name_space: %u\n\r"),info->name_space));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_PPC_L2_LIF_EXTENDER_INFO_print(
    SOC_SAND_IN  SOC_PPC_L2_LIF_EXTENDER_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "service_type: %u\n\r"),info->service_type));
  LOG_CLI((BSL_META_U(unit,
                      "vsid: %u\n\r"),info->vsid));
  SOC_PPC_L2_LIF_AC_LEARN_RECORD_print(&(info->learn_record)); 
  SOC_PPC_L2_LIF_DEFAULT_FORWARDING_INFO_print(&(info->default_frwrd)); 
  LOG_CLI((BSL_META_U(unit,
                      "cos_profile: %u\n\r"),info->cos_profile));
  LOG_CLI((BSL_META_U(unit,
                      "lif_profile: %u\n\r"),info->lif_profile));
  LOG_CLI((BSL_META_U(unit,
                      "protection_pointer: %u\n\r"),info->protection_pointer));
  LOG_CLI((BSL_META_U(unit,
                      "protection_pass_value: %u\n\r"),info->protection_pass_value));
  LOG_CLI((BSL_META_U(unit,
                      "oam_valid: %u\n\r"),info->oam_valid));
  LOG_CLI((BSL_META_U(unit,
                      "vsi_assignment_mode: %u\n\r"),info->vsi_assignment_mode));
  LOG_CLI((BSL_META_U(unit,
                      "default_forward_profile: %u\n\r"),info->default_forward_profile));
  LOG_CLI((BSL_META_U(unit,
                      "use_lif: %u\n\r"),info->use_lif));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

