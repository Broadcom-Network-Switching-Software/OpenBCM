/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/






#include <shared/bsl.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/TMC/tmc_api_tcam_key.h>





















void
  SOC_TMC_TCAM_RULE_INFO_clear(
    SOC_SAND_OUT SOC_TMC_TCAM_RULE_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_TCAM_RULE_INFO));
  for (ind = 0; ind < SOC_TMC_TCAM_RULE_NOF_UINT32S_MAX; ++ind)
  {
    info->val[ind] = 0;
  }
  for (ind = 0; ind < SOC_TMC_TCAM_RULE_NOF_UINT32S_MAX; ++ind)
  {
    info->mask[ind] = 0;
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_TCAM_KEY_FORMAT_clear(
    SOC_SAND_OUT SOC_TMC_TCAM_KEY_FORMAT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_TCAM_KEY_FORMAT));
  info->type = SOC_TMC_TCAM_NOF_KEY_FORMAT_TYPES;
  info->pmf = SOC_TMC_NOF_PMF_TCAM_KEY_SRCS;
  info->egr_acl = SOC_TMC_EGR_NOF_ACL_DB_TYPES;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_TCAM_KEY_clear(
    SOC_SAND_OUT SOC_TMC_TCAM_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_TCAM_KEY));
  info->size = SOC_TMC_NOF_TCAM_BANK_ENTRY_SIZES;
  SOC_TMC_TCAM_RULE_INFO_clear(&(info->data));
  SOC_TMC_TCAM_KEY_FORMAT_clear(&(info->format));
  info->pmf_pgm_id = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_TCAM_OUTPUT_clear(
    SOC_SAND_OUT SOC_TMC_TCAM_OUTPUT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_TCAM_OUTPUT));
  info->val = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PMF_TCAM_FLD_L3_clear(
    SOC_SAND_OUT SOC_TMC_PMF_TCAM_FLD_L3 *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PMF_TCAM_FLD_L3));
  info->mode = SOC_TMC_NOF_PMF_TCAM_FLD_L3_MODES;
  info->ipv4_fld = SOC_TMC_NOF_PMF_FLD_IPV4S;
  info->ipv6_fld = SOC_TMC_NOF_PMF_FLD_IPV6S;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PMF_TCAM_FLD_A_B_clear(
    SOC_SAND_OUT SOC_TMC_PMF_TCAM_FLD_A_B *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PMF_TCAM_FLD_A_B));
  info->loc = SOC_TMC_NOF_PMF_TCAM_FLD_A_B_LOCS;
  info->irpp_fld = SOC_TMC_NOF_PMF_IRPP_INFO_FIELDS;
  info->sub_header = SOC_TMC_NOF_PMF_IRPP_INFO_FIELDS;
  info->sub_header_offset = 0;
  info->pmf_pgm_id = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void
  SOC_TMC_TCAM_KEY_FLD_TYPE_clear(
    SOC_SAND_IN  SOC_TMC_TCAM_KEY_FORMAT    *key_format,
    SOC_SAND_OUT SOC_TMC_TCAM_KEY_FLD_TYPE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_TCAM_KEY_FLD_TYPE));

  if (key_format->type == SOC_TMC_TCAM_KEY_FORMAT_TYPE_PMF)
  {
    if (key_format->pmf == SOC_TMC_PMF_TCAM_KEY_SRC_L2)
    {
      info->l2 = SOC_TMC_NOF_PMF_TCAM_FLD_L2S;
    }
    else if ((key_format->pmf == SOC_TMC_PMF_TCAM_KEY_SRC_L3_IPV4) || (key_format->pmf == SOC_TMC_PMF_TCAM_KEY_SRC_L3_IPV6))
    {
      SOC_TMC_PMF_TCAM_FLD_L3_clear(&(info->l3));
      if (key_format->pmf == SOC_TMC_PMF_TCAM_KEY_SRC_L3_IPV4)
      {
        info->l3.mode = SOC_TMC_PMF_TCAM_FLD_L3_MODE_IPV4;
      }
      else 
      {
        info->l3.mode = SOC_TMC_PMF_TCAM_FLD_L3_MODE_IPV6;
      }
    }
    else
    {
      SOC_TMC_PMF_TCAM_FLD_A_B_clear(&(info->a_b));
    }
  }
  else if(key_format->type == SOC_TMC_TCAM_KEY_FORMAT_TYPE_EGR_ACL)
  {
    switch (key_format->egr_acl)
    {
    case SOC_TMC_EGR_ACL_DB_TYPE_ETH:
      info->egr_l2 = SOC_TMC_EGR_NOF_ACL_TCAM_FLD_L2S;
    	break;
    case SOC_TMC_EGR_ACL_DB_TYPE_IPV4:
      info->egr_ipv4 = SOC_TMC_EGR_NOF_ACL_TCAM_FLD_IPV4S;
      break;
    case SOC_TMC_EGR_ACL_DB_TYPE_MPLS:
      info->egr_mpls = SOC_TMC_EGR_NOF_ACL_TCAM_FLD_MPLSS;
      break;
    case SOC_TMC_EGR_ACL_DB_TYPE_TM:
      info->egr_tm = SOC_TMC_EGR_NOF_ACL_TCAM_FLD_TMS;
      break;
    default:
      break;
    }
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_TCAM_KEY_FLD_VAL_clear(
    SOC_SAND_OUT SOC_TMC_TCAM_KEY_FLD_VAL *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_TCAM_KEY_FLD_VAL));
  for (ind = 0; ind < SOC_TMC_TCAM_KEY_FLD_NOF_UINT32S_MAX; ++ind)
  {
    info->val[ind] = 0;
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_TMC_DEBUG_IS_LVL1

const char*
  SOC_TMC_EGR_ACL_DB_TYPE_to_string(
    SOC_SAND_IN  SOC_TMC_EGR_ACL_DB_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_EGR_ACL_DB_TYPE_ETH:
    str = "eth";
  break;
  case SOC_TMC_EGR_ACL_DB_TYPE_IPV4:
    str = "ipv4";
  break;
  case SOC_TMC_EGR_ACL_DB_TYPE_MPLS:
    str = "mpls";
  break;
  case SOC_TMC_EGR_ACL_DB_TYPE_TM:
    str = "tm";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_PMF_TCAM_KEY_SRC_to_string(
    SOC_SAND_IN  SOC_TMC_PMF_TCAM_KEY_SRC enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_PMF_TCAM_KEY_SRC_L2:
    str = "l2";
  break;
  case SOC_TMC_PMF_TCAM_KEY_SRC_L3_IPV4:
    str = "l3_ipv4";
    break;
  case SOC_TMC_PMF_TCAM_KEY_SRC_L3_IPV6:
    str = "l3_ipv6";
    break;
  case SOC_TMC_PMF_TCAM_KEY_SRC_B_A:
    str = "b_a";
  break;
  case SOC_TMC_PMF_TCAM_KEY_SRC_A_71_0:
    str = "a_71_0";
  break;
  case SOC_TMC_PMF_TCAM_KEY_SRC_A_103_32:
    str = "a_103_32";
  break;
  case SOC_TMC_PMF_TCAM_KEY_SRC_A_143_0:
    str = "a_143_0";
  break;
  case SOC_TMC_PMF_TCAM_KEY_SRC_A_175_32:
    str = "a_175_32";
  break;
  case SOC_TMC_PMF_TCAM_KEY_SRC_B_71_0:
    str = "b_71_0";
  break;
  case SOC_TMC_PMF_TCAM_KEY_SRC_B_103_32:
    str = "b_103_32";
  break;
  case SOC_TMC_PMF_TCAM_KEY_SRC_B_143_0:
    str = "b_143_0";
  break;
  case SOC_TMC_PMF_TCAM_KEY_SRC_B_175_32:
    str = "b_175_32";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_TCAM_KEY_FORMAT_TYPE_to_string(
    SOC_SAND_IN  SOC_TMC_TCAM_KEY_FORMAT_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_TCAM_KEY_FORMAT_TYPE_PMF:
    str = "pmf";
  break;
  case SOC_TMC_TCAM_KEY_FORMAT_TYPE_EGR_ACL:
    str = "egr_acl";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_PMF_TCAM_FLD_L2_to_string(
    SOC_SAND_IN  SOC_TMC_PMF_TCAM_FLD_L2 enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_PMF_TCAM_FLD_L2_LLVP:
    str = "llvp";
  break;
  case SOC_TMC_PMF_TCAM_FLD_L2_STAG:
    str = "stag";
  break;
  case SOC_TMC_PMF_TCAM_FLD_L2_CTAG_IN_AC:
    str = "ctag_in_ac";
  break;
  case SOC_TMC_PMF_TCAM_FLD_L2_SA:
    str = "sa";
  break;
  case SOC_TMC_PMF_TCAM_FLD_L2_DA:
    str = "da";
  break;
  case SOC_TMC_PMF_TCAM_FLD_L2_ETHERTYPE:
    str = "ethertype";
  break;
  case SOC_TMC_PMF_TCAM_FLD_L2_IN_PP_PORT:
    str = "in_pp_port";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_PMF_TCAM_FLD_L3_MODE_to_string(
    SOC_SAND_IN  SOC_TMC_PMF_TCAM_FLD_L3_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_PMF_TCAM_FLD_L3_MODE_IPV4:
    str = "ipv4";
  break;
  case SOC_TMC_PMF_TCAM_FLD_L3_MODE_IPV6:
    str = "ipv6";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_PMF_FLD_IPV4_to_string(
    SOC_SAND_IN  SOC_TMC_PMF_FLD_IPV4 enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_PMF_FLD_IPV4_L4OPS:
    str = "l4ops";
  break;
  case SOC_TMC_PMF_FLD_IPV4_NEXT_PRTCL:
    str = "next_prtcl";
  break;
  case SOC_TMC_PMF_FLD_IPV4_DF:
    str = "df";
  break;
  case SOC_TMC_PMF_FLD_IPV4_MF:
    str = "mf";
  break;
  case SOC_TMC_PMF_FLD_IPV4_FRAG_NON_0:
    str = "frag_non_0";
  break;
  case SOC_TMC_PMF_FLD_IPV4_L4OPS_FLAG:
    str = "l4ops_flag";
  break;
  case SOC_TMC_PMF_FLD_IPV4_SIP:
    str = "sip";
  break;
  case SOC_TMC_PMF_FLD_IPV4_DIP:
    str = "dip";
  break;
  case SOC_TMC_PMF_FLD_IPV4_L4OPS_OPT:
    str = "l4ops_opt";
  break;
  case SOC_TMC_PMF_FLD_IPV4_SRC_PORT:
    str = "src_port";
  break;
  case SOC_TMC_PMF_FLD_IPV4_DEST_PORT:
    str = "dest_port";
  break;
  case SOC_TMC_PMF_FLD_IPV4_TOS:
    str = "tos";
  break;
  case SOC_TMC_PMF_FLD_IPV4_TCP_CTL:
    str = "tcp_ctl";
  break;
  case SOC_TMC_PMF_FLD_IPV4_IN_AC_VRF:
    str = "in_ac_vrf";
  break;
  case SOC_TMC_PMF_FLD_IPV4_IN_PP_PORT:
    str = "in_pp_port";
  break;
  case SOC_TMC_PMF_FLD_IPV4_IN_VID:
    str = "in_vid";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_PMF_FLD_IPV6_to_string(
    SOC_SAND_IN  SOC_TMC_PMF_FLD_IPV6 enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_PMF_FLD_IPV6_L4OPS:
    str = "l4ops";
  break;
  case SOC_TMC_PMF_FLD_IPV6_SIP_HIGH:
    str = "sip_high";
    break;
  case SOC_TMC_PMF_FLD_IPV6_SIP_LOW:
    str = "sip_low";
    break;
  case SOC_TMC_PMF_FLD_IPV6_DIP_LOW:
    str = "dip_low";
    break;
  case SOC_TMC_PMF_FLD_IPV6_DIP_HIGH:
    str = "dip_high";
    break;
  case SOC_TMC_PMF_FLD_IPV6_NEXT_PRTCL:
    str = "next_prtcl";
  break;
  case SOC_TMC_PMF_FLD_IPV6_PP_PORT_TCP_CTL:
    str = "pp_port_tcp_ctl";
  break;
  case SOC_TMC_PMF_FLD_IPV6_IN_AC_VRF:
    str = "in_ac_vrf";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_PMF_TCAM_FLD_A_B_LOC_to_string(
    SOC_SAND_IN  SOC_TMC_PMF_TCAM_FLD_A_B_LOC enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_PMF_TCAM_FLD_A_B_LOC_IRPP:
    str = "irpp";
  break;
  case SOC_TMC_PMF_TCAM_FLD_A_B_LOC_HDR:
    str = "hdr";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_EGR_ACL_TCAM_FLD_L2_to_string(
    SOC_SAND_IN  SOC_TMC_EGR_ACL_TCAM_FLD_L2 enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_EGR_ACL_TCAM_FLD_L2_ETH_TYPE_CODE:
    str = "eth_type_code";
  break;
  case SOC_TMC_EGR_ACL_TCAM_FLD_L2_ETH_TAG_FORMAT:
    str = "eth_tag_format";
  break;
  case SOC_TMC_EGR_ACL_TCAM_FLD_L2_OUTER_TAG:
    str = "outer_tag";
  break;
  case SOC_TMC_EGR_ACL_TCAM_FLD_L2_INNER_TAG:
    str = "inner_tag";
  break;
  case SOC_TMC_EGR_ACL_TCAM_FLD_L2_SA:
    str = "sa";
  break;
  case SOC_TMC_EGR_ACL_TCAM_FLD_L2_DA:
    str = "da";
  break;
  case SOC_TMC_EGR_ACL_TCAM_FLD_L2_OUT_PP_PORT_ACL_DATA:
    str = "out_pp_port_acl_data";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_EGR_ACL_TCAM_FLD_IPV4_to_string(
    SOC_SAND_IN  SOC_TMC_EGR_ACL_TCAM_FLD_IPV4 enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_EGR_ACL_TCAM_FLD_IPV4_TOS:
    str = "tos";
  break;
  case SOC_TMC_EGR_ACL_TCAM_FLD_IPV4_L4_PRO_CODE:
    str = "l4_pro_code";
  break;
  case SOC_TMC_EGR_ACL_TCAM_FLD_IPV4_SIP:
    str = "sip";
  break;
  case SOC_TMC_EGR_ACL_TCAM_FLD_IPV4_DIP:
    str = "dip";
  break;
  case SOC_TMC_EGR_ACL_TCAM_FLD_IPV4_OUTER_VID:
    str = "outer_vid";
  break;
  case SOC_TMC_EGR_ACL_TCAM_FLD_IPV4_OUT_PP_PORT_ACL_DATA:
    str = "out_pp_port_acl_data";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_EGR_ACL_TCAM_FLD_MPLS_to_string(
    SOC_SAND_IN  SOC_TMC_EGR_ACL_TCAM_FLD_MPLS enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_EGR_ACL_TCAM_FLD_MPLS_FTMH:
    str = "ftmh";
  break;
  case SOC_TMC_EGR_ACL_TCAM_FLD_MPLS_HDR_DATA:
    str = "hdr_data";
  break;
  case SOC_TMC_EGR_ACL_TCAM_FLD_MPLS_LABEL:
    str = "label";
    break;
  case SOC_TMC_EGR_ACL_TCAM_FLD_MPLS_EXP:
    str = "exp";
    break;
  case SOC_TMC_EGR_ACL_TCAM_FLD_MPLS_TTL:
    str = "ttl";
    break;
  case SOC_TMC_EGR_ACL_TCAM_FLD_MPLS_OUT_PP_PORT_ACL_DATA:
    str = "out_pp_port_acl_data";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_EGR_ACL_TCAM_FLD_TM_to_string(
    SOC_SAND_IN  SOC_TMC_EGR_ACL_TCAM_FLD_TM enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_EGR_ACL_TCAM_FLD_TM_FTMH:
    str = "ftmh_base";
  break;
  case SOC_TMC_EGR_ACL_TCAM_FLD_TM_HDR_DATA:
    str = "hdr_data";
  break;
  case SOC_TMC_EGR_ACL_TCAM_FLD_TM_OUT_PP_PORT_ACL_DATA:
    str = "out_pp_port_acl_data";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_TMC_TCAM_RULE_INFO_print(
    SOC_SAND_IN  SOC_TMC_TCAM_RULE_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < SOC_TMC_TCAM_RULE_NOF_UINT32S_MAX; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "val[%u]: %u\n\r"),ind,info->val[ind]));
  }
  for (ind = 0; ind < SOC_TMC_TCAM_RULE_NOF_UINT32S_MAX; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "mask[%u]: %u\n\r"),ind,info->mask[ind]));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_TCAM_KEY_FORMAT_print(
    SOC_SAND_IN  SOC_TMC_TCAM_KEY_FORMAT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "type %s "), SOC_TMC_TCAM_KEY_FORMAT_TYPE_to_string(info->type)));
  LOG_CLI((BSL_META_U(unit,
                      "pmf %s "), SOC_TMC_PMF_TCAM_KEY_SRC_to_string(info->pmf)));
  LOG_CLI((BSL_META_U(unit,
                      "egr_acl %s "), SOC_TMC_EGR_ACL_DB_TYPE_to_string(info->egr_acl)));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_TCAM_KEY_print(
    SOC_SAND_IN  SOC_TMC_TCAM_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "size %s "), SOC_TMC_TCAM_BANK_ENTRY_SIZE_to_string(info->size)));
  LOG_CLI((BSL_META_U(unit,
                      "data:")));
  SOC_TMC_TCAM_RULE_INFO_print(&(info->data));
  LOG_CLI((BSL_META_U(unit,
                      "format:")));
  SOC_TMC_TCAM_KEY_FORMAT_print(&(info->format));
  LOG_CLI((BSL_META_U(unit,
                      "pmf_pgm_id: %u\n\r"),info->pmf_pgm_id));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_TCAM_OUTPUT_print(
    SOC_SAND_IN  SOC_TMC_TCAM_OUTPUT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "val: %u\n\r"),info->val));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PMF_TCAM_FLD_L3_print(
    SOC_SAND_IN  SOC_TMC_PMF_TCAM_FLD_L3 *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "mode %s "), SOC_TMC_PMF_TCAM_FLD_L3_MODE_to_string(info->mode)));
  LOG_CLI((BSL_META_U(unit,
                      "ipv4_fld %s "), SOC_TMC_PMF_FLD_IPV4_to_string(info->ipv4_fld)));
  LOG_CLI((BSL_META_U(unit,
                      "ipv6_fld %s "), SOC_TMC_PMF_FLD_IPV6_to_string(info->ipv6_fld)));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PMF_TCAM_FLD_A_B_print(
    SOC_SAND_IN  SOC_TMC_PMF_TCAM_FLD_A_B *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "loc %s "), SOC_TMC_PMF_TCAM_FLD_A_B_LOC_to_string(info->loc)));
  LOG_CLI((BSL_META_U(unit,
                      "irpp_fld %s "), SOC_TMC_PMF_IRPP_INFO_FIELD_to_string(info->irpp_fld)));
  LOG_CLI((BSL_META_U(unit,
                      "sub_header %s "), SOC_TMC_PMF_IRPP_INFO_FIELD_to_string(info->sub_header)));
  LOG_CLI((BSL_META_U(unit,
                      "sub_header_offset: %u\n\r"),info->sub_header_offset));
  LOG_CLI((BSL_META_U(unit,
                      "pmf_pgm_id: %u\n\r"),info->pmf_pgm_id));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_TCAM_KEY_FLD_TYPE_print(
    SOC_SAND_IN  SOC_TMC_TCAM_KEY_FLD_TYPE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "l2 %s "), SOC_TMC_PMF_TCAM_FLD_L2_to_string(info->l2)));
  LOG_CLI((BSL_META_U(unit,
                      "l3:")));
  SOC_TMC_PMF_TCAM_FLD_L3_print(&(info->l3));
  LOG_CLI((BSL_META_U(unit,
                      "a_b:")));
  SOC_TMC_PMF_TCAM_FLD_A_B_print(&(info->a_b));
  LOG_CLI((BSL_META_U(unit,
                      "egr_l2 %s "), SOC_TMC_EGR_ACL_TCAM_FLD_L2_to_string(info->egr_l2)));
  LOG_CLI((BSL_META_U(unit,
                      "egr_ipv4 %s "), SOC_TMC_EGR_ACL_TCAM_FLD_IPV4_to_string(info->egr_ipv4)));
  LOG_CLI((BSL_META_U(unit,
                      "egr_mpls %s "), SOC_TMC_EGR_ACL_TCAM_FLD_MPLS_to_string(info->egr_mpls)));
  LOG_CLI((BSL_META_U(unit,
                      "egr_tm %s "), SOC_TMC_EGR_ACL_TCAM_FLD_TM_to_string(info->egr_tm)));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_TCAM_KEY_FLD_VAL_print(
    SOC_SAND_IN  SOC_TMC_TCAM_KEY_FLD_VAL *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < SOC_TMC_TCAM_KEY_FLD_NOF_UINT32S_MAX; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "val[%u]: %u\n\r"),ind,info->val[ind]));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

