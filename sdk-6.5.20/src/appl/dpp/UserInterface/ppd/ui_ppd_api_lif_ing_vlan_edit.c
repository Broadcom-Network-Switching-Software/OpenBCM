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
 


#include <soc/dpp/PPD/ppd_api_lif_ing_vlan_edit.h>                                                               
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_lif_ing_vlan_edit.h>

#if LINK_PPD_LIBRARIES

  
#ifdef UI_LIF_ING_VLAN_EDIT
/********************************************************************
*  Function handler: command_id_set (section llp_ing_vlan_edit)
 */
int
ui_ppd_api_lif_ing_vlan_edit_command_id_set(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_KEY
    prm_command_key;
  uint32
    prm_command_id;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif_ing_vlan_edit");
  soc_sand_proc_name = "soc_ppd_lif_ing_vlan_edit_command_id_set";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_KEY_clear(&prm_command_key);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_COMMAND_ID_SET_COMMAND_ID_SET_COMMAND_KEY_EDIT_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_COMMAND_ID_SET_COMMAND_ID_SET_COMMAND_KEY_EDIT_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_command_key.edit_profile = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_COMMAND_ID_SET_COMMAND_ID_SET_COMMAND_KEY_TAG_FORMAT_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_COMMAND_ID_SET_COMMAND_ID_SET_COMMAND_KEY_TAG_FORMAT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_command_key.tag_format = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter command_key after command_id_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = soc_ppd_lif_ing_vlan_edit_command_id_get(
    unit,
    &prm_command_key,
    &prm_command_id
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_ing_vlan_edit_command_id_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_ing_vlan_edit_command_id_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_COMMAND_ID_SET_COMMAND_ID_SET_COMMAND_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_COMMAND_ID_SET_COMMAND_ID_SET_COMMAND_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_command_id = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_lif_ing_vlan_edit_command_id_set(
    unit,
    &prm_command_key,
    prm_command_id
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_ing_vlan_edit_command_id_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_ing_vlan_edit_command_id_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: command_id_get (section llp_ing_vlan_edit)
 */
int
ui_ppd_api_lif_ing_vlan_edit_command_id_get(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_KEY
    prm_command_key;
  uint32
    prm_command_id;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif_ing_vlan_edit");
  soc_sand_proc_name = "soc_ppd_lif_ing_vlan_edit_command_id_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_KEY_clear(&prm_command_key);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_COMMAND_ID_GET_COMMAND_ID_GET_COMMAND_KEY_EDIT_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_COMMAND_ID_GET_COMMAND_ID_GET_COMMAND_KEY_EDIT_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_command_key.edit_profile = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_COMMAND_ID_GET_COMMAND_ID_GET_COMMAND_KEY_TAG_FORMAT_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_COMMAND_ID_GET_COMMAND_ID_GET_COMMAND_KEY_TAG_FORMAT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_command_key.tag_format = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter command_key after command_id_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_lif_ing_vlan_edit_command_id_get(
    unit,
    &prm_command_key,
    &prm_command_id
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_ing_vlan_edit_command_id_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_ing_vlan_edit_command_id_get");
    goto exit;
  }

  send_string_to_screen("--> command_id:", TRUE);
  cli_out("command_id: %u\n",prm_command_id);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: command_info_set (section llp_ing_vlan_edit)
 */
int
ui_ppd_api_lif_ing_vlan_edit_command_info_set(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_command_ndx;
  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO
    prm_command_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif_ing_vlan_edit");
  soc_sand_proc_name = "soc_ppd_lif_ing_vlan_edit_command_info_set";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO_clear(&prm_command_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_COMMAND_INFO_SET_COMMAND_INFO_SET_COMMAND_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_COMMAND_INFO_SET_COMMAND_INFO_SET_COMMAND_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_command_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter command_ndx after command_info_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = soc_ppd_lif_ing_vlan_edit_command_info_get(
    unit,
    prm_command_ndx,
    &prm_command_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_ing_vlan_edit_command_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_ing_vlan_edit_command_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_COMMAND_INFO_SET_COMMAND_INFO_SET_COMMAND_INFO_OUTER_TAG_PCP_DEI_SOURCE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_COMMAND_INFO_SET_COMMAND_INFO_SET_COMMAND_INFO_OUTER_TAG_PCP_DEI_SOURCE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_command_info.outer_tag.pcp_dei_source = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_COMMAND_INFO_SET_COMMAND_INFO_SET_COMMAND_INFO_OUTER_TAG_VID_SOURCE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_COMMAND_INFO_SET_COMMAND_INFO_SET_COMMAND_INFO_OUTER_TAG_VID_SOURCE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_command_info.outer_tag.vid_source = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_COMMAND_INFO_SET_COMMAND_INFO_SET_COMMAND_INFO_OUTER_TAG_TPID_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_COMMAND_INFO_SET_COMMAND_INFO_SET_COMMAND_INFO_OUTER_TAG_TPID_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_command_info.outer_tag.tpid_index = param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_COMMAND_INFO_SET_COMMAND_INFO_SET_COMMAND_INFO_TPID_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_COMMAND_INFO_SET_COMMAND_INFO_SET_COMMAND_INFO_OUTER_TAG_TPID_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_command_info.tpid_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_COMMAND_INFO_SET_COMMAND_INFO_SET_COMMAND_INFO_INNER_TAG_PCP_DEI_SOURCE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_COMMAND_INFO_SET_COMMAND_INFO_SET_COMMAND_INFO_INNER_TAG_PCP_DEI_SOURCE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_command_info.inner_tag.pcp_dei_source = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_COMMAND_INFO_SET_COMMAND_INFO_SET_COMMAND_INFO_INNER_TAG_VID_SOURCE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_COMMAND_INFO_SET_COMMAND_INFO_SET_COMMAND_INFO_INNER_TAG_VID_SOURCE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_command_info.inner_tag.vid_source = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_COMMAND_INFO_SET_COMMAND_INFO_SET_COMMAND_INFO_INNER_TAG_TPID_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_COMMAND_INFO_SET_COMMAND_INFO_SET_COMMAND_INFO_INNER_TAG_TPID_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_command_info.inner_tag.tpid_index = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_COMMAND_INFO_SET_COMMAND_INFO_SET_COMMAND_INFO_TAGS_TO_REMOVE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_COMMAND_INFO_SET_COMMAND_INFO_SET_COMMAND_INFO_TAGS_TO_REMOVE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_command_info.tags_to_remove = (uint8)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_lif_ing_vlan_edit_command_info_set(
    unit,
    prm_command_ndx,
    &prm_command_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_ing_vlan_edit_command_info_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_ing_vlan_edit_command_info_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: command_info_get (section llp_ing_vlan_edit)
 */
int
ui_ppd_api_lif_ing_vlan_edit_command_info_get(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_command_ndx;
  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO
    prm_command_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif_ing_vlan_edit");
  soc_sand_proc_name = "soc_ppd_lif_ing_vlan_edit_command_info_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO_clear(&prm_command_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_COMMAND_INFO_GET_COMMAND_INFO_GET_COMMAND_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_COMMAND_INFO_GET_COMMAND_INFO_GET_COMMAND_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_command_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter command_ndx after command_info_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_lif_ing_vlan_edit_command_info_get(
    unit,
    prm_command_ndx,
    &prm_command_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_ing_vlan_edit_command_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_ing_vlan_edit_command_info_get");
    goto exit;
  }

  send_string_to_screen("--> command_info:", TRUE);
  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO_print(&prm_command_info);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: pcp_map_stag_set (section llp_ing_vlan_edit)
 */
int
ui_ppd_api_lif_ing_vlan_edit_pcp_map_stag_set(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_pcp_profile_ndx;
  SOC_SAND_PP_PCP_UP   
    prm_tag_pcp_ndx;
  SOC_SAND_PP_DEI_CFI   
    prm_tag_dei_ndx;
  SOC_SAND_PP_PCP_UP   
    prm_out_pcp;
  SOC_SAND_PP_DEI_CFI   
    prm_out_dei;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif_ing_vlan_edit");
  soc_sand_proc_name = "soc_ppd_lif_ing_vlan_edit_pcp_map_stag_set";

  unit = soc_ppd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_SET_PCP_MAP_STAG_SET_PCP_PROFILE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_SET_PCP_MAP_STAG_SET_PCP_PROFILE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pcp_profile_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter pcp_profile_ndx after pcp_map_stag_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_SET_PCP_MAP_STAG_SET_TAG_PCP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_SET_PCP_MAP_STAG_SET_TAG_PCP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tag_pcp_ndx = (uint8)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter tag_pcp_ndx after pcp_map_stag_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_SET_PCP_MAP_STAG_SET_TAG_DEI_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_SET_PCP_MAP_STAG_SET_TAG_DEI_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tag_dei_ndx = (uint8)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter tag_dei_ndx after pcp_map_stag_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = soc_ppd_lif_ing_vlan_edit_pcp_map_stag_get(
    unit,
    prm_pcp_profile_ndx,
    prm_tag_pcp_ndx,
    prm_tag_dei_ndx,
    &prm_out_pcp,
    &prm_out_dei
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_ing_vlan_edit_pcp_map_stag_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_ing_vlan_edit_pcp_map_stag_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_SET_PCP_MAP_STAG_SET_OUT_PCP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_SET_PCP_MAP_STAG_SET_OUT_PCP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_out_pcp = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_SET_PCP_MAP_STAG_SET_OUT_DEI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_SET_PCP_MAP_STAG_SET_OUT_DEI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_out_dei = (uint8)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_lif_ing_vlan_edit_pcp_map_stag_set(
    unit,
    prm_pcp_profile_ndx,
    prm_tag_pcp_ndx,
    prm_tag_dei_ndx,
    prm_out_pcp,
    prm_out_dei
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_ing_vlan_edit_pcp_map_stag_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_ing_vlan_edit_pcp_map_stag_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: pcp_map_stag_get (section llp_ing_vlan_edit)
 */
int
ui_ppd_api_lif_ing_vlan_edit_pcp_map_stag_get(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_pcp_profile_ndx;
  SOC_SAND_PP_PCP_UP   
    prm_tag_pcp_ndx;
  SOC_SAND_PP_DEI_CFI   
    prm_tag_dei_ndx;
  SOC_SAND_PP_PCP_UP   
    prm_out_pcp;
  SOC_SAND_PP_DEI_CFI   
    prm_out_dei;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif_ing_vlan_edit");
  soc_sand_proc_name = "soc_ppd_lif_ing_vlan_edit_pcp_map_stag_get";

  unit = soc_ppd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_GET_PCP_MAP_STAG_GET_PCP_PROFILE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_GET_PCP_MAP_STAG_GET_PCP_PROFILE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pcp_profile_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter pcp_profile_ndx after pcp_map_stag_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_GET_PCP_MAP_STAG_GET_TAG_PCP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_GET_PCP_MAP_STAG_GET_TAG_PCP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tag_pcp_ndx = (uint8)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter tag_pcp_ndx after pcp_map_stag_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_GET_PCP_MAP_STAG_GET_TAG_DEI_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_GET_PCP_MAP_STAG_GET_TAG_DEI_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tag_dei_ndx = (uint8)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter tag_dei_ndx after pcp_map_stag_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_lif_ing_vlan_edit_pcp_map_stag_get(
    unit,
    prm_pcp_profile_ndx,
    prm_tag_pcp_ndx,
    prm_tag_dei_ndx,
    &prm_out_pcp,
    &prm_out_dei
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_ing_vlan_edit_pcp_map_stag_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_ing_vlan_edit_pcp_map_stag_get");
    goto exit;
  }

  send_string_to_screen("--> out_pcp:", TRUE);
  cli_out("out_pcp: %u\n",prm_out_pcp);

  send_string_to_screen("--> out_dei:", TRUE);
  cli_out("out_dei: %u\n",prm_out_dei);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: pcp_map_ctag_set (section llp_ing_vlan_edit)
 */
int
ui_ppd_api_lif_ing_vlan_edit_pcp_map_ctag_set(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_pcp_profile_ndx;
  SOC_SAND_PP_PCP_UP   
    prm_tag_up_ndx;
  SOC_SAND_PP_PCP_UP   
    prm_out_pcp;
  SOC_SAND_PP_DEI_CFI   
    prm_out_dei;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif_ing_vlan_edit");
  soc_sand_proc_name = "soc_ppd_lif_ing_vlan_edit_pcp_map_ctag_set";

  unit = soc_ppd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_SET_PCP_MAP_CTAG_SET_PCP_PROFILE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_SET_PCP_MAP_CTAG_SET_PCP_PROFILE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pcp_profile_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter pcp_profile_ndx after pcp_map_ctag_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_SET_PCP_MAP_CTAG_SET_TAG_UP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_SET_PCP_MAP_CTAG_SET_TAG_UP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tag_up_ndx = (uint8)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter tag_up_ndx after pcp_map_ctag_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = soc_ppd_lif_ing_vlan_edit_pcp_map_ctag_get(
    unit,
    prm_pcp_profile_ndx,
    prm_tag_up_ndx,
    &prm_out_pcp,
    &prm_out_dei
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_ing_vlan_edit_pcp_map_ctag_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_ing_vlan_edit_pcp_map_ctag_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_SET_PCP_MAP_CTAG_SET_OUT_PCP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_SET_PCP_MAP_CTAG_SET_OUT_PCP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_out_pcp = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_SET_PCP_MAP_CTAG_SET_OUT_DEI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_SET_PCP_MAP_CTAG_SET_OUT_DEI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_out_dei = (uint8)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_lif_ing_vlan_edit_pcp_map_ctag_set(
    unit,
    prm_pcp_profile_ndx,
    prm_tag_up_ndx,
    prm_out_pcp,
    prm_out_dei
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_ing_vlan_edit_pcp_map_ctag_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_ing_vlan_edit_pcp_map_ctag_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: pcp_map_ctag_get (section llp_ing_vlan_edit)
 */
int
ui_ppd_api_lif_ing_vlan_edit_pcp_map_ctag_get(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_pcp_profile_ndx;
  SOC_SAND_PP_PCP_UP   
    prm_tag_up_ndx;
  SOC_SAND_PP_PCP_UP   
    prm_out_pcp;
  SOC_SAND_PP_DEI_CFI   
    prm_out_dei;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif_ing_vlan_edit");
  soc_sand_proc_name = "soc_ppd_lif_ing_vlan_edit_pcp_map_ctag_get";

  unit = soc_ppd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_GET_PCP_MAP_CTAG_GET_PCP_PROFILE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_GET_PCP_MAP_CTAG_GET_PCP_PROFILE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pcp_profile_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter pcp_profile_ndx after pcp_map_ctag_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_GET_PCP_MAP_CTAG_GET_TAG_UP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_GET_PCP_MAP_CTAG_GET_TAG_UP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tag_up_ndx = (uint8)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter tag_up_ndx after pcp_map_ctag_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_lif_ing_vlan_edit_pcp_map_ctag_get(
    unit,
    prm_pcp_profile_ndx,
    prm_tag_up_ndx,
    &prm_out_pcp,
    &prm_out_dei
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_ing_vlan_edit_pcp_map_ctag_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_ing_vlan_edit_pcp_map_ctag_get");
    goto exit;
  }

  send_string_to_screen("--> out_pcp:", TRUE);
  cli_out("out_pcp: %u\n",prm_out_pcp);

  send_string_to_screen("--> out_dei:", TRUE);
  cli_out("out_dei: %u\n",prm_out_dei);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: pcp_map_untagged_set (section llp_ing_vlan_edit)
 */
int
ui_ppd_api_lif_ing_vlan_edit_pcp_map_untagged_set(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_pcp_profile_ndx;
  SOC_SAND_PP_TC   
    prm_tc_ndx;
  SOC_SAND_PP_DP   
    prm_dp_ndx;
  SOC_SAND_PP_PCP_UP   
    prm_out_pcp;
  SOC_SAND_PP_DEI_CFI   
    prm_out_dei;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif_ing_vlan_edit");
  soc_sand_proc_name = "soc_ppd_lif_ing_vlan_edit_pcp_map_untagged_set";

  unit = soc_ppd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_SET_PCP_MAP_UNTAGGED_SET_PCP_PROFILE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_SET_PCP_MAP_UNTAGGED_SET_PCP_PROFILE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pcp_profile_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter pcp_profile_ndx after pcp_map_untagged_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_SET_PCP_MAP_UNTAGGED_SET_TC_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_SET_PCP_MAP_UNTAGGED_SET_TC_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tc_ndx = (uint8)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter tc_ndx after pcp_map_untagged_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_SET_PCP_MAP_UNTAGGED_SET_DP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_SET_PCP_MAP_UNTAGGED_SET_DP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_dp_ndx = (uint8)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter dp_ndx after pcp_map_untagged_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = soc_ppd_lif_ing_vlan_edit_pcp_map_untagged_get(
    unit,
    prm_pcp_profile_ndx,
    prm_tc_ndx,
    prm_dp_ndx,
    &prm_out_pcp,
    &prm_out_dei
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_ing_vlan_edit_pcp_map_untagged_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_ing_vlan_edit_pcp_map_untagged_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_SET_PCP_MAP_UNTAGGED_SET_OUT_PCP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_SET_PCP_MAP_UNTAGGED_SET_OUT_PCP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_out_pcp = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_SET_PCP_MAP_UNTAGGED_SET_OUT_DEI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_SET_PCP_MAP_UNTAGGED_SET_OUT_DEI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_out_dei = (uint8)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_lif_ing_vlan_edit_pcp_map_untagged_set(
    unit,
    prm_pcp_profile_ndx,
    prm_tc_ndx,
    prm_dp_ndx,
    prm_out_pcp,
    prm_out_dei
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_ing_vlan_edit_pcp_map_untagged_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_ing_vlan_edit_pcp_map_untagged_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: pcp_map_untagged_get (section llp_ing_vlan_edit)
 */
int
ui_ppd_api_lif_ing_vlan_edit_pcp_map_untagged_get(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_pcp_profile_ndx;
  SOC_SAND_PP_TC   
    prm_tc_ndx;
  SOC_SAND_PP_DP   
    prm_dp_ndx;
  SOC_SAND_PP_PCP_UP   
    prm_out_pcp;
  SOC_SAND_PP_DEI_CFI   
    prm_out_dei;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif_ing_vlan_edit");
  soc_sand_proc_name = "soc_ppd_lif_ing_vlan_edit_pcp_map_untagged_get";

  unit = soc_ppd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_GET_PCP_MAP_UNTAGGED_GET_PCP_PROFILE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_GET_PCP_MAP_UNTAGGED_GET_PCP_PROFILE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pcp_profile_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter pcp_profile_ndx after pcp_map_untagged_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_GET_PCP_MAP_UNTAGGED_GET_TC_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_GET_PCP_MAP_UNTAGGED_GET_TC_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tc_ndx = (uint8)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter tc_ndx after pcp_map_untagged_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_GET_PCP_MAP_UNTAGGED_GET_DP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_GET_PCP_MAP_UNTAGGED_GET_DP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_dp_ndx = (uint8)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter dp_ndx after pcp_map_untagged_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_lif_ing_vlan_edit_pcp_map_untagged_get(
    unit,
    prm_pcp_profile_ndx,
    prm_tc_ndx,
    prm_dp_ndx,
    &prm_out_pcp,
    &prm_out_dei
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_ing_vlan_edit_pcp_map_untagged_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_ing_vlan_edit_pcp_map_untagged_get");
    goto exit;
  }

  send_string_to_screen("--> out_pcp:", TRUE);
  cli_out("out_pcp: %u\n",prm_out_pcp);

  send_string_to_screen("--> out_dei:", TRUE);
  cli_out("out_dei: %u\n",prm_out_dei);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: profile_to_pcp_profile_map_set (section llp_ing_vlan_edit)
 */
int
ui_ppd_api_lif_ing_vlan_edit_profile_to_pcp_profile_map_set(
  CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif_ing_vlan_edit");
  soc_sand_proc_name = "soc_ppd_lif_ing_vlan_edit_profile_to_pcp_profile_map_set";

  unit = soc_ppd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PROFILE_TO_PCP_PROFILE_MAP_SET_PROFILE_TO_PCP_PROFILE_MAP_SET_VLAN_EDIT_PROFILE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PROFILE_TO_PCP_PROFILE_MAP_SET_PROFILE_TO_PCP_PROFILE_MAP_SET_VLAN_EDIT_PROFILE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vlan_edit_profile_ndx after profile_to_pcp_profile_map_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PROFILE_TO_PCP_PROFILE_MAP_SET_PROFILE_TO_PCP_PROFILE_MAP_SET_PCP_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PROFILE_TO_PCP_PROFILE_MAP_SET_PROFILE_TO_PCP_PROFILE_MAP_SET_PCP_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PROFILE_TO_PCP_PROFILE_MAP_SET_PROFILE_TO_PCP_PROFILE_MAP_SET_USE_PCP_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PROFILE_TO_PCP_PROFILE_MAP_SET_PROFILE_TO_PCP_PROFILE_MAP_SET_USE_PCP_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: profile_to_pcp_profile_map_get (section llp_ing_vlan_edit)
 */
int
ui_ppd_api_lif_ing_vlan_edit_profile_to_pcp_profile_map_get(
  CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif_ing_vlan_edit");
  soc_sand_proc_name = "soc_ppd_lif_ing_vlan_edit_profile_to_pcp_profile_map_get";

  unit = soc_ppd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PROFILE_TO_PCP_PROFILE_MAP_GET_PROFILE_TO_PCP_PROFILE_MAP_GET_VLAN_EDIT_PROFILE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PROFILE_TO_PCP_PROFILE_MAP_GET_PROFILE_TO_PCP_PROFILE_MAP_GET_VLAN_EDIT_PROFILE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vlan_edit_profile_ndx after profile_to_pcp_profile_map_get***", TRUE);
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Section handler: llp_ing_vlan_edit
 */
int
ui_ppd_api_lif_ing_vlan_edit(
                             CURRENT_LINE *current_line
                             )
{
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif_ing_vlan_edit");

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_COMMAND_ID_SET_COMMAND_ID_SET_ID,1))
  {
    ui_ret = ui_ppd_api_lif_ing_vlan_edit_command_id_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_COMMAND_ID_GET_COMMAND_ID_GET_ID,1))
  {
    ui_ret = ui_ppd_api_lif_ing_vlan_edit_command_id_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_COMMAND_INFO_SET_COMMAND_INFO_SET_ID,1))
  {
    ui_ret = ui_ppd_api_lif_ing_vlan_edit_command_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_COMMAND_INFO_GET_COMMAND_INFO_GET_ID,1))
  {
    ui_ret = ui_ppd_api_lif_ing_vlan_edit_command_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_SET_PCP_MAP_STAG_SET_ID,1))
  {
    ui_ret = ui_ppd_api_lif_ing_vlan_edit_pcp_map_stag_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_GET_PCP_MAP_STAG_GET_ID,1))
  {
    ui_ret = ui_ppd_api_lif_ing_vlan_edit_pcp_map_stag_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_SET_PCP_MAP_CTAG_SET_ID,1))
  {
    ui_ret = ui_ppd_api_lif_ing_vlan_edit_pcp_map_ctag_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_GET_PCP_MAP_CTAG_GET_ID,1))
  {
    ui_ret = ui_ppd_api_lif_ing_vlan_edit_pcp_map_ctag_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_SET_PCP_MAP_UNTAGGED_SET_ID,1))
  {
    ui_ret = ui_ppd_api_lif_ing_vlan_edit_pcp_map_untagged_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_GET_PCP_MAP_UNTAGGED_GET_ID,1))
  {
    ui_ret = ui_ppd_api_lif_ing_vlan_edit_pcp_map_untagged_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after llp_ing_vlan_edit***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
          
#endif /* lif_ing_vlan_edit */ 


#endif /* LINK_PPD_LIBRARIES */ 

