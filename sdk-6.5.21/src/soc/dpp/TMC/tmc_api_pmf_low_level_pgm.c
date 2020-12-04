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

#include <soc/dpp/TMC/tmc_api_pmf_low_level_pgm.h>






















void
  SOC_TMC_PMF_PGM_SEL_VAL_PFQ_SEM_clear(
    SOC_SAND_OUT SOC_TMC_PMF_PGM_SEL_VAL_PFQ_SEM *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PMF_PGM_SEL_VAL_PFQ_SEM));
  info->pfq = 0;
  info->sem_13_8_ndx = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PMF_PGM_SEL_VAL_FWD_TTC_clear(
    SOC_SAND_OUT SOC_TMC_PMF_PGM_SEL_VAL_FWD_TTC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PMF_PGM_SEL_VAL_FWD_TTC));
  info->fwd = SOC_TMC_NOF_PKT_FRWRD_TYPES;
  info->ttc = SOC_TMC_NOF_TUNNEL_TERM_CODES;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PMF_PGM_SEL_VAL_PRSR_PMF_clear(
    SOC_SAND_OUT SOC_TMC_PMF_PGM_SEL_VAL_PRSR_PMF *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PMF_PGM_SEL_VAL_PRSR_PMF));
  info->prsr = 0;
  info->port_pmf = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PMF_PGM_SEL_VAL_LKP_clear(
    SOC_SAND_OUT SOC_TMC_PMF_PGM_SEL_VAL_LKP *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PMF_PGM_SEL_VAL_LKP));
  info->lem_1st_found = 0;
  info->lem_2nd_found = 0;
  info->sem_1st_found = 0;
  info->sem_2nd_found = 0;
  info->lpm_1st_not_dflt = 0;
  info->lpm_2nd_not_dflt = 0;
  info->tcam_found = 0;
  info->elk_found = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PMF_PGM_SEL_VAL_LLVP_PFC_clear(
    SOC_SAND_OUT SOC_TMC_PMF_PGM_SEL_VAL_LLVP_PFC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PMF_PGM_SEL_VAL_LLVP_PFC));
  info->llvp = 0;
  info->pmf_pro = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PMF_PGM_SEL_VAL_clear(
    SOC_SAND_IN  SOC_TMC_PMF_PGM_SEL_TYPE sel_type,
    SOC_SAND_OUT SOC_TMC_PMF_PGM_SEL_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PMF_PGM_SEL_VAL));

  switch(sel_type)
  {
  case SOC_TMC_PMF_PGM_SEL_TYPE_EEI_OUTLIF_15_8:
    info->eei_outlif_15_8 = 0;
    break;
  case SOC_TMC_PMF_PGM_SEL_TYPE_EEI_OUTLIF_7_0:
    info->eei_outlif_7_0 = 0;
    break;
  case SOC_TMC_PMF_PGM_SEL_TYPE_SEM_NDX_7_0:
    info->sem_7_0_ndx = 0;
    break;
  case SOC_TMC_PMF_PGM_SEL_TYPE_PFQ_SEM_NDX_13_8:
    SOC_TMC_PMF_PGM_SEL_VAL_PFQ_SEM_clear(&(info->pfq_sem));
    break;
  case SOC_TMC_PMF_PGM_SEL_TYPE_FWDING_TTC_CODE:
    SOC_TMC_PMF_PGM_SEL_VAL_FWD_TTC_clear(&(info->fwd_ttc));
    break;
  case SOC_TMC_PMF_PGM_SEL_TYPE_PARSER_PMF_PRO:
    SOC_TMC_PMF_PGM_SEL_VAL_PRSR_PMF_clear(&(info->prsr_pmf));
    break;
  case SOC_TMC_PMF_PGM_SEL_TYPE_LOOKUPS:
    SOC_TMC_PMF_PGM_SEL_VAL_LKP_clear(&(info->lkp));
    break;
  case SOC_TMC_PMF_PGM_SEL_TYPE_LLVP_PFC:
  default:
    SOC_TMC_PMF_PGM_SEL_VAL_LLVP_PFC_clear(&(info->llvp_pfc));
    break;
  }

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PMF_PGM_SELECTION_ENTRY_clear(
    SOC_SAND_OUT SOC_TMC_PMF_PGM_SELECTION_ENTRY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PMF_PGM_SELECTION_ENTRY));
  info->type = SOC_TMC_NOF_PMF_PGM_SEL_TYPES;
  SOC_TMC_PMF_PGM_SEL_VAL_clear(info->type, &(info->val));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PMF_PGM_VALIDITY_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PMF_PGM_VALIDITY_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PMF_PGM_VALIDITY_INFO));
  for (ind = 0; ind < SOC_TMC_PMF_NOF_PGMS; ++ind)
  {
    info->is_pgm_valid[ind] = 0;
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PMF_PGM_BYTES_TO_RMV_clear(
    SOC_SAND_OUT SOC_TMC_PMF_PGM_BYTES_TO_RMV *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PMF_PGM_BYTES_TO_RMV));
  info->header_type = SOC_TMC_NOF_PMF_PGM_BYTES_TO_RMV_HDRS;
  info->nof_bytes = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PMF_PGM_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PMF_PGM_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PMF_PGM_INFO));
  for (ind = 0; ind < SOC_TMC_PMF_NOF_CYCLES; ++ind)
  {
    info->lkp_profile_id[ind] = 0;
  }
  info->tag_profile_id = 0;
  info->header_type = SOC_TMC_PORT_NOF_HEADER_TYPES;
  SOC_TMC_PMF_PGM_BYTES_TO_RMV_clear(&(info->bytes_to_rmv));
  info->copy_pgm_var = 0;
  info->fc_type = SOC_TMC_PORTS_NOF_FC_TYPES;
  info->header_profile = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_TMC_DEBUG_IS_LVL1

const char*
  SOC_TMC_PMF_PGM_SEL_TYPE_to_string(
    SOC_SAND_IN  SOC_TMC_PMF_PGM_SEL_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_PMF_PGM_SEL_TYPE_EEI_OUTLIF_15_8:
    str = "eei_outlif_15_8";
  break;
  case SOC_TMC_PMF_PGM_SEL_TYPE_EEI_OUTLIF_7_0:
    str = "eei_outlif_7_0";
  break;
  case SOC_TMC_PMF_PGM_SEL_TYPE_SEM_NDX_7_0:
    str = "sem_ndx_7_0";
  break;
  case SOC_TMC_PMF_PGM_SEL_TYPE_PFQ_SEM_NDX_13_8:
    str = "pfq_sem_ndx_13_8";
  break;
  case SOC_TMC_PMF_PGM_SEL_TYPE_FWDING_TTC_CODE:
    str = "fwding_ttc_code";
  break;
  case SOC_TMC_PMF_PGM_SEL_TYPE_PARSER_PMF_PRO:
    str = "parser_pmf_pro";
  break;
  case SOC_TMC_PMF_PGM_SEL_TYPE_LOOKUPS:
    str = "lookups";
  break;
  case SOC_TMC_PMF_PGM_SEL_TYPE_LLVP_PFC:
    str = "llvp_pfc";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_PMF_PGM_BYTES_TO_RMV_HDR_to_string(
    SOC_SAND_IN  SOC_TMC_PMF_PGM_BYTES_TO_RMV_HDR enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_PMF_PGM_BYTES_TO_RMV_HDR_START:
    str = "start";
  break;
  case SOC_TMC_PMF_PGM_BYTES_TO_RMV_HDR_1ST:
    str = "hdr_1st";
  break;
  case SOC_TMC_PMF_PGM_BYTES_TO_RMV_HDR_FWDING:
    str = "fwding";
  break;
  case SOC_TMC_PMF_PGM_BYTES_TO_RMV_HDR_POST_FWDING:
    str = "post_fwding";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_TMC_PMF_PGM_SEL_VAL_PFQ_SEM_print(
    SOC_SAND_IN  SOC_TMC_PMF_PGM_SEL_VAL_PFQ_SEM *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "pfq: %u\n\r"),info->pfq));
  LOG_CLI((BSL_META_U(unit,
                      "sem_13_8_ndx: %u\n\r"),info->sem_13_8_ndx));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PMF_PGM_SEL_VAL_FWD_TTC_print(
    SOC_SAND_IN  SOC_TMC_PMF_PGM_SEL_VAL_FWD_TTC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "fwd %s "), SOC_TMC_PKT_FRWRD_TYPE_to_string(info->fwd)));
  LOG_CLI((BSL_META_U(unit,
                      "ttc %s "), SOC_TMC_TUNNEL_TERM_CODE_to_string(info->ttc)));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PMF_PGM_SEL_VAL_PRSR_PMF_print(
    SOC_SAND_IN  SOC_TMC_PMF_PGM_SEL_VAL_PRSR_PMF *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "prsr: %u\n\r"),info->prsr));
  LOG_CLI((BSL_META_U(unit,
                      "port_pmf: %u\n\r"),info->port_pmf));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PMF_PGM_SEL_VAL_LKP_print(
    SOC_SAND_IN  SOC_TMC_PMF_PGM_SEL_VAL_LKP *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "lem_1st_found: %u\n\r"),info->lem_1st_found));
  LOG_CLI((BSL_META_U(unit,
                      "lem_2nd_found: %u\n\r"),info->lem_2nd_found));
  LOG_CLI((BSL_META_U(unit,
                      "sem_1st_found: %u\n\r"),info->sem_1st_found));
  LOG_CLI((BSL_META_U(unit,
                      "sem_2nd_found: %u\n\r"),info->sem_2nd_found));
  LOG_CLI((BSL_META_U(unit,
                      "lpm_1st_not_dflt: %u\n\r"),info->lpm_1st_not_dflt));
  LOG_CLI((BSL_META_U(unit,
                      "lpm_2nd_not_dflt: %u\n\r"),info->lpm_2nd_not_dflt));
  LOG_CLI((BSL_META_U(unit,
                      "tcam_found: %u\n\r"),info->tcam_found));
  LOG_CLI((BSL_META_U(unit,
                      "elk_found: %u\n\r"),info->elk_found));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PMF_PGM_SEL_VAL_LLVP_PFC_print(
    SOC_SAND_IN  SOC_TMC_PMF_PGM_SEL_VAL_LLVP_PFC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "llvp: %u\n\r"),info->llvp));
  LOG_CLI((BSL_META_U(unit,
                      "pmf_pro: %u\n\r"),info->pmf_pro));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PMF_PGM_SEL_VAL_print(
    SOC_SAND_IN  SOC_TMC_PMF_PGM_SEL_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "eei_outlif_15_8: %u\n\r"),info->eei_outlif_15_8));
  LOG_CLI((BSL_META_U(unit,
                      "eei_outlif_7_0: %u\n\r"),info->eei_outlif_7_0));
  LOG_CLI((BSL_META_U(unit,
                      "sem_7_0_ndx: %u\n\r"),info->sem_7_0_ndx));
  LOG_CLI((BSL_META_U(unit,
                      "pfq_sem:")));
  SOC_TMC_PMF_PGM_SEL_VAL_PFQ_SEM_print(&(info->pfq_sem));
  LOG_CLI((BSL_META_U(unit,
                      "fwd_ttc:")));
  SOC_TMC_PMF_PGM_SEL_VAL_FWD_TTC_print(&(info->fwd_ttc));
  LOG_CLI((BSL_META_U(unit,
                      "prsr_pmf:")));
  SOC_TMC_PMF_PGM_SEL_VAL_PRSR_PMF_print(&(info->prsr_pmf));
  LOG_CLI((BSL_META_U(unit,
                      "lkp:")));
  SOC_TMC_PMF_PGM_SEL_VAL_LKP_print(&(info->lkp));
  LOG_CLI((BSL_META_U(unit,
                      "llvp_pfc:")));
  SOC_TMC_PMF_PGM_SEL_VAL_LLVP_PFC_print(&(info->llvp_pfc));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PMF_PGM_SELECTION_ENTRY_print(
    SOC_SAND_IN  SOC_TMC_PMF_PGM_SELECTION_ENTRY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "type %s "), SOC_TMC_PMF_PGM_SEL_TYPE_to_string(info->type)));
  LOG_CLI((BSL_META_U(unit,
                      "val:")));
  SOC_TMC_PMF_PGM_SEL_VAL_print(&(info->val));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PMF_PGM_VALIDITY_INFO_print(
    SOC_SAND_IN  SOC_TMC_PMF_PGM_VALIDITY_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < SOC_TMC_PMF_NOF_PGMS; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "is_pgm_valid[%u]: %u\n\r"),ind,info->is_pgm_valid[ind]));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PMF_PGM_BYTES_TO_RMV_print(
    SOC_SAND_IN  SOC_TMC_PMF_PGM_BYTES_TO_RMV *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "header_type %s "), SOC_TMC_PMF_PGM_BYTES_TO_RMV_HDR_to_string(info->header_type)));
  LOG_CLI((BSL_META_U(unit,
                      "nof_bytes: %u\n\r"),info->nof_bytes));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PMF_PGM_INFO_print(
    SOC_SAND_IN  SOC_TMC_PMF_PGM_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < SOC_TMC_PMF_NOF_CYCLES; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "lkp_profile_id[%u]: %u\n\r"),ind,info->lkp_profile_id[ind]));
  }
  LOG_CLI((BSL_META_U(unit,
                      "tag_profile_id: %u\n\r"),info->tag_profile_id));
  LOG_CLI((BSL_META_U(unit,
                      "header_type %s \n\r"), SOC_TMC_PORT_HEADER_TYPE_to_string(info->header_type)));
  LOG_CLI((BSL_META_U(unit,
                      "bytes_to_rmv:")));
  SOC_TMC_PMF_PGM_BYTES_TO_RMV_print(&(info->bytes_to_rmv));
  LOG_CLI((BSL_META_U(unit,
                      "copy_pgm_var: %u\n\r"),info->copy_pgm_var));
  LOG_CLI((BSL_META_U(unit,
                      "fc_type %s \n\r"), SOC_TMC_PORTS_FC_TYPE_to_string(info->fc_type)));
  LOG_CLI((BSL_META_U(unit,
                      "header_profile %d \n\r"), info->header_profile));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

