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
 
  
#include <soc/dpp/PPD/ppd_api_lag.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_lag.h>

#if LINK_PPD_LIBRARIES

#ifdef UI_LAG
/******************************************************************** 
 *  Function handler: set (section lag)
 */
int 
  ui_ppd_api_lag_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_lag_ndx,
    indx;
  SOC_PPC_LAG_INFO   
    prm_lag_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lag"); 
  soc_sand_proc_name = "soc_ppd_lag_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_LAG_INFO_clear(&prm_lag_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LAG_SET_SET_LAG_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LAG_SET_SET_LAG_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_lag_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter lag_ndx after set***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LAG_SET_SET_LAG_INFO_LB_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LAG_SET_SET_LAG_INFO_LB_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lag_info.lb_type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LAG_SET_SET_LAG_INFO_MEMBERS_INDEX_ID,1))
  {
    prm_lag_info.nof_entries = 0;
    for (indx = 0; indx < 80; ++prm_lag_info.nof_entries, ++indx, ui_ret = 0)
    {
      UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(SOC_PARAM_PPD_LAG_SET_SET_LAG_INFO_MEMBERS_INDEX_ID, indx + 1);
      if (ui_ret)
      {
        ui_ret = 0;
        break;
      }
      prm_lag_info.members[prm_lag_info.nof_entries].sys_port = (uint32)param_val->value.ulong_value;
    }
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LAG_SET_SET_LAG_INFO_MEMBERS_MEMBER_ID_ID,1))
  {
    prm_lag_info.nof_entries = 0;
    for (indx = 0; indx < 80; ++prm_lag_info.nof_entries, ++indx, ui_ret = 0)
    {
      UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(SOC_PARAM_PPD_LAG_SET_SET_LAG_INFO_MEMBERS_MEMBER_ID_ID, indx + 1);
      if (ui_ret)
      {
        ui_ret = 0;
        break;
      }
      prm_lag_info.members[prm_lag_info.nof_entries].member_id = (uint32)param_val->value.ulong_value;
    }
  }


  /* Call function */
  ret = soc_ppd_lag_set(
          unit,
          prm_lag_ndx,
          &prm_lag_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_lag_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_lag_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: get (section lag)
 */
int 
  ui_ppd_api_lag_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_lag_ndx;
  SOC_PPC_LAG_INFO   
    prm_lag_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lag"); 
  soc_sand_proc_name = "soc_ppd_lag_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_LAG_INFO_clear(&prm_lag_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LAG_GET_GET_LAG_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LAG_GET_GET_LAG_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_lag_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter lag_ndx after get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_lag_get(
          unit,
          prm_lag_ndx,
          &prm_lag_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_lag_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_lag_get");   
    goto exit; 
  } 

  SOC_PPC_LAG_INFO_print(&prm_lag_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: member_add (section lag)
 */
int 
  ui_ppd_api_lag_member_add(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_lag_ndx;
  SOC_PPC_LAG_MEMBER   
    prm_member;
  SOC_SAND_SUCCESS_FAILURE   
    prm_success;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lag"); 
  soc_sand_proc_name = "soc_ppd_lag_member_add"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_LAG_MEMBER_clear(&prm_member);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LAG_MEMBER_ADD_MEMBER_ADD_LAG_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LAG_MEMBER_ADD_MEMBER_ADD_LAG_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_lag_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter lag_ndx after member_add***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LAG_MEMBER_ADD_MEMBER_ADD_MEMBER_MEMBER_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LAG_MEMBER_ADD_MEMBER_ADD_MEMBER_MEMBER_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_member.member_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LAG_MEMBER_ADD_MEMBER_ADD_MEMBER_SYS_PORT_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LAG_MEMBER_ADD_MEMBER_ADD_MEMBER_SYS_PORT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_member.sys_port = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_lag_member_add(
          unit,
          prm_lag_ndx,
          &prm_member,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_lag_member_add - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_lag_member_add");   
    goto exit; 
  } 

  cli_out("success: %s\n\r",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: member_remove (section lag)
 */
int 
  ui_ppd_api_lag_member_remove(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_lag_ndx;
  SOC_PPC_LAG_MEMBER   
    prm_member;   
   
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lag"); 
  soc_sand_proc_name = "soc_ppd_lag_member_remove"; 
 
  SOC_PPC_LAG_MEMBER_clear(&prm_member);
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LAG_MEMBER_REMOVE_MEMBER_REMOVE_LAG_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LAG_MEMBER_REMOVE_MEMBER_REMOVE_LAG_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_lag_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter lag_ndx after member_remove***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LAG_MEMBER_REMOVE_MEMBER_REMOVE_SYS_PORT_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LAG_MEMBER_REMOVE_MEMBER_REMOVE_SYS_PORT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_member.sys_port = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_lag_member_remove(
          unit,
          prm_lag_ndx,
          &prm_member 
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_lag_member_remove - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_lag_member_remove");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: hashing_global_info_set (section lag)
 */
int 
  ui_ppd_api_lag_hashing_global_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_LAG_HASH_GLOBAL_INFO   
    prm_glbl_hash_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lag"); 
  soc_sand_proc_name = "soc_ppd_lag_hashing_global_info_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_LAG_HASH_GLOBAL_INFO_clear(&prm_glbl_hash_info);
 
  /* Get parameters */ 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_lag_hashing_global_info_get(
          unit,
          &prm_glbl_hash_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_lag_hashing_global_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_lag_hashing_global_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LAG_HASHING_GLOBAL_INFO_SET_HASHING_GLOBAL_INFO_SET_GLBL_HASH_INFO_KEY_SHIFT_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LAG_HASHING_GLOBAL_INFO_SET_HASHING_GLOBAL_INFO_SET_GLBL_HASH_INFO_KEY_SHIFT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_glbl_hash_info.key_shift = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LAG_HASHING_GLOBAL_INFO_SET_HASHING_GLOBAL_INFO_SET_GLBL_HASH_INFO_HASH_FUNC_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LAG_HASHING_GLOBAL_INFO_SET_HASHING_GLOBAL_INFO_SET_GLBL_HASH_INFO_HASH_FUNC_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_glbl_hash_info.hash_func_id = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LAG_HASHING_GLOBAL_INFO_SET_HASHING_GLOBAL_INFO_SET_GLBL_HASH_INFO_SEED_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LAG_HASHING_GLOBAL_INFO_SET_HASHING_GLOBAL_INFO_SET_GLBL_HASH_INFO_SEED_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_glbl_hash_info.seed = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LAG_HASHING_GLOBAL_INFO_SET_HASHING_GLOBAL_INFO_SET_GLBL_HASH_INFO_USE_PORT_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LAG_HASHING_GLOBAL_INFO_SET_HASHING_GLOBAL_INFO_SET_GLBL_HASH_INFO_USE_PORT_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_glbl_hash_info.use_port_id = (uint8)param_val->value.ulong_value;
  } 

  /* Call function */
  ret = soc_ppd_lag_hashing_global_info_set(
          unit,
          &prm_glbl_hash_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_lag_hashing_global_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_lag_hashing_global_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: hashing_global_info_get (section lag)
 */
int 
  ui_ppd_api_lag_hashing_global_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_LAG_HASH_GLOBAL_INFO   
    prm_glbl_hash_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lag"); 
  soc_sand_proc_name = "soc_ppd_lag_hashing_global_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_LAG_HASH_GLOBAL_INFO_clear(&prm_glbl_hash_info);
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_ppd_lag_hashing_global_info_get(
          unit,
          &prm_glbl_hash_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_lag_hashing_global_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_lag_hashing_global_info_get");   
    goto exit; 
  } 

  SOC_PPC_LAG_HASH_GLOBAL_INFO_print(&prm_glbl_hash_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: hashing_port_info_set (section lag)
 */
int 
  ui_ppd_api_lag_hashing_port_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_PORT   
    prm_port_ndx;
  SOC_PPC_LAG_HASH_PORT_INFO   
    prm_lag_hash_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lag"); 
  soc_sand_proc_name = "soc_ppd_lag_hashing_port_info_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_LAG_HASH_PORT_INFO_clear(&prm_lag_hash_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LAG_HASHING_PORT_INFO_SET_HASHING_PORT_INFO_SET_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LAG_HASHING_PORT_INFO_SET_HASHING_PORT_INFO_SET_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter port_ndx after hashing_port_info_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_lag_hashing_port_info_get(
          unit,
          SOC_CORE_DEFAULT,
          prm_port_ndx,
          &prm_lag_hash_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_lag_hashing_port_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_lag_hashing_port_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LAG_HASHING_PORT_INFO_SET_HASHING_PORT_INFO_SET_LAG_HASH_INFO_FIRST_HEADER_TO_PARSE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LAG_HASHING_PORT_INFO_SET_HASHING_PORT_INFO_SET_LAG_HASH_INFO_FIRST_HEADER_TO_PARSE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lag_hash_info.first_header_to_parse = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LAG_HASHING_PORT_INFO_SET_HASHING_PORT_INFO_SET_LAG_HASH_INFO_NOF_HEADERS_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LAG_HASHING_PORT_INFO_SET_HASHING_PORT_INFO_SET_LAG_HASH_INFO_NOF_HEADERS_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lag_hash_info.nof_headers = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_lag_hashing_port_info_set(
          unit,
          SOC_CORE_DEFAULT,
          prm_port_ndx,
          &prm_lag_hash_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_lag_hashing_port_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_lag_hashing_port_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: hashing_port_info_get (section lag)
 */
int 
  ui_ppd_api_lag_hashing_port_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_PORT   
    prm_port_ndx;
  SOC_PPC_LAG_HASH_PORT_INFO   
    prm_lag_hash_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lag"); 
  soc_sand_proc_name = "soc_ppd_lag_hashing_port_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_LAG_HASH_PORT_INFO_clear(&prm_lag_hash_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LAG_HASHING_PORT_INFO_GET_HASHING_PORT_INFO_GET_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LAG_HASHING_PORT_INFO_GET_HASHING_PORT_INFO_GET_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter port_ndx after hashing_port_info_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_lag_hashing_port_info_get(
          unit,
          SOC_CORE_DEFAULT,
          prm_port_ndx,
          &prm_lag_hash_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_lag_hashing_port_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_lag_hashing_port_info_get");   
    goto exit; 
  } 

  SOC_PPC_LAG_HASH_PORT_INFO_print(&prm_lag_hash_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: hashing_mask_set (section lag)
 */
int 
  ui_ppd_api_lag_hashing_mask_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_HASH_MASK_INFO   
    prm_mask_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lag"); 
  soc_sand_proc_name = "soc_ppd_lag_hashing_mask_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_HASH_MASK_INFO_clear(&prm_mask_info);
 
  /* Get parameters */ 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_lag_hashing_mask_get(
          unit,
          &prm_mask_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_lag_hashing_mask_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_lag_hashing_mask_get");   
    goto exit; 
  } 

if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LAG_HASHING_MASK_SET_HASHING_MASK_SET_MASK_INFO_EXPECT_CW_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LAG_HASHING_MASK_SET_HASHING_MASK_SET_MASK_INFO_EXPECT_CW_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_mask_info.expect_cw = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LAG_HASHING_MASK_SET_HASHING_MASK_SET_MASK_INFO_IS_SYMMETRIC_KEY_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LAG_HASHING_MASK_SET_HASHING_MASK_SET_MASK_INFO_IS_SYMMETRIC_KEY_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_mask_info.is_symmetric_key = (uint8)param_val->value.ulong_value;
  }
   
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LAG_HASHING_MASK_SET_HASHING_MASK_SET_MASKS_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LAG_HASHING_MASK_SET_HASHING_MASK_SET_MASKS_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_mask_info.mask = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LAG_HASHING_MASK_SET_HASHING_MASK_SET_MASKS_REMOVE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LAG_HASHING_MASK_SET_HASHING_MASK_SET_MASKS_REMOVE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_mask_info.mask |= param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LAG_HASHING_MASK_SET_HASHING_MASK_SET_MASKS_ADD_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LAG_HASHING_MASK_SET_HASHING_MASK_SET_MASKS_ADD_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_mask_info.mask &= ~param_val->numeric_equivalent;
  } 


  /* Call function */
  ret = soc_ppd_lag_hashing_mask_set(
          unit,
          &prm_mask_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_lag_hashing_mask_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_lag_hashing_mask_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: hashing_mask_get (section lag)
 */
int 
  ui_ppd_api_lag_hashing_mask_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32
    ret;   
  SOC_PPC_HASH_MASK_INFO   
    prm_mask_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lag"); 
  soc_sand_proc_name = "soc_ppd_lag_hashing_mask_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_HASH_MASK_INFO_clear(&prm_mask_info);
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_ppd_lag_hashing_mask_get(
          unit,
          &prm_mask_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_lag_hashing_mask_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_lag_hashing_mask_get");   
    goto exit; 
  } 

  SOC_PPC_HASH_MASK_INFO_print(&prm_mask_info);


  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
#endif
#ifdef UI_LAG/* { lag*/
/******************************************************************** 
 *  Section handler: lag
 */ 
int 
  ui_ppd_api_lag( 
    CURRENT_LINE *current_line 
  ) 
{   
  int
    ret = FALSE;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lag"); 
 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LAG_SET_SET_ID,1)) 
  { 
    ret = ui_ppd_api_lag_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LAG_GET_GET_ID,1)) 
  { 
    ret = ui_ppd_api_lag_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LAG_MEMBER_ADD_MEMBER_ADD_ID,1)) 
  { 
    ret = ui_ppd_api_lag_member_add(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LAG_MEMBER_REMOVE_MEMBER_REMOVE_ID,1)) 
  { 
    ret = ui_ppd_api_lag_member_remove(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LAG_HASHING_GLOBAL_INFO_SET_HASHING_GLOBAL_INFO_SET_ID,1)) 
  { 
    ret = ui_ppd_api_lag_hashing_global_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LAG_HASHING_GLOBAL_INFO_GET_HASHING_GLOBAL_INFO_GET_ID,1)) 
  { 
    ret = ui_ppd_api_lag_hashing_global_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LAG_HASHING_PORT_INFO_SET_HASHING_PORT_INFO_SET_ID,1)) 
  { 
    ret = ui_ppd_api_lag_hashing_port_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LAG_HASHING_PORT_INFO_GET_HASHING_PORT_INFO_GET_ID,1)) 
  { 
    ret = ui_ppd_api_lag_hashing_port_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LAG_HASHING_MASK_SET_HASHING_MASK_SET_ID,1)) 
  { 
    ret = ui_ppd_api_lag_hashing_mask_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LAG_HASHING_MASK_GET_HASHING_MASK_GET_ID,1)) 
  { 
    ret = ui_ppd_api_lag_hashing_mask_get(current_line); 
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting function name after lag***", TRUE); 
  }
  
  ui_ret = ret; 
    
  goto exit; 
exit:        
  return ui_ret; 
}                
          
#endif /* lag */ 


#endif /* LINK_PPD_LIBRARIES */ 

