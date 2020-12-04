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
 


#include <soc/dpp/PPD/ppd_api_frwrd_mact_mgmt.h>                                                               
#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_frwrd_mact_mgmt.h>

#if LINK_PPD_LIBRARIES

#ifdef UI_FRWRD_MACT_MGMT

int
ui_ppd_frwrd_mact_lookup_type_set(
  CURRENT_LINE *current_line
  )
{
    uint32
    ret;
  SOC_PPC_FRWRD_MACT_LOOKUP_TYPE
    lookup_type = SOC_PPC_FRWRD_MACT_LOOKUP_TYPE_SA_LOOKUP;

  UI_MACROS_INIT_FUNCTION("ui_ppd_frwrd_mact_lookup_type_set");
  soc_sand_proc_name = "soc_ppd_frwrd_mact_lookup_type_set";

  unit = soc_ppd_get_default_unit();

  /* Get parameters */

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_PPD_FRWRD_MACT_LOOKUP_TYPE_SET_PPD_FRWRD_MACT_LOOKUP_TYPE_SET_LOOKUP_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_PPD_FRWRD_MACT_LOOKUP_TYPE_SET_PPD_FRWRD_MACT_LOOKUP_TYPE_SET_LOOKUP_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    lookup_type = (uint32)param_val->value.ulong_value;
  } 

  /* Call function */
  ret = soc_ppd_frwrd_mact_lookup_type_set(
          unit,
          lookup_type
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_mact_lookup_type_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_lookup_type_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret; 
}

/********************************************************************
*  Function handler: oper_mode_info_set (section frwrd_mact)
 */
int
ui_ppd_api_frwrd_mact_oper_mode_info_set(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_PPC_FRWRD_MACT_OPER_MODE_INFO
    prm_oper_mode_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_mact");
  soc_sand_proc_name = "soc_ppd_frwrd_mact_oper_mode_info_set";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_FRWRD_MACT_OPER_MODE_INFO_clear(&prm_oper_mode_info);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_ppd_frwrd_mact_oper_mode_info_get(
          unit,
          &prm_oper_mode_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_mact_oper_mode_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_oper_mode_info_get");
    goto exit;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_OPER_MODE_INFO_SET_OPER_MODE_INFO_SET_OPER_MODE_INFO_AUTO_MODE_INFO_HEADER_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_MACT_OPER_MODE_INFO_SET_OPER_MODE_INFO_SET_OPER_MODE_INFO_AUTO_MODE_INFO_HEADER_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_oper_mode_info.learn_msgs_info.info.header_type = param_val->numeric_equivalent;
    prm_oper_mode_info.shadow_msgs_info.info.header_type = param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_OPER_MODE_INFO_SET_OPER_MODE_INFO_SET_OPER_MODE_INFO_AUTO_MODE_INFO_HEADER_TYPE_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_OPER_MODE_INFO_SET_OPER_MODE_INFO_SET_OPER_MODE_INFO_AUTO_MODE_INFO_HEADER_TYPE_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_oper_mode_info.learn_msgs_info.info.header[7] = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_OPER_MODE_INFO_SET_OPER_MODE_INFO_SET_OPER_MODE_INFO_LEARN_MSGS_INFO_INFO_HEADER_TYPE_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_OPER_MODE_INFO_SET_OPER_MODE_INFO_SET_OPER_MODE_INFO_LEARN_MSGS_INFO_INFO_HEADER_TYPE_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_oper_mode_info.shadow_msgs_info.info.header[7] = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_OPER_MODE_INFO_SET_OPER_MODE_INFO_SET_OPER_MODE_INFO_SHADOW_MODE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_MACT_OPER_MODE_INFO_SET_OPER_MODE_INFO_SET_OPER_MODE_INFO_SHADOW_MODE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_oper_mode_info.shadow_mode = param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_OPER_MODE_INFO_SET_OPER_MODE_INFO_SET_OPER_MODE_INFO_LEARN_MSGS_INFO_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_MACT_OPER_MODE_INFO_SET_OPER_MODE_INFO_SET_OPER_MODE_INFO_LEARN_MSGS_INFO_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_oper_mode_info.learn_msgs_info.type = param_val->numeric_equivalent;
    prm_oper_mode_info.shadow_msgs_info.type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_OPER_MODE_INFO_SET_OPER_MODE_INFO_SET_OPER_MODE_INFO_PETRA_A_COMPATIBLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_OPER_MODE_INFO_SET_OPER_MODE_INFO_SET_OPER_MODE_INFO_PETRA_A_COMPATIBLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_oper_mode_info.soc_petra_a_compatible = (uint8)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_OPER_MODE_INFO_SET_OPER_MODE_INFO_SET_OPER_MODE_INFO_LEARNING_MODE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_MACT_OPER_MODE_INFO_SET_OPER_MODE_INFO_SET_OPER_MODE_INFO_LEARNING_MODE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_oper_mode_info.learning_mode = param_val->numeric_equivalent;
  }

  /* Call function */
  ret = soc_ppd_frwrd_mact_oper_mode_info_set(
          unit,
          &prm_oper_mode_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_mact_oper_mode_info_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_oper_mode_info_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret; 
}

/********************************************************************
*  Function handler: oper_mode_info_get (section frwrd_mact)
 */
int
ui_ppd_api_frwrd_mact_oper_mode_info_get(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_PPC_FRWRD_MACT_OPER_MODE_INFO
    prm_oper_mode_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_mact");
  soc_sand_proc_name = "soc_ppd_frwrd_mact_oper_mode_info_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_FRWRD_MACT_OPER_MODE_INFO_clear(&prm_oper_mode_info);

  /* Get parameters */

  /* Call function */
  ret = soc_ppd_frwrd_mact_oper_mode_info_get(
    unit,
    &prm_oper_mode_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_mact_oper_mode_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_oper_mode_info_get");
    goto exit;
  }

  send_string_to_screen("--> oper_mode_info:", TRUE);
  SOC_PPC_FRWRD_MACT_OPER_MODE_INFO_print(&prm_oper_mode_info);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: aging_info_set (section frwrd_mact)
 */
int
ui_ppd_api_frwrd_mact_aging_info_set(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_PPC_FRWRD_MACT_AGING_INFO
    prm_aging_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_mact");
  soc_sand_proc_name = "soc_ppd_frwrd_mact_aging_info_set";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_FRWRD_MACT_AGING_INFO_clear(&prm_aging_info);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_ppd_frwrd_mact_aging_info_get(
    unit,
    &prm_aging_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_mact_aging_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_aging_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_AGING_INFO_SET_AGING_INFO_SET_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_AGING_INFO_SET_AGING_INFO_SET_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_aging_info.enable_aging = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_AGING_INFO_SET_AGING_INFO_SET_TIME_SEC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_AGING_INFO_SET_AGING_INFO_SET_TIME_SEC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_aging_info.aging_time.sec = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_AGING_INFO_SET_AGING_INFO_SET_TIME_MILI_SEC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_AGING_INFO_SET_AGING_INFO_SET_TIME_MILI_SEC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_aging_info.aging_time.mili_sec = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_frwrd_mact_aging_info_set(
    unit,
    &prm_aging_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_mact_aging_info_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_aging_info_set");
    goto exit;
  }

  send_string_to_screen("--> aging_info:", TRUE);
  SOC_PPC_FRWRD_MACT_AGING_INFO_print(&prm_aging_info);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: aging_info_get (section frwrd_mact)
 */
int
ui_ppd_api_frwrd_mact_aging_info_get(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_PPC_FRWRD_MACT_AGING_INFO
    prm_aging_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_mact");
  soc_sand_proc_name = "soc_ppd_frwrd_mact_aging_info_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_FRWRD_MACT_AGING_INFO_clear(&prm_aging_info);

  /* Get parameters */

  /* Call function */
  ret = soc_ppd_frwrd_mact_aging_info_get(
    unit,
    &prm_aging_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_mact_aging_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_aging_info_get");
    goto exit;
  }

  send_string_to_screen("--> aging_info:", TRUE);
  SOC_PPC_FRWRD_MACT_AGING_INFO_print(&prm_aging_info);

  goto exit;
exit:
  return ui_ret;
}
  
/******************************************************************** 
 *  Function handler: aging_events_handle_info_set (section frwrd_mact)
 */
int 
  ui_ppd_api_frwrd_mact_aging_events_handle_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE   
    prm_aging_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_mact"); 
  soc_sand_proc_name = "soc_ppd_frwrd_mact_aging_events_handle_info_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE_clear(&prm_aging_info);
 
  /* Get parameters */ 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_frwrd_mact_aging_events_handle_info_get(
          unit,
          &prm_aging_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_mact_aging_events_handle_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_aging_events_handle_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_SET_AGING_EVENTS_HANDLE_INFO_SET_AGING_INFO_EVENT_WHEN_REFRESHED_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_SET_AGING_EVENTS_HANDLE_INFO_SET_AGING_INFO_EVENT_WHEN_REFRESHED_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_aging_info.event_when_refreshed = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_SET_AGING_EVENTS_HANDLE_INFO_SET_AGING_INFO_EVENT_WHEN_AGED_OUT_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_SET_AGING_EVENTS_HANDLE_INFO_SET_AGING_INFO_EVENT_WHEN_AGED_OUT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_aging_info.event_when_aged_out = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_SET_AGING_EVENTS_HANDLE_INFO_SET_AGING_INFO_DELETE_INTERNALLY_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_SET_AGING_EVENTS_HANDLE_INFO_SET_AGING_INFO_DELETE_INTERNALLY_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_aging_info.delete_internally = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_frwrd_mact_aging_events_handle_info_set(
          unit,
          &prm_aging_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_mact_aging_events_handle_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_aging_events_handle_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: aging_events_handle_info_get (section frwrd_mact)
 */
int 
  ui_ppd_api_frwrd_mact_aging_events_handle_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE   
    prm_aging_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_mact"); 
  soc_sand_proc_name = "soc_ppd_frwrd_mact_aging_events_handle_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE_clear(&prm_aging_info);
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_ppd_frwrd_mact_aging_events_handle_info_get(
          unit,
          &prm_aging_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_mact_aging_events_handle_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_aging_events_handle_info_get");   
    goto exit; 
  } 

  SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE_print(&prm_aging_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: fid_profile_to_fid_map_set (section frwrd_mact)
 */
int
ui_ppd_api_frwrd_mact_fid_profile_to_fid_map_set(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_fid_profile_ndx;
  SOC_PPC_FID   
    prm_fid;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_mact");
  soc_sand_proc_name = "soc_ppd_frwrd_mact_fid_profile_to_fid_map_set";

  unit = soc_ppd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_SET_FID_PROFILE_TO_FID_MAP_SET_FID_PROFILE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_SET_FID_PROFILE_TO_FID_MAP_SET_FID_PROFILE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_fid_profile_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter fid_profile_ndx after fid_profile_to_fid_map_set***", TRUE); 
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = soc_ppd_frwrd_mact_fid_profile_to_fid_map_get(
    unit,
    prm_fid_profile_ndx,
    &prm_fid
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_mact_fid_profile_to_fid_map_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ui_ret, "soc_ppd_frwrd_mact_fid_profile_to_fid_map_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_SET_FID_PROFILE_TO_FID_MAP_SET_FID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_SET_FID_PROFILE_TO_FID_MAP_SET_FID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_fid = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_frwrd_mact_fid_profile_to_fid_map_set(
    unit,
    prm_fid_profile_ndx,
    prm_fid
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_mact_fid_profile_to_fid_map_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ui_ret, "soc_ppd_frwrd_mact_fid_profile_to_fid_map_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: fid_profile_to_fid_map_get (section frwrd_mact)
 */
int
ui_ppd_api_frwrd_mact_fid_profile_to_fid_map_get(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_fid_profile_ndx;
  SOC_PPC_FID   
    prm_fid;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_mact");
  soc_sand_proc_name = "soc_ppd_frwrd_mact_fid_profile_to_fid_map_get";

  unit = soc_ppd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_GET_FID_PROFILE_TO_FID_MAP_GET_FID_PROFILE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_GET_FID_PROFILE_TO_FID_MAP_GET_FID_PROFILE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_fid_profile_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter fid_profile_ndx after fid_profile_info_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_frwrd_mact_fid_profile_to_fid_map_get(
    unit,
    prm_fid_profile_ndx,
    &prm_fid
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_mact_fid_profile_to_fid_map_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ui_ret, "soc_ppd_frwrd_mact_fid_profile_to_fid_map_get");
    goto exit;
  }

  send_string_to_screen("--> fid:", TRUE);
  cli_out("fid: %u\n",prm_fid);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: mac_limit_info_set (section frwrd_mact)
 */
int
ui_ppd_api_frwrd_mact_mac_limit_glbl_info_set(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO
    prm_limit_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_mact");
  soc_sand_proc_name = "soc_ppd_frwrd_mact_mac_limit_glbl_info_set";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_clear(&prm_limit_info);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_ppd_frwrd_mact_mac_limit_glbl_info_get(
    unit,
    &prm_limit_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_mact_mac_limit_glbl_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_mac_limit_glbl_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_SET_MAC_LIMIT_INFO_SET_LIMIT_INFO_FID_BASE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_SET_MAC_LIMIT_INFO_SET_LIMIT_INFO_FID_BASE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_limit_info.fid_base = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_SET_MAC_LIMIT_INFO_SET_LIMIT_INFO_STATIC_MAY_EXCEED_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_SET_MAC_LIMIT_INFO_SET_LIMIT_INFO_STATIC_MAY_EXCEED_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_limit_info.static_may_exceed = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_SET_MAC_LIMIT_INFO_SET_LIMIT_INFO_GLBL_LIMIT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_SET_MAC_LIMIT_INFO_SET_LIMIT_INFO_GLBL_LIMIT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_limit_info.glbl_limit = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_SET_MAC_LIMIT_INFO_SET_LIMIT_INFO_CPU_MAY_EXCEED_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_SET_MAC_LIMIT_INFO_SET_LIMIT_INFO_CPU_MAY_EXCEED_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_limit_info.cpu_may_exceed = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_SET_MAC_LIMIT_INFO_SET_LIMIT_INFO_GENERATE_EVENT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_SET_MAC_LIMIT_INFO_SET_LIMIT_INFO_GENERATE_EVENT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_limit_info.generate_event = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_SET_MAC_LIMIT_INFO_SET_LIMIT_INFO_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_SET_MAC_LIMIT_INFO_SET_LIMIT_INFO_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_limit_info.enable = (uint8)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_frwrd_mact_mac_limit_glbl_info_set(
    unit,
    &prm_limit_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_mact_mac_limit_glbl_info_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_mac_limit_glbl_info_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: mac_limit_info_get (section frwrd_mact)
 */
int
ui_ppd_api_frwrd_mact_mac_limit_glbl_info_get(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO
    prm_limit_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_mact");
  soc_sand_proc_name = "soc_ppd_frwrd_mact_mac_limit_glbl_info_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_clear(&prm_limit_info);

  /* Get parameters */

  /* Call function */
  ret = soc_ppd_frwrd_mact_mac_limit_glbl_info_get(
    unit,
    &prm_limit_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_mact_mac_limit_glbl_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_mac_limit_glbl_info_get");
    goto exit;
  }

  send_string_to_screen("--> limit_info:", TRUE);
  SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_print(&prm_limit_info);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: limit_profile_info_set (section frwrd_mact)
 */
int
ui_ppd_api_frwrd_mact_learn_profile_limit_info_set(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_mac_learn_profile_ndx;
  SOC_PPC_FRWRD_MACT_MAC_LIMIT_INFO   
    prm_limit_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_mact_mgmt"); 
  soc_sand_proc_name = "soc_ppd_frwrd_mact_learn_profile_limit_info_set";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_FRWRD_MACT_MAC_LIMIT_INFO_clear(&prm_limit_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_SET_LIMIT_PROFILE_INFO_SET_LIMIT_PROFILE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_SET_LIMIT_PROFILE_INFO_SET_LIMIT_PROFILE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mac_learn_profile_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mac_learn_profile_ndx after frwrd_mact_learn_profile_limit_info_set***", TRUE); 
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = soc_ppd_frwrd_mact_learn_profile_limit_info_get(
    unit,
          prm_mac_learn_profile_ndx,
    &prm_limit_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_mact_learn_profile_limit_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_learn_profile_limit_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_SET_LIMIT_PROFILE_INFO_SET_LIMIT_INFO_ACTION_WHEN_EXCEED_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_SET_LIMIT_PROFILE_INFO_SET_LIMIT_INFO_ACTION_WHEN_EXCEED_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_limit_info.action_when_exceed = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_SET_LIMIT_PROFILE_INFO_SET_LIMIT_INFO_NOF_ENTRIES_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_SET_LIMIT_PROFILE_INFO_SET_LIMIT_INFO_NOF_ENTRIES_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_limit_info.nof_entries = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_SET_LIMIT_PROFILE_INFO_SET_LIMIT_INFO_IS_LIMITED_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_SET_LIMIT_PROFILE_INFO_SET_LIMIT_INFO_IS_LIMITED_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_limit_info.is_limited = (uint8)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_frwrd_mact_learn_profile_limit_info_set(
    unit,
          prm_mac_learn_profile_ndx,
    &prm_limit_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_mact_learn_profile_limit_info_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_learn_profile_limit_info_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: limit_profile_info_get (section frwrd_mact)
 */
int
ui_ppd_api_frwrd_mact_learn_profile_limit_info_get(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_mac_learn_profile_ndx;
  SOC_PPC_FRWRD_MACT_MAC_LIMIT_INFO   
    prm_limit_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_mact_mgmt"); 
  soc_sand_proc_name = "soc_ppd_frwrd_mact_learn_profile_limit_info_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_FRWRD_MACT_MAC_LIMIT_INFO_clear(&prm_limit_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_GET_LIMIT_PROFILE_INFO_GET_LIMIT_PROFILE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_GET_LIMIT_PROFILE_INFO_GET_LIMIT_PROFILE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mac_learn_profile_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter limit_profile_ndx after limit_profile_info_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_frwrd_mact_learn_profile_limit_info_get(
    unit,
          prm_mac_learn_profile_ndx,
          &prm_limit_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_mact_learn_profile_limit_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_learn_profile_limit_info_get");
    goto exit;
  }

  send_string_to_screen("--> limit_info:", TRUE);
  SOC_PPC_FRWRD_MACT_MAC_LIMIT_INFO_print(&prm_limit_info);

  goto exit;
exit:
  return ui_ret;
}
  
/******************************************************************** 
 *  Function handler: event_handle_profile_set (section frwrd_mact)
 */
int 
  ui_ppd_api_frwrd_mact_event_handle_profile_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_mac_learn_profile_ndx;
  uint32   
    prm_event_handle_profile;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_mact"); 
  soc_sand_proc_name = "soc_ppd_frwrd_mact_event_handle_profile_set"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_EVENT_HANDLE_PROFILE_SET_EVENT_HANDLE_PROFILE_SET_MAC_LEARN_PROFILE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_EVENT_HANDLE_PROFILE_SET_EVENT_HANDLE_PROFILE_SET_MAC_LEARN_PROFILE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_mac_learn_profile_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter mac_learn_profile_ndx after event_handle_profile_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_frwrd_mact_event_handle_profile_get(
          unit,
          prm_mac_learn_profile_ndx,
          &prm_event_handle_profile
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_mact_event_handle_profile_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_event_handle_profile_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_EVENT_HANDLE_PROFILE_SET_EVENT_HANDLE_PROFILE_SET_EVENT_HANDLE_PROFILE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_EVENT_HANDLE_PROFILE_SET_EVENT_HANDLE_PROFILE_SET_EVENT_HANDLE_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_event_handle_profile = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_frwrd_mact_event_handle_profile_set(
          unit,
          prm_mac_learn_profile_ndx,
          prm_event_handle_profile
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_mact_event_handle_profile_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_event_handle_profile_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: event_handle_profile_get (section frwrd_mact)
 */
int 
  ui_ppd_api_frwrd_mact_event_handle_profile_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_mac_learn_profile_ndx;
  uint32   
    prm_event_handle_profile;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_mact"); 
  soc_sand_proc_name = "soc_ppd_frwrd_mact_event_handle_profile_get"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_EVENT_HANDLE_PROFILE_GET_EVENT_HANDLE_PROFILE_GET_MAC_LEARN_PROFILE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_EVENT_HANDLE_PROFILE_GET_EVENT_HANDLE_PROFILE_GET_MAC_LEARN_PROFILE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_mac_learn_profile_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter mac_learn_profile_ndx after event_handle_profile_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_frwrd_mact_event_handle_profile_get(
          unit,
          prm_mac_learn_profile_ndx,
          &prm_event_handle_profile
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_mact_event_handle_profile_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_event_handle_profile_get");   
    goto exit; 
  } 

  cli_out("event_handle_profile: %u\n\r",prm_event_handle_profile);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: event_get (section frwrd_mact)
 */
int 
  ui_ppd_api_frwrd_mact_event_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_FRWRD_MACT_EVENT_BUFFER   
    prm_event_buf;
  SOC_PPC_FRWRD_MACT_EVENT_INFO   
    prm_mact_event;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_mact"); 
  soc_sand_proc_name = "soc_ppd_frwrd_mact_event_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_MACT_EVENT_BUFFER_clear(&prm_event_buf);
  SOC_PPC_FRWRD_MACT_EVENT_INFO_clear(&prm_mact_event);
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_ppd_frwrd_mact_event_get(
          unit,
          &prm_event_buf
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_mact_event_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_event_get");   
    goto exit; 
  } 

  if (prm_event_buf.buff_len != 0) {
	  ret = soc_ppd_frwrd_mact_event_parse(
		unit,
		&prm_event_buf,
		&prm_mact_event
		);
	  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
	  { 
		send_string_to_screen(" *** soc_ppd_frwrd_mact_event_parse - FAIL", TRUE); 
		soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_event_parse");   
		goto exit; 
	  } 
  }

  SOC_PPC_FRWRD_MACT_EVENT_BUFFER_print(&prm_event_buf);
  if (prm_event_buf.buff_len != 0) {
	  SOC_PPC_FRWRD_MACT_EVENT_INFO_print(&prm_mact_event);
  }
  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: event_handle_info_set (section frwrd_mact)
 */
int 
  ui_ppd_api_frwrd_mact_event_handle_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY   
    prm_event_key;
  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO   
    prm_handle_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_mact"); 
  soc_sand_proc_name = "soc_ppd_frwrd_mact_event_handle_info_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY_clear(&prm_event_key);
  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO_clear(&prm_handle_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_EVENT_HANDLE_INFO_SET_EVENT_HANDLE_INFO_SET_EVENT_KEY_FID_EVENT_HANDLE_PROFILE_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_EVENT_HANDLE_INFO_SET_EVENT_HANDLE_INFO_SET_EVENT_KEY_FID_EVENT_HANDLE_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_event_key.vsi_event_handle_profile = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_EVENT_HANDLE_INFO_SET_EVENT_HANDLE_INFO_SET_EVENT_KEY_IS_LAG_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_EVENT_HANDLE_INFO_SET_EVENT_HANDLE_INFO_SET_EVENT_KEY_IS_LAG_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_event_key.is_lag = (uint8)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_EVENT_HANDLE_INFO_SET_EVENT_HANDLE_INFO_SET_EVENT_KEY_EVENT_TYPE_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_MACT_EVENT_HANDLE_INFO_SET_EVENT_HANDLE_INFO_SET_EVENT_KEY_EVENT_TYPE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_event_key.event_type = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter event_key after event_handle_info_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_frwrd_mact_event_handle_info_get(
          unit,
          &prm_event_key,
          &prm_handle_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_mact_event_handle_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_event_handle_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_EVENT_HANDLE_INFO_SET_EVENT_HANDLE_INFO_SET_HANDLE_INFO_SEND_TO_SHADOW_FIFO_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_EVENT_HANDLE_INFO_SET_EVENT_HANDLE_INFO_SET_HANDLE_INFO_SEND_TO_SHADOW_FIFO_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_handle_info.send_to_shadow_fifo = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_EVENT_HANDLE_INFO_SET_EVENT_HANDLE_INFO_SET_HANDLE_INFO_SEND_TO_LEARNING_FIFO_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_EVENT_HANDLE_INFO_SET_EVENT_HANDLE_INFO_SET_HANDLE_INFO_SEND_TO_LEARNING_FIFO_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_handle_info.send_to_learning_fifo = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_EVENT_HANDLE_INFO_SET_EVENT_HANDLE_INFO_SET_HANDLE_INFO_SELF_LEARNING_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_EVENT_HANDLE_INFO_SET_EVENT_HANDLE_INFO_SET_HANDLE_INFO_SELF_LEARNING_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_handle_info.self_learning = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_frwrd_mact_event_handle_info_set(
          unit,
          &prm_event_key,
          &prm_handle_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_mact_event_handle_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_event_handle_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: event_handle_info_get (section frwrd_mact)
 */
int 
  ui_ppd_api_frwrd_mact_event_handle_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY   
    prm_event_key;
  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO   
    prm_handle_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_mact"); 
  soc_sand_proc_name = "soc_ppd_frwrd_mact_event_handle_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY_clear(&prm_event_key);
  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO_clear(&prm_handle_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_EVENT_HANDLE_INFO_GET_EVENT_HANDLE_INFO_GET_EVENT_KEY_FID_EVENT_HANDLE_PROFILE_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_EVENT_HANDLE_INFO_GET_EVENT_HANDLE_INFO_GET_EVENT_KEY_FID_EVENT_HANDLE_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_event_key.vsi_event_handle_profile = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_EVENT_HANDLE_INFO_GET_EVENT_HANDLE_INFO_GET_EVENT_KEY_IS_LAG_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_EVENT_HANDLE_INFO_GET_EVENT_HANDLE_INFO_GET_EVENT_KEY_IS_LAG_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_event_key.is_lag = (uint8)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_EVENT_HANDLE_INFO_GET_EVENT_HANDLE_INFO_GET_EVENT_KEY_EVENT_TYPE_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_MACT_EVENT_HANDLE_INFO_GET_EVENT_HANDLE_INFO_GET_EVENT_KEY_EVENT_TYPE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_event_key.event_type = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter event_key after event_handle_info_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_frwrd_mact_event_handle_info_get(
          unit,
          &prm_event_key,
          &prm_handle_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_mact_event_handle_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_event_handle_info_get");   
    goto exit; 
  } 

  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO_print(&prm_handle_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
*  Function handler: ui_ppd_api_frwrd_mact_fid_aging_profile_set (section frwrd_mact)
 */
int 
  ui_ppd_api_frwrd_mact_fid_aging_profile_set(
    CURRENT_LINE *current_line 
  ) 
{   
   uint32 
    ret;   
  uint32   
    prm_mac_learn_profile_ndx;
  uint32   
    prm_fid_aging_profile;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_mact"); 
  soc_sand_proc_name = "soc_ppd_frwrd_mact_fid_aging_profile_set"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_FID_AGING_PROFILE_SET_FID_AGING_PROFILE_SET_MAC_LEARN_PROFILE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_FID_AGING_PROFILE_SET_FID_AGING_PROFILE_SET_MAC_LEARN_PROFILE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_mac_learn_profile_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter mac_learn_profile_ndx after fid_aging_profile_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_frwrd_mact_fid_aging_profile_get(
          unit,
          prm_mac_learn_profile_ndx,
          &prm_fid_aging_profile
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_mact_fid_aging_profile_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_fid_aging_profile_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_FID_AGING_PROFILE_SET_FID_AGING_PROFILE_SET_FID_AGING_PROFILE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_FID_AGING_PROFILE_SET_FID_AGING_PROFILE_SET_FID_AGING_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fid_aging_profile = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_frwrd_mact_fid_aging_profile_set(
          unit,
          prm_mac_learn_profile_ndx,
          prm_fid_aging_profile
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_mact_fid_aging_profile_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_fid_aging_profile_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
}

/******************************************************************** 
*  Function handler: ui_ppd_api_frwrd_mact_fid_aging_profile_get (section frwrd_mact)
 */
int 
  ui_ppd_api_frwrd_mact_fid_aging_profile_get(
    CURRENT_LINE *current_line 
  ) 
{ 
   uint32 
    ret;   
  uint32   
    prm_mac_learn_profile_ndx;
  uint32   
    prm_fid_aging_profile;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_mact"); 
  soc_sand_proc_name = "soc_ppd_frwrd_mact_fid_aging_profile_get"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_FID_AGING_PROFILE_GET_FID_AGING_PROFILE_GET_MAC_LEARN_PROFILE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_FID_AGING_PROFILE_GET_FID_AGING_PROFILE_GET_MAC_LEARN_PROFILE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_mac_learn_profile_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter mac_learn_profile_ndx after fid_aging_profile_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_frwrd_mact_fid_aging_profile_get(
          unit,
          prm_mac_learn_profile_ndx,
          &prm_fid_aging_profile
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_mact_fid_aging_profile_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_fid_aging_profile_get");   
    goto exit; 
  } 

  cli_out("event_handle_profile: %u\n\r",prm_fid_aging_profile);

  
  goto exit; 
exit: 
  return ui_ret;  
}

/******************************************************************** 
 *  Function handler: learn_msgs_distribution_info_set (section frwrd_mact)
 */
int 
  ui_ppd_api_frwrd_mact_learn_msgs_distribution_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32 
    prm_header_index = 0xFFFFFFFF;  
  SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO   
    prm_distribution_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_mact"); 
  soc_sand_proc_name = "soc_ppd_frwrd_mact_learn_msgs_distribution_info_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO_clear(&prm_distribution_info);
 
  /* Get parameters */ 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_frwrd_mact_learn_msgs_distribution_info_get(
          unit,
          &prm_distribution_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_mact_learn_msgs_distribution_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_learn_msgs_distribution_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_SET_LEARN_MSGS_DISTRIBUTION_INFO_SET_DISTRIBUTION_INFO_HEADER_INDEX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_SET_LEARN_MSGS_DISTRIBUTION_INFO_SET_DISTRIBUTION_INFO_HEADER_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_header_index = (uint32)param_val->value.ulong_value;
  } 

  if(prm_header_index != 0xFFFFFFFF)          
  { 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_SET_LEARN_MSGS_DISTRIBUTION_INFO_SET_DISTRIBUTION_INFO_HEADER_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_SET_LEARN_MSGS_DISTRIBUTION_INFO_SET_DISTRIBUTION_INFO_HEADER_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_distribution_info.header[ prm_header_index] = (uint32)param_val->value.ulong_value;
  } 

  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_SET_LEARN_MSGS_DISTRIBUTION_INFO_SET_DISTRIBUTION_INFO_HEADER_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_SET_LEARN_MSGS_DISTRIBUTION_INFO_SET_DISTRIBUTION_INFO_HEADER_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_distribution_info.header_type = param_val->numeric_equivalent;
  } 


  /* Call function */
  ret = soc_ppd_frwrd_mact_learn_msgs_distribution_info_set(
          unit,
          &prm_distribution_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_mact_learn_msgs_distribution_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_learn_msgs_distribution_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: learn_msgs_distribution_info_get (section frwrd_mact)
 */
int 
  ui_ppd_api_frwrd_mact_learn_msgs_distribution_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO   
    prm_distribution_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_mact"); 
  soc_sand_proc_name = "soc_ppd_frwrd_mact_learn_msgs_distribution_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO_clear(&prm_distribution_info);
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_ppd_frwrd_mact_learn_msgs_distribution_info_get(
          unit,
          &prm_distribution_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_mact_learn_msgs_distribution_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_learn_msgs_distribution_info_get");   
    goto exit; 
  } 

  SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO_print(&prm_distribution_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: shadow_msgs_distribution_info_set (section frwrd_mact)
 */
int 
  ui_ppd_api_frwrd_mact_shadow_msgs_distribution_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32 
    prm_header_index = 0xFFFFFFFF;  
  SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO   
    prm_distribution_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_mact"); 
  soc_sand_proc_name = "soc_ppd_frwrd_mact_shadow_msgs_distribution_info_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO_clear(&prm_distribution_info);
 
  /* Get parameters */ 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_frwrd_mact_shadow_msgs_distribution_info_get(
          unit,
          &prm_distribution_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_mact_shadow_msgs_distribution_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_shadow_msgs_distribution_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_SET_SHADOW_MSGS_DISTRIBUTION_INFO_SET_DISTRIBUTION_INFO_HEADER_INDEX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_SET_SHADOW_MSGS_DISTRIBUTION_INFO_SET_DISTRIBUTION_INFO_HEADER_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_header_index = (uint32)param_val->value.ulong_value;
  } 

  if(prm_header_index != 0xFFFFFFFF)          
  { 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_SET_SHADOW_MSGS_DISTRIBUTION_INFO_SET_DISTRIBUTION_INFO_HEADER_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_SET_SHADOW_MSGS_DISTRIBUTION_INFO_SET_DISTRIBUTION_INFO_HEADER_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_distribution_info.header[ prm_header_index] = (uint32)param_val->value.ulong_value;
  } 

  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_SET_SHADOW_MSGS_DISTRIBUTION_INFO_SET_DISTRIBUTION_INFO_HEADER_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_SET_SHADOW_MSGS_DISTRIBUTION_INFO_SET_DISTRIBUTION_INFO_HEADER_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_distribution_info.header_type = param_val->numeric_equivalent;
  } 


  /* Call function */
  ret = soc_ppd_frwrd_mact_shadow_msgs_distribution_info_set(
          unit,
          &prm_distribution_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_mact_shadow_msgs_distribution_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_shadow_msgs_distribution_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: shadow_msgs_distribution_info_get (section frwrd_mact)
 */
int 
  ui_ppd_api_frwrd_mact_shadow_msgs_distribution_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO   
    prm_distribution_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_mact"); 
  soc_sand_proc_name = "soc_ppd_frwrd_mact_shadow_msgs_distribution_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO_clear(&prm_distribution_info);
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_ppd_frwrd_mact_shadow_msgs_distribution_info_get(
          unit,
          &prm_distribution_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_mact_shadow_msgs_distribution_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_shadow_msgs_distribution_info_get");   
    goto exit; 
  } 

  SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO_print(&prm_distribution_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: mac_limit_exceeded_info_get (section frwrd_mact)
 */
int 
  ui_ppd_api_frwrd_mact_mac_limit_exceeded_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO   
    prm_exceed_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_mact"); 
  soc_sand_proc_name = "soc_ppd_frwrd_mact_mac_limit_exceeded_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_clear(&prm_exceed_info);
 
  /* Call function */
  ret = soc_ppd_frwrd_mact_mac_limit_exceeded_info_get(
          unit,
          &prm_exceed_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_mact_mac_limit_exceeded_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_mac_limit_exceeded_info_get");   
    goto exit; 
  } 

  SOC_PPC_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_print(&prm_exceed_info);

  goto exit; 
exit: 
  return ui_ret; 
} 
  
/********************************************************************
*  Function handler: port_info_set (section frwrd_mact)
 */
int
ui_ppd_api_frwrd_mact_port_info_set(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_PPC_PORT   
    prm_local_port_ndx;
  SOC_PPC_FRWRD_MACT_PORT_INFO
    prm_port_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_mact");
  soc_sand_proc_name = "soc_ppd_frwrd_mact_port_info_set";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_FRWRD_MACT_PORT_INFO_clear(&prm_port_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_PORT_INFO_SET_PORT_INFO_SET_LOCAL_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_PORT_INFO_SET_PORT_INFO_SET_LOCAL_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_local_port_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter local_port_ndx after port_info_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = soc_ppd_frwrd_mact_port_info_get(
    unit,
    SOC_CORE_DEFAULT,
    prm_local_port_ndx,
    &prm_port_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_mact_port_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_port_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_SA_UNKNOWN_ACTION_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_SA_UNKNOWN_ACTION_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_info.sa_unknown_action_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_DA_UNKNOWN_ACTION_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_DA_UNKNOWN_ACTION_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_info.da_unknown_action_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_SA_KNOWN_ACTION_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_SA_KNOWN_ACTION_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_info.sa_drop_action_profile = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_frwrd_mact_port_info_set(
    unit,
    SOC_CORE_DEFAULT,
    prm_local_port_ndx,
    &prm_port_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_mact_port_info_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_port_info_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: port_info_get (section frwrd_mact)
 */
int
ui_ppd_api_frwrd_mact_port_info_get(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_PPC_PORT   
    prm_local_port_ndx;
  SOC_PPC_FRWRD_MACT_PORT_INFO
    prm_port_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_mact");
  soc_sand_proc_name = "soc_ppd_frwrd_mact_port_info_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_FRWRD_MACT_PORT_INFO_clear(&prm_port_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_PORT_INFO_GET_PORT_INFO_GET_LOCAL_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_PORT_INFO_GET_PORT_INFO_GET_LOCAL_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_local_port_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter local_port_ndx after port_info_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_frwrd_mact_port_info_get(
    unit,
    SOC_CORE_DEFAULT,
    prm_local_port_ndx,
    &prm_port_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_mact_port_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_port_info_get");
    goto exit;
  }

  send_string_to_screen("--> port_info:", TRUE);
  SOC_PPC_FRWRD_MACT_PORT_INFO_print(&prm_port_info);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: trap_info_set (section frwrd_mact)
 */
int
ui_ppd_api_frwrd_mact_trap_info_set(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_PPC_FRWRD_MACT_TRAP_TYPE
    prm_trap_type_ndx;
  uint32
    prm_port_profile_ndx;
  SOC_PPC_ACTION_PROFILE
    prm_action_profile;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_mact");
  soc_sand_proc_name = "soc_ppd_frwrd_mact_trap_info_set";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_ACTION_PROFILE_clear(&prm_action_profile);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_TRAP_INFO_SET_TRAP_INFO_SET_TRAP_TYPE_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_MACT_TRAP_INFO_SET_TRAP_INFO_SET_TRAP_TYPE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_trap_type_ndx = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter trap_type_ndx after trap_info_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_TRAP_INFO_SET_TRAP_INFO_SET_PORT_PROFILE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_TRAP_INFO_SET_TRAP_INFO_SET_PORT_PROFILE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_profile_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter port_profile_ndx after trap_info_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = soc_ppd_frwrd_mact_trap_info_get(
    unit,
    prm_trap_type_ndx,
    prm_port_profile_ndx,
    &prm_action_profile
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_mact_trap_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_trap_info_get");
    goto exit;
  }

  /*trap-code
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_TRAP_INFO_SET_TRAP_INFO_SET_ACTION_PROFILE_TRAP_CODE_LSB_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_MACT_TRAP_INFO_SET_TRAP_INFO_SET_ACTION_PROFILE_TRAP_CODE_LSB_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_action_profile.trap_code = (SOC_PPC_TRAP_CODE)param_val->numeric_equivalent;
  }
  */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_TRAP_INFO_SET_TRAP_INFO_SET_ACTION_PROFILE_TRAP_CODE_LSB_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_TRAP_INFO_SET_TRAP_INFO_SET_ACTION_PROFILE_TRAP_CODE_LSB_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_action_profile.trap_code = (SOC_PPC_TRAP_CODE)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_TRAP_INFO_SET_TRAP_INFO_SET_ACTION_PROFILE_FRWRD_STRENGTH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_TRAP_INFO_SET_TRAP_INFO_SET_ACTION_PROFILE_FRWRD_STRENGTH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_action_profile.frwrd_action_strength = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_TRAP_INFO_SET_TRAP_INFO_SET_ACTION_PROFILE_SNOOP_STRENGTH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_TRAP_INFO_SET_TRAP_INFO_SET_ACTION_PROFILE_SNOOP_STRENGTH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_action_profile.snoop_action_strength = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_frwrd_mact_trap_info_set(
    unit,
    prm_trap_type_ndx,
    prm_port_profile_ndx,
    &prm_action_profile
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_mact_trap_info_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_trap_info_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: trap_info_get (section frwrd_mact)
 */
int
ui_ppd_api_frwrd_mact_trap_info_get(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_PPC_FRWRD_MACT_TRAP_TYPE
    prm_trap_type_ndx;
  uint32
    prm_port_profile_ndx;
  SOC_PPC_ACTION_PROFILE
    prm_action_profile;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_mact");
  soc_sand_proc_name = "soc_ppd_frwrd_mact_trap_info_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_ACTION_PROFILE_clear(&prm_action_profile);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_TRAP_INFO_GET_TRAP_INFO_GET_TRAP_TYPE_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_MACT_TRAP_INFO_GET_TRAP_INFO_GET_TRAP_TYPE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_trap_type_ndx = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter trap_type_ndx after trap_info_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_TRAP_INFO_GET_TRAP_INFO_GET_PORT_PROFILE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_TRAP_INFO_GET_TRAP_INFO_GET_PORT_PROFILE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_profile_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter port_profile_ndx after trap_info_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_frwrd_mact_trap_info_get(
    unit,
    prm_trap_type_ndx,
    prm_port_profile_ndx,
    &prm_action_profile
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_mact_trap_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_trap_info_get");
    goto exit;
  }

  send_string_to_screen("--> action_profile:", TRUE);
  SOC_PPC_ACTION_PROFILE_print(&prm_action_profile);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: ip_compatible_mc_info_set (section frwrd_mact)
 */
int
ui_ppd_api_frwrd_mact_ip_compatible_mc_info_set(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_mact");
  soc_sand_proc_name = "soc_ppd_frwrd_mact_ip_compatible_mc_info_set";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_clear(&prm_info);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_ppd_frwrd_mact_ip_compatible_mc_info_get(
    unit,
    &prm_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_mact_ip_compatible_mc_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_ip_compatible_mc_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_SET_IP_COMPATIBLE_MC_INFO_SET_INFO_MASK_FID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_SET_IP_COMPATIBLE_MC_INFO_SET_INFO_MASK_FID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.mask_fid = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_SET_IP_COMPATIBLE_MC_INFO_SET_INFO_ENABLE_IPV4_MC_COMPATIBLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_SET_IP_COMPATIBLE_MC_INFO_SET_INFO_ENABLE_IPV4_MC_COMPATIBLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.enable_ipv4_mc_compatible = (uint8)param_val->value.ulong_value;
  } 
  /* Call function */
  ret = soc_ppd_frwrd_mact_ip_compatible_mc_info_set(
    unit,
    &prm_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_mact_ip_compatible_mc_info_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_ip_compatible_mc_info_set");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_print(&prm_info);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: ip_compatible_mc_info_get (section frwrd_mact)
 */
int
ui_ppd_api_frwrd_mact_ip_compatible_mc_info_get(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_mact");
  soc_sand_proc_name = "soc_ppd_frwrd_mact_ip_compatible_mc_info_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_clear(&prm_info);

  /* Get parameters */

  /* Call function */
  ret = soc_ppd_frwrd_mact_ip_compatible_mc_info_get(
    unit,
    &prm_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_mact_ip_compatible_mc_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_ip_compatible_mc_info_get");
    goto exit;
  }

  send_string_to_screen("--> info:", TRUE);
  SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_print(&prm_info);

  goto exit;
exit:
  return ui_ret;
}

/******************************************************************** 
 *  Function handler: frwrd_mact_event_parse (section frwrd_mact_mgmt)
 */
int 
  ui_ppd_api_frwrd_mact_mgmt_frwrd_mact_event_parse(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32 
    prm_buff_index = 0xFFFFFFFF;  
  SOC_PPC_FRWRD_MACT_EVENT_BUFFER   
    prm_event_buf;
  SOC_PPC_FRWRD_MACT_EVENT_INFO   
    prm_mact_event;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_mact_mgmt"); 
  soc_sand_proc_name = "soc_ppd_frwrd_mact_event_parse"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_MACT_EVENT_BUFFER_clear(&prm_event_buf);
  SOC_PPC_FRWRD_MACT_EVENT_INFO_clear(&prm_mact_event);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_EVENT_PARSE_FRWRD_MACT_EVENT_PARSE_EVENT_BUF_BUFF_LEN_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_EVENT_PARSE_FRWRD_MACT_EVENT_PARSE_EVENT_BUF_BUFF_LEN_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_event_buf.buff_len = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_EVENT_PARSE_FRWRD_MACT_EVENT_PARSE_EVENT_BUF_BUFF_INDEX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_EVENT_PARSE_FRWRD_MACT_EVENT_PARSE_EVENT_BUF_BUFF_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_buff_index = (uint32)param_val->value.ulong_value;
  } 

  if(prm_buff_index != 0xFFFFFFFF)          
  { 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_EVENT_PARSE_FRWRD_MACT_EVENT_PARSE_EVENT_BUF_BUFF_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_EVENT_PARSE_FRWRD_MACT_EVENT_PARSE_EVENT_BUF_BUFF_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_event_buf.buff[ prm_buff_index] = (uint32)param_val->value.ulong_value;
  } 

  }   


  /* Call function */
  ret = soc_ppd_frwrd_mact_event_parse(
          unit,
          &prm_event_buf,
          &prm_mact_event
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_mact_event_parse - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_event_parse");   
    goto exit; 
  } 

  SOC_PPC_FRWRD_MACT_EVENT_INFO_print(&prm_mact_event);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
#endif
#ifdef UI_FRWRD_MACT_MGMT/* { frwrd_mact_mgmt*/
/******************************************************************** 
*  Section handler: frwrd_mact
 */
int
  ui_ppd_api_frwrd_mact_mgmt(
                           CURRENT_LINE *current_line
                           )
{
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_mact");

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_PPD_FRWRD_MACT_LOOKUP_TYPE_SET_PPD_FRWRD_MACT_LOOKUP_TYPE_SET_ID,1))
  {
    ui_ret = ui_ppd_frwrd_mact_lookup_type_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_OPER_MODE_INFO_SET_OPER_MODE_INFO_SET_ID,1))
  {
    ui_ret = ui_ppd_api_frwrd_mact_oper_mode_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_OPER_MODE_INFO_GET_OPER_MODE_INFO_GET_ID,1))
  {
    ui_ret = ui_ppd_api_frwrd_mact_oper_mode_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_AGING_INFO_SET_AGING_INFO_SET_ID,1))
  {
    ui_ret = ui_ppd_api_frwrd_mact_aging_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_AGING_INFO_GET_AGING_INFO_GET_ID,1))
  {
    ui_ret = ui_ppd_api_frwrd_mact_aging_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_SET_AGING_EVENTS_HANDLE_INFO_SET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_frwrd_mact_aging_events_handle_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_GET_AGING_EVENTS_HANDLE_INFO_GET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_frwrd_mact_aging_events_handle_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_SET_FID_PROFILE_TO_FID_MAP_SET_ID,1)) 
  {
    ui_ret = ui_ppd_api_frwrd_mact_fid_profile_to_fid_map_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_GET_FID_PROFILE_TO_FID_MAP_GET_ID,1)) 
  {
    ui_ret = ui_ppd_api_frwrd_mact_fid_profile_to_fid_map_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_SET_MAC_LIMIT_INFO_SET_ID,1))
  {
    ui_ret = ui_ppd_api_frwrd_mact_mac_limit_glbl_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_MAC_LIMIT_INFO_GET_MAC_LIMIT_INFO_GET_ID,1))
  {
    ui_ret = ui_ppd_api_frwrd_mact_mac_limit_glbl_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_SET_LIMIT_PROFILE_INFO_SET_ID,1))
  {
    ui_ret = ui_ppd_api_frwrd_mact_learn_profile_limit_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_GET_LIMIT_PROFILE_INFO_GET_ID,1))
  {
    ui_ret = ui_ppd_api_frwrd_mact_learn_profile_limit_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_EVENT_HANDLE_PROFILE_SET_EVENT_HANDLE_PROFILE_SET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_frwrd_mact_event_handle_profile_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_EVENT_HANDLE_PROFILE_GET_EVENT_HANDLE_PROFILE_GET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_frwrd_mact_event_handle_profile_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_EVENT_GET_EVENT_GET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_frwrd_mact_event_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_EVENT_HANDLE_INFO_SET_EVENT_HANDLE_INFO_SET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_frwrd_mact_event_handle_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_EVENT_HANDLE_INFO_GET_EVENT_HANDLE_INFO_GET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_frwrd_mact_event_handle_info_get(current_line); 
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_FID_AGING_PROFILE_SET_FID_AGING_PROFILE_SET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_frwrd_mact_fid_aging_profile_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_FID_AGING_PROFILE_GET_FID_AGING_PROFILE_GET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_frwrd_mact_fid_aging_profile_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_SET_LEARN_MSGS_DISTRIBUTION_INFO_SET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_frwrd_mact_learn_msgs_distribution_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_GET_LEARN_MSGS_DISTRIBUTION_INFO_GET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_frwrd_mact_learn_msgs_distribution_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_SET_SHADOW_MSGS_DISTRIBUTION_INFO_SET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_frwrd_mact_shadow_msgs_distribution_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_GET_SHADOW_MSGS_DISTRIBUTION_INFO_GET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_frwrd_mact_shadow_msgs_distribution_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_GET_MAC_LIMIT_EXCEEDED_INFO_GET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_frwrd_mact_mac_limit_exceeded_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_PORT_INFO_SET_PORT_INFO_SET_ID,1))
  {
    ui_ret = ui_ppd_api_frwrd_mact_port_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_PORT_INFO_GET_PORT_INFO_GET_ID,1))
  {
    ui_ret = ui_ppd_api_frwrd_mact_port_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_TRAP_INFO_SET_TRAP_INFO_SET_ID,1))
  {
    ui_ret = ui_ppd_api_frwrd_mact_trap_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_TRAP_INFO_GET_TRAP_INFO_GET_ID,1))
  {
    ui_ret = ui_ppd_api_frwrd_mact_trap_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_SET_IP_COMPATIBLE_MC_INFO_SET_ID,1))
  {
    ui_ret = ui_ppd_api_frwrd_mact_ip_compatible_mc_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_GET_IP_COMPATIBLE_MC_INFO_GET_ID,1))
  {
    ui_ret = ui_ppd_api_frwrd_mact_ip_compatible_mc_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_EVENT_PARSE_FRWRD_MACT_EVENT_PARSE_ID,1)) 
  { 
    ui_ret = ui_ppd_api_frwrd_mact_mgmt_frwrd_mact_event_parse(current_line); 
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting function name after frwrd_mact***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
#endif /* frwrd_mact_mgmt */ 


#endif /* LINK_PPD_LIBRARIES */ 

