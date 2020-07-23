/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/*
 * General include file for reference design.
 */
#include <appl/diag/dpp/ref_sys.h>
#include <bcm_app/dpp/../H/usrApp.h>

/*
 * Utilities include file.
 */

#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <appl/diag/dpp/utils_defi.h>
#include <appl/dpp/UserInterface/ui_defi.h>

#include <soc/dpp/T20E/t20e_api_general.h>
#include <soc/dpp/T20E/t20e_api_lif.h>
#include <soc/dpp/T20E/t20e_api_tbl_access.h>
#include <soc/dpp/T20E/t20e_tbl_access.h>
#include <soc/dpp/T20E/t20e_lem_access.h>
#include <soc/dpp/T20E/t20e_ac_access.h>
#include <soc/dpp/T20E/t20e_ac_access.h>
#include <soc/dpp/T20E/t20e_reg_access.h>
#include <soc/dpp/T20E/t20e_api_hw_if.h>
#include <soc/dpp/T20E/t20e_api_mgmt.h>
#include <soc/dpp/T20E/t20e_api_diagnostics.h>

#include <appl/dpp/UserInterface/ui_pure_defi_t20e_acc.h>

#define UI_T20E_ACC_PP 1
#define UI_T20E_ACC_MACT 1
#define UI_T20E_REG_ACCESS 1
#define UI_T20E_HW_IF  1
#define UI_T20E_MGMT   1
#define UI_T20E_DIAGNOSTICS   1

static
  uint32
    Default_unit = 0;

void
  t20e_set_default_unit(uint32 dev_id)
{
  Default_unit = dev_id;
}

uint32
  t20e_get_default_unit()
{
  return Default_unit;
}



void
  T20E_PP_LEM_TBL_KEY_INFO_clear(
    SOC_SAND_OUT T20E_PP_LEM_TBL_KEY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_LEM_TBL_KEY_INFO));
  info->offset = 0;
  T20E_FRWRD_ILM_KEY_clear(&(info->ilm));
exit:
  return;
}

void
  T20E_PP_LLVP_CLASSIFICATION_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_LLVP_CLASSIFICATION_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_LLVP_CLASSIFICATION_TBL_DATA));
  info->incoming_vid_exist = 0;
  info->incoming_tag_exist = 0;
  info->incoming_s_tag_exist = 0;
  info->acceptable_frame_type_action = 0;
  info->tag_edit_profile = 0;
  info->ac_key_encoding = 0;
exit:
  return;
}
void
  T20E_PP_IPV4_SUBNET_CAMI_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_IPV4_SUBNET_CAMI_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_IPV4_SUBNET_CAMI_TBL_DATA));
  info->ipv4_subnet_bmap = 0;
exit:
  return;
}
void
  T20E_PP_IPV4_SUBNET_TABLE_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_IPV4_SUBNET_TABLE_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_IPV4_SUBNET_TABLE_TBL_DATA));
  info->ipv4_subnet_vid = 0;
  info->ipv4_subnet_vid_valid = 0;
  info->ipv4_subnet_tc = 0;
  info->ipv4_subnet_tc_valid = 0;
exit:
  return;
}
void
  T20E_PP_PORT_PROTOCOL_CAMI_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_PORT_PROTOCOL_CAMI_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_PORT_PROTOCOL_CAMI_TBL_DATA));
  info->ethernet_type_bmap = 0;
exit:
  return;
}
void
  T20E_PP_PORT_PROTOCOL_TO_VID_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_PORT_PROTOCOL_TO_VID_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_PORT_PROTOCOL_TO_VID_TBL_DATA));
  info->vid = 0;
  info->vid_valid = 0;
  info->traffic_class = 0;
  info->traffic_class_valid = 0;
exit:
  return;
}
void
  T20E_PP_VLAN_PORT_MEMBERSHIP_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_VLAN_PORT_MEMBERSHIP_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_VLAN_PORT_MEMBERSHIP_TBL_DATA));
  info->port_bmap_membership = 0;
exit:
  return;
}
void
  T20E_PP_RESERVED_MC_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_RESERVED_MC_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_RESERVED_MC_TBL_DATA));
  info->reserved_mc_trap_code = 0;
exit:
  return;
}
void
  T20E_PP_PCP_DECODING_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_PCP_DECODING_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_PCP_DECODING_TBL_DATA));
  info->traffic_class = 0;
  info->drop_eligible = 0;
exit:
  return;
}
void
  T20E_PP_TOS_TO_TC_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_TOS_TO_TC_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_TOS_TO_TC_TBL_DATA));
  info->traffic_class = 0;
  info->traffic_class_valid = 0;
exit:
  return;
}
void
  T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_DATA));
  info->traffic_class = 0;
  info->traffic_class_valid = 0;
exit:
  return;
}
void
  T20E_PP_DROP_PRECEDENCE_MAP_TC_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_DROP_PRECEDENCE_MAP_TC_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_DROP_PRECEDENCE_MAP_TC_TBL_DATA));
  info->drop_precedence = 0;
exit:
  return;
}
void
  T20E_PP_AC_OFFSET_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_AC_OFFSET_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_AC_OFFSET_TBL_DATA));
  info->ac_offset = 0;
exit:
  return;
}
void
  T20E_PP_COS_PROFILE_DECODING_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_COS_PROFILE_DECODING_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_COS_PROFILE_DECODING_TBL_DATA));
  info->force_or_map = 0;
  info->map_profile = 0;
  info->traffic_class = 0;
  info->drop_precedence = 0;
exit:
  return;
}
void
  T20E_PP_COS_PROFILE_MAPPING_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_COS_PROFILE_MAPPING_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_COS_PROFILE_MAPPING_TBL_DATA));
  info->traffic_class_ow = 0;
  info->traffic_class = 0;
  info->drop_precedence_ow = 0;
  info->drop_precedence = 0;
exit:
  return;
}
void
  T20E_PP_STP_STATE_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_STP_STATE_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_STP_STATE_TBL_DATA));
  info->stp_state = 0;
exit:
  return;
}
void
  T20E_PP_PCP_DEI_MAP_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_PCP_DEI_MAP_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_PCP_DEI_MAP_TBL_DATA));
  info->pcp = 0;
  info->dei = 0;
exit:
  return;
}
void
  T20E_PP_IVEC_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_IVEC_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_IVEC_TBL_DATA));
  info->command = 0;
exit:
  return;
}
void
  T20E_PP_ACTION_PROFILE_MPLS_VALUE_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_ACTION_PROFILE_MPLS_VALUE_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_ACTION_PROFILE_MPLS_VALUE_TBL_DATA));
  info->action_not_bos_frwrd = 0;
  info->action_not_bos_snoop = 0;
  info->action_bos_frwrd = 0;
  info->action_bos_snoop = 0;
exit:
  return;
}
void
  T20E_PP_MPLS_COS_PROFILE_DECODING_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_MPLS_COS_PROFILE_DECODING_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_MPLS_COS_PROFILE_DECODING_TBL_DATA));
  info->force_or_map = 0;
  info->map_profile = 0;
  info->traffic_class = 0;
  info->drop_precedence = 0;
exit:
  return;
}
void
  T20E_PP_MPLS_COS_MAPPING_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_MPLS_COS_MAPPING_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_MPLS_COS_MAPPING_TBL_DATA));
  info->traffic_class_ow = 0;
  info->traffic_class = 0;
  info->drop_precedence_ow = 0;
  info->drop_precedence = 0;
exit:
  return;
}
void
  T20E_PP_MPLS_TERMINATION_COS_MAPPING_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_MPLS_TERMINATION_COS_MAPPING_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_MPLS_TERMINATION_COS_MAPPING_TBL_DATA));
  info->traffic_class_ow = 0;
  info->traffic_class = 0;
  info->drop_precedence_ow = 0;
  info->drop_precedence = 0;
exit:
  return;
}
void
  T20E_PP_MPLS_TUNNEL_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_MPLS_TUNNEL_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_MPLS_TUNNEL_TBL_DATA));
  info->termination_valid = 0;
  info->counter_valid = 0;
  info->policer_valid = 0;
  info->stat_tag_valid = 0;
exit:
  return;
}
void
  T20E_PP_PROTECTION_INSTANCE_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_PROTECTION_INSTANCE_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_PROTECTION_INSTANCE_TBL_DATA));
  info->status_bmap = 0;
exit:
  return;
}
void
  T20E_PP_DESTINATION_STATUS_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_DESTINATION_STATUS_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_DESTINATION_STATUS_TBL_DATA));
  info->status_bmap = 0;
exit:
  return;
}
void
  T20E_PP_PORT_CONFIG_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_PORT_CONFIG_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_PORT_CONFIG_TBL_DATA));
  info->vlan_classification_profile = 0;
  info->tpid1_indx = 0;
  info->tpid2_indx = 0;
  info->ignore_incoming_vid = 0;
  info->ignore_incoming_pcp = 0;
  info->vid_ipv4_subnet_enable = 0;
  info->vid_port_protocol_enable = 0;
  info->default_initial_vid = 0;
  info->protocol_profile = 0;
  info->reserved_mc_trap_profile = 0;
  info->pcp_profile = 0;
  info->tc_tos_enable = 0;
  info->tc_tos_index = 0;
  info->tc_l4_protocol_enable = 0;
  info->tc_subnet_enable = 0;
  info->tc_port_protocol_enable = 0;
  info->default_traffic_class = 0;
  info->use_dei = 0;
  info->drop_precedence_profile = 0;
  info->learn_ac = 0;
  info->use_ll = 0;
  info->default_snoop_cmd = 0;
  info->system_port = 0;
exit:
  return;
}
void
  T20E_PP_MEP_LEVEL_PROFILE_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_MEP_LEVEL_PROFILE_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_MEP_LEVEL_PROFILE_TBL_DATA));
  info->level0 = 0;
  info->level1 = 0;
  info->level2 = 0;
  info->level3 = 0;
  info->level4 = 0;
  info->level5 = 0;
  info->level6 = 0;
  info->level7 = 0;
exit:
  return;
}
void
  T20E_PP_OAM_ACTION_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_OAM_ACTION_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_OAM_ACTION_TBL_DATA));
  info->oam_action_profile = 0;
exit:
  return;
}
void
  T20E_PP_FORWARD_ACTION_PROFILE_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_FORWARD_ACTION_PROFILE_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_FORWARD_ACTION_PROFILE_TBL_DATA));
  info->trap = 0;
  info->control = 0;
  info->drop = 0;
  info->destination = 0;
  info->destination_valid = 0;
  info->tc_value = 0;
  info->tc_ow = 0;
  info->dp_value = 0;
  info->dp_ow = 0;
  info->strength = 0;
exit:
  return;
}
void
  T20E_PP_SNOOP_ACTION_PROFILE_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_SNOOP_ACTION_PROFILE_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_SNOOP_ACTION_PROFILE_TBL_DATA));
  info->strength = 0;
exit:
  return;
}
void
  T20E_PP_BURST_SIZE_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_BURST_SIZE_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_BURST_SIZE_TBL_DATA));
  info->max_bs_bytes = 0;
exit:
  return;
}
void
  T20E_PP_MANTISSA_DIVISION_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_MANTISSA_DIVISION_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_MANTISSA_DIVISION_TBL_DATA));
  info->value = 0;
exit:
  return;
}
void
  T20E_PP_GLAG_RANGE_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_GLAG_RANGE_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_GLAG_RANGE_TBL_DATA));
  info->lag_range = 0;
exit:
  return;
}
void
  T20E_PP_MODULO_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_MODULO_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_MODULO_TBL_DATA));
  info->lag_offset = 0;
exit:
  return;
}
void
  T20E_PP_GLAG_OUT_PORT_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_GLAG_OUT_PORT_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_GLAG_OUT_PORT_TBL_DATA));
  info->out_system_port = 0;
exit:
  return;
}
void
  T20E_PP_VID_MIRR_CAMI_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_VID_MIRR_CAMI_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_VID_MIRR_CAMI_TBL_DATA));
  info->vid_mirr_bmap = 0;
exit:
  return;
}
void
  T20E_PP_VID_MIRR_PROFILE_TABLE_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_VID_MIRR_PROFILE_TABLE_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_VID_MIRR_PROFILE_TABLE_TBL_DATA));
  info->vid_mirr_profile = 0;
exit:
  return;
}
void
  T20E_PP_OUTPUT_PORT_CONFIG1_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_OUTPUT_PORT_CONFIG1_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_OUTPUT_PORT_CONFIG1_TBL_DATA));
  info->port_type = 0;
  info->disable_learning = 0;
  info->disable_filtering = 0;
  info->vsi_filtering_enabled = 0;
  info->stp_state_filtering_enabled = 0;
  info->pep_acc_frm_types_filtering_enabled = 0;
  info->hair_pin_filtering_enabled = 0;
  info->unknown_da_uc_filter_enabled = 0;
  info->unknown_da_mc_filter_enabled = 0;
  info->unknown_da_bc_filter_enabled = 0;
  info->split_horizon_filter_enabled = 0;
  info->mtu_check = 0;
  info->outport_port_is_glag_member = 0;
  info->outport_sys_port_id = 0;
  info->default_action_profile = 0;
  info->default_snoop_command = 0;
  info->mtu = 0;
exit:
  return;
}
void
  T20E_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA));
  info->outer_pcp_dei_source = 0;
  info->outer_vid_source = 0;
  info->tpid_profile = 0;
  info->inner_pcp_dei_source = 0;
  info->inner_vid_source = 0;
  info->bytes_to_remove = 0;
exit:
  return;
}
void
  T20E_PP_EGRESS_MPLS_PROFILE1_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_EGRESS_MPLS_PROFILE1_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_EGRESS_MPLS_PROFILE1_TBL_DATA));
  info->pipe = 0;
  info->ttl = 0;
  info->cw = 0;
exit:
  return;
}
void
  T20E_PP_EGRESS_MPLS_PROFILE2_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_EGRESS_MPLS_PROFILE2_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_EGRESS_MPLS_PROFILE2_TBL_DATA));
  info->exp = 0;
exit:
  return;
}
void
  T20E_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_DATA *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_DATA));
  for (ind=0; ind<2; ++ind)
  {
    info->dest_mac[ind] = 0;
  }
  info->out_tpid_index = 0;
  info->out_vid = 0;
  info->out_vid_valid = 0;
  info->out_pcp = 0;
  info->out_pcp_valid = 0;
exit:
  return;
}
void
  T20E_PP_EGRESS_OUT_VSI_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_EGRESS_OUT_VSI_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_EGRESS_OUT_VSI_TBL_DATA));
  info->sa_lsb = 0;
exit:
  return;
}
void
  T20E_PP_EGRESS_LLVP_TABLE_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_EGRESS_LLVP_TABLE_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_EGRESS_LLVP_TABLE_TBL_DATA));
  info->incoming_tag_format = 0;
  info->packet_has_pcp_dei = 0;
  info->packet_has_up = 0;
  info->packet_has_ctag = 0;
  info->ctag_offset = 0;
exit:
  return;
}
void
  T20E_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA));
  info->outer_tpid_type = 0;
  info->inner_tpid_type = 0;
  info->outer_vid_source = 0;
  info->inner_vid_source = 0;
  info->outer_pcp_dei_source = 0;
  info->inner_pcp_dei_source = 0;
  info->bytes_to_remove = 0;
exit:
  return;
}
void
  T20E_PP_EGRESS_PCP_DEI_MAP_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_EGRESS_PCP_DEI_MAP_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_EGRESS_PCP_DEI_MAP_TBL_DATA));
  info->pcp_dei = 0;
exit:
  return;
}
void
  T20E_PP_EGRESS_TC_DP_TO_UP_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_EGRESS_TC_DP_TO_UP_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_EGRESS_TC_DP_TO_UP_TBL_DATA));
  info->user_priority = 0;
exit:
  return;
}
void
  T20E_PP_EGRESS_PORT_VLAN_MEMBERSHIP_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_EGRESS_PORT_VLAN_MEMBERSHIP_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_EGRESS_PORT_VLAN_MEMBERSHIP_TBL_DATA));
  info->port_vid_untagged = 0;
exit:
  return;
}
void
  T20E_PP_EGRESS_STP_STATE_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_EGRESS_STP_STATE_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_EGRESS_STP_STATE_TBL_DATA));
  info->state_state = 0;
exit:
  return;
}
void
  T20E_PP_EGRESS_ACCEPTABLE_FRAME_TYPES_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_EGRESS_ACCEPTABLE_FRAME_TYPES_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_EGRESS_ACCEPTABLE_FRAME_TYPES_TBL_DATA));
  info->acceptable_frame_type = 0;
exit:
  return;
}
void
  T20E_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_DATA));
  info->level0 = 0;
  info->level1 = 0;
  info->level2 = 0;
  info->level3 = 0;
  info->level4 = 0;
  info->level5 = 0;
  info->level6 = 0;
  info->level7 = 0;
exit:
  return;
}
void
  T20E_PP_EGRESS_OAM_ACTION_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_EGRESS_OAM_ACTION_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_EGRESS_OAM_ACTION_TBL_DATA));
  info->oam_action_profile = 0;
exit:
  return;
}
void
  T20E_PP_EGRESS_ACTION_PROFILE_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_EGRESS_ACTION_PROFILE_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_EGRESS_ACTION_PROFILE_TBL_DATA));
  info->trap = 0;
  info->control = 0;
  info->drop = 0;
  info->destination = 0;
  info->tc_value = 0;
  info->tc_ow = 0;
  info->dp_value = 0;
  info->dp_ow = 0;
  info->strength = 0;
exit:
  return;
}
void
  T20E_PP_EGRESS_SNOOP_PROFILE_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_EGRESS_SNOOP_PROFILE_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_EGRESS_SNOOP_PROFILE_TBL_DATA));
  info->strength = 0;
exit:
  return;
}
void
  T20E_PP_AC_KEY_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_AC_KEY_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_AC_KEY_TBL_DATA));
  info->ac_base = 0;
  info->ac_op_code = 0;
  info->stp_topology_id = 0;
  info->enable_my_mac = 0;
  info->my_mac_lsb = 0;
  info->enable_routing_mpls = 0;
  info->mep_index_base = 0;
  info->mep_index_base_valid = 0;
  info->mp_max_level = 0;
  info->mp_max_level_valid = 0;
exit:
  return;
}
void
  T20E_PP_FEC_LOW_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_FEC_LOW_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_FEC_LOW_TBL_DATA));
  info->primary_destination = 0;
  info->secondary_destination = 0;
  info->primary_outlif = 0;
  info->secondary_outlif = 0;
  info->valid = 0;
exit:
  return;
}
void
  T20E_PP_FEC_HIGH_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_FEC_HIGH_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_FEC_HIGH_TBL_DATA));
  info->primary_out_rif = 0;
  info->secondary_out_rif = 0;
  info->protection_instance = 0;
  info->protection_type = 0;
exit:
  return;
}
void
  T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_DATA));
  info->ac_mpls_mvr = 0;
  info->mpls_command = 0;
  info->out_vsi = 0;
  info->out_ac = 0;
  info->mpls_out_vsi = 0;
  info->label = 0;
  info->has_cw = 0;
  info->next_eep = 0;
  info->counter_valid = 0;
  info->counter_ptr = 0;
exit:
  return;
}
void
  T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_DATA));
  info->valid = 0;
  info->stp_topology_id = 0;
  info->mp_index_base = 0;
  info->mp_index_base_valid = 0;
  info->mp_max_level = 0;
  info->mp_max_level_valid = 0;
  info->out_tag_profile = 0;
  info->edit_vid = 0;
  info->edit_up = 0;
  info->counter_valid = 0;
  info->counter_ptr = 0;
exit:
  return;
}
void
  T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_DATA));
  info->valid = 0;
  info->mp_index_base_valid = 0;
  info->mp_index_base = 0;
  info->mp_max_level_valid = 0;
  info->mp_max_level = 0;
  info->counter_valid = 0;
  info->counter_ptr = 0;
  info->out_tag_profile = 0;
  info->pcp_dei_profile = 0;
  info->pep_pvid = 0;
  info->pep_pcp = 0;
  info->pep_acc_frm_types = 0;
  info->pep_cos_profile = 0;
  info->pep_edit_profile = 0;
  info->inner_edit_vid = 0;
  info->stp_topology_id = 0;
  info->outer_edit_vid = 0;
exit:
  return;
}
void
  T20E_PP_MAC_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_MAC_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_MAC_TBL_DATA));
  info->valid = 0;
  info->asd = 0;
  info->destination = 0;
  info->is_dynamic = 0;
exit:
  return;
}
void
  T20E_PP_AC_LOW_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_AC_LOW_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_AC_LOW_TBL_DATA));
  info->unknown_frwd_asd = 0;
  info->unknown_frwd_dst = 0;
  info->learn_destination = 0;
  info->vlan_edit_vid = 0;
  info->vlan_edit_pcp_dei_profile = 0;
exit:
  return;
}
void
  T20E_PP_PWE_LOW_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_PWE_LOW_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_PWE_LOW_TBL_DATA));
  info->unknown_frwd_asd = 0;
  info->unknown_frwd_dst = 0;
  info->reverse_fec = 0;
  info->vsi = 0;
exit:
  return;
}
void
  T20E_PP_AC_HIGH_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_AC_HIGH_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_AC_HIGH_TBL_DATA));
  info->vlan_edit_profile = 0;
  info->learn_enable = 0;
  info->link_layer_termination = 0;
  info->mef_l2_cpsf_tunneling = 0;
  info->cos_profile = 0;
  info->vsi = 0;
  info->policer_enable = 0;
  info->counters_enable = 0;
  info->statistics_tag_enable = 0;
  info->policer_ptr = 0;
  info->counter_or_stat_ptr = 0;
  info->valid = 0;
exit:
  return;
}
void
  T20E_PP_AC_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_AC_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_AC_TBL_DATA));
  info->unknown_frwd_asd = 0;
  info->unknown_frwd_dst = 0;
  info->learn_destination = 0;
  info->vlan_edit_vid = 0;
  info->vlan_edit_pcp_dei_profile = 0;
  info->vlan_edit_profile = 0;
  info->learn_enable = 0;
  info->link_layer_termination = 0;
  info->mef_l2_cpsf_tunneling = 0;
  info->cos_profile = 0;
  info->vsi = 0;
  info->policer_enable = 0;
  info->counters_enable = 0;
  info->statistics_tag_enable = 0;
  info->policer_ptr = 0;
  info->counter_or_stat_ptr = 0;
  info->valid = 0;
exit:
  return;
}
void
  T20E_PP_PWE_HIGH_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_PWE_HIGH_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_PWE_HIGH_TBL_DATA));
  info->learn_asd = 0;
  info->learn_enable = 0;
  info->cos_profile = 0;
  info->tpid_profile = 0;
  info->orientation_is_hub = 0;
  info->policer_enable = 0;
  info->counter_enable = 0;
  info->stat_tag_enable = 0;
  info->policer_ptr = 0;
  info->counter_ptr_stat_tag = 0;
  info->point_to_point = 0;
  info->next_protocol = 0;
  info->valid = 0;
exit:
  return;
}
void
  T20E_PP_PWE_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_PWE_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_PWE_TBL_DATA));
  info->next_protocol = 0;
  info->vsi = 0;
  info->unknown_frwd_dst = 0;
  info->unknown_frwd_asd = 0;
  info->unknown_frwd_dst = 0;
  info->cos_profile = 0;
  info->learn_enable = 0;
  info->tpid_profile = 0;
  info->reverse_fec = 0;
  info->reverse_vc = 0;
  info->orientation_is_hub = 0;
  info->policer_enable = 0;
  info->counter_enable = 0;
  info->stat_tag_enable = 0;
  info->policer_ptr = 0;
  info->counter_ptr_stat_tag = 0;
  info->point_to_point = 0;
  info->valid = 0;
exit:
  return;
}
void
  T20E_PP_AGING_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_AGING_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_AGING_TBL_DATA));
  info->age_status_0 = 0;
  info->age_status_1 = 0;
  info->age_status_2 = 0;
  info->age_status_3 = 0;
  info->age_status_4 = 0;
  info->age_status_5 = 0;
  info->age_status_6 = 0;
  info->age_status_7 = 0;
  info->age_status_8 = 0;
  info->age_status_9 = 0;
  info->age_status_10 = 0;
  info->age_status_11 = 0;
  info->age_status_12 = 0;
  info->age_status_13 = 0;
  info->age_status_14 = 0;
  info->age_status_15 = 0;
exit:
  return;
}
void
  T20E_PP_CNT_INP_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_CNT_INP_TBL_DATA *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_CNT_INP_TBL_DATA));
  for (ind=0; ind<2; ++ind)
  {
    info->octets[ind] = 0;
  }
  info->packets = 0;
exit:
  return;
}
void
  T20E_PP_EGRESS_COUNTERS_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_EGRESS_COUNTERS_TBL_DATA *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_EGRESS_COUNTERS_TBL_DATA));
  for (ind=0; ind<2; ++ind)
  {
    info->octets[ind] = 0;
  }
  info->packets = 0;
exit:
  return;
}
void
  T20E_PP_PLC_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_PLC_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_PLC_TBL_DATA));
exit:
  return;
}
void
  T20E_PP_EGRESS_MPLS_TUNNEL_TBL_DATA_clear(
    SOC_SAND_OUT T20E_PP_EGRESS_MPLS_TUNNEL_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_PP_EGRESS_MPLS_TUNNEL_TBL_DATA));
  info->orientation = 0;
  info->mpls1_command = 0;
  info->mpls1_label = 0;
  info->mpls2_command = 0;
  info->mpls2_label = 0;
  info->out_vsi = 0;
  info->next_eep = 0;
exit:
  return;
}

void
  T20E_MACT_FID_COUNTER_PROFILE_DB_TBL_DATA_clear(
    SOC_SAND_OUT T20E_MACT_FID_COUNTER_PROFILE_DB_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_MACT_FID_COUNTER_PROFILE_DB_TBL_DATA));
  info->limit = 0;
  info->interrupt_en = 0;
  info->limit_en = 0;
exit:
  return;
}
void
  T20E_MACT_FID_COUNTER_DB_TBL_DATA_clear(
    SOC_SAND_OUT T20E_MACT_FID_COUNTER_DB_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_MACT_FID_COUNTER_DB_TBL_DATA));
  info->entry_count = 0;
  info->profile_pointer = 0;
exit:
  return;
}
/*
void
  T20E_MACT_FLUSH_DB_TBL_DATA_clear(
    SOC_SAND_OUT T20E_MACT_FLUSH_DB_TBL_DATA *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(T20E_MACT_FLUSH_DB_TBL_DATA));
  info->compare_valid = 0;
  info->compare_key_18_msbs_data = 0;
  info->compare_key_18_msbs_mask = 0;
  for (ind=0; ind<2; ++ind)
  {
    info->compare_payload_data[ind] = 0;
  }
  for (ind=0; ind<2; ++ind)
  {
    info->compare_payload_mask[ind] = 0;
  }
  info->action_drop = 0;
  for (ind=0; ind<2; ++ind)
  {
    info->action_transplant_payload_data[ind] = 0;
  }
  for (ind=0; ind<2; ++ind)
  {
    info->action_transplant_payload_mask[ind] = 0;
  }
exit:
  return;
}
*/


#if T20E_DEBUG

const char*
  T20E_PP_LEM_TBL_KEY_TYPE_to_string(
    SOC_SAND_IN  T20E_PP_LEM_TBL_KEY_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case T20E_PP_LEM_TBL_KEY_TYPE_DIRECT:
    str = "T20E_PP_LEM_TBL_KEY_TYPE_DIRECT";
  break;
  case T20E_PP_LEM_TBL_KEY_TYPE_ILM:
    str = "T20E_PP_LEM_TBL_KEY_TYPE_ILM";
  break;
  default:
    str = " Unknown Enumerator Value";
  }
  return str;
}

void
  T20E_PP_LEM_TBL_KEY_INFO_print(
    SOC_SAND_IN  T20E_PP_LEM_TBL_KEY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "offset: %lu\n\r",info->offset);
  soc_sand_os_printf( "ilm:");
  T20E_FRWRD_ILM_KEY_print(&(info->ilm));
exit:
  return;
}

void
  T20E_PP_LEM_TBL_KEY_print(
    SOC_SAND_IN  T20E_PP_LEM_TBL_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "type %s ", T20E_PP_LEM_TBL_KEY_TYPE_to_string(info->type));
  soc_sand_os_printf( "info:");
  T20E_PP_LEM_TBL_KEY_INFO_print(&(info->info));
exit:
  return;
}

void
  T20E_PP_LLVP_CLASSIFICATION_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_LLVP_CLASSIFICATION_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "incoming_vid_exist: %lu\n\r",info->incoming_vid_exist);
  soc_sand_os_printf( "incoming_tag_exist: %lu\n\r",info->incoming_tag_exist);
  soc_sand_os_printf( "incoming_s_tag_exist: %lu\n\r",info->incoming_s_tag_exist);
  soc_sand_os_printf( "acceptable_frame_type_action: %lu\n\r",info->acceptable_frame_type_action);
  soc_sand_os_printf( "tag_edit_profile: %lu\n\r",info->tag_edit_profile);
  soc_sand_os_printf( "ac_key_encoding: %lu\n\r",info->ac_key_encoding);
exit:
  return;
}

void
  T20E_PP_IPV4_SUBNET_CAMI_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_IPV4_SUBNET_CAMI_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "ipv4_subnet_bmap: %lu\n\r",info->ipv4_subnet_bmap);
exit:
  return;
}

void
  T20E_PP_IPV4_SUBNET_TABLE_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_IPV4_SUBNET_TABLE_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "ipv4_subnet_vid: %lu\n\r",info->ipv4_subnet_vid);
  soc_sand_os_printf( "ipv4_subnet_vid_valid: %lu\n\r",info->ipv4_subnet_vid_valid);
  soc_sand_os_printf( "ipv4_subnet_tc: %lu\n\r",info->ipv4_subnet_tc);
  soc_sand_os_printf( "ipv4_subnet_tc_valid: %lu\n\r",info->ipv4_subnet_tc_valid);
exit:
  return;
}

void
  T20E_PP_PORT_PROTOCOL_CAMI_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_PORT_PROTOCOL_CAMI_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "ethernet_type_bmap: %lu\n\r",info->ethernet_type_bmap);
exit:
  return;
}

void
  T20E_PP_PORT_PROTOCOL_TO_VID_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_PORT_PROTOCOL_TO_VID_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "vid: %lu\n\r",info->vid);
  soc_sand_os_printf( "vid_valid: %lu\n\r",info->vid_valid);
  soc_sand_os_printf( "traffic_class: %lu\n\r",info->traffic_class);
  soc_sand_os_printf( "traffic_class_valid: %lu\n\r",info->traffic_class_valid);
exit:
  return;
}

void
  T20E_PP_VLAN_PORT_MEMBERSHIP_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_VLAN_PORT_MEMBERSHIP_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "port_bmap_membership: %lu\n\r",info->port_bmap_membership);
exit:
  return;
}

void
  T20E_PP_RESERVED_MC_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_RESERVED_MC_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "reserved_mc_trap_code: %lu\n\r",info->reserved_mc_trap_code);
exit:
  return;
}

void
  T20E_PP_PCP_DECODING_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_PCP_DECODING_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "traffic_class: %lu\n\r",info->traffic_class);
  soc_sand_os_printf( "drop_eligible: %lu\n\r",info->drop_eligible);
exit:
  return;
}

void
  T20E_PP_TOS_TO_TC_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_TOS_TO_TC_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "traffic_class: %lu\n\r",info->traffic_class);
  soc_sand_os_printf( "traffic_class_valid: %lu\n\r",info->traffic_class_valid);
exit:
  return;
}

void
  T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "traffic_class: %lu\n\r",info->traffic_class);
  soc_sand_os_printf( "traffic_class_valid: %lu\n\r",info->traffic_class_valid);
exit:
  return;
}

void
  T20E_PP_DROP_PRECEDENCE_MAP_TC_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_DROP_PRECEDENCE_MAP_TC_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "drop_precedence: %lu\n\r",info->drop_precedence);
exit:
  return;
}

void
  T20E_PP_AC_OFFSET_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_AC_OFFSET_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "ac_offset: %lu\n\r",info->ac_offset);
exit:
  return;
}

void
  T20E_PP_COS_PROFILE_DECODING_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_COS_PROFILE_DECODING_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "force_or_map: %lu\n\r",info->force_or_map);
  soc_sand_os_printf( "map_profile: %lu\n\r",info->map_profile);
  soc_sand_os_printf( "traffic_class: %lu\n\r",info->traffic_class);
  soc_sand_os_printf( "drop_precedence: %lu\n\r",info->drop_precedence);
exit:
  return;
}

void
  T20E_PP_COS_PROFILE_MAPPING_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_COS_PROFILE_MAPPING_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "traffic_class_ow: %lu\n\r",info->traffic_class_ow);
  soc_sand_os_printf( "traffic_class: %lu\n\r",info->traffic_class);
  soc_sand_os_printf( "drop_precedence_ow: %lu\n\r",info->drop_precedence_ow);
  soc_sand_os_printf( "drop_precedence: %lu\n\r",info->drop_precedence);
exit:
  return;
}

void
  T20E_PP_STP_STATE_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_STP_STATE_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "stp_state: %lu\n\r",info->stp_state);
exit:
  return;
}

void
  T20E_PP_PCP_DEI_MAP_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_PCP_DEI_MAP_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "pcp: %lu\n\r",info->pcp);
  soc_sand_os_printf( "dei: %lu\n\r",info->dei);
exit:
  return;
}

void
  T20E_PP_IVEC_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_IVEC_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "command: %lu\n\r",info->command);
exit:
  return;
}

void
  T20E_PP_ACTION_PROFILE_MPLS_VALUE_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_ACTION_PROFILE_MPLS_VALUE_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "action_not_bos_frwrd: %lu\n\r",info->action_not_bos_frwrd);
  soc_sand_os_printf( "action_not_bos_snoop: %lu\n\r",info->action_not_bos_snoop);
  soc_sand_os_printf( "action_bos_frwrd: %lu\n\r",info->action_bos_frwrd);
  soc_sand_os_printf( "action_bos_snoop: %lu\n\r",info->action_bos_snoop);
exit:
  return;
}

void
  T20E_PP_MPLS_COS_PROFILE_DECODING_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_MPLS_COS_PROFILE_DECODING_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "force_or_map: %lu\n\r",info->force_or_map);
  soc_sand_os_printf( "map_profile: %lu\n\r",info->map_profile);
  soc_sand_os_printf( "traffic_class: %lu\n\r",info->traffic_class);
  soc_sand_os_printf( "drop_precedence: %lu\n\r",info->drop_precedence);
exit:
  return;
}

void
  T20E_PP_MPLS_COS_MAPPING_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_MPLS_COS_MAPPING_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "traffic_class_ow: %lu\n\r",info->traffic_class_ow);
  soc_sand_os_printf( "traffic_class: %lu\n\r",info->traffic_class);
  soc_sand_os_printf( "drop_precedence_ow: %lu\n\r",info->drop_precedence_ow);
  soc_sand_os_printf( "drop_precedence: %lu\n\r",info->drop_precedence);
exit:
  return;
}

void
  T20E_PP_MPLS_TERMINATION_COS_MAPPING_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_MPLS_TERMINATION_COS_MAPPING_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "traffic_class_ow: %lu\n\r",info->traffic_class_ow);
  soc_sand_os_printf( "traffic_class: %lu\n\r",info->traffic_class);
  soc_sand_os_printf( "drop_precedence_ow: %lu\n\r",info->drop_precedence_ow);
  soc_sand_os_printf( "drop_precedence: %lu\n\r",info->drop_precedence);
exit:
  return;
}

void
  T20E_PP_MPLS_TUNNEL_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_MPLS_TUNNEL_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "termination_valid: %lu\n\r",info->termination_valid);
  soc_sand_os_printf( "counter_valid: %lu\n\r",info->counter_valid);
  soc_sand_os_printf( "policer_valid: %lu\n\r",info->policer_valid);
  soc_sand_os_printf( "stat_tag_valid: %lu\n\r",info->stat_tag_valid);
exit:
  return;
}

void
  T20E_PP_PROTECTION_INSTANCE_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_PROTECTION_INSTANCE_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "status_bmap: %lu\n\r",info->status_bmap);
exit:
  return;
}

void
  T20E_PP_DESTINATION_STATUS_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_DESTINATION_STATUS_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "status_bmap: %lu\n\r",info->status_bmap);
exit:
  return;
}

void
  T20E_PP_PORT_CONFIG_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_PORT_CONFIG_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "vlan_classification_profile: %lu\n\r",info->vlan_classification_profile);
  soc_sand_os_printf( "tpid1_indx: %lu\n\r",info->tpid1_indx);
  soc_sand_os_printf( "tpid2_indx: %lu\n\r",info->tpid2_indx);
  soc_sand_os_printf( "ignore_incoming_vid: %lu\n\r",info->ignore_incoming_vid);
  soc_sand_os_printf( "ignore_incoming_pcp: %lu\n\r",info->ignore_incoming_pcp);
  soc_sand_os_printf( "vid_ipv4_subnet_enable: %lu\n\r",info->vid_ipv4_subnet_enable);
  soc_sand_os_printf( "vid_port_protocol_enable: %lu\n\r",info->vid_port_protocol_enable);
  soc_sand_os_printf( "default_initial_vid: %lu\n\r",info->default_initial_vid);
  soc_sand_os_printf( "protocol_profile: %lu\n\r",info->protocol_profile);
  soc_sand_os_printf( "reserved_mc_trap_profile: %lu\n\r",info->reserved_mc_trap_profile);
  soc_sand_os_printf( "pcp_profile: %lu\n\r",info->pcp_profile);
  soc_sand_os_printf( "tc_tos_enable: %lu\n\r",info->tc_tos_enable);
  soc_sand_os_printf( "tc_tos_index: %lu\n\r",info->tc_tos_index);
  soc_sand_os_printf( "tc_l4_protocol_enable: %lu\n\r",info->tc_l4_protocol_enable);
  soc_sand_os_printf( "tc_subnet_enable: %lu\n\r",info->tc_subnet_enable);
  soc_sand_os_printf( "tc_port_protocol_enable: %lu\n\r",info->tc_port_protocol_enable);
  soc_sand_os_printf( "default_traffic_class: %lu\n\r",info->default_traffic_class);
  soc_sand_os_printf( "use_dei: %lu\n\r",info->use_dei);
  soc_sand_os_printf( "drop_precedence_profile: %lu\n\r",info->drop_precedence_profile);
  soc_sand_os_printf( "learn_ac: %lu\n\r",info->learn_ac);
  soc_sand_os_printf( "use_ll: %lu\n\r",info->use_ll);
  soc_sand_os_printf( "default_snoop_cmd: %lu\n\r",info->default_snoop_cmd);
  soc_sand_os_printf( "system_port: %lu\n\r",info->system_port);
exit:
  return;
}

void
  T20E_PP_MEP_LEVEL_PROFILE_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_MEP_LEVEL_PROFILE_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "level0: %lu\n\r",info->level0);
  soc_sand_os_printf( "level1: %lu\n\r",info->level1);
  soc_sand_os_printf( "level2: %lu\n\r",info->level2);
  soc_sand_os_printf( "level3: %lu\n\r",info->level3);
  soc_sand_os_printf( "level4: %lu\n\r",info->level4);
  soc_sand_os_printf( "level5: %lu\n\r",info->level5);
  soc_sand_os_printf( "level6: %lu\n\r",info->level6);
  soc_sand_os_printf( "level7: %lu\n\r",info->level7);
exit:
  return;
}

void
  T20E_PP_OAM_ACTION_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_OAM_ACTION_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "oam_action_profile: %lu\n\r",info->oam_action_profile);
exit:
  return;
}

void
  T20E_PP_FORWARD_ACTION_PROFILE_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_FORWARD_ACTION_PROFILE_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "trap: %lu\n\r",info->trap);
  soc_sand_os_printf( "control: %lu\n\r",info->control);
  soc_sand_os_printf( "drop: %lu\n\r",info->drop);
  soc_sand_os_printf( "destination: %lu\n\r",info->destination);
  soc_sand_os_printf( "destination_valid: %lu\n\r",info->destination_valid);
  soc_sand_os_printf( "tc_value: %lu\n\r",info->tc_value);
  soc_sand_os_printf( "tc_ow: %lu\n\r",info->tc_ow);
  soc_sand_os_printf( "dp_value: %lu\n\r",info->dp_value);
  soc_sand_os_printf( "dp_ow: %lu\n\r",info->dp_ow);
  soc_sand_os_printf( "strength: %lu\n\r",info->strength);
exit:
  return;
}

void
  T20E_PP_SNOOP_ACTION_PROFILE_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_SNOOP_ACTION_PROFILE_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "strength: %lu\n\r",info->strength);
exit:
  return;
}

void
  T20E_PP_BURST_SIZE_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_BURST_SIZE_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "max_bs_bytes: %lu\n\r",info->max_bs_bytes);
exit:
  return;
}

void
  T20E_PP_MANTISSA_DIVISION_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_MANTISSA_DIVISION_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "value: %lu\n\r",info->value);
exit:
  return;
}

void
  T20E_PP_GLAG_RANGE_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_GLAG_RANGE_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "lag_range: %lu\n\r",info->lag_range);
exit:
  return;
}

void
  T20E_PP_MODULO_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_MODULO_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "lag_offset: %lu\n\r",info->lag_offset);
exit:
  return;
}

void
  T20E_PP_GLAG_OUT_PORT_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_GLAG_OUT_PORT_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "out_system_port: %lu\n\r",info->out_system_port);
exit:
  return;
}

void
  T20E_PP_VID_MIRR_CAMI_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_VID_MIRR_CAMI_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "vid_mirr_bmap: %lu\n\r",info->vid_mirr_bmap);
exit:
  return;
}

void
  T20E_PP_VID_MIRR_PROFILE_TABLE_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_VID_MIRR_PROFILE_TABLE_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "vid_mirr_profile: %lu\n\r",info->vid_mirr_profile);
exit:
  return;
}

void
  T20E_PP_OUTPUT_PORT_CONFIG1_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_OUTPUT_PORT_CONFIG1_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "port_type: %lu\n\r",info->port_type);
  soc_sand_os_printf( "disable_learning: %lu\n\r",info->disable_learning);
  soc_sand_os_printf( "disable_filtering: %lu\n\r",info->disable_filtering);
  soc_sand_os_printf( "vsi_filtering_enabled: %lu\n\r",info->vsi_filtering_enabled);
  soc_sand_os_printf( "stp_state_filtering_enabled: %lu\n\r",info->stp_state_filtering_enabled);
  soc_sand_os_printf( "pep_acc_frm_types_filtering_enabled: %lu\n\r",info->pep_acc_frm_types_filtering_enabled);
  soc_sand_os_printf( "hair_pin_filtering_enabled: %lu\n\r",info->hair_pin_filtering_enabled);
  soc_sand_os_printf( "unknown_da_uc_filter_enabled: %lu\n\r",info->unknown_da_uc_filter_enabled);
  soc_sand_os_printf( "unknown_da_mc_filter_enabled: %lu\n\r",info->unknown_da_mc_filter_enabled);
  soc_sand_os_printf( "unknown_da_bc_filter_enabled: %lu\n\r",info->unknown_da_bc_filter_enabled);
  soc_sand_os_printf( "split_horizon_filter_enabled: %lu\n\r",info->split_horizon_filter_enabled);
  soc_sand_os_printf( "mtu_check: %lu\n\r",info->mtu_check);
  soc_sand_os_printf( "outport_port_is_glag_member: %lu\n\r",info->outport_port_is_glag_member);
  soc_sand_os_printf( "outport_sys_port_id: %lu\n\r",info->outport_sys_port_id);
  soc_sand_os_printf( "default_action_profile: %lu\n\r",info->default_action_profile);
  soc_sand_os_printf( "default_snoop_command: %lu\n\r",info->default_snoop_command);
  soc_sand_os_printf( "mtu: %lu\n\r",info->mtu);
exit:
  return;
}

void
  T20E_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "outer_pcp_dei_source: %lu\n\r",info->outer_pcp_dei_source);
  soc_sand_os_printf( "outer_vid_source: %lu\n\r",info->outer_vid_source);
  soc_sand_os_printf( "tpid_profile: %lu\n\r",info->tpid_profile);
  soc_sand_os_printf( "inner_pcp_dei_source: %lu\n\r",info->inner_pcp_dei_source);
  soc_sand_os_printf( "inner_vid_source: %lu\n\r",info->inner_vid_source);
  soc_sand_os_printf( "bytes_to_remove: %lu\n\r",info->bytes_to_remove);
exit:
  return;
}

void
  T20E_PP_EGRESS_MPLS_PROFILE1_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_EGRESS_MPLS_PROFILE1_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "pipe: %lu\n\r",info->pipe);
  soc_sand_os_printf( "ttl: %lu\n\r",info->ttl);
  soc_sand_os_printf( "cw: %lu\n\r",info->cw);
exit:
  return;
}

void
  T20E_PP_EGRESS_MPLS_PROFILE2_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_EGRESS_MPLS_PROFILE2_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "exp: %lu\n\r",info->exp);
exit:
  return;
}

void
  T20E_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_DATA *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind=0; ind<2; ++ind)
  {
    soc_sand_os_printf( "dest_mac[%u]: %lu\n\r",ind,info->dest_mac[ind]);
  }
  soc_sand_os_printf( "out_tpid_index: %lu\n\r",info->out_tpid_index);
  soc_sand_os_printf( "out_vid: %lu\n\r",info->out_vid);
  soc_sand_os_printf( "out_vid_valid: %lu\n\r",info->out_vid_valid);
  soc_sand_os_printf( "out_pcp: %lu\n\r",info->out_pcp);
  soc_sand_os_printf( "out_pcp_valid: %lu\n\r",info->out_pcp_valid);
exit:
  return;
}

void
  T20E_PP_EGRESS_OUT_VSI_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_EGRESS_OUT_VSI_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "sa_lsb: %lu\n\r",info->sa_lsb);
exit:
  return;
}

void
  T20E_PP_EGRESS_LLVP_TABLE_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_EGRESS_LLVP_TABLE_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "incoming_tag_format: %lu\n\r",info->incoming_tag_format);
  soc_sand_os_printf( "packet_has_pcp_dei: %lu\n\r",info->packet_has_pcp_dei);
  soc_sand_os_printf( "packet_has_up: %lu\n\r",info->packet_has_up);
  soc_sand_os_printf( "packet_has_ctag: %lu\n\r",info->packet_has_ctag);
  soc_sand_os_printf( "ctag_offset: %lu\n\r",info->ctag_offset);
exit:
  return;
}

void
  T20E_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "outer_tpid_type: %lu\n\r",info->outer_tpid_type);
  soc_sand_os_printf( "inner_tpid_type: %lu\n\r",info->inner_tpid_type);
  soc_sand_os_printf( "outer_vid_source: %lu\n\r",info->outer_vid_source);
  soc_sand_os_printf( "inner_vid_source: %lu\n\r",info->inner_vid_source);
  soc_sand_os_printf( "outer_pcp_dei_source: %lu\n\r",info->outer_pcp_dei_source);
  soc_sand_os_printf( "inner_pcp_dei_source: %lu\n\r",info->inner_pcp_dei_source);
  soc_sand_os_printf( "bytes_to_remove: %lu\n\r",info->bytes_to_remove);
exit:
  return;
}

void
  T20E_PP_EGRESS_PCP_DEI_MAP_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_EGRESS_PCP_DEI_MAP_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "pcp_dei: %lu\n\r",info->pcp_dei);
exit:
  return;
}

void
  T20E_PP_EGRESS_TC_DP_TO_UP_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_EGRESS_TC_DP_TO_UP_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "user_priority: %lu\n\r",info->user_priority);
exit:
  return;
}

void
  T20E_PP_EGRESS_PORT_VLAN_MEMBERSHIP_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_EGRESS_PORT_VLAN_MEMBERSHIP_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "port_vid_untagged: %lu\n\r",info->port_vid_untagged);
exit:
  return;
}

void
  T20E_PP_EGRESS_STP_STATE_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_EGRESS_STP_STATE_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "state_state: %lu\n\r",info->state_state);
exit:
  return;
}

void
  T20E_PP_EGRESS_ACCEPTABLE_FRAME_TYPES_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_EGRESS_ACCEPTABLE_FRAME_TYPES_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "acceptable_frame_type: %lu\n\r",info->acceptable_frame_type);
exit:
  return;
}

void
  T20E_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "level0: %lu\n\r",info->level0);
  soc_sand_os_printf( "level1: %lu\n\r",info->level1);
  soc_sand_os_printf( "level2: %lu\n\r",info->level2);
  soc_sand_os_printf( "level3: %lu\n\r",info->level3);
  soc_sand_os_printf( "level4: %lu\n\r",info->level4);
  soc_sand_os_printf( "level5: %lu\n\r",info->level5);
  soc_sand_os_printf( "level6: %lu\n\r",info->level6);
  soc_sand_os_printf( "level7: %lu\n\r",info->level7);
exit:
  return;
}

void
  T20E_PP_EGRESS_OAM_ACTION_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_EGRESS_OAM_ACTION_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "oam_action_profile: %lu\n\r",info->oam_action_profile);
exit:
  return;
}

void
  T20E_PP_EGRESS_ACTION_PROFILE_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_EGRESS_ACTION_PROFILE_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "trap: %lu\n\r",info->trap);
  soc_sand_os_printf( "control: %lu\n\r",info->control);
  soc_sand_os_printf( "drop: %lu\n\r",info->drop);
  soc_sand_os_printf( "destination: %lu\n\r",info->destination);
  soc_sand_os_printf( "tc_value: %lu\n\r",info->tc_value);
  soc_sand_os_printf( "tc_ow: %lu\n\r",info->tc_ow);
  soc_sand_os_printf( "dp_value: %lu\n\r",info->dp_value);
  soc_sand_os_printf( "dp_ow: %lu\n\r",info->dp_ow);
  soc_sand_os_printf( "strength: %lu\n\r",info->strength);
exit:
  return;
}

void
  T20E_PP_EGRESS_SNOOP_PROFILE_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_EGRESS_SNOOP_PROFILE_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "strength: %lu\n\r",info->strength);
exit:
  return;
}

void
  T20E_PP_AC_KEY_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_AC_KEY_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "ac_base: %lu\n\r",info->ac_base);
  soc_sand_os_printf( "ac_op_code: %lu\n\r",info->ac_op_code);
  soc_sand_os_printf( "stp_topology_id: %lu\n\r",info->stp_topology_id);
  soc_sand_os_printf( "enable_my_mac: %lu\n\r",info->enable_my_mac);
  soc_sand_os_printf( "my_mac_lsb: %lu\n\r",info->my_mac_lsb);
  soc_sand_os_printf( "enable_routing_mpls: %lu\n\r",info->enable_routing_mpls);
  soc_sand_os_printf( "mep_index_base: %lu\n\r",info->mep_index_base);
  soc_sand_os_printf( "mep_index_base_valid: %lu\n\r",info->mep_index_base_valid);
  soc_sand_os_printf( "mp_max_level: %lu\n\r",info->mp_max_level);
  soc_sand_os_printf( "mp_max_level_valid: %lu\n\r",info->mp_max_level_valid);
exit:
  return;
}

void
  T20E_PP_FEC_LOW_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_FEC_LOW_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "primary_destination: %lu\n\r",info->primary_destination);
  soc_sand_os_printf( "secondary_destination: %lu\n\r",info->secondary_destination);
  soc_sand_os_printf( "primary_outlif: %lu\n\r",info->primary_outlif);
  soc_sand_os_printf( "secondary_outlif: %lu\n\r",info->secondary_outlif);
  soc_sand_os_printf( "valid: %lu\n\r",info->valid);
exit:
  return;
}

void
  T20E_PP_FEC_HIGH_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_FEC_HIGH_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "primary_out_rif: %lu\n\r",info->primary_out_rif);
  soc_sand_os_printf( "secondary_out_rif: %lu\n\r",info->secondary_out_rif);
  soc_sand_os_printf( "protection_instance: %lu\n\r",info->protection_instance);
  soc_sand_os_printf( "protection_type: %lu\n\r",info->protection_type);
exit:
  return;
}

void
  T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "ac_mpls_mvr: %lu\n\r",info->ac_mpls_mvr);
  soc_sand_os_printf( "mpls_command: %lu\n\r",info->mpls_command);
  soc_sand_os_printf( "out_vsi: %lu\n\r",info->out_vsi);
  soc_sand_os_printf( "out_ac: %lu\n\r",info->out_ac);
  soc_sand_os_printf( "mpls_out_vsi: %lu\n\r",info->mpls_out_vsi);
  soc_sand_os_printf( "label: %lu\n\r",info->label);
  soc_sand_os_printf( "has_cw: %lu\n\r",info->has_cw);
  soc_sand_os_printf( "next_eep: %lu\n\r",info->next_eep);
  soc_sand_os_printf( "counter_valid: %lu\n\r",info->counter_valid);
  soc_sand_os_printf( "counter_ptr: %lu\n\r",info->counter_ptr);
exit:
  return;
}

void
  T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_EEI_DATA_print(
    SOC_SAND_IN  T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_DATA *info
  )
{
  uint32
    indx;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (indx = 0 ; indx<4; ++indx )
  {
    soc_sand_os_printf( "pointer %lu: %lu   valid:%lu\n\r",indx, info->eei_counters_ptr[indx], info->eei_counters_valid[indx]);
  }

exit:
  return;
}

void
  T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "valid: %lu\n\r",info->valid);
  soc_sand_os_printf( "stp_topology_id: %lu\n\r",info->stp_topology_id);
  soc_sand_os_printf( "mp_index_base: %lu\n\r",info->mp_index_base);
  soc_sand_os_printf( "mp_index_base_valid: %lu\n\r",info->mp_index_base_valid);
  soc_sand_os_printf( "mp_max_level: %lu\n\r",info->mp_max_level);
  soc_sand_os_printf( "mp_max_level_valid: %lu\n\r",info->mp_max_level_valid);
  soc_sand_os_printf( "out_tag_profile: %lu\n\r",info->out_tag_profile);
  soc_sand_os_printf( "edit_vid: %lu\n\r",info->edit_vid);
  soc_sand_os_printf( "edit_up: %lu\n\r",info->edit_up);
  soc_sand_os_printf( "counter_valid: %lu\n\r",info->counter_valid);
  soc_sand_os_printf( "counter_ptr: %lu\n\r",info->counter_ptr);
exit:
  return;
}

void
  T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "valid: %lu\n\r",info->valid);
  soc_sand_os_printf( "mp_index_base_valid: %lu\n\r",info->mp_index_base_valid);
  soc_sand_os_printf( "mp_index_base: %lu\n\r",info->mp_index_base);
  soc_sand_os_printf( "mp_max_level_valid: %lu\n\r",info->mp_max_level_valid);
  soc_sand_os_printf( "mp_max_level: %lu\n\r",info->mp_max_level);
  soc_sand_os_printf( "counter_valid: %lu\n\r",info->counter_valid);
  soc_sand_os_printf( "counter_ptr: %lu\n\r",info->counter_ptr);
  soc_sand_os_printf( "out_tag_profile: %lu\n\r",info->out_tag_profile);
  soc_sand_os_printf( "pcp_dei_profile: %lu\n\r",info->pcp_dei_profile);
  soc_sand_os_printf( "pep_pvid: %lu\n\r",info->pep_pvid);
  soc_sand_os_printf( "pep_pcp: %lu\n\r",info->pep_pcp);
  soc_sand_os_printf( "pep_acc_frm_types: %lu\n\r",info->pep_acc_frm_types);
  soc_sand_os_printf( "pep_cos_profile: %lu\n\r",info->pep_cos_profile);
  soc_sand_os_printf( "pep_edit_profile: %lu\n\r",info->pep_edit_profile);
  soc_sand_os_printf( "inner_edit_vid: %lu\n\r",info->inner_edit_vid);
  soc_sand_os_printf( "stp_topology_id: %lu\n\r",info->stp_topology_id);
  soc_sand_os_printf( "outer_edit_vid: %lu\n\r",info->outer_edit_vid);
exit:
  return;
}

void
  T20E_PP_MAC_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_MAC_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "valid: %lu\n\r",info->valid);
  soc_sand_os_printf( "asd: %lu\n\r",info->asd);
  soc_sand_os_printf( "destination: %lu\n\r",info->destination);
  soc_sand_os_printf( "is_dynamic: %lu\n\r",info->is_dynamic);
exit:
  return;
}

void
  T20E_PP_AC_LOW_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_AC_LOW_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "unknown_frwd_asd: %lu\n\r",info->unknown_frwd_asd);
  soc_sand_os_printf( "unknown_frwd_dst: %lu\n\r",info->unknown_frwd_dst);
  soc_sand_os_printf( "learn_destination: %lu\n\r",info->learn_destination);
  soc_sand_os_printf( "vlan_edit_vid: %lu\n\r",info->vlan_edit_vid);
  soc_sand_os_printf( "vlan_edit_pcp_dei_profile: %lu\n\r",info->vlan_edit_pcp_dei_profile);
exit:
  return;
}

void
  T20E_PP_PWE_LOW_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_PWE_LOW_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "unknown_frwd_asd: %lu\n\r",info->unknown_frwd_asd);
  soc_sand_os_printf( "unknown_frwd_dst: %lu\n\r",info->unknown_frwd_dst);
  soc_sand_os_printf( "reverse_fec: %lu\n\r",info->reverse_fec);
  soc_sand_os_printf( "vsi: %lu\n\r",info->vsi);
exit:
  return;
}

void
  T20E_PP_AC_HIGH_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_AC_HIGH_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "vlan_edit_profile: %lu\n\r",info->vlan_edit_profile);
  soc_sand_os_printf( "learn_enable: %lu\n\r",info->learn_enable);
  soc_sand_os_printf( "link_layer_termination: %lu\n\r",info->link_layer_termination);
  soc_sand_os_printf( "mef_l2_cpsf_tunneling: %lu\n\r",info->mef_l2_cpsf_tunneling);
  soc_sand_os_printf( "cos_profile: %lu\n\r",info->cos_profile);
  soc_sand_os_printf( "vsi: %lu\n\r",info->vsi);
  soc_sand_os_printf( "policer_enable: %lu\n\r",info->policer_enable);
  soc_sand_os_printf( "counters_enable: %lu\n\r",info->counters_enable);
  soc_sand_os_printf( "statistics_tag_enable: %lu\n\r",info->statistics_tag_enable);
  soc_sand_os_printf( "policer_ptr: %lu\n\r",info->policer_ptr);
  soc_sand_os_printf( "counter_or_stat_ptr: %lu\n\r",info->counter_or_stat_ptr);
  soc_sand_os_printf( "valid: %lu\n\r",info->valid);
exit:
  return;
}

void
  T20E_PP_AC_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_AC_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "unknown_frwd_asd: %lu\n\r",info->unknown_frwd_asd);
  soc_sand_os_printf( "unknown_frwd_dst: %lu\n\r",info->unknown_frwd_dst);
  soc_sand_os_printf( "learn_destination: %lu\n\r",info->learn_destination);
  soc_sand_os_printf( "vlan_edit_vid: %lu\n\r",info->vlan_edit_vid);
  soc_sand_os_printf( "vlan_edit_pcp_dei_profile: %lu\n\r",info->vlan_edit_pcp_dei_profile);
  soc_sand_os_printf( "vlan_edit_profile: %lu\n\r",info->vlan_edit_profile);
  soc_sand_os_printf( "learn_enable: %lu\n\r",info->learn_enable);
  soc_sand_os_printf( "link_layer_termination: %lu\n\r",info->link_layer_termination);
  soc_sand_os_printf( "mef_l2_cpsf_tunneling: %lu\n\r",info->mef_l2_cpsf_tunneling);
  soc_sand_os_printf( "cos_profile: %lu\n\r",info->cos_profile);
  soc_sand_os_printf( "vsi: %lu\n\r",info->vsi);
  soc_sand_os_printf( "policer_enable: %lu\n\r",info->policer_enable);
  soc_sand_os_printf( "counters_enable: %lu\n\r",info->counters_enable);
  soc_sand_os_printf( "statistics_tag_enable: %lu\n\r",info->statistics_tag_enable);
  soc_sand_os_printf( "policer_ptr: %lu\n\r",info->policer_ptr);
  soc_sand_os_printf( "counter_or_stat_ptr: %lu\n\r",info->counter_or_stat_ptr);
  soc_sand_os_printf( "valid: %lu\n\r",info->valid);
exit:
  return;
}

void
  T20E_PP_PWE_HIGH_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_PWE_HIGH_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "asd: %lu\n\r",info->learn_asd);
  soc_sand_os_printf( "learn_enable: %lu\n\r",info->learn_enable);
  soc_sand_os_printf( "cos_profile: %lu\n\r",info->cos_profile);
  soc_sand_os_printf( "tpid_profile: %lu\n\r",info->tpid_profile);
  soc_sand_os_printf( "orientation_is_hub: %lu\n\r",info->orientation_is_hub);
  soc_sand_os_printf( "policer_enable: %lu\n\r",info->policer_enable);
  soc_sand_os_printf( "counter_enable: %lu\n\r",info->counter_enable);
  soc_sand_os_printf( "stat_tag_enable: %lu\n\r",info->stat_tag_enable);
  soc_sand_os_printf( "policer_ptr: %lu\n\r",info->policer_ptr);
  soc_sand_os_printf( "counter_ptr_stat_tag: %lu\n\r",info->counter_ptr_stat_tag);
  soc_sand_os_printf( "point_to_point: %lu\n\r",info->point_to_point);
  soc_sand_os_printf( "next_protocol: %lu\n\r",info->next_protocol);
  soc_sand_os_printf( "valid: %lu\n\r",info->valid);
exit:
  return;
}

void
  T20E_PP_PWE_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_PWE_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "next_protocol: %lu\n\r",info->next_protocol);
  soc_sand_os_printf( "vsi: %lu\n\r",info->vsi);
  soc_sand_os_printf( "unknown_frwd_asd: %lu\n\r",info->unknown_frwd_asd);
  soc_sand_os_printf( "unknown_frwd_dst: %lu\n\r",info->unknown_frwd_dst);
  soc_sand_os_printf( "cos_profile: %lu\n\r",info->cos_profile);
  soc_sand_os_printf( "learn_enable: %lu\n\r",info->learn_enable);
  soc_sand_os_printf( "tpid_profile: %lu\n\r",info->tpid_profile);
  soc_sand_os_printf( "reverse_fec: %lu\n\r",info->reverse_fec);
  soc_sand_os_printf( "reverse_vc: %lu\n\r",info->reverse_vc);
  soc_sand_os_printf( "orientation_is_hub: %lu\n\r",info->orientation_is_hub);
  soc_sand_os_printf( "policer_enable: %lu\n\r",info->policer_enable);
  soc_sand_os_printf( "counter_enable: %lu\n\r",info->counter_enable);
  soc_sand_os_printf( "stat_tag_enable: %lu\n\r",info->stat_tag_enable);
  soc_sand_os_printf( "policer_ptr: %lu\n\r",info->policer_ptr);
  soc_sand_os_printf( "counter_ptr_stat_tag: %lu\n\r",info->counter_ptr_stat_tag);
  soc_sand_os_printf( "point_to_point: %lu\n\r",info->point_to_point);
  soc_sand_os_printf( "valid: %lu\n\r",info->valid);
exit:
  return;
}

void
  T20E_PP_AGING_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_AGING_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "age_status_0: %lu\n\r",info->age_status_0);
  soc_sand_os_printf( "age_status_1: %lu\n\r",info->age_status_1);
  soc_sand_os_printf( "age_status_2: %lu\n\r",info->age_status_2);
  soc_sand_os_printf( "age_status_3: %lu\n\r",info->age_status_3);
  soc_sand_os_printf( "age_status_4: %lu\n\r",info->age_status_4);
  soc_sand_os_printf( "age_status_5: %lu\n\r",info->age_status_5);
  soc_sand_os_printf( "age_status_6: %lu\n\r",info->age_status_6);
  soc_sand_os_printf( "age_status_7: %lu\n\r",info->age_status_7);
  soc_sand_os_printf( "age_status_8: %lu\n\r",info->age_status_8);
  soc_sand_os_printf( "age_status_9: %lu\n\r",info->age_status_9);
  soc_sand_os_printf( "age_status_10: %lu\n\r",info->age_status_10);
  soc_sand_os_printf( "age_status_11: %lu\n\r",info->age_status_11);
  soc_sand_os_printf( "age_status_12: %lu\n\r",info->age_status_12);
  soc_sand_os_printf( "age_status_13: %lu\n\r",info->age_status_13);
  soc_sand_os_printf( "age_status_14: %lu\n\r",info->age_status_14);
  soc_sand_os_printf( "age_status_15: %lu\n\r",info->age_status_15);
exit:
  return;
}

void
  T20E_PP_CNT_INP_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_CNT_INP_TBL_DATA *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind=0; ind<2; ++ind)
  {
    soc_sand_os_printf( "octets[%u]: %lu\n\r",ind,info->octets[ind]);
  }
  soc_sand_os_printf( "packets: %lu\n\r",info->packets);
exit:
  return;
}

void
  T20E_PP_EGRESS_COUNTERS_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_EGRESS_COUNTERS_TBL_DATA *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind=0; ind<2; ++ind)
  {
    soc_sand_os_printf( "octets[%u]: %lu\n\r",ind,info->octets[ind]);
  }
  soc_sand_os_printf( "packets: %lu\n\r",info->packets);
exit:
  return;
}

void
  T20E_PP_PLC_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_PLC_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "policer_e_fct: %lu\n\r",info->policer_e_fct);
  soc_sand_os_printf( "policer_c_fct: %lu\n\r",info->policer_c_fct);
  soc_sand_os_printf( "policer_ebs_ptr: %lu\n\r",info->policer_ebs_ptr);
  soc_sand_os_printf( "policer_cbs_ptr: %lu\n\r",info->policer_cbs_ptr);
  soc_sand_os_printf( "policer_eir_exponent: %lu\n\r",info->policer_eir_exponent);
  soc_sand_os_printf( "policer_eir_mantissa: %lu\n\r",info->policer_eir_mantissa);
  soc_sand_os_printf( "policer_cir_exponent: %lu\n\r",info->policer_cir_exponent);
  soc_sand_os_printf( "policer_cir_mantissa: %lu\n\r",info->policer_cir_mantissa);
  soc_sand_os_printf( "policer_color_aware: %lu\n\r",info->policer_color_aware);
  soc_sand_os_printf( "policer_parity: %lu\n\r",info->policer_parity);

exit:
  return;
}

void
  T20E_PP_EGRESS_MPLS_TUNNEL_TBL_DATA_print(
    SOC_SAND_IN  T20E_PP_EGRESS_MPLS_TUNNEL_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "orientation: %lu\n\r",info->orientation);
  soc_sand_os_printf( "mpls1_command: %lu\n\r",info->mpls1_command);
  soc_sand_os_printf( "mpls1_label: %lu\n\r",info->mpls1_label);
  soc_sand_os_printf( "mpls2_command: %lu\n\r",info->mpls2_command);
  soc_sand_os_printf( "mpls2_label: %lu\n\r",info->mpls2_label);
  soc_sand_os_printf( "out_vsi: %lu\n\r",info->out_vsi);
  soc_sand_os_printf( "next_eep: %lu\n\r",info->next_eep);
exit:
  return;
}

void
  T20E_MACT_FID_COUNTER_PROFILE_DB_TBL_DATA_print(
    SOC_SAND_IN  T20E_MACT_FID_COUNTER_PROFILE_DB_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "limit: %lu\n\r",info->limit);
  soc_sand_os_printf( "interrupt_en: %lu\n\r",info->interrupt_en);
  soc_sand_os_printf( "limit_en: %lu\n\r",info->limit_en);
exit:
  return;
}

void
  T20E_MACT_FID_COUNTER_DB_TBL_DATA_print(
    SOC_SAND_IN  T20E_MACT_FID_COUNTER_DB_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "entry_count: %lu\n\r",info->entry_count);
  soc_sand_os_printf( "profile_pointer: %lu\n\r",info->profile_pointer);
exit:
  return;
}
/*
void
  T20E_MACT_FLUSH_DB_TBL_DATA_print(
    SOC_SAND_IN  T20E_MACT_FLUSH_DB_TBL_DATA *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "compare_valid: %lu\n\r",info->compare_valid);
  soc_sand_os_printf( "compare_key_18_msbs_data: %lu\n\r",info->compare_key_18_msbs_data);
  soc_sand_os_printf( "compare_key_18_msbs_mask: %lu\n\r",info->compare_key_18_msbs_mask);
  for (ind=0; ind<2; ++ind)
  {
    soc_sand_os_printf( "compare_payload_data[%u]: %lu\n\r",ind,info->compare_payload_data[ind]);
  }
  for (ind=0; ind<2; ++ind)
  {
    soc_sand_os_printf( "compare_payload_mask[%u]: %lu\n\r",ind,info->compare_payload_mask[ind]);
  }
  soc_sand_os_printf( "action_drop: %lu\n\r",info->action_drop);
  for (ind=0; ind<2; ++ind)
  {
    soc_sand_os_printf( "action_transplant_payload_data[%u]: %lu\n\r",ind,info->action_transplant_payload_data[ind]);
  }
  for (ind=0; ind<2; ++ind)
  {
    soc_sand_os_printf( "action_transplant_payload_mask[%u]: %lu\n\r",ind,info->action_transplant_payload_mask[ind]);
  }
exit:
  return;
}
*/

#endif

#ifdef UI_T20E_ACC_PP
/********************************************************************
 *  Function handler: ac_access_entry_add_unsafe (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_ac_access_entry_add_unsafe(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  T20E_L2_LIF_AC_KEY
    prm_ac_key;
  T20E_PP_AC_KEY_TBL_DATA
    prm_payload;
  SOC_SAND_SUCCESS_FAILURE
    prm_success;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_ac_access_entry_add_unsafe";

  unit = t20e_get_default_unit();
  T20E_L2_LIF_AC_KEY_clear(&prm_ac_key);
  T20E_PP_AC_KEY_TBL_DATA_clear(&prm_payload);

  prm_ac_key.inner_vid = T20E_LIF_IGNORE_INNER_VID;
  prm_ac_key.outer_vid = T20E_LIF_IGNORE_OUTER_VID;


  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_AC_ACCESS_ENTRY_ADD_UNSAFE_AC_ACCESS_ENTRY_ADD_UNSAFE_AC_KEY_INNER_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_AC_ACCESS_ENTRY_ADD_UNSAFE_AC_ACCESS_ENTRY_ADD_UNSAFE_AC_KEY_INNER_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_key.inner_vid = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_AC_ACCESS_ENTRY_ADD_UNSAFE_AC_ACCESS_ENTRY_ADD_UNSAFE_AC_KEY_OUTER_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_AC_ACCESS_ENTRY_ADD_UNSAFE_AC_ACCESS_ENTRY_ADD_UNSAFE_AC_KEY_OUTER_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_key.outer_vid = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_AC_ACCESS_ENTRY_ADD_UNSAFE_AC_ACCESS_ENTRY_ADD_UNSAFE_AC_KEY_VLAN_DOMAIN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_AC_ACCESS_ENTRY_ADD_UNSAFE_AC_ACCESS_ENTRY_ADD_UNSAFE_AC_KEY_VLAN_DOMAIN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_key.vlan_domain = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ac_key after ac_access_entry_add_unsafe***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_AC_ACCESS_ENTRY_ADD_UNSAFE_AC_ACCESS_ENTRY_ADD_UNSAFE_PAYLOAD_MP_MAX_LEVEL_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_AC_ACCESS_ENTRY_ADD_UNSAFE_AC_ACCESS_ENTRY_ADD_UNSAFE_PAYLOAD_MP_MAX_LEVEL_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_payload.mp_max_level_valid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_AC_ACCESS_ENTRY_ADD_UNSAFE_AC_ACCESS_ENTRY_ADD_UNSAFE_PAYLOAD_MP_MAX_LEVEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_AC_ACCESS_ENTRY_ADD_UNSAFE_AC_ACCESS_ENTRY_ADD_UNSAFE_PAYLOAD_MP_MAX_LEVEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_payload.mp_max_level = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_AC_ACCESS_ENTRY_ADD_UNSAFE_AC_ACCESS_ENTRY_ADD_UNSAFE_PAYLOAD_MEP_INDEX_BASE_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_AC_ACCESS_ENTRY_ADD_UNSAFE_AC_ACCESS_ENTRY_ADD_UNSAFE_PAYLOAD_MEP_INDEX_BASE_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_payload.mep_index_base_valid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_AC_ACCESS_ENTRY_ADD_UNSAFE_AC_ACCESS_ENTRY_ADD_UNSAFE_PAYLOAD_MEP_INDEX_BASE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_AC_ACCESS_ENTRY_ADD_UNSAFE_AC_ACCESS_ENTRY_ADD_UNSAFE_PAYLOAD_MEP_INDEX_BASE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_payload.mep_index_base = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_AC_ACCESS_ENTRY_ADD_UNSAFE_AC_ACCESS_ENTRY_ADD_UNSAFE_PAYLOAD_ENABLE_ROUTING_MPLS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_AC_ACCESS_ENTRY_ADD_UNSAFE_AC_ACCESS_ENTRY_ADD_UNSAFE_PAYLOAD_ENABLE_ROUTING_MPLS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_payload.enable_routing_mpls = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_AC_ACCESS_ENTRY_ADD_UNSAFE_AC_ACCESS_ENTRY_ADD_UNSAFE_PAYLOAD_MY_MAC_LSB_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_AC_ACCESS_ENTRY_ADD_UNSAFE_AC_ACCESS_ENTRY_ADD_UNSAFE_PAYLOAD_MY_MAC_LSB_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_payload.my_mac_lsb = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_AC_ACCESS_ENTRY_ADD_UNSAFE_AC_ACCESS_ENTRY_ADD_UNSAFE_PAYLOAD_ENABLE_MY_MAC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_AC_ACCESS_ENTRY_ADD_UNSAFE_AC_ACCESS_ENTRY_ADD_UNSAFE_PAYLOAD_ENABLE_MY_MAC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_payload.enable_my_mac = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_AC_ACCESS_ENTRY_ADD_UNSAFE_AC_ACCESS_ENTRY_ADD_UNSAFE_PAYLOAD_STP_TOPOLOGY_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_AC_ACCESS_ENTRY_ADD_UNSAFE_AC_ACCESS_ENTRY_ADD_UNSAFE_PAYLOAD_STP_TOPOLOGY_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_payload.stp_topology_id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_AC_ACCESS_ENTRY_ADD_UNSAFE_AC_ACCESS_ENTRY_ADD_UNSAFE_PAYLOAD_AC_OP_CODE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_AC_ACCESS_ENTRY_ADD_UNSAFE_AC_ACCESS_ENTRY_ADD_UNSAFE_PAYLOAD_AC_OP_CODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_payload.ac_op_code = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_AC_ACCESS_ENTRY_ADD_UNSAFE_AC_ACCESS_ENTRY_ADD_UNSAFE_PAYLOAD_AC_BASE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_AC_ACCESS_ENTRY_ADD_UNSAFE_AC_ACCESS_ENTRY_ADD_UNSAFE_PAYLOAD_AC_BASE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_payload.ac_base = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_ac_access_entry_add_unsafe(
          unit,
          &prm_ac_key,
          &prm_payload,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_ac_access_entry_add_unsafe - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_ac_access_entry_add_unsafe");
    goto exit;
  }

  send_string_to_screen("--> success:", TRUE);



  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ac_access_entry_remove_unsafe (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_ac_access_entry_remove_unsafe(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  T20E_L2_LIF_AC_KEY
    prm_ac_key;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_ac_access_entry_remove_unsafe";

  unit = t20e_get_default_unit();
  T20E_L2_LIF_AC_KEY_clear(&prm_ac_key);
  prm_ac_key.inner_vid = T20E_LIF_IGNORE_INTERNAL_VID;
  prm_ac_key.outer_vid = T20E_LIF_IGNORE_OUTER_VID;


  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_AC_ACCESS_ENTRY_REMOVE_UNSAFE_AC_ACCESS_ENTRY_REMOVE_UNSAFE_AC_KEY_INNER_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_AC_ACCESS_ENTRY_REMOVE_UNSAFE_AC_ACCESS_ENTRY_REMOVE_UNSAFE_AC_KEY_INNER_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_key.inner_vid = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_AC_ACCESS_ENTRY_REMOVE_UNSAFE_AC_ACCESS_ENTRY_REMOVE_UNSAFE_AC_KEY_OUTER_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_AC_ACCESS_ENTRY_REMOVE_UNSAFE_AC_ACCESS_ENTRY_REMOVE_UNSAFE_AC_KEY_OUTER_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_key.outer_vid = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_AC_ACCESS_ENTRY_REMOVE_UNSAFE_AC_ACCESS_ENTRY_REMOVE_UNSAFE_AC_KEY_VLAN_DOMAIN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_AC_ACCESS_ENTRY_REMOVE_UNSAFE_AC_ACCESS_ENTRY_REMOVE_UNSAFE_AC_KEY_VLAN_DOMAIN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_key.vlan_domain = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ac_key after ac_access_entry_remove_unsafe***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_ac_access_entry_remove_unsafe(
          unit,
          &prm_ac_key
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_ac_access_entry_remove_unsafe - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_ac_access_entry_remove_unsafe");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ac_access_tbl_clear_unsafe (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_ac_access_tbl_clear_unsafe(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_ac_access_tbl_clear_unsafe";

  unit = t20e_get_default_unit();

  /* Get parameters */

  /* Call function */
  ret = t20e_ac_access_tbl_clear_unsafe(
          unit
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_ac_access_tbl_clear_unsafe - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_ac_access_tbl_clear_unsafe");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ac_access_internal_entry_add_unsafe (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_ac_access_internal_entry_add_unsafe(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  T20E_L2_LIF_AC_KEY
    prm_ac_key;
  T20E_PP_AC_KEY_TBL_DATA
    prm_payload;
  uint32
    prm_tbl_id;
  uint32
    prm_entry_indx;
  uint32
    prm_nof_steps;
  SOC_SAND_SUCCESS_FAILURE
    prm_success;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_ac_access_internal_entry_add_unsafe";

  unit = t20e_get_default_unit();
  T20E_L2_LIF_AC_KEY_clear(&prm_ac_key);
  prm_ac_key.inner_vid = T20E_LIF_IGNORE_INNER_VID;
  prm_ac_key.outer_vid = T20E_LIF_IGNORE_OUTER_VID;
  T20E_PP_AC_KEY_TBL_DATA_clear(&prm_payload);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_AC_KEY_INNER_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_AC_KEY_INNER_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_key.inner_vid = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_AC_KEY_OUTER_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_AC_KEY_OUTER_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_key.outer_vid = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_AC_KEY_VLAN_DOMAIN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_AC_KEY_VLAN_DOMAIN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_key.vlan_domain = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ac_key after ac_access_internal_entry_add_unsafe***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_PAYLOAD_MP_MAX_LEVEL_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_PAYLOAD_MP_MAX_LEVEL_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_payload.mp_max_level_valid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_PAYLOAD_MP_MAX_LEVEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_PAYLOAD_MP_MAX_LEVEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_payload.mp_max_level = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_PAYLOAD_MEP_INDEX_BASE_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_PAYLOAD_MEP_INDEX_BASE_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_payload.mep_index_base_valid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_PAYLOAD_MEP_INDEX_BASE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_PAYLOAD_MEP_INDEX_BASE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_payload.mep_index_base = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_PAYLOAD_ENABLE_ROUTING_MPLS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_PAYLOAD_ENABLE_ROUTING_MPLS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_payload.enable_routing_mpls = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_PAYLOAD_MY_MAC_LSB_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_PAYLOAD_MY_MAC_LSB_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_payload.my_mac_lsb = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_PAYLOAD_ENABLE_MY_MAC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_PAYLOAD_ENABLE_MY_MAC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_payload.enable_my_mac = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_PAYLOAD_STP_TOPOLOGY_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_PAYLOAD_STP_TOPOLOGY_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_payload.stp_topology_id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_PAYLOAD_AC_OP_CODE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_PAYLOAD_AC_OP_CODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_payload.ac_op_code = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_PAYLOAD_AC_BASE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_PAYLOAD_AC_BASE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_payload.ac_base = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_ac_access_internal_entry_add_unsafe(
          unit,
          &prm_ac_key,
          &prm_payload,
          &prm_tbl_id,
          &prm_entry_indx,
          &prm_nof_steps,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_ac_access_internal_entry_add_unsafe - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_ac_access_internal_entry_add_unsafe");
    goto exit;
  }

  send_string_to_screen("--> tbl_id:", TRUE);
  soc_sand_os_printf( "tbl_id: %lu\n",prm_tbl_id);

  send_string_to_screen("--> entry_indx:", TRUE);
  soc_sand_os_printf( "entry_indx: %lu\n",prm_entry_indx);

  send_string_to_screen("--> nof_steps:", TRUE);
  soc_sand_os_printf( "nof_steps: %lu\n",prm_nof_steps);

  send_string_to_screen("--> success:", TRUE);



  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ac_access_get_block_unsafe (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_ac_access_get_block_unsafe(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_TABLE_BLOCK_RANGE
    prm_range;
  T20E_L2_LIF_AC_KEY
    prm_ac_keys;
  uint32
    prm_nof_entries;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_ac_access_get_block_unsafe";

  unit = t20e_get_default_unit();
  T20E_L2_LIF_AC_KEY_clear(&prm_ac_keys);

  /* Get parameters */

  /* Call function */
  ret = t20e_ac_access_get_block_unsafe(
          unit,
          &prm_range,
          &prm_ac_keys,
          &prm_nof_entries
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_ac_access_get_block_unsafe - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_ac_access_get_block_unsafe");
    goto exit;
  }

  send_string_to_screen("--> ac_keys:", TRUE);
  T20E_L2_LIF_AC_KEY_print(&prm_ac_keys);

  send_string_to_screen("--> nof_entries:", TRUE);
  soc_sand_os_printf( "nof_entries: %lu\n",prm_nof_entries);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ac_access_hw_set_callback_unsafe (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_ac_access_hw_set_callback_unsafe(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_tbl_ndx;
  uint32
    prm_entry_offset_ndx;
  uint8
    prm_payload[5];
  uint8
    prm_verifier[5];

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_ac_access_hw_set_callback_unsafe";

  unit = t20e_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_AC_ACCESS_HW_SET_CALLBACK_UNSAFE_AC_ACCESS_HW_SET_CALLBACK_UNSAFE_TBL_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_AC_ACCESS_HW_SET_CALLBACK_UNSAFE_AC_ACCESS_HW_SET_CALLBACK_UNSAFE_TBL_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter tbl_ndx after ac_access_hw_set_callback_unsafe***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_AC_ACCESS_HW_SET_CALLBACK_UNSAFE_AC_ACCESS_HW_SET_CALLBACK_UNSAFE_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_AC_ACCESS_HW_SET_CALLBACK_UNSAFE_AC_ACCESS_HW_SET_CALLBACK_UNSAFE_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after ac_access_hw_set_callback_unsafe***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_AC_ACCESS_HW_SET_CALLBACK_UNSAFE_AC_ACCESS_HW_SET_CALLBACK_UNSAFE_PAYLOAD_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_AC_ACCESS_HW_SET_CALLBACK_UNSAFE_AC_ACCESS_HW_SET_CALLBACK_UNSAFE_PAYLOAD_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_payload[0] = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_AC_ACCESS_HW_SET_CALLBACK_UNSAFE_AC_ACCESS_HW_SET_CALLBACK_UNSAFE_VERIFIER_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_AC_ACCESS_HW_SET_CALLBACK_UNSAFE_AC_ACCESS_HW_SET_CALLBACK_UNSAFE_VERIFIER_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_verifier[0] = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_ac_access_hw_set_callback_unsafe(
          unit,
          prm_tbl_ndx,
          prm_entry_offset_ndx,
          prm_payload,
          prm_verifier
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_ac_access_hw_set_callback_unsafe - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_ac_access_hw_set_callback_unsafe");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ac_access_hw_get_callback_unsafe (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_ac_access_hw_get_callback_unsafe(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_tbl_ndx;
  uint32
    prm_entry_offset_ndx;
  uint8
    prm_payload[5];
  uint32
    print_indx;
  uint8
    prm_verifier[5];

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_ac_access_hw_get_callback_unsafe";

  unit = t20e_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_AC_ACCESS_HW_GET_CALLBACK_UNSAFE_AC_ACCESS_HW_GET_CALLBACK_UNSAFE_TBL_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_AC_ACCESS_HW_GET_CALLBACK_UNSAFE_AC_ACCESS_HW_GET_CALLBACK_UNSAFE_TBL_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter tbl_ndx after ac_access_hw_get_callback_unsafe***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_AC_ACCESS_HW_GET_CALLBACK_UNSAFE_AC_ACCESS_HW_GET_CALLBACK_UNSAFE_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_AC_ACCESS_HW_GET_CALLBACK_UNSAFE_AC_ACCESS_HW_GET_CALLBACK_UNSAFE_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after ac_access_hw_get_callback_unsafe***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_ac_access_hw_get_callback_unsafe(
          unit,
          prm_tbl_ndx,
          prm_entry_offset_ndx,
          prm_payload,
          prm_verifier
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_ac_access_hw_get_callback_unsafe - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_ac_access_hw_get_callback_unsafe");
    goto exit;
  }

  send_string_to_screen("--> payload(lsb on left):", TRUE);
  for (print_indx = 0 ; print_indx <5; ++print_indx)
  {
    soc_sand_os_printf( "0x%08x  ", prm_payload[print_indx]);
  }
  soc_sand_os_printf( "\n");

  send_string_to_screen("--> verifier(lsb on left):", TRUE);
  for (print_indx = 0 ; print_indx <5; ++print_indx)
  {
    soc_sand_os_printf( "0x%08x  ", prm_verifier[print_indx]);
  }
  soc_sand_os_printf( "\n");


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_lem_da_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_lem_da_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  T20E_PP_LEM_TBL_KEY
    prm_entry_key;
  T20E_PP_MAC_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_lem_da_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_LEM_TBL_KEY_clear(&prm_entry_key);
  T20E_PP_MAC_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LEM_DA_TBL_SET_PP_LEM_DA_TBL_SET_ENTRY_KEY_INFO_ILM_INRIF_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_LEM_DA_TBL_SET_PP_LEM_DA_TBL_SET_ENTRY_KEY_INFO_ILM_INRIF_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_key.info.ilm.inrif = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LEM_DA_TBL_SET_PP_LEM_DA_TBL_SET_ENTRY_KEY_INFO_ILM_IN_LOCAL_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_LEM_DA_TBL_SET_PP_LEM_DA_TBL_SET_ENTRY_KEY_INFO_ILM_IN_LOCAL_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_key.info.ilm.in_local_port = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LEM_DA_TBL_SET_PP_LEM_DA_TBL_SET_ENTRY_KEY_INFO_ILM_MAPPED_EXP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_LEM_DA_TBL_SET_PP_LEM_DA_TBL_SET_ENTRY_KEY_INFO_ILM_MAPPED_EXP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_key.info.ilm.mapped_exp = (uint8)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LEM_DA_TBL_SET_PP_LEM_DA_TBL_SET_ENTRY_KEY_INFO_ILM_IN_LABEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_LEM_DA_TBL_SET_PP_LEM_DA_TBL_SET_ENTRY_KEY_INFO_ILM_IN_LABEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_key.info.ilm.in_label = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LEM_DA_TBL_SET_PP_LEM_DA_TBL_SET_ENTRY_KEY_INFO_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_LEM_DA_TBL_SET_PP_LEM_DA_TBL_SET_ENTRY_KEY_INFO_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_key.info.offset = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LEM_DA_TBL_SET_PP_LEM_DA_TBL_SET_ENTRY_KEY_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_T20E_PP_LEM_DA_TBL_SET_PP_LEM_DA_TBL_SET_ENTRY_KEY_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_key.type = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_key after pp_lem_da_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_lem_da_tbl_get_unsafe(
          unit,
          &prm_entry_key,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_lem_da_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_lem_da_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LEM_DA_TBL_SET_PP_LEM_DA_TBL_SET_TBL_DATA_IS_DYNAMIC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_LEM_DA_TBL_SET_PP_LEM_DA_TBL_SET_TBL_DATA_IS_DYNAMIC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.is_dynamic = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LEM_DA_TBL_SET_PP_LEM_DA_TBL_SET_TBL_DATA_DESTINATION_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_LEM_DA_TBL_SET_PP_LEM_DA_TBL_SET_TBL_DATA_DESTINATION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.destination = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LEM_DA_TBL_SET_PP_LEM_DA_TBL_SET_TBL_DATA_ASD_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_LEM_DA_TBL_SET_PP_LEM_DA_TBL_SET_TBL_DATA_ASD_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.asd = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LEM_DA_TBL_SET_PP_LEM_DA_TBL_SET_TBL_DATA_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_LEM_DA_TBL_SET_PP_LEM_DA_TBL_SET_TBL_DATA_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.valid = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_lem_da_tbl_set_unsafe(
          unit,
          &prm_entry_key,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_lem_da_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_lem_da_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_lem_da_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_lem_da_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  T20E_PP_LEM_TBL_KEY
    prm_entry_key;
  T20E_PP_MAC_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_lem_da_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_LEM_TBL_KEY_clear(&prm_entry_key);
  T20E_PP_MAC_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LEM_DA_TBL_GET_PP_LEM_DA_TBL_GET_ENTRY_KEY_INFO_ILM_INRIF_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_LEM_DA_TBL_GET_PP_LEM_DA_TBL_GET_ENTRY_KEY_INFO_ILM_INRIF_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_key.info.ilm.inrif = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LEM_DA_TBL_GET_PP_LEM_DA_TBL_GET_ENTRY_KEY_INFO_ILM_IN_LOCAL_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_LEM_DA_TBL_GET_PP_LEM_DA_TBL_GET_ENTRY_KEY_INFO_ILM_IN_LOCAL_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_key.info.ilm.in_local_port = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LEM_DA_TBL_GET_PP_LEM_DA_TBL_GET_ENTRY_KEY_INFO_ILM_MAPPED_EXP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_LEM_DA_TBL_GET_PP_LEM_DA_TBL_GET_ENTRY_KEY_INFO_ILM_MAPPED_EXP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_key.info.ilm.mapped_exp = (uint8)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LEM_DA_TBL_GET_PP_LEM_DA_TBL_GET_ENTRY_KEY_INFO_ILM_IN_LABEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_LEM_DA_TBL_GET_PP_LEM_DA_TBL_GET_ENTRY_KEY_INFO_ILM_IN_LABEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_key.info.ilm.in_label = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LEM_DA_TBL_GET_PP_LEM_DA_TBL_GET_ENTRY_KEY_INFO_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_LEM_DA_TBL_GET_PP_LEM_DA_TBL_GET_ENTRY_KEY_INFO_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_key.info.offset = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LEM_DA_TBL_GET_PP_LEM_DA_TBL_GET_ENTRY_KEY_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_T20E_PP_LEM_DA_TBL_GET_PP_LEM_DA_TBL_GET_ENTRY_KEY_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_key.type = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_key after pp_lem_da_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_lem_da_tbl_get_unsafe(
          unit,
          &prm_entry_key,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_lem_da_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_lem_da_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_MAC_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_lem_sa_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_lem_sa_tbl_set(
    CURRENT_LINE *current_line
  )
{
  T20E_PP_LEM_TBL_KEY
    prm_entry_key;
  T20E_PP_MAC_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_lem_sa_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_LEM_TBL_KEY_clear(&prm_entry_key);
  T20E_PP_MAC_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LEM_SA_TBL_SET_PP_LEM_SA_TBL_SET_ENTRY_KEY_INFO_ILM_INRIF_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_LEM_SA_TBL_SET_PP_LEM_SA_TBL_SET_ENTRY_KEY_INFO_ILM_INRIF_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_key.info.ilm.inrif = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LEM_SA_TBL_SET_PP_LEM_SA_TBL_SET_ENTRY_KEY_INFO_ILM_IN_LOCAL_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_LEM_SA_TBL_SET_PP_LEM_SA_TBL_SET_ENTRY_KEY_INFO_ILM_IN_LOCAL_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_key.info.ilm.in_local_port = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LEM_SA_TBL_SET_PP_LEM_SA_TBL_SET_ENTRY_KEY_INFO_ILM_MAPPED_EXP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_LEM_SA_TBL_SET_PP_LEM_SA_TBL_SET_ENTRY_KEY_INFO_ILM_MAPPED_EXP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_key.info.ilm.mapped_exp = (uint8)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LEM_SA_TBL_SET_PP_LEM_SA_TBL_SET_ENTRY_KEY_INFO_ILM_IN_LABEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_LEM_SA_TBL_SET_PP_LEM_SA_TBL_SET_ENTRY_KEY_INFO_ILM_IN_LABEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_key.info.ilm.in_label = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LEM_SA_TBL_SET_PP_LEM_SA_TBL_SET_ENTRY_KEY_INFO_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_LEM_SA_TBL_SET_PP_LEM_SA_TBL_SET_ENTRY_KEY_INFO_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_key.info.offset = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LEM_SA_TBL_SET_PP_LEM_SA_TBL_SET_ENTRY_KEY_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_T20E_PP_LEM_SA_TBL_SET_PP_LEM_SA_TBL_SET_ENTRY_KEY_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_key.type = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_key after pp_lem_sa_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first
  ret = t20e_pp_lem_sa_tbl_get_unsafe(
          unit,
          &prm_entry_key,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_lem_sa_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_lem_sa_tbl_get");
    goto exit;
  } */

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LEM_SA_TBL_SET_PP_LEM_SA_TBL_SET_TBL_DATA_IS_DYNAMIC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_LEM_SA_TBL_SET_PP_LEM_SA_TBL_SET_TBL_DATA_IS_DYNAMIC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.is_dynamic = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LEM_SA_TBL_SET_PP_LEM_SA_TBL_SET_TBL_DATA_DESTINATION_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_LEM_SA_TBL_SET_PP_LEM_SA_TBL_SET_TBL_DATA_DESTINATION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.destination = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LEM_SA_TBL_SET_PP_LEM_SA_TBL_SET_TBL_DATA_ASD_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_LEM_SA_TBL_SET_PP_LEM_SA_TBL_SET_TBL_DATA_ASD_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.asd = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LEM_SA_TBL_SET_PP_LEM_SA_TBL_SET_TBL_DATA_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_LEM_SA_TBL_SET_PP_LEM_SA_TBL_SET_TBL_DATA_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.valid = (uint32)param_val->value.ulong_value;
  }


  /* Call function
  ret = t20e_pp_lem_sa_tbl_set_unsafe(
          unit,
          &prm_entry_key,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_lem_sa_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_lem_sa_tbl_set");
    goto exit;
  }

  */
  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_lem_sa_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_lem_sa_tbl_get(
    CURRENT_LINE *current_line
  )
{
  T20E_PP_LEM_TBL_KEY
    prm_entry_key;
  T20E_PP_MAC_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_lem_sa_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_LEM_TBL_KEY_clear(&prm_entry_key);
  T20E_PP_MAC_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LEM_SA_TBL_GET_PP_LEM_SA_TBL_GET_ENTRY_KEY_INFO_ILM_INRIF_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_LEM_SA_TBL_GET_PP_LEM_SA_TBL_GET_ENTRY_KEY_INFO_ILM_INRIF_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_key.info.ilm.inrif = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LEM_SA_TBL_GET_PP_LEM_SA_TBL_GET_ENTRY_KEY_INFO_ILM_IN_LOCAL_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_LEM_SA_TBL_GET_PP_LEM_SA_TBL_GET_ENTRY_KEY_INFO_ILM_IN_LOCAL_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_key.info.ilm.in_local_port = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LEM_SA_TBL_GET_PP_LEM_SA_TBL_GET_ENTRY_KEY_INFO_ILM_MAPPED_EXP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_LEM_SA_TBL_GET_PP_LEM_SA_TBL_GET_ENTRY_KEY_INFO_ILM_MAPPED_EXP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_key.info.ilm.mapped_exp = (uint8)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LEM_SA_TBL_GET_PP_LEM_SA_TBL_GET_ENTRY_KEY_INFO_ILM_IN_LABEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_LEM_SA_TBL_GET_PP_LEM_SA_TBL_GET_ENTRY_KEY_INFO_ILM_IN_LABEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_key.info.ilm.in_label = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LEM_SA_TBL_GET_PP_LEM_SA_TBL_GET_ENTRY_KEY_INFO_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_LEM_SA_TBL_GET_PP_LEM_SA_TBL_GET_ENTRY_KEY_INFO_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_key.info.offset = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LEM_SA_TBL_GET_PP_LEM_SA_TBL_GET_ENTRY_KEY_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_T20E_PP_LEM_SA_TBL_GET_PP_LEM_SA_TBL_GET_ENTRY_KEY_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_key.type = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_key after pp_lem_sa_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function
  ret = t20e_pp_lem_sa_tbl_get_unsafe(
          unit,
          &prm_entry_key,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_lem_sa_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_lem_sa_tbl_get");
    goto exit;
  }*/

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_MAC_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_set_reps_for_tbl_unsafe (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_set_reps_for_tbl_unsafe(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_nof_reps;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_set_reps_for_tbl_unsafe";

  unit = t20e_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_SET_REPS_FOR_TBL_UNSAFE_PP_SET_REPS_FOR_TBL_UNSAFE_NOF_REPS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_SET_REPS_FOR_TBL_UNSAFE_PP_SET_REPS_FOR_TBL_UNSAFE_NOF_REPS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_nof_reps = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_set_reps_for_tbl_unsafe(
          unit,
          prm_nof_reps
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_set_reps_for_tbl_unsafe - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_set_reps_for_tbl_unsafe");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_llvp_classification_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_llvp_classification_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_port_profile_ndx;
  uint32
    prm_out_tag_encap_ndx;
  uint32
    prm_out_is_prio_ndx;
  uint32
    prm_in_tag_encap_ndx;
  T20E_PP_LLVP_CLASSIFICATION_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_llvp_classification_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_LLVP_CLASSIFICATION_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LLVP_CLASSIFICATION_TBL_SET_PP_LLVP_CLASSIFICATION_TBL_SET_PORT_PROFILE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_LLVP_CLASSIFICATION_TBL_SET_PP_LLVP_CLASSIFICATION_TBL_SET_PORT_PROFILE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_profile_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter port_profile_ndx after pp_llvp_classification_tbl_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LLVP_CLASSIFICATION_TBL_SET_PP_LLVP_CLASSIFICATION_TBL_SET_OUT_TAG_ENCAP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_LLVP_CLASSIFICATION_TBL_SET_PP_LLVP_CLASSIFICATION_TBL_SET_OUT_TAG_ENCAP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_out_tag_encap_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter out_tag_encap_ndx after pp_llvp_classification_tbl_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LLVP_CLASSIFICATION_TBL_SET_PP_LLVP_CLASSIFICATION_TBL_SET_OUT_IS_PRIO_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_LLVP_CLASSIFICATION_TBL_SET_PP_LLVP_CLASSIFICATION_TBL_SET_OUT_IS_PRIO_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_out_is_prio_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter out_is_prio_ndx after pp_llvp_classification_tbl_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LLVP_CLASSIFICATION_TBL_SET_PP_LLVP_CLASSIFICATION_TBL_SET_IN_TAG_ENCAP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_LLVP_CLASSIFICATION_TBL_SET_PP_LLVP_CLASSIFICATION_TBL_SET_IN_TAG_ENCAP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_in_tag_encap_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter in_tag_encap_ndx after pp_llvp_classification_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_llvp_classification_tbl_get(
          unit,
          T20E_TBL_LLVP_ENTRY_INDX(prm_port_profile_ndx,prm_out_tag_encap_ndx,prm_out_is_prio_ndx,prm_in_tag_encap_ndx),
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_llvp_classification_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_llvp_classification_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LLVP_CLASSIFICATION_TBL_SET_PP_LLVP_CLASSIFICATION_TBL_SET_TBL_DATA_USE_TWO_VLANS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_LLVP_CLASSIFICATION_TBL_SET_PP_LLVP_CLASSIFICATION_TBL_SET_TBL_DATA_USE_TWO_VLANS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.ac_key_encoding = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LLVP_CLASSIFICATION_TBL_SET_PP_LLVP_CLASSIFICATION_TBL_SET_TBL_DATA_TAG_EDIT_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_LLVP_CLASSIFICATION_TBL_SET_PP_LLVP_CLASSIFICATION_TBL_SET_TBL_DATA_TAG_EDIT_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.tag_edit_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LLVP_CLASSIFICATION_TBL_SET_PP_LLVP_CLASSIFICATION_TBL_SET_TBL_DATA_ACCEPTABLE_FRAME_TYPE_ACTION_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_LLVP_CLASSIFICATION_TBL_SET_PP_LLVP_CLASSIFICATION_TBL_SET_TBL_DATA_ACCEPTABLE_FRAME_TYPE_ACTION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.acceptable_frame_type_action = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LLVP_CLASSIFICATION_TBL_SET_PP_LLVP_CLASSIFICATION_TBL_SET_TBL_DATA_INCOMING_S_TAG_EXIST_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_LLVP_CLASSIFICATION_TBL_SET_PP_LLVP_CLASSIFICATION_TBL_SET_TBL_DATA_INCOMING_S_TAG_EXIST_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.incoming_s_tag_exist = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LLVP_CLASSIFICATION_TBL_SET_PP_LLVP_CLASSIFICATION_TBL_SET_TBL_DATA_INCOMING_TAG_EXIST_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_LLVP_CLASSIFICATION_TBL_SET_PP_LLVP_CLASSIFICATION_TBL_SET_TBL_DATA_INCOMING_TAG_EXIST_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.incoming_tag_exist = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LLVP_CLASSIFICATION_TBL_SET_PP_LLVP_CLASSIFICATION_TBL_SET_TBL_DATA_INCOMING_VID_EXIST_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_LLVP_CLASSIFICATION_TBL_SET_PP_LLVP_CLASSIFICATION_TBL_SET_TBL_DATA_INCOMING_VID_EXIST_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.incoming_vid_exist = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_llvp_classification_tbl_set(
          unit,
          T20E_TBL_LLVP_ENTRY_INDX(prm_port_profile_ndx,prm_out_tag_encap_ndx,prm_out_is_prio_ndx,prm_in_tag_encap_ndx),
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_llvp_classification_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_llvp_classification_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_llvp_classification_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_llvp_classification_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_port_profile_ndx;
  uint32
    prm_out_tag_encap_ndx;
  uint32
    prm_out_is_prio_ndx;
  uint32
    prm_in_tag_encap_ndx;
  T20E_PP_LLVP_CLASSIFICATION_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_llvp_classification_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_LLVP_CLASSIFICATION_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LLVP_CLASSIFICATION_TBL_GET_PP_LLVP_CLASSIFICATION_TBL_GET_PORT_PROFILE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_LLVP_CLASSIFICATION_TBL_GET_PP_LLVP_CLASSIFICATION_TBL_GET_PORT_PROFILE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_profile_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter port_profile_ndx after pp_llvp_classification_tbl_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LLVP_CLASSIFICATION_TBL_GET_PP_LLVP_CLASSIFICATION_TBL_GET_OUT_TAG_ENCAP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_LLVP_CLASSIFICATION_TBL_GET_PP_LLVP_CLASSIFICATION_TBL_GET_OUT_TAG_ENCAP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_out_tag_encap_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter out_tag_encap_ndx after pp_llvp_classification_tbl_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LLVP_CLASSIFICATION_TBL_GET_PP_LLVP_CLASSIFICATION_TBL_GET_OUT_IS_PRIO_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_LLVP_CLASSIFICATION_TBL_GET_PP_LLVP_CLASSIFICATION_TBL_GET_OUT_IS_PRIO_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_out_is_prio_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter out_is_prio_ndx after pp_llvp_classification_tbl_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LLVP_CLASSIFICATION_TBL_GET_PP_LLVP_CLASSIFICATION_TBL_GET_IN_TAG_ENCAP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_LLVP_CLASSIFICATION_TBL_GET_PP_LLVP_CLASSIFICATION_TBL_GET_IN_TAG_ENCAP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_in_tag_encap_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter in_tag_encap_ndx after pp_llvp_classification_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_llvp_classification_tbl_get(
          unit,
          T20E_TBL_LLVP_ENTRY_INDX(prm_port_profile_ndx,prm_out_tag_encap_ndx,prm_out_is_prio_ndx,prm_in_tag_encap_ndx),
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_llvp_classification_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_llvp_classification_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_LLVP_CLASSIFICATION_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_ipv4_subnet_cami_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_ipv4_subnet_cami_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_IPV4_SUBNET_CAMI_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_ipv4_subnet_cami_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_IPV4_SUBNET_CAMI_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_IPV4_SUBNET_CAMI_TBL_SET_PP_IPV4_SUBNET_CAMI_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_IPV4_SUBNET_CAMI_TBL_SET_PP_IPV4_SUBNET_CAMI_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_ipv4_subnet_cami_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_ipv4_subnet_cami_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_ipv4_subnet_cami_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_ipv4_subnet_cami_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_IPV4_SUBNET_CAMI_TBL_SET_PP_IPV4_SUBNET_CAMI_TBL_SET_TBL_DATA_IPV4_SUBNET_BMAP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_IPV4_SUBNET_CAMI_TBL_SET_PP_IPV4_SUBNET_CAMI_TBL_SET_TBL_DATA_IPV4_SUBNET_BMAP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.ipv4_subnet_bmap = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_ipv4_subnet_cami_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_ipv4_subnet_cami_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_ipv4_subnet_cami_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_ipv4_subnet_cami_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_ipv4_subnet_cami_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_IPV4_SUBNET_CAMI_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_ipv4_subnet_cami_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_IPV4_SUBNET_CAMI_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_IPV4_SUBNET_CAMI_TBL_GET_PP_IPV4_SUBNET_CAMI_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_IPV4_SUBNET_CAMI_TBL_GET_PP_IPV4_SUBNET_CAMI_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_ipv4_subnet_cami_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_ipv4_subnet_cami_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_ipv4_subnet_cami_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_ipv4_subnet_cami_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_IPV4_SUBNET_CAMI_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_ipv4_subnet_table_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_ipv4_subnet_table_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_IPV4_SUBNET_TABLE_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_ipv4_subnet_table_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_IPV4_SUBNET_TABLE_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_IPV4_SUBNET_TABLE_TBL_SET_PP_IPV4_SUBNET_TABLE_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_IPV4_SUBNET_TABLE_TBL_SET_PP_IPV4_SUBNET_TABLE_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_ipv4_subnet_table_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_ipv4_subnet_table_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_ipv4_subnet_table_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_ipv4_subnet_table_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_IPV4_SUBNET_TABLE_TBL_SET_PP_IPV4_SUBNET_TABLE_TBL_SET_TBL_DATA_IPV4_SUBNET_TC_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_IPV4_SUBNET_TABLE_TBL_SET_PP_IPV4_SUBNET_TABLE_TBL_SET_TBL_DATA_IPV4_SUBNET_TC_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.ipv4_subnet_tc_valid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_IPV4_SUBNET_TABLE_TBL_SET_PP_IPV4_SUBNET_TABLE_TBL_SET_TBL_DATA_IPV4_SUBNET_TC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_IPV4_SUBNET_TABLE_TBL_SET_PP_IPV4_SUBNET_TABLE_TBL_SET_TBL_DATA_IPV4_SUBNET_TC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.ipv4_subnet_tc = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_IPV4_SUBNET_TABLE_TBL_SET_PP_IPV4_SUBNET_TABLE_TBL_SET_TBL_DATA_IPV4_SUBNET_VID_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_IPV4_SUBNET_TABLE_TBL_SET_PP_IPV4_SUBNET_TABLE_TBL_SET_TBL_DATA_IPV4_SUBNET_VID_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.ipv4_subnet_vid_valid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_IPV4_SUBNET_TABLE_TBL_SET_PP_IPV4_SUBNET_TABLE_TBL_SET_TBL_DATA_IPV4_SUBNET_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_IPV4_SUBNET_TABLE_TBL_SET_PP_IPV4_SUBNET_TABLE_TBL_SET_TBL_DATA_IPV4_SUBNET_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.ipv4_subnet_vid = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_ipv4_subnet_table_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_ipv4_subnet_table_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_ipv4_subnet_table_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_ipv4_subnet_table_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_ipv4_subnet_table_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_IPV4_SUBNET_TABLE_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_ipv4_subnet_table_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_IPV4_SUBNET_TABLE_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_IPV4_SUBNET_TABLE_TBL_GET_PP_IPV4_SUBNET_TABLE_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_IPV4_SUBNET_TABLE_TBL_GET_PP_IPV4_SUBNET_TABLE_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_ipv4_subnet_table_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_ipv4_subnet_table_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_ipv4_subnet_table_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_ipv4_subnet_table_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_IPV4_SUBNET_TABLE_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_port_protocol_cami_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_port_protocol_cami_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_PORT_PROTOCOL_CAMI_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_port_protocol_cami_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_PORT_PROTOCOL_CAMI_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PORT_PROTOCOL_CAMI_TBL_SET_PP_PORT_PROTOCOL_CAMI_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PORT_PROTOCOL_CAMI_TBL_SET_PP_PORT_PROTOCOL_CAMI_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_port_protocol_cami_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_port_protocol_cami_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_port_protocol_cami_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_port_protocol_cami_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PORT_PROTOCOL_CAMI_TBL_SET_PP_PORT_PROTOCOL_CAMI_TBL_SET_TBL_DATA_ETHERNET_TYPE_BMAP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PORT_PROTOCOL_CAMI_TBL_SET_PP_PORT_PROTOCOL_CAMI_TBL_SET_TBL_DATA_ETHERNET_TYPE_BMAP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.ethernet_type_bmap = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_port_protocol_cami_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_port_protocol_cami_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_port_protocol_cami_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_port_protocol_cami_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_port_protocol_cami_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_PORT_PROTOCOL_CAMI_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_port_protocol_cami_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_PORT_PROTOCOL_CAMI_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PORT_PROTOCOL_CAMI_TBL_GET_PP_PORT_PROTOCOL_CAMI_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PORT_PROTOCOL_CAMI_TBL_GET_PP_PORT_PROTOCOL_CAMI_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_port_protocol_cami_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_port_protocol_cami_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_port_protocol_cami_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_port_protocol_cami_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_PORT_PROTOCOL_CAMI_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_port_protocol_to_vid_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_port_protocol_to_vid_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_PORT_PROTOCOL_TO_VID_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_port_protocol_to_vid_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_PORT_PROTOCOL_TO_VID_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PORT_PROTOCOL_TO_VID_TBL_SET_PP_PORT_PROTOCOL_TO_VID_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PORT_PROTOCOL_TO_VID_TBL_SET_PP_PORT_PROTOCOL_TO_VID_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_port_protocol_to_vid_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_port_protocol_to_vid_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_port_protocol_to_vid_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_port_protocol_to_vid_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PORT_PROTOCOL_TO_VID_TBL_SET_PP_PORT_PROTOCOL_TO_VID_TBL_SET_TBL_DATA_TRAFFIC_CLASS_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PORT_PROTOCOL_TO_VID_TBL_SET_PP_PORT_PROTOCOL_TO_VID_TBL_SET_TBL_DATA_TRAFFIC_CLASS_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.traffic_class_valid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PORT_PROTOCOL_TO_VID_TBL_SET_PP_PORT_PROTOCOL_TO_VID_TBL_SET_TBL_DATA_TRAFFIC_CLASS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PORT_PROTOCOL_TO_VID_TBL_SET_PP_PORT_PROTOCOL_TO_VID_TBL_SET_TBL_DATA_TRAFFIC_CLASS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.traffic_class = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PORT_PROTOCOL_TO_VID_TBL_SET_PP_PORT_PROTOCOL_TO_VID_TBL_SET_TBL_DATA_VID_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PORT_PROTOCOL_TO_VID_TBL_SET_PP_PORT_PROTOCOL_TO_VID_TBL_SET_TBL_DATA_VID_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.vid_valid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PORT_PROTOCOL_TO_VID_TBL_SET_PP_PORT_PROTOCOL_TO_VID_TBL_SET_TBL_DATA_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PORT_PROTOCOL_TO_VID_TBL_SET_PP_PORT_PROTOCOL_TO_VID_TBL_SET_TBL_DATA_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.vid = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_port_protocol_to_vid_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_port_protocol_to_vid_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_port_protocol_to_vid_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_port_protocol_to_vid_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_port_protocol_to_vid_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_PORT_PROTOCOL_TO_VID_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_port_protocol_to_vid_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_PORT_PROTOCOL_TO_VID_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PORT_PROTOCOL_TO_VID_TBL_GET_PP_PORT_PROTOCOL_TO_VID_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PORT_PROTOCOL_TO_VID_TBL_GET_PP_PORT_PROTOCOL_TO_VID_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_port_protocol_to_vid_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_port_protocol_to_vid_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_port_protocol_to_vid_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_port_protocol_to_vid_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_PORT_PROTOCOL_TO_VID_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_vlan_port_membership_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_vlan_port_membership_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_VLAN_PORT_MEMBERSHIP_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_vlan_port_membership_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_VLAN_PORT_MEMBERSHIP_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_VLAN_PORT_MEMBERSHIP_TBL_SET_PP_VLAN_PORT_MEMBERSHIP_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_VLAN_PORT_MEMBERSHIP_TBL_SET_PP_VLAN_PORT_MEMBERSHIP_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_vlan_port_membership_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_vlan_port_membership_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_vlan_port_membership_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_vlan_port_membership_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_VLAN_PORT_MEMBERSHIP_TBL_SET_PP_VLAN_PORT_MEMBERSHIP_TBL_SET_TBL_DATA_PORT_BMAP_MEMBERSHIP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_VLAN_PORT_MEMBERSHIP_TBL_SET_PP_VLAN_PORT_MEMBERSHIP_TBL_SET_TBL_DATA_PORT_BMAP_MEMBERSHIP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.port_bmap_membership = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_vlan_port_membership_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_vlan_port_membership_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_vlan_port_membership_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_vlan_port_membership_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_vlan_port_membership_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_VLAN_PORT_MEMBERSHIP_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_vlan_port_membership_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_VLAN_PORT_MEMBERSHIP_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_VLAN_PORT_MEMBERSHIP_TBL_GET_PP_VLAN_PORT_MEMBERSHIP_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_VLAN_PORT_MEMBERSHIP_TBL_GET_PP_VLAN_PORT_MEMBERSHIP_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_vlan_port_membership_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_vlan_port_membership_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_vlan_port_membership_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_vlan_port_membership_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_VLAN_PORT_MEMBERSHIP_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_reserved_mc_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_reserved_mc_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_RESERVED_MC_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_reserved_mc_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_RESERVED_MC_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_RESERVED_MC_TBL_SET_PP_RESERVED_MC_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_RESERVED_MC_TBL_SET_PP_RESERVED_MC_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_reserved_mc_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_reserved_mc_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_reserved_mc_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_reserved_mc_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_RESERVED_MC_TBL_SET_PP_RESERVED_MC_TBL_SET_TBL_DATA_RESERVED_MC_TRAP_CODE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_RESERVED_MC_TBL_SET_PP_RESERVED_MC_TBL_SET_TBL_DATA_RESERVED_MC_TRAP_CODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.reserved_mc_trap_code = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_reserved_mc_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_reserved_mc_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_reserved_mc_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_reserved_mc_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_reserved_mc_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_RESERVED_MC_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_reserved_mc_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_RESERVED_MC_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_RESERVED_MC_TBL_GET_PP_RESERVED_MC_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_RESERVED_MC_TBL_GET_PP_RESERVED_MC_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_reserved_mc_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_reserved_mc_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_reserved_mc_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_reserved_mc_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_RESERVED_MC_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_pcp_decoding_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_pcp_decoding_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_PCP_DECODING_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_pcp_decoding_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_PCP_DECODING_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PCP_DECODING_TBL_SET_PP_PCP_DECODING_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PCP_DECODING_TBL_SET_PP_PCP_DECODING_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_pcp_decoding_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_pcp_decoding_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_pcp_decoding_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_pcp_decoding_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PCP_DECODING_TBL_SET_PP_PCP_DECODING_TBL_SET_TBL_DATA_DROP_ELIGIBLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PCP_DECODING_TBL_SET_PP_PCP_DECODING_TBL_SET_TBL_DATA_DROP_ELIGIBLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.drop_eligible = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PCP_DECODING_TBL_SET_PP_PCP_DECODING_TBL_SET_TBL_DATA_TRAFFIC_CLASS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PCP_DECODING_TBL_SET_PP_PCP_DECODING_TBL_SET_TBL_DATA_TRAFFIC_CLASS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.traffic_class = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_pcp_decoding_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_pcp_decoding_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_pcp_decoding_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_pcp_decoding_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_pcp_decoding_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_PCP_DECODING_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_pcp_decoding_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_PCP_DECODING_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PCP_DECODING_TBL_GET_PP_PCP_DECODING_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PCP_DECODING_TBL_GET_PP_PCP_DECODING_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_pcp_decoding_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_pcp_decoding_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_pcp_decoding_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_pcp_decoding_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_PCP_DECODING_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_tos_to_tc_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_tos_to_tc_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint8
    prm_is_ipv4_ndx;
  uint32
    prm_tbl_ndx;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_TOS_TO_TC_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_tos_to_tc_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_TOS_TO_TC_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_TOS_TO_TC_TBL_SET_PP_TOS_TO_TC_TBL_SET_IS_IPV4_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_TOS_TO_TC_TBL_SET_PP_TOS_TO_TC_TBL_SET_IS_IPV4_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_is_ipv4_ndx = (uint8)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter is_ipv4_ndx after pp_tos_to_tc_tbl_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_TOS_TO_TC_TBL_SET_PP_TOS_TO_TC_TBL_SET_TBL_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_TOS_TO_TC_TBL_SET_PP_TOS_TO_TC_TBL_SET_TBL_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter tbl_ndx after pp_tos_to_tc_tbl_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_TOS_TO_TC_TBL_SET_PP_TOS_TO_TC_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_TOS_TO_TC_TBL_SET_PP_TOS_TO_TC_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_tos_to_tc_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_tos_to_tc_tbl_get(
          unit,
          prm_is_ipv4_ndx,
          prm_tbl_ndx,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_tos_to_tc_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_tos_to_tc_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_TOS_TO_TC_TBL_SET_PP_TOS_TO_TC_TBL_SET_TBL_DATA_TRAFFIC_CLASS_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_TOS_TO_TC_TBL_SET_PP_TOS_TO_TC_TBL_SET_TBL_DATA_TRAFFIC_CLASS_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.traffic_class_valid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_TOS_TO_TC_TBL_SET_PP_TOS_TO_TC_TBL_SET_TBL_DATA_TRAFFIC_CLASS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_TOS_TO_TC_TBL_SET_PP_TOS_TO_TC_TBL_SET_TBL_DATA_TRAFFIC_CLASS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.traffic_class = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_tos_to_tc_tbl_set(
          unit,
          prm_is_ipv4_ndx,
          prm_tbl_ndx,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_tos_to_tc_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_tos_to_tc_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_tos_to_tc_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_tos_to_tc_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint8
    prm_is_ipv4_ndx;
  uint32
    prm_tbl_ndx;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_TOS_TO_TC_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_tos_to_tc_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_TOS_TO_TC_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_TOS_TO_TC_TBL_GET_PP_TOS_TO_TC_TBL_GET_IS_IPV4_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_TOS_TO_TC_TBL_GET_PP_TOS_TO_TC_TBL_GET_IS_IPV4_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_is_ipv4_ndx = (uint8)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter is_ipv4_ndx after pp_tos_to_tc_tbl_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_TOS_TO_TC_TBL_GET_PP_TOS_TO_TC_TBL_GET_TBL_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_TOS_TO_TC_TBL_GET_PP_TOS_TO_TC_TBL_GET_TBL_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter tbl_ndx after pp_tos_to_tc_tbl_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_TOS_TO_TC_TBL_GET_PP_TOS_TO_TC_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_TOS_TO_TC_TBL_GET_PP_TOS_TO_TC_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_tos_to_tc_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_tos_to_tc_tbl_get(
          unit,
          prm_is_ipv4_ndx,
          prm_tbl_ndx,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_tos_to_tc_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_tos_to_tc_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_TOS_TO_TC_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_l4_port_ranges_to_traffic_class_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_l4_port_ranges_to_traffic_class_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_l4_port_ranges_to_traffic_class_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_SET_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_SET_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_l4_port_ranges_to_traffic_class_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_l4_port_ranges_to_traffic_class_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_l4_port_ranges_to_traffic_class_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_l4_port_ranges_to_traffic_class_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_SET_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_SET_TBL_DATA_TRAFFIC_CLASS_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_SET_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_SET_TBL_DATA_TRAFFIC_CLASS_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.traffic_class_valid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_SET_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_SET_TBL_DATA_TRAFFIC_CLASS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_SET_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_SET_TBL_DATA_TRAFFIC_CLASS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.traffic_class = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_l4_port_ranges_to_traffic_class_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_l4_port_ranges_to_traffic_class_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_l4_port_ranges_to_traffic_class_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_l4_port_ranges_to_traffic_class_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_l4_port_ranges_to_traffic_class_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_l4_port_ranges_to_traffic_class_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_GET_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_GET_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_l4_port_ranges_to_traffic_class_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_l4_port_ranges_to_traffic_class_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_l4_port_ranges_to_traffic_class_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_l4_port_ranges_to_traffic_class_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_l4_port_ranges_to_traffic_class_tbl_parsed_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_l4_port_ranges_to_traffic_class_tbl_parsed_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_is_tc_ndx;
  uint32
    prm_src_range_ndx;
  uint32
    prm_dest_range_ndx;
  T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_l4_port_ranges_to_traffic_class_tbl_parsed_set";

  unit = t20e_get_default_unit();
  T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_SET_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_SET_IS_TC_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_SET_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_SET_IS_TC_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_is_tc_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter is_tc_ndx after pp_l4_port_ranges_to_traffic_class_tbl_parsed_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_SET_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_SET_SRC_RANGE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_SET_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_SET_SRC_RANGE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_src_range_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter src_range_ndx after pp_l4_port_ranges_to_traffic_class_tbl_parsed_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_SET_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_SET_DEST_RANGE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_SET_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_SET_DEST_RANGE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_dest_range_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter dest_range_ndx after pp_l4_port_ranges_to_traffic_class_tbl_parsed_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_l4_port_ranges_to_traffic_class_tbl_parsed_get(
          unit,
          prm_is_tc_ndx,
          prm_src_range_ndx,
          prm_dest_range_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_l4_port_ranges_to_traffic_class_tbl_parsed_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_l4_port_ranges_to_traffic_class_tbl_parsed_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_SET_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_SET_TBL_DATA_TRAFFIC_CLASS_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_SET_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_SET_TBL_DATA_TRAFFIC_CLASS_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.traffic_class_valid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_SET_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_SET_TBL_DATA_TRAFFIC_CLASS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_SET_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_SET_TBL_DATA_TRAFFIC_CLASS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.traffic_class = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_l4_port_ranges_to_traffic_class_tbl_parsed_set(
          unit,
          prm_is_tc_ndx,
          prm_src_range_ndx,
          prm_dest_range_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_l4_port_ranges_to_traffic_class_tbl_parsed_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_l4_port_ranges_to_traffic_class_tbl_parsed_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_l4_port_ranges_to_traffic_class_tbl_parsed_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_l4_port_ranges_to_traffic_class_tbl_parsed_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_is_tc_ndx;
  uint32
    prm_src_range_ndx;
  uint32
    prm_dest_range_ndx;
  T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_l4_port_ranges_to_traffic_class_tbl_parsed_get";

  unit = t20e_get_default_unit();
  T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_GET_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_GET_IS_TC_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_GET_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_GET_IS_TC_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_is_tc_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter is_tc_ndx after pp_l4_port_ranges_to_traffic_class_tbl_parsed_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_GET_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_GET_SRC_RANGE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_GET_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_GET_SRC_RANGE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_src_range_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter src_range_ndx after pp_l4_port_ranges_to_traffic_class_tbl_parsed_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_GET_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_GET_DEST_RANGE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_GET_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_GET_DEST_RANGE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_dest_range_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter dest_range_ndx after pp_l4_port_ranges_to_traffic_class_tbl_parsed_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_l4_port_ranges_to_traffic_class_tbl_parsed_get(
          unit,
          prm_is_tc_ndx,
          prm_src_range_ndx,
          prm_dest_range_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_l4_port_ranges_to_traffic_class_tbl_parsed_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_l4_port_ranges_to_traffic_class_tbl_parsed_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_drop_precedence_map_tc_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_drop_precedence_map_tc_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_DROP_PRECEDENCE_MAP_TC_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_drop_precedence_map_tc_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_DROP_PRECEDENCE_MAP_TC_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_DROP_PRECEDENCE_MAP_TC_TBL_SET_PP_DROP_PRECEDENCE_MAP_TC_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_DROP_PRECEDENCE_MAP_TC_TBL_SET_PP_DROP_PRECEDENCE_MAP_TC_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_drop_precedence_map_tc_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_drop_precedence_map_tc_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_drop_precedence_map_tc_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_drop_precedence_map_tc_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_DROP_PRECEDENCE_MAP_TC_TBL_SET_PP_DROP_PRECEDENCE_MAP_TC_TBL_SET_TBL_DATA_DROP_PRECEDENCE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_DROP_PRECEDENCE_MAP_TC_TBL_SET_PP_DROP_PRECEDENCE_MAP_TC_TBL_SET_TBL_DATA_DROP_PRECEDENCE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.drop_precedence = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_drop_precedence_map_tc_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_drop_precedence_map_tc_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_drop_precedence_map_tc_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_drop_precedence_map_tc_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_drop_precedence_map_tc_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_DROP_PRECEDENCE_MAP_TC_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_drop_precedence_map_tc_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_DROP_PRECEDENCE_MAP_TC_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_DROP_PRECEDENCE_MAP_TC_TBL_GET_PP_DROP_PRECEDENCE_MAP_TC_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_DROP_PRECEDENCE_MAP_TC_TBL_GET_PP_DROP_PRECEDENCE_MAP_TC_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_drop_precedence_map_tc_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_drop_precedence_map_tc_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_drop_precedence_map_tc_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_drop_precedence_map_tc_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_DROP_PRECEDENCE_MAP_TC_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_ac_offset_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_ac_offset_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_AC_OFFSET_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_ac_offset_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_AC_OFFSET_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AC_OFFSET_TBL_SET_PP_AC_OFFSET_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AC_OFFSET_TBL_SET_PP_AC_OFFSET_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_ac_offset_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_ac_offset_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_ac_offset_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_ac_offset_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AC_OFFSET_TBL_SET_PP_AC_OFFSET_TBL_SET_TBL_DATA_AC_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AC_OFFSET_TBL_SET_PP_AC_OFFSET_TBL_SET_TBL_DATA_AC_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.ac_offset = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_ac_offset_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_ac_offset_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_ac_offset_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_ac_offset_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_ac_offset_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_AC_OFFSET_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_ac_offset_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_AC_OFFSET_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AC_OFFSET_TBL_GET_PP_AC_OFFSET_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AC_OFFSET_TBL_GET_PP_AC_OFFSET_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_ac_offset_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_ac_offset_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_ac_offset_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_ac_offset_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_AC_OFFSET_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_cos_profile_decoding_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_cos_profile_decoding_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_COS_PROFILE_DECODING_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_cos_profile_decoding_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_COS_PROFILE_DECODING_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_COS_PROFILE_DECODING_TBL_SET_PP_COS_PROFILE_DECODING_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_COS_PROFILE_DECODING_TBL_SET_PP_COS_PROFILE_DECODING_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_cos_profile_decoding_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_cos_profile_decoding_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_cos_profile_decoding_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_cos_profile_decoding_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_COS_PROFILE_DECODING_TBL_SET_PP_COS_PROFILE_DECODING_TBL_SET_TBL_DATA_DROP_PRECEDENCE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_COS_PROFILE_DECODING_TBL_SET_PP_COS_PROFILE_DECODING_TBL_SET_TBL_DATA_DROP_PRECEDENCE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.drop_precedence = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_COS_PROFILE_DECODING_TBL_SET_PP_COS_PROFILE_DECODING_TBL_SET_TBL_DATA_TRAFFIC_CLASS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_COS_PROFILE_DECODING_TBL_SET_PP_COS_PROFILE_DECODING_TBL_SET_TBL_DATA_TRAFFIC_CLASS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.traffic_class = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_COS_PROFILE_DECODING_TBL_SET_PP_COS_PROFILE_DECODING_TBL_SET_TBL_DATA_MAP_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_COS_PROFILE_DECODING_TBL_SET_PP_COS_PROFILE_DECODING_TBL_SET_TBL_DATA_MAP_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.map_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_COS_PROFILE_DECODING_TBL_SET_PP_COS_PROFILE_DECODING_TBL_SET_TBL_DATA_FORCE_OR_MAP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_COS_PROFILE_DECODING_TBL_SET_PP_COS_PROFILE_DECODING_TBL_SET_TBL_DATA_FORCE_OR_MAP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.force_or_map = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_cos_profile_decoding_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_cos_profile_decoding_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_cos_profile_decoding_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_cos_profile_decoding_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_cos_profile_decoding_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_COS_PROFILE_DECODING_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_cos_profile_decoding_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_COS_PROFILE_DECODING_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_COS_PROFILE_DECODING_TBL_GET_PP_COS_PROFILE_DECODING_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_COS_PROFILE_DECODING_TBL_GET_PP_COS_PROFILE_DECODING_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_cos_profile_decoding_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_cos_profile_decoding_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_cos_profile_decoding_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_cos_profile_decoding_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_COS_PROFILE_DECODING_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_cos_profile_mapping_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_cos_profile_mapping_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_COS_PROFILE_MAPPING_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_cos_profile_mapping_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_COS_PROFILE_MAPPING_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_COS_PROFILE_MAPPING_TBL_SET_PP_COS_PROFILE_MAPPING_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_COS_PROFILE_MAPPING_TBL_SET_PP_COS_PROFILE_MAPPING_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_cos_profile_mapping_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_cos_profile_mapping_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_cos_profile_mapping_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_cos_profile_mapping_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_COS_PROFILE_MAPPING_TBL_SET_PP_COS_PROFILE_MAPPING_TBL_SET_TBL_DATA_DROP_PRECEDENCE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_COS_PROFILE_MAPPING_TBL_SET_PP_COS_PROFILE_MAPPING_TBL_SET_TBL_DATA_DROP_PRECEDENCE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.drop_precedence = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_COS_PROFILE_MAPPING_TBL_SET_PP_COS_PROFILE_MAPPING_TBL_SET_TBL_DATA_DROP_PRECEDENCE_OW_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_COS_PROFILE_MAPPING_TBL_SET_PP_COS_PROFILE_MAPPING_TBL_SET_TBL_DATA_DROP_PRECEDENCE_OW_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.drop_precedence_ow = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_COS_PROFILE_MAPPING_TBL_SET_PP_COS_PROFILE_MAPPING_TBL_SET_TBL_DATA_TRAFFIC_CLASS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_COS_PROFILE_MAPPING_TBL_SET_PP_COS_PROFILE_MAPPING_TBL_SET_TBL_DATA_TRAFFIC_CLASS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.traffic_class = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_COS_PROFILE_MAPPING_TBL_SET_PP_COS_PROFILE_MAPPING_TBL_SET_TBL_DATA_TRAFFIC_CLASS_OW_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_COS_PROFILE_MAPPING_TBL_SET_PP_COS_PROFILE_MAPPING_TBL_SET_TBL_DATA_TRAFFIC_CLASS_OW_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.traffic_class_ow = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_cos_profile_mapping_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_cos_profile_mapping_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_cos_profile_mapping_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_cos_profile_mapping_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_cos_profile_mapping_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_COS_PROFILE_MAPPING_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_cos_profile_mapping_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_COS_PROFILE_MAPPING_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_COS_PROFILE_MAPPING_TBL_GET_PP_COS_PROFILE_MAPPING_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_COS_PROFILE_MAPPING_TBL_GET_PP_COS_PROFILE_MAPPING_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_cos_profile_mapping_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_cos_profile_mapping_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_cos_profile_mapping_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_cos_profile_mapping_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_COS_PROFILE_MAPPING_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_stp_state_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_stp_state_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_STP_STATE_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_stp_state_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_STP_STATE_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_STP_STATE_TBL_SET_PP_STP_STATE_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_STP_STATE_TBL_SET_PP_STP_STATE_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_stp_state_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_stp_state_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_stp_state_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_stp_state_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_STP_STATE_TBL_SET_PP_STP_STATE_TBL_SET_TBL_DATA_STP_STATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_STP_STATE_TBL_SET_PP_STP_STATE_TBL_SET_TBL_DATA_STP_STATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.stp_state = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_stp_state_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_stp_state_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_stp_state_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_stp_state_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_stp_state_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_STP_STATE_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_stp_state_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_STP_STATE_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_STP_STATE_TBL_GET_PP_STP_STATE_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_STP_STATE_TBL_GET_PP_STP_STATE_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_stp_state_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_stp_state_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_stp_state_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_stp_state_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_STP_STATE_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_pcp_dei_map_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_pcp_dei_map_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_PCP_DEI_MAP_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_pcp_dei_map_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_PCP_DEI_MAP_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PCP_DEI_MAP_TBL_SET_PP_PCP_DEI_MAP_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PCP_DEI_MAP_TBL_SET_PP_PCP_DEI_MAP_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_pcp_dei_map_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_pcp_dei_map_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_pcp_dei_map_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_pcp_dei_map_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PCP_DEI_MAP_TBL_SET_PP_PCP_DEI_MAP_TBL_SET_TBL_DATA_DEI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PCP_DEI_MAP_TBL_SET_PP_PCP_DEI_MAP_TBL_SET_TBL_DATA_DEI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.dei = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PCP_DEI_MAP_TBL_SET_PP_PCP_DEI_MAP_TBL_SET_TBL_DATA_PCP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PCP_DEI_MAP_TBL_SET_PP_PCP_DEI_MAP_TBL_SET_TBL_DATA_PCP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.pcp = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_pcp_dei_map_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_pcp_dei_map_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_pcp_dei_map_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_pcp_dei_map_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_pcp_dei_map_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_PCP_DEI_MAP_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_pcp_dei_map_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_PCP_DEI_MAP_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PCP_DEI_MAP_TBL_GET_PP_PCP_DEI_MAP_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PCP_DEI_MAP_TBL_GET_PP_PCP_DEI_MAP_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_pcp_dei_map_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_pcp_dei_map_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_pcp_dei_map_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_pcp_dei_map_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_PCP_DEI_MAP_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_ivec_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_ivec_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_IVEC_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_ivec_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_IVEC_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_IVEC_TBL_SET_PP_IVEC_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_IVEC_TBL_SET_PP_IVEC_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_ivec_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_ivec_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_ivec_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_ivec_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_IVEC_TBL_SET_PP_IVEC_TBL_SET_TBL_DATA_COMMAND_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_IVEC_TBL_SET_PP_IVEC_TBL_SET_TBL_DATA_COMMAND_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.command = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_ivec_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_ivec_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_ivec_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_ivec_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_ivec_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_IVEC_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_ivec_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_IVEC_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_IVEC_TBL_GET_PP_IVEC_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_IVEC_TBL_GET_PP_IVEC_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_ivec_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_ivec_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_ivec_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_ivec_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_IVEC_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_action_profile_mpls_value_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_action_profile_mpls_value_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_ACTION_PROFILE_MPLS_VALUE_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_action_profile_mpls_value_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_ACTION_PROFILE_MPLS_VALUE_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_ACTION_PROFILE_MPLS_VALUE_TBL_SET_PP_ACTION_PROFILE_MPLS_VALUE_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_ACTION_PROFILE_MPLS_VALUE_TBL_SET_PP_ACTION_PROFILE_MPLS_VALUE_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_action_profile_mpls_value_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_action_profile_mpls_value_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_action_profile_mpls_value_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_action_profile_mpls_value_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_ACTION_PROFILE_MPLS_VALUE_TBL_SET_PP_ACTION_PROFILE_MPLS_VALUE_TBL_SET_TBL_DATA_ACTION_BOS_SNOOP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_ACTION_PROFILE_MPLS_VALUE_TBL_SET_PP_ACTION_PROFILE_MPLS_VALUE_TBL_SET_TBL_DATA_ACTION_BOS_SNOOP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.action_bos_snoop = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_ACTION_PROFILE_MPLS_VALUE_TBL_SET_PP_ACTION_PROFILE_MPLS_VALUE_TBL_SET_TBL_DATA_ACTION_BOS_FRWRD_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_ACTION_PROFILE_MPLS_VALUE_TBL_SET_PP_ACTION_PROFILE_MPLS_VALUE_TBL_SET_TBL_DATA_ACTION_BOS_FRWRD_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.action_bos_frwrd = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_ACTION_PROFILE_MPLS_VALUE_TBL_SET_PP_ACTION_PROFILE_MPLS_VALUE_TBL_SET_TBL_DATA_ACTION_NOT_BOS_SNOOP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_ACTION_PROFILE_MPLS_VALUE_TBL_SET_PP_ACTION_PROFILE_MPLS_VALUE_TBL_SET_TBL_DATA_ACTION_NOT_BOS_SNOOP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.action_not_bos_snoop = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_ACTION_PROFILE_MPLS_VALUE_TBL_SET_PP_ACTION_PROFILE_MPLS_VALUE_TBL_SET_TBL_DATA_ACTION_NOT_BOS_FRWRD_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_ACTION_PROFILE_MPLS_VALUE_TBL_SET_PP_ACTION_PROFILE_MPLS_VALUE_TBL_SET_TBL_DATA_ACTION_NOT_BOS_FRWRD_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.action_not_bos_frwrd = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_action_profile_mpls_value_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_action_profile_mpls_value_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_action_profile_mpls_value_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_action_profile_mpls_value_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_action_profile_mpls_value_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_ACTION_PROFILE_MPLS_VALUE_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_action_profile_mpls_value_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_ACTION_PROFILE_MPLS_VALUE_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_ACTION_PROFILE_MPLS_VALUE_TBL_GET_PP_ACTION_PROFILE_MPLS_VALUE_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_ACTION_PROFILE_MPLS_VALUE_TBL_GET_PP_ACTION_PROFILE_MPLS_VALUE_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_action_profile_mpls_value_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_action_profile_mpls_value_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_action_profile_mpls_value_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_action_profile_mpls_value_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_ACTION_PROFILE_MPLS_VALUE_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_mpls_cos_profile_decoding_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_mpls_cos_profile_decoding_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_MPLS_COS_PROFILE_DECODING_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_mpls_cos_profile_decoding_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_MPLS_COS_PROFILE_DECODING_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MPLS_COS_PROFILE_DECODING_TBL_SET_PP_MPLS_COS_PROFILE_DECODING_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_MPLS_COS_PROFILE_DECODING_TBL_SET_PP_MPLS_COS_PROFILE_DECODING_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_mpls_cos_profile_decoding_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_mpls_cos_profile_decoding_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_mpls_cos_profile_decoding_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_mpls_cos_profile_decoding_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MPLS_COS_PROFILE_DECODING_TBL_SET_PP_MPLS_COS_PROFILE_DECODING_TBL_SET_TBL_DATA_DROP_PRECEDENCE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_MPLS_COS_PROFILE_DECODING_TBL_SET_PP_MPLS_COS_PROFILE_DECODING_TBL_SET_TBL_DATA_DROP_PRECEDENCE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.drop_precedence = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MPLS_COS_PROFILE_DECODING_TBL_SET_PP_MPLS_COS_PROFILE_DECODING_TBL_SET_TBL_DATA_TRAFFIC_CLASS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_MPLS_COS_PROFILE_DECODING_TBL_SET_PP_MPLS_COS_PROFILE_DECODING_TBL_SET_TBL_DATA_TRAFFIC_CLASS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.traffic_class = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MPLS_COS_PROFILE_DECODING_TBL_SET_PP_MPLS_COS_PROFILE_DECODING_TBL_SET_TBL_DATA_MAP_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_MPLS_COS_PROFILE_DECODING_TBL_SET_PP_MPLS_COS_PROFILE_DECODING_TBL_SET_TBL_DATA_MAP_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.map_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MPLS_COS_PROFILE_DECODING_TBL_SET_PP_MPLS_COS_PROFILE_DECODING_TBL_SET_TBL_DATA_FORCE_OR_MAP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_MPLS_COS_PROFILE_DECODING_TBL_SET_PP_MPLS_COS_PROFILE_DECODING_TBL_SET_TBL_DATA_FORCE_OR_MAP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.force_or_map = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_mpls_cos_profile_decoding_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_mpls_cos_profile_decoding_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_mpls_cos_profile_decoding_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_mpls_cos_profile_decoding_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_mpls_cos_profile_decoding_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_MPLS_COS_PROFILE_DECODING_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_mpls_cos_profile_decoding_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_MPLS_COS_PROFILE_DECODING_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MPLS_COS_PROFILE_DECODING_TBL_GET_PP_MPLS_COS_PROFILE_DECODING_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_MPLS_COS_PROFILE_DECODING_TBL_GET_PP_MPLS_COS_PROFILE_DECODING_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_mpls_cos_profile_decoding_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_mpls_cos_profile_decoding_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_mpls_cos_profile_decoding_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_mpls_cos_profile_decoding_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_MPLS_COS_PROFILE_DECODING_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_mpls_cos_mapping_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_mpls_cos_mapping_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_MPLS_COS_MAPPING_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_mpls_cos_mapping_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_MPLS_COS_MAPPING_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MPLS_COS_MAPPING_TBL_SET_PP_MPLS_COS_MAPPING_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_MPLS_COS_MAPPING_TBL_SET_PP_MPLS_COS_MAPPING_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_mpls_cos_mapping_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_mpls_cos_mapping_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_mpls_cos_mapping_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_mpls_cos_mapping_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MPLS_COS_MAPPING_TBL_SET_PP_MPLS_COS_MAPPING_TBL_SET_TBL_DATA_DROP_PRECEDENCE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_MPLS_COS_MAPPING_TBL_SET_PP_MPLS_COS_MAPPING_TBL_SET_TBL_DATA_DROP_PRECEDENCE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.drop_precedence = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MPLS_COS_MAPPING_TBL_SET_PP_MPLS_COS_MAPPING_TBL_SET_TBL_DATA_DROP_PRECEDENCE_OW_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_MPLS_COS_MAPPING_TBL_SET_PP_MPLS_COS_MAPPING_TBL_SET_TBL_DATA_DROP_PRECEDENCE_OW_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.drop_precedence_ow = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MPLS_COS_MAPPING_TBL_SET_PP_MPLS_COS_MAPPING_TBL_SET_TBL_DATA_TRAFFIC_CLASS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_MPLS_COS_MAPPING_TBL_SET_PP_MPLS_COS_MAPPING_TBL_SET_TBL_DATA_TRAFFIC_CLASS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.traffic_class = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MPLS_COS_MAPPING_TBL_SET_PP_MPLS_COS_MAPPING_TBL_SET_TBL_DATA_TRAFFIC_CLASS_OW_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_MPLS_COS_MAPPING_TBL_SET_PP_MPLS_COS_MAPPING_TBL_SET_TBL_DATA_TRAFFIC_CLASS_OW_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.traffic_class_ow = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_mpls_cos_mapping_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_mpls_cos_mapping_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_mpls_cos_mapping_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_mpls_cos_mapping_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_mpls_cos_mapping_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_MPLS_COS_MAPPING_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_mpls_cos_mapping_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_MPLS_COS_MAPPING_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MPLS_COS_MAPPING_TBL_GET_PP_MPLS_COS_MAPPING_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_MPLS_COS_MAPPING_TBL_GET_PP_MPLS_COS_MAPPING_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_mpls_cos_mapping_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_mpls_cos_mapping_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_mpls_cos_mapping_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_mpls_cos_mapping_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_MPLS_COS_MAPPING_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_mpls_termination_cos_mapping_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_mpls_termination_cos_mapping_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_MPLS_TERMINATION_COS_MAPPING_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_mpls_termination_cos_mapping_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_MPLS_TERMINATION_COS_MAPPING_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MPLS_TERMINATION_COS_MAPPING_TBL_SET_PP_MPLS_TERMINATION_COS_MAPPING_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_MPLS_TERMINATION_COS_MAPPING_TBL_SET_PP_MPLS_TERMINATION_COS_MAPPING_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_mpls_termination_cos_mapping_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_mpls_termination_cos_mapping_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_mpls_termination_cos_mapping_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_mpls_termination_cos_mapping_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MPLS_TERMINATION_COS_MAPPING_TBL_SET_PP_MPLS_TERMINATION_COS_MAPPING_TBL_SET_TBL_DATA_DROP_PRECEDENCE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_MPLS_TERMINATION_COS_MAPPING_TBL_SET_PP_MPLS_TERMINATION_COS_MAPPING_TBL_SET_TBL_DATA_DROP_PRECEDENCE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.drop_precedence = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MPLS_TERMINATION_COS_MAPPING_TBL_SET_PP_MPLS_TERMINATION_COS_MAPPING_TBL_SET_TBL_DATA_DROP_PRECEDENCE_OW_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_MPLS_TERMINATION_COS_MAPPING_TBL_SET_PP_MPLS_TERMINATION_COS_MAPPING_TBL_SET_TBL_DATA_DROP_PRECEDENCE_OW_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.drop_precedence_ow = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MPLS_TERMINATION_COS_MAPPING_TBL_SET_PP_MPLS_TERMINATION_COS_MAPPING_TBL_SET_TBL_DATA_TRAFFIC_CLASS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_MPLS_TERMINATION_COS_MAPPING_TBL_SET_PP_MPLS_TERMINATION_COS_MAPPING_TBL_SET_TBL_DATA_TRAFFIC_CLASS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.traffic_class = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MPLS_TERMINATION_COS_MAPPING_TBL_SET_PP_MPLS_TERMINATION_COS_MAPPING_TBL_SET_TBL_DATA_TRAFFIC_CLASS_OW_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_MPLS_TERMINATION_COS_MAPPING_TBL_SET_PP_MPLS_TERMINATION_COS_MAPPING_TBL_SET_TBL_DATA_TRAFFIC_CLASS_OW_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.traffic_class_ow = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_mpls_termination_cos_mapping_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_mpls_termination_cos_mapping_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_mpls_termination_cos_mapping_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_mpls_termination_cos_mapping_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_mpls_termination_cos_mapping_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_MPLS_TERMINATION_COS_MAPPING_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_mpls_termination_cos_mapping_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_MPLS_TERMINATION_COS_MAPPING_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MPLS_TERMINATION_COS_MAPPING_TBL_GET_PP_MPLS_TERMINATION_COS_MAPPING_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_MPLS_TERMINATION_COS_MAPPING_TBL_GET_PP_MPLS_TERMINATION_COS_MAPPING_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_mpls_termination_cos_mapping_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_mpls_termination_cos_mapping_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_mpls_termination_cos_mapping_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_mpls_termination_cos_mapping_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_MPLS_TERMINATION_COS_MAPPING_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_mpls_tunnel_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_mpls_tunnel_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_MPLS_TUNNEL_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_mpls_tunnel_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_MPLS_TUNNEL_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MPLS_TUNNEL_TBL_SET_PP_MPLS_TUNNEL_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_MPLS_TUNNEL_TBL_SET_PP_MPLS_TUNNEL_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_mpls_tunnel_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_mpls_tunnel_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_mpls_tunnel_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_mpls_tunnel_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MPLS_TUNNEL_TBL_SET_PP_MPLS_TUNNEL_TBL_SET_TBL_DATA_STAT_TAG_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_MPLS_TUNNEL_TBL_SET_PP_MPLS_TUNNEL_TBL_SET_TBL_DATA_STAT_TAG_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.stat_tag_valid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MPLS_TUNNEL_TBL_SET_PP_MPLS_TUNNEL_TBL_SET_TBL_DATA_POLICER_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_MPLS_TUNNEL_TBL_SET_PP_MPLS_TUNNEL_TBL_SET_TBL_DATA_POLICER_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.policer_valid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MPLS_TUNNEL_TBL_SET_PP_MPLS_TUNNEL_TBL_SET_TBL_DATA_COUNTER_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_MPLS_TUNNEL_TBL_SET_PP_MPLS_TUNNEL_TBL_SET_TBL_DATA_COUNTER_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.counter_valid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MPLS_TUNNEL_TBL_SET_PP_MPLS_TUNNEL_TBL_SET_TBL_DATA_TERMINATION_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_MPLS_TUNNEL_TBL_SET_PP_MPLS_TUNNEL_TBL_SET_TBL_DATA_TERMINATION_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.termination_valid = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_mpls_tunnel_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_mpls_tunnel_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_mpls_tunnel_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_mpls_tunnel_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_mpls_tunnel_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_MPLS_TUNNEL_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_mpls_tunnel_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_MPLS_TUNNEL_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MPLS_TUNNEL_TBL_GET_PP_MPLS_TUNNEL_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_MPLS_TUNNEL_TBL_GET_PP_MPLS_TUNNEL_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_mpls_tunnel_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_mpls_tunnel_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_mpls_tunnel_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_mpls_tunnel_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_MPLS_TUNNEL_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_protection_instance_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_protection_instance_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_PROTECTION_INSTANCE_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_protection_instance_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_PROTECTION_INSTANCE_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PROTECTION_INSTANCE_TBL_SET_PP_PROTECTION_INSTANCE_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PROTECTION_INSTANCE_TBL_SET_PP_PROTECTION_INSTANCE_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_protection_instance_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_protection_instance_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_protection_instance_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_protection_instance_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PROTECTION_INSTANCE_TBL_SET_PP_PROTECTION_INSTANCE_TBL_SET_TBL_DATA_STATUS_BMAP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PROTECTION_INSTANCE_TBL_SET_PP_PROTECTION_INSTANCE_TBL_SET_TBL_DATA_STATUS_BMAP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.status_bmap = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_protection_instance_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_protection_instance_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_protection_instance_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_protection_instance_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_protection_instance_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_PROTECTION_INSTANCE_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_protection_instance_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_PROTECTION_INSTANCE_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PROTECTION_INSTANCE_TBL_GET_PP_PROTECTION_INSTANCE_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PROTECTION_INSTANCE_TBL_GET_PP_PROTECTION_INSTANCE_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_protection_instance_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_protection_instance_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_protection_instance_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_protection_instance_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_PROTECTION_INSTANCE_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_destination_status_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_destination_status_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_DESTINATION_STATUS_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_destination_status_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_DESTINATION_STATUS_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_DESTINATION_STATUS_TBL_SET_PP_DESTINATION_STATUS_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_DESTINATION_STATUS_TBL_SET_PP_DESTINATION_STATUS_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_destination_status_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_destination_status_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_destination_status_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_destination_status_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_DESTINATION_STATUS_TBL_SET_PP_DESTINATION_STATUS_TBL_SET_TBL_DATA_STATUS_BMAP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_DESTINATION_STATUS_TBL_SET_PP_DESTINATION_STATUS_TBL_SET_TBL_DATA_STATUS_BMAP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.status_bmap = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_destination_status_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_destination_status_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_destination_status_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_destination_status_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_destination_status_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_DESTINATION_STATUS_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_destination_status_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_DESTINATION_STATUS_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_DESTINATION_STATUS_TBL_GET_PP_DESTINATION_STATUS_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_DESTINATION_STATUS_TBL_GET_PP_DESTINATION_STATUS_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_destination_status_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_destination_status_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_destination_status_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_destination_status_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_DESTINATION_STATUS_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_port_config_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_port_config_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_PORT_CONFIG_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_port_config_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_PORT_CONFIG_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_port_config_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_port_config_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_port_config_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_port_config_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_SYSTEM_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_SYSTEM_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.system_port = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_DEFAULT_SNOOP_CMD_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_DEFAULT_SNOOP_CMD_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.default_snoop_cmd = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_USE_LL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_USE_LL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.use_ll = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_LEARN_AC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_LEARN_AC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.learn_ac = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_DROP_PRECEDENCE_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_DROP_PRECEDENCE_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.drop_precedence_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_USE_DEI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_USE_DEI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.use_dei = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_DEFAULT_TRAFFIC_CLASS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_DEFAULT_TRAFFIC_CLASS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.default_traffic_class = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_TC_PORT_PROTOCOL_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_TC_PORT_PROTOCOL_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.tc_port_protocol_enable = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_TC_SUBNET_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_TC_SUBNET_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.tc_subnet_enable = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_TC_L4_PROTOCOL_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_TC_L4_PROTOCOL_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.tc_l4_protocol_enable = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_TC_TOS_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_TC_TOS_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.tc_tos_index = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_TC_TOS_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_TC_TOS_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.tc_tos_enable = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_PCP_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_PCP_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.pcp_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_RESERVED_MC_TRAP_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_RESERVED_MC_TRAP_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.reserved_mc_trap_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_PROTOCOL_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_PROTOCOL_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.protocol_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_DEFAULT_INITIAL_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_DEFAULT_INITIAL_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.default_initial_vid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_VID_PORT_PROTOCOL_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_VID_PORT_PROTOCOL_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.vid_port_protocol_enable = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_VID_IPV4_SUBNET_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_VID_IPV4_SUBNET_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.vid_ipv4_subnet_enable = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_IGNORE_INCOMING_PCP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_IGNORE_INCOMING_PCP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.ignore_incoming_pcp = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_IGNORE_INCOMING_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_IGNORE_INCOMING_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.ignore_incoming_vid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_TPID2_INDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_TPID2_INDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.tpid2_indx = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_TPID1_INDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_TPID1_INDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.tpid1_indx = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_VLAN_CLASSIFICATION_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_VLAN_CLASSIFICATION_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.vlan_classification_profile = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_port_config_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_port_config_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_port_config_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_port_config_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_port_config_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_PORT_CONFIG_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_port_config_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_PORT_CONFIG_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PORT_CONFIG_TBL_GET_PP_PORT_CONFIG_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PORT_CONFIG_TBL_GET_PP_PORT_CONFIG_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_port_config_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_port_config_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_port_config_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_port_config_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_PORT_CONFIG_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_mep_level_profile_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_mep_level_profile_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_MEP_LEVEL_PROFILE_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_mep_level_profile_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_MEP_LEVEL_PROFILE_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MEP_LEVEL_PROFILE_TBL_SET_PP_MEP_LEVEL_PROFILE_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_MEP_LEVEL_PROFILE_TBL_SET_PP_MEP_LEVEL_PROFILE_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_mep_level_profile_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_mep_level_profile_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_mep_level_profile_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_mep_level_profile_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MEP_LEVEL_PROFILE_TBL_SET_PP_MEP_LEVEL_PROFILE_TBL_SET_TBL_DATA_LEVEL7_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_MEP_LEVEL_PROFILE_TBL_SET_PP_MEP_LEVEL_PROFILE_TBL_SET_TBL_DATA_LEVEL7_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.level7 = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MEP_LEVEL_PROFILE_TBL_SET_PP_MEP_LEVEL_PROFILE_TBL_SET_TBL_DATA_LEVEL6_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_MEP_LEVEL_PROFILE_TBL_SET_PP_MEP_LEVEL_PROFILE_TBL_SET_TBL_DATA_LEVEL6_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.level6 = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MEP_LEVEL_PROFILE_TBL_SET_PP_MEP_LEVEL_PROFILE_TBL_SET_TBL_DATA_LEVEL5_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_MEP_LEVEL_PROFILE_TBL_SET_PP_MEP_LEVEL_PROFILE_TBL_SET_TBL_DATA_LEVEL5_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.level5 = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MEP_LEVEL_PROFILE_TBL_SET_PP_MEP_LEVEL_PROFILE_TBL_SET_TBL_DATA_LEVEL4_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_MEP_LEVEL_PROFILE_TBL_SET_PP_MEP_LEVEL_PROFILE_TBL_SET_TBL_DATA_LEVEL4_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.level4 = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MEP_LEVEL_PROFILE_TBL_SET_PP_MEP_LEVEL_PROFILE_TBL_SET_TBL_DATA_LEVEL3_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_MEP_LEVEL_PROFILE_TBL_SET_PP_MEP_LEVEL_PROFILE_TBL_SET_TBL_DATA_LEVEL3_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.level3 = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MEP_LEVEL_PROFILE_TBL_SET_PP_MEP_LEVEL_PROFILE_TBL_SET_TBL_DATA_LEVEL2_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_MEP_LEVEL_PROFILE_TBL_SET_PP_MEP_LEVEL_PROFILE_TBL_SET_TBL_DATA_LEVEL2_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.level2 = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MEP_LEVEL_PROFILE_TBL_SET_PP_MEP_LEVEL_PROFILE_TBL_SET_TBL_DATA_LEVEL1_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_MEP_LEVEL_PROFILE_TBL_SET_PP_MEP_LEVEL_PROFILE_TBL_SET_TBL_DATA_LEVEL1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.level1 = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MEP_LEVEL_PROFILE_TBL_SET_PP_MEP_LEVEL_PROFILE_TBL_SET_TBL_DATA_LEVEL0_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_MEP_LEVEL_PROFILE_TBL_SET_PP_MEP_LEVEL_PROFILE_TBL_SET_TBL_DATA_LEVEL0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.level0 = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_mep_level_profile_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_mep_level_profile_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_mep_level_profile_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_mep_level_profile_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_mep_level_profile_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_MEP_LEVEL_PROFILE_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_mep_level_profile_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_MEP_LEVEL_PROFILE_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MEP_LEVEL_PROFILE_TBL_GET_PP_MEP_LEVEL_PROFILE_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_MEP_LEVEL_PROFILE_TBL_GET_PP_MEP_LEVEL_PROFILE_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_mep_level_profile_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_mep_level_profile_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_mep_level_profile_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_mep_level_profile_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_MEP_LEVEL_PROFILE_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_oam_action_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_oam_action_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_OAM_ACTION_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_oam_action_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_OAM_ACTION_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_OAM_ACTION_TBL_SET_PP_OAM_ACTION_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_OAM_ACTION_TBL_SET_PP_OAM_ACTION_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_oam_action_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_oam_action_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_oam_action_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_oam_action_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_OAM_ACTION_TBL_SET_PP_OAM_ACTION_TBL_SET_TBL_DATA_OAM_ACTION_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_OAM_ACTION_TBL_SET_PP_OAM_ACTION_TBL_SET_TBL_DATA_OAM_ACTION_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.oam_action_profile = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_oam_action_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_oam_action_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_oam_action_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_oam_action_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_oam_action_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_OAM_ACTION_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_oam_action_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_OAM_ACTION_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_OAM_ACTION_TBL_GET_PP_OAM_ACTION_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_OAM_ACTION_TBL_GET_PP_OAM_ACTION_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_oam_action_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_oam_action_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_oam_action_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_oam_action_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_OAM_ACTION_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_forward_action_profile_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_forward_action_profile_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_FORWARD_ACTION_PROFILE_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_forward_action_profile_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_FORWARD_ACTION_PROFILE_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_FORWARD_ACTION_PROFILE_TBL_SET_PP_FORWARD_ACTION_PROFILE_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_FORWARD_ACTION_PROFILE_TBL_SET_PP_FORWARD_ACTION_PROFILE_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_forward_action_profile_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_forward_action_profile_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_forward_action_profile_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_forward_action_profile_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_FORWARD_ACTION_PROFILE_TBL_SET_PP_FORWARD_ACTION_PROFILE_TBL_SET_TBL_DATA_STRENGTH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_FORWARD_ACTION_PROFILE_TBL_SET_PP_FORWARD_ACTION_PROFILE_TBL_SET_TBL_DATA_STRENGTH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.strength = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_FORWARD_ACTION_PROFILE_TBL_SET_PP_FORWARD_ACTION_PROFILE_TBL_SET_TBL_DATA_DP_OW_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_FORWARD_ACTION_PROFILE_TBL_SET_PP_FORWARD_ACTION_PROFILE_TBL_SET_TBL_DATA_DP_OW_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.dp_ow = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_FORWARD_ACTION_PROFILE_TBL_SET_PP_FORWARD_ACTION_PROFILE_TBL_SET_TBL_DATA_DP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_FORWARD_ACTION_PROFILE_TBL_SET_PP_FORWARD_ACTION_PROFILE_TBL_SET_TBL_DATA_DP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.dp_value = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_FORWARD_ACTION_PROFILE_TBL_SET_PP_FORWARD_ACTION_PROFILE_TBL_SET_TBL_DATA_TC_OW_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_FORWARD_ACTION_PROFILE_TBL_SET_PP_FORWARD_ACTION_PROFILE_TBL_SET_TBL_DATA_TC_OW_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.tc_ow = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_FORWARD_ACTION_PROFILE_TBL_SET_PP_FORWARD_ACTION_PROFILE_TBL_SET_TBL_DATA_TC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_FORWARD_ACTION_PROFILE_TBL_SET_PP_FORWARD_ACTION_PROFILE_TBL_SET_TBL_DATA_TC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.tc_value = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_FORWARD_ACTION_PROFILE_TBL_SET_PP_FORWARD_ACTION_PROFILE_TBL_SET_TBL_DATA_DESTINATION_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_FORWARD_ACTION_PROFILE_TBL_SET_PP_FORWARD_ACTION_PROFILE_TBL_SET_TBL_DATA_DESTINATION_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.destination_valid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_FORWARD_ACTION_PROFILE_TBL_SET_PP_FORWARD_ACTION_PROFILE_TBL_SET_TBL_DATA_DESTINATION_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_FORWARD_ACTION_PROFILE_TBL_SET_PP_FORWARD_ACTION_PROFILE_TBL_SET_TBL_DATA_DESTINATION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.destination = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_FORWARD_ACTION_PROFILE_TBL_SET_PP_FORWARD_ACTION_PROFILE_TBL_SET_TBL_DATA_DROP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_FORWARD_ACTION_PROFILE_TBL_SET_PP_FORWARD_ACTION_PROFILE_TBL_SET_TBL_DATA_DROP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.drop = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_FORWARD_ACTION_PROFILE_TBL_SET_PP_FORWARD_ACTION_PROFILE_TBL_SET_TBL_DATA_CONTROL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_FORWARD_ACTION_PROFILE_TBL_SET_PP_FORWARD_ACTION_PROFILE_TBL_SET_TBL_DATA_CONTROL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.control = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_FORWARD_ACTION_PROFILE_TBL_SET_PP_FORWARD_ACTION_PROFILE_TBL_SET_TBL_DATA_TRAP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_FORWARD_ACTION_PROFILE_TBL_SET_PP_FORWARD_ACTION_PROFILE_TBL_SET_TBL_DATA_TRAP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.trap = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_forward_action_profile_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_forward_action_profile_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_forward_action_profile_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_forward_action_profile_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_forward_action_profile_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_FORWARD_ACTION_PROFILE_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_forward_action_profile_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_FORWARD_ACTION_PROFILE_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_FORWARD_ACTION_PROFILE_TBL_GET_PP_FORWARD_ACTION_PROFILE_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_FORWARD_ACTION_PROFILE_TBL_GET_PP_FORWARD_ACTION_PROFILE_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_forward_action_profile_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_forward_action_profile_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_forward_action_profile_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_forward_action_profile_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_FORWARD_ACTION_PROFILE_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_snoop_action_profile_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_snoop_action_profile_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_SNOOP_ACTION_PROFILE_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_snoop_action_profile_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_SNOOP_ACTION_PROFILE_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_SNOOP_ACTION_PROFILE_TBL_SET_PP_SNOOP_ACTION_PROFILE_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_SNOOP_ACTION_PROFILE_TBL_SET_PP_SNOOP_ACTION_PROFILE_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_snoop_action_profile_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_snoop_action_profile_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_snoop_action_profile_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_snoop_action_profile_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_SNOOP_ACTION_PROFILE_TBL_SET_PP_SNOOP_ACTION_PROFILE_TBL_SET_TBL_DATA_STRENGTH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_SNOOP_ACTION_PROFILE_TBL_SET_PP_SNOOP_ACTION_PROFILE_TBL_SET_TBL_DATA_STRENGTH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.strength = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_snoop_action_profile_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_snoop_action_profile_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_snoop_action_profile_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_snoop_action_profile_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_snoop_action_profile_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_SNOOP_ACTION_PROFILE_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_snoop_action_profile_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_SNOOP_ACTION_PROFILE_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_SNOOP_ACTION_PROFILE_TBL_GET_PP_SNOOP_ACTION_PROFILE_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_SNOOP_ACTION_PROFILE_TBL_GET_PP_SNOOP_ACTION_PROFILE_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_snoop_action_profile_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_snoop_action_profile_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_snoop_action_profile_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_snoop_action_profile_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_SNOOP_ACTION_PROFILE_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_burst_size_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_burst_size_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_BURST_SIZE_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_burst_size_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_BURST_SIZE_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_BURST_SIZE_TBL_SET_PP_BURST_SIZE_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_BURST_SIZE_TBL_SET_PP_BURST_SIZE_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_burst_size_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_burst_size_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_burst_size_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_burst_size_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_BURST_SIZE_TBL_SET_PP_BURST_SIZE_TBL_SET_TBL_DATA_MAX_BS_BYTES_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_BURST_SIZE_TBL_SET_PP_BURST_SIZE_TBL_SET_TBL_DATA_MAX_BS_BYTES_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.max_bs_bytes = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_burst_size_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_burst_size_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_burst_size_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_burst_size_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_burst_size_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_BURST_SIZE_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_burst_size_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_BURST_SIZE_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_BURST_SIZE_TBL_GET_PP_BURST_SIZE_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_BURST_SIZE_TBL_GET_PP_BURST_SIZE_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_burst_size_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_burst_size_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_burst_size_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_burst_size_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_BURST_SIZE_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_mantissa_division_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_mantissa_division_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_MANTISSA_DIVISION_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_mantissa_division_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_MANTISSA_DIVISION_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MANTISSA_DIVISION_TBL_SET_PP_MANTISSA_DIVISION_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_MANTISSA_DIVISION_TBL_SET_PP_MANTISSA_DIVISION_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_mantissa_division_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_mantissa_division_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_mantissa_division_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_mantissa_division_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MANTISSA_DIVISION_TBL_SET_PP_MANTISSA_DIVISION_TBL_SET_TBL_DATA_VALUE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_MANTISSA_DIVISION_TBL_SET_PP_MANTISSA_DIVISION_TBL_SET_TBL_DATA_VALUE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.value = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_mantissa_division_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_mantissa_division_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_mantissa_division_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_mantissa_division_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_mantissa_division_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_MANTISSA_DIVISION_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_mantissa_division_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_MANTISSA_DIVISION_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MANTISSA_DIVISION_TBL_GET_PP_MANTISSA_DIVISION_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_MANTISSA_DIVISION_TBL_GET_PP_MANTISSA_DIVISION_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_mantissa_division_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_mantissa_division_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_mantissa_division_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_mantissa_division_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_MANTISSA_DIVISION_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_glag_range_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_glag_range_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_GLAG_RANGE_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_glag_range_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_GLAG_RANGE_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_GLAG_RANGE_TBL_SET_PP_GLAG_RANGE_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_GLAG_RANGE_TBL_SET_PP_GLAG_RANGE_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_glag_range_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_glag_range_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_glag_range_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_glag_range_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_GLAG_RANGE_TBL_SET_PP_GLAG_RANGE_TBL_SET_TBL_DATA_LAG_RANGE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_GLAG_RANGE_TBL_SET_PP_GLAG_RANGE_TBL_SET_TBL_DATA_LAG_RANGE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.lag_range = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_glag_range_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_glag_range_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_glag_range_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_glag_range_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_glag_range_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_GLAG_RANGE_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_glag_range_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_GLAG_RANGE_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_GLAG_RANGE_TBL_GET_PP_GLAG_RANGE_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_GLAG_RANGE_TBL_GET_PP_GLAG_RANGE_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_glag_range_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_glag_range_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_glag_range_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_glag_range_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_GLAG_RANGE_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_modulo_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_modulo_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_MODULO_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_modulo_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_MODULO_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MODULO_TBL_SET_PP_MODULO_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_MODULO_TBL_SET_PP_MODULO_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_modulo_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_modulo_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_modulo_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_modulo_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MODULO_TBL_SET_PP_MODULO_TBL_SET_TBL_DATA_LAG_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_MODULO_TBL_SET_PP_MODULO_TBL_SET_TBL_DATA_LAG_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.lag_offset = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_modulo_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_modulo_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_modulo_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_modulo_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_modulo_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_MODULO_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_modulo_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_MODULO_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MODULO_TBL_GET_PP_MODULO_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_MODULO_TBL_GET_PP_MODULO_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_modulo_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_modulo_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_modulo_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_modulo_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_MODULO_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_glag_out_port_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_glag_out_port_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_GLAG_OUT_PORT_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_glag_out_port_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_GLAG_OUT_PORT_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_GLAG_OUT_PORT_TBL_SET_PP_GLAG_OUT_PORT_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_GLAG_OUT_PORT_TBL_SET_PP_GLAG_OUT_PORT_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_glag_out_port_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_glag_out_port_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_glag_out_port_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_glag_out_port_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_GLAG_OUT_PORT_TBL_SET_PP_GLAG_OUT_PORT_TBL_SET_TBL_DATA_OUT_SYSTEM_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_GLAG_OUT_PORT_TBL_SET_PP_GLAG_OUT_PORT_TBL_SET_TBL_DATA_OUT_SYSTEM_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.out_system_port = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_glag_out_port_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_glag_out_port_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_glag_out_port_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_glag_out_port_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_glag_out_port_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_GLAG_OUT_PORT_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_glag_out_port_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_GLAG_OUT_PORT_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_GLAG_OUT_PORT_TBL_GET_PP_GLAG_OUT_PORT_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_GLAG_OUT_PORT_TBL_GET_PP_GLAG_OUT_PORT_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_glag_out_port_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_glag_out_port_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_glag_out_port_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_glag_out_port_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_GLAG_OUT_PORT_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_vid_mirr_cami_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_vid_mirr_cami_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_VID_MIRR_CAMI_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_vid_mirr_cami_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_VID_MIRR_CAMI_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_VID_MIRR_CAMI_TBL_SET_PP_VID_MIRR_CAMI_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_VID_MIRR_CAMI_TBL_SET_PP_VID_MIRR_CAMI_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_vid_mirr_cami_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_vid_mirr_cami_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_vid_mirr_cami_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_vid_mirr_cami_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_VID_MIRR_CAMI_TBL_SET_PP_VID_MIRR_CAMI_TBL_SET_TBL_DATA_VID_MIRR_BMAP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_VID_MIRR_CAMI_TBL_SET_PP_VID_MIRR_CAMI_TBL_SET_TBL_DATA_VID_MIRR_BMAP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.vid_mirr_bmap = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_vid_mirr_cami_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_vid_mirr_cami_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_vid_mirr_cami_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_vid_mirr_cami_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_vid_mirr_cami_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_VID_MIRR_CAMI_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_vid_mirr_cami_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_VID_MIRR_CAMI_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_VID_MIRR_CAMI_TBL_GET_PP_VID_MIRR_CAMI_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_VID_MIRR_CAMI_TBL_GET_PP_VID_MIRR_CAMI_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_vid_mirr_cami_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_vid_mirr_cami_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_vid_mirr_cami_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_vid_mirr_cami_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_VID_MIRR_CAMI_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_vid_mirr_profile_table_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_vid_mirr_profile_table_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_VID_MIRR_PROFILE_TABLE_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_vid_mirr_profile_table_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_VID_MIRR_PROFILE_TABLE_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_VID_MIRR_PROFILE_TABLE_TBL_SET_PP_VID_MIRR_PROFILE_TABLE_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_VID_MIRR_PROFILE_TABLE_TBL_SET_PP_VID_MIRR_PROFILE_TABLE_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_vid_mirr_profile_table_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_vid_mirr_profile_table_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_vid_mirr_profile_table_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_vid_mirr_profile_table_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_VID_MIRR_PROFILE_TABLE_TBL_SET_PP_VID_MIRR_PROFILE_TABLE_TBL_SET_TBL_DATA_VID_MIRR_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_VID_MIRR_PROFILE_TABLE_TBL_SET_PP_VID_MIRR_PROFILE_TABLE_TBL_SET_TBL_DATA_VID_MIRR_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.vid_mirr_profile = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_vid_mirr_profile_table_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_vid_mirr_profile_table_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_vid_mirr_profile_table_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_vid_mirr_profile_table_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_vid_mirr_profile_table_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_VID_MIRR_PROFILE_TABLE_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_vid_mirr_profile_table_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_VID_MIRR_PROFILE_TABLE_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_VID_MIRR_PROFILE_TABLE_TBL_GET_PP_VID_MIRR_PROFILE_TABLE_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_VID_MIRR_PROFILE_TABLE_TBL_GET_PP_VID_MIRR_PROFILE_TABLE_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_vid_mirr_profile_table_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_vid_mirr_profile_table_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_vid_mirr_profile_table_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_vid_mirr_profile_table_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_VID_MIRR_PROFILE_TABLE_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_output_port_config1_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_output_port_config1_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_OUTPUT_PORT_CONFIG1_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_output_port_config1_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_OUTPUT_PORT_CONFIG1_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_output_port_config1_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_output_port_config1_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_output_port_config1_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_output_port_config1_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_TBL_DATA_MTU_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_TBL_DATA_MTU_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.mtu = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_TBL_DATA_DEFAULT_SNOOP_COMMAND_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_TBL_DATA_DEFAULT_SNOOP_COMMAND_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.default_snoop_command = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_TBL_DATA_DEFAULT_ACTION_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_TBL_DATA_DEFAULT_ACTION_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.default_action_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_TBL_DATA_OUTPORT_SYS_PORT_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_TBL_DATA_OUTPORT_SYS_PORT_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.outport_sys_port_id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_TBL_DATA_OUTPORT_PORT_IS_GLAG_MEMBER_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_TBL_DATA_OUTPORT_PORT_IS_GLAG_MEMBER_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.outport_port_is_glag_member = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_TBL_DATA_MTU_CHECK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_TBL_DATA_MTU_CHECK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.mtu_check = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_TBL_DATA_SPLIT_HORIZON_FILTER_ENABLED_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_TBL_DATA_SPLIT_HORIZON_FILTER_ENABLED_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.split_horizon_filter_enabled = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_TBL_DATA_UNKNOWN_DA_BC_FILTER_ENABLED_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_TBL_DATA_UNKNOWN_DA_BC_FILTER_ENABLED_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.unknown_da_bc_filter_enabled = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_TBL_DATA_UNKNOWN_DA_MC_FILTER_ENABLED_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_TBL_DATA_UNKNOWN_DA_MC_FILTER_ENABLED_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.unknown_da_mc_filter_enabled = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_TBL_DATA_UNKNOWN_DA_UC_FILTER_ENABLED_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_TBL_DATA_UNKNOWN_DA_UC_FILTER_ENABLED_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.unknown_da_uc_filter_enabled = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_TBL_DATA_HAIR_PIN_FILTERING_ENABLED_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_TBL_DATA_HAIR_PIN_FILTERING_ENABLED_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.hair_pin_filtering_enabled = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_TBL_DATA_PEP_ACC_FRM_TYPES_FILTERING_ENABLED_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_TBL_DATA_PEP_ACC_FRM_TYPES_FILTERING_ENABLED_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.pep_acc_frm_types_filtering_enabled = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_TBL_DATA_STP_STATE_FILTERING_ENABLED_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_TBL_DATA_STP_STATE_FILTERING_ENABLED_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.stp_state_filtering_enabled = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_TBL_DATA_VSI_FILTERING_ENABLED_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_TBL_DATA_VSI_FILTERING_ENABLED_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.vsi_filtering_enabled = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_TBL_DATA_DISABLE_FILTERING_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_TBL_DATA_DISABLE_FILTERING_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.disable_filtering = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_TBL_DATA_DISABLE_LEARNING_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_TBL_DATA_DISABLE_LEARNING_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.disable_learning = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_TBL_DATA_PORT_TYPE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_TBL_DATA_PORT_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.port_type = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_output_port_config1_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_output_port_config1_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_output_port_config1_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_output_port_config1_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_output_port_config1_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_OUTPUT_PORT_CONFIG1_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_output_port_config1_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_OUTPUT_PORT_CONFIG1_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_GET_PP_OUTPUT_PORT_CONFIG1_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_GET_PP_OUTPUT_PORT_CONFIG1_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_output_port_config1_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_output_port_config1_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_output_port_config1_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_output_port_config1_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_OUTPUT_PORT_CONFIG1_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_ingress_vlan_edit_command_map_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_ingress_vlan_edit_command_map_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_ingress_vlan_edit_command_map_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_ingress_vlan_edit_command_map_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_ingress_vlan_edit_command_map_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_ingress_vlan_edit_command_map_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_ingress_vlan_edit_command_map_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_TBL_DATA_BYTES_TO_REMOVE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_TBL_DATA_BYTES_TO_REMOVE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.bytes_to_remove = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_TBL_DATA_INNER_TPID_SOURCE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_TBL_DATA_INNER_TPID_SOURCE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.tpid_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_TBL_DATA_INNER_VID_SOURCE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_TBL_DATA_INNER_VID_SOURCE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.inner_vid_source = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_TBL_DATA_INNER_PCP_DEI_SOURCE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_TBL_DATA_INNER_PCP_DEI_SOURCE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.inner_pcp_dei_source = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_TBL_DATA_OUTER_VID_SOURCE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_TBL_DATA_OUTER_VID_SOURCE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.outer_vid_source = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_TBL_DATA_OUTER_PCP_DEI_SOURCE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_TBL_DATA_OUTER_PCP_DEI_SOURCE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.outer_pcp_dei_source = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_ingress_vlan_edit_command_map_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_ingress_vlan_edit_command_map_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_ingress_vlan_edit_command_map_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_ingress_vlan_edit_command_map_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_ingress_vlan_edit_command_map_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_ingress_vlan_edit_command_map_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_GET_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_GET_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_ingress_vlan_edit_command_map_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_ingress_vlan_edit_command_map_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_ingress_vlan_edit_command_map_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_ingress_vlan_edit_command_map_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_egress_mpls_profile1_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_egress_mpls_profile1_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_EGRESS_MPLS_PROFILE1_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_egress_mpls_profile1_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_EGRESS_MPLS_PROFILE1_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_MPLS_PROFILE1_TBL_SET_PP_EGRESS_MPLS_PROFILE1_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_MPLS_PROFILE1_TBL_SET_PP_EGRESS_MPLS_PROFILE1_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_egress_mpls_profile1_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_egress_mpls_profile1_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_mpls_profile1_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_mpls_profile1_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_MPLS_PROFILE1_TBL_SET_PP_EGRESS_MPLS_PROFILE1_TBL_SET_TBL_DATA_CW_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_MPLS_PROFILE1_TBL_SET_PP_EGRESS_MPLS_PROFILE1_TBL_SET_TBL_DATA_CW_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.cw = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_MPLS_PROFILE1_TBL_SET_PP_EGRESS_MPLS_PROFILE1_TBL_SET_TBL_DATA_TTL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_MPLS_PROFILE1_TBL_SET_PP_EGRESS_MPLS_PROFILE1_TBL_SET_TBL_DATA_TTL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.ttl = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_MPLS_PROFILE1_TBL_SET_PP_EGRESS_MPLS_PROFILE1_TBL_SET_TBL_DATA_PIPE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_MPLS_PROFILE1_TBL_SET_PP_EGRESS_MPLS_PROFILE1_TBL_SET_TBL_DATA_PIPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.pipe = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_egress_mpls_profile1_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_mpls_profile1_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_mpls_profile1_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_egress_mpls_profile1_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_egress_mpls_profile1_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_EGRESS_MPLS_PROFILE1_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_egress_mpls_profile1_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_EGRESS_MPLS_PROFILE1_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_MPLS_PROFILE1_TBL_GET_PP_EGRESS_MPLS_PROFILE1_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_MPLS_PROFILE1_TBL_GET_PP_EGRESS_MPLS_PROFILE1_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_egress_mpls_profile1_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_egress_mpls_profile1_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_mpls_profile1_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_mpls_profile1_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_EGRESS_MPLS_PROFILE1_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_egress_mpls_profile2_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_egress_mpls_profile2_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_EGRESS_MPLS_PROFILE2_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_egress_mpls_profile2_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_EGRESS_MPLS_PROFILE2_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_MPLS_PROFILE2_TBL_SET_PP_EGRESS_MPLS_PROFILE2_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_MPLS_PROFILE2_TBL_SET_PP_EGRESS_MPLS_PROFILE2_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_egress_mpls_profile2_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_egress_mpls_profile2_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_mpls_profile2_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_mpls_profile2_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_MPLS_PROFILE2_TBL_SET_PP_EGRESS_MPLS_PROFILE2_TBL_SET_TBL_DATA_EXP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_MPLS_PROFILE2_TBL_SET_PP_EGRESS_MPLS_PROFILE2_TBL_SET_TBL_DATA_EXP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.exp = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_egress_mpls_profile2_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_mpls_profile2_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_mpls_profile2_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_egress_mpls_profile2_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_egress_mpls_profile2_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_EGRESS_MPLS_PROFILE2_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_egress_mpls_profile2_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_EGRESS_MPLS_PROFILE2_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_MPLS_PROFILE2_TBL_GET_PP_EGRESS_MPLS_PROFILE2_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_MPLS_PROFILE2_TBL_GET_PP_EGRESS_MPLS_PROFILE2_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_egress_mpls_profile2_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_egress_mpls_profile2_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_mpls_profile2_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_mpls_profile2_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_EGRESS_MPLS_PROFILE2_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_egress_encapsulation_link_layer_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_egress_encapsulation_link_layer_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  uint32
    prm_dest_mac_index = 0xFFFFFFFF;
  T20E_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_egress_encapsulation_link_layer_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_egress_encapsulation_link_layer_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_egress_encapsulation_link_layer_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_encapsulation_link_layer_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_encapsulation_link_layer_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_TBL_DATA_OUT_PCP_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_TBL_DATA_OUT_PCP_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.out_pcp_valid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_TBL_DATA_OUT_PCP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_TBL_DATA_OUT_PCP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.out_pcp = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_TBL_DATA_OUT_VID_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_TBL_DATA_OUT_VID_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.out_vid_valid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_TBL_DATA_OUT_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_TBL_DATA_OUT_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.out_vid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_TBL_DATA_OUT_TPID_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_TBL_DATA_OUT_TPID_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.out_tpid_index = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_TBL_DATA_DEST_MAC_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_TBL_DATA_DEST_MAC_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_dest_mac_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_dest_mac_index != 0xFFFFFFFF)
  {

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_TBL_DATA_DEST_MAC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_TBL_DATA_DEST_MAC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.dest_mac[ prm_dest_mac_index] = (uint32)param_val->value.ulong_value;
  }

  }


  /* Call function */
  ret = t20e_pp_egress_encapsulation_link_layer_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_encapsulation_link_layer_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_encapsulation_link_layer_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_egress_encapsulation_link_layer_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_egress_encapsulation_link_layer_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_egress_encapsulation_link_layer_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_GET_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_GET_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_egress_encapsulation_link_layer_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_egress_encapsulation_link_layer_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_encapsulation_link_layer_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_encapsulation_link_layer_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_egress_out_vsi_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_egress_out_vsi_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_EGRESS_OUT_VSI_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_egress_out_vsi_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_EGRESS_OUT_VSI_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_OUT_VSI_TBL_SET_PP_EGRESS_OUT_VSI_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_OUT_VSI_TBL_SET_PP_EGRESS_OUT_VSI_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_egress_out_vsi_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_egress_out_vsi_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_out_vsi_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_out_vsi_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_OUT_VSI_TBL_SET_PP_EGRESS_OUT_VSI_TBL_SET_TBL_DATA_SA_LSB_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_OUT_VSI_TBL_SET_PP_EGRESS_OUT_VSI_TBL_SET_TBL_DATA_SA_LSB_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.sa_lsb = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_egress_out_vsi_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_out_vsi_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_out_vsi_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_egress_out_vsi_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_egress_out_vsi_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_EGRESS_OUT_VSI_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_egress_out_vsi_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_EGRESS_OUT_VSI_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_OUT_VSI_TBL_GET_PP_EGRESS_OUT_VSI_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_OUT_VSI_TBL_GET_PP_EGRESS_OUT_VSI_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_egress_out_vsi_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_egress_out_vsi_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_out_vsi_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_out_vsi_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_EGRESS_OUT_VSI_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_egress_llvp_table_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_egress_llvp_table_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_EGRESS_LLVP_TABLE_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_egress_llvp_table_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_EGRESS_LLVP_TABLE_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_LLVP_TABLE_TBL_SET_PP_EGRESS_LLVP_TABLE_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_LLVP_TABLE_TBL_SET_PP_EGRESS_LLVP_TABLE_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_egress_llvp_table_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_egress_llvp_table_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_llvp_table_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_llvp_table_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_LLVP_TABLE_TBL_SET_PP_EGRESS_LLVP_TABLE_TBL_SET_TBL_DATA_INCOMING_TAG_FORMAT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_LLVP_TABLE_TBL_SET_PP_EGRESS_LLVP_TABLE_TBL_SET_TBL_DATA_INCOMING_TAG_FORMAT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.incoming_tag_format = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_egress_llvp_table_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_llvp_table_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_llvp_table_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_egress_llvp_table_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_egress_llvp_table_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_EGRESS_LLVP_TABLE_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_egress_llvp_table_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_EGRESS_LLVP_TABLE_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_LLVP_TABLE_TBL_GET_PP_EGRESS_LLVP_TABLE_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_LLVP_TABLE_TBL_GET_PP_EGRESS_LLVP_TABLE_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_egress_llvp_table_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_egress_llvp_table_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_llvp_table_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_llvp_table_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_EGRESS_LLVP_TABLE_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_egress_vlan_edit_command_map_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_egress_vlan_edit_command_map_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_incoming_tag_format_ndx;
  uint32
    prm_out_tag_profile_ndx;
  T20E_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_egress_vlan_edit_command_map_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_INCOMING_TAG_FORMAT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_INCOMING_TAG_FORMAT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_incoming_tag_format_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter incoming_tag_format_ndx after pp_egress_vlan_edit_command_map_tbl_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_OUT_TAG_PROFILE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_OUT_TAG_PROFILE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_out_tag_profile_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter out_tag_profile_ndx after pp_egress_vlan_edit_command_map_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_egress_vlan_edit_command_map_tbl_get(
          unit,
          T20E_TBL_EG_VLAN_EDIT_COMMAND_MAP_ENTRY_INDX(prm_incoming_tag_format_ndx,prm_out_tag_profile_ndx),
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_vlan_edit_command_map_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_vlan_edit_command_map_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_TBL_DATA_BYTES_TO_REMOVE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_TBL_DATA_BYTES_TO_REMOVE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.bytes_to_remove = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_TBL_DATA_INNER_PCP_DEI_SOURCE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_TBL_DATA_INNER_PCP_DEI_SOURCE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.inner_pcp_dei_source = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_TBL_DATA_OUTER_PCP_DEI_SOURCE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_TBL_DATA_OUTER_PCP_DEI_SOURCE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.outer_pcp_dei_source = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_TBL_DATA_INNER_VID_SOURCE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_TBL_DATA_INNER_VID_SOURCE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.inner_vid_source = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_TBL_DATA_OUTER_VID_SOURCE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_TBL_DATA_OUTER_VID_SOURCE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.outer_vid_source = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_TBL_DATA_INNER_TPID_TYPE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_TBL_DATA_INNER_TPID_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.inner_tpid_type = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_TBL_DATA_OUTER_TPID_TYPE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_TBL_DATA_OUTER_TPID_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.outer_tpid_type = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_egress_vlan_edit_command_map_tbl_set(
          unit,
          T20E_TBL_EG_VLAN_EDIT_COMMAND_MAP_ENTRY_INDX(prm_incoming_tag_format_ndx,prm_out_tag_profile_ndx),
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_vlan_edit_command_map_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_vlan_edit_command_map_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_egress_vlan_edit_command_map_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_egress_vlan_edit_command_map_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_incoming_tag_format_ndx;
  uint32
    prm_out_tag_profile_ndx;
  T20E_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_egress_vlan_edit_command_map_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_GET_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_GET_INCOMING_TAG_FORMAT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_GET_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_GET_INCOMING_TAG_FORMAT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_incoming_tag_format_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter incoming_tag_format_ndx after pp_egress_vlan_edit_command_map_tbl_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_GET_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_GET_OUT_TAG_PROFILE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_GET_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_GET_OUT_TAG_PROFILE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_out_tag_profile_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter out_tag_profile_ndx after pp_egress_vlan_edit_command_map_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_egress_vlan_edit_command_map_tbl_get(
          unit,
          T20E_TBL_EG_VLAN_EDIT_COMMAND_MAP_ENTRY_INDX(prm_incoming_tag_format_ndx,prm_out_tag_profile_ndx),
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_vlan_edit_command_map_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_vlan_edit_command_map_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_egress_pcp_dei_map_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_egress_pcp_dei_map_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_EGRESS_PCP_DEI_MAP_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_egress_pcp_dei_map_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_EGRESS_PCP_DEI_MAP_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_PCP_DEI_MAP_TBL_SET_PP_EGRESS_PCP_DEI_MAP_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_PCP_DEI_MAP_TBL_SET_PP_EGRESS_PCP_DEI_MAP_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_egress_pcp_dei_map_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_egress_pcp_dei_map_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_pcp_dei_map_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_pcp_dei_map_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_PCP_DEI_MAP_TBL_SET_PP_EGRESS_PCP_DEI_MAP_TBL_SET_TBL_DATA_PCP_DEI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_PCP_DEI_MAP_TBL_SET_PP_EGRESS_PCP_DEI_MAP_TBL_SET_TBL_DATA_PCP_DEI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.pcp_dei = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_egress_pcp_dei_map_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_pcp_dei_map_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_pcp_dei_map_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_egress_pcp_dei_map_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_egress_pcp_dei_map_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_EGRESS_PCP_DEI_MAP_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_egress_pcp_dei_map_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_EGRESS_PCP_DEI_MAP_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_PCP_DEI_MAP_TBL_GET_PP_EGRESS_PCP_DEI_MAP_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_PCP_DEI_MAP_TBL_GET_PP_EGRESS_PCP_DEI_MAP_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_egress_pcp_dei_map_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_egress_pcp_dei_map_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_pcp_dei_map_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_pcp_dei_map_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_EGRESS_PCP_DEI_MAP_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_egress_tc_dp_to_up_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_egress_tc_dp_to_up_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_EGRESS_TC_DP_TO_UP_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_egress_tc_dp_to_up_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_EGRESS_TC_DP_TO_UP_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_TC_DP_TO_UP_TBL_SET_PP_EGRESS_TC_DP_TO_UP_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_TC_DP_TO_UP_TBL_SET_PP_EGRESS_TC_DP_TO_UP_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_egress_tc_dp_to_up_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_egress_tc_dp_to_up_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_tc_dp_to_up_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_tc_dp_to_up_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_TC_DP_TO_UP_TBL_SET_PP_EGRESS_TC_DP_TO_UP_TBL_SET_TBL_DATA_USER_PRIORITY_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_TC_DP_TO_UP_TBL_SET_PP_EGRESS_TC_DP_TO_UP_TBL_SET_TBL_DATA_USER_PRIORITY_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.user_priority = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_egress_tc_dp_to_up_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_tc_dp_to_up_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_tc_dp_to_up_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_egress_tc_dp_to_up_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_egress_tc_dp_to_up_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_EGRESS_TC_DP_TO_UP_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_egress_tc_dp_to_up_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_EGRESS_TC_DP_TO_UP_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_TC_DP_TO_UP_TBL_GET_PP_EGRESS_TC_DP_TO_UP_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_TC_DP_TO_UP_TBL_GET_PP_EGRESS_TC_DP_TO_UP_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_egress_tc_dp_to_up_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_egress_tc_dp_to_up_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_tc_dp_to_up_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_tc_dp_to_up_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_EGRESS_TC_DP_TO_UP_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_egress_port_vlan_membership_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_egress_port_vlan_membership_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_EGRESS_PORT_VLAN_MEMBERSHIP_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_egress_port_vlan_membership_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_EGRESS_PORT_VLAN_MEMBERSHIP_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_PORT_VLAN_MEMBERSHIP_TBL_SET_PP_EGRESS_PORT_VLAN_MEMBERSHIP_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_PORT_VLAN_MEMBERSHIP_TBL_SET_PP_EGRESS_PORT_VLAN_MEMBERSHIP_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_egress_port_vlan_membership_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_egress_port_vlan_membership_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_port_vlan_membership_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_port_vlan_membership_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_PORT_VLAN_MEMBERSHIP_TBL_SET_PP_EGRESS_PORT_VLAN_MEMBERSHIP_TBL_SET_TBL_DATA_PORT_VID_UNTAGGED_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_PORT_VLAN_MEMBERSHIP_TBL_SET_PP_EGRESS_PORT_VLAN_MEMBERSHIP_TBL_SET_TBL_DATA_PORT_VID_UNTAGGED_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.port_vid_untagged = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_egress_port_vlan_membership_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_port_vlan_membership_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_port_vlan_membership_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_egress_port_vlan_membership_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_egress_port_vlan_membership_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_EGRESS_PORT_VLAN_MEMBERSHIP_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_egress_port_vlan_membership_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_EGRESS_PORT_VLAN_MEMBERSHIP_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_PORT_VLAN_MEMBERSHIP_TBL_GET_PP_EGRESS_PORT_VLAN_MEMBERSHIP_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_PORT_VLAN_MEMBERSHIP_TBL_GET_PP_EGRESS_PORT_VLAN_MEMBERSHIP_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_egress_port_vlan_membership_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_egress_port_vlan_membership_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_port_vlan_membership_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_port_vlan_membership_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_EGRESS_PORT_VLAN_MEMBERSHIP_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_egress_stp_state_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_egress_stp_state_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_topology_id_ndx;
  uint32
    prm_outport_ndx;
  T20E_PP_EGRESS_STP_STATE_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_egress_stp_state_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_EGRESS_STP_STATE_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_STP_STATE_TBL_SET_PP_EGRESS_STP_STATE_TBL_SET_TOPOLOGY_ID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_STP_STATE_TBL_SET_PP_EGRESS_STP_STATE_TBL_SET_TOPOLOGY_ID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_topology_id_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter topology_id_ndx after pp_egress_stp_state_tbl_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_STP_STATE_TBL_SET_PP_EGRESS_STP_STATE_TBL_SET_OUTPORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_STP_STATE_TBL_SET_PP_EGRESS_STP_STATE_TBL_SET_OUTPORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_outport_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter outport_ndx after pp_egress_stp_state_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_egress_stp_state_tbl_get(
          unit,
          T20E_TBL_EG_PORT_TOPOLOGY_TO_ENTRY_INDX(prm_topology_id_ndx,prm_outport_ndx),
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_stp_state_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_stp_state_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_STP_STATE_TBL_SET_PP_EGRESS_STP_STATE_TBL_SET_TBL_DATA_STATE_STATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_STP_STATE_TBL_SET_PP_EGRESS_STP_STATE_TBL_SET_TBL_DATA_STATE_STATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.state_state = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_egress_stp_state_tbl_set(
          unit,
          T20E_TBL_EG_PORT_TOPOLOGY_TO_ENTRY_INDX(prm_topology_id_ndx,prm_outport_ndx),
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_stp_state_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_stp_state_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_egress_stp_state_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_egress_stp_state_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_topology_id_ndx;
  uint32
    prm_outport_ndx;
  T20E_PP_EGRESS_STP_STATE_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_egress_stp_state_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_EGRESS_STP_STATE_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_STP_STATE_TBL_GET_PP_EGRESS_STP_STATE_TBL_GET_TOPOLOGY_ID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_STP_STATE_TBL_GET_PP_EGRESS_STP_STATE_TBL_GET_TOPOLOGY_ID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_topology_id_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter topology_id_ndx after pp_egress_stp_state_tbl_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_STP_STATE_TBL_GET_PP_EGRESS_STP_STATE_TBL_GET_OUTPORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_STP_STATE_TBL_GET_PP_EGRESS_STP_STATE_TBL_GET_OUTPORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_outport_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter outport_ndx after pp_egress_stp_state_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_egress_stp_state_tbl_get(
          unit,
          T20E_TBL_EG_PORT_TOPOLOGY_TO_ENTRY_INDX(prm_topology_id_ndx,prm_outport_ndx),
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_stp_state_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_stp_state_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_EGRESS_STP_STATE_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_egress_acceptable_frame_types_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_egress_acceptable_frame_types_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_vlan_format_ndx;
  uint32
    prm_pep_acc_profile_ndx;
  T20E_PP_EGRESS_ACCEPTABLE_FRAME_TYPES_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_egress_acceptable_frame_types_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_EGRESS_ACCEPTABLE_FRAME_TYPES_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_ACCEPTABLE_FRAME_TYPES_TBL_SET_PP_EGRESS_ACCEPTABLE_FRAME_TYPES_TBL_SET_VLAN_FORMAT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_ACCEPTABLE_FRAME_TYPES_TBL_SET_PP_EGRESS_ACCEPTABLE_FRAME_TYPES_TBL_SET_VLAN_FORMAT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vlan_format_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vlan_format_ndx after pp_egress_acceptable_frame_types_tbl_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_ACCEPTABLE_FRAME_TYPES_TBL_SET_PP_EGRESS_ACCEPTABLE_FRAME_TYPES_TBL_SET_PEP_ACC_PROFILE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_ACCEPTABLE_FRAME_TYPES_TBL_SET_PP_EGRESS_ACCEPTABLE_FRAME_TYPES_TBL_SET_PEP_ACC_PROFILE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pep_acc_profile_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter pep_acc_profile_ndx after pp_egress_acceptable_frame_types_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_egress_acceptable_frame_types_tbl_get(
          unit,
          T20E_TBL_EG_ACC_FRM_TYPE_TO_ENTRY_INDX(prm_vlan_format_ndx,prm_pep_acc_profile_ndx),
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_acceptable_frame_types_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_acceptable_frame_types_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_ACCEPTABLE_FRAME_TYPES_TBL_SET_PP_EGRESS_ACCEPTABLE_FRAME_TYPES_TBL_SET_TBL_DATA_ACCEPTABLE_FRAME_TYPE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_ACCEPTABLE_FRAME_TYPES_TBL_SET_PP_EGRESS_ACCEPTABLE_FRAME_TYPES_TBL_SET_TBL_DATA_ACCEPTABLE_FRAME_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.acceptable_frame_type = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_egress_acceptable_frame_types_tbl_set(
          unit,
          T20E_TBL_EG_ACC_FRM_TYPE_TO_ENTRY_INDX(prm_vlan_format_ndx,prm_pep_acc_profile_ndx),
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_acceptable_frame_types_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_acceptable_frame_types_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_egress_acceptable_frame_types_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_egress_acceptable_frame_types_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_vlan_format_ndx;
  uint32
    prm_pep_acc_profile_ndx;
  T20E_PP_EGRESS_ACCEPTABLE_FRAME_TYPES_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_egress_acceptable_frame_types_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_EGRESS_ACCEPTABLE_FRAME_TYPES_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_ACCEPTABLE_FRAME_TYPES_TBL_GET_PP_EGRESS_ACCEPTABLE_FRAME_TYPES_TBL_GET_VLAN_FORMAT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_ACCEPTABLE_FRAME_TYPES_TBL_GET_PP_EGRESS_ACCEPTABLE_FRAME_TYPES_TBL_GET_VLAN_FORMAT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vlan_format_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vlan_format_ndx after pp_egress_acceptable_frame_types_tbl_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_ACCEPTABLE_FRAME_TYPES_TBL_GET_PP_EGRESS_ACCEPTABLE_FRAME_TYPES_TBL_GET_PEP_ACC_PROFILE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_ACCEPTABLE_FRAME_TYPES_TBL_GET_PP_EGRESS_ACCEPTABLE_FRAME_TYPES_TBL_GET_PEP_ACC_PROFILE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pep_acc_profile_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter pep_acc_profile_ndx after pp_egress_acceptable_frame_types_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_egress_acceptable_frame_types_tbl_get(
          unit,
          T20E_TBL_EG_ACC_FRM_TYPE_TO_ENTRY_INDX(prm_vlan_format_ndx,prm_pep_acc_profile_ndx),
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_acceptable_frame_types_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_acceptable_frame_types_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_EGRESS_ACCEPTABLE_FRAME_TYPES_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_egress_mep_level_profile_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_egress_mep_level_profile_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_egress_mep_level_profile_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_egress_mep_level_profile_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_egress_mep_level_profile_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_mep_level_profile_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_mep_level_profile_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_TBL_DATA_LEVEL7_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_TBL_DATA_LEVEL7_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.level7 = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_TBL_DATA_LEVEL6_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_TBL_DATA_LEVEL6_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.level6 = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_TBL_DATA_LEVEL5_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_TBL_DATA_LEVEL5_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.level5 = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_TBL_DATA_LEVEL4_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_TBL_DATA_LEVEL4_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.level4 = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_TBL_DATA_LEVEL3_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_TBL_DATA_LEVEL3_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.level3 = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_TBL_DATA_LEVEL2_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_TBL_DATA_LEVEL2_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.level2 = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_TBL_DATA_LEVEL1_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_TBL_DATA_LEVEL1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.level1 = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_TBL_DATA_LEVEL0_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_TBL_DATA_LEVEL0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.level0 = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_egress_mep_level_profile_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_mep_level_profile_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_mep_level_profile_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_egress_mep_level_profile_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_egress_mep_level_profile_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_egress_mep_level_profile_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_GET_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_GET_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_egress_mep_level_profile_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_egress_mep_level_profile_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_mep_level_profile_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_mep_level_profile_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_egress_oam_action_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_egress_oam_action_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_EGRESS_OAM_ACTION_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_egress_oam_action_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_EGRESS_OAM_ACTION_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_OAM_ACTION_TBL_SET_PP_EGRESS_OAM_ACTION_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_OAM_ACTION_TBL_SET_PP_EGRESS_OAM_ACTION_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_egress_oam_action_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_egress_oam_action_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_oam_action_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_oam_action_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_OAM_ACTION_TBL_SET_PP_EGRESS_OAM_ACTION_TBL_SET_TBL_DATA_OAM_ACTION_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_OAM_ACTION_TBL_SET_PP_EGRESS_OAM_ACTION_TBL_SET_TBL_DATA_OAM_ACTION_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.oam_action_profile = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_egress_oam_action_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_oam_action_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_oam_action_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_egress_oam_action_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_egress_oam_action_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_EGRESS_OAM_ACTION_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_egress_oam_action_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_EGRESS_OAM_ACTION_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_OAM_ACTION_TBL_GET_PP_EGRESS_OAM_ACTION_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_OAM_ACTION_TBL_GET_PP_EGRESS_OAM_ACTION_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_egress_oam_action_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_egress_oam_action_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_oam_action_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_oam_action_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_EGRESS_OAM_ACTION_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_egress_action_profile_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_egress_action_profile_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_EGRESS_ACTION_PROFILE_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_egress_action_profile_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_EGRESS_ACTION_PROFILE_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_ACTION_PROFILE_TBL_SET_PP_EGRESS_ACTION_PROFILE_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_ACTION_PROFILE_TBL_SET_PP_EGRESS_ACTION_PROFILE_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_egress_action_profile_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_egress_action_profile_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_action_profile_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_action_profile_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_ACTION_PROFILE_TBL_SET_PP_EGRESS_ACTION_PROFILE_TBL_SET_TBL_DATA_STRENGTH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_ACTION_PROFILE_TBL_SET_PP_EGRESS_ACTION_PROFILE_TBL_SET_TBL_DATA_STRENGTH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.strength = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_ACTION_PROFILE_TBL_SET_PP_EGRESS_ACTION_PROFILE_TBL_SET_TBL_DATA_DP_OW_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_ACTION_PROFILE_TBL_SET_PP_EGRESS_ACTION_PROFILE_TBL_SET_TBL_DATA_DP_OW_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.dp_ow = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_ACTION_PROFILE_TBL_SET_PP_EGRESS_ACTION_PROFILE_TBL_SET_TBL_DATA_DP_VALUE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_ACTION_PROFILE_TBL_SET_PP_EGRESS_ACTION_PROFILE_TBL_SET_TBL_DATA_DP_VALUE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.dp_value = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_ACTION_PROFILE_TBL_SET_PP_EGRESS_ACTION_PROFILE_TBL_SET_TBL_DATA_TC_OW_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_ACTION_PROFILE_TBL_SET_PP_EGRESS_ACTION_PROFILE_TBL_SET_TBL_DATA_TC_OW_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.tc_ow = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_ACTION_PROFILE_TBL_SET_PP_EGRESS_ACTION_PROFILE_TBL_SET_TBL_DATA_TC_VALUE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_ACTION_PROFILE_TBL_SET_PP_EGRESS_ACTION_PROFILE_TBL_SET_TBL_DATA_TC_VALUE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.tc_value = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_ACTION_PROFILE_TBL_SET_PP_EGRESS_ACTION_PROFILE_TBL_SET_TBL_DATA_DESTINATION_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_ACTION_PROFILE_TBL_SET_PP_EGRESS_ACTION_PROFILE_TBL_SET_TBL_DATA_DESTINATION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.destination = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_ACTION_PROFILE_TBL_SET_PP_EGRESS_ACTION_PROFILE_TBL_SET_TBL_DATA_DROP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_ACTION_PROFILE_TBL_SET_PP_EGRESS_ACTION_PROFILE_TBL_SET_TBL_DATA_DROP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.drop = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_ACTION_PROFILE_TBL_SET_PP_EGRESS_ACTION_PROFILE_TBL_SET_TBL_DATA_CONTROL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_ACTION_PROFILE_TBL_SET_PP_EGRESS_ACTION_PROFILE_TBL_SET_TBL_DATA_CONTROL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.control = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_ACTION_PROFILE_TBL_SET_PP_EGRESS_ACTION_PROFILE_TBL_SET_TBL_DATA_TRAP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_ACTION_PROFILE_TBL_SET_PP_EGRESS_ACTION_PROFILE_TBL_SET_TBL_DATA_TRAP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.trap = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_egress_action_profile_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_action_profile_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_action_profile_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_egress_action_profile_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_egress_action_profile_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_EGRESS_ACTION_PROFILE_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_egress_action_profile_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_EGRESS_ACTION_PROFILE_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_ACTION_PROFILE_TBL_GET_PP_EGRESS_ACTION_PROFILE_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_ACTION_PROFILE_TBL_GET_PP_EGRESS_ACTION_PROFILE_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_egress_action_profile_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_egress_action_profile_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_action_profile_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_action_profile_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_EGRESS_ACTION_PROFILE_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_egress_snoop_profile_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_egress_snoop_profile_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_EGRESS_SNOOP_PROFILE_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_egress_snoop_profile_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_EGRESS_SNOOP_PROFILE_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_SNOOP_PROFILE_TBL_SET_PP_EGRESS_SNOOP_PROFILE_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_SNOOP_PROFILE_TBL_SET_PP_EGRESS_SNOOP_PROFILE_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_egress_snoop_profile_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_egress_snoop_profile_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_snoop_profile_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_snoop_profile_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_SNOOP_PROFILE_TBL_SET_PP_EGRESS_SNOOP_PROFILE_TBL_SET_TBL_DATA_STRENGTH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_SNOOP_PROFILE_TBL_SET_PP_EGRESS_SNOOP_PROFILE_TBL_SET_TBL_DATA_STRENGTH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.strength = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_egress_snoop_profile_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_snoop_profile_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_snoop_profile_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_egress_snoop_profile_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_egress_snoop_profile_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_EGRESS_SNOOP_PROFILE_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_egress_snoop_profile_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_EGRESS_SNOOP_PROFILE_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_SNOOP_PROFILE_TBL_GET_PP_EGRESS_SNOOP_PROFILE_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_SNOOP_PROFILE_TBL_GET_PP_EGRESS_SNOOP_PROFILE_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_egress_snoop_profile_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_egress_snoop_profile_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_snoop_profile_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_snoop_profile_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_EGRESS_SNOOP_PROFILE_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_fec_low_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_fec_low_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_FEC_LOW_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_fec_low_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_FEC_LOW_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_FEC_LOW_TBL_SET_PP_FEC_LOW_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_FEC_LOW_TBL_SET_PP_FEC_LOW_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_fec_low_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_fec_low_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_fec_low_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_fec_low_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_FEC_LOW_TBL_SET_PP_FEC_LOW_TBL_SET_TBL_DATA_SECONDARY_OUTLIF_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_FEC_LOW_TBL_SET_PP_FEC_LOW_TBL_SET_TBL_DATA_SECONDARY_OUTLIF_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.secondary_outlif = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_FEC_LOW_TBL_SET_PP_FEC_LOW_TBL_SET_TBL_DATA_VALID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_FEC_LOW_TBL_SET_PP_FEC_LOW_TBL_SET_TBL_DATA_VALID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.valid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_FEC_LOW_TBL_SET_PP_FEC_LOW_TBL_SET_TBL_DATA_PRIMARY_OUTLIF_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_FEC_LOW_TBL_SET_PP_FEC_LOW_TBL_SET_TBL_DATA_PRIMARY_OUTLIF_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.primary_outlif = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_FEC_LOW_TBL_SET_PP_FEC_LOW_TBL_SET_TBL_DATA_SECONDARY_DESTINATION_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_FEC_LOW_TBL_SET_PP_FEC_LOW_TBL_SET_TBL_DATA_SECONDARY_DESTINATION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.secondary_destination = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_FEC_LOW_TBL_SET_PP_FEC_LOW_TBL_SET_TBL_DATA_PRIMARY_DESTINATION_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_FEC_LOW_TBL_SET_PP_FEC_LOW_TBL_SET_TBL_DATA_PRIMARY_DESTINATION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.primary_destination = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_fec_low_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_fec_low_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_fec_low_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_fec_low_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_fec_low_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_FEC_LOW_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_fec_low_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_FEC_LOW_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_FEC_LOW_TBL_GET_PP_FEC_LOW_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_FEC_LOW_TBL_GET_PP_FEC_LOW_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_fec_low_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_fec_low_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_fec_low_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_fec_low_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_FEC_LOW_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_fec_high_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_fec_high_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_FEC_HIGH_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_fec_high_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_FEC_HIGH_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_FEC_HIGH_TBL_SET_PP_FEC_HIGH_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_FEC_HIGH_TBL_SET_PP_FEC_HIGH_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_fec_high_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_fec_high_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_fec_high_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_fec_high_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_FEC_HIGH_TBL_SET_PP_FEC_HIGH_TBL_SET_TBL_DATA_PROTECTION_TYPE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_FEC_HIGH_TBL_SET_PP_FEC_HIGH_TBL_SET_TBL_DATA_PROTECTION_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.protection_type = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_FEC_HIGH_TBL_SET_PP_FEC_HIGH_TBL_SET_TBL_DATA_PROTECTION_INSTANCE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_FEC_HIGH_TBL_SET_PP_FEC_HIGH_TBL_SET_TBL_DATA_PROTECTION_INSTANCE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.protection_instance = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_FEC_HIGH_TBL_SET_PP_FEC_HIGH_TBL_SET_TBL_DATA_SECONDARY_OUT_RIF_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_FEC_HIGH_TBL_SET_PP_FEC_HIGH_TBL_SET_TBL_DATA_SECONDARY_OUT_RIF_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.secondary_out_rif = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_FEC_HIGH_TBL_SET_PP_FEC_HIGH_TBL_SET_TBL_DATA_PRIMARY_OUT_RIF_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_FEC_HIGH_TBL_SET_PP_FEC_HIGH_TBL_SET_TBL_DATA_PRIMARY_OUT_RIF_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.primary_out_rif = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_fec_high_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_fec_high_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_fec_high_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_fec_high_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_fec_high_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_FEC_HIGH_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_fec_high_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_FEC_HIGH_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_FEC_HIGH_TBL_GET_PP_FEC_HIGH_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_FEC_HIGH_TBL_GET_PP_FEC_HIGH_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_fec_high_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_fec_high_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_fec_high_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_fec_high_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_FEC_HIGH_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_egress_mpls_ac_table_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_egress_mpls_ac_table_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_DATA
    prm_tbl_data;
  T20E_PP_EGRESS_MPLS_AC_TABLE_ACC_TYPE
    access_type;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_egress_mpls_ac_table_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_egress_mpls_ac_table_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_egress_mpls_ac_table_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data,
          &access_type
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_mpls_ac_table_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_mpls_ac_table_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_TBL_DATA_COUNTER_PTR_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_TBL_DATA_COUNTER_PTR_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.counter_ptr = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_TBL_DATA_COUNTER_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_TBL_DATA_COUNTER_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.counter_valid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_TBL_DATA_NEXT_EEP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_TBL_DATA_NEXT_EEP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.next_eep = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_TBL_DATA_HAS_CW_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_TBL_DATA_HAS_CW_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.has_cw = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_TBL_DATA_LABEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_TBL_DATA_LABEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.label = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_TBL_DATA_MPLS_OUT_VSI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_TBL_DATA_MPLS_OUT_VSI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.mpls_out_vsi = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_TBL_DATA_OUT_AC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_TBL_DATA_OUT_AC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.out_ac = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_TBL_DATA_OUT_VSI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_TBL_DATA_OUT_VSI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.out_vsi = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_TBL_DATA_MPLS_COMMAND_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_TBL_DATA_MPLS_COMMAND_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.mpls_command = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_TBL_DATA_AC_MPLS_MVR_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_TBL_DATA_AC_MPLS_MVR_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.ac_mpls_mvr = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_egress_mpls_ac_table_tbl_set(
          unit,
          access_type,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_mpls_ac_table_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_mpls_ac_table_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_egress_mpls_ac_table_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_egress_mpls_ac_table_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_DATA
    prm_tbl_data;
  T20E_PP_EGRESS_MPLS_AC_TABLE_ACC_TYPE
    access_type;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_egress_mpls_ac_table_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_GET_PP_EGRESS_MPLS_AC_TABLE_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_GET_PP_EGRESS_MPLS_AC_TABLE_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_egress_mpls_ac_table_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_egress_mpls_ac_table_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data,
          &access_type
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_mpls_ac_table_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_mpls_ac_table_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  if (prm_entry_offset_ndx >= (1<<18))
  {
    T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_EEI_DATA_print(&prm_tbl_data);
  }
  else
  {
    T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_DATA_print(&prm_tbl_data);

  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_egress_ctag_translation_table_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_egress_ctag_translation_table_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_edit_profile_ndx;
  uint32
    prm_outport_ndx;
  uint32
    prm_cvid_ndx;
  uint8
    prm_is_cep_ndx=FALSE;
  T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_egress_ctag_translation_table_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_EDIT_PROFILE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_EDIT_PROFILE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_edit_profile_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter edit_profile_ndx after pp_egress_ctag_translation_table_tbl_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_OUTPORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_OUTPORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_outport_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter outport_ndx after pp_egress_ctag_translation_table_tbl_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_IS_CEP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_IS_CEP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_is_cep_ndx = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_CVID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_CVID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_cvid_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter cvid_ndx after pp_egress_ctag_translation_table_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_egress_ctag_translation_table_tbl_get(
          unit,
          prm_is_cep_ndx,
          T20E_TBL_ACC_CEP_KEY_TO_C_TRANS_ENTRY_INDX(prm_cvid_ndx,prm_outport_ndx,prm_edit_profile_ndx,0),
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_ctag_translation_table_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_ctag_translation_table_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_TBL_DATA_COUNTER_PTR_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_TBL_DATA_COUNTER_PTR_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.counter_ptr = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_TBL_DATA_COUNTER_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_TBL_DATA_COUNTER_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.counter_valid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_TBL_DATA_EDIT_UP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_TBL_DATA_EDIT_UP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.edit_up = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_TBL_DATA_EDIT_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_TBL_DATA_EDIT_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.edit_vid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_TBL_DATA_OUT_TAG_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_TBL_DATA_OUT_TAG_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.out_tag_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_TBL_DATA_MP_MAX_LEVEL_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_TBL_DATA_MP_MAX_LEVEL_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.mp_max_level_valid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_TBL_DATA_MP_MAX_LEVEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_TBL_DATA_MP_MAX_LEVEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.mp_max_level = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_TBL_DATA_MP_INDEX_BASE_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_TBL_DATA_MP_INDEX_BASE_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.mp_index_base_valid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_TBL_DATA_MP_INDEX_BASE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_TBL_DATA_MP_INDEX_BASE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.mp_index_base = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_TBL_DATA_STP_TOPOLOGY_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_TBL_DATA_STP_TOPOLOGY_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.stp_topology_id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_TBL_DATA_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_TBL_DATA_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.valid = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_egress_ctag_translation_table_tbl_set(
          unit,
          prm_is_cep_ndx,
          T20E_TBL_ACC_CEP_KEY_TO_C_TRANS_ENTRY_INDX(prm_cvid_ndx,prm_outport_ndx,prm_edit_profile_ndx,0),
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_ctag_translation_table_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_ctag_translation_table_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_egress_ctag_translation_table_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_egress_ctag_translation_table_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_edit_profile_ndx;
  uint32
    prm_outport_ndx;
  uint32
    prm_cvid_ndx;
  uint8
    prm_is_cep_ndx=FALSE;
  T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_egress_ctag_translation_table_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_GET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_GET_EDIT_PROFILE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_GET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_GET_EDIT_PROFILE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_edit_profile_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter edit_profile_ndx after pp_egress_ctag_translation_table_tbl_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_GET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_GET_OUTPORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_GET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_GET_OUTPORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_outport_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter outport_ndx after pp_egress_ctag_translation_table_tbl_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_GET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_GET_IS_CEP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_GET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_GET_IS_CEP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_is_cep_ndx = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_GET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_GET_CVID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_GET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_GET_CVID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_cvid_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter cvid_ndx after pp_egress_ctag_translation_table_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_egress_ctag_translation_table_tbl_get(
          unit,
          prm_is_cep_ndx,
          T20E_TBL_ACC_CEP_KEY_TO_C_TRANS_ENTRY_INDX(prm_cvid_ndx,prm_outport_ndx,prm_edit_profile_ndx,0),
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_ctag_translation_table_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_ctag_translation_table_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_egress_ctag_translation_raw_table_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_egress_ctag_translation_raw_table_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  uint8
    prm_is_cep_ndx=FALSE;
  T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_egress_ctag_translation_raw_table_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_egress_ctag_translation_raw_table_tbl_set***", TRUE);
    goto exit;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_ENTRY_IS_CEP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_ENTRY_IS_CEP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_is_cep_ndx = (uint8)param_val->value.ulong_value;
  }



  /* This is a set function, so call GET function first */
  ret = t20e_pp_egress_ctag_translation_table_tbl_get(
          unit,
          prm_is_cep_ndx,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_ctag_translation_raw_table_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_ctag_translation_raw_table_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_TBL_DATA_COUNTER_PTR_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_TBL_DATA_COUNTER_PTR_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.counter_ptr = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_TBL_DATA_COUNTER_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_TBL_DATA_COUNTER_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.counter_valid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_TBL_DATA_EDIT_UP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_TBL_DATA_EDIT_UP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.edit_up = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_TBL_DATA_EDIT_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_TBL_DATA_EDIT_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.edit_vid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_TBL_DATA_OUT_TAG_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_TBL_DATA_OUT_TAG_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.out_tag_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_TBL_DATA_MP_MAX_LEVEL_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_TBL_DATA_MP_MAX_LEVEL_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.mp_max_level_valid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_TBL_DATA_MP_MAX_LEVEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_TBL_DATA_MP_MAX_LEVEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.mp_max_level = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_TBL_DATA_MP_INDEX_BASE_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_TBL_DATA_MP_INDEX_BASE_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.mp_index_base_valid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_TBL_DATA_MP_INDEX_BASE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_TBL_DATA_MP_INDEX_BASE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.mp_index_base = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_TBL_DATA_STP_TOPOLOGY_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_TBL_DATA_STP_TOPOLOGY_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.stp_topology_id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_TBL_DATA_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_TBL_DATA_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.valid = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_egress_ctag_translation_table_tbl_set(
          unit,
          prm_is_cep_ndx,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_ctag_translation_raw_table_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_ctag_translation_raw_table_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_egress_ctag_translation_raw_table_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_egress_ctag_translation_raw_table_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  uint8
    prm_is_cep_ndx=FALSE;
  T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_egress_ctag_translation_raw_table_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_GET_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_GET_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_egress_ctag_translation_raw_table_tbl_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_GET_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_GET_ENTRY_IS_CEP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_GET_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_GET_ENTRY_IS_CEP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_is_cep_ndx = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_egress_ctag_translation_table_tbl_get(
          unit,
          prm_is_cep_ndx,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_ctag_translation_raw_table_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_ctag_translation_raw_table_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_egress_vlan_edit_and_membership_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_egress_vlan_edit_and_membership_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_vsi_ndx;
  uint32
    prm_port_ndx;
  uint8
    prm_is_cep_ndx=FALSE;
  uint32
    prm_acc_mode_ndx;
  T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_egress_vlan_edit_and_membership_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_VSI_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_VSI_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsi_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vsi_ndx after pp_egress_vlan_edit_and_membership_tbl_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter port_ndx after pp_egress_vlan_edit_and_membership_tbl_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_IS_CEP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_IS_CEP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_is_cep_ndx = (uint8)param_val->value.ulong_value;
  }


  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_ACC_MODE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_ACC_MODE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_acc_mode_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter acc_mode_ndx after pp_egress_vlan_edit_and_membership_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_egress_vlan_edit_and_membership_tbl_get(
          unit,
          prm_is_cep_ndx,
          T20E_TBL_ACC_VSI_TO_EVMT_ENTRY_INDX(prm_vsi_ndx,prm_port_ndx,prm_acc_mode_ndx),
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_vlan_edit_and_membership_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_vlan_edit_and_membership_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_TBL_DATA_OUTER_EDIT_TAG_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_TBL_DATA_OUTER_EDIT_TAG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.outer_edit_vid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_TBL_DATA_STP_TOPOLOGY_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_TBL_DATA_STP_TOPOLOGY_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.stp_topology_id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_TBL_DATA_INNER_EDIT_TAG_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_TBL_DATA_INNER_EDIT_TAG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.inner_edit_vid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_TBL_DATA_PEP_EDIT_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_TBL_DATA_PEP_EDIT_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.pep_edit_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_TBL_DATA_PEP_COS_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_TBL_DATA_PEP_COS_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.pep_cos_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_TBL_DATA_PEP_ACC_FRM_TYPES_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_TBL_DATA_PEP_ACC_FRM_TYPES_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.pep_acc_frm_types = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_TBL_DATA_PEP_PCP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_TBL_DATA_PEP_PCP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.pep_pcp = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_TBL_DATA_PEP_PVID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_TBL_DATA_PEP_PVID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.pep_pvid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_TBL_DATA_PCP_DEI_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_TBL_DATA_PCP_DEI_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.pcp_dei_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_TBL_DATA_OUT_TAG_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_TBL_DATA_OUT_TAG_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.out_tag_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_TBL_DATA_COUNTER_PTR_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_TBL_DATA_COUNTER_PTR_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.counter_ptr = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_TBL_DATA_COUNTER_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_TBL_DATA_COUNTER_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.counter_valid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_TBL_DATA_MP_MAX_LEVEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_TBL_DATA_MP_MAX_LEVEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.mp_max_level = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_TBL_DATA_MP_MAX_LEVEL_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_TBL_DATA_MP_MAX_LEVEL_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.mp_max_level_valid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_TBL_DATA_MP_INDEX_BASE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_TBL_DATA_MP_INDEX_BASE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.mp_index_base = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_TBL_DATA_MP_INDEX_BASE_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_TBL_DATA_MP_INDEX_BASE_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.mp_index_base_valid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_TBL_DATA_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_TBL_DATA_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.valid = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_egress_vlan_edit_and_membership_tbl_set(
          unit,
          prm_is_cep_ndx,
          T20E_TBL_ACC_VSI_TO_EVMT_ENTRY_INDX(prm_vsi_ndx,prm_port_ndx,prm_acc_mode_ndx),
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_vlan_edit_and_membership_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_vlan_edit_and_membership_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_egress_vlan_edit_and_membership_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_egress_vlan_edit_and_membership_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_vsi_ndx;
  uint32
    prm_port_ndx;
  uint32
    prm_acc_mode_ndx;
  uint8
    prm_is_cep_ndx=FALSE;
  T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_egress_vlan_edit_and_membership_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_GET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_GET_VSI_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_GET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_GET_VSI_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsi_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vsi_ndx after pp_egress_vlan_edit_and_membership_tbl_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_GET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_GET_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_GET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_GET_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter port_ndx after pp_egress_vlan_edit_and_membership_tbl_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_GET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_GET_IS_CEP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_GET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_GET_IS_CEP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_is_cep_ndx = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_GET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_GET_ACC_MODE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_GET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_GET_ACC_MODE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_acc_mode_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter acc_mode_ndx after pp_egress_vlan_edit_and_membership_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_egress_vlan_edit_and_membership_tbl_get(
          unit,
          prm_is_cep_ndx,
          T20E_TBL_ACC_VSI_TO_EVMT_ENTRY_INDX(prm_vsi_ndx,prm_port_ndx,prm_acc_mode_ndx),
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_vlan_edit_and_membership_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_vlan_edit_and_membership_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_egress_vlan_edit_and_membership_raw_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_egress_vlan_edit_and_membership_raw_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_ac_ndx;
  uint8
    prm_is_cep_ndx=FALSE;
  T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_egress_vlan_edit_and_membership_raw_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_AC_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_AC_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ac_ndx after pp_egress_vlan_edit_and_membership_raw_tbl_set***", TRUE);
    goto exit;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_IS_CEP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_IS_CEP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_is_cep_ndx = (uint8)param_val->value.ulong_value;
  }

  /* This is a set function, so call GET function first */
  ret = t20e_pp_egress_vlan_edit_and_membership_tbl_get(
          unit,
          prm_is_cep_ndx,
          prm_ac_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_vlan_edit_and_membership_raw_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_vlan_edit_and_membership_raw_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_TBL_DATA_OUTER_EDIT_TAG_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_TBL_DATA_OUTER_EDIT_TAG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.outer_edit_vid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_TBL_DATA_STP_TOPOLOGY_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_TBL_DATA_STP_TOPOLOGY_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.stp_topology_id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_TBL_DATA_INNER_EDIT_TAG_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_TBL_DATA_INNER_EDIT_TAG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.inner_edit_vid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_TBL_DATA_PEP_EDIT_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_TBL_DATA_PEP_EDIT_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.pep_edit_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_TBL_DATA_PEP_COS_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_TBL_DATA_PEP_COS_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.pep_cos_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_TBL_DATA_PEP_ACC_FRM_TYPES_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_TBL_DATA_PEP_ACC_FRM_TYPES_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.pep_acc_frm_types = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_TBL_DATA_PEP_PCP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_TBL_DATA_PEP_PCP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.pep_pcp = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_TBL_DATA_PEP_PVID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_TBL_DATA_PEP_PVID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.pep_pvid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_TBL_DATA_PCP_DEI_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_TBL_DATA_PCP_DEI_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.pcp_dei_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_TBL_DATA_OUT_TAG_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_TBL_DATA_OUT_TAG_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.out_tag_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_TBL_DATA_COUNTER_PTR_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_TBL_DATA_COUNTER_PTR_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.counter_ptr = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_TBL_DATA_COUNTER_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_TBL_DATA_COUNTER_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.counter_valid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_TBL_DATA_MP_MAX_LEVEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_TBL_DATA_MP_MAX_LEVEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.mp_max_level = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_TBL_DATA_MP_MAX_LEVEL_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_TBL_DATA_MP_MAX_LEVEL_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.mp_max_level_valid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_TBL_DATA_MP_INDEX_BASE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_TBL_DATA_MP_INDEX_BASE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.mp_index_base = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_TBL_DATA_MP_INDEX_BASE_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_TBL_DATA_MP_INDEX_BASE_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.mp_index_base_valid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_TBL_DATA_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_TBL_DATA_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.valid = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_egress_vlan_edit_and_membership_tbl_set(
          unit,
          prm_is_cep_ndx,
          prm_ac_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_vlan_edit_and_membership_raw_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_vlan_edit_and_membership_raw_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_egress_vlan_edit_and_membership_raw_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_egress_vlan_edit_and_membership_raw_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_ac_ndx;
  uint8
    prm_is_cep_ndx=FALSE;
  T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_egress_vlan_edit_and_membership_raw_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_GET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_GET_AC_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_GET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_GET_AC_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ac_ndx after pp_egress_vlan_edit_and_membership_raw_tbl_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_GET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_GET_IS_CEP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_GET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_GET_IS_CEP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_is_cep_ndx = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_egress_vlan_edit_and_membership_tbl_get(
          unit,
          prm_is_cep_ndx,
          prm_ac_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_vlan_edit_and_membership_raw_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_vlan_edit_and_membership_raw_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_ac_low_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_ac_low_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_AC_LOW_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_ac_low_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_AC_LOW_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AC_LOW_TBL_SET_PP_AC_LOW_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AC_LOW_TBL_SET_PP_AC_LOW_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_ac_low_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_ac_low_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_ac_low_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_ac_low_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AC_LOW_TBL_SET_PP_AC_LOW_TBL_SET_TBL_DATA_VLAN_EDIT_TAG_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AC_LOW_TBL_SET_PP_AC_LOW_TBL_SET_TBL_DATA_VLAN_EDIT_TAG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.vlan_edit_vid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AC_LOW_TBL_SET_PP_AC_LOW_TBL_SET_TBL_DATA_VLAN_EDIT_TAG_PCP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AC_LOW_TBL_SET_PP_AC_LOW_TBL_SET_TBL_DATA_VLAN_EDIT_TAG_PCP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.vlan_edit_pcp_dei_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AC_LOW_TBL_SET_PP_AC_LOW_TBL_SET_TBL_DATA_LEARN_DESTINATION_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AC_LOW_TBL_SET_PP_AC_LOW_TBL_SET_TBL_DATA_LEARN_DESTINATION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.learn_destination = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AC_LOW_TBL_SET_PP_AC_LOW_TBL_SET_TBL_DATA_UNKNOWN_FRWD_DEST_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AC_LOW_TBL_SET_PP_AC_LOW_TBL_SET_TBL_DATA_UNKNOWN_FRWD_DEST_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.unknown_frwd_dst = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AC_LOW_TBL_SET_PP_AC_LOW_TBL_SET_TBL_DATA_ASD_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AC_LOW_TBL_SET_PP_AC_LOW_TBL_SET_TBL_DATA_ASD_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.unknown_frwd_asd = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_ac_low_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_ac_low_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_ac_low_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_ac_low_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_ac_low_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_AC_LOW_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_ac_low_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_AC_LOW_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AC_LOW_TBL_GET_PP_AC_LOW_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AC_LOW_TBL_GET_PP_AC_LOW_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_ac_low_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_ac_low_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_ac_low_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_ac_low_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_AC_LOW_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_ac_high_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_ac_high_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_AC_HIGH_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_ac_high_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_AC_HIGH_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AC_HIGH_TBL_SET_PP_AC_HIGH_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AC_HIGH_TBL_SET_PP_AC_HIGH_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_ac_high_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_ac_high_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_ac_high_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_ac_high_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AC_HIGH_TBL_SET_PP_AC_HIGH_TBL_SET_TBL_DATA_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AC_HIGH_TBL_SET_PP_AC_HIGH_TBL_SET_TBL_DATA_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.valid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AC_HIGH_TBL_SET_PP_AC_HIGH_TBL_SET_TBL_DATA_COUNTER_OR_STAT_PTR_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AC_HIGH_TBL_SET_PP_AC_HIGH_TBL_SET_TBL_DATA_COUNTER_OR_STAT_PTR_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.counter_or_stat_ptr = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AC_HIGH_TBL_SET_PP_AC_HIGH_TBL_SET_TBL_DATA_POLICER_PTR_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AC_HIGH_TBL_SET_PP_AC_HIGH_TBL_SET_TBL_DATA_POLICER_PTR_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.policer_ptr = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AC_HIGH_TBL_SET_PP_AC_HIGH_TBL_SET_TBL_DATA_STATISTICS_TAG_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AC_HIGH_TBL_SET_PP_AC_HIGH_TBL_SET_TBL_DATA_STATISTICS_TAG_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.statistics_tag_enable = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AC_HIGH_TBL_SET_PP_AC_HIGH_TBL_SET_TBL_DATA_COUNTERS_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AC_HIGH_TBL_SET_PP_AC_HIGH_TBL_SET_TBL_DATA_COUNTERS_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.counters_enable = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AC_HIGH_TBL_SET_PP_AC_HIGH_TBL_SET_TBL_DATA_POLICER_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AC_HIGH_TBL_SET_PP_AC_HIGH_TBL_SET_TBL_DATA_POLICER_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.policer_enable = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AC_HIGH_TBL_SET_PP_AC_HIGH_TBL_SET_TBL_DATA_VSI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AC_HIGH_TBL_SET_PP_AC_HIGH_TBL_SET_TBL_DATA_VSI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.vsi = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AC_HIGH_TBL_SET_PP_AC_HIGH_TBL_SET_TBL_DATA_COS_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AC_HIGH_TBL_SET_PP_AC_HIGH_TBL_SET_TBL_DATA_COS_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.cos_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AC_HIGH_TBL_SET_PP_AC_HIGH_TBL_SET_TBL_DATA_MEF_L2_CPSF_TUNNELING_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AC_HIGH_TBL_SET_PP_AC_HIGH_TBL_SET_TBL_DATA_MEF_L2_CPSF_TUNNELING_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.mef_l2_cpsf_tunneling = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AC_HIGH_TBL_SET_PP_AC_HIGH_TBL_SET_TBL_DATA_LINK_LAYER_TERMINATION_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AC_HIGH_TBL_SET_PP_AC_HIGH_TBL_SET_TBL_DATA_LINK_LAYER_TERMINATION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.link_layer_termination = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AC_HIGH_TBL_SET_PP_AC_HIGH_TBL_SET_TBL_DATA_LEARN_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AC_HIGH_TBL_SET_PP_AC_HIGH_TBL_SET_TBL_DATA_LEARN_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.learn_enable = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AC_HIGH_TBL_SET_PP_AC_HIGH_TBL_SET_TBL_DATA_VLAN_EDIT_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AC_HIGH_TBL_SET_PP_AC_HIGH_TBL_SET_TBL_DATA_VLAN_EDIT_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.vlan_edit_profile = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_ac_high_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_ac_high_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_ac_high_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_ac_high_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_ac_high_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_AC_HIGH_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_ac_high_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_AC_HIGH_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AC_HIGH_TBL_GET_PP_AC_HIGH_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AC_HIGH_TBL_GET_PP_AC_HIGH_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_ac_high_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_ac_high_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_ac_high_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_ac_high_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_AC_HIGH_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_ac_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_ac_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_AC_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_ac_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_AC_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_ac_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_ac_tbl_get_unsafe(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_ac_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_ac_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_TBL_DATA_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_TBL_DATA_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.valid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_TBL_DATA_COUNTER_OR_STAT_PTR_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_TBL_DATA_COUNTER_OR_STAT_PTR_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.counter_or_stat_ptr = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_TBL_DATA_POLICER_PTR_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_TBL_DATA_POLICER_PTR_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.policer_ptr = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_TBL_DATA_STATISTICS_TAG_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_TBL_DATA_STATISTICS_TAG_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.statistics_tag_enable = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_TBL_DATA_COUNTERS_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_TBL_DATA_COUNTERS_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.counters_enable = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_TBL_DATA_POLICER_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_TBL_DATA_POLICER_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.policer_enable = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_TBL_DATA_VSI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_TBL_DATA_VSI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.vsi = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_TBL_DATA_COS_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_TBL_DATA_COS_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.cos_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_TBL_DATA_LINK_LAYER_TERMINATION_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_TBL_DATA_LINK_LAYER_TERMINATION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.link_layer_termination = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_TBL_DATA_MEF_L2_CPSF_TUNNELING_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_TBL_DATA_MEF_L2_CPSF_TUNNELING_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.mef_l2_cpsf_tunneling = (uint32)param_val->value.ulong_value;
  }
    

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_TBL_DATA_LEARN_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_TBL_DATA_LEARN_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.learn_enable = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_TBL_DATA_VLAN_EDIT_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_TBL_DATA_VLAN_EDIT_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.vlan_edit_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_TBL_DATA_VLAN_EDIT_TAG_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_TBL_DATA_VLAN_EDIT_TAG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.vlan_edit_vid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_TBL_DATA_VLAN_EDIT_TAG_PCP_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_TBL_DATA_VLAN_EDIT_TAG_PCP_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.vlan_edit_pcp_dei_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_TBL_DATA_LEARN_DESTINATION_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_TBL_DATA_LEARN_DESTINATION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.learn_destination = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_TBL_DATA_UNKNOWN_FRWD_DEST_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_TBL_DATA_UNKNOWN_FRWD_DEST_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.unknown_frwd_dst = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_TBL_DATA_EEI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_TBL_DATA_EEI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.unknown_frwd_asd = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_ac_tbl_set_unsafe(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_ac_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_ac_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_ac_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_ac_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_AC_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_ac_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_AC_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AC_TBL_GET_PP_AC_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AC_TBL_GET_PP_AC_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_ac_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_ac_tbl_get_unsafe(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_ac_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_ac_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_AC_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_pwe_high_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_pwe_high_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_PWE_HIGH_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_pwe_high_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_PWE_HIGH_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PWE_HIGH_TBL_SET_PP_PWE_HIGH_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PWE_HIGH_TBL_SET_PP_PWE_HIGH_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_pwe_high_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_pwe_high_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_pwe_high_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_pwe_high_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PWE_HIGH_TBL_SET_PP_PWE_HIGH_TBL_SET_TBL_DATA_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PWE_HIGH_TBL_SET_PP_PWE_HIGH_TBL_SET_TBL_DATA_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.valid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PWE_HIGH_TBL_SET_PP_PWE_HIGH_TBL_SET_TBL_DATA_NEXT_PROTOCOL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PWE_HIGH_TBL_SET_PP_PWE_HIGH_TBL_SET_TBL_DATA_NEXT_PROTOCOL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.next_protocol = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PWE_HIGH_TBL_SET_PP_PWE_HIGH_TBL_SET_TBL_DATA_COUNTER_PTR_STAT_TAG_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PWE_HIGH_TBL_SET_PP_PWE_HIGH_TBL_SET_TBL_DATA_COUNTER_PTR_STAT_TAG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.counter_ptr_stat_tag = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PWE_HIGH_TBL_SET_PP_PWE_HIGH_TBL_SET_TBL_DATA_POINT_TO_POINT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PWE_HIGH_TBL_SET_PP_PWE_HIGH_TBL_SET_TBL_DATA_POINT_TO_POINT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.point_to_point = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PWE_HIGH_TBL_SET_PP_PWE_HIGH_TBL_SET_TBL_DATA_POLICER_PTR_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PWE_HIGH_TBL_SET_PP_PWE_HIGH_TBL_SET_TBL_DATA_POLICER_PTR_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.policer_ptr = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PWE_HIGH_TBL_SET_PP_PWE_HIGH_TBL_SET_TBL_DATA_STAT_TAG_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PWE_HIGH_TBL_SET_PP_PWE_HIGH_TBL_SET_TBL_DATA_STAT_TAG_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.stat_tag_enable = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PWE_HIGH_TBL_SET_PP_PWE_HIGH_TBL_SET_TBL_DATA_COUNTER_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PWE_HIGH_TBL_SET_PP_PWE_HIGH_TBL_SET_TBL_DATA_COUNTER_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.counter_enable = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PWE_HIGH_TBL_SET_PP_PWE_HIGH_TBL_SET_TBL_DATA_POLICER_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PWE_HIGH_TBL_SET_PP_PWE_HIGH_TBL_SET_TBL_DATA_POLICER_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.policer_enable = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PWE_HIGH_TBL_SET_PP_PWE_HIGH_TBL_SET_TBL_DATA_ORIENTATION_IS_HUB_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PWE_HIGH_TBL_SET_PP_PWE_HIGH_TBL_SET_TBL_DATA_ORIENTATION_IS_HUB_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.orientation_is_hub = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PWE_HIGH_TBL_SET_PP_PWE_HIGH_TBL_SET_TBL_DATA_TPID_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PWE_HIGH_TBL_SET_PP_PWE_HIGH_TBL_SET_TBL_DATA_TPID_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.tpid_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PWE_HIGH_TBL_SET_PP_PWE_HIGH_TBL_SET_TBL_DATA_COS_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PWE_HIGH_TBL_SET_PP_PWE_HIGH_TBL_SET_TBL_DATA_COS_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.cos_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PWE_HIGH_TBL_SET_PP_PWE_HIGH_TBL_SET_TBL_DATA_LEARN_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PWE_HIGH_TBL_SET_PP_PWE_HIGH_TBL_SET_TBL_DATA_LEARN_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.learn_enable = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PWE_HIGH_TBL_SET_PP_PWE_HIGH_TBL_SET_TBL_DATA_ASD_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PWE_HIGH_TBL_SET_PP_PWE_HIGH_TBL_SET_TBL_DATA_ASD_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.learn_asd = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_pwe_high_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_pwe_high_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_pwe_high_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_pwe_high_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_pwe_high_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_PWE_HIGH_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_pwe_high_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_PWE_HIGH_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PWE_HIGH_TBL_GET_PP_PWE_HIGH_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PWE_HIGH_TBL_GET_PP_PWE_HIGH_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_pwe_high_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_pwe_high_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_pwe_high_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_pwe_high_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_PWE_HIGH_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_pwe_low_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_pwe_low_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_PWE_LOW_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_pwe_low_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_PWE_LOW_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PWE_LOW_TBL_SET_PP_PWE_LOW_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PWE_LOW_TBL_SET_PP_PWE_LOW_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_pwe_low_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_pwe_low_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_pwe_low_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_pwe_low_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PWE_LOW_TBL_SET_PP_PWE_LOW_TBL_SET_TBL_DATA_VSI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PWE_LOW_TBL_SET_PP_PWE_LOW_TBL_SET_TBL_DATA_VSI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.vsi = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PWE_LOW_TBL_SET_PP_PWE_LOW_TBL_SET_TBL_DATA_REVERSE_FEC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PWE_LOW_TBL_SET_PP_PWE_LOW_TBL_SET_TBL_DATA_REVERSE_FEC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.reverse_fec = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PWE_LOW_TBL_SET_PP_PWE_LOW_TBL_SET_TBL_DATA_UNKNOWN_FRWD_DEST_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PWE_LOW_TBL_SET_PP_PWE_LOW_TBL_SET_TBL_DATA_UNKNOWN_FRWD_DEST_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.unknown_frwd_dst = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PWE_LOW_TBL_SET_PP_PWE_LOW_TBL_SET_TBL_DATA_EEI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PWE_LOW_TBL_SET_PP_PWE_LOW_TBL_SET_TBL_DATA_EEI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.unknown_frwd_asd = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_pwe_low_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_pwe_low_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_pwe_low_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_pwe_low_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_pwe_low_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_PWE_LOW_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_pwe_low_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_PWE_LOW_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PWE_LOW_TBL_GET_PP_PWE_LOW_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PWE_LOW_TBL_GET_PP_PWE_LOW_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_pwe_low_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_pwe_low_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_pwe_low_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_pwe_low_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_PWE_LOW_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_pwe_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_pwe_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_PWE_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_pwe_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_PWE_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_pwe_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_pwe_tbl_get_unsafe(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_pwe_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_pwe_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_TBL_DATA_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_TBL_DATA_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.valid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_TBL_DATA_COUNTER_PTR_STAT_TAG_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_TBL_DATA_COUNTER_PTR_STAT_TAG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.counter_ptr_stat_tag = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_TBL_DATA_POLICER_PTR_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_TBL_DATA_POLICER_PTR_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.policer_ptr = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_TBL_DATA_STAT_TAG_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_TBL_DATA_STAT_TAG_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.stat_tag_enable = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_TBL_DATA_COUNTER_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_TBL_DATA_COUNTER_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.counter_enable = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_TBL_DATA_POLICER_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_TBL_DATA_POLICER_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.policer_enable = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_TBL_DATA_ORIENTATION_IS_HUB_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_TBL_DATA_ORIENTATION_IS_HUB_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.orientation_is_hub = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_TBL_DATA_REVERSE_VC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_TBL_DATA_REVERSE_VC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.reverse_vc = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_TBL_DATA_REVERSE_FEC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_TBL_DATA_REVERSE_FEC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.reverse_fec = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_TBL_DATA_TPID_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_TBL_DATA_TPID_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.tpid_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_TBL_DATA_LEARN_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_TBL_DATA_LEARN_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.learn_enable = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_TBL_DATA_COS_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_TBL_DATA_COS_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.cos_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_TBL_DATA_UNKNOWN_FRWD_DEST_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_TBL_DATA_UNKNOWN_FRWD_DEST_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.unknown_frwd_dst = (uint32)param_val->value.ulong_value;
  }

    if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_TBL_DATA_UNKNOWN_FRWD_ASD_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_TBL_DATA_UNKNOWN_FRWD_ASD_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.unknown_frwd_asd = (uint32)param_val->value.ulong_value;
  }


  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_TBL_DATA_VSI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_TBL_DATA_VSI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.vsi = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_TBL_DATA_NEXT_PROTOCOL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_TBL_DATA_NEXT_PROTOCOL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.next_protocol = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_pwe_tbl_set_unsafe(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_pwe_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_pwe_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_pwe_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_pwe_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_PWE_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_pwe_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_PWE_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PWE_TBL_GET_PP_PWE_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PWE_TBL_GET_PP_PWE_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_pwe_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_pwe_tbl_get_unsafe(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_pwe_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_pwe_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_PWE_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_aging_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_aging_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_AGING_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_aging_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_AGING_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_aging_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_aging_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_aging_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_aging_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_TBL_DATA_AGE_STATUS_15_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_TBL_DATA_AGE_STATUS_15_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.age_status_15 = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_TBL_DATA_AGE_STATUS_14_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_TBL_DATA_AGE_STATUS_14_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.age_status_14 = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_TBL_DATA_AGE_STATUS_13_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_TBL_DATA_AGE_STATUS_13_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.age_status_13 = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_TBL_DATA_AGE_STATUS_12_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_TBL_DATA_AGE_STATUS_12_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.age_status_12 = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_TBL_DATA_AGE_STATUS_11_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_TBL_DATA_AGE_STATUS_11_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.age_status_11 = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_TBL_DATA_AGE_STATUS_10_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_TBL_DATA_AGE_STATUS_10_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.age_status_10 = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_TBL_DATA_AGE_STATUS_9_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_TBL_DATA_AGE_STATUS_9_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.age_status_9 = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_TBL_DATA_AGE_STATUS_8_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_TBL_DATA_AGE_STATUS_8_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.age_status_8 = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_TBL_DATA_AGE_STATUS_7_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_TBL_DATA_AGE_STATUS_7_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.age_status_7 = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_TBL_DATA_AGE_STATUS_6_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_TBL_DATA_AGE_STATUS_6_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.age_status_6 = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_TBL_DATA_AGE_STATUS_5_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_TBL_DATA_AGE_STATUS_5_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.age_status_5 = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_TBL_DATA_AGE_STATUS_4_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_TBL_DATA_AGE_STATUS_4_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.age_status_4 = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_TBL_DATA_AGE_STATUS_3_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_TBL_DATA_AGE_STATUS_3_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.age_status_3 = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_TBL_DATA_AGE_STATUS_2_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_TBL_DATA_AGE_STATUS_2_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.age_status_2 = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_TBL_DATA_AGE_STATUS_1_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_TBL_DATA_AGE_STATUS_1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.age_status_1 = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_TBL_DATA_AGE_STATUS_0_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_TBL_DATA_AGE_STATUS_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.age_status_0 = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_aging_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_aging_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_aging_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_aging_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_aging_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_AGING_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_aging_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_AGING_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AGING_TBL_GET_PP_AGING_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_AGING_TBL_GET_PP_AGING_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_aging_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_aging_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_aging_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_aging_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_AGING_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_cnt_inp_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_cnt_inp_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  uint32
    prm_octets_index = 0xFFFFFFFF;
  T20E_PP_CNT_INP_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_cnt_inp_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_CNT_INP_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_CNT_INP_TBL_SET_PP_CNT_INP_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_CNT_INP_TBL_SET_PP_CNT_INP_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_cnt_inp_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_cnt_inp_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_cnt_inp_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_cnt_inp_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_CNT_INP_TBL_SET_PP_CNT_INP_TBL_SET_TBL_DATA_PACKETS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_CNT_INP_TBL_SET_PP_CNT_INP_TBL_SET_TBL_DATA_PACKETS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.packets = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_CNT_INP_TBL_SET_PP_CNT_INP_TBL_SET_TBL_DATA_OCTETS_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_CNT_INP_TBL_SET_PP_CNT_INP_TBL_SET_TBL_DATA_OCTETS_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_octets_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_octets_index != 0xFFFFFFFF)
  {

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_CNT_INP_TBL_SET_PP_CNT_INP_TBL_SET_TBL_DATA_OCTETS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_CNT_INP_TBL_SET_PP_CNT_INP_TBL_SET_TBL_DATA_OCTETS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.octets[ prm_octets_index] = (uint32)param_val->value.ulong_value;
  }

  }


  /* Call function */
  ret = t20e_pp_cnt_inp_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_cnt_inp_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_cnt_inp_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_cnt_inp_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_cnt_inp_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_CNT_INP_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_cnt_inp_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_CNT_INP_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_CNT_INP_TBL_GET_PP_CNT_INP_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_CNT_INP_TBL_GET_PP_CNT_INP_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_cnt_inp_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_cnt_inp_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_cnt_inp_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_cnt_inp_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_CNT_INP_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_egress_counters_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_egress_counters_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  uint32
    prm_counter0_index = 0xFFFFFFFF;
  T20E_PP_EGRESS_COUNTERS_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_egress_counters_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_EGRESS_COUNTERS_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_COUNTERS_TBL_SET_PP_EGRESS_COUNTERS_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_COUNTERS_TBL_SET_PP_EGRESS_COUNTERS_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_egress_counters_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_egress_counters_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_counters_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_counters_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_COUNTERS_TBL_SET_PP_EGRESS_COUNTERS_TBL_SET_TBL_DATA_COUNTER0_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_COUNTERS_TBL_SET_PP_EGRESS_COUNTERS_TBL_SET_TBL_DATA_COUNTER0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.octets[0] = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_COUNTERS_TBL_SET_PP_EGRESS_COUNTERS_TBL_SET_TBL_DATA_COUNTER1_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_COUNTERS_TBL_SET_PP_EGRESS_COUNTERS_TBL_SET_TBL_DATA_COUNTER1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.octets[1] = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_COUNTERS_TBL_SET_PP_EGRESS_COUNTERS_TBL_SET_TBL_DATA_PACKETS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_COUNTERS_TBL_SET_PP_EGRESS_COUNTERS_TBL_SET_TBL_DATA_PACKETS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.packets = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_egress_counters_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_counters_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_counters_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_egress_counters_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_egress_counters_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_EGRESS_COUNTERS_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_egress_counters_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_EGRESS_COUNTERS_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_COUNTERS_TBL_GET_PP_EGRESS_COUNTERS_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_COUNTERS_TBL_GET_PP_EGRESS_COUNTERS_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_egress_counters_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_egress_counters_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_counters_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_counters_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_EGRESS_COUNTERS_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_plc_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_plc_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  uint32
    prm_info_index = 0xFFFFFFFF;
  T20E_PP_PLC_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_plc_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_PLC_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PLC_TBL_SET_PP_PLC_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PLC_TBL_SET_PP_PLC_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_plc_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_plc_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_plc_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_plc_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PLC_TBL_SET_PP_PLC_TBL_SET_TBL_DATA_INFO_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PLC_TBL_SET_PP_PLC_TBL_SET_TBL_DATA_INFO_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_info_index != 0xFFFFFFFF)
  {

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PLC_TBL_SET_PP_PLC_TBL_SET_TBL_DATA_INFO_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PLC_TBL_SET_PP_PLC_TBL_SET_TBL_DATA_INFO_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.policer_e_fct = (uint32)param_val->value.ulong_value;
  }

  }


  /* Call function */
  ret = t20e_pp_plc_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_plc_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_plc_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_plc_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_plc_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_PLC_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_plc_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_PLC_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PLC_TBL_GET_PP_PLC_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_PLC_TBL_GET_PP_PLC_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_plc_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_plc_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_plc_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_plc_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_PLC_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_egress_mpls_tunnel_tbl_set (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_egress_mpls_tunnel_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_EGRESS_MPLS_TUNNEL_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_egress_mpls_tunnel_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_EGRESS_MPLS_TUNNEL_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_MPLS_TUNNEL_TBL_SET_PP_EGRESS_MPLS_TUNNEL_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_MPLS_TUNNEL_TBL_SET_PP_EGRESS_MPLS_TUNNEL_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_egress_mpls_tunnel_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_egress_mpls_tunnel_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_mpls_tunnel_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_mpls_tunnel_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_MPLS_TUNNEL_TBL_SET_PP_EGRESS_MPLS_TUNNEL_TBL_SET_TBL_DATA_NEXT_EEP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_MPLS_TUNNEL_TBL_SET_PP_EGRESS_MPLS_TUNNEL_TBL_SET_TBL_DATA_NEXT_EEP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.next_eep = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_MPLS_TUNNEL_TBL_SET_PP_EGRESS_MPLS_TUNNEL_TBL_SET_TBL_DATA_OUT_VSI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_MPLS_TUNNEL_TBL_SET_PP_EGRESS_MPLS_TUNNEL_TBL_SET_TBL_DATA_OUT_VSI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.out_vsi = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_MPLS_TUNNEL_TBL_SET_PP_EGRESS_MPLS_TUNNEL_TBL_SET_TBL_DATA_MPLS2_LABEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_MPLS_TUNNEL_TBL_SET_PP_EGRESS_MPLS_TUNNEL_TBL_SET_TBL_DATA_MPLS2_LABEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.mpls2_label = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_MPLS_TUNNEL_TBL_SET_PP_EGRESS_MPLS_TUNNEL_TBL_SET_TBL_DATA_MPLS2_COMMAND_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_MPLS_TUNNEL_TBL_SET_PP_EGRESS_MPLS_TUNNEL_TBL_SET_TBL_DATA_MPLS2_COMMAND_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.mpls2_command = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_MPLS_TUNNEL_TBL_SET_PP_EGRESS_MPLS_TUNNEL_TBL_SET_TBL_DATA_MPLS1_LABEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_MPLS_TUNNEL_TBL_SET_PP_EGRESS_MPLS_TUNNEL_TBL_SET_TBL_DATA_MPLS1_LABEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.mpls1_label = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_MPLS_TUNNEL_TBL_SET_PP_EGRESS_MPLS_TUNNEL_TBL_SET_TBL_DATA_MPLS1_COMMAND_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_MPLS_TUNNEL_TBL_SET_PP_EGRESS_MPLS_TUNNEL_TBL_SET_TBL_DATA_MPLS1_COMMAND_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.mpls1_command = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_MPLS_TUNNEL_TBL_SET_PP_EGRESS_MPLS_TUNNEL_TBL_SET_TBL_DATA_ORIENTATION_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_MPLS_TUNNEL_TBL_SET_PP_EGRESS_MPLS_TUNNEL_TBL_SET_TBL_DATA_ORIENTATION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.orientation = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_egress_mpls_tunnel_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_mpls_tunnel_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_mpls_tunnel_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pp_egress_mpls_tunnel_tbl_get (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_pp_egress_mpls_tunnel_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_PP_EGRESS_MPLS_TUNNEL_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_pp_egress_mpls_tunnel_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_EGRESS_MPLS_TUNNEL_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_MPLS_TUNNEL_TBL_GET_PP_EGRESS_MPLS_TUNNEL_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_EGRESS_MPLS_TUNNEL_TBL_GET_PP_EGRESS_MPLS_TUNNEL_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after pp_egress_mpls_tunnel_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_egress_mpls_tunnel_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_egress_mpls_tunnel_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_egress_mpls_tunnel_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_PP_EGRESS_MPLS_TUNNEL_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}


/********************************************************************
 *  Function handler: ac_access_entry_get_unsafe (section t20e_acc_pp)
 */
int
  ui_t20e_acc_pp_ac_access_entry_get_unsafe(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  T20E_L2_LIF_AC_KEY
    prm_ac_keys;
  T20E_PP_AC_KEY_TBL_DATA
    prm_data;
  uint8
    prm_found;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");
  soc_sand_proc_name = "t20e_ac_access_entry_get_unsafe";

  unit = t20e_get_default_unit();
  T20E_L2_LIF_AC_KEY_clear(&prm_ac_keys);
  T20E_PP_AC_KEY_TBL_DATA_clear(&prm_data);

  prm_ac_keys.inner_vid = T20E_LIF_IGNORE_INTERNAL_VID;
  prm_ac_keys.outer_vid = T20E_LIF_IGNORE_OUTER_VID;


  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_AC_ACCESS_ENTRY_GET_UNSAFE_AC_ACCESS_ENTRY_GET_UNSAFE_AC_KEYS_INNER_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_AC_ACCESS_ENTRY_GET_UNSAFE_AC_ACCESS_ENTRY_GET_UNSAFE_AC_KEYS_INNER_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_keys.inner_vid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_AC_ACCESS_ENTRY_GET_UNSAFE_AC_ACCESS_ENTRY_GET_UNSAFE_AC_KEYS_OUTER_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_AC_ACCESS_ENTRY_GET_UNSAFE_AC_ACCESS_ENTRY_GET_UNSAFE_AC_KEYS_OUTER_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_keys.outer_vid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_AC_ACCESS_ENTRY_GET_UNSAFE_AC_ACCESS_ENTRY_GET_UNSAFE_AC_KEYS_VLAN_DOMAIN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_AC_ACCESS_ENTRY_GET_UNSAFE_AC_ACCESS_ENTRY_GET_UNSAFE_AC_KEYS_VLAN_DOMAIN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_keys.vlan_domain = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_ac_access_entry_get_unsafe(
          unit,
          &prm_ac_keys,
          &prm_data,
          &prm_found
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_ac_access_entry_get_unsafe - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_ac_access_entry_get_unsafe");
    goto exit;
  }

  send_string_to_screen("--> data:", TRUE);
  T20E_PP_AC_KEY_TBL_DATA_print(&prm_data);

  send_string_to_screen("--> found:", TRUE);
  soc_sand_os_printf( "found: %u\n",prm_found);


  goto exit;
exit:
  return ui_ret;
}

#endif
#ifdef UI_T20E_ACC_MACT
/********************************************************************
 *  Function handler: mact_set_reps_for_tbl_unsafe (section t20e_acc_mact)
 */
int
  ui_t20e_acc_mact_mact_set_reps_for_tbl_unsafe(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_nof_reps;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_mact");
  soc_sand_proc_name = "t20e_mact_set_reps_for_tbl_unsafe";

  unit = t20e_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MACT_SET_REPS_FOR_TBL_UNSAFE_MACT_SET_REPS_FOR_TBL_UNSAFE_NOF_REPS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_MACT_SET_REPS_FOR_TBL_UNSAFE_MACT_SET_REPS_FOR_TBL_UNSAFE_NOF_REPS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_nof_reps = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_mact_set_reps_for_tbl_unsafe(
          unit,
          prm_nof_reps
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_mact_set_reps_for_tbl_unsafe - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_mact_set_reps_for_tbl_unsafe");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: t20e_pp_lem_mac_da_tbl_set (section t20e_acc_mact)
 */
int
  ui_t20e_acc_mact_t20e_pp_lem_mac_da_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  T20E_PP_LEM_TBL_KEY
    prm_mac_key;
  T20E_PP_MAC_TBL_DATA
    prm_mac_val;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_mact");
  soc_sand_proc_name = "t20e_pp_lem_mac_da_tbl_set";

  unit = t20e_get_default_unit();
  T20E_PP_LEM_TBL_KEY_clear(&prm_mac_key);
  T20E_PP_MAC_TBL_DATA_clear(&prm_mac_val);
  prm_mac_key.type = T20E_PP_LEM_TBL_KEY_TYPE_MACT;

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LEM_MAC_DA_TBL_SET_T20E_PP_LEM_MAC_DA_TBL_SET_MAC_KEY_FID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_LEM_MAC_DA_TBL_SET_T20E_PP_LEM_MAC_DA_TBL_SET_MAC_KEY_FID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mac_key.info.mact_entry.key_val.mac.fid = (uint32)param_val->value.ulong_value;
  }
  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_T20E_PP_LEM_MAC_DA_TBL_SET_T20E_PP_LEM_MAC_DA_TBL_SET_MAC_KEY_MAC_ID,1,
         &param_val,VAL_TEXT,err_msg))
  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_mac_key.info.mact_entry.key_val.mac.mac));
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mac_key after t20e_pp_lem_mac_da_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_pp_lem_da_tbl_get_unsafe(
          unit,
          &prm_mac_key,
          &prm_mac_val
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_lem_mac_da_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_lem_mac_da_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LEM_MAC_DA_TBL_SET_T20E_PP_LEM_MAC_DA_TBL_SET_MAC_VAL_IS_DYNAMIC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_LEM_MAC_DA_TBL_SET_T20E_PP_LEM_MAC_DA_TBL_SET_MAC_VAL_IS_DYNAMIC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mac_val.is_dynamic = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LEM_MAC_DA_TBL_SET_T20E_PP_LEM_MAC_DA_TBL_SET_MAC_VAL_DESTINATION_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_LEM_MAC_DA_TBL_SET_T20E_PP_LEM_MAC_DA_TBL_SET_MAC_VAL_DESTINATION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mac_val.destination = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LEM_MAC_DA_TBL_SET_T20E_PP_LEM_MAC_DA_TBL_SET_MAC_VAL_ASD_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_LEM_MAC_DA_TBL_SET_T20E_PP_LEM_MAC_DA_TBL_SET_MAC_VAL_ASD_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mac_val.asd = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LEM_MAC_DA_TBL_SET_T20E_PP_LEM_MAC_DA_TBL_SET_MAC_VAL_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_LEM_MAC_DA_TBL_SET_T20E_PP_LEM_MAC_DA_TBL_SET_MAC_VAL_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mac_val.valid = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_pp_lem_da_tbl_set_unsafe(
          unit,
          &prm_mac_key,
          &prm_mac_val
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_lem_mac_da_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_lem_mac_da_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: t20e_pp_lem_mac_da_tbl_get (section t20e_acc_mact)
 */
int
  ui_t20e_acc_mact_t20e_pp_lem_mac_da_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  T20E_PP_LEM_TBL_KEY
    prm_mac_key;
  T20E_PP_MAC_TBL_DATA
    prm_mac_val;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_mact");
  soc_sand_proc_name = "t20e_pp_lem_mac_da_tbl_get";

  unit = t20e_get_default_unit();
  T20E_PP_LEM_TBL_KEY_clear(&prm_mac_key);
  T20E_PP_MAC_TBL_DATA_clear(&prm_mac_val);
  prm_mac_key.type = T20E_PP_LEM_TBL_KEY_TYPE_MACT;


  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LEM_MAC_DA_TBL_GET_T20E_PP_LEM_MAC_DA_TBL_GET_MAC_KEY_FID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_PP_LEM_MAC_DA_TBL_GET_T20E_PP_LEM_MAC_DA_TBL_GET_MAC_KEY_FID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mac_key.info.mact_entry.key_val.mac.fid = (uint32)param_val->value.ulong_value;
  }
  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_T20E_PP_LEM_MAC_DA_TBL_GET_T20E_PP_LEM_MAC_DA_TBL_GET_MAC_KEY_MAC_ID,1,
         &param_val,VAL_TEXT,err_msg))
  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_mac_key.info.mact_entry.key_val.mac.mac));
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mac_key after t20e_pp_lem_mac_da_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_pp_lem_da_tbl_get_unsafe(
          unit,
          &prm_mac_key,
          &prm_mac_val
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_pp_lem_mac_da_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_pp_lem_mac_da_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> mac_val:", TRUE);
  T20E_PP_MAC_TBL_DATA_print(&prm_mac_val);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mact_fid_counter_profile_db_tbl_set (section t20e_acc_mact)
 */
int
  ui_t20e_acc_mact_mact_fid_counter_profile_db_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_MACT_FID_COUNTER_PROFILE_DB_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_mact");
  soc_sand_proc_name = "t20e_mact_fid_counter_profile_db_tbl_set";

  unit = t20e_get_default_unit();
  T20E_MACT_FID_COUNTER_PROFILE_DB_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MACT_FID_COUNTER_PROFILE_DB_TBL_SET_MACT_FID_COUNTER_PROFILE_DB_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_MACT_FID_COUNTER_PROFILE_DB_TBL_SET_MACT_FID_COUNTER_PROFILE_DB_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after mact_fid_counter_profile_db_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_mact_fid_counter_profile_db_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_mact_fid_counter_profile_db_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_mact_fid_counter_profile_db_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MACT_FID_COUNTER_PROFILE_DB_TBL_SET_MACT_FID_COUNTER_PROFILE_DB_TBL_SET_TBL_DATA_LIMIT_EN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_MACT_FID_COUNTER_PROFILE_DB_TBL_SET_MACT_FID_COUNTER_PROFILE_DB_TBL_SET_TBL_DATA_LIMIT_EN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.limit_en = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MACT_FID_COUNTER_PROFILE_DB_TBL_SET_MACT_FID_COUNTER_PROFILE_DB_TBL_SET_TBL_DATA_INTERRUPT_EN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_MACT_FID_COUNTER_PROFILE_DB_TBL_SET_MACT_FID_COUNTER_PROFILE_DB_TBL_SET_TBL_DATA_INTERRUPT_EN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.interrupt_en = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MACT_FID_COUNTER_PROFILE_DB_TBL_SET_MACT_FID_COUNTER_PROFILE_DB_TBL_SET_TBL_DATA_LIMIT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_MACT_FID_COUNTER_PROFILE_DB_TBL_SET_MACT_FID_COUNTER_PROFILE_DB_TBL_SET_TBL_DATA_LIMIT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.limit = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_mact_fid_counter_profile_db_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_mact_fid_counter_profile_db_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_mact_fid_counter_profile_db_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mact_fid_counter_profile_db_tbl_get (section t20e_acc_mact)
 */
int
  ui_t20e_acc_mact_mact_fid_counter_profile_db_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_MACT_FID_COUNTER_PROFILE_DB_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_mact");
  soc_sand_proc_name = "t20e_mact_fid_counter_profile_db_tbl_get";

  unit = t20e_get_default_unit();
  T20E_MACT_FID_COUNTER_PROFILE_DB_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MACT_FID_COUNTER_PROFILE_DB_TBL_GET_MACT_FID_COUNTER_PROFILE_DB_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_MACT_FID_COUNTER_PROFILE_DB_TBL_GET_MACT_FID_COUNTER_PROFILE_DB_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after mact_fid_counter_profile_db_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_mact_fid_counter_profile_db_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_mact_fid_counter_profile_db_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_mact_fid_counter_profile_db_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_MACT_FID_COUNTER_PROFILE_DB_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mact_fid_counter_db_tbl_set (section t20e_acc_mact)
 */
int
  ui_t20e_acc_mact_mact_fid_counter_db_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_MACT_FID_COUNTER_DB_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_mact");
  soc_sand_proc_name = "t20e_mact_fid_counter_db_tbl_set";

  unit = t20e_get_default_unit();
  T20E_MACT_FID_COUNTER_DB_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MACT_FID_COUNTER_DB_TBL_SET_MACT_FID_COUNTER_DB_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_MACT_FID_COUNTER_DB_TBL_SET_MACT_FID_COUNTER_DB_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after mact_fid_counter_db_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_mact_fid_counter_db_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_mact_fid_counter_db_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_mact_fid_counter_db_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MACT_FID_COUNTER_DB_TBL_SET_MACT_FID_COUNTER_DB_TBL_SET_TBL_DATA_PROFILE_POINTER_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_MACT_FID_COUNTER_DB_TBL_SET_MACT_FID_COUNTER_DB_TBL_SET_TBL_DATA_PROFILE_POINTER_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.profile_pointer = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MACT_FID_COUNTER_DB_TBL_SET_MACT_FID_COUNTER_DB_TBL_SET_TBL_DATA_ENTRY_COUNT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_MACT_FID_COUNTER_DB_TBL_SET_MACT_FID_COUNTER_DB_TBL_SET_TBL_DATA_ENTRY_COUNT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.entry_count = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_mact_fid_counter_db_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_mact_fid_counter_db_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_mact_fid_counter_db_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mact_fid_counter_db_tbl_get (section t20e_acc_mact)
 */
int
  ui_t20e_acc_mact_mact_fid_counter_db_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_MACT_FID_COUNTER_DB_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_mact");
  soc_sand_proc_name = "t20e_mact_fid_counter_db_tbl_get";

  unit = t20e_get_default_unit();
  T20E_MACT_FID_COUNTER_DB_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MACT_FID_COUNTER_DB_TBL_GET_MACT_FID_COUNTER_DB_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_MACT_FID_COUNTER_DB_TBL_GET_MACT_FID_COUNTER_DB_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after mact_fid_counter_db_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_mact_fid_counter_db_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_mact_fid_counter_db_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_mact_fid_counter_db_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_MACT_FID_COUNTER_DB_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mact_flush_db_tbl_set (section t20e_acc_mact)
 */
#if flsuh_db_not_defined
int
  ui_t20e_acc_mact_mact_flush_db_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  uint32
    prm_action_transplant_payload_mask_index = 0xFFFFFFFF;
  uint32
    prm_action_transplant_payload_data_index = 0xFFFFFFFF;
  uint32
    prm_compare_payload_mask_index = 0xFFFFFFFF;
  uint32
    prm_compare_payload_data_index = 0xFFFFFFFF;
  T20E_MACT_FLUSH_DB_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_mact");
  soc_sand_proc_name = "t20e_mact_flush_db_tbl_set";

  unit = t20e_get_default_unit();
  T20E_MACT_FLUSH_DB_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MACT_FLUSH_DB_TBL_SET_MACT_FLUSH_DB_TBL_SET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_MACT_FLUSH_DB_TBL_SET_MACT_FLUSH_DB_TBL_SET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after mact_flush_db_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_mact_flush_db_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_mact_flush_db_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_mact_flush_db_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MACT_FLUSH_DB_TBL_SET_MACT_FLUSH_DB_TBL_SET_TBL_DATA_ACTION_TRANSPLANT_PAYLOAD_MASK_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_MACT_FLUSH_DB_TBL_SET_MACT_FLUSH_DB_TBL_SET_TBL_DATA_ACTION_TRANSPLANT_PAYLOAD_MASK_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_action_transplant_payload_mask_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_action_transplant_payload_mask_index != 0xFFFFFFFF)
  {

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MACT_FLUSH_DB_TBL_SET_MACT_FLUSH_DB_TBL_SET_TBL_DATA_ACTION_TRANSPLANT_PAYLOAD_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_MACT_FLUSH_DB_TBL_SET_MACT_FLUSH_DB_TBL_SET_TBL_DATA_ACTION_TRANSPLANT_PAYLOAD_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.action_transplant_payload_mask[ prm_action_transplant_payload_mask_index] = (uint32)param_val->value.ulong_value;
  }

  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MACT_FLUSH_DB_TBL_SET_MACT_FLUSH_DB_TBL_SET_TBL_DATA_ACTION_TRANSPLANT_PAYLOAD_DATA_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_MACT_FLUSH_DB_TBL_SET_MACT_FLUSH_DB_TBL_SET_TBL_DATA_ACTION_TRANSPLANT_PAYLOAD_DATA_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_action_transplant_payload_data_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_action_transplant_payload_data_index != 0xFFFFFFFF)
  {

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MACT_FLUSH_DB_TBL_SET_MACT_FLUSH_DB_TBL_SET_TBL_DATA_ACTION_TRANSPLANT_PAYLOAD_DATA_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_MACT_FLUSH_DB_TBL_SET_MACT_FLUSH_DB_TBL_SET_TBL_DATA_ACTION_TRANSPLANT_PAYLOAD_DATA_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.action_transplant_payload_data[ prm_action_transplant_payload_data_index] = (uint32)param_val->value.ulong_value;
  }

  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MACT_FLUSH_DB_TBL_SET_MACT_FLUSH_DB_TBL_SET_TBL_DATA_ACTION_DROP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_MACT_FLUSH_DB_TBL_SET_MACT_FLUSH_DB_TBL_SET_TBL_DATA_ACTION_DROP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.action_drop = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MACT_FLUSH_DB_TBL_SET_MACT_FLUSH_DB_TBL_SET_TBL_DATA_COMPARE_PAYLOAD_MASK_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_MACT_FLUSH_DB_TBL_SET_MACT_FLUSH_DB_TBL_SET_TBL_DATA_COMPARE_PAYLOAD_MASK_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_compare_payload_mask_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_compare_payload_mask_index != 0xFFFFFFFF)
  {

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MACT_FLUSH_DB_TBL_SET_MACT_FLUSH_DB_TBL_SET_TBL_DATA_COMPARE_PAYLOAD_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_MACT_FLUSH_DB_TBL_SET_MACT_FLUSH_DB_TBL_SET_TBL_DATA_COMPARE_PAYLOAD_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.compare_payload_mask[ prm_compare_payload_mask_index] = (uint32)param_val->value.ulong_value;
  }

  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MACT_FLUSH_DB_TBL_SET_MACT_FLUSH_DB_TBL_SET_TBL_DATA_COMPARE_PAYLOAD_DATA_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_MACT_FLUSH_DB_TBL_SET_MACT_FLUSH_DB_TBL_SET_TBL_DATA_COMPARE_PAYLOAD_DATA_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_compare_payload_data_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_compare_payload_data_index != 0xFFFFFFFF)
  {

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MACT_FLUSH_DB_TBL_SET_MACT_FLUSH_DB_TBL_SET_TBL_DATA_COMPARE_PAYLOAD_DATA_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_MACT_FLUSH_DB_TBL_SET_MACT_FLUSH_DB_TBL_SET_TBL_DATA_COMPARE_PAYLOAD_DATA_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.compare_payload_data[ prm_compare_payload_data_index] = (uint32)param_val->value.ulong_value;
  }

  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MACT_FLUSH_DB_TBL_SET_MACT_FLUSH_DB_TBL_SET_TBL_DATA_COMPARE_KEY_18_MSBS_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_MACT_FLUSH_DB_TBL_SET_MACT_FLUSH_DB_TBL_SET_TBL_DATA_COMPARE_KEY_18_MSBS_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.compare_key_18_msbs_mask = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MACT_FLUSH_DB_TBL_SET_MACT_FLUSH_DB_TBL_SET_TBL_DATA_COMPARE_KEY_18_MSBS_DATA_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_MACT_FLUSH_DB_TBL_SET_MACT_FLUSH_DB_TBL_SET_TBL_DATA_COMPARE_KEY_18_MSBS_DATA_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.compare_key_18_msbs_data = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MACT_FLUSH_DB_TBL_SET_MACT_FLUSH_DB_TBL_SET_TBL_DATA_COMPARE_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_MACT_FLUSH_DB_TBL_SET_MACT_FLUSH_DB_TBL_SET_TBL_DATA_COMPARE_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_data.compare_valid = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_mact_flush_db_tbl_set(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_mact_flush_db_tbl_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_mact_flush_db_tbl_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mact_flush_db_tbl_get (section t20e_acc_mact)
 */
int
  ui_t20e_acc_mact_mact_flush_db_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_offset_ndx;
  T20E_MACT_FLUSH_DB_TBL_DATA
    prm_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_mact");
  soc_sand_proc_name = "t20e_mact_flush_db_tbl_get";

  unit = t20e_get_default_unit();
  T20E_MACT_FLUSH_DB_TBL_DATA_clear(&prm_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MACT_FLUSH_DB_TBL_GET_MACT_FLUSH_DB_TBL_GET_ENTRY_OFFSET_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_MACT_FLUSH_DB_TBL_GET_MACT_FLUSH_DB_TBL_GET_ENTRY_OFFSET_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_offset_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_offset_ndx after mact_flush_db_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_mact_flush_db_tbl_get(
          unit,
          prm_entry_offset_ndx,
          &prm_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_mact_flush_db_tbl_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_mact_flush_db_tbl_get");
    goto exit;
  }

  send_string_to_screen("--> tbl_data:", TRUE);
  T20E_MACT_FLUSH_DB_TBL_DATA_print(&prm_tbl_data);


  goto exit;
exit:
  return ui_ret;
}
#endif /*flsuh_db_not_defined*/

#endif

#ifdef UI_T20E_REG_ACCESS

STATIC uint32
  t20e_ui_read_fld(
    SOC_SAND_IN  int      unit,
    SOC_SAND_IN  T20E_REG_FIELD*  field,
    SOC_SAND_OUT uint32*         val
  )
{
  uint32
    res = SOC_SAND_OK,
    offset = 0,
    reg_val = 0,
    fld_val = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(T20E_READ_FLD_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(field);
  SOC_SAND_CHECK_NULL_INPUT(val);


  offset = field->base;

  res = soc_sand_mem_read_unsafe(
          unit,
          &(reg_val),
          offset,
          sizeof(uint32),
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = t20e_field_from_reg_get(
          &(reg_val),
          field,
          &(fld_val)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  *val = fld_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in t20e_ui_read_fld",0,0);
}

STATIC uint32
  t20e_ui_write_fld(
    SOC_SAND_IN  int      unit,
    SOC_SAND_IN  T20E_REG_FIELD*  field,
    SOC_SAND_IN  uint32          val
  )
{
  uint32
    max_fld_val,
    res = SOC_SAND_OK;
  uint32
    offset = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(T20E_WRITE_FLD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(field);

  max_fld_val = T20E_FLD_MAX(*field);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    val, max_fld_val,
    T20E_VAL_IS_OUT_OF_RANGE_ERR, 10, exit
 );


  offset = field->base;

  res = soc_sand_read_modify_write(
          soc_sand_get_chip_descriptor_base_addr(unit),
          offset,
          field->lsb,
          SOC_SAND_BITS_MASK(field->msb, field->lsb),
          val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in t20e_ui_write_fld",offset,val);
}

/********************************************************************
 *  Function handler: read_fld (section reg_access)
 */
int
  ui_t20e_acc_reg_access_read_fld(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  T20E_REG_FIELD
    prm_field;
  uint32
    prm_val = 0;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_reg_access");
  soc_sand_proc_name = "t20e_read_fld";

  unit = t20e_get_default_unit();
  prm_field.step = 0;
  prm_field.lsb = 0;
  prm_field.msb = 31;

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_READ_FLD_READ_FLD_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_READ_FLD_READ_FLD_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_field.base = (uint32)param_val->value.ulong_value;
  }

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_READ_FLD_READ_FLD_FIELD_LSB_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_READ_FLD_READ_FLD_FIELD_LSB_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_field.lsb = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_READ_FLD_READ_FLD_FIELD_MSB_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_READ_FLD_READ_FLD_FIELD_MSB_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_field.msb = (uint8)param_val->value.ulong_value;
  }

  prm_field.base *= 4;

  /* Call function */
  ret = t20e_ui_read_fld(
          unit,
          &prm_field,
          &prm_val
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    t20e_disp_result(ret, "t20e_read_fld");
    goto exit;
  }

  soc_sand_os_printf( "val: 0x%.8lX\n\r",prm_val);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: write_fld (section reg_access)
 */
int
  ui_t20e_acc_reg_access_write_fld(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  T20E_REG_FIELD
    prm_field;
  uint32
    prm_val = 0;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_reg_access");
  soc_sand_proc_name = "t20e_write_fld";

  unit = t20e_get_default_unit();
  prm_field.step = 0;
  prm_field.lsb = 0;
  prm_field.msb = 31;

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_WRITE_FLD_WRITE_FLD_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_WRITE_FLD_WRITE_FLD_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_field.base = (uint32)param_val->value.ulong_value;
  }

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_WRITE_FLD_WRITE_FLD_FIELD_LSB_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_WRITE_FLD_WRITE_FLD_FIELD_LSB_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_field.lsb = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_WRITE_FLD_WRITE_FLD_FIELD_MSB_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_WRITE_FLD_WRITE_FLD_FIELD_MSB_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_field.msb = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_WRITE_FLD_WRITE_FLD_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_WRITE_FLD_WRITE_FLD_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_val = (uint32)param_val->value.ulong_value;
  }

  prm_field.base *= 4;

  /* Call function */
  ret = t20e_ui_write_fld(
          unit,
          &prm_field,
          prm_val
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    t20e_disp_result(ret, "t20e_write_fld");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: read_reg (section reg_access)
 */
int
  ui_t20e_acc_reg_access_read_reg(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  T20E_REG_FIELD
    prm_field;
  uint32
    prm_val,
    do_ndx,
    nof_occurs = 1,
    wait_time=1,
    sec[3],
    nano[3];

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_reg_access");
  soc_sand_proc_name = "t20e_read_reg";

  unit = t20e_get_default_unit();
  prm_field.step = 0;
  prm_field.lsb = 0;
  prm_field.msb = 31;
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_READ_REG_READ_REG_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_READ_REG_READ_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_field.base = (uint32)param_val->value.ulong_value;
  }

  /* Get parameters */
  prm_field.base *= 4;

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_DO_TWICE_ID, 1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_DO_TWICE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    nof_occurs = 2;
    wait_time = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  for(do_ndx=0;do_ndx<nof_occurs;do_ndx++)
  {
    ret = t20e_ui_read_fld(
            unit,
            &prm_field,
            &prm_val
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      t20e_disp_result(ret, "t20e_read_reg");
      goto exit;
    }

    if(nof_occurs==2)
    {
      soc_sand_os_get_time(&sec[do_ndx],&nano[do_ndx]);
      if(do_ndx == 0)
      {
        sal_msleep(wait_time);
      }
      else
      {
        soc_sand_os_get_time_diff(sec[0],nano[0],sec[1],nano[1],&sec[2],&nano[2]);
        soc_sand_os_printf( " Waited %d seconds and %d nanoseconds\n\r", sec[2],nano[2]);
      }
    }
    soc_sand_os_printf( "val: 0x%.8lX\n\r",prm_val);
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: write_reg (section reg_access)
 */
int
  ui_t20e_acc_reg_access_write_reg(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = 0;
  T20E_REG_FIELD
    prm_field;
  uint32
    prm_val = 0;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_reg_access");
  soc_sand_proc_name = "t20e_write_reg";

  unit = t20e_get_default_unit();
  prm_field.step = 0;
  prm_field.lsb = 0;
  prm_field.msb = 31;

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_WRITE_REG_WRITE_REG_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_WRITE_REG_WRITE_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_field.base = (uint32)param_val->value.ulong_value;
  }

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_WRITE_REG_WRITE_REG_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_WRITE_REG_WRITE_REG_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_val = (uint32)param_val->value.ulong_value;
  }

  prm_field.base *= 4;

  /* Call function */
  ret = t20e_ui_write_fld(
          unit,
          &prm_field,
          prm_val
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_write_reg - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_write_reg");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

int
  ui_t20e_iread(
    CURRENT_LINE *current_line
  )
{
  uint32
    addr=0,
    data[3],
    ret=0,
    size=0,
    do_ndx,
    nof_occurs = 1,
    wait_time=0,
    sec[3],
    nano[3],
    module_id = 0;

  UI_MACROS_INIT_FUNCTION("ui_t20e_iread");
  soc_sand_proc_name = "ui_t20e_iread";

  unit = t20e_get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_IRW_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_IRW_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    addr = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_SIZE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_SIZE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    size = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_DO_TWICE_ID, 1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_DO_TWICE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    nof_occurs = 2;
    wait_time = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_IACC_MODULE_ID_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_T20E_IACC_MODULE_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    module_id = param_val->numeric_equivalent;
  }

  for(do_ndx=0;do_ndx<nof_occurs;do_ndx++)
  {
    ret = soc_sand_tbl_read(
      unit,
      data,
      addr,
      size * sizeof(uint32),
      module_id,
      size * sizeof(uint32)
      );
    if(nof_occurs==2)
    {
      soc_sand_os_get_time(&sec[do_ndx],&nano[do_ndx]);
      if(do_ndx == 0)
      {
        sal_msleep(wait_time);
      }
      else
      {
        soc_sand_os_get_time_diff(sec[0],nano[0],sec[1],nano[1],&sec[2],&nano[2]);
        soc_sand_os_printf( " Waited %d: %d \n\r", sec[2],nano[2]);
      }
    }
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** soc_sand_mem_iread - FAIL", TRUE);
      t20e_disp_result(ret, "soc_sand_mem_iread");
      goto exit;
    }
    soc_sand_os_printf( "addr 0x%x data 0x%x 0x%x 0x%x", addr, data[2], data[1], data[0]);
  }

  goto exit;
exit:
  return ui_ret;
}

int
  ui_t20e_iwrite(
    CURRENT_LINE *current_line
  )
{
  uint32
    addr=0,
    param_i,
    data[3],
    ret,
    vals[3],
    size=0,
    do_ndx,
    nof_occurs = 1,
    wait_time=0,
    sec[3],
    nano[3],
    module_id = 0;

  UI_MACROS_INIT_FUNCTION("ui_t20e_iwrite");
  soc_sand_proc_name = "ui_t20e_iwrite";

  unit = t20e_get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_IRW_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_IRW_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    addr = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_SIZE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_SIZE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    size = (uint32)param_val->value.ulong_value;
  }

  for (param_i=1; param_i<=3; ++param_i)
  {
    ret = search_param_val_pairs(current_line,&match_index,PARAM_T20E_ACC_IWRITE_VAL_ID,param_i);
    if (0 != ret)
    {
      ret=0;
      break;
    }
    UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(PARAM_T20E_ACC_IWRITE_VAL_ID,param_i);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    vals[param_i-1] = param_val->value.ulong_value;
  }


  if(size == 1)
  {
    data[0] = vals[0];
  }
  else if(size == 2)
  {
    data[1] = vals[0];
    data[0] = vals[1];
  }
 else if(size == 3)
  {
    data[2] = vals[0];
    data[1] = vals[1];
    data[0] = vals[2];
  }
  else
  {
    soc_sand_os_printf( "size %d is invalid\n\r", size);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_DO_TWICE_ID, 1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_DO_TWICE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    nof_occurs = 2;
    wait_time = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_IACC_MODULE_ID_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_T20E_IACC_MODULE_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    module_id = param_val->numeric_equivalent;
  }

  for(do_ndx=0;do_ndx<nof_occurs;do_ndx++)
  {
    ret = soc_sand_tbl_write(
      unit,
      data,
      addr,
      size * sizeof(uint32),
      module_id,
      size * sizeof(uint32)
      );
    if(nof_occurs==2)
    {
      soc_sand_os_get_time(&sec[do_ndx],&nano[do_ndx]);
      if(do_ndx == 0)
      {
        sal_msleep(wait_time);
      }
      else
      {
        soc_sand_os_get_time_diff(sec[0],nano[0],sec[1],nano[1],&sec[2],&nano[2]);
        soc_sand_os_printf( " Waited %d: %d \n\r", sec[2],nano[2]);
      }
    }
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** soc_sand_mem_iwrite - FAIL", TRUE);
      t20e_disp_result(ret, "soc_sand_mem_iwrite");
      goto exit;
    }
  }


  goto exit;
exit:
  return ui_ret;
}

#endif


#ifdef UI_T20E_HW_IF
/********************************************************************
 *  Function handler: cpu_info_set (section hw_if)
 */
int
  ui_t20e_acc_hw_if_cpu_info_set(
    CURRENT_LINE *current_line
  )
{
  return 0;
}

/********************************************************************
 *  Function handler: cpu_info_get (section hw_if)
 */
int
  ui_t20e_acc_hw_if_cpu_info_get(
    CURRENT_LINE *current_line
  )
{
  return 0;
}

/********************************************************************
 *  Function handler: dram_info_set (section hw_if)
 */
int
  ui_t20e_acc_hw_if_dram_info_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  T20E_HW_IF_DRAM_INFO
    prm_dram_info;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_hw_if");
  soc_sand_proc_name = "t20e_hw_if_dram_info_set";

  unit = t20e_get_default_unit();
  T20E_HW_IF_DRAM_INFO_clear(&prm_dram_info);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = t20e_hw_if_dram_info_get(
          unit,
          &prm_dram_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_hw_if_dram_info_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_hw_if_dram_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_HW_IF_DRAM_INFO_SET_DRAM_INFO_SET_DRAM_INFO_DRAM_SIZE_MBIT_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_T20E_HW_IF_DRAM_INFO_SET_DRAM_INFO_SET_DRAM_INFO_DRAM_SIZE_MBIT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_dram_info.rldram_size_mbit = param_val->numeric_equivalent;
  }


  /* Call function */
  ret = t20e_hw_if_dram_info_set(
          unit,
          &prm_dram_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_hw_if_dram_info_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_hw_if_dram_info_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: dram_info_get (section hw_if)
 */
int
  ui_t20e_acc_hw_if_dram_info_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  T20E_HW_IF_DRAM_INFO
    prm_dram_info;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_hw_if");
  soc_sand_proc_name = "t20e_hw_if_dram_info_get";

  unit = t20e_get_default_unit();
  T20E_HW_IF_DRAM_INFO_clear(&prm_dram_info);

  /* Get parameters */

  /* Call function */
  ret = t20e_hw_if_dram_info_get(
          unit,
          &prm_dram_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_hw_if_dram_info_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_hw_if_dram_info_get");
    goto exit;
  }

  send_string_to_screen("--> dram_info:", TRUE);
  T20E_HW_IF_DRAM_INFO_print(&prm_dram_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: qdr_info_set (section hw_if)
 */
int
  ui_t20e_acc_hw_if_qdr_info_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  T20E_HW_IF_QDR_INFO
    prm_qdr_info;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_hw_if");
  soc_sand_proc_name = "t20e_hw_if_qdr_info_set";

  unit = t20e_get_default_unit();
  T20E_HW_IF_QDR_INFO_clear(&prm_qdr_info);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = t20e_hw_if_qdr_info_get(
          unit,
          &prm_qdr_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_hw_if_qdr_info_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_hw_if_qdr_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_HW_IF_QDR_INFO_SET_QDR_INFO_SET_QDR_INFO_QDR_SIZE_MBIT_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_T20E_HW_IF_QDR_INFO_SET_QDR_INFO_SET_QDR_INFO_QDR_SIZE_MBIT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_qdr_info.qdr_size_mbit = param_val->numeric_equivalent;
  }


  /* Call function */
  ret = t20e_hw_if_qdr_info_set(
          unit,
          &prm_qdr_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_hw_if_qdr_info_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_hw_if_qdr_info_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: qdr_info_get (section hw_if)
 */
int
  ui_t20e_acc_hw_if_qdr_info_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  T20E_HW_IF_QDR_INFO
    prm_qdr_info;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_hw_if");
  soc_sand_proc_name = "t20e_hw_if_qdr_info_get";

  unit = t20e_get_default_unit();
  T20E_HW_IF_QDR_INFO_clear(&prm_qdr_info);

  /* Get parameters */

  /* Call function */
  ret = t20e_hw_if_qdr_info_get(
          unit,
          &prm_qdr_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_hw_if_qdr_info_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_hw_if_qdr_info_get");
    goto exit;
  }

  send_string_to_screen("--> qdr_info:", TRUE);
  T20E_HW_IF_QDR_INFO_print(&prm_qdr_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: xaui_info_set (section hw_if)
 */
int
  ui_t20e_acc_hw_if_xaui_info_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_xaui_ndx;
  T20E_HW_IF_XAUI_INFO
    prm_xaui_info;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_hw_if");
  soc_sand_proc_name = "t20e_hw_if_xaui_info_set";

  unit = t20e_get_default_unit();
  T20E_HW_IF_XAUI_INFO_clear(&prm_xaui_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_HW_IF_XAUI_INFO_SET_XAUI_INFO_SET_XAUI_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_HW_IF_XAUI_INFO_SET_XAUI_INFO_SET_XAUI_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_xaui_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter xaui_ndx after xaui_info_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_hw_if_xaui_info_get(
          unit,
          prm_xaui_ndx,
          &prm_xaui_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_hw_if_xaui_info_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_hw_if_xaui_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_HW_IF_XAUI_INFO_SET_XAUI_INFO_SET_XAUI_INFO_SERDES_INFO_TX_VODCTRL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_HW_IF_XAUI_INFO_SET_XAUI_INFO_SET_XAUI_INFO_SERDES_INFO_TX_VODCTRL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_xaui_info.serdes_info.tx_vodctrl = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_HW_IF_XAUI_INFO_SET_XAUI_INFO_SET_XAUI_INFO_SERDES_INFO_TX_PREEMP_2T_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_HW_IF_XAUI_INFO_SET_XAUI_INFO_SET_XAUI_INFO_SERDES_INFO_TX_PREEMP_2T_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_xaui_info.serdes_info.tx_preemp_2t = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_HW_IF_XAUI_INFO_SET_XAUI_INFO_SET_XAUI_INFO_SERDES_INFO_TX_PREEMP_1T_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_HW_IF_XAUI_INFO_SET_XAUI_INFO_SET_XAUI_INFO_SERDES_INFO_TX_PREEMP_1T_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_xaui_info.serdes_info.tx_preemp_1t = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_HW_IF_XAUI_INFO_SET_XAUI_INFO_SET_XAUI_INFO_SERDES_INFO_TX_PREEMP_0T_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_HW_IF_XAUI_INFO_SET_XAUI_INFO_SET_XAUI_INFO_SERDES_INFO_TX_PREEMP_0T_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_xaui_info.serdes_info.tx_preemp_0t = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_HW_IF_XAUI_INFO_SET_XAUI_INFO_SET_XAUI_INFO_SERDES_INFO_RX_EQ_DC_GAIN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_HW_IF_XAUI_INFO_SET_XAUI_INFO_SET_XAUI_INFO_SERDES_INFO_RX_EQ_DC_GAIN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_xaui_info.serdes_info.rx_eq_dc_gain = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_HW_IF_XAUI_INFO_SET_XAUI_INFO_SET_XAUI_INFO_SERDES_INFO_RX_EQ_CTRL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_HW_IF_XAUI_INFO_SET_XAUI_INFO_SET_XAUI_INFO_SERDES_INFO_RX_EQ_CTRL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_xaui_info.serdes_info.rx_eq_ctrl = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_HW_IF_XAUI_INFO_SET_XAUI_INFO_SET_XAUI_INFO_ENABLE_BCT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_HW_IF_XAUI_INFO_SET_XAUI_INFO_SET_XAUI_INFO_ENABLE_BCT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_xaui_info.enable_bct = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_HW_IF_XAUI_INFO_SET_XAUI_INFO_SET_XAUI_INFO_ENABLE_FLOW_CTRL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_HW_IF_XAUI_INFO_SET_XAUI_INFO_SET_XAUI_INFO_ENABLE_FLOW_CTRL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_xaui_info.enable_flow_ctrl = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_HW_IF_XAUI_INFO_SET_XAUI_INFO_SET_XAUI_INFO_ENABLE_IF_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_HW_IF_XAUI_INFO_SET_XAUI_INFO_SET_XAUI_INFO_ENABLE_IF_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_xaui_info.enable_if = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_hw_if_xaui_info_set(
          unit,
          prm_xaui_ndx,
          &prm_xaui_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_hw_if_xaui_info_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_hw_if_xaui_info_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: xaui_info_get (section hw_if)
 */
int
  ui_t20e_acc_hw_if_xaui_info_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_xaui_ndx;
  T20E_HW_IF_XAUI_INFO
    prm_xaui_info;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_hw_if");
  soc_sand_proc_name = "t20e_hw_if_xaui_info_get";

  unit = t20e_get_default_unit();
  T20E_HW_IF_XAUI_INFO_clear(&prm_xaui_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_HW_IF_XAUI_INFO_GET_XAUI_INFO_GET_XAUI_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_HW_IF_XAUI_INFO_GET_XAUI_INFO_GET_XAUI_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_xaui_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter xaui_ndx after xaui_info_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_hw_if_xaui_info_get(
          unit,
          prm_xaui_ndx,
          &prm_xaui_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_hw_if_xaui_info_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_hw_if_xaui_info_get");
    goto exit;
  }

  send_string_to_screen("--> xaui_info:", TRUE);
  T20E_HW_IF_XAUI_INFO_print(&prm_xaui_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: spaui_info_set (section hw_if)
 */
int
  ui_t20e_acc_hw_if_spaui_info_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_spaui_ndx;
  T20E_HW_IF_SPAUI_INFO
    prm_spaui_info;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_hw_if");
  soc_sand_proc_name = "t20e_hw_if_spaui_info_set";

  unit = t20e_get_default_unit();
  T20E_HW_IF_SPAUI_INFO_clear(&prm_spaui_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_HW_IF_SPAUI_INFO_SET_SPAUI_INFO_SET_SPAUI_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_HW_IF_SPAUI_INFO_SET_SPAUI_INFO_SET_SPAUI_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_spaui_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter spaui_ndx after spaui_info_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = t20e_hw_if_spaui_info_get(
          unit,
          prm_spaui_ndx,
          &prm_spaui_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_hw_if_spaui_info_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_hw_if_spaui_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_HW_IF_SPAUI_INFO_SET_SPAUI_INFO_SET_SPAUI_INFO_SERDES_INFO_TX_VODCTRL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_HW_IF_SPAUI_INFO_SET_SPAUI_INFO_SET_SPAUI_INFO_SERDES_INFO_TX_VODCTRL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_spaui_info.serdes_info.tx_vodctrl = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_HW_IF_SPAUI_INFO_SET_SPAUI_INFO_SET_SPAUI_INFO_SERDES_INFO_TX_PREEMP_2T_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_HW_IF_SPAUI_INFO_SET_SPAUI_INFO_SET_SPAUI_INFO_SERDES_INFO_TX_PREEMP_2T_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_spaui_info.serdes_info.tx_preemp_2t = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_HW_IF_SPAUI_INFO_SET_SPAUI_INFO_SET_SPAUI_INFO_SERDES_INFO_TX_PREEMP_1T_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_HW_IF_SPAUI_INFO_SET_SPAUI_INFO_SET_SPAUI_INFO_SERDES_INFO_TX_PREEMP_1T_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_spaui_info.serdes_info.tx_preemp_1t = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_HW_IF_SPAUI_INFO_SET_SPAUI_INFO_SET_SPAUI_INFO_SERDES_INFO_TX_PREEMP_0T_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_HW_IF_SPAUI_INFO_SET_SPAUI_INFO_SET_SPAUI_INFO_SERDES_INFO_TX_PREEMP_0T_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_spaui_info.serdes_info.tx_preemp_0t = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_HW_IF_SPAUI_INFO_SET_SPAUI_INFO_SET_SPAUI_INFO_SERDES_INFO_RX_EQ_DC_GAIN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_HW_IF_SPAUI_INFO_SET_SPAUI_INFO_SET_SPAUI_INFO_SERDES_INFO_RX_EQ_DC_GAIN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_spaui_info.serdes_info.rx_eq_dc_gain = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_HW_IF_SPAUI_INFO_SET_SPAUI_INFO_SET_SPAUI_INFO_SERDES_INFO_RX_EQ_CTRL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_HW_IF_SPAUI_INFO_SET_SPAUI_INFO_SET_SPAUI_INFO_SERDES_INFO_RX_EQ_CTRL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_spaui_info.serdes_info.rx_eq_ctrl = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_HW_IF_SPAUI_INFO_SET_SPAUI_INFO_SET_SPAUI_INFO_ENABLE_BCT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_HW_IF_SPAUI_INFO_SET_SPAUI_INFO_SET_SPAUI_INFO_ENABLE_BCT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_spaui_info.enable_bct_rx = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_hw_if_spaui_info_set(
          unit,
          prm_spaui_ndx,
          &prm_spaui_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_hw_if_spaui_info_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_hw_if_spaui_info_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: spaui_info_get (section hw_if)
 */
int
  ui_t20e_acc_hw_if_spaui_info_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_spaui_ndx;
  T20E_HW_IF_SPAUI_INFO
    prm_spaui_info;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_hw_if");
  soc_sand_proc_name = "t20e_hw_if_spaui_info_get";

  unit = t20e_get_default_unit();
  T20E_HW_IF_SPAUI_INFO_clear(&prm_spaui_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_HW_IF_SPAUI_INFO_GET_SPAUI_INFO_GET_SPAUI_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_HW_IF_SPAUI_INFO_GET_SPAUI_INFO_GET_SPAUI_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_spaui_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter spaui_ndx after spaui_info_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_hw_if_spaui_info_get(
          unit,
          prm_spaui_ndx,
          &prm_spaui_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_hw_if_spaui_info_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_hw_if_spaui_info_get");
    goto exit;
  }

  send_string_to_screen("--> spaui_info:", TRUE);
  T20E_HW_IF_SPAUI_INFO_print(&prm_spaui_info);


  goto exit;
exit:
  return ui_ret;
}

#endif
#ifdef UI_T20E_MGMT
/********************************************************************
 *  Function handler: register_device (section mgmt)
 */
int
  ui_t20e_acc_mgmt_register_device(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_base_address;
  SOC_SAND_RESET_DEVICE_FUNC_PTR
    prm_reset_device_ptr;
  uint32
    prm_unit_ptr;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_mgmt");
  soc_sand_proc_name = "t20e_register_device";

  unit = t20e_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_REGISTER_DEVICE_REGISTER_DEVICE_BASE_ADDRESS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_REGISTER_DEVICE_REGISTER_DEVICE_BASE_ADDRESS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_base_address = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_REGISTER_DEVICE_REGISTER_DEVICE_RESET_DEVICE_PTR_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_REGISTER_DEVICE_REGISTER_DEVICE_RESET_DEVICE_PTR_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_reset_device_ptr = (SOC_SAND_RESET_DEVICE_FUNC_PTR)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_register_device(
          &prm_base_address,
          prm_reset_device_ptr,
          &prm_unit_ptr
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_register_device - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_register_device");
    goto exit;
  }

  send_string_to_screen("--> unit_ptr:", TRUE);
  soc_sand_os_printf( "unit_ptr: %d\n",prm_unit_ptr);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: unregister_device (section mgmt)
 */
int
  ui_t20e_acc_mgmt_unregister_device(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_mgmt");
  soc_sand_proc_name = "t20e_unregister_device";

  unit = t20e_get_default_unit();

  /* Get parameters */

  /* Call function */
  ret = t20e_unregister_device(
          unit
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_unregister_device - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_unregister_device");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: operation_mode_set (section mgmt)
 */
int
  ui_t20e_acc_mgmt_operation_mode_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  T20E_MGMT_OPERATION_MODE
    prm_op_mode;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_mgmt");
  soc_sand_proc_name = "t20e_mgmt_operation_mode_set";

  unit = t20e_get_default_unit();
  T20E_MGMT_OPERATION_MODE_clear(&prm_op_mode);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = t20e_mgmt_operation_mode_get(
          unit,
          &prm_op_mode
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_mgmt_operation_mode_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_mgmt_operation_mode_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MGMT_OPERATION_MODE_SET_OPERATION_MODE_SET_OP_MODE_ENABLE_POLICING_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_MGMT_OPERATION_MODE_SET_OPERATION_MODE_SET_OP_MODE_ENABLE_POLICING_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_op_mode.enable_metering = (uint8)param_val->value.ulong_value;
  }

  

  /* Call function */
  ret = t20e_mgmt_operation_mode_set(
          unit,
          &prm_op_mode
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_mgmt_operation_mode_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_mgmt_operation_mode_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: operation_mode_get (section mgmt)
 */
int
  ui_t20e_acc_mgmt_operation_mode_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  T20E_MGMT_OPERATION_MODE
    prm_op_mode;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_mgmt");
  soc_sand_proc_name = "t20e_mgmt_operation_mode_get";

  unit = t20e_get_default_unit();
  T20E_MGMT_OPERATION_MODE_clear(&prm_op_mode);

  /* Get parameters */

  /* Call function */
  ret = t20e_mgmt_operation_mode_get(
          unit,
          &prm_op_mode
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_mgmt_operation_mode_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_mgmt_operation_mode_get");
    goto exit;
  }

  send_string_to_screen("--> op_mode:", TRUE);
  T20E_MGMT_OPERATION_MODE_print(&prm_op_mode);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: init_sequence_phase1 (section mgmt)
 */
int
  ui_t20e_acc_mgmt_init_sequence_phase1(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_spaui_index = 0xFFFFFFFF;
  uint32
    prm_xaui_index = 0xFFFFFFFF;
  T20E_MGMT_INIT_PHASE1_INFO
    prm_phase1_info;
  uint8
    prm_silent;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_mgmt");
  soc_sand_proc_name = "t20e_mgmt_init_sequence_phase1";

  unit = t20e_get_default_unit();
  T20E_MGMT_INIT_PHASE1_INFO_clear(&prm_phase1_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_phase1_info.mask = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_SPAUI_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_SPAUI_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_spaui_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_spaui_index != 0xFFFFFFFF)
  {

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_SPAUI_SERDES_INFO_TX_VODCTRL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_SPAUI_SERDES_INFO_TX_VODCTRL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_phase1_info.hw_adjust.nif.spaui[ prm_spaui_index].serdes_info.tx_vodctrl = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_SPAUI_SERDES_INFO_TX_PREEMP_2T_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_SPAUI_SERDES_INFO_TX_PREEMP_2T_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_phase1_info.hw_adjust.nif.spaui[ prm_spaui_index].serdes_info.tx_preemp_2t = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_SPAUI_SERDES_INFO_TX_PREEMP_1T_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_SPAUI_SERDES_INFO_TX_PREEMP_1T_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_phase1_info.hw_adjust.nif.spaui[ prm_spaui_index].serdes_info.tx_preemp_1t = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_SPAUI_SERDES_INFO_TX_PREEMP_0T_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_SPAUI_SERDES_INFO_TX_PREEMP_0T_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_phase1_info.hw_adjust.nif.spaui[ prm_spaui_index].serdes_info.tx_preemp_0t = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_SPAUI_SERDES_INFO_RX_EQ_DC_GAIN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_SPAUI_SERDES_INFO_RX_EQ_DC_GAIN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_phase1_info.hw_adjust.nif.spaui[ prm_spaui_index].serdes_info.rx_eq_dc_gain = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_SPAUI_SERDES_INFO_RX_EQ_CTRL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_SPAUI_SERDES_INFO_RX_EQ_CTRL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_phase1_info.hw_adjust.nif.spaui[ prm_spaui_index].serdes_info.rx_eq_ctrl = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_SPAUI_ENABLE_BCT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_SPAUI_ENABLE_BCT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_phase1_info.hw_adjust.nif.spaui[ prm_spaui_index].enable_bct_rx = (uint8)param_val->value.ulong_value;
  }

  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_XAUI_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_XAUI_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_xaui_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_xaui_index != 0xFFFFFFFF)
  {

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_XAUI_SERDES_INFO_TX_VODCTRL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_XAUI_SERDES_INFO_TX_VODCTRL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_phase1_info.hw_adjust.nif.xaui[ prm_xaui_index].serdes_info.tx_vodctrl = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_XAUI_SERDES_INFO_TX_PREEMP_2T_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_XAUI_SERDES_INFO_TX_PREEMP_2T_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_phase1_info.hw_adjust.nif.xaui[ prm_xaui_index].serdes_info.tx_preemp_2t = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_XAUI_SERDES_INFO_TX_PREEMP_1T_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_XAUI_SERDES_INFO_TX_PREEMP_1T_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_phase1_info.hw_adjust.nif.xaui[ prm_xaui_index].serdes_info.tx_preemp_1t = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_XAUI_SERDES_INFO_TX_PREEMP_0T_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_XAUI_SERDES_INFO_TX_PREEMP_0T_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_phase1_info.hw_adjust.nif.xaui[ prm_xaui_index].serdes_info.tx_preemp_0t = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_XAUI_SERDES_INFO_RX_EQ_DC_GAIN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_XAUI_SERDES_INFO_RX_EQ_DC_GAIN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_phase1_info.hw_adjust.nif.xaui[ prm_xaui_index].serdes_info.rx_eq_dc_gain = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_XAUI_SERDES_INFO_RX_EQ_CTRL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_XAUI_SERDES_INFO_RX_EQ_CTRL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_phase1_info.hw_adjust.nif.xaui[ prm_xaui_index].serdes_info.rx_eq_ctrl = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_XAUI_ENABLE_BCT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_XAUI_ENABLE_BCT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_phase1_info.hw_adjust.nif.xaui[ prm_xaui_index].enable_bct = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_XAUI_ENABLE_FLOW_CTRL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_XAUI_ENABLE_FLOW_CTRL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_phase1_info.hw_adjust.nif.xaui[ prm_xaui_index].enable_flow_ctrl = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_XAUI_ENABLE_IF_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_XAUI_ENABLE_IF_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_phase1_info.hw_adjust.nif.xaui[ prm_xaui_index].enable_if = (uint8)param_val->value.ulong_value;
  }

  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_QDR_QDR_SIZE_MBIT_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_QDR_QDR_SIZE_MBIT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_phase1_info.hw_adjust.qdr.qdr_size_mbit = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_DRAM_DRAM_SIZE_MBIT_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_DRAM_DRAM_SIZE_MBIT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_phase1_info.hw_adjust.dram.rldram_size_mbit = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_SILENT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_SILENT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_silent = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_mgmt_init_sequence_phase1(
          unit,
          &prm_phase1_info,
          prm_silent
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_mgmt_init_sequence_phase1 - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_mgmt_init_sequence_phase1");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: init_sequence_phase2 (section mgmt)
 */
int
  ui_t20e_acc_mgmt_init_sequence_phase2(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  T20E_MGMT_INIT_PHASE2_INFO
    prm_phase2_info;
  uint8
    prm_silent;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_mgmt");
  soc_sand_proc_name = "t20e_mgmt_init_sequence_phase2";

  unit = t20e_get_default_unit();
  T20E_MGMT_INIT_PHASE2_INFO_clear(&prm_phase2_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE2_INIT_SEQUENCE_PHASE2_PHASE2_INFO_TBD_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE2_INIT_SEQUENCE_PHASE2_PHASE2_INFO_TBD_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_phase2_info.tbd = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE2_INIT_SEQUENCE_PHASE2_SILENT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE2_INIT_SEQUENCE_PHASE2_SILENT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_silent = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_mgmt_init_sequence_phase2(
          unit,
          &prm_phase2_info,
          prm_silent
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_mgmt_init_sequence_phase2 - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_mgmt_init_sequence_phase2");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: enable_traffic_set (section mgmt)
 */
int
  ui_t20e_acc_mgmt_enable_traffic_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint8
    prm_enable;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_mgmt");
  soc_sand_proc_name = "t20e_mgmt_enable_traffic_set";

  unit = t20e_get_default_unit();

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = t20e_mgmt_enable_traffic_get(
          unit,
          &prm_enable
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_mgmt_enable_traffic_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_mgmt_enable_traffic_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MGMT_ENABLE_TRAFFIC_SET_ENABLE_TRAFFIC_SET_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_MGMT_ENABLE_TRAFFIC_SET_ENABLE_TRAFFIC_SET_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_enable = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = t20e_mgmt_enable_traffic_set(
          unit,
          prm_enable
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_mgmt_enable_traffic_set - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_mgmt_enable_traffic_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: enable_traffic_get (section mgmt)
 */
int
  ui_t20e_acc_mgmt_enable_traffic_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint8
    prm_enable;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_mgmt");
  soc_sand_proc_name = "t20e_mgmt_enable_traffic_get";

  unit = t20e_get_default_unit();

  /* Get parameters */

  /* Call function */
  ret = t20e_mgmt_enable_traffic_get(
          unit,
          &prm_enable
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_mgmt_enable_traffic_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_mgmt_enable_traffic_get");
    goto exit;
  }

  send_string_to_screen("--> enable:", TRUE);
  soc_sand_os_printf( "enable: %u\n",prm_enable);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: pckt_size_range_set (section mgmt)
 */
int
  ui_t20e_acc_mgmt_pckt_size_range_set(
    CURRENT_LINE *current_line
  )
{
  return 0;
}

/********************************************************************
 *  Function handler: pckt_size_range_get (section mgmt)
 */
int
  ui_t20e_acc_mgmt_pckt_size_range_get(
    CURRENT_LINE *current_line
  )
{
  return 0;
}

#endif
#ifdef UI_T20E_DIAGNOSTICS
/********************************************************************
 *  Function handler: diag_errors_get (section diagnostics)
 */
int
  ui_t20e_acc_diagnostics_diag_errors_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  T20E_DIAG_ERR_SELECT
    prm_error_select_ndx;
  T20E_DIAG_ERRORS_VEC
    prm_errors_vec;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_diagnostics");
  soc_sand_proc_name = "t20e_diag_errors_get";

  unit = t20e_get_default_unit();
  T20E_DIAG_ERRORS_VEC_clear(&prm_errors_vec);

  prm_error_select_ndx = T20E_DIAG_ERR_SELECT_ALL;

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_DIAG_ERRORS_GET_DIAG_ERRORS_GET_ERROR_SELECT_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_T20E_DIAG_ERRORS_GET_DIAG_ERRORS_GET_ERROR_SELECT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_error_select_ndx = param_val->numeric_equivalent;
  }


  /* Call function */
  ret = t20e_diag_errors_get(
          unit,
          prm_error_select_ndx,
          &prm_errors_vec
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_diag_errors_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_diag_errors_get");
    goto exit;
  }

  send_string_to_screen("--> errors_vec:", TRUE);
  T20E_DIAG_ERRORS_VEC_print(&prm_errors_vec);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: diag_counter_get (section diagnostics)
 */
int
  ui_t20e_acc_diagnostics_diag_counter_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  T20E_DIAG_CNT_TYPE
    prm_counter_type_ndx;
  SOC_SAND_64CNT
    prm_counter_val;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_diagnostics");
  soc_sand_proc_name = "t20e_diag_counter_get";

  soc_sand_64cnt_clear(&prm_counter_val);

  unit = t20e_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_DIAG_COUNTER_GET_DIAG_COUNTER_GET_COUNTER_TYPE_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_T20E_DIAG_COUNTER_GET_DIAG_COUNTER_GET_COUNTER_TYPE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_counter_type_ndx = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter counter_type_ndx after diag_counter_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = t20e_diag_counter_get(
          unit,
          prm_counter_type_ndx,
          &prm_counter_val
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_diag_counter_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_diag_counter_get");
    goto exit;
  }

  send_string_to_screen("--> counter_val:", TRUE);
  soc_sand_64cnt_print(&prm_counter_val, FALSE);



  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: diag_all_counters_get (section diagnostics)
 */
int
  ui_t20e_acc_diagnostics_diag_all_counters_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_64CNT
    prm_counters[T20E_DIAG_NOF_COUNTERS];
  SOC_SAND_PRINT_FLAVORS
    prm_print_flavor = SOC_SAND_BIT(SOC_SAND_PRINT_FLAVORS_NO_ZEROS) | SOC_SAND_BIT(SOC_SAND_PRINT_FLAVORS_SHORT);

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_diagnostics");
  soc_sand_proc_name = "t20e_diag_all_counters_get";

  unit = t20e_get_default_unit();

  /* Get parameters */

  /* Call function */
  ret = t20e_diag_all_counters_get(
          unit,
          prm_counters
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_diag_all_counters_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_diag_all_counters_get");
    goto exit;
  }

  send_string_to_screen("--> counters:", TRUE);
  /* Call function */
  ret = t20e_diag_all_counters_print(
          prm_counters,
          prm_print_flavor
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_diag_all_counters_print - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_diag_all_counters_print");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: diag_all_mib_counters_print (section diagnostics)
 */
int
  ui_t20e_acc_diagnostics_diag_all_mib_counters_print(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    nif_ndx;
  T20E_DIAG_MIB_COUNTER
    prm_counters;
  SOC_SAND_PRINT_FLAVORS
    prm_print_flavor = SOC_SAND_BIT(SOC_SAND_PRINT_FLAVORS_NO_ZEROS);

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_diagnostics");
  soc_sand_proc_name = "t20e_diag_all_counters_get";

  unit = t20e_get_default_unit();

  /* Clear parameters */
  T20E_DIAG_MIB_COUNTER_clear(&prm_counters);

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_DIAG_ALL_MIB_COUNTERS_PRINT_DIAG_ALL_COUNTERS_PRINT_NIF_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_DIAG_ALL_MIB_COUNTERS_PRINT_DIAG_ALL_COUNTERS_PRINT_NIF_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    nif_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter nif_ndx after mib_counter_print***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = t20e_diag_all_mib_counters_get(
          unit,
          nif_ndx,
          &prm_counters
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_diag_all_mib_counters_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_diag_all_mib_counters_get");
    goto exit;
  }

  send_string_to_screen("--> counters:", TRUE);
  /* Call function */
  ret = t20e_diag_all_mib_counters_print(
          &prm_counters,
          prm_print_flavor
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_diag_all_mib_counters_print - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_diag_all_mib_counters_print");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: diag_mib_counter_print (section diagnostics)
 */
int
  ui_t20e_acc_diagnostics_diag_mib_counter_print(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    nif_ndx;
  T20E_DIAG_MIB_COUNTER_TYPE
    counter_type;
  uint32
    counter_val = 0;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_diagnostics");
  soc_sand_proc_name = "t20e_diag_all_counters_get";

  unit = t20e_get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_DIAG_MIB_COUNTER_PRINT_DIAG_ALL_COUNTERS_PRINT_NIF_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_DIAG_MIB_COUNTER_PRINT_DIAG_ALL_COUNTERS_PRINT_NIF_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    nif_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter nif_ndx after mib_counter_print***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_DIAG_MIB_COUNTER_PRINT_DIAG_ALL_COUNTERS_PRINT_COUNTER_TYPE_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_T20E_DIAG_MIB_COUNTER_PRINT_DIAG_ALL_COUNTERS_PRINT_COUNTER_TYPE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    counter_type = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter counter_type after nif_ndx***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = t20e_diag_mib_counter_get(
          unit,
          nif_ndx,
          counter_type,
          &counter_val
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_diag_mib_counter_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_diag_mib_counter_get");
    goto exit;
  }

  send_string_to_screen("--> counters:", TRUE);
  soc_sand_os_printf( "%s: %lu\r\n", T20E_DIAG_MIB_COUNTER_TYPE_to_string(counter_type), counter_val);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: diag_all_counters_print (section diagnostics)
 */
int
  ui_t20e_acc_diagnostics_diag_all_counters_print(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_counters_index = 0xFFFFFFFF;
  SOC_SAND_64CNT
    prm_counters[T20E_DIAG_NOF_COUNTERS];
  uint8
    print_zero = FALSE,
    print_long = FALSE;
  SOC_SAND_PRINT_FLAVORS
    prm_print_flavor=0;

  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_diagnostics");
  soc_sand_proc_name = "t20e_diag_all_counters_print";

  unit = t20e_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_DIAG_ALL_COUNTERS_PRINT_DIAG_ALL_COUNTERS_PRINT_COUNTERS_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_DIAG_ALL_COUNTERS_PRINT_DIAG_ALL_COUNTERS_PRINT_COUNTERS_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    print_zero = (uint8)param_val->value.ulong_value;
  }


  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_DIAG_ALL_COUNTERS_PRINT_DIAG_ALL_COUNTERS_PRINT_COUNTERS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_T20E_DIAG_ALL_COUNTERS_PRINT_DIAG_ALL_COUNTERS_PRINT_COUNTERS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    print_long = (uint8)param_val->value.ulong_value;
  }

  if (print_zero == FALSE)
  {
    prm_print_flavor = SOC_SAND_BIT(SOC_SAND_PRINT_FLAVORS_NO_ZEROS);
  }
  if (print_long == FALSE)
  {
    prm_print_flavor |= SOC_SAND_BIT(SOC_SAND_PRINT_FLAVORS_SHORT);
  }


  /* Get parameters */

  /* Call function */
  ret = t20e_diag_all_counters_get(
          unit,
          prm_counters
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_diag_all_counters_get - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_diag_all_counters_get");
    goto exit;
  }

  /* Call function */
  ret = t20e_diag_all_counters_print(
          prm_counters,
          prm_print_flavor
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** t20e_diag_all_counters_print - FAIL", TRUE);
    t20e_disp_result(ret, "t20e_diag_all_counters_print");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

#endif
#ifdef UI_T20E_ACC_PP/* { t20e_acc_pp*/
/********************************************************************
 *  Section handler: t20e_acc_pp
 */
int
  ui_t20e_acc_pp(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_pp");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_AC_ACCESS_ENTRY_ADD_UNSAFE_AC_ACCESS_ENTRY_ADD_UNSAFE_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_ac_access_entry_add_unsafe(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_AC_ACCESS_ENTRY_REMOVE_UNSAFE_AC_ACCESS_ENTRY_REMOVE_UNSAFE_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_ac_access_entry_remove_unsafe(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_AC_ACCESS_TBL_CLEAR_UNSAFE_AC_ACCESS_TBL_CLEAR_UNSAFE_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_ac_access_tbl_clear_unsafe(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_ac_access_internal_entry_add_unsafe(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_AC_ACCESS_GET_BLOCK_UNSAFE_AC_ACCESS_GET_BLOCK_UNSAFE_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_ac_access_get_block_unsafe(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_AC_ACCESS_HW_SET_CALLBACK_UNSAFE_AC_ACCESS_HW_SET_CALLBACK_UNSAFE_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_ac_access_hw_set_callback_unsafe(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_AC_ACCESS_HW_GET_CALLBACK_UNSAFE_AC_ACCESS_HW_GET_CALLBACK_UNSAFE_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_ac_access_hw_get_callback_unsafe(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LEM_DA_TBL_SET_PP_LEM_DA_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_lem_da_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LEM_DA_TBL_GET_PP_LEM_DA_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_lem_da_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LEM_SA_TBL_SET_PP_LEM_SA_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_lem_sa_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LEM_SA_TBL_GET_PP_LEM_SA_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_lem_sa_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_SET_REPS_FOR_TBL_UNSAFE_PP_SET_REPS_FOR_TBL_UNSAFE_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_set_reps_for_tbl_unsafe(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LLVP_CLASSIFICATION_TBL_SET_PP_LLVP_CLASSIFICATION_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_llvp_classification_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LLVP_CLASSIFICATION_TBL_GET_PP_LLVP_CLASSIFICATION_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_llvp_classification_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_IPV4_SUBNET_CAMI_TBL_SET_PP_IPV4_SUBNET_CAMI_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_ipv4_subnet_cami_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_IPV4_SUBNET_CAMI_TBL_GET_PP_IPV4_SUBNET_CAMI_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_ipv4_subnet_cami_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_IPV4_SUBNET_TABLE_TBL_SET_PP_IPV4_SUBNET_TABLE_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_ipv4_subnet_table_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_IPV4_SUBNET_TABLE_TBL_GET_PP_IPV4_SUBNET_TABLE_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_ipv4_subnet_table_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PORT_PROTOCOL_CAMI_TBL_SET_PP_PORT_PROTOCOL_CAMI_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_port_protocol_cami_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PORT_PROTOCOL_CAMI_TBL_GET_PP_PORT_PROTOCOL_CAMI_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_port_protocol_cami_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PORT_PROTOCOL_TO_VID_TBL_SET_PP_PORT_PROTOCOL_TO_VID_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_port_protocol_to_vid_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PORT_PROTOCOL_TO_VID_TBL_GET_PP_PORT_PROTOCOL_TO_VID_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_port_protocol_to_vid_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_VLAN_PORT_MEMBERSHIP_TBL_SET_PP_VLAN_PORT_MEMBERSHIP_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_vlan_port_membership_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_VLAN_PORT_MEMBERSHIP_TBL_GET_PP_VLAN_PORT_MEMBERSHIP_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_vlan_port_membership_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_RESERVED_MC_TBL_SET_PP_RESERVED_MC_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_reserved_mc_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_RESERVED_MC_TBL_GET_PP_RESERVED_MC_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_reserved_mc_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PCP_DECODING_TBL_SET_PP_PCP_DECODING_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_pcp_decoding_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PCP_DECODING_TBL_GET_PP_PCP_DECODING_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_pcp_decoding_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_TOS_TO_TC_TBL_SET_PP_TOS_TO_TC_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_tos_to_tc_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_TOS_TO_TC_TBL_GET_PP_TOS_TO_TC_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_tos_to_tc_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_SET_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_l4_port_ranges_to_traffic_class_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_GET_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_l4_port_ranges_to_traffic_class_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_SET_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_l4_port_ranges_to_traffic_class_tbl_parsed_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_GET_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_l4_port_ranges_to_traffic_class_tbl_parsed_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_DROP_PRECEDENCE_MAP_TC_TBL_SET_PP_DROP_PRECEDENCE_MAP_TC_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_drop_precedence_map_tc_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_DROP_PRECEDENCE_MAP_TC_TBL_GET_PP_DROP_PRECEDENCE_MAP_TC_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_drop_precedence_map_tc_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AC_OFFSET_TBL_SET_PP_AC_OFFSET_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_ac_offset_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AC_OFFSET_TBL_GET_PP_AC_OFFSET_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_ac_offset_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_COS_PROFILE_DECODING_TBL_SET_PP_COS_PROFILE_DECODING_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_cos_profile_decoding_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_COS_PROFILE_DECODING_TBL_GET_PP_COS_PROFILE_DECODING_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_cos_profile_decoding_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_COS_PROFILE_MAPPING_TBL_SET_PP_COS_PROFILE_MAPPING_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_cos_profile_mapping_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_COS_PROFILE_MAPPING_TBL_GET_PP_COS_PROFILE_MAPPING_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_cos_profile_mapping_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_STP_STATE_TBL_SET_PP_STP_STATE_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_stp_state_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_STP_STATE_TBL_GET_PP_STP_STATE_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_stp_state_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PCP_DEI_MAP_TBL_SET_PP_PCP_DEI_MAP_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_pcp_dei_map_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PCP_DEI_MAP_TBL_GET_PP_PCP_DEI_MAP_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_pcp_dei_map_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_IVEC_TBL_SET_PP_IVEC_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_ivec_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_IVEC_TBL_GET_PP_IVEC_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_ivec_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_ACTION_PROFILE_MPLS_VALUE_TBL_SET_PP_ACTION_PROFILE_MPLS_VALUE_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_action_profile_mpls_value_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_ACTION_PROFILE_MPLS_VALUE_TBL_GET_PP_ACTION_PROFILE_MPLS_VALUE_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_action_profile_mpls_value_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MPLS_COS_PROFILE_DECODING_TBL_SET_PP_MPLS_COS_PROFILE_DECODING_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_mpls_cos_profile_decoding_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MPLS_COS_PROFILE_DECODING_TBL_GET_PP_MPLS_COS_PROFILE_DECODING_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_mpls_cos_profile_decoding_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MPLS_COS_MAPPING_TBL_SET_PP_MPLS_COS_MAPPING_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_mpls_cos_mapping_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MPLS_COS_MAPPING_TBL_GET_PP_MPLS_COS_MAPPING_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_mpls_cos_mapping_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MPLS_TERMINATION_COS_MAPPING_TBL_SET_PP_MPLS_TERMINATION_COS_MAPPING_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_mpls_termination_cos_mapping_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MPLS_TERMINATION_COS_MAPPING_TBL_GET_PP_MPLS_TERMINATION_COS_MAPPING_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_mpls_termination_cos_mapping_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MPLS_TUNNEL_TBL_SET_PP_MPLS_TUNNEL_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_mpls_tunnel_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MPLS_TUNNEL_TBL_GET_PP_MPLS_TUNNEL_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_mpls_tunnel_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PROTECTION_INSTANCE_TBL_SET_PP_PROTECTION_INSTANCE_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_protection_instance_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PROTECTION_INSTANCE_TBL_GET_PP_PROTECTION_INSTANCE_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_protection_instance_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_DESTINATION_STATUS_TBL_SET_PP_DESTINATION_STATUS_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_destination_status_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_DESTINATION_STATUS_TBL_GET_PP_DESTINATION_STATUS_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_destination_status_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_port_config_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PORT_CONFIG_TBL_GET_PP_PORT_CONFIG_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_port_config_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MEP_LEVEL_PROFILE_TBL_SET_PP_MEP_LEVEL_PROFILE_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_mep_level_profile_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MEP_LEVEL_PROFILE_TBL_GET_PP_MEP_LEVEL_PROFILE_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_mep_level_profile_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_OAM_ACTION_TBL_SET_PP_OAM_ACTION_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_oam_action_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_OAM_ACTION_TBL_GET_PP_OAM_ACTION_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_oam_action_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_FORWARD_ACTION_PROFILE_TBL_SET_PP_FORWARD_ACTION_PROFILE_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_forward_action_profile_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_FORWARD_ACTION_PROFILE_TBL_GET_PP_FORWARD_ACTION_PROFILE_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_forward_action_profile_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_SNOOP_ACTION_PROFILE_TBL_SET_PP_SNOOP_ACTION_PROFILE_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_snoop_action_profile_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_SNOOP_ACTION_PROFILE_TBL_GET_PP_SNOOP_ACTION_PROFILE_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_snoop_action_profile_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_BURST_SIZE_TBL_SET_PP_BURST_SIZE_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_burst_size_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_BURST_SIZE_TBL_GET_PP_BURST_SIZE_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_burst_size_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MANTISSA_DIVISION_TBL_SET_PP_MANTISSA_DIVISION_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_mantissa_division_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MANTISSA_DIVISION_TBL_GET_PP_MANTISSA_DIVISION_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_mantissa_division_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_GLAG_RANGE_TBL_SET_PP_GLAG_RANGE_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_glag_range_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_GLAG_RANGE_TBL_GET_PP_GLAG_RANGE_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_glag_range_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MODULO_TBL_SET_PP_MODULO_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_modulo_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_MODULO_TBL_GET_PP_MODULO_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_modulo_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_GLAG_OUT_PORT_TBL_SET_PP_GLAG_OUT_PORT_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_glag_out_port_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_GLAG_OUT_PORT_TBL_GET_PP_GLAG_OUT_PORT_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_glag_out_port_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_VID_MIRR_CAMI_TBL_SET_PP_VID_MIRR_CAMI_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_vid_mirr_cami_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_VID_MIRR_CAMI_TBL_GET_PP_VID_MIRR_CAMI_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_vid_mirr_cami_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_VID_MIRR_PROFILE_TABLE_TBL_SET_PP_VID_MIRR_PROFILE_TABLE_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_vid_mirr_profile_table_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_VID_MIRR_PROFILE_TABLE_TBL_GET_PP_VID_MIRR_PROFILE_TABLE_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_vid_mirr_profile_table_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_output_port_config1_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_GET_PP_OUTPUT_PORT_CONFIG1_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_output_port_config1_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_ingress_vlan_edit_command_map_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_GET_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_ingress_vlan_edit_command_map_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_MPLS_PROFILE1_TBL_SET_PP_EGRESS_MPLS_PROFILE1_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_egress_mpls_profile1_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_MPLS_PROFILE1_TBL_GET_PP_EGRESS_MPLS_PROFILE1_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_egress_mpls_profile1_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_MPLS_PROFILE2_TBL_SET_PP_EGRESS_MPLS_PROFILE2_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_egress_mpls_profile2_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_MPLS_PROFILE2_TBL_GET_PP_EGRESS_MPLS_PROFILE2_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_egress_mpls_profile2_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_egress_encapsulation_link_layer_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_GET_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_egress_encapsulation_link_layer_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_OUT_VSI_TBL_SET_PP_EGRESS_OUT_VSI_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_egress_out_vsi_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_OUT_VSI_TBL_GET_PP_EGRESS_OUT_VSI_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_egress_out_vsi_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_LLVP_TABLE_TBL_SET_PP_EGRESS_LLVP_TABLE_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_egress_llvp_table_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_LLVP_TABLE_TBL_GET_PP_EGRESS_LLVP_TABLE_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_egress_llvp_table_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_egress_vlan_edit_command_map_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_GET_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_egress_vlan_edit_command_map_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_PCP_DEI_MAP_TBL_SET_PP_EGRESS_PCP_DEI_MAP_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_egress_pcp_dei_map_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_PCP_DEI_MAP_TBL_GET_PP_EGRESS_PCP_DEI_MAP_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_egress_pcp_dei_map_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_TC_DP_TO_UP_TBL_SET_PP_EGRESS_TC_DP_TO_UP_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_egress_tc_dp_to_up_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_TC_DP_TO_UP_TBL_GET_PP_EGRESS_TC_DP_TO_UP_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_egress_tc_dp_to_up_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_PORT_VLAN_MEMBERSHIP_TBL_SET_PP_EGRESS_PORT_VLAN_MEMBERSHIP_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_egress_port_vlan_membership_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_PORT_VLAN_MEMBERSHIP_TBL_GET_PP_EGRESS_PORT_VLAN_MEMBERSHIP_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_egress_port_vlan_membership_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_STP_STATE_TBL_SET_PP_EGRESS_STP_STATE_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_egress_stp_state_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_STP_STATE_TBL_GET_PP_EGRESS_STP_STATE_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_egress_stp_state_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_ACCEPTABLE_FRAME_TYPES_TBL_SET_PP_EGRESS_ACCEPTABLE_FRAME_TYPES_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_egress_acceptable_frame_types_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_ACCEPTABLE_FRAME_TYPES_TBL_GET_PP_EGRESS_ACCEPTABLE_FRAME_TYPES_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_egress_acceptable_frame_types_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_egress_mep_level_profile_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_GET_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_egress_mep_level_profile_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_OAM_ACTION_TBL_SET_PP_EGRESS_OAM_ACTION_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_egress_oam_action_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_OAM_ACTION_TBL_GET_PP_EGRESS_OAM_ACTION_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_egress_oam_action_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_ACTION_PROFILE_TBL_SET_PP_EGRESS_ACTION_PROFILE_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_egress_action_profile_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_ACTION_PROFILE_TBL_GET_PP_EGRESS_ACTION_PROFILE_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_egress_action_profile_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_SNOOP_PROFILE_TBL_SET_PP_EGRESS_SNOOP_PROFILE_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_egress_snoop_profile_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_SNOOP_PROFILE_TBL_GET_PP_EGRESS_SNOOP_PROFILE_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_egress_snoop_profile_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_FEC_LOW_TBL_SET_PP_FEC_LOW_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_fec_low_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_FEC_LOW_TBL_GET_PP_FEC_LOW_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_fec_low_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_FEC_HIGH_TBL_SET_PP_FEC_HIGH_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_fec_high_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_FEC_HIGH_TBL_GET_PP_FEC_HIGH_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_fec_high_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_egress_mpls_ac_table_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_GET_PP_EGRESS_MPLS_AC_TABLE_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_egress_mpls_ac_table_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_egress_ctag_translation_table_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_GET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_egress_ctag_translation_table_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_egress_ctag_translation_raw_table_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_GET_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_egress_ctag_translation_raw_table_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_egress_vlan_edit_and_membership_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_GET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_egress_vlan_edit_and_membership_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_egress_vlan_edit_and_membership_raw_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_GET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_egress_vlan_edit_and_membership_raw_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AC_LOW_TBL_SET_PP_AC_LOW_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_ac_low_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AC_LOW_TBL_GET_PP_AC_LOW_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_ac_low_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AC_HIGH_TBL_SET_PP_AC_HIGH_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_ac_high_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AC_HIGH_TBL_GET_PP_AC_HIGH_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_ac_high_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_ac_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AC_TBL_GET_PP_AC_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_ac_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PWE_HIGH_TBL_SET_PP_PWE_HIGH_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_pwe_high_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PWE_HIGH_TBL_GET_PP_PWE_HIGH_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_pwe_high_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PWE_LOW_TBL_SET_PP_PWE_LOW_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_pwe_low_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PWE_LOW_TBL_GET_PP_PWE_LOW_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_pwe_low_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_pwe_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PWE_TBL_GET_PP_PWE_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_pwe_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_aging_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_AGING_TBL_GET_PP_AGING_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_aging_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_CNT_INP_TBL_SET_PP_CNT_INP_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_cnt_inp_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_CNT_INP_TBL_GET_PP_CNT_INP_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_cnt_inp_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_COUNTERS_TBL_SET_PP_EGRESS_COUNTERS_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_egress_counters_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_COUNTERS_TBL_GET_PP_EGRESS_COUNTERS_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_egress_counters_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PLC_TBL_SET_PP_PLC_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_plc_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_PLC_TBL_GET_PP_PLC_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_plc_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_MPLS_TUNNEL_TBL_SET_PP_EGRESS_MPLS_TUNNEL_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_egress_mpls_tunnel_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_EGRESS_MPLS_TUNNEL_TBL_GET_PP_EGRESS_MPLS_TUNNEL_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_pp_egress_mpls_tunnel_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_AC_ACCESS_ENTRY_GET_UNSAFE_AC_ACCESS_ENTRY_GET_UNSAFE_ID,1))
  {
    ui_ret = ui_t20e_acc_pp_ac_access_entry_get_unsafe(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after t20e_acc_pp***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* { */t20e_acc_pp

#ifdef UI_T20E_ACC_MACT/* { t20e_acc_mact*/
/********************************************************************
 *  Section handler: t20e_acc_mact
 */
int
  ui_t20e_acc_mact(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_mact");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MACT_SET_REPS_FOR_TBL_UNSAFE_MACT_SET_REPS_FOR_TBL_UNSAFE_ID,1))
  {
    ui_ret = ui_t20e_acc_mact_mact_set_reps_for_tbl_unsafe(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LEM_MAC_DA_TBL_SET_T20E_PP_LEM_MAC_DA_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_mact_t20e_pp_lem_mac_da_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_PP_LEM_MAC_DA_TBL_GET_T20E_PP_LEM_MAC_DA_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_mact_t20e_pp_lem_mac_da_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MACT_FID_COUNTER_PROFILE_DB_TBL_SET_MACT_FID_COUNTER_PROFILE_DB_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_mact_mact_fid_counter_profile_db_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MACT_FID_COUNTER_PROFILE_DB_TBL_GET_MACT_FID_COUNTER_PROFILE_DB_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_mact_mact_fid_counter_profile_db_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MACT_FID_COUNTER_DB_TBL_SET_MACT_FID_COUNTER_DB_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_mact_mact_fid_counter_db_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MACT_FID_COUNTER_DB_TBL_GET_MACT_FID_COUNTER_DB_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_mact_mact_fid_counter_db_tbl_get(current_line);
  }
  /*
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MACT_FLUSH_DB_TBL_SET_MACT_FLUSH_DB_TBL_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_mact_mact_flush_db_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MACT_FLUSH_DB_TBL_GET_MACT_FLUSH_DB_TBL_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_mact_mact_flush_db_tbl_get(current_line);
  }
  */
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after t20e_acc_mact***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* { */t20e_acc_mact

#ifdef UI_T20E_REG_ACCESS/* { reg_access*/
/********************************************************************
 *  Section handler: reg_access
 */
int
  ui_t20e_acc_reg_access(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_reg_access");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_READ_FLD_READ_FLD_ID,1))
  {
    ui_ret = ui_t20e_acc_reg_access_read_fld(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_WRITE_FLD_WRITE_FLD_ID,1))
  {
    ui_ret = ui_t20e_acc_reg_access_write_fld(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_READ_REG_READ_REG_ID,1))
  {
    ui_ret = ui_t20e_acc_reg_access_read_reg(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_WRITE_REG_WRITE_REG_ID,1))
  {
    ui_ret = ui_t20e_acc_reg_access_write_reg(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_IREAD_ID,1))
  {
    ui_ret = ui_t20e_iread(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_IWRITE_ID,1))
  {
    ui_ret = ui_t20e_iwrite(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after reg_access***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif /* { reg_access*/
 #ifdef UI_T20E_HW_IF/* { hw_if*/
/********************************************************************
 *  Section handler: hw_if
 */
int
  ui_t20e_acc_hw_if(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_hw_if");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_HW_IF_CPU_INFO_SET_CPU_INFO_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_hw_if_cpu_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_HW_IF_CPU_INFO_GET_CPU_INFO_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_hw_if_cpu_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_HW_IF_DRAM_INFO_SET_DRAM_INFO_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_hw_if_dram_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_HW_IF_DRAM_INFO_GET_DRAM_INFO_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_hw_if_dram_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_HW_IF_QDR_INFO_SET_QDR_INFO_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_hw_if_qdr_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_HW_IF_QDR_INFO_GET_QDR_INFO_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_hw_if_qdr_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_HW_IF_XAUI_INFO_SET_XAUI_INFO_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_hw_if_xaui_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_HW_IF_XAUI_INFO_GET_XAUI_INFO_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_hw_if_xaui_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_HW_IF_SPAUI_INFO_SET_SPAUI_INFO_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_hw_if_spaui_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_HW_IF_SPAUI_INFO_GET_SPAUI_INFO_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_hw_if_spaui_info_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after hw_if***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* { */hw_if

#ifdef UI_T20E_MGMT/* { mgmt*/
/********************************************************************
 *  Section handler: mgmt
 */
int
  ui_t20e_acc_mgmt(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_mgmt");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_REGISTER_DEVICE_REGISTER_DEVICE_ID,1))
  {
    ui_ret = ui_t20e_acc_mgmt_register_device(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_UNREGISTER_DEVICE_UNREGISTER_DEVICE_ID,1))
  {
    ui_ret = ui_t20e_acc_mgmt_unregister_device(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MGMT_OPERATION_MODE_SET_OPERATION_MODE_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_mgmt_operation_mode_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MGMT_OPERATION_MODE_GET_OPERATION_MODE_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_mgmt_operation_mode_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_ID,1))
  {
    ui_ret = ui_t20e_acc_mgmt_init_sequence_phase1(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE2_INIT_SEQUENCE_PHASE2_ID,1))
  {
    ui_ret = ui_t20e_acc_mgmt_init_sequence_phase2(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MGMT_ENABLE_TRAFFIC_SET_ENABLE_TRAFFIC_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_mgmt_enable_traffic_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MGMT_ENABLE_TRAFFIC_GET_ENABLE_TRAFFIC_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_mgmt_enable_traffic_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MGMT_PCKT_SIZE_RANGE_SET_PCKT_SIZE_RANGE_SET_ID,1))
  {
    ui_ret = ui_t20e_acc_mgmt_pckt_size_range_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_MGMT_PCKT_SIZE_RANGE_GET_PCKT_SIZE_RANGE_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_mgmt_pckt_size_range_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after mgmt***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* { */mgmt

#ifdef UI_T20E_DIAGNOSTICS/* { diagnostics*/
/********************************************************************
 *  Section handler: diagnostics
 */
int
  ui_t20e_acc_diagnostics(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_t20e_acc_diagnostics");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_DIAG_ERRORS_GET_DIAG_ERRORS_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_diagnostics_diag_errors_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_DIAG_COUNTER_GET_DIAG_COUNTER_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_diagnostics_diag_counter_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_DIAG_ALL_COUNTERS_GET_DIAG_ALL_COUNTERS_GET_ID,1))
  {
    ui_ret = ui_t20e_acc_diagnostics_diag_all_counters_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_DIAG_ALL_COUNTERS_PRINT_DIAG_ALL_COUNTERS_PRINT_ID,1))
  {
    ui_ret = ui_t20e_acc_diagnostics_diag_all_counters_print(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_DIAG_ALL_MIB_COUNTERS_PRINT_DIAG_ALL_COUNTERS_PRINT_ID,1))
  {
    ui_ret = ui_t20e_acc_diagnostics_diag_all_mib_counters_print(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_DIAG_MIB_COUNTER_PRINT_DIAG_ALL_COUNTERS_PRINT_ID,1))
  {
    ui_ret = ui_t20e_acc_diagnostics_diag_mib_counter_print(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after diagnostics***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* { */diagnostics

/*****************************************************
*NAME
*  subject_t20e_acc
*TYPE: PROC
*DATE: 29/DEC/2002
*FUNCTION:
*  Process input line which has an 'subject_t20e_acc' subject.
*  Input line is assumed to have been checked and
*  found to be of right format.
*CALLING SEQUENCE:
*  subject_t20e_acc(current_line,current_line_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    CURRENT_LINE *current_line -
*      Pointer to prompt line to process.
*    CURRENT_LINE **current_line_ptr -
*      Pointer to prompt line to be displayed after
*      this procedure finishes execution. Caller
*      points this variable to the pointer to
*      the next line to display. If called function wishes
*      to set the next line to display, it replaces
*      the pointer to the next line to display.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then some error has occurred.
*  SOC_SAND_INDIRECT:
*    Processing results. See 'current_line_ptr'.
*REMARKS:
*  This procedure should be carried out under 'task_safe'
*  state (i.e., task can not be deleted while this
*  procedure is being carried out).
*SEE ALSO:
 */
int
  subject_t20e_acc(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  )
{
  unsigned int
    match_index;
  int
    ui_ret;
  char
    err_msg[80*8] = "" ;
  unsigned int
    ;
  char
    *proc_name ;


  proc_name = "subject_t20e_acc" ;
  ui_ret = FALSE ;
  unit = 0;


  /*
   * the rest of the line is empty
   */
  if (current_line->num_param_names == 0)
  {
    send_string_to_screen("\r\n",FALSE) ;
    send_string_to_screen("'subject_t20e_acc()' function was called with no parameters.\r\n",FALSE) ;
    goto exit ;
  }
  /*
   * Else, there are parameters on the line (not just 'subject_t20e_acc').
   */

  send_array_to_screen("\r\n",2) ;



  if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_ACC_PP_ID,1))
  {
    ui_ret = ui_t20e_acc_pp(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_ACC_MACT_ID,1))
  {
    ui_ret = ui_t20e_acc_mact(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_ACC_REG_ACCESS_ID,1))
  {
    ui_ret = ui_t20e_acc_reg_access(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_ACC_HW_IF_ID,1))
  {
    ui_ret = ui_t20e_acc_hw_if(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_ACC_MGMT_ID,1))
  {
    ui_ret = ui_t20e_acc_mgmt(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_T20E_ACC_DIAGNOSTICS_ID,1))
  {
    ui_ret = ui_t20e_acc_diagnostics(current_line);
  }
  else
  {
    /*
     * Enter if an unknown request.
     */
    send_string_to_screen(
      "\r\n"
      "*** t20e_acc command with unknown parameters'.\r\n"
      "    Syntax error/sw error...\r\n",
      TRUE)  ;
    ui_ret = TRUE  ;
    goto exit  ;
  }

exit:
  return (ui_ret);
}

