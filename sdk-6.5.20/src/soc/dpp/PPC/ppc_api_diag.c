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
#include <soc/dpp/SAND/Utils/sand_bitstream.h>


#include <soc/dpp/PPC/ppc_api_diag.h>

#include <soc/dpp/PPC/ppc_api_eg_vlan_edit.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_occupation_mgmt.h>
#include <soc/dpp/ARAD/arad_parser.h>




#define PPC_API_DIAG_INVALID_INITIAL_VID                               (-1)
















void
  SOC_PPC_DIAG_BUFFER_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_BUFFER *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_DIAG_BUFFER));
  for (ind = 0; ind < SOC_PPC_DIAG_BUFF_MAX_SIZE; ++ind)
  {
    info->buff[ind] = 0;
  }
  info->buff_len = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_MODE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_MODE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_DIAG_MODE_INFO));
  info->flavor = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_VSI_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_VSI_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_DIAG_VSI_INFO));
  info->vsi = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_DB_USE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_DB_USE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_DIAG_DB_USE_INFO));
  info->lkup_num = 0;
  info->bank_id = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_TCAM_USE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_TCAM_USE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_DIAG_TCAM_USE_INFO));
  info->lkup_num = 0;
  info->lkup_usage = SOC_PPC_NOF_DIAG_TCAM_USAGES;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_RECEIVED_PACKET_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_RECEIVED_PACKET_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_DIAG_RECEIVED_PACKET_INFO));
  SOC_PPC_DIAG_BUFFER_clear(&(info->packet_header));
  info->in_tm_port = 0;
  soc_sand_SAND_PP_SYS_PORT_ID_clear(&(info->src_sys_port));
  info->in_pp_port = 0;
  info->pp_context = SOC_TMC_PORT_NOF_HEADER_TYPES;
  info->packet_qual = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_PPC_DIAG_TRAP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_TRAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_DIAG_TRAP_INFO));
  info->is_pkt_trapped = 0;
  info->code = SOC_PPC_NOF_TRAP_CODES;
  SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO_clear(&(info->info));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_SNOOP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_SNOOP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_DIAG_SNOOP_INFO));
  info->code = SOC_PPC_NOF_TRAP_CODES;
  SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO_clear(&(info->info));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_TRAPS_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_TRAPS_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_DIAG_TRAPS_INFO));
  for (ind = 0; ind < SOC_PPC_NOF_TRAP_CODES/SOC_SAND_NOF_BITS_IN_UINT32; ++ind)
  {
    info->trap_stack[ind] = 0;
  }
  SOC_PPC_DIAG_TRAP_INFO_clear(&(info->committed_trap));
  SOC_PPC_DIAG_SNOOP_INFO_clear(&(info->committed_snoop));
  SOC_PPC_DIAG_TRAP_INFO_clear(&(info->trap_updated_dest));

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_TRAP_TO_CPU_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_TRAP_TO_CPU_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_DIAG_TRAP_TO_CPU_INFO));
  info->cpu_dest = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_IPV4_VPN_ROUTE_KEY_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_IPV4_VPN_ROUTE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_DIAG_IPV4_VPN_ROUTE_KEY));
  info->vrf_id = 0;
  SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY_clear(&(info->subnet));

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_LEM_KEY_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_LEM_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_DIAG_LEM_KEY));

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_LIF_KEY_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_LIF_KEY *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_DIAG_LIF_KEY));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_LIF_VALUE_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_LIF_VALUE *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_DIAG_LIF_VALUE));

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_LIF_LKUP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_LIF_LKUP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_DIAG_LIF_LKUP_INFO));
  info->type = SOC_PPC_NOF_DIAG_LIF_LKUP_TYPES;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_TCAM_KEY_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_TCAM_KEY *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_DIAG_TCAM_KEY));
  for (ind = 0; ind < SOC_PPC_DIAG_TCAM_KEY_NOF_UINT32S_MAX; ++ind)
  {
    info->val[ind] = 0;
  }
  for (ind = 0; ind < SOC_PPC_DIAG_TCAM_KEY_NOF_UINT32S_MAX; ++ind)
  {
    info->mask[ind] = 0;
  }
  info->length = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_TCAM_VALUE_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_TCAM_VALUE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_DIAG_TCAM_VALUE));
  info->fec_ptr = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_LEM_VALUE_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_LEM_VALUE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_DIAG_LEM_VALUE));
  SOC_PPC_FRWRD_MACT_ENTRY_VALUE_clear(&(info->mact));
  SOC_PPC_BMACT_ENTRY_INFO_clear(&(info->bmact));
  SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO_clear(&info->host);
  SOC_PPC_FRWRD_DECISION_INFO_clear(&(info->ilm));
  SOC_PPC_LLP_SA_AUTH_MAC_INFO_clear(&(info->sa_auth));
  SOC_PPC_LLP_VID_ASSIGN_MAC_INFO_clear(&(info->sa_vid_assign));

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_PARSING_L2_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_PARSING_L2_INFO *info
  )
{
  uint32
    ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_DIAG_PARSING_L2_INFO));
  info->encap_type = SOC_SAND_PP_NOF_ETH_ENCAP_TYPES;
  SOC_PPC_LLP_PARSE_INFO_clear(&(info->tag_fromat));
  for (ind = 0; ind < 2; ind++)
  {
    SOC_SAND_PP_VLAN_TAG_clear(&(info->vlan_tags[ind]));
  }
  info->vlan_tag_format = SOC_SAND_PP_NOF_ETHERNET_FRAME_VLAN_FORMATS;
  info->next_prtcl = SOC_PPC_NOF_L2_NEXT_PRTCL_TYPES;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_PARSING_MPLS_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_PARSING_MPLS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_DIAG_PARSING_MPLS_INFO));
  info->bos = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_PARSING_IP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_PARSING_IP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_DIAG_PARSING_IP_INFO));
  info->next_prtcl = SOC_PPC_NOF_L3_NEXT_PRTCL_TYPES;
  info->is_mc = 0;
  info->is_fragmented = 0;
  info->hdr_err = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_PARSING_HEADER_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_PARSING_HEADER_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_DIAG_PARSING_HEADER_INFO));
  info->hdr_type = SOC_PPC_PKT_HDR_TYPE_NONE;
  info->hdr_offset = 0;
  SOC_PPC_DIAG_PARSING_L2_INFO_clear(&(info->eth));
  SOC_PPC_DIAG_PARSING_IP_INFO_clear(&(info->ip));
  SOC_PPC_DIAG_PARSING_MPLS_INFO_clear(&(info->mpls));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_PARSING_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_PARSING_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_DIAG_PARSING_INFO));
  info->pfc_hw = -1;
  info->initial_vid = PPC_API_DIAG_INVALID_INITIAL_VID; 
  for (ind = 0; ind < SOC_PPC_DIAG_MAX_NOF_HDRS; ++ind)
  {
    SOC_PPC_DIAG_PARSING_HEADER_INFO_clear(&(info->hdrs_stack[ind]));
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_TERM_MPLS_LABEL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_TERM_MPLS_LABEL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_DIAG_TERM_MPLS_LABEL_INFO));
  info->range_index = 0;
  SOC_PPC_MPLS_TERM_LABEL_RANGE_clear(&(info->range));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_TERM_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_TERM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_DIAG_TERM_INFO));
  info->term_type = SOC_PPC_NOF_PKT_TERM_TYPES;
  info->frwrd_type = SOC_PPC_NOF_PKT_FRWRD_TYPES;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_IPV4_VPN_KEY_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_IPV4_VPN_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_DIAG_IPV4_VPN_KEY));
  info->vrf = 0;
  SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY_clear(&(info->key));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_IPV6_VPN_KEY_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_IPV6_VPN_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_DIAG_IPV6_VPN_KEY));
  info->vrf = 0;
  SOC_PPC_FRWRD_IPV6_VPN_ROUTE_KEY_clear(&(info->key));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


#if defined(INCLUDE_KBP)
void
  SOC_PPC_DIAG_IPV4_UNICAST_RPF_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_IPV4_UNICAST_RPF *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  info->vrf = 0;
  info->sip = 0;
  info->dip = 0;

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_IPV4_MULTICAST_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_IPV4_MULTICAST *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  info->vrf = 0;
  info->in_rif = 0;
  info->sip = 0;
  info->dip = 0;

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_IPV6_UNICAST_RPF_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_IPV6_UNICAST_RPF *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  info->vrf = 0;
  soc_sand_SAND_PP_IPV6_ADDRESS_clear(&info->sip);
  soc_sand_SAND_PP_IPV6_ADDRESS_clear(&info->dip);

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_IPV6_MULTICAST_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_IPV6_MULTICAST *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  info->vrf = 0;
  info->in_rif = 0;
  soc_sand_SAND_PP_IPV6_ADDRESS_clear(&info->sip);
  soc_sand_SAND_PP_IPV6_ADDRESS_clear(&info->dip);

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_MPLS_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_MPLS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  info->in_rif = 0;
  info->in_port = 0;
  info->exp = 0;
  info->mpls_label = 0;

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_TRILL_UNICAST_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_TRILL_UNICAST *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  info->egress_nick = 0;

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_TRILL_MULTICAST_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_TRILL_MULTICAST *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  info->esdai = 0;
  info->fid_vsi = 0;
  info->dist_tree_nick = 0;

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_IP_REPLY_RECORD_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_IP_REPLY_RECORD *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  info->match_status = 0;
  info->is_synamic = 0;
  info->p2p_service = 0;
  info->identifier = 0;
  info->out_lif_valid = 0;
  info->out_lif = 0;
  SOC_PPC_FRWRD_DECISION_INFO_clear(&info->destination);
  info->eei = 0;
  info->fec_ptr = 0;

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_SECOND_IP_REPLY_RECORD_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_SECOND_IP_REPLY_RECORD *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  info->match_status = 0;
  info->dest_id = 0;

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
#endif 

void
  SOC_PPC_DIAG_FRWRD_LKUP_KEY_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_FRWRD_LKUP_KEY *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_DIAG_FRWRD_LKUP_KEY));
  SOC_PPC_FRWRD_MACT_ENTRY_KEY_clear(&(info->mact));
  SOC_PPC_BMACT_ENTRY_KEY_clear(&(info->bmact));
  SOC_PPC_DIAG_IPV4_VPN_KEY_clear(&(info->ipv4_uc));
  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY_clear(&(info->ipv4_mc));
  SOC_PPC_DIAG_IPV6_VPN_KEY_clear(&(info->ipv6_uc));
  SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY_clear(&(info->ipv6_mc));
  SOC_PPC_FRWRD_ILM_KEY_clear(&(info->ilm));
  info->trill_uc = 0;
  SOC_PPC_TRILL_MC_ROUTE_KEY_clear(&(info->trill_mc));
  for (ind = 0; ind < 2; ++ind)
  {
    info->raw[ind] = 0;
  }
#if defined(INCLUDE_KBP)
  SOC_PPC_DIAG_IPV4_UNICAST_RPF_clear(&info->kbp_ipv4_unicast_rpf);
  SOC_PPC_DIAG_IPV4_MULTICAST_clear(&info->kbp_ipv4_multicast);
  SOC_PPC_DIAG_IPV6_UNICAST_RPF_clear(&info->kbp_ipv6_unicast_rpf);
  SOC_PPC_DIAG_IPV6_MULTICAST_clear(&info->kbp_ipv6_multicast);
  SOC_PPC_DIAG_MPLS_clear(&info->kbp_mpls);
  SOC_PPC_DIAG_TRILL_UNICAST_clear(&info->kbp_trill_unicast);
  SOC_PPC_DIAG_TRILL_MULTICAST_clear(&info->kbp_trill_multicast);
#endif 
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_FRWRD_LKUP_VALUE_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_FRWRD_LKUP_VALUE *info
  )
{
  uint32
    ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_DIAG_FRWRD_LKUP_VALUE));
  SOC_PPC_FRWRD_MACT_ENTRY_VALUE_clear(&(info->mact));
  SOC_PPC_BMACT_ENTRY_INFO_clear(&(info->bmact));
  SOC_PPC_FRWRD_DECISION_INFO_clear(&(info->frwrd_decision));
  SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO_clear(&(info->host_info));
  for (ind = 0; ind < 2; ++ind)
  {
    info->raw[ind] = 0;
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_FRWRD_LKUP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_FRWRD_LKUP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_DIAG_FRWRD_LKUP_INFO));
  info->frwrd_type = SOC_PPC_NOF_DIAG_FWD_LKUP_TYPES;
  info->frwrd_hdr_index = 0;
  SOC_PPC_DIAG_FRWRD_LKUP_KEY_clear(&(info->lkup_key));
  info->key_found = 0;
  SOC_PPC_DIAG_FRWRD_LKUP_VALUE_clear(&(info->lkup_res));
#if defined(INCLUDE_KBP)
  info->is_kbp = FALSE;
  SOC_PPC_DIAG_IP_REPLY_RECORD_clear(&info->ip_reply_record);
  SOC_PPC_DIAG_SECOND_IP_REPLY_RECORD_clear(&info->second_ip_reply_result);
#endif 
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_LEARN_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_LEARN_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_DIAG_LEARN_INFO));
  info->valid = 0;
  info->ingress = 0;
  info->is_new_key = 0;
  SOC_PPC_FRWRD_MACT_ENTRY_KEY_clear(&(info->key));
  SOC_PPC_FRWRD_MACT_ENTRY_VALUE_clear(&(info->value));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_VLAN_EDIT_RES_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_VLAN_EDIT_RES *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_DIAG_VLAN_EDIT_RES));
  info->cmd_id = 0;
  info->adv_mode = SOC_SAND_U32_MAX;
  SOC_SAND_PP_VLAN_TAG_clear(&(info->ac_tag));
  SOC_SAND_PP_VLAN_TAG_clear(&(info->ac_tag2));
  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO_clear(&(info->cmd_info));
  
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_FRWRD_DECISION_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_FRWRD_DECISION_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_DIAG_FRWRD_DECISION_INFO));
  SOC_PPC_FRWRD_DECISION_INFO_clear(&(info->frwrd_decision));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_FRWRD_DECISION_TRACE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_FRWRD_DECISION_TRACE_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_DIAG_FRWRD_DECISION_TRACE_INFO));
  for (ind = 0; ind < SOC_PPC_NOF_DIAG_FRWRD_DECISION_PHASES; ++ind)
  {
    SOC_PPC_DIAG_FRWRD_DECISION_INFO_clear(&(info->frwrd[ind]));
    SOC_PPC_TRAP_INFO_clear(&(info->trap[ind]));
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_PPC_DIAG_PKT_TM_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_PKT_TM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_DIAG_PKT_TM_INFO));
  info->valid_fields = 0;
  SOC_PPC_FRWRD_DECISION_INFO_clear(&(info->frwrd_decision));
  info->tc = 0;
  info->dp = 0;
  info->meter1 = 0;
  info->meter2 = 0;
  info->dp_meter_cmd = 0;
  info->counter1 = 0;
  info->counter2 = 0;
  info->cud = 0;
  info->eth_meter_ptr = 0;
  info->ingress_shaping_da = 0;
  info->ecn_capable = 0;
  info->cni = 0;
  info->da_type = SOC_SAND_PP_NOF_ETHERNET_DA_TYPES;
  info->st_vsq_ptr = 0;
  info->lag_lb_key = 0;
  info->ignore_cp = 0;
  info->snoop_id = 0;

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_ENCAP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_ENCAP_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_DIAG_ENCAP_INFO));
  SOC_PPC_MPLS_COMMAND_clear(&(info->mpls_cmd));
  for (ind = 0; ind < SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX; ++ind)
  {
    SOC_PPC_EG_ENCAP_ENTRY_INFO_clear(&(info->encap_info[ind]));
  }
  for (ind = 0; ind < SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX; ++ind) 
  {
    info->eep[ind] = 0;
  }
  info->ll_vsi = 0;
  info->out_ac = 0;
  info->pp_port = 0;
  info->tm_port = 0;
  info->nof_eeps = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_EG_DROP_LOG_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_EG_DROP_LOG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_DIAG_EG_DROP_LOG_INFO));
  info->drop_log[0] = 0;
  info->drop_log[1] = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_ETH_PACKET_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_ETH_PACKET_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_DIAG_ETH_PACKET_INFO));
  info->ether_type = 0;
  soc_sand_SAND_PP_MAC_ADDRESS_clear(&(info->da));
  soc_sand_SAND_PP_MAC_ADDRESS_clear(&(info->sa));
  info->is_tagged = 0;
  SOC_SAND_PP_VLAN_TAG_clear(&(info->tag));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void 
  SOC_PPC_DIAG_EGRESS_VLAN_EDIT_INFO_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_EGRESS_VLAN_EDIT_INFO *prm_vec_res
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(prm_vec_res);

    sal_memset(prm_vec_res, 0x0, sizeof(SOC_PPC_DIAG_EGRESS_VLAN_EDIT_INFO));
    SOC_PPC_EG_VLAN_EDIT_COMMAND_KEY_clear(&(prm_vec_res->key));
    SOC_SAND_PP_VLAN_TAG_clear(&(prm_vec_res->ac_tag));
    SOC_SAND_PP_VLAN_TAG_clear(&(prm_vec_res->ac_tag2));
    
    SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO_clear(&(prm_vec_res->cmd_info));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_TRILL_KEY_clear(
    SOC_SAND_OUT SOC_PPC_DIAG_TRILL_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_DIAG_TRILL_KEY));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_DIAG_FWD_LKUP_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_DIAG_FWD_LKUP_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_DIAG_FWD_LKUP_TYPE_NONE:
    str = "none";
  break;
  case SOC_PPC_DIAG_FWD_LKUP_TYPE_MACT:
    str = "mact";
  break;
  case SOC_PPC_DIAG_FWD_LKUP_TYPE_BMACT:
    str = "bmact";
  break;
  case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_UC:
    str = "ipv4_uc";
  break;
  case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_MC:
    str = "ipv4_mc";
  break;
  case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_VPN:
    str = "ipv4_vpn";
  break;
  case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV6_UC:
    str = "ipv6_uc";
  break;
  case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV6_MC:
    str = "ipv6_mc";
  break;
  case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV6_VPN:
    str = "ipv6_vpn";
  break;
  case SOC_PPC_DIAG_FWD_LKUP_TYPE_ILM:
    str = "ilm";
  break;
  case SOC_PPC_DIAG_FWD_LKUP_TYPE_TRILL_UC:
    str = "trill_uc";
  break;
  case SOC_PPC_DIAG_FWD_LKUP_TYPE_TRILL_MC:
    str = "trill_mc";
  break;
  case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_HOST:
    str = "ipv4_host";
  break;
  case SOC_PPC_DIAG_FWD_LKUP_TYPE_FCF:
    str = "fcf";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_DIAG_DB_USAGE_INFO_LKUP_NUM_to_string(
    SOC_SAND_IN  uint32 enum_val
  )
{
  const char* str = NULL;
  switch (enum_val)
  {
  case 0:
    str = "VT";
    break;
  case 1:
    str = "TT";
    break;
  default:
    str = "Unknown";
  }

  return str;
}

const char*
  SOC_PPC_DIAG_DB_USAGE_INFO_BANK_ID_to_string(
    SOC_SAND_IN  uint32 enum_val
  )
{
  const char* str = NULL;
  switch (enum_val)
  {
  case 0:
    str = "SEMA";
    break;
  case 1:
    str = "SEMB";
    break;
  case 2:
    str = "TCAM";
    break;
  case 3:
    str = "PORT";
    break;
  default:
    str = "Unknown";
  }

  return str;
}


const char*
  SOC_PPC_DIAG_TCAM_USAGE_to_string(
    SOC_SAND_IN  SOC_PPC_DIAG_TCAM_USAGE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_DIAG_TCAM_USAGE_FRWRDING:
    str = "frwrding";
  break;
  case SOC_PPC_DIAG_TCAM_USAGE_PMF:
    str = "pmf";
  break;
  case SOC_PPC_DIAG_TCAM_USAGE_EGR_ACL:
    str = "egr_acl";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_DIAG_FLAVOR_to_string(
    SOC_SAND_IN  SOC_PPC_DIAG_FLAVOR enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_DIAG_FLAVOR_NONE:
    str = "none";
  break;
  case SOC_PPC_DIAG_FLAVOR_RAW:
    str = "raw";
  break;
  case SOC_PPC_DIAG_FLAVOR_CLEAR_ON_GET:
    str = "clear_on_get";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_DIAG_LEM_LKUP_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_DIAG_LEM_LKUP_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_DIAG_LEM_LKUP_TYPE_NONE:
    str = "none";
  break;
  case SOC_PPC_DIAG_LEM_LKUP_TYPE_MACT:
    str = "mact";
  break;
  case SOC_PPC_DIAG_LEM_LKUP_TYPE_BMACT:
    str = "bmact";
  break;
  case SOC_PPC_DIAG_LEM_LKUP_TYPE_HOST:
    str = "host";
  break;
  case SOC_PPC_DIAG_LEM_LKUP_TYPE_ILM:
    str = "ilm";
  break;
  case SOC_PPC_DIAG_LEM_LKUP_TYPE_SA_AUTH:
    str = "sa_auth";
  break;
  case SOC_PPC_DIAG_LEM_LKUP_TYPE_EXTEND_P2P:
    str = "extend_p2p";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_DIAG_LIF_LKUP_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_DIAG_LIF_LKUP_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_DIAG_LIF_LKUP_TYPE_NONE:
    str = "none";
  break;
  case SOC_PPC_DIAG_LIF_LKUP_TYPE_AC:
    str = "ac";
  break;
  case SOC_PPC_DIAG_LIF_LKUP_TYPE_PWE:
    str = "pwe";
  break;
  case SOC_PPC_DIAG_LIF_LKUP_TYPE_MPLS_TUNNEL:
    str = "mpls_tunnel";
  break;
  case SOC_PPC_DIAG_LIF_LKUP_TYPE_IPV4_TUNNEL:
    str = "ipv4_tunnel";
  break;
  case SOC_PPC_DIAG_LIF_LKUP_TYPE_TRILL:
    str = "trill";
  break;
  case SOC_PPC_DIAG_LIF_LKUP_TYPE_MIM_ISID:
    str = "mim_isid";
  break;
  case SOC_PPC_DIAG_LIF_LKUP_TYPE_L2GRE: 
    str = "l2gre";
  break;
  case SOC_PPC_DIAG_LIF_LKUP_TYPE_VXLAN: 
    str = "vxlan";
  break;
  case SOC_PPC_DIAG_LIF_LKUP_TYPE_EXTENDER: 
    str = "extender";
  break;
  case SOC_PPC_DIAG_LIF_LKUP_TYPE_DOUBLE_DATA_ENTRY:
    str = "double data entry";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_DIAG_DB_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_DIAG_DB_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_DIAG_DB_TYPE_LIF:
    str = "lif";
  break;
  case SOC_PPC_DIAG_DB_TYPE_LEM:
    str = "lem";
  break;
  case SOC_PPC_DIAG_DB_TYPE_LPM:
    str = "lpm";
  break;
  case SOC_PPC_DIAG_DB_TYPE_TCAM_FRWRD:
    str = "tcam_frwrd";
  break;
  case SOC_PPC_DIAG_DB_TYPE_TCAM_PMF:
    str = "tcam_pmf";
  break;
  case SOC_PPC_DIAG_DB_TYPE_TCAM_EGR:
    str = "tcam_egr";
  break;
  case SOC_PPC_DIAG_DB_TYPE_EG_LIF:
    str = "eg_lif";
  break;
  case SOC_PPC_DIAG_DB_TYPE_EG_TUNNEL:
    str = "eg_tunnel";
  break;
  case SOC_PPC_DIAG_DB_TYPE_EG_LL:
    str = "eg_ll";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_DIAG_FRWRD_DECISION_PHASE_to_string(
    SOC_SAND_IN  SOC_PPC_DIAG_FRWRD_DECISION_PHASE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_DIAG_FRWRD_DECISION_PHASE_INIT_PORT:
    str = "init_port";
  break;
  case SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LIF:
    str = "lif";
  break;
  case SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND:
    str = "lkup_found";
  break;
  case SOC_PPC_DIAG_FRWRD_DECISION_PHASE_PMF:
    str = "pmf";
  break;
  case SOC_PPC_DIAG_FRWRD_DECISION_PHASE_FEC:
    str = "fec";
  break;
  case SOC_PPC_DIAG_FRWRD_DECISION_PHASE_TRAP:
    str = "trap";
  break;
  case SOC_PPC_DIAG_FRWRD_DECISION_PHASE_ING_RESOLVED:
    str = "ing_resolved";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_DIAG_MPLS_TERM_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_DIAG_MPLS_TERM_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_DIAG_MPLS_TERM_TYPE_NONE:
    str = "none";
  break;
  case SOC_PPC_DIAG_MPLS_TERM_TYPE_RANGE:
    str = "range";
  break;
  case SOC_PPC_DIAG_MPLS_TERM_TYPE_LIF_LKUP:
    str = "lif_lkup";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_DIAG_PKT_TRACE_to_string(
    SOC_SAND_IN  SOC_PPC_DIAG_PKT_TRACE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_DIAG_PKT_TRACE_LIF:
    str = "lif";
  break;
  case SOC_PPC_DIAG_PKT_TRACE_TUNNEL_RNG:
    str = "tunnel_rng";
  break;
  case SOC_PPC_DIAG_PKT_TRACE_TRAP:
    str = "trap";
  break;
  case SOC_PPC_DIAG_PKT_TRACE_FEC:
    str = "fec";
  break;
  case SOC_PPC_DIAG_PKT_TRACE_EG_DROP_LOG:
    str = "eg_drop_log";
  break;
  case SOC_PPC_DIAG_PKT_TRACE_ALL:
    str = "all";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_DIAG_EG_DROP_REASON_to_string(
    SOC_SAND_IN  SOC_PPC_DIAG_EG_DROP_REASON enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_DIAG_EG_DROP_REASON_NONE:
    str = "none";
  break;
  case SOC_PPC_DIAG_EG_DROP_REASON_CNM:
    str = "cnm";
  break;
  case SOC_PPC_DIAG_EG_DROP_REASON_CFM_TRAP:
    str = "cfm_trap";
  break;
  case SOC_PPC_DIAG_EG_DROP_REASON_NO_VSI_TRANSLATION:
    str = "no_vsi_translation";
  break;
  case SOC_PPC_DIAG_EG_DROP_REASON_DSS_STACKING:
    str = "dss_stacking";
  break;
  case SOC_PPC_DIAG_EG_DROP_REASON_LAG_MULTICAST:
    str = "lag_multicast";
  break;
  case SOC_PPC_DIAG_EG_DROP_REASON_EXCLUDE_SRC:
    str = "exclude_src";
  break;
  case SOC_PPC_DIAG_EG_DROP_REASON_VLAN_MEMBERSHIP:
    str = "vlan_membership";
  break;
  case SOC_PPC_DIAG_EG_DROP_REASON_UNACCEPTABLE_FRAME_TYPE:
    str = "unacceptable_frame_type";
  break;
  case SOC_PPC_DIAG_EG_DROP_REASON_SRC_EQUAL_DEST:
    str = "src_equal_dest";
  break;
  case SOC_PPC_DIAG_EG_DROP_REASON_UNKNOWN_DA:
    str = "unknown_da";
  break;
  case SOC_PPC_DIAG_EG_DROP_REASON_SPLIT_HORIZON:
    str = "split_horizon";
  break;
  case SOC_PPC_DIAG_EG_DROP_REASON_PRIVATE_VLAN:
    str = "private_vlan";
  break;
  case SOC_PPC_DIAG_EG_DROP_REASON_TTL_SCOPE:
    str = "ttl_scope";
  break;
  case SOC_PPC_DIAG_EG_DROP_REASON_MTU_VIOLATION:
    str = "mtu_violation";
  break;
  case SOC_PPC_DIAG_EG_DROP_REASON_TRILL_TTL_ZERO:
    str = "trill_ttl_zero";
  break;
  case SOC_PPC_DIAG_EG_DROP_REASON_TRILL_SAME_INTERFACE:
    str = "trill_same_interface";
  break;
  case SOC_PPC_DIAG_EG_DROP_REASON_BOUNCE_BACK:
    str = "bounce_back";
  break;
  case SOC_PPC_DIAG_EG_DROP_REASON_ILLEGAL_EEP:
    str = "illegal_eep";
  break;
 case SOC_PPC_DIAG_EG_DROP_REASON_ILLEGAL_EEI:
    str = "illegal_eei";   
    break;
 case SOC_PPC_DIAG_EG_DROP_REASON_PHP_CONFLICT:
    str = "php_conflict";  
    break;
 case SOC_PPC_DIAG_EG_DROP_REASON_POP_IP_VERSION_ERR:
   str = "pop_ip_version_err";  
   break;
 case SOC_PPC_DIAG_EG_DROP_REASON_MODIFY_SNOOPED_PACKET:
  str = "modify_snooped_packet"; 
  break;
 case SOC_PPC_DIAG_EG_DROP_REASON_IP_TUNNEL_SNOOPED_PACKET:
  str = "ip_tunnel_snooped_packet"; 
  break;
 case SOC_PPC_DIAG_EG_DROP_REASON_EEDB_LINK_LAYER_ENTRY_NEEDED:
  str = "eedb_link_layer_entry_needed";
  break;
 case SOC_PPC_DIAG_EG_DROP_REASON_STP_BLOCK:
    str = "stp_block";   
    break;
 case SOC_PPC_DIAG_EG_DROP_REASON_OUT_LIF_WITH_DROP:
   str = "out_lif_with_drop";  
   break;
 case SOC_PPC_DIAG_EG_DROP_REASON_EEDB_LAST_ENTRY_NOT_AC:
  str = "eedb_last_entry_not_ac"; 
  break;
 case SOC_PPC_DIAG_EG_DROP_REASON_OTM_INVALID:
    str = "out_tm_port_invalid";   
    break;
 case SOC_PPC_DIAG_EG_DROP_REASON_IPV4_VERSION_ERR:
   str = "ipv4_version_err";  
   break;
 case SOC_PPC_DIAG_EG_DROP_REASON_IPV6_VERSION_ERR:
   str = "ipv6_version_err";  
   break;
 case SOC_PPC_DIAG_EG_DROP_REASON_IPV4_CHECKSUM_ERR:
   str = "ipv4_checksum_err";  
   break;
 case SOC_PPC_DIAG_EG_DROP_REASON_IPV4_IHL_SHORT:
   str = "ipv4_ihl_short";  
   break;
 case SOC_PPC_DIAG_EG_DROP_REASON_IPV4_TOTAL_LEGNTH:
   str = "ipv4_total_legnth";  
   break;
 case SOC_PPC_DIAG_EG_DROP_REASON_IPV4_TTL_1:
    str = "ipv4_ttl_1";   
    break;
 case SOC_PPC_DIAG_EG_DROP_REASON_IPV6_TTL_1:
    str = "ipv6_ttl_1";   
    break;
 case SOC_PPC_DIAG_EG_DROP_REASON_IPV4_WITH_OPTIONS:
   str = "ipv4_with_options";  
   break;
 case SOC_PPC_DIAG_EG_DROP_REASON_IPV4_TTL_0:
    str = "ipv4_ttl_0";   
    break;
 case SOC_PPC_DIAG_EG_DROP_REASON_IPV6_TTL_0:
    str = "ipv6_ttl_0";   
    break;
 case SOC_PPC_DIAG_EG_DROP_REASON_IPV4_SIP_EQUAL_DIP:
   str = "ipv4_sip_equal_dip";  
   break;
 case SOC_PPC_DIAG_EG_DROP_REASON_IPV4_DIP_IS_ZERO:
   str = "ipv4_dip_is_zero";  
   break;
 case SOC_PPC_DIAG_EG_DROP_REASON_IPV4_SIP_IS_MC:
   str = "ipv4_sip_is_mc";  
   break;
 case SOC_PPC_DIAG_EG_DROP_REASON_IPV6_SIP_IS_MC:
   str = "ipv6_sip_is_mc";  
   break;
 case SOC_PPC_DIAG_EG_DROP_REASON_IPV6_DIP_UNSPECIFIED:
   str = "ipv6_dip_unspecified"; 
   break;
 case SOC_PPC_DIAG_EG_DROP_REASON_IPV6_SIP_UNSPECIFIED:
   str = "ipv6_sip_unspecified"; 
   break;
 case SOC_PPC_DIAG_EG_DROP_REASON_IPV6_LOOPBACK:
   str = "ipv6_loopback";  
   break;
 case SOC_PPC_DIAG_EG_DROP_REASON_IPV6_HOP_BY_HOP:
   str = "ipv6_hop_by_hop";  
   break;
 case SOC_PPC_DIAG_EG_DROP_REASON_IPV6_LINK_LOCAL_DEST:
   str = "ipv6_link_local_dest"; 
   break;
 case SOC_PPC_DIAG_EG_DROP_REASON_IPV6_SITE_LOCAL_DEST:
   str = "ipv6_site_local_dest"; 
   break;
 case SOC_PPC_DIAG_EG_DROP_REASON_IPV6_LINK_LOCAL_SRC:
   str = "ipv6_link_local_src";  
   break;
 case SOC_PPC_DIAG_EG_DROP_REASON_IPV6_SITE_LOCAL_SRC:
   str = "ipv6_site_local_src";  
   break;
 case SOC_PPC_DIAG_EG_DROP_REASON_IPV6_IPV4_COMPATIBLE:
   str = "ipv6_ipv4_compatible"; 
   break;
 case SOC_PPC_DIAG_EG_DROP_REASON_IPV6_IPV4_MAPPED:
   str = "ipv6_ipv4_mapped";  
   break;
 case SOC_PPC_DIAG_EG_DROP_REASON_IPV6_DEST_MC:
    str = "ipv6_dest_mc";  
    break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_DIAG_PKT_TM_FIELD_to_string(
    SOC_SAND_IN  SOC_PPC_DIAG_PKT_TM_FIELD enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_DIAG_PKT_TM_FIELD_TC:
    str = "tc";
  break;
  case SOC_PPC_DIAG_PKT_TM_FIELD_DP:
    str = "dp";
  break;
  case SOC_PPC_DIAG_PKT_TM_FIELD_DEST:
    str = "dest";
  break;
  case SOC_PPC_DIAG_PKT_TM_FIELD_METER1:
    str = "meter1";
  break;
  case SOC_PPC_DIAG_PKT_TM_FIELD_METER2:
    str = "meter2";
  break;
  case SOC_PPC_DIAG_PKT_TM_FIELD_MTR_CMD:
    str = "mtr_cmd";
  break;
  case SOC_PPC_DIAG_PKT_TM_FIELD_COUNTER1:
    str = "counter1";
  break;
  case SOC_PPC_DIAG_PKT_TM_FIELD_COUNTER2:
    str = "counter2";
  break;
  case SOC_PPC_DIAG_PKT_TM_FIELD_CUD:
    str = "cud";
  break;
  case SOC_PPC_DIAG_PKT_TM_FIELD_ETH_METER_PTR:
    str = "eth_meter_ptr";
  break;
  case SOC_PPC_DIAG_PKT_TM_FIELD_ING_SHAPING_DA:
    str = "ing_shaping_da";
  break;
  case SOC_PPC_DIAG_PKT_TM_FIELD_ECN_CAPABLE:
    str = "ecn_capable";
  break;
  case SOC_PPC_DIAG_PKT_TM_FIELD_CNI:
    str = "cni";
  break;
  case SOC_PPC_DIAG_PKT_TM_FIELD_DA_TYPE:
    str = "da_type";
  break;
  case SOC_PPC_DIAG_PKT_TM_FIELD_ST_VSQ_PTR:
    str = "st_vsq_ptr";
  break;
  case SOC_PPC_DIAG_PKT_TM_FIELD_LAG_LB_KEY:
    str = "lag_lb_key";
  break;
  case SOC_PPC_DIAG_PKT_TM_FIELD_IGNORE_CP:
    str = "ignore_cp";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_PPC_DIAG_BUFFER_print(
    SOC_SAND_IN  SOC_PPC_DIAG_BUFFER *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < SOC_PPC_DIAG_BUFF_MAX_SIZE; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "buff[%u]: 0x%08x\n\r"),ind,info->buff[ind]));
  }
  LOG_CLI((BSL_META_U(unit,
                      "buff_len: %u\n\r"),info->buff_len));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_MODE_INFO_print(
    SOC_SAND_IN  SOC_PPC_DIAG_MODE_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "flavors:\n\r")));
  if (info->flavor == 0)
  {
    LOG_CLI((BSL_META_U(unit,
                        " No flavors \n\r")));
    goto exit;
  }

  for (ind = 0; ind < 32; ++ind)
  {
    if (soc_sand_bitstream_test_bit(&(info->flavor),ind))
    {
      LOG_CLI((BSL_META_U(unit,
                          "%s "), SOC_PPC_DIAG_FLAVOR_to_string((SOC_PPC_DIAG_FLAVOR)SOC_SAND_BIT(ind))));
    }
  }
  LOG_CLI((BSL_META_U(unit,
                      "\n\r")));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_VSI_INFO_print(
    SOC_SAND_IN  SOC_PPC_DIAG_VSI_INFO *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Last VSI: %d (0x%04x)\n\r"), info->vsi, info->vsi));
  
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_DB_USE_INFO_print(
    SOC_SAND_IN  SOC_PPC_DIAG_DB_USE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,"\n------------------------------\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      " lkup_num: %s , bank_id: %s \n\r"),SOC_PPC_DIAG_DB_USAGE_INFO_LKUP_NUM_to_string(info->lkup_num),
           SOC_PPC_DIAG_DB_USAGE_INFO_BANK_ID_to_string(info->bank_id)));
  LOG_CLI((BSL_META_U(unit,"------------------------------\n\r")));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_TCAM_USE_INFO_print(
    SOC_SAND_IN  SOC_PPC_DIAG_TCAM_USE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "lkup_num: %u\n\r"),info->lkup_num));
  LOG_CLI((BSL_META_U(unit,
                      "lkup_usage %s "), SOC_PPC_DIAG_TCAM_USAGE_to_string(info->lkup_usage)));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_RECEIVED_PACKET_INFO_print(
    SOC_SAND_IN  SOC_PPC_DIAG_RECEIVED_PACKET_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "packet_header:\n\r")));
  SOC_PPC_DIAG_BUFFER_print(&(info->packet_header));
  LOG_CLI((BSL_META_U(unit,
                      "in_tm_port: %u\n\r"),info->in_tm_port));
  LOG_CLI((BSL_META_U(unit,
                      "src_sys_port: ")));
  soc_sand_SAND_PP_SYS_PORT_ID_print(&(info->src_sys_port));
  LOG_CLI((BSL_META_U(unit,
                      "in_pp_port: %u\n\r"),info->in_pp_port));
  LOG_CLI((BSL_META_U(unit,
                      "pp_context: %s\n\r"), SOC_TMC_PORT_HEADER_TYPE_to_string(info->pp_context)));
  LOG_CLI((BSL_META_U(unit,
                      "packet_qual: %u\n\r"),info->packet_qual));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_TRAP_INFO_print(
    SOC_SAND_IN  SOC_PPC_DIAG_TRAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if (info->is_pkt_trapped)
  {
    LOG_CLI((BSL_META_U(unit,
                        "code %s, BCM API: %s\n"), SOC_PPC_TRAP_CODE_to_string(info->code), SOC_PPC_TRAP_CODE_to_api_string(info->code)));
    SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO_print(&(info->info));
  }
  else
  {
    LOG_CLI((BSL_META_U(unit,
                        " Not valid \n")));
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_SNOOP_INFO_print(
    SOC_SAND_IN  SOC_PPC_DIAG_SNOOP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "code %s, BCM API: %s\n"), SOC_PPC_TRAP_CODE_to_string(info->code), SOC_PPC_TRAP_CODE_to_api_string(info->code)));
  LOG_CLI((BSL_META_U(unit,
                      "info:\n")));
  SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO_print(&(info->info));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_TRAPS_INFO_print(
    SOC_SAND_IN  SOC_PPC_DIAG_TRAPS_INFO *info
  )
{
  uint32
    ind;
  uint8
    have_traps = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "\n\rCommitted_trap:")));
  SOC_PPC_DIAG_TRAP_INFO_print(&(info->committed_trap));
  LOG_CLI((BSL_META_U(unit,
                      "\n\rCommitted_snoop:")));
  SOC_PPC_DIAG_SNOOP_INFO_print(&(info->committed_snoop));
  LOG_CLI((BSL_META_U(unit,
                      "\n\rTrap_updated_dest:")));
  SOC_PPC_DIAG_TRAP_INFO_print(&(info->trap_updated_dest));

  LOG_CLI((BSL_META_U(unit,
                      "\nConsidered trap_code:\n\n")));

  for (ind = 0; ind < SOC_PPC_NOF_TRAP_CODES; ++ind)
  {
    if (soc_sand_bitstream_test_bit(info->trap_stack,ind))
    {
      have_traps = TRUE;
      LOG_CLI((BSL_META_U(unit,
                          "   %s \n\tBCM API: %s \n\ttrap_strength:%d\n"), SOC_PPC_TRAP_CODE_to_string((SOC_PPC_TRAP_CODE)ind), SOC_PPC_TRAP_CODE_to_api_string((SOC_PPC_TRAP_CODE)ind), info->trap_stack_strength[ind]));

      
      if (info->additional_trap_stack_strength[ind] != 0)
      {
        LOG_CLI((BSL_META_U(unit,
                            "\tvtt_trap_strength:%d\n"), info->additional_trap_stack_strength[ind]));
      }
    }
  }

  if(!have_traps) {
    LOG_CLI((BSL_META_U(unit,
                        " No traps.\n\r")));
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_TRAP_TO_CPU_INFO_print(
    SOC_SAND_IN  SOC_PPC_DIAG_TRAP_TO_CPU_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "cpu_dest: %u\n\r"),info->cpu_dest));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_IPV4_VPN_ROUTE_KEY_print(
    SOC_SAND_IN  SOC_PPC_DIAG_IPV4_VPN_ROUTE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "vrf_id: %u\n\r"),info->vrf_id));
  SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY_print(&(info->subnet));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_EXTEND_P2P_KEY_print(
    SOC_SAND_IN  SOC_PPC_DIAG_EXTEND_P2P_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "type %s "), SOC_PPC_DIAG_LIF_LKUP_TYPE_to_string(info->type)));
  SOC_PPC_DIAG_LIF_KEY_print(&info->key,info->type);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void
  SOC_PPC_DIAG_LEM_KEY_print(
    SOC_SAND_IN  SOC_PPC_DIAG_LEM_KEY *info,
    SOC_SAND_IN  SOC_PPC_DIAG_LEM_LKUP_TYPE type
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  switch(type)
  {
  case SOC_PPC_DIAG_LEM_LKUP_TYPE_MACT:
    LOG_CLI((BSL_META_U(unit,
                        "mact:\n\r")));
    SOC_PPC_FRWRD_MACT_ENTRY_KEY_print(&(info->mact));
  break;
  case SOC_PPC_DIAG_LEM_LKUP_TYPE_BMACT:
    LOG_CLI((BSL_META_U(unit,
                        "bmact:\n\r")));
    SOC_PPC_BMACT_ENTRY_KEY_print(&(info->bmact));
  break;
  case SOC_PPC_DIAG_LEM_LKUP_TYPE_HOST:
    LOG_CLI((BSL_META_U(unit,
                        "host:\n\r")));
    SOC_PPC_DIAG_IPV4_VPN_ROUTE_KEY_print(&(info->host));
  break;
  case SOC_PPC_DIAG_LEM_LKUP_TYPE_ILM:
    LOG_CLI((BSL_META_U(unit,
                        "ilm:\n\r")));
    SOC_PPC_FRWRD_ILM_KEY_print(&(info->ilm));
  break;
  case SOC_PPC_DIAG_LEM_LKUP_TYPE_SA_AUTH:
    LOG_CLI((BSL_META_U(unit,
                        "sa_auth:\n\r")));
    soc_sand_SAND_PP_MAC_ADDRESS_print(&(info->sa_auth));
  break;
  case SOC_PPC_DIAG_LEM_LKUP_TYPE_EXTEND_P2P:
    SOC_PPC_DIAG_EXTEND_P2P_KEY_print(&(info->extend_p2p_key));
    break;
  default:
    for (ind = 0; ind < 3; ++ind)
    {
      LOG_CLI((BSL_META_U(unit,
                          "raw[%u]: 0x%08x\n\r"),ind,info->raw[ind]));
    }
    break;
  }
  LOG_CLI((BSL_META_U(unit,
                      "\n\r")));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_LIF_KEY_print(
    SOC_SAND_IN  SOC_PPC_DIAG_LIF_KEY *info,
    SOC_SAND_IN  SOC_PPC_DIAG_LIF_LKUP_TYPE lkup_type
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  switch(lkup_type)
  {
  case SOC_PPC_DIAG_LIF_LKUP_TYPE_AC:
    SOC_PPC_L2_LIF_AC_KEY_print(&(info->ac));
  break;
  case SOC_PPC_DIAG_LIF_LKUP_TYPE_PWE:
    LOG_CLI((BSL_META_U(unit,
                        "pwe: %u\n\r"),info->pwe));
  break;
  case SOC_PPC_DIAG_LIF_LKUP_TYPE_MPLS_TUNNEL:
    SOC_PPC_MPLS_LABEL_RIF_KEY_print(&(info->mpls));
  break;
  case SOC_PPC_DIAG_LIF_LKUP_TYPE_IPV4_TUNNEL:
      SOC_PPC_RIF_IP_TERM_KEY_print(&info->ip_tunnel);
  break;
  case SOC_PPC_DIAG_LIF_LKUP_TYPE_MIM_ISID:
    SOC_PPC_L2_LIF_ISID_KEY_print(&(info->mim));
  break;
  case SOC_PPC_DIAG_LIF_LKUP_TYPE_L2GRE:
    SOC_PPC_L2_LIF_GRE_KEY_print(&(info->l2gre));
  break;
  case SOC_PPC_DIAG_LIF_LKUP_TYPE_VXLAN:
    SOC_PPC_L2_LIF_VXLAN_KEY_print(&(info->vxlan));
  break;
  case SOC_PPC_DIAG_LIF_LKUP_TYPE_EXTENDER:
    SOC_PPC_L2_LIF_EXTENDER_KEY_print(&(info->extender));
  break;
  default:
    for (ind = 0; ind < 6; ++ind)
    {
      LOG_CLI((BSL_META_U(unit,
                          "raw[%u]: 0x%08x\n\r"),ind,info->raw[ind]));
    }
  break;
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_LIF_VALUE_print(
    SOC_SAND_IN  SOC_PPC_DIAG_LIF_VALUE *info,
    SOC_SAND_IN  SOC_PPC_DIAG_LIF_LKUP_TYPE lkup_type
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  switch(lkup_type)
  {
  case SOC_PPC_DIAG_LIF_LKUP_TYPE_AC:
    SOC_PPC_L2_LIF_AC_INFO_print(&(info->ac));
  break;
  case SOC_PPC_DIAG_LIF_LKUP_TYPE_PWE:
    SOC_PPC_L2_LIF_PWE_INFO_print(&(info->pwe));
  break;
  case SOC_PPC_DIAG_LIF_LKUP_TYPE_MPLS_TUNNEL:
    SOC_PPC_MPLS_TERM_INFO_print(&(info->mpls));
  break;
  case SOC_PPC_DIAG_LIF_LKUP_TYPE_IPV4_TUNNEL:
    SOC_PPC_RIF_IP_TERM_INFO_print(&(info->ip));
  break;
  case SOC_PPC_DIAG_LIF_LKUP_TYPE_MIM_ISID:
    SOC_PPC_L2_LIF_ISID_INFO_print(&(info->mim));
  break;
  case SOC_PPC_DIAG_LIF_LKUP_TYPE_L2GRE:
    SOC_PPC_L2_LIF_GRE_INFO_print(&(info->l2gre));
  break;
  case SOC_PPC_DIAG_LIF_LKUP_TYPE_VXLAN:
    SOC_PPC_L2_LIF_VXLAN_INFO_print(&(info->vxlan));
  break;
  case SOC_PPC_DIAG_LIF_LKUP_TYPE_EXTENDER:
    SOC_PPC_L2_LIF_EXTENDER_INFO_print(&(info->extender));
  break;
  default:
    for (ind = 0; ind < 4; ++ind)
    {
      LOG_CLI((BSL_META_U(unit,
                          "raw[%u]: 0x%08x  "),ind,info->raw[ind]));
    }
  break;
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_LIF_LKUP_INFO_print(
    SOC_SAND_IN  SOC_PPC_DIAG_LIF_LKUP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if (info->found)
  {
      LOG_CLI((BSL_META_U(unit,
                          "type %s "), SOC_PPC_DIAG_LIF_LKUP_TYPE_to_string(info->type)));
      LOG_CLI((BSL_META_U(unit,
                          "\n\rbase_index: %u\n\r"),info->base_index));
      LOG_CLI((BSL_META_U(unit,
                          "opcode_id: %u\n\r"),info->opcode_id));
      LOG_CLI((BSL_META_U(unit,
                        "value:\n\r")));
    SOC_PPC_DIAG_LIF_VALUE_print(&(info->value),info->type);
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_TCAM_KEY_print(
    SOC_SAND_IN  SOC_PPC_DIAG_TCAM_KEY *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < SOC_PPC_DIAG_TCAM_KEY_NOF_UINT32S_MAX; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "val[%u]: %u\n\r"),ind,info->val[ind]));
  }
  for (ind = 0; ind < SOC_PPC_DIAG_TCAM_KEY_NOF_UINT32S_MAX; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "mask[%u]: %u\n\r"),ind,info->mask[ind]));
  }
  LOG_CLI((BSL_META_U(unit,
                      "length: %u\n\r"),info->length));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_TCAM_VALUE_print(
    SOC_SAND_IN  SOC_PPC_DIAG_TCAM_VALUE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "fec_ptr: %u\n\r"),info->fec_ptr));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_LEM_VALUE_print(
    SOC_SAND_IN  SOC_PPC_DIAG_LEM_VALUE *info,
    SOC_SAND_IN  SOC_PPC_DIAG_LEM_LKUP_TYPE type
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  switch(type)
  {
  case SOC_PPC_DIAG_LEM_LKUP_TYPE_MACT:
    LOG_CLI((BSL_META_U(unit,
                        "mact:\n\r")));
    SOC_PPC_FRWRD_MACT_ENTRY_VALUE_print(&(info->mact));
  break;
  case SOC_PPC_DIAG_LEM_LKUP_TYPE_BMACT:
    LOG_CLI((BSL_META_U(unit,
                        "bmact:\n\r")));
    SOC_PPC_BMACT_ENTRY_INFO_print(&(info->bmact));
  break;
  case SOC_PPC_DIAG_LEM_LKUP_TYPE_HOST:
    SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO_print(&info->host);
  break;
  case SOC_PPC_DIAG_LEM_LKUP_TYPE_ILM:
    LOG_CLI((BSL_META_U(unit,
                        "ilm:\n\r")));
    SOC_PPC_FRWRD_DECISION_INFO_print(&(info->ilm));
  break;
  case SOC_PPC_DIAG_LEM_LKUP_TYPE_SA_AUTH:
    LOG_CLI((BSL_META_U(unit,
                        "sa_auth:\n\r")));
    SOC_PPC_LLP_SA_AUTH_MAC_INFO_print(&(info->sa_auth));
    LOG_CLI((BSL_META_U(unit,
                        "VID-assignment:\n\r")));
    SOC_PPC_LLP_VID_ASSIGN_MAC_INFO_print(&(info->sa_vid_assign));
  break;
  default:
    for (ind = 0; ind < 2; ++ind)
    {
      LOG_CLI((BSL_META_U(unit,
                          "raw[%u]: 0x%08x\n\r"),ind,info->raw[ind]));
    }
   break;
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_PARSING_L2_INFO_print(
    SOC_SAND_IN  SOC_PPC_DIAG_PARSING_L2_INFO *info
  )
{
  uint32
    ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "encap_type %s "), SOC_SAND_PP_ETH_ENCAP_TYPE_to_string(info->encap_type)));
  LOG_CLI((BSL_META_U(unit,
                      "tag_fromat:")));
  SOC_PPC_LLP_PARSE_INFO_print(&(info->tag_fromat));
  LOG_CLI((BSL_META_U(unit,
                      "vlan_tags:")));
  for (ind = 0; ind < 2; ind++)
  {
    LOG_CLI((BSL_META_U(unit,
                        "\n\rvlan_tag[%u]:"), ind));
    LOG_CLI((BSL_META_U(unit,
                        "tpid: %08x  "), info->vlan_tags[ind].tpid));
    LOG_CLI((BSL_META_U(unit,
                        "vid: %u  "),info->vlan_tags[ind].vid));
  }
  LOG_CLI((BSL_META_U(unit,
                      "\n\rvlan_tag_format: %d (%s) \n\r"), info->vlan_tag_format, soc_sand_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_to_string(info->vlan_tag_format)));
  LOG_CLI((BSL_META_U(unit,
                      "next_prtcl %s \n\r"), SOC_PPC_L2_NEXT_PRTCL_TYPE_to_string(info->next_prtcl)));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_PARSING_MPLS_INFO_print(
    SOC_SAND_IN  SOC_PPC_DIAG_PARSING_MPLS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "bos: %u\n\r"),info->bos));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_PARSING_IP_INFO_print(
    SOC_SAND_IN  SOC_PPC_DIAG_PARSING_IP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "next_prtcl %s "), SOC_PPC_L3_NEXT_PRTCL_TYPE_to_string(info->next_prtcl)));
  LOG_CLI((BSL_META_U(unit,
                      "is_mc: %u\n\r"),info->is_mc));
  LOG_CLI((BSL_META_U(unit,
                      "is_fragmented: %u\n\r"),info->is_fragmented));
  LOG_CLI((BSL_META_U(unit,
                      "hdr_err: %u\n\r"),info->hdr_err));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_PARSING_HEADER_INFO_print(
    SOC_SAND_IN  SOC_PPC_DIAG_PARSING_HEADER_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "hdr_type %s "), SOC_PPC_PKT_HDR_TYPE_to_string(info->hdr_type)));
  LOG_CLI((BSL_META_U(unit,
                      "hdr_offset: %u bytes\n\r"),info->hdr_offset));
  switch(info->hdr_type)
  {
    case SOC_PPC_PKT_HDR_TYPE_ETH:
      SOC_PPC_DIAG_PARSING_L2_INFO_print(&(info->eth));
    break;
    case SOC_PPC_PKT_HDR_TYPE_IPV4:
    case SOC_PPC_PKT_HDR_TYPE_IPV6:
      SOC_PPC_DIAG_PARSING_IP_INFO_print(&(info->ip));
    break;
    case SOC_PPC_PKT_HDR_TYPE_MPLS:
      SOC_PPC_DIAG_PARSING_MPLS_INFO_print(&(info->mpls));
    break;
    default:
      break;
  }
  
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_PARSING_INFO_print(
    int unit,
    SOC_SAND_IN  SOC_PPC_DIAG_PARSING_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "hdr_type %s \n\r"), dpp_parser_pfc_string_by_hw(unit, info->pfc_hw)));

  for (ind = 0; ind < SOC_PPC_DIAG_MAX_NOF_HDRS; ++ind)
  {
    if (info->hdrs_stack[ind].hdr_type == SOC_PPC_PKT_HDR_TYPE_NONE)
    {
      break;
    }
    LOG_CLI((BSL_META_U(unit,
                        "\n\rheader[%u]:\n\r"),ind));
    SOC_PPC_DIAG_PARSING_HEADER_INFO_print(&(info->hdrs_stack[ind]));
  }

  if (info->initial_vid != PPC_API_DIAG_INVALID_INITIAL_VID) {
      LOG_CLI((BSL_META_U(unit,
                          "\n\rinitial_vid %u \n\r"), info->initial_vid));
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_TERM_MPLS_LABEL_INFO_print(
    SOC_SAND_IN  SOC_PPC_DIAG_TERM_MPLS_LABEL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "range_index: %u\n\r"), info->range_index));
  LOG_CLI((BSL_META_U(unit,
                      "range:")));
  SOC_PPC_MPLS_TERM_LABEL_RANGE_print(&(info->range));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_TERM_INFO_print(
    SOC_SAND_IN  SOC_PPC_DIAG_TERM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "term_type %s%s "),
                      (info->is_scnd_my_mac ? "eth_" : ""),
                      SOC_PPC_PKT_TERM_TYPE_to_string(info->term_type)));
  if (info->term_type == SOC_PPC_PKT_TERM_TYPE_NONE)
  {
    goto exit;
  }
  LOG_CLI((BSL_META_U(unit,
                      "\n\rfrwrd_type %s "), SOC_PPC_PKT_FRWRD_TYPE_to_string(info->frwrd_type)));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_IPV4_VPN_KEY_print(
    SOC_SAND_IN  SOC_PPC_DIAG_IPV4_VPN_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "vrf: %u\n\r"),info->vrf));
  LOG_CLI((BSL_META_U(unit,
                      "key:\n\r")));
  SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY_print(&(info->key));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_IPV6_VPN_KEY_print(
    SOC_SAND_IN  SOC_PPC_DIAG_IPV6_VPN_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "vrf: %u\n\r"),info->vrf));
  LOG_CLI((BSL_META_U(unit,
                      "key:\n\r")));
  SOC_PPC_FRWRD_IPV6_VPN_ROUTE_KEY_print(&(info->key));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if defined(INCLUDE_KBP)
void
  SOC_PPC_DIAG_IPV4_UNICAST_RPF_print(
    SOC_SAND_IN SOC_PPC_DIAG_IPV4_UNICAST_RPF *info
  )
{
  char
    decimal_ip[SOC_SAND_PP_IPV4_MAX_IP_STRING];
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Master Key:\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      "\tvrf : %d\n\r"), info->vrf));
  LOG_CLI((BSL_META_U(unit,
                      "\tSIP: %s\n\r"), soc_sand_pp_ip_long_to_string(info->sip,1,decimal_ip)));
  LOG_CLI((BSL_META_U(unit,
                      "\tDIP: %s\n\r"), soc_sand_pp_ip_long_to_string(info->dip,1,decimal_ip)));

  LOG_CLI((BSL_META_U(unit,
                      "\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      "Search 1:\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      "\tvrf : %d\n\r"), info->vrf));
  LOG_CLI((BSL_META_U(unit,
                      "\tDIP: %s\n\r"), soc_sand_pp_ip_long_to_string(info->dip,1,decimal_ip)));

  LOG_CLI((BSL_META_U(unit,
                      "\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      "Search 2:\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      "\tvrf : %d\n\r"), info->vrf));
  LOG_CLI((BSL_META_U(unit,
                      "\tSIP: %s\n\r"), soc_sand_pp_ip_long_to_string(info->sip,1,decimal_ip)));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_IPV4_MULTICAST_print(
    SOC_SAND_IN SOC_PPC_DIAG_IPV4_MULTICAST *info
  )
{
  char
    decimal_ip[SOC_SAND_PP_IPV4_MAX_IP_STRING];
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Master Key:\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      "\tvrf : %d\n\r"), info->vrf));
  LOG_CLI((BSL_META_U(unit,
                      "\tin_rif : %d\n\r"), info->in_rif));
  LOG_CLI((BSL_META_U(unit,
                      "\tSIP: %s\n\r"), soc_sand_pp_ip_long_to_string(info->sip,1,decimal_ip)));
  LOG_CLI((BSL_META_U(unit,
                      "\tDIP: %s\n\r"), soc_sand_pp_ip_long_to_string(info->dip,1,decimal_ip)));

  LOG_CLI((BSL_META_U(unit,
                      "\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      "Search 1:\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      "\tin-rif : %d\n\r"), info->in_rif));
  LOG_CLI((BSL_META_U(unit,
                      "\tDIP: %s\n\r"), soc_sand_pp_ip_long_to_string(info->dip,1,decimal_ip)));

  LOG_CLI((BSL_META_U(unit,
                      "\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      "Search 2:\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      "\tvrf : %d\n\r"), info->vrf));
  LOG_CLI((BSL_META_U(unit,
                      "\tSIP: %s\n\r"), soc_sand_pp_ip_long_to_string(info->sip,1,decimal_ip)));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_IPV6_UNICAST_RPF_print(
    SOC_SAND_IN SOC_PPC_DIAG_IPV6_UNICAST_RPF *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Master Key:\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      "\tvrf : %d\n\r"), info->vrf));
  LOG_CLI((BSL_META_U(unit,
                      "\tSIP: ")));
  soc_sand_SAND_PP_IPV6_ADDRESS_print(&info->sip);
  LOG_CLI((BSL_META_U(unit,
                      "\n\r")));

  LOG_CLI((BSL_META_U(unit,
                      "\tDIP: ")));
  soc_sand_SAND_PP_IPV6_ADDRESS_print(&info->dip);
  LOG_CLI((BSL_META_U(unit,
                      "\n\r")));

  LOG_CLI((BSL_META_U(unit,
                      "\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      "Search 1:\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      "\tvrf : %d\n\r"), info->vrf));
  LOG_CLI((BSL_META_U(unit,
                      "\tDIP: ")));
  soc_sand_SAND_PP_IPV6_ADDRESS_print(&info->dip);
  LOG_CLI((BSL_META_U(unit,
                      "\n\r")));

  LOG_CLI((BSL_META_U(unit,
                      "\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      "Search 2:\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      "\tvrf : %d\n\r"), info->vrf));
  LOG_CLI((BSL_META_U(unit,
                      "\tSIP: ")));
  soc_sand_SAND_PP_IPV6_ADDRESS_print(&info->sip);
  LOG_CLI((BSL_META_U(unit,
                      "\n\r")));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_IPV6_MULTICAST_print(
    SOC_SAND_IN SOC_PPC_DIAG_IPV6_MULTICAST *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Master Key:\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      "\tvrf : %d\n\r"), info->vrf));
  LOG_CLI((BSL_META_U(unit,
                      "\tin_rif : %d\n\r"), info->in_rif));
  LOG_CLI((BSL_META_U(unit,
                      "\tSIP: ")));
  soc_sand_SAND_PP_IPV6_ADDRESS_print(&info->sip);
  LOG_CLI((BSL_META_U(unit,
                      "\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      "\tDIP: ")));
  soc_sand_SAND_PP_IPV6_ADDRESS_print(&info->dip);
  LOG_CLI((BSL_META_U(unit,
                      "\n\r")));

  LOG_CLI((BSL_META_U(unit,
                      "\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      "Search 1:\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      "\tin_rif : %d\n\r"), info->in_rif));
  LOG_CLI((BSL_META_U(unit,
                      "\tSIP: ")));
  soc_sand_SAND_PP_IPV6_ADDRESS_print(&info->sip);
  LOG_CLI((BSL_META_U(unit,
                      "\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      "\tDIP: ")));
  soc_sand_SAND_PP_IPV6_ADDRESS_print(&info->dip);
  LOG_CLI((BSL_META_U(unit,
                      "\n\r")));

  LOG_CLI((BSL_META_U(unit,
                      "\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      "Search 2:\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      "\tvrf : %d\n\r"), info->vrf));
  LOG_CLI((BSL_META_U(unit,
                      "\tSIP: ")));
  soc_sand_SAND_PP_IPV6_ADDRESS_print(&info->sip);
  LOG_CLI((BSL_META_U(unit,
                      "\n\r")));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_MPLS_print(
    SOC_SAND_IN SOC_PPC_DIAG_MPLS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Master Key:\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      "\tin_rif : %d\n\r"), info->in_rif));
  LOG_CLI((BSL_META_U(unit,
                      "\tin_port : %d\n\r"), info->in_port));
  LOG_CLI((BSL_META_U(unit,
                      "\texp : %d\n\r"), info->exp));
  LOG_CLI((BSL_META_U(unit,
                      "\tmpls_label : %d\n\r"), info->mpls_label));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_TRILL_UNICAST_print(
    SOC_SAND_IN SOC_PPC_DIAG_TRILL_UNICAST *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Master Key:\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      "\tegress_nick : %d\n\r"), info->egress_nick));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_TRILL_MULTICAST_print(
    SOC_SAND_IN SOC_PPC_DIAG_TRILL_MULTICAST *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Master Key:\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      "\tesdai : %d\n\r"), info->esdai));
  LOG_CLI((BSL_META_U(unit,
                      "\tfid_vsi : %d\n\r"), info->fid_vsi));
  LOG_CLI((BSL_META_U(unit,
                      "\tdist_tree_nick : %d\n\r"), info->dist_tree_nick));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_IP_REPLY_RECORD_print(
    SOC_SAND_IN SOC_PPC_DIAG_IP_REPLY_RECORD *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "\tkey found: %s\n\r"), info->match_status ? "Yes" : "No"));

  if(info->match_status) {
    LOG_CLI((BSL_META_U(unit,
                        "\tdynamic: %s\n\r"), info->is_synamic ? "Yes" : "No"));
    LOG_CLI((BSL_META_U(unit,
                        "\tP2P Service: %s\n\r"), info->p2p_service ? "Yes": "No"));
    if(info->identifier == 0x0) {
      LOG_CLI((BSL_META_U(unit,
                          "\tOut lif is %svalid\n\r"), info->out_lif_valid ? "": "not "));
      if(info->out_lif_valid) {
        LOG_CLI((BSL_META_U(unit,
                            "\tOut Lif : %d\n\r"), info->out_lif));
      }
      LOG_CLI((BSL_META_U(unit,
                          "\tDestination:\t")));
      SOC_PPC_FRWRD_DECISION_INFO_print(&info->destination);
      LOG_CLI((BSL_META_U(unit,
                          "\n\r")));
    }
    if(info->identifier == 0x1) {
      LOG_CLI((BSL_META_U(unit,
                          "\tEEI : %d\n\r"), info->eei));
      LOG_CLI((BSL_META_U(unit,
                          "\tFec Ptr: %d\n\r"), info->fec_ptr));
    }
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_SECOND_IP_REPLY_RECORD_print(
    SOC_SAND_IN SOC_PPC_DIAG_SECOND_IP_REPLY_RECORD *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if(info->match_status) {
    LOG_CLI((BSL_META_U(unit,
                        "\tKey found: Yes\n\r")));
    LOG_CLI((BSL_META_U(unit,
                        "\tType: fec\n\r")));
    LOG_CLI((BSL_META_U(unit,
                        "\tdest_id: %d\n\r"), info->dest_id));
  } else {
    LOG_CLI((BSL_META_U(unit,
                        "\tKey found: No\n\r")));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
 
#endif 
void
  SOC_PPC_DIAG_FRWRD_LKUP_KEY_print(
    SOC_SAND_IN  SOC_PPC_DIAG_FRWRD_LKUP_KEY *info,
    SOC_SAND_IN  SOC_PPC_DIAG_FWD_LKUP_TYPE frwrd_type,
    SOC_SAND_IN  uint8                      is_kbp
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  switch(frwrd_type)
  {
    case SOC_PPC_DIAG_FWD_LKUP_TYPE_MACT:
      LOG_CLI((BSL_META_U(unit,
                          "mact:\n\r")));
      SOC_PPC_FRWRD_MACT_ENTRY_KEY_print(&(info->mact));
    break;
    case SOC_PPC_DIAG_FWD_LKUP_TYPE_BMACT:
      LOG_CLI((BSL_META_U(unit,
                          "bmact:\n\r")));
      SOC_PPC_BMACT_ENTRY_KEY_print(&(info->bmact));
    break;
    case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_UC:
      LOG_CLI((BSL_META_U(unit,
                          "ipv4_uc:\n\r")));
#if defined(INCLUDE_KBP)
      if(is_kbp) {
        SOC_PPC_DIAG_IPV4_UNICAST_RPF_print(&(info->kbp_ipv4_unicast_rpf));
      } else
#endif 
      { 
        SOC_PPC_DIAG_IPV4_VPN_KEY_print(&(info->ipv4_uc));
      }
    break;
    case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_VPN:
    case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_HOST:
      LOG_CLI((BSL_META_U(unit,
                          "ipv4_vpn:\n\r")));
      SOC_PPC_DIAG_IPV4_VPN_KEY_print(&(info->ipv4_uc));
      break;
    case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_MC:
      LOG_CLI((BSL_META_U(unit,
                          "ipv4_mc:\n\r")));
#if defined(INCLUDE_KBP)
      if(is_kbp) {
        SOC_PPC_DIAG_IPV4_MULTICAST_print(&(info->kbp_ipv4_multicast));
      } else
#endif 
      {
        SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY_print(&(info->ipv4_mc));
      }
    break;
    case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV6_UC:
      LOG_CLI((BSL_META_U(unit,
                          "ipv6_uc:\n\r")));
#if defined(INCLUDE_KBP)
      if(is_kbp) {
        SOC_PPC_DIAG_IPV6_UNICAST_RPF_print(&(info->kbp_ipv6_unicast_rpf));
      } else
#endif 
      {
        SOC_PPC_DIAG_IPV6_VPN_KEY_print(&(info->ipv6_uc));
      }
    break;
    case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV6_MC:
      LOG_CLI((BSL_META_U(unit,
                          "ipv6_mc:\n\r")));
#if defined(INCLUDE_KBP)
      if(is_kbp) {
        SOC_PPC_DIAG_IPV6_MULTICAST_print(&(info->kbp_ipv6_multicast));
      } else
#endif 
      {
        SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY_print(&(info->ipv6_mc));
      }
    break;
    case SOC_PPC_DIAG_FWD_LKUP_TYPE_ILM:
      LOG_CLI((BSL_META_U(unit,
                          "ilm:\n\r")));
#if defined(INCLUDE_KBP)
      if(is_kbp) {
        SOC_PPC_DIAG_MPLS_print(&(info->kbp_mpls));
      } else
#endif 
      {
        SOC_PPC_FRWRD_ILM_KEY_print(&(info->ilm));
      }
    break;
    case SOC_PPC_DIAG_FWD_LKUP_TYPE_TRILL_UC:
      LOG_CLI((BSL_META_U(unit,
                          "trill_uc:\n\r")));
#if defined(INCLUDE_KBP)
      if(is_kbp) {
        SOC_PPC_DIAG_TRILL_UNICAST_print(&(info->kbp_trill_unicast));
      } else
#endif 
      {
        LOG_CLI((BSL_META_U(unit,
                            "trill_uc: %u\n\r"),info->trill_uc));
      }
    break;
    case SOC_PPC_DIAG_FWD_LKUP_TYPE_TRILL_MC:
      LOG_CLI((BSL_META_U(unit,
                          "trill_mc:\n\r")));
#if defined(INCLUDE_KBP)
      if(is_kbp) {
        SOC_PPC_DIAG_TRILL_MULTICAST_print(&(info->kbp_trill_multicast));
      } else
#endif 
      {
        SOC_PPC_TRILL_MC_ROUTE_KEY_print(&(info->trill_mc));
      }
    break;
  case SOC_PPC_DIAG_FWD_LKUP_TYPE_FCF:
    LOG_CLI((BSL_META_U(unit,
                        "trill_mc:\n\r")));
    SOC_PPC_FRWRD_FCF_ROUTE_KEY_print(&(info->fcf));
  break;
    default:
    for (ind = 0; ind < 2; ++ind)
    {
      LOG_CLI((BSL_META_U(unit,
                          "raw[%u]: 0x%08x\n\r"),ind,info->raw[ind]));
    }
    break;
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_FRWRD_LKUP_VALUE_print(
    SOC_SAND_IN  SOC_PPC_DIAG_FRWRD_LKUP_VALUE *info,
    SOC_SAND_IN  SOC_PPC_DIAG_FWD_LKUP_TYPE frwrd_type
  )
{
  uint32
    ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  switch(frwrd_type)
  {
  case SOC_PPC_DIAG_FWD_LKUP_TYPE_MACT:
    LOG_CLI((BSL_META_U(unit,
                        "mact:\n\r")));
    SOC_PPC_FRWRD_MACT_ENTRY_VALUE_print(&(info->mact));
  break;
  case SOC_PPC_DIAG_FWD_LKUP_TYPE_BMACT:
    LOG_CLI((BSL_META_U(unit,
                        "bmact:\n\r")));
    SOC_PPC_BMACT_ENTRY_INFO_print(&(info->bmact));
  break;
  case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_UC:
  case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_VPN:
  case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_MC:
  case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV6_UC:
  case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV6_MC:
  case SOC_PPC_DIAG_FWD_LKUP_TYPE_ILM:
  case SOC_PPC_DIAG_FWD_LKUP_TYPE_TRILL_UC:
  case SOC_PPC_DIAG_FWD_LKUP_TYPE_TRILL_MC:
    LOG_CLI((BSL_META_U(unit,
                        "frwrd_decision:\n\r")));
    SOC_PPC_FRWRD_DECISION_INFO_print(&(info->frwrd_decision));
  break;
  case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_HOST:
    LOG_CLI((BSL_META_U(unit,
                        "host_lookup:\n\r")));
    SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO_print(&(info->host_info));
  break;
  default:
    for (ind = 0; ind < 2; ++ind)
    {
      LOG_CLI((BSL_META_U(unit,
                          "raw[%u]: 0x%08x\n\r"),ind,info->raw[ind]));
    }
    break;
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_FRWRD_LKUP_INFO_print(
    SOC_SAND_IN  SOC_PPC_DIAG_FRWRD_LKUP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "frwrd_type %s "), SOC_PPC_DIAG_FWD_LKUP_TYPE_to_string(info->frwrd_type)));
  LOG_CLI((BSL_META_U(unit,
                      "frwrd_hdr_index: %u\n\r"), info->frwrd_hdr_index));
  LOG_CLI((BSL_META_U(unit,
                      "lkup_key:\n\r")));
#if defined(INCLUDE_KBP)
  if(info->is_kbp) {
    LOG_CLI((BSL_META_U(unit,
                        "look up device: ELK\n\r")));
  }
  SOC_PPC_DIAG_FRWRD_LKUP_KEY_print(&(info->lkup_key),info->frwrd_type, info->is_kbp);
#else
  SOC_PPC_DIAG_FRWRD_LKUP_KEY_print(&(info->lkup_key),info->frwrd_type, FALSE);
#endif 
#if defined(INCLUDE_KBP)
  if(info->is_kbp) {
    LOG_CLI((BSL_META_U(unit,
                        "Result 1:\n\r")));
    SOC_PPC_DIAG_IP_REPLY_RECORD_print(&(info->ip_reply_record));
    LOG_CLI((BSL_META_U(unit,
                        "Result 2:\n\r")));
    SOC_PPC_DIAG_SECOND_IP_REPLY_RECORD_print(&(info->second_ip_reply_result));
  } else
#endif 
  {
    LOG_CLI((BSL_META_U(unit,
                        "key_found: %u\n\r"),info->key_found));
    if (info->key_found)
    {
      LOG_CLI((BSL_META_U(unit,
                          "lkup_res:\n\r")));
      SOC_PPC_DIAG_FRWRD_LKUP_VALUE_print(&(info->lkup_res),info->frwrd_type);
    }
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_LEARN_INFO_print(
    SOC_SAND_IN  SOC_PPC_DIAG_LEARN_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if (info->valid)
  {
    LOG_CLI((BSL_META_U(unit,
                        "ingress: %u\n\r"),info->ingress));
    LOG_CLI((BSL_META_U(unit,
                        "is_new_key: %u\n\r"),info->is_new_key));
    LOG_CLI((BSL_META_U(unit,
                        "key:\n\r")));
    SOC_PPC_FRWRD_MACT_ENTRY_KEY_print(&(info->key));
    LOG_CLI((BSL_META_U(unit,
                        "value:\n\r")));
    SOC_PPC_FRWRD_MACT_ENTRY_VALUE_print(&(info->value));
  }
  else
  {
    LOG_CLI((BSL_META_U(unit,
                        " No Learning info for packet.\n\r")));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_VLAN_EDIT_RES_print(
    SOC_SAND_IN  SOC_PPC_DIAG_VLAN_EDIT_RES *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if (info->adv_mode != SOC_SAND_U32_MAX) {
      LOG_CLI((BSL_META_U(unit,
                          "adv_edit_mode: %u\n\r"),info->adv_mode));
  }

  LOG_CLI((BSL_META_U(unit,
                      "cmd_id: %u\n\r"),info->cmd_id));
  LOG_CLI((BSL_META_U(unit,
                      "ac_tag:")));
  LOG_CLI((BSL_META_U(unit,
                      "vid: %u  "),info->ac_tag.vid));
  LOG_CLI((BSL_META_U(unit,
                      "\n\rac_tag2:")));
  LOG_CLI((BSL_META_U(unit,
                      "vid: %u  "),info->ac_tag2.vid));
  LOG_CLI((BSL_META_U(unit,
                      "\n\rcommand:")));
  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO_print(&(info->cmd_info));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_FRWRD_DECISION_INFO_print(
    SOC_SAND_IN  SOC_PPC_DIAG_FRWRD_DECISION_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if (info->frwrd_decision.type != SOC_PPC_NOF_FRWRD_DECISION_TYPES)
  {
    SOC_PPC_FRWRD_DECISION_INFO_print_table_format("",&(info->frwrd_decision));
  }
  else
  {
        LOG_CLI((BSL_META_U(unit,
                            "       -        Not Valid       -          ")));
  }

  LOG_CLI((BSL_META_U(unit,"|")));

  if (info->outlif != 0)
  {
        LOG_CLI((BSL_META_U(unit,
                            " %-5u|"), info->outlif));
  }
  
  else
  {
        LOG_CLI((BSL_META_U(unit,
                            "  -   |")));
  }


exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_FRWRD_DECISION_TRACE_INFO_print(
    SOC_SAND_IN  SOC_PPC_DIAG_FRWRD_DECISION_TRACE_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit, " -------------------------------------------------------------------------------------------------\n\r")));
  LOG_CLI((BSL_META_U(unit, "|                                     Forwarding decision/TRAP Trace                              |\n\r")));
  LOG_CLI((BSL_META_U(unit, "|               | Destination  |     Additional Info        |      |     Trap info              |\n\r")));
  LOG_CLI((BSL_META_U(unit, "|   Phase       | Type  | Val  | Type |                     |outlif|   trap-code     |str| qual |\n\r")));
  LOG_CLI((BSL_META_U(unit, " -------------------------------------------------------------------------------------------------\n\r")));
  for (ind = 0; ind < SOC_PPC_NOF_DIAG_FRWRD_DECISION_PHASES; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "| %-12s  |"), SOC_PPC_DIAG_FRWRD_DECISION_PHASE_to_string(ind)));
    SOC_PPC_DIAG_FRWRD_DECISION_INFO_print(&(info->frwrd[ind]));
    if (ind != SOC_PPC_DIAG_FRWRD_DECISION_PHASE_ING_RESOLVED && info->trap[ind].action_profile.frwrd_action_strength > 0)
    {
      LOG_CLI((BSL_META_U(unit,
                          "%-17s|"), SOC_PPC_TRAP_CODE_to_string(info->trap[ind].action_profile.trap_code)));
      LOG_CLI((BSL_META_U(unit,
                          " %u |"), info->trap[ind].action_profile.frwrd_action_strength));
      LOG_CLI((BSL_META_U(unit,
                          " %-6u |\n\r"), info->trap_qual[ind]));
    }
    else
    {
      LOG_CLI((BSL_META_U(unit,
                          "%-17s|"), "            -"));
      LOG_CLI((BSL_META_U(unit,
                          "%s|"), "      |"));
      LOG_CLI((BSL_META_U(unit,
                          " %s |\n\r"), "-" ));
    }
  }
  LOG_CLI((BSL_META_U(unit,
                      " -------------------------------------------------------------------------------------------------\n\r"
               )));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_PKT_TM_INFO_print(
    SOC_SAND_IN  SOC_PPC_DIAG_PKT_TM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if (info->valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_DEST)
  {
    LOG_CLI((BSL_META_U(unit,
                        "frwrd_decision:\n\r")));
    SOC_PPC_FRWRD_DECISION_INFO_print(&(info->frwrd_decision));
    LOG_CLI((BSL_META_U(unit,
                        "\n\r")));
  }
  else
  {
    LOG_CLI((BSL_META_U(unit,
                        "frwrd_decision: Not Valid\n\r")));
  }
  if (info->valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_TC)
  {
    LOG_CLI((BSL_META_U(unit,
                        "tc: %u\n\r"), info->tc));
  }
  else
  {
    LOG_CLI((BSL_META_U(unit,
                        "tc: Not Valid\n\r")));
  }
  if (info->valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_DP)
  {
    LOG_CLI((BSL_META_U(unit,
                        "dp: %u\n\r"), info->dp));
  }
  else
  {
    LOG_CLI((BSL_META_U(unit,
                        "dp: Not Valid\n\r")));
  }
  if (info->valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_METER1)
  {
    LOG_CLI((BSL_META_U(unit,
                        "meter0: %u\n\r"), info->meter1));
  }
  else
  {
    LOG_CLI((BSL_META_U(unit,
                        "meter0: Not Valid\n\r")));
  }
  if (info->valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_METER2)
  {
    LOG_CLI((BSL_META_U(unit,
                        "meter1: %u\n\r"), info->meter2));
  }
  else
  {
    LOG_CLI((BSL_META_U(unit,
                        "meter1: Not Valid\n\r")));
  }
  if (info->valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_MTR_CMD)
  {
    LOG_CLI((BSL_META_U(unit,
                        "dp_meter_cmd: %u\n\r"), info->dp_meter_cmd));
  }
  else
  {
    LOG_CLI((BSL_META_U(unit,
                        "dp_meter_cmd: Not Valid\n\r")));
  }
  if (info->valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_COUNTER1)
  {
    LOG_CLI((BSL_META_U(unit,
                        "counter1: %u\n\r"), info->counter1));
  }
  else
  {
    LOG_CLI((BSL_META_U(unit,
                        "counter1: Not Valid\n\r")));
  }
  if (info->valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_COUNTER2)
  {
    LOG_CLI((BSL_META_U(unit,
                        "counter2: %u\n\r"), info->counter2));
  }
  else
  {
    LOG_CLI((BSL_META_U(unit,
                        "counter2: Not Valid\n\r")));
  }
  if (info->valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_CUD)
  {
    LOG_CLI((BSL_META_U(unit,
                        "cud: %u\n\r"), info->cud));
  }
  else
  {
    LOG_CLI((BSL_META_U(unit,
                        "cud: Not Valid\n\r")));
  }
  if (info->valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_ETH_METER_PTR)
  {
    LOG_CLI((BSL_META_U(unit,
                        "eth_meter_ptr: %u\n\r"), info->eth_meter_ptr));
  }
  else
  {
    LOG_CLI((BSL_META_U(unit,
                        "eth_meter_ptr: Not Valid\n\r")));
  }
  if (info->valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_ING_SHAPING_DA)
  {
    LOG_CLI((BSL_META_U(unit,
                        "ingress_shaping_da: %u\n\r"), info->ingress_shaping_da));
  }
  else
  {
      if(info->ingress_shaping_da != -1) {
          LOG_CLI((BSL_META_U(unit,
                  "ingress_shaping_da: Not Valid\n\r")));
      }
  }
  if (info->valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_ECN_CAPABLE)
  {
    LOG_CLI((BSL_META_U(unit,
                        "ecn_capable: %u\n\r"), info->ecn_capable));
  }
  else
  {
      if(info->ecn_capable != -1) {
          LOG_CLI((BSL_META_U(unit,
                  "ecn_capable: Not Valid\n\r")));
      }
  }
  if (info->valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_CNI)
  {
    LOG_CLI((BSL_META_U(unit,
                        "cni: %u\n\r"), info->cni));
  }
  else
  {
      if(info->cni != -1) {
          LOG_CLI((BSL_META_U(unit,
                  "cni: Not Valid\n\r")));
      }
  }
  if (info->valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_DA_TYPE)
  {
    LOG_CLI((BSL_META_U(unit,
                        "da_type: %s\n\r"), soc_sand_SAND_PP_ETHERNET_DA_TYPE_to_string(info->da_type)));
  }
  else
  {
      if(info->da_type != -1) {
          LOG_CLI((BSL_META_U(unit,
                  "da_type: Not Valid\n\r")));
      }
  }
  if (info->valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_ST_VSQ_PTR)
  {
    LOG_CLI((BSL_META_U(unit,
                        "st_vsq_ptr: %u\n\r"), info->st_vsq_ptr));
  }
  else
  {
    LOG_CLI((BSL_META_U(unit,
                        "st_vsq_ptr: Not Valid\n\r")));
  }
  if (info->valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_LAG_LB_KEY)
  {
    LOG_CLI((BSL_META_U(unit,
                        "lag_lb_key: %u\n\r"), info->lag_lb_key));
  }
  else
  {
    LOG_CLI((BSL_META_U(unit,
                        "lag_lb_key: Not Valid\n\r")));
  }
  if (info->valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_IGNORE_CP)
  {
    LOG_CLI((BSL_META_U(unit,
                        "ignore_cp: %u\n\r"), info->ignore_cp));
  }
  else
  {
      if(info->ignore_cp != -1) {
          LOG_CLI((BSL_META_U(unit,
                  "ignore_cp: Not Valid\n\r")));
      }
  }
  if (info->valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_SNOOP_ID)
  {
    LOG_CLI((BSL_META_U(unit,
                        "snoop id: %u\n\r"), info->snoop_id));
  }
  else
  {
    LOG_CLI((BSL_META_U(unit,
                        "snoop_id: Not Valid\n\r")));
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_ENCAP_INFO_print(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PPC_DIAG_ENCAP_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if (info->eei.type == SOC_PPC_EEI_TYPE_TRILL)
  {
    LOG_CLI((BSL_META_U(unit, "trill explicit data:\n\r")));
    LOG_CLI((BSL_META_U(unit, "    is_multicast: %s\n\r"),
                 info->eei.val.trill_dest.is_multicast ? "YES" : "NO"));
    LOG_CLI((BSL_META_U(unit, "    egress_nick_name: %u\n\r"),
                 info->eei.val.trill_dest.dest_nick));
    LOG_CLI((BSL_META_U(unit, "\n\r")));
  }

  if (info->mpls_cmd.command != SOC_PPC_NOF_MPLS_COMMAND_TYPES) 
  {
    LOG_CLI((BSL_META_U(unit,
                        "mpls_cmd:\n\r")));
    SOC_PPC_MPLS_COMMAND_print(&(info->mpls_cmd));
    LOG_CLI((BSL_META_U(unit,
                        "\n\r")));  
  }

  for (ind = 0; ind < info->nof_eeps; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "eep[%u]: %u\n\r"),ind,info->eep[ind]));

    LOG_CLI((BSL_META_U(unit,
                        "encap_info[%u]:"),ind));
    SOC_PPC_EG_ENCAP_ENTRY_INFO_print(unit, &(info->encap_info[ind]));
    LOG_CLI((BSL_META_U(unit,
                        "\n\r")));
  }  
  LOG_CLI((BSL_META_U(unit,
                      "ll_vsi: %u\n\r"),info->ll_vsi));
  LOG_CLI((BSL_META_U(unit,
                      "out_ac: %u\n\r"),info->out_ac));
  LOG_CLI((BSL_META_U(unit,
                      "pp_port: %u\n\r"),info->pp_port));
  LOG_CLI((BSL_META_U(unit,
                      "tm_port: %u\n\r"),info->tm_port));
  if (info->rif_is_valid) {
      LOG_CLI((BSL_META_U(unit,
                          "Native Out-RIF: %u\n\n\r"),info->out_rif));
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_EG_DROP_LOG_INFO_print(
    SOC_SAND_IN  SOC_PPC_DIAG_EG_DROP_LOG_INFO *info
  )
{
  uint32
    ind;
  uint32
    enum_set=0;
  

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "drops:\n\r")));
  if (info->drop_log[0] == 0 && info->drop_log[1] == 0)
  {
    LOG_CLI((BSL_META_U(unit,
                        " No drops\n\r")));
    goto exit;
  }
  for (ind = 0; ind < (SOC_PPC_NOF_DIAG_EG_DROP_REASONS - 1); ++ind)
  {

    
    if(ind == 31) {
        enum_set = SOC_PPC_DIAG_FILTER_SECOND_SET;
    }

    if (soc_sand_bitstream_test_bit((info->drop_log),ind))
    {
      LOG_CLI((BSL_META_U(unit,
                          " %s \n\r"), SOC_PPC_DIAG_EG_DROP_REASON_to_string((SOC_PPC_DIAG_EG_DROP_REASON)(enum_set | SOC_SAND_BIT(ind%31)))));
    }
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_PPC_DIAG_ETH_PACKET_INFO_print(
    SOC_SAND_IN  SOC_PPC_DIAG_ETH_PACKET_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "ether_type: %u\n\r"),info->ether_type));
  LOG_CLI((BSL_META_U(unit,
                      "da:\n\r")));
  soc_sand_SAND_PP_MAC_ADDRESS_print(&(info->da));
  LOG_CLI((BSL_META_U(unit,
                      "sa:")));
  soc_sand_SAND_PP_MAC_ADDRESS_print(&(info->sa));
  LOG_CLI((BSL_META_U(unit,
                      "is_tagged: %u\n\r"),info->is_tagged));
  LOG_CLI((BSL_META_U(unit,
                      "tag:")));
  SOC_SAND_PP_VLAN_TAG_print(&(info->tag));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void 
  SOC_PPC_DIAG_EGRESS_VLAN_EDIT_INFO_print(
     SOC_SAND_IN SOC_PPC_DIAG_EGRESS_VLAN_EDIT_INFO *prm_vec_res
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(prm_vec_res);

  LOG_CLI((BSL_META_U(unit,
                      "Egress vlan edit info:\n\r")));

  
  LOG_CLI((BSL_META_U(unit,
                      "\n\rCommand key: \n\r")));
  SOC_PPC_EG_VLAN_EDIT_COMMAND_KEY_print(&(prm_vec_res->key));

  
  LOG_CLI((BSL_META_U(unit,
                      "\n\rCommand information: \n\r")));
  SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO_print(&(prm_vec_res->cmd_info));

  
  LOG_CLI((BSL_META_U(unit,
                      "\n\rac tag vlan id: %u\n\r"), (prm_vec_res->ac_tag).vid));

  
  LOG_CLI((BSL_META_U(unit,
                      "\n\rac tag 2 vlan id: %u\n\r"), (prm_vec_res->ac_tag2).vid));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_TRILL_KEY_print(
    SOC_SAND_IN SOC_PPC_DIAG_TRILL_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);  

  if (info->trill_key_type == 0) 
  { 
    LOG_CLI((BSL_META_U(unit,
                        "trill key TRILL MC RPF:\n\r")));
    LOG_CLI((BSL_META_U(unit,
                        "dist_tree: %u\n\r"), (info->dist_tree)));
    LOG_CLI((BSL_META_U(unit,
                        "ing_nick: %u\n\r"), (info->ing_nick)));
    LOG_CLI((BSL_META_U(unit,
                        "link_adj_id: %u\n\r"), (info->link_adj_id)));
    LOG_CLI((BSL_META_U(unit,
                        "port: %u\n\r"), (info->port)));
  }

  if (info->trill_key_type == 1) 
  { 
    LOG_CLI((BSL_META_U(unit,
                        "trill key TRILL NICK:\n\r")));
    LOG_CLI((BSL_META_U(unit,
                        "nickname: %u\n\r"), (info->nick_name)));    
  }

  if (info->trill_key_type == 2) 
  { 
    LOG_CLI((BSL_META_U(unit,
                        "trill key TRILL APPOINTED FORWARDER\r")));
    LOG_CLI((BSL_META_U(unit,
                        "vsi: %u\n\r"), (info->vsi)));
    LOG_CLI((BSL_META_U(unit,
                        "port: %u\n\r"), (info->port)));
  }

  if (info->trill_key_type == 3) 
  {
     LOG_CLI((BSL_META_U(unit,
                         "trill key NATIVE INNER TPID:\n\r")));
     LOG_CLI((BSL_META_U(unit,
                         "native_inner_tpid: %u\n\r"), (info->native_inner_tpid)));
  }

  if (info->trill_key_type == 4) 
  {    
    LOG_CLI((BSL_META_U(unit,
                        "trill key TRILL VSI:\n\r")));
    LOG_CLI((BSL_META_U(unit,
                        "high_vid: %u\n\r"), (info->high_vid)));
    LOG_CLI((BSL_META_U(unit,
                        "low_vid: %u\n\r"), (info->low_vid)));
  }
  

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void soc_ppc_diag_glem_outlif_print(soc_ppc_diag_glem_outlif_t *info){
    int unit = BSL_UNIT_UNKNOWN;
    char *source = NULL, *look_num = "";
    uint8 display_source = TRUE, display_accessed = FALSE;
    
    
    switch (info->source) {
    case soc_ppc_diag_glem_outlif_source_ftmh:
        source = "FTMH.LIF";
        look_num = " #1";
        break;
    case soc_ppc_diag_glem_outlif_source_eei:
        source = "EEI";
        look_num = " #2";
        break;
    case soc_ppc_diag_glem_outlif_source_cud1:
        source = "CUD1";
        look_num = " #1";
        break;
    case soc_ppc_diag_glem_outlif_source_cud2:
        source = "CUD2";
        look_num = " #2";
        break;
    case soc_ppc_diag_glem_outlif_source_user:
         
        display_source = FALSE;
        display_accessed = TRUE;
        break;
    default:
        return;
    }

    
    LOG_CLI((BSL_META_U(unit,
                        "Glem lookup%s:\n\n"
                        "  Key:\n"
                        "    Global-Out-Lif: 0x%08x\n"), look_num, info->global_outlif));
    if (display_source) {
        LOG_CLI((BSL_META_U(unit,
                        "    Taken from %s.\n"), source));
    }

    
    LOG_CLI((BSL_META_U(unit,
                        "\n  Result:\n")));

    if (info->found) {
        LOG_CLI((BSL_META_U(unit, 
                        "    Local lif: 0x%08x\n"
                        "    Global lif mapping is %s.\n"), 
                        info->local_outlif,
                        ((info->mapped) ? "mapped" : "direct")
                        ));

        if (display_accessed) {
            LOG_CLI((BSL_META_U(unit, 
                        "      %s accessed before.\n"),
                        ((info->accessed) ? "Was" : "Wasn't")));
        }
    }
    else {
        LOG_CLI((BSL_META_U(unit,
                        "    NOT FOUND!\n")));
    }

    LOG_CLI((BSL_META_U(unit, "\n\n")));

    return;
}



#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

