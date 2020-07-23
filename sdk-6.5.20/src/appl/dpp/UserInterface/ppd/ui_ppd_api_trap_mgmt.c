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

 

  
#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>    
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_trap_mgmt.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_lif.h>
#include <soc/dpp/PPD/ppd_api_frwrd_mact_mgmt.h>    

#include <appl/diag/dpp/ui_defx.h>


#if LINK_PPD_LIBRARIES

#ifdef UI_TRAP_MGMT

/******************************************************************** 
*  Function handler: trap_frwrd_profile_info_set (section trap_mgmt)
 */
int 
ui_ppd_api_trap_mgmt_trap_frwrd_profile_info_set(
  CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_TRAP_CODE   
    trap_code_ndx;
  SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO   
    prm_profile_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_trap_mgmt"); 
  soc_sand_proc_name = "soc_ppd_trap_frwrd_profile_info_set"; 

  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO_clear(&prm_profile_info);
  prm_profile_info.processing_info.enable_learning = TRUE;
  prm_profile_info.strength = 5;

  /* Get parameters */   
   /*trap-code
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_TRAP_FRWRD_PROFILE_INFO_SET_TRAP_FRWRD_PROFILE_INFO_SET_PROFILE_NDX_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_TRAP_FRWRD_PROFILE_INFO_SET_TRAP_FRWRD_PROFILE_INFO_SET_PROFILE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    trap_code_ndx = param_val->numeric_equivalent;
  } 
  */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_TRAP_FRWRD_PROFILE_INFO_SET_TRAP_FRWRD_PROFILE_INFO_SET_PROFILE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_TRAP_FRWRD_PROFILE_INFO_SET_TRAP_FRWRD_PROFILE_INFO_SET_PROFILE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    trap_code_ndx = (SOC_PPC_TRAP_CODE)param_val->value.ulong_value;
  } 

  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter profile_ndx after trap_frwrd_profile_info_set***", TRUE); 
    goto exit; 
  } 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_trap_frwrd_profile_info_get(
    unit,
    trap_code_ndx,
    &prm_profile_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_trap_frwrd_profile_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_trap_frwrd_profile_info_get");   
    goto exit; 
  } 

  prm_profile_info.bitmap_mask = 0;

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_TRAP_FRWRD_PROFILE_INFO_SET_TRAP_FRWRD_PROFILE_INFO_SET_PROFILE_INFO_PROCESSING_INFO_FRWRD_OFFSET_INDEX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_TRAP_FRWRD_PROFILE_INFO_SET_TRAP_FRWRD_PROFILE_INFO_SET_PROFILE_INFO_PROCESSING_INFO_FRWRD_OFFSET_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_profile_info.processing_info.frwrd_offset_index = (uint32)param_val->value.ulong_value;
    prm_profile_info.bitmap_mask |= SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_FRWRD_OFFSET;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_TRAP_FRWRD_PROFILE_INFO_SET_TRAP_FRWRD_PROFILE_INFO_SET_PROFILE_INFO_PROCESSING_INFO_IS_CONTROL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_TRAP_FRWRD_PROFILE_INFO_SET_TRAP_FRWRD_PROFILE_INFO_SET_PROFILE_INFO_PROCESSING_INFO_IS_CONTROL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_profile_info.processing_info.is_control = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_TRAP_FRWRD_PROFILE_INFO_SET_TRAP_FRWRD_PROFILE_INFO_SET_PROFILE_INFO_PROCESSING_INFO_IS_TRAP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_TRAP_FRWRD_PROFILE_INFO_SET_TRAP_FRWRD_PROFILE_INFO_SET_PROFILE_INFO_PROCESSING_INFO_IS_TRAP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_profile_info.processing_info.is_trap = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_TRAP_FRWRD_PROFILE_INFO_SET_TRAP_FRWRD_PROFILE_INFO_SET_PROFILE_INFO_PROCESSING_INFO_ENABLE_LEARNING_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_TRAP_FRWRD_PROFILE_INFO_SET_TRAP_FRWRD_PROFILE_INFO_SET_PROFILE_INFO_PROCESSING_INFO_ENABLE_LEARNING_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_profile_info.processing_info.enable_learning = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_TRAP_FRWRD_PROFILE_INFO_SET_TRAP_FRWRD_PROFILE_INFO_SET_PROFILE_INFO_POLICING_INFO_ETHERNET_POLICE_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_TRAP_FRWRD_PROFILE_INFO_SET_TRAP_FRWRD_PROFILE_INFO_SET_PROFILE_INFO_POLICING_INFO_ETHERNET_POLICE_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_profile_info.policing_info.ethernet_police_id = (uint32)param_val->value.ulong_value;
    prm_profile_info.bitmap_mask |= SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_POLICER;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_TRAP_FRWRD_PROFILE_INFO_SET_TRAP_FRWRD_PROFILE_INFO_SET_PROFILE_INFO_METER_INFO_METER_COMMAND_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_TRAP_FRWRD_PROFILE_INFO_SET_TRAP_FRWRD_PROFILE_INFO_SET_PROFILE_INFO_METER_INFO_METER_COMMAND_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_profile_info.meter_info.meter_command = (uint32)param_val->value.ulong_value;
    prm_profile_info.bitmap_mask |= SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DP_METER_CMD;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_TRAP_FRWRD_PROFILE_INFO_SET_TRAP_FRWRD_PROFILE_INFO_SET_PROFILE_INFO_METER_INFO_METER_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_TRAP_FRWRD_PROFILE_INFO_SET_TRAP_FRWRD_PROFILE_INFO_SET_PROFILE_INFO_METER_INFO_METER_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_profile_info.meter_info.meter_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_TRAP_FRWRD_PROFILE_INFO_SET_TRAP_FRWRD_PROFILE_INFO_SET_PROFILE_INFO_METER_INFO_METER_SELECT_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_TRAP_FRWRD_PROFILE_INFO_SET_TRAP_FRWRD_PROFILE_INFO_SET_PROFILE_INFO_METER_INFO_METER_SELECT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_profile_info.meter_info.meter_select = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_TRAP_FRWRD_PROFILE_INFO_SET_TRAP_FRWRD_PROFILE_INFO_SET_PROFILE_INFO_COUNT_INFO_COUNTER_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_TRAP_FRWRD_PROFILE_INFO_SET_TRAP_FRWRD_PROFILE_INFO_SET_PROFILE_INFO_COUNT_INFO_COUNTER_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_profile_info.count_info.counter_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_TRAP_FRWRD_PROFILE_INFO_SET_TRAP_FRWRD_PROFILE_INFO_SET_PROFILE_INFO_COUNT_INFO_POINTER_SELECT_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_TRAP_FRWRD_PROFILE_INFO_SET_TRAP_FRWRD_PROFILE_INFO_SET_PROFILE_INFO_COUNT_INFO_POINTER_SELECT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_profile_info.count_info.counter_select = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_TRAP_FRWRD_PROFILE_INFO_SET_TRAP_FRWRD_PROFILE_INFO_SET_PROFILE_INFO_COS_INFO_DP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_TRAP_FRWRD_PROFILE_INFO_SET_TRAP_FRWRD_PROFILE_INFO_SET_PROFILE_INFO_COS_INFO_DP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_profile_info.cos_info.dp = (uint8)param_val->value.ulong_value;
    prm_profile_info.bitmap_mask |= SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DP;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_TRAP_FRWRD_PROFILE_INFO_SET_TRAP_FRWRD_PROFILE_INFO_SET_PROFILE_INFO_COS_INFO_TC_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_TRAP_FRWRD_PROFILE_INFO_SET_TRAP_FRWRD_PROFILE_INFO_SET_PROFILE_INFO_COS_INFO_TC_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_profile_info.cos_info.tc = (uint8)param_val->value.ulong_value;
    prm_profile_info.bitmap_mask |= SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_TC;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_TRAP_FRWRD_PROFILE_INFO_SET_TRAP_FRWRD_PROFILE_INFO_SET_PROFILE_INFO_DEST_INFO_VSI_SHIFT_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_TRAP_FRWRD_PROFILE_INFO_SET_TRAP_FRWRD_PROFILE_INFO_SET_PROFILE_INFO_DEST_INFO_VSI_SHIFT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_profile_info.dest_info.vsi_shift = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_TRAP_FRWRD_PROFILE_INFO_SET_TRAP_FRWRD_PROFILE_INFO_SET_PROFILE_INFO_DEST_INFO_ADD_VSI_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_TRAP_FRWRD_PROFILE_INFO_SET_TRAP_FRWRD_PROFILE_INFO_SET_PROFILE_INFO_DEST_INFO_ADD_VSI_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_profile_info.dest_info.add_vsi = (uint8)param_val->value.ulong_value;
  } 

   ret = ui_ppd_frwrd_decision_set(
            current_line,
            1,
            &(prm_profile_info.dest_info.frwrd_dest)
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** ui_ppd_frwrd_decision_set - FAIL", TRUE);
      soc_sand_disp_result_proc(ret, "ui_ppd_frwrd_decision_set");
      goto exit;
    }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_DECISION_FD_TYPE_ID,1))
  {
    prm_profile_info.bitmap_mask |= SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DEST;
  }



  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_TRAP_FRWRD_PROFILE_INFO_SET_TRAP_FRWRD_PROFILE_INFO_SET_PROFILE_INFO_DEST_INFO_IS_OAM_DEST_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_TRAP_FRWRD_PROFILE_INFO_SET_TRAP_FRWRD_PROFILE_INFO_SET_PROFILE_INFO_DEST_INFO_IS_OAM_DEST_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_profile_info.dest_info.is_oam_dest = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_TRAP_FRWRD_PROFILE_INFO_SET_TRAP_FRWRD_PROFILE_INFO_SET_PROFILE_INFO_BITMAP_MASK_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_TRAP_FRWRD_PROFILE_INFO_SET_TRAP_FRWRD_PROFILE_INFO_SET_PROFILE_INFO_BITMAP_MASK_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_profile_info.bitmap_mask = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_TRAP_FRWRD_PROFILE_INFO_SET_TRAP_FRWRD_PROFILE_INFO_SET_PROFILE_INFO_STRENGTH_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_TRAP_FRWRD_PROFILE_INFO_SET_TRAP_FRWRD_PROFILE_INFO_SET_PROFILE_INFO_STRENGTH_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_profile_info.strength = (uint32)param_val->value.ulong_value;
  } 

  /* Call function */  
  ret = soc_ppd_trap_frwrd_profile_info_set(
    unit,
    trap_code_ndx,
    &prm_profile_info,
    SOC_CORE_ALL
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_trap_frwrd_profile_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_trap_frwrd_profile_info_set");   
    goto exit; 
  } 


  goto exit; 
exit: 
  return ui_ret; 
} 

/******************************************************************** 
*  Function handler: trap_frwrd_profile_info_get (section trap_mgmt)
 */
int 
ui_ppd_api_trap_mgmt_trap_frwrd_profile_info_get(
  CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_TRAP_CODE   
    trap_code_ndx;
  SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO   
    prm_profile_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_trap_mgmt"); 
  soc_sand_proc_name = "soc_ppd_trap_frwrd_profile_info_get"; 

  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO_clear(&prm_profile_info);

  /* Get parameters */ 
    /*trap-code
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_TRAP_FRWRD_PROFILE_INFO_GET_TRAP_FRWRD_PROFILE_INFO_GET_PROFILE_NDX_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_TRAP_FRWRD_PROFILE_INFO_GET_TRAP_FRWRD_PROFILE_INFO_GET_PROFILE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    trap_code_ndx = param_val->numeric_equivalent;
  } 
  */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_TRAP_FRWRD_PROFILE_INFO_GET_TRAP_FRWRD_PROFILE_INFO_GET_PROFILE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_TRAP_FRWRD_PROFILE_INFO_GET_TRAP_FRWRD_PROFILE_INFO_GET_PROFILE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    trap_code_ndx = (SOC_PPC_TRAP_CODE)param_val->value.ulong_value;
  } 

  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter profile_ndx after trap_frwrd_profile_info_get***", TRUE); 
    goto exit; 
  } 

  /* Call function */  
  ret = soc_ppd_trap_frwrd_profile_info_get(
    unit,
    trap_code_ndx,
    &prm_profile_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_trap_frwrd_profile_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_trap_frwrd_profile_info_get");   
    goto exit; 
  } 

  SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO_print(&prm_profile_info);


  goto exit; 
exit: 
  return ui_ret; 
} 

/******************************************************************** 
*  Function handler: trap_snoop_profile_info_set (section trap_mgmt)
 */
int 
ui_ppd_api_trap_mgmt_trap_snoop_profile_info_set(
  CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_TRAP_CODE   
    trap_code_ndx;
  SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO   
    prm_profile_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_trap_mgmt"); 
  soc_sand_proc_name = "soc_ppd_trap_snoop_profile_info_set"; 

  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO_clear(&prm_profile_info);

  /* Get parameters */ 
    /*trap-code
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_TRAP_SNOOP_PROFILE_INFO_SET_TRAP_SNOOP_PROFILE_INFO_SET_PROFILE_NDX_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_TRAP_SNOOP_PROFILE_INFO_SET_TRAP_SNOOP_PROFILE_INFO_SET_PROFILE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    trap_code_ndx = param_val->numeric_equivalent;
  } 
  */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_TRAP_SNOOP_PROFILE_INFO_SET_TRAP_SNOOP_PROFILE_INFO_SET_PROFILE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_TRAP_SNOOP_PROFILE_INFO_SET_TRAP_SNOOP_PROFILE_INFO_SET_PROFILE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    trap_code_ndx = (SOC_PPC_TRAP_CODE)param_val->value.ulong_value;
  } 

  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter profile_ndx after trap_snoop_profile_info_set***", TRUE); 
    goto exit; 
  } 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_trap_snoop_profile_info_get(
    unit,
    trap_code_ndx,
    &prm_profile_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_trap_snoop_profile_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_trap_snoop_profile_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_TRAP_SNOOP_PROFILE_INFO_SET_TRAP_SNOOP_PROFILE_INFO_SET_PROFILE_INFO_SNOOP_CMND_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_TRAP_SNOOP_PROFILE_INFO_SET_TRAP_SNOOP_PROFILE_INFO_SET_PROFILE_INFO_SNOOP_CMND_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_profile_info.snoop_cmnd = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_TRAP_SNOOP_PROFILE_INFO_SET_TRAP_SNOOP_PROFILE_INFO_SET_PROFILE_INFO_STRENGTH_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_TRAP_SNOOP_PROFILE_INFO_SET_TRAP_SNOOP_PROFILE_INFO_SET_PROFILE_INFO_STRENGTH_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_profile_info.strength = (uint32)param_val->value.ulong_value;
  } 

  /* Call function */  
  ret = soc_ppd_trap_snoop_profile_info_set(
    unit,
    trap_code_ndx,
    &prm_profile_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_trap_snoop_profile_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_trap_snoop_profile_info_set");   
    goto exit; 
  } 


  goto exit; 
exit: 
  return ui_ret; 
} 

/******************************************************************** 
*  Function handler: trap_snoop_profile_info_get (section trap_mgmt)
 */
int 
ui_ppd_api_trap_mgmt_trap_snoop_profile_info_get(
  CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_TRAP_CODE   
    trap_code_ndx;
  SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO   
    prm_profile_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_trap_mgmt"); 
  soc_sand_proc_name = "soc_ppd_trap_snoop_profile_info_get"; 

  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO_clear(&prm_profile_info);

  /* Get parameters */ 
    /*trap-code
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_TRAP_SNOOP_PROFILE_INFO_GET_TRAP_SNOOP_PROFILE_INFO_GET_PROFILE_NDX_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_TRAP_SNOOP_PROFILE_INFO_GET_TRAP_SNOOP_PROFILE_INFO_GET_PROFILE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    trap_code_ndx = param_val->numeric_equivalent;
  } 
  */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_TRAP_SNOOP_PROFILE_INFO_GET_TRAP_SNOOP_PROFILE_INFO_GET_PROFILE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_TRAP_SNOOP_PROFILE_INFO_GET_TRAP_SNOOP_PROFILE_INFO_GET_PROFILE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    trap_code_ndx = (SOC_PPC_TRAP_CODE)param_val->value.ulong_value;
  } 

  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter profile_ndx after trap_snoop_profile_info_get***", TRUE); 
    goto exit; 
  } 

  /* Call function */  
  ret = soc_ppd_trap_snoop_profile_info_get(
    unit,
    trap_code_ndx,
    &prm_profile_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_trap_snoop_profile_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_trap_snoop_profile_info_get");   
    goto exit; 
  } 

  SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO_print(&prm_profile_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: trap_to_eg_action_map_set (section trap_mgmt)
 */
int 
  ui_ppd_api_trap_mgmt_trap_to_eg_action_map_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_TRAP_EG_TYPE   
    prm_trap_type_bitmap_ndx;
  uint32   
    prm_eg_action_profile=0;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_trap_mgmt"); 
  soc_sand_proc_name = "soc_ppd_trap_to_eg_action_map_set"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_TRAP_TO_EG_ACTION_MAP_SET_TRAP_TO_EG_ACTION_MAP_SET_TRAP_TYPE_BITMAP_NDX_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_TRAP_TO_EG_ACTION_MAP_SET_TRAP_TO_EG_ACTION_MAP_SET_TRAP_TYPE_BITMAP_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_trap_type_bitmap_ndx = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter trap_type_bitmap_ndx after trap_to_eg_action_map_set***", TRUE); 
    goto exit; 
  } 


  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_TRAP_TO_EG_ACTION_MAP_SET_TRAP_TO_EG_ACTION_MAP_SET_EG_ACTION_PROFILE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_TRAP_TO_EG_ACTION_MAP_SET_TRAP_TO_EG_ACTION_MAP_SET_EG_ACTION_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_eg_action_profile = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_trap_to_eg_action_map_set(
          unit,
          prm_trap_type_bitmap_ndx,
          prm_eg_action_profile
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_trap_to_eg_action_map_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_trap_to_eg_action_map_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: trap_to_eg_action_map_get (section trap_mgmt)
 */
int 
  ui_ppd_api_trap_mgmt_trap_to_eg_action_map_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_TRAP_EG_TYPE   
    prm_trap_type_bitmap_ndx;
  uint32   
    prm_eg_action_profile;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_trap_mgmt"); 
  soc_sand_proc_name = "soc_ppd_trap_to_eg_action_map_get"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_TRAP_TO_EG_ACTION_MAP_GET_TRAP_TO_EG_ACTION_MAP_GET_TRAP_TYPE_BITMAP_NDX_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_TRAP_TO_EG_ACTION_MAP_GET_TRAP_TO_EG_ACTION_MAP_GET_TRAP_TYPE_BITMAP_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_trap_type_bitmap_ndx = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter trap_type_bitmap_ndx after trap_to_eg_action_map_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_trap_to_eg_action_map_get(
          unit,
          prm_trap_type_bitmap_ndx,
          &prm_eg_action_profile
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_trap_to_eg_action_map_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_trap_to_eg_action_map_get");   
    goto exit; 
  } 

  cli_out("eg_action_profile: %u\n\r",prm_eg_action_profile);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: trap_eg_profile_info_set (section trap_mgmt)
 */
int 
  ui_ppd_api_trap_mgmt_trap_eg_profile_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_profile_ndx;
  SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO   
    prm_profile_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_trap_mgmt"); 
  soc_sand_proc_name = "soc_ppd_trap_eg_profile_info_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO_clear(&prm_profile_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_TRAP_EG_PROFILE_INFO_SET_TRAP_EG_PROFILE_INFO_SET_PROFILE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_TRAP_EG_PROFILE_INFO_SET_TRAP_EG_PROFILE_INFO_SET_PROFILE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_profile_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter profile_ndx after trap_eg_profile_info_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_trap_eg_profile_info_get(
          unit,
          prm_profile_ndx,
          &prm_profile_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_trap_eg_profile_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_trap_eg_profile_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_TRAP_EG_PROFILE_INFO_SET_TRAP_EG_PROFILE_INFO_SET_PROFILE_INFO_CUD_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_TRAP_EG_PROFILE_INFO_SET_TRAP_EG_PROFILE_INFO_SET_PROFILE_INFO_CUD_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_profile_info.cud = (uint32)param_val->value.ulong_value;
    prm_profile_info.bitmap_mask = SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_CUD;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_TRAP_EG_PROFILE_INFO_SET_TRAP_EG_PROFILE_INFO_SET_PROFILE_INFO_COS_INFO_DP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_TRAP_EG_PROFILE_INFO_SET_TRAP_EG_PROFILE_INFO_SET_PROFILE_INFO_COS_INFO_DP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_profile_info.cos_info.dp = (uint8)param_val->value.ulong_value;
    prm_profile_info.bitmap_mask = SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DP;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_TRAP_EG_PROFILE_INFO_SET_TRAP_EG_PROFILE_INFO_SET_PROFILE_INFO_COS_INFO_TC_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_TRAP_EG_PROFILE_INFO_SET_TRAP_EG_PROFILE_INFO_SET_PROFILE_INFO_COS_INFO_TC_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_profile_info.cos_info.tc = (uint8)param_val->value.ulong_value;
    prm_profile_info.bitmap_mask = SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_TC;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_TRAP_EG_PROFILE_INFO_SET_TRAP_EG_PROFILE_INFO_SET_PROFILE_INFO_OUT_TM_PORT_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_TRAP_EG_PROFILE_INFO_SET_TRAP_EG_PROFILE_INFO_SET_PROFILE_INFO_OUT_TM_PORT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_profile_info.out_tm_port = (uint32)param_val->value.ulong_value;
    prm_profile_info.bitmap_mask = SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DEST;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_TRAP_EG_PROFILE_INFO_SET_TRAP_EG_PROFILE_INFO_SET_PROFILE_INFO_BITMAP_MASK_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_TRAP_EG_PROFILE_INFO_SET_TRAP_EG_PROFILE_INFO_SET_PROFILE_INFO_BITMAP_MASK_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_profile_info.bitmap_mask = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_trap_eg_profile_info_set(
          unit,
          prm_profile_ndx,
          &prm_profile_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_trap_eg_profile_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_trap_eg_profile_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: trap_eg_profile_info_get (section trap_mgmt)
 */
int 
  ui_ppd_api_trap_mgmt_trap_eg_profile_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_profile_ndx;
  SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO   
    prm_profile_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_trap_mgmt"); 
  soc_sand_proc_name = "soc_ppd_trap_eg_profile_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO_clear(&prm_profile_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_TRAP_EG_PROFILE_INFO_GET_TRAP_EG_PROFILE_INFO_GET_PROFILE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_TRAP_EG_PROFILE_INFO_GET_TRAP_EG_PROFILE_INFO_GET_PROFILE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_profile_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter profile_ndx after trap_eg_profile_info_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_trap_eg_profile_info_get(
          unit,
          prm_profile_ndx,
          &prm_profile_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_trap_eg_profile_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_trap_eg_profile_info_get");   
    goto exit; 
  } 

  SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO_print(&prm_profile_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
*  Function handler: trap_mact_event_get (section trap_mgmt)
 */
int 
ui_ppd_api_trap_mgmt_trap_mact_event_get(
  CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_buff[SOC_PPC_TRAP_EVENT_BUFF_MAX_SIZE];
  uint32   
    prm_buff_len;
  SOC_PPC_TRAP_MACT_EVENT_INFO   
    prm_mact_event;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_trap_mgmt"); 
  soc_sand_proc_name = "soc_ppd_trap_mact_event_get"; 

  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_TRAP_MACT_EVENT_INFO_clear(&prm_mact_event);

  /* Get parameters */ 

  /* Call function */  
  ret = soc_ppd_trap_mact_event_get(
    unit,
    prm_buff,
    &prm_buff_len
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_trap_mact_event_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_trap_mact_event_get");   
    goto exit; 
  } 
  /*
  cli_out("buff: \n");

  for (indx = SOC_PPC_TRAP_EVENT_BUFF_MAX_SIZE; indx > 0 ; --indx)
  {
  cli_out("%02x ",prm_buff[indx-1]);
  }

  cli_out("buff_len: %u\n",prm_buff_len);
  */
  ret = soc_ppd_trap_mact_event_parse(
    unit,
    prm_buff,
    prm_buff_len,
    &prm_mact_event
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_trap_mgmt_mact_event_parse - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_trap_mgmt_mact_event_parse");   
    goto exit; 
  } 

  SOC_PPC_TRAP_MACT_EVENT_INFO_print(&prm_mact_event);

  goto exit; 
exit: 
  return ui_ret; 
} 

/******************************************************************** 
*  Function handler: trap_mact_event_parse (section trap_mgmt)
 */
int 
ui_ppd_api_trap_mgmt_trap_mact_event_parse(
  CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32 
    prm_buff_index = 0xFFFFFFFF;  
  uint32   
    prm_buff[SOC_PPC_TRAP_EVENT_BUFF_MAX_SIZE];
  uint32   
    prm_buff_len=0;
  SOC_PPC_TRAP_MACT_EVENT_INFO   
    prm_mact_event;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_trap_mgmt"); 
  soc_sand_proc_name = "soc_ppd_trap_mact_event_parse"; 

  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_TRAP_MACT_EVENT_INFO_clear(&prm_mact_event);

  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_TRAP_MACT_EVENT_PARSE_TRAP_MACT_EVENT_PARSE_BUFF_INDEX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_TRAP_MACT_EVENT_PARSE_TRAP_MACT_EVENT_PARSE_BUFF_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_buff_index = (uint32)param_val->value.ulong_value;
  } 

  if(prm_buff_index != 0xFFFFFFFF)          
  { 

    if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_TRAP_MACT_EVENT_PARSE_TRAP_MACT_EVENT_PARSE_BUFF_ID,1)) 
    { 
      UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_TRAP_MACT_EVENT_PARSE_TRAP_MACT_EVENT_PARSE_BUFF_ID);  
      UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
      prm_buff[ prm_buff_index] = (uint32)param_val->value.ulong_value;
    } 

  }   

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_TRAP_MACT_EVENT_PARSE_TRAP_MACT_EVENT_PARSE_BUFF_LEN_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_TRAP_MACT_EVENT_PARSE_TRAP_MACT_EVENT_PARSE_BUFF_LEN_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_buff_len = (uint32)param_val->value.ulong_value;
  } 

  /* Call function */  
  ret = soc_ppd_trap_mact_event_parse(
    unit,
    prm_buff,
    prm_buff_len,
    &prm_mact_event
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_trap_mact_event_parse - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_trap_mact_event_parse");   
    goto exit; 
  } 

  SOC_PPC_TRAP_MACT_EVENT_INFO_print(&prm_mact_event);


  goto exit; 
exit: 
  return ui_ret; 
} 

/******************************************************************** 
*  Section handler: trap_mgmt
 */ 
int 
ui_ppd_api_trap_mgmt( 
                     CURRENT_LINE *current_line 
                     ) 
{   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_trap_mgmt"); 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_TRAP_FRWRD_PROFILE_INFO_SET_TRAP_FRWRD_PROFILE_INFO_SET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_trap_mgmt_trap_frwrd_profile_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_TRAP_FRWRD_PROFILE_INFO_GET_TRAP_FRWRD_PROFILE_INFO_GET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_trap_mgmt_trap_frwrd_profile_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_TRAP_SNOOP_PROFILE_INFO_SET_TRAP_SNOOP_PROFILE_INFO_SET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_trap_mgmt_trap_snoop_profile_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_TRAP_SNOOP_PROFILE_INFO_GET_TRAP_SNOOP_PROFILE_INFO_GET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_trap_mgmt_trap_snoop_profile_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_TRAP_TO_EG_ACTION_MAP_SET_TRAP_TO_EG_ACTION_MAP_SET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_trap_mgmt_trap_to_eg_action_map_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_TRAP_TO_EG_ACTION_MAP_GET_TRAP_TO_EG_ACTION_MAP_GET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_trap_mgmt_trap_to_eg_action_map_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_TRAP_EG_PROFILE_INFO_SET_TRAP_EG_PROFILE_INFO_SET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_trap_mgmt_trap_eg_profile_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_TRAP_EG_PROFILE_INFO_GET_TRAP_EG_PROFILE_INFO_GET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_trap_mgmt_trap_eg_profile_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_TRAP_MACT_EVENT_GET_TRAP_MACT_EVENT_GET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_trap_mgmt_trap_mact_event_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_TRAP_MACT_EVENT_PARSE_TRAP_MACT_EVENT_PARSE_ID,1)) 
  { 
    ui_ret = ui_ppd_api_trap_mgmt_trap_mact_event_parse(current_line); 
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting function name after trap_mgmt***", TRUE); 
  } 

  goto exit; 
exit:        
  return ui_ret; 
}                

#endif
#endif /* LINK_PPD_LIBRARIES */ 

