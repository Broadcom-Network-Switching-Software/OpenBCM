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
 

  
#include <soc/dpp/PPD/ppd_api_llp_mirror.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_llp_mirror.h>

#if LINK_PPD_LIBRARIES

#ifdef UI_LLP_MIRROR
/******************************************************************** 
 *  Function handler: port_vlan_add (section llp_mirror)
 */
int 
  ui_ppd_api_llp_mirror_port_vlan_add(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_PORT   
    prm_local_port_ndx;
  SOC_SAND_PP_VLAN_ID   
    prm_vid_ndx;
  uint32   
    prm_mirror_profile;
  SOC_SAND_SUCCESS_FAILURE   
    prm_success;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_mirror"); 
  soc_sand_proc_name = "soc_ppd_llp_mirror_port_vlan_add"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_MIRROR_PORT_VLAN_ADD_PORT_VLAN_ADD_LOCAL_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_MIRROR_PORT_VLAN_ADD_PORT_VLAN_ADD_LOCAL_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_local_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter local_port_ndx after port_vlan_add***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_MIRROR_PORT_VLAN_ADD_PORT_VLAN_ADD_VID_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_MIRROR_PORT_VLAN_ADD_PORT_VLAN_ADD_VID_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_vid_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter vid_ndx after port_vlan_add***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_MIRROR_PORT_VLAN_ADD_PORT_VLAN_ADD_MIRROR_PROFILE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_MIRROR_PORT_VLAN_ADD_PORT_VLAN_ADD_MIRROR_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_mirror_profile = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_llp_mirror_port_vlan_add(
          unit,
		  SOC_CORE_DEFAULT,
          prm_local_port_ndx,
          prm_vid_ndx,
          prm_mirror_profile,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_llp_mirror_port_vlan_add - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_mirror_port_vlan_add");   
    goto exit; 
  } 

  cli_out("success: %s\n\r",SOC_SAND_SUCCESS_FAILURE_to_string(prm_success));

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: port_vlan_remove (section llp_mirror)
 */
int 
  ui_ppd_api_llp_mirror_port_vlan_remove(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_PORT   
    prm_local_port_ndx;
  SOC_SAND_PP_VLAN_ID   
    prm_vid_ndx;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_mirror"); 
  soc_sand_proc_name = "soc_ppd_llp_mirror_port_vlan_remove"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_MIRROR_PORT_VLAN_REMOVE_PORT_VLAN_REMOVE_LOCAL_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_MIRROR_PORT_VLAN_REMOVE_PORT_VLAN_REMOVE_LOCAL_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_local_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter local_port_ndx after port_vlan_remove***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_MIRROR_PORT_VLAN_REMOVE_PORT_VLAN_REMOVE_VID_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_MIRROR_PORT_VLAN_REMOVE_PORT_VLAN_REMOVE_VID_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_vid_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter vid_ndx after port_vlan_remove***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_llp_mirror_port_vlan_remove(
          unit,
          prm_local_port_ndx,
          prm_vid_ndx
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_llp_mirror_port_vlan_remove - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_mirror_port_vlan_remove");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: port_vlan_get (section llp_mirror)
 */
int 
  ui_ppd_api_llp_mirror_port_vlan_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_PORT   
    prm_local_port_ndx;
  SOC_SAND_PP_VLAN_ID   
    prm_vid_ndx;
  uint32   
    prm_mirror_profile;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_mirror"); 
  soc_sand_proc_name = "soc_ppd_llp_mirror_port_vlan_get"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_MIRROR_PORT_VLAN_GET_PORT_VLAN_GET_LOCAL_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_MIRROR_PORT_VLAN_GET_PORT_VLAN_GET_LOCAL_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_local_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter local_port_ndx after port_vlan_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_MIRROR_PORT_VLAN_GET_PORT_VLAN_GET_VID_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_MIRROR_PORT_VLAN_GET_PORT_VLAN_GET_VID_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_vid_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter vid_ndx after port_vlan_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_llp_mirror_port_vlan_get(
          unit,
		  SOC_CORE_DEFAULT,
          prm_local_port_ndx,
          prm_vid_ndx,
          &prm_mirror_profile
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_llp_mirror_port_vlan_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_mirror_port_vlan_get");   
    goto exit; 
  } 

  cli_out("mirror_profile: %u\n\r",prm_mirror_profile);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: port_dflt_set (section llp_mirror)
 */
int 
  ui_ppd_api_llp_mirror_port_dflt_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_PORT   
    prm_local_port_ndx;
  SOC_PPC_LLP_MIRROR_PORT_DFLT_INFO   
    prm_dflt_mirroring_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_mirror"); 
  soc_sand_proc_name = "soc_ppd_llp_mirror_port_dflt_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_LLP_MIRROR_PORT_DFLT_INFO_clear(&prm_dflt_mirroring_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_MIRROR_PORT_DFLT_SET_PORT_DFLT_SET_LOCAL_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_MIRROR_PORT_DFLT_SET_PORT_DFLT_SET_LOCAL_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_local_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter local_port_ndx after port_dflt_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_llp_mirror_port_dflt_get(
          unit,
		  SOC_CORE_DEFAULT,
          prm_local_port_ndx,
          &prm_dflt_mirroring_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_llp_mirror_port_dflt_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_mirror_port_dflt_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_MIRROR_PORT_DFLT_SET_PORT_DFLT_SET_DFLT_MIRRORING_INFO_UNTAGGED_DFLT_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_MIRROR_PORT_DFLT_SET_PORT_DFLT_SET_DFLT_MIRRORING_INFO_UNTAGGED_DFLT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_dflt_mirroring_info.untagged_dflt = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_MIRROR_PORT_DFLT_SET_PORT_DFLT_SET_DFLT_MIRRORING_INFO_TAGGED_DFLT_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_MIRROR_PORT_DFLT_SET_PORT_DFLT_SET_DFLT_MIRRORING_INFO_TAGGED_DFLT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_dflt_mirroring_info.tagged_dflt = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_llp_mirror_port_dflt_set(
          unit,
          prm_local_port_ndx,
          &prm_dflt_mirroring_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_llp_mirror_port_dflt_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_mirror_port_dflt_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: port_dflt_get (section llp_mirror)
 */
int 
  ui_ppd_api_llp_mirror_port_dflt_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_PORT   
    prm_local_port_ndx;
  SOC_PPC_LLP_MIRROR_PORT_DFLT_INFO   
    prm_dflt_mirroring_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_mirror"); 
  soc_sand_proc_name = "soc_ppd_llp_mirror_port_dflt_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_LLP_MIRROR_PORT_DFLT_INFO_clear(&prm_dflt_mirroring_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_MIRROR_PORT_DFLT_GET_PORT_DFLT_GET_LOCAL_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_MIRROR_PORT_DFLT_GET_PORT_DFLT_GET_LOCAL_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_local_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter local_port_ndx after port_dflt_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_llp_mirror_port_dflt_get(
          unit,
		  SOC_CORE_DEFAULT,
          prm_local_port_ndx,
          &prm_dflt_mirroring_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_llp_mirror_port_dflt_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_mirror_port_dflt_get");   
    goto exit; 
  } 

  SOC_PPC_LLP_MIRROR_PORT_DFLT_INFO_print(&prm_dflt_mirroring_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
#endif
#ifdef UI_LLP_MIRROR/* { llp_mirror*/
/******************************************************************** 
 *  Section handler: llp_mirror
 */ 
int 
  ui_ppd_api_llp_mirror( 
    CURRENT_LINE *current_line 
  ) 
{   
  int
    ret;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_mirror"); 
 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_MIRROR_PORT_VLAN_ADD_PORT_VLAN_ADD_ID,1)) 
  { 
    ret = ui_ppd_api_llp_mirror_port_vlan_add(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_MIRROR_PORT_VLAN_REMOVE_PORT_VLAN_REMOVE_ID,1)) 
  { 
    ret = ui_ppd_api_llp_mirror_port_vlan_remove(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_MIRROR_PORT_VLAN_GET_PORT_VLAN_GET_ID,1)) 
  { 
    ret = ui_ppd_api_llp_mirror_port_vlan_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_MIRROR_PORT_DFLT_SET_PORT_DFLT_SET_ID,1)) 
  { 
    ret = ui_ppd_api_llp_mirror_port_dflt_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_MIRROR_PORT_DFLT_GET_PORT_DFLT_GET_ID,1)) 
  { 
    ret = ui_ppd_api_llp_mirror_port_dflt_get(current_line); 
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting function name after llp_mirror***", TRUE); 
  } 
    
  goto exit; 
exit:        
  return ui_ret; 
}                
          
#endif /* llp_mirror */ 


#endif /* LINK_PPD_LIBRARIES */ 

