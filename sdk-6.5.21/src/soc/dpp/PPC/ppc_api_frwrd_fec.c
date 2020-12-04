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

#include <soc/dpp/PPC/ppc_api_frwrd_fec.h>





















void
  SOC_PPC_FRWRD_FEC_GLBL_INFO_clear(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_GLBL_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < SOC_DPP_DEFS_MAX(ECMP_MAX_SIZE); ++ind)
  {
    info->ecmp_sizes[ind] = 0;
  }
  info->ecmp_sizes_nof_entries = SOC_DPP_DEFS_GET(unit, ecmp_max_size);
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_FEC_ENTRY_RPF_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_ENTRY_RPF_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_FEC_ENTRY_RPF_INFO));
  info->rpf_mode = SOC_PPC_NOF_FRWRD_FEC_RPF_MODES;
  info->expected_in_rif = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_FEC_ENTRY_APP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_ENTRY_APP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_FEC_ENTRY_APP_INFO));
  info->out_ac_id = 0;
  info->out_rif = 0;
  info->dist_tree_nick = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_FEC_ENTRY_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_ENTRY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_FEC_ENTRY_INFO));
  info->type = SOC_PPC_NOF_FEC_TYPES_ARAD;
  soc_sand_SAND_PP_DESTINATION_ID_clear(&(info->dest));
  info->eep = 0;
  SOC_PPC_FRWRD_FEC_ENTRY_APP_INFO_clear(&(info->app_info));
  SOC_PPC_FRWRD_FEC_ENTRY_RPF_INFO_clear(&(info->rpf_info));
  SOC_PPC_ACTION_PROFILE_clear(&(info->trap_info));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO));
  info->trap_if_accessed = 0;
  info->accessed = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_FEC_PROTECT_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_PROTECT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_FEC_PROTECT_INFO));
  info->oam_instance_id = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_FEC_ECMP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_ECMP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_FEC_ECMP_INFO));
  info->base_fec_id = 0;
  info->size = 1;
  info->is_protected = FALSE;
  info->rpf_fec_index = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_FEC_ENTRY_USE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_ENTRY_USE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_FEC_ENTRY_USE_INFO));
  info->type = SOC_PPC_NOF_FRWRD_FEC_ENTRY_USE_TYPES;
  info->nof_entries = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_FEC_MATCH_RULE_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_MATCH_RULE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_FEC_MATCH_RULE));
  info->type = SOC_PPC_NOF_FRWRD_FEC_MATCH_RULE_TYPES;
  info->value = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO));

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_FEC_ECMP_HASH_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_ECMP_HASH_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_FEC_ECMP_HASH_PORT_INFO));
  info->nof_headers = 0;
  info->skip_bos = FALSE;
  info->start_from_bos = FALSE;

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_FEC_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_FEC_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_FEC_TYPE_DROP:
    str = "drop";
  break;
  case SOC_PPC_FEC_TYPE_IP_MC:
    str = "ip_mc";
  break;
  case SOC_PPC_FEC_TYPE_IP_UC:
    str = "ip_uc";
  break;
  case SOC_PPC_FEC_TYPE_TRILL_MC:
    str = "trill_mc";
  break;
  case SOC_PPC_FEC_TYPE_TRILL_UC:
    str = "trill_uc";
  break;
  case SOC_PPC_FEC_TYPE_BRIDGING_INTO_TUNNEL:
    str = "bridging_into_tunnel";
  break;
  case SOC_PPC_FEC_TYPE_BRIDGING_WITH_AC:
    str = "bridging_with_ac";
  break;
  case SOC_PPC_FEC_TYPE_MPLS_LSR:
    str = "mpls_lsr";
  break;
  case SOC_PPC_FEC_TYPE_SIMPLE_DEST:
    str = "simple_dest";
  break;
  case SOC_PPC_FEC_TYPE_ROUTING:
    str = "routing";
  break;
  case SOC_PPC_FEC_TYPE_TUNNELING:
    str = "tunneling";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_FEC_TYPE_to_string_short(
    SOC_SAND_IN  SOC_PPC_FEC_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_FEC_TYPE_DROP:
    str = "drop";
  break;
  case SOC_PPC_FEC_TYPE_IP_MC:
    str = "ip_mc";
  break;
  case SOC_PPC_FEC_TYPE_IP_UC:
    str = "ip_uc";
  break;
  case SOC_PPC_FEC_TYPE_TRILL_MC:
    str = "trl_mc";
  break;
  case SOC_PPC_FEC_TYPE_TRILL_UC:
    str = "trl_uc";
  break;
  case SOC_PPC_FEC_TYPE_BRIDGING_INTO_TUNNEL:
    str = "b_tnl";
  break;
  case SOC_PPC_FEC_TYPE_BRIDGING_WITH_AC:
    str = "b_ac";
  break;
  case SOC_PPC_FEC_TYPE_MPLS_LSR:
    str = "mpls_lsr";
  break;
  case SOC_PPC_FEC_TYPE_SIMPLE_DEST:
    str = "simple";
  break;
  case SOC_PPC_FEC_TYPE_ROUTING:
    str = "route";
  break;
  case SOC_PPC_FEC_TYPE_TUNNELING:
    str = "tunnel";
  break;
  case SOC_PPC_FEC_TYPE_TUNNELING_EEI_OUTLIF:
    str = "tunnel-eei";
  break;
  case SOC_PPC_FEC_TYPE_TUNNELING_EEI_MPLS_COMMAND:
    str = "mpls_cmd";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_FRWRD_FEC_RPF_MODE_to_string(
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_RPF_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_FRWRD_FEC_RPF_MODE_NONE:
    str = "none";
  break;
  case SOC_PPC_FRWRD_FEC_RPF_MODE_UC_STRICT:
    str = "uc_strict";
  break;
  case SOC_PPC_FRWRD_FEC_RPF_MODE_UC_LOOSE:
    str = "uc_loose";
  break;
  case SOC_PPC_FRWRD_FEC_RPF_MODE_MC_EXPLICIT:
    str = "mc_explicit";
  break;
  case SOC_PPC_FRWRD_FEC_RPF_MODE_MC_USE_SIP:
    str = "mc_use_sip";
  break;
  case SOC_PPC_FRWRD_FEC_RPF_MODE_MC_USE_SIP_WITH_ECMP:
    str = "mc_use_sip_with_ecmp";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_FRWRD_FEC_PROTECT_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_PROTECT_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_FRWRD_FEC_PROTECT_TYPE_NONE:
    str = "none";
  break;
  case SOC_PPC_FRWRD_FEC_PROTECT_TYPE_FACILITY:
    str = "facility";
  break;
  case SOC_PPC_FRWRD_FEC_PROTECT_TYPE_PATH:
    str = "path";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_FRWRD_FEC_ENTRY_USE_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_USE_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_FRWRD_FEC_ENTRY_USE_TYPE_NONE:
    str = "none";
  break;
  case SOC_PPC_FRWRD_FEC_ENTRY_USE_TYPE_ONE_FEC:
    str = "one_fec";
  break;
  case SOC_PPC_FRWRD_FEC_ENTRY_USE_TYPE_ECMP:
    str = "ecmp";
  break;
  case SOC_PPC_FRWRD_FEC_ENTRY_USE_TYPE_PATH_PROTECT:
    str = "path_protect";
  break;
  case SOC_PPC_FRWRD_FEC_ENTRY_USE_TYPE_FACILITY_PROTECT:
    str = "facility_protect";
  break;
  case SOC_PPC_FRWRD_FEC_ENTRY_USE_TYPE_ECMP_PROTECTED:
    str = "ecmp_protected";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_FRWRD_FEC_MATCH_RULE_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_MATCH_RULE_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_FRWRD_FEC_MATCH_RULE_TYPE_ALL:
    str = "all";
  break;
  case SOC_PPC_FRWRD_FEC_MATCH_RULE_TYPE_APP_TYPE:
    str = "app_type";
  break;
  case SOC_PPC_FRWRD_FEC_MATCH_RULE_TYPE_ACCESSED:
    str = "accessed";
  break;
  case SOC_PPC_FRWRD_FEC_MATCH_RULE_TYPE_PATH_PROTECTED:
    str = "path_protected";
  break;
  case SOC_PPC_FRWRD_FEC_MATCH_RULE_TYPE_FACILITY_PROTECTED:
    str = "facility_protected";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_PPC_FRWRD_FEC_GLBL_INFO_print(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_GLBL_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < SOC_DPP_DEFS_GET(unit, ecmp_max_size); ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "ecmp_sizes[%u]: %u\n\r"),ind,info->ecmp_sizes[ind]));
  }
  LOG_CLI((BSL_META_U(unit,
                      "ecmp_sizes_nof_entries: %u\n\r"),info->ecmp_sizes_nof_entries));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_FEC_ENTRY_RPF_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_RPF_INFO *info
  )
{
  uint32
    indx;
  uint8
    rpf_set = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  
  for(indx = SOC_PPC_FRWRD_FEC_RPF_MODE_UC_LOOSE; indx <= SOC_PPC_FRWRD_FEC_RPF_MODE_UC_STRICT; indx = indx << 1) {
      if(indx & info->rpf_mode) {
          LOG_CLI((BSL_META_U(unit,
                              "uc_rpf_mode %s \n\r"), SOC_PPC_FRWRD_FEC_RPF_MODE_to_string(indx)));
          rpf_set = TRUE;
      }
  }
  if(!rpf_set) {
      LOG_CLI((BSL_META_U(unit,
                          "uc_rpf_mode %s \n\r"), SOC_PPC_FRWRD_FEC_RPF_MODE_to_string(SOC_PPC_FRWRD_FEC_RPF_MODE_NONE)));
  }

  
  for(indx = SOC_PPC_FRWRD_FEC_RPF_MODE_MC_EXPLICIT; indx <= SOC_PPC_FRWRD_FEC_RPF_MODE_MC_USE_SIP_WITH_ECMP; indx = indx << 1) {
      if(indx & info->rpf_mode) {
          LOG_CLI((BSL_META_U(unit,
                              "mc_rpf_mode %s \n\r"), SOC_PPC_FRWRD_FEC_RPF_MODE_to_string(indx)));
      }
  }
  LOG_CLI((BSL_META_U(unit,
                      "expected_in_rif: %u\n\r"),info->expected_in_rif));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_FEC_ENTRY_APP_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_APP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "out_ac_id: %u\n\r"),info->out_ac_id));
  LOG_CLI((BSL_META_U(unit,
                      "out_rif: %u\n\r"),info->out_rif));
  LOG_CLI((BSL_META_U(unit,
                      "dist_tree_nick: %u\n\r"),info->dist_tree_nick));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_FEC_ENTRY_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "type %s "), SOC_PPC_FEC_TYPE_to_string(info->type)));
  LOG_CLI((BSL_META_U(unit,
                      "dest:")));
  soc_sand_SAND_PP_DESTINATION_ID_print(&(info->dest));
  LOG_CLI((BSL_META_U(unit,
                      "eep: %u\n\r"),info->eep));
  switch(info->type)
  {
  case SOC_PPC_FEC_TYPE_DROP:
  case SOC_PPC_FEC_TYPE_TRILL_UC:
  case SOC_PPC_FEC_TYPE_BRIDGING_WITH_AC:
    LOG_CLI((BSL_META_U(unit,
                        "out_ac_id: %u\n\r"),info->app_info.out_ac_id));
    break;
  case SOC_PPC_FEC_TYPE_IP_MC:
    SOC_PPC_FRWRD_FEC_ENTRY_RPF_INFO_print(&info->rpf_info);
    break;
  case SOC_PPC_FEC_TYPE_IP_UC:
  case SOC_PPC_FEC_TYPE_ROUTING:
    LOG_CLI((BSL_META_U(unit,
                        "out_rif: %u\n\r"),info->app_info.out_rif));
    SOC_PPC_FRWRD_FEC_ENTRY_RPF_INFO_print(&info->rpf_info);
    break;
  case SOC_PPC_FEC_TYPE_TRILL_MC:
    LOG_CLI((BSL_META_U(unit,
                        "dist_tree_nick: %u\n\r"),info->app_info.dist_tree_nick));
    break;
  case SOC_PPC_FEC_TYPE_MPLS_LSR:
    LOG_CLI((BSL_META_U(unit,
                        "out_rif: %u\n\r"),info->app_info.out_rif));
    break;
  default:
    break;
  }
  if (info->dest.dest_type == SOC_SAND_PP_DEST_TRAP)
  {
    LOG_CLI((BSL_META_U(unit,
                        "trap_info:")));
    SOC_PPC_ACTION_PROFILE_print(&(info->trap_info));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "trap_if_accessed: %u\n\r"),info->trap_if_accessed));
  LOG_CLI((BSL_META_U(unit,
                      "accessed: %u\n\r"),info->accessed));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_FEC_PROTECT_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_PROTECT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "oam_instance_id: %u\n\r"),info->oam_instance_id));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_FEC_ECMP_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ECMP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "base_fec_id: %u\n\r"),info->base_fec_id));
  LOG_CLI((BSL_META_U(unit,
                      "size: %u\n\r"),info->size));
  LOG_CLI((BSL_META_U(unit,
                      "is_protected: %u\n\r"),info->is_protected));
  LOG_CLI((BSL_META_U(unit,
                      "rpf_fec_index: %u\n\r"),info->rpf_fec_index));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_PPC_FRWRD_FEC_ENTRY_USE_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ENTRY_USE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "type %s "), SOC_PPC_FRWRD_FEC_ENTRY_USE_TYPE_to_string(info->type)));
  LOG_CLI((BSL_META_U(unit,
                      "nof_entries: %u\n\r"),info->nof_entries));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_FEC_MATCH_RULE_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_MATCH_RULE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "type %s "), SOC_PPC_FRWRD_FEC_MATCH_RULE_TYPE_to_string(info->type)));
  LOG_CLI((BSL_META_U(unit,
                      "value: %u\n\r"),info->value));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "use_port_id: %u\n\r"),info->use_port_id));
  LOG_CLI((BSL_META_U(unit,
                      "seed: %u\n\r"),info->seed));
  LOG_CLI((BSL_META_U(unit,
                      "hash_func_id: %u\n\r"), info->hash_func_id));
  LOG_CLI((BSL_META_U(unit,
                      "key_shift: %u\n\r"), info->key_shift));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_FEC_ECMP_HASH_PORT_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_FEC_ECMP_HASH_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "nof_headers: %u\n\r"), info->nof_headers));

  LOG_CLI((BSL_META_U(unit,
                      "start_from_bos: %u\n\r"), info->start_from_bos));
  LOG_CLI((BSL_META_U(unit,
                      "skip_bos: %u\n\r"), info->skip_bos));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

