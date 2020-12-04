/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/* we need this junk function only to avoid building error of pedantic compilation */
void ___junk_function_ui_petra_pp_acc(void){
}

#if PETRA_PP
/*
 * General include file for reference design.
 */
#include <appl/diag/dpp/ref_sys.h>

/*
 * Utilities include file.
 */

#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_pp_mac.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>
#include <appl/diag/dpp/utils_defi.h>
#include <appl/dpp/UserInterface/ui_defi.h>


#include <appl/dpp/UserInterface/ui_pure_defi_petra_pp_acc.h>


#include <soc/dpp/Petra/PP/petra_pp_tbl_access.h>
#include <soc/dpp/Petra/PP/petra_pp_ipv4_lpm_mngr.h>
#include <soc/dpp/Petra/petra_framework.h>


extern uint32
  soc_petra_get_default_unit();

void
  soc_petra_pp_PETRA_PP_EPNI_ARP_TBL_DATA_clear(
    SOC_SAND_OUT SOC_PETRA_PP_EPNI_ARP_TBL_DATA *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_PP_EPNI_ARP_TBL_DATA));
  for (ind=0; ind<2; ++ind)
  {
    info->dmac[ind] = 0;
  }

exit:
  return;
}


void
  soc_petra_pp_PETRA_PP_EPNI_PTT_TBL_DATA_clear(
    SOC_SAND_OUT SOC_PETRA_PP_EPNI_PTT_TBL_DATA *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_PP_EPNI_PTT_TBL_DATA));
  for (ind=0; ind<3; ++ind)
  {
    info->transmit_tag_enable[ind] = 0;
  }

exit:
  return;
}



void
  soc_petra_pp_PETRA_PP_EPNI_ARP_TBL_DATA_print(
    SOC_SAND_IN SOC_PETRA_PP_EPNI_ARP_TBL_DATA *info
  )
{
  SOC_SAND_PP_MAC_ADDRESS
    mac_add_struct;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_pp_mac_address_long_to_struct(
    info->dmac,
    &(mac_add_struct)
  );

  soc_sand_SAND_PP_MAC_ADDRESS_print(&mac_add_struct);
exit:
  return;
}



void
  soc_petra_pp_PETRA_PP_EPNI_PTT_TBL_DATA_print(
    SOC_SAND_IN SOC_PETRA_PP_EPNI_PTT_TBL_DATA *info
  )
{
  uint32 ind=0 ;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind=0; ind<3; ++ind)
  {
    soc_sand_os_printf( "transmit_tag_enable[%u]: %lu\n",ind,info->transmit_tag_enable[ind]);
  }
exit:
  return;
}


void
  soc_petra_pp_PETRA_PP_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_DATA_clear(
    SOC_SAND_OUT SOC_PETRA_PP_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_DATA *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_PP_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_DATA));
  info->type = 0;
  info->stamp = 0;
  info->is_fid = 0;
  info->cid_or_fid = 0;
  for (ind=0; ind<2; ++ind)
  {
    info->mac[ind] = 0;
  }
  info->payload = 0;
  info->age_status = 0;
  info->refreshed_by_dsp = 0;
  info->self = 0;

exit:
  return;
}


void
  soc_petra_pp_PETRA_PP_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_DATA_BUFFER_clear(
    SOC_SAND_OUT SOC_PETRA_PP_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_DATA_BUFFER *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_PP_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_DATA_BUFFER));
  for (ind=0; ind<3; ++ind)
  {
    info->buffer[ind] = 0;
  }

exit:
  return;
}


void
  soc_petra_pp_PETRA_PP_IHP_PORT_INFO_TBL_DATA_clear(
    SOC_SAND_OUT SOC_PETRA_PP_IHP_PORT_INFO_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_PP_IHP_PORT_INFO_TBL_DATA));
  info->cvlan_tpid_valid = 0;
  info->svlan_tpid_valid = 0;
  info->port_map = 0;

exit:
  return;
}


void
  soc_petra_pp_PETRA_PP_IHP_ETH_PORT_INFO1_TBL_DATA_clear(
    SOC_SAND_OUT SOC_PETRA_PP_IHP_ETH_PORT_INFO1_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_PP_IHP_ETH_PORT_INFO1_TBL_DATA));
  info->port_type = 0;
  info->cid_subnet_enable = 0;
  info->cid_protocol_enable = 0;
  info->cep_pvid = 0;
  info->pvid = 0;
  info->cvlan_up = 0;
  info->da_not_found_fwd_action = 0;
  info->sa_not_found_fwd_action = 0;
  info->enable_tunneled_lb = 0;

exit:
  return;
}


void
  soc_petra_pp_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_DATA_clear(
    SOC_SAND_OUT SOC_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_DATA));
  info->cep_ingress_filter_enable = 0;
  info->cep_accept_tagged = 0;
  info->cep_accept_untagged_priority = 0;
  info->pep_egress_filter_enable = 0;
  info->pep_generate_priority_stag = 0;
  info->cep_port_member_of_cep_pvid = 0;
  info->cep_port_member_of_pep_pvid = 0;
  info->cep_port_member_of_others = 0;
  info->pep_port_member_of_cep_pvid = 0;
  info->pep_port_member_of_pep_pvid = 0;
  info->pep_port_member_of_others = 0;
  info->prog_port_default = 0;
  info->pep_pvid = 0;
  info->pep_tx_tagged = 0;
  info->use_dei = 0;
  info->pcp_decoding_table_index = 0;
  info->up_to_tc_table_index = 0;
  info->up_override = 0;
  info->tc_up_override = 0;
  info->tc_tos_enable = 0;
  info->tc_tos_index = 0;
  info->tc_l4_protocol_enable = 0;
  info->drop_precedence_port_select = 0;
  info->pep_dei = 0;
  info->acl_port = 0;
  info->accept_tagged = 0;
  info->accept_untagged_priority = 0;
  info->no_control = 0;
  info->no_intercept_or_snoop = 0;
  info->non_authorized_mode8021x = 0;
  info->enable_ipv4_routing = 0;
  info->enable_ipv6_routing = 0;

exit:
  return;
}


void
  soc_petra_pp_PETRA_PP_IHP_CID_INFO_TBL_DATA_clear(
    SOC_SAND_OUT SOC_PETRA_PP_IHP_CID_INFO_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_PP_IHP_CID_INFO_TBL_DATA));
  info->cid_default_tc = 0;
  info->da_not_found_uc_fwd_action = 0;
  info->da_not_found_mc_fwd_action = 0;
  info->ingress_filter_enable = 0;
  info->disable_uc_routing = 0;
  info->disable_mc_routing = 0;
  info->valid = 0;
  info->enable_l3_lag_hash = 0;

exit:
  return;
}


void
  soc_petra_pp_PETRA_PP_IHP_CID_TO_TOPOLOGY_TBL_DATA_clear(
    SOC_SAND_OUT SOC_PETRA_PP_IHP_CID_TO_TOPOLOGY_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_PP_IHP_CID_TO_TOPOLOGY_TBL_DATA));
  info->cid_to_topology = 0;

exit:
  return;
}


void
  soc_petra_pp_PETRA_PP_IHP_STP_TABLE_TBL_DATA_clear(
    SOC_SAND_OUT SOC_PETRA_PP_IHP_STP_TABLE_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_PP_IHP_STP_TABLE_TBL_DATA));
  info->stp_table = 0;

exit:
  return;
}


void
  soc_petra_pp_PETRA_PP_IHP_PORT_AND_PROTOCOL_TO_CID_INDEX_TBL_DATA_clear(
    SOC_SAND_OUT SOC_PETRA_PP_IHP_PORT_AND_PROTOCOL_TO_CID_INDEX_TBL_DATA *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_PP_IHP_PORT_AND_PROTOCOL_TO_CID_INDEX_TBL_DATA));
  for (ind=0; ind<2; ++ind)
  {
    info->port_and_protocol_to_cid_index[ind] = 0;
  }

exit:
  return;
}


void
  soc_petra_pp_PETRA_PP_IHP_CID_INDEX_TO_CID_TBL_DATA_clear(
    SOC_SAND_OUT SOC_PETRA_PP_IHP_CID_INDEX_TO_CID_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_PP_IHP_CID_INDEX_TO_CID_TBL_DATA));
  info->valid = 0;
  info->cid = 0;
  info->cvlan_up = 0;

exit:
  return;
}


void
  soc_petra_pp_PETRA_PP_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_DATA_clear(
    SOC_SAND_OUT SOC_PETRA_PP_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_PP_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_DATA));
  info->type = 0;
  info->destination_index = 0;
  info->cos_overwrite = 0;
  info->cos = 0;
  info->policer = 0;
  info->snoop = 0;

exit:
  return;
}


void
  soc_petra_pp_PETRA_PP_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_DATA_clear(
    SOC_SAND_OUT SOC_PETRA_PP_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_PP_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_DATA));
  info->type = 0;
  info->destination = 0;
  info->cos = 0;
  info->cos_overwrite = 0;
  info->cpu_code = 0;
  info->policer = 0;
  info->add_cid_to_dest = 0;

exit:
  return;
}


void
  soc_petra_pp_PETRA_PP_IHP_CID_PORT_MEMBERSHIP_TBL_DATA_clear(
    SOC_SAND_OUT SOC_PETRA_PP_IHP_CID_PORT_MEMBERSHIP_TBL_DATA *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_PP_IHP_CID_PORT_MEMBERSHIP_TBL_DATA));
  for (ind=0; ind<3; ++ind)
  {
    info->cid_port_membership[ind] = 0;
  }

exit:
  return;
}


void
  soc_petra_pp_PETRA_PP_IHP_IS_CID_SHARED_TBL_DATA_clear(
    SOC_SAND_OUT SOC_PETRA_PP_IHP_IS_CID_SHARED_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_PP_IHP_IS_CID_SHARED_TBL_DATA));
  info->is_cid_shared = 0;

exit:
  return;
}


void
  soc_petra_pp_PETRA_PP_IHP_TOS_TO_TC_TBL_DATA_clear(
    SOC_SAND_OUT SOC_PETRA_PP_IHP_TOS_TO_TC_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_PP_IHP_TOS_TO_TC_TBL_DATA));
  info->traffic_class = 0;
  info->valid = 0;

exit:
  return;
}


void
  soc_petra_pp_PETRA_PP_IHP_EVENT_FIFO_TBL_DATA_clear(
    SOC_SAND_OUT SOC_PETRA_PP_IHP_EVENT_FIFO_TBL_DATA *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_PP_IHP_EVENT_FIFO_TBL_DATA));
  info->type = 0;
  info->stamp = 0;
  for (ind=0; ind<2; ++ind)
  {
    info->mac[ind] = 0;
  }
  info->cid_or_fid = 0;
  info->is_fid = 0;
  info->destination = 0;
  info->da_fwd_type = 0;
  info->sa_drop = 0;
  info->fap_port = 0;

exit:
  return;
}


void
  soc_petra_pp_PETRA_PP_IHP_FEC_TABLE_TBL_DATA_clear(
    SOC_SAND_OUT SOC_PETRA_PP_IHP_FEC_TABLE_TBL_DATA *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_PP_IHP_FEC_TABLE_TBL_DATA));
  for (ind=0; ind<2; ++ind)
  {
    info->fec_table[ind] = 0;
  }

exit:
  return;
}


void
  soc_petra_pp_PETRA_PP_IHP_FEC_UC_TABLE_TBL_DATA_clear(
    SOC_SAND_OUT SOC_PETRA_PP_IHP_FEC_UC_TABLE_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_PP_IHP_FEC_UC_TABLE_TBL_DATA));
  info->command = 0;
  info->destination = 0;
  info->tos_based_cos = 0;
  info->counter_pointer = 0;
  info->out_vid = 0;
  info->arp_pointer = 0;
  info->cpu_code = 0;
  info->rpf_enable = 0;

exit:
  return;
}


void
  soc_petra_pp_PETRA_PP_IHP_FEC_MC_TABLE_TBL_DATA_clear(
    SOC_SAND_OUT SOC_PETRA_PP_IHP_FEC_MC_TABLE_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_PP_IHP_FEC_MC_TABLE_TBL_DATA));
  info->command = 0;
  info->destination = 0;
  info->tos_based_cos = 0;
  info->counter_pointer = 0;
  info->expected_in_vid = 0;
  info->expected_in_port = 0;
  info->cpu_code = 0;

exit:
  return;
}


void
  soc_petra_pp_PETRA_PP_IHP_FEC_PROG_PORT_TABLE_TBL_DATA_clear(
    SOC_SAND_OUT SOC_PETRA_PP_IHP_FEC_PROG_PORT_TABLE_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_PP_IHP_FEC_PROG_PORT_TABLE_TBL_DATA));
  info->snoop_command = 0;
  info->cos_key = 0;
  info->destination = 0;

exit:
  return;
}


void
  soc_petra_pp_PETRA_PP_IHP_SMOOTH_DIVISION_TBL_DATA_clear(
    SOC_SAND_OUT SOC_PETRA_PP_IHP_SMOOTH_DIVISION_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_PP_IHP_SMOOTH_DIVISION_TBL_DATA));
  info->smooth_division = 0;

exit:
  return;
}


void
  soc_petra_pp_PETRA_PP_IHP_LONGEST_PREFIX_MATCH_TBL_DATA_clear(
    SOC_SAND_OUT SOC_PETRA_PP_IHP_LONGEST_PREFIX_MATCH_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_PP_IHP_LONGEST_PREFIX_MATCH_TBL_DATA));
  info->longest_prefix_match = 0;

exit:
  return;
}


void
  soc_petra_pp_PETRA_PP_IHP_LONGEST_PREFIX_MATCH0_TBL_DATA_clear(
    SOC_SAND_OUT SOC_PETRA_PP_IHP_LONGEST_PREFIX_MATCH0_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_PP_IHP_LONGEST_PREFIX_MATCH0_TBL_DATA));
  info->longest_prefix_match0 = 0;

exit:
  return;
}


void
  soc_petra_pp_PETRA_PP_IHP_LONGEST_PREFIX_MATCH1_TBL_DATA_clear(
    SOC_SAND_OUT SOC_PETRA_PP_IHP_LONGEST_PREFIX_MATCH1_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_PP_IHP_LONGEST_PREFIX_MATCH1_TBL_DATA));
  info->longest_prefix_match1 = 0;

exit:
  return;
}


void
  soc_petra_pp_PETRA_PP_IHP_LONGEST_PREFIX_MATCH2_TBL_DATA_clear(
    SOC_SAND_OUT SOC_PETRA_PP_IHP_LONGEST_PREFIX_MATCH2_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_PP_IHP_LONGEST_PREFIX_MATCH2_TBL_DATA));
  info->longest_prefix_match2 = 0;

exit:
  return;
}

void
  soc_petra_pp_PETRA_PP_IHP_TCAM_BANK_TBL_DATA_clear(
    SOC_SAND_OUT SOC_PETRA_PP_IHP_TCAM_BANK_TBL_DATA *info
  )
{
  uint32
    indx;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_PP_IHP_TCAM_BANK_TBL_DATA));
  for (indx = 0 ; indx < SOC_PETRA_PP_IHP_TCAM_DATA_WIDTH; ++indx)
  {
    info->mask[indx] = 0xFFFFFFFF;
  }
  for (indx = 0 ; indx < SOC_PETRA_PP_IHP_TCAM_DATA_WIDTH; ++indx)
  {
    info->value[indx] = 0;
  }
exit:
  return;
}

void
soc_petra_pp_PETRA_PP_IHP_TCAM_COMPARE_DATA_clear(
  SOC_SAND_OUT SOC_PETRA_PP_IHP_TCAM_COMPARE_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_PP_IHP_TCAM_COMPARE_DATA));

exit:
  return;
}



void
  soc_petra_pp_PETRA_PP_IHP_TCAM_ACTION_BANK_A_TBL_DATA_clear(
    SOC_SAND_OUT SOC_PETRA_PP_IHP_TCAM_ACTION_BANK_A_TBL_DATA *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_PP_IHP_TCAM_ACTION_BANK_A_TBL_DATA));
  for (ind=0; ind<2; ++ind)
  {
    info->tcam_action_bank_a[ind] = 0;
  }

exit:
  return;
}


void
  soc_petra_pp_PETRA_PP_IHP_TCAM_ACTION_BANK_B_TBL_DATA_clear(
    SOC_SAND_OUT SOC_PETRA_PP_IHP_TCAM_ACTION_BANK_B_TBL_DATA *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_PP_IHP_TCAM_ACTION_BANK_B_TBL_DATA));
  for (ind=0; ind<2; ++ind)
  {
    info->tcam_action_bank_b[ind] = 0;
  }

exit:
  return;
}


void
  soc_petra_pp_PETRA_PP_IHP_TCAM_ACTION_BANK_C_TBL_DATA_clear(
    SOC_SAND_OUT SOC_PETRA_PP_IHP_TCAM_ACTION_BANK_C_TBL_DATA *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_PP_IHP_TCAM_ACTION_BANK_C_TBL_DATA));
  for (ind=0; ind<2; ++ind)
  {
    info->tcam_action_bank_c[ind] = 0;
  }

exit:
  return;
}


void
  soc_petra_pp_PETRA_PP_IHP_TCAM_ACTION_BANK_D_TBL_DATA_clear(
    SOC_SAND_OUT SOC_PETRA_PP_IHP_TCAM_ACTION_BANK_D_TBL_DATA *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_PP_IHP_TCAM_ACTION_BANK_D_TBL_DATA));
  for (ind=0; ind<2; ++ind)
  {
    info->tcam_action_bank_d[ind] = 0;
  }

exit:
  return;
}


void
  soc_petra_pp_PETRA_PP_IHP_TOS_TO_COS_TBL_DATA_clear(
    SOC_SAND_OUT SOC_PETRA_PP_IHP_TOS_TO_COS_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_PP_IHP_TOS_TO_COS_TBL_DATA));
  info->tos_to_cos = 0;

exit:
  return;
}





void
  soc_petra_pp_PETRA_PP_IHP_FEC_ACCESSED_TABLE_TBL_DATA_clear(
    SOC_SAND_OUT SOC_PETRA_PP_IHP_FEC_ACCESSED_TABLE_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_PP_IHP_FEC_ACCESSED_TABLE_TBL_DATA));
  info->fec_accessed_table[0] = 0;
  info->fec_accessed_table[1] = 0;
  info->fec_accessed_table[2] = 0;
  info->fec_accessed_table[3] = 0;

exit:
  return;
}


void
  soc_petra_pp_PETRA_PP_IHP_IPV4_STAT_TABLE_TBL_DATA_clear(
    SOC_SAND_OUT SOC_PETRA_PP_IHP_IPV4_STAT_TABLE_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_PP_IHP_IPV4_STAT_TABLE_TBL_DATA));
  info->counter_val = 0;

exit:
  return;
}


void
  soc_petra_pp_PETRA_PP_IHP_TCAM_BANK_A_TBL_DATA_clear(
    SOC_SAND_OUT SOC_PETRA_PP_IHP_TCAM_BANK_A_TBL_DATA *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_PP_IHP_TCAM_BANK_A_TBL_DATA));
  for (ind=0; ind<10; ++ind)
  {
    info->tcam_bank_a[ind] = 0;
  }

exit:
  return;
}


void
  soc_petra_pp_PETRA_PP_IHP_TCAM_BANK_B_TBL_DATA_clear(
    SOC_SAND_OUT SOC_PETRA_PP_IHP_TCAM_BANK_B_TBL_DATA *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_PP_IHP_TCAM_BANK_B_TBL_DATA));
  for (ind=0; ind<10; ++ind)
  {
    info->tcam_bank_b[ind] = 0;
  }

exit:
  return;
}


void
  soc_petra_pp_PETRA_PP_IHP_TCAM_BANK_C_TBL_DATA_clear(
    SOC_SAND_OUT SOC_PETRA_PP_IHP_TCAM_BANK_C_TBL_DATA *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_PP_IHP_TCAM_BANK_C_TBL_DATA));
  for (ind=0; ind<10; ++ind)
  {
    info->tcam_bank_c[ind] = 0;
  }

exit:
  return;
}


void
  soc_petra_pp_PETRA_PP_IHP_TCAM_BANK_D_TBL_DATA_clear(
    SOC_SAND_OUT SOC_PETRA_PP_IHP_TCAM_BANK_D_TBL_DATA *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_PP_IHP_TCAM_BANK_D_TBL_DATA));
  for (ind=0; ind<10; ++ind)
  {
    info->tcam_bank_d[ind] = 0;
  }

exit:
  return;
}


void
  soc_petra_pp_PETRA_PP_IHP_IPV6_TC_TO_COS_TBL_DATA_clear(
    SOC_SAND_OUT SOC_PETRA_PP_IHP_IPV6_TC_TO_COS_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_PP_IHP_IPV6_TC_TO_COS_TBL_DATA));
  info->ipv6_tc_to_cos = 0;

exit:
  return;
}


void
  soc_petra_pp_PETRA_PP_IHP_CID_SUBNET_TBL_DATA_clear(
    SOC_SAND_OUT SOC_PETRA_PP_IHP_CID_SUBNET_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_PP_IHP_CID_SUBNET_TBL_DATA));
  info->ip = 0;
  info->mask_minus_one = 0;
  info->cid = 0;
  info->traffic_class = 0;
  info->cvlan_up = 0;
  info->valid = 0;

exit:
  return;
}


void
  soc_petra_pp_PETRA_PP_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_DATA_clear(
    SOC_SAND_OUT SOC_PETRA_PP_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_PP_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_DATA));
  info->type = 0;
  info->destination = 0;
  info->cos = 0;
  info->cos_overwrite = 0;
  info->cpu_code = 0;
  info->policer = 0;
  info->add_cid_to_dest = 0;

exit:
  return;
}



void
  soc_petra_pp_PETRA_PP_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_DATA_print(
    SOC_SAND_IN SOC_PETRA_PP_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_DATA *info
  )
{
  uint32 ind=0 ;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "type: %lu\n",info->type);
  soc_sand_os_printf( "stamp: %lu\n",info->stamp);
  soc_sand_os_printf( "is_fid: %lu\n",info->is_fid);
  soc_sand_os_printf( "cid_or_fid: %lu\n",info->cid_or_fid);
  for (ind=0; ind<2; ++ind)
  {
    soc_sand_os_printf( "mac[%u]: %lu\n",ind,info->mac[ind]);
  }
  soc_sand_os_printf( "payload: %lu\n",info->payload);
  soc_sand_os_printf( "age_status: %lu\n",info->age_status);
  soc_sand_os_printf( "refreshed_by_dsp: %lu\n",info->refreshed_by_dsp);
  soc_sand_os_printf( "self: %lu\n",info->self);
exit:
  return;
}



void
  soc_petra_pp_PETRA_PP_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_DATA_BUFFER_print(
    SOC_SAND_IN SOC_PETRA_PP_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_DATA_BUFFER *info
  )
{
  uint32 ind=0 ;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind=0; ind<3; ++ind)
  {
    soc_sand_os_printf( "buffer[%u]: %lu\n",ind,info->buffer[ind]);
  }
exit:
  return;
}



void
  soc_petra_pp_PETRA_PP_IHP_PORT_INFO_TBL_DATA_print(
    SOC_SAND_IN SOC_PETRA_PP_IHP_PORT_INFO_TBL_DATA *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "cvlan_tpid_valid: %lu\n",info->cvlan_tpid_valid);
  soc_sand_os_printf( "svlan_tpid_valid: %lu\n",info->svlan_tpid_valid);
  soc_sand_os_printf( "port_map: %lu\n",info->port_map);
exit:
  return;
}



void
  soc_petra_pp_PETRA_PP_IHP_ETH_PORT_INFO1_TBL_DATA_print(
    SOC_SAND_IN SOC_PETRA_PP_IHP_ETH_PORT_INFO1_TBL_DATA *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "port_type: %lu\n",info->port_type);
  soc_sand_os_printf( "cid_subnet_enable: %lu\n",info->cid_subnet_enable);
  soc_sand_os_printf( "cid_protocol_enable: %lu\n",info->cid_protocol_enable);
  soc_sand_os_printf( "cep_pvid: %lu\n",info->cep_pvid);
  soc_sand_os_printf( "pvid: %lu\n",info->pvid);
  soc_sand_os_printf( "cvlan_up: %lu\n",info->cvlan_up);
  soc_sand_os_printf( "da_not_found_fwd_action: %lu\n",info->da_not_found_fwd_action);
  soc_sand_os_printf( "sa_not_found_fwd_action: %lu\n",info->sa_not_found_fwd_action);
  soc_sand_os_printf( "enable_tunneled_lb: %lu\n",info->enable_tunneled_lb);
exit:
  return;
}



void
  soc_petra_pp_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_DATA_print(
    SOC_SAND_IN SOC_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_DATA *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "cep_ingress_filter_enable: %lu\n",info->cep_ingress_filter_enable);
  soc_sand_os_printf( "cep_accept_tagged: %lu\n",info->cep_accept_tagged);
  soc_sand_os_printf( "cep_accept_untagged_priority: %lu\n",info->cep_accept_untagged_priority);
  soc_sand_os_printf( "pep_egress_filter_enable: %lu\n",info->pep_egress_filter_enable);
  soc_sand_os_printf( "pep_generate_priority_stag: %lu\n",info->pep_generate_priority_stag);
  soc_sand_os_printf( "cep_port_member_of_cep_pvid: %lu\n",info->cep_port_member_of_cep_pvid);
  soc_sand_os_printf( "cep_port_member_of_pep_pvid: %lu\n",info->cep_port_member_of_pep_pvid);
  soc_sand_os_printf( "cep_port_member_of_others: %lu\n",info->cep_port_member_of_others);
  soc_sand_os_printf( "pep_port_member_of_cep_pvid: %lu\n",info->pep_port_member_of_cep_pvid);
  soc_sand_os_printf( "pep_port_member_of_pep_pvid: %lu\n",info->pep_port_member_of_pep_pvid);
  soc_sand_os_printf( "pep_port_member_of_others: %lu\n",info->pep_port_member_of_others);
  soc_sand_os_printf( "prog_port_default: %lu\n",info->prog_port_default);
  soc_sand_os_printf( "pep_pvid: %lu\n",info->pep_pvid);
  soc_sand_os_printf( "pep_tx_tagged: %lu\n",info->pep_tx_tagged);
  soc_sand_os_printf( "use_dei: %lu\n",info->use_dei);
  soc_sand_os_printf( "pcp_decoding_table_index: %lu\n",info->pcp_decoding_table_index);
  soc_sand_os_printf( "up_to_tc_table_index: %lu\n",info->up_to_tc_table_index);
  soc_sand_os_printf( "up_override: %lu\n",info->up_override);
  soc_sand_os_printf( "tc_up_override: %lu\n",info->tc_up_override);
  soc_sand_os_printf( "tc_tos_enable: %lu\n",info->tc_tos_enable);
  soc_sand_os_printf( "tc_tos_index: %lu\n",info->tc_tos_index);
  soc_sand_os_printf( "tc_l4_protocol_enable: %lu\n",info->tc_l4_protocol_enable);
  soc_sand_os_printf( "drop_precedence_port_select: %lu\n",info->drop_precedence_port_select);
  soc_sand_os_printf( "pep_dei: %lu\n",info->pep_dei);
  soc_sand_os_printf( "acl_port: %lu\n",info->acl_port);
  soc_sand_os_printf( "accept_tagged: %lu\n",info->accept_tagged);
  soc_sand_os_printf( "accept_untagged_priority: %lu\n",info->accept_untagged_priority);
  soc_sand_os_printf( "no_control: %lu\n",info->no_control);
  soc_sand_os_printf( "no_intercept_or_snoop: %lu\n",info->no_intercept_or_snoop);
  soc_sand_os_printf( "non_authorized_mode8021x: %lu\n",info->non_authorized_mode8021x);
  soc_sand_os_printf( "enable_ipv4_routing: %lu\n",info->enable_ipv4_routing);
  soc_sand_os_printf( "enable_ipv6_routing: %lu\n",info->enable_ipv6_routing);
exit:
  return;
}



void
  soc_petra_pp_PETRA_PP_IHP_CID_INFO_TBL_DATA_print(
    SOC_SAND_IN SOC_PETRA_PP_IHP_CID_INFO_TBL_DATA *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "cid_default_tc: %lu\n",info->cid_default_tc);
  soc_sand_os_printf( "da_not_found_uc_fwd_action: %lu\n",info->da_not_found_uc_fwd_action);
  soc_sand_os_printf( "da_not_found_mc_fwd_action: %lu\n",info->da_not_found_mc_fwd_action);
  soc_sand_os_printf( "ingress_filter_enable: %lu\n",info->ingress_filter_enable);
  soc_sand_os_printf( "disable_uc_routing: %lu\n",info->disable_uc_routing);
  soc_sand_os_printf( "disable_mc_routing: %lu\n",info->disable_mc_routing);
  soc_sand_os_printf( "valid: %lu\n",info->valid);
  soc_sand_os_printf( "enable_l3_lag_hash: %lu\n",info->enable_l3_lag_hash);
exit:
  return;
}



void
  soc_petra_pp_PETRA_PP_IHP_CID_TO_TOPOLOGY_TBL_DATA_print(
    SOC_SAND_IN SOC_PETRA_PP_IHP_CID_TO_TOPOLOGY_TBL_DATA *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "cid_to_topology: %lu\n",info->cid_to_topology);
exit:
  return;
}



void
  soc_petra_pp_PETRA_PP_IHP_STP_TABLE_TBL_DATA_print(
    SOC_SAND_IN SOC_PETRA_PP_IHP_STP_TABLE_TBL_DATA *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "stp_table: %lu\n",info->stp_table);
exit:
  return;
}



void
  soc_petra_pp_PETRA_PP_IHP_PORT_AND_PROTOCOL_TO_CID_INDEX_TBL_DATA_print(
    SOC_SAND_IN SOC_PETRA_PP_IHP_PORT_AND_PROTOCOL_TO_CID_INDEX_TBL_DATA *info
  )
{
  uint32 ind=0 ;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind=0; ind<2; ++ind)
  {
    soc_sand_os_printf( "port_and_protocol_to_cid_index[%u]: %lu\n",ind,info->port_and_protocol_to_cid_index[ind]);
  }
exit:
  return;
}



void
  soc_petra_pp_PETRA_PP_IHP_CID_INDEX_TO_CID_TBL_DATA_print(
    SOC_SAND_IN SOC_PETRA_PP_IHP_CID_INDEX_TO_CID_TBL_DATA *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "valid: %lu\n",info->valid);
  soc_sand_os_printf( "cid: %lu\n",info->cid);
  soc_sand_os_printf( "cvlan_up: %lu\n",info->cvlan_up);
exit:
  return;
}



void
  soc_petra_pp_PETRA_PP_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_DATA_print(
    SOC_SAND_IN SOC_PETRA_PP_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_DATA *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "type: %lu\n",info->type);
  soc_sand_os_printf( "destination_index: %lu\n",info->destination_index);
  soc_sand_os_printf( "cos_overwrite: %lu\n",info->cos_overwrite);
  soc_sand_os_printf( "cos: %lu\n",info->cos);
  soc_sand_os_printf( "policer: %lu\n",info->policer);
  soc_sand_os_printf( "snoop: %lu\n",info->snoop);
exit:
  return;
}



void
  soc_petra_pp_PETRA_PP_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_DATA_print(
    SOC_SAND_IN SOC_PETRA_PP_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_DATA *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "type: %lu\n",info->type);
  soc_sand_os_printf( "destination: %lu\n",info->destination);
  soc_sand_os_printf( "cos: %lu\n",info->cos);
  soc_sand_os_printf( "cos_overwrite: %lu\n",info->cos_overwrite);
  soc_sand_os_printf( "cpu_code: %lu\n",info->cpu_code);
  soc_sand_os_printf( "policer: %lu\n",info->policer);
  soc_sand_os_printf( "add_cid_to_dest: %lu\n",info->add_cid_to_dest);
exit:
  return;
}



void
  soc_petra_pp_PETRA_PP_IHP_CID_PORT_MEMBERSHIP_TBL_DATA_print(
    SOC_SAND_IN SOC_PETRA_PP_IHP_CID_PORT_MEMBERSHIP_TBL_DATA *info
  )
{
  uint32 ind=0 ;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind=0; ind<3; ++ind)
  {
    soc_sand_os_printf( "cid_port_membership[%u]: %lu\n",ind,info->cid_port_membership[ind]);
  }
exit:
  return;
}



void
  soc_petra_pp_PETRA_PP_IHP_IS_CID_SHARED_TBL_DATA_print(
    SOC_SAND_IN SOC_PETRA_PP_IHP_IS_CID_SHARED_TBL_DATA *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "is_cid_shared: %lu\n",info->is_cid_shared);
exit:
  return;
}



void
  soc_petra_pp_PETRA_PP_IHP_TOS_TO_TC_TBL_DATA_print(
    SOC_SAND_IN SOC_PETRA_PP_IHP_TOS_TO_TC_TBL_DATA *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "traffic_class: %lu\n",info->traffic_class);
  soc_sand_os_printf( "valid: %lu\n",info->valid);
exit:
  return;
}



void
  soc_petra_pp_PETRA_PP_IHP_EVENT_FIFO_TBL_DATA_print(
    SOC_SAND_IN SOC_PETRA_PP_IHP_EVENT_FIFO_TBL_DATA *info
  )
{
  uint32 ind=0 ;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "type: %lu\n",info->type);
  soc_sand_os_printf( "stamp: %lu\n",info->stamp);
  for (ind=0; ind<2; ++ind)
  {
    soc_sand_os_printf( "mac[%u]: %lu\n",ind,info->mac[ind]);
  }
  soc_sand_os_printf( "cid_or_fid: %lu\n",info->cid_or_fid);
  soc_sand_os_printf( "is_fid: %lu\n",info->is_fid);
  soc_sand_os_printf( "destination: %lu\n",info->destination);
  soc_sand_os_printf( "da_fwd_type: %lu\n",info->da_fwd_type);
  soc_sand_os_printf( "sa_drop: %lu\n",info->sa_drop);
  soc_sand_os_printf( "fap_port: %lu\n",info->fap_port);
exit:
  return;
}



void
  soc_petra_pp_PETRA_PP_IHP_FEC_TABLE_TBL_DATA_print(
    SOC_SAND_IN SOC_PETRA_PP_IHP_FEC_TABLE_TBL_DATA *info
  )
{
  uint32 ind=0 ;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind=0; ind<2; ++ind)
  {
    soc_sand_os_printf( "fec_table[%u]: %lu\n",ind,info->fec_table[ind]);
  }
exit:
  return;
}



void
  soc_petra_pp_PETRA_PP_IHP_FEC_UC_TABLE_TBL_DATA_print(
    SOC_SAND_IN SOC_PETRA_PP_IHP_FEC_UC_TABLE_TBL_DATA *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "command: %lu\n",info->command);
  soc_sand_os_printf( "destination: %lu\n",info->destination);
  soc_sand_os_printf( "tos_based_cos: %lu\n",info->tos_based_cos);
  soc_sand_os_printf( "counter_pointer: %lu\n",info->counter_pointer);
  soc_sand_os_printf( "out_vid: %lu\n",info->out_vid);
  soc_sand_os_printf( "arp_pointer: %lu\n",info->arp_pointer);
  soc_sand_os_printf( "cpu_code: %lu\n",info->cpu_code);
  soc_sand_os_printf( "rpf_enable: %lu\n",info->rpf_enable);
exit:
  return;
}



void
  soc_petra_pp_PETRA_PP_IHP_FEC_MC_TABLE_TBL_DATA_print(
    SOC_SAND_IN SOC_PETRA_PP_IHP_FEC_MC_TABLE_TBL_DATA *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "command: %lu\n",info->command);
  soc_sand_os_printf( "destination: %lu\n",info->destination);
  soc_sand_os_printf( "tos_based_cos: %lu\n",info->tos_based_cos);
  soc_sand_os_printf( "counter_pointer: %lu\n",info->counter_pointer);
  soc_sand_os_printf( "expected_in_vid: %lu\n",info->expected_in_vid);
  soc_sand_os_printf( "expected_in_port: %lu\n",info->expected_in_port);
  soc_sand_os_printf( "cpu_code: %lu\n",info->cpu_code);
exit:
  return;
}



void
  soc_petra_pp_PETRA_PP_IHP_FEC_PROG_PORT_TABLE_TBL_DATA_print(
    SOC_SAND_IN SOC_PETRA_PP_IHP_FEC_PROG_PORT_TABLE_TBL_DATA *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "snoop_command: %lu\n",info->snoop_command);
  soc_sand_os_printf( "cos_key: %lu\n",info->cos_key);
  soc_sand_os_printf( "destination: %lu\n",info->destination);
exit:
  return;
}



void
  soc_petra_pp_PETRA_PP_IHP_SMOOTH_DIVISION_TBL_DATA_print(
    SOC_SAND_IN SOC_PETRA_PP_IHP_SMOOTH_DIVISION_TBL_DATA *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "smooth_division: %lu\n",info->smooth_division);
exit:
  return;
}



void
  soc_petra_pp_PETRA_PP_IHP_LONGEST_PREFIX_MATCH_TBL_DATA_print(
    SOC_SAND_IN SOC_PETRA_PP_IHP_LONGEST_PREFIX_MATCH_TBL_DATA *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "longest_prefix_match: %lu\n",info->longest_prefix_match);
exit:
  return;
}



void
  soc_petra_pp_PETRA_PP_IHP_LONGEST_PREFIX_MATCH0_TBL_DATA_print(
    SOC_SAND_IN SOC_PETRA_PP_IHP_LONGEST_PREFIX_MATCH0_TBL_DATA *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "longest_prefix_match0: %lu\n",info->longest_prefix_match0);

exit:
  return;
}



void
  soc_petra_pp_PETRA_PP_IHP_LONGEST_PREFIX_MATCH1_TBL_DATA_print(
    SOC_SAND_IN SOC_PETRA_PP_IHP_LONGEST_PREFIX_MATCH1_TBL_DATA *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "longest_prefix_match1: %lu\n",info->longest_prefix_match1);
exit:
  return;
}



void
  soc_petra_pp_PETRA_PP_IHP_LONGEST_PREFIX_MATCH2_TBL_DATA_print(
    SOC_SAND_IN SOC_PETRA_PP_IHP_LONGEST_PREFIX_MATCH2_TBL_DATA *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "longest_prefix_match2: %lu\n",info->longest_prefix_match2);
exit:
  return;
}

void
  soc_petra_pp_PETRA_PP_IHP_TCAM_BANK_TBL_DATA_print(
    SOC_SAND_IN SOC_PETRA_PP_IHP_TCAM_BANK_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);
  soc_sand_os_printf( "valid: %u\n",info->valid);
  if (!info->valid)
  {
    goto exit;
  }

  soc_sand_bitstream_print(info->mask, SOC_PETRA_PP_IHP_TCAM_DATA_WIDTH);
  soc_sand_bitstream_print(info->value, SOC_PETRA_PP_IHP_TCAM_DATA_WIDTH);
exit:
  return;
}

void
  soc_petra_pp_PETRA_PP_IHP_TCAM_COMPARE_DATA_print(
    SOC_SAND_IN SOC_PETRA_PP_IHP_TCAM_COMPARE_DATA *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "address: %lu\n",info->address);
  soc_sand_os_printf( "found: %u\n",info->found);

exit:
  return;
}

void
  soc_petra_pp_PETRA_PP_IHP_TCAM_ACTION_BANK_A_TBL_DATA_print(
    SOC_SAND_IN SOC_PETRA_PP_IHP_TCAM_ACTION_BANK_A_TBL_DATA *info
  )
{
  uint32 ind=0 ;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind=0; ind<2; ++ind)
  {
    soc_sand_os_printf( "tcam_action_bank_a[%u]: %lu\n",ind,info->tcam_action_bank_a[ind]);
  }
exit:
  return;
}



void
  soc_petra_pp_PETRA_PP_IHP_TCAM_ACTION_BANK_B_TBL_DATA_print(
    SOC_SAND_IN SOC_PETRA_PP_IHP_TCAM_ACTION_BANK_B_TBL_DATA *info
  )
{
  uint32 ind=0 ;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind=0; ind<2; ++ind)
  {
    soc_sand_os_printf( "tcam_action_bank_b[%u]: %lu\n",ind,info->tcam_action_bank_b[ind]);
  }
exit:
  return;
}



void
  soc_petra_pp_PETRA_PP_IHP_TCAM_ACTION_BANK_C_TBL_DATA_print(
    SOC_SAND_IN SOC_PETRA_PP_IHP_TCAM_ACTION_BANK_C_TBL_DATA *info
  )
{
  uint32 ind=0 ;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind=0; ind<2; ++ind)
  {
    soc_sand_os_printf( "tcam_action_bank_c[%u]: %lu\n",ind,info->tcam_action_bank_c[ind]);
  }
exit:
  return;
}



void
  soc_petra_pp_PETRA_PP_IHP_TCAM_ACTION_BANK_D_TBL_DATA_print(
    SOC_SAND_IN SOC_PETRA_PP_IHP_TCAM_ACTION_BANK_D_TBL_DATA *info
  )
{
  uint32 ind=0 ;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind=0; ind<2; ++ind)
  {
    soc_sand_os_printf( "tcam_action_bank_d[%u]: %lu\n",ind,info->tcam_action_bank_d[ind]);
  }
exit:
  return;
}



void
  soc_petra_pp_PETRA_PP_IHP_TOS_TO_COS_TBL_DATA_print(
    SOC_SAND_IN SOC_PETRA_PP_IHP_TOS_TO_COS_TBL_DATA *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "tos_to_cos: %lu\n",info->tos_to_cos);
exit:
  return;
}



void
  soc_petra_pp_PETRA_PP_IHP_FEC_ACCESSED_TABLE_TBL_DATA_print(
    SOC_SAND_IN SOC_PETRA_PP_IHP_FEC_ACCESSED_TABLE_TBL_DATA *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "fec_accessed_table[0]: %lu\n",info->fec_accessed_table[0]);
  soc_sand_os_printf( "fec_accessed_table[1]: %lu\n",info->fec_accessed_table[1]);
  soc_sand_os_printf( "fec_accessed_table[2]: %lu\n",info->fec_accessed_table[2]);
  soc_sand_os_printf( "fec_accessed_table[3]: %lu\n",info->fec_accessed_table[3]);
exit:
  return;
}



void
  soc_petra_pp_PETRA_PP_IHP_IPV4_STAT_TABLE_TBL_DATA_print(
    SOC_SAND_IN SOC_PETRA_PP_IHP_IPV4_STAT_TABLE_TBL_DATA *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "counter_val: %lu\n",info->counter_val);
exit:
  return;
}



void
  soc_petra_pp_PETRA_PP_IHP_TCAM_BANK_A_TBL_DATA_print(
    SOC_SAND_IN SOC_PETRA_PP_IHP_TCAM_BANK_A_TBL_DATA *info
  )
{
  uint32 ind=0 ;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind=0; ind<10; ++ind)
  {
    soc_sand_os_printf( "tcam_bank_a[%u]: %lu\n",ind,info->tcam_bank_a[ind]);
  }
exit:
  return;
}



void
  soc_petra_pp_PETRA_PP_IHP_TCAM_BANK_B_TBL_DATA_print(
    SOC_SAND_IN SOC_PETRA_PP_IHP_TCAM_BANK_B_TBL_DATA *info
  )
{
  uint32 ind=0 ;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind=0; ind<10; ++ind)
  {
    soc_sand_os_printf( "tcam_bank_b[%u]: %lu\n",ind,info->tcam_bank_b[ind]);
  }
exit:
  return;
}



void
  soc_petra_pp_PETRA_PP_IHP_TCAM_BANK_C_TBL_DATA_print(
    SOC_SAND_IN SOC_PETRA_PP_IHP_TCAM_BANK_C_TBL_DATA *info
  )
{
  uint32 ind=0 ;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind=0; ind<10; ++ind)
  {
    soc_sand_os_printf( "tcam_bank_c[%u]: %lu\n",ind,info->tcam_bank_c[ind]);
  }
exit:
  return;
}



void
  soc_petra_pp_PETRA_PP_IHP_TCAM_BANK_D_TBL_DATA_print(
    SOC_SAND_IN SOC_PETRA_PP_IHP_TCAM_BANK_D_TBL_DATA *info
  )
{
  uint32 ind=0 ;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind=0; ind<10; ++ind)
  {
    soc_sand_os_printf( "tcam_bank_d[%u]: %lu\n",ind,info->tcam_bank_d[ind]);
  }
exit:
  return;
}



void
  soc_petra_pp_PETRA_PP_IHP_IPV6_TC_TO_COS_TBL_DATA_print(
    SOC_SAND_IN SOC_PETRA_PP_IHP_IPV6_TC_TO_COS_TBL_DATA *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "ipv6_tc_to_cos: %lu\n",info->ipv6_tc_to_cos);
exit:
  return;
}



void
  soc_petra_pp_PETRA_PP_IHP_CID_SUBNET_TBL_DATA_print(
    SOC_SAND_IN SOC_PETRA_PP_IHP_CID_SUBNET_TBL_DATA *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "ip: %lu\n",info->ip);
  soc_sand_os_printf( "mask_minus_one: %lu\n",info->mask_minus_one);
  soc_sand_os_printf( "cid: %lu\n",info->cid);
  soc_sand_os_printf( "traffic_class: %lu\n",info->traffic_class);
  soc_sand_os_printf( "cvlan_up: %lu\n",info->cvlan_up);
  soc_sand_os_printf( "valid: %lu\n",info->valid);
exit:
  return;
}



void
  soc_petra_pp_PETRA_PP_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_DATA_print(
    SOC_SAND_IN SOC_PETRA_PP_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_DATA *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "type: %lu\n",info->type);
  soc_sand_os_printf( "destination: %lu\n",info->destination);
  soc_sand_os_printf( "cos: %lu\n",info->cos);
  soc_sand_os_printf( "cos_overwrite: %lu\n",info->cos_overwrite);
  soc_sand_os_printf( "cpu_code: %lu\n",info->cpu_code);
  soc_sand_os_printf( "policer: %lu\n",info->policer);
  soc_sand_os_printf( "add_cid_to_dest: %lu\n",info->add_cid_to_dest);
exit:
  return;
}


void
  soc_petra_pp_PETRA_PP_EGQ_PCT_TBL_DATA_clear(
    SOC_SAND_OUT SOC_PETRA_PP_EGQ_PCT_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_PP_EGQ_PCT_TBL_DATA));
  info->sa_lsb = 0;
  info->port_pcp_encode_select = 0;
  info->cep_tx_tagged_other = 0;
  info->cep_tx_tagged_pep_pvid = 0;
  info->cep_tx_tagged_cep_pvid = 0;
  info->pep_port_default_priority = 0;
  info->pep_pvid = 0;
  info->cep_pvid = 0;
  info->ethernet_bridge_port_type = 0;

exit:
  return;
}


void
  soc_petra_pp_PETRA_PP_EGQ_PPCT_TBL_DATA_clear(
    SOC_SAND_OUT SOC_PETRA_PP_EGQ_PPCT_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_PP_EGQ_PPCT_TBL_DATA));
  info->unknown_bc_da_action_filter = 0;
  info->unknown_mc_da_action_filter = 0;
  info->unknown_uc_da_action_filter = 0;
  info->mtu = 0;
  info->mtu_violation_action = 0;
  info->pep_port_all_others_member = 0;
  info->pep_port_pep_pvid_member = 0;
  info->pep_enable_ingress_filtering = 0;
  info->pep_acceptable_frame_types = 0;
  info->pep_pvid = 0;
  info->cep_port_all_others_member = 0;
  info->cep_port_cep_pvid_member = 0;
  info->cep_enable_egress_filtering = 0;
  info->cep_pvid = 0;
  info->egress_vlan_filtering_enable = 0;
  info->ethernet_bridge_port_type = 0;

exit:
  return;
}


void
  soc_petra_pp_PETRA_PP_EGQ_STP_TBL_DATA_clear(
    SOC_SAND_OUT SOC_PETRA_PP_EGQ_STP_TBL_DATA *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_PP_EGQ_STP_TBL_DATA));
  for (ind=0; ind<2; ++ind)
  {
    info->egress_stp_state[ind] = 0;
  }

exit:
  return;
}


void
  soc_petra_pp_PETRA_PP_EGQ_PCT_TBL_DATA_print(
    SOC_SAND_IN SOC_PETRA_PP_EGQ_PCT_TBL_DATA *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "sa_lsb: %lu\n",info->sa_lsb);
  soc_sand_os_printf( "port_pcp_encode_select: %lu\n",info->port_pcp_encode_select);
  soc_sand_os_printf( "cep_tx_tagged_other: %lu\n",info->cep_tx_tagged_other);
  soc_sand_os_printf( "cep_tx_tagged_pep_pvid: %lu\n",info->cep_tx_tagged_pep_pvid);
  soc_sand_os_printf( "cep_tx_tagged_cep_pvid: %lu\n",info->cep_tx_tagged_cep_pvid);
  soc_sand_os_printf( "pep_port_default_priority: %lu\n",info->pep_port_default_priority);
  soc_sand_os_printf( "pep_pvid: %lu\n",info->pep_pvid);
  soc_sand_os_printf( "cep_pvid: %lu\n",info->cep_pvid);
  soc_sand_os_printf( "ethernet_bridge_port_type: %lu\n",info->ethernet_bridge_port_type);
exit:
  return;
}



void
  soc_petra_pp_PETRA_PP_EGQ_PPCT_TBL_DATA_print(
    SOC_SAND_IN SOC_PETRA_PP_EGQ_PPCT_TBL_DATA *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf( "unknown_bc_da_action_filter: %lu\n",info->unknown_bc_da_action_filter);
  soc_sand_os_printf( "unknown_mc_da_action_filter: %lu\n",info->unknown_mc_da_action_filter);
  soc_sand_os_printf( "unknown_uc_da_action_filter: %lu\n",info->unknown_uc_da_action_filter);
  soc_sand_os_printf( "mtu: %lu\n",info->mtu);
  soc_sand_os_printf( "mtu_violation_action: %lu\n",info->mtu_violation_action);
  soc_sand_os_printf( "pep_port_all_others_member: %lu\n",info->pep_port_all_others_member);
  soc_sand_os_printf( "pep_port_pep_pvid_member: %lu\n",info->pep_port_pep_pvid_member);
  soc_sand_os_printf( "pep_enable_ingress_filtering: %lu\n",info->pep_enable_ingress_filtering);
  soc_sand_os_printf( "pep_acceptable_frame_types: %lu\n",info->pep_acceptable_frame_types);
  soc_sand_os_printf( "pep_pvid: %lu\n",info->pep_pvid);
  soc_sand_os_printf( "cep_port_all_others_member: %lu\n",info->cep_port_all_others_member);
  soc_sand_os_printf( "cep_port_cep_pvid_member: %lu\n",info->cep_port_cep_pvid_member);
  soc_sand_os_printf( "cep_enable_egress_filtering: %lu\n",info->cep_enable_egress_filtering);
  soc_sand_os_printf( "cep_pvid: %lu\n",info->cep_pvid);
  soc_sand_os_printf( "egress_vlan_filtering_enable: %lu\n",info->egress_vlan_filtering_enable);
  soc_sand_os_printf( "ethernet_bridge_port_type: %lu\n",info->ethernet_bridge_port_type);
exit:
  return;
}



void
  soc_petra_pp_PETRA_PP_EGQ_STP_TBL_DATA_print(
    SOC_SAND_IN SOC_PETRA_PP_EGQ_STP_TBL_DATA *info
  )
{
  uint32 ind=0 ;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind=0; ind<2; ++ind)
  {
    soc_sand_os_printf( "egress_stp_state[%u]: %lu\n",ind,info->egress_stp_state[ind]);
  }
exit:
  return;
}


/********************************************************************
 *  Function handler: ihp_management_requests_fifo_tbl_set (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_management_requests_fifo_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_DATA
    prm_IHP_management_requests_fifo_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_management_requests_fifo_tbl_set";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_DATA_clear(&prm_IHP_management_requests_fifo_tbl_data);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_petra_pp_ihp_management_requests_fifo_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_management_requests_fifo_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_management_requests_fifo_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_management_requests_fifo_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_DATA_SELF_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_DATA_SELF_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_management_requests_fifo_tbl_data.self = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_DATA_REFRESHED_BY_DSP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_DATA_REFRESHED_BY_DSP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_management_requests_fifo_tbl_data.refreshed_by_dsp = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_DATA_AGE_STATUS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_DATA_AGE_STATUS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_management_requests_fifo_tbl_data.age_status = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_DATA_PAYLOAD_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_DATA_PAYLOAD_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_management_requests_fifo_tbl_data.payload = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_DATA_MAC_0_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_DATA_MAC_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_management_requests_fifo_tbl_data.mac[0] = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_DATA_CID_OR_FID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_DATA_CID_OR_FID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_management_requests_fifo_tbl_data.cid_or_fid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_DATA_IS_FID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_DATA_IS_FID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_management_requests_fifo_tbl_data.is_fid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_DATA_STAMP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_DATA_STAMP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_management_requests_fifo_tbl_data.stamp = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_DATA_TYPE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_DATA_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_management_requests_fifo_tbl_data.type = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = soc_petra_pp_ihp_management_requests_fifo_tbl_set_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_management_requests_fifo_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_management_requests_fifo_tbl_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_management_requests_fifo_tbl_set");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_management_requests_fifo_tbl_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_management_requests_fifo_tbl_get (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_management_requests_fifo_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_DATA
    prm_IHP_management_requests_fifo_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_management_requests_fifo_tbl_get";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_GET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_GET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_DATA_clear(&prm_IHP_management_requests_fifo_tbl_data);

  /* Get parameters */

  /* Call function */
  ret = soc_petra_pp_ihp_management_requests_fifo_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_management_requests_fifo_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_management_requests_fifo_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_management_requests_fifo_tbl_get");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_management_requests_fifo_tbl_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> IHP_management_requests_fifo_tbl_data:", TRUE);
  soc_petra_pp_PETRA_PP_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_DATA_print(&prm_IHP_management_requests_fifo_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_port_info_tbl_set (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_port_info_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_PORT_INFO_TBL_DATA
    prm_IHP_port_info_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_port_info_tbl_set";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_PORT_INFO_TBL_SET_IHP_PORT_INFO_TBL_SET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_PORT_INFO_TBL_DATA_clear(&prm_IHP_port_info_tbl_data);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_petra_pp_ihp_port_info_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_port_info_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_port_info_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_port_info_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_PORT_INFO_TBL_SET_IHP_PORT_INFO_TBL_SET_IHP_PORT_INFO_TBL_DATA_PORT_MAP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_PORT_INFO_TBL_SET_IHP_PORT_INFO_TBL_SET_IHP_PORT_INFO_TBL_DATA_PORT_MAP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_port_info_tbl_data.port_map = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_PORT_INFO_TBL_SET_IHP_PORT_INFO_TBL_SET_IHP_PORT_INFO_TBL_DATA_SVLAN_TPID_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_PORT_INFO_TBL_SET_IHP_PORT_INFO_TBL_SET_IHP_PORT_INFO_TBL_DATA_SVLAN_TPID_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_port_info_tbl_data.svlan_tpid_valid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_PORT_INFO_TBL_SET_IHP_PORT_INFO_TBL_SET_IHP_PORT_INFO_TBL_DATA_CVLAN_TPID_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_PORT_INFO_TBL_SET_IHP_PORT_INFO_TBL_SET_IHP_PORT_INFO_TBL_DATA_CVLAN_TPID_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_port_info_tbl_data.cvlan_tpid_valid = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = soc_petra_pp_ihp_port_info_tbl_set_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_port_info_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_port_info_tbl_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_port_info_tbl_set");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_port_info_tbl_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_port_info_tbl_get (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_port_info_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_PORT_INFO_TBL_DATA
    prm_IHP_port_info_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_port_info_tbl_get";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_PORT_INFO_TBL_GET_IHP_PORT_INFO_TBL_GET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_PORT_INFO_TBL_DATA_clear(&prm_IHP_port_info_tbl_data);

  /* Get parameters */

  /* Call function */
  ret = soc_petra_pp_ihp_port_info_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_port_info_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_port_info_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_port_info_tbl_get");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_port_info_tbl_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> IHP_port_info_tbl_data:", TRUE);
  soc_petra_pp_PETRA_PP_IHP_PORT_INFO_TBL_DATA_print(&prm_IHP_port_info_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_eth_port_info1_tbl_set (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_eth_port_info1_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_ETH_PORT_INFO1_TBL_DATA
    prm_IHP_eth_port_info1_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_eth_port_info1_tbl_set";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_SET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_ETH_PORT_INFO1_TBL_DATA_clear(&prm_IHP_eth_port_info1_tbl_data);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_petra_pp_ihp_eth_port_info1_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_eth_port_info1_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_eth_port_info1_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_eth_port_info1_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_DATA_ENABLE_TUNNELED_LB_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_DATA_ENABLE_TUNNELED_LB_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_eth_port_info1_tbl_data.enable_tunneled_lb = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_DATA_SA_NOT_FOUND_FWD_ACTION_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_DATA_SA_NOT_FOUND_FWD_ACTION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_eth_port_info1_tbl_data.sa_not_found_fwd_action = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_DATA_DA_NOT_FOUND_FWD_ACTION_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_DATA_DA_NOT_FOUND_FWD_ACTION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_eth_port_info1_tbl_data.da_not_found_fwd_action = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_DATA_CVLAN_UP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_DATA_CVLAN_UP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_eth_port_info1_tbl_data.cvlan_up = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_DATA_PVID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_DATA_PVID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_eth_port_info1_tbl_data.pvid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_DATA_CEP_PVID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_DATA_CEP_PVID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_eth_port_info1_tbl_data.cep_pvid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_DATA_CID_PROTOCOL_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_DATA_CID_PROTOCOL_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_eth_port_info1_tbl_data.cid_protocol_enable = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_DATA_CID_SUBNET_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_DATA_CID_SUBNET_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_eth_port_info1_tbl_data.cid_subnet_enable = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_DATA_PORT_TYPE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_DATA_PORT_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_eth_port_info1_tbl_data.port_type = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = soc_petra_pp_ihp_eth_port_info1_tbl_set_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_eth_port_info1_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_eth_port_info1_tbl_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_eth_port_info1_tbl_set");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_eth_port_info1_tbl_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_eth_port_info1_tbl_get (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_eth_port_info1_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_ETH_PORT_INFO1_TBL_DATA
    prm_IHP_eth_port_info1_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_eth_port_info1_tbl_get";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO1_TBL_GET_IHP_ETH_PORT_INFO1_TBL_GET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_ETH_PORT_INFO1_TBL_DATA_clear(&prm_IHP_eth_port_info1_tbl_data);

  /* Get parameters */

  /* Call function */
  ret = soc_petra_pp_ihp_eth_port_info1_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_eth_port_info1_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_eth_port_info1_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_eth_port_info1_tbl_get");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_eth_port_info1_tbl_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> IHP_eth_port_info1_tbl_data:", TRUE);
  soc_petra_pp_PETRA_PP_IHP_ETH_PORT_INFO1_TBL_DATA_print(&prm_IHP_eth_port_info1_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_eth_port_info2_tbl_set (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_eth_port_info2_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_DATA
    prm_IHP_eth_port_info2_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_eth_port_info2_tbl_set";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_DATA_clear(&prm_IHP_eth_port_info2_tbl_data);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_petra_pp_ihp_eth_port_info2_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_eth_port_info2_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_eth_port_info2_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_eth_port_info2_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_ENABLE_IPV6_ROUTING_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_ENABLE_IPV6_ROUTING_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_eth_port_info2_tbl_data.enable_ipv6_routing = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_ENABLE_IPV4_ROUTING_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_ENABLE_IPV4_ROUTING_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_eth_port_info2_tbl_data.enable_ipv4_routing = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_NON_AUTHORIZED_MODE8021X_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_NON_AUTHORIZED_MODE8021X_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_eth_port_info2_tbl_data.non_authorized_mode8021x = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_NO_INTERCEPT_OR_SNOOP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_NO_INTERCEPT_OR_SNOOP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_eth_port_info2_tbl_data.no_intercept_or_snoop = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_NO_CONTROL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_NO_CONTROL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_eth_port_info2_tbl_data.no_control = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_ACCEPT_UNTAGGED_PRIORITY_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_ACCEPT_UNTAGGED_PRIORITY_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_eth_port_info2_tbl_data.accept_untagged_priority = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_ACCEPT_TAGGED_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_ACCEPT_TAGGED_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_eth_port_info2_tbl_data.accept_tagged = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_ACL_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_ACL_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_eth_port_info2_tbl_data.acl_port = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_PEP_DEI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_PEP_DEI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_eth_port_info2_tbl_data.pep_dei = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_DROP_PRECEDENCE_PORT_SELECT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_DROP_PRECEDENCE_PORT_SELECT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_eth_port_info2_tbl_data.drop_precedence_port_select = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_TC_L4_PROTOCOL_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_TC_L4_PROTOCOL_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_eth_port_info2_tbl_data.tc_l4_protocol_enable = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_TC_TOS_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_TC_TOS_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_eth_port_info2_tbl_data.tc_tos_index = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_TC_TOS_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_TC_TOS_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_eth_port_info2_tbl_data.tc_tos_enable = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_TC_UP_OVERRIDE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_TC_UP_OVERRIDE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_eth_port_info2_tbl_data.tc_up_override = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_UP_OVERRIDE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_UP_OVERRIDE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_eth_port_info2_tbl_data.up_override = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_UP_TO_TC_TABLE_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_UP_TO_TC_TABLE_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_eth_port_info2_tbl_data.up_to_tc_table_index = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_PCP_DECODING_TABLE_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_PCP_DECODING_TABLE_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_eth_port_info2_tbl_data.pcp_decoding_table_index = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_USE_DEI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_USE_DEI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_eth_port_info2_tbl_data.use_dei = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_PEP_TX_TAGGED_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_PEP_TX_TAGGED_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_eth_port_info2_tbl_data.pep_tx_tagged = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_PEP_PVID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_PEP_PVID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_eth_port_info2_tbl_data.pep_pvid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_PROG_PORT_DEFAULT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_PROG_PORT_DEFAULT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_eth_port_info2_tbl_data.prog_port_default = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_PEP_PORT_MEMBER_OF_OTHERS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_PEP_PORT_MEMBER_OF_OTHERS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_eth_port_info2_tbl_data.pep_port_member_of_others = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_PEP_PORT_MEMBER_OF_PEP_PVID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_PEP_PORT_MEMBER_OF_PEP_PVID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_eth_port_info2_tbl_data.pep_port_member_of_pep_pvid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_PEP_PORT_MEMBER_OF_CEP_PVID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_PEP_PORT_MEMBER_OF_CEP_PVID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_eth_port_info2_tbl_data.pep_port_member_of_cep_pvid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_CEP_PORT_MEMBER_OF_OTHERS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_CEP_PORT_MEMBER_OF_OTHERS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_eth_port_info2_tbl_data.cep_port_member_of_others = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_CEP_PORT_MEMBER_OF_PEP_PVID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_CEP_PORT_MEMBER_OF_PEP_PVID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_eth_port_info2_tbl_data.cep_port_member_of_pep_pvid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_CEP_PORT_MEMBER_OF_CEP_PVID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_CEP_PORT_MEMBER_OF_CEP_PVID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_eth_port_info2_tbl_data.cep_port_member_of_cep_pvid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_PEP_GENERATE_PRIORITY_STAG_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_PEP_GENERATE_PRIORITY_STAG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_eth_port_info2_tbl_data.pep_generate_priority_stag = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_PEP_EGRESS_FILTER_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_PEP_EGRESS_FILTER_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_eth_port_info2_tbl_data.pep_egress_filter_enable = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_CEP_ACCEPT_UNTAGGED_PRIORITY_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_CEP_ACCEPT_UNTAGGED_PRIORITY_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_eth_port_info2_tbl_data.cep_accept_untagged_priority = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_CEP_ACCEPT_TAGGED_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_CEP_ACCEPT_TAGGED_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_eth_port_info2_tbl_data.cep_accept_tagged = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_CEP_INGRESS_FILTER_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_CEP_INGRESS_FILTER_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_eth_port_info2_tbl_data.cep_ingress_filter_enable = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = soc_petra_pp_ihp_eth_port_info2_tbl_set_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_eth_port_info2_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_eth_port_info2_tbl_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_eth_port_info2_tbl_set");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_eth_port_info2_tbl_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_eth_port_info2_tbl_get (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_eth_port_info2_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_DATA
    prm_IHP_eth_port_info2_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_eth_port_info2_tbl_get";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_GET_IHP_ETH_PORT_INFO2_TBL_GET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_DATA_clear(&prm_IHP_eth_port_info2_tbl_data);

  /* Get parameters */

  /* Call function */
  ret = soc_petra_pp_ihp_eth_port_info2_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_eth_port_info2_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_eth_port_info2_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_eth_port_info2_tbl_get");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_eth_port_info2_tbl_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> IHP_eth_port_info2_tbl_data:", TRUE);
  soc_petra_pp_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_DATA_print(&prm_IHP_eth_port_info2_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_cid_info_tbl_set (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_cid_info_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_CID_INFO_TBL_DATA
    prm_IHP_cid_info_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_cid_info_tbl_set";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_CID_INFO_TBL_SET_IHP_CID_INFO_TBL_SET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_CID_INFO_TBL_DATA_clear(&prm_IHP_cid_info_tbl_data);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_petra_pp_ihp_cid_info_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_cid_info_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_cid_info_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_cid_info_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_CID_INFO_TBL_SET_IHP_CID_INFO_TBL_SET_IHP_CID_INFO_TBL_DATA_ENABLE_L3_LAG_HASH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_CID_INFO_TBL_SET_IHP_CID_INFO_TBL_SET_IHP_CID_INFO_TBL_DATA_ENABLE_L3_LAG_HASH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_cid_info_tbl_data.enable_l3_lag_hash = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_CID_INFO_TBL_SET_IHP_CID_INFO_TBL_SET_IHP_CID_INFO_TBL_DATA_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_CID_INFO_TBL_SET_IHP_CID_INFO_TBL_SET_IHP_CID_INFO_TBL_DATA_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_cid_info_tbl_data.valid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_CID_INFO_TBL_SET_IHP_CID_INFO_TBL_SET_IHP_CID_INFO_TBL_DATA_DISABLE_MC_ROUTING_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_CID_INFO_TBL_SET_IHP_CID_INFO_TBL_SET_IHP_CID_INFO_TBL_DATA_DISABLE_MC_ROUTING_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_cid_info_tbl_data.disable_mc_routing = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_CID_INFO_TBL_SET_IHP_CID_INFO_TBL_SET_IHP_CID_INFO_TBL_DATA_DISABLE_UC_ROUTING_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_CID_INFO_TBL_SET_IHP_CID_INFO_TBL_SET_IHP_CID_INFO_TBL_DATA_DISABLE_UC_ROUTING_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_cid_info_tbl_data.disable_uc_routing = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_CID_INFO_TBL_SET_IHP_CID_INFO_TBL_SET_IHP_CID_INFO_TBL_DATA_INGRESS_FILTER_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_CID_INFO_TBL_SET_IHP_CID_INFO_TBL_SET_IHP_CID_INFO_TBL_DATA_INGRESS_FILTER_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_cid_info_tbl_data.ingress_filter_enable = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_CID_INFO_TBL_SET_IHP_CID_INFO_TBL_SET_IHP_CID_INFO_TBL_DATA_DA_NOT_FOUND_MC_FWD_ACTION_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_CID_INFO_TBL_SET_IHP_CID_INFO_TBL_SET_IHP_CID_INFO_TBL_DATA_DA_NOT_FOUND_MC_FWD_ACTION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_cid_info_tbl_data.da_not_found_mc_fwd_action = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_CID_INFO_TBL_SET_IHP_CID_INFO_TBL_SET_IHP_CID_INFO_TBL_DATA_DA_NOT_FOUND_UC_FWD_ACTION_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_CID_INFO_TBL_SET_IHP_CID_INFO_TBL_SET_IHP_CID_INFO_TBL_DATA_DA_NOT_FOUND_UC_FWD_ACTION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_cid_info_tbl_data.da_not_found_uc_fwd_action = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_CID_INFO_TBL_SET_IHP_CID_INFO_TBL_SET_IHP_CID_INFO_TBL_DATA_CID_DEFAULT_TC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_CID_INFO_TBL_SET_IHP_CID_INFO_TBL_SET_IHP_CID_INFO_TBL_DATA_CID_DEFAULT_TC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_cid_info_tbl_data.cid_default_tc = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = soc_petra_pp_ihp_cid_info_tbl_set_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_cid_info_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_cid_info_tbl_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_cid_info_tbl_set");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_cid_info_tbl_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_cid_info_tbl_get (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_cid_info_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_CID_INFO_TBL_DATA
    prm_IHP_cid_info_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_cid_info_tbl_get";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_CID_INFO_TBL_GET_IHP_CID_INFO_TBL_GET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_CID_INFO_TBL_DATA_clear(&prm_IHP_cid_info_tbl_data);

  /* Get parameters */

  /* Call function */
  ret = soc_petra_pp_ihp_cid_info_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_cid_info_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_cid_info_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_cid_info_tbl_get");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_cid_info_tbl_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> IHP_cid_info_tbl_data:", TRUE);
  soc_petra_pp_PETRA_PP_IHP_CID_INFO_TBL_DATA_print(&prm_IHP_cid_info_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_cid_to_topology_tbl_set (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_cid_to_topology_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_CID_TO_TOPOLOGY_TBL_DATA
    prm_IHP_cid_to_topology_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_cid_to_topology_tbl_set";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_CID_TO_TOPOLOGY_TBL_SET_IHP_CID_TO_TOPOLOGY_TBL_SET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_CID_TO_TOPOLOGY_TBL_DATA_clear(&prm_IHP_cid_to_topology_tbl_data);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_petra_pp_ihp_cid_to_topology_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_cid_to_topology_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_cid_to_topology_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_cid_to_topology_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_CID_TO_TOPOLOGY_TBL_SET_IHP_CID_TO_TOPOLOGY_TBL_SET_IHP_CID_TO_TOPOLOGY_TBL_DATA_CID_TO_TOPOLOGY_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_CID_TO_TOPOLOGY_TBL_SET_IHP_CID_TO_TOPOLOGY_TBL_SET_IHP_CID_TO_TOPOLOGY_TBL_DATA_CID_TO_TOPOLOGY_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_cid_to_topology_tbl_data.cid_to_topology = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = soc_petra_pp_ihp_cid_to_topology_tbl_set_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_cid_to_topology_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_cid_to_topology_tbl_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_cid_to_topology_tbl_set");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_cid_to_topology_tbl_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_cid_to_topology_tbl_get (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_cid_to_topology_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_CID_TO_TOPOLOGY_TBL_DATA
    prm_IHP_cid_to_topology_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_cid_to_topology_tbl_get";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_CID_TO_TOPOLOGY_TBL_GET_IHP_CID_TO_TOPOLOGY_TBL_GET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_CID_TO_TOPOLOGY_TBL_DATA_clear(&prm_IHP_cid_to_topology_tbl_data);

  /* Get parameters */

  /* Call function */
  ret = soc_petra_pp_ihp_cid_to_topology_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_cid_to_topology_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_cid_to_topology_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_cid_to_topology_tbl_get");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_cid_to_topology_tbl_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> IHP_cid_to_topology_tbl_data:", TRUE);
  soc_petra_pp_PETRA_PP_IHP_CID_TO_TOPOLOGY_TBL_DATA_print(&prm_IHP_cid_to_topology_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_stp_table_tbl_set (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_stp_table_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_STP_TABLE_TBL_DATA
    prm_IHP_stp_table_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_stp_table_tbl_set";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_STP_TABLE_TBL_SET_IHP_STP_TABLE_TBL_SET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_STP_TABLE_TBL_DATA_clear(&prm_IHP_stp_table_tbl_data);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_petra_pp_ihp_stp_table_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_stp_table_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_stp_table_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_stp_table_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_STP_TABLE_TBL_SET_IHP_STP_TABLE_TBL_SET_IHP_STP_TABLE_TBL_DATA_STP_TABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_STP_TABLE_TBL_SET_IHP_STP_TABLE_TBL_SET_IHP_STP_TABLE_TBL_DATA_STP_TABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_stp_table_tbl_data.stp_table = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = soc_petra_pp_ihp_stp_table_tbl_set_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_stp_table_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_stp_table_tbl_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_stp_table_tbl_set");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_stp_table_tbl_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_stp_table_tbl_get (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_stp_table_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_STP_TABLE_TBL_DATA
    prm_IHP_stp_table_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_stp_table_tbl_get";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_STP_TABLE_TBL_GET_IHP_STP_TABLE_TBL_GET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_STP_TABLE_TBL_DATA_clear(&prm_IHP_stp_table_tbl_data);

  /* Get parameters */

  /* Call function */
  ret = soc_petra_pp_ihp_stp_table_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_stp_table_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_stp_table_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_stp_table_tbl_get");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_stp_table_tbl_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> IHP_stp_table_tbl_data:", TRUE);
  soc_petra_pp_PETRA_PP_IHP_STP_TABLE_TBL_DATA_print(&prm_IHP_stp_table_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_port_and_protocol_to_cid_index_tbl_set (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_port_and_protocol_to_cid_index_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_PORT_AND_PROTOCOL_TO_CID_INDEX_TBL_DATA
    prm_IHP_port_and_protocol_to_cid_index_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_port_and_protocol_to_cid_index_tbl_set";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_PORT_AND_PROTOCOL_TO_CID_INDEX_TBL_SET_IHP_PORT_AND_PROTOCOL_TO_CID_INDEX_TBL_SET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_PORT_AND_PROTOCOL_TO_CID_INDEX_TBL_DATA_clear(&prm_IHP_port_and_protocol_to_cid_index_tbl_data);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_petra_pp_ihp_port_and_protocol_to_cid_index_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_port_and_protocol_to_cid_index_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_port_and_protocol_to_cid_index_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_port_and_protocol_to_cid_index_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_PORT_AND_PROTOCOL_TO_CID_INDEX_TBL_SET_IHP_PORT_AND_PROTOCOL_TO_CID_INDEX_TBL_SET_IHP_PORT_AND_PROTOCOL_TO_CID_INDEX_TBL_DATA_PORT_AND_PROTOCOL_TO_CID_INDEX_0_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_PORT_AND_PROTOCOL_TO_CID_INDEX_TBL_SET_IHP_PORT_AND_PROTOCOL_TO_CID_INDEX_TBL_SET_IHP_PORT_AND_PROTOCOL_TO_CID_INDEX_TBL_DATA_PORT_AND_PROTOCOL_TO_CID_INDEX_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_port_and_protocol_to_cid_index_tbl_data.port_and_protocol_to_cid_index[0] = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = soc_petra_pp_ihp_port_and_protocol_to_cid_index_tbl_set_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_port_and_protocol_to_cid_index_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_port_and_protocol_to_cid_index_tbl_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_port_and_protocol_to_cid_index_tbl_set");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_port_and_protocol_to_cid_index_tbl_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_port_and_protocol_to_cid_index_tbl_get (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_port_and_protocol_to_cid_index_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_PORT_AND_PROTOCOL_TO_CID_INDEX_TBL_DATA
    prm_IHP_port_and_protocol_to_cid_index_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_port_and_protocol_to_cid_index_tbl_get";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_PORT_AND_PROTOCOL_TO_CID_INDEX_TBL_GET_IHP_PORT_AND_PROTOCOL_TO_CID_INDEX_TBL_GET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_PORT_AND_PROTOCOL_TO_CID_INDEX_TBL_DATA_clear(&prm_IHP_port_and_protocol_to_cid_index_tbl_data);

  /* Get parameters */

  /* Call function */
  ret = soc_petra_pp_ihp_port_and_protocol_to_cid_index_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_port_and_protocol_to_cid_index_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_port_and_protocol_to_cid_index_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_port_and_protocol_to_cid_index_tbl_get");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_port_and_protocol_to_cid_index_tbl_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> IHP_port_and_protocol_to_cid_index_tbl_data:", TRUE);
  soc_petra_pp_PETRA_PP_IHP_PORT_AND_PROTOCOL_TO_CID_INDEX_TBL_DATA_print(&prm_IHP_port_and_protocol_to_cid_index_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_cid_index_to_cid_tbl_set (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_cid_index_to_cid_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_CID_INDEX_TO_CID_TBL_DATA
    prm_IHP_cid_index_to_cid_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_cid_index_to_cid_tbl_set";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_CID_INDEX_TO_CID_TBL_SET_IHP_CID_INDEX_TO_CID_TBL_SET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_CID_INDEX_TO_CID_TBL_DATA_clear(&prm_IHP_cid_index_to_cid_tbl_data);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_petra_pp_ihp_cid_index_to_cid_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_cid_index_to_cid_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_cid_index_to_cid_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_cid_index_to_cid_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_CID_INDEX_TO_CID_TBL_SET_IHP_CID_INDEX_TO_CID_TBL_SET_IHP_CID_INDEX_TO_CID_TBL_DATA_CVLAN_UP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_CID_INDEX_TO_CID_TBL_SET_IHP_CID_INDEX_TO_CID_TBL_SET_IHP_CID_INDEX_TO_CID_TBL_DATA_CVLAN_UP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_cid_index_to_cid_tbl_data.cvlan_up = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_CID_INDEX_TO_CID_TBL_SET_IHP_CID_INDEX_TO_CID_TBL_SET_IHP_CID_INDEX_TO_CID_TBL_DATA_CID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_CID_INDEX_TO_CID_TBL_SET_IHP_CID_INDEX_TO_CID_TBL_SET_IHP_CID_INDEX_TO_CID_TBL_DATA_CID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_cid_index_to_cid_tbl_data.cid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_CID_INDEX_TO_CID_TBL_SET_IHP_CID_INDEX_TO_CID_TBL_SET_IHP_CID_INDEX_TO_CID_TBL_DATA_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_CID_INDEX_TO_CID_TBL_SET_IHP_CID_INDEX_TO_CID_TBL_SET_IHP_CID_INDEX_TO_CID_TBL_DATA_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_cid_index_to_cid_tbl_data.valid = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = soc_petra_pp_ihp_cid_index_to_cid_tbl_set_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_cid_index_to_cid_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_cid_index_to_cid_tbl_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_cid_index_to_cid_tbl_set");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_cid_index_to_cid_tbl_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_cid_index_to_cid_tbl_get (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_cid_index_to_cid_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_CID_INDEX_TO_CID_TBL_DATA
    prm_IHP_cid_index_to_cid_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_cid_index_to_cid_tbl_get";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_CID_INDEX_TO_CID_TBL_GET_IHP_CID_INDEX_TO_CID_TBL_GET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_CID_INDEX_TO_CID_TBL_DATA_clear(&prm_IHP_cid_index_to_cid_tbl_data);

  /* Get parameters */

  /* Call function */
  ret = soc_petra_pp_ihp_cid_index_to_cid_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_cid_index_to_cid_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_cid_index_to_cid_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_cid_index_to_cid_tbl_get");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_cid_index_to_cid_tbl_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> IHP_cid_index_to_cid_tbl_data:", TRUE);
  soc_petra_pp_PETRA_PP_IHP_CID_INDEX_TO_CID_TBL_DATA_print(&prm_IHP_cid_index_to_cid_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_bridge_control_to_forwarding_params_tbl_set (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_bridge_control_to_forwarding_params_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_DATA
    prm_IHP_bridge_control_to_forwarding_params_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_bridge_control_to_forwarding_params_tbl_set";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_SET_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_SET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_DATA_clear(&prm_IHP_bridge_control_to_forwarding_params_tbl_data);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_petra_pp_ihp_bridge_control_to_forwarding_params_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_bridge_control_to_forwarding_params_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_bridge_control_to_forwarding_params_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_bridge_control_to_forwarding_params_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_SET_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_SET_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_DATA_SNOOP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_SET_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_SET_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_DATA_SNOOP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_bridge_control_to_forwarding_params_tbl_data.snoop = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_SET_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_SET_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_DATA_POLICER_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_SET_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_SET_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_DATA_POLICER_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_bridge_control_to_forwarding_params_tbl_data.policer = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_SET_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_SET_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_DATA_COS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_SET_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_SET_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_DATA_COS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_bridge_control_to_forwarding_params_tbl_data.cos = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_SET_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_SET_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_DATA_COS_OVERWRITE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_SET_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_SET_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_DATA_COS_OVERWRITE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_bridge_control_to_forwarding_params_tbl_data.cos_overwrite = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_SET_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_SET_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_DATA_DESTINATION_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_SET_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_SET_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_DATA_DESTINATION_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_bridge_control_to_forwarding_params_tbl_data.destination_index = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_SET_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_SET_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_DATA_TYPE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_SET_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_SET_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_DATA_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_bridge_control_to_forwarding_params_tbl_data.type = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = soc_petra_pp_ihp_bridge_control_to_forwarding_params_tbl_set_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_bridge_control_to_forwarding_params_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_bridge_control_to_forwarding_params_tbl_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_bridge_control_to_forwarding_params_tbl_set");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_bridge_control_to_forwarding_params_tbl_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_bridge_control_to_forwarding_params_tbl_get (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_bridge_control_to_forwarding_params_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_DATA
    prm_IHP_bridge_control_to_forwarding_params_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_bridge_control_to_forwarding_params_tbl_get";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_GET_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_GET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_DATA_clear(&prm_IHP_bridge_control_to_forwarding_params_tbl_data);

  /* Get parameters */

  /* Call function */
  ret = soc_petra_pp_ihp_bridge_control_to_forwarding_params_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_bridge_control_to_forwarding_params_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_bridge_control_to_forwarding_params_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_bridge_control_to_forwarding_params_tbl_get");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_bridge_control_to_forwarding_params_tbl_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> IHP_bridge_control_to_forwarding_params_tbl_data:", TRUE);
  soc_petra_pp_PETRA_PP_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_DATA_print(&prm_IHP_bridge_control_to_forwarding_params_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_cid_da_not_found_fwd_action_tbl_set (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_cid_da_not_found_fwd_action_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_DATA
    prm_IHP_cid_da_not_found_fwd_action_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_cid_da_not_found_fwd_action_tbl_set";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_SET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_DATA_clear(&prm_IHP_cid_da_not_found_fwd_action_tbl_data);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_petra_pp_ihp_cid_da_not_found_fwd_action_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_cid_da_not_found_fwd_action_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_cid_da_not_found_fwd_action_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_cid_da_not_found_fwd_action_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_DATA_ADD_CID_TO_DEST_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_DATA_ADD_CID_TO_DEST_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_cid_da_not_found_fwd_action_tbl_data.add_cid_to_dest = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_DATA_POLICER_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_DATA_POLICER_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_cid_da_not_found_fwd_action_tbl_data.policer = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_DATA_CPU_CODE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_DATA_CPU_CODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_cid_da_not_found_fwd_action_tbl_data.cpu_code = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_DATA_COS_OVERWRITE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_DATA_COS_OVERWRITE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_cid_da_not_found_fwd_action_tbl_data.cos_overwrite = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_DATA_COS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_DATA_COS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_cid_da_not_found_fwd_action_tbl_data.cos = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_DATA_DESTINATION_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_DATA_DESTINATION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_cid_da_not_found_fwd_action_tbl_data.destination = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_DATA_TYPE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_DATA_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_cid_da_not_found_fwd_action_tbl_data.type = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = soc_petra_pp_ihp_cid_da_not_found_fwd_action_tbl_set_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_cid_da_not_found_fwd_action_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_cid_da_not_found_fwd_action_tbl_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_cid_da_not_found_fwd_action_tbl_set");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_cid_da_not_found_fwd_action_tbl_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_cid_da_not_found_fwd_action_tbl_get (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_cid_da_not_found_fwd_action_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_DATA
    prm_IHP_cid_da_not_found_fwd_action_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_cid_da_not_found_fwd_action_tbl_get";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_GET_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_GET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_DATA_clear(&prm_IHP_cid_da_not_found_fwd_action_tbl_data);

  /* Get parameters */

  /* Call function */
  ret = soc_petra_pp_ihp_cid_da_not_found_fwd_action_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_cid_da_not_found_fwd_action_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_cid_da_not_found_fwd_action_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_cid_da_not_found_fwd_action_tbl_get");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_cid_da_not_found_fwd_action_tbl_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> IHP_cid_da_not_found_fwd_action_tbl_data:", TRUE);
  soc_petra_pp_PETRA_PP_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_DATA_print(&prm_IHP_cid_da_not_found_fwd_action_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_cid_port_membership_tbl_set (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_cid_port_membership_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_CID_PORT_MEMBERSHIP_TBL_DATA
    prm_IHP_cid_port_membership_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_cid_port_membership_tbl_set";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_CID_PORT_MEMBERSHIP_TBL_SET_IHP_CID_PORT_MEMBERSHIP_TBL_SET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_CID_PORT_MEMBERSHIP_TBL_DATA_clear(&prm_IHP_cid_port_membership_tbl_data);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_petra_pp_ihp_cid_port_membership_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_cid_port_membership_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_cid_port_membership_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_cid_port_membership_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_CID_PORT_MEMBERSHIP_TBL_SET_IHP_CID_PORT_MEMBERSHIP_TBL_SET_IHP_CID_PORT_MEMBERSHIP_TBL_DATA_CID_PORT_MEMBERSHIP_2_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_CID_PORT_MEMBERSHIP_TBL_SET_IHP_CID_PORT_MEMBERSHIP_TBL_SET_IHP_CID_PORT_MEMBERSHIP_TBL_DATA_CID_PORT_MEMBERSHIP_2_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_cid_port_membership_tbl_data.cid_port_membership[2] = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_CID_PORT_MEMBERSHIP_TBL_SET_IHP_CID_PORT_MEMBERSHIP_TBL_SET_IHP_CID_PORT_MEMBERSHIP_TBL_DATA_CID_PORT_MEMBERSHIP_1_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_CID_PORT_MEMBERSHIP_TBL_SET_IHP_CID_PORT_MEMBERSHIP_TBL_SET_IHP_CID_PORT_MEMBERSHIP_TBL_DATA_CID_PORT_MEMBERSHIP_1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_cid_port_membership_tbl_data.cid_port_membership[1] = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_CID_PORT_MEMBERSHIP_TBL_SET_IHP_CID_PORT_MEMBERSHIP_TBL_SET_IHP_CID_PORT_MEMBERSHIP_TBL_DATA_CID_PORT_MEMBERSHIP_0_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_CID_PORT_MEMBERSHIP_TBL_SET_IHP_CID_PORT_MEMBERSHIP_TBL_SET_IHP_CID_PORT_MEMBERSHIP_TBL_DATA_CID_PORT_MEMBERSHIP_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_cid_port_membership_tbl_data.cid_port_membership[0] = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = soc_petra_pp_ihp_cid_port_membership_tbl_set_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_cid_port_membership_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_cid_port_membership_tbl_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_cid_port_membership_tbl_set");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_cid_port_membership_tbl_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_cid_port_membership_tbl_get (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_cid_port_membership_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_CID_PORT_MEMBERSHIP_TBL_DATA
    prm_IHP_cid_port_membership_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_cid_port_membership_tbl_get";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_CID_PORT_MEMBERSHIP_TBL_GET_IHP_CID_PORT_MEMBERSHIP_TBL_GET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_CID_PORT_MEMBERSHIP_TBL_DATA_clear(&prm_IHP_cid_port_membership_tbl_data);

  /* Get parameters */

  /* Call function */
  ret = soc_petra_pp_ihp_cid_port_membership_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_cid_port_membership_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_cid_port_membership_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_cid_port_membership_tbl_get");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_cid_port_membership_tbl_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> IHP_cid_port_membership_tbl_data:", TRUE);
  soc_petra_pp_PETRA_PP_IHP_CID_PORT_MEMBERSHIP_TBL_DATA_print(&prm_IHP_cid_port_membership_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_is_cid_shared_tbl_set (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_is_cid_shared_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_IS_CID_SHARED_TBL_DATA
    prm_IHP_is_cid_shared_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_is_cid_shared_tbl_set";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_IS_CID_SHARED_TBL_SET_IHP_IS_CID_SHARED_TBL_SET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_IS_CID_SHARED_TBL_DATA_clear(&prm_IHP_is_cid_shared_tbl_data);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_petra_pp_ihp_is_cid_shared_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_is_cid_shared_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_is_cid_shared_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_is_cid_shared_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_IS_CID_SHARED_TBL_SET_IHP_IS_CID_SHARED_TBL_SET_IHP_IS_CID_SHARED_TBL_DATA_IS_CID_SHARED_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_IS_CID_SHARED_TBL_SET_IHP_IS_CID_SHARED_TBL_SET_IHP_IS_CID_SHARED_TBL_DATA_IS_CID_SHARED_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_is_cid_shared_tbl_data.is_cid_shared = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = soc_petra_pp_ihp_is_cid_shared_tbl_set_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_is_cid_shared_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_is_cid_shared_tbl_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_is_cid_shared_tbl_set");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_is_cid_shared_tbl_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_is_cid_shared_tbl_get (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_is_cid_shared_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_IS_CID_SHARED_TBL_DATA
    prm_IHP_is_cid_shared_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_is_cid_shared_tbl_get";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_IS_CID_SHARED_TBL_GET_IHP_IS_CID_SHARED_TBL_GET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_IS_CID_SHARED_TBL_DATA_clear(&prm_IHP_is_cid_shared_tbl_data);

  /* Get parameters */

  /* Call function */
  ret = soc_petra_pp_ihp_is_cid_shared_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_is_cid_shared_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_is_cid_shared_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_is_cid_shared_tbl_get");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_is_cid_shared_tbl_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> IHP_is_cid_shared_tbl_data:", TRUE);
  soc_petra_pp_PETRA_PP_IHP_IS_CID_SHARED_TBL_DATA_print(&prm_IHP_is_cid_shared_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_tos_to_tc_tbl_set (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_tos_to_tc_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_TOS_TO_TC_TBL_DATA
    prm_IHP_tos_to_tc_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_tos_to_tc_tbl_set";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_TOS_TO_TC_TBL_SET_IHP_TOS_TO_TC_TBL_SET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_TOS_TO_TC_TBL_DATA_clear(&prm_IHP_tos_to_tc_tbl_data);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_petra_pp_ihp_tos_to_tc_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_tos_to_tc_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_tos_to_tc_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_tos_to_tc_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_TOS_TO_TC_TBL_SET_IHP_TOS_TO_TC_TBL_SET_IHP_TOS_TO_TC_TBL_DATA_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_TOS_TO_TC_TBL_SET_IHP_TOS_TO_TC_TBL_SET_IHP_TOS_TO_TC_TBL_DATA_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_tos_to_tc_tbl_data.valid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_TOS_TO_TC_TBL_SET_IHP_TOS_TO_TC_TBL_SET_IHP_TOS_TO_TC_TBL_DATA_TRAFFIC_CLASS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_TOS_TO_TC_TBL_SET_IHP_TOS_TO_TC_TBL_SET_IHP_TOS_TO_TC_TBL_DATA_TRAFFIC_CLASS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_tos_to_tc_tbl_data.traffic_class = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = soc_petra_pp_ihp_tos_to_tc_tbl_set_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_tos_to_tc_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_tos_to_tc_tbl_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_tos_to_tc_tbl_set");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_tos_to_tc_tbl_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_tos_to_tc_tbl_get (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_tos_to_tc_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_TOS_TO_TC_TBL_DATA
    prm_IHP_tos_to_tc_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_tos_to_tc_tbl_get";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_TOS_TO_TC_TBL_GET_IHP_TOS_TO_TC_TBL_GET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_TOS_TO_TC_TBL_DATA_clear(&prm_IHP_tos_to_tc_tbl_data);

  /* Get parameters */

  /* Call function */
  ret = soc_petra_pp_ihp_tos_to_tc_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_tos_to_tc_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_tos_to_tc_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_tos_to_tc_tbl_get");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_tos_to_tc_tbl_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> IHP_tos_to_tc_tbl_data:", TRUE);
  soc_petra_pp_PETRA_PP_IHP_TOS_TO_TC_TBL_DATA_print(&prm_IHP_tos_to_tc_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_event_fifo_tbl_set (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_event_fifo_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_EVENT_FIFO_TBL_DATA
    prm_IHP_event_fifo_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_event_fifo_tbl_set";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_SET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_EVENT_FIFO_TBL_DATA_clear(&prm_IHP_event_fifo_tbl_data);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_petra_pp_ihp_event_fifo_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_event_fifo_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_event_fifo_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_event_fifo_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_DATA_FAP_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_DATA_FAP_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_event_fifo_tbl_data.fap_port = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_DATA_SA_DROP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_DATA_SA_DROP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_event_fifo_tbl_data.sa_drop = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_DATA_DA_FWD_TYPE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_DATA_DA_FWD_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_event_fifo_tbl_data.da_fwd_type = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_DATA_DESTINATION_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_DATA_DESTINATION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_event_fifo_tbl_data.destination = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_DATA_IS_FID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_DATA_IS_FID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_event_fifo_tbl_data.is_fid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_DATA_CID_OR_FID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_DATA_CID_OR_FID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_event_fifo_tbl_data.cid_or_fid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_DATA_MAC_0_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_DATA_MAC_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_event_fifo_tbl_data.mac[0] = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_DATA_STAMP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_DATA_STAMP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_event_fifo_tbl_data.stamp = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_DATA_TYPE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_DATA_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_event_fifo_tbl_data.type = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = soc_petra_pp_ihp_event_fifo_tbl_set_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_event_fifo_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_event_fifo_tbl_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_event_fifo_tbl_set");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_event_fifo_tbl_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_event_fifo_tbl_get (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_event_fifo_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_EVENT_FIFO_TBL_DATA
    prm_IHP_event_fifo_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_event_fifo_tbl_get";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_EVENT_FIFO_TBL_GET_IHP_EVENT_FIFO_TBL_GET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_EVENT_FIFO_TBL_DATA_clear(&prm_IHP_event_fifo_tbl_data);

  /* Get parameters */

  /* Call function */
  ret = soc_petra_pp_ihp_event_fifo_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_event_fifo_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_event_fifo_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_event_fifo_tbl_get");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_event_fifo_tbl_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> IHP_event_fifo_tbl_data:", TRUE);
  soc_petra_pp_PETRA_PP_IHP_EVENT_FIFO_TBL_DATA_print(&prm_IHP_event_fifo_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_fec_table_tbl_set (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_fec_table_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_FEC_TABLE_TBL_DATA
    prm_IHP_fec_table_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_fec_table_tbl_set";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_FEC_TABLE_TBL_SET_IHP_FEC_TABLE_TBL_SET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_FEC_TABLE_TBL_DATA_clear(&prm_IHP_fec_table_tbl_data);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_petra_pp_ihp_fec_table_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_fec_table_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_fec_table_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_fec_table_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_FEC_TABLE_TBL_SET_IHP_FEC_TABLE_TBL_SET_IHP_FEC_TABLE_TBL_DATA_FEC_TABLE_0_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_FEC_TABLE_TBL_SET_IHP_FEC_TABLE_TBL_SET_IHP_FEC_TABLE_TBL_DATA_FEC_TABLE_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_fec_table_tbl_data.fec_table[0] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_FEC_TABLE_TBL_SET_IHP_FEC_TABLE_TBL_SET_IHP_FEC_TABLE_TBL_DATA_FEC_TABLE_1_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_FEC_TABLE_TBL_SET_IHP_FEC_TABLE_TBL_SET_IHP_FEC_TABLE_TBL_DATA_FEC_TABLE_1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_fec_table_tbl_data.fec_table[1] = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = soc_petra_pp_ihp_fec_table_tbl_set_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_fec_table_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_fec_table_tbl_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_fec_table_tbl_set");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_fec_table_tbl_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_fec_table_tbl_get (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_fec_table_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_FEC_TABLE_TBL_DATA
    prm_IHP_fec_table_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_fec_table_tbl_get";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_FEC_TABLE_TBL_GET_IHP_FEC_TABLE_TBL_GET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_FEC_TABLE_TBL_DATA_clear(&prm_IHP_fec_table_tbl_data);

  /* Get parameters */

  /* Call function */
  ret = soc_petra_pp_ihp_fec_table_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_fec_table_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_fec_table_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_fec_table_tbl_get");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_fec_table_tbl_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> IHP_fec_table_tbl_data:", TRUE);
  soc_petra_pp_PETRA_PP_IHP_FEC_TABLE_TBL_DATA_print(&prm_IHP_fec_table_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_fec_uc_table_tbl_set (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_fec_uc_table_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_FEC_UC_TABLE_TBL_DATA
    prm_IHP_fec_uc_table_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_fec_uc_table_tbl_set";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_FEC_UC_TABLE_TBL_SET_IHP_FEC_UC_TABLE_TBL_SET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_FEC_UC_TABLE_TBL_DATA_clear(&prm_IHP_fec_uc_table_tbl_data);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_petra_pp_ihp_fec_uc_table_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_fec_uc_table_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_fec_uc_table_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_fec_uc_table_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_FEC_UC_TABLE_TBL_SET_IHP_FEC_UC_TABLE_TBL_SET_IHP_FEC_UC_TABLE_TBL_DATA_RPF_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_FEC_UC_TABLE_TBL_SET_IHP_FEC_UC_TABLE_TBL_SET_IHP_FEC_UC_TABLE_TBL_DATA_RPF_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_fec_uc_table_tbl_data.rpf_enable = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_FEC_UC_TABLE_TBL_SET_IHP_FEC_UC_TABLE_TBL_SET_IHP_FEC_UC_TABLE_TBL_DATA_CPU_CODE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_FEC_UC_TABLE_TBL_SET_IHP_FEC_UC_TABLE_TBL_SET_IHP_FEC_UC_TABLE_TBL_DATA_CPU_CODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_fec_uc_table_tbl_data.cpu_code = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_FEC_UC_TABLE_TBL_SET_IHP_FEC_UC_TABLE_TBL_SET_IHP_FEC_UC_TABLE_TBL_DATA_ARP_POINTER_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_FEC_UC_TABLE_TBL_SET_IHP_FEC_UC_TABLE_TBL_SET_IHP_FEC_UC_TABLE_TBL_DATA_ARP_POINTER_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_fec_uc_table_tbl_data.arp_pointer = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_FEC_UC_TABLE_TBL_SET_IHP_FEC_UC_TABLE_TBL_SET_IHP_FEC_UC_TABLE_TBL_DATA_OUT_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_FEC_UC_TABLE_TBL_SET_IHP_FEC_UC_TABLE_TBL_SET_IHP_FEC_UC_TABLE_TBL_DATA_OUT_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_fec_uc_table_tbl_data.out_vid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_FEC_UC_TABLE_TBL_SET_IHP_FEC_UC_TABLE_TBL_SET_IHP_FEC_UC_TABLE_TBL_DATA_COUNTER_POINTER_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_FEC_UC_TABLE_TBL_SET_IHP_FEC_UC_TABLE_TBL_SET_IHP_FEC_UC_TABLE_TBL_DATA_COUNTER_POINTER_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_fec_uc_table_tbl_data.counter_pointer = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_FEC_UC_TABLE_TBL_SET_IHP_FEC_UC_TABLE_TBL_SET_IHP_FEC_UC_TABLE_TBL_DATA_TOS_BASED_COS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_FEC_UC_TABLE_TBL_SET_IHP_FEC_UC_TABLE_TBL_SET_IHP_FEC_UC_TABLE_TBL_DATA_TOS_BASED_COS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_fec_uc_table_tbl_data.tos_based_cos = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_FEC_UC_TABLE_TBL_SET_IHP_FEC_UC_TABLE_TBL_SET_IHP_FEC_UC_TABLE_TBL_DATA_DESTINATION_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_FEC_UC_TABLE_TBL_SET_IHP_FEC_UC_TABLE_TBL_SET_IHP_FEC_UC_TABLE_TBL_DATA_DESTINATION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_fec_uc_table_tbl_data.destination = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_FEC_UC_TABLE_TBL_SET_IHP_FEC_UC_TABLE_TBL_SET_IHP_FEC_UC_TABLE_TBL_DATA_COMMAND_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_FEC_UC_TABLE_TBL_SET_IHP_FEC_UC_TABLE_TBL_SET_IHP_FEC_UC_TABLE_TBL_DATA_COMMAND_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_fec_uc_table_tbl_data.command = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = soc_petra_pp_ihp_fec_uc_table_tbl_set_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_fec_uc_table_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_fec_uc_table_tbl_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_fec_uc_table_tbl_set");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_fec_uc_table_tbl_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_fec_uc_table_tbl_get (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_fec_uc_table_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_FEC_UC_TABLE_TBL_DATA
    prm_IHP_fec_uc_table_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_fec_uc_table_tbl_get";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_FEC_UC_TABLE_TBL_GET_IHP_FEC_UC_TABLE_TBL_GET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_FEC_UC_TABLE_TBL_DATA_clear(&prm_IHP_fec_uc_table_tbl_data);

  /* Get parameters */

  /* Call function */
  ret = soc_petra_pp_ihp_fec_uc_table_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_fec_uc_table_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_fec_uc_table_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_fec_uc_table_tbl_get");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_fec_uc_table_tbl_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> IHP_fec_uc_table_tbl_data:", TRUE);
  soc_petra_pp_PETRA_PP_IHP_FEC_UC_TABLE_TBL_DATA_print(&prm_IHP_fec_uc_table_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_fec_mc_table_tbl_set (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_fec_mc_table_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_FEC_MC_TABLE_TBL_DATA
    prm_IHP_fec_mc_table_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_fec_mc_table_tbl_set";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_FEC_MC_TABLE_TBL_SET_IHP_FEC_MC_TABLE_TBL_SET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_FEC_MC_TABLE_TBL_DATA_clear(&prm_IHP_fec_mc_table_tbl_data);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_petra_pp_ihp_fec_mc_table_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_fec_mc_table_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_fec_mc_table_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_fec_mc_table_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_FEC_MC_TABLE_TBL_SET_IHP_FEC_MC_TABLE_TBL_SET_IHP_FEC_MC_TABLE_TBL_DATA_CPU_CODE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_FEC_MC_TABLE_TBL_SET_IHP_FEC_MC_TABLE_TBL_SET_IHP_FEC_MC_TABLE_TBL_DATA_CPU_CODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_fec_mc_table_tbl_data.cpu_code = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_FEC_MC_TABLE_TBL_SET_IHP_FEC_MC_TABLE_TBL_SET_IHP_FEC_MC_TABLE_TBL_DATA_EXPECTED_IN_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_FEC_MC_TABLE_TBL_SET_IHP_FEC_MC_TABLE_TBL_SET_IHP_FEC_MC_TABLE_TBL_DATA_EXPECTED_IN_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_fec_mc_table_tbl_data.expected_in_port = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_FEC_MC_TABLE_TBL_SET_IHP_FEC_MC_TABLE_TBL_SET_IHP_FEC_MC_TABLE_TBL_DATA_EXPECTED_IN_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_FEC_MC_TABLE_TBL_SET_IHP_FEC_MC_TABLE_TBL_SET_IHP_FEC_MC_TABLE_TBL_DATA_EXPECTED_IN_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_fec_mc_table_tbl_data.expected_in_vid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_FEC_MC_TABLE_TBL_SET_IHP_FEC_MC_TABLE_TBL_SET_IHP_FEC_MC_TABLE_TBL_DATA_COUNTER_POINTER_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_FEC_MC_TABLE_TBL_SET_IHP_FEC_MC_TABLE_TBL_SET_IHP_FEC_MC_TABLE_TBL_DATA_COUNTER_POINTER_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_fec_mc_table_tbl_data.counter_pointer = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_FEC_MC_TABLE_TBL_SET_IHP_FEC_MC_TABLE_TBL_SET_IHP_FEC_MC_TABLE_TBL_DATA_TOS_BASED_COS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_FEC_MC_TABLE_TBL_SET_IHP_FEC_MC_TABLE_TBL_SET_IHP_FEC_MC_TABLE_TBL_DATA_TOS_BASED_COS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_fec_mc_table_tbl_data.tos_based_cos = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_FEC_MC_TABLE_TBL_SET_IHP_FEC_MC_TABLE_TBL_SET_IHP_FEC_MC_TABLE_TBL_DATA_DESTINATION_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_FEC_MC_TABLE_TBL_SET_IHP_FEC_MC_TABLE_TBL_SET_IHP_FEC_MC_TABLE_TBL_DATA_DESTINATION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_fec_mc_table_tbl_data.destination = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_FEC_MC_TABLE_TBL_SET_IHP_FEC_MC_TABLE_TBL_SET_IHP_FEC_MC_TABLE_TBL_DATA_COMMAND_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_FEC_MC_TABLE_TBL_SET_IHP_FEC_MC_TABLE_TBL_SET_IHP_FEC_MC_TABLE_TBL_DATA_COMMAND_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_fec_mc_table_tbl_data.command = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = soc_petra_pp_ihp_fec_mc_table_tbl_set_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_fec_mc_table_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_fec_mc_table_tbl_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_fec_mc_table_tbl_set");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_fec_mc_table_tbl_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_fec_mc_table_tbl_get (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_fec_mc_table_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_FEC_MC_TABLE_TBL_DATA
    prm_IHP_fec_mc_table_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_fec_mc_table_tbl_get";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_FEC_MC_TABLE_TBL_GET_IHP_FEC_MC_TABLE_TBL_GET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_FEC_MC_TABLE_TBL_DATA_clear(&prm_IHP_fec_mc_table_tbl_data);

  /* Get parameters */

  /* Call function */
  ret = soc_petra_pp_ihp_fec_mc_table_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_fec_mc_table_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_fec_mc_table_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_fec_mc_table_tbl_get");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_fec_mc_table_tbl_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> IHP_fec_mc_table_tbl_data:", TRUE);
  soc_petra_pp_PETRA_PP_IHP_FEC_MC_TABLE_TBL_DATA_print(&prm_IHP_fec_mc_table_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_fec_prog_port_table_tbl_set (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_fec_prog_port_table_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_FEC_PROG_PORT_TABLE_TBL_DATA
    prm_IHP_fec_prog_port_table_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_fec_prog_port_table_tbl_set";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_FEC_PROG_PORT_TABLE_TBL_SET_IHP_FEC_PROG_PORT_TABLE_TBL_SET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_FEC_PROG_PORT_TABLE_TBL_DATA_clear(&prm_IHP_fec_prog_port_table_tbl_data);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_petra_pp_ihp_fec_prog_port_table_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_fec_prog_port_table_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_fec_prog_port_table_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_fec_prog_port_table_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_FEC_PROG_PORT_TABLE_TBL_SET_IHP_FEC_PROG_PORT_TABLE_TBL_SET_IHP_FEC_PROG_PORT_TABLE_TBL_DATA_DESTINATION_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_FEC_PROG_PORT_TABLE_TBL_SET_IHP_FEC_PROG_PORT_TABLE_TBL_SET_IHP_FEC_PROG_PORT_TABLE_TBL_DATA_DESTINATION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_fec_prog_port_table_tbl_data.destination = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_FEC_PROG_PORT_TABLE_TBL_SET_IHP_FEC_PROG_PORT_TABLE_TBL_SET_IHP_FEC_PROG_PORT_TABLE_TBL_DATA_COS_KEY_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_FEC_PROG_PORT_TABLE_TBL_SET_IHP_FEC_PROG_PORT_TABLE_TBL_SET_IHP_FEC_PROG_PORT_TABLE_TBL_DATA_COS_KEY_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_fec_prog_port_table_tbl_data.cos_key = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_FEC_PROG_PORT_TABLE_TBL_SET_IHP_FEC_PROG_PORT_TABLE_TBL_SET_IHP_FEC_PROG_PORT_TABLE_TBL_DATA_SNOOP_COMMAND_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_FEC_PROG_PORT_TABLE_TBL_SET_IHP_FEC_PROG_PORT_TABLE_TBL_SET_IHP_FEC_PROG_PORT_TABLE_TBL_DATA_SNOOP_COMMAND_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_fec_prog_port_table_tbl_data.snoop_command = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = soc_petra_pp_ihp_fec_prog_port_table_tbl_set_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_fec_prog_port_table_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_fec_prog_port_table_tbl_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_fec_prog_port_table_tbl_set");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_fec_prog_port_table_tbl_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_fec_prog_port_table_tbl_get (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_fec_prog_port_table_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_FEC_PROG_PORT_TABLE_TBL_DATA
    prm_IHP_fec_prog_port_table_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_fec_prog_port_table_tbl_get";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_FEC_PROG_PORT_TABLE_TBL_GET_IHP_FEC_PROG_PORT_TABLE_TBL_GET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_FEC_PROG_PORT_TABLE_TBL_DATA_clear(&prm_IHP_fec_prog_port_table_tbl_data);

  /* Get parameters */

  /* Call function */
  ret = soc_petra_pp_ihp_fec_prog_port_table_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_fec_prog_port_table_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_fec_prog_port_table_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_fec_prog_port_table_tbl_get");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_fec_prog_port_table_tbl_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> IHP_fec_prog_port_table_tbl_data:", TRUE);
  soc_petra_pp_PETRA_PP_IHP_FEC_PROG_PORT_TABLE_TBL_DATA_print(&prm_IHP_fec_prog_port_table_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_smooth_division_tbl_set (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_smooth_division_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_SMOOTH_DIVISION_TBL_DATA
    prm_IHP_smooth_division_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_smooth_division_tbl_set";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_SMOOTH_DIVISION_TBL_SET_IHP_SMOOTH_DIVISION_TBL_SET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_SMOOTH_DIVISION_TBL_DATA_clear(&prm_IHP_smooth_division_tbl_data);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_petra_pp_ihp_smooth_division_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_smooth_division_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_smooth_division_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_smooth_division_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_SMOOTH_DIVISION_TBL_SET_IHP_SMOOTH_DIVISION_TBL_SET_IHP_SMOOTH_DIVISION_TBL_DATA_SMOOTH_DIVISION_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_SMOOTH_DIVISION_TBL_SET_IHP_SMOOTH_DIVISION_TBL_SET_IHP_SMOOTH_DIVISION_TBL_DATA_SMOOTH_DIVISION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_smooth_division_tbl_data.smooth_division = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = soc_petra_pp_ihp_smooth_division_tbl_set_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_smooth_division_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_smooth_division_tbl_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_smooth_division_tbl_set");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_smooth_division_tbl_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_smooth_division_tbl_get (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_smooth_division_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_SMOOTH_DIVISION_TBL_DATA
    prm_IHP_smooth_division_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_smooth_division_tbl_get";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_SMOOTH_DIVISION_TBL_GET_IHP_SMOOTH_DIVISION_TBL_GET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_SMOOTH_DIVISION_TBL_DATA_clear(&prm_IHP_smooth_division_tbl_data);

  /* Get parameters */

  /* Call function */
  ret = soc_petra_pp_ihp_smooth_division_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_smooth_division_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_smooth_division_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_smooth_division_tbl_get");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_smooth_division_tbl_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> IHP_smooth_division_tbl_data:", TRUE);
  soc_petra_pp_PETRA_PP_IHP_SMOOTH_DIVISION_TBL_DATA_print(&prm_IHP_smooth_division_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_longest_prefix_match_tbl_set (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_longest_prefix_match_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint8
    prm_tbl_ndx;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_LONGEST_PREFIX_MATCH_TBL_DATA
    prm_IHP_longest_prefix_match_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_longest_prefix_match_tbl_set";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_LONGEST_PREFIX_MATCH_TBL_SET_IHP_LONGEST_PREFIX_MATCH_TBL_SET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_LONGEST_PREFIX_MATCH_TBL_DATA_clear(&prm_IHP_longest_prefix_match_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_LONGEST_PREFIX_MATCH_TBL_SET_IHP_LONGEST_PREFIX_MATCH_TBL_SET_TBL_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_LONGEST_PREFIX_MATCH_TBL_SET_IHP_LONGEST_PREFIX_MATCH_TBL_SET_TBL_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_ndx = (uint8)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter tbl_ndx after ihp_longest_prefix_match_tbl_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = soc_petra_pp_ihp_longest_prefix_match_tbl_get_unsafe(
          unit,
          prm_tbl_ndx,
          prm_entry_ndx,
          &prm_IHP_longest_prefix_match_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_longest_prefix_match_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_longest_prefix_match_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_LONGEST_PREFIX_MATCH_TBL_SET_IHP_LONGEST_PREFIX_MATCH_TBL_SET_IHP_LONGEST_PREFIX_MATCH_TBL_DATA_LONGEST_PREFIX_MATCH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_LONGEST_PREFIX_MATCH_TBL_SET_IHP_LONGEST_PREFIX_MATCH_TBL_SET_IHP_LONGEST_PREFIX_MATCH_TBL_DATA_LONGEST_PREFIX_MATCH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_longest_prefix_match_tbl_data.longest_prefix_match = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = soc_petra_pp_ihp_longest_prefix_match_tbl_set_unsafe(
          unit,
          prm_tbl_ndx,
          prm_entry_ndx,
          &prm_IHP_longest_prefix_match_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_longest_prefix_match_tbl_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_longest_prefix_match_tbl_set");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_longest_prefix_match_tbl_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_longest_prefix_match_tbl_get (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_longest_prefix_match_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint8
    prm_tbl_ndx;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_LONGEST_PREFIX_MATCH_TBL_DATA
    prm_IHP_longest_prefix_match_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_longest_prefix_match_tbl_get";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_LONGEST_PREFIX_MATCH_TBL_GET_IHP_LONGEST_PREFIX_MATCH_TBL_GET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_LONGEST_PREFIX_MATCH_TBL_DATA_clear(&prm_IHP_longest_prefix_match_tbl_data);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_LONGEST_PREFIX_MATCH_TBL_GET_IHP_LONGEST_PREFIX_MATCH_TBL_GET_TBL_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_LONGEST_PREFIX_MATCH_TBL_GET_IHP_LONGEST_PREFIX_MATCH_TBL_GET_TBL_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tbl_ndx = (uint8)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter tbl_ndx after ihp_longest_prefix_match_tbl_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = soc_petra_pp_ihp_longest_prefix_match_tbl_get_unsafe(
          unit,
          prm_tbl_ndx,
          prm_entry_ndx,
          &prm_IHP_longest_prefix_match_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_longest_prefix_match_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_longest_prefix_match_tbl_get");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_longest_prefix_match_tbl_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> IHP_longest_prefix_match_tbl_data:", TRUE);
  soc_petra_pp_PETRA_PP_IHP_LONGEST_PREFIX_MATCH_TBL_DATA_print(&prm_IHP_longest_prefix_match_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_longest_prefix_match0_tbl_set (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_longest_prefix_match0_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_LONGEST_PREFIX_MATCH0_TBL_DATA
    prm_IHP_longest_prefix_match0_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_longest_prefix_match0_tbl_set";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_LONGEST_PREFIX_MATCH0_TBL_SET_IHP_LONGEST_PREFIX_MATCH0_TBL_SET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_LONGEST_PREFIX_MATCH0_TBL_DATA_clear(&prm_IHP_longest_prefix_match0_tbl_data);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_petra_pp_ihp_longest_prefix_match0_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_longest_prefix_match0_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_longest_prefix_match0_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_longest_prefix_match0_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_LONGEST_PREFIX_MATCH0_TBL_SET_IHP_LONGEST_PREFIX_MATCH0_TBL_SET_IHP_LONGEST_PREFIX_MATCH0_TBL_DATA_LONGEST_PREFIX_MATCH0_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_LONGEST_PREFIX_MATCH0_TBL_SET_IHP_LONGEST_PREFIX_MATCH0_TBL_SET_IHP_LONGEST_PREFIX_MATCH0_TBL_DATA_LONGEST_PREFIX_MATCH0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_longest_prefix_match0_tbl_data.longest_prefix_match0 = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = soc_petra_pp_ihp_longest_prefix_match0_tbl_set_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_longest_prefix_match0_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_longest_prefix_match0_tbl_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_longest_prefix_match0_tbl_set");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_longest_prefix_match0_tbl_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_longest_prefix_match0_tbl_get (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_longest_prefix_match0_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_LONGEST_PREFIX_MATCH0_TBL_DATA
    prm_IHP_longest_prefix_match0_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_longest_prefix_match0_tbl_get";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_LONGEST_PREFIX_MATCH0_TBL_GET_IHP_LONGEST_PREFIX_MATCH0_TBL_GET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_LONGEST_PREFIX_MATCH0_TBL_DATA_clear(&prm_IHP_longest_prefix_match0_tbl_data);

  /* Get parameters */

  /* Call function */
  ret = soc_petra_pp_ihp_longest_prefix_match0_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_longest_prefix_match0_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_longest_prefix_match0_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_longest_prefix_match0_tbl_get");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_longest_prefix_match0_tbl_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> IHP_longest_prefix_match0_tbl_data:", TRUE);
  soc_petra_pp_PETRA_PP_IHP_LONGEST_PREFIX_MATCH0_TBL_DATA_print(&prm_IHP_longest_prefix_match0_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_longest_prefix_match1_tbl_set (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_longest_prefix_match1_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_LONGEST_PREFIX_MATCH1_TBL_DATA
    prm_IHP_longest_prefix_match1_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_longest_prefix_match1_tbl_set";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_LONGEST_PREFIX_MATCH1_TBL_SET_IHP_LONGEST_PREFIX_MATCH1_TBL_SET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_LONGEST_PREFIX_MATCH1_TBL_DATA_clear(&prm_IHP_longest_prefix_match1_tbl_data);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_petra_pp_ihp_longest_prefix_match1_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_longest_prefix_match1_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_longest_prefix_match1_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_longest_prefix_match1_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_LONGEST_PREFIX_MATCH1_TBL_SET_IHP_LONGEST_PREFIX_MATCH1_TBL_SET_IHP_LONGEST_PREFIX_MATCH1_TBL_DATA_LONGEST_PREFIX_MATCH1_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_LONGEST_PREFIX_MATCH1_TBL_SET_IHP_LONGEST_PREFIX_MATCH1_TBL_SET_IHP_LONGEST_PREFIX_MATCH1_TBL_DATA_LONGEST_PREFIX_MATCH1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_longest_prefix_match1_tbl_data.longest_prefix_match1 = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = soc_petra_pp_ihp_longest_prefix_match1_tbl_set_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_longest_prefix_match1_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_longest_prefix_match1_tbl_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_longest_prefix_match1_tbl_set");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_longest_prefix_match1_tbl_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_longest_prefix_match1_tbl_get (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_longest_prefix_match1_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_LONGEST_PREFIX_MATCH1_TBL_DATA
    prm_IHP_longest_prefix_match1_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_longest_prefix_match1_tbl_get";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_LONGEST_PREFIX_MATCH1_TBL_GET_IHP_LONGEST_PREFIX_MATCH1_TBL_GET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_LONGEST_PREFIX_MATCH1_TBL_DATA_clear(&prm_IHP_longest_prefix_match1_tbl_data);

  /* Get parameters */

  /* Call function */
  ret = soc_petra_pp_ihp_longest_prefix_match1_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_longest_prefix_match1_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_longest_prefix_match1_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_longest_prefix_match1_tbl_get");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_longest_prefix_match1_tbl_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> IHP_longest_prefix_match1_tbl_data:", TRUE);
  soc_petra_pp_PETRA_PP_IHP_LONGEST_PREFIX_MATCH1_TBL_DATA_print(&prm_IHP_longest_prefix_match1_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_longest_prefix_match2_tbl_set (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_longest_prefix_match2_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_LONGEST_PREFIX_MATCH2_TBL_DATA
    prm_IHP_longest_prefix_match2_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_longest_prefix_match2_tbl_set";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_LONGEST_PREFIX_MATCH2_TBL_SET_IHP_LONGEST_PREFIX_MATCH2_TBL_SET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_LONGEST_PREFIX_MATCH2_TBL_DATA_clear(&prm_IHP_longest_prefix_match2_tbl_data);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_petra_pp_ihp_longest_prefix_match2_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_longest_prefix_match2_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_longest_prefix_match2_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_longest_prefix_match2_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_LONGEST_PREFIX_MATCH2_TBL_SET_IHP_LONGEST_PREFIX_MATCH2_TBL_SET_IHP_LONGEST_PREFIX_MATCH2_TBL_DATA_LONGEST_PREFIX_MATCH2_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_LONGEST_PREFIX_MATCH2_TBL_SET_IHP_LONGEST_PREFIX_MATCH2_TBL_SET_IHP_LONGEST_PREFIX_MATCH2_TBL_DATA_LONGEST_PREFIX_MATCH2_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_longest_prefix_match2_tbl_data.longest_prefix_match2 = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = soc_petra_pp_ihp_longest_prefix_match2_tbl_set_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_longest_prefix_match2_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_longest_prefix_match2_tbl_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_longest_prefix_match2_tbl_set");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_longest_prefix_match2_tbl_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_longest_prefix_match2_tbl_get (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_longest_prefix_match2_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_LONGEST_PREFIX_MATCH2_TBL_DATA
    prm_IHP_longest_prefix_match2_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_longest_prefix_match2_tbl_get";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_LONGEST_PREFIX_MATCH2_TBL_GET_IHP_LONGEST_PREFIX_MATCH2_TBL_GET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_LONGEST_PREFIX_MATCH2_TBL_DATA_clear(&prm_IHP_longest_prefix_match2_tbl_data);

  /* Get parameters */

  /* Call function */
  ret = soc_petra_pp_ihp_longest_prefix_match2_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_longest_prefix_match2_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_longest_prefix_match2_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_longest_prefix_match2_tbl_get");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_longest_prefix_match2_tbl_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> IHP_longest_prefix_match2_tbl_data:", TRUE);
  soc_petra_pp_PETRA_PP_IHP_LONGEST_PREFIX_MATCH2_TBL_DATA_print(&prm_IHP_longest_prefix_match2_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_tcam_action_bank_a_tbl_set (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_tcam_action_bank_a_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_TCAM_ACTION_BANK_BUFFER_TBL_DATA
    prm_IHP_tcam_action_bank_a_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_tcam_action_bank_a_tbl_set";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_ACTION_BANK_A_TBL_SET_IHP_TCAM_ACTION_BANK_A_TBL_SET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;


/*  soc_petra_pp_PETRA_PP_IHP_TCAM_ACTION_BANK_A_TBL_DATA_clear(&prm_IHP_tcam_action_bank_a_tbl_data);*/

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_petra_pp_ihp_tcam_action_bank_buffer_tbl_get_unsafe(
          unit,
          0,
          prm_entry_ndx,
          &prm_IHP_tcam_action_bank_a_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_tcam_action_bank_a_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_tcam_action_bank_a_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_ACTION_BANK_A_TBL_SET_IHP_TCAM_ACTION_BANK_A_TBL_SET_IHP_TCAM_ACTION_BANK_A_TBL_DATA_TCAM_ACTION_BANK_A_0_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_ACTION_BANK_A_TBL_SET_IHP_TCAM_ACTION_BANK_A_TBL_SET_IHP_TCAM_ACTION_BANK_A_TBL_DATA_TCAM_ACTION_BANK_A_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_tcam_action_bank_a_tbl_data.tcam_action[0] = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = soc_petra_pp_ihp_tcam_action_bank_buffer_tbl_set_unsafe(
          unit,
          0,
          prm_entry_ndx,
          &prm_IHP_tcam_action_bank_a_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_tcam_action_bank_a_tbl_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_tcam_action_bank_a_tbl_set");
    goto exit;
  }

  soc_sand_os_printf( "tcam_action[0]: %08x \n ", prm_IHP_tcam_action_bank_a_tbl_data.tcam_action[0]);
  soc_sand_os_printf( "tcam_action[1]: %08x \n ", prm_IHP_tcam_action_bank_a_tbl_data.tcam_action[1]);

  send_string_to_screen(" *** soc_petra_pp_ihp_tcam_action_bank_a_tbl_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_tcam_action_bank_a_tbl_get (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_tcam_action_bank_a_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx,
    bank_id;
  SOC_PETRA_PP_IHP_TCAM_ACTION_BANK_BUFFER_TBL_DATA
    prm_IHP_tcam_action_bank_a_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_tcam_action_bank_a_tbl_get";

  prm_IHP_tcam_action_bank_a_tbl_data.tcam_action[0] = 0;
  prm_IHP_tcam_action_bank_a_tbl_data.tcam_action[1] = 0;


  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_ACTION_BANK_A_TBL_GET_IHP_TCAM_ACTION_BANK_A_TBL_GET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_ACTION_BANK_A_TBL_GET_IHP_TCAM_ACTION_BANK_A_TBL_GET_BANK_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_ACTION_BANK_A_TBL_GET_IHP_TCAM_ACTION_BANK_A_TBL_GET_BANK_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    bank_id = (uint32)param_val->value.ulong_value;
  }


  if (bank_id > 3)
  {
    goto exit;
  }

/*  soc_petra_pp_PETRA_PP_IHP_TCAM_ACTION_BANK_A_TBL_DATA_clear(&prm_IHP_tcam_action_bank_a_tbl_data);*/

  /* Get parameters */

  /* Call function */
  ret = soc_petra_pp_ihp_tcam_action_bank_buffer_tbl_get_unsafe(
          unit,
          bank_id,
          prm_entry_ndx,
          &prm_IHP_tcam_action_bank_a_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_tcam_action_bank_a_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_tcam_action_bank_a_tbl_get");
    goto exit;
  }

  soc_sand_os_printf( "tcam_action[0]: %08x \n ", prm_IHP_tcam_action_bank_a_tbl_data.tcam_action[0]);
  soc_sand_os_printf( "tcam_action[1]: %08x \n ", prm_IHP_tcam_action_bank_a_tbl_data.tcam_action[1]);
  send_string_to_screen(" *** soc_petra_pp_ihp_tcam_action_bank_a_tbl_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> IHP_tcam_action_bank_a_tbl_data:", TRUE);
/*  soc_petra_pp_PETRA_PP_IHP_TCAM_ACTION_BANK_A_TBL_DATA_print(&prm_IHP_tcam_action_bank_a_tbl_data);*/


  goto exit;
exit:
  return ui_ret;
}


/********************************************************************
 *  Function handler: ihp_tcam_action_bank_b_tbl_set (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_tcam_action_bank_b_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_TCAM_ACTION_BANK_B_TBL_DATA
    prm_IHP_tcam_action_bank_b_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_tcam_action_bank_b_tbl_set";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_ACTION_BANK_B_TBL_SET_IHP_TCAM_ACTION_BANK_B_TBL_SET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_TCAM_ACTION_BANK_B_TBL_DATA_clear(&prm_IHP_tcam_action_bank_b_tbl_data);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_petra_pp_ihp_tcam_action_bank_b_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_tcam_action_bank_b_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_tcam_action_bank_b_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_tcam_action_bank_b_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_ACTION_BANK_B_TBL_SET_IHP_TCAM_ACTION_BANK_B_TBL_SET_IHP_TCAM_ACTION_BANK_B_TBL_DATA_TCAM_ACTION_BANK_B_0_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_ACTION_BANK_B_TBL_SET_IHP_TCAM_ACTION_BANK_B_TBL_SET_IHP_TCAM_ACTION_BANK_B_TBL_DATA_TCAM_ACTION_BANK_B_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_tcam_action_bank_b_tbl_data.tcam_action_bank_b[0] = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = soc_petra_pp_ihp_tcam_action_bank_b_tbl_set_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_tcam_action_bank_b_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_tcam_action_bank_b_tbl_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_tcam_action_bank_b_tbl_set");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_tcam_action_bank_b_tbl_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_tcam_action_bank_b_tbl_get (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_tcam_action_bank_b_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_TCAM_ACTION_BANK_B_TBL_DATA
    prm_IHP_tcam_action_bank_b_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_tcam_action_bank_b_tbl_get";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_ACTION_BANK_B_TBL_GET_IHP_TCAM_ACTION_BANK_B_TBL_GET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_TCAM_ACTION_BANK_B_TBL_DATA_clear(&prm_IHP_tcam_action_bank_b_tbl_data);

  /* Get parameters */

  /* Call function */
  ret = soc_petra_pp_ihp_tcam_action_bank_b_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_tcam_action_bank_b_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_tcam_action_bank_b_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_tcam_action_bank_b_tbl_get");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_tcam_action_bank_b_tbl_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> IHP_tcam_action_bank_b_tbl_data:", TRUE);
  soc_petra_pp_PETRA_PP_IHP_TCAM_ACTION_BANK_B_TBL_DATA_print(&prm_IHP_tcam_action_bank_b_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_tcam_action_bank_c_tbl_set (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_tcam_action_bank_c_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_TCAM_ACTION_BANK_C_TBL_DATA
    prm_IHP_tcam_action_bank_c_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_tcam_action_bank_c_tbl_set";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_ACTION_BANK_C_TBL_SET_IHP_TCAM_ACTION_BANK_C_TBL_SET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_TCAM_ACTION_BANK_C_TBL_DATA_clear(&prm_IHP_tcam_action_bank_c_tbl_data);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_petra_pp_ihp_tcam_action_bank_c_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_tcam_action_bank_c_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_tcam_action_bank_c_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_tcam_action_bank_c_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_ACTION_BANK_C_TBL_SET_IHP_TCAM_ACTION_BANK_C_TBL_SET_IHP_TCAM_ACTION_BANK_C_TBL_DATA_TCAM_ACTION_BANK_C_0_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_ACTION_BANK_C_TBL_SET_IHP_TCAM_ACTION_BANK_C_TBL_SET_IHP_TCAM_ACTION_BANK_C_TBL_DATA_TCAM_ACTION_BANK_C_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_tcam_action_bank_c_tbl_data.tcam_action_bank_c[0] = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = soc_petra_pp_ihp_tcam_action_bank_c_tbl_set_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_tcam_action_bank_c_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_tcam_action_bank_c_tbl_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_tcam_action_bank_c_tbl_set");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_tcam_action_bank_c_tbl_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_tcam_action_bank_c_tbl_get (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_tcam_action_bank_c_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_TCAM_ACTION_BANK_C_TBL_DATA
    prm_IHP_tcam_action_bank_c_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_tcam_action_bank_c_tbl_get";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_ACTION_BANK_C_TBL_GET_IHP_TCAM_ACTION_BANK_C_TBL_GET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_TCAM_ACTION_BANK_C_TBL_DATA_clear(&prm_IHP_tcam_action_bank_c_tbl_data);

  /* Get parameters */

  /* Call function */
  ret = soc_petra_pp_ihp_tcam_action_bank_c_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_tcam_action_bank_c_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_tcam_action_bank_c_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_tcam_action_bank_c_tbl_get");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_tcam_action_bank_c_tbl_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> IHP_tcam_action_bank_c_tbl_data:", TRUE);
  soc_petra_pp_PETRA_PP_IHP_TCAM_ACTION_BANK_C_TBL_DATA_print(&prm_IHP_tcam_action_bank_c_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_tcam_action_bank_d_tbl_set (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_tcam_action_bank_d_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_TCAM_ACTION_BANK_D_TBL_DATA
    prm_IHP_tcam_action_bank_d_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_tcam_action_bank_d_tbl_set";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_ACTION_BANK_D_TBL_SET_IHP_TCAM_ACTION_BANK_D_TBL_SET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_TCAM_ACTION_BANK_D_TBL_DATA_clear(&prm_IHP_tcam_action_bank_d_tbl_data);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_petra_pp_ihp_tcam_action_bank_d_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_tcam_action_bank_d_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_tcam_action_bank_d_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_tcam_action_bank_d_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_ACTION_BANK_D_TBL_SET_IHP_TCAM_ACTION_BANK_D_TBL_SET_IHP_TCAM_ACTION_BANK_D_TBL_DATA_TCAM_ACTION_BANK_D_0_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_ACTION_BANK_D_TBL_SET_IHP_TCAM_ACTION_BANK_D_TBL_SET_IHP_TCAM_ACTION_BANK_D_TBL_DATA_TCAM_ACTION_BANK_D_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_tcam_action_bank_d_tbl_data.tcam_action_bank_d[0] = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = soc_petra_pp_ihp_tcam_action_bank_d_tbl_set_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_tcam_action_bank_d_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_tcam_action_bank_d_tbl_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_tcam_action_bank_d_tbl_set");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_tcam_action_bank_d_tbl_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_tcam_action_bank_d_tbl_get (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_tcam_action_bank_d_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_TCAM_ACTION_BANK_D_TBL_DATA
    prm_IHP_tcam_action_bank_d_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_tcam_action_bank_d_tbl_get";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_ACTION_BANK_D_TBL_GET_IHP_TCAM_ACTION_BANK_D_TBL_GET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_TCAM_ACTION_BANK_D_TBL_DATA_clear(&prm_IHP_tcam_action_bank_d_tbl_data);

  /* Get parameters */

  /* Call function */
  ret = soc_petra_pp_ihp_tcam_action_bank_d_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_tcam_action_bank_d_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_tcam_action_bank_d_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_tcam_action_bank_d_tbl_get");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_tcam_action_bank_d_tbl_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> IHP_tcam_action_bank_d_tbl_data:", TRUE);
  soc_petra_pp_PETRA_PP_IHP_TCAM_ACTION_BANK_D_TBL_DATA_print(&prm_IHP_tcam_action_bank_d_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_tos_to_cos_tbl_set (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_tos_to_cos_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_TOS_TO_COS_TBL_DATA
    prm_IHP_tos_to_cos_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_tos_to_cos_tbl_set";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_TOS_TO_COS_TBL_SET_IHP_TOS_TO_COS_TBL_SET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_TOS_TO_COS_TBL_DATA_clear(&prm_IHP_tos_to_cos_tbl_data);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_petra_pp_ihp_tos_to_cos_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_tos_to_cos_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_tos_to_cos_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_tos_to_cos_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_TOS_TO_COS_TBL_SET_IHP_TOS_TO_COS_TBL_SET_IHP_TOS_TO_COS_TBL_DATA_TOS_TO_COS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_TOS_TO_COS_TBL_SET_IHP_TOS_TO_COS_TBL_SET_IHP_TOS_TO_COS_TBL_DATA_TOS_TO_COS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_tos_to_cos_tbl_data.tos_to_cos = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = soc_petra_pp_ihp_tos_to_cos_tbl_set_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_tos_to_cos_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_tos_to_cos_tbl_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_tos_to_cos_tbl_set");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_tos_to_cos_tbl_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_tos_to_cos_tbl_get (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_tos_to_cos_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_TOS_TO_COS_TBL_DATA
    prm_IHP_tos_to_cos_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_tos_to_cos_tbl_get";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_TOS_TO_COS_TBL_GET_IHP_TOS_TO_COS_TBL_GET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_TOS_TO_COS_TBL_DATA_clear(&prm_IHP_tos_to_cos_tbl_data);

  /* Get parameters */

  /* Call function */
  ret = soc_petra_pp_ihp_tos_to_cos_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_tos_to_cos_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_tos_to_cos_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_tos_to_cos_tbl_get");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_tos_to_cos_tbl_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> IHP_tos_to_cos_tbl_data:", TRUE);
  soc_petra_pp_PETRA_PP_IHP_TOS_TO_COS_TBL_DATA_print(&prm_IHP_tos_to_cos_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_fec_accessed_table_tbl_set (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_fec_accessed_table_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_FEC_ACCESSED_TABLE_TBL_DATA
    prm_IHP_fec_accessed_table_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_fec_accessed_table_tbl_set";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_FEC_ACCESSED_TABLE_TBL_SET_IHP_FEC_ACCESSED_TABLE_TBL_SET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_FEC_ACCESSED_TABLE_TBL_DATA_clear(&prm_IHP_fec_accessed_table_tbl_data);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_petra_pp_ihp_fec_accessed_table_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_fec_accessed_table_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_fec_accessed_table_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_fec_accessed_table_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_FEC_ACCESSED_TABLE_TBL_SET_IHP_FEC_ACCESSED_TABLE_TBL_SET_IHP_FEC_ACCESSED_TABLE_TBL_DATA_FEC_ACCESSED_TABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_FEC_ACCESSED_TABLE_TBL_SET_IHP_FEC_ACCESSED_TABLE_TBL_SET_IHP_FEC_ACCESSED_TABLE_TBL_DATA_FEC_ACCESSED_TABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_fec_accessed_table_tbl_data.fec_accessed_table[0] = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = soc_petra_pp_ihp_fec_accessed_table_tbl_set_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_fec_accessed_table_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_fec_accessed_table_tbl_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_fec_accessed_table_tbl_set");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_fec_accessed_table_tbl_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_fec_accessed_table_tbl_get (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_fec_accessed_table_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_FEC_ACCESSED_TABLE_TBL_DATA
    prm_IHP_fec_accessed_table_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_fec_accessed_table_tbl_get";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_FEC_ACCESSED_TABLE_TBL_GET_IHP_FEC_ACCESSED_TABLE_TBL_GET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_FEC_ACCESSED_TABLE_TBL_DATA_clear(&prm_IHP_fec_accessed_table_tbl_data);

  /* Get parameters */

  /* Call function */
  ret = soc_petra_pp_ihp_fec_accessed_table_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_fec_accessed_table_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_fec_accessed_table_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_fec_accessed_table_tbl_get");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_fec_accessed_table_tbl_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> IHP_fec_accessed_table_tbl_data:", TRUE);
  soc_petra_pp_PETRA_PP_IHP_FEC_ACCESSED_TABLE_TBL_DATA_print(&prm_IHP_fec_accessed_table_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_ipv4_stat_table_tbl_set (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_ipv4_stat_table_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_IPV4_STAT_TABLE_TBL_DATA
    prm_IHP_ipv4_stat_table_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_ipv4_stat_table_tbl_set";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_IPV4_STAT_TABLE_TBL_SET_IHP_IPV4_STAT_TABLE_TBL_SET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_IPV4_STAT_TABLE_TBL_DATA_clear(&prm_IHP_ipv4_stat_table_tbl_data);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_petra_pp_ihp_ipv4_stat_table_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_ipv4_stat_table_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_ipv4_stat_table_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_ipv4_stat_table_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_IPV4_STAT_TABLE_TBL_SET_IHP_IPV4_STAT_TABLE_TBL_SET_IHP_IPV4_STAT_TABLE_TBL_DATA_COUNTER_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_IPV4_STAT_TABLE_TBL_SET_IHP_IPV4_STAT_TABLE_TBL_SET_IHP_IPV4_STAT_TABLE_TBL_DATA_COUNTER_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_ipv4_stat_table_tbl_data.counter_val = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = soc_petra_pp_ihp_ipv4_stat_table_tbl_set_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_ipv4_stat_table_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_ipv4_stat_table_tbl_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_ipv4_stat_table_tbl_set");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_ipv4_stat_table_tbl_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_ipv4_stat_table_tbl_get (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_ipv4_stat_table_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_IPV4_STAT_TABLE_TBL_DATA
    prm_IHP_ipv4_stat_table_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_ipv4_stat_table_tbl_get";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_IPV4_STAT_TABLE_TBL_GET_IHP_IPV4_STAT_TABLE_TBL_GET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_IPV4_STAT_TABLE_TBL_DATA_clear(&prm_IHP_ipv4_stat_table_tbl_data);

  /* Get parameters */

  /* Call function */
  ret = soc_petra_pp_ihp_ipv4_stat_table_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_ipv4_stat_table_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_ipv4_stat_table_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_ipv4_stat_table_tbl_get");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_ipv4_stat_table_tbl_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> IHP_ipv4_stat_table_tbl_data:", TRUE);
  soc_petra_pp_PETRA_PP_IHP_IPV4_STAT_TABLE_TBL_DATA_print(&prm_IHP_ipv4_stat_table_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_tcam_bank_a_tbl_set (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_tcam_bank_a_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_TCAM_BANK_A_TBL_DATA
    prm_IHP_tcam_bank_a_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_tcam_bank_a_tbl_set";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_BANK_A_TBL_SET_IHP_TCAM_BANK_A_TBL_SET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_TCAM_BANK_A_TBL_DATA_clear(&prm_IHP_tcam_bank_a_tbl_data);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_petra_pp_ihp_tcam_bank_a_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_tcam_bank_a_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_tcam_bank_a_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_tcam_bank_a_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_BANK_A_TBL_SET_IHP_TCAM_BANK_A_TBL_SET_IHP_TCAM_BANK_A_TBL_DATA_TCAM_BANK_A_0_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_BANK_A_TBL_SET_IHP_TCAM_BANK_A_TBL_SET_IHP_TCAM_BANK_A_TBL_DATA_TCAM_BANK_A_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_tcam_bank_a_tbl_data.tcam_bank_a[0] = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = soc_petra_pp_ihp_tcam_bank_a_tbl_set_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_tcam_bank_a_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_tcam_bank_a_tbl_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_tcam_bank_a_tbl_set");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_tcam_bank_a_tbl_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_tcam_bank_a_tbl_get (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_tcam_bank_a_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_TCAM_BANK_A_TBL_DATA
    prm_IHP_tcam_bank_a_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_tcam_bank_a_tbl_get";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_BANK_A_TBL_GET_IHP_TCAM_BANK_A_TBL_GET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_TCAM_BANK_A_TBL_DATA_clear(&prm_IHP_tcam_bank_a_tbl_data);

  /* Get parameters */

  /* Call function */
  ret = soc_petra_pp_ihp_tcam_bank_a_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_tcam_bank_a_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_tcam_bank_a_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_tcam_bank_a_tbl_get");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_tcam_bank_a_tbl_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> IHP_tcam_bank_a_tbl_data:", TRUE);
  soc_petra_pp_PETRA_PP_IHP_TCAM_BANK_A_TBL_DATA_print(&prm_IHP_tcam_bank_a_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: tcam_read (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_tcam_read(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx,
    bank_ndx,
    up_to,
    print_content,
    indx,
    nof_entries_in_line,
    nof_fails;

  SOC_PETRA_PP_IHP_TCAM_BANK_TBL_DATA
    data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_petra_pp_PETRA_PP_IHP_TCAM_BANK_TBL_DATA_clear(&data);
  prm_entry_ndx = 0;
  bank_ndx = 0;
  nof_entries_in_line = 1;
  print_content = 2;
  nof_fails = 0;

  soc_sand_proc_name = "soc_petra_pp_ihp_tcam_action_bank_a_tbl_get";

  unit = soc_petra_get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_TCAM_READ_BANK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_TCAM_READ_BANK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    bank_ndx = ((uint32)param_val->value.ulong_value);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_TCAM_READ_ENTRY_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_TCAM_READ_ENTRY_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_ndx = ((uint32)param_val->value.ulong_value);
    up_to = prm_entry_ndx;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_TCAM_READ_PRINT_CONT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_TCAM_READ_PRINT_CONT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    print_content = ((uint32)param_val->value.ulong_value);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_TCAM_READ_NOF_ENTRIES_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_TCAM_READ_NOF_ENTRIES_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    nof_entries_in_line = ((uint32)param_val->value.ulong_value);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_TCAM_READ_UP_TO_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_TCAM_READ_UP_TO_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    up_to = ((uint32)param_val->value.ulong_value);
  }


  for (indx = prm_entry_ndx;  indx<= up_to; ++indx)
  {
    /* Call function */
    ret = soc_petra_pp_ihp_tcam_tbl_read_unsafe(
            unit,
            bank_ndx,
            nof_entries_in_line,
            indx,
            &data
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** soc_petra_pp_ihp_tcam_tbl_read_unsafe - FAIL", TRUE);
      soc_petra_disp_result(ret, "soc_petra_pp_ihp_tcam_tbl_read_unsafe");
      goto exit;
    }
    if (!data.valid)
    {
      if (print_content == 1)
      {
        soc_sand_os_printf( "invaled at: %lu", indx);
      }
      nof_fails++;
    }

    if (print_content == 2)
    {
      soc_petra_pp_PETRA_PP_IHP_TCAM_BANK_TBL_DATA_print(&data);
    }
  }

  soc_sand_os_printf( "number of invaled entries: %lu", nof_fails);


  goto exit;
exit:
  return ui_ret;
}


/********************************************************************
 *  Function handler: tcam_write (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_tcam_write(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx,
    indx,
    bank_ndx,
    nof_entries_in_line;
  SOC_PETRA_PP_IHP_TCAM_BANK_TBL_DATA
    data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_petra_pp_PETRA_PP_IHP_TCAM_BANK_TBL_DATA_clear(&data);
  prm_entry_ndx = 0;
  bank_ndx = 0;
  nof_entries_in_line = 1;
  data.valid = 1;


  soc_sand_proc_name = "soc_petra_pp_ihp_tcam_action_bank_a_tbl_get";

  unit = soc_petra_get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_TCAM_WRITE_BANK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_TCAM_WRITE_BANK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    bank_ndx = ((uint32)param_val->value.ulong_value);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_TCAM_WRITE_ENTRY_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_TCAM_WRITE_ENTRY_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_ndx = ((uint32)param_val->value.ulong_value);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_TCAM_WRITE_NOF_ENTRIES_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_TCAM_WRITE_NOF_ENTRIES_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    nof_entries_in_line = ((uint32)param_val->value.ulong_value);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_TCAM_WRITE_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_TCAM_WRITE_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.valid = ((uint32)param_val->value.ulong_value);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_TCAM_WRITE_VALUE_ID,1))
  {
    for (indx = 0; indx < 80;  ++indx, ui_ret = 0)
    {
      UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(PARAM_PETRA_PP_TCAM_WRITE_VALUE_ID, indx + 1);
      if (ui_ret)
      {
        ui_ret = 0;
        break;
      }
      data.value[indx] = (uint32)param_val->value.ulong_value;
    }
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_TCAM_WRITE_MASK_ID,1))
  {
    for (indx = 0; indx < 80;  ++indx, ui_ret = 0)
    {
      UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(PARAM_PETRA_PP_TCAM_WRITE_MASK_ID, indx + 1);
      if (ui_ret)
      {
        ui_ret = 0;
        break;
      }
      data.mask[indx] = (uint32)param_val->value.ulong_value;
    }
  }

  /* Call function */
  ret = soc_petra_pp_ihp_tcam_tbl_write_unsafe(
          unit,
          bank_ndx,
          nof_entries_in_line,
          prm_entry_ndx,
          &data
        );

  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_tcam_tbl_write_unsafe - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_tcam_tbl_write_unsafe");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_tcam_tbl_write_unsafe - SUCCEEDED", TRUE);
  send_string_to_screen("--> soc_petra_pp_ihp_tcam_tbl_write_unsafe:", TRUE);
  soc_petra_pp_PETRA_PP_IHP_TCAM_BANK_TBL_DATA_print(&data);


  goto exit;
exit:
  return ui_ret;
}
/********************************************************************
 *  Function handler: ihp_tcam_bank_c_tbl_set (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_tcam_flush(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    bank_ndx;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_api_ihp_tcam_flush";

  bank_ndx = 0;
  unit = soc_petra_get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_TCAM_FLUSH_BANK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_TCAM_FLUSH_BANK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    bank_ndx = ((uint32)param_val->value.ulong_value);
  }

  /* Call function */
  ret = soc_petra_pp_ihp_tcam_tbl_flush_unsafe(
          unit,
          bank_ndx
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_api_ihp_tcam_flush - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_api_ihp_tcam_flush");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_api_ihp_tcam_flush - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_tcam_bank_c_tbl_get (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_tcam_compare(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    indx,
    bank_ndx,
    nof_entries_in_line;
  SOC_PETRA_PP_IHP_TCAM_BANK_TBL_DATA
    data;
  SOC_PETRA_PP_IHP_TCAM_COMPARE_DATA
    found_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_petra_pp_PETRA_PP_IHP_TCAM_BANK_TBL_DATA_clear(&data);

  soc_sand_proc_name = "soc_petra_pp_ihp_tcam_action_bank_a_tbl_get";
  bank_ndx = 0;
  nof_entries_in_line = 1;
  data.valid = 1;

  unit = soc_petra_get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_TCAM_COMPARE_BANK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_TCAM_COMPARE_BANK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    bank_ndx = ((uint32)param_val->value.ulong_value);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_TCAM_COMPARE_NOF_ENTRIES_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_TCAM_COMPARE_NOF_ENTRIES_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    nof_entries_in_line = ((uint32)param_val->value.ulong_value);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_TCAM_COMPARE_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_TCAM_COMPARE_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    data.valid = ((uint32)param_val->value.ulong_value);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_TCAM_COMPARE_VALUE_ID,1))
  {
    for (indx = 0; indx < 80;  ++indx, ui_ret = 0)
    {
      UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(PARAM_PETRA_PP_TCAM_COMPARE_VALUE_ID, indx + 1);
      if (ui_ret)
      {
        ui_ret = 0;
        break;
      }
      data.value[indx] = (uint32)param_val->value.ulong_value;
    }
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_TCAM_COMPARE_MASK_ID,1))
  {
    for (indx = 0; indx < 80;  ++indx, ui_ret = 0)
    {
      UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(PARAM_PETRA_PP_TCAM_COMPARE_MASK_ID, indx + 1);
      if (ui_ret)
      {
        ui_ret = 0;
        break;
      }
      data.mask[indx] = (uint32)param_val->value.ulong_value;
    }
  }

  /* Call function */
  ret = soc_petra_pp_ihp_tcam_tbl_compare_unsafe(
          unit,
          bank_ndx,
          nof_entries_in_line,
          &data,
          &found_data
        );

  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_tcam_tbl_compare_unsafe - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_tcam_tbl_compare_unsafe");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_tcam_tbl_compare_unsafe - SUCCEEDED", TRUE);
  send_string_to_screen("--> soc_petra_pp_ihp_tcam_tbl_compare_unsafe:", TRUE);
  soc_petra_pp_PETRA_PP_IHP_TCAM_BANK_TBL_DATA_print(&data);
  soc_petra_pp_PETRA_PP_IHP_TCAM_COMPARE_DATA_print(&found_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_tcam_bank_d_tbl_set (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_tcam_bank_d_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_TCAM_BANK_D_TBL_DATA
    prm_IHP_tcam_bank_d_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_tcam_bank_d_tbl_set";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_BANK_D_TBL_SET_IHP_TCAM_BANK_D_TBL_SET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_TCAM_BANK_D_TBL_DATA_clear(&prm_IHP_tcam_bank_d_tbl_data);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_petra_pp_ihp_tcam_bank_d_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_tcam_bank_d_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_tcam_bank_d_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_tcam_bank_d_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_BANK_D_TBL_SET_IHP_TCAM_BANK_D_TBL_SET_IHP_TCAM_BANK_D_TBL_DATA_TCAM_BANK_D_0_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_BANK_D_TBL_SET_IHP_TCAM_BANK_D_TBL_SET_IHP_TCAM_BANK_D_TBL_DATA_TCAM_BANK_D_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_tcam_bank_d_tbl_data.tcam_bank_d[0] = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = soc_petra_pp_ihp_tcam_bank_d_tbl_set_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_tcam_bank_d_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_tcam_bank_d_tbl_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_tcam_bank_d_tbl_set");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_tcam_bank_d_tbl_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_tcam_bank_d_tbl_get (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_tcam_bank_d_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_TCAM_BANK_D_TBL_DATA
    prm_IHP_tcam_bank_d_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_tcam_bank_d_tbl_get";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_BANK_D_TBL_GET_IHP_TCAM_BANK_D_TBL_GET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_TCAM_BANK_D_TBL_DATA_clear(&prm_IHP_tcam_bank_d_tbl_data);

  /* Get parameters */

  /* Call function */
  ret = soc_petra_pp_ihp_tcam_bank_d_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_tcam_bank_d_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_tcam_bank_d_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_tcam_bank_d_tbl_get");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_tcam_bank_d_tbl_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> IHP_tcam_bank_d_tbl_data:", TRUE);
  soc_petra_pp_PETRA_PP_IHP_TCAM_BANK_D_TBL_DATA_print(&prm_IHP_tcam_bank_d_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_ipv6_tc_to_cos_tbl_set (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_ipv6_tc_to_cos_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_IPV6_TC_TO_COS_TBL_DATA
    prm_IHP_ipv6_tc_to_cos_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_ipv6_tc_to_cos_tbl_set";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_IPV6_TC_TO_COS_TBL_SET_IHP_IPV6_TC_TO_COS_TBL_SET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_IPV6_TC_TO_COS_TBL_DATA_clear(&prm_IHP_ipv6_tc_to_cos_tbl_data);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_petra_pp_ihp_ipv6_tc_to_cos_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_ipv6_tc_to_cos_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_ipv6_tc_to_cos_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_ipv6_tc_to_cos_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_IPV6_TC_TO_COS_TBL_SET_IHP_IPV6_TC_TO_COS_TBL_SET_IHP_IPV6_TC_TO_COS_TBL_DATA_IPV6_TC_TO_COS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_IPV6_TC_TO_COS_TBL_SET_IHP_IPV6_TC_TO_COS_TBL_SET_IHP_IPV6_TC_TO_COS_TBL_DATA_IPV6_TC_TO_COS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_ipv6_tc_to_cos_tbl_data.ipv6_tc_to_cos = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = soc_petra_pp_ihp_ipv6_tc_to_cos_tbl_set_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_ipv6_tc_to_cos_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_ipv6_tc_to_cos_tbl_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_ipv6_tc_to_cos_tbl_set");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_ipv6_tc_to_cos_tbl_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_ipv6_tc_to_cos_tbl_get (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_ipv6_tc_to_cos_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_IPV6_TC_TO_COS_TBL_DATA
    prm_IHP_ipv6_tc_to_cos_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_ipv6_tc_to_cos_tbl_get";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_IPV6_TC_TO_COS_TBL_GET_IHP_IPV6_TC_TO_COS_TBL_GET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_IPV6_TC_TO_COS_TBL_DATA_clear(&prm_IHP_ipv6_tc_to_cos_tbl_data);

  /* Get parameters */

  /* Call function */
  ret = soc_petra_pp_ihp_ipv6_tc_to_cos_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_ipv6_tc_to_cos_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_ipv6_tc_to_cos_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_ipv6_tc_to_cos_tbl_get");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_ipv6_tc_to_cos_tbl_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> IHP_ipv6_tc_to_cos_tbl_data:", TRUE);
  soc_petra_pp_PETRA_PP_IHP_IPV6_TC_TO_COS_TBL_DATA_print(&prm_IHP_ipv6_tc_to_cos_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_cid_subnet_tbl_set (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_cid_subnet_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_CID_SUBNET_TBL_DATA
    prm_IHP_cid_subnet_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_cid_subnet_tbl_set";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_CID_SUBNET_TBL_SET_IHP_CID_SUBNET_TBL_SET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_CID_SUBNET_TBL_DATA_clear(&prm_IHP_cid_subnet_tbl_data);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_petra_pp_ihp_cid_subnet_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_cid_subnet_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_cid_subnet_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_cid_subnet_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_CID_SUBNET_TBL_SET_IHP_CID_SUBNET_TBL_SET_IHP_CID_SUBNET_TBL_DATA_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_CID_SUBNET_TBL_SET_IHP_CID_SUBNET_TBL_SET_IHP_CID_SUBNET_TBL_DATA_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_cid_subnet_tbl_data.valid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_CID_SUBNET_TBL_SET_IHP_CID_SUBNET_TBL_SET_IHP_CID_SUBNET_TBL_DATA_CVLAN_UP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_CID_SUBNET_TBL_SET_IHP_CID_SUBNET_TBL_SET_IHP_CID_SUBNET_TBL_DATA_CVLAN_UP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_cid_subnet_tbl_data.cvlan_up = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_CID_SUBNET_TBL_SET_IHP_CID_SUBNET_TBL_SET_IHP_CID_SUBNET_TBL_DATA_TRAFFIC_CLASS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_CID_SUBNET_TBL_SET_IHP_CID_SUBNET_TBL_SET_IHP_CID_SUBNET_TBL_DATA_TRAFFIC_CLASS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_cid_subnet_tbl_data.traffic_class = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_CID_SUBNET_TBL_SET_IHP_CID_SUBNET_TBL_SET_IHP_CID_SUBNET_TBL_DATA_CID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_CID_SUBNET_TBL_SET_IHP_CID_SUBNET_TBL_SET_IHP_CID_SUBNET_TBL_DATA_CID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_cid_subnet_tbl_data.cid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_CID_SUBNET_TBL_SET_IHP_CID_SUBNET_TBL_SET_IHP_CID_SUBNET_TBL_DATA_MASK_MINUS_ONE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_CID_SUBNET_TBL_SET_IHP_CID_SUBNET_TBL_SET_IHP_CID_SUBNET_TBL_DATA_MASK_MINUS_ONE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_cid_subnet_tbl_data.mask_minus_one = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_CID_SUBNET_TBL_SET_IHP_CID_SUBNET_TBL_SET_IHP_CID_SUBNET_TBL_DATA_IP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_CID_SUBNET_TBL_SET_IHP_CID_SUBNET_TBL_SET_IHP_CID_SUBNET_TBL_DATA_IP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_cid_subnet_tbl_data.ip = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = soc_petra_pp_ihp_cid_subnet_tbl_set_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_cid_subnet_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_cid_subnet_tbl_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_cid_subnet_tbl_set");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_cid_subnet_tbl_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_cid_subnet_tbl_get (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_cid_subnet_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_CID_SUBNET_TBL_DATA
    prm_IHP_cid_subnet_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_cid_subnet_tbl_get";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_CID_SUBNET_TBL_GET_IHP_CID_SUBNET_TBL_GET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_CID_SUBNET_TBL_DATA_clear(&prm_IHP_cid_subnet_tbl_data);

  /* Get parameters */

  /* Call function */
  ret = soc_petra_pp_ihp_cid_subnet_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_cid_subnet_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_cid_subnet_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_cid_subnet_tbl_get");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_cid_subnet_tbl_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> IHP_cid_subnet_tbl_data:", TRUE);
  soc_petra_pp_PETRA_PP_IHP_CID_SUBNET_TBL_DATA_print(&prm_IHP_cid_subnet_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_port_da_sa_not_found_fwd_action_tbl_set (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_port_da_sa_not_found_fwd_action_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_DATA
    prm_IHP_port_da_sa_not_found_fwd_action_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_port_da_sa_not_found_fwd_action_tbl_set";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_SET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_DATA_clear(&prm_IHP_port_da_sa_not_found_fwd_action_tbl_data);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_petra_pp_ihp_port_da_sa_not_found_fwd_action_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_port_da_sa_not_found_fwd_action_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_port_da_sa_not_found_fwd_action_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_port_da_sa_not_found_fwd_action_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_DATA_ADD_CID_TO_DEST_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_DATA_ADD_CID_TO_DEST_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_port_da_sa_not_found_fwd_action_tbl_data.add_cid_to_dest = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_DATA_POLICER_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_DATA_POLICER_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_port_da_sa_not_found_fwd_action_tbl_data.policer = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_DATA_CPU_CODE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_DATA_CPU_CODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_port_da_sa_not_found_fwd_action_tbl_data.cpu_code = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_DATA_COS_OVERWRITE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_DATA_COS_OVERWRITE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_port_da_sa_not_found_fwd_action_tbl_data.cos_overwrite = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_DATA_COS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_DATA_COS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_port_da_sa_not_found_fwd_action_tbl_data.cos = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_DATA_DESTINATION_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_DATA_DESTINATION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_port_da_sa_not_found_fwd_action_tbl_data.destination = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_DATA_TYPE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_DATA_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_IHP_port_da_sa_not_found_fwd_action_tbl_data.type = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = soc_petra_pp_ihp_port_da_sa_not_found_fwd_action_tbl_set_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_port_da_sa_not_found_fwd_action_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_port_da_sa_not_found_fwd_action_tbl_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_port_da_sa_not_found_fwd_action_tbl_set");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_port_da_sa_not_found_fwd_action_tbl_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ihp_port_da_sa_not_found_fwd_action_tbl_get (section soc_petra_pp_ihp_api)
 */
int
  ui_petra_pp_ihp_api_ihp_port_da_sa_not_found_fwd_action_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_DATA
    prm_IHP_port_da_sa_not_found_fwd_action_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");
  soc_sand_proc_name = "soc_petra_pp_ihp_port_da_sa_not_found_fwd_action_tbl_get";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_GET_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_GET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_DATA_clear(&prm_IHP_port_da_sa_not_found_fwd_action_tbl_data);

  /* Get parameters */

  /* Call function */
  ret = soc_petra_pp_ihp_port_da_sa_not_found_fwd_action_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_IHP_port_da_sa_not_found_fwd_action_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_ihp_port_da_sa_not_found_fwd_action_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_ihp_port_da_sa_not_found_fwd_action_tbl_get");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_ihp_port_da_sa_not_found_fwd_action_tbl_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> IHP_port_da_sa_not_found_fwd_action_tbl_data:", TRUE);
  soc_petra_pp_PETRA_PP_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_DATA_print(&prm_IHP_port_da_sa_not_found_fwd_action_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: egq_pct_tbl_set (section soc_petra_pp_egq_api)
 */
int
  ui_petra_pp_egq_api_egq_pct_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_EGQ_PCT_TBL_DATA
    prm_EGQ_pct_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_egq_api");
  soc_sand_proc_name = "soc_petra_pp_egq_pct_tbl_set";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_SET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_EGQ_PCT_TBL_DATA_clear(&prm_EGQ_pct_tbl_data);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_petra_pp_egq_pct_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_EGQ_pct_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_egq_pct_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_egq_pct_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_DATA_ETHERNET_BRIDGE_PORT_TYPE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_DATA_ETHERNET_BRIDGE_PORT_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_EGQ_pct_tbl_data.ethernet_bridge_port_type = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_DATA_CEP_PVID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_DATA_CEP_PVID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_EGQ_pct_tbl_data.cep_pvid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_DATA_PEP_PVID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_DATA_PEP_PVID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_EGQ_pct_tbl_data.pep_pvid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_DATA_PEP_PORT_DEFAULT_PRIORITY_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_DATA_PEP_PORT_DEFAULT_PRIORITY_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_EGQ_pct_tbl_data.pep_port_default_priority = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_DATA_CEP_TX_TAGGED_CEP_PVID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_DATA_CEP_TX_TAGGED_CEP_PVID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_EGQ_pct_tbl_data.cep_tx_tagged_cep_pvid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_DATA_CEP_TX_TAGGED_PEP_PVID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_DATA_CEP_TX_TAGGED_PEP_PVID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_EGQ_pct_tbl_data.cep_tx_tagged_pep_pvid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_DATA_CEP_TX_TAGGED_OTHER_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_DATA_CEP_TX_TAGGED_OTHER_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_EGQ_pct_tbl_data.cep_tx_tagged_other = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_DATA_PORT_PCP_ENCODE_SELECT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_DATA_PORT_PCP_ENCODE_SELECT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_EGQ_pct_tbl_data.port_pcp_encode_select = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_DATA_SA_LSB_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_DATA_SA_LSB_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_EGQ_pct_tbl_data.sa_lsb = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = soc_petra_pp_egq_pct_tbl_set_unsafe(
          unit,
          prm_entry_ndx,
          &prm_EGQ_pct_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_egq_pct_tbl_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_egq_pct_tbl_set");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_egq_pct_tbl_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: egq_pct_tbl_get (section soc_petra_pp_egq_api)
 */
int
  ui_petra_pp_egq_api_egq_pct_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_EGQ_PCT_TBL_DATA
    prm_EGQ_pct_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_egq_api");
  soc_sand_proc_name = "soc_petra_pp_egq_pct_tbl_get";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_EGQ_PCT_TBL_GET_EGQ_PCT_TBL_GET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_EGQ_PCT_TBL_DATA_clear(&prm_EGQ_pct_tbl_data);

  /* Get parameters */

  /* Call function */
  ret = soc_petra_pp_egq_pct_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_EGQ_pct_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_egq_pct_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_egq_pct_tbl_get");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_egq_pct_tbl_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> EGQ_pct_tbl_data:", TRUE);
  soc_petra_pp_PETRA_PP_EGQ_PCT_TBL_DATA_print(&prm_EGQ_pct_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: egq_ppct_tbl_set (section soc_petra_pp_egq_api)
 */
int
  ui_petra_pp_egq_api_egq_ppct_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_EGQ_PPCT_TBL_DATA
    prm_EGQ_ppct_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_egq_api");
  soc_sand_proc_name = "soc_petra_pp_egq_ppct_tbl_set";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_EGQ_PPCT_TBL_DATA_clear(&prm_EGQ_ppct_tbl_data);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_petra_pp_egq_ppct_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_EGQ_ppct_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_egq_ppct_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_egq_ppct_tbl_get");
    goto exit;
  }


  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_ETHERNET_BRIDGE_PORT_TYPE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_ETHERNET_BRIDGE_PORT_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_EGQ_ppct_tbl_data.ethernet_bridge_port_type = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_EGRESS_VLAN_FILTERING_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_EGRESS_VLAN_FILTERING_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_EGQ_ppct_tbl_data.egress_vlan_filtering_enable = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_CEP_PVID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_CEP_PVID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_EGQ_ppct_tbl_data.cep_pvid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_CEP_ENABLE_EGRESS_FILTERING_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_CEP_ENABLE_EGRESS_FILTERING_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_EGQ_ppct_tbl_data.cep_enable_egress_filtering = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_CEP_PORT_CEP_PVID_MEMBER_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_CEP_PORT_CEP_PVID_MEMBER_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_EGQ_ppct_tbl_data.cep_port_cep_pvid_member = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_CEP_PORT_ALL_OTHERS_MEMBER_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_CEP_PORT_ALL_OTHERS_MEMBER_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_EGQ_ppct_tbl_data.cep_port_all_others_member = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_PEP_PVID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_PEP_PVID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_EGQ_ppct_tbl_data.pep_pvid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_PEP_ACCEPTABLE_FRAME_TYPES_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_PEP_ACCEPTABLE_FRAME_TYPES_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_EGQ_ppct_tbl_data.pep_acceptable_frame_types = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_PEP_ENABLE_INGRESS_FILTERING_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_PEP_ENABLE_INGRESS_FILTERING_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_EGQ_ppct_tbl_data.pep_enable_ingress_filtering = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_PEP_PORT_PEP_PVID_MEMBER_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_PEP_PORT_PEP_PVID_MEMBER_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_EGQ_ppct_tbl_data.pep_port_pep_pvid_member = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_PEP_PORT_ALL_OTHERS_MEMBER_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_PEP_PORT_ALL_OTHERS_MEMBER_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_EGQ_ppct_tbl_data.pep_port_all_others_member = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_MTU_VIOLATION_ACTION_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_MTU_VIOLATION_ACTION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_EGQ_ppct_tbl_data.mtu_violation_action = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_MTU_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_MTU_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_EGQ_ppct_tbl_data.mtu = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_UNKNOWN_UC_DA_ACTION_FILTER_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_UNKNOWN_UC_DA_ACTION_FILTER_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_EGQ_ppct_tbl_data.unknown_uc_da_action_filter = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_UNKNOWN_MC_DA_ACTION_FILTER_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_UNKNOWN_MC_DA_ACTION_FILTER_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_EGQ_ppct_tbl_data.unknown_mc_da_action_filter = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_UNKNOWN_BC_DA_ACTION_FILTER_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_UNKNOWN_BC_DA_ACTION_FILTER_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_EGQ_ppct_tbl_data.unknown_bc_da_action_filter = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = soc_petra_pp_egq_ppct_tbl_set_unsafe(
          unit,
          prm_entry_ndx,
          &prm_EGQ_ppct_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_egq_ppct_tbl_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_egq_ppct_tbl_set");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_egq_ppct_tbl_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: egq_ppct_tbl_get (section soc_petra_pp_egq_api)
 */
int
  ui_petra_pp_egq_api_egq_ppct_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_EGQ_PPCT_TBL_DATA
    prm_EGQ_ppct_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_egq_api");
  soc_sand_proc_name = "soc_petra_pp_egq_ppct_tbl_get";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_GET_EGQ_PPCT_TBL_GET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_EGQ_PPCT_TBL_DATA_clear(&prm_EGQ_ppct_tbl_data);

  /* Get parameters */

  /* Call function */
  ret = soc_petra_pp_egq_ppct_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_EGQ_ppct_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_egq_ppct_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_egq_ppct_tbl_get");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_egq_ppct_tbl_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> EGQ_ppct_tbl_data:", TRUE);
  soc_petra_pp_PETRA_PP_EGQ_PPCT_TBL_DATA_print(&prm_EGQ_ppct_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: egq_stp_tbl_set (section soc_petra_pp_egq_api)
 */
int
  ui_petra_pp_egq_api_egq_stp_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_EGQ_STP_TBL_DATA
    prm_EGQ_stp_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_egq_api");
  soc_sand_proc_name = "soc_petra_pp_egq_stp_tbl_set";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_EGQ_STP_TBL_SET_EGQ_STP_TBL_SET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_EGQ_STP_TBL_DATA_clear(&prm_EGQ_stp_tbl_data);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_petra_pp_egq_stp_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_EGQ_stp_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_egq_stp_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_egq_stp_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_EGQ_STP_TBL_SET_EGQ_STP_TBL_SET_EGQ_STP_TBL_DATA_EGRESS_STP_STATE_1_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_EGQ_STP_TBL_SET_EGQ_STP_TBL_SET_EGQ_STP_TBL_DATA_EGRESS_STP_STATE_1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_EGQ_stp_tbl_data.egress_stp_state[1] = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_EGQ_STP_TBL_SET_EGQ_STP_TBL_SET_EGQ_STP_TBL_DATA_EGRESS_STP_STATE_0_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_EGQ_STP_TBL_SET_EGQ_STP_TBL_SET_EGQ_STP_TBL_DATA_EGRESS_STP_STATE_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_EGQ_stp_tbl_data.egress_stp_state[0] = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = soc_petra_pp_egq_stp_tbl_set_unsafe(
          unit,
          prm_entry_ndx,
          &prm_EGQ_stp_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_egq_stp_tbl_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_egq_stp_tbl_set");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_egq_stp_tbl_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: egq_stp_tbl_get (section soc_petra_pp_egq_api)
 */
int
  ui_petra_pp_egq_api_egq_stp_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_EGQ_STP_TBL_DATA
    prm_EGQ_stp_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_egq_api");
  soc_sand_proc_name = "soc_petra_pp_egq_stp_tbl_get";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_EGQ_STP_TBL_GET_EGQ_STP_TBL_GET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_EGQ_STP_TBL_DATA_clear(&prm_EGQ_stp_tbl_data);

  /* Get parameters */

  /* Call function */
  ret = soc_petra_pp_egq_stp_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_EGQ_stp_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_egq_stp_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_egq_stp_tbl_get");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_egq_stp_tbl_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> EGQ_stp_tbl_data:", TRUE);
  soc_petra_pp_PETRA_PP_EGQ_STP_TBL_DATA_print(&prm_EGQ_stp_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: epni_arp_tbl_set (section soc_petra_pp_epni_api)
 */
int
  ui_petra_pp_epni_api_epni_arp_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_EPNI_ARP_TBL_DATA
    prm_EPNI_arp_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_epni_api");
  soc_sand_proc_name = "soc_petra_pp_epni_arp_tbl_set";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_EPNI_ARP_TBL_SET_EPNI_ARP_TBL_SET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_EPNI_ARP_TBL_DATA_clear(&prm_EPNI_arp_tbl_data);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_petra_pp_epni_arp_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_EPNI_arp_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_epni_arp_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_epni_arp_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_EPNI_ARP_TBL_SET_EPNI_ARP_TBL_SET_EPNI_ARP_TBL_DATA_DMAC_0_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_EPNI_ARP_TBL_SET_EPNI_ARP_TBL_SET_EPNI_ARP_TBL_DATA_DMAC_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_EPNI_arp_tbl_data.dmac[0] = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = soc_petra_pp_epni_arp_tbl_set_unsafe(
          unit,
          prm_entry_ndx,
          &prm_EPNI_arp_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_epni_arp_tbl_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_epni_arp_tbl_set");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_epni_arp_tbl_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: epni_arp_tbl_get (section soc_petra_pp_epni_api)
 */
int
  ui_petra_pp_epni_api_epni_arp_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_EPNI_ARP_TBL_DATA
    prm_EPNI_arp_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_epni_api");
  soc_sand_proc_name = "soc_petra_pp_epni_arp_tbl_get";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_EPNI_ARP_TBL_GET_EPNI_ARP_TBL_GET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_EPNI_ARP_TBL_DATA_clear(&prm_EPNI_arp_tbl_data);

  /* Get parameters */

  /* Call function */
  ret = soc_petra_pp_epni_arp_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_EPNI_arp_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_epni_arp_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_epni_arp_tbl_get");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_epni_arp_tbl_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> EPNI_arp_tbl_data:", TRUE);
  soc_petra_pp_PETRA_PP_EPNI_ARP_TBL_DATA_print(&prm_EPNI_arp_tbl_data);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: epni_ptt_tbl_set (section soc_petra_pp_epni_api)
 */
int
  ui_petra_pp_epni_api_epni_ptt_tbl_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_EPNI_PTT_TBL_DATA
    prm_EPNI_ptt_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_epni_api");
  soc_sand_proc_name = "soc_petra_pp_epni_ptt_tbl_set";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_EPNI_PTT_TBL_SET_EPNI_PTT_TBL_SET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_EPNI_PTT_TBL_DATA_clear(&prm_EPNI_ptt_tbl_data);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_petra_pp_epni_ptt_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_EPNI_ptt_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_epni_ptt_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_epni_ptt_tbl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_EPNI_PTT_TBL_SET_EPNI_PTT_TBL_SET_EPNI_PTT_TBL_DATA_TRANSMIT_TAG_ENABLE_2_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_EPNI_PTT_TBL_SET_EPNI_PTT_TBL_SET_EPNI_PTT_TBL_DATA_TRANSMIT_TAG_ENABLE_2_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_EPNI_ptt_tbl_data.transmit_tag_enable[2] = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_EPNI_PTT_TBL_SET_EPNI_PTT_TBL_SET_EPNI_PTT_TBL_DATA_TRANSMIT_TAG_ENABLE_1_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_EPNI_PTT_TBL_SET_EPNI_PTT_TBL_SET_EPNI_PTT_TBL_DATA_TRANSMIT_TAG_ENABLE_1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_EPNI_ptt_tbl_data.transmit_tag_enable[1] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_EPNI_PTT_TBL_SET_EPNI_PTT_TBL_SET_EPNI_PTT_TBL_DATA_TRANSMIT_TAG_ENABLE_0_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_EPNI_PTT_TBL_SET_EPNI_PTT_TBL_SET_EPNI_PTT_TBL_DATA_TRANSMIT_TAG_ENABLE_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_EPNI_ptt_tbl_data.transmit_tag_enable[0] = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = soc_petra_pp_epni_ptt_tbl_set_unsafe(
          unit,
          prm_entry_ndx,
          &prm_EPNI_ptt_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_epni_ptt_tbl_set - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_epni_ptt_tbl_set");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_epni_ptt_tbl_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: epni_ptt_tbl_get (section soc_petra_pp_epni_api)
 */
int
  ui_petra_pp_epni_api_epni_ptt_tbl_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_PETRA_PP_EPNI_PTT_TBL_DATA
    prm_EPNI_ptt_tbl_data;

  UI_MACROS_INIT_FUNCTION("ui_petra_pp_epni_api");
  soc_sand_proc_name = "soc_petra_pp_epni_ptt_tbl_get";

  unit = soc_petra_get_default_unit();

  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PETRA_PP_PETRA_PP_EPNI_PTT_TBL_GET_EPNI_PTT_TBL_GET_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_entry_ndx = param_val->value.ulong_value;

  soc_petra_pp_PETRA_PP_EPNI_PTT_TBL_DATA_clear(&prm_EPNI_ptt_tbl_data);

  /* Get parameters */

  /* Call function */
  ret = soc_petra_pp_epni_ptt_tbl_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_EPNI_ptt_tbl_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_petra_pp_epni_ptt_tbl_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "soc_petra_pp_epni_ptt_tbl_get");
    goto exit;
  }

  send_string_to_screen(" *** soc_petra_pp_epni_ptt_tbl_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> EPNI_ptt_tbl_data:", TRUE);
  soc_petra_pp_PETRA_PP_EPNI_PTT_TBL_DATA_print(&prm_EPNI_ptt_tbl_data);


  goto exit;
exit:
  return ui_ret;
}




/********************************************************************
 *  Section handler: soc_petra_pp_ihp_api
 */
int
  ui_petra_pp_ihp_api(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_petra_pp_ihp_api");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_management_requests_fifo_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_GET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_GET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_management_requests_fifo_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_PORT_INFO_TBL_SET_IHP_PORT_INFO_TBL_SET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_port_info_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_PORT_INFO_TBL_GET_IHP_PORT_INFO_TBL_GET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_port_info_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_SET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_eth_port_info1_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO1_TBL_GET_IHP_ETH_PORT_INFO1_TBL_GET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_eth_port_info1_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_eth_port_info2_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_GET_IHP_ETH_PORT_INFO2_TBL_GET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_eth_port_info2_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_CID_INFO_TBL_SET_IHP_CID_INFO_TBL_SET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_cid_info_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_CID_INFO_TBL_GET_IHP_CID_INFO_TBL_GET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_cid_info_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_CID_TO_TOPOLOGY_TBL_SET_IHP_CID_TO_TOPOLOGY_TBL_SET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_cid_to_topology_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_CID_TO_TOPOLOGY_TBL_GET_IHP_CID_TO_TOPOLOGY_TBL_GET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_cid_to_topology_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_STP_TABLE_TBL_SET_IHP_STP_TABLE_TBL_SET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_stp_table_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_STP_TABLE_TBL_GET_IHP_STP_TABLE_TBL_GET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_stp_table_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_PORT_AND_PROTOCOL_TO_CID_INDEX_TBL_SET_IHP_PORT_AND_PROTOCOL_TO_CID_INDEX_TBL_SET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_port_and_protocol_to_cid_index_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_PORT_AND_PROTOCOL_TO_CID_INDEX_TBL_GET_IHP_PORT_AND_PROTOCOL_TO_CID_INDEX_TBL_GET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_port_and_protocol_to_cid_index_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_CID_INDEX_TO_CID_TBL_SET_IHP_CID_INDEX_TO_CID_TBL_SET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_cid_index_to_cid_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_CID_INDEX_TO_CID_TBL_GET_IHP_CID_INDEX_TO_CID_TBL_GET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_cid_index_to_cid_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_SET_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_SET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_bridge_control_to_forwarding_params_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_GET_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_GET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_bridge_control_to_forwarding_params_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_SET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_cid_da_not_found_fwd_action_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_GET_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_GET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_cid_da_not_found_fwd_action_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_CID_PORT_MEMBERSHIP_TBL_SET_IHP_CID_PORT_MEMBERSHIP_TBL_SET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_cid_port_membership_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_CID_PORT_MEMBERSHIP_TBL_GET_IHP_CID_PORT_MEMBERSHIP_TBL_GET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_cid_port_membership_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_IS_CID_SHARED_TBL_SET_IHP_IS_CID_SHARED_TBL_SET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_is_cid_shared_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_IS_CID_SHARED_TBL_GET_IHP_IS_CID_SHARED_TBL_GET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_is_cid_shared_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_TOS_TO_TC_TBL_SET_IHP_TOS_TO_TC_TBL_SET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_tos_to_tc_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_TOS_TO_TC_TBL_GET_IHP_TOS_TO_TC_TBL_GET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_tos_to_tc_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_SET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_event_fifo_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_EVENT_FIFO_TBL_GET_IHP_EVENT_FIFO_TBL_GET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_event_fifo_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_FEC_TABLE_TBL_SET_IHP_FEC_TABLE_TBL_SET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_fec_table_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_FEC_TABLE_TBL_GET_IHP_FEC_TABLE_TBL_GET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_fec_table_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_FEC_UC_TABLE_TBL_SET_IHP_FEC_UC_TABLE_TBL_SET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_fec_uc_table_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_FEC_UC_TABLE_TBL_GET_IHP_FEC_UC_TABLE_TBL_GET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_fec_uc_table_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_FEC_MC_TABLE_TBL_SET_IHP_FEC_MC_TABLE_TBL_SET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_fec_mc_table_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_FEC_MC_TABLE_TBL_GET_IHP_FEC_MC_TABLE_TBL_GET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_fec_mc_table_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_FEC_PROG_PORT_TABLE_TBL_SET_IHP_FEC_PROG_PORT_TABLE_TBL_SET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_fec_prog_port_table_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_FEC_PROG_PORT_TABLE_TBL_GET_IHP_FEC_PROG_PORT_TABLE_TBL_GET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_fec_prog_port_table_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_SMOOTH_DIVISION_TBL_SET_IHP_SMOOTH_DIVISION_TBL_SET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_smooth_division_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_SMOOTH_DIVISION_TBL_GET_IHP_SMOOTH_DIVISION_TBL_GET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_smooth_division_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_LONGEST_PREFIX_MATCH_TBL_SET_IHP_LONGEST_PREFIX_MATCH_TBL_SET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_longest_prefix_match_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_LONGEST_PREFIX_MATCH_TBL_GET_IHP_LONGEST_PREFIX_MATCH_TBL_GET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_longest_prefix_match_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_LONGEST_PREFIX_MATCH0_TBL_SET_IHP_LONGEST_PREFIX_MATCH0_TBL_SET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_longest_prefix_match0_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_LONGEST_PREFIX_MATCH0_TBL_GET_IHP_LONGEST_PREFIX_MATCH0_TBL_GET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_longest_prefix_match0_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_LONGEST_PREFIX_MATCH1_TBL_SET_IHP_LONGEST_PREFIX_MATCH1_TBL_SET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_longest_prefix_match1_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_LONGEST_PREFIX_MATCH1_TBL_GET_IHP_LONGEST_PREFIX_MATCH1_TBL_GET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_longest_prefix_match1_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_LONGEST_PREFIX_MATCH2_TBL_SET_IHP_LONGEST_PREFIX_MATCH2_TBL_SET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_longest_prefix_match2_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_LONGEST_PREFIX_MATCH2_TBL_GET_IHP_LONGEST_PREFIX_MATCH2_TBL_GET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_longest_prefix_match2_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_ACTION_BANK_A_TBL_SET_IHP_TCAM_ACTION_BANK_A_TBL_SET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_tcam_action_bank_a_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_ACTION_BANK_A_TBL_GET_IHP_TCAM_ACTION_BANK_A_TBL_GET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_tcam_action_bank_a_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_ACTION_BANK_B_TBL_SET_IHP_TCAM_ACTION_BANK_B_TBL_SET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_tcam_action_bank_b_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_ACTION_BANK_B_TBL_GET_IHP_TCAM_ACTION_BANK_B_TBL_GET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_tcam_action_bank_b_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_ACTION_BANK_C_TBL_SET_IHP_TCAM_ACTION_BANK_C_TBL_SET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_tcam_action_bank_c_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_ACTION_BANK_C_TBL_GET_IHP_TCAM_ACTION_BANK_C_TBL_GET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_tcam_action_bank_c_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_ACTION_BANK_D_TBL_SET_IHP_TCAM_ACTION_BANK_D_TBL_SET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_tcam_action_bank_d_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_ACTION_BANK_D_TBL_GET_IHP_TCAM_ACTION_BANK_D_TBL_GET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_tcam_action_bank_d_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_TOS_TO_COS_TBL_SET_IHP_TOS_TO_COS_TBL_SET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_tos_to_cos_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_TOS_TO_COS_TBL_GET_IHP_TOS_TO_COS_TBL_GET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_tos_to_cos_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_FEC_ACCESSED_TABLE_TBL_SET_IHP_FEC_ACCESSED_TABLE_TBL_SET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_fec_accessed_table_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_FEC_ACCESSED_TABLE_TBL_GET_IHP_FEC_ACCESSED_TABLE_TBL_GET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_fec_accessed_table_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_IPV4_STAT_TABLE_TBL_SET_IHP_IPV4_STAT_TABLE_TBL_SET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_ipv4_stat_table_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_IPV4_STAT_TABLE_TBL_GET_IHP_IPV4_STAT_TABLE_TBL_GET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_ipv4_stat_table_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_BANK_A_TBL_SET_IHP_TCAM_BANK_A_TBL_SET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_tcam_bank_a_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_BANK_A_TBL_GET_IHP_TCAM_BANK_A_TBL_GET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_tcam_bank_a_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_TCAM_READ_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_tcam_read(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_TCAM_WRITE_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_tcam_write(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_TCAM_FLUSH_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_tcam_flush(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_TCAM_COMPARE_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_tcam_compare(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_BANK_D_TBL_SET_IHP_TCAM_BANK_D_TBL_SET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_tcam_bank_d_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_BANK_D_TBL_GET_IHP_TCAM_BANK_D_TBL_GET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_tcam_bank_d_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_IPV6_TC_TO_COS_TBL_SET_IHP_IPV6_TC_TO_COS_TBL_SET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_ipv6_tc_to_cos_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_IPV6_TC_TO_COS_TBL_GET_IHP_IPV6_TC_TO_COS_TBL_GET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_ipv6_tc_to_cos_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_CID_SUBNET_TBL_SET_IHP_CID_SUBNET_TBL_SET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_cid_subnet_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_CID_SUBNET_TBL_GET_IHP_CID_SUBNET_TBL_GET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_cid_subnet_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_SET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_port_da_sa_not_found_fwd_action_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_GET_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_GET_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api_ihp_port_da_sa_not_found_fwd_action_tbl_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after soc_petra_pp_ihp_api***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}



/********************************************************************
 *  Section handler: soc_petra_pp_egq_api
 */
int
  ui_petra_pp_egq_api(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_petra_pp_egq_api");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_SET_ID,1))
  {
    ui_ret = ui_petra_pp_egq_api_egq_pct_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_EGQ_PCT_TBL_GET_EGQ_PCT_TBL_GET_ID,1))
  {
    ui_ret = ui_petra_pp_egq_api_egq_pct_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_ID,1))
  {
    ui_ret = ui_petra_pp_egq_api_egq_ppct_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_GET_EGQ_PPCT_TBL_GET_ID,1))
  {
    ui_ret = ui_petra_pp_egq_api_egq_ppct_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_EGQ_STP_TBL_SET_EGQ_STP_TBL_SET_ID,1))
  {
    ui_ret = ui_petra_pp_egq_api_egq_stp_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_EGQ_STP_TBL_GET_EGQ_STP_TBL_GET_ID,1))
  {
    ui_ret = ui_petra_pp_egq_api_egq_stp_tbl_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after soc_petra_pp_egq_api***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}




/********************************************************************
 *  Section handler: soc_petra_pp_epni_api
 */
int
  ui_petra_pp_epni_api(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_petra_pp_epni_api");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_EPNI_ARP_TBL_SET_EPNI_ARP_TBL_SET_ID,1))
  {
    ui_ret = ui_petra_pp_epni_api_epni_arp_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_EPNI_ARP_TBL_GET_EPNI_ARP_TBL_GET_ID,1))
  {
    ui_ret = ui_petra_pp_epni_api_epni_arp_tbl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_EPNI_PTT_TBL_SET_EPNI_PTT_TBL_SET_ID,1))
  {
    ui_ret = ui_petra_pp_epni_api_epni_ptt_tbl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_PETRA_PP_EPNI_PTT_TBL_GET_EPNI_PTT_TBL_GET_ID,1))
  {
    ui_ret = ui_petra_pp_epni_api_epni_ptt_tbl_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after soc_petra_pp_epni_api***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}



/*****************************************************
*NAME
*  subject_petra_pp_api
*TYPE: PROC
*DATE: 29/DEC/2002
*FUNCTION:
*  Process input line which has an 'subject_petra_pp_api' subject.
*  Input line is assumed to have been checked and
*  found to be of right format.
*CALLING SEQUENCE:
*  subject_petra_pp_api(current_line,current_line_ptr)
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
  subject_petra_pp_acc(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  )
{
  unsigned int
    match_index;
  int
    ui_ret;
  unsigned int
    ;
  char
    *proc_name ;


  proc_name = "subject_petra_pp_acc" ;
  ui_ret = FALSE ;
  unit = 0;


  /*
   * the rest of the line is empty
   */
  if (current_line->num_param_names == 0)
  {
    send_string_to_screen("\r\n",FALSE) ;
    send_string_to_screen("'subject_petra_pp_api()' function was called with no parameters.\r\n",FALSE) ;
    goto exit ;
  }
  /*
   * Else, there are parameters on the line (not just 'subject_petra_pp_api').
   */

  send_array_to_screen("\r\n",2) ;



  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_IHP_API_ID,1))
  {
    ui_ret = ui_petra_pp_ihp_api(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_EGQ_API_ID,1))
  {
    ui_ret = ui_petra_pp_egq_api(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_EPNI_API_ID,1))
  {
    ui_ret = ui_petra_pp_epni_api(current_line);
  }
  else
  {
    /*
     * Enter if an unknown request.
     */
    send_string_to_screen(
      "\r\n"
      "*** soc_petra_pp_api command with unknown parameters'.\r\n"
      "    Syntax error/sw error...\r\n",
      TRUE)  ;
    ui_ret = TRUE  ;
    goto exit  ;
  }

exit:
  return (ui_ret);
}

#endif
