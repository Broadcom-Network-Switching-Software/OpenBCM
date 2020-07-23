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
 
#include <appl/dpp/UserInterface/ui_pure_defi_ppd_api.h> 
 
  
#include <soc/dpp/PPD/ppd_api_eg_filter.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_eg_filter.h>

#include <soc/dpp/mbcm.h>

#if LINK_PPD_LIBRARIES

#ifdef UI_EG_FILTER
/******************************************************************** 
 *  Function handler: port_info_set (section eg_filter)
 */
int 
  ui_ppd_api_eg_filter_port_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32
    enabled = FALSE;
  bcm_port_t
    prm_out_port_ndx;
  SOC_PPC_PORT
    soc_ppd_port;
  SOC_PPC_EG_FILTER_PORT_INFO
    prm_port_info;
  int
    core = 0;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_filter"); 
  soc_sand_proc_name = "soc_ppd_eg_filter_port_info_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_EG_FILTER_PORT_INFO_clear(&prm_port_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_PORT_INFO_SET_PORT_INFO_SET_OUT_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_FILTER_PORT_INFO_SET_PORT_INFO_SET_OUT_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_out_port_ndx = (uint32)param_val->value.ulong_value;
    ret = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, prm_out_port_ndx, &soc_ppd_port, &core));
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
        send_string_to_screen(" *** SW error - fail to get core number from  parameter local_port_ndx***", TRUE);
        goto exit;
    }
  }
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter out_port_ndx after port_info_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_eg_filter_port_info_get(
          unit,
		  core,
          soc_ppd_port,
          &prm_port_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_eg_filter_port_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_filter_port_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_ACCEPTABLE_FRAMES_PROFILE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_FILTER_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_ACCEPTABLE_FRAMES_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_info.acceptable_frames_profile = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_FILTER_MASK_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_EG_FILTER_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_FILTER_MASK_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_info.filter_mask |= (uint32)param_val->numeric_equivalent;
    if (param_val->numeric_equivalent == SOC_PPC_EG_FILTER_PORT_ENABLE_NONE)
    {
      prm_port_info.filter_mask = 0;
    }
    enabled = (uint32)param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_FILTER_MASK2_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_EG_FILTER_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_FILTER_MASK2_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_info.filter_mask &= ~(uint32)param_val->numeric_equivalent;
    if (enabled == (uint32)param_val->numeric_equivalent)
    {
      send_string_to_screen(" *** SW error - same type was enabled and disabled***", TRUE);     
    }
  } 



  /* Call function */
  ret = soc_ppd_eg_filter_port_info_set(
          unit,
          core,
          soc_ppd_port,
          &prm_port_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_eg_filter_port_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_filter_port_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: port_info_get (section eg_filter)
 */
int 
  ui_ppd_api_eg_filter_port_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  bcm_port_t
    prm_out_port_ndx;
  SOC_PPC_PORT
    soc_ppd_port;
  SOC_PPC_EG_FILTER_PORT_INFO
    prm_port_info;
  int
    core = 0;

   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_filter"); 
  soc_sand_proc_name = "soc_ppd_eg_filter_port_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_EG_FILTER_PORT_INFO_clear(&prm_port_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_PORT_INFO_GET_PORT_INFO_GET_OUT_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_FILTER_PORT_INFO_GET_PORT_INFO_GET_OUT_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_out_port_ndx = (uint32)param_val->value.ulong_value;
    ret = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, prm_out_port_ndx, &soc_ppd_port, &core));
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
        send_string_to_screen(" *** SW error - fail to get core number from  parameter local_port_ndx***", TRUE);
        goto exit;
    }
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter out_port_ndx after port_info_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_eg_filter_port_info_get(
          unit,
		  core,
          soc_ppd_port,
          &prm_port_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_eg_filter_port_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_filter_port_info_get");   
    goto exit; 
  } 

  SOC_PPC_EG_FILTER_PORT_INFO_print(&prm_port_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: vsi_port_membership_set (section eg_filter)
 */
int 
  ui_ppd_api_eg_filter_vsi_port_membership_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_VSI_ID   
    prm_vsid_ndx;
  SOC_PPC_PORT   
    prm_out_port_ndx;
  uint8   
    prm_is_member;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_filter"); 
  soc_sand_proc_name = "soc_ppd_eg_filter_vsi_port_membership_set"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_VSI_PORT_MEMBERSHIP_SET_VSI_PORT_MEMBERSHIP_SET_VSID_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_FILTER_VSI_PORT_MEMBERSHIP_SET_VSI_PORT_MEMBERSHIP_SET_VSID_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_vsid_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter vsid_ndx after vsi_port_membership_set***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_VSI_PORT_MEMBERSHIP_SET_VSI_PORT_MEMBERSHIP_SET_OUT_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_FILTER_VSI_PORT_MEMBERSHIP_SET_VSI_PORT_MEMBERSHIP_SET_OUT_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_out_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter out_port_ndx after vsi_port_membership_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_eg_filter_vsi_port_membership_get(
          unit,
          SOC_CORE_DEFAULT,
          prm_vsid_ndx,
          prm_out_port_ndx,
          &prm_is_member
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_eg_filter_vsi_port_membership_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_filter_vsi_port_membership_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_VSI_PORT_MEMBERSHIP_SET_VSI_PORT_MEMBERSHIP_SET_IS_MEMBER_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_FILTER_VSI_PORT_MEMBERSHIP_SET_VSI_PORT_MEMBERSHIP_SET_IS_MEMBER_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_is_member = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_eg_filter_vsi_port_membership_set(
          unit,
          SOC_CORE_DEFAULT,
          prm_vsid_ndx,
          prm_out_port_ndx,
          prm_is_member
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_eg_filter_vsi_port_membership_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_filter_vsi_port_membership_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: vsi_port_membership_get (section eg_filter)
 */
int 
  ui_ppd_api_eg_filter_vsi_port_membership_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_VSI_ID   
    prm_vsid_ndx;
  SOC_PPC_PORT   
    prm_out_port_ndx;
  uint8   
    prm_is_member;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_filter"); 
  soc_sand_proc_name = "soc_ppd_eg_filter_vsi_port_membership_get"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_VSI_PORT_MEMBERSHIP_GET_VSI_PORT_MEMBERSHIP_GET_VSID_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_FILTER_VSI_PORT_MEMBERSHIP_GET_VSI_PORT_MEMBERSHIP_GET_VSID_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_vsid_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter vsid_ndx after vsi_port_membership_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_VSI_PORT_MEMBERSHIP_GET_VSI_PORT_MEMBERSHIP_GET_OUT_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_FILTER_VSI_PORT_MEMBERSHIP_GET_VSI_PORT_MEMBERSHIP_GET_OUT_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_out_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter out_port_ndx after vsi_port_membership_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_eg_filter_vsi_port_membership_get(
          unit,
          SOC_CORE_DEFAULT,
          prm_vsid_ndx,
          prm_out_port_ndx,
          &prm_is_member
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_eg_filter_vsi_port_membership_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_filter_vsi_port_membership_get");   
    goto exit; 
  } 

  cli_out("is_member: %u\n\r",prm_is_member);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: cvid_port_membership_set (section eg_filter)
 */
int 
  ui_ppd_api_eg_filter_cvid_port_membership_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_SAND_PP_VLAN_ID   
    prm_cvid_ndx;
  SOC_PPC_PORT   
    prm_out_port_ndx;
  uint8   
    prm_is_member;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_filter"); 
  soc_sand_proc_name = "soc_ppd_eg_filter_cvid_port_membership_set"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_CVID_PORT_MEMBERSHIP_SET_CVID_PORT_MEMBERSHIP_SET_CVID_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_FILTER_CVID_PORT_MEMBERSHIP_SET_CVID_PORT_MEMBERSHIP_SET_CVID_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_cvid_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter cvid_ndx after cvid_port_membership_set***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_CVID_PORT_MEMBERSHIP_SET_CVID_PORT_MEMBERSHIP_SET_OUT_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_FILTER_CVID_PORT_MEMBERSHIP_SET_CVID_PORT_MEMBERSHIP_SET_OUT_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_out_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter out_port_ndx after cvid_port_membership_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_eg_filter_cvid_port_membership_get(
          unit,
          prm_cvid_ndx,
          prm_out_port_ndx,
          &prm_is_member
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_eg_filter_cvid_port_membership_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_filter_cvid_port_membership_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_CVID_PORT_MEMBERSHIP_SET_CVID_PORT_MEMBERSHIP_SET_IS_MEMBER_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_FILTER_CVID_PORT_MEMBERSHIP_SET_CVID_PORT_MEMBERSHIP_SET_IS_MEMBER_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_is_member = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_eg_filter_cvid_port_membership_set(
          unit,
          prm_cvid_ndx,
          prm_out_port_ndx,
          prm_is_member
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_eg_filter_cvid_port_membership_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_filter_cvid_port_membership_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: cvid_port_membership_get (section eg_filter)
 */
int 
  ui_ppd_api_eg_filter_cvid_port_membership_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_SAND_PP_VLAN_ID   
    prm_cvid_ndx;
  SOC_PPC_PORT   
    prm_out_port_ndx;
  uint8   
    prm_is_member;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_filter"); 
  soc_sand_proc_name = "soc_ppd_eg_filter_cvid_port_membership_get"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_CVID_PORT_MEMBERSHIP_GET_CVID_PORT_MEMBERSHIP_GET_CVID_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_FILTER_CVID_PORT_MEMBERSHIP_GET_CVID_PORT_MEMBERSHIP_GET_CVID_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_cvid_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter cvid_ndx after cvid_port_membership_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_CVID_PORT_MEMBERSHIP_GET_CVID_PORT_MEMBERSHIP_GET_OUT_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_FILTER_CVID_PORT_MEMBERSHIP_GET_CVID_PORT_MEMBERSHIP_GET_OUT_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_out_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter out_port_ndx after cvid_port_membership_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_eg_filter_cvid_port_membership_get(
          unit,
          prm_cvid_ndx,
          prm_out_port_ndx,
          &prm_is_member
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_eg_filter_cvid_port_membership_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_filter_cvid_port_membership_get");   
    goto exit; 
  } 

  cli_out("is_member: %u\n\r",prm_is_member);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: port_acceptable_frames_set (section eg_filter)
 */
int 
  ui_ppd_api_eg_filter_port_acceptable_frames_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_port_acc_frames_profile_ndx;
  SOC_PPC_LLP_PARSE_INFO   
    prm_eg_prsr_out_key;
  uint8   
    prm_accept;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_filter"); 
  soc_sand_proc_name = "soc_ppd_eg_filter_port_acceptable_frames_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_LLP_PARSE_INFO_clear(&prm_eg_prsr_out_key);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_PORT_ACCEPTABLE_FRAMES_SET_PORT_ACCEPTABLE_FRAMES_SET_PORT_ACC_FRAMES_PROFILE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_FILTER_PORT_ACCEPTABLE_FRAMES_SET_PORT_ACCEPTABLE_FRAMES_SET_PORT_ACC_FRAMES_PROFILE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_port_acc_frames_profile_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter port_acc_frames_profile_ndx after port_acceptable_frames_set***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_PORT_ACCEPTABLE_FRAMES_SET_PORT_ACCEPTABLE_FRAMES_SET_EG_PRSR_OUT_KEY_IS_OUTER_PRIO_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_FILTER_PORT_ACCEPTABLE_FRAMES_SET_PORT_ACCEPTABLE_FRAMES_SET_EG_PRSR_OUT_KEY_IS_OUTER_PRIO_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_eg_prsr_out_key.is_outer_prio = (uint8)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_PORT_ACCEPTABLE_FRAMES_SET_PORT_ACCEPTABLE_FRAMES_SET_EG_PRSR_OUT_KEY_INNER_TPID_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_EG_FILTER_PORT_ACCEPTABLE_FRAMES_SET_PORT_ACCEPTABLE_FRAMES_SET_EG_PRSR_OUT_KEY_INNER_TPID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_eg_prsr_out_key.inner_tpid = param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_PORT_ACCEPTABLE_FRAMES_SET_PORT_ACCEPTABLE_FRAMES_SET_EG_PRSR_OUT_KEY_OUTER_TPID_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_EG_FILTER_PORT_ACCEPTABLE_FRAMES_SET_PORT_ACCEPTABLE_FRAMES_SET_EG_PRSR_OUT_KEY_OUTER_TPID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_eg_prsr_out_key.outer_tpid = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter eg_prsr_out_key after port_acceptable_frames_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_eg_filter_port_acceptable_frames_get(
          unit,
          prm_port_acc_frames_profile_ndx,
          &prm_eg_prsr_out_key,
          &prm_accept
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_eg_filter_port_acceptable_frames_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_filter_port_acceptable_frames_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_PORT_ACCEPTABLE_FRAMES_SET_PORT_ACCEPTABLE_FRAMES_SET_ACCEPT_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_FILTER_PORT_ACCEPTABLE_FRAMES_SET_PORT_ACCEPTABLE_FRAMES_SET_ACCEPT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_accept = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_eg_filter_port_acceptable_frames_set(
          unit,
          prm_port_acc_frames_profile_ndx,
          0,
          &prm_eg_prsr_out_key,
          prm_accept
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_eg_filter_port_acceptable_frames_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_filter_port_acceptable_frames_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: port_acceptable_frames_get (section eg_filter)
 */
int 
  ui_ppd_api_eg_filter_port_acceptable_frames_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_port_acc_frames_profile_ndx;
  SOC_PPC_LLP_PARSE_INFO   
    prm_eg_prsr_out_key;
  uint8   
    prm_accept;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_filter"); 
  soc_sand_proc_name = "soc_ppd_eg_filter_port_acceptable_frames_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_LLP_PARSE_INFO_clear(&prm_eg_prsr_out_key);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_PORT_ACCEPTABLE_FRAMES_GET_PORT_ACCEPTABLE_FRAMES_GET_PORT_ACC_FRAMES_PROFILE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_FILTER_PORT_ACCEPTABLE_FRAMES_GET_PORT_ACCEPTABLE_FRAMES_GET_PORT_ACC_FRAMES_PROFILE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_port_acc_frames_profile_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter port_acc_frames_profile_ndx after port_acceptable_frames_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_PORT_ACCEPTABLE_FRAMES_GET_PORT_ACCEPTABLE_FRAMES_GET_EG_PRSR_OUT_KEY_IS_OUTER_PRIO_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_FILTER_PORT_ACCEPTABLE_FRAMES_GET_PORT_ACCEPTABLE_FRAMES_GET_EG_PRSR_OUT_KEY_IS_OUTER_PRIO_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_eg_prsr_out_key.is_outer_prio = (uint8)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_PORT_ACCEPTABLE_FRAMES_GET_PORT_ACCEPTABLE_FRAMES_GET_EG_PRSR_OUT_KEY_INNER_TPID_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_EG_FILTER_PORT_ACCEPTABLE_FRAMES_GET_PORT_ACCEPTABLE_FRAMES_GET_EG_PRSR_OUT_KEY_INNER_TPID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_eg_prsr_out_key.inner_tpid = param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_PORT_ACCEPTABLE_FRAMES_GET_PORT_ACCEPTABLE_FRAMES_GET_EG_PRSR_OUT_KEY_OUTER_TPID_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_EG_FILTER_PORT_ACCEPTABLE_FRAMES_GET_PORT_ACCEPTABLE_FRAMES_GET_EG_PRSR_OUT_KEY_OUTER_TPID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_eg_prsr_out_key.outer_tpid = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter eg_prsr_out_key after port_acceptable_frames_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_eg_filter_port_acceptable_frames_get(
          unit,
          prm_port_acc_frames_profile_ndx,
          &prm_eg_prsr_out_key,
          &prm_accept
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_eg_filter_port_acceptable_frames_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_filter_port_acceptable_frames_get");   
    goto exit; 
  } 

  cli_out("accept: %u\n\r",prm_accept);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: pep_acceptable_frames_set (section eg_filter)
 */
int 
  ui_ppd_api_eg_filter_pep_acceptable_frames_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_PEP_KEY   
    prm_pep_key;
  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT   
    prm_vlan_format_ndx;
  uint8   
    prm_accept;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_filter"); 
  soc_sand_proc_name = "soc_ppd_eg_filter_pep_acceptable_frames_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_PEP_KEY_clear(&prm_pep_key);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_PEP_ACCEPTABLE_FRAMES_SET_PEP_ACCEPTABLE_FRAMES_SET_PEP_KEY_VSI_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_FILTER_PEP_ACCEPTABLE_FRAMES_SET_PEP_ACCEPTABLE_FRAMES_SET_PEP_KEY_VSI_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_pep_key.vsi = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_PEP_ACCEPTABLE_FRAMES_SET_PEP_ACCEPTABLE_FRAMES_SET_PEP_KEY_PORT_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_FILTER_PEP_ACCEPTABLE_FRAMES_SET_PEP_ACCEPTABLE_FRAMES_SET_PEP_KEY_PORT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_pep_key.port = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter pep_key after pep_acceptable_frames_set***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_PEP_ACCEPTABLE_FRAMES_SET_PEP_ACCEPTABLE_FRAMES_SET_VLAN_FORMAT_NDX_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_EG_FILTER_PEP_ACCEPTABLE_FRAMES_SET_PEP_ACCEPTABLE_FRAMES_SET_VLAN_FORMAT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_vlan_format_ndx = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter vlan_format_ndx after pep_acceptable_frames_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_eg_filter_pep_acceptable_frames_get(
          unit,
          &prm_pep_key,
          prm_vlan_format_ndx,
          &prm_accept
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_eg_filter_pep_acceptable_frames_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_filter_pep_acceptable_frames_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_PEP_ACCEPTABLE_FRAMES_SET_PEP_ACCEPTABLE_FRAMES_SET_ACCEPT_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_FILTER_PEP_ACCEPTABLE_FRAMES_SET_PEP_ACCEPTABLE_FRAMES_SET_ACCEPT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_accept = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_eg_filter_pep_acceptable_frames_set(
          unit,
          &prm_pep_key,
          prm_vlan_format_ndx,
          prm_accept
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_eg_filter_pep_acceptable_frames_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_filter_pep_acceptable_frames_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: pep_acceptable_frames_get (section eg_filter)
 */
int 
  ui_ppd_api_eg_filter_pep_acceptable_frames_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_PEP_KEY   
    prm_pep_key;
  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT   
    prm_vlan_format_ndx;
  uint8   
    prm_accept;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_filter"); 
  soc_sand_proc_name = "soc_ppd_eg_filter_pep_acceptable_frames_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_PEP_KEY_clear(&prm_pep_key);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_PEP_ACCEPTABLE_FRAMES_GET_PEP_ACCEPTABLE_FRAMES_GET_PEP_KEY_VSI_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_FILTER_PEP_ACCEPTABLE_FRAMES_GET_PEP_ACCEPTABLE_FRAMES_GET_PEP_KEY_VSI_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_pep_key.vsi = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_PEP_ACCEPTABLE_FRAMES_GET_PEP_ACCEPTABLE_FRAMES_GET_PEP_KEY_PORT_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_FILTER_PEP_ACCEPTABLE_FRAMES_GET_PEP_ACCEPTABLE_FRAMES_GET_PEP_KEY_PORT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_pep_key.port = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter pep_key after pep_acceptable_frames_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_PEP_ACCEPTABLE_FRAMES_GET_PEP_ACCEPTABLE_FRAMES_GET_VLAN_FORMAT_NDX_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_EG_FILTER_PEP_ACCEPTABLE_FRAMES_GET_PEP_ACCEPTABLE_FRAMES_GET_VLAN_FORMAT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_vlan_format_ndx = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter vlan_format_ndx after pep_acceptable_frames_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_eg_filter_pep_acceptable_frames_get(
          unit,
          &prm_pep_key,
          prm_vlan_format_ndx,
          &prm_accept
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_eg_filter_pep_acceptable_frames_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_filter_pep_acceptable_frames_get");   
    goto exit; 
  } 

  cli_out("accept: %u\n\r",prm_accept);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: pvlan_port_type_set (section eg_filter)
 */
int 
  ui_ppd_api_eg_filter_pvlan_port_type_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_SAND_PP_SYS_PORT_ID   
    prm_src_sys_port_ndx;
  SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE   
    prm_pvlan_port_type;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_filter"); 
  soc_sand_proc_name = "soc_ppd_eg_filter_pvlan_port_type_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  soc_sand_SAND_PP_SYS_PORT_ID_clear(&prm_src_sys_port_ndx);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_PVLAN_PORT_TYPE_SET_PVLAN_PORT_TYPE_SET_SRC_SYS_PORT_NDX_SYS_ID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_FILTER_PVLAN_PORT_TYPE_SET_PVLAN_PORT_TYPE_SET_SRC_SYS_PORT_NDX_SYS_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_src_sys_port_ndx.sys_id = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_PVLAN_PORT_TYPE_SET_PVLAN_PORT_TYPE_SET_SRC_SYS_PORT_NDX_SYS_PORT_TYPE_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_EG_FILTER_PVLAN_PORT_TYPE_SET_PVLAN_PORT_TYPE_SET_SRC_SYS_PORT_NDX_SYS_PORT_TYPE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_src_sys_port_ndx.sys_port_type = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter src_sys_port_ndx after pvlan_port_type_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_eg_filter_pvlan_port_type_get(
          unit,
          &prm_src_sys_port_ndx,
          &prm_pvlan_port_type
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_eg_filter_pvlan_port_type_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_filter_pvlan_port_type_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_PVLAN_PORT_TYPE_SET_PVLAN_PORT_TYPE_SET_PVLAN_PORT_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_EG_FILTER_PVLAN_PORT_TYPE_SET_PVLAN_PORT_TYPE_SET_PVLAN_PORT_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_pvlan_port_type = param_val->numeric_equivalent;
  } 


  /* Call function */
  ret = soc_ppd_eg_filter_pvlan_port_type_set(
          unit,
          &prm_src_sys_port_ndx,
          prm_pvlan_port_type
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_eg_filter_pvlan_port_type_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_filter_pvlan_port_type_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: pvlan_port_type_get (section eg_filter)
 */
int 
  ui_ppd_api_eg_filter_pvlan_port_type_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_SAND_PP_SYS_PORT_ID   
    prm_src_sys_port_ndx;
  SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE   
    prm_pvlan_port_type;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_filter"); 
  soc_sand_proc_name = "soc_ppd_eg_filter_pvlan_port_type_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  soc_sand_SAND_PP_SYS_PORT_ID_clear(&prm_src_sys_port_ndx);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_PVLAN_PORT_TYPE_GET_PVLAN_PORT_TYPE_GET_SRC_SYS_PORT_NDX_SYS_ID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_FILTER_PVLAN_PORT_TYPE_GET_PVLAN_PORT_TYPE_GET_SRC_SYS_PORT_NDX_SYS_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_src_sys_port_ndx.sys_id = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_PVLAN_PORT_TYPE_GET_PVLAN_PORT_TYPE_GET_SRC_SYS_PORT_NDX_SYS_PORT_TYPE_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_EG_FILTER_PVLAN_PORT_TYPE_GET_PVLAN_PORT_TYPE_GET_SRC_SYS_PORT_NDX_SYS_PORT_TYPE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_src_sys_port_ndx.sys_port_type = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter src_sys_port_ndx after pvlan_port_type_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_eg_filter_pvlan_port_type_get(
          unit,
          &prm_src_sys_port_ndx,
          &prm_pvlan_port_type
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_eg_filter_pvlan_port_type_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_filter_pvlan_port_type_get");   
    goto exit; 
  } 

  cli_out("pvlan_port_type: %s\n\r",SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_to_string(prm_pvlan_port_type));

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: split_horizon_out_ac_orientation_set (section eg_filter)
 */
int 
  ui_ppd_api_eg_filter_split_horizon_out_ac_orientation_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_AC_ID   
    prm_out_ac_ndx;
  SOC_SAND_PP_HUB_SPOKE_ORIENTATION   
    prm_orientation;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_filter"); 
  soc_sand_proc_name = "soc_ppd_eg_filter_split_horizon_out_ac_orientation_set"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_SET_SPLIT_HORIZON_OUT_AC_ORIENTATION_SET_OUT_AC_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_SET_SPLIT_HORIZON_OUT_AC_ORIENTATION_SET_OUT_AC_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_out_ac_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter out_ac_ndx after split_horizon_out_ac_orientation_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_eg_filter_split_horizon_out_ac_orientation_get(
          unit,
          prm_out_ac_ndx,
          &prm_orientation
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_eg_filter_split_horizon_out_ac_orientation_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_filter_split_horizon_out_ac_orientation_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_SET_SPLIT_HORIZON_OUT_AC_ORIENTATION_SET_ORIENTATION_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_SET_SPLIT_HORIZON_OUT_AC_ORIENTATION_SET_ORIENTATION_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_orientation = param_val->numeric_equivalent;
  } 


  /* Call function */
  ret = soc_ppd_eg_filter_split_horizon_out_ac_orientation_set(
          unit,
          prm_out_ac_ndx,
          prm_orientation
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_eg_filter_split_horizon_out_ac_orientation_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_filter_split_horizon_out_ac_orientation_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: split_horizon_out_ac_orientation_get (section eg_filter)
 */
int 
  ui_ppd_api_eg_filter_split_horizon_out_ac_orientation_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_AC_ID   
    prm_out_ac_ndx;
  SOC_SAND_PP_HUB_SPOKE_ORIENTATION   
    prm_orientation;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_filter"); 
  soc_sand_proc_name = "soc_ppd_eg_filter_split_horizon_out_ac_orientation_get"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_GET_SPLIT_HORIZON_OUT_AC_ORIENTATION_GET_OUT_AC_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_GET_SPLIT_HORIZON_OUT_AC_ORIENTATION_GET_OUT_AC_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_out_ac_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter out_ac_ndx after split_horizon_out_ac_orientation_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_eg_filter_split_horizon_out_ac_orientation_get(
          unit,
          prm_out_ac_ndx,
          &prm_orientation
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_eg_filter_split_horizon_out_ac_orientation_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_filter_split_horizon_out_ac_orientation_get");   
    goto exit; 
  } 

  cli_out("orientation: %s\n\r",soc_sand_SAND_PP_HUB_SPOKE_ORIENTATION_to_string(prm_orientation));

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: split_horizon_out_lif_orientation_set (section eg_filter)
 */
int 
  ui_ppd_api_eg_filter_split_horizon_out_lif_orientation_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32
    prm_out_lif_ndx;
  SOC_SAND_PP_HUB_SPOKE_ORIENTATION   
    prm_orientation;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_filter"); 
  soc_sand_proc_name = "soc_ppd_eg_filter_split_horizon_out_lif_orientation_set"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_SPLIT_HORIZON_OUT_LIF_ORIENTATION_SET_SPLIT_HORIZON_OUT_LIF_ORIENTATION_SET_OUT_LIF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_FILTER_SPLIT_HORIZON_OUT_LIF_ORIENTATION_SET_SPLIT_HORIZON_OUT_LIF_ORIENTATION_SET_OUT_LIF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_out_lif_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter out_lif_ndx after split_horizon_out_lif_orientation_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_eg_filter_split_horizon_out_lif_orientation_get(
          unit,
          prm_out_lif_ndx,
          &prm_orientation
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_eg_filter_split_horizon_out_lif_orientation_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_filter_split_horizon_out_lif_orientation_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_SPLIT_HORIZON_OUT_LIF_ORIENTATION_SET_SPLIT_HORIZON_OUT_LIF_ORIENTATION_SET_ORIENTATION_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_EG_FILTER_SPLIT_HORIZON_OUT_LIF_ORIENTATION_SET_SPLIT_HORIZON_OUT_LIF_ORIENTATION_SET_ORIENTATION_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_orientation = param_val->numeric_equivalent;
  } 


  /* Call function */
  ret = soc_ppd_eg_filter_split_horizon_out_lif_orientation_set(
          unit,
          prm_out_lif_ndx,
          prm_orientation
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_eg_filter_split_horizon_out_lif_orientation_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_filter_split_horizon_out_lif_orientation_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: split_horizon_out_lif_orientation_get (section eg_filter)
 */
int 
  ui_ppd_api_eg_filter_split_horizon_out_lif_orientation_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32
    prm_out_lif_ndx;
  SOC_SAND_PP_HUB_SPOKE_ORIENTATION   
    prm_orientation;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_filter"); 
  soc_sand_proc_name = "soc_ppd_eg_filter_split_horizon_out_lif_orientation_get"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_SPLIT_HORIZON_OUT_LIF_ORIENTATION_GET_SPLIT_HORIZON_OUT_LIF_ORIENTATION_GET_OUT_LIF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_FILTER_SPLIT_HORIZON_OUT_LIF_ORIENTATION_GET_SPLIT_HORIZON_OUT_LIF_ORIENTATION_GET_OUT_LIF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_out_lif_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter out_lif_ndx after split_horizon_out_lif_orientation_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_eg_filter_split_horizon_out_lif_orientation_get(
          unit,
          prm_out_lif_ndx,
          &prm_orientation
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_eg_filter_split_horizon_out_lif_orientation_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_filter_split_horizon_out_lif_orientation_get");   
    goto exit; 
  } 

  cli_out("orientation: %s\n\r",soc_sand_SAND_PP_HUB_SPOKE_ORIENTATION_to_string(prm_orientation));

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
#endif
#ifdef UI_EG_FILTER/* { eg_filter*/
/******************************************************************** 
 *  Section handler: eg_filter
 */ 
int 
  ui_ppd_api_eg_filter( 
    CURRENT_LINE *current_line 
  ) 
{   
  int
    ret = FALSE;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_filter"); 
 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_PORT_INFO_SET_PORT_INFO_SET_ID,1)) 
  { 
    ret = ui_ppd_api_eg_filter_port_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_PORT_INFO_GET_PORT_INFO_GET_ID,1)) 
  { 
    ret = ui_ppd_api_eg_filter_port_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_VSI_PORT_MEMBERSHIP_SET_VSI_PORT_MEMBERSHIP_SET_ID,1)) 
  { 
    ret = ui_ppd_api_eg_filter_vsi_port_membership_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_VSI_PORT_MEMBERSHIP_GET_VSI_PORT_MEMBERSHIP_GET_ID,1)) 
  { 
    ret = ui_ppd_api_eg_filter_vsi_port_membership_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_CVID_PORT_MEMBERSHIP_SET_CVID_PORT_MEMBERSHIP_SET_ID,1)) 
  { 
    ret = ui_ppd_api_eg_filter_cvid_port_membership_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_CVID_PORT_MEMBERSHIP_GET_CVID_PORT_MEMBERSHIP_GET_ID,1)) 
  { 
    ret = ui_ppd_api_eg_filter_cvid_port_membership_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_PORT_ACCEPTABLE_FRAMES_SET_PORT_ACCEPTABLE_FRAMES_SET_ID,1)) 
  { 
    ret = ui_ppd_api_eg_filter_port_acceptable_frames_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_PORT_ACCEPTABLE_FRAMES_GET_PORT_ACCEPTABLE_FRAMES_GET_ID,1)) 
  { 
    ret = ui_ppd_api_eg_filter_port_acceptable_frames_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_PEP_ACCEPTABLE_FRAMES_SET_PEP_ACCEPTABLE_FRAMES_SET_ID,1)) 
  { 
    ret = ui_ppd_api_eg_filter_pep_acceptable_frames_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_PEP_ACCEPTABLE_FRAMES_GET_PEP_ACCEPTABLE_FRAMES_GET_ID,1)) 
  { 
    ret = ui_ppd_api_eg_filter_pep_acceptable_frames_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_PVLAN_PORT_TYPE_SET_PVLAN_PORT_TYPE_SET_ID,1)) 
  { 
    ret = ui_ppd_api_eg_filter_pvlan_port_type_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_PVLAN_PORT_TYPE_GET_PVLAN_PORT_TYPE_GET_ID,1)) 
  { 
    ret = ui_ppd_api_eg_filter_pvlan_port_type_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_SET_SPLIT_HORIZON_OUT_AC_ORIENTATION_SET_ID,1)) 
  { 
    ret = ui_ppd_api_eg_filter_split_horizon_out_ac_orientation_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_GET_SPLIT_HORIZON_OUT_AC_ORIENTATION_GET_ID,1)) 
  { 
    ret = ui_ppd_api_eg_filter_split_horizon_out_ac_orientation_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_SPLIT_HORIZON_OUT_LIF_ORIENTATION_GET_SPLIT_HORIZON_OUT_LIF_ORIENTATION_GET_ID,1)) 
  { 
    ret = ui_ppd_api_eg_filter_split_horizon_out_lif_orientation_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_FILTER_SPLIT_HORIZON_OUT_LIF_ORIENTATION_SET_SPLIT_HORIZON_OUT_LIF_ORIENTATION_SET_ID,1)) 
  { 
    ret = ui_ppd_api_eg_filter_split_horizon_out_lif_orientation_set(current_line); 
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting function name after eg_filter***", TRUE); 
  } 
  
  ui_ret = ret;
    
  goto exit; 
exit:        
  return ui_ret; 
}                
          
#endif /* eg_filter */ 


#endif /* LINK_PPD_LIBRARIES */ 

