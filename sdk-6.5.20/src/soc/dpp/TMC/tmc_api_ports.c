/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/






#include <shared/bsl.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/TMC/tmc_api_ports.h>


























void
  SOC_TMC_PORT2IF_MAPPING_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PORT2IF_MAPPING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PORT2IF_MAPPING_INFO));
  info->if_id = SOC_TMC_IF_ID_NONE;
  info->channel_id = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PORTS_LAG_MEMBER_clear(
    SOC_SAND_OUT SOC_TMC_PORTS_LAG_MEMBER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PORTS_LAG_MEMBER));
  info->sys_port = 0;
  info->member_id = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PORTS_LAG_INFO_ARAD_clear(
    SOC_SAND_OUT SOC_TMC_PORTS_LAG_INFO_ARAD *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PORTS_LAG_INFO_ARAD));
  info->nof_entries = 0;
  
  for (ind=0; ind<SOC_TMC_PORTS_LAG_MEMBERS_MAX; ++ind)
  {
    SOC_TMC_PORTS_LAG_MEMBER_clear(&(info->lag_member_sys_ports[ind]));
    info->lag_member_sys_ports[ind].member_id = ind % SOC_TMC_PORTS_LAG_OUT_MEMBERS_MAX;
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PORTS_LAG_INFO_PETRAB_clear(
    SOC_SAND_OUT SOC_TMC_PORTS_LAG_INFO *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PORTS_LAG_INFO));
  info->nof_entries = 0;
  
  for (ind=0; ind<SOC_TMC_PORTS_LAG_MEMBERS_MAX; ++ind)
  {
    SOC_TMC_PORTS_LAG_MEMBER_clear(&(info->lag_member_sys_ports[ind]));
    info->lag_member_sys_ports[ind].member_id = ind % SOC_TMC_PORTS_LAG_OUT_MEMBERS_MAX;
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PORTS_OVERRIDE_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PORTS_OVERRIDE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PORTS_OVERRIDE_INFO));
  info->enable = 0;
  info->override_val = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PORT_INBOUND_MIRROR_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PORT_INBOUND_MIRROR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PORT_INBOUND_MIRROR_INFO));
  info->enable = 0;
  SOC_TMC_DEST_INFO_clear(&(info->destination));
  SOC_TMC_PORTS_OVERRIDE_INFO_clear(&(info->dp_override));
  SOC_TMC_PORTS_OVERRIDE_INFO_clear(&(info->tc_override));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PORT_OUTBOUND_MIRROR_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PORT_OUTBOUND_MIRROR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PORT_OUTBOUND_MIRROR_INFO));
  info->enable = 0;
  info->ifp_id = 0;
  info->skip_port_deafult_enable = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PORT_SNOOP_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PORT_SNOOP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PORT_SNOOP_INFO));
  info->enable = 0;
  info->size_bytes = SOC_TMC_PORTS_NOF_SNOOP_SIZES;
  SOC_TMC_DEST_INFO_clear(&(info->destination));
  SOC_TMC_PORTS_OVERRIDE_INFO_clear(&(info->dp_override));
  SOC_TMC_PORTS_OVERRIDE_INFO_clear(&(info->tc_override));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PORTS_ITMH_BASE_clear(
    SOC_SAND_OUT SOC_TMC_PORTS_ITMH_BASE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PORTS_ITMH_BASE));
  info->pp_header_present = 0;
  info->out_mirror_dis = 0;
  info->snoop_cmd_ndx = 0;
  info->exclude_src = 0;
  info->tr_cls = 0;
  info->dp = 0;
  SOC_TMC_DEST_INFO_clear(&(info->destination));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PORTS_ITMH_EXT_SRC_PORT_clear(
    SOC_SAND_OUT SOC_TMC_PORTS_ITMH_EXT_SRC_PORT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PORTS_ITMH_EXT_SRC_PORT));
  info->enable = 0;
  info->is_src_sys_port = 0;
  SOC_TMC_DEST_SYS_PORT_INFO_clear(&(info->src_port));
  SOC_TMC_DEST_INFO_clear(&(info->destination));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PORTS_ITMH_clear(
    SOC_SAND_OUT SOC_TMC_PORTS_ITMH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PORTS_ITMH));
  SOC_TMC_PORTS_ITMH_BASE_clear(&(info->base));
  SOC_TMC_PORTS_ITMH_EXT_SRC_PORT_clear(&(info->extension));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PORTS_ISP_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PORTS_ISP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PORTS_ISP_INFO));
  info->enable = FALSE;
  info->queue_id = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
SOC_TMC_PORT_LAG_SYS_PORT_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PORT_LAG_SYS_PORT_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PORT_LAG_SYS_PORT_INFO));
  info->in_member = 0;
  info->in_lag = 0;
  info->nof_of_out_lags = 0;
  for (ind=0; ind<SOC_TMC_MAX_NOF_LAG_MEMBER_IN_GROUP; ++ind)
  {
    info->out_lags[ind] = 0;
  }

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PORTS_STAG_FIELDS_clear(
    SOC_SAND_OUT SOC_TMC_PORTS_STAG_FIELDS   *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PORTS_STAG_FIELDS));
  info->cid = 0;
  info->ifp = 0;
  info->tr_cls = 0;
  info->dp = 0;
  info->data_type = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PORTS_OTMH_EXTENSIONS_EN_clear(
    SOC_SAND_OUT SOC_TMC_PORTS_OTMH_EXTENSIONS_EN *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PORTS_OTMH_EXTENSIONS_EN));
  info->outlif_ext_en = SOC_TMC_PORTS_FTMH_EXT_OUTLIF_IF_MC;
  info->src_ext_en = 0;
  info->dest_ext_en = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PORT_EGR_HDR_CR_DISCOUNT_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PORT_EGR_HDR_CR_DISCOUNT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PORT_EGR_HDR_CR_DISCOUNT_INFO));
  info->uc_credit_discount = 0;
  info->mc_credit_discount = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PORT_PP_PORT_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PORT_PP_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PORT_PP_PORT_INFO));
  info->is_stag_enabled = 0;
  info->first_header_size = 0;
  info->fc_type = SOC_TMC_PORTS_FC_TYPE_NONE;
  info->header_type = SOC_TMC_PORT_HEADER_TYPE_NONE;
  info->header_type_out = SOC_TMC_PORT_HEADER_TYPE_NONE;
  info->is_snoop_enabled = 0;
  info->is_tm_ing_shaping_enabled = 0;
  info->is_tm_pph_present_enabled = 0;
  info->is_tm_src_syst_port_ext_present = 0;
  info->mirror_profile = 0;
  info->flags = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PORT_COUNTER_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PORT_COUNTER_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PORT_COUNTER_INFO));
  info->processor_id = SOC_TMC_CNT_NOF_PROCESSOR_IDS;
  info->id = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PORTS_FORWARDING_HEADER_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PORTS_FORWARDING_HEADER_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PORTS_FORWARDING_HEADER_INFO));
  SOC_TMC_DEST_INFO_clear(&(info->destination));
  info->tr_cls = 0;
  info->dp = 0;
  info->snoop_cmd_ndx = 0;
  SOC_TMC_PORT_COUNTER_INFO_clear(&(info->counter));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PORT_INJECTED_CPU_HEADER_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PORT_INJECTED_CPU_HEADER_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PORT_INJECTED_CPU_HEADER_INFO));
  info->hdr_format = SOC_TMC_PORT_NOF_INJECTED_HDR_FORMATS;
  info->pp_port_for_tm_traffic = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PORTS_SWAP_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PORTS_SWAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PORTS_SWAP_INFO));

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PORTS_PON_TUNNEL_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PORTS_PON_TUNNEL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PORTS_PON_TUNNEL_INFO));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PORTS_PROGRAMS_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PORTS_PROGRAMS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PORTS_PROGRAMS_INFO));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_L2_ENCAP_INFO_clear(
    SOC_SAND_OUT SOC_TMC_L2_ENCAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_L2_ENCAP_INFO));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PORTS_APPLICATION_MAPPING_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PORTS_APPLICATION_MAPPING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PORTS_APPLICATION_MAPPING_INFO));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_TMC_DEBUG_IS_LVL1

const char*
  SOC_TMC_PORT_DIRECTION_to_string(
    SOC_SAND_IN  SOC_TMC_PORT_DIRECTION enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_PORT_DIRECTION_INCOMING:
    str = "incoming";
  break;
  case SOC_TMC_PORT_DIRECTION_OUTGOING:
    str = "outgoing";
  break;
  case SOC_TMC_PORT_DIRECTION_BOTH:
    str = "both";
  break;
  case SOC_TMC_PORT_NOF_DIRECTIONS:
    str = " Not initialized";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_PORT_HEADER_TYPE_to_string(
    SOC_SAND_IN  SOC_TMC_PORT_HEADER_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_PORT_HEADER_TYPE_NONE:
    str = "none";
  break;
  case SOC_TMC_PORT_HEADER_TYPE_ETH:
    str = "eth";
  break;
  case SOC_TMC_PORT_HEADER_TYPE_RAW:
    str = "raw";
  break;
  case SOC_TMC_PORT_HEADER_TYPE_TM:
    str = "tm";
  break;
  case SOC_TMC_PORT_HEADER_TYPE_PROG:
    str = "prog";
  break;
  case SOC_TMC_PORT_HEADER_TYPE_CPU:
    str = "cpu";
    break;
  case SOC_TMC_PORT_HEADER_TYPE_STACKING:
    str = "stacking";
    break;
  case SOC_TMC_PORT_HEADER_TYPE_TDM:
    str = "tdm";
    break;
  case SOC_TMC_PORT_HEADER_TYPE_TDM_RAW:
    str = "tdm_raw";
    break;
  case SOC_TMC_PORT_HEADER_TYPE_TDM_PMM:
    str = "tdm_pmm";
    break;
  case SOC_TMC_PORT_HEADER_TYPE_INJECTED:
    str = "injected";
    break;
  case SOC_TMC_PORT_HEADER_TYPE_INJECTED_2:
    str = "injected_2";
    break;
  case SOC_TMC_PORT_HEADER_TYPE_INJECTED_PP:
    str = "injected_pp";
    break;
  case SOC_TMC_PORT_HEADER_TYPE_INJECTED_2_PP:
    str = "injected_2_pp";
    break;
  case SOC_TMC_PORT_NOF_HEADER_TYPES:
    str = " Not initialized";
  break;
  case SOC_TMC_PORT_HEADER_TYPE_UDH_ETH:
    str = " udh_eth";
  break;
  case SOC_TMC_PORT_HEADER_TYPE_MPLS_RAW:
    str = "mpls_raw";
  break;
  case SOC_TMC_PORT_HEADER_TYPE_L2_ENCAP_EXTERNAL_CPU:
    str = "l2_encap_external_cpu";
  break;
  case SOC_TMC_PORT_HEADER_TYPE_MIRROR_RAW:
    str = "mirror_raw";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_PORTS_SNOOP_SIZE_to_string(
    SOC_SAND_IN  SOC_TMC_PORTS_SNOOP_SIZE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_PORTS_SNOOP_SIZE_BYTES_64:
    str = "bytes_64";
  break;
  case SOC_TMC_PORTS_SNOOP_SIZE_BYTES_192:
    str = "bytes_192";
  break;
  case SOC_TMC_PORTS_SNOOP_SIZE_ALL:
    str = "all";
  break;
  case SOC_TMC_PORTS_NOF_SNOOP_SIZES:
    str = " Not initialized";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_PORTS_FTMH_EXT_OUTLIF_to_string(
    SOC_SAND_IN  SOC_TMC_PORTS_FTMH_EXT_OUTLIF enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_PORTS_FTMH_EXT_OUTLIF_NEVER:
    str = "never";
  break;
  case SOC_TMC_PORTS_FTMH_EXT_OUTLIF_IF_MC:
    str = "if_mc";
  break;
  case SOC_TMC_PORTS_FTMH_EXT_OUTLIF_ALWAYS:
    str = "always";
  break;
  case SOC_TMC_PORTS_FTMH_NOF_EXT_OUTLIFS:
    str = " Not initialized";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_PORT_EGR_HDR_CR_DISCOUNT_TYPE_to_string(
    SOC_SAND_IN SOC_TMC_PORT_EGR_HDR_CR_DISCOUNT_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_PORT_EGR_HDR_CR_DISCOUNT_TYPE_A:
    str = "a";
  break;
  case SOC_TMC_PORT_EGR_HDR_CR_DISCOUNT_TYPE_B:
    str = "b";
  break;
  case SOC_TMC_PORT_NOF_EGR_HDR_CR_DISCOUNT_TYPES:
    str = " Not initialized";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_PORT_INJECTED_HDR_FORMAT_to_string(
    SOC_SAND_IN  SOC_TMC_PORT_INJECTED_HDR_FORMAT enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_PORT_INJECTED_HDR_FORMAT_NONE:
    str = "none";
  break;
  case SOC_TMC_PORT_INJECTED_HDR_FORMAT_PP_PORT:
    str = "pp_port";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_PORTS_VT_PROFILE_to_string(
    SOC_SAND_IN  SOC_TMC_PORTS_VT_PROFILE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_PORTS_VT_PROFILE_NONE:
    str = "none";
  break;  
  case SOC_TMC_PORTS_VT_PROFILE_OVERLAY_RCY:
    str = "overlay_rcy";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_PORTS_TT_PROFILE_to_string(
    SOC_SAND_IN  SOC_TMC_PORTS_TT_PROFILE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_PORTS_TT_PROFILE_NONE:
    str = "none";
  break;
  case SOC_TMC_PORTS_TT_PROFILE_OVERLAY_RCY:
    str = "overlay_rcy";
  break; 
  case SOC_TMC_PORTS_TT_PROFILE_PON:
    str = "pon";
  break;  
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_PORTS_FLP_PROFILE_to_string(
    SOC_SAND_IN  SOC_TMC_PORTS_FLP_PROFILE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_PORTS_FLP_PROFILE_NONE:
    str = "none";
  break;
  case SOC_TMC_PORTS_FLP_PROFILE_OVERLAY_RCY:
    str = "overlay_rcy";
  break;
  case SOC_TMC_PORTS_FLP_PROFILE_PON:
    str = "pon port";
  break;
  case SOC_TMC_PORTS_FLP_PROFILE_OAMP:
    str = "oamp port";
  break;
  default:
    str = " Unknown";
  }
  return str;
}


void
  SOC_TMC_PORT2IF_MAPPING_INFO_print(
    SOC_SAND_IN SOC_TMC_PORT2IF_MAPPING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "  If_id: %s \n\r"),
           SOC_TMC_INTERFACE_ID_to_string(info->if_id)
           ));
  LOG_CLI((BSL_META_U(unit,
                      "  Channel_id: %u\n\r"),info->channel_id));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PORTS_LAG_MEMBER_print(
    SOC_SAND_IN  SOC_TMC_PORTS_LAG_MEMBER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if (info->member_id == SOC_TMC_FAP_PORT_ID_INVALID)
  {
    LOG_CLI((BSL_META_U(unit,
                        "Sys_port: %4u, Member_id: %s\n\r"),info->sys_port, "Unknown at local FAP"));
  }
  else
  {
    LOG_CLI((BSL_META_U(unit,
                        "Sys_port: %4u, Member_id: %u\n\r"),info->sys_port, info->member_id));
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PORTS_LAG_INFO_print(
    SOC_SAND_IN SOC_TMC_PORTS_LAG_INFO *info
  )
{
  uint32 ind=0;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Nof_entries:              %u\n\r"),info->nof_entries));
  for (ind=0; ind<info->nof_entries; ++ind)
  {
    SOC_TMC_PORTS_LAG_MEMBER_print(&(info->lag_member_sys_ports[ind]));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PORTS_OVERRIDE_INFO_print(
    SOC_SAND_IN SOC_TMC_PORTS_OVERRIDE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      " Enable:       %d\n\r"),info->enable));
  LOG_CLI((BSL_META_U(unit,
                      " Override_val: %u\n\r"),info->override_val));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PORT_INBOUND_MIRROR_INFO_print(
    SOC_SAND_IN SOC_TMC_PORT_INBOUND_MIRROR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Enable: %d\n\r"),info->enable));
  LOG_CLI((BSL_META_U(unit,
                      "Destination: ")));
  SOC_TMC_DEST_INFO_print(&(info->destination));
  LOG_CLI((BSL_META_U(unit,
                      "Dp_override:\n\r")));
  SOC_TMC_PORTS_OVERRIDE_INFO_print(&(info->dp_override));
  LOG_CLI((BSL_META_U(unit,
                      "Tc_override:\n\r")));
  SOC_TMC_PORTS_OVERRIDE_INFO_print(&(info->tc_override));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PORT_OUTBOUND_MIRROR_INFO_print(
    SOC_SAND_IN SOC_TMC_PORT_OUTBOUND_MIRROR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Enable: %d\n\r"),info->enable));
  LOG_CLI((BSL_META_U(unit,
                      "Ifp_id: %u\n\r"),info->ifp_id));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PORT_SNOOP_INFO_print(
    SOC_SAND_IN SOC_TMC_PORT_SNOOP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Enable: %d\n\r"),info->enable));
  LOG_CLI((BSL_META_U(unit,
                      "Size_bytes: %s[Bytes] \n\r"),
           SOC_TMC_PORTS_SNOOP_SIZE_to_string(info->size_bytes)
           ));
  LOG_CLI((BSL_META_U(unit,
                      "Destination: ")));
  SOC_TMC_DEST_INFO_print(&(info->destination));
  LOG_CLI((BSL_META_U(unit,
                      "Dp_override:\n\r")));
  SOC_TMC_PORTS_OVERRIDE_INFO_print(&(info->dp_override));
  LOG_CLI((BSL_META_U(unit,
                      "Tc_override:\n\r")));
  SOC_TMC_PORTS_OVERRIDE_INFO_print(&(info->tc_override));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PORTS_ITMH_BASE_print(
    SOC_SAND_IN SOC_TMC_PORTS_ITMH_BASE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      " Pp_header_present: %d\n\r"),info->pp_header_present));
  LOG_CLI((BSL_META_U(unit,
                      " Out_mirror_dis:    %d\n\r"),info->out_mirror_dis));
  LOG_CLI((BSL_META_U(unit,
                      " Snoop_cmd_ndx:     %u\n\r"),info->snoop_cmd_ndx));
  LOG_CLI((BSL_META_U(unit,
                      " Exclude_src:       %d\n\r"),info->exclude_src));
  LOG_CLI((BSL_META_U(unit,
                      " Tr_cls:            %u\n\r"),info->tr_cls));
  LOG_CLI((BSL_META_U(unit,
                      " Dp:                %u\n\r"),info->dp));
  LOG_CLI((BSL_META_U(unit,
                      " Destination: ")));
  SOC_TMC_DEST_INFO_print(&(info->destination));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PORT_LAG_SYS_PORT_INFO_print(
    SOC_SAND_IN SOC_TMC_PORT_LAG_SYS_PORT_INFO *info
  )
{
  uint32
    ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if (info->in_member)
  {
    LOG_CLI((BSL_META_U(unit,
                        "In_lag:     %d\n\r"),info->in_lag));
  }
  else
  {
    LOG_CLI((BSL_META_U(unit,
                        "system port is not member in in-Lag\n\r")));
  }

  if (info->nof_of_out_lags == 0)
  {
    LOG_CLI((BSL_META_U(unit,
                        "system port is not member in Out-Lag\n\r")));
  }
  else
  {
    LOG_CLI((BSL_META_U(unit,
                        "Out_lags:")));
    for (ind=0; ind<SOC_TMC_MAX_NOF_LAG_MEMBER_IN_GROUP; ++ind)
    {
      if (info->out_lags[ind])
      {
        LOG_CLI((BSL_META_U(unit,
                            " %d"),ind));
        if (info->out_lags[ind] > 1)
        {
          LOG_CLI((BSL_META_U(unit,
                              "(%d)"),info->out_lags[ind]));
        }
      }
    }
    LOG_CLI((BSL_META_U(unit,
                        "\n\r")));
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PORTS_ITMH_EXT_SRC_PORT_print(
    SOC_SAND_IN SOC_TMC_PORTS_ITMH_EXT_SRC_PORT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      " Enable: %d\n\r"),info->enable));
  LOG_CLI((BSL_META_U(unit,
                      " Is_src_sys_port: %d\n\r"),info->is_src_sys_port));
  LOG_CLI((BSL_META_U(unit,
                      " Src_port: ")));
  SOC_TMC_DEST_SYS_PORT_INFO_print(&(info->src_port));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PORTS_ITMH_print(
    SOC_SAND_IN SOC_TMC_PORTS_ITMH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Base:\n\r")));
  SOC_TMC_PORTS_ITMH_BASE_print(&(info->base));
  LOG_CLI((BSL_META_U(unit,
                      "Extension:\n\r")));
  SOC_TMC_PORTS_ITMH_EXT_SRC_PORT_print(&(info->extension));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PORTS_ISP_INFO_print(
    SOC_SAND_IN SOC_TMC_PORTS_ISP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Enable:   %u\n\r"),info->enable));
  LOG_CLI((BSL_META_U(unit,
                      "Queue_id: %u\n\r"),info->queue_id));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PORTS_STAG_FIELDS_print(
    SOC_SAND_IN SOC_TMC_PORTS_STAG_FIELDS   *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Cid:       %d\n\r"),info->cid));
  LOG_CLI((BSL_META_U(unit,
                      "Ifp:       %d\n\r"),info->ifp));
  LOG_CLI((BSL_META_U(unit,
                      "Tr_cls:    %d\n\r"),info->tr_cls));
  LOG_CLI((BSL_META_U(unit,
                      "Dp:        %d\n\r"),info->dp));
  LOG_CLI((BSL_META_U(unit,
                      "Data_type: %d\n\r"),info->data_type));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PORTS_OTMH_EXTENSIONS_EN_print(
    SOC_SAND_IN SOC_TMC_PORTS_OTMH_EXTENSIONS_EN *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Outlif_ext_en: %s\n\r"),
           SOC_TMC_PORTS_FTMH_EXT_OUTLIF_to_string(info->outlif_ext_en)
           ));
  LOG_CLI((BSL_META_U(unit,
                      "Src_ext_en:    %d\n\r"),info->src_ext_en));
  LOG_CLI((BSL_META_U(unit,
                      "Dest_ext_en:   %d\n\r"),info->dest_ext_en));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PORT_EGR_HDR_CR_DISCOUNT_INFO_print(
    SOC_SAND_IN SOC_TMC_PORT_EGR_HDR_CR_DISCOUNT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Uc_credit_discount: %d[Bytes]\n\r"),info->uc_credit_discount));
  LOG_CLI((BSL_META_U(unit,
                      "Mc_credit_discount: %d[Bytes]\n\r"),info->mc_credit_discount));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

const char*
  SOC_TMC_PORTS_FC_TYPE_to_string(
    SOC_SAND_IN  SOC_TMC_PORTS_FC_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_PORTS_FC_TYPE_NONE:
    str = "none";
    break;
  case SOC_TMC_PORTS_FC_TYPE_LL:
    str = "ll";
  break;
  case SOC_TMC_PORTS_FC_TYPE_CB2:
    str = "cb2";
  break;
  case SOC_TMC_PORTS_FC_TYPE_CB8:
    str = "cb8";
  break;
  default:
    str = " Unknown Enumerator Value";
  }
  return str;
}

void
  SOC_TMC_PORT_PP_PORT_INFO_print(
    SOC_SAND_IN  SOC_TMC_PORT_PP_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "is_stag_enabled: %u, "),info->is_stag_enabled));
  LOG_CLI((BSL_META_U(unit,
                      "first_header_size: %u, "),info->first_header_size));
  LOG_CLI((BSL_META_U(unit,
                      "fc_type %s, "), SOC_TMC_PORTS_FC_TYPE_to_string(info->fc_type)));
  LOG_CLI((BSL_META_U(unit,
                      "header_type %s \n\r"), SOC_TMC_PORT_HEADER_TYPE_to_string(info->header_type)));
  LOG_CLI((BSL_META_U(unit,
                      "header_type_out %s \n\r"), SOC_TMC_PORT_HEADER_TYPE_to_string(info->header_type_out)));
  LOG_CLI((BSL_META_U(unit,
                      "    is_snoop_enabled: %u, "),info->is_snoop_enabled));
  LOG_CLI((BSL_META_U(unit,
                      "is_tm_ing_shaping_enabled: %u, "),info->is_tm_ing_shaping_enabled));
  LOG_CLI((BSL_META_U(unit,
                      "is_tm_pph_present_enabled: %u, "),info->is_tm_pph_present_enabled));
  LOG_CLI((BSL_META_U(unit,
                      "is_tm_src_syst_port_ext_present: %u, "),info->is_tm_src_syst_port_ext_present));
  LOG_CLI((BSL_META_U(unit,
                      "mirror_profile: %u\n\r"),info->mirror_profile));
  LOG_CLI((BSL_META_U(unit,
                      "flags:\n\r")));
  if (info->flags & SOC_TMC_PORT_PP_PORT_RCY_OVERLAY_PTCH) {
      LOG_CLI((BSL_META_U(unit,
                          "    SOC_TMC_PORT_PP_PORT_RCY_OVERLAY_PTCH\n\r")));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PORT_COUNTER_INFO_print(
    SOC_SAND_IN  SOC_TMC_PORT_COUNTER_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "processor_id %s "), SOC_TMC_CNT_PROCESSOR_ID_to_string(info->processor_id)));
  LOG_CLI((BSL_META_U(unit,
                      "id: %u\n\r"),info->id));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PORTS_FORWARDING_HEADER_INFO_print(
    SOC_SAND_IN  SOC_TMC_PORTS_FORWARDING_HEADER_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "destination:")));
  SOC_TMC_DEST_INFO_print(&(info->destination));
  LOG_CLI((BSL_META_U(unit,
                      "tr_cls: %u\n\r"),info->tr_cls));
  LOG_CLI((BSL_META_U(unit,
                      "dp: %u\n\r"),info->dp));
  LOG_CLI((BSL_META_U(unit,
                      "snoop_cmd_ndx: %u\n\r"),info->snoop_cmd_ndx));
  LOG_CLI((BSL_META_U(unit,
                      "counter:")));
  SOC_TMC_PORT_COUNTER_INFO_print(&(info->counter));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PORT_INJECTED_CPU_HEADER_INFO_print(
    SOC_SAND_IN  SOC_TMC_PORT_INJECTED_CPU_HEADER_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "hdr_format %s "), SOC_TMC_PORT_INJECTED_HDR_FORMAT_to_string(info->hdr_format)));
  LOG_CLI((BSL_META_U(unit,
                      "pp_port_for_tm_traffic: %u\n\r"),info->pp_port_for_tm_traffic));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PORTS_SWAP_INFO_print(
    SOC_SAND_IN  SOC_TMC_PORTS_SWAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "enable: %d\n\r"),info->enable));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PORTS_PON_TUNNEL_INFO_print(
    SOC_SAND_IN  SOC_TMC_PORTS_PON_TUNNEL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "pp_port: %u\n\r"),info->pp_port));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PORTS_PROGRAMS_INFO_print(
    SOC_SAND_IN  SOC_TMC_PORTS_PROGRAMS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "ptc_vt_profile %s "), SOC_TMC_PORTS_VT_PROFILE_to_string(info->ptc_vt_profile)));
  LOG_CLI((BSL_META_U(unit,
                      "ptc_tt_profile %s "), SOC_TMC_PORTS_TT_PROFILE_to_string(info->ptc_tt_profile)));
  LOG_CLI((BSL_META_U(unit,
                      "ptc_flp_profile %s "), SOC_TMC_PORTS_FLP_PROFILE_to_string(info->ptc_flp_profile)));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

