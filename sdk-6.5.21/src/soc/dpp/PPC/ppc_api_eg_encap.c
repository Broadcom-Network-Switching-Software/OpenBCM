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

#include <soc/dpp/PPC/ppc_api_eg_encap.h>
#include <soc/dpp/JER/JER_PP/jer_pp_eg_encap.h>





















void
  SOC_PPC_EG_ENCAP_RANGE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_RANGE_INFO *info
  )
{
  uint32
      ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_EG_ENCAP_RANGE_INFO));
  info->ll_limit = 0;
  info->ip_tnl_limit = 0;
  for (ind = 0; ind < SOC_DPP_DEFS_MAX(EG_ENCAP_NOF_BANKS); ++ind)
  {
    info->bank_access_phase[ind] = SOC_PPC_EG_ENCAP_ACCESS_PHASE_ONE;
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_PPC_EG_ENCAP_PROTECTION_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_PROTECTION_INFO   *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(*info));
  info->is_protection_valid = 0;
  info->protection_pointer = 0;
  info->protection_pass_value = 0;

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_PPC_EG_ENCAP_SWAP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_SWAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_EG_ENCAP_SWAP_INFO));
  info->swap_label = 0;
  info->out_vsi = 0;
  info->oam_lif_set = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_ENCAP_PWE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_PWE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_EG_ENCAP_PWE_INFO));
  info->label = 0;
  info->push_profile = 0;
  info->orientation = SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB;
  info->oam_lif_set = 0;
  SOC_PPC_EG_ENCAP_PROTECTION_INFO_clear(&(info->protection_info));
  SOC_PPC_EG_ENCAP_MPLS_ENCAP_INFO_clear(&(info->egress_tunnel_label_info));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_ENCAP_POP_INTO_ETH_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_POP_INTO_ETH_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_EG_ENCAP_POP_INTO_ETH_INFO));
  info->tpid_profile = 0;
  info->has_cw = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_ENCAP_POP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_POP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_EG_ENCAP_POP_INFO));
  info->pop_type = SOC_PPC_NOF_MPLS_COMMAND_TYPES;
  SOC_PPC_EG_ENCAP_POP_INTO_ETH_INFO_clear(&(info->ethernet_info));
  info->pop_next_header = SOC_PPC_NOF_PKT_FRWRD_TYPES; 
  info->model = SOC_SAND_PP_NOF_MPLS_TUNNEL_MODELS;    
  info->oam_lif_set = 0;
  info->out_vsi = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_ENCAP_MPLS_TUNNEL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_MPLS_TUNNEL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_EG_ENCAP_MPLS_TUNNEL_INFO));
  info->tunnel_label = 0;
  info->push_profile = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_ENCAP_MPLS_ENCAP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_MPLS_ENCAP_INFO *info
  )
{
  uint32
    ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_EG_ENCAP_MPLS_ENCAP_INFO));
  for (ind = 0; ind < SOC_PPC_EG_ENCAP_MPLS_MAX_NOF_TUNNELS; ++ind)
  {
    SOC_PPC_EG_ENCAP_MPLS_TUNNEL_INFO_clear(&(info->tunnels[ind]));
  }
  info->addition_label = BCM_MPLS_LABEL_INVALID;
  info->nof_tunnels = 0;
  info->orientation = SOC_SAND_PP_NOF_HUB_SPOKE_ORIENTATIONS;
  info->out_vsi = 0;
  info->oam_lif_set = 0;
  SOC_PPC_EG_ENCAP_PROTECTION_INFO_clear(&(info->protection_info));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_ENCAP_IPV4_TUNNEL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_IPV4_TUNNEL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_EG_ENCAP_IPV4_TUNNEL_INFO));
  info->dest = 0;
  info->src_index = 0;
  info->ttl_index = 0;
  info->tos_index = 0;
  info->enable_gre = 0;
  info->encapsulation_mode = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_ENCAP_IPV4_ENCAP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_IPV4_ENCAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_EG_ENCAP_IPV4_ENCAP_INFO));
  SOC_PPC_EG_ENCAP_IPV4_TUNNEL_INFO_clear(&(info->dest));
  info->out_vsi = 0;
  info->oam_lif_set = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_ENCAP_IPV6_TUNNEL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_IPV6_TUNNEL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_EG_ENCAP_IPV6_TUNNEL_INFO));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_ENCAP_IPV6_ENCAP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_IPV6_ENCAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_EG_ENCAP_IPV6_ENCAP_INFO));
  SOC_PPC_EG_ENCAP_IPV6_TUNNEL_INFO_clear(&(info->tunnel));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_ENCAP_MIRROR_TUNNEL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_MIRROR_TUNNEL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_EG_ENCAP_MIRROR_TUNNEL_INFO));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_ENCAP_MIRROR_ENCAP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_MIRROR_ENCAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_EG_ENCAP_MIRROR_ENCAP_INFO));
  SOC_PPC_EG_ENCAP_MIRROR_TUNNEL_INFO_clear(&(info->tunnel));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_ENCAP_LL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_LL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_EG_ENCAP_LL_INFO));
  soc_sand_SAND_PP_MAC_ADDRESS_clear(&(info->dest_mac));
  info->out_vid_valid = 0;
  info->out_vid = 0;
  info->pcp_dei_valid = 0;
  info->pcp_dei = 0;
  info->tpid_index = 0;
  info->ll_remark_profile = 0; 
  info->out_ac_lsb = 0;
  info->out_ac_valid = 0;
  info->oam_lif_set = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_PPC_EG_ENCAP_VSI_ENCAP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_VSI_ENCAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);
  sal_memset(info, 0x0, sizeof(SOC_PPC_EG_ENCAP_VSI_ENCAP_INFO));
  info->out_vsi = 0;
  info->remark_profile = 0; 
  info->oam_lif_set = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void 
  SOC_PPC_EG_ENCAP_DATA_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_DATA_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);
  sal_memset(info, 0x0, sizeof(SOC_PPC_EG_ENCAP_DATA_INFO)); 
  info->oam_lif_set = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void 
  SOC_PPC_EG_ENCAP_DATA_PROG_ENTRY_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_DATA_PROG_ENTRY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);
  sal_memset(info, 0x0, sizeof(SOC_PPC_EG_ENCAP_DATA_PROG_ENTRY_INFO)); 
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO));
  info->model = SOC_SAND_PP_NOF_MPLS_TUNNEL_MODELS;
  info->exp_model = SOC_SAND_PP_NOF_MPLS_TUNNEL_MODELS;
  info->ttl_model = SOC_SAND_PP_NOF_MPLS_TUNNEL_MODELS;
  info->has_cw = 0;
  info->ttl = 0;
  info->exp_mark_mode = SOC_PPC_NOF_EG_ENCAP_EXP_MARK_MODES;
  info->exp = 0;
  info->add_entropy_label = 0;
  info->add_entropy_label_indicator = 0;
  info->remark_profile = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void 
  SOC_PPC_EG_ENCAP_ADDITIONAL_LABEL_PROFILE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_ADDITIONAL_LABEL_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_EG_ENCAP_ADDITIONAL_LABEL_PROFILE_INFO));
  info->exp = 0;
  info->ttl = 0;
  info->exp_model = SOC_SAND_PP_NOF_MPLS_TUNNEL_MODELS;
  info->ttl_model = SOC_SAND_PP_NOF_MPLS_TUNNEL_MODELS;
  info->label_type = 0;
  info->label = -1;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_ENCAP_PUSH_EXP_KEY_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_PUSH_EXP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_EG_ENCAP_PUSH_EXP_KEY));
  info->push_profile = 0;
  info->tc = 0;
  info->dp = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_ENCAP_PWE_GLBL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_PWE_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_EG_ENCAP_PWE_GLBL_INFO));
  info->cw = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_ENCAP_GLBL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_EG_ENCAP_GLBL_INFO));
  info->l2_gre_prtcl_type = 0;
  info->unkown_gre_prtcl_type = 0;
  info->unkown_ip_next_prtcl_type = 0;
  info->eth_ip_val = 0;
  info->unkown_ll_ether_type = 0;
  info->vxlan_udp_dest_port = 0;

  SOC_SAND_MAGIC_NUM_SET;

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



void
  SOC_PPC_EG_ENCAP_ENTRY_VALUE_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_ENTRY_VALUE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_EG_ENCAP_ENTRY_VALUE));
  SOC_PPC_EG_ENCAP_SWAP_INFO_clear(&(info->swap_info));
  SOC_PPC_EG_ENCAP_PWE_INFO_clear(&(info->pwe_info));
  SOC_PPC_EG_ENCAP_POP_INFO_clear(&(info->pop_info));
  SOC_PPC_EG_ENCAP_MPLS_ENCAP_INFO_clear(&(info->mpls_encap_info));
  SOC_PPC_EG_ENCAP_IPV4_ENCAP_INFO_clear(&(info->ipv4_encap_info));
  SOC_PPC_EG_ENCAP_IPV6_ENCAP_INFO_clear(&(info->ipv6_encap_info));
  SOC_PPC_EG_ENCAP_MIRROR_ENCAP_INFO_clear(&(info->mirror_encap_info));
  SOC_PPC_EG_ENCAP_LL_INFO_clear(&(info->ll_info));
  SOC_PPC_EG_ENCAP_VSI_ENCAP_INFO_clear(&(info->vsi_info));
  SOC_PPC_EG_ENCAP_DATA_INFO_clear(&(info->data_info));
  SOC_PPC_EG_ENCAP_OVERLAY_ARP_ENCAP_INFO_clear(&(info->overlay_arp_encap_info));
  SOC_PPC_EG_ENCAP_TRILL_INFO_clear(&(info->trill_encap_info)); 
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_PPC_EG_ENCAP_ENTRY_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_ENTRY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  info->entry_type = SOC_PPC_NOF_EG_ENCAP_ENTRY_TYPES_ARAD;
  SOC_PPC_EG_ENCAP_ENTRY_VALUE_clear(&(info->entry_val));

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void 
SOC_PPC_EG_ENCAP_IP_TUNNEL_SIZE_PROTOCOL_TEMPLATE_INFO_clear(
   SOC_PPC_EG_ENCAP_IP_TUNNEL_SIZE_PROTOCOL_TEMPLATE_INFO *info
   ) 
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_EG_ENCAP_IP_TUNNEL_SIZE_PROTOCOL_TEMPLATE_INFO));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void 
 SOC_PPC_EG_ENCAP_OVERLAY_ARP_ENCAP_INFO_print(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN SOC_PPC_EG_ENCAP_OVERLAY_ARP_ENCAP_INFO *info
    ) 
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "dest_mac:")));
  soc_sand_SAND_PP_MAC_ADDRESS_print(&(info->dest_mac));
  LOG_CLI((BSL_META_U(unit,
                      "\n\r")));

  
  if (SOC_IS_JERICHO(unit)) {
      LOG_CLI((BSL_META_U(unit,
                          "src_mac (%d lsbs):"), JER_PP_EG_ENCAP_ROO_LINK_LAYER_SA_LSBS_NOF_BITS));
  } else {
      LOG_CLI((BSL_META_U(unit,
                          "src_mac:")));
  }
  soc_sand_SAND_PP_MAC_ADDRESS_print(&(info->src_mac));
  LOG_CLI((BSL_META_U(unit,
                      "\n\r")));

  LOG_CLI((BSL_META_U(unit,
                      "pcp_dei: %u\n\r"),info->pcp_dei));
  LOG_CLI((BSL_META_U(unit,
                      "out_vid: %u\n\r"),info->out_vid));

  
  if (SOC_IS_ARADPLUS_A0(unit)) {
      LOG_CLI((BSL_META_U(unit,
                          "outer tpid: %u\n\r"),info->outer_tpid));
      LOG_CLI((BSL_META_U(unit,
                          "ether_type: %u\n\r"),info->ether_type));
      LOG_CLI((BSL_META_U(unit,
                          "ll_vsi_pointer: %u\n\r"),info->ll_vsi_pointer));

  }

  
  if (SOC_IS_JERICHO(unit)) {
      LOG_CLI((BSL_META_U(unit,
                          "inner vid: %u\n\r"),info->inner_vid));
      LOG_CLI((BSL_META_U(unit,
                          "inner pcp dei: %u\n\r"),info->inner_pcp_dei));
      LOG_CLI((BSL_META_U(unit,
                          "pcp dei profile: %u\n\r"),info->pcp_dei_profile));
      LOG_CLI((BSL_META_U(unit,
                          "remark profile: %u\n\r"),info->remark_profile));
      LOG_CLI((BSL_META_U(unit,
                          "num of tags: %u\n\r"),info->nof_tags));
      LOG_CLI((BSL_META_U(unit,
                          "eth_type_index: %u\n\r"),info->eth_type_index));
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_ENCAP_OVERLAY_ARP_ENCAP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_OVERLAY_ARP_ENCAP_INFO *info
  ) 
{
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

    sal_memset(info, 0x0, sizeof(SOC_PPC_EG_ENCAP_OVERLAY_ARP_ENCAP_INFO));
    soc_sand_SAND_PP_MAC_ADDRESS_clear(&(info->src_mac));
    soc_sand_SAND_PP_MAC_ADDRESS_clear(&(info->dest_mac));
    info->ether_type = 0;
    info->out_vid = 0;
    info->outer_tpid = 0;
    info->pcp_dei = 0;

    SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void 
  SOC_PPC_EG_ENCAP_TRILL_INFO_clear(
     SOC_SAND_OUT SOC_PPC_EG_ENCAP_TRILL_INFO *info
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);
    sal_memset(info, 0x0, sizeof(SOC_PPC_EG_ENCAP_TRILL_INFO));

    SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_EG_ENCAP_EEP_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_EEP_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_EG_ENCAP_EEP_TYPE_LIF_EEP:
    str = "lif_eep";
  break;
  case SOC_PPC_EG_ENCAP_EEP_TYPE_TUNNEL_EEP:
    str = "tunnel_eep";
  break;
  case SOC_PPC_EG_ENCAP_EEP_TYPE_LL:
    str = "ll";
  break;
  case SOC_PPC_EG_ENCAP_EEP_TYPE_VSI:
    str = "rif"; 
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_to_string(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_NONE:
    str = "none";
  break;
  case SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_ETHER_IP:
    str = "ether_ip";
  break;
  case SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_BASIC_GRE:
    str = "basic_gre";
  break;
  case SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_ENHANCE_GRE:
    str = "enhance_gre";
  break;
  case SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_VXLAN: 
      str = "vxlan";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_EG_ENCAP_EXP_MARK_MODE_to_string(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_EXP_MARK_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_EG_ENCAP_EXP_MARK_MODE_FROM_PUSH_PROFILE:
    str = "from_push_profile";
  break;
  case SOC_PPC_EG_ENCAP_EXP_MARK_MODE_MAP_TC_DP:
    str = "map_tc_dp";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_EG_ENCAP_ENTRY_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_ENTRY_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_EG_ENCAP_ENTRY_TYPE_VSI:
    str = "vsi";
  break;
  case SOC_PPC_EG_ENCAP_ENTRY_TYPE_AC:
    str = "ac";
  break;
  case SOC_PPC_EG_ENCAP_ENTRY_TYPE_SWAP_CMND:
    str = "swap_cmnd";
  break;
  case SOC_PPC_EG_ENCAP_ENTRY_TYPE_PWE:
    str = "pwe";
  break;
  case SOC_PPC_EG_ENCAP_ENTRY_TYPE_POP_CMND:
    str = "pop_cmnd";
  break;
  case SOC_PPC_EG_ENCAP_ENTRY_TYPE_MPLS_ENCAP:
    str = "mpls_encap";
  break;
  case SOC_PPC_EG_ENCAP_ENTRY_TYPE_IPV4_ENCAP:
    str = "ipv4_encap";
  break;
 case SOC_PPC_EG_ENCAP_ENTRY_TYPE_IPV6_ENCAP:
    str = "ipv6_encap";
  break;
  case SOC_PPC_EG_ENCAP_ENTRY_TYPE_LL_ENCAP:
    str = "ll_encap";
  break;
  case SOC_PPC_EG_ENCAP_ENTRY_TYPE_NULL:
    str = "null";
  break;
  default:
    str = " Unknown";
  }
  return str;
}
const char*
  SOC_PPC_EG_ENCAP_ACCESS_PHASE_to_string(
    SOC_SAND_IN SOC_PPC_EG_ENCAP_ACCESS_PHASE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_EG_ENCAP_ACCESS_PHASE_ONE:
    str = "phase one";
  break;
  case SOC_PPC_EG_ENCAP_ACCESS_PHASE_TWO:
    str = "phase two";
  break;
  case SOC_PPC_EG_ENCAP_ACCESS_PHASE_THREE:
    str = "phase three";
  break;
  case SOC_PPC_EG_ENCAP_ACCESS_PHASE_FOUR:
    str = "phase four";
  break;
  case SOC_PPC_EG_ENCAP_ACCESS_PHASE_FIVE:
    str = "phase five";
  break;
  case SOC_PPC_EG_ENCAP_ACCESS_PHASE_SIX:
    str = "phase sic";
  break;
  default:
    str = " Unknown";
  }
  return str;
}
void
  SOC_PPC_EG_ENCAP_RANGE_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_RANGE_INFO *info
  )
{
  uint32
      ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "ll_limit: %u\n\r"),info->ll_limit));
  LOG_CLI((BSL_META_U(unit,
                      "ip_tnl_limit: %u\n\r"),info->ip_tnl_limit));
  for (ind = 0; ind < SOC_DPP_DEFS_MAX(EG_ENCAP_NOF_BANKS); ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "bank_access_phase %d: %s\n\r"), ind, SOC_PPC_EG_ENCAP_ACCESS_PHASE_to_string(info->bank_access_phase[ind])));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void SOC_PPC_EG_ENCAP_PROTECTION_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PROTECTION_INFO   *info)
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if (info->is_protection_valid) {
      LOG_CLI((BSL_META_U(unit,
                          "protection_pointer: %u\n\r"),info->protection_pointer));
      LOG_CLI((BSL_META_U(unit,
                          "protection_pass_value: %u\n\r"),info->protection_pass_value));
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_PPC_EG_ENCAP_SWAP_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_SWAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "swap_label: %u\n\r"),info->swap_label));
  LOG_CLI((BSL_META_U(unit,
                      "out_vsi: %u\n\r"),info->out_vsi));
  LOG_CLI((BSL_META_U(unit,
                      "oam_lif_set: %u\n\r"),info->oam_lif_set));
  LOG_CLI((BSL_META_U(unit,
                      "outlif_profile: 0x%02x\n\r"),info->outlif_profile));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void
  SOC_PPC_EG_ENCAP_VSI_ENCAP_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_VSI_ENCAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);
  LOG_CLI((BSL_META_U(unit,
                      "out_vsi: %u\n\r"),info->out_vsi));
  LOG_CLI((BSL_META_U(unit,
                      "remark_profile: %u\n\r"),info->remark_profile));
  LOG_CLI((BSL_META_U(unit,
                      "oam_lif_set: %u\n\r"),info->oam_lif_set));
  LOG_CLI((BSL_META_U(unit,
                      "outlif_profile: 0x%02x\n\r"),info->outlif_profile));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_ENCAP_DATA_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_DATA_INFO *info
  )
{
  uint32 ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);
  for (ind = 0; ind < SOC_PPC_EG_ENCAP_DATA_INFO_MAX_SIZE; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "data_entry[%u]: 0x%08x "), ind,  info->data_entry[ind]));    
  }    
  LOG_CLI((BSL_META_U(unit,
                      "oam_lif_set: %u\n\r"),info->oam_lif_set));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_PPC_EG_ENCAP_DATA_PROG_ENTRY_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_DATA_PROG_ENTRY_INFO *info
  )
{
  uint32 ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);
  LOG_CLI((BSL_META_U(unit,
                      "prog_data_entry: ")));
  for (ind = 0; ind < SOC_PPC_EG_ENCAP_DATA_PROG_ENTRY_MAX_SIZE; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "[%u]: 0x%08x "), ind,  info->data_entry[ind]));    
  }    
  LOG_CLI((BSL_META_U(unit,
                      "\n\r")));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_ENCAP_PWE_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PWE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "label: %u\n\r"),info->label));
  LOG_CLI((BSL_META_U(unit,
                      "push_profile: %u\n\r"),info->push_profile));
  LOG_CLI((BSL_META_U(unit,
                      "oam_lif_set: %u\n\r"),info->oam_lif_set));
  LOG_CLI((BSL_META_U(unit,
                      "outlif_profile: 0x%02x\n\r"),info->outlif_profile));
  SOC_PPC_EG_ENCAP_PROTECTION_INFO_print(&(info->protection_info));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_ENCAP_POP_INTO_ETH_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_POP_INTO_ETH_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "tpid_profile: %u\n\r"),info->tpid_profile));
  LOG_CLI((BSL_META_U(unit,
                      "has_cw: %u\n\r"),info->has_cw));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_ENCAP_POP_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_POP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "pop_type %s \n\r"), SOC_PPC_MPLS_COMMAND_TYPE_to_string(info->pop_type)));
  LOG_CLI((BSL_META_U(unit,
                      "ethernet_info:")));
  SOC_PPC_EG_ENCAP_POP_INTO_ETH_INFO_print(&(info->ethernet_info));
  LOG_CLI((BSL_META_U(unit,
                      "pop_next_header %s \n\r"), SOC_PPC_PKT_FRWRD_TYPE_to_string(info->pop_type)));
  LOG_CLI((BSL_META_U(unit,
                      "model %s \n\r"), soc_sand_SAND_PP_MPLS_TUNNEL_MODEL_to_string(info->pop_type)));
  LOG_CLI((BSL_META_U(unit,
                      "oam_lif_set: %u\n\r"),info->oam_lif_set));
  LOG_CLI((BSL_META_U(unit,
                      "outlif_profile: 0x%02x\n\r"),info->outlif_profile));
  LOG_CLI((BSL_META_U(unit,
                      "sys_vsi: %u\n\r"),info->out_vsi));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_ENCAP_MPLS_TUNNEL_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_MPLS_TUNNEL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "  tunnel_label: %u\n\r"),info->tunnel_label));
  LOG_CLI((BSL_META_U(unit,
                      "  push_profile: %u\n\r"),info->push_profile));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_ENCAP_MPLS_ENCAP_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_MPLS_ENCAP_INFO *info
  )
{
  uint32
    ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "nof_tunnels: %u\n\r"),info->nof_tunnels));
  for (ind = 0; ind < info->nof_tunnels; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "tunnel %d:\n\r"), ind+1));
    SOC_PPC_EG_ENCAP_MPLS_TUNNEL_INFO_print(&(info->tunnels[ind]));
  }
  if(info->addition_label != BCM_MPLS_LABEL_INVALID)
  {
      LOG_CLI((BSL_META_U(unit,
                        "addtional label: %d\n\r"), info->addition_label));
  }

  LOG_CLI((BSL_META_U(unit,
                      "orientation %s \n\r"), soc_sand_SAND_PP_HUB_SPOKE_ORIENTATION_to_string(info->orientation)));
  LOG_CLI((BSL_META_U(unit,
                      "out_vsi: %u\n\r"),info->out_vsi));
  LOG_CLI((BSL_META_U(unit,
                      "oam_lif_set: %u\n\r"),info->oam_lif_set));
  LOG_CLI((BSL_META_U(unit,
                      "outlif_profile: 0x%02x\n\r"),info->outlif_profile));
  SOC_PPC_EG_ENCAP_PROTECTION_INFO_print(&(info->protection_info));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_ENCAP_IPV4_TUNNEL_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_IPV4_TUNNEL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "dest: %u\n\r"),info->dest));
  LOG_CLI((BSL_META_U(unit,
                      "src_index: %u\n\r"),info->src_index));
  LOG_CLI((BSL_META_U(unit,
                      "ttl_index: %u\n\r"), info->ttl_index));
  LOG_CLI((BSL_META_U(unit,
                      "tos_index: %u\n\r"), info->tos_index));
  LOG_CLI((BSL_META_U(unit,
                      "enable_gre: %u\n\r"),info->enable_gre));
  LOG_CLI((BSL_META_U(unit,
                      "encapsulation_mode: %u\n\r"),info->encapsulation_mode));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_ENCAP_IPV4_ENCAP_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_IPV4_ENCAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "dest:")));
  SOC_PPC_EG_ENCAP_IPV4_TUNNEL_INFO_print(&(info->dest));
  LOG_CLI((BSL_META_U(unit,
                      "out_vsi: %u\n\r"),info->out_vsi));
  LOG_CLI((BSL_META_U(unit,
                      "oam_lif_set: %u\n\r"),info->oam_lif_set));
  LOG_CLI((BSL_META_U(unit,
                      "outlif_profile: 0x%02x\n\r"),info->outlif_profile));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_ENCAP_IPV6_TUNNEL_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_IPV6_TUNNEL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "dest:")));
  soc_sand_SAND_PP_IPV6_ADDRESS_print(&(info->dest));
  LOG_CLI((BSL_META_U(unit,
                      "\n\r")));
  
  LOG_CLI((BSL_META_U(unit,
                      "src:")));
  soc_sand_SAND_PP_IPV6_ADDRESS_print(&(info->src));
  LOG_CLI((BSL_META_U(unit,
                      "\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      "src_index: 0x%08x\n\r"),info->flow_label));
  LOG_CLI((BSL_META_U(unit,
                      "hop_limit: 0x%08x\n\r"),info->hop_limit));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_ENCAP_IPV6_ENCAP_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_IPV6_ENCAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "tunnel:")));
  SOC_PPC_EG_ENCAP_IPV6_TUNNEL_INFO_print(&(info->tunnel));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_ENCAP_MIRROR_TUNNEL_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_MIRROR_TUNNEL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "dest:")));
  soc_sand_SAND_PP_MAC_ADDRESS_print(&(info->dest));
  LOG_CLI((BSL_META_U(unit,
                      "\n\r")));

  LOG_CLI((BSL_META_U(unit,
                      "src:")));
  soc_sand_SAND_PP_MAC_ADDRESS_print(&(info->src));
  LOG_CLI((BSL_META_U(unit,
                      "\n\r")));

  LOG_CLI((BSL_META_U(unit,
                      "erspan: %hu  "),info->erspan_id));
  LOG_CLI((BSL_META_U(unit,
                      "\n\r")));
  
  LOG_CLI((BSL_META_U(unit,
                      "tpid: %04x  "), info->tpid));
  LOG_CLI((BSL_META_U(unit,
                      "vid: %hu  "),info->vid));
  LOG_CLI((BSL_META_U(unit,
                      "pcp: %hhu  "), info->pcp));
  LOG_CLI((BSL_META_U(unit,
                      "ether_type: %04x  "), info->ether_type));
  LOG_CLI((BSL_META_U(unit,
                      "encap_id: %04x  "), info->encap_id));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_ENCAP_MIRROR_ENCAP_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_MIRROR_ENCAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "tunnel:")));
  SOC_PPC_EG_ENCAP_MIRROR_TUNNEL_INFO_print(&(info->tunnel));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_PPC_EG_ENCAP_LL_INFO_print(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_LL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "dest_mac:")));
  soc_sand_SAND_PP_MAC_ADDRESS_print(&(info->dest_mac));
  LOG_CLI((BSL_META_U(unit,
                      "\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      "out_vid_valid: %u\n\r"),info->out_vid_valid));
  LOG_CLI((BSL_META_U(unit,
                      "out_vid: %u\n\r"),info->out_vid));
  LOG_CLI((BSL_META_U(unit,
                      "pcp_dei_valid: %u\n\r"),info->pcp_dei_valid));
  LOG_CLI((BSL_META_U(unit,
                      "pcp_dei: %u\n\r"), info->pcp_dei));
  LOG_CLI((BSL_META_U(unit,
                      "tpid_index: %u\n\r"), info->tpid_index));
  LOG_CLI((BSL_META_U(unit,
                      "ll_remark_profile: %u\n\r"), info->ll_remark_profile));
  LOG_CLI((BSL_META_U(unit,
                      "out_ac_valid: %u\n\r"),info->out_ac_valid));
  LOG_CLI((BSL_META_U(unit,
                      "out_ac_lsb: %u\n\r"),info->out_ac_lsb));
  LOG_CLI((BSL_META_U(unit,
                      "oam_lif_set: %u\n\r"),info->oam_lif_set));
  LOG_CLI((BSL_META_U(unit,
                      "outlif_profile: 0x%02x\n\r"),info->outlif_profile));
  if (SOC_IS_JERICHO_PLUS(unit) && !SOC_IS_QAX_A0(unit)) {
      LOG_CLI((BSL_META_U(unit,
                          "is native: %u\n\r"), info->native_ll));    
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "model %s \n\r"), soc_sand_SAND_PP_MPLS_TUNNEL_MODEL_to_string(info->model)));
  LOG_CLI((BSL_META_U(unit,
                      "has_cw: %u\n\r"),info->has_cw));
  LOG_CLI((BSL_META_U(unit,
                      "ttl: %u\n\r"), info->ttl));
  LOG_CLI((BSL_META_U(unit,
                      "exp_mark_mode %s\n\r"), SOC_PPC_EG_ENCAP_EXP_MARK_MODE_to_string(info->exp_mark_mode)));
  LOG_CLI((BSL_META_U(unit,
                      "exp: %u\n\r"), info->exp));
  LOG_CLI((BSL_META_U(unit,
                      "add_entropy_label: %u\n\r"),info->add_entropy_label));
  LOG_CLI((BSL_META_U(unit,
                      "add_entropy_label_indicator: %u\n\r"),info->add_entropy_label_indicator));
  LOG_CLI((BSL_META_U(unit,
                      "remark_profile: %u\n\r"),info->remark_profile));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_ENCAP_ADDITIONAL_LABEL_PROFILE_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_ADDITIONAL_LABEL_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "exp: %u\n\r"), info->exp));
  LOG_CLI((BSL_META_U(unit,
                      "ttl: %u\n\r"), info->ttl));
  LOG_CLI((BSL_META_U(unit,
                      "exp model %s \n\r"), soc_sand_SAND_PP_MPLS_TUNNEL_MODEL_to_string(info->exp_model)));
  LOG_CLI((BSL_META_U(unit,
                      "ttl model %s \n\r"), soc_sand_SAND_PP_MPLS_TUNNEL_MODEL_to_string(info->ttl_model)));
  LOG_CLI((BSL_META_U(unit,
                      "label type: %u\n\r"),info->label_type));
  LOG_CLI((BSL_META_U(unit,
                      "label: %u\n\r"),info->label));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_ENCAP_PUSH_EXP_KEY_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PUSH_EXP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "push_profile: %u\n\r"),info->push_profile));
  LOG_CLI((BSL_META_U(unit,
                      "tc: %u\n\r"), info->tc));
  LOG_CLI((BSL_META_U(unit,
                      "dp: %u\n\r"), info->dp));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_ENCAP_PWE_GLBL_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_PWE_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "cw: %u\n\r"),info->cw));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_ENCAP_GLBL_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "l2_gre_prtcl_type: %u\n\r"),info->l2_gre_prtcl_type));
  LOG_CLI((BSL_META_U(unit,
                      "unkown_gre_prtcl_type: %u\n\r"),info->unkown_gre_prtcl_type));
  LOG_CLI((BSL_META_U(unit,
                      "unkown_ip_next_prtcl_type: %u\n\r"),info->unkown_ip_next_prtcl_type));
  LOG_CLI((BSL_META_U(unit,
                      "eth_ip_val: %u\n\r"),info->eth_ip_val));
  LOG_CLI((BSL_META_U(unit,
                      "unkown_ll_ether_type: %u\n\r"),info->unkown_ll_ether_type));
  LOG_CLI((BSL_META_U(unit,
                      "vxlan_udp_dest_port: %u\n\r"),info->vxlan_udp_dest_port));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_PPC_EG_ENCAP_ENTRY_VALUE_print(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_ENTRY_VALUE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "out_ac_info: %u\n\r"),info->vsi));
  SOC_PPC_EG_AC_INFO_print(&(info->out_ac));
  LOG_CLI((BSL_META_U(unit,
                      "swap_info:")));
  SOC_PPC_EG_ENCAP_SWAP_INFO_print(&(info->swap_info));
  LOG_CLI((BSL_META_U(unit,
                      "pwe_info:")));
  SOC_PPC_EG_ENCAP_PWE_INFO_print(&(info->pwe_info));
  LOG_CLI((BSL_META_U(unit,
                      "pop_info:")));
  SOC_PPC_EG_ENCAP_POP_INFO_print(&(info->pop_info));
  LOG_CLI((BSL_META_U(unit,
                      "mpls_encap_info:")));
  SOC_PPC_EG_ENCAP_MPLS_ENCAP_INFO_print(&(info->mpls_encap_info));
  LOG_CLI((BSL_META_U(unit,
                      "ipv4_encap_info:")));
  SOC_PPC_EG_ENCAP_IPV4_ENCAP_INFO_print(&(info->ipv4_encap_info));
  LOG_CLI((BSL_META_U(unit,
                      "ipv6_encap_info:")));
  SOC_PPC_EG_ENCAP_IPV6_ENCAP_INFO_print(&(info->ipv6_encap_info));
  LOG_CLI((BSL_META_U(unit,
                      "mirror_encap_info:")));
  SOC_PPC_EG_ENCAP_MIRROR_ENCAP_INFO_print(&(info->mirror_encap_info));
  LOG_CLI((BSL_META_U(unit,
                      "ll_info:")));
  SOC_PPC_EG_ENCAP_LL_INFO_print(unit, &(info->ll_info));
  LOG_CLI((BSL_META_U(unit,
                      "vsi_info:")));
  SOC_PPC_EG_ENCAP_VSI_ENCAP_INFO_print(&(info->vsi_info));
  LOG_CLI((BSL_META_U(unit,
                      "data_info:")));
  SOC_PPC_EG_ENCAP_DATA_INFO_print(&(info->data_info));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
SOC_PPC_EG_ENCAP_TRILL_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_TRILL_INFO *info)
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "my_nickname_index: %u\n\r"), info->my_nickname_index));
  LOG_CLI((BSL_META_U(unit,
                      "multicast: %u\n\r"),info->m));
  LOG_CLI((BSL_META_U(unit,
                      "egress_nick: 0x%04X\n\r"), info->nick));
  LOG_CLI((BSL_META_U(unit,
                      "outlif_profile: 0x%02x\n\r"), info->outlif_profile));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_PPC_EG_ENCAP_ENTRY_INFO_print(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_ENTRY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);
  switch(info->entry_type)
  {
    case SOC_PPC_EG_ENCAP_ENTRY_TYPE_VSI:
      LOG_CLI((BSL_META_U(unit,
                          "out-RIF VSI Encapsulation:\n\r")));
      SOC_PPC_EG_ENCAP_VSI_ENCAP_INFO_print(&(info->entry_val.vsi_info));
    break;
    case SOC_PPC_EG_ENCAP_ENTRY_TYPE_AC:
      LOG_CLI((BSL_META_U(unit,
                          "Out-AC:\n\r")));
      SOC_PPC_EG_AC_INFO_print(&(info->entry_val.out_ac));
    break;
    case SOC_PPC_EG_ENCAP_ENTRY_TYPE_SWAP_CMND:
      LOG_CLI((BSL_META_U(unit,
                          "SWAP Command:\n\r")));
      SOC_PPC_EG_ENCAP_SWAP_INFO_print(&(info->entry_val.swap_info));
    break;
    case SOC_PPC_EG_ENCAP_ENTRY_TYPE_PWE:
      LOG_CLI((BSL_META_U(unit,
                          "PWE:\n\r")));
      SOC_PPC_EG_ENCAP_PWE_INFO_print(&(info->entry_val.pwe_info));
    break;
    case SOC_PPC_EG_ENCAP_ENTRY_TYPE_POP_CMND:
      LOG_CLI((BSL_META_U(unit,
                          "POP Command:\n\r")));
      SOC_PPC_EG_ENCAP_POP_INFO_print(&(info->entry_val.pop_info));
    break;
    case SOC_PPC_EG_ENCAP_ENTRY_TYPE_MPLS_ENCAP:
      LOG_CLI((BSL_META_U(unit,
                          "MPLS Encapsulation:\n\r")));
      SOC_PPC_EG_ENCAP_MPLS_ENCAP_INFO_print(&(info->entry_val.mpls_encap_info));
    break;
    case SOC_PPC_EG_ENCAP_ENTRY_TYPE_IPV4_ENCAP:
      LOG_CLI((BSL_META_U(unit,
                          "IPv4 Encapsulation:\n\r")));
      SOC_PPC_EG_ENCAP_IPV4_ENCAP_INFO_print(&(info->entry_val.ipv4_encap_info));
    break;
    case SOC_PPC_EG_ENCAP_ENTRY_TYPE_IPV6_ENCAP:
      LOG_CLI((BSL_META_U(unit,
                          "IPV6 Encapsulation:\n\r")));
      SOC_PPC_EG_ENCAP_IPV6_ENCAP_INFO_print(&(info->entry_val.ipv6_encap_info));
    break;
    case SOC_PPC_EG_ENCAP_ENTRY_TYPE_MIRROR_ENCAP:
      LOG_CLI((BSL_META_U(unit,
                          "MIRROR Encapsulation:\n\r")));
      SOC_PPC_EG_ENCAP_MIRROR_ENCAP_INFO_print(&(info->entry_val.mirror_encap_info));
    break;
    case SOC_PPC_EG_ENCAP_ENTRY_TYPE_LL_ENCAP:
      LOG_CLI((BSL_META_U(unit,
                          "LL Encapsulation:\n\r")));
      SOC_PPC_EG_ENCAP_LL_INFO_print(unit, &(info->entry_val.ll_info));
    break;
    case SOC_PPC_EG_ENCAP_ENTRY_TYPE_DATA:
      LOG_CLI((BSL_META_U(unit,
                          "DATA Encapsulation:\n\r")));
      SOC_PPC_EG_ENCAP_DATA_INFO_print(&(info->entry_val.data_info));
    break;
    case SOC_PPC_EG_ENCAP_ENTRY_TYPE_ROO_LL_ENCAP:
      LOG_CLI((BSL_META_U(unit,
                          "ROO LL Encapsulation:\n\r")));
      SOC_PPC_EG_ENCAP_OVERLAY_ARP_ENCAP_INFO_print(unit, &(info->entry_val.overlay_arp_encap_info));
    break;
    case SOC_PPC_EG_ENCAP_ENTRY_TYPE_TRILL_ENCAP:
      LOG_CLI((BSL_META_U(unit,
                          "TRILL Encapsulation:\n\r")));
      SOC_PPC_EG_ENCAP_TRILL_INFO_print(&(info->entry_val.trill_encap_info));
    break;
    case SOC_PPC_EG_ENCAP_ENTRY_TYPE_NULL:
      LOG_CLI((BSL_META_U(unit,
                          "NULL Encapsulation:\n\r")));
    break;
    default:
      break;
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



void
  SOC_PPC_EG_MAP_ENCAP_INTPRI_COLOR_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_MAP_ENCAP_INTPRI_COLOR_INFO *info
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);
    sal_memset(info, 0x0, sizeof(*info));

    SOC_SAND_MAGIC_NUM_SET;

exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void 
SOC_PPC_EG_ENCAP_IP_TUNNEL_SRC_IP_clear(
   SOC_SAND_OUT SOC_PPC_EG_ENCAP_IP_TUNNEL_SRC_IP *info
   ) 
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_EG_ENCAP_IP_TUNNEL_SRC_IP));
  info->src_ip.address[0] = 0xFFFFFFFF;

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#include <soc/dpp/SAND/Utils/sand_footer.h>

