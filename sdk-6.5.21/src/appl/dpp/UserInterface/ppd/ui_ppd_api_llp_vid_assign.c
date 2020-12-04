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
 
  
#include <soc/dpp/PPD/ppd_api_llp_vid_assign.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_llp_vid_assign.h>

#if LINK_PPD_LIBRARIES

#ifdef UI_LLP_VID_ASSIGN
/******************************************************************** 
 *  Function handler: port_info_set (section llp_vid_assign)
 */
int 
  ui_ppd_api_llp_vid_assign_port_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_PORT   
    prm_local_port_ndx;
  SOC_PPC_LLP_VID_ASSIGN_PORT_INFO   
    prm_port_vid_assign_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_vid_assign"); 
  soc_sand_proc_name = "soc_ppd_llp_vid_assign_port_info_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_LLP_VID_ASSIGN_PORT_INFO_clear(&prm_port_vid_assign_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_VID_ASSIGN_PORT_INFO_SET_PORT_INFO_SET_LOCAL_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_VID_ASSIGN_PORT_INFO_SET_PORT_INFO_SET_LOCAL_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_local_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter local_port_ndx after port_info_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_llp_vid_assign_port_info_get(
          unit,
          SOC_CORE_DEFAULT,
          prm_local_port_ndx,
          &prm_port_vid_assign_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_llp_vid_assign_port_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_vid_assign_port_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_VID_ASSIGN_PORT_INFO_SET_PORT_INFO_SET_PORT_VID_ASSIGN_INFO_ENABLE_PROTOCOL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_VID_ASSIGN_PORT_INFO_SET_PORT_INFO_SET_PORT_VID_ASSIGN_INFO_ENABLE_PROTOCOL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_vid_assign_info.enable_protocol = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_VID_ASSIGN_PORT_INFO_SET_PORT_INFO_SET_PORT_VID_ASSIGN_INFO_ENABLE_SUBNET_IP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_VID_ASSIGN_PORT_INFO_SET_PORT_INFO_SET_PORT_VID_ASSIGN_INFO_ENABLE_SUBNET_IP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_vid_assign_info.enable_subnet_ip = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_VID_ASSIGN_PORT_INFO_SET_PORT_INFO_SET_PORT_VID_ASSIGN_INFO_ENABLE_SA_BASED_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_VID_ASSIGN_PORT_INFO_SET_PORT_INFO_SET_PORT_VID_ASSIGN_INFO_ENABLE_SA_BASED_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_vid_assign_info.enable_sa_based = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_VID_ASSIGN_PORT_INFO_SET_PORT_INFO_SET_PORT_VID_ASSIGN_INFO_IGNORE_INCOMING_TAG_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_VID_ASSIGN_PORT_INFO_SET_PORT_INFO_SET_PORT_VID_ASSIGN_INFO_IGNORE_INCOMING_TAG_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_vid_assign_info.ignore_incoming_tag = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_VID_ASSIGN_PORT_INFO_SET_PORT_INFO_SET_PORT_VID_ASSIGN_INFO_PVID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_VID_ASSIGN_PORT_INFO_SET_PORT_INFO_SET_PORT_VID_ASSIGN_INFO_PVID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_vid_assign_info.pvid = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_llp_vid_assign_port_info_set(
          unit,
          SOC_CORE_DEFAULT,
          prm_local_port_ndx,
          &prm_port_vid_assign_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_llp_vid_assign_port_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_vid_assign_port_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: port_info_get (section llp_vid_assign)
 */
int 
  ui_ppd_api_llp_vid_assign_port_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_PORT   
    prm_local_port_ndx;
  SOC_PPC_LLP_VID_ASSIGN_PORT_INFO   
    prm_port_vid_assign_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_vid_assign"); 
  soc_sand_proc_name = "soc_ppd_llp_vid_assign_port_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_LLP_VID_ASSIGN_PORT_INFO_clear(&prm_port_vid_assign_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_VID_ASSIGN_PORT_INFO_GET_PORT_INFO_GET_LOCAL_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_VID_ASSIGN_PORT_INFO_GET_PORT_INFO_GET_LOCAL_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_local_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter local_port_ndx after port_info_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_llp_vid_assign_port_info_get(
          unit,
          SOC_CORE_DEFAULT,
          prm_local_port_ndx,
          &prm_port_vid_assign_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_llp_vid_assign_port_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_vid_assign_port_info_get");   
    goto exit; 
  } 

  SOC_PPC_LLP_VID_ASSIGN_PORT_INFO_print(&prm_port_vid_assign_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: mac_based_add (section llp_vid_assign)
 */
int 
  ui_ppd_api_llp_vid_assign_mac_based_add(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_SAND_PP_MAC_ADDRESS   
    prm_mac_address_key;
  SOC_PPC_LLP_VID_ASSIGN_MAC_INFO   
    prm_mac_based_info;
  SOC_SAND_SUCCESS_FAILURE   
    prm_success;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_vid_assign"); 
  soc_sand_proc_name = "soc_ppd_llp_vid_assign_mac_based_add"; 
 
  unit = soc_ppd_get_default_unit(); 
  soc_sand_SAND_PP_MAC_ADDRESS_clear(&prm_mac_address_key);
  SOC_PPC_LLP_VID_ASSIGN_MAC_INFO_clear(&prm_mac_based_info);
 
  /* Get parameters */ 
  if (!get_val_of(
         current_line,(int *)&match_index,SOC_PARAM_PPD_LLP_VID_ASSIGN_MAC_BASED_ADD_MAC_BASED_ADD_MAC_ADDRESS_KEY_ID,1,  
         &param_val,VAL_TEXT,err_msg)) 
  { 
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_mac_address_key));
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter mac_address_key after mac_based_add***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_VID_ASSIGN_MAC_BASED_ADD_MAC_BASED_ADD_MAC_BASED_INFO_OVERRIDE_TAGGED_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_VID_ASSIGN_MAC_BASED_ADD_MAC_BASED_ADD_MAC_BASED_INFO_OVERRIDE_TAGGED_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_mac_based_info.override_tagged = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_VID_ASSIGN_MAC_BASED_ADD_MAC_BASED_ADD_MAC_BASED_INFO_USE_FOR_UNTAGGED_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_VID_ASSIGN_MAC_BASED_ADD_MAC_BASED_ADD_MAC_BASED_INFO_USE_FOR_UNTAGGED_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_mac_based_info.use_for_untagged = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_VID_ASSIGN_MAC_BASED_ADD_MAC_BASED_ADD_MAC_BASED_INFO_VID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_VID_ASSIGN_MAC_BASED_ADD_MAC_BASED_ADD_MAC_BASED_INFO_VID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_mac_based_info.vid = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_llp_vid_assign_mac_based_add(
          unit,
          &prm_mac_address_key,
          &prm_mac_based_info,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_llp_vid_assign_mac_based_add - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_vid_assign_mac_based_add");   
    goto exit; 
  } 

  cli_out("success: %s\n\r",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: mac_based_remove (section llp_vid_assign)
 */
int 
  ui_ppd_api_llp_vid_assign_mac_based_remove(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_SAND_PP_MAC_ADDRESS   
    prm_mac_address_key;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_vid_assign"); 
  soc_sand_proc_name = "soc_ppd_llp_vid_assign_mac_based_remove"; 
 
  unit = soc_ppd_get_default_unit(); 
  soc_sand_SAND_PP_MAC_ADDRESS_clear(&prm_mac_address_key);
 
  /* Get parameters */ 
  if (!get_val_of(
         current_line,(int *)&match_index,SOC_PARAM_PPD_LLP_VID_ASSIGN_MAC_BASED_REMOVE_MAC_BASED_REMOVE_MAC_ADDRESS_KEY_ID,1,  
         &param_val,VAL_TEXT,err_msg)) 
  { 
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_mac_address_key));
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter mac_address_key after mac_based_remove***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_llp_vid_assign_mac_based_remove(
          unit,
          &prm_mac_address_key
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_llp_vid_assign_mac_based_remove - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_vid_assign_mac_based_remove");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: mac_based_get (section llp_vid_assign)
 */
int 
  ui_ppd_api_llp_vid_assign_mac_based_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_SAND_PP_MAC_ADDRESS   
    prm_mac_address_key;
  SOC_PPC_LLP_VID_ASSIGN_MAC_INFO   
    prm_mac_based_info;
  uint8   
    prm_found;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_vid_assign"); 
  soc_sand_proc_name = "soc_ppd_llp_vid_assign_mac_based_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  soc_sand_SAND_PP_MAC_ADDRESS_clear(&prm_mac_address_key);
  SOC_PPC_LLP_VID_ASSIGN_MAC_INFO_clear(&prm_mac_based_info);
 
  /* Get parameters */ 
  if (!get_val_of(
         current_line,(int *)&match_index,SOC_PARAM_PPD_LLP_VID_ASSIGN_MAC_BASED_GET_MAC_BASED_GET_MAC_ADDRESS_KEY_ID,1,  
         &param_val,VAL_TEXT,err_msg)) 
  { 
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_mac_address_key));
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter mac_address_key after mac_based_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_llp_vid_assign_mac_based_get(
          unit,
          &prm_mac_address_key,
          &prm_mac_based_info,
          &prm_found
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_llp_vid_assign_mac_based_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_vid_assign_mac_based_get");   
    goto exit; 
  } 

  SOC_PPC_LLP_VID_ASSIGN_MAC_INFO_print(&prm_mac_based_info);

  cli_out("found: %u\n\r",prm_found);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: mac_based_get_block (section llp_vid_assign)
 */
int 
  ui_ppd_api_llp_vid_assign_mac_based_get_block(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE   
    prm_rule;
  SOC_SAND_TABLE_BLOCK_RANGE   
    prm_block_range;
  SOC_SAND_PP_MAC_ADDRESS   
    prm_mac_address_key_arr;
  SOC_PPC_LLP_VID_ASSIGN_MAC_INFO   
    prm_vid_assign_info_arr;
  uint32   
    prm_nof_entries;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_vid_assign"); 
  soc_sand_proc_name = "soc_ppd_llp_vid_assign_mac_based_get_block"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE_clear(&prm_rule);
  soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&prm_block_range);
  soc_sand_SAND_PP_MAC_ADDRESS_clear(&prm_mac_address_key_arr);
  SOC_PPC_LLP_VID_ASSIGN_MAC_INFO_clear(&prm_vid_assign_info_arr);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_VID_ASSIGN_MAC_BASED_GET_BLOCK_MAC_BASED_GET_BLOCK_RULE_VID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_VID_ASSIGN_MAC_BASED_GET_BLOCK_MAC_BASED_GET_BLOCK_RULE_VID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rule.vid = (uint32)param_val->value.ulong_value;

  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_VID_ASSIGN_MAC_BASED_GET_BLOCK_MAC_BASED_GET_BLOCK_RULE_RULE_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LLP_VID_ASSIGN_MAC_BASED_GET_BLOCK_MAC_BASED_GET_BLOCK_RULE_RULE_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rule.rule_type = param_val->numeric_equivalent;
  } 


  /* Call function */
  ret = soc_ppd_llp_vid_assign_mac_based_get_block(
          unit,
          &prm_rule,
          &prm_block_range,
          &prm_mac_address_key_arr,
          &prm_vid_assign_info_arr,
          &prm_nof_entries
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_llp_vid_assign_mac_based_get_block - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_vid_assign_mac_based_get_block");   
    goto exit; 
  } 

  soc_sand_SAND_PP_MAC_ADDRESS_print(&prm_mac_address_key_arr);


  SOC_PPC_LLP_VID_ASSIGN_MAC_INFO_print(&prm_vid_assign_info_arr);

  cli_out("nof_entries: %u\n\r",prm_nof_entries);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: ipv4_subnet_based_set (section llp_vid_assign)
 */
int 
  ui_ppd_api_llp_vid_assign_ipv4_subnet_based_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_entry_ndx;
  SOC_SAND_PP_IPV4_SUBNET   
    prm_subnet;
  SOC_PPC_LLP_VID_ASSIGN_IPV4_SUBNET_INFO   
    prm_subnet_based_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_vid_assign"); 
  soc_sand_proc_name = "soc_ppd_llp_vid_assign_ipv4_subnet_based_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  soc_sand_SAND_PP_IPV4_SUBNET_clear(&prm_subnet);
  SOC_PPC_LLP_VID_ASSIGN_IPV4_SUBNET_INFO_clear(&prm_subnet_based_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_VID_ASSIGN_IPV4_SUBNET_BASED_SET_IPV4_SUBNET_BASED_SET_ENTRY_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_VID_ASSIGN_IPV4_SUBNET_BASED_SET_IPV4_SUBNET_BASED_SET_ENTRY_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_entry_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter entry_ndx after ipv4_subnet_based_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_llp_vid_assign_ipv4_subnet_based_get(
          unit,
          prm_entry_ndx,
          &prm_subnet,
          &prm_subnet_based_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_llp_vid_assign_ipv4_subnet_based_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_vid_assign_ipv4_subnet_based_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_VID_ASSIGN_IPV4_SUBNET_BASED_SET_IPV4_SUBNET_BASED_SET_SUBNET_PREFIX_LEN_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_VID_ASSIGN_IPV4_SUBNET_BASED_SET_IPV4_SUBNET_BASED_SET_SUBNET_PREFIX_LEN_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_subnet.prefix_len = (uint8)param_val->value.ulong_value;
  } 

  if (!get_val_of(
         current_line,(int *)&match_index,SOC_PARAM_PPD_LLP_VID_ASSIGN_IPV4_SUBNET_BASED_SET_IPV4_SUBNET_BASED_SET_SUBNET_IP_ADDRESS_ID,1,  
         &param_val,VAL_IP,err_msg)) 
  { 
    prm_subnet.ip_address = param_val->value.ip_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_VID_ASSIGN_IPV4_SUBNET_BASED_SET_IPV4_SUBNET_BASED_SET_SUBNET_BASED_INFO_VID_IS_VALID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_VID_ASSIGN_IPV4_SUBNET_BASED_SET_IPV4_SUBNET_BASED_SET_SUBNET_BASED_INFO_VID_IS_VALID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_subnet_based_info.vid_is_valid = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_VID_ASSIGN_IPV4_SUBNET_BASED_SET_IPV4_SUBNET_BASED_SET_SUBNET_BASED_INFO_VID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_VID_ASSIGN_IPV4_SUBNET_BASED_SET_IPV4_SUBNET_BASED_SET_SUBNET_BASED_INFO_VID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_subnet_based_info.vid = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_llp_vid_assign_ipv4_subnet_based_set(
          unit,
          prm_entry_ndx,
          &prm_subnet,
          &prm_subnet_based_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_llp_vid_assign_ipv4_subnet_based_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_vid_assign_ipv4_subnet_based_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: ipv4_subnet_based_get (section llp_vid_assign)
 */
int 
  ui_ppd_api_llp_vid_assign_ipv4_subnet_based_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_entry_ndx;
  SOC_SAND_PP_IPV4_SUBNET   
    prm_subnet;
  SOC_PPC_LLP_VID_ASSIGN_IPV4_SUBNET_INFO   
    prm_subnet_based_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_vid_assign"); 
  soc_sand_proc_name = "soc_ppd_llp_vid_assign_ipv4_subnet_based_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  soc_sand_SAND_PP_IPV4_SUBNET_clear(&prm_subnet);
  SOC_PPC_LLP_VID_ASSIGN_IPV4_SUBNET_INFO_clear(&prm_subnet_based_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_VID_ASSIGN_IPV4_SUBNET_BASED_GET_IPV4_SUBNET_BASED_GET_ENTRY_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_VID_ASSIGN_IPV4_SUBNET_BASED_GET_IPV4_SUBNET_BASED_GET_ENTRY_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_entry_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter entry_ndx after ipv4_subnet_based_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_llp_vid_assign_ipv4_subnet_based_get(
          unit,
          prm_entry_ndx,
          &prm_subnet,
          &prm_subnet_based_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_llp_vid_assign_ipv4_subnet_based_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_vid_assign_ipv4_subnet_based_get");   
    goto exit; 
  } 

  soc_sand_SAND_PP_IPV4_SUBNET_print(&prm_subnet);

  cli_out("\n\r");

  SOC_PPC_LLP_VID_ASSIGN_IPV4_SUBNET_INFO_print(&prm_subnet_based_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: protocol_based_set (section llp_vid_assign)
 */
int 
  ui_ppd_api_llp_vid_assign_protocol_based_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_port_profile_ndx;
  uint16   
    prm_ether_type_ndx;
  SOC_PPC_LLP_VID_ASSIGN_PRTCL_INFO   
    prm_prtcl_assign_info;
  SOC_SAND_SUCCESS_FAILURE   
    prm_success;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_vid_assign"); 
  soc_sand_proc_name = "soc_ppd_llp_vid_assign_protocol_based_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_LLP_VID_ASSIGN_PRTCL_INFO_clear(&prm_prtcl_assign_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_VID_ASSIGN_PROTOCOL_BASED_SET_PROTOCOL_BASED_SET_PORT_PROFILE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_VID_ASSIGN_PROTOCOL_BASED_SET_PROTOCOL_BASED_SET_PORT_PROFILE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_port_profile_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter port_profile_ndx after protocol_based_set***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_VID_ASSIGN_PROTOCOL_BASED_SET_PROTOCOL_BASED_SET_ETHER_TYPE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_VID_ASSIGN_PROTOCOL_BASED_SET_PROTOCOL_BASED_SET_ETHER_TYPE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_ether_type_ndx = (uint16)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter ether_type_ndx after protocol_based_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
/*
  ret = soc_ppd_llp_vid_assign_protocol_based_get(
          unit,
          prm_port_profile_ndx,
          prm_ether_type_ndx,
          &prm_prtcl_assign_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_llp_vid_assign_protocol_based_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_vid_assign_protocol_based_get");   
    goto exit; 
  } 
  */

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_VID_ASSIGN_PROTOCOL_BASED_SET_PROTOCOL_BASED_SET_PRTCL_ASSIGN_INFO_VID_IS_VALID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_VID_ASSIGN_PROTOCOL_BASED_SET_PROTOCOL_BASED_SET_PRTCL_ASSIGN_INFO_VID_IS_VALID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_prtcl_assign_info.vid_is_valid = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_VID_ASSIGN_PROTOCOL_BASED_SET_PROTOCOL_BASED_SET_PRTCL_ASSIGN_INFO_VID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_VID_ASSIGN_PROTOCOL_BASED_SET_PROTOCOL_BASED_SET_PRTCL_ASSIGN_INFO_VID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_prtcl_assign_info.vid = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_llp_vid_assign_protocol_based_set(
          unit,
          prm_port_profile_ndx,
          prm_ether_type_ndx,
          &prm_prtcl_assign_info,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_llp_vid_assign_protocol_based_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_vid_assign_protocol_based_set");   
    goto exit; 
  } 

  cli_out("success: %s\n\r",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: protocol_based_get (section llp_vid_assign)
 */
int 
  ui_ppd_api_llp_vid_assign_protocol_based_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_port_profile_ndx;
  uint16   
    prm_ether_type_ndx;
  SOC_PPC_LLP_VID_ASSIGN_PRTCL_INFO   
    prm_prtcl_assign_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_vid_assign"); 
  soc_sand_proc_name = "soc_ppd_llp_vid_assign_protocol_based_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_LLP_VID_ASSIGN_PRTCL_INFO_clear(&prm_prtcl_assign_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_VID_ASSIGN_PROTOCOL_BASED_GET_PROTOCOL_BASED_GET_PORT_PROFILE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_VID_ASSIGN_PROTOCOL_BASED_GET_PROTOCOL_BASED_GET_PORT_PROFILE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_port_profile_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter port_profile_ndx after protocol_based_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_VID_ASSIGN_PROTOCOL_BASED_GET_PROTOCOL_BASED_GET_ETHER_TYPE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_VID_ASSIGN_PROTOCOL_BASED_GET_PROTOCOL_BASED_GET_ETHER_TYPE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_ether_type_ndx = (uint16)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter ether_type_ndx after protocol_based_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_llp_vid_assign_protocol_based_get(
          unit,
          prm_port_profile_ndx,
          prm_ether_type_ndx,
          &prm_prtcl_assign_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_llp_vid_assign_protocol_based_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_vid_assign_protocol_based_get");   
    goto exit; 
  } 

  SOC_PPC_LLP_VID_ASSIGN_PRTCL_INFO_print(&prm_prtcl_assign_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
#endif
#ifdef UI_LLP_VID_ASSIGN/* { llp_vid_assign*/
/******************************************************************** 
 *  Section handler: llp_vid_assign
 */ 
int 
  ui_ppd_api_llp_vid_assign( 
    CURRENT_LINE *current_line 
  ) 
{   
  int
    ret = FALSE;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_vid_assign"); 
 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_VID_ASSIGN_PORT_INFO_SET_PORT_INFO_SET_ID,1)) 
  { 
    ret = ui_ppd_api_llp_vid_assign_port_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_VID_ASSIGN_PORT_INFO_GET_PORT_INFO_GET_ID,1)) 
  { 
    ret = ui_ppd_api_llp_vid_assign_port_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_VID_ASSIGN_MAC_BASED_ADD_MAC_BASED_ADD_ID,1)) 
  { 
    ret = ui_ppd_api_llp_vid_assign_mac_based_add(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_VID_ASSIGN_MAC_BASED_REMOVE_MAC_BASED_REMOVE_ID,1)) 
  { 
    ret = ui_ppd_api_llp_vid_assign_mac_based_remove(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_VID_ASSIGN_MAC_BASED_GET_MAC_BASED_GET_ID,1)) 
  { 
    ret = ui_ppd_api_llp_vid_assign_mac_based_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_VID_ASSIGN_MAC_BASED_GET_BLOCK_MAC_BASED_GET_BLOCK_ID,1)) 
  { 
    ret = ui_ppd_api_llp_vid_assign_mac_based_get_block(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_VID_ASSIGN_IPV4_SUBNET_BASED_SET_IPV4_SUBNET_BASED_SET_ID,1)) 
  { 
    ret = ui_ppd_api_llp_vid_assign_ipv4_subnet_based_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_VID_ASSIGN_IPV4_SUBNET_BASED_GET_IPV4_SUBNET_BASED_GET_ID,1)) 
  { 
    ret = ui_ppd_api_llp_vid_assign_ipv4_subnet_based_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_VID_ASSIGN_PROTOCOL_BASED_SET_PROTOCOL_BASED_SET_ID,1)) 
  { 
    ret = ui_ppd_api_llp_vid_assign_protocol_based_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_VID_ASSIGN_PROTOCOL_BASED_GET_PROTOCOL_BASED_GET_ID,1)) 
  { 
    ret = ui_ppd_api_llp_vid_assign_protocol_based_get(current_line); 
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting function name after llp_vid_assign***", TRUE); 
  } 
  
  ui_ret = ret;
    
  goto exit; 
exit:        
  return ui_ret; 
}                
          
#endif /* llp_vid_assign */ 


#endif /* LINK_PPD_LIBRARIES */ 

