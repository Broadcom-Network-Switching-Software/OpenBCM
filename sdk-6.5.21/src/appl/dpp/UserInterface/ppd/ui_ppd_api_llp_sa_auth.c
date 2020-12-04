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
 
  
#include <soc/dpp/PPD/ppd_api_llp_sa_auth.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_llp_sa_auth.h>

#if LINK_PPD_LIBRARIES


#ifdef UI_LLP_SA_AUTH
/******************************************************************** 
 *  Function handler: port_info_set (section llp_sa_auth)
 */
int 
  ui_ppd_api_llp_sa_auth_port_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_PORT   
    prm_local_port_ndx;
  SOC_PPC_LLP_SA_AUTH_PORT_INFO   
    prm_port_auth_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_sa_auth"); 
  soc_sand_proc_name = "soc_ppd_llp_sa_auth_port_info_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_LLP_SA_AUTH_PORT_INFO_clear(&prm_port_auth_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_SA_AUTH_PORT_INFO_SET_PORT_INFO_SET_LOCAL_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_SA_AUTH_PORT_INFO_SET_PORT_INFO_SET_LOCAL_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_local_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter local_port_ndx after port_info_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_llp_sa_auth_port_info_get(
          unit,
          SOC_CORE_DEFAULT,
          prm_local_port_ndx,
          &prm_port_auth_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_llp_sa_auth_port_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_sa_auth_port_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_SA_AUTH_PORT_INFO_SET_PORT_INFO_SET_PORT_AUTH_INFO_SA_AUTH_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_SA_AUTH_PORT_INFO_SET_PORT_INFO_SET_PORT_AUTH_INFO_SA_AUTH_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_auth_info.sa_auth_enable = (uint8)param_val->value.ulong_value;
  } 

  /* Call function */
  ret = soc_ppd_llp_sa_auth_port_info_set(
          unit,
          SOC_CORE_DEFAULT,
          prm_local_port_ndx,
          &prm_port_auth_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_llp_sa_auth_port_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_sa_auth_port_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: port_info_get (section llp_sa_auth)
 */
int 
  ui_ppd_api_llp_sa_auth_port_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_PORT   
    prm_local_port_ndx;
  SOC_PPC_LLP_SA_AUTH_PORT_INFO   
    prm_port_auth_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_sa_auth"); 
  soc_sand_proc_name = "soc_ppd_llp_sa_auth_port_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_LLP_SA_AUTH_PORT_INFO_clear(&prm_port_auth_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_SA_AUTH_PORT_INFO_GET_PORT_INFO_GET_LOCAL_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_SA_AUTH_PORT_INFO_GET_PORT_INFO_GET_LOCAL_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_local_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter local_port_ndx after port_info_get***", TRUE); 
    goto exit; 
  } 

  /* Call function */
  ret = soc_ppd_llp_sa_auth_port_info_get(
          unit,
          SOC_CORE_DEFAULT,
          prm_local_port_ndx,
          &prm_port_auth_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_llp_sa_auth_port_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_sa_auth_port_info_get");   
    goto exit; 
  } 

  SOC_PPC_LLP_SA_AUTH_PORT_INFO_print(&prm_port_auth_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: mac_info_set (section llp_sa_auth)
 */
int 
  ui_ppd_api_llp_sa_auth_mac_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_SAND_PP_MAC_ADDRESS   
    prm_mac_address_key;
  SOC_PPC_LLP_SA_AUTH_MAC_INFO   
    prm_mac_auth_info;
  uint8   
    prm_enable;
  SOC_SAND_SUCCESS_FAILURE   
    prm_success;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_sa_auth"); 
  soc_sand_proc_name = "soc_ppd_llp_sa_auth_mac_info_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  soc_sand_SAND_PP_MAC_ADDRESS_clear(&prm_mac_address_key);
  SOC_PPC_LLP_SA_AUTH_MAC_INFO_clear(&prm_mac_auth_info);
 
  /* Get parameters */ 
  if (!get_val_of(
         current_line,(int *)&match_index,SOC_PARAM_PPD_LLP_SA_AUTH_MAC_INFO_SET_MAC_INFO_SET_MAC_ADDRESS_KEY_ID,1,  
         &param_val,VAL_TEXT,err_msg)) 
  { 
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_mac_address_key));
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter mac_address_key after mac_info_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_llp_sa_auth_mac_info_get(
          unit,
          &prm_mac_address_key,
          &prm_mac_auth_info,
          &prm_enable
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_llp_sa_auth_mac_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_sa_auth_mac_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_SA_AUTH_MAC_INFO_SET_MAC_INFO_SET_MAC_AUTH_INFO_EXPECT_SYSTEM_PORT_SYS_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_SA_AUTH_MAC_INFO_SET_MAC_INFO_SET_MAC_AUTH_INFO_EXPECT_SYSTEM_PORT_SYS_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_mac_auth_info.expect_system_port.sys_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_SA_AUTH_MAC_INFO_SET_MAC_INFO_SET_MAC_AUTH_INFO_EXPECT_SYSTEM_PORT_SYS_PORT_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LLP_SA_AUTH_MAC_INFO_SET_MAC_INFO_SET_MAC_AUTH_INFO_EXPECT_SYSTEM_PORT_SYS_PORT_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_mac_auth_info.expect_system_port.sys_port_type = param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_SA_AUTH_MAC_INFO_SET_MAC_INFO_SET_MAC_AUTH_INFO_EXPECT_TAG_VID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_SA_AUTH_MAC_INFO_SET_MAC_INFO_SET_MAC_AUTH_INFO_EXPECT_TAG_VID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_mac_auth_info.expect_tag_vid = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_SA_AUTH_MAC_INFO_SET_MAC_INFO_SET_MAC_AUTH_INFO_TAGGED_ONLY_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_SA_AUTH_MAC_INFO_SET_MAC_INFO_SET_MAC_AUTH_INFO_TAGGED_ONLY_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_mac_auth_info.tagged_only = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_SA_AUTH_MAC_INFO_SET_MAC_INFO_SET_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_SA_AUTH_MAC_INFO_SET_MAC_INFO_SET_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_enable = (uint8)param_val->value.ulong_value;
  } 

  /* Call function */
  ret = soc_ppd_llp_sa_auth_mac_info_set(
          unit,
          &prm_mac_address_key,
          &prm_mac_auth_info,
          prm_enable,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_llp_sa_auth_mac_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_sa_auth_mac_info_set");   
    goto exit; 
  } 

  cli_out("success: %s\n\r",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: mac_info_get (section llp_sa_auth)
 */
int 
  ui_ppd_api_llp_sa_auth_mac_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_SAND_PP_MAC_ADDRESS   
    prm_mac_address_key;
  SOC_PPC_LLP_SA_AUTH_MAC_INFO   
    prm_mac_auth_info;
  uint8   
    prm_enable;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_sa_auth"); 
  soc_sand_proc_name = "soc_ppd_llp_sa_auth_mac_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  soc_sand_SAND_PP_MAC_ADDRESS_clear(&prm_mac_address_key);
  SOC_PPC_LLP_SA_AUTH_MAC_INFO_clear(&prm_mac_auth_info);
 
  /* Get parameters */ 
  if (!get_val_of(
         current_line,(int *)&match_index,SOC_PARAM_PPD_LLP_SA_AUTH_MAC_INFO_GET_MAC_INFO_GET_MAC_ADDRESS_KEY_ID,1,  
         &param_val,VAL_TEXT,err_msg)) 
  { 
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_mac_address_key));
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter mac_address_key after mac_info_get***", TRUE); 
    goto exit; 
  } 

  /* Call function */
  ret = soc_ppd_llp_sa_auth_mac_info_get(
          unit,
          &prm_mac_address_key,
          &prm_mac_auth_info,
          &prm_enable
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_llp_sa_auth_mac_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_sa_auth_mac_info_get");   
    goto exit; 
  } 

  SOC_PPC_LLP_SA_AUTH_MAC_INFO_print(&prm_mac_auth_info);

  cli_out("enable: %u\n\r",prm_enable);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: get_block (section llp_sa_auth)
 */
int 
  ui_ppd_api_llp_sa_auth_get_block(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_LLP_SA_AUTH_MATCH_RULE   
    prm_rule;
  SOC_SAND_TABLE_BLOCK_RANGE   
    prm_block_range;
  SOC_SAND_PP_MAC_ADDRESS   
    prm_mac_address_key_arr[10];
  SOC_PPC_LLP_SA_AUTH_MAC_INFO   
    prm_auth_info_arr[10];
  uint32   
    prm_nof_entries,
    index;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_sa_auth"); 
  soc_sand_proc_name = "soc_ppd_llp_sa_auth_get_block"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_LLP_SA_AUTH_MATCH_RULE_clear(&prm_rule);
  soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&prm_block_range);
  for (index = 0; index < 10; index++) {
      soc_sand_SAND_PP_MAC_ADDRESS_clear(&prm_mac_address_key_arr[index]);
      SOC_PPC_LLP_SA_AUTH_MAC_INFO_clear(&prm_auth_info_arr[index]);
  }

  prm_block_range.entries_to_act = 10;
  prm_block_range.entries_to_scan = SOC_SAND_TBL_ITER_SCAN_ALL;
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_SA_AUTH_GET_BLOCK_GET_BLOCK_RULE_VID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_SA_AUTH_GET_BLOCK_GET_BLOCK_RULE_VID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rule.vid = (uint32)param_val->value.ulong_value;
  } else {
    prm_rule.vid = SOC_PPD_IGNORE_VAL;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_SA_AUTH_GET_BLOCK_GET_BLOCK_RULE_PORT_SYS_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_SA_AUTH_GET_BLOCK_GET_BLOCK_RULE_PORT_SYS_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rule.port.sys_id = (uint32)param_val->value.ulong_value;
  } else {
    prm_rule.port.sys_id = SOC_PPD_IGNORE_VAL;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_SA_AUTH_GET_BLOCK_GET_BLOCK_RULE_PORT_SYS_PORT_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LLP_SA_AUTH_GET_BLOCK_GET_BLOCK_RULE_PORT_SYS_PORT_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rule.port.sys_port_type = param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_SA_AUTH_GET_BLOCK_GET_BLOCK_RULE_RULE_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LLP_SA_AUTH_GET_BLOCK_GET_BLOCK_RULE_RULE_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rule.rule_type = param_val->numeric_equivalent;
  } else {
    prm_rule.rule_type = SOC_PPC_LLP_SA_MATCH_RULE_TYPE_ALL;
  }
  /* Call function */
  ret = soc_ppd_llp_sa_auth_get_block(
          unit,
          &prm_rule,
          &prm_block_range,
          prm_mac_address_key_arr,
          prm_auth_info_arr,
          &prm_nof_entries
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_llp_sa_auth_get_block - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_sa_auth_get_block");   
    goto exit; 
  }
   
  for (index = 0; index < prm_nof_entries; index++) {
      soc_sand_SAND_PP_MAC_ADDRESS_print(&prm_mac_address_key_arr[index]);
      SOC_PPC_LLP_SA_AUTH_MAC_INFO_print(&prm_auth_info_arr[index]);
      cli_out("\n\r");
  }
  cli_out("nof_entries: %u\n\r",prm_nof_entries);
  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
#endif
#ifdef UI_LLP_SA_AUTH/* { llp_sa_auth*/
/******************************************************************** 
 *  Section handler: llp_sa_auth
 */ 
int 
  ui_ppd_api_llp_sa_auth( 
    CURRENT_LINE *current_line 
  ) 
{   
  int
    ret = FALSE;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_sa_auth"); 
 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_SA_AUTH_PORT_INFO_SET_PORT_INFO_SET_ID,1)) 
  { 
    ret = ui_ppd_api_llp_sa_auth_port_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_SA_AUTH_PORT_INFO_GET_PORT_INFO_GET_ID,1)) 
  { 
    ret = ui_ppd_api_llp_sa_auth_port_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_SA_AUTH_MAC_INFO_SET_MAC_INFO_SET_ID,1)) 
  { 
    ret = ui_ppd_api_llp_sa_auth_mac_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_SA_AUTH_MAC_INFO_GET_MAC_INFO_GET_ID,1)) 
  { 
    ret = ui_ppd_api_llp_sa_auth_mac_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_SA_AUTH_GET_BLOCK_GET_BLOCK_ID,1)) 
  { 
    ret = ui_ppd_api_llp_sa_auth_get_block(current_line); 
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting function name after llp_sa_auth***", TRUE); 
  } 
  
  ui_ret = ret;
    
  goto exit; 
exit:        
  return ui_ret; 
}                
          
#endif /* llp_sa_auth */ 


#endif /* LINK_PPD_LIBRARIES */ 

