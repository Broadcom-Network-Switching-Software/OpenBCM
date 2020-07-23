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
 
  
#include <soc/dpp/PPD/ppd_api_llp_parse.h>
#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_llp_parse.h>

#if LINK_PPD_LIBRARIES

#ifdef UI_LLP_PARSE
/******************************************************************** 
 *  Function handler: tpid_values_set (section llp_parse)
 */
int 
  ui_ppd_api_llp_parse_tpid_values_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32 
    prm_tpid_vals_index = 0xFFFFFFFF;  
  SOC_PPC_LLP_PARSE_TPID_VALUES   
    prm_tpid_vals;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_parse"); 
  soc_sand_proc_name = "soc_ppd_llp_parse_tpid_values_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_LLP_PARSE_TPID_VALUES_clear(&prm_tpid_vals);
 
  /* Get parameters */ 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_llp_parse_tpid_values_get(
          unit,
          &prm_tpid_vals
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_llp_parse_tpid_values_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_parse_tpid_values_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_PARSE_TPID_VALUES_SET_TPID_VALUES_SET_TPID_VALS_TPID_VALS_INDEX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_PARSE_TPID_VALUES_SET_TPID_VALUES_SET_TPID_VALS_TPID_VALS_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tpid_vals_index = (uint32)param_val->value.ulong_value;
  } 

  if(prm_tpid_vals_index != 0xFFFFFFFF && prm_tpid_vals_index < SOC_PPC_LLP_PARSE_NOF_TPID_VALS)          
  { 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_PARSE_TPID_VALUES_SET_TPID_VALUES_SET_TPID_VALS_TPID_VALS_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_PARSE_TPID_VALUES_SET_TPID_VALUES_SET_TPID_VALS_TPID_VALS_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tpid_vals.tpid_vals[prm_tpid_vals_index] = (uint16)param_val->value.ulong_value;
  } 

  }   


  /* Call function */
  ret = soc_ppd_llp_parse_tpid_values_set(
          unit,
          &prm_tpid_vals
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_llp_parse_tpid_values_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_parse_tpid_values_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: tpid_values_get (section llp_parse)
 */
int 
  ui_ppd_api_llp_parse_tpid_values_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_LLP_PARSE_TPID_VALUES   
    prm_tpid_vals;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_parse"); 
  soc_sand_proc_name = "soc_ppd_llp_parse_tpid_values_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_LLP_PARSE_TPID_VALUES_clear(&prm_tpid_vals);
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_ppd_llp_parse_tpid_values_get(
          unit,
          &prm_tpid_vals
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_llp_parse_tpid_values_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_parse_tpid_values_get");   
    goto exit; 
  } 

  SOC_PPC_LLP_PARSE_TPID_VALUES_print(&prm_tpid_vals);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: tpid_profile_info_set (section llp_parse)
 */
int 
  ui_ppd_api_llp_parse_tpid_profile_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_tpid_profile_ndx;
  SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO   
    prm_tpid_profile_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_parse"); 
  soc_sand_proc_name = "soc_ppd_llp_parse_tpid_profile_info_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO_clear(&prm_tpid_profile_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_PARSE_TPID_PROFILE_INFO_SET_TPID_PROFILE_INFO_SET_TPID_PROFILE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_PARSE_TPID_PROFILE_INFO_SET_TPID_PROFILE_INFO_SET_TPID_PROFILE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_tpid_profile_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter tpid_profile_ndx after tpid_profile_info_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_llp_parse_tpid_profile_info_get(
          unit,
          prm_tpid_profile_ndx,
          &prm_tpid_profile_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_llp_parse_tpid_profile_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_parse_tpid_profile_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_PARSE_TPID_PROFILE_INFO_SET_TPID_PROFILE_INFO_SET_TPID_PROFILE_INFO_TPID2_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LLP_PARSE_TPID_PROFILE_INFO_SET_TPID_PROFILE_INFO_SET_TPID_PROFILE_INFO_TPID2_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tpid_profile_info.tpid2.type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_PARSE_TPID_PROFILE_INFO_SET_TPID_PROFILE_INFO_SET_TPID_PROFILE_INFO_TPID2_INDEX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_PARSE_TPID_PROFILE_INFO_SET_TPID_PROFILE_INFO_SET_TPID_PROFILE_INFO_TPID2_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tpid_profile_info.tpid2.index = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_PARSE_TPID_PROFILE_INFO_SET_TPID_PROFILE_INFO_SET_TPID_PROFILE_INFO_TPID1_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LLP_PARSE_TPID_PROFILE_INFO_SET_TPID_PROFILE_INFO_SET_TPID_PROFILE_INFO_TPID1_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tpid_profile_info.tpid1.type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_PARSE_TPID_PROFILE_INFO_SET_TPID_PROFILE_INFO_SET_TPID_PROFILE_INFO_TPID1_INDEX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_PARSE_TPID_PROFILE_INFO_SET_TPID_PROFILE_INFO_SET_TPID_PROFILE_INFO_TPID1_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tpid_profile_info.tpid1.index = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_llp_parse_tpid_profile_info_set(
          unit,
          prm_tpid_profile_ndx,
          &prm_tpid_profile_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_llp_parse_tpid_profile_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_parse_tpid_profile_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: tpid_profile_info_get (section llp_parse)
 */
int 
  ui_ppd_api_llp_parse_tpid_profile_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_tpid_profile_ndx;
  SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO   
    prm_tpid_profile_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_parse"); 
  soc_sand_proc_name = "soc_ppd_llp_parse_tpid_profile_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO_clear(&prm_tpid_profile_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_PARSE_TPID_PROFILE_INFO_GET_TPID_PROFILE_INFO_GET_TPID_PROFILE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_PARSE_TPID_PROFILE_INFO_GET_TPID_PROFILE_INFO_GET_TPID_PROFILE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_tpid_profile_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter tpid_profile_ndx after tpid_profile_info_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_llp_parse_tpid_profile_info_get(
          unit,
          prm_tpid_profile_ndx,
          &prm_tpid_profile_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_llp_parse_tpid_profile_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_parse_tpid_profile_info_get");   
    goto exit; 
  } 

  SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO_print(&prm_tpid_profile_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: port_profile_to_tpid_profile_map_set (section llp_parse)
 */
int 
  ui_ppd_api_llp_parse_port_profile_to_tpid_profile_map_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_port_profile_ndx;
  uint32   
    prm_tpid_profile_id;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_parse"); 
  soc_sand_proc_name = "soc_ppd_llp_parse_port_profile_to_tpid_profile_map_set"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_SET_PORT_PROFILE_TO_TPID_PROFILE_MAP_SET_PORT_PROFILE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_SET_PORT_PROFILE_TO_TPID_PROFILE_MAP_SET_PORT_PROFILE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_port_profile_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter port_profile_ndx after port_profile_to_tpid_profile_map_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_llp_parse_port_profile_to_tpid_profile_map_get(
          unit,
          prm_port_profile_ndx,
          &prm_tpid_profile_id
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_llp_parse_port_profile_to_tpid_profile_map_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_parse_port_profile_to_tpid_profile_map_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_SET_PORT_PROFILE_TO_TPID_PROFILE_MAP_SET_TPID_PROFILE_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_SET_PORT_PROFILE_TO_TPID_PROFILE_MAP_SET_TPID_PROFILE_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tpid_profile_id = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_llp_parse_port_profile_to_tpid_profile_map_set(
          unit,
          prm_port_profile_ndx,
          prm_tpid_profile_id
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_llp_parse_port_profile_to_tpid_profile_map_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_parse_port_profile_to_tpid_profile_map_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: port_profile_to_tpid_profile_map_get (section llp_parse)
 */
int 
  ui_ppd_api_llp_parse_port_profile_to_tpid_profile_map_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_port_profile_ndx;
  uint32   
    prm_tpid_profile_id;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_parse"); 
  soc_sand_proc_name = "soc_ppd_llp_parse_port_profile_to_tpid_profile_map_get"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_GET_PORT_PROFILE_TO_TPID_PROFILE_MAP_GET_PORT_PROFILE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_GET_PORT_PROFILE_TO_TPID_PROFILE_MAP_GET_PORT_PROFILE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_port_profile_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter port_profile_ndx after port_profile_to_tpid_profile_map_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_llp_parse_port_profile_to_tpid_profile_map_get(
          unit,
          prm_port_profile_ndx,
          &prm_tpid_profile_id
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_llp_parse_port_profile_to_tpid_profile_map_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_parse_port_profile_to_tpid_profile_map_get");   
    goto exit; 
  } 

  cli_out("tpid_profile_id: %u\n\r",prm_tpid_profile_id);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: packet_format_info_set (section llp_parse)
 */
int 
  ui_ppd_api_llp_parse_packet_format_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_port_profile_ndx;
  SOC_PPC_LLP_PARSE_INFO   
    prm_parse_key;
  SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO   
    prm_format_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_parse"); 
  soc_sand_proc_name = "soc_ppd_llp_parse_packet_format_info_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_LLP_PARSE_INFO_clear(&prm_parse_key);
  SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO_clear(&prm_format_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_PARSE_PACKET_FORMAT_INFO_SET_PACKET_FORMAT_INFO_SET_PORT_PROFILE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_PARSE_PACKET_FORMAT_INFO_SET_PACKET_FORMAT_INFO_SET_PORT_PROFILE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_port_profile_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter port_profile_ndx after packet_format_info_set***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_PARSE_PACKET_FORMAT_INFO_SET_PACKET_FORMAT_INFO_SET_PARSE_KEY_IS_OUTER_PRIO_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_PARSE_PACKET_FORMAT_INFO_SET_PACKET_FORMAT_INFO_SET_PARSE_KEY_IS_OUTER_PRIO_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_parse_key.is_outer_prio = (uint8)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_PARSE_PACKET_FORMAT_INFO_SET_PACKET_FORMAT_INFO_SET_PARSE_KEY_INNER_TPID_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LLP_PARSE_PACKET_FORMAT_INFO_SET_PACKET_FORMAT_INFO_SET_PARSE_KEY_INNER_TPID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_parse_key.inner_tpid = param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_PARSE_PACKET_FORMAT_INFO_SET_PACKET_FORMAT_INFO_SET_PARSE_KEY_OUTER_TPID_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LLP_PARSE_PACKET_FORMAT_INFO_SET_PACKET_FORMAT_INFO_SET_PARSE_KEY_OUTER_TPID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_parse_key.outer_tpid = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter parse_key after packet_format_info_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_llp_parse_packet_format_info_get(
          unit,
          prm_port_profile_ndx,
          &prm_parse_key,
          &prm_format_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_llp_parse_packet_format_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_parse_packet_format_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_PARSE_PACKET_FORMAT_INFO_SET_PACKET_FORMAT_INFO_SET_FORMAT_INFO_DFLT_EDIT_PCP_PROFILE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_PARSE_PACKET_FORMAT_INFO_SET_PACKET_FORMAT_INFO_SET_FORMAT_INFO_DFLT_EDIT_PCP_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_format_info.dflt_edit_pcp_profile = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_PARSE_PACKET_FORMAT_INFO_SET_PACKET_FORMAT_INFO_SET_FORMAT_INFO_DLFT_EDIT_COMMAND_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_PARSE_PACKET_FORMAT_INFO_SET_PACKET_FORMAT_INFO_SET_FORMAT_INFO_DLFT_EDIT_COMMAND_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_format_info.dlft_edit_command_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_PARSE_PACKET_FORMAT_INFO_SET_PACKET_FORMAT_INFO_SET_FORMAT_INFO_PRIORITY_TAG_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LLP_PARSE_PACKET_FORMAT_INFO_SET_PACKET_FORMAT_INFO_SET_FORMAT_INFO_PRIORITY_TAG_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_format_info.priority_tag_type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_PARSE_PACKET_FORMAT_INFO_SET_PACKET_FORMAT_INFO_SET_FORMAT_INFO_TAG_FORMAT_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LLP_PARSE_PACKET_FORMAT_INFO_SET_PACKET_FORMAT_INFO_SET_FORMAT_INFO_TAG_FORMAT_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_format_info.tag_format = param_val->numeric_equivalent;
  } 

  /*trap-code
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_PARSE_PACKET_FORMAT_INFO_SET_PACKET_FORMAT_INFO_SET_FORMAT_INFO_ACTION_TRAP_CODE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LLP_PARSE_PACKET_FORMAT_INFO_SET_PACKET_FORMAT_INFO_SET_FORMAT_INFO_ACTION_TRAP_CODE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_format_info.action_trap_code = (SOC_PPC_TRAP_CODE)param_val->numeric_equivalent;
  } 
  */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_PARSE_PACKET_FORMAT_INFO_SET_PACKET_FORMAT_INFO_SET_FORMAT_INFO_ACTION_TRAP_CODE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_PARSE_PACKET_FORMAT_INFO_SET_PACKET_FORMAT_INFO_SET_FORMAT_INFO_ACTION_TRAP_CODE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_format_info.action_trap_code = (SOC_PPC_TRAP_CODE)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_llp_parse_packet_format_info_set(
          unit,
          prm_port_profile_ndx,
          &prm_parse_key,
          &prm_format_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_llp_parse_packet_format_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_parse_packet_format_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: packet_format_info_get (section llp_parse)
 */
int 
  ui_ppd_api_llp_parse_packet_format_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_port_profile_ndx;
  SOC_PPC_LLP_PARSE_INFO   
    prm_parse_key;
  SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO   
    prm_format_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_parse"); 
  soc_sand_proc_name = "soc_ppd_llp_parse_packet_format_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_LLP_PARSE_INFO_clear(&prm_parse_key);
  SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO_clear(&prm_format_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_PARSE_PACKET_FORMAT_INFO_GET_PACKET_FORMAT_INFO_GET_PORT_PROFILE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_PARSE_PACKET_FORMAT_INFO_GET_PACKET_FORMAT_INFO_GET_PORT_PROFILE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_port_profile_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter port_profile_ndx after packet_format_info_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_PARSE_PACKET_FORMAT_INFO_GET_PACKET_FORMAT_INFO_GET_PARSE_KEY_IS_OUTER_PRIO_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_PARSE_PACKET_FORMAT_INFO_GET_PACKET_FORMAT_INFO_GET_PARSE_KEY_IS_OUTER_PRIO_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_parse_key.is_outer_prio = (uint8)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_PARSE_PACKET_FORMAT_INFO_GET_PACKET_FORMAT_INFO_GET_PARSE_KEY_INNER_TPID_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LLP_PARSE_PACKET_FORMAT_INFO_GET_PACKET_FORMAT_INFO_GET_PARSE_KEY_INNER_TPID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_parse_key.inner_tpid = param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_PARSE_PACKET_FORMAT_INFO_GET_PACKET_FORMAT_INFO_GET_PARSE_KEY_OUTER_TPID_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LLP_PARSE_PACKET_FORMAT_INFO_GET_PACKET_FORMAT_INFO_GET_PARSE_KEY_OUTER_TPID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_parse_key.outer_tpid = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter parse_key after packet_format_info_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_llp_parse_packet_format_info_get(
          unit,
          prm_port_profile_ndx,
          &prm_parse_key,
          &prm_format_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_llp_parse_packet_format_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_parse_packet_format_info_get");   
    goto exit; 
  } 

  SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO_print(&prm_format_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
#endif
#ifdef UI_LLP_PARSE/* { llp_parse*/
/******************************************************************** 
 *  Section handler: llp_parse
 */ 
int 
  ui_ppd_api_llp_parse( 
    CURRENT_LINE *current_line 
  ) 
{   
  int
    ret = FALSE;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_parse"); 
 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_PARSE_TPID_VALUES_SET_TPID_VALUES_SET_ID,1)) 
  { 
    ret = ui_ppd_api_llp_parse_tpid_values_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_PARSE_TPID_VALUES_GET_TPID_VALUES_GET_ID,1)) 
  { 
    ret = ui_ppd_api_llp_parse_tpid_values_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_PARSE_TPID_PROFILE_INFO_SET_TPID_PROFILE_INFO_SET_ID,1)) 
  { 
    ret = ui_ppd_api_llp_parse_tpid_profile_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_PARSE_TPID_PROFILE_INFO_GET_TPID_PROFILE_INFO_GET_ID,1)) 
  { 
    ret = ui_ppd_api_llp_parse_tpid_profile_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_SET_PORT_PROFILE_TO_TPID_PROFILE_MAP_SET_ID,1)) 
  { 
    ret = ui_ppd_api_llp_parse_port_profile_to_tpid_profile_map_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_PARSE_PORT_PROFILE_TO_TPID_PROFILE_MAP_GET_PORT_PROFILE_TO_TPID_PROFILE_MAP_GET_ID,1)) 
  { 
    ret = ui_ppd_api_llp_parse_port_profile_to_tpid_profile_map_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_PARSE_PACKET_FORMAT_INFO_SET_PACKET_FORMAT_INFO_SET_ID,1)) 
  { 
    ret = ui_ppd_api_llp_parse_packet_format_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_PARSE_PACKET_FORMAT_INFO_GET_PACKET_FORMAT_INFO_GET_ID,1)) 
  { 
    ret = ui_ppd_api_llp_parse_packet_format_info_get(current_line); 
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting function name after llp_parse***", TRUE); 
  } 
  
  ui_ret = ret;
    
  goto exit; 
exit:        
  return ui_ret; 
}                
          
#endif /* llp_parse */ 


#endif /* LINK_PPD_LIBRARIES */ 

