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
 

 
#include <soc/dpp/PPD/ppd_api_metering.h>                                                               
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_metering.h>

#if LINK_PPD_LIBRARIES

#ifdef UI_METERING
  
/******************************************************************** 
 *  Function handler: mtr_meters_group_info_set (section metering)
 */
int 
  ui_ppd_api_metering_mtr_meters_group_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_mtr_group_ndx;
  SOC_PPC_MTR_GROUP_INFO   
    prm_mtr_group_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_metering"); 
  soc_sand_proc_name = "soc_ppd_mtr_meters_group_info_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_MTR_GROUP_INFO_clear(&prm_mtr_group_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_METERS_GROUP_INFO_SET_MTR_METERS_GROUP_INFO_SET_MTR_GROUP_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MTR_METERS_GROUP_INFO_SET_MTR_METERS_GROUP_INFO_SET_MTR_GROUP_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_mtr_group_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter mtr_group_ndx after mtr_meters_group_info_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_mtr_meters_group_info_get(
          unit,
		  0/*core*/,
          prm_mtr_group_ndx,
          &prm_mtr_group_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_mtr_meters_group_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_mtr_meters_group_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_METERS_GROUP_INFO_SET_MTR_METERS_GROUP_INFO_SET_MTR_GROUP_INFO_MAX_PACKET_SIZE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MTR_METERS_GROUP_INFO_SET_MTR_METERS_GROUP_INFO_SET_MTR_GROUP_INFO_MAX_PACKET_SIZE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_mtr_group_info.max_packet_size = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_METERS_GROUP_INFO_SET_MTR_METERS_GROUP_INFO_SET_MTR_GROUP_INFO_IS_HR_ENABLED_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MTR_METERS_GROUP_INFO_SET_MTR_METERS_GROUP_INFO_SET_MTR_GROUP_INFO_IS_HR_ENABLED_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_mtr_group_info.is_hr_enabled = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_mtr_meters_group_info_set(
          unit,
		  0, /*use default core*/
          prm_mtr_group_ndx,
          &prm_mtr_group_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_mtr_meters_group_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_mtr_meters_group_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: mtr_meters_group_info_get (section metering)
 */
int 
  ui_ppd_api_metering_mtr_meters_group_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_mtr_group_ndx;
  SOC_PPC_MTR_GROUP_INFO   
    prm_mtr_group_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_metering"); 
  soc_sand_proc_name = "soc_ppd_mtr_meters_group_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_MTR_GROUP_INFO_clear(&prm_mtr_group_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_METERS_GROUP_INFO_GET_MTR_METERS_GROUP_INFO_GET_MTR_GROUP_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MTR_METERS_GROUP_INFO_GET_MTR_METERS_GROUP_INFO_GET_MTR_GROUP_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_mtr_group_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter mtr_group_ndx after mtr_meters_group_info_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_mtr_meters_group_info_get(
          unit,
		  0/*core*/,
          prm_mtr_group_ndx,
          &prm_mtr_group_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_mtr_meters_group_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_mtr_meters_group_info_get");   
    goto exit; 
  } 

  SOC_PPC_MTR_GROUP_INFO_print(&prm_mtr_group_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: mtr_bw_profile_add (section metering)
 */
int 
ui_ppd_api_metering_mtr_bw_profile_add(
                                       CURRENT_LINE *current_line 
                                       ) 
{   
  uint32 
    ret;   
  uint32   
    prm_mtr_group_ndx;
  uint32   
    prm_bw_profile_ndx;
  SOC_PPC_MTR_BW_PROFILE_INFO   
    prm_bw_profile_info;
  SOC_PPC_MTR_BW_PROFILE_INFO   
    prm_exact_bw_profile_info;
  SOC_SAND_SUCCESS_FAILURE   
    prm_success;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_metering"); 
  soc_sand_proc_name = "soc_ppd_mtr_bw_profile_add"; 

  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_MTR_BW_PROFILE_INFO_clear(&prm_bw_profile_info);
  SOC_PPC_MTR_BW_PROFILE_INFO_clear(&prm_exact_bw_profile_info);
  prm_bw_profile_info.cir = 1000;
  prm_bw_profile_info.eir = 1000;
  prm_mtr_group_ndx = 0;

  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_BW_PROFILE_ADD_MTR_BW_PROFILE_ADD_MTR_GROUP_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MTR_BW_PROFILE_ADD_MTR_BW_PROFILE_ADD_MTR_GROUP_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_mtr_group_ndx = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_BW_PROFILE_ADD_MTR_BW_PROFILE_ADD_BW_PROFILE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MTR_BW_PROFILE_ADD_MTR_BW_PROFILE_ADD_BW_PROFILE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_bw_profile_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter bw_profile_ndx after mtr_bw_profile_add***", TRUE); 
    goto exit; 
  } 

  /* Call Get function */
  ret = soc_ppd_mtr_bw_profile_get(
          unit,
          0, /* default core*/
          prm_mtr_group_ndx,
          prm_bw_profile_ndx,
          &prm_bw_profile_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_mtr_bw_profile_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ui_ret, "soc_ppd_mtr_bw_profile_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_BW_PROFILE_ADD_MTR_BW_PROFILE_ADD_BW_PROFILE_INFO_DISABLE_CIR_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MTR_BW_PROFILE_ADD_MTR_BW_PROFILE_ADD_BW_PROFILE_INFO_DISABLE_CIR_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_bw_profile_info.disable_cir = (uint8)param_val->value.ulong_value;
  }
  
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_BW_PROFILE_ADD_MTR_BW_PROFILE_ADD_BW_PROFILE_INFO_DISABLE_EIR_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MTR_BW_PROFILE_ADD_MTR_BW_PROFILE_ADD_BW_PROFILE_INFO_DISABLE_EIR_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_bw_profile_info.disable_eir = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_BW_PROFILE_ADD_MTR_BW_PROFILE_ADD_BW_PROFILE_INFO_IS_COUPLING_ENABLED_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MTR_BW_PROFILE_ADD_MTR_BW_PROFILE_ADD_BW_PROFILE_INFO_IS_COUPLING_ENABLED_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_bw_profile_info.is_coupling_enabled = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_BW_PROFILE_ADD_MTR_BW_PROFILE_ADD_BW_PROFILE_INFO_IS_FAIRNESS_ENABLED_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MTR_BW_PROFILE_ADD_MTR_BW_PROFILE_ADD_BW_PROFILE_INFO_IS_FAIRNESS_ENABLED_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_bw_profile_info.is_fairness_enabled = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_BW_PROFILE_ADD_MTR_BW_PROFILE_ADD_BW_PROFILE_INFO_COLOR_MODE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_MTR_BW_PROFILE_ADD_MTR_BW_PROFILE_ADD_BW_PROFILE_INFO_COLOR_MODE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_bw_profile_info.color_mode = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_BW_PROFILE_ADD_MTR_BW_PROFILE_ADD_BW_PROFILE_INFO_EBS_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MTR_BW_PROFILE_ADD_MTR_BW_PROFILE_ADD_BW_PROFILE_INFO_EBS_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_bw_profile_info.ebs = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_BW_PROFILE_ADD_MTR_BW_PROFILE_ADD_BW_PROFILE_INFO_CBS_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MTR_BW_PROFILE_ADD_MTR_BW_PROFILE_ADD_BW_PROFILE_INFO_CBS_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_bw_profile_info.cbs = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_BW_PROFILE_ADD_MTR_BW_PROFILE_ADD_BW_PROFILE_INFO_EIR_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MTR_BW_PROFILE_ADD_MTR_BW_PROFILE_ADD_BW_PROFILE_INFO_EIR_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_bw_profile_info.eir = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_BW_PROFILE_ADD_MTR_BW_PROFILE_ADD_BW_PROFILE_INFO_CIR_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MTR_BW_PROFILE_ADD_MTR_BW_PROFILE_ADD_BW_PROFILE_INFO_CIR_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_bw_profile_info.cir = (uint32)param_val->value.ulong_value;
  } 

  /* Call function */  
  ret = soc_ppd_mtr_bw_profile_add(
    unit,
    0, /* default core*/
    prm_mtr_group_ndx,
    prm_bw_profile_ndx,
    &prm_bw_profile_info,
    &prm_exact_bw_profile_info,
    &prm_success
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_mtr_bw_profile_add - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_mtr_bw_profile_add");   
    goto exit; 
  } 

  SOC_PPC_MTR_BW_PROFILE_INFO_print(&prm_exact_bw_profile_info);

  cli_out("success: %s\n",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));


  goto exit; 
exit: 
  return ui_ret; 
} 

/******************************************************************** 
*  Function handler: mtr_bw_profile_get (section metering)
 */
int 
ui_ppd_api_metering_mtr_bw_profile_get(
                                       CURRENT_LINE *current_line 
                                       ) 
{   
  uint32 
    ret;   
  uint32   
    prm_mtr_group_ndx;
  uint32   
    prm_bw_profile_ndx;
  SOC_PPC_MTR_BW_PROFILE_INFO   
    prm_bw_profile_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_metering"); 
  soc_sand_proc_name = "soc_ppd_mtr_bw_profile_get"; 

  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_MTR_BW_PROFILE_INFO_clear(&prm_bw_profile_info);
  prm_mtr_group_ndx = 0;

  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_BW_PROFILE_GET_MTR_BW_PROFILE_GET_MTR_GROUP_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MTR_BW_PROFILE_GET_MTR_BW_PROFILE_GET_MTR_GROUP_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_mtr_group_ndx = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_BW_PROFILE_GET_MTR_BW_PROFILE_GET_BW_PROFILE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MTR_BW_PROFILE_GET_MTR_BW_PROFILE_GET_BW_PROFILE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_bw_profile_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter bw_profile_ndx after mtr_bw_profile_get***", TRUE); 
    goto exit; 
  } 

  /* Call function */  
  ret = soc_ppd_mtr_bw_profile_get(
    unit,
    0, /* default core*/
    prm_mtr_group_ndx,
    prm_bw_profile_ndx,
    &prm_bw_profile_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_mtr_bw_profile_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_mtr_bw_profile_get");   
    goto exit; 
  } 

  SOC_PPC_MTR_BW_PROFILE_INFO_print(&prm_bw_profile_info);


  goto exit; 
exit: 
  return ui_ret; 
} 

/******************************************************************** 
*  Function handler: mtr_bw_profile_remove (section metering)
 */
int 
ui_ppd_api_metering_mtr_bw_profile_remove(
  CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_mtr_group_ndx;
  uint32   
    prm_bw_profile_ndx;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_metering"); 
  soc_sand_proc_name = "soc_ppd_mtr_bw_profile_remove"; 

  unit = soc_ppd_get_default_unit(); 
  prm_mtr_group_ndx = 0;

  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_BW_PROFILE_REMOVE_MTR_BW_PROFILE_REMOVE_MTR_GROUP_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MTR_BW_PROFILE_REMOVE_MTR_BW_PROFILE_REMOVE_MTR_GROUP_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_mtr_group_ndx = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_BW_PROFILE_REMOVE_MTR_BW_PROFILE_REMOVE_BW_PROFILE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MTR_BW_PROFILE_REMOVE_MTR_BW_PROFILE_REMOVE_BW_PROFILE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_bw_profile_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter bw_profile_ndx after mtr_bw_profile_remove***", TRUE); 
    goto exit; 
  } 

  /* Call function */  
  ret = soc_ppd_mtr_bw_profile_remove(
    unit,
    0, /* default core*/
    prm_mtr_group_ndx,
    prm_bw_profile_ndx
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_mtr_bw_profile_remove - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_mtr_bw_profile_remove");   
    goto exit; 
  } 


  goto exit; 
exit: 
  return ui_ret; 
} 

/******************************************************************** 
*  Function handler: mtr_meter_ins_to_bw_profile_map_set (section metering)
 */
int 
ui_ppd_api_metering_mtr_meter_ins_to_bw_profile_map_set(
  CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_MTR_METER_ID   
    prm_meter_ins_ndx;
  uint32   
    prm_bw_profile_id;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_metering"); 
  soc_sand_proc_name = "soc_ppd_mtr_meter_ins_to_bw_profile_map_set"; 

  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_MTR_METER_ID_clear(&prm_meter_ins_ndx);

  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_METER_INS_TO_BW_PROFILE_MAP_SET_MTR_METER_INS_TO_BW_PROFILE_MAP_SET_METER_INS_NDX_ID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MTR_METER_INS_TO_BW_PROFILE_MAP_SET_MTR_METER_INS_TO_BW_PROFILE_MAP_SET_METER_INS_NDX_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_meter_ins_ndx.id = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_METER_INS_TO_BW_PROFILE_MAP_SET_MTR_METER_INS_TO_BW_PROFILE_MAP_SET_METER_INS_NDX_GROUP_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MTR_METER_INS_TO_BW_PROFILE_MAP_SET_MTR_METER_INS_TO_BW_PROFILE_MAP_SET_METER_INS_NDX_GROUP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_meter_ins_ndx.group = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter meter_ins_ndx after mtr_meter_ins_to_bw_profile_map_set***", TRUE); 
    goto exit; 
  } 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_mtr_meter_ins_to_bw_profile_map_get(
    unit,
    0, /* default core*/
    &prm_meter_ins_ndx,
    &prm_bw_profile_id
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_mtr_meter_ins_to_bw_profile_map_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_mtr_meter_ins_to_bw_profile_map_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_METER_INS_TO_BW_PROFILE_MAP_SET_MTR_METER_INS_TO_BW_PROFILE_MAP_SET_BW_PROFILE_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MTR_METER_INS_TO_BW_PROFILE_MAP_SET_MTR_METER_INS_TO_BW_PROFILE_MAP_SET_BW_PROFILE_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_bw_profile_id = (uint32)param_val->value.ulong_value;
  } 

  /* Call function */  
  ret = soc_ppd_mtr_meter_ins_to_bw_profile_map_set(
    unit,
    0, /* default core*/
    &prm_meter_ins_ndx,
    prm_bw_profile_id
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_mtr_meter_ins_to_bw_profile_map_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_mtr_meter_ins_to_bw_profile_map_set");   
    goto exit; 
  } 


  goto exit; 
exit: 
  return ui_ret; 
} 

/******************************************************************** 
*  Function handler: mtr_meter_ins_to_bw_profile_map_get (section metering)
 */
int 
ui_ppd_api_metering_mtr_meter_ins_to_bw_profile_map_get(
  CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_MTR_METER_ID   
    prm_meter_ins_ndx;
  uint32   
    prm_bw_profile_id;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_metering"); 
  soc_sand_proc_name = "soc_ppd_mtr_meter_ins_to_bw_profile_map_get"; 

  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_MTR_METER_ID_clear(&prm_meter_ins_ndx);

  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_METER_INS_TO_BW_PROFILE_MAP_GET_MTR_METER_INS_TO_BW_PROFILE_MAP_GET_METER_INS_NDX_GROUP_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MTR_METER_INS_TO_BW_PROFILE_MAP_GET_MTR_METER_INS_TO_BW_PROFILE_MAP_GET_METER_INS_NDX_GROUP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_meter_ins_ndx.group = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_METER_INS_TO_BW_PROFILE_MAP_GET_MTR_METER_INS_TO_BW_PROFILE_MAP_GET_METER_INS_NDX_ID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MTR_METER_INS_TO_BW_PROFILE_MAP_GET_MTR_METER_INS_TO_BW_PROFILE_MAP_GET_METER_INS_NDX_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_meter_ins_ndx.id = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter meter_ins_ndx after mtr_meter_ins_to_bw_profile_map_get***", TRUE); 
    goto exit; 
  } 

  /* Call function */  
  ret = soc_ppd_mtr_meter_ins_to_bw_profile_map_get(
    unit,
    0, /* default core*/
    &prm_meter_ins_ndx,
    &prm_bw_profile_id
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_mtr_meter_ins_to_bw_profile_map_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_mtr_meter_ins_to_bw_profile_map_get");   
    goto exit; 
  } 

  cli_out("bw_profile_id: %u\n",prm_bw_profile_id);


  goto exit; 
exit: 
  return ui_ret; 
} 

/******************************************************************** 
*  Function handler: mtr_eth_policer_enable_set (section metering)
 */
int 
ui_ppd_api_metering_mtr_eth_policer_enable_set(
  CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint8   
    prm_enable;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_metering"); 
  soc_sand_proc_name = "soc_ppd_mtr_eth_policer_enable_set"; 

  unit = soc_ppd_get_default_unit(); 

  /* Get parameters */ 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_mtr_eth_policer_enable_get(
    unit,
    &prm_enable
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_mtr_eth_policer_enable_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_mtr_eth_policer_enable_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_ETH_POLICER_ENABLE_SET_MTR_ETH_POLICER_ENABLE_SET_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MTR_ETH_POLICER_ENABLE_SET_MTR_ETH_POLICER_ENABLE_SET_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_enable = (uint8)param_val->value.ulong_value;
  } 

  /* Call function */  
  ret = soc_ppd_mtr_eth_policer_enable_set(
    unit,
    prm_enable
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_mtr_eth_policer_enable_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_mtr_eth_policer_enable_set");   
    goto exit; 
  } 


  goto exit; 
exit: 
  return ui_ret; 
} 

/******************************************************************** 
*  Function handler: mtr_eth_policer_enable_get (section metering)
 */
int 
ui_ppd_api_metering_mtr_eth_policer_enable_get(
  CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint8   
    prm_enable;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_metering"); 
  soc_sand_proc_name = "soc_ppd_mtr_eth_policer_enable_get"; 

  unit = soc_ppd_get_default_unit(); 

  /* Get parameters */ 

  /* Call function */  
  ret = soc_ppd_mtr_eth_policer_enable_get(
    unit,
    &prm_enable
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_mtr_eth_policer_enable_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_mtr_eth_policer_enable_get");   
    goto exit; 
  } 

  cli_out("enable: %u\n",prm_enable);


  goto exit; 
exit: 
  return ui_ret; 
} 

/******************************************************************** 
*  Function handler: mtr_eth_policer_params_set (section metering)
 */
int 
ui_ppd_api_metering_mtr_eth_policer_params_set(
  CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_PORT   
    prm_port_ndx;
  SOC_PPC_MTR_ETH_TYPE   
    prm_eth_type_ndx;
  SOC_PPC_MTR_BW_PROFILE_INFO   
    prm_policer_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_metering"); 
  soc_sand_proc_name = "soc_ppd_mtr_eth_policer_params_set"; 

  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_MTR_BW_PROFILE_INFO_clear(&prm_policer_info);

  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_ETH_POLICER_PARAMS_SET_MTR_ETH_POLICER_PARAMS_SET_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MTR_ETH_POLICER_PARAMS_SET_MTR_ETH_POLICER_PARAMS_SET_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter port_ndx after mtr_eth_policer_params_set***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_ETH_POLICER_PARAMS_SET_MTR_ETH_POLICER_PARAMS_SET_ETH_TYPE_NDX_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_MTR_ETH_POLICER_PARAMS_SET_MTR_ETH_POLICER_PARAMS_SET_ETH_TYPE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_eth_type_ndx = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter eth_type_ndx after mtr_eth_policer_params_set***", TRUE); 
    goto exit; 
  } 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_mtr_eth_policer_params_get(
    unit,
    prm_port_ndx,
    prm_eth_type_ndx,
    &prm_policer_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_mtr_eth_policer_params_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_mtr_eth_policer_params_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_ETH_POLICER_PARAMS_SET_MTR_ETH_POLICER_PARAMS_SET_DISABLE_EIR_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MTR_ETH_POLICER_PARAMS_SET_MTR_ETH_POLICER_PARAMS_SET_DISABLE_EIR_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_policer_info.disable_eir = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_ETH_POLICER_PARAMS_SET_MTR_ETH_POLICER_PARAMS_SET_DISABLE_CIR_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MTR_ETH_POLICER_PARAMS_SET_MTR_ETH_POLICER_PARAMS_SET_DISABLE_CIR_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_policer_info.disable_cir = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_ETH_POLICER_PARAMS_SET_MTR_ETH_POLICER_PARAMS_SET_POLICER_INFO_IS_COUPLING_ENABLED_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MTR_ETH_POLICER_PARAMS_SET_MTR_ETH_POLICER_PARAMS_SET_POLICER_INFO_IS_COUPLING_ENABLED_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_policer_info.is_coupling_enabled = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_ETH_POLICER_PARAMS_SET_MTR_ETH_POLICER_PARAMS_SET_POLICER_INFO_IS_FAIRNESS_ENABLED_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MTR_ETH_POLICER_PARAMS_SET_MTR_ETH_POLICER_PARAMS_SET_POLICER_INFO_IS_FAIRNESS_ENABLED_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_policer_info.is_fairness_enabled = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_ETH_POLICER_PARAMS_SET_MTR_ETH_POLICER_PARAMS_SET_POLICER_INFO_COLOR_MODE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_MTR_ETH_POLICER_PARAMS_SET_MTR_ETH_POLICER_PARAMS_SET_POLICER_INFO_COLOR_MODE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_policer_info.color_mode = param_val->numeric_equivalent;
  } 
  
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_ETH_POLICER_PARAMS_SET_MTR_ETH_POLICER_PARAMS_SET_POLICER_INFO_IS_MTR_ENABLED_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MTR_ETH_POLICER_PARAMS_SET_MTR_ETH_POLICER_PARAMS_SET_POLICER_INFO_IS_MTR_ENABLED_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_policer_info.is_mtr_enabled = (uint8)param_val->value.ulong_value;
  }

   if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_ETH_POLICER_PARAMS_SET_MTR_ETH_POLICER_PARAMS_SET_POLICER_INFO_IS_PACKET_MODE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MTR_ETH_POLICER_PARAMS_SET_MTR_ETH_POLICER_PARAMS_SET_POLICER_INFO_IS_PACKET_MODE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_policer_info.is_packet_mode = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_ETH_POLICER_PARAMS_SET_MTR_ETH_POLICER_PARAMS_SET_POLICER_INFO_EBS_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MTR_ETH_POLICER_PARAMS_SET_MTR_ETH_POLICER_PARAMS_SET_POLICER_INFO_EBS_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_policer_info.ebs = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_ETH_POLICER_PARAMS_SET_MTR_ETH_POLICER_PARAMS_SET_POLICER_INFO_CBS_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MTR_ETH_POLICER_PARAMS_SET_MTR_ETH_POLICER_PARAMS_SET_POLICER_INFO_CBS_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_policer_info.cbs = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_ETH_POLICER_PARAMS_SET_MTR_ETH_POLICER_PARAMS_SET_POLICER_INFO_EIR_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MTR_ETH_POLICER_PARAMS_SET_MTR_ETH_POLICER_PARAMS_SET_POLICER_INFO_EIR_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_policer_info.eir = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_ETH_POLICER_PARAMS_SET_MTR_ETH_POLICER_PARAMS_SET_POLICER_INFO_CIR_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MTR_ETH_POLICER_PARAMS_SET_MTR_ETH_POLICER_PARAMS_SET_POLICER_INFO_CIR_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_policer_info.cir = (uint32)param_val->value.ulong_value;
  } 

  /* Call function */  
  ret = soc_ppd_mtr_eth_policer_params_set(
    unit,
    prm_port_ndx,
    prm_eth_type_ndx,
    &prm_policer_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_mtr_eth_policer_params_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_mtr_eth_policer_params_set");   
    goto exit; 
  } 


  goto exit; 
exit: 
  return ui_ret; 
} 

/******************************************************************** 
*  Function handler: mtr_eth_policer_params_get (section metering)
 */
int 
ui_ppd_api_metering_mtr_eth_policer_params_get(
  CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_PORT   
    prm_port_ndx;
  SOC_PPC_MTR_ETH_TYPE   
    prm_eth_type_ndx;
  SOC_PPC_MTR_BW_PROFILE_INFO   
    prm_policer_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_metering"); 
  soc_sand_proc_name = "soc_ppd_mtr_eth_policer_params_get"; 

  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_MTR_BW_PROFILE_INFO_clear(&prm_policer_info);

  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_ETH_POLICER_PARAMS_GET_MTR_ETH_POLICER_PARAMS_GET_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MTR_ETH_POLICER_PARAMS_GET_MTR_ETH_POLICER_PARAMS_GET_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter port_ndx after mtr_eth_policer_params_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_ETH_POLICER_PARAMS_GET_MTR_ETH_POLICER_PARAMS_GET_ETH_TYPE_NDX_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_MTR_ETH_POLICER_PARAMS_GET_MTR_ETH_POLICER_PARAMS_GET_ETH_TYPE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_eth_type_ndx = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter eth_type_ndx after mtr_eth_policer_params_get***", TRUE); 
    goto exit; 
  } 

  /* Call function */  
  ret = soc_ppd_mtr_eth_policer_params_get(
    unit,
    prm_port_ndx,
    prm_eth_type_ndx,
    &prm_policer_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_mtr_eth_policer_params_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_mtr_eth_policer_params_get");   
    goto exit; 
  } 

  SOC_PPC_MTR_BW_PROFILE_INFO_print(&prm_policer_info);


  goto exit; 
exit: 
  return ui_ret; 
} 

/******************************************************************** 
 *  Function handler: mtr_eth_policer_glbl_profile_set (section metering)
 */
int 
  ui_ppd_api_metering_mtr_eth_policer_glbl_profile_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_glbl_profile_idx = 0x0;
  SOC_PPC_MTR_BW_PROFILE_INFO   
    prm_policer_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_metering"); 
  soc_sand_proc_name = "soc_ppd_mtr_eth_policer_glbl_profile_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_MTR_BW_PROFILE_INFO_clear(&prm_policer_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_ETH_POLICER_GLBL_PROFILE_SET_MTR_ETH_POLICER_GLBL_PROFILE_SET_GLBL_PROFILE_IDX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MTR_ETH_POLICER_GLBL_PROFILE_SET_MTR_ETH_POLICER_GLBL_PROFILE_SET_GLBL_PROFILE_IDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_glbl_profile_idx = (uint32)param_val->value.ulong_value;
  } 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_mtr_eth_policer_glbl_profile_get(
          unit,
          prm_glbl_profile_idx,
          &prm_policer_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_mtr_eth_policer_glbl_profile_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_mtr_eth_policer_glbl_profile_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_ETH_POLICER_GLBL_PROFILE_SET_MTR_ETH_POLICER_GLBL_PROFILE_SET_DISABLE_EIR_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MTR_ETH_POLICER_GLBL_PROFILE_SET_MTR_ETH_POLICER_GLBL_PROFILE_SET_DISABLE_EIR_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_policer_info.disable_eir = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_ETH_POLICER_GLBL_PROFILE_SET_MTR_ETH_POLICER_GLBL_PROFILE_SET_DISABLE_CIR_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MTR_ETH_POLICER_GLBL_PROFILE_SET_MTR_ETH_POLICER_GLBL_PROFILE_SET_DISABLE_CIR_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_policer_info.disable_cir = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_ETH_POLICER_GLBL_PROFILE_SET_MTR_ETH_POLICER_GLBL_PROFILE_SET_IS_MTR_ENABLED_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MTR_ETH_POLICER_GLBL_PROFILE_SET_MTR_ETH_POLICER_GLBL_PROFILE_SET_IS_MTR_ENABLED_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_policer_info.is_mtr_enabled = (uint8)param_val->value.ulong_value;
  }

   if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_ETH_POLICER_GLBL_PROFILE_SET_MTR_ETH_POLICER_GLBL_PROFILE_SET_POLICER_INFO_IS_PACKET_MODE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MTR_ETH_POLICER_GLBL_PROFILE_SET_MTR_ETH_POLICER_GLBL_PROFILE_SET_POLICER_INFO_IS_PACKET_MODE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_policer_info.is_packet_mode = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_ETH_POLICER_GLBL_PROFILE_SET_MTR_ETH_POLICER_GLBL_PROFILE_SET_POLICER_INFO_IS_COUPLING_ENABLED_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MTR_ETH_POLICER_GLBL_PROFILE_SET_MTR_ETH_POLICER_GLBL_PROFILE_SET_POLICER_INFO_IS_COUPLING_ENABLED_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_policer_info.is_coupling_enabled = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_ETH_POLICER_GLBL_PROFILE_SET_MTR_ETH_POLICER_GLBL_PROFILE_SET_POLICER_INFO_IS_FAIRNESS_ENABLED_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MTR_ETH_POLICER_GLBL_PROFILE_SET_MTR_ETH_POLICER_GLBL_PROFILE_SET_POLICER_INFO_IS_FAIRNESS_ENABLED_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_policer_info.is_fairness_enabled = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_ETH_POLICER_GLBL_PROFILE_SET_MTR_ETH_POLICER_GLBL_PROFILE_SET_POLICER_INFO_COLOR_MODE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_MTR_ETH_POLICER_GLBL_PROFILE_SET_MTR_ETH_POLICER_GLBL_PROFILE_SET_POLICER_INFO_COLOR_MODE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_policer_info.color_mode = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_ETH_POLICER_GLBL_PROFILE_SET_MTR_ETH_POLICER_GLBL_PROFILE_SET_POLICER_INFO_EBS_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MTR_ETH_POLICER_GLBL_PROFILE_SET_MTR_ETH_POLICER_GLBL_PROFILE_SET_POLICER_INFO_EBS_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_policer_info.ebs = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_ETH_POLICER_GLBL_PROFILE_SET_MTR_ETH_POLICER_GLBL_PROFILE_SET_POLICER_INFO_CBS_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MTR_ETH_POLICER_GLBL_PROFILE_SET_MTR_ETH_POLICER_GLBL_PROFILE_SET_POLICER_INFO_CBS_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_policer_info.cbs = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_ETH_POLICER_GLBL_PROFILE_SET_MTR_ETH_POLICER_GLBL_PROFILE_SET_POLICER_INFO_EIR_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MTR_ETH_POLICER_GLBL_PROFILE_SET_MTR_ETH_POLICER_GLBL_PROFILE_SET_POLICER_INFO_EIR_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_policer_info.eir = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_ETH_POLICER_GLBL_PROFILE_SET_MTR_ETH_POLICER_GLBL_PROFILE_SET_POLICER_INFO_CIR_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MTR_ETH_POLICER_GLBL_PROFILE_SET_MTR_ETH_POLICER_GLBL_PROFILE_SET_POLICER_INFO_CIR_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_policer_info.cir = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_mtr_eth_policer_glbl_profile_set(
          unit,
          prm_glbl_profile_idx,
          &prm_policer_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_mtr_eth_policer_glbl_profile_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_mtr_eth_policer_glbl_profile_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: mtr_eth_policer_glbl_profile_get (section metering)
 */
int 
  ui_ppd_api_metering_mtr_eth_policer_glbl_profile_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_glbl_profile_idx = 0x0;
  SOC_PPC_MTR_BW_PROFILE_INFO   
    prm_policer_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_metering"); 
  soc_sand_proc_name = "soc_ppd_mtr_eth_policer_glbl_profile_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_MTR_BW_PROFILE_INFO_clear(&prm_policer_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_ETH_POLICER_GLBL_PROFILE_GET_MTR_ETH_POLICER_GLBL_PROFILE_GET_GLBL_PROFILE_IDX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MTR_ETH_POLICER_GLBL_PROFILE_GET_MTR_ETH_POLICER_GLBL_PROFILE_GET_GLBL_PROFILE_IDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_glbl_profile_idx = (uint32)param_val->value.ulong_value;
  } 

  /* Call function */
  ret = soc_ppd_mtr_eth_policer_glbl_profile_get(
          unit,
          prm_glbl_profile_idx,
          &prm_policer_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_mtr_eth_policer_glbl_profile_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_mtr_eth_policer_glbl_profile_get");   
    goto exit; 
  } 

  cli_out("glbl_profile_idx: %u\n\r",prm_glbl_profile_idx);

  SOC_PPC_MTR_BW_PROFILE_INFO_print(&prm_policer_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: mtr_eth_policer_glbl_profile_map_set (section metering)
 */
int 
  ui_ppd_api_metering_mtr_eth_policer_glbl_profile_map_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_PORT   
    prm_port_ndx;
  SOC_PPC_MTR_ETH_TYPE   
    prm_eth_type_ndx;
  uint32   
    prm_glbl_profile_idx = 0x0;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_metering"); 
  soc_sand_proc_name = "soc_ppd_mtr_eth_policer_glbl_profile_map_set"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_ETH_POLICER_GLBL_PROFILE_MAP_SET_MTR_ETH_POLICER_GLBL_PROFILE_MAP_SET_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MTR_ETH_POLICER_GLBL_PROFILE_MAP_SET_MTR_ETH_POLICER_GLBL_PROFILE_MAP_SET_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_port_ndx = (SOC_PPC_PORT)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter port_ndx after mtr_eth_policer_glbl_profile_map_set***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_ETH_POLICER_GLBL_PROFILE_MAP_SET_MTR_ETH_POLICER_GLBL_PROFILE_MAP_SET_ETH_TYPE_NDX_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_MTR_ETH_POLICER_GLBL_PROFILE_MAP_SET_MTR_ETH_POLICER_GLBL_PROFILE_MAP_SET_ETH_TYPE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_eth_type_ndx = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter eth_type_ndx after mtr_eth_policer_glbl_profile_map_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_mtr_eth_policer_glbl_profile_map_get(
          unit,
          prm_port_ndx,
          prm_eth_type_ndx,
          &prm_glbl_profile_idx
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_mtr_eth_policer_glbl_profile_map_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_mtr_eth_policer_glbl_profile_map_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_ETH_POLICER_GLBL_PROFILE_MAP_SET_MTR_ETH_POLICER_GLBL_PROFILE_MAP_SET_GLBL_PROFILE_IDX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MTR_ETH_POLICER_GLBL_PROFILE_MAP_SET_MTR_ETH_POLICER_GLBL_PROFILE_MAP_SET_GLBL_PROFILE_IDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_glbl_profile_idx = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_mtr_eth_policer_glbl_profile_map_set(
          unit,
          prm_port_ndx,
          prm_eth_type_ndx,
          prm_glbl_profile_idx
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_mtr_eth_policer_glbl_profile_map_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_mtr_eth_policer_glbl_profile_map_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: mtr_eth_policer_glbl_profile_map_get (section metering)
 */
int 
  ui_ppd_api_metering_mtr_eth_policer_glbl_profile_map_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_PORT   
    prm_port_ndx;
  SOC_PPC_MTR_ETH_TYPE   
    prm_eth_type_ndx;
  uint32   
    prm_glbl_profile_idx = 0x0;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_metering"); 
  soc_sand_proc_name = "soc_ppd_mtr_eth_policer_glbl_profile_map_get"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_ETH_POLICER_GLBL_PROFILE_MAP_GET_MTR_ETH_POLICER_GLBL_PROFILE_MAP_GET_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MTR_ETH_POLICER_GLBL_PROFILE_MAP_GET_MTR_ETH_POLICER_GLBL_PROFILE_MAP_GET_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_port_ndx = (SOC_PPC_PORT)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter port_ndx after mtr_eth_policer_glbl_profile_map_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_ETH_POLICER_GLBL_PROFILE_MAP_GET_MTR_ETH_POLICER_GLBL_PROFILE_MAP_GET_ETH_TYPE_NDX_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_MTR_ETH_POLICER_GLBL_PROFILE_MAP_GET_MTR_ETH_POLICER_GLBL_PROFILE_MAP_GET_ETH_TYPE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_eth_type_ndx = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter eth_type_ndx after mtr_eth_policer_glbl_profile_map_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_mtr_eth_policer_glbl_profile_map_get(
          unit,
          prm_port_ndx,
          prm_eth_type_ndx,
          &prm_glbl_profile_idx
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_mtr_eth_policer_glbl_profile_map_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_mtr_eth_policer_glbl_profile_map_get");   
    goto exit; 
  } 

  cli_out("glbl_profile_idx: %u\n\r",prm_glbl_profile_idx);

  
  goto exit; 
exit: 
  return ui_ret; 
} 

/******************************************************************** 
*  Section handler: metering
 */ 
int 
ui_ppd_api_metering( 
                    CURRENT_LINE *current_line 
                    ) 
{   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_metering"); 
 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_METERS_GROUP_INFO_SET_MTR_METERS_GROUP_INFO_SET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_metering_mtr_meters_group_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_METERS_GROUP_INFO_GET_MTR_METERS_GROUP_INFO_GET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_metering_mtr_meters_group_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_BW_PROFILE_ADD_MTR_BW_PROFILE_ADD_ID,1)) 
  { 
    ui_ret = ui_ppd_api_metering_mtr_bw_profile_add(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_BW_PROFILE_GET_MTR_BW_PROFILE_GET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_metering_mtr_bw_profile_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_BW_PROFILE_REMOVE_MTR_BW_PROFILE_REMOVE_ID,1)) 
  { 
    ui_ret = ui_ppd_api_metering_mtr_bw_profile_remove(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_METER_INS_TO_BW_PROFILE_MAP_SET_MTR_METER_INS_TO_BW_PROFILE_MAP_SET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_metering_mtr_meter_ins_to_bw_profile_map_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_METER_INS_TO_BW_PROFILE_MAP_GET_MTR_METER_INS_TO_BW_PROFILE_MAP_GET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_metering_mtr_meter_ins_to_bw_profile_map_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_ETH_POLICER_ENABLE_SET_MTR_ETH_POLICER_ENABLE_SET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_metering_mtr_eth_policer_enable_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_ETH_POLICER_ENABLE_GET_MTR_ETH_POLICER_ENABLE_GET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_metering_mtr_eth_policer_enable_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_ETH_POLICER_PARAMS_SET_MTR_ETH_POLICER_PARAMS_SET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_metering_mtr_eth_policer_params_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_ETH_POLICER_PARAMS_GET_MTR_ETH_POLICER_PARAMS_GET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_metering_mtr_eth_policer_params_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_ETH_POLICER_GLBL_PROFILE_SET_MTR_ETH_POLICER_GLBL_PROFILE_SET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_metering_mtr_eth_policer_glbl_profile_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_ETH_POLICER_GLBL_PROFILE_GET_MTR_ETH_POLICER_GLBL_PROFILE_GET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_metering_mtr_eth_policer_glbl_profile_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_ETH_POLICER_GLBL_PROFILE_MAP_SET_MTR_ETH_POLICER_GLBL_PROFILE_MAP_SET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_metering_mtr_eth_policer_glbl_profile_map_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MTR_ETH_POLICER_GLBL_PROFILE_MAP_GET_MTR_ETH_POLICER_GLBL_PROFILE_MAP_GET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_metering_mtr_eth_policer_glbl_profile_map_get(current_line); 
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting function name after metering***", TRUE); 
  } 

  goto exit; 
exit:        
  return ui_ret; 
}                

#endif
#endif /* LINK_PPD_LIBRARIES */ 

