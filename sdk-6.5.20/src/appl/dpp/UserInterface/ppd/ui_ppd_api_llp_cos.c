/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

 
/* 
 * Utilities include file. 
 */ 
#include <shared/bsl.h> 
#include <soc/dpp/SAND/Utils/sand_os_interface.h> 
#include <appl/diag/dpp/utils_defi.h> 
#include <appl/dpp/UserInterface/ui_defi.h> 
 


#include <soc/dpp/PPD/ppd_api_llp_cos.h>  
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_llp_cos.h>

#if LINK_PPD_LIBRARIES

#ifdef UI_LLP_COS
/********************************************************************
*  Function handler: glbl_info_set (section llp_cos)
 */
int
ui_ppd_api_llp_cos_glbl_info_set(
                                 CURRENT_LINE *current_line
                                 )
{
  uint32
    ret;
  SOC_PPC_LLP_COS_GLBL_INFO
    prm_glbl_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_cos");
  soc_sand_proc_name = "soc_ppd_llp_cos_glbl_info_set";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_LLP_COS_GLBL_INFO_clear(&prm_glbl_info);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_ppd_llp_cos_glbl_info_get(
    unit,
    &prm_glbl_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_llp_cos_glbl_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_cos_glbl_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_DEFAULT_DP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_COS_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_DEFAULT_DP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_glbl_info.default_dp = (uint8)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_llp_cos_glbl_info_set(
    unit,
    &prm_glbl_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_llp_cos_glbl_info_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_cos_glbl_info_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: glbl_info_get (section llp_cos)
 */
int
ui_ppd_api_llp_cos_glbl_info_get(
                                 CURRENT_LINE *current_line
                                 )
{
  uint32
    ret;
  SOC_PPC_LLP_COS_GLBL_INFO
    prm_glbl_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_cos");
  soc_sand_proc_name = "soc_ppd_llp_cos_glbl_info_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_LLP_COS_GLBL_INFO_clear(&prm_glbl_info);

  /* Get parameters */

  /* Call function */
  ret = soc_ppd_llp_cos_glbl_info_get(
    unit,
    &prm_glbl_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_llp_cos_glbl_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_cos_glbl_info_get");
    goto exit;
  }

  send_string_to_screen("--> glbl_info:", TRUE);
  SOC_PPC_LLP_COS_GLBL_INFO_print(&prm_glbl_info);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: port_info_set (section llp_cos)
 */
int
ui_ppd_api_llp_cos_port_info_set(
                                 CURRENT_LINE *current_line
                                 )
{
  uint32
    ret;
  SOC_PPC_PORT   
    prm_local_port_ndx;
  SOC_PPC_LLP_COS_PORT_INFO
    prm_port_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_cos");
  soc_sand_proc_name = "soc_ppd_llp_cos_port_info_set";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_LLP_COS_PORT_INFO_clear(&prm_port_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_PORT_INFO_SET_PORT_INFO_SET_LOCAL_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_COS_PORT_INFO_SET_PORT_INFO_SET_LOCAL_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_local_port_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter local_port_ndx after port_info_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = soc_ppd_llp_cos_port_info_get(
    unit,
    SOC_CORE_DEFAULT,
    prm_local_port_ndx,
    &prm_port_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_llp_cos_port_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_cos_port_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_DEFAULT_TC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_COS_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_DEFAULT_TC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_info.default_tc = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_L4_INFO_USE_L4_PRTCL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_COS_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_L4_INFO_USE_L4_PRTCL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_info.l4_info.use_l4_prtcl = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_L3_INFO_USE_IP_SUBNET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_COS_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_L3_INFO_USE_IP_SUBNET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_info.l3_info.use_ip_subnet = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_L3_INFO_USE_MPLS_TERM_LBL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_COS_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_L3_INFO_USE_MPLS_TERM_LBL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_info.l3_info.use_mpls_term_lbl = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_L3_INFO_IP_QOS_TO_TC_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_COS_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_L3_INFO_IP_QOS_TO_TC_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_info.l3_info.ip_qos_to_tc_index = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_L3_INFO_USE_IP_QOS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_COS_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_L3_INFO_USE_IP_QOS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_info.l3_info.use_ip_qos = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_L2_INFO_TBLS_SELECT_TC_TO_DP_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_COS_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_L2_INFO_TBLS_SELECT_TC_TO_DP_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_info.l2_info.tbls_select.tc_to_dp_index = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_L2_INFO_TBLS_SELECT_UP_TO_DP_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_COS_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_L2_INFO_TBLS_SELECT_UP_TO_DP_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_info.l2_info.tbls_select.up_to_dp_index = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_L2_INFO_TBLS_SELECT_TC_TO_UP_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_COS_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_L2_INFO_TBLS_SELECT_TC_TO_UP_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_info.l2_info.tbls_select.tc_to_up_index = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_L2_INFO_TBLS_SELECT_IN_UP_TO_TC_AND_DE_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_COS_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_L2_INFO_TBLS_SELECT_IN_UP_TO_TC_AND_DE_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_info.l2_info.tbls_select.in_up_to_tc_and_de_index = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_L2_INFO_USE_DEI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_COS_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_L2_INFO_USE_DEI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_info.l2_info.use_dei = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_L2_INFO_USE_L2_PROTOCOL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_COS_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_L2_INFO_USE_L2_PROTOCOL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_info.l2_info.use_l2_protocol = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_L2_INFO_IGNORE_PCP_FOR_TC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_COS_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_L2_INFO_IGNORE_PCP_FOR_TC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_info.l2_info.ignore_pkt_pcp_for_tc = (uint8)param_val->value.ulong_value;
  }
  

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_L2_INFO_UP_USE_USE_FOR_OUT_UP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_COS_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_L2_INFO_UP_USE_USE_FOR_OUT_UP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_info.l2_info.up_use.use_for_out_up = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_L2_INFO_UP_USE_USE_FOR_TC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_COS_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_L2_INFO_UP_USE_USE_FOR_TC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_info.l2_info.up_use.use_for_tc = (uint8)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_llp_cos_port_info_set(
    unit,
    SOC_CORE_DEFAULT,
    prm_local_port_ndx,
    &prm_port_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_llp_cos_port_info_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_cos_port_info_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: port_info_get (section llp_cos)
 */
int
ui_ppd_api_llp_cos_port_info_get(
                                 CURRENT_LINE *current_line
                                 )
{
  uint32
    ret;
  SOC_PPC_PORT   
    prm_local_port_ndx;
  SOC_PPC_LLP_COS_PORT_INFO
    prm_port_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_cos");
  soc_sand_proc_name = "soc_ppd_llp_cos_port_info_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_LLP_COS_PORT_INFO_clear(&prm_port_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_PORT_INFO_GET_PORT_INFO_GET_LOCAL_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_COS_PORT_INFO_GET_PORT_INFO_GET_LOCAL_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_local_port_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter local_port_ndx after port_info_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_llp_cos_port_info_get(
    unit,
    SOC_CORE_DEFAULT,
    prm_local_port_ndx,
    &prm_port_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_llp_cos_port_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_cos_port_info_get");
    goto exit;
  }

  send_string_to_screen("--> port_info:", TRUE);
  SOC_PPC_LLP_COS_PORT_INFO_print(&prm_port_info);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: mapping_table_entry_set (section llp_cos)
 */
int
ui_ppd_api_llp_cos_mapping_table_entry_set(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_PPC_LLP_COS_MAPPING_TABLE
    prm_mapping_tbl_ndx;
  uint32
    prm_table_id_ndx;
  uint32
    prm_entry_ndx;
  SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO
    prm_entry;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_cos");
  soc_sand_proc_name = "soc_ppd_llp_cos_mapping_table_entry_set";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO_clear(&prm_entry);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_MAPPING_TABLE_ENTRY_SET_MAPPING_TABLE_ENTRY_SET_MAPPING_TBL_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LLP_COS_MAPPING_TABLE_ENTRY_SET_MAPPING_TABLE_ENTRY_SET_MAPPING_TBL_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mapping_tbl_ndx = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mapping_tbl_ndx after mapping_table_entry_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_MAPPING_TABLE_ENTRY_SET_MAPPING_TABLE_ENTRY_SET_TABLE_ID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_COS_MAPPING_TABLE_ENTRY_SET_MAPPING_TABLE_ENTRY_SET_TABLE_ID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_table_id_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter table_id_ndx after mapping_table_entry_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_MAPPING_TABLE_ENTRY_SET_MAPPING_TABLE_ENTRY_SET_ENTRY_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_COS_MAPPING_TABLE_ENTRY_SET_MAPPING_TABLE_ENTRY_SET_ENTRY_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_ndx after mapping_table_entry_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = soc_ppd_llp_cos_mapping_table_entry_get(
    unit,
    prm_mapping_tbl_ndx,
    prm_table_id_ndx,
    prm_entry_ndx,
    &prm_entry
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_llp_cos_mapping_table_entry_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_cos_mapping_table_entry_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_MAPPING_TABLE_ENTRY_SET_MAPPING_TABLE_ENTRY_SET_ENTRY_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_COS_MAPPING_TABLE_ENTRY_SET_MAPPING_TABLE_ENTRY_SET_ENTRY_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry.valid = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_MAPPING_TABLE_ENTRY_SET_MAPPING_TABLE_ENTRY_SET_ENTRY_VALUE2_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_COS_MAPPING_TABLE_ENTRY_SET_MAPPING_TABLE_ENTRY_SET_ENTRY_VALUE2_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry.value2 = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_MAPPING_TABLE_ENTRY_SET_MAPPING_TABLE_ENTRY_SET_ENTRY_VALUE1_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_COS_MAPPING_TABLE_ENTRY_SET_MAPPING_TABLE_ENTRY_SET_ENTRY_VALUE1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry.value1 = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_llp_cos_mapping_table_entry_set(
    unit,
    prm_mapping_tbl_ndx,
    prm_table_id_ndx,
    prm_entry_ndx,
    &prm_entry
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_llp_cos_mapping_table_entry_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_cos_mapping_table_entry_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: mapping_table_entry_get (section llp_cos)
 */
int
ui_ppd_api_llp_cos_mapping_table_entry_get(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_PPC_LLP_COS_MAPPING_TABLE
    prm_mapping_tbl_ndx;
  uint32
    prm_table_id_ndx;
  uint32
    prm_entry_ndx;
  SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO
    prm_entry;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_cos");
  soc_sand_proc_name = "soc_ppd_llp_cos_mapping_table_entry_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO_clear(&prm_entry);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_MAPPING_TABLE_ENTRY_GET_MAPPING_TABLE_ENTRY_GET_MAPPING_TBL_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LLP_COS_MAPPING_TABLE_ENTRY_GET_MAPPING_TABLE_ENTRY_GET_MAPPING_TBL_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mapping_tbl_ndx = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mapping_tbl_ndx after mapping_table_entry_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_MAPPING_TABLE_ENTRY_GET_MAPPING_TABLE_ENTRY_GET_TABLE_ID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_COS_MAPPING_TABLE_ENTRY_GET_MAPPING_TABLE_ENTRY_GET_TABLE_ID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_table_id_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter table_id_ndx after mapping_table_entry_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_MAPPING_TABLE_ENTRY_GET_MAPPING_TABLE_ENTRY_GET_ENTRY_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_COS_MAPPING_TABLE_ENTRY_GET_MAPPING_TABLE_ENTRY_GET_ENTRY_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_ndx after mapping_table_entry_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_llp_cos_mapping_table_entry_get(
    unit,
    prm_mapping_tbl_ndx,
    prm_table_id_ndx,
    prm_entry_ndx,
    &prm_entry
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_llp_cos_mapping_table_entry_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_cos_mapping_table_entry_get");
    goto exit;
  }

  send_string_to_screen("--> entry:", TRUE);
  SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO_print(&prm_entry);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: l4_port_range_info_set (section llp_cos)
 */
int
ui_ppd_api_llp_cos_l4_port_range_info_set(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_range_ndx;
  SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO
    prm_range_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_cos");
  soc_sand_proc_name = "soc_ppd_llp_cos_l4_port_range_info_set";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO_clear(&prm_range_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_L4_PORT_RANGE_INFO_SET_L4_PORT_RANGE_INFO_SET_RANGE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_COS_L4_PORT_RANGE_INFO_SET_L4_PORT_RANGE_INFO_SET_RANGE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter range_ndx after l4_port_range_info_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = soc_ppd_llp_cos_l4_port_range_info_get(
    unit,
    prm_range_ndx,
    &prm_range_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_llp_cos_l4_port_range_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_cos_l4_port_range_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_L4_PORT_RANGE_INFO_SET_L4_PORT_RANGE_INFO_SET_RANGE_INFO_IN_RANGE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_COS_L4_PORT_RANGE_INFO_SET_L4_PORT_RANGE_INFO_SET_RANGE_INFO_IN_RANGE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_info.in_range = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_L4_PORT_RANGE_INFO_SET_L4_PORT_RANGE_INFO_SET_RANGE_INFO_END_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_COS_L4_PORT_RANGE_INFO_SET_L4_PORT_RANGE_INFO_SET_RANGE_INFO_END_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_info.end = (uint16)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_L4_PORT_RANGE_INFO_SET_L4_PORT_RANGE_INFO_SET_RANGE_INFO_START_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_COS_L4_PORT_RANGE_INFO_SET_L4_PORT_RANGE_INFO_SET_RANGE_INFO_START_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_info.start = (uint16)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_llp_cos_l4_port_range_info_set(
    unit,
    prm_range_ndx,
    &prm_range_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_llp_cos_l4_port_range_info_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_cos_l4_port_range_info_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: l4_port_range_info_get (section llp_cos)
 */
int
ui_ppd_api_llp_cos_l4_port_range_info_get(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_range_ndx;
  SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO
    prm_range_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_cos");
  soc_sand_proc_name = "soc_ppd_llp_cos_l4_port_range_info_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO_clear(&prm_range_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_L4_PORT_RANGE_INFO_GET_L4_PORT_RANGE_INFO_GET_RANGE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_COS_L4_PORT_RANGE_INFO_GET_L4_PORT_RANGE_INFO_GET_RANGE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter range_ndx after l4_port_range_info_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_llp_cos_l4_port_range_info_get(
    unit,
    prm_range_ndx,
    &prm_range_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_llp_cos_l4_port_range_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_cos_l4_port_range_info_get");
    goto exit;
  }

  send_string_to_screen("--> range_info:", TRUE);
  SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO_print(&prm_range_info);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: l4_port_range_to_tc_info_set (section llp_cos)
 */
int
ui_ppd_api_llp_cos_l4_port_range_to_tc_info_set(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_L4_PRTCL_TYPE
    prm_l4_prtcl_type_ndx;
  uint32
    prm_src_port_match_range_ndx;
  uint32
    prm_dest_port_match_range_ndx;
  SOC_PPC_LLP_COS_TC_INFO
    prm_tc_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_cos");
  soc_sand_proc_name = "soc_ppd_llp_cos_l4_port_range_to_tc_info_set";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_LLP_COS_TC_INFO_clear(&prm_tc_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_SET_L4_PORT_RANGE_TO_TC_INFO_SET_L4_PRTCL_TYPE_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_SET_L4_PORT_RANGE_TO_TC_INFO_SET_L4_PRTCL_TYPE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_l4_prtcl_type_ndx = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter l4_prtcl_type_ndx after l4_port_range_to_tc_info_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_SET_L4_PORT_RANGE_TO_TC_INFO_SET_SRC_PORT_MATCH_RANGE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_SET_L4_PORT_RANGE_TO_TC_INFO_SET_SRC_PORT_MATCH_RANGE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_src_port_match_range_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter src_port_match_range_ndx after l4_port_range_to_tc_info_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_SET_L4_PORT_RANGE_TO_TC_INFO_SET_DEST_PORT_MATCH_RANGE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_SET_L4_PORT_RANGE_TO_TC_INFO_SET_DEST_PORT_MATCH_RANGE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_dest_port_match_range_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter dest_port_match_range_ndx after l4_port_range_to_tc_info_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = soc_ppd_llp_cos_l4_port_range_to_tc_info_get(
    unit,
    prm_l4_prtcl_type_ndx,
    prm_src_port_match_range_ndx,
    prm_dest_port_match_range_ndx,
    &prm_tc_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_llp_cos_l4_port_range_to_tc_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_cos_l4_port_range_to_tc_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_SET_L4_PORT_RANGE_TO_TC_INFO_SET_TC_INFO_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_SET_L4_PORT_RANGE_TO_TC_INFO_SET_TC_INFO_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tc_info.valid = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_SET_L4_PORT_RANGE_TO_TC_INFO_SET_TC_INFO_TC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_SET_L4_PORT_RANGE_TO_TC_INFO_SET_TC_INFO_TC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tc_info.tc = (uint8)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_llp_cos_l4_port_range_to_tc_info_set(
    unit,
    prm_l4_prtcl_type_ndx,
    prm_src_port_match_range_ndx,
    prm_dest_port_match_range_ndx,
    &prm_tc_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_llp_cos_l4_port_range_to_tc_info_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_cos_l4_port_range_to_tc_info_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: l4_port_range_to_tc_info_get (section llp_cos)
 */
int
ui_ppd_api_llp_cos_l4_port_range_to_tc_info_get(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_L4_PRTCL_TYPE
    prm_l4_prtcl_type_ndx;
  uint32
    prm_src_port_match_range_ndx;
  uint32
    prm_dest_port_match_range_ndx;
  SOC_PPC_LLP_COS_TC_INFO
    prm_tc_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_cos");
  soc_sand_proc_name = "soc_ppd_llp_cos_l4_port_range_to_tc_info_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_LLP_COS_TC_INFO_clear(&prm_tc_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_GET_L4_PORT_RANGE_TO_TC_INFO_GET_L4_PRTCL_TYPE_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_GET_L4_PORT_RANGE_TO_TC_INFO_GET_L4_PRTCL_TYPE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_l4_prtcl_type_ndx = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter l4_prtcl_type_ndx after l4_port_range_to_tc_info_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_GET_L4_PORT_RANGE_TO_TC_INFO_GET_SRC_PORT_MATCH_RANGE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_GET_L4_PORT_RANGE_TO_TC_INFO_GET_SRC_PORT_MATCH_RANGE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_src_port_match_range_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter src_port_match_range_ndx after l4_port_range_to_tc_info_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_GET_L4_PORT_RANGE_TO_TC_INFO_GET_DEST_PORT_MATCH_RANGE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_GET_L4_PORT_RANGE_TO_TC_INFO_GET_DEST_PORT_MATCH_RANGE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_dest_port_match_range_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter dest_port_match_range_ndx after l4_port_range_to_tc_info_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_llp_cos_l4_port_range_to_tc_info_get(
    unit,
    prm_l4_prtcl_type_ndx,
    prm_src_port_match_range_ndx,
    prm_dest_port_match_range_ndx,
    &prm_tc_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_llp_cos_l4_port_range_to_tc_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_cos_l4_port_range_to_tc_info_get");
    goto exit;
  }

  send_string_to_screen("--> tc_info:", TRUE);
  SOC_PPC_LLP_COS_TC_INFO_print(&prm_tc_info);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: ipv4_subnet_based_set (section llp_cos)
 */
int
ui_ppd_api_llp_cos_ipv4_subnet_based_set(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_SAND_PP_IPV4_SUBNET
    prm_subnet_key;
  SOC_PPC_LLP_COS_IPV4_SUBNET_INFO
    prm_subnet_based_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_cos");
  soc_sand_proc_name = "soc_ppd_llp_cos_ipv4_subnet_based_set";

  unit = soc_ppd_get_default_unit();
  soc_sand_SAND_PP_IPV4_SUBNET_clear(&prm_subnet_key);
  SOC_PPC_LLP_COS_IPV4_SUBNET_INFO_clear(&prm_subnet_based_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_IPV4_SUBNET_BASED_SET_IPV4_SUBNET_BASED_SET_ENTRY_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_COS_IPV4_SUBNET_BASED_SET_IPV4_SUBNET_BASED_SET_ENTRY_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_ndx after ipv4_subnet_based_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_IPV4_SUBNET_BASED_GET_IPV4_SUBNET_BASED_GET_SUBNET_KEY_PREFIX_LEN_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_COS_IPV4_SUBNET_BASED_GET_IPV4_SUBNET_BASED_GET_SUBNET_KEY_PREFIX_LEN_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_subnet_key.prefix_len = (uint8)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_IPV4_SUBNET_BASED_GET_IPV4_SUBNET_BASED_GET_SUBNET_KEY_IP_ADDRESS_ID,1))          
  {  
    if (!get_val_of(
      current_line,(int *)&match_index,SOC_PARAM_PPD_LLP_COS_IPV4_SUBNET_BASED_GET_IPV4_SUBNET_BASED_GET_SUBNET_KEY_IP_ADDRESS_ID,1,
      &param_val,VAL_IP,err_msg))
    {
      prm_subnet_key.ip_address = param_val->value.ip_value;
    }
  }

  /* This is a set function, so call GET function first */
  ret = soc_ppd_llp_cos_ipv4_subnet_based_get(
    unit,
    prm_entry_ndx,
    &prm_subnet_key,
    &prm_subnet_based_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_llp_cos_ipv4_subnet_based_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_cos_ipv4_subnet_based_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_IPV4_SUBNET_BASED_SET_IPV4_SUBNET_BASED_SET_SUBNET_BASED_INFO_TC_IS_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_COS_IPV4_SUBNET_BASED_SET_IPV4_SUBNET_BASED_SET_SUBNET_BASED_INFO_TC_IS_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_subnet_based_info.tc_is_valid = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_IPV4_SUBNET_BASED_SET_IPV4_SUBNET_BASED_SET_SUBNET_BASED_INFO_TC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_COS_IPV4_SUBNET_BASED_SET_IPV4_SUBNET_BASED_SET_SUBNET_BASED_INFO_TC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_subnet_based_info.tc = (SOC_SAND_PP_TC)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_llp_cos_ipv4_subnet_based_set(
    unit,
    prm_entry_ndx,
    &prm_subnet_key,
    &prm_subnet_based_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_llp_cos_ipv4_subnet_based_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_cos_ipv4_subnet_based_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: ipv4_subnet_based_get (section llp_cos)
 */
int
ui_ppd_api_llp_cos_ipv4_subnet_based_get(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_SAND_PP_IPV4_SUBNET
    prm_subnet_key;
  SOC_PPC_LLP_COS_IPV4_SUBNET_INFO
    prm_subnet_based_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_cos");
  soc_sand_proc_name = "soc_ppd_llp_cos_ipv4_subnet_based_get";

  unit = soc_ppd_get_default_unit();
  soc_sand_SAND_PP_IPV4_SUBNET_clear(&prm_subnet_key);
  SOC_PPC_LLP_COS_IPV4_SUBNET_INFO_clear(&prm_subnet_based_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_IPV4_SUBNET_BASED_GET_IPV4_SUBNET_BASED_GET_ENTRY_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_COS_IPV4_SUBNET_BASED_GET_IPV4_SUBNET_BASED_GET_ENTRY_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_ndx after ipv4_subnet_based_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_llp_cos_ipv4_subnet_based_get(
    unit,
    prm_entry_ndx,
    &prm_subnet_key,
    &prm_subnet_based_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_llp_cos_ipv4_subnet_based_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_cos_ipv4_subnet_based_get");
    goto exit;
  }

  send_string_to_screen("--> subnet_based_info:", TRUE);
  soc_sand_SAND_PP_IPV4_SUBNET_print(&prm_subnet_key);
  send_string_to_screen("", TRUE);
  SOC_PPC_LLP_COS_IPV4_SUBNET_INFO_print(&prm_subnet_based_info);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: protocol_based_set (section llp_cos)
 */
int
ui_ppd_api_llp_cos_protocol_based_set(
                                      CURRENT_LINE *current_line
                                      )
{
  uint32
    ret;
  uint32
    prm_profile_ndx;
  SOC_SAND_PP_ETHER_TYPE   
    prm_ether_type_ndx;
  SOC_PPC_LLP_COS_PRTCL_INFO
    prm_prtcl_assign_info;
  SOC_SAND_SUCCESS_FAILURE
    prm_success;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_cos");
  soc_sand_proc_name = "soc_ppd_llp_cos_protocol_based_set";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_LLP_COS_PRTCL_INFO_clear(&prm_prtcl_assign_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_PROTOCOL_BASED_SET_PROTOCOL_BASED_SET_PROFILE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_COS_PROTOCOL_BASED_SET_PROTOCOL_BASED_SET_PROFILE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_profile_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter profile_ndx after protocol_based_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_PROTOCOL_BASED_SET_PROTOCOL_BASED_SET_ETHER_TYPE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_COS_PROTOCOL_BASED_SET_PROTOCOL_BASED_SET_ETHER_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ether_type_ndx = (uint16)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter profile_ndx after protocol_based_set***", TRUE);
    goto exit;
  }


  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_PROTOCOL_BASED_SET_PROTOCOL_BASED_SET_PRTCL_ASSIGN_INFO_TC_IS_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_COS_PROTOCOL_BASED_SET_PROTOCOL_BASED_SET_PRTCL_ASSIGN_INFO_TC_IS_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_prtcl_assign_info.tc_is_valid = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_PROTOCOL_BASED_SET_PROTOCOL_BASED_SET_PRTCL_ASSIGN_INFO_TC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_COS_PROTOCOL_BASED_SET_PROTOCOL_BASED_SET_PRTCL_ASSIGN_INFO_TC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_prtcl_assign_info.tc = (SOC_SAND_PP_TC)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_llp_cos_protocol_based_set(
    unit,
    prm_profile_ndx,
    prm_ether_type_ndx,
    &prm_prtcl_assign_info,
    &prm_success
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_llp_cos_protocol_based_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_cos_protocol_based_set");
    goto exit;
  }

  send_string_to_screen("--> success:", TRUE);
  cli_out("success: %s\n",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: protocol_based_get (section llp_cos)
 */
int
ui_ppd_api_llp_cos_protocol_based_get(
                                      CURRENT_LINE *current_line
                                      )
{
  uint32
    ret;
  uint32
    prm_profile_ndx;
  SOC_SAND_PP_ETHER_TYPE   
    prm_ether_type_ndx;
  SOC_PPC_LLP_COS_PRTCL_INFO
    prm_prtcl_assign_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_cos");
  soc_sand_proc_name = "soc_ppd_llp_cos_protocol_based_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_LLP_COS_PRTCL_INFO_clear(&prm_prtcl_assign_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_PROTOCOL_BASED_GET_PROTOCOL_BASED_GET_PROFILE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_COS_PROTOCOL_BASED_GET_PROTOCOL_BASED_GET_PROFILE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_profile_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter profile_ndx after protocol_based_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_PROTOCOL_BASED_GET_PROTOCOL_BASED_GET_ETHER_TYPE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_COS_PROTOCOL_BASED_GET_PROTOCOL_BASED_GET_ETHER_TYPE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ether_type_ndx = (uint16)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter profile_ndx after protocol_based_set***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_llp_cos_protocol_based_get(
    unit,
    prm_profile_ndx,
    prm_ether_type_ndx,
    &prm_prtcl_assign_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_llp_cos_protocol_based_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_cos_protocol_based_get");
    goto exit;
  }

  send_string_to_screen("--> prtcl_assign_info:", TRUE);
  SOC_PPC_LLP_COS_PRTCL_INFO_print(&prm_prtcl_assign_info);

  send_string_to_screen("--> success:", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Section handler: llp_cos
 */
int
ui_ppd_api_llp_cos(
                   CURRENT_LINE *current_line
                   )
{
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_cos");

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_GLBL_INFO_SET_GLBL_INFO_SET_ID,1))
  {
    ui_ret = ui_ppd_api_llp_cos_glbl_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_GLBL_INFO_GET_GLBL_INFO_GET_ID,1))
  {
    ui_ret = ui_ppd_api_llp_cos_glbl_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_PORT_INFO_SET_PORT_INFO_SET_ID,1))
  {
    ui_ret = ui_ppd_api_llp_cos_port_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_PORT_INFO_GET_PORT_INFO_GET_ID,1))
  {
    ui_ret = ui_ppd_api_llp_cos_port_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_MAPPING_TABLE_ENTRY_SET_MAPPING_TABLE_ENTRY_SET_ID,1))
  {
    ui_ret = ui_ppd_api_llp_cos_mapping_table_entry_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_MAPPING_TABLE_ENTRY_GET_MAPPING_TABLE_ENTRY_GET_ID,1))
  {
    ui_ret = ui_ppd_api_llp_cos_mapping_table_entry_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_L4_PORT_RANGE_INFO_SET_L4_PORT_RANGE_INFO_SET_ID,1))
  {
    ui_ret = ui_ppd_api_llp_cos_l4_port_range_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_L4_PORT_RANGE_INFO_GET_L4_PORT_RANGE_INFO_GET_ID,1))
  {
    ui_ret = ui_ppd_api_llp_cos_l4_port_range_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_SET_L4_PORT_RANGE_TO_TC_INFO_SET_ID,1))
  {
    ui_ret = ui_ppd_api_llp_cos_l4_port_range_to_tc_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_GET_L4_PORT_RANGE_TO_TC_INFO_GET_ID,1))
  {
    ui_ret = ui_ppd_api_llp_cos_l4_port_range_to_tc_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_IPV4_SUBNET_BASED_SET_IPV4_SUBNET_BASED_SET_ID,1))
  {
    ui_ret = ui_ppd_api_llp_cos_ipv4_subnet_based_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_IPV4_SUBNET_BASED_GET_IPV4_SUBNET_BASED_GET_ID,1))
  {
    ui_ret = ui_ppd_api_llp_cos_ipv4_subnet_based_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_PROTOCOL_BASED_SET_PROTOCOL_BASED_SET_ID,1))
  {
    ui_ret = ui_ppd_api_llp_cos_protocol_based_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_COS_PROTOCOL_BASED_GET_PROTOCOL_BASED_GET_ID,1))
  {
    ui_ret = ui_ppd_api_llp_cos_protocol_based_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after llp_cos***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif
#endif /* LINK_PPD_LIBRARIES */ 

