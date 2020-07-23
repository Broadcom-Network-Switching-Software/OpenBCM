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

#include <soc/dpp/PPC/ppc_api_general.h>
#include <soc/dpp/PPC/ppc_api_oam.h>





















void
  SOC_PPC_OAM_ETH_MP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_OAM_ETH_MP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_OAM_ETH_MP_INFO));
  info->acc_mep_id = 0;
  info->func_type = 0;
  info->valid = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_OAM_ETH_ACC_FUNC_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_OAM_ETH_ACC_FUNC_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_OAM_ETH_ACC_FUNC_TYPE_0:
    str = "func type 0";
  break;
  case SOC_PPC_OAM_ETH_ACC_FUNC_TYPE_1:
    str = "func type 1";
  break;
  case SOC_PPC_OAM_ETH_ACC_FUNC_TYPE_2:
    str = "func type 2";
  break;
  case SOC_PPC_OAM_ETH_ACC_FUNC_TYPE_3:
    str = "func type 3";
  break;
  case SOC_PPC_OAM_ETH_ACC_FUNC_TYPE_4:
    str = "func type 4";
  break;
  case SOC_PPC_OAM_ETH_ACC_FUNC_TYPE_5:
    str = "func type 5";
  break;
  default:
    str = " Unknown Enumerator Value";
  }
  return str;
}

const char*
  SOC_PPC_OAM_ETH_MP_LEVEL_to_string(
    SOC_SAND_IN  SOC_PPC_OAM_ETH_MP_LEVEL enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_OAM_ETH_MP_LEVEL_0:
    str = "level 0";
  break;
  case SOC_PPC_OAM_ETH_MP_LEVEL_1:
    str = "level 1";
  break;
  case SOC_PPC_OAM_ETH_MP_LEVEL_2:
    str = "level 2";
  break;
  case SOC_PPC_OAM_ETH_MP_LEVEL_3:
    str = "level 3";
  break;
  case SOC_PPC_OAM_ETH_MP_LEVEL_4:
    str = "level 4";
  break;
  case SOC_PPC_OAM_ETH_MP_LEVEL_5:
    str = "level 5";
  break;
  case SOC_PPC_OAM_ETH_MP_LEVEL_6:
    str = "level 5";
  break;
  case SOC_PPC_OAM_ETH_MP_LEVEL_7:
    str = "level 5";
  break;
  default:
    str = " Unknown Enumerator Value";
  }
  return str;
}

void
  SOC_PPC_OAM_ETH_MP_INFO_print(
    SOC_SAND_IN  SOC_PPC_OAM_ETH_MP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "valid: %u\n\r"),info->valid));
  LOG_CLI((BSL_META_U(unit,
                      "acc_mep_id: %u\n\r"),info->acc_mep_id));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 


void
  SOC_PPC_OAM_INIT_TRAP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_OAM_INIT_TRAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_OAM_INIT_TRAP_INFO));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_OAM_MEP_PROFILE_DATA_clear(
    SOC_SAND_OUT SOC_PPC_OAM_MEP_PROFILE_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_OAM_MEP_PROFILE_DATA));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_OAM_LIF_PROFILE_DATA_clear(
    SOC_SAND_OUT SOC_PPC_OAM_LIF_PROFILE_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_OAM_LIF_PROFILE_DATA));
  SOC_SAND_MAGIC_NUM_SET;
  SOC_PPC_OAM_MEP_PROFILE_DATA_clear(&info->mep_profile_data);
  SOC_PPC_OAM_MEP_PROFILE_DATA_clear(&info->mip_profile_data);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_OAM_RMEP_INFO_DATA_clear(
    SOC_SAND_OUT SOC_PPC_OAM_RMEP_INFO_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_OAM_RMEP_INFO_DATA));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_OAM_OAMP_RMEP_DB_EXT_ENTRY_clear(
    SOC_SAND_OUT SOC_PPC_OAM_OAMP_RMEP_DB_EXT_ENTRY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_OAM_OAMP_RMEP_DB_EXT_ENTRY));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_OAM_OAMP_RMEP_DB_EXT_ENTRY_print(
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_RMEP_DB_EXT_ENTRY *rmep_ext_entry
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(rmep_ext_entry);

  LOG_CLI((BSL_META_U(unit,
                      "last_prd_pkt_cnt_1731: %u\n\r"),rmep_ext_entry->last_prd_pkt_cnt_1731));
  LOG_CLI((BSL_META_U(unit,
                      "last_prd_pkt_cnt_1711: %u\n\r"),rmep_ext_entry->last_prd_pkt_cnt_1711));
  LOG_CLI((BSL_META_U(unit,
                      "rx_err: %u\n\r"),rmep_ext_entry->rx_err));
  LOG_CLI((BSL_META_U(unit,
                      "loc: %u\n\r"),rmep_ext_entry->loc));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_OAM_OAMP_SD_SF_DB_ENTRY_clear(
    SOC_SAND_OUT SOC_PPC_OAM_OAMP_SD_SF_DB_ENTRY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_OAM_OAMP_SD_SF_DB_ENTRY));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_OAM_OAMP_SD_SF_DB_ENTRY_print(
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_SD_SF_DB_ENTRY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "entry_format: %u\n\r"),info->entry_format));
  LOG_CLI((BSL_META_U(unit,
                      "sum_cnt: %u\n\r"),info->sum_cnt));
  LOG_CLI((BSL_META_U(unit,
                      "thresh_profile: %u\n\r"),info->thresh_profile));
  LOG_CLI((BSL_META_U(unit,
                      "sf: %u\n\r"),info->sf));
  LOG_CLI((BSL_META_U(unit,
                      "sd: %u\n\r"),info->sd));
  LOG_CLI((BSL_META_U(unit,
                      "rmep_db_ptr: %u\n\r"),info->rmep_db_ptr));
  LOG_CLI((BSL_META_U(unit,
                      "ccm_tx_rate: %u\n\r"),info->ccm_tx_rate));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_OAM_OAMP_SD_SF_Y_1711_DB_ENTRY_clear(
    SOC_SAND_OUT SOC_PPC_OAM_OAMP_SD_SF_Y_1711_DB_ENTRY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_OAM_OAMP_SD_SF_Y_1711_DB_ENTRY));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_OAM_OAMP_SD_SF_Y_1711_DB_ENTRY_print(
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_SD_SF_Y_1711_DB_ENTRY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "prd_pkt_cnt_0: %u\n\r"),info->prd_pkt_cnt_0));
  LOG_CLI((BSL_META_U(unit,
                      "prd_pkt_cnt_1: %u\n\r"),info->prd_pkt_cnt_1));
  LOG_CLI((BSL_META_U(unit,
                      "prd_pkt_cnt_2: %u\n\r"),info->prd_pkt_cnt_2));
  LOG_CLI((BSL_META_U(unit,
                      "prd_err_ind_0: %u\n\r"),info->prd_err_ind_0));
  LOG_CLI((BSL_META_U(unit,
                      "prd_err_ind_1: %u\n\r"),info->prd_err_ind_1));
  LOG_CLI((BSL_META_U(unit,
                      "prd_err_ind_2: %u\n\r"),info->prd_err_ind_2));
  LOG_CLI((BSL_META_U(unit,
                      "d_excess: %u\n\r"),info->d_excess));
  LOG_CLI((BSL_META_U(unit,
                      "d_mismerge: %u\n\r"),info->d_mismerge));
LOG_CLI((BSL_META_U(unit,
                    "d_mismatch: %u\n\r"),info->d_mismatch));
LOG_CLI((BSL_META_U(unit,
                    "rmep_db_ptr: %u\n\r"),info->rmep_db_ptr));
LOG_CLI((BSL_META_U(unit,
                    "ccm_tx_rate: %u\n\r"),info->ccm_tx_rate));
LOG_CLI((BSL_META_U(unit,
                    "allert_method: %u\n\r"),info->allert_method));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_OAM_OAMP_SD_SF_PROFILE_DB_ENTRY_clear(
    SOC_SAND_OUT SOC_PPC_OAM_OAMP_SD_SF_PROFILE_DB *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_OAM_OAMP_SD_SF_PROFILE_DB));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_OAM_OAMP_SD_SF_PROFILE_DB_ENTRY_print(
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_SD_SF_PROFILE_DB *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "wnd_lngth: %u\n\r"),info->wnd_lngth));
  LOG_CLI((BSL_META_U(unit,
                      "sd_set_thresh: %u\n\r"),info->sd_set_thresh));
  LOG_CLI((BSL_META_U(unit,
                      "sd_clr_thresh: %u\n\r"),info->sd_clr_thresh));
  LOG_CLI((BSL_META_U(unit,
                      "sf_set_thresh: %u\n\r"),info->sf_set_thresh));
  LOG_CLI((BSL_META_U(unit,
                      "sf_clr_thresh: %u\n\r"),info->sf_clr_thresh));
  LOG_CLI((BSL_META_U(unit,
                      "alert_method: %u\n\r"),info->alert_method));
  LOG_CLI((BSL_META_U(unit,
                      "supress_alerts: %u\n\r"),info->supress_alerts));


exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void
  SOC_PPC_OAM_OAMP_RMEP_DB_ENTRY_clear(
    SOC_SAND_OUT SOC_PPC_OAM_OAMP_RMEP_DB_ENTRY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_OAM_OAMP_RMEP_DB_ENTRY));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_OAM_OAMP_RMEP_DB_ENTRY_print(
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_RMEP_DB_ENTRY *rmep_entry
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(rmep_entry);

  LOG_CLI((BSL_META_U(unit,
                      "ccm_period: %u\n\r"),rmep_entry->ccm_period));
  LOG_CLI((BSL_META_U(unit,
                      "is_event_mask: %u\n\r"),rmep_entry->is_event_mask));
  LOG_CLI((BSL_META_U(unit,
                      "is_state_auto_handle: %u\n\r"),rmep_entry->is_state_auto_handle));
  LOG_CLI((BSL_META_U(unit,
                      "loc_clear_threshold: %u\n\r"),rmep_entry->loc_clear_threshold));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_OAM_OAMP_MEP_DB_ENTRY_clear(
    SOC_SAND_OUT SOC_PPC_OAM_OAMP_MEP_DB_ENTRY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_OAM_OAMP_MEP_DB_ENTRY));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_OAM_OAMP_MEP_DB_ENTRY_print(
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_MEP_DB_ENTRY *mep_entry
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(mep_entry);

  LOG_CLI((BSL_META_U(unit,
                      "src_mac_address: ")));
  soc_sand_SAND_PP_MAC_ADDRESS_print(&mep_entry->src_mac_address);
  LOG_CLI((BSL_META_U(unit,
                      "mdl: %u\n\r"),mep_entry->mdl));
  LOG_CLI((BSL_META_U(unit,
                      "icc_ndx: %u\n\r"),mep_entry->icc_ndx));
  LOG_CLI((BSL_META_U(unit,
                      "ccm_interval: %u\n\r"),mep_entry->ccm_interval));
  LOG_CLI((BSL_META_U(unit,
                      "rdi: %u\n\r"),mep_entry->rdi));
  LOG_CLI((BSL_META_U(unit,
                      "is_upmep: %u\n\r"),mep_entry->is_upmep));
  LOG_CLI((BSL_META_U(unit,
                      "inner_tpid: %u\n\r"),mep_entry->inner_tpid));
  LOG_CLI((BSL_META_U(unit,
                      "inner_vid_dei_pcp: %u\n\r"),mep_entry->inner_vid_dei_pcp));
  LOG_CLI((BSL_META_U(unit,
                      "outer_tpid: %u\n\r"),mep_entry->outer_tpid));
  LOG_CLI((BSL_META_U(unit,
                      "outer_vid_dei_pcp: %u\n\r"),mep_entry->outer_vid_dei_pcp));
  LOG_CLI((BSL_META_U(unit,
                      "priority: %u\n\r"),mep_entry->priority));
  LOG_CLI((BSL_META_U(unit,
                      "system_port: %u\n\r"),mep_entry->system_port));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_clear(
    SOC_SAND_OUT SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY));
  info->remote_gport = _SHR_GPORT_INVALID;

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_OAM_SAT_CTF_ENTRY_clear(
    SOC_SAND_OUT SOC_PPC_OAM_SAT_CTF_ENTRY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_OAM_SAT_CTF_ENTRY));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_OAM_SAT_GTF_ENTRY_clear(
    SOC_SAND_OUT SOC_PPC_OAM_SAT_GTF_ENTRY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_OAM_SAT_GTF_ENTRY));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_OAM_OAMP_LM_DM_MEP_DB_ENTRY_clear(
    SOC_SAND_OUT SOC_PPC_OAM_OAMP_LM_DM_MEP_DB_ENTRY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_OAM_OAMP_LM_DM_MEP_DB_ENTRY));
  SOC_SAND_MAGIC_NUM_SET;

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_OAM_OAMP_DM_INFO_GET_clear(
    SOC_SAND_OUT SOC_PPC_OAM_OAMP_DM_INFO_GET *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_OAM_OAMP_DM_INFO_GET));
  SOC_SAND_MAGIC_NUM_SET;

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_PPC_OAM_OAMP_LM_INFO_GET_clear(
    SOC_SAND_OUT SOC_PPC_OAM_OAMP_LM_INFO_GET *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_OAM_OAMP_LM_INFO_GET));
  SOC_SAND_MAGIC_NUM_SET;

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY_clear(
    SOC_SAND_OUT SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_print(
    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY *classifier_entry
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(classifier_entry);

  LOG_CLI((BSL_META_U(unit,
                      "dst_mac_address: ")));
  soc_sand_SAND_PP_MAC_ADDRESS_print(&classifier_entry->dst_mac_address);
  LOG_CLI((BSL_META_U(unit,
                      "\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      "lif: %u\n\r"),classifier_entry->lif));
  LOG_CLI((BSL_META_U(unit,
                      "port: %u\n\r"),classifier_entry->port));
  LOG_CLI((BSL_META_U(unit,
                      "md_level: %u\n\r"),classifier_entry->md_level));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD_clear(
    SOC_SAND_OUT SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD_print(
    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD *oem1_entry_payload
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(oem1_entry_payload);

  LOG_CLI((BSL_META_U(unit,
                      "mep_bitmap: %u\n\r"),oem1_entry_payload->mep_bitmap));
  LOG_CLI((BSL_META_U(unit,
                      "mip_bitmap: %u\n\r"),oem1_entry_payload->mip_bitmap));
  LOG_CLI((BSL_META_U(unit,
                      "counter_ndx: %u\n\r"),oem1_entry_payload->counter_ndx));
  LOG_CLI((BSL_META_U(unit,
                      "mp_profile: %u\n\r"),oem1_entry_payload->mp_profile));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD_clear(
    SOC_SAND_OUT SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD_print(
    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD *oem2_entry_payload
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(oem2_entry_payload);

  LOG_CLI((BSL_META_U(unit,
                      "oam_id: %u\n\r"),oem2_entry_payload->oam_id));
  LOG_CLI((BSL_META_U(unit,
                      "mp_profile: %u\n\r"),oem2_entry_payload->mp_profile));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_KEY_clear(
    SOC_SAND_OUT SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_KEY));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_KEY_clear(
    SOC_SAND_OUT SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_KEY));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



void
  SOC_PPC_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD_clear(
    SOC_SAND_OUT SOC_PPC_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD_print(
    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD *oam_entry_payload
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(oam_entry_payload);
  LOG_CLI((BSL_META_U(unit,
                      "snoop_strength: %u\n\r"),oam_entry_payload->snoop_strength));
  LOG_CLI((BSL_META_U(unit,
                      "forward_disable: %u\n\r"),oam_entry_payload->forward_disable));
  LOG_CLI((BSL_META_U(unit,
                      "mirror_profile: %u\n\r"),oam_entry_payload->mirror_profile));
  LOG_CLI((BSL_META_U(unit,
                      "cpu_trap_code: %u\n\r"),oam_entry_payload->cpu_trap_code));
  LOG_CLI((BSL_META_U(unit,
                      "up_map: %u\n\r"),oam_entry_payload->up_map));
  LOG_CLI((BSL_META_U(unit,
                      "sub_type: %u\n\r"),oam_entry_payload->sub_type));
  LOG_CLI((BSL_META_U(unit,
                      "meter_disable: %u\n\r"),oam_entry_payload->meter_disable));
  LOG_CLI((BSL_META_U(unit,
                      "counter_disable: %u\n\r"),oam_entry_payload->counter_disable));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_OAM_CLASSIFIER_OAM1_ENTRY_KEY_clear(
    SOC_SAND_OUT SOC_PPC_OAM_CLASSIFIER_OAM1_ENTRY_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_OAM_CLASSIFIER_OAM1_ENTRY_KEY));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_OAM_CLASSIFIER_OAM2_ENTRY_KEY_clear(
    SOC_SAND_OUT SOC_PPC_OAM_CLASSIFIER_OAM2_ENTRY_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_OAM_CLASSIFIER_OAM2_ENTRY_KEY));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_OAM_TCAM_ENTRY_KEY_clear(
    SOC_SAND_OUT SOC_PPC_OAM_TCAM_ENTRY_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_OAM_TCAM_ENTRY_KEY));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_OAM_TCAM_ENTRY_ACTION_clear(
    SOC_SAND_OUT SOC_PPC_OAM_TCAM_ENTRY_ACTION *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_OAM_TCAM_ENTRY_ACTION));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



void
  SOC_PPC_BFD_INIT_TRAP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_BFD_INIT_TRAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_BFD_INIT_TRAP_INFO));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_BFD_PDU_STATIC_REGISTER_clear(
    SOC_SAND_OUT SOC_PPC_BFD_PDU_STATIC_REGISTER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_BFD_PDU_STATIC_REGISTER));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_BFD_CC_PACKET_STATIC_REGISTER_clear(
    SOC_SAND_OUT SOC_PPC_BFD_CC_PACKET_STATIC_REGISTER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_BFD_CC_PACKET_STATIC_REGISTER));
  SOC_SAND_MAGIC_NUM_SET;
  SOC_PPC_BFD_PDU_STATIC_REGISTER_clear(&(info->bfd_static_reg_fields)); 

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_MPLS_PWE_PROFILE_DATA_clear(
    SOC_SAND_OUT SOC_PPC_MPLS_PWE_PROFILE_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_MPLS_PWE_PROFILE_DATA));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_BFD_IP_MULTI_HOP_TOS_TTL_DATA_clear(
    SOC_SAND_OUT SOC_PPC_BFD_IP_MULTI_HOP_TOS_TTL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_BFD_IP_MULTI_HOP_TOS_TTL_DATA));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES_clear(
    SOC_SAND_OUT SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_OAM_OAMP_PUNT_PROFILE_DATA_clear(
    SOC_SAND_OUT SOC_PPC_OAM_OAMP_PUNT_PROFILE_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_OAM_OAMP_PUNT_PROFILE_DATA));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_OAM_EVENT_DATA_clear(
    SOC_SAND_OUT SOC_PPC_OAM_EVENT_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_OAM_EVENT_DATA));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_OAM_RX_REPORT_EVENT_DATA_clear(
    SOC_SAND_OUT SOC_PPC_OAM_RX_REPORT_EVENT_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_OAM_RX_REPORT_EVENT_DATA));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


#include <soc/dpp/SAND/Utils/sand_footer.h>

