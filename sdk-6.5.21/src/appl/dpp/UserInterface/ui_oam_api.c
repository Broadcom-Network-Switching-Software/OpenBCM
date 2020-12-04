/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/*
 * General include file for reference design.
 */
#include <appl/diag/dpp/ref_sys.h>
#include <bcm_app/dpp/../H/usrApp.h>

/*
 * Utilities include file.
 */

#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <appl/diag/dpp/utils_defi.h>
#include <appl/dpp/UserInterface/ui_defi.h>

#include <appl/dpp/UserInterface/ui_pure_defi_oam_api.h>

#include <soc/dpp/oam/oam_api_general.h>
#include <soc/dpp/oam/oam_api_eth.h>
#include <soc/dpp/oam/oam_api_bfd.h>
#include <soc/dpp/oam/oam_api_mpls.h>
#include <soc/dpp/oam/oam_framework.h>

static uint32
  Default_unit = 0;

void
  oam_set_default_unit(uint32 dev_id)
{
  Default_unit = dev_id;
}

uint32
  oam_get_default_unit()
{
  return Default_unit;
}

#ifdef UI_GENERAL
/********************************************************************
 *  Function handler: info_set (section general)
 */
int
  ui_oam_api_general_info_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  OAM_GENERAL_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_oam_api_general");
  soc_sand_proc_name = "oam_general_info_set";

  unit = oam_get_default_unit();
  OAM_GENERAL_INFO_clear(&prm_info);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = oam_general_info_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_general_info_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_general_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_GENERAL_INFO_SET_INFO_SET_INFO_CPU_DP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_GENERAL_INFO_SET_INFO_SET_INFO_CPU_DP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.cpu_dp = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_GENERAL_INFO_SET_INFO_SET_INFO_CPU_TC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_GENERAL_INFO_SET_INFO_SET_INFO_CPU_TC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.cpu_tc = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_GENERAL_INFO_SET_INFO_SET_INFO_CPU_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_GENERAL_INFO_SET_INFO_SET_INFO_CPU_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.cpu_port = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_GENERAL_INFO_SET_INFO_SET_INFO_SOURCE_SYS_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_GENERAL_INFO_SET_INFO_SET_INFO_SOURCE_SYS_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.source_sys_port = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = oam_general_info_set(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_general_info_set - FAIL", TRUE);
    oam_disp_result(ret, "oam_general_info_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: info_get (section general)
 */
int
  ui_oam_api_general_info_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  OAM_GENERAL_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_oam_api_general");
  soc_sand_proc_name = "oam_general_info_get";

  unit = oam_get_default_unit();
  OAM_GENERAL_INFO_clear(&prm_info);

  /* Get parameters */

  /* Call function */
  ret = oam_general_info_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_general_info_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_general_info_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  OAM_GENERAL_INFO_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: msg_info_get (section general)
 */
int
  ui_oam_api_general_msg_info_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  OAM_MSG_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_oam_api_general");
  soc_sand_proc_name = "oam_msg_info_get";

  unit = oam_get_default_unit();
  OAM_MSG_INFO_clear(&prm_info);

  /* Get parameters */

  /* Call function */
  ret = oam_msg_info_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_msg_info_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_msg_info_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  OAM_MSG_INFO_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

#endif
#ifdef UI_ETH
/********************************************************************
 *  Function handler: global_info_set (section eth)
 */
int
  ui_oam_api_eth_global_info_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  OAM_ETH_GLOBAL_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_oam_api_eth");
  soc_sand_proc_name = "oam_eth_global_info_set";

  unit = oam_get_default_unit();
  OAM_ETH_GLOBAL_INFO_clear(&prm_info);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = oam_eth_global_info_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_eth_global_info_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_eth_global_info_get");
    goto exit;
  }

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_OAM_ETH_GLOBAL_INFO_SET_GLOBAL_INFO_SET_INFO_UP_MEP_MAC_ID,1,
         &param_val,VAL_TEXT,err_msg))
  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_info.up_mep_mac));
  }
  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_OAM_ETH_GLOBAL_INFO_SET_GLOBAL_INFO_SET_INFO_DN_MEP_MAC_ID,1,
         &param_val,VAL_TEXT,err_msg))
  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_info.dn_mep_mac));
  }

  /* Call function */
  ret = oam_eth_global_info_set(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_eth_global_info_set - FAIL", TRUE);
    oam_disp_result(ret, "oam_eth_global_info_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: global_info_get (section eth)
 */
int
  ui_oam_api_eth_global_info_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  OAM_ETH_GLOBAL_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_oam_api_eth");
  soc_sand_proc_name = "oam_eth_global_info_get";

  unit = oam_get_default_unit();
  OAM_ETH_GLOBAL_INFO_clear(&prm_info);

  /* Get parameters */

  /* Call function */
  ret = oam_eth_global_info_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_eth_global_info_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_eth_global_info_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  OAM_ETH_GLOBAL_INFO_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: cos_mapping_info_set (section eth)
 */
int
  ui_oam_api_eth_cos_mapping_info_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  OAM_ETH_COS_ID
    prm_cos_ndx;
  OAM_ETH_COS_MAPPING_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_oam_api_eth");
  soc_sand_proc_name = "oam_eth_cos_mapping_info_set";

  unit = oam_get_default_unit();
  OAM_ETH_COS_MAPPING_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_COS_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_ETH_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_COS_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_cos_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter cos_ndx after cos_mapping_info_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = oam_eth_cos_mapping_info_get(
          unit,
          prm_cos_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_eth_cos_mapping_info_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_eth_cos_mapping_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_INFO_UP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_ETH_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_INFO_UP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.up = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_INFO_DP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_ETH_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_INFO_DP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.dp = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_INFO_TC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_ETH_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_INFO_TC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.tc = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = oam_eth_cos_mapping_info_set(
          unit,
          prm_cos_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_eth_cos_mapping_info_set - FAIL", TRUE);
    oam_disp_result(ret, "oam_eth_cos_mapping_info_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: cos_mapping_info_get (section eth)
 */
int
  ui_oam_api_eth_cos_mapping_info_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  OAM_ETH_COS_ID
    prm_cos_ndx;
  OAM_ETH_COS_MAPPING_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_oam_api_eth");
  soc_sand_proc_name = "oam_eth_cos_mapping_info_get";

  unit = oam_get_default_unit();
  OAM_ETH_COS_MAPPING_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_COS_MAPPING_INFO_GET_COS_MAPPING_INFO_GET_COS_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_ETH_COS_MAPPING_INFO_GET_COS_MAPPING_INFO_GET_COS_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_cos_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter cos_ndx after cos_mapping_info_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = oam_eth_cos_mapping_info_get(
          unit,
          prm_cos_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_eth_cos_mapping_info_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_eth_cos_mapping_info_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  OAM_ETH_COS_MAPPING_INFO_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: acc_mep_info_set (section eth)
 */
int
  ui_oam_api_eth_acc_mep_info_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  OAM_ETH_ACC_MEP_ID
    prm_acc_mep_ndx;
  OAM_ETH_ACC_MEP_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_oam_api_eth");
  soc_sand_proc_name = "oam_eth_acc_mep_info_set";

  unit = oam_get_default_unit();
  OAM_ETH_ACC_MEP_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_ACC_MEP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_ACC_MEP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_acc_mep_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter acc_mep_ndx after acc_mep_info_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = oam_eth_acc_mep_info_get(
          unit,
          prm_acc_mep_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_eth_acc_mep_info_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_eth_acc_mep_info_get");
    goto exit;
  }

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FUNC_INFO_TST_INFO_DA_ID,1,
         &param_val,VAL_TEXT,err_msg))
  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_info.func_info.tst_info.da));
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FUNC_INFO_TST_INFO_NOF_PACKETS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FUNC_INFO_TST_INFO_NOF_PACKETS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.func_info.tst_info.nof_packets = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FUNC_INFO_TST_INFO_PACKET_SIZE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FUNC_INFO_TST_INFO_PACKET_SIZE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.func_info.tst_info.packet_size = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FUNC_INFO_TST_INFO_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FUNC_INFO_TST_INFO_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.func_info.tst_info.rate = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FUNC_INFO_TST_INFO_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FUNC_INFO_TST_INFO_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.func_info.tst_info.valid = (uint8)param_val->value.ulong_value;
  }

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FUNC_INFO_DLM_INFO_DA_ID,1,
         &param_val,VAL_TEXT,err_msg))
  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_info.func_info.dlm_info.da));
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FUNC_INFO_DLM_INFO_INTERVAL_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FUNC_INFO_DLM_INFO_INTERVAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.func_info.dlm_info.interval = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FUNC_INFO_DLM_INFO_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FUNC_INFO_DLM_INFO_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.func_info.dlm_info.valid = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FUNC_INFO_CCM_INFO_LM_EN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FUNC_INFO_CCM_INFO_LM_EN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.func_info.ccm_info.lm_en = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FUNC_INFO_CCM_INFO_CCM_INTERVAL_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FUNC_INFO_CCM_INFO_CCM_INTERVAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.func_info.ccm_info.ccm_interval = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FUNC_INFO_CCM_INFO_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FUNC_INFO_CCM_INFO_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.func_info.ccm_info.valid = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FRWD_INFO_DOWN_COS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FRWD_INFO_DOWN_COS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.frwd_info.down.cos = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FRWD_INFO_DOWN_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FRWD_INFO_DOWN_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.frwd_info.down.vid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FRWD_INFO_DOWN_SOURCE_PP_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FRWD_INFO_DOWN_SOURCE_PP_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.frwd_info.down.source_pp_port = (SOC_SAND_PP_LOCAL_PORT_ID)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FRWD_INFO_UP_COS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FRWD_INFO_UP_COS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.frwd_info.up.cos = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FRWD_INFO_UP_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FRWD_INFO_UP_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.frwd_info.up.vid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FRWD_INFO_UP_SYS_DEST_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FRWD_INFO_UP_SYS_DEST_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.frwd_info.up.sys_dest_port = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FRWD_INFO_UP_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FRWD_INFO_UP_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.frwd_info.up.valid = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_DIRECTION_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_DIRECTION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.direction = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_MD_LEVEL_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_MD_LEVEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.md_level = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_MEP_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_MEP_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.mep_id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_MA_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_MA_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.ma_id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.valid = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = oam_eth_acc_mep_info_set(
          unit,
          prm_acc_mep_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_eth_acc_mep_info_set - FAIL", TRUE);
    oam_disp_result(ret, "oam_eth_acc_mep_info_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: acc_mep_info_get (section eth)
 */
int
  ui_oam_api_eth_acc_mep_info_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  OAM_ETH_ACC_MEP_ID
    prm_acc_mep_ndx;
  OAM_ETH_ACC_MEP_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_oam_api_eth");
  soc_sand_proc_name = "oam_eth_acc_mep_info_get";

  unit = oam_get_default_unit();
  OAM_ETH_ACC_MEP_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_ACC_MEP_INFO_GET_ACC_MEP_INFO_GET_ACC_MEP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_ETH_ACC_MEP_INFO_GET_ACC_MEP_INFO_GET_ACC_MEP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_acc_mep_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter acc_mep_ndx after acc_mep_info_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = oam_eth_acc_mep_info_get(
          unit,
          prm_acc_mep_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_eth_acc_mep_info_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_eth_acc_mep_info_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  OAM_ETH_ACC_MEP_INFO_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: acc_mep_tx_rdi_set (section eth)
 */
int
  ui_oam_api_eth_acc_mep_tx_rdi_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  OAM_ETH_ACC_MEP_ID
    prm_acc_mep_ndx;
  uint8
    prm_rdi;

  UI_MACROS_INIT_FUNCTION("ui_oam_api_eth");
  soc_sand_proc_name = "oam_eth_acc_mep_tx_rdi_set";

  unit = oam_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_ACC_MEP_TX_RDI_SET_ACC_MEP_TX_RDI_SET_ACC_MEP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_ETH_ACC_MEP_TX_RDI_SET_ACC_MEP_TX_RDI_SET_ACC_MEP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_acc_mep_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter acc_mep_ndx after acc_mep_tx_rdi_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = oam_eth_acc_mep_tx_rdi_get(
          unit,
          prm_acc_mep_ndx,
          &prm_rdi
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_eth_acc_mep_tx_rdi_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_eth_acc_mep_tx_rdi_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_ACC_MEP_TX_RDI_SET_ACC_MEP_TX_RDI_SET_RDI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_ETH_ACC_MEP_TX_RDI_SET_ACC_MEP_TX_RDI_SET_RDI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_rdi = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = oam_eth_acc_mep_tx_rdi_set(
          unit,
          prm_acc_mep_ndx,
          prm_rdi
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_eth_acc_mep_tx_rdi_set - FAIL", TRUE);
    oam_disp_result(ret, "oam_eth_acc_mep_tx_rdi_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: acc_mep_tx_rdi_get (section eth)
 */
int
  ui_oam_api_eth_acc_mep_tx_rdi_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  OAM_ETH_ACC_MEP_ID
    prm_acc_mep_ndx;
  uint8
    prm_rdi;

  UI_MACROS_INIT_FUNCTION("ui_oam_api_eth");
  soc_sand_proc_name = "oam_eth_acc_mep_tx_rdi_get";

  unit = oam_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_ACC_MEP_TX_RDI_GET_ACC_MEP_TX_RDI_GET_ACC_MEP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_ETH_ACC_MEP_TX_RDI_GET_ACC_MEP_TX_RDI_GET_ACC_MEP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_acc_mep_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter acc_mep_ndx after acc_mep_tx_rdi_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = oam_eth_acc_mep_tx_rdi_get(
          unit,
          prm_acc_mep_ndx,
          &prm_rdi
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_eth_acc_mep_tx_rdi_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_eth_acc_mep_tx_rdi_get");
    goto exit;
  }

  send_string_to_screen("--> rdi:", TRUE);
  soc_sand_os_printf( "rdi: %u\n",prm_rdi);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: remote_mep_set (section eth)
 */
int
  ui_oam_api_eth_remote_mep_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  OAM_ETH_ACC_MEP_ID
    prm_acc_mep_ndx;
  uint32
    prm_remote_mep_ndx;
  OAM_ETH_REMOTE_MEP_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_oam_api_eth");
  soc_sand_proc_name = "oam_eth_remote_mep_set";

  unit = oam_get_default_unit();
  OAM_ETH_REMOTE_MEP_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_REMOTE_MEP_SET_REMOTE_MEP_SET_ACC_MEP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_ETH_REMOTE_MEP_SET_REMOTE_MEP_SET_ACC_MEP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_acc_mep_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter acc_mep_ndx after remote_mep_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_REMOTE_MEP_SET_REMOTE_MEP_SET_REMOTE_MEP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_ETH_REMOTE_MEP_SET_REMOTE_MEP_SET_REMOTE_MEP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_remote_mep_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter remote_mep_ndx after remote_mep_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = oam_eth_remote_mep_get(
          unit,
          prm_acc_mep_ndx,
          prm_remote_mep_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_eth_remote_mep_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_eth_remote_mep_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_REMOTE_MEP_SET_REMOTE_MEP_SET_INFO_LOCAL_DEFECT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_ETH_REMOTE_MEP_SET_REMOTE_MEP_SET_INFO_LOCAL_DEFECT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.local_defect = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_REMOTE_MEP_SET_REMOTE_MEP_SET_INFO_RDI_RECEIVED_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_ETH_REMOTE_MEP_SET_REMOTE_MEP_SET_INFO_RDI_RECEIVED_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.rdi_received = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_REMOTE_MEP_SET_REMOTE_MEP_SET_INFO_CCM_INTERVAL_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_OAM_ETH_REMOTE_MEP_SET_REMOTE_MEP_SET_INFO_CCM_INTERVAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.ccm_interval = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_REMOTE_MEP_SET_REMOTE_MEP_SET_INFO_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_ETH_REMOTE_MEP_SET_REMOTE_MEP_SET_INFO_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.valid = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = oam_eth_remote_mep_set(
          unit,
          prm_acc_mep_ndx,
          prm_remote_mep_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_eth_remote_mep_set - FAIL", TRUE);
    oam_disp_result(ret, "oam_eth_remote_mep_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: remote_mep_get (section eth)
 */
int
  ui_oam_api_eth_remote_mep_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  OAM_ETH_ACC_MEP_ID
    prm_acc_mep_ndx;
  uint32
    prm_remote_mep_ndx;
  OAM_ETH_REMOTE_MEP_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_oam_api_eth");
  soc_sand_proc_name = "oam_eth_remote_mep_get";

  unit = oam_get_default_unit();
  OAM_ETH_REMOTE_MEP_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_REMOTE_MEP_GET_REMOTE_MEP_GET_ACC_MEP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_ETH_REMOTE_MEP_GET_REMOTE_MEP_GET_ACC_MEP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_acc_mep_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter acc_mep_ndx after remote_mep_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_REMOTE_MEP_GET_REMOTE_MEP_GET_REMOTE_MEP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_ETH_REMOTE_MEP_GET_REMOTE_MEP_GET_REMOTE_MEP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_remote_mep_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter remote_mep_ndx after remote_mep_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = oam_eth_remote_mep_get(
          unit,
          prm_acc_mep_ndx,
          prm_remote_mep_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_eth_remote_mep_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_eth_remote_mep_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  OAM_ETH_REMOTE_MEP_INFO_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mep_delay_measurement_get (section eth)
 */
int
  ui_oam_api_eth_mep_delay_measurement_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  OAM_ETH_ACC_MEP_ID
    prm_acc_mep_ndx;
  uint32
    prm_dm;

  UI_MACROS_INIT_FUNCTION("ui_oam_api_eth");
  soc_sand_proc_name = "oam_eth_mep_delay_measurement_get";

  unit = oam_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_MEP_DELAY_MEASUREMENT_GET_MEP_DELAY_MEASUREMENT_GET_ACC_MEP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_ETH_MEP_DELAY_MEASUREMENT_GET_MEP_DELAY_MEASUREMENT_GET_ACC_MEP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_acc_mep_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter acc_mep_ndx after mep_delay_measurement_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = oam_eth_mep_delay_measurement_get(
          unit,
          prm_acc_mep_ndx,
          &prm_dm
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_eth_mep_delay_measurement_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_eth_mep_delay_measurement_get");
    goto exit;
  }

  send_string_to_screen("--> dm:", TRUE);
  soc_sand_os_printf( "dm: %lu\n",prm_dm);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mep_loss_measurement_get (section eth)
 */
int
  ui_oam_api_eth_mep_loss_measurement_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  OAM_ETH_ACC_MEP_ID
    prm_acc_mep_ndx;
  OAM_ETH_ACC_MEP_LM_CNT_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_oam_api_eth");
  soc_sand_proc_name = "oam_eth_mep_loss_measurement_get";

  unit = oam_get_default_unit();
  OAM_ETH_ACC_MEP_LM_CNT_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_MEP_LOSS_MEASUREMENT_GET_MEP_LOSS_MEASUREMENT_GET_ACC_MEP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_ETH_MEP_LOSS_MEASUREMENT_GET_MEP_LOSS_MEASUREMENT_GET_ACC_MEP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_acc_mep_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter acc_mep_ndx after mep_loss_measurement_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = oam_eth_mep_loss_measurement_get(
          unit,
          prm_acc_mep_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_eth_mep_loss_measurement_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_eth_mep_loss_measurement_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  OAM_ETH_ACC_MEP_LM_CNT_INFO_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mep_tst_packet_counter_get (section eth)
 */
int
  ui_oam_api_eth_mep_tst_packet_counter_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_counter;

  UI_MACROS_INIT_FUNCTION("ui_oam_api_eth");
  soc_sand_proc_name = "oam_eth_mep_tst_packet_counter_get";

  unit = oam_get_default_unit();

  /* Get parameters */

  /* Call function */
  ret = oam_eth_mep_tst_packet_counter_get(
          unit,
          &prm_counter
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_eth_mep_tst_packet_counter_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_eth_mep_tst_packet_counter_get");
    goto exit;
  }

  send_string_to_screen("--> counter:", TRUE);
  soc_sand_os_printf( "counter: %lu\n",prm_counter);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mep_tst_packet_counter_clear (section eth)
 */
int
  ui_oam_api_eth_mep_tst_packet_counter_clear(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;

  UI_MACROS_INIT_FUNCTION("ui_oam_api_eth");
  soc_sand_proc_name = "oam_eth_mep_tst_packet_counter_clear";

  unit = oam_get_default_unit();

  /* Get parameters */

  /* Call function */
  ret = oam_eth_mep_tst_packet_counter_clear(
          unit
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_eth_mep_tst_packet_counter_clear - FAIL", TRUE);
    oam_disp_result(ret, "oam_eth_mep_tst_packet_counter_clear");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

#endif
#ifdef UI_BFD
/********************************************************************
 *  Function handler: cos_mapping_info_set (section bfd)
 */
int
  ui_oam_api_bfd_cos_mapping_info_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  OAM_BFD_COS_ID
    prm_cos_ndx;
  OAM_BFD_COS_MAPPING_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_oam_api_bfd");
  soc_sand_proc_name = "oam_bfd_cos_mapping_info_set";

  unit = oam_get_default_unit();
  OAM_BFD_COS_MAPPING_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_COS_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_BFD_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_COS_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_cos_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter cos_ndx after cos_mapping_info_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = oam_bfd_cos_mapping_info_get(
          unit,
          prm_cos_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_bfd_cos_mapping_info_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_bfd_cos_mapping_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_INFO_PWE_EXP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_BFD_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_INFO_PWE_EXP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.pwe_exp = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_INFO_TNL_EXP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_BFD_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_INFO_TNL_EXP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.tnl_exp = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_INFO_DP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_BFD_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_INFO_DP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.dp = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_INFO_TC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_BFD_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_INFO_TC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.tc = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = oam_bfd_cos_mapping_info_set(
          unit,
          prm_cos_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_bfd_cos_mapping_info_set - FAIL", TRUE);
    oam_disp_result(ret, "oam_bfd_cos_mapping_info_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: cos_mapping_info_get (section bfd)
 */
int
  ui_oam_api_bfd_cos_mapping_info_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  OAM_BFD_COS_ID
    prm_cos_ndx;
  OAM_BFD_COS_MAPPING_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_oam_api_bfd");
  soc_sand_proc_name = "oam_bfd_cos_mapping_info_get";

  unit = oam_get_default_unit();
  OAM_BFD_COS_MAPPING_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_COS_MAPPING_INFO_GET_COS_MAPPING_INFO_GET_COS_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_BFD_COS_MAPPING_INFO_GET_COS_MAPPING_INFO_GET_COS_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_cos_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter cos_ndx after cos_mapping_info_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = oam_bfd_cos_mapping_info_get(
          unit,
          prm_cos_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_bfd_cos_mapping_info_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_bfd_cos_mapping_info_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  OAM_BFD_COS_MAPPING_INFO_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ttl_mapping_info_set (section bfd)
 */
int
  ui_oam_api_bfd_ttl_mapping_info_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  OAM_BFD_TTL_ID
    prm_ttl_ndx;
  OAM_BFD_TTL_MAPPING_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_oam_api_bfd");
  soc_sand_proc_name = "oam_bfd_ttl_mapping_info_set";

  unit = oam_get_default_unit();
  OAM_BFD_TTL_MAPPING_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_TTL_MAPPING_INFO_SET_TTL_MAPPING_INFO_SET_TTL_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_BFD_TTL_MAPPING_INFO_SET_TTL_MAPPING_INFO_SET_TTL_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ttl_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ttl_ndx after ttl_mapping_info_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = oam_bfd_ttl_mapping_info_get(
          unit,
          prm_ttl_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_bfd_ttl_mapping_info_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_bfd_ttl_mapping_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_TTL_MAPPING_INFO_SET_TTL_MAPPING_INFO_SET_INFO_PWE_TTL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_BFD_TTL_MAPPING_INFO_SET_TTL_MAPPING_INFO_SET_INFO_PWE_TTL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.pwe_ttl = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_TTL_MAPPING_INFO_SET_TTL_MAPPING_INFO_SET_INFO_TNL_TTL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_BFD_TTL_MAPPING_INFO_SET_TTL_MAPPING_INFO_SET_INFO_TNL_TTL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.tnl_ttl = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = oam_bfd_ttl_mapping_info_set(
          unit,
          prm_ttl_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_bfd_ttl_mapping_info_set - FAIL", TRUE);
    oam_disp_result(ret, "oam_bfd_ttl_mapping_info_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ttl_mapping_info_get (section bfd)
 */
int
  ui_oam_api_bfd_ttl_mapping_info_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  OAM_BFD_TTL_ID
    prm_ttl_ndx;
  OAM_BFD_TTL_MAPPING_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_oam_api_bfd");
  soc_sand_proc_name = "oam_bfd_ttl_mapping_info_get";

  unit = oam_get_default_unit();
  OAM_BFD_TTL_MAPPING_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_TTL_MAPPING_INFO_GET_TTL_MAPPING_INFO_GET_TTL_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_BFD_TTL_MAPPING_INFO_GET_TTL_MAPPING_INFO_GET_TTL_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ttl_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ttl_ndx after ttl_mapping_info_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = oam_bfd_ttl_mapping_info_get(
          unit,
          prm_ttl_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_bfd_ttl_mapping_info_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_bfd_ttl_mapping_info_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  OAM_BFD_TTL_MAPPING_INFO_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ip_mapping_info_set (section bfd)
 */
int
  ui_oam_api_bfd_ip_mapping_info_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  OAM_BFD_IP_ID
    prm_ip_ndx;
  OAM_BFD_IP_MAPPING_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_oam_api_bfd");
  soc_sand_proc_name = "oam_bfd_ip_mapping_info_set";

  unit = oam_get_default_unit();
  OAM_BFD_IP_MAPPING_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_IP_MAPPING_INFO_SET_IP_MAPPING_INFO_SET_IP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_BFD_IP_MAPPING_INFO_SET_IP_MAPPING_INFO_SET_IP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ip_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ip_ndx after ip_mapping_info_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = oam_bfd_ip_mapping_info_get(
          unit,
          prm_ip_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_bfd_ip_mapping_info_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_bfd_ip_mapping_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_IP_MAPPING_INFO_SET_IP_MAPPING_INFO_SET_INFO_IP_ID,1))
  {
    UI_MACROS_GET_TEXT_VAL(PARAM_OAM_BFD_IP_MAPPING_INFO_SET_IP_MAPPING_INFO_SET_INFO_IP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    soc_sand_pp_ipv4_address_string_parse(param_val->value.val_text, &(prm_info.ip));
  }


  /* Call function */
  ret = oam_bfd_ip_mapping_info_set(
          unit,
          prm_ip_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_bfd_ip_mapping_info_set - FAIL", TRUE);
    oam_disp_result(ret, "oam_bfd_ip_mapping_info_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ip_mapping_info_get (section bfd)
 */
int
  ui_oam_api_bfd_ip_mapping_info_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  OAM_BFD_IP_ID
    prm_ip_ndx;
  OAM_BFD_IP_MAPPING_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_oam_api_bfd");
  soc_sand_proc_name = "oam_bfd_ip_mapping_info_get";

  unit = oam_get_default_unit();
  OAM_BFD_IP_MAPPING_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_IP_MAPPING_INFO_GET_IP_MAPPING_INFO_GET_IP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_BFD_IP_MAPPING_INFO_GET_IP_MAPPING_INFO_GET_IP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ip_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ip_ndx after ip_mapping_info_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = oam_bfd_ip_mapping_info_get(
          unit,
          prm_ip_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_bfd_ip_mapping_info_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_bfd_ip_mapping_info_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  OAM_BFD_IP_MAPPING_INFO_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: my_discriminator_range_set (section bfd)
 */
int
  ui_oam_api_bfd_my_discriminator_range_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  OAM_BFD_DISCRIMINATOR_RANGE
    prm_disc_range;

  UI_MACROS_INIT_FUNCTION("ui_oam_api_bfd");
  soc_sand_proc_name = "oam_bfd_my_discriminator_range_set";

  unit = oam_get_default_unit();
  OAM_BFD_DISCRIMINATOR_RANGE_clear(&prm_disc_range);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = oam_bfd_my_discriminator_range_get(
          unit,
          &prm_disc_range
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_bfd_my_discriminator_range_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_bfd_my_discriminator_range_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_MY_DISCRIMINATOR_RANGE_SET_MY_DISCRIMINATOR_RANGE_SET_DISC_RANGE_END_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_BFD_MY_DISCRIMINATOR_RANGE_SET_MY_DISCRIMINATOR_RANGE_SET_DISC_RANGE_END_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_disc_range.end = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_MY_DISCRIMINATOR_RANGE_SET_MY_DISCRIMINATOR_RANGE_SET_DISC_RANGE_START_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_BFD_MY_DISCRIMINATOR_RANGE_SET_MY_DISCRIMINATOR_RANGE_SET_DISC_RANGE_START_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_disc_range.start = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_MY_DISCRIMINATOR_RANGE_SET_MY_DISCRIMINATOR_RANGE_SET_DISC_RANGE_BASE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_BFD_MY_DISCRIMINATOR_RANGE_SET_MY_DISCRIMINATOR_RANGE_SET_DISC_RANGE_BASE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_disc_range.base = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = oam_bfd_my_discriminator_range_set(
          unit,
          &prm_disc_range
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_bfd_my_discriminator_range_set - FAIL", TRUE);
    oam_disp_result(ret, "oam_bfd_my_discriminator_range_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: my_discriminator_range_get (section bfd)
 */
int
  ui_oam_api_bfd_my_discriminator_range_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  OAM_BFD_DISCRIMINATOR_RANGE
    prm_disc_range;

  UI_MACROS_INIT_FUNCTION("ui_oam_api_bfd");
  soc_sand_proc_name = "oam_bfd_my_discriminator_range_get";

  unit = oam_get_default_unit();
  OAM_BFD_DISCRIMINATOR_RANGE_clear(&prm_disc_range);

  /* Get parameters */

  /* Call function */
  ret = oam_bfd_my_discriminator_range_get(
          unit,
          &prm_disc_range
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_bfd_my_discriminator_range_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_bfd_my_discriminator_range_get");
    goto exit;
  }

  send_string_to_screen("--> disc_range:", TRUE);
  OAM_BFD_DISCRIMINATOR_RANGE_print(&prm_disc_range);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: tx_info_set (section bfd)
 */
int
  ui_oam_api_bfd_tx_info_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_my_discriminator_ndx;
  OAM_BFD_TX_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_oam_api_bfd");
  soc_sand_proc_name = "oam_bfd_tx_info_set";

  unit = oam_get_default_unit();
  OAM_BFD_TX_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_MY_DISCRIMINATOR_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_MY_DISCRIMINATOR_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_my_discriminator_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter my_discriminator_ndx after tx_info_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = oam_bfd_tx_info_get(
          unit,
          prm_my_discriminator_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_bfd_tx_info_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_bfd_tx_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_IP_INFO_TTL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_IP_INFO_TTL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.fw_info.ip_info.ttl = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_IP_INFO_COS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_IP_INFO_COS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.fw_info.ip_info.cos = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_IP_INFO_DST_IP_ID,1))
  {
    UI_MACROS_GET_TEXT_VAL(PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_IP_INFO_DST_IP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    soc_sand_pp_ipv4_address_string_parse(param_val->value.val_text, &(prm_info.fw_info.ip_info.dst_ip));
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_IP_INFO_SRC_IP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_IP_INFO_SRC_IP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.fw_info.ip_info.src_ip = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_WITH_IP_INFO_TTL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_WITH_IP_INFO_TTL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.fw_info.mpls_pwe_with_ip_info.ttl = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_WITH_IP_INFO_COS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_WITH_IP_INFO_COS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.fw_info.mpls_pwe_with_ip_info.cos = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_WITH_IP_INFO_PWE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_WITH_IP_INFO_PWE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.fw_info.mpls_pwe_with_ip_info.pwe = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_WITH_IP_INFO_EEP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_WITH_IP_INFO_EEP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.fw_info.mpls_pwe_with_ip_info.eep = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_WITH_IP_INFO_SRC_IP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_WITH_IP_INFO_SRC_IP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.fw_info.mpls_pwe_with_ip_info.src_ip = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_INFO_TTL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_INFO_TTL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.fw_info.mpls_pwe_info.ttl = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_INFO_COS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_INFO_COS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.fw_info.mpls_pwe_info.cos = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_INFO_PWE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_INFO_PWE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.fw_info.mpls_pwe_info.pwe = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_INFO_EEP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_INFO_EEP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.fw_info.mpls_pwe_info.eep = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_TUNNEL_INFO_TTL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_TUNNEL_INFO_TTL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.fw_info.mpls_tunnel_info.ttl = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_TUNNEL_INFO_COS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_TUNNEL_INFO_COS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.fw_info.mpls_tunnel_info.cos = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_TUNNEL_INFO_EEP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_TUNNEL_INFO_EEP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.fw_info.mpls_tunnel_info.eep = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_NG_INFO_MIN_TX_INTERVAL_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_NG_INFO_MIN_TX_INTERVAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.ng_info.min_tx_interval = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_NG_INFO_MIN_RX_INTERVAL_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_NG_INFO_MIN_RX_INTERVAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.ng_info.min_rx_interval = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_NG_INFO_DETECT_MULT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_NG_INFO_DETECT_MULT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.ng_info.detect_mult = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_BFD_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_BFD_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.bfd_type = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_DISCRIMINATOR_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_DISCRIMINATOR_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.discriminator = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_TX_RATE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_TX_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.tx_rate = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.valid = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = oam_bfd_tx_info_set(
          unit,
          prm_my_discriminator_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_bfd_tx_info_set - FAIL", TRUE);
    oam_disp_result(ret, "oam_bfd_tx_info_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: tx_info_get (section bfd)
 */
int
  ui_oam_api_bfd_tx_info_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_my_discriminator_ndx;
  OAM_BFD_TX_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_oam_api_bfd");
  soc_sand_proc_name = "oam_bfd_tx_info_get";

  unit = oam_get_default_unit();
  OAM_BFD_TX_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_TX_INFO_GET_TX_INFO_GET_MY_DISCRIMINATOR_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_BFD_TX_INFO_GET_TX_INFO_GET_MY_DISCRIMINATOR_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_my_discriminator_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter my_discriminator_ndx after tx_info_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = oam_bfd_tx_info_get(
          unit,
          prm_my_discriminator_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_bfd_tx_info_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_bfd_tx_info_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  OAM_BFD_TX_INFO_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: rx_info_set (section bfd)
 */
int
  ui_oam_api_bfd_rx_info_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_my_discriminator_ndx;
  OAM_BFD_RX_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_oam_api_bfd");
  soc_sand_proc_name = "oam_bfd_rx_info_set";

  unit = oam_get_default_unit();
  OAM_BFD_RX_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_RX_INFO_SET_RX_INFO_SET_MY_DISCRIMINATOR_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_BFD_RX_INFO_SET_RX_INFO_SET_MY_DISCRIMINATOR_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_my_discriminator_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter my_discriminator_ndx after rx_info_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = oam_bfd_rx_info_get(
          unit,
          prm_my_discriminator_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_bfd_rx_info_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_bfd_rx_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_RX_INFO_SET_RX_INFO_SET_INFO_DEFECT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_BFD_RX_INFO_SET_RX_INFO_SET_INFO_DEFECT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.defect = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_RX_INFO_SET_RX_INFO_SET_INFO_LIFE_TIME_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_BFD_RX_INFO_SET_RX_INFO_SET_INFO_LIFE_TIME_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.life_time = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = oam_bfd_rx_info_set(
          unit,
          prm_my_discriminator_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_bfd_rx_info_set - FAIL", TRUE);
    oam_disp_result(ret, "oam_bfd_rx_info_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: rx_info_get (section bfd)
 */
int
  ui_oam_api_bfd_rx_info_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_my_discriminator_ndx;
  OAM_BFD_RX_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_oam_api_bfd");
  soc_sand_proc_name = "oam_bfd_rx_info_get";

  unit = oam_get_default_unit();
  OAM_BFD_RX_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_RX_INFO_GET_RX_INFO_GET_MY_DISCRIMINATOR_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_BFD_RX_INFO_GET_RX_INFO_GET_MY_DISCRIMINATOR_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_my_discriminator_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter my_discriminator_ndx after rx_info_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = oam_bfd_rx_info_get(
          unit,
          prm_my_discriminator_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_bfd_rx_info_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_bfd_rx_info_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  OAM_BFD_RX_INFO_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

#endif
#ifdef UI_MPLS
/********************************************************************
 *  Function handler: cos_mapping_info_set (section mpls)
 */
int
  ui_oam_api_mpls_cos_mapping_info_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  OAM_MPLS_COS_ID
    prm_cos_ndx;
  OAM_MPLS_COS_MAPPING_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_oam_api_mpls");
  soc_sand_proc_name = "oam_mpls_cos_mapping_info_set";

  unit = oam_get_default_unit();
  OAM_MPLS_COS_MAPPING_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_COS_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MPLS_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_COS_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_cos_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter cos_ndx after cos_mapping_info_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = oam_mpls_cos_mapping_info_get(
          unit,
          prm_cos_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_mpls_cos_mapping_info_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_mpls_cos_mapping_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_INFO_EXP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MPLS_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_INFO_EXP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.exp = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_INFO_DP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MPLS_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_INFO_DP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.dp = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_INFO_TC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MPLS_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_INFO_TC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.tc = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = oam_mpls_cos_mapping_info_set(
          unit,
          prm_cos_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_mpls_cos_mapping_info_set - FAIL", TRUE);
    oam_disp_result(ret, "oam_mpls_cos_mapping_info_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: cos_mapping_info_get (section mpls)
 */
int
  ui_oam_api_mpls_cos_mapping_info_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  OAM_MPLS_COS_ID
    prm_cos_ndx;
  OAM_MPLS_COS_MAPPING_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_oam_api_mpls");
  soc_sand_proc_name = "oam_mpls_cos_mapping_info_get";

  unit = oam_get_default_unit();
  OAM_MPLS_COS_MAPPING_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_COS_MAPPING_INFO_GET_COS_MAPPING_INFO_GET_COS_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MPLS_COS_MAPPING_INFO_GET_COS_MAPPING_INFO_GET_COS_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_cos_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter cos_ndx after cos_mapping_info_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = oam_mpls_cos_mapping_info_get(
          unit,
          prm_cos_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_mpls_cos_mapping_info_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_mpls_cos_mapping_info_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  OAM_MPLS_COS_MAPPING_INFO_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ttl_mapping_info_set (section mpls)
 */
int
  ui_oam_api_mpls_ttl_mapping_info_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  OAM_MPLS_TTL_ID
    prm_ttl_ndx;
  OAM_MPLS_TTL_MAPPING_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_oam_api_mpls");
  soc_sand_proc_name = "oam_mpls_ttl_mapping_info_set";

  unit = oam_get_default_unit();
  OAM_MPLS_TTL_MAPPING_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_TTL_MAPPING_INFO_SET_TTL_MAPPING_INFO_SET_TTL_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MPLS_TTL_MAPPING_INFO_SET_TTL_MAPPING_INFO_SET_TTL_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ttl_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ttl_ndx after ttl_mapping_info_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = oam_mpls_ttl_mapping_info_get(
          unit,
          prm_ttl_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_mpls_ttl_mapping_info_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_mpls_ttl_mapping_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_TTL_MAPPING_INFO_SET_TTL_MAPPING_INFO_SET_INFO_TTL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MPLS_TTL_MAPPING_INFO_SET_TTL_MAPPING_INFO_SET_INFO_TTL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.ttl = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = oam_mpls_ttl_mapping_info_set(
          unit,
          prm_ttl_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_mpls_ttl_mapping_info_set - FAIL", TRUE);
    oam_disp_result(ret, "oam_mpls_ttl_mapping_info_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ttl_mapping_info_get (section mpls)
 */
int
  ui_oam_api_mpls_ttl_mapping_info_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  OAM_MPLS_TTL_ID
    prm_ttl_ndx;
  OAM_MPLS_TTL_MAPPING_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_oam_api_mpls");
  soc_sand_proc_name = "oam_mpls_ttl_mapping_info_get";

  unit = oam_get_default_unit();
  OAM_MPLS_TTL_MAPPING_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_TTL_MAPPING_INFO_GET_TTL_MAPPING_INFO_GET_TTL_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MPLS_TTL_MAPPING_INFO_GET_TTL_MAPPING_INFO_GET_TTL_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ttl_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ttl_ndx after ttl_mapping_info_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = oam_mpls_ttl_mapping_info_get(
          unit,
          prm_ttl_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_mpls_ttl_mapping_info_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_mpls_ttl_mapping_info_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  OAM_MPLS_TTL_MAPPING_INFO_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ip_mapping_info_set (section mpls)
 */
int
  ui_oam_api_mpls_ip_mapping_info_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  OAM_MPLS_IP_ID
    prm_ip_ndx;
  uint8
    prm_is_ipv6;
  OAM_MPLS_IP_MAPPING_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_oam_api_mpls");
  soc_sand_proc_name = "oam_mpls_ip_mapping_info_set";

  unit = oam_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_IP_MAPPING_INFO_SET_IP_MAPPING_INFO_SET_IP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MPLS_IP_MAPPING_INFO_SET_IP_MAPPING_INFO_SET_IP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ip_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ip_ndx after ip_mapping_info_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = oam_mpls_ip_mapping_info_get(
          unit,
          prm_ip_ndx,
          &prm_is_ipv6,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_mpls_ip_mapping_info_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_mpls_ip_mapping_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_IP_MAPPING_INFO_SET_IP_MAPPING_INFO_SET_IS_IPV6_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MPLS_IP_MAPPING_INFO_SET_IP_MAPPING_INFO_SET_IS_IPV6_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_is_ipv6 = (uint8)param_val->value.ulong_value;
  }

  if (prm_is_ipv6)
  {
    if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_IP_MAPPING_INFO_SET_IP_MAPPING_INFO_SET_INFO_IPV6_IP_ID,1))
    {
      OAM_MPLS_IPV6_MAPPING_INFO_clear(&prm_info.ipv6);
      UI_MACROS_GET_TEXT_VAL(PARAM_OAM_MPLS_IP_MAPPING_INFO_SET_IP_MAPPING_INFO_SET_INFO_IPV6_IP_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      soc_sand_pp_ipv6_address_string_parse(param_val->value.val_text, &(prm_info.ipv6.ip));
    }
  }
  else
  {
    if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_IP_MAPPING_INFO_SET_IP_MAPPING_INFO_SET_INFO_IPV4_IP_ID,1))
    {
      OAM_MPLS_IPV4_MAPPING_INFO_clear(&prm_info.ipv4);
      UI_MACROS_GET_TEXT_VAL(PARAM_OAM_MPLS_IP_MAPPING_INFO_SET_IP_MAPPING_INFO_SET_INFO_IPV4_IP_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      soc_sand_pp_ipv4_address_string_parse(param_val->value.val_text, &(prm_info.ipv4.ip));
    }
  }

  /* Call function */
  ret = oam_mpls_ip_mapping_info_set(
          unit,
          prm_ip_ndx,
          prm_is_ipv6,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_mpls_ip_mapping_info_set - FAIL", TRUE);
    oam_disp_result(ret, "oam_mpls_ip_mapping_info_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ip_mapping_info_get (section mpls)
 */
int
  ui_oam_api_mpls_ip_mapping_info_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  OAM_MPLS_IP_ID
    prm_ip_ndx;
  uint8
    prm_is_ipv6;
  OAM_MPLS_IP_MAPPING_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_oam_api_mpls");
  soc_sand_proc_name = "oam_mpls_ip_mapping_info_get";

  unit = oam_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_IP_MAPPING_INFO_GET_IP_MAPPING_INFO_GET_IP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MPLS_IP_MAPPING_INFO_GET_IP_MAPPING_INFO_GET_IP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ip_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ip_ndx after ip_mapping_info_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = oam_mpls_ip_mapping_info_get(
          unit,
          prm_ip_ndx,
          &prm_is_ipv6,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_mpls_ip_mapping_info_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_mpls_ip_mapping_info_get");
    goto exit;
  }

  send_string_to_screen("--> is_ipv6:", TRUE);
  soc_sand_os_printf( "is_ipv6: %u\n",prm_is_ipv6);

  if (prm_is_ipv6)
  {
    send_string_to_screen("--> info:", TRUE);
    OAM_MPLS_IPV6_MAPPING_INFO_print(&prm_info.ipv6);
  }
  else
  {
    send_string_to_screen("--> info:", TRUE);
    OAM_MPLS_IPV4_MAPPING_INFO_print(&prm_info.ipv4);
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: label_ranges_set (section mpls)
 */
int
  ui_oam_api_mpls_label_ranges_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  OAM_MPLS_LABEL_RANGE
    prm_tnl_range;
  OAM_MPLS_LABEL_RANGE
    prm_pwe_range;

  UI_MACROS_INIT_FUNCTION("ui_oam_api_mpls");
  soc_sand_proc_name = "oam_mpls_label_ranges_set";

  unit = oam_get_default_unit();
  OAM_MPLS_LABEL_RANGE_clear(&prm_tnl_range);
  OAM_MPLS_LABEL_RANGE_clear(&prm_pwe_range);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = oam_mpls_label_ranges_get(
          unit,
          &prm_tnl_range,
          &prm_pwe_range
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_mpls_label_ranges_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_mpls_label_ranges_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_TNL_RANGE_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_TNL_RANGE_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tnl_range.enable = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_TNL_RANGE_END_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_TNL_RANGE_END_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tnl_range.end = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_TNL_RANGE_START_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_TNL_RANGE_START_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tnl_range.start = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_TNL_RANGE_BASE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_TNL_RANGE_BASE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tnl_range.base = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_PWE_RANGE_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_PWE_RANGE_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pwe_range.enable = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_PWE_RANGE_END_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_PWE_RANGE_END_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pwe_range.end = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_PWE_RANGE_START_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_PWE_RANGE_START_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pwe_range.start = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_PWE_RANGE_BASE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_PWE_RANGE_BASE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pwe_range.base = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = oam_mpls_label_ranges_set(
          unit,
          &prm_tnl_range,
          &prm_pwe_range
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_mpls_label_ranges_set - FAIL", TRUE);
    oam_disp_result(ret, "oam_mpls_label_ranges_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: label_ranges_get (section mpls)
 */
int
  ui_oam_api_mpls_label_ranges_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  OAM_MPLS_LABEL_RANGE
    prm_tnl_range;
  OAM_MPLS_LABEL_RANGE
    prm_pwe_range;

  UI_MACROS_INIT_FUNCTION("ui_oam_api_mpls");
  soc_sand_proc_name = "oam_mpls_label_ranges_get";

  unit = oam_get_default_unit();
  OAM_MPLS_LABEL_RANGE_clear(&prm_tnl_range);
  OAM_MPLS_LABEL_RANGE_clear(&prm_pwe_range);

  /* Get parameters */

  /* Call function */
  ret = oam_mpls_label_ranges_get(
          unit,
          &prm_tnl_range,
          &prm_pwe_range
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_mpls_label_ranges_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_mpls_label_ranges_get");
    goto exit;
  }

  send_string_to_screen("--> tnl_range:", TRUE);
  OAM_MPLS_LABEL_RANGE_print(&prm_tnl_range);

  send_string_to_screen("--> pwe_range:", TRUE);
  OAM_MPLS_LABEL_RANGE_print(&prm_pwe_range);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: configurable_ffd_rate_set (section mpls)
 */
int
  ui_oam_api_mpls_configurable_ffd_rate_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  OAM_MPLS_CC_PKT_TYPE
    prm_rate_ndx;
  uint32
    prm_interval;

  UI_MACROS_INIT_FUNCTION("ui_oam_api_mpls");
  soc_sand_proc_name = "oam_mpls_configurable_ffd_rate_set";

  unit = oam_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_CONFIGURABLE_FFD_RATE_SET_CONFIGURABLE_FFD_RATE_SET_RATE_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_OAM_MPLS_CONFIGURABLE_FFD_RATE_SET_CONFIGURABLE_FFD_RATE_SET_RATE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_rate_ndx = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter rate_ndx after configurable_ffd_rate_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = oam_mpls_configurable_ffd_rate_get(
          unit,
          prm_rate_ndx,
          &prm_interval
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_mpls_configurable_ffd_rate_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_mpls_configurable_ffd_rate_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_CONFIGURABLE_FFD_RATE_SET_CONFIGURABLE_FFD_RATE_SET_INTERVAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MPLS_CONFIGURABLE_FFD_RATE_SET_CONFIGURABLE_FFD_RATE_SET_INTERVAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_interval = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = oam_mpls_configurable_ffd_rate_set(
          unit,
          prm_rate_ndx,
          prm_interval
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_mpls_configurable_ffd_rate_set - FAIL", TRUE);
    oam_disp_result(ret, "oam_mpls_configurable_ffd_rate_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: configurable_ffd_rate_get (section mpls)
 */
int
  ui_oam_api_mpls_configurable_ffd_rate_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  OAM_MPLS_CC_PKT_TYPE
    prm_rate_ndx;
  uint32
    prm_interval;

  UI_MACROS_INIT_FUNCTION("ui_oam_api_mpls");
  soc_sand_proc_name = "oam_mpls_configurable_ffd_rate_get";

  unit = oam_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_CONFIGURABLE_FFD_RATE_GET_CONFIGURABLE_FFD_RATE_GET_RATE_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_OAM_MPLS_CONFIGURABLE_FFD_RATE_GET_CONFIGURABLE_FFD_RATE_GET_RATE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_rate_ndx = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter rate_ndx after configurable_ffd_rate_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = oam_mpls_configurable_ffd_rate_get(
          unit,
          prm_rate_ndx,
          &prm_interval
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_mpls_configurable_ffd_rate_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_mpls_configurable_ffd_rate_get");
    goto exit;
  }

  send_string_to_screen("--> interval:", TRUE);
  soc_sand_os_printf( "interval: %d\n",prm_interval);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: lsp_tx_info_set (section mpls)
 */
int
  ui_oam_api_mpls_lsp_tx_info_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_mot_ndx;
  OAM_MPLS_LSP_TX_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_oam_api_mpls");
  soc_sand_proc_name = "oam_mpls_lsp_tx_info_set";

  unit = oam_get_default_unit();
  OAM_MPLS_LSP_TX_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_MOT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_MOT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mot_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mot_ndx after lsp_tx_info_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = oam_mpls_lsp_tx_info_get(
          unit,
          prm_mot_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_mpls_lsp_tx_info_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_mpls_lsp_tx_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_DEFECT_INFO_FDI_EN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_DEFECT_INFO_FDI_EN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.defect_info.fdi_en = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_DEFECT_INFO_BDI_EN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_DEFECT_INFO_BDI_EN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.defect_info.bdi_en = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_DEFECT_INFO_DEFECT_LOCATION_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_DEFECT_INFO_DEFECT_LOCATION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.defect_info.defect_location = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_DEFECT_INFO_DEFECT_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_DEFECT_INFO_DEFECT_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.defect_info.defect_type = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_PACKET_INFO_LSP_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_PACKET_INFO_LSP_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.packet_info.lsp_id = (uint16)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_PACKET_INFO_LSR_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_PACKET_INFO_LSR_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.packet_info.lsr_id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_PACKET_INFO_EEP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_PACKET_INFO_EEP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.packet_info.eep = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_PACKET_INFO_TTL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_PACKET_INFO_TTL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.packet_info.ttl = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_PACKET_INFO_COS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_PACKET_INFO_COS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.packet_info.cos = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_PACKET_INFO_SYSTEM_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_PACKET_INFO_SYSTEM_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.packet_info.system_port = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.type = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.valid = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = oam_mpls_lsp_tx_info_set(
          unit,
          prm_mot_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_mpls_lsp_tx_info_set - FAIL", TRUE);
    oam_disp_result(ret, "oam_mpls_lsp_tx_info_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: lsp_tx_info_get (section mpls)
 */
int
  ui_oam_api_mpls_lsp_tx_info_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_mot_ndx;
  OAM_MPLS_LSP_TX_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_oam_api_mpls");
  soc_sand_proc_name = "oam_mpls_lsp_tx_info_get";

  unit = oam_get_default_unit();
  OAM_MPLS_LSP_TX_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_LSP_TX_INFO_GET_LSP_TX_INFO_GET_MOT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MPLS_LSP_TX_INFO_GET_LSP_TX_INFO_GET_MOT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mot_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mot_ndx after lsp_tx_info_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = oam_mpls_lsp_tx_info_get(
          unit,
          prm_mot_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_mpls_lsp_tx_info_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_mpls_lsp_tx_info_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  OAM_MPLS_LSP_TX_INFO_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: lsp_rx_info_set (section mpls)
 */
int
  ui_oam_api_mpls_lsp_rx_info_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_mor_ndx;
  OAM_MPLS_LSP_RX_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_oam_api_mpls");
  soc_sand_proc_name = "oam_mpls_lsp_rx_info_set";

  unit = oam_get_default_unit();
  OAM_MPLS_LSP_RX_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_LSP_RX_INFO_SET_LSP_RX_INFO_SET_MOR_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MPLS_LSP_RX_INFO_SET_LSP_RX_INFO_SET_MOR_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mor_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mor_ndx after lsp_rx_info_set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = oam_mpls_lsp_rx_info_get(
          unit,
          prm_mor_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_mpls_lsp_rx_info_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_mpls_lsp_rx_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_LSP_RX_INFO_SET_LSP_RX_INFO_SET_INFO_IS_FFD_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MPLS_LSP_RX_INFO_SET_LSP_RX_INFO_SET_INFO_IS_FFD_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.is_ffd = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_LSP_RX_INFO_SET_LSP_RX_INFO_SET_INFO_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MPLS_LSP_RX_INFO_SET_LSP_RX_INFO_SET_INFO_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.valid = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = oam_mpls_lsp_rx_info_set(
          unit,
          prm_mor_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_mpls_lsp_rx_info_set - FAIL", TRUE);
    oam_disp_result(ret, "oam_mpls_lsp_rx_info_set");
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: lsp_rx_info_get (section mpls)
 */
int
  ui_oam_api_mpls_lsp_rx_info_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_mor_ndx;
  OAM_MPLS_LSP_RX_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_oam_api_mpls");
  soc_sand_proc_name = "oam_mpls_lsp_rx_info_get";

  unit = oam_get_default_unit();
  OAM_MPLS_LSP_RX_INFO_clear(&prm_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_LSP_RX_INFO_GET_LSP_RX_INFO_GET_MOR_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_OAM_MPLS_LSP_RX_INFO_GET_LSP_RX_INFO_GET_MOR_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mor_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mor_ndx after lsp_rx_info_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = oam_mpls_lsp_rx_info_get(
          unit,
          prm_mor_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** oam_mpls_lsp_rx_info_get - FAIL", TRUE);
    oam_disp_result(ret, "oam_mpls_lsp_rx_info_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  OAM_MPLS_LSP_RX_INFO_print(&prm_info);


  goto exit;
exit:
  return ui_ret;
}

#endif

#ifdef UI_GENERAL/* { general*/
/********************************************************************
 *  Section handler: general
 */
int
  ui_oam_api_general(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_oam_api_general");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_GENERAL_INFO_SET_INFO_SET_ID,1))
  {
    ui_ret = ui_oam_api_general_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_GENERAL_INFO_GET_INFO_GET_ID,1))
  {
    ui_ret = ui_oam_api_general_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MSG_INFO_GET_MSG_INFO_GET_ID,1))
  {
    ui_ret = ui_oam_api_general_msg_info_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after general***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* { */general

#ifdef UI_ETH/* { eth*/
/********************************************************************
 *  Section handler: eth
 */
int
  ui_oam_api_eth(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_oam_api_eth");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_GLOBAL_INFO_SET_GLOBAL_INFO_SET_ID,1))
  {
    ui_ret = ui_oam_api_eth_global_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_GLOBAL_INFO_GET_GLOBAL_INFO_GET_ID,1))
  {
    ui_ret = ui_oam_api_eth_global_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_ID,1))
  {
    ui_ret = ui_oam_api_eth_cos_mapping_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_COS_MAPPING_INFO_GET_COS_MAPPING_INFO_GET_ID,1))
  {
    ui_ret = ui_oam_api_eth_cos_mapping_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_ID,1))
  {
    ui_ret = ui_oam_api_eth_acc_mep_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_ACC_MEP_INFO_GET_ACC_MEP_INFO_GET_ID,1))
  {
    ui_ret = ui_oam_api_eth_acc_mep_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_ACC_MEP_TX_RDI_SET_ACC_MEP_TX_RDI_SET_ID,1))
  {
    ui_ret = ui_oam_api_eth_acc_mep_tx_rdi_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_ACC_MEP_TX_RDI_GET_ACC_MEP_TX_RDI_GET_ID,1))
  {
    ui_ret = ui_oam_api_eth_acc_mep_tx_rdi_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_REMOTE_MEP_SET_REMOTE_MEP_SET_ID,1))
  {
    ui_ret = ui_oam_api_eth_remote_mep_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_REMOTE_MEP_GET_REMOTE_MEP_GET_ID,1))
  {
    ui_ret = ui_oam_api_eth_remote_mep_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_MEP_DELAY_MEASUREMENT_GET_MEP_DELAY_MEASUREMENT_GET_ID,1))
  {
    ui_ret = ui_oam_api_eth_mep_delay_measurement_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_MEP_LOSS_MEASUREMENT_GET_MEP_LOSS_MEASUREMENT_GET_ID,1))
  {
    ui_ret = ui_oam_api_eth_mep_loss_measurement_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_MEP_TST_PACKET_COUNTER_GET_MEP_TST_PACKET_COUNTER_GET_ID,1))
  {
    ui_ret = ui_oam_api_eth_mep_tst_packet_counter_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_ETH_MEP_TST_PACKET_COUNTER_CLEAR_MEP_TST_PACKET_COUNTER_CLEAR_ID,1))
  {
    ui_ret = ui_oam_api_eth_mep_tst_packet_counter_clear(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after eth***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* { */eth

#ifdef UI_BFD/* { bfd*/
/********************************************************************
 *  Section handler: bfd
 */
int
  ui_oam_api_bfd(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_oam_api_bfd");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_ID,1))
  {
    ui_ret = ui_oam_api_bfd_cos_mapping_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_COS_MAPPING_INFO_GET_COS_MAPPING_INFO_GET_ID,1))
  {
    ui_ret = ui_oam_api_bfd_cos_mapping_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_TTL_MAPPING_INFO_SET_TTL_MAPPING_INFO_SET_ID,1))
  {
    ui_ret = ui_oam_api_bfd_ttl_mapping_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_TTL_MAPPING_INFO_GET_TTL_MAPPING_INFO_GET_ID,1))
  {
    ui_ret = ui_oam_api_bfd_ttl_mapping_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_IP_MAPPING_INFO_SET_IP_MAPPING_INFO_SET_ID,1))
  {
    ui_ret = ui_oam_api_bfd_ip_mapping_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_IP_MAPPING_INFO_GET_IP_MAPPING_INFO_GET_ID,1))
  {
    ui_ret = ui_oam_api_bfd_ip_mapping_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_MY_DISCRIMINATOR_RANGE_SET_MY_DISCRIMINATOR_RANGE_SET_ID,1))
  {
    ui_ret = ui_oam_api_bfd_my_discriminator_range_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_MY_DISCRIMINATOR_RANGE_GET_MY_DISCRIMINATOR_RANGE_GET_ID,1))
  {
    ui_ret = ui_oam_api_bfd_my_discriminator_range_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_ID,1))
  {
    ui_ret = ui_oam_api_bfd_tx_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_TX_INFO_GET_TX_INFO_GET_ID,1))
  {
    ui_ret = ui_oam_api_bfd_tx_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_RX_INFO_SET_RX_INFO_SET_ID,1))
  {
    ui_ret = ui_oam_api_bfd_rx_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_BFD_RX_INFO_GET_RX_INFO_GET_ID,1))
  {
    ui_ret = ui_oam_api_bfd_rx_info_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after bfd***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* { */bfd

#ifdef UI_MPLS/* { mpls*/
/********************************************************************
 *  Section handler: mpls
 */
int
  ui_oam_api_mpls(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_oam_api_mpls");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_ID,1))
  {
    ui_ret = ui_oam_api_mpls_cos_mapping_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_COS_MAPPING_INFO_GET_COS_MAPPING_INFO_GET_ID,1))
  {
    ui_ret = ui_oam_api_mpls_cos_mapping_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_TTL_MAPPING_INFO_SET_TTL_MAPPING_INFO_SET_ID,1))
  {
    ui_ret = ui_oam_api_mpls_ttl_mapping_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_TTL_MAPPING_INFO_GET_TTL_MAPPING_INFO_GET_ID,1))
  {
    ui_ret = ui_oam_api_mpls_ttl_mapping_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_IP_MAPPING_INFO_SET_IP_MAPPING_INFO_SET_ID,1))
  {
    ui_ret = ui_oam_api_mpls_ip_mapping_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_IP_MAPPING_INFO_GET_IP_MAPPING_INFO_GET_ID,1))
  {
    ui_ret = ui_oam_api_mpls_ip_mapping_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_ID,1))
  {
    ui_ret = ui_oam_api_mpls_label_ranges_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_LABEL_RANGES_GET_LABEL_RANGES_GET_ID,1))
  {
    ui_ret = ui_oam_api_mpls_label_ranges_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_CONFIGURABLE_FFD_RATE_SET_CONFIGURABLE_FFD_RATE_SET_ID,1))
  {
    ui_ret = ui_oam_api_mpls_configurable_ffd_rate_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_CONFIGURABLE_FFD_RATE_GET_CONFIGURABLE_FFD_RATE_GET_ID,1))
  {
    ui_ret = ui_oam_api_mpls_configurable_ffd_rate_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_ID,1))
  {
    ui_ret = ui_oam_api_mpls_lsp_tx_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_LSP_TX_INFO_GET_LSP_TX_INFO_GET_ID,1))
  {
    ui_ret = ui_oam_api_mpls_lsp_tx_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_LSP_RX_INFO_SET_LSP_RX_INFO_SET_ID,1))
  {
    ui_ret = ui_oam_api_mpls_lsp_rx_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_MPLS_LSP_RX_INFO_GET_LSP_RX_INFO_GET_ID,1))
  {
    ui_ret = ui_oam_api_mpls_lsp_rx_info_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after mpls***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* { */mpls


/*****************************************************
*NAME
*  subject_oam_api
*TYPE: PROC
*DATE: 29/DEC/2002
*FUNCTION:
*  Process input line which has an 'subject_oam_api' subject.
*  Input line is assumed to have been checked and
*  found to be of right format.
*CALLING SEQUENCE:
*  subject_oam_api(current_line,current_line_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    CURRENT_LINE *current_line -
*      Pointer to prompt line to process.
*    CURRENT_LINE **current_line_ptr -
*      Pointer to prompt line to be displayed after
*      this procedure finishes execution. Caller
*      points this variable to the pointer to
*      the next line to display. If called function wishes
*      to set the next line to display, it replaces
*      the pointer to the next line to display.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then some error has occurred.
*  SOC_SAND_INDIRECT:
*    Processing results. See 'current_line_ptr'.
*REMARKS:
*  This procedure should be carried out under 'task_safe'
*  state (i.e., task can not be deleted while this
*  procedure is being carried out).
*SEE ALSO:
 */
int
  subject_oam_api(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  )
{
  unsigned int
    match_index;
  int
    ui_ret;
  unsigned int
    ;
  char
    *proc_name ;


  proc_name = "subject_oam_api" ;
  ui_ret = FALSE ;
  unit = 0;


  /*
   * the rest of the line is empty
   */
  if (current_line->num_param_names == 0)
  {
    send_string_to_screen("\r\n",FALSE) ;
    send_string_to_screen("'subject_oam_api()' function was called with no parameters.\r\n",FALSE) ;
    goto exit ;
  }
  /*
   * Else, there are parameters on the line (not just 'subject_oam_api').
   */

  send_array_to_screen("\r\n",2) ;



  if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_API_GENERAL_ID,1))
  {
    ui_ret = ui_oam_api_general(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_API_ETH_ID,1))
  {
    ui_ret = ui_oam_api_eth(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_API_BFD_ID,1))
  {
    ui_ret = ui_oam_api_bfd(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_OAM_API_MPLS_ID,1))
  {
    ui_ret = ui_oam_api_mpls(current_line);
  }
  else
  {
    /*
     * Enter if an unknown request.
     */
    send_string_to_screen(
      "\r\n"
      "*** oam_api command with unknown parameters'.\r\n"
      "    Syntax error/sw error...\r\n",
      TRUE)  ;
    ui_ret = TRUE  ;
    goto exit  ;
  }

exit:
  return (ui_ret);
}

