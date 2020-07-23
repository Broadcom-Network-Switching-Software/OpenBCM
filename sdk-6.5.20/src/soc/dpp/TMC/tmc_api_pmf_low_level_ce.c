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

#include <soc/dpp/TMC/tmc_api_pmf_low_level_ce.h>





















void
  SOC_TMC_PMF_CE_PACKET_HEADER_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PMF_CE_PACKET_HEADER_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PMF_CE_PACKET_HEADER_INFO));
  info->sub_header = SOC_TMC_NOF_PMF_CE_SUB_HEADERS;
  info->offset = 0;
  info->nof_bits = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


#if SOC_TMC_DEBUG_IS_LVL1

const char*
  SOC_TMC_PMF_CE_SUB_HEADER_to_string(
    SOC_SAND_IN  SOC_TMC_PMF_CE_SUB_HEADER enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_PMF_CE_SUB_HEADER_0:
    str = "header_0";
  break;
  case SOC_TMC_PMF_CE_SUB_HEADER_1:
    str = "header_1";
  break;
  case SOC_TMC_PMF_CE_SUB_HEADER_2:
    str = "header_2";
  break;
  case SOC_TMC_PMF_CE_SUB_HEADER_3:
    str = "header_3";
  break;
  case SOC_TMC_PMF_CE_SUB_HEADER_FWD:
    str = "fwd";
  break;
  case SOC_TMC_PMF_CE_SUB_HEADER_FWD_POST:
    str = "fwd_post";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_PMF_IRPP_INFO_FIELD_to_string(
    SOC_SAND_IN  SOC_TMC_PMF_IRPP_INFO_FIELD enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_PMF_IRPP_INFO_FIELD_TM_PORT:
    str = "tm_port";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_TM_PORT32:
    str = "tm_port32";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_PCKT_HEADER_SIZE:
    str = "pckt_header_size";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_SYST_PORT_SRC:
    str = "syst_port_src";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_IN_PP_PORT:
    str = "in_pp_port";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_PP_CONTEXT:
    str = "pp_context";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_0:
    str = "header_offset_0";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_1:
    str = "header_offset_1";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_2:
    str = "header_offset_2";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_3:
    str = "header_offset_3";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_4:
    str = "header_offset_4";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_HEADER_OFFSET_5:
    str = "header_offset_5";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_0:
    str = "pckt_format_qual_0";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_1:
    str = "pckt_format_qual_1";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_2:
    str = "pckt_format_qual_2";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_3:
    str = "pckt_format_qual_3";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_4:
    str = "pckt_format_qual_4";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL_5:
    str = "pckt_format_qual_5";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_PCKT_FORMAT_QUAL1_EXT:
    str = "pckt_format_qual1_ext";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_PARSER_PMF_PROFILE:
    str = "parser_pmf_profile";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_PACKET_FORMAT_CODE:
    str = "packet_format_code";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_PACKET_FORMAT_CODE_2MSB:
    str = "packet_format_code_2msb";
    break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_LLVP_INCOMING_TAG_STRUCT:
    str = "llvp_incoming_tag_struct";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_FWD_ACTION_STRENGTH:
    str = "fwd_action_strength";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_FWD_ACTION_DEST_VALID:
    str = "fwd_action_dest_valid";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_FWD_ACTION_TC_VALID:
    str = "fwd_action_tc_valid";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_FWD_ACTION_DP_VALID:
    str = "fwd_action_dp_valid";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_FWD_ACTION_CPU_TRAP_CODE:
    str = "fwd_action_cpu_trap_code";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_FWD_ACTION_CPU_TRAP_QUAL:
    str = "fwd_action_cpu_trap_qual";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_CPU_TRAP_CODE_PRO:
    str = "cpu_trap_code_pro";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_UP:
    str = "up";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_SNOOP_STRENGTH:
    str = "snoop_strength";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_SNOOP_CODE:
    str = "snoop_code";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_LEARN_DEST:
    str = "learn_dest";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_LEARN_ASD:
    str = "learn_asd";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_SEM_NDX:
    str = "sem_ndx";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_LL_MIRROR_PROFILE:
    str = "ll_mirror_profile";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_VSI:
    str = "vsi";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_SYSTEM_VSI:
    str = "system_vsi";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_ORIENTATION_IS_HUB:
    str = "orientation_is_hub";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_VLAN_EDIT_TAG:
    str = "vlan_edit_tag";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_STP_STATE:
    str = "stp_state";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_FWDING_CODE:
    str = "fwding_code";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_FWDING_OFFSET_NDX:
    str = "fwding_offset_ndx";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_FWDING_OFFSET_EXT:
    str = "fwding_offset_ext";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_TUNNEL_TERMINATION_CODE:
    str = "tunnel_termination_code";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_IN_RIF:
    str = "in_rif";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_VRF:
    str = "vrf";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_PCKT_IS_COMP_MC:
    str = "pckt_is_comp_mc";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_MY_BMAC:
    str = "my_bmac";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_EEI_OR_OUTLIF:
    str = "eei_or_outlif";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_IN_TTL:
    str = "in_ttl";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_IN_DSCP_EXP:
    str = "in_dscp_exp";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_RPF_FEC_PTR_VALID:
    str = "rpf_fec_ptr_valid";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_SEM_1ST_LOOKUP_RESULT:
    str = "sem_1st_lookup_result";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_SEM_2ND_LOOKUP_RESULT:
    str = "sem_2nd_lookup_result";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_LEM_1ST_LOOKUP_RESULT_DEST:
    str = "lem_1st_lookup_result_dest";
    break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_LEM_2ND_LOOKUP_RESULT_DEST:
    str = "lem_2nd_lookup_result_dest";
    break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_LEM_1ST_LOOKUP_RESULT_ASD:
    str = "lem_1st_lookup_result_asd";
    break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_LEM_2ND_LOOKUP_RESULT_ASD:
    str = "lem_2nd_lookup_result_asd";
    break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_LPM_1ST_LOOKUP_RESULT:
    str = "lpm_1st_lookup_result";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_LPM_2ND_LOOKUP_RESULT:
    str = "lpm_2nd_lookup_result";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_TCAM_LOOKUP_RESULT:
    str = "tcam_lookup_result";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_ELK_LOOKUP_RESULT:
    str = "elk_lookup_result";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_KEY_GEN_VAR:
    str = "key_gen_var";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_COPY_PGM_VARIABLE:
    str = "copy_pgm_variable";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_PACKET_SIZE_RANGE:
    str = "packet_size_range";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_L4_OPS:
    str = "l4_ops";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_KEY_PGM_PROFILE:
    str = "key_pgm_profile";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_CUSTOM1:
    str = "custom1";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_CUSTOM2:
    str = "custom2";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_CUSTOM3:
    str = "custom3";
  break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_ZERO_8:
    str = "zero_8";
    break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_ZERO_16:
    str = "zero_16";
    break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_ZERO_32:
    str = "zero_32";
    break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_ONES_32:
    str = "ones_32";
    break;
  case SOC_TMC_PMF_IRPP_INFO_FIELD_LEARN_INFO:
    str = "learn_info";
    break;

  default:
    str = " Unknown";
  }
  return str;
}


void
  SOC_TMC_PMF_CE_PACKET_HEADER_INFO_print(
    SOC_SAND_IN  SOC_TMC_PMF_CE_PACKET_HEADER_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "sub_header %s, "), SOC_TMC_PMF_CE_SUB_HEADER_to_string(info->sub_header)));
  LOG_CLI((BSL_META_U(unit,
                      "offset: %u, "),info->offset));
  LOG_CLI((BSL_META_U(unit,
                      "nof_bits: %u\n\r"),info->nof_bits));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

