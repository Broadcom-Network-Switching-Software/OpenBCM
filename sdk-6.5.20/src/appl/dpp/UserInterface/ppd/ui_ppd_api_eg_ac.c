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
 

 
#include <soc/dpp/PPD/ppd_api_eg_ac.h>                                                               
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_eg_ac.h>

#if LINK_PPD_LIBRARIES

#ifdef UI_EG_AC
/********************************************************************
*  Function handler: info_set (section eg_ac)
 */
int
ui_ppd_api_eg_ac_info_set(
                          CURRENT_LINE *current_line
                          )
{
  uint32
    ret;
  uint32
    prm_out_ac_ndx;
  SOC_PPC_EG_AC_INFO
    tmp_ac_info,
    prm_ac_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_ac");
  soc_sand_proc_name = "soc_ppd_eg_ac_info_set";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_EG_AC_INFO_clear(&prm_ac_info);
  SOC_PPC_EG_AC_INFO_clear(&tmp_ac_info);
  prm_ac_info.edit_info.nof_tags = 2;

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_INFO_SET_INFO_SET_OUT_AC_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_INFO_SET_INFO_SET_OUT_AC_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_out_ac_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter out_ac_ndx after info_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */ 
  ret = soc_ppd_eg_ac_info_get(
    unit,
    prm_out_ac_ndx,
    &tmp_ac_info
    );

  if (soc_sand_get_error_code_from_error_word(ret) == SOC_SAND_OK)
  {
    prm_ac_info.edit_info.edit_profile = tmp_ac_info.edit_info.edit_profile;
    prm_ac_info.edit_info.nof_tags = tmp_ac_info.edit_info.nof_tags;
    prm_ac_info.edit_info.pcp_profile = tmp_ac_info.edit_info.pcp_profile;
    prm_ac_info.edit_info.vlan_tags[0] = tmp_ac_info.edit_info.vlan_tags[0];
    prm_ac_info.edit_info.vlan_tags[1] = tmp_ac_info.edit_info.vlan_tags[1];
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_INFO_SET_INFO_SET_AC_INFO_EDIT_INFO_PCP_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_INFO_SET_INFO_SET_AC_INFO_EDIT_INFO_PCP_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_info.edit_info.pcp_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_INFO_SET_INFO_SET_AC_INFO_EDIT_INFO_EDIT_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_INFO_SET_INFO_SET_AC_INFO_EDIT_INFO_EDIT_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_info.edit_info.edit_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_INFO_SET_INFO_SET_AC_INFO_EDIT_INFO_OUTER_VLAN_TAGS_DEI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_INFO_SET_INFO_SET_AC_INFO_EDIT_INFO_OUTER_VLAN_TAGS_DEI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_info.edit_info.vlan_tags[0].dei = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_INFO_SET_INFO_SET_AC_INFO_EDIT_INFO_OUTER_VLAN_TAGS_PCP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_INFO_SET_INFO_SET_AC_INFO_EDIT_INFO_OUTER_VLAN_TAGS_PCP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_info.edit_info.vlan_tags[0].pcp = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_INFO_SET_INFO_SET_AC_INFO_EDIT_INFO_OUTER_VLAN_TAGS_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_INFO_SET_INFO_SET_AC_INFO_EDIT_INFO_OUTER_VLAN_TAGS_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_info.edit_info.vlan_tags[0].vid = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_INFO_SET_INFO_SET_AC_INFO_EDIT_INFO_INNER_VLAN_TAGS_DEI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_INFO_SET_INFO_SET_AC_INFO_EDIT_INFO_INNER_VLAN_TAGS_DEI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_info.edit_info.vlan_tags[1].dei = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_INFO_SET_INFO_SET_AC_INFO_EDIT_INFO_INNER_VLAN_TAGS_PCP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_INFO_SET_INFO_SET_AC_INFO_EDIT_INFO_INNER_VLAN_TAGS_PCP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_info.edit_info.vlan_tags[1].pcp = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_INFO_SET_INFO_SET_AC_INFO_EDIT_INFO_INNER_VLAN_TAGS_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_INFO_SET_INFO_SET_AC_INFO_EDIT_INFO_INNER_VLAN_TAGS_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_info.edit_info.vlan_tags[1].vid = (uint32)param_val->value.ulong_value;
    prm_ac_info.edit_info.nof_tags = 2;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_INFO_SET_INFO_SET_AC_INFO_EDIT_INFO_OAM_LIF_SET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_INFO_SET_INFO_SET_AC_INFO_EDIT_INFO_OAM_LIF_SET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_info.edit_info.oam_lif_set = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_INFO_SET_INFO_SET_AC_INFO_EDIT_INFO_NOF_TAGS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_INFO_SET_INFO_SET_AC_INFO_EDIT_INFO_NOF_TAGS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_info.edit_info.nof_tags = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_eg_ac_info_set(
    unit,
    prm_out_ac_ndx,
    &prm_ac_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_ac_info_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_ac_info_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: info_get (section eg_ac)
 */
int
ui_ppd_api_eg_ac_info_get(
                          CURRENT_LINE *current_line
                          )
{
  uint32
    ret;
  SOC_PPC_AC_ID   
    prm_out_ac_ndx;
  SOC_PPC_EG_AC_INFO
    prm_ac_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_ac");
  soc_sand_proc_name = "soc_ppd_eg_ac_info_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_EG_AC_INFO_clear(&prm_ac_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_INFO_GET_INFO_GET_OUT_AC_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_INFO_GET_INFO_GET_OUT_AC_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_out_ac_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter out_ac_ndx after info_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_eg_ac_info_get(
    unit,
    prm_out_ac_ndx,
    &prm_ac_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_ac_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_ac_info_get");
    goto exit;
  }

  send_string_to_screen("--> ac_info:", TRUE);
  SOC_PPC_EG_AC_INFO_print(&prm_ac_info);

  goto exit;
exit:
  return ui_ret;
}
  
/******************************************************************** 
 *  Function handler: mp_info_set (section eg_ac)
 */
int 
  ui_ppd_api_eg_ac_mp_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_AC_ID   
    prm_out_ac_ndx;
  SOC_PPC_EG_AC_MP_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_ac"); 
  soc_sand_proc_name = "soc_ppd_eg_ac_mp_info_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_EG_AC_MP_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_MP_INFO_SET_MP_INFO_SET_OUT_AC_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_MP_INFO_SET_MP_INFO_SET_OUT_AC_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_out_ac_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter out_ac_ndx after mp_info_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_eg_ac_mp_info_get(
          unit,
          prm_out_ac_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_eg_ac_mp_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_ac_mp_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_MP_INFO_SET_MP_INFO_SET_INFO_IS_VALID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_MP_INFO_SET_MP_INFO_SET_INFO_IS_VALID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.is_valid = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_MP_INFO_SET_MP_INFO_SET_INFO_MP_LEVEL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_MP_INFO_SET_MP_INFO_SET_INFO_MP_LEVEL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.mp_level = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_eg_ac_mp_info_set(
          unit,
          prm_out_ac_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_eg_ac_mp_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_ac_mp_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: mp_info_get (section eg_ac)
 */
int 
  ui_ppd_api_eg_ac_mp_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_AC_ID   
    prm_out_ac_ndx;
  SOC_PPC_EG_AC_MP_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_ac"); 
  soc_sand_proc_name = "soc_ppd_eg_ac_mp_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_EG_AC_MP_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_MP_INFO_GET_MP_INFO_GET_OUT_AC_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_MP_INFO_GET_MP_INFO_GET_OUT_AC_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_out_ac_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter out_ac_ndx after mp_info_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_eg_ac_mp_info_get(
          unit,
          prm_out_ac_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_eg_ac_mp_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_ac_mp_info_get");   
    goto exit; 
  } 

  SOC_PPC_EG_AC_MP_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
*  Function handler: port_vsi_info_set (section eg_ac)
 */
int
ui_ppd_api_eg_ac_port_vsi_info_add(
                                   CURRENT_LINE *current_line
                                   )
{
  uint32
    ret;
  SOC_PPC_AC_ID   
    prm_out_ac_ndx = 0;
  SOC_PPC_EG_AC_VBP_KEY
    prm_vbp_key;
  SOC_PPC_EG_AC_INFO
    prm_ac_info;
  SOC_SAND_SUCCESS_FAILURE   
    prm_success;
/*
  uint32   
    prm_out_ac;
  uint8   
    prm_found;
*/

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_ac");
  soc_sand_proc_name = "soc_ppd_eg_ac_port_vsi_info_add"; 

  unit = soc_ppd_get_default_unit();
  SOC_PPC_EG_AC_VBP_KEY_clear(&prm_vbp_key);
  SOC_PPC_EG_AC_INFO_clear(&prm_ac_info);
  prm_ac_info.edit_info.nof_tags = 2;

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_PORT_VSI_INFO_ADD_PORT_VSI_INFO_ADD_OUT_AC_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_PORT_VSI_INFO_ADD_PORT_VSI_INFO_ADD_OUT_AC_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_out_ac_ndx = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_PORT_VSI_INFO_SET_PORT_VSI_INFO_SET_VBP_KEY_VSI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_PORT_VSI_INFO_SET_PORT_VSI_INFO_SET_VBP_KEY_VSI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vbp_key.vsi = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_PORT_VSI_INFO_SET_PORT_VSI_INFO_SET_VBP_KEY_OUT_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_PORT_VSI_INFO_SET_PORT_VSI_INFO_SET_VBP_KEY_OUT_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vbp_key.vlan_domain = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vbp_key after port_vsi_info_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first 
  ret = soc_ppd_eg_ac_port_vsi_info_get(
    unit,
    &prm_vbp_key,
    &prm_out_ac,
    &prm_ac_info,
    &prm_found
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_ac_port_vsi_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_ac_port_vsi_info_get");
    goto exit;
  }*/

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_PORT_VSI_INFO_SET_PORT_VSI_INFO_SET_AC_INFO_EDIT_INFO_PCP_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_PORT_VSI_INFO_SET_PORT_VSI_INFO_SET_AC_INFO_EDIT_INFO_PCP_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_info.edit_info.pcp_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_PORT_VSI_INFO_SET_PORT_VSI_INFO_SET_AC_INFO_EDIT_INFO_EDIT_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_PORT_VSI_INFO_SET_PORT_VSI_INFO_SET_AC_INFO_EDIT_INFO_EDIT_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_info.edit_info.edit_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_PORT_VSI_INFO_SET_PORT_VSI_INFO_SET_AC_INFO_EDIT_INFO_OAM_LIF_SET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_PORT_VSI_INFO_SET_PORT_VSI_INFO_SET_AC_INFO_EDIT_INFO_OAM_LIF_SET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_info.edit_info.oam_lif_set = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_PORT_VSI_INFO_SET_PORT_VSI_INFO_SET_AC_INFO_EDIT_INFO_NOF_TAGS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_PORT_VSI_INFO_SET_PORT_VSI_INFO_SET_AC_INFO_EDIT_INFO_NOF_TAGS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_info.edit_info.nof_tags = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_PORT_VSI_INFO_SET_PORT_VSI_INFO_SET_AC_INFO_EDIT_INFO_VLAN_TAGS_OUTER_DEI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_PORT_VSI_INFO_SET_PORT_VSI_INFO_SET_AC_INFO_EDIT_INFO_VLAN_TAGS_OUTER_DEI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_info.edit_info.vlan_tags[0].dei = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_PORT_VSI_INFO_SET_PORT_VSI_INFO_SET_AC_INFO_EDIT_INFO_VLAN_TAGS_OUTER_PCP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_PORT_VSI_INFO_SET_PORT_VSI_INFO_SET_AC_INFO_EDIT_INFO_VLAN_TAGS_OUTER_PCP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_info.edit_info.vlan_tags[0].pcp = (uint8)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_PORT_VSI_INFO_SET_PORT_VSI_INFO_SET_AC_INFO_EDIT_INFO_VLAN_TAGS_OUTER_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_PORT_VSI_INFO_SET_PORT_VSI_INFO_SET_AC_INFO_EDIT_INFO_VLAN_TAGS_OUTER_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_info.edit_info.vlan_tags[0].vid = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_PORT_VSI_INFO_SET_PORT_VSI_INFO_SET_AC_INFO_EDIT_INFO_VLAN_TAGS_INNER_DEI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_PORT_VSI_INFO_SET_PORT_VSI_INFO_SET_AC_INFO_EDIT_INFO_VLAN_TAGS_INNER_DEI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_info.edit_info.vlan_tags[1].dei = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_PORT_VSI_INFO_SET_PORT_VSI_INFO_SET_AC_INFO_EDIT_INFO_VLAN_TAGS_INNER_PCP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_PORT_VSI_INFO_SET_PORT_VSI_INFO_SET_AC_INFO_EDIT_INFO_VLAN_TAGS_INNER_PCP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_info.edit_info.vlan_tags[1].pcp = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_PORT_VSI_INFO_SET_PORT_VSI_INFO_SET_AC_INFO_EDIT_INFO_VLAN_TAGS_INNER_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_PORT_VSI_INFO_SET_PORT_VSI_INFO_SET_AC_INFO_EDIT_INFO_VLAN_TAGS_INNER_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_info.edit_info.vlan_tags[1].vid = (uint32)param_val->value.ulong_value;
    prm_ac_info.edit_info.nof_tags = 2;
  }

  /* Call function */  
  ret = soc_ppd_eg_ac_port_vsi_info_add(
    unit,
    prm_out_ac_ndx,
    &prm_vbp_key,
    &prm_ac_info,
    &prm_success
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_eg_ac_port_vsi_info_add - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_ac_port_vsi_info_add");   
    goto exit;
  }

  cli_out("success: %s\n",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));

  goto exit;
exit:
  return ui_ret;
}
/******************************************************************** 
*  Function handler: port_vsi_info_remove (section eg_ac)
 */
int 
ui_ppd_api_eg_ac_port_vsi_info_remove(
                                      CURRENT_LINE *current_line 
                                      ) 
{   
  uint32 
    ret;   
  SOC_PPC_EG_AC_VBP_KEY   
    prm_vbp_key;
  SOC_PPC_AC_ID   
    prm_out_ac;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_ac"); 
  soc_sand_proc_name = "soc_ppd_eg_ac_port_vsi_info_remove"; 

  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_EG_AC_VBP_KEY_clear(&prm_vbp_key);

  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_PORT_VSI_INFO_REMOVE_PORT_VSI_INFO_REMOVE_VBP_KEY_VSI_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_PORT_VSI_INFO_REMOVE_PORT_VSI_INFO_REMOVE_VBP_KEY_VSI_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_vbp_key.vsi = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_PORT_VSI_INFO_REMOVE_PORT_VSI_INFO_REMOVE_VBP_KEY_OUT_PORT_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_PORT_VSI_INFO_REMOVE_PORT_VSI_INFO_REMOVE_VBP_KEY_OUT_PORT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_vbp_key.vlan_domain = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter vbp_key after port_vsi_info_remove***", TRUE); 
    goto exit; 
  } 

  /* Call function */  
  ret = soc_ppd_eg_ac_port_vsi_info_remove(
    unit,
    &prm_vbp_key,
    &prm_out_ac
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_eg_ac_port_vsi_info_remove - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_ac_port_vsi_info_remove");   
    goto exit; 
  } 

  cli_out("out_ac: %u\n",prm_out_ac);


  goto exit; 
exit: 
  return ui_ret; 
} 
/********************************************************************
*  Function handler: port_vsi_info_get (section eg_ac)
 */
int
ui_ppd_api_eg_ac_port_vsi_info_get(
                                   CURRENT_LINE *current_line
                                   )
{
  uint32
    ret;
  SOC_PPC_EG_AC_VBP_KEY   
    prm_vbp_key;
  SOC_PPC_AC_ID   
    prm_out_ac;
  SOC_PPC_EG_AC_INFO   
    prm_ac_info;
  uint8   
    prm_found;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_ac");
  soc_sand_proc_name = "soc_ppd_eg_ac_port_vsi_info_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_EG_AC_VBP_KEY_clear(&prm_vbp_key);
  SOC_PPC_EG_AC_INFO_clear(&prm_ac_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_PORT_VSI_INFO_GET_PORT_VSI_INFO_GET_VBP_KEY_VSI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_PORT_VSI_INFO_GET_PORT_VSI_INFO_GET_VBP_KEY_VSI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vbp_key.vsi = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_PORT_VSI_INFO_GET_PORT_VSI_INFO_GET_VBP_KEY_OUT_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_PORT_VSI_INFO_GET_PORT_VSI_INFO_GET_VBP_KEY_OUT_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vbp_key.vlan_domain = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vbp_key after port_vsi_info_get***", TRUE);
    goto exit;
  }

  /* Call function */  
  ret = soc_ppd_eg_ac_port_vsi_info_get(
    unit,
    &prm_vbp_key,
    &prm_out_ac,
    &prm_ac_info,
    &prm_found
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  {
    send_string_to_screen(" *** soc_ppd_eg_ac_port_vsi_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_ac_port_vsi_info_get");
    goto exit;
  }

  cli_out("out_ac: %u\n",prm_out_ac);

  SOC_PPC_EG_AC_INFO_print(&prm_ac_info);

  cli_out("found: %u\n",prm_found);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: port_cvid_info_set (section eg_ac)
 */
int
ui_ppd_api_eg_ac_port_cvid_info_add(
                                    CURRENT_LINE *current_line
                                    )
{
  uint32
    ret;
  SOC_PPC_AC_ID   
    prm_out_ac_ndx = 0;
  SOC_PPC_EG_AC_CEP_PORT_KEY
    prm_cep_key;
  uint32   
    prm_out_ac;
  SOC_PPC_EG_AC_INFO   
    prm_ac_info;
  uint8   
    prm_found;
  SOC_SAND_SUCCESS_FAILURE   
    prm_success;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_ac");
  soc_sand_proc_name = "soc_ppd_eg_ac_port_cvid_info_set";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_EG_AC_CEP_PORT_KEY_clear(&prm_cep_key);
  SOC_PPC_EG_AC_INFO_clear(&prm_ac_info);

  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_PORT_CVID_INFO_ADD_PORT_CVID_INFO_ADD_OUT_AC_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_PORT_CVID_INFO_ADD_PORT_CVID_INFO_ADD_OUT_AC_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_out_ac_ndx = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_PORT_CVID_INFO_SET_PORT_CVID_INFO_SET_CEP_KEY_PEP_EDIT_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_PORT_CVID_INFO_SET_PORT_CVID_INFO_SET_CEP_KEY_PEP_EDIT_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_cep_key.pep_edit_profile = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_PORT_CVID_INFO_SET_PORT_CVID_INFO_SET_CEP_KEY_CVID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_PORT_CVID_INFO_SET_PORT_CVID_INFO_SET_CEP_KEY_CVID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_cep_key.cvid = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_PORT_CVID_INFO_SET_PORT_CVID_INFO_SET_CEP_KEY_OUT_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_PORT_CVID_INFO_SET_PORT_CVID_INFO_SET_CEP_KEY_OUT_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_cep_key.vlan_domain = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter cep_key after port_cvid_info_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = soc_ppd_eg_ac_port_cvid_info_get(
    unit,
    &prm_cep_key,
    &prm_out_ac,
    &prm_ac_info,
    &prm_found
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_ac_port_cvid_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_ac_port_cvid_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_PORT_CVID_INFO_SET_PORT_CVID_INFO_SET_AC_INFO_EDIT_INFO_PCP_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_PORT_CVID_INFO_SET_PORT_CVID_INFO_SET_AC_INFO_EDIT_INFO_PCP_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_info.edit_info.pcp_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_PORT_CVID_INFO_SET_PORT_CVID_INFO_SET_AC_INFO_EDIT_INFO_EDIT_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_PORT_CVID_INFO_SET_PORT_CVID_INFO_SET_AC_INFO_EDIT_INFO_EDIT_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_info.edit_info.edit_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_PORT_CVID_INFO_SET_PORT_CVID_INFO_SET_AC_INFO_EDIT_INFO_VLAN_TAGS_OUTER_DEI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_PORT_CVID_INFO_SET_PORT_CVID_INFO_SET_AC_INFO_EDIT_INFO_VLAN_TAGS_OUTER_DEI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_info.edit_info.vlan_tags[0].dei = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_PORT_CVID_INFO_SET_PORT_CVID_INFO_SET_AC_INFO_EDIT_INFO_VLAN_TAGS_OUTER_PCP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_PORT_CVID_INFO_SET_PORT_CVID_INFO_SET_AC_INFO_EDIT_INFO_VLAN_TAGS_OUTER_PCP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_info.edit_info.vlan_tags[0].pcp = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_PORT_CVID_INFO_SET_PORT_CVID_INFO_SET_AC_INFO_EDIT_INFO_VLAN_TAGS_OUTER_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_PORT_CVID_INFO_SET_PORT_CVID_INFO_SET_AC_INFO_EDIT_INFO_VLAN_TAGS_OUTER_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_info.edit_info.vlan_tags[0].vid = (uint32)param_val->value.ulong_value;
    prm_ac_info.edit_info.nof_tags = 1;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_PORT_CVID_INFO_SET_PORT_CVID_INFO_SET_AC_INFO_EDIT_INFO_VLAN_TAGS_INNER_DEI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_PORT_CVID_INFO_SET_PORT_CVID_INFO_SET_AC_INFO_EDIT_INFO_VLAN_TAGS_INNER_DEI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_info.edit_info.vlan_tags[1].dei = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_PORT_CVID_INFO_SET_PORT_CVID_INFO_SET_AC_INFO_EDIT_INFO_VLAN_TAGS_INNER_PCP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_PORT_CVID_INFO_SET_PORT_CVID_INFO_SET_AC_INFO_EDIT_INFO_VLAN_TAGS_INNER_PCP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_info.edit_info.vlan_tags[1].pcp = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_PORT_CVID_INFO_SET_PORT_CVID_INFO_SET_AC_INFO_EDIT_INFO_VLAN_TAGS_INNER_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_PORT_CVID_INFO_SET_PORT_CVID_INFO_SET_AC_INFO_EDIT_INFO_VLAN_TAGS_INNER_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_info.edit_info.vlan_tags[1].vid = (uint32)param_val->value.ulong_value;
    prm_ac_info.edit_info.nof_tags = 2;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_PORT_CVID_INFO_SET_PORT_CVID_INFO_SET_AC_INFO_EDIT_INFO_OAM_LIF_SET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_PORT_CVID_INFO_SET_PORT_CVID_INFO_SET_AC_INFO_EDIT_INFO_OAM_LIF_SET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_info.edit_info.oam_lif_set = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_PORT_CVID_INFO_SET_PORT_CVID_INFO_SET_AC_INFO_EDIT_INFO_NOF_TAGS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_PORT_CVID_INFO_SET_PORT_CVID_INFO_SET_AC_INFO_EDIT_INFO_NOF_TAGS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_info.edit_info.nof_tags = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_eg_ac_port_cvid_info_add(
    unit,
    prm_out_ac_ndx,
    &prm_cep_key,
    &prm_ac_info,
    &prm_success
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_eg_ac_port_cvid_info_add - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_ac_port_cvid_info_add");   
    goto exit;
  }

  cli_out("success: %s\n",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));

  goto exit;
exit:
  return ui_ret;
}
/******************************************************************** 
*  Function handler: port_cvid_info_remove (section eg_ac)
 */
int 
ui_ppd_api_eg_ac_port_cvid_info_remove(
                                       CURRENT_LINE *current_line 
                                       ) 
{   
  uint32 
    ret;   
  SOC_PPC_EG_AC_CEP_PORT_KEY   
    prm_cep_key;
  SOC_PPC_AC_ID   
    prm_out_ac;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_ac"); 
  soc_sand_proc_name = "soc_ppd_eg_ac_port_cvid_info_remove"; 

  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_EG_AC_CEP_PORT_KEY_clear(&prm_cep_key);

  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_PORT_CVID_INFO_REMOVE_PORT_CVID_INFO_REMOVE_CEP_KEY_PEP_EDIT_PROFILE_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_PORT_CVID_INFO_REMOVE_PORT_CVID_INFO_REMOVE_CEP_KEY_PEP_EDIT_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_cep_key.pep_edit_profile = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_PORT_CVID_INFO_REMOVE_PORT_CVID_INFO_REMOVE_CEP_KEY_CVID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_PORT_CVID_INFO_REMOVE_PORT_CVID_INFO_REMOVE_CEP_KEY_CVID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_cep_key.cvid = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_PORT_CVID_INFO_REMOVE_PORT_CVID_INFO_REMOVE_CEP_KEY_OUT_PORT_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_PORT_CVID_INFO_REMOVE_PORT_CVID_INFO_REMOVE_CEP_KEY_OUT_PORT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_cep_key.vlan_domain = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter cep_key after port_cvid_info_remove***", TRUE); 
    goto exit; 
  } 

  /* Call function */  
  ret = soc_ppd_eg_ac_port_cvid_info_remove(
    unit,
    &prm_cep_key,
    &prm_out_ac
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_eg_ac_port_cvid_info_remove - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_ac_port_cvid_info_remove");   
    goto exit; 
  } 

  cli_out("out_ac: %u\n",prm_out_ac);


  goto exit; 
exit: 
  return ui_ret; 
} 

/******************************************************************** 
*  Function handler: port_cvid_info_get (section eg_ac)
 */
int 
ui_ppd_api_eg_ac_port_cvid_info_get(
                                    CURRENT_LINE *current_line 
                                    ) 
{   
  uint32 
    ret;   
  SOC_PPC_EG_AC_CEP_PORT_KEY   
    prm_cep_key;
  SOC_PPC_AC_ID   
    prm_out_ac;
  SOC_PPC_EG_AC_INFO   
    prm_ac_info;
  uint8   
    prm_found;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_ac");
  soc_sand_proc_name = "soc_ppd_eg_ac_port_cvid_info_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_EG_AC_CEP_PORT_KEY_clear(&prm_cep_key);
  SOC_PPC_EG_AC_INFO_clear(&prm_ac_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_PORT_CVID_INFO_GET_PORT_CVID_INFO_GET_CEP_KEY_PEP_EDIT_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_PORT_CVID_INFO_GET_PORT_CVID_INFO_GET_CEP_KEY_PEP_EDIT_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_cep_key.pep_edit_profile = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_PORT_CVID_INFO_GET_PORT_CVID_INFO_GET_CEP_KEY_CVID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_PORT_CVID_INFO_GET_PORT_CVID_INFO_GET_CEP_KEY_CVID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_cep_key.cvid = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_PORT_CVID_INFO_GET_PORT_CVID_INFO_GET_CEP_KEY_OUT_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_AC_PORT_CVID_INFO_GET_PORT_CVID_INFO_GET_CEP_KEY_OUT_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_cep_key.vlan_domain = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter cep_key after port_cvid_info_get***", TRUE);
    goto exit;
  }

  /* Call function */  
  ret = soc_ppd_eg_ac_port_cvid_info_get(
    unit,
    &prm_cep_key,
    &prm_out_ac,
    &prm_ac_info,
    &prm_found
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_eg_ac_port_cvid_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_ac_port_cvid_info_get");
    goto exit;
  }

  cli_out("out_ac: %u\n",prm_out_ac);

  SOC_PPC_EG_AC_INFO_print(&prm_ac_info);

  cli_out("found: %u\n",prm_found);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Section handler: eg_ac
 */
int
ui_ppd_api_eg_ac(
                 CURRENT_LINE *current_line
                 )
{
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_ac");

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_INFO_SET_INFO_SET_ID,1))
  {
    ui_ret = ui_ppd_api_eg_ac_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_INFO_GET_INFO_GET_ID,1))
  {
    ui_ret = ui_ppd_api_eg_ac_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_MP_INFO_SET_MP_INFO_SET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_eg_ac_mp_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_MP_INFO_GET_MP_INFO_GET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_eg_ac_mp_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_PORT_VSI_INFO_SET_PORT_VSI_INFO_SET_ID,1))
  {
    ui_ret = ui_ppd_api_eg_ac_port_vsi_info_add(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_PORT_VSI_INFO_REMOVE_PORT_VSI_INFO_REMOVE_ID,1)) 
  { 
    ui_ret = ui_ppd_api_eg_ac_port_vsi_info_remove(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_PORT_VSI_INFO_GET_PORT_VSI_INFO_GET_ID,1))
  {
    ui_ret = ui_ppd_api_eg_ac_port_vsi_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_PORT_CVID_INFO_SET_PORT_CVID_INFO_SET_ID,1))
  {
    ui_ret = ui_ppd_api_eg_ac_port_cvid_info_add(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_PORT_CVID_INFO_REMOVE_PORT_CVID_INFO_REMOVE_ID,1)) 
  { 
    ui_ret = ui_ppd_api_eg_ac_port_cvid_info_remove(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_AC_PORT_CVID_INFO_GET_PORT_CVID_INFO_GET_ID,1))
  {
    ui_ret = ui_ppd_api_eg_ac_port_cvid_info_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after eg_ac***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif
#endif /* LINK_PPD_LIBRARIES */ 

