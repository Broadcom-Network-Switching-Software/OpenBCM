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
 

#include <soc/dpp/PPD/ppd_api_eg_vlan_edit.h>                                                               
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_eg_vlan_edit.h>

#if LINK_PPD_LIBRARIES

#ifdef UI_EG_VLAN_EDIT
/********************************************************************
*  Function handler: pep_info_set (section eg_vlan_edit)
 */
int
ui_ppd_api_eg_vlan_edit_pep_info_set(
                                     CURRENT_LINE *current_line
                                     )
{
  uint32
    ret;
  SOC_PPC_PEP_KEY
    prm_pep_key;
  SOC_PPC_EG_VLAN_EDIT_PEP_INFO
    prm_pep_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_vlan_edit");
  soc_sand_proc_name = "soc_ppd_eg_vlan_edit_pep_info_set";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_PEP_KEY_clear(&prm_pep_key);
  SOC_PPC_EG_VLAN_EDIT_PEP_INFO_clear(&prm_pep_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PEP_INFO_SET_PEP_INFO_SET_PEP_KEY_VSI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_PEP_INFO_SET_PEP_INFO_SET_PEP_KEY_VSI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pep_key.vsi = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PEP_INFO_SET_PEP_INFO_SET_PEP_KEY_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_PEP_INFO_SET_PEP_INFO_SET_PEP_KEY_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pep_key.port = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter pep_key after pep_info_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = soc_ppd_eg_vlan_edit_pep_info_get(
    unit,
    &prm_pep_key,
    &prm_pep_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_vlan_edit_pep_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_vlan_edit_pep_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PEP_INFO_SET_PEP_INFO_SET_PEP_INFO_PEP_EDIT_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_PEP_INFO_SET_PEP_INFO_SET_PEP_INFO_PEP_EDIT_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pep_info.pep_edit_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PEP_INFO_SET_PEP_INFO_SET_PEP_INFO_UP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_PEP_INFO_SET_PEP_INFO_SET_PEP_INFO_UP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pep_info.up = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PEP_INFO_SET_PEP_INFO_SET_PEP_INFO_PVID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_PEP_INFO_SET_PEP_INFO_SET_PEP_INFO_PVID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pep_info.pvid = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_eg_vlan_edit_pep_info_set(
    unit,
    &prm_pep_key,
    &prm_pep_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_vlan_edit_pep_info_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_vlan_edit_pep_info_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: pep_info_get (section eg_vlan_edit)
 */
int
ui_ppd_api_eg_vlan_edit_pep_info_get(
                                     CURRENT_LINE *current_line
                                     )
{
  uint32
    ret;
  SOC_PPC_PEP_KEY
    prm_pep_key;
  SOC_PPC_EG_VLAN_EDIT_PEP_INFO
    prm_pep_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_vlan_edit");
  soc_sand_proc_name = "soc_ppd_eg_vlan_edit_pep_info_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_PEP_KEY_clear(&prm_pep_key);
  SOC_PPC_EG_VLAN_EDIT_PEP_INFO_clear(&prm_pep_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PEP_INFO_GET_PEP_INFO_GET_PEP_KEY_VSI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_PEP_INFO_GET_PEP_INFO_GET_PEP_KEY_VSI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pep_key.vsi = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PEP_INFO_GET_PEP_INFO_GET_PEP_KEY_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_PEP_INFO_GET_PEP_INFO_GET_PEP_KEY_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pep_key.port = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter pep_key after pep_info_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_eg_vlan_edit_pep_info_get(
    unit,
    &prm_pep_key,
    &prm_pep_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_vlan_edit_pep_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_vlan_edit_pep_info_get");
    goto exit;
  }

  send_string_to_screen("--> pep_info:", TRUE);
  SOC_PPC_EG_VLAN_EDIT_PEP_INFO_print(&prm_pep_info);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: command_info_set (section eg_vlan_edit)
 */
int
ui_ppd_api_eg_vlan_edit_command_info_set(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_PPC_EG_VLAN_EDIT_COMMAND_KEY
    prm_command_key;
  SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO
    prm_command_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_vlan_edit");
  soc_sand_proc_name = "soc_ppd_eg_vlan_edit_command_info_set";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_EG_VLAN_EDIT_COMMAND_KEY_clear(&prm_command_key);
  SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO_clear(&prm_command_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_COMMAND_INFO_SET_COMMAND_INFO_SET_COMMAND_KEY_EDIT_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_COMMAND_INFO_SET_COMMAND_INFO_SET_COMMAND_KEY_EDIT_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_command_key.edit_profile = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_COMMAND_INFO_SET_COMMAND_INFO_SET_COMMAND_KEY_TAG_FORMAT_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_COMMAND_INFO_SET_COMMAND_INFO_SET_COMMAND_KEY_TAG_FORMAT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_command_key.tag_format = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter command_key after command_info_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = soc_ppd_eg_vlan_edit_command_info_get(
    unit,
    &prm_command_key,
    &prm_command_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_vlan_edit_command_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_vlan_edit_command_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_COMMAND_INFO_SET_COMMAND_INFO_SET_COMMAND_INFO_CEP_EDITING_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_COMMAND_INFO_SET_COMMAND_INFO_SET_COMMAND_INFO_CEP_EDITING_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_command_info.cep_editing = (uint8)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_COMMAND_INFO_SET_COMMAND_INFO_SET_COMMAND_INFO_OUTER_TAG_PCP_DEI_SOURCE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_COMMAND_INFO_SET_COMMAND_INFO_SET_COMMAND_INFO_OUTER_TAG_PCP_DEI_SOURCE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_command_info.outer_tag.pcp_dei_source = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_COMMAND_INFO_SET_COMMAND_INFO_SET_COMMAND_INFO_OUTER_TAG_VID_SOURCE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_COMMAND_INFO_SET_COMMAND_INFO_SET_COMMAND_INFO_OUTER_TAG_VID_SOURCE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_command_info.outer_tag.vid_source = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_COMMAND_INFO_SET_COMMAND_INFO_SET_COMMAND_INFO_OUTER_TAG_TPID_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_COMMAND_INFO_SET_COMMAND_INFO_SET_COMMAND_INFO_OUTER_TAG_TPID_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_command_info.outer_tag.tpid_index = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_COMMAND_INFO_SET_COMMAND_INFO_SET_COMMAND_INFO_INNER_TAG_PCP_DEI_SOURCE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_COMMAND_INFO_SET_COMMAND_INFO_SET_COMMAND_INFO_INNER_TAG_PCP_DEI_SOURCE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_command_info.inner_tag.pcp_dei_source = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_COMMAND_INFO_SET_COMMAND_INFO_SET_COMMAND_INFO_INNER_TAG_VID_SOURCE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_COMMAND_INFO_SET_COMMAND_INFO_SET_COMMAND_INFO_INNER_TAG_VID_SOURCE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_command_info.inner_tag.vid_source = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_COMMAND_INFO_SET_COMMAND_INFO_SET_COMMAND_INFO_INNER_TAG_TPID_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_COMMAND_INFO_SET_COMMAND_INFO_SET_COMMAND_INFO_INNER_TAG_TPID_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_command_info.inner_tag.tpid_index = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_COMMAND_INFO_SET_COMMAND_INFO_SET_COMMAND_INFO_TAGS_TO_REMOVE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_COMMAND_INFO_SET_COMMAND_INFO_SET_COMMAND_INFO_TAGS_TO_REMOVE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_command_info.tags_to_remove = (uint8)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_eg_vlan_edit_command_info_set(
    unit,
    &prm_command_key,
    &prm_command_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_vlan_edit_command_info_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_vlan_edit_command_info_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: command_info_get (section eg_vlan_edit)
 */
int
ui_ppd_api_eg_vlan_edit_command_info_get(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_PPC_EG_VLAN_EDIT_COMMAND_KEY
    prm_command_key;
  SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO
    prm_command_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_vlan_edit");
  soc_sand_proc_name = "soc_ppd_eg_vlan_edit_command_info_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_EG_VLAN_EDIT_COMMAND_KEY_clear(&prm_command_key);
  SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO_clear(&prm_command_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_COMMAND_INFO_GET_COMMAND_INFO_GET_COMMAND_KEY_EDIT_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_COMMAND_INFO_GET_COMMAND_INFO_GET_COMMAND_KEY_EDIT_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_command_key.edit_profile = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_COMMAND_INFO_GET_COMMAND_INFO_GET_COMMAND_KEY_TAG_FORMAT_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_COMMAND_INFO_GET_COMMAND_INFO_GET_COMMAND_KEY_TAG_FORMAT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_command_key.tag_format = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter command_key after command_info_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_eg_vlan_edit_command_info_get(
    unit,
    &prm_command_key,
    &prm_command_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_vlan_edit_command_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_vlan_edit_command_info_get");
    goto exit;
  }

  send_string_to_screen("--> command_info:", TRUE);
  SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO_print(&prm_command_info);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: port_vlan_transmit_outer_tag_set (section eg_vlan_edit)
 */
int
ui_ppd_api_eg_vlan_edit_port_vlan_transmit_outer_tag_set(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_PPC_PORT   
    prm_local_port_ndx;
  SOC_SAND_PP_VLAN_ID   
    prm_vid_ndx;
  uint8
    prm_transmit_outer_tag;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_vlan_edit");
  soc_sand_proc_name = "soc_ppd_eg_vlan_edit_port_vlan_transmit_outer_tag_set";

  unit = soc_ppd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PORT_VLAN_TRANSMIT_OUTER_TAG_SET_PORT_VLAN_TRANSMIT_OUTER_TAG_SET_LOCAL_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_PORT_VLAN_TRANSMIT_OUTER_TAG_SET_PORT_VLAN_TRANSMIT_OUTER_TAG_SET_LOCAL_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_local_port_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter local_port_ndx after port_vlan_transmit_outer_tag_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PORT_VLAN_TRANSMIT_OUTER_TAG_SET_PORT_VLAN_TRANSMIT_OUTER_TAG_SET_VID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_PORT_VLAN_TRANSMIT_OUTER_TAG_SET_PORT_VLAN_TRANSMIT_OUTER_TAG_SET_VID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vid_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vid_ndx after port_vlan_transmit_outer_tag_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = soc_ppd_eg_vlan_edit_port_vlan_transmit_outer_tag_get(
    unit,
    SOC_CORE_DEFAULT,
    prm_local_port_ndx,
    prm_vid_ndx,
    &prm_transmit_outer_tag
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_vlan_edit_port_vlan_transmit_outer_tag_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_vlan_edit_port_vlan_transmit_outer_tag_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PORT_VLAN_TRANSMIT_OUTER_TAG_SET_PORT_VLAN_TRANSMIT_OUTER_TAG_SET_TRANSMIT_OUTER_TAG_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_PORT_VLAN_TRANSMIT_OUTER_TAG_SET_PORT_VLAN_TRANSMIT_OUTER_TAG_SET_TRANSMIT_OUTER_TAG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_transmit_outer_tag = (uint8)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_eg_vlan_edit_port_vlan_transmit_outer_tag_set(
    unit,
    SOC_CORE_DEFAULT,
    prm_local_port_ndx,
    prm_vid_ndx,
    prm_transmit_outer_tag
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_vlan_edit_port_vlan_transmit_outer_tag_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_vlan_edit_port_vlan_transmit_outer_tag_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: port_vlan_transmit_outer_tag_get (section eg_vlan_edit)
 */
int
ui_ppd_api_eg_vlan_edit_port_vlan_transmit_outer_tag_get(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_PPC_PORT   
    prm_local_port_ndx;
  SOC_SAND_PP_VLAN_ID   
    prm_vid_ndx;
  uint8
    prm_transmit_outer_tag;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_vlan_edit");
  soc_sand_proc_name = "soc_ppd_eg_vlan_edit_port_vlan_transmit_outer_tag_get";

  unit = soc_ppd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PORT_VLAN_TRANSMIT_OUTER_TAG_GET_PORT_VLAN_TRANSMIT_OUTER_TAG_GET_LOCAL_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_PORT_VLAN_TRANSMIT_OUTER_TAG_GET_PORT_VLAN_TRANSMIT_OUTER_TAG_GET_LOCAL_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_local_port_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter local_port_ndx after port_vlan_transmit_outer_tag_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PORT_VLAN_TRANSMIT_OUTER_TAG_GET_PORT_VLAN_TRANSMIT_OUTER_TAG_GET_VID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_PORT_VLAN_TRANSMIT_OUTER_TAG_GET_PORT_VLAN_TRANSMIT_OUTER_TAG_GET_VID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vid_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vid_ndx after port_vlan_transmit_outer_tag_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_eg_vlan_edit_port_vlan_transmit_outer_tag_get(
    unit,
    SOC_CORE_DEFAULT,
    prm_local_port_ndx,
    prm_vid_ndx,
    &prm_transmit_outer_tag
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_vlan_edit_port_vlan_transmit_outer_tag_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_vlan_edit_port_vlan_transmit_outer_tag_get");
    goto exit;
  }

  send_string_to_screen("--> transmit_outer_tag:", TRUE);
  cli_out("transmit_outer_tag: %u\n",prm_transmit_outer_tag);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: pcp_profile_set (section eg_vlan_edit)
 */
int
ui_ppd_api_eg_vlan_edit_pcp_profile_info_set(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_pcp_profile_ndx;
  SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY
    prm_map_key;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_vlan_edit");
  soc_sand_proc_name = "soc_ppd_eg_vlan_edit_pcp_profile_info_set";

  unit = soc_ppd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_PROFILE_INFO_SET_PCP_PROFILE_SET_PCP_PROFILE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_PROFILE_INFO_SET_PCP_PROFILE_SET_PCP_PROFILE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pcp_profile_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter pcp_profile_ndx after pcp_profile_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = soc_ppd_eg_vlan_edit_pcp_profile_info_get(
    unit,
    prm_pcp_profile_ndx,
    &prm_map_key
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_vlan_edit_pcp_profile_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_vlan_edit_pcp_profile_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_PROFILE_INFO_SET_PCP_PROFILE_SET_MAP_KEY_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_PROFILE_INFO_SET_PCP_PROFILE_SET_MAP_KEY_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_map_key = param_val->numeric_equivalent;
  }

  /* Call function */
  ret = soc_ppd_eg_vlan_edit_pcp_profile_info_set(
    unit,
    prm_pcp_profile_ndx,
    prm_map_key
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_vlan_edit_pcp_profile_info_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_vlan_edit_pcp_profile_info_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: pcp_profile_get (section eg_vlan_edit)
 */
int
ui_ppd_api_eg_vlan_edit_pcp_profile_info_get(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_pcp_profile_ndx;
  SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY
    prm_map_key;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_vlan_edit");
  soc_sand_proc_name = "soc_ppd_eg_vlan_edit_pcp_profile_info_get";

  unit = soc_ppd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_PROFILE_INFO_GET_PCP_PROFILE_GET_PCP_PROFILE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_PROFILE_INFO_GET_PCP_PROFILE_GET_PCP_PROFILE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pcp_profile_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter pcp_profile_ndx after pcp_profile_get***", TRUE);
    goto exit;
  }
  /*
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_PROFILE_INFO_GET_PCP_PROFILE_GET_MAP_KEY_ID,1))
  {
  UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_PROFILE_INFO_GET_PCP_PROFILE_GET_MAP_KEY_ID);
  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  prm_map_key = param_val->numeric_equivalent;
  }
  else
  {
  send_string_to_screen(" *** SW error - expecting parameter map_key after pcp_profile_get***", TRUE);
  goto exit;
  }
  */

  /* Call function */
  ret = soc_ppd_eg_vlan_edit_pcp_profile_info_get(
    unit,
    prm_pcp_profile_ndx,
    &prm_map_key
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_vlan_edit_pcp_profile_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_vlan_edit_pcp_profile_info_get");
    goto exit;
  }

  cli_out("prm_map_key: %s\n",SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY_to_string(prm_map_key));

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: pcp_map_stag_set (section eg_vlan_edit)
 */
int
ui_ppd_api_eg_vlan_edit_pcp_map_stag_set(
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

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_vlan_edit");
  soc_sand_proc_name = "soc_ppd_eg_vlan_edit_pcp_map_stag_set";

  unit = soc_ppd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_STAG_SET_PCP_MAP_STAG_SET_PCP_PROFILE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_STAG_SET_PCP_MAP_STAG_SET_PCP_PROFILE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pcp_profile_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter pcp_profile_ndx after pcp_map_stag_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_STAG_SET_PCP_MAP_STAG_SET_TAG_PCP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_STAG_SET_PCP_MAP_STAG_SET_TAG_PCP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tag_pcp_ndx = (uint8)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter tag_pcp_ndx after pcp_map_stag_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_STAG_SET_PCP_MAP_STAG_SET_TAG_DEI_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_STAG_SET_PCP_MAP_STAG_SET_TAG_DEI_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tag_dei_ndx = (uint8)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter tag_dei_ndx after pcp_map_stag_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = soc_ppd_eg_vlan_edit_pcp_map_stag_get(
    unit,
    prm_pcp_profile_ndx,
    prm_tag_pcp_ndx,
    prm_tag_dei_ndx,
    &prm_out_pcp,
    &prm_out_dei
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_vlan_edit_pcp_map_stag_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_vlan_edit_pcp_map_stag_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_STAG_SET_PCP_MAP_STAG_SET_OUT_PCP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_STAG_SET_PCP_MAP_STAG_SET_OUT_PCP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_out_pcp = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_STAG_SET_PCP_MAP_STAG_SET_OUT_DEI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_STAG_SET_PCP_MAP_STAG_SET_OUT_DEI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_out_dei = (uint8)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_eg_vlan_edit_pcp_map_stag_set(
    unit,
    prm_pcp_profile_ndx,
    prm_tag_pcp_ndx,
    prm_tag_dei_ndx,
    prm_out_pcp,
    prm_out_dei
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_vlan_edit_pcp_map_stag_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_vlan_edit_pcp_map_stag_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: pcp_map_stag_get (section eg_vlan_edit)
 */
int
ui_ppd_api_eg_vlan_edit_pcp_map_stag_get(
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

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_vlan_edit");
  soc_sand_proc_name = "soc_ppd_eg_vlan_edit_pcp_map_stag_get";

  unit = soc_ppd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_STAG_GET_PCP_MAP_STAG_GET_PCP_PROFILE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_STAG_GET_PCP_MAP_STAG_GET_PCP_PROFILE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pcp_profile_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter pcp_profile_ndx after pcp_map_stag_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_STAG_GET_PCP_MAP_STAG_GET_TAG_PCP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_STAG_GET_PCP_MAP_STAG_GET_TAG_PCP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tag_pcp_ndx = (uint8)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter tag_pcp_ndx after pcp_map_stag_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_STAG_GET_PCP_MAP_STAG_GET_TAG_DEI_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_STAG_GET_PCP_MAP_STAG_GET_TAG_DEI_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tag_dei_ndx = (uint8)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter tag_dei_ndx after pcp_map_stag_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_eg_vlan_edit_pcp_map_stag_get(
    unit,
    prm_pcp_profile_ndx,
    prm_tag_pcp_ndx,
    prm_tag_dei_ndx,
    &prm_out_pcp,
    &prm_out_dei
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_vlan_edit_pcp_map_stag_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_vlan_edit_pcp_map_stag_get");
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
*  Function handler: pcp_map_ctag_set (section eg_vlan_edit)
 */
int
ui_ppd_api_eg_vlan_edit_pcp_map_ctag_set(
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

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_vlan_edit");
  soc_sand_proc_name = "soc_ppd_eg_vlan_edit_pcp_map_ctag_set";

  unit = soc_ppd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_CTAG_SET_PCP_MAP_CTAG_SET_PCP_PROFILE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_CTAG_SET_PCP_MAP_CTAG_SET_PCP_PROFILE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pcp_profile_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter pcp_profile_ndx after pcp_map_ctag_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_CTAG_SET_PCP_MAP_CTAG_SET_TAG_UP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_CTAG_SET_PCP_MAP_CTAG_SET_TAG_UP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tag_up_ndx = (uint8)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter tag_up_ndx after pcp_map_ctag_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = soc_ppd_eg_vlan_edit_pcp_map_ctag_get(
    unit,
    prm_pcp_profile_ndx,
    prm_tag_up_ndx,
    &prm_out_pcp,
    &prm_out_dei
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_vlan_edit_pcp_map_ctag_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_vlan_edit_pcp_map_ctag_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_CTAG_SET_PCP_MAP_CTAG_SET_OUT_PCP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_CTAG_SET_PCP_MAP_CTAG_SET_OUT_PCP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_out_pcp = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_CTAG_SET_PCP_MAP_CTAG_SET_OUT_DEI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_CTAG_SET_PCP_MAP_CTAG_SET_OUT_DEI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_out_dei = (uint8)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_eg_vlan_edit_pcp_map_ctag_set(
    unit,
    prm_pcp_profile_ndx,
    prm_tag_up_ndx,
    prm_out_pcp,
    prm_out_dei
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_vlan_edit_pcp_map_ctag_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_vlan_edit_pcp_map_ctag_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: pcp_map_ctag_get (section eg_vlan_edit)
 */
int
ui_ppd_api_eg_vlan_edit_pcp_map_ctag_get(
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

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_vlan_edit");
  soc_sand_proc_name = "soc_ppd_eg_vlan_edit_pcp_map_ctag_get";

  unit = soc_ppd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_CTAG_GET_PCP_MAP_CTAG_GET_PCP_PROFILE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_CTAG_GET_PCP_MAP_CTAG_GET_PCP_PROFILE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pcp_profile_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter pcp_profile_ndx after pcp_map_ctag_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_CTAG_GET_PCP_MAP_CTAG_GET_TAG_UP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_CTAG_GET_PCP_MAP_CTAG_GET_TAG_UP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tag_up_ndx = (uint8)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter tag_up_ndx after pcp_map_ctag_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_eg_vlan_edit_pcp_map_ctag_get(
    unit,
    prm_pcp_profile_ndx,
    prm_tag_up_ndx,
    &prm_out_pcp,
    &prm_out_dei
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_vlan_edit_pcp_map_ctag_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_vlan_edit_pcp_map_ctag_get");
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
*  Function handler: pcp_map_untagged_set (section eg_vlan_edit)
 */
int
ui_ppd_api_eg_vlan_edit_pcp_map_untagged_set(
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

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_vlan_edit");
  soc_sand_proc_name = "soc_ppd_eg_vlan_edit_pcp_map_untagged_set";

  unit = soc_ppd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_SET_PCP_MAP_UNTAGGED_SET_PCP_PROFILE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_SET_PCP_MAP_UNTAGGED_SET_PCP_PROFILE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pcp_profile_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter pcp_profile_ndx after pcp_map_untagged_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_SET_PCP_MAP_UNTAGGED_SET_TC_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_SET_PCP_MAP_UNTAGGED_SET_TC_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tc_ndx = (uint8)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter tc_ndx after pcp_map_untagged_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_SET_PCP_MAP_UNTAGGED_SET_DP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_SET_PCP_MAP_UNTAGGED_SET_DP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_dp_ndx = (uint8)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter dp_ndx after pcp_map_untagged_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = soc_ppd_eg_vlan_edit_pcp_map_untagged_get(
    unit,
    prm_pcp_profile_ndx,
    prm_tc_ndx,
    prm_dp_ndx,
    &prm_out_pcp,
    &prm_out_dei
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_vlan_edit_pcp_map_untagged_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_vlan_edit_pcp_map_untagged_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_SET_PCP_MAP_UNTAGGED_SET_OUT_PCP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_SET_PCP_MAP_UNTAGGED_SET_OUT_PCP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_out_pcp = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_SET_PCP_MAP_UNTAGGED_SET_OUT_DEI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_SET_PCP_MAP_UNTAGGED_SET_OUT_DEI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_out_dei = (uint8)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_eg_vlan_edit_pcp_map_untagged_set(
    unit,
    prm_pcp_profile_ndx,
    prm_tc_ndx,
    prm_dp_ndx,
    prm_out_pcp,
    prm_out_dei
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_vlan_edit_pcp_map_untagged_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_vlan_edit_pcp_map_untagged_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: pcp_map_untagged_get (section eg_vlan_edit)
 */
int
ui_ppd_api_eg_vlan_edit_pcp_map_untagged_get(
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

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_vlan_edit");
  soc_sand_proc_name = "soc_ppd_eg_vlan_edit_pcp_map_untagged_get";

  unit = soc_ppd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_GET_PCP_MAP_UNTAGGED_GET_PCP_PROFILE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_GET_PCP_MAP_UNTAGGED_GET_PCP_PROFILE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pcp_profile_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter pcp_profile_ndx after pcp_map_untagged_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_GET_PCP_MAP_UNTAGGED_GET_TC_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_GET_PCP_MAP_UNTAGGED_GET_TC_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tc_ndx = (uint8)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter tc_ndx after pcp_map_untagged_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_GET_PCP_MAP_UNTAGGED_GET_DP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_GET_PCP_MAP_UNTAGGED_GET_DP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_dp_ndx = (uint8)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter dp_ndx after pcp_map_untagged_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_eg_vlan_edit_pcp_map_untagged_get(
    unit,
    prm_pcp_profile_ndx,
    prm_tc_ndx,
    prm_dp_ndx,
    &prm_out_pcp,
    &prm_out_dei
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_vlan_edit_pcp_map_untagged_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_vlan_edit_pcp_map_untagged_get");
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
*  Function handler: pcp_map_dscp_exp_set (section eg_vlan_edit)
 */
int
ui_ppd_api_eg_vlan_edit_pcp_map_dscp_exp_set(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_PPC_PKT_FRWRD_TYPE                  
    prm_pkt_type_ndx;
  uint8 
    prm_dscp_exp_ndx;
  SOC_SAND_PP_PCP_UP   
    prm_out_pcp;
  SOC_SAND_PP_DEI_CFI   
    prm_out_dei;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_vlan_edit");
  soc_sand_proc_name = "soc_ppd_eg_vlan_edit_pcp_map_dscp_exp_set";

  unit = soc_ppd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_DSCP_EXP_SET_PCP_MAP_DSCP_EXP_SET_PKT_TYPE_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_DSCP_EXP_SET_PCP_MAP_DSCP_EXP_SET_PKT_TYPE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pkt_type_ndx = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter pkt_type_ndx after pcp_map_dscp_exp_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_DSCP_EXP_SET_PCP_MAP_DSCP_EXP_SET_DSCP_EXP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_DSCP_EXP_SET_PCP_MAP_DSCP_EXP_SET_DSCP_EXP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_dscp_exp_ndx = (uint8)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter dscp_exp_ndx after pcp_map_dscp_exp_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = soc_ppd_eg_vlan_edit_pcp_map_dscp_exp_get(
    unit,
    prm_pkt_type_ndx,
    prm_dscp_exp_ndx,
    &prm_out_pcp,
    &prm_out_dei
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_vlan_edit_pcp_map_dscp_exp_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_vlan_edit_pcp_map_dscp_exp_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_DSCP_EXP_SET_PCP_MAP_DSCP_EXP_SET_OUT_PCP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_DSCP_EXP_SET_PCP_MAP_DSCP_EXP_SET_OUT_PCP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_out_pcp = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_DSCP_EXP_SET_PCP_MAP_DSCP_EXP_SET_OUT_DEI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_DSCP_EXP_SET_PCP_MAP_DSCP_EXP_SET_OUT_DEI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_out_dei = (uint8)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_eg_vlan_edit_pcp_map_dscp_exp_set(
    unit,
    prm_pkt_type_ndx,
    prm_dscp_exp_ndx,
    prm_out_pcp,
    prm_out_dei
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_vlan_edit_pcp_map_dscp_exp_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_vlan_edit_pcp_map_dscp_exp_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: pcp_map_dscp_exp_get (section eg_vlan_edit)
 */
int
ui_ppd_api_eg_vlan_edit_pcp_map_dscp_exp_get(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_PPC_PKT_FRWRD_TYPE
    prm_pkt_type_ndx;
  uint8
    prm_dscp_exp_ndx;
  SOC_SAND_PP_PCP_UP   
    prm_out_pcp;
  SOC_SAND_PP_DEI_CFI   
    prm_out_dei;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_vlan_edit");
  soc_sand_proc_name = "soc_ppd_eg_vlan_edit_pcp_map_dscp_exp_get";

  unit = soc_ppd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_DSCP_EXP_GET_PCP_MAP_DSCP_EXP_GET_PKT_TYPE_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_DSCP_EXP_GET_PCP_MAP_DSCP_EXP_GET_PKT_TYPE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pkt_type_ndx = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter pkt_type_ndx after pcp_map_dscp_exp_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_DSCP_EXP_GET_PCP_MAP_DSCP_EXP_GET_DSCP_EXP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_DSCP_EXP_GET_PCP_MAP_DSCP_EXP_GET_DSCP_EXP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_dscp_exp_ndx = (uint8)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter dscp_exp_ndx after pcp_map_dscp_exp_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_eg_vlan_edit_pcp_map_dscp_exp_get(
    unit,
    prm_pkt_type_ndx,
    prm_dscp_exp_ndx,
    &prm_out_pcp,
    &prm_out_dei
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_vlan_edit_pcp_map_dscp_exp_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_vlan_edit_pcp_map_dscp_exp_get");
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
*  Section handler: eg_vlan_edit
 */
int
ui_ppd_api_eg_vlan_edit(
                        CURRENT_LINE *current_line
                        )
{
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_vlan_edit");

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PEP_INFO_SET_PEP_INFO_SET_ID,1))
  {
    ui_ret = ui_ppd_api_eg_vlan_edit_pep_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PEP_INFO_GET_PEP_INFO_GET_ID,1))
  {
    ui_ret = ui_ppd_api_eg_vlan_edit_pep_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_COMMAND_INFO_SET_COMMAND_INFO_SET_ID,1))
  {
    ui_ret = ui_ppd_api_eg_vlan_edit_command_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_COMMAND_INFO_GET_COMMAND_INFO_GET_ID,1))
  {
    ui_ret = ui_ppd_api_eg_vlan_edit_command_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PORT_VLAN_TRANSMIT_OUTER_TAG_SET_PORT_VLAN_TRANSMIT_OUTER_TAG_SET_ID,1))
  {
    ui_ret = ui_ppd_api_eg_vlan_edit_port_vlan_transmit_outer_tag_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PORT_VLAN_TRANSMIT_OUTER_TAG_GET_PORT_VLAN_TRANSMIT_OUTER_TAG_GET_ID,1))
  {
    ui_ret = ui_ppd_api_eg_vlan_edit_port_vlan_transmit_outer_tag_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_PROFILE_INFO_SET_PCP_PROFILE_SET_ID,1))
  {
    ui_ret = ui_ppd_api_eg_vlan_edit_pcp_profile_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_PROFILE_INFO_GET_PCP_PROFILE_GET_ID,1))
  {
    ui_ret = ui_ppd_api_eg_vlan_edit_pcp_profile_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_STAG_SET_PCP_MAP_STAG_SET_ID,1))
  {
    ui_ret = ui_ppd_api_eg_vlan_edit_pcp_map_stag_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_STAG_GET_PCP_MAP_STAG_GET_ID,1))
  {
    ui_ret = ui_ppd_api_eg_vlan_edit_pcp_map_stag_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_CTAG_SET_PCP_MAP_CTAG_SET_ID,1))
  {
    ui_ret = ui_ppd_api_eg_vlan_edit_pcp_map_ctag_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_CTAG_GET_PCP_MAP_CTAG_GET_ID,1))
  {
    ui_ret = ui_ppd_api_eg_vlan_edit_pcp_map_ctag_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_SET_PCP_MAP_UNTAGGED_SET_ID,1))
  {
    ui_ret = ui_ppd_api_eg_vlan_edit_pcp_map_untagged_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_UNTAGGED_GET_PCP_MAP_UNTAGGED_GET_ID,1))
  {
    ui_ret = ui_ppd_api_eg_vlan_edit_pcp_map_untagged_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_DSCP_EXP_GET_PCP_MAP_DSCP_EXP_GET_ID,1))
  {
    ui_ret = ui_ppd_api_eg_vlan_edit_pcp_map_dscp_exp_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_VLAN_EDIT_PCP_MAP_DSCP_EXP_SET_PCP_MAP_DSCP_EXP_SET_ID,1))
  {
    ui_ret = ui_ppd_api_eg_vlan_edit_pcp_map_dscp_exp_set(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after eg_vlan_edit***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif
#endif /* LINK_PPD_LIBRARIES */ 

