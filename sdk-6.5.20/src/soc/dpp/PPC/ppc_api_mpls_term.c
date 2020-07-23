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

#include <soc/dpp/PPC/ppc_api_mpls_term.h>





















void
  SOC_PPC_MPLS_TERM_LABEL_RANGE_clear(
    SOC_SAND_OUT SOC_PPC_MPLS_TERM_LABEL_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_MPLS_TERM_LABEL_RANGE));
  info->first_label = 0;
  info->last_label = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_MPLS_TERM_LKUP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_MPLS_TERM_LKUP_INFO *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_MPLS_TERM_LKUP_INFO));
  info->key_type = SOC_PPC_MPLS_TERM_KEY_TYPE_LABEL;
  
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_MPLS_TERM_INFO_clear(
    SOC_SAND_OUT SOC_PPC_MPLS_TERM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_MPLS_TERM_INFO));
  SOC_PPC_L2_LIF_DEFAULT_FORWARDING_INFO_clear(&(info->default_frwrd));
  info->processing_type = SOC_PPC_NOF_MPLS_TERM_MODEL_TYPES;
  info->rif = 0;
  info->cos_profile = 0;
  info->next_prtcl = SOC_PPC_NOF_L3_NEXT_PRTCL_TYPES;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_MPLS_TERM_LABEL_RANGE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_MPLS_TERM_LABEL_RANGE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_MPLS_TERM_LABEL_RANGE_INFO));
  SOC_PPC_MPLS_TERM_LABEL_RANGE_clear(&(info->range));
  SOC_PPC_MPLS_TERM_INFO_clear(&(info->term_info));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO));
  SOC_PPC_ACTION_PROFILE_clear(&(info->bos_action));
  SOC_PPC_ACTION_PROFILE_clear(&(info->non_bos_action));
  SOC_PPC_MPLS_TERM_INFO_clear(&(info->term_info));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_MPLS_TERM_RESERVED_LABELS_GLBL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_MPLS_TERM_RESERVED_LABELS_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_MPLS_TERM_RESERVED_LABELS_GLBL_INFO));
  info->processing_type = SOC_PPC_NOF_MPLS_TERM_MODEL_TYPES;
  info->default_rif = 0;
  info->uc_rpf_enable = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_MPLS_TERM_COS_INFO_clear(
    SOC_SAND_OUT SOC_PPC_MPLS_TERM_COS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_MPLS_TERM_COS_INFO));
  info->use_for_pipe = 0;
  info->use_for_uniform = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_MPLS_TERM_LABEL_COS_KEY_clear(
    SOC_SAND_OUT SOC_PPC_MPLS_TERM_LABEL_COS_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_MPLS_TERM_LABEL_COS_KEY));
  info->in_exp = 0;
  info->model = SOC_SAND_PP_NOF_MPLS_TUNNEL_MODELS;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_MPLS_TERM_LABEL_COS_VAL_clear(
    SOC_SAND_OUT SOC_PPC_MPLS_TERM_LABEL_COS_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_MPLS_TERM_LABEL_COS_VAL));
  info->tc = 0;
  info->overwrite_tc = 0;
  info->dp = 0;
  info->overwrite_dp = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_MPLS_TERM_PROFILE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_MPLS_TERM_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_MPLS_TERM_PROFILE_INFO));
  info->flags = 0;
  info->nof_headers = 1;
  info->ttl_exp_index = 0;

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_MPLS_TERM_RANGE_ACTION_INFO_clear(
    SOC_SAND_OUT SOC_PPC_MPLS_TERM_RANGE_ACTION_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_MPLS_TERM_RANGE_ACTION_INFO));
  info->label_low = 0;
  info->label_high = 0;
  info->bos_value = 0;
  info->bos_value_mask = 1;

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_MPLS_TERM_RANGE_PROFILE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_MPLS_TERM_RANGE_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_MPLS_TERM_RANGE_PROFILE_INFO));
  info->mpls_label_range_tag_mode = 0;
  info->mpls_label_range_has_cw = 0;
  info->mpls_label_range_set_outer_vid = 0;
  info->mpls_label_range_set_inner_vid = 0;
  info->mpls_label_range_use_base = 0;

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_MPLS_TERM_MODEL_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_MODEL_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_MPLS_TERM_MODEL_PIPE:
    str = "pipe";
  break;
  case SOC_PPC_MPLS_TERM_MODEL_UNIFORM:
    str = "uniform";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_MPLS_TERM_KEY_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_KEY_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_MPLS_TERM_KEY_TYPE_LABEL:
    str = "label";
  break;
  case SOC_PPC_MPLS_TERM_KEY_TYPE_LABEL_RIF:
    str = "label_rif";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_PPC_MPLS_TERM_LABEL_RANGE_print(
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_LABEL_RANGE *info
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
  SOC_PPC_MPLS_TERM_LKUP_INFO_print(
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_LKUP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);
  LOG_CLI((BSL_META_U(unit,
                      "key_type %s "),SOC_PPC_MPLS_TERM_KEY_TYPE_to_string(info->key_type)));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_MPLS_TERM_INFO_print(
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "processing_type %s "), SOC_PPC_MPLS_TERM_MODEL_TYPE_to_string(info->processing_type)));
  if (info->rif == SOC_PPC_RIF_NULL) {
      LOG_CLI((BSL_META_U(unit,
                          "rif: null\n")));
  }
  else{
      LOG_CLI((BSL_META_U(unit,
                          "rif: %u\n\r"),info->rif));
  }
  LOG_CLI((BSL_META_U(unit,
                      "term_cos_profile: %u\n\r"),info->cos_profile));
  LOG_CLI((BSL_META_U(unit,
                      "lif_term_profile: %u\n\r"),info->term_profile));
  LOG_CLI((BSL_META_U(unit,
                      "next_prtcl %s\n\r"), SOC_PPC_L3_NEXT_PRTCL_TYPE_to_string(info->next_prtcl)));

  
  LOG_CLI((BSL_META_U(unit,
                      "in_lif_profile: %u\n\r"),info->lif_profile));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_MPLS_TERM_LABEL_RANGE_INFO_print(
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_LABEL_RANGE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "range:")));
  SOC_PPC_MPLS_TERM_LABEL_RANGE_print(&(info->range));
  LOG_CLI((BSL_META_U(unit,
                      "term_info:")));
  SOC_PPC_MPLS_TERM_INFO_print(&(info->term_info));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO_print(
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      " - bos_action:\n\r")));
  SOC_PPC_ACTION_PROFILE_print(&(info->bos_action));
  LOG_CLI((BSL_META_U(unit,
                      " - non_bos_action:\n\r")));
  SOC_PPC_ACTION_PROFILE_print(&(info->non_bos_action));
  LOG_CLI((BSL_META_U(unit,
                      " - term_info:\n\r")));
  SOC_PPC_MPLS_TERM_INFO_print(&(info->term_info));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_MPLS_TERM_RESERVED_LABELS_GLBL_INFO_print(
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_RESERVED_LABELS_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "processing_type %s "), SOC_PPC_MPLS_TERM_MODEL_TYPE_to_string(info->processing_type)));
  LOG_CLI((BSL_META_U(unit,
                      "default_rif: %u\n\r"),info->default_rif));
  LOG_CLI((BSL_META_U(unit,
                      "uc_rpf_enable: %u\n\r"),info->uc_rpf_enable));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_MPLS_TERM_COS_INFO_print(
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_COS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "use_for_pipe: %u\n\r"),info->use_for_pipe));
  LOG_CLI((BSL_META_U(unit,
                      "use_for_uniform: %u\n\r"),info->use_for_uniform));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_MPLS_TERM_LABEL_COS_KEY_print(
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_LABEL_COS_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "in_exp: %u\n\r"), info->in_exp));
  LOG_CLI((BSL_META_U(unit,
                      "model %s "), soc_sand_SAND_PP_MPLS_TUNNEL_MODEL_to_string(info->model)));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_MPLS_TERM_LABEL_COS_VAL_print(
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_LABEL_COS_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "tc: %u\n\r"), info->tc));
  LOG_CLI((BSL_META_U(unit,
                      "overwrite_tc: %u\n\r"),info->overwrite_tc));
  LOG_CLI((BSL_META_U(unit,
                      "dp: %u\n\r"), info->dp));
  LOG_CLI((BSL_META_U(unit,
                      "overwrite_dp: %u\n\r"),info->overwrite_dp));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_MPLS_TERM_PROFILE_INFO_print(
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "flags:\n\r")));
  if (info->flags & SOC_PPC_MPLS_TERM_DISCARD_TTL_0) {
	  LOG_CLI((BSL_META_U(unit,
                              "\t DISCARD_TTL_0\n\r")));
  }
  if (info->flags & SOC_PPC_MPLS_TERM_DISCARD_TTL_1) {
	  LOG_CLI((BSL_META_U(unit,
                              "\t DISCARD_TTL_1\n\r")));
  }
  if (info->flags & SOC_PPC_MPLS_TERM_HAS_CW) {
	  LOG_CLI((BSL_META_U(unit,
                              "\t HAS_CW\n\r")));
  }
  if (info->flags & SOC_PPC_MPLS_TERM_SKIP_ETH) {
	  LOG_CLI((BSL_META_U(unit,
                              "\t SKIP_ETH\n\r")));
  }
  LOG_CLI((BSL_META_U(unit,
                      "number_of_headers: %u\n\r"),info->nof_headers));
  LOG_CLI((BSL_META_U(unit,
                      "ttl_exp_index: %u\n\r"),info->ttl_exp_index));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_MPLS_TERM_RANGE_ACTION_INFO_print(
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_RANGE_ACTION_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "label_low: %u\n\r"), info->label_low));
  LOG_CLI((BSL_META_U(unit,
                      "label_high: %u\n\r"),info->label_high));
  LOG_CLI((BSL_META_U(unit,
                      "bos_value: %u\n\r"), info->bos_value));
  LOG_CLI((BSL_META_U(unit,
                      "bos_value_mask: %u\n\r"),info->bos_value_mask));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_MPLS_TERM_RANGE_PROFILE_INFO_print(
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_RANGE_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "tag_mode: %u\n\r"), info->mpls_label_range_tag_mode));
  LOG_CLI((BSL_META_U(unit,
                      "has_cw: %u\n\r"),info->mpls_label_range_has_cw));
  LOG_CLI((BSL_META_U(unit,
                      "set_outer_vid: %u\n\r"), info->mpls_label_range_set_outer_vid));
  LOG_CLI((BSL_META_U(unit,
                      "set_inner_vid: %u\n\r"),info->mpls_label_range_set_inner_vid));
  LOG_CLI((BSL_META_U(unit,
                      "use_base: %u\n\r"),info->mpls_label_range_use_base));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

