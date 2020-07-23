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

#include <soc/dpp/PPC/ppc_api_metering.h>






















void
  SOC_PPC_MTR_METER_ID_clear(
    SOC_SAND_OUT SOC_PPC_MTR_METER_ID *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_MTR_METER_ID));
  info->group = 0;
  info->id = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_MTR_GROUP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_MTR_GROUP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_MTR_GROUP_INFO));
  info->is_hr_enabled = 0;
  info->max_packet_size = 0;
  info->rand_mode_enable = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_MTR_BW_PROFILE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_MTR_BW_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_MTR_BW_PROFILE_INFO));
  info->disable_cir = 0;
  info->disable_eir = 0;
  info->cir = 0;
  info->eir = 0;
  info->max_cir = 0;
  info->max_eir = 0;
  info->cbs = 0;
  info->ebs = 0;
  info->color_mode = SOC_PPC_NOF_MTR_COLOR_MODES;
  info->is_fairness_enabled = 0;
  info->is_coupling_enabled = 0;
  info->is_sharing_enabled = 0;
  info->is_mtr_enabled = 0;
  info->is_packet_mode = 0;
  info->ir_rev_exp = ARAD_PP_INVALID_IR_REV_EXP;
  info->mark_black = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_MTR_COLOR_DECISION_INFO_clear(
    SOC_SAND_OUT SOC_PPC_MTR_COLOR_DECISION_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_MTR_COLOR_DECISION_INFO));
  info->flags = 0;
  info->policer0_color = 0;
  info->policer0_other_bucket_has_credits = 0;
  info->policer0_update_bucket = 0;
  info->policer1_color = 0;
  info->policer1_other_bucket_has_credits = 0;
  info->policer1_update_bucket = 0;
  info->policer_color = 0;
  SOC_SAND_MAGIC_NUM_SET;

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_MTR_COLOR_RESOLUTION_INFO_clear(
    SOC_SAND_OUT SOC_PPC_MTR_COLOR_RESOLUTION_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_MTR_COLOR_RESOLUTION_INFO));
  info->ethernet_policer_color = 0;
  info->flags = 0;
  info->incoming_color = 0;
  info->policer_action = 0;
  info->policer_color = 0;
  info->ingress_color = 0;
  info->egress_color = 0;

  SOC_SAND_MAGIC_NUM_SET;

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_MTR_ETH_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_MTR_ETH_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_MTR_ETH_TYPE_UNKNOW_UC:
    str = "unknow_uc";
  break;
  case SOC_PPC_MTR_ETH_TYPE_KNOW_UC:
    str = "know_uc";
  break;
  case SOC_PPC_MTR_ETH_TYPE_UNKNOW_MC:
    str = "unknow_mc";
  break;
  case SOC_PPC_MTR_ETH_TYPE_KNOW_MC:
    str = "know_mc";
  break;
  case SOC_PPC_MTR_ETH_TYPE_BC:
    str = "bc";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_MTR_COLOR_MODE_to_string(
    SOC_SAND_IN  SOC_PPC_MTR_COLOR_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_MTR_COLOR_MODE_BLIND:
    str = "blind";
  break;
  case SOC_PPC_MTR_COLOR_MODE_AWARE:
    str = "aware";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_MTR_RES_USE_to_string(
    SOC_SAND_IN  SOC_PPC_MTR_RES_USE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_MTR_RES_USE_NONE:
    str = "none";
  break;
  case SOC_PPC_MTR_RES_USE_OW_DP:
    str = "ow_dp";
  break;
  case SOC_PPC_MTR_RES_USE_OW_DE:
    str = "ow_de";
  break;
  case SOC_PPC_MTR_RES_USE_OW_DP_DE:
    str = "ow_dp_de";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_MTR_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_MTR_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_MTR_TYPE_INLIF:
    str = "inlif";
  break;
  case SOC_PPC_MTR_TYPE_OUTLIF:
    str = "outlif";
  break;
  case SOC_PPC_MTR_TYPE_IN_AC:
    str = "in_ac";
  break;
  case SOC_PPC_MTR_TYPE_OUT_AC:
    str = "out_ac";
  break;
  case SOC_PPC_MTR_TYPE_VSID:
    str = "vsid";
  break;
  case SOC_PPC_MTR_TYPE_IN_PWE:
    str = "in_pwe";
  break;
  case SOC_PPC_MTR_TYPE_IN_LABEL:
    str = "in_label";
  break;
  case SOC_PPC_MTR_TYPE_OUT_LABEL:
    str = "out_label";
  break;
  case SOC_PPC_MTR_TYPE_FEC_ID:
    str = "fec_id";
  break;
  case SOC_PPC_MTR_TYPE_IN_AC_AND_TC:
    str = "in_ac_and_tc";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_MTR_MODE_to_string(
    SOC_SAND_IN  bcm_policer_mode_t enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case bcmPolicerModeSrTcm:
    str = "bcmPolicerModeSrTcm";
  break;
  case bcmPolicerModeCommitted:
    str = "bcmPolicerModeCommitted";
  break;
  case bcmPolicerModePeak:
    str = "bcmPolicerModePeak";
  break;
  case bcmPolicerModeTrTcm:
    str = "bcmPolicerModeTrTcm";
  break;
  case bcmPolicerModeTrTcmDs:
    str = "bcmPolicerModeTrTcmDs";
  break;
  case bcmPolicerModeGreen:
    str = "bcmPolicerModeGreen";
  break;
  case bcmPolicerModePassThrough:
    str = "bcmPolicerModePassThrough";
  break;
  case bcmPolicerModeSrTcmModified:
    str = "bcmPolicerModeSrTcmModified";
  break;
  case bcmPolicerModeCoupledTrTcmDs:
    str = "bcmPolicerModeCoupledTrTcmDs";
  break;
  case bcmPolicerModeCascade:
    str = "bcmPolicerModeCascade";
  break;
  case bcmPolicerModeCoupledCascade:
    str = "bcmPolicerModeCoupledCascade";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_PPC_MTR_METER_ID_print(
    SOC_SAND_IN  SOC_PPC_MTR_METER_ID *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if(info->id == SOC_PPC_MTR_ASSIGN_NO_METER)
  {
    LOG_CLI((BSL_META_U(unit,
                        "No Meter is associated \n\r")));
  }
  else
  {
    LOG_CLI((BSL_META_U(unit,
                        "group: %u\n\r"),info->group));
    LOG_CLI((BSL_META_U(unit,
                        "id: %u\n\r"),info->id));
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_MTR_GROUP_INFO_print(
    SOC_SAND_IN  SOC_PPC_MTR_GROUP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "is_hr_enabled: %u\n\r"),info->is_hr_enabled));
  LOG_CLI((BSL_META_U(unit,
                      "max_packet_size: %u[Bytes]\n\r"),info->max_packet_size));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_MTR_BW_PROFILE_INFO_print(
    SOC_SAND_IN  SOC_PPC_MTR_BW_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "disable_cir: %u\n\r"),info->disable_cir));
  LOG_CLI((BSL_META_U(unit,
                      "disable_eir: %u\n\r"),info->disable_eir));
  LOG_CLI((BSL_META_U(unit,
                      "cir: %u\n\r"),info->cir));
  LOG_CLI((BSL_META_U(unit,
                      "eir: %u\n\r"),info->eir));
  LOG_CLI((BSL_META_U(unit,
                      "cbs: %u\n\r"),info->cbs));
  LOG_CLI((BSL_META_U(unit,
                      "ebs: %u\n\r"),info->ebs));
  LOG_CLI((BSL_META_U(unit,
                      "color_mode %s \n\r"), SOC_PPC_MTR_COLOR_MODE_to_string(info->color_mode)));
  LOG_CLI((BSL_META_U(unit,
                      "is_coupling_enabled: %u\n\r"),info->is_coupling_enabled));
  LOG_CLI((BSL_META_U(unit,
                      "is_mtr_enabled: %u\n\r"),info->is_mtr_enabled));
  LOG_CLI((BSL_META_U(unit,
                      "is_packet_mode: %u\n\r"),info->is_packet_mode));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

