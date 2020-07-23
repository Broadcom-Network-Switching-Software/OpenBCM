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
 
  
#include <soc/dpp/PPD/ppd_api_fp.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_fp.h>

#if LINK_PPD_LIBRARIES

#ifdef UI_FP
/******************************************************************** 
 *  Function handler: packet_format_group_set (section fp)
 */
int 
  ui_ppd_api_fp_packet_format_group_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_pfg_ndx;
  SOC_PPC_PMF_PFG_INFO   
    prm_info;
  SOC_SAND_SUCCESS_FAILURE   
    prm_success;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_fp"); 
  soc_sand_proc_name = "soc_ppd_fp_packet_format_group_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_PMF_PFG_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_PACKET_FORMAT_GROUP_SET_PACKET_FORMAT_GROUP_SET_PFG_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FP_PACKET_FORMAT_GROUP_SET_PACKET_FORMAT_GROUP_SET_PFG_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_pfg_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter pfg_ndx after packet_format_group_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_fp_packet_format_group_get(
          unit,
          prm_pfg_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_fp_packet_format_group_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_fp_packet_format_group_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_PACKET_FORMAT_GROUP_SET_PACKET_FORMAT_GROUP_SET_INFO_PP_PORTS_BMP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FP_PACKET_FORMAT_GROUP_SET_PACKET_FORMAT_GROUP_SET_INFO_PP_PORTS_BMP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.pp_ports_bmp.arr[0] = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_PACKET_FORMAT_GROUP_SET_PACKET_FORMAT_GROUP_SET_INFO_VLAN_TAG_STRUCTURE_BMP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FP_PACKET_FORMAT_GROUP_SET_PACKET_FORMAT_GROUP_SET_INFO_VLAN_TAG_STRUCTURE_BMP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.vlan_tag_structure_bmp = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_PACKET_FORMAT_GROUP_SET_PACKET_FORMAT_GROUP_SET_INFO_HDR_FORMAT_BMP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FP_PACKET_FORMAT_GROUP_SET_PACKET_FORMAT_GROUP_SET_INFO_HDR_FORMAT_BMP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.hdr_format_bmp = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_fp_packet_format_group_set(
          unit,
          prm_pfg_ndx,
          &prm_info,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_fp_packet_format_group_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_fp_packet_format_group_set");   
    goto exit; 
  } 

  cli_out("success: %s\n\r",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: packet_format_group_get (section fp)
 */
int 
  ui_ppd_api_fp_packet_format_group_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_pfg_ndx;
  SOC_PPC_PMF_PFG_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_fp"); 
  soc_sand_proc_name = "soc_ppd_fp_packet_format_group_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_PMF_PFG_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_PACKET_FORMAT_GROUP_GET_PACKET_FORMAT_GROUP_GET_PFG_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FP_PACKET_FORMAT_GROUP_GET_PACKET_FORMAT_GROUP_GET_PFG_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_pfg_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter pfg_ndx after packet_format_group_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_fp_packet_format_group_get(
          unit,
          prm_pfg_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_fp_packet_format_group_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_fp_packet_format_group_get");   
    goto exit; 
  } 

  SOC_PPC_PMF_PFG_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: database_create (section fp)
 */
int 
  ui_ppd_api_fp_database_create(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_db_id_ndx;
  SOC_PPC_FP_DATABASE_INFO   
    prm_info;
  SOC_SAND_SUCCESS_FAILURE   
    prm_success;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_fp"); 
  soc_sand_proc_name = "soc_ppd_fp_database_create"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FP_DATABASE_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_DATABASE_CREATE_DATABASE_CREATE_DB_ID_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FP_DATABASE_CREATE_DATABASE_CREATE_DB_ID_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_db_id_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter db_id_ndx after database_create***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_DATABASE_CREATE_DATABASE_CREATE_INFO_STRENGTH_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FP_DATABASE_CREATE_DATABASE_CREATE_INFO_STRENGTH_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.strength = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_DATABASE_CREATE_DATABASE_CREATE_INFO_ACTION_TYPES_INDEX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FP_DATABASE_CREATE_DATABASE_CREATE_INFO_ACTION_TYPES_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_DATABASE_CREATE_DATABASE_CREATE_INFO_ACTION_TYPE0_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FP_DATABASE_CREATE_DATABASE_CREATE_INFO_ACTION_TYPE0_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.action_types[0] = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_DATABASE_CREATE_DATABASE_CREATE_INFO_ACTION_TYPE1_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FP_DATABASE_CREATE_DATABASE_CREATE_INFO_ACTION_TYPE1_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.action_types[1] = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_DATABASE_CREATE_DATABASE_CREATE_INFO_ACTION_TYPE2_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FP_DATABASE_CREATE_DATABASE_CREATE_INFO_ACTION_TYPE2_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.action_types[2] = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_DATABASE_CREATE_DATABASE_CREATE_INFO_ACTION_TYPE3_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FP_DATABASE_CREATE_DATABASE_CREATE_INFO_ACTION_TYPE3_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.action_types[3] = param_val->numeric_equivalent;
  } 
  
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_DATABASE_CREATE_DATABASE_CREATE_INFO_QUAL_TYPE0_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FP_DATABASE_CREATE_DATABASE_CREATE_INFO_QUAL_TYPE0_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.qual_types[0] = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_DATABASE_CREATE_DATABASE_CREATE_INFO_QUAL_TYPE1_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FP_DATABASE_CREATE_DATABASE_CREATE_INFO_QUAL_TYPE1_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.qual_types[1] = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_DATABASE_CREATE_DATABASE_CREATE_INFO_QUAL_TYPE2_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FP_DATABASE_CREATE_DATABASE_CREATE_INFO_QUAL_TYPE2_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.qual_types[2] = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_DATABASE_CREATE_DATABASE_CREATE_INFO_QUAL_TYPE3_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FP_DATABASE_CREATE_DATABASE_CREATE_INFO_QUAL_TYPE3_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.qual_types[3] = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_DATABASE_CREATE_DATABASE_CREATE_INFO_SUPPORTED_PFGS_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FP_DATABASE_CREATE_DATABASE_CREATE_INFO_SUPPORTED_PFGS_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.supported_pfgs = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_DATABASE_CREATE_DATABASE_CREATE_INFO_DB_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FP_DATABASE_CREATE_DATABASE_CREATE_INFO_DB_TYPE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.db_type = param_val->numeric_equivalent;
  } 


  /* Call function */
  ret = soc_ppd_fp_database_create(
          unit,
          prm_db_id_ndx,
          &prm_info,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_fp_database_create - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_fp_database_create");   
    goto exit; 
  } 

  cli_out("success: %s\n\r",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: database_get (section fp)
 */
int 
  ui_ppd_api_fp_database_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_db_id_ndx;
  SOC_PPC_FP_DATABASE_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_fp"); 
  soc_sand_proc_name = "soc_ppd_fp_database_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FP_DATABASE_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_DATABASE_GET_DATABASE_GET_DB_ID_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FP_DATABASE_GET_DATABASE_GET_DB_ID_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_db_id_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter db_id_ndx after database_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_fp_database_get(
          unit,
          prm_db_id_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_fp_database_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_fp_database_get");   
    goto exit; 
  } 

  SOC_PPC_FP_DATABASE_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: database_destroy (section fp)
 */
int 
  ui_ppd_api_fp_database_destroy(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_db_id_ndx;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_fp"); 
  soc_sand_proc_name = "soc_ppd_fp_database_destroy"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_DATABASE_DESTROY_DATABASE_DESTROY_DB_ID_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FP_DATABASE_DESTROY_DATABASE_DESTROY_DB_ID_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_db_id_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter db_id_ndx after database_destroy***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_fp_database_destroy(
          unit,
          prm_db_id_ndx
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_fp_database_destroy - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_fp_database_destroy");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: entry_add (section fp)
 */
int 
  ui_ppd_api_fp_entry_add(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_db_id_ndx;
  uint32   
    prm_entry_id_ndx;
  SOC_PPC_FP_DATABASE_INFO
    db_info;
  SOC_PPC_FP_ENTRY_INFO   
    prm_info;
  SOC_SAND_SUCCESS_FAILURE   
    prm_success;
  unsigned int 
    param_i, action_and_val_ndx;
 
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_fp"); 
  soc_sand_proc_name = "soc_ppd_fp_entry_add"; 
 
  unit = soc_ppd_get_default_unit();
  SOC_PPC_FP_DATABASE_INFO_clear(&db_info);
  SOC_PPC_FP_ENTRY_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_ENTRY_ADD_ENTRY_ADD_DB_ID_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FP_ENTRY_ADD_ENTRY_ADD_DB_ID_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_db_id_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter db_id_ndx after entry_add***", TRUE); 
    goto exit; 
  }

  ret = soc_ppd_fp_database_get(
          unit,
          prm_db_id_ndx,
          &db_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_fp_entry_add - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_fp_entry_add");   
    goto exit; 
  }

  for (action_and_val_ndx = 0; action_and_val_ndx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX; action_and_val_ndx++)
  {
    prm_info.actions[action_and_val_ndx].type = db_info.action_types[action_and_val_ndx];
  }
  for (action_and_val_ndx = 0; action_and_val_ndx < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; action_and_val_ndx++)
  {
    prm_info.qual_vals[action_and_val_ndx].type = db_info.qual_types[action_and_val_ndx];
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_ENTRY_ADD_ENTRY_ADD_ENTRY_ID_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FP_ENTRY_ADD_ENTRY_ADD_ENTRY_ID_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_entry_id_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter entry_id_ndx after entry_add***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_ENTRY_ADD_ENTRY_ADD_INFO_PRIORITY_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FP_ENTRY_ADD_ENTRY_ADD_INFO_PRIORITY_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.priority = (uint32)param_val->value.ulong_value;
  }
  
  for (param_i=1; param_i<=5; ++param_i)
  {
    ret = search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_ENTRY_ADD_ENTRY_ADD_INFO_ACTIONS_VAL_ARR_ID,param_i);
    if (0 != ret)
    {
      ret=0;
      break;
    }
    UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(SOC_PARAM_PPD_FP_ENTRY_ADD_ENTRY_ADD_INFO_ACTIONS_VAL_ARR_ID,param_i);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.actions[param_i - 1].val = (uint32)param_val->value.ulong_value;
  }

  for (param_i=1; param_i<=5; ++param_i)
  {
    ret = search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_ENTRY_ADD_ENTRY_ADD_INFO_QUAL_VALS_IS_VALID_LSB_ARR_ID,param_i);
    if (0 != ret)
    {
      ret=0;
      break;
    }
    UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(SOC_PARAM_PPD_FP_ENTRY_ADD_ENTRY_ADD_INFO_QUAL_VALS_IS_VALID_LSB_ARR_ID,param_i);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.qual_vals[param_i - 1].is_valid.arr[0] = (uint32)param_val->value.ulong_value;    
  }

  for (param_i=1; param_i<=5; ++param_i)
  {
    ret = search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_ENTRY_ADD_ENTRY_ADD_INFO_QUAL_VALS_IS_VALID_MSB_ARR_ID,param_i);
    if (0 != ret)
    {
      ret=0;
      break;
    }
    UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(SOC_PARAM_PPD_FP_ENTRY_ADD_ENTRY_ADD_INFO_QUAL_VALS_IS_VALID_MSB_ARR_ID,param_i);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.qual_vals[param_i - 1].is_valid.arr[1] = (uint32)param_val->value.ulong_value; 
  }

  for (param_i=1; param_i<=5; ++param_i)
  {
    ret = search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_ENTRY_ADD_ENTRY_ADD_INFO_QUAL_VALS_VAL_LSB_ARR_ID,param_i);
    if (0 != ret)
    {
      ret=0;
      break;
    }
    UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(SOC_PARAM_PPD_FP_ENTRY_ADD_ENTRY_ADD_INFO_QUAL_VALS_VAL_LSB_ARR_ID,param_i);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.qual_vals[param_i - 1].val.arr[0] = (uint32)param_val->value.ulong_value; 
  }

  for (param_i=1; param_i<=5; ++param_i)
  {
    ret = search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_ENTRY_ADD_ENTRY_ADD_INFO_QUAL_VALS_VAL_MSB_ARR_ID,param_i);
    if (0 != ret)
    {
      ret=0;
      break;
    }
    UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(SOC_PARAM_PPD_FP_ENTRY_ADD_ENTRY_ADD_INFO_QUAL_VALS_VAL_MSB_ARR_ID,param_i);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.qual_vals[param_i - 1].val.arr[1] = (uint32)param_val->value.ulong_value; 
  }
 
  /* Call function */
  ret = soc_ppd_fp_entry_add(
          unit,
          prm_db_id_ndx,
          prm_entry_id_ndx,
          &prm_info,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_fp_entry_add - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_fp_entry_add");   
    goto exit; 
  } 

  cli_out("success: %s\n\r",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: entry_get (section fp)
 */
int 
  ui_ppd_api_fp_entry_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_db_id_ndx;
  uint32   
    prm_entry_id_ndx;
  uint8   
    prm_is_found;
  SOC_PPC_FP_ENTRY_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_fp"); 
  soc_sand_proc_name = "soc_ppd_fp_entry_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FP_ENTRY_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_ENTRY_GET_ENTRY_GET_DB_ID_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FP_ENTRY_GET_ENTRY_GET_DB_ID_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_db_id_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter db_id_ndx after entry_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_ENTRY_GET_ENTRY_GET_ENTRY_ID_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FP_ENTRY_GET_ENTRY_GET_ENTRY_ID_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_entry_id_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter entry_id_ndx after entry_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_fp_entry_get(
          unit,
          prm_db_id_ndx,
          prm_entry_id_ndx,
          &prm_is_found,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_fp_entry_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_fp_entry_get");   
    goto exit; 
  } 

  cli_out("is_found: %u\n\r",prm_is_found);

  SOC_PPC_FP_ENTRY_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: entry_remove (section fp)
 */
int 
  ui_ppd_api_fp_entry_remove(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_db_id_ndx;
  uint32   
    prm_entry_id_ndx;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_fp"); 
  soc_sand_proc_name = "soc_ppd_fp_entry_remove"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_ENTRY_REMOVE_ENTRY_REMOVE_DB_ID_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FP_ENTRY_REMOVE_ENTRY_REMOVE_DB_ID_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_db_id_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter db_id_ndx after entry_remove***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_ENTRY_REMOVE_ENTRY_REMOVE_ENTRY_ID_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FP_ENTRY_REMOVE_ENTRY_REMOVE_ENTRY_ID_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_entry_id_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter entry_id_ndx after entry_remove***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_fp_entry_remove(
          unit,
          prm_db_id_ndx,
          prm_entry_id_ndx,
          0
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_fp_entry_remove - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_fp_entry_remove");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: database_entries_get_block (section fp)
 */
int 
  ui_ppd_api_fp_database_entries_get_block(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_db_id_ndx,
    ndx;
  SOC_SAND_TABLE_BLOCK_RANGE   
    prm_block_range;
  SOC_PPC_FP_ENTRY_INFO   
    prm_entries[100];
  uint32   
    prm_nof_entries;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_fp"); 
  soc_sand_proc_name = "soc_ppd_fp_database_entries_get_block"; 
 
  unit = soc_ppd_get_default_unit(); 
  soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&prm_block_range);
  for (ndx = 0; ndx < 100; ++ndx)
  {
    SOC_PPC_FP_ENTRY_INFO_clear(&prm_entries[ndx]);
  }
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_DATABASE_ENTRIES_GET_BLOCK_DATABASE_ENTRIES_GET_BLOCK_DB_ID_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FP_DATABASE_ENTRIES_GET_BLOCK_DATABASE_ENTRIES_GET_BLOCK_DB_ID_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_db_id_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter db_id_ndx after database_entries_get_block***", TRUE); 
    goto exit; 
  } 

  prm_block_range.entries_to_scan = 100;
  prm_block_range.entries_to_act  = 100;

  /* Call function */
  ret = soc_ppd_fp_database_entries_get_block(
          unit,
          prm_db_id_ndx,
          &prm_block_range,
          prm_entries,
          &prm_nof_entries
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_fp_database_entries_get_block - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_fp_database_entries_get_block");   
    goto exit; 
  } 

  for (ndx = 0; ndx <prm_nof_entries; ++ndx)
  {
    SOC_PPC_FP_ENTRY_INFO_print(&prm_entries[ndx]);
  }

  cli_out("nof_entries: %u\n\r",prm_nof_entries);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: direct_extraction_entry_add (section fp)
 */
int 
  ui_ppd_api_fp_direct_extraction_entry_add(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_db_id_ndx;
  uint32   
    prm_entry_id_ndx;
  uint32 
    prm_actions_index = 0xFFFFFFFF;  
  uint32 
    prm_fld_ext_index = 0xFFFFFFFF;  
  uint32 
    prm_qual_vals_index = 0xFFFFFFFF;  
  SOC_PPC_FP_DIR_EXTR_ENTRY_INFO   
    prm_info;
  SOC_SAND_SUCCESS_FAILURE   
    prm_success;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_fp"); 
  soc_sand_proc_name = "soc_ppd_fp_direct_extraction_entry_add"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FP_DIR_EXTR_ENTRY_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_DIRECT_EXTRACTION_ENTRY_ADD_DIRECT_EXTRACTION_ENTRY_ADD_DB_ID_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FP_DIRECT_EXTRACTION_ENTRY_ADD_DIRECT_EXTRACTION_ENTRY_ADD_DB_ID_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_db_id_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter db_id_ndx after direct_extraction_entry_add***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_DIRECT_EXTRACTION_ENTRY_ADD_DIRECT_EXTRACTION_ENTRY_ADD_ENTRY_ID_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FP_DIRECT_EXTRACTION_ENTRY_ADD_DIRECT_EXTRACTION_ENTRY_ADD_ENTRY_ID_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_entry_id_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter entry_id_ndx after direct_extraction_entry_add***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_DIRECT_EXTRACTION_ENTRY_ADD_DIRECT_EXTRACTION_ENTRY_ADD_INFO_PRIORITY_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FP_DIRECT_EXTRACTION_ENTRY_ADD_DIRECT_EXTRACTION_ENTRY_ADD_INFO_PRIORITY_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.priority = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_DIRECT_EXTRACTION_ENTRY_ADD_DIRECT_EXTRACTION_ENTRY_ADD_INFO_ACTIONS_INDEX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FP_DIRECT_EXTRACTION_ENTRY_ADD_DIRECT_EXTRACTION_ENTRY_ADD_INFO_ACTIONS_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_actions_index = (uint32)param_val->value.ulong_value;
  } 

  if(prm_actions_index != 0xFFFFFFFF)          
  { 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_DIRECT_EXTRACTION_ENTRY_ADD_DIRECT_EXTRACTION_ENTRY_ADD_INFO_ACTIONS_BASE_VAL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FP_DIRECT_EXTRACTION_ENTRY_ADD_DIRECT_EXTRACTION_ENTRY_ADD_INFO_ACTIONS_BASE_VAL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.actions[ prm_actions_index].base_val = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_DIRECT_EXTRACTION_ENTRY_ADD_DIRECT_EXTRACTION_ENTRY_ADD_INFO_ACTIONS_NOF_FIELDS_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FP_DIRECT_EXTRACTION_ENTRY_ADD_DIRECT_EXTRACTION_ENTRY_ADD_INFO_ACTIONS_NOF_FIELDS_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.actions[ prm_actions_index].nof_fields = (uint32)param_val->value.ulong_value;
  } 

  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_DIRECT_EXTRACTION_ENTRY_ADD_DIRECT_EXTRACTION_ENTRY_ADD_INFO_ACTIONS_FLD_EXT_INDEX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FP_DIRECT_EXTRACTION_ENTRY_ADD_DIRECT_EXTRACTION_ENTRY_ADD_INFO_ACTIONS_FLD_EXT_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fld_ext_index = (uint32)param_val->value.ulong_value;
  } 

  if(prm_fld_ext_index != 0xFFFFFFFF)          
  { 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_DIRECT_EXTRACTION_ENTRY_ADD_DIRECT_EXTRACTION_ENTRY_ADD_INFO_ACTIONS_FLD_EXT_NOF_BITS_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FP_DIRECT_EXTRACTION_ENTRY_ADD_DIRECT_EXTRACTION_ENTRY_ADD_INFO_ACTIONS_FLD_EXT_NOF_BITS_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.actions[ prm_actions_index].fld_ext[ prm_fld_ext_index].nof_bits = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_DIRECT_EXTRACTION_ENTRY_ADD_DIRECT_EXTRACTION_ENTRY_ADD_INFO_ACTIONS_FLD_EXT_CST_VAL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FP_DIRECT_EXTRACTION_ENTRY_ADD_DIRECT_EXTRACTION_ENTRY_ADD_INFO_ACTIONS_FLD_EXT_CST_VAL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.actions[ prm_actions_index].fld_ext[ prm_fld_ext_index].cst_val = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_DIRECT_EXTRACTION_ENTRY_ADD_DIRECT_EXTRACTION_ENTRY_ADD_INFO_ACTIONS_FLD_EXT_FLD_LSB_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FP_DIRECT_EXTRACTION_ENTRY_ADD_DIRECT_EXTRACTION_ENTRY_ADD_INFO_ACTIONS_FLD_EXT_FLD_LSB_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.actions[ prm_actions_index].fld_ext[ prm_fld_ext_index].fld_lsb = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_DIRECT_EXTRACTION_ENTRY_ADD_DIRECT_EXTRACTION_ENTRY_ADD_INFO_ACTIONS_FLD_EXT_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FP_DIRECT_EXTRACTION_ENTRY_ADD_DIRECT_EXTRACTION_ENTRY_ADD_INFO_ACTIONS_FLD_EXT_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.actions[ prm_actions_index].fld_ext[ prm_fld_ext_index].type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_DIRECT_EXTRACTION_ENTRY_ADD_DIRECT_EXTRACTION_ENTRY_ADD_INFO_ACTIONS_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FP_DIRECT_EXTRACTION_ENTRY_ADD_DIRECT_EXTRACTION_ENTRY_ADD_INFO_ACTIONS_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.actions[ prm_actions_index].type = param_val->numeric_equivalent;
  } 

  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_DIRECT_EXTRACTION_ENTRY_ADD_DIRECT_EXTRACTION_ENTRY_ADD_INFO_QUAL_VALS_INDEX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FP_DIRECT_EXTRACTION_ENTRY_ADD_DIRECT_EXTRACTION_ENTRY_ADD_INFO_QUAL_VALS_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_qual_vals_index = (uint32)param_val->value.ulong_value;
  } 

  if(prm_qual_vals_index != 0xFFFFFFFF)          
  { 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_DIRECT_EXTRACTION_ENTRY_ADD_DIRECT_EXTRACTION_ENTRY_ADD_INFO_QUAL_VALS_IS_VALID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FP_DIRECT_EXTRACTION_ENTRY_ADD_DIRECT_EXTRACTION_ENTRY_ADD_INFO_QUAL_VALS_IS_VALID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.qual_vals[prm_qual_vals_index].is_valid.arr[0] = param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_DIRECT_EXTRACTION_ENTRY_ADD_DIRECT_EXTRACTION_ENTRY_ADD_INFO_QUAL_VALS_VAL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FP_DIRECT_EXTRACTION_ENTRY_ADD_DIRECT_EXTRACTION_ENTRY_ADD_INFO_QUAL_VALS_VAL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.qual_vals[ prm_qual_vals_index].val.arr[0] = param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_DIRECT_EXTRACTION_ENTRY_ADD_DIRECT_EXTRACTION_ENTRY_ADD_INFO_QUAL_VALS_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FP_DIRECT_EXTRACTION_ENTRY_ADD_DIRECT_EXTRACTION_ENTRY_ADD_INFO_QUAL_VALS_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.qual_vals[ prm_qual_vals_index].type = param_val->numeric_equivalent;
  } 

  }   


  /* Call function */
  ret = soc_ppd_fp_direct_extraction_entry_add(
          unit,
          prm_db_id_ndx,
          prm_entry_id_ndx,
          &prm_info,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_fp_direct_extraction_entry_add - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_fp_direct_extraction_entry_add");   
    goto exit; 
  } 

  cli_out("success: %s\n\r",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: direct_extraction_entry_get (section fp)
 */
int 
  ui_ppd_api_fp_direct_extraction_entry_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_db_id_ndx;
  uint32   
    prm_entry_id_ndx;
  uint8   
    prm_is_found;
  SOC_PPC_FP_DIR_EXTR_ENTRY_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_fp"); 
  soc_sand_proc_name = "soc_ppd_fp_direct_extraction_entry_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FP_DIR_EXTR_ENTRY_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_DIRECT_EXTRACTION_ENTRY_GET_DIRECT_EXTRACTION_ENTRY_GET_DB_ID_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FP_DIRECT_EXTRACTION_ENTRY_GET_DIRECT_EXTRACTION_ENTRY_GET_DB_ID_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_db_id_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter db_id_ndx after direct_extraction_entry_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_DIRECT_EXTRACTION_ENTRY_GET_DIRECT_EXTRACTION_ENTRY_GET_ENTRY_ID_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FP_DIRECT_EXTRACTION_ENTRY_GET_DIRECT_EXTRACTION_ENTRY_GET_ENTRY_ID_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_entry_id_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter entry_id_ndx after direct_extraction_entry_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_fp_direct_extraction_entry_get(
          unit,
          prm_db_id_ndx,
          prm_entry_id_ndx,
          &prm_is_found,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_fp_direct_extraction_entry_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_fp_direct_extraction_entry_get");   
    goto exit; 
  } 

  cli_out("is_found: %u\n\r",prm_is_found);

  SOC_PPC_FP_DIR_EXTR_ENTRY_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: direct_extraction_entry_remove (section fp)
 */
int 
  ui_ppd_api_fp_direct_extraction_entry_remove(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_db_id_ndx;
  uint32   
    prm_entry_id_ndx;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_fp"); 
  soc_sand_proc_name = "soc_ppd_fp_direct_extraction_entry_remove"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_DIRECT_EXTRACTION_ENTRY_REMOVE_DIRECT_EXTRACTION_ENTRY_REMOVE_DB_ID_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FP_DIRECT_EXTRACTION_ENTRY_REMOVE_DIRECT_EXTRACTION_ENTRY_REMOVE_DB_ID_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_db_id_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter db_id_ndx after direct_extraction_entry_remove***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_DIRECT_EXTRACTION_ENTRY_REMOVE_DIRECT_EXTRACTION_ENTRY_REMOVE_ENTRY_ID_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FP_DIRECT_EXTRACTION_ENTRY_REMOVE_DIRECT_EXTRACTION_ENTRY_REMOVE_ENTRY_ID_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_entry_id_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter entry_id_ndx after direct_extraction_entry_remove***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_fp_direct_extraction_entry_remove(
          unit,
          prm_db_id_ndx,
          prm_entry_id_ndx
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_fp_direct_extraction_entry_remove - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_fp_direct_extraction_entry_remove");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: direct_extraction_db_entries_get (section fp)
 */
int 
  ui_ppd_api_fp_direct_extraction_db_entries_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    idx,
    ret;   
  uint32   
    prm_db_id_ndx;
  SOC_SAND_TABLE_BLOCK_RANGE   
    prm_block_range;
  SOC_PPC_FP_DIR_EXTR_ENTRY_INFO   
    prm_entries[16];
  uint32   
    prm_nof_entries;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_fp"); 
  soc_sand_proc_name = "soc_ppd_fp_direct_extraction_db_entries_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&prm_block_range);
  for (idx = 0; idx < 16; ++idx)
  {
    SOC_PPC_FP_DIR_EXTR_ENTRY_INFO_clear(&prm_entries[idx]);
  }
 
  prm_block_range.entries_to_act = 16;
  prm_block_range.entries_to_scan = 16;
  prm_block_range.iter = 0;


  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_DIRECT_EXTRACTION_DB_ENTRIES_GET_DIRECT_EXTRACTION_DB_ENTRIES_GET_DB_ID_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FP_DIRECT_EXTRACTION_DB_ENTRIES_GET_DIRECT_EXTRACTION_DB_ENTRIES_GET_DB_ID_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_db_id_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter db_id_ndx after direct_extraction_db_entries_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_fp_direct_extraction_db_entries_get(
          unit,
          prm_db_id_ndx,
          &prm_block_range,
           prm_entries,
          &prm_nof_entries
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_fp_direct_extraction_db_entries_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_fp_direct_extraction_db_entries_get");   
    goto exit; 
  } 


  cli_out("nof_entries: %u\n\r",prm_nof_entries);
  for (idx = 0; idx < prm_nof_entries; ++idx)
  { 
    SOC_PPC_FP_DIR_EXTR_ENTRY_INFO_print(&prm_entries[idx]);
  }

  
  goto exit; 
exit: 
  return ui_ret; 
}

/******************************************************************** 
 *  Function handler: control_set (section fp)
 */
int 
  ui_ppd_api_fp_control_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_FP_CONTROL_INDEX   
    prm_control_ndx;
  SOC_PPC_FP_CONTROL_INFO   
    prm_info;
  SOC_SAND_SUCCESS_FAILURE
    prm_success;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_fp"); 
  soc_sand_proc_name = "soc_ppd_fp_control_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FP_CONTROL_INDEX_clear(&prm_control_ndx);
  SOC_PPC_FP_CONTROL_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_CONTROL_SET_CONTROL_SET_CONTROL_NDX_VAL_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FP_CONTROL_SET_CONTROL_SET_CONTROL_NDX_VAL_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_control_ndx.val_ndx = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_CONTROL_SET_CONTROL_SET_CONTROL_NDX_TYPE_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FP_CONTROL_SET_CONTROL_SET_CONTROL_NDX_TYPE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_control_ndx.type = param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_CONTROL_SET_CONTROL_SET_CONTROL_NDX_DB_ID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FP_CONTROL_SET_CONTROL_SET_CONTROL_NDX_DB_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_control_ndx.db_id = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter control_ndx after control_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_fp_control_get(
          unit,
          SOC_CORE_INVALID,
          &prm_control_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_fp_control_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_fp_control_get");   
    goto exit; 
  } 


  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_CONTROL_SET_CONTROL_SET_INFO_VAL0_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FP_CONTROL_SET_CONTROL_SET_INFO_VAL0_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.val[0] = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_CONTROL_SET_CONTROL_SET_INFO_VAL1_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FP_CONTROL_SET_CONTROL_SET_INFO_VAL1_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.val[1] = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_CONTROL_SET_CONTROL_SET_INFO_VAL2_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FP_CONTROL_SET_CONTROL_SET_INFO_VAL2_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.val[2] = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_CONTROL_SET_CONTROL_SET_INFO_VAL3_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FP_CONTROL_SET_CONTROL_SET_INFO_VAL3_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.val[3] = (uint32)param_val->value.ulong_value;
  } 

  /* Call function */
  ret = soc_ppd_fp_control_set(
          unit,
          SOC_CORE_INVALID,
          &prm_control_ndx,
          &prm_info,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_fp_control_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_fp_control_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: control_get (section fp)
 */
int 
  ui_ppd_api_fp_control_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_FP_CONTROL_INDEX   
    prm_control_ndx;
  SOC_PPC_FP_CONTROL_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_fp"); 
  soc_sand_proc_name = "soc_ppd_fp_control_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FP_CONTROL_INDEX_clear(&prm_control_ndx);
  SOC_PPC_FP_CONTROL_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_CONTROL_GET_CONTROL_GET_CONTROL_NDX_VAL_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FP_CONTROL_GET_CONTROL_GET_CONTROL_NDX_VAL_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_control_ndx.val_ndx = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_CONTROL_GET_CONTROL_GET_CONTROL_NDX_TYPE_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FP_CONTROL_GET_CONTROL_GET_CONTROL_NDX_TYPE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_control_ndx.type = param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_CONTROL_GET_CONTROL_GET_CONTROL_NDX_DB_ID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FP_CONTROL_GET_CONTROL_GET_CONTROL_NDX_DB_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_control_ndx.db_id = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter control_ndx after control_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_fp_control_get(
          unit,
          SOC_CORE_INVALID,
          &prm_control_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_fp_control_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_fp_control_get");   
    goto exit; 
  } 

  SOC_PPC_FP_CONTROL_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: egr_db_map_set (section fp)
 */
int 
  ui_ppd_api_fp_egr_db_map_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_FP_FWD_TYPE   
    prm_fwd_type_ndx;
  uint32   
    prm_port_profile_ndx;
  uint32   
    prm_db_id;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_fp"); 
  soc_sand_proc_name = "soc_ppd_fp_egr_db_map_set"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_EGR_DB_MAP_SET_EGR_DB_MAP_SET_FWD_TYPE_NDX_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FP_EGR_DB_MAP_SET_EGR_DB_MAP_SET_FWD_TYPE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fwd_type_ndx = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter fwd_type_ndx after egr_db_map_set***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_EGR_DB_MAP_SET_EGR_DB_MAP_SET_PORT_PROFILE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FP_EGR_DB_MAP_SET_EGR_DB_MAP_SET_PORT_PROFILE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_port_profile_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter port_profile_ndx after egr_db_map_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_fp_egr_db_map_get(
          unit,
          prm_fwd_type_ndx,
          prm_port_profile_ndx,
          &prm_db_id
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_fp_egr_db_map_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_fp_egr_db_map_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_EGR_DB_MAP_SET_EGR_DB_MAP_SET_DB_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FP_EGR_DB_MAP_SET_EGR_DB_MAP_SET_DB_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_db_id = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_fp_egr_db_map_set(
          unit,
          prm_fwd_type_ndx,
          prm_port_profile_ndx,
          prm_db_id
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_fp_egr_db_map_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_fp_egr_db_map_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: egr_db_map_get (section fp)
 */
int 
  ui_ppd_api_fp_egr_db_map_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_FP_FWD_TYPE   
    prm_fwd_type_ndx;
  uint32   
    prm_port_profile_ndx;
  uint32   
    prm_db_id;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_fp"); 
  soc_sand_proc_name = "soc_ppd_fp_egr_db_map_get"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_EGR_DB_MAP_GET_EGR_DB_MAP_GET_FWD_TYPE_NDX_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FP_EGR_DB_MAP_GET_EGR_DB_MAP_GET_FWD_TYPE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fwd_type_ndx = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter fwd_type_ndx after egr_db_map_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_EGR_DB_MAP_GET_EGR_DB_MAP_GET_PORT_PROFILE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FP_EGR_DB_MAP_GET_EGR_DB_MAP_GET_PORT_PROFILE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_port_profile_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter port_profile_ndx after egr_db_map_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_fp_egr_db_map_get(
          unit,
          prm_fwd_type_ndx,
          prm_port_profile_ndx,
          &prm_db_id
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_fp_egr_db_map_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_fp_egr_db_map_get");   
    goto exit; 
  } 

  cli_out("db_id: %u\n\r",prm_db_id);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
/******************************************************************** 
 *  Function handler: packet_diag_get (section fp)
 */
int 
  ui_ppd_api_fp_packet_diag_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_FP_PACKET_DIAG_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_fp"); 
  soc_sand_proc_name = "soc_ppd_fp_packet_diag_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FP_PACKET_DIAG_INFO_clear(&prm_info);
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_ppd_fp_packet_diag_get(
          unit,
          SOC_CORE_INVALID,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_fp_packet_diag_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_fp_packet_diag_get");   
    goto exit; 
  } 

  SOC_PPC_FP_PACKET_DIAG_INFO_print(unit, &prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
#endif
#ifdef UI_FP/* { fp*/
/******************************************************************** 
 *  Section handler: fp
 */ 
int 
  ui_ppd_api_fp( 
    CURRENT_LINE *current_line 
  ) 
{   
  int
    ret = FALSE;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_fp"); 
 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_PACKET_FORMAT_GROUP_SET_PACKET_FORMAT_GROUP_SET_ID,1)) 
  { 
    ret = ui_ppd_api_fp_packet_format_group_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_PACKET_FORMAT_GROUP_GET_PACKET_FORMAT_GROUP_GET_ID,1)) 
  { 
    ret = ui_ppd_api_fp_packet_format_group_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_DATABASE_CREATE_DATABASE_CREATE_ID,1)) 
  { 
    ret = ui_ppd_api_fp_database_create(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_DATABASE_GET_DATABASE_GET_ID,1)) 
  { 
    ret = ui_ppd_api_fp_database_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_DATABASE_DESTROY_DATABASE_DESTROY_ID,1)) 
  { 
    ret = ui_ppd_api_fp_database_destroy(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_ENTRY_ADD_ENTRY_ADD_ID,1)) 
  { 
    ret = ui_ppd_api_fp_entry_add(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_ENTRY_GET_ENTRY_GET_ID,1)) 
  { 
    ret = ui_ppd_api_fp_entry_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_ENTRY_REMOVE_ENTRY_REMOVE_ID,1)) 
  { 
    ret = ui_ppd_api_fp_entry_remove(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_DATABASE_ENTRIES_GET_BLOCK_DATABASE_ENTRIES_GET_BLOCK_ID,1)) 
  { 
    ret = ui_ppd_api_fp_database_entries_get_block(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_DIRECT_EXTRACTION_ENTRY_ADD_DIRECT_EXTRACTION_ENTRY_ADD_ID,1)) 
  { 
    ret = ui_ppd_api_fp_direct_extraction_entry_add(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_DIRECT_EXTRACTION_ENTRY_GET_DIRECT_EXTRACTION_ENTRY_GET_ID,1)) 
  { 
    ret = ui_ppd_api_fp_direct_extraction_entry_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_DIRECT_EXTRACTION_ENTRY_REMOVE_DIRECT_EXTRACTION_ENTRY_REMOVE_ID,1)) 
  { 
    ret = ui_ppd_api_fp_direct_extraction_entry_remove(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_DIRECT_EXTRACTION_DB_ENTRIES_GET_DIRECT_EXTRACTION_DB_ENTRIES_GET_ID,1)) 
  { 
    ret = ui_ppd_api_fp_direct_extraction_db_entries_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_CONTROL_SET_CONTROL_SET_ID,1)) 
  { 
    ret = ui_ppd_api_fp_control_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_CONTROL_GET_CONTROL_GET_ID,1)) 
  { 
    ret = ui_ppd_api_fp_control_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_EGR_DB_MAP_SET_EGR_DB_MAP_SET_ID,1)) 
  { 
    ret = ui_ppd_api_fp_egr_db_map_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_EGR_DB_MAP_GET_EGR_DB_MAP_GET_ID,1)) 
  { 
    ret = ui_ppd_api_fp_egr_db_map_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FP_PACKET_DIAG_GET_PACKET_DIAG_GET_ID,1)) 
  { 
    ret = ui_ppd_api_fp_packet_diag_get(current_line); 
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting function name after fp***", TRUE); 
  } 
  
  ui_ret = ret;
    
  goto exit; 
exit:        
  return ui_ret; 
}                
          
#endif /* fp */ 


#endif /* LINK_PPD_LIBRARIES */ 

