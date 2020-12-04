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
 


#include <soc/dpp/PPD/ppd_api_lif_cos.h>
#include <soc/dpp/PPD/ppd_api_lif_table.h>
#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_lif_cos.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_lif.h>

#include <appl/diag/diag_alloc.h>

#if LINK_PPD_LIBRARIES

#ifdef UI_LIF_COS
/********************************************************************
*  Function handler: ac_profile_info_set (section lif_cos)
 */
int
ui_ppd_api_lif_cos_ac_profile_info_set(
                                       CURRENT_LINE *current_line
                                       )
{
  uint32
    ret;
  uint32
    prm_profile_ndx;
  SOC_PPC_LIF_COS_AC_PROFILE_INFO
    prm_profile_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif_cos");
  soc_sand_proc_name = "soc_ppd_lif_cos_ac_profile_info_set";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_LIF_COS_AC_PROFILE_INFO_clear(&prm_profile_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_AC_PROFILE_INFO_SET_AC_PROFILE_INFO_SET_PROFILE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_AC_PROFILE_INFO_SET_AC_PROFILE_INFO_SET_PROFILE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_profile_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter profile_ndx after ac_profile_info_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = soc_ppd_lif_cos_ac_profile_info_get(
    unit,
    prm_profile_ndx,
    &prm_profile_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_cos_ac_profile_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_cos_ac_profile_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_AC_PROFILE_INFO_SET_AC_PROFILE_INFO_SET_PROFILE_INFO_MAP_TABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_AC_PROFILE_INFO_SET_AC_PROFILE_INFO_SET_PROFILE_INFO_MAP_TABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_profile_info.map_table = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_AC_PROFILE_INFO_SET_AC_PROFILE_INFO_SET_PROFILE_INFO_DP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_AC_PROFILE_INFO_SET_AC_PROFILE_INFO_SET_PROFILE_INFO_DP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_profile_info.dp = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_AC_PROFILE_INFO_SET_AC_PROFILE_INFO_SET_PROFILE_INFO_TC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_AC_PROFILE_INFO_SET_AC_PROFILE_INFO_SET_PROFILE_INFO_TC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_profile_info.tc = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_AC_PROFILE_INFO_SET_AC_PROFILE_INFO_SET_PROFILE_INFO_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LIF_COS_AC_PROFILE_INFO_SET_AC_PROFILE_INFO_SET_PROFILE_INFO_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_profile_info.type = param_val->numeric_equivalent;
  }

  /* Call function */
  ret = soc_ppd_lif_cos_ac_profile_info_set(
    unit,
    prm_profile_ndx,
    &prm_profile_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_cos_ac_profile_info_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_cos_ac_profile_info_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: ac_profile_info_get (section lif_cos)
 */
int
ui_ppd_api_lif_cos_ac_profile_info_get(
                                       CURRENT_LINE *current_line
                                       )
{
  uint32
    ret;
  uint32
    prm_profile_ndx;
  SOC_PPC_LIF_COS_AC_PROFILE_INFO
    prm_profile_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif_cos");
  soc_sand_proc_name = "soc_ppd_lif_cos_ac_profile_info_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_LIF_COS_AC_PROFILE_INFO_clear(&prm_profile_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_AC_PROFILE_INFO_GET_AC_PROFILE_INFO_GET_PROFILE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_AC_PROFILE_INFO_GET_AC_PROFILE_INFO_GET_PROFILE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_profile_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter profile_ndx after ac_profile_info_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_lif_cos_ac_profile_info_get(
    unit,
    prm_profile_ndx,
    &prm_profile_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_cos_ac_profile_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_cos_ac_profile_info_get");
    goto exit;
  }

  send_string_to_screen("--> profile_info:", TRUE);
  SOC_PPC_LIF_COS_AC_PROFILE_INFO_print(&prm_profile_info);

  goto exit;
exit:
  return ui_ret;
}

/******************************************************************** 
 *  Function handler: profile_info_set (section lif_cos)
 */
int 
  ui_ppd_api_lif_cos_profile_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_cos_profile_ndx;
  SOC_PPC_LIF_COS_PROFILE_INFO   
    prm_profile_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif_cos"); 
  soc_sand_proc_name = "soc_ppd_lif_cos_profile_info_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_LIF_COS_PROFILE_INFO_clear(&prm_profile_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PROFILE_INFO_SET_PROFILE_INFO_SET_COS_PROFILE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_PROFILE_INFO_SET_PROFILE_INFO_SET_COS_PROFILE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_cos_profile_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter cos_profile_ndx after profile_info_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_lif_cos_profile_info_get(
          unit,
          prm_cos_profile_ndx,
          &prm_profile_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_lif_cos_profile_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_cos_profile_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PROFILE_INFO_SET_PROFILE_INFO_SET_PROFILE_INFO_FORCED_DP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_PROFILE_INFO_SET_PROFILE_INFO_SET_PROFILE_INFO_FORCED_DP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_profile_info.forced_dp = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PROFILE_INFO_SET_PROFILE_INFO_SET_PROFILE_INFO_FORCED_TC_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_PROFILE_INFO_SET_PROFILE_INFO_SET_PROFILE_INFO_FORCED_TC_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_profile_info.forced_tc = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PROFILE_INFO_SET_PROFILE_INFO_SET_PROFILE_INFO_MAP_FROM_TC_DP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_PROFILE_INFO_SET_PROFILE_INFO_SET_PROFILE_INFO_MAP_FROM_TC_DP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_profile_info.map_from_tc_dp = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PROFILE_INFO_SET_PROFILE_INFO_SET_PROFILE_INFO_MAP_WHEN_L2_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_PROFILE_INFO_SET_PROFILE_INFO_SET_PROFILE_INFO_MAP_WHEN_L2_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_profile_info.map_when_l2 = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PROFILE_INFO_SET_PROFILE_INFO_SET_PROFILE_INFO_MAP_WHEN_MPLS_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_PROFILE_INFO_SET_PROFILE_INFO_SET_PROFILE_INFO_MAP_WHEN_MPLS_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_profile_info.map_when_mpls = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PROFILE_INFO_SET_PROFILE_INFO_SET_PROFILE_INFO_MAP_WHEN_IP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_PROFILE_INFO_SET_PROFILE_INFO_SET_PROFILE_INFO_MAP_WHEN_IP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_profile_info.map_when_ip = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_lif_cos_profile_info_set(
          unit,
          prm_cos_profile_ndx,
          &prm_profile_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_lif_cos_profile_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_cos_profile_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: profile_info_get (section lif_cos)
 */
int 
  ui_ppd_api_lif_cos_profile_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_cos_profile_ndx;
  SOC_PPC_LIF_COS_PROFILE_INFO   
    prm_profile_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif_cos"); 
  soc_sand_proc_name = "soc_ppd_lif_cos_profile_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_LIF_COS_PROFILE_INFO_clear(&prm_profile_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PROFILE_INFO_GET_PROFILE_INFO_GET_COS_PROFILE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_PROFILE_INFO_GET_PROFILE_INFO_GET_COS_PROFILE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_cos_profile_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter cos_profile_ndx after profile_info_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_lif_cos_profile_info_get(
          unit,
          prm_cos_profile_ndx,
          &prm_profile_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_lif_cos_profile_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_cos_profile_info_get");   
    goto exit; 
  } 

  SOC_PPC_LIF_COS_PROFILE_INFO_print(&prm_profile_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/********************************************************************
*  Function handler: profile_map_l2_info_set (section lif_cos)
 */
int
ui_ppd_api_lif_cos_profile_map_l2_info_set(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_map_tbl_ndx;
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_L2_KEY
    prm_map_key;
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY
    prm_map_value;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif_cos");
  soc_sand_proc_name = "soc_ppd_lif_cos_profile_map_l2_info_set";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_L2_KEY_clear(&prm_map_key);
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY_clear(&prm_map_value);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_L2_INFO_SET_PROFILE_MAP_L2_INFO_SET_MAP_TBL_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_L2_INFO_SET_PROFILE_MAP_L2_INFO_SET_MAP_TBL_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_map_tbl_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter map_tbl_ndx after profile_map_l2_info_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_L2_INFO_SET_PROFILE_MAP_L2_INFO_SET_MAP_KEY_INCOMING_DEI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_L2_INFO_SET_PROFILE_MAP_L2_INFO_SET_MAP_KEY_INCOMING_DEI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_map_key.incoming_dei = (uint8)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_L2_INFO_SET_PROFILE_MAP_L2_INFO_SET_MAP_KEY_INCOMING_UP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_L2_INFO_SET_PROFILE_MAP_L2_INFO_SET_MAP_KEY_INCOMING_UP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_map_key.incoming_up = (uint8)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_L2_INFO_SET_PROFILE_MAP_L2_INFO_SET_MAP_KEY_OUTER_TPID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_L2_INFO_SET_PROFILE_MAP_L2_INFO_SET_MAP_KEY_OUTER_TPID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_map_key.outer_tpid = (uint8)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter map_key after profile_map_l2_info_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = soc_ppd_lif_cos_profile_map_l2_info_get(
    unit,
    prm_map_tbl_ndx,
    &prm_map_key,
    &prm_map_value
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_cos_profile_map_l2_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_cos_profile_map_l2_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_L2_INFO_SET_PROFILE_MAP_L2_INFO_SET_MAP_VALUE_DP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_L2_INFO_SET_PROFILE_MAP_L2_INFO_SET_MAP_VALUE_DP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_map_value.dp = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_L2_INFO_SET_PROFILE_MAP_L2_INFO_SET_MAP_VALUE_TC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_L2_INFO_SET_PROFILE_MAP_L2_INFO_SET_MAP_VALUE_TC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_map_value.tc = (uint8)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_lif_cos_profile_map_l2_info_set(
    unit,
    prm_map_tbl_ndx,
    &prm_map_key,
    &prm_map_value
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_cos_profile_map_l2_info_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_cos_profile_map_l2_info_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: profile_map_l2_info_get (section lif_cos)
 */
int
ui_ppd_api_lif_cos_profile_map_l2_info_get(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_map_tbl_ndx;
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_L2_KEY
    prm_map_key;
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY
    prm_map_value;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif_cos");
  soc_sand_proc_name = "soc_ppd_lif_cos_profile_map_l2_info_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_L2_KEY_clear(&prm_map_key);
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY_clear(&prm_map_value);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_L2_INFO_GET_PROFILE_MAP_L2_INFO_GET_MAP_TBL_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_L2_INFO_GET_PROFILE_MAP_L2_INFO_GET_MAP_TBL_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_map_tbl_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter map_tbl_ndx after profile_map_l2_info_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_L2_INFO_GET_PROFILE_MAP_L2_INFO_GET_MAP_KEY_INCOMING_DEI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_L2_INFO_GET_PROFILE_MAP_L2_INFO_GET_MAP_KEY_INCOMING_DEI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_map_key.incoming_dei = (uint8)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_L2_INFO_GET_PROFILE_MAP_L2_INFO_GET_MAP_KEY_INCOMING_UP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_L2_INFO_GET_PROFILE_MAP_L2_INFO_GET_MAP_KEY_INCOMING_UP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_map_key.incoming_up = (uint8)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_L2_INFO_GET_PROFILE_MAP_L2_INFO_GET_MAP_KEY_OUTER_TPID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_L2_INFO_GET_PROFILE_MAP_L2_INFO_GET_MAP_KEY_OUTER_TPID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_map_key.outer_tpid = (uint8)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter map_key after profile_map_l2_info_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_lif_cos_profile_map_l2_info_get(
    unit,
    prm_map_tbl_ndx,
    &prm_map_key,
    &prm_map_value
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_cos_profile_map_l2_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_cos_profile_map_l2_info_get");
    goto exit;
  }

  send_string_to_screen("--> map_value:", TRUE);
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY_print(&prm_map_value);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: profile_map_ip_info_set (section lif_cos)
 */
int
ui_ppd_api_lif_cos_profile_map_ip_info_set(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_map_tbl_ndx;
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_IP_KEY
    prm_map_key;
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY
    prm_map_value;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif_cos");
  soc_sand_proc_name = "soc_ppd_lif_cos_profile_map_ip_info_set";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_IP_KEY_clear(&prm_map_key);
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY_clear(&prm_map_value);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_IP_INFO_SET_PROFILE_MAP_IP_INFO_SET_MAP_TBL_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_IP_INFO_SET_PROFILE_MAP_IP_INFO_SET_MAP_TBL_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_map_tbl_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter map_tbl_ndx after profile_map_ip_info_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_IP_INFO_SET_PROFILE_MAP_IP_INFO_SET_MAP_KEY_DSCP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_IP_INFO_SET_PROFILE_MAP_IP_INFO_SET_MAP_KEY_DSCP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_map_key.tos = (uint8)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_IP_INFO_SET_PROFILE_MAP_IP_INFO_SET_MAP_KEY_IP_TYPE_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_IP_INFO_SET_PROFILE_MAP_IP_INFO_SET_MAP_KEY_IP_TYPE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_map_key.ip_type = param_val->numeric_equivalent;
  } 
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter map_key after profile_map_ip_info_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = soc_ppd_lif_cos_profile_map_ip_info_get(
    unit,
    prm_map_tbl_ndx,
    &prm_map_key,
    &prm_map_value
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_cos_profile_map_ip_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_cos_profile_map_ip_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_IP_INFO_SET_PROFILE_MAP_IP_INFO_SET_MAP_VALUE_DP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_IP_INFO_SET_PROFILE_MAP_IP_INFO_SET_MAP_VALUE_DP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_map_value.dp = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_IP_INFO_SET_PROFILE_MAP_IP_INFO_SET_MAP_VALUE_TC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_IP_INFO_SET_PROFILE_MAP_IP_INFO_SET_MAP_VALUE_TC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_map_value.tc = (uint8)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_lif_cos_profile_map_ip_info_set(
    unit,
    prm_map_tbl_ndx,
    &prm_map_key,
    &prm_map_value
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_cos_profile_map_ip_info_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_cos_profile_map_ip_info_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: profile_map_ip_info_get (section lif_cos)
 */
int
ui_ppd_api_lif_cos_profile_map_ip_info_get(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_map_tbl_ndx;
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_IP_KEY
    prm_map_key;
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY
    prm_map_value;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif_cos");
  soc_sand_proc_name = "soc_ppd_lif_cos_profile_map_ip_info_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_IP_KEY_clear(&prm_map_key);
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY_clear(&prm_map_value);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_IP_INFO_GET_PROFILE_MAP_IP_INFO_GET_MAP_TBL_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_IP_INFO_GET_PROFILE_MAP_IP_INFO_GET_MAP_TBL_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_map_tbl_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter map_tbl_ndx after profile_map_ip_info_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_IP_INFO_GET_PROFILE_MAP_IP_INFO_GET_MAP_KEY_DSCP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_IP_INFO_GET_PROFILE_MAP_IP_INFO_GET_MAP_KEY_DSCP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_map_key.tos = (uint8)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_IP_INFO_GET_PROFILE_MAP_IP_INFO_GET_MAP_KEY_IP_TYPE_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_IP_INFO_GET_PROFILE_MAP_IP_INFO_GET_MAP_KEY_IP_TYPE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_map_key.ip_type = param_val->numeric_equivalent;
  } 
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter map_key after profile_map_ip_info_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_lif_cos_profile_map_ip_info_get(
    unit,
    prm_map_tbl_ndx,
    &prm_map_key,
    &prm_map_value
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_cos_profile_map_ip_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_cos_profile_map_ip_info_get");
    goto exit;
  }

  send_string_to_screen("--> map_value:", TRUE);
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY_print(&prm_map_value);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: pwe_profile_info_set (section lif_cos)
 */
int
ui_ppd_api_lif_cos_pwe_profile_info_set(
                                        CURRENT_LINE *current_line
                                        )
{
  uint32
    ret;
  uint32
    prm_profile_ndx;
  SOC_PPC_LIF_COS_PWE_PROFILE_INFO
    prm_profile_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif_cos");
  soc_sand_proc_name = "soc_ppd_lif_cos_pwe_profile_info_set";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_LIF_COS_PWE_PROFILE_INFO_clear(&prm_profile_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PWE_PROFILE_INFO_SET_PWE_PROFILE_INFO_SET_PROFILE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_PWE_PROFILE_INFO_SET_PWE_PROFILE_INFO_SET_PROFILE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_profile_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter profile_ndx after pwe_profile_info_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = soc_ppd_lif_cos_pwe_profile_info_get(
    unit,
    prm_profile_ndx,
    &prm_profile_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_cos_pwe_profile_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_cos_pwe_profile_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PWE_PROFILE_INFO_SET_PWE_PROFILE_INFO_SET_PROFILE_INFO_MAP_TABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_PWE_PROFILE_INFO_SET_PWE_PROFILE_INFO_SET_PROFILE_INFO_MAP_TABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_profile_info.map_table = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PWE_PROFILE_INFO_SET_PWE_PROFILE_INFO_SET_PROFILE_INFO_DP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_PWE_PROFILE_INFO_SET_PWE_PROFILE_INFO_SET_PROFILE_INFO_DP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_profile_info.dp = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PWE_PROFILE_INFO_SET_PWE_PROFILE_INFO_SET_PROFILE_INFO_TC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_PWE_PROFILE_INFO_SET_PWE_PROFILE_INFO_SET_PROFILE_INFO_TC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_profile_info.tc = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PWE_PROFILE_INFO_SET_PWE_PROFILE_INFO_SET_PROFILE_INFO_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LIF_COS_PWE_PROFILE_INFO_SET_PWE_PROFILE_INFO_SET_PROFILE_INFO_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_profile_info.type = param_val->numeric_equivalent;
  }

  /* Call function */
  ret = soc_ppd_lif_cos_pwe_profile_info_set(
    unit,
    prm_profile_ndx,
    &prm_profile_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_cos_pwe_profile_info_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_cos_pwe_profile_info_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: pwe_profile_info_get (section lif_cos)
 */
int
ui_ppd_api_lif_cos_pwe_profile_info_get(
                                        CURRENT_LINE *current_line
                                        )
{
  uint32
    ret;
  uint32
    prm_profile_ndx;
  SOC_PPC_LIF_COS_PWE_PROFILE_INFO
    prm_profile_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif_cos");
  soc_sand_proc_name = "soc_ppd_lif_cos_pwe_profile_info_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_LIF_COS_PWE_PROFILE_INFO_clear(&prm_profile_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PWE_PROFILE_INFO_GET_PWE_PROFILE_INFO_GET_PROFILE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_PWE_PROFILE_INFO_GET_PWE_PROFILE_INFO_GET_PROFILE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_profile_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter profile_ndx after pwe_profile_info_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_lif_cos_pwe_profile_info_get(
    unit,
    prm_profile_ndx,
    &prm_profile_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_cos_pwe_profile_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_cos_pwe_profile_info_get");
    goto exit;
  }

  send_string_to_screen("--> profile_info:", TRUE);
  SOC_PPC_LIF_COS_PWE_PROFILE_INFO_print(&prm_profile_info);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: profile_map_mpls_label_info_set (section lif_cos)
 */
int
ui_ppd_api_lif_cos_profile_map_mpls_label_info_set(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_map_tbl_ndx;
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY
    prm_map_key;
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY
    prm_map_value;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif_cos");
  soc_sand_proc_name = "soc_ppd_lif_cos_profile_map_mpls_label_info_set";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY_clear(&prm_map_key);
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY_clear(&prm_map_value);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_SET_PROFILE_MAP_MPLS_LABEL_INFO_SET_MAP_TBL_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_SET_PROFILE_MAP_MPLS_LABEL_INFO_SET_MAP_TBL_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_map_tbl_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter map_tbl_ndx after profile_map_mpls_label_info_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_SET_PROFILE_MAP_MPLS_LABEL_INFO_SET_MAP_KEY_IN_EXP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_SET_PROFILE_MAP_MPLS_LABEL_INFO_SET_MAP_KEY_IN_EXP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_map_key.in_exp = (uint8)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter map_key after profile_map_mpls_label_info_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = soc_ppd_lif_cos_profile_map_mpls_label_info_get(
    unit,
    prm_map_tbl_ndx,
    &prm_map_key,
    &prm_map_value
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_cos_profile_map_mpls_label_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_cos_profile_map_mpls_label_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_SET_PROFILE_MAP_MPLS_LABEL_INFO_SET_MAP_VALUE_DP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_SET_PROFILE_MAP_MPLS_LABEL_INFO_SET_MAP_VALUE_DP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_map_value.dp = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_SET_PROFILE_MAP_MPLS_LABEL_INFO_SET_MAP_VALUE_TC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_SET_PROFILE_MAP_MPLS_LABEL_INFO_SET_MAP_VALUE_TC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_map_value.tc = (uint8)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_lif_cos_profile_map_mpls_label_info_set(
    unit,
    prm_map_tbl_ndx,
    &prm_map_key,
    &prm_map_value
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_cos_profile_map_mpls_label_info_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_cos_profile_map_mpls_label_info_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: profile_map_mpls_label_info_get (section lif_cos)
 */
int
ui_ppd_api_lif_cos_profile_map_mpls_label_info_get(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_map_tbl_ndx;
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY
    prm_map_key;
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY
    prm_map_value;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif_cos");
  soc_sand_proc_name = "soc_ppd_lif_cos_profile_map_mpls_label_info_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY_clear(&prm_map_key);
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY_clear(&prm_map_value);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_GET_PROFILE_MAP_MPLS_LABEL_INFO_GET_MAP_TBL_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_GET_PROFILE_MAP_MPLS_LABEL_INFO_GET_MAP_TBL_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_map_tbl_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter map_tbl_ndx after profile_map_mpls_label_info_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_GET_PROFILE_MAP_MPLS_LABEL_INFO_GET_MAP_KEY_IN_EXP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_GET_PROFILE_MAP_MPLS_LABEL_INFO_GET_MAP_KEY_IN_EXP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_map_key.in_exp = (uint8)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter map_key after profile_map_mpls_label_info_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_lif_cos_profile_map_mpls_label_info_get(
    unit,
    prm_map_tbl_ndx,
    &prm_map_key,
    &prm_map_value
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_cos_profile_map_mpls_label_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_cos_profile_map_mpls_label_info_get");
    goto exit;
  }

  send_string_to_screen("--> map_value:", TRUE);
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY_print(&prm_map_value);

  goto exit;
exit:
  return ui_ret;
}

/******************************************************************** 
 *  Function handler: profile_map_tc_dp_info_set (section lif_cos)
 */
int 
  ui_ppd_api_lif_cos_profile_map_tc_dp_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_map_tbl_ndx;
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY   
    prm_map_key;
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY   
    prm_map_value;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif_cos"); 
  soc_sand_proc_name = "soc_ppd_lif_cos_profile_map_tc_dp_info_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY_clear(&prm_map_key);
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY_clear(&prm_map_value);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_TC_DP_INFO_SET_PROFILE_MAP_TC_DP_INFO_SET_MAP_TBL_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_TC_DP_INFO_SET_PROFILE_MAP_TC_DP_INFO_SET_MAP_TBL_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_map_tbl_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter map_tbl_ndx after profile_map_tc_dp_info_set***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_TC_DP_INFO_SET_PROFILE_MAP_TC_DP_INFO_SET_MAP_KEY_DP_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_TC_DP_INFO_SET_PROFILE_MAP_TC_DP_INFO_SET_MAP_KEY_DP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_map_key.dp = (uint8)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_TC_DP_INFO_SET_PROFILE_MAP_TC_DP_INFO_SET_MAP_KEY_TC_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_TC_DP_INFO_SET_PROFILE_MAP_TC_DP_INFO_SET_MAP_KEY_TC_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_map_key.tc = (uint8)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter map_key after profile_map_tc_dp_info_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_lif_cos_profile_map_tc_dp_info_get(
          unit,
          prm_map_tbl_ndx,
          &prm_map_key,
          &prm_map_value
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_lif_cos_profile_map_tc_dp_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_cos_profile_map_tc_dp_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_TC_DP_INFO_SET_PROFILE_MAP_TC_DP_INFO_SET_MAP_VALUE_DP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_TC_DP_INFO_SET_PROFILE_MAP_TC_DP_INFO_SET_MAP_VALUE_DP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_map_value.dp = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_TC_DP_INFO_SET_PROFILE_MAP_TC_DP_INFO_SET_MAP_VALUE_TC_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_TC_DP_INFO_SET_PROFILE_MAP_TC_DP_INFO_SET_MAP_VALUE_TC_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_map_value.tc = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_lif_cos_profile_map_tc_dp_info_set(
          unit,
          prm_map_tbl_ndx,
          &prm_map_key,
          &prm_map_value
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_lif_cos_profile_map_tc_dp_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_cos_profile_map_tc_dp_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: profile_map_tc_dp_info_get (section lif_cos)
 */
int 
  ui_ppd_api_lif_cos_profile_map_tc_dp_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_map_tbl_ndx;
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY   
    prm_map_key;
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY   
    prm_map_value;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif_cos"); 
  soc_sand_proc_name = "soc_ppd_lif_cos_profile_map_tc_dp_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY_clear(&prm_map_key);
  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY_clear(&prm_map_value);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_TC_DP_INFO_GET_PROFILE_MAP_TC_DP_INFO_GET_MAP_TBL_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_TC_DP_INFO_GET_PROFILE_MAP_TC_DP_INFO_GET_MAP_TBL_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_map_tbl_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter map_tbl_ndx after profile_map_tc_dp_info_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_TC_DP_INFO_GET_PROFILE_MAP_TC_DP_INFO_GET_MAP_KEY_DP_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_TC_DP_INFO_GET_PROFILE_MAP_TC_DP_INFO_GET_MAP_KEY_DP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_map_key.dp = (uint8)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_TC_DP_INFO_GET_PROFILE_MAP_TC_DP_INFO_GET_MAP_KEY_TC_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_TC_DP_INFO_GET_PROFILE_MAP_TC_DP_INFO_GET_MAP_KEY_TC_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_map_key.tc = (uint8)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter map_key after profile_map_tc_dp_info_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_lif_cos_profile_map_tc_dp_info_get(
          unit,
          prm_map_tbl_ndx,
          &prm_map_key,
          &prm_map_value
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_lif_cos_profile_map_tc_dp_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_cos_profile_map_tc_dp_info_get");   
    goto exit; 
  } 

  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY_print(&prm_map_value);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
*  Function handler: opcode_types_set (section lif_cos)
 */
int
ui_ppd_api_lif_cos_opcode_types_set(
                                    CURRENT_LINE *current_line
                                    )
{
  uint32
    ret;
  uint32
    prm_opcode_ndx;
  SOC_PPC_LIF_COS_OPCODE_TYPE
    prm_opcode_type;
  uint32
    use_l3=0,use_tc_dp=0;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif_cos");
  soc_sand_proc_name = "soc_ppd_lif_cos_opcode_types_set";

  unit = soc_ppd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_OPCODE_TYPES_SET_OPCODE_TYPES_SET_OPCODE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_OPCODE_TYPES_SET_OPCODE_TYPES_SET_OPCODE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_opcode_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter opcode_ndx after opcode_types_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = soc_ppd_lif_cos_opcode_types_get(
    unit,
    prm_opcode_ndx,
    &prm_opcode_type
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_cos_opcode_types_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_cos_opcode_types_get");
    goto exit;
  }

  use_l3 = prm_opcode_type & SOC_PPC_LIF_COS_OPCODE_TYPE_L3;
  use_tc_dp = prm_opcode_type & SOC_PPC_LIF_COS_OPCODE_TYPE_TC_DP;

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_OPCODE_TYPES_SET_OPCODE_TYPES_SET_OPCODE_TYPE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_OPCODE_TYPES_SET_OPCODE_TYPES_SET_OPCODE_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    use_l3 = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_OPCODE_TYPES_SET_OPCODE_TYPES_SET_OPCODE_TYPE_UES_TC_DP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_OPCODE_TYPES_SET_OPCODE_TYPES_SET_OPCODE_TYPE_UES_TC_DP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    use_tc_dp = (uint32)param_val->value.ulong_value;
  }

  if (use_tc_dp)
  {
    prm_opcode_type = SOC_PPC_LIF_COS_OPCODE_TYPE_TC_DP;
  }
  else
  {
    prm_opcode_type = SOC_PPC_LIF_COS_OPCODE_TYPE_L2;
  }
  if (use_l3)
  {
    prm_opcode_type |= SOC_PPC_LIF_COS_OPCODE_TYPE_L3;
  }

  /* Call function */
  ret = soc_ppd_lif_cos_opcode_types_set(
    unit,
    prm_opcode_ndx,
    prm_opcode_type
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_cos_opcode_types_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_cos_opcode_types_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: opcode_types_get (section lif_cos)
 */
int
ui_ppd_api_lif_cos_opcode_types_get(
                                    CURRENT_LINE *current_line
                                    )
{
  uint32
    ret;
  uint32
    prm_opcode_ndx;
  SOC_PPC_LIF_COS_OPCODE_TYPE
    prm_opcode_type;
  uint32
    use_l3=0,use_tc_dp=0;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif_cos");
  soc_sand_proc_name = "soc_ppd_lif_cos_opcode_types_get";

  unit = soc_ppd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_OPCODE_TYPES_GET_OPCODE_TYPES_GET_OPCODE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_OPCODE_TYPES_GET_OPCODE_TYPES_GET_OPCODE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_opcode_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter opcode_ndx after opcode_types_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_lif_cos_opcode_types_get(
    unit,
    prm_opcode_ndx,
    &prm_opcode_type
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_cos_opcode_types_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_cos_opcode_types_get");
    goto exit;
  }

  use_l3 = prm_opcode_type & SOC_PPC_LIF_COS_OPCODE_TYPE_L3;
  use_tc_dp = prm_opcode_type & SOC_PPC_LIF_COS_OPCODE_TYPE_TC_DP;

  if (use_tc_dp)
  {
    cli_out("use_tc_dp: %u\n", 1);
    cli_out("use_l2: %u\n", 0);
  }
  else
  {
    cli_out("use_tc_dp: %u\n", 0);
    cli_out("use_l2: %u\n", 1);
  }
  cli_out("use_l3: %u\n", use_l3);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: opcode_ipv6_tos_map_set (section lif_cos)
 */
int
ui_ppd_api_lif_cos_opcode_ipv6_tos_map_set(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_opcode_ndx;
  SOC_SAND_PP_IPV6_TC   
    prm_ipv6_tos_ndx;
  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO
    prm_action_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif_cos");
  soc_sand_proc_name = "soc_ppd_lif_cos_opcode_ipv6_tos_map_set";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO_clear(&prm_action_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_OPCODE_IPV6_TOS_MAP_SET_OPCODE_IPV6_TOS_MAP_SET_OPCODE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_OPCODE_IPV6_TOS_MAP_SET_OPCODE_IPV6_TOS_MAP_SET_OPCODE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_opcode_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter opcode_ndx after opcode_ipv6_tos_map_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_OPCODE_IPV6_TOS_MAP_SET_OPCODE_IPV6_TOS_MAP_SET_IPV6_TOS_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_OPCODE_IPV6_TOS_MAP_SET_OPCODE_IPV6_TOS_MAP_SET_IPV6_TOS_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ipv6_tos_ndx = (uint8)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ipv6_tos_ndx after opcode_ipv6_tos_map_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = soc_ppd_lif_cos_opcode_ipv6_tos_map_get(
    unit,
    prm_opcode_ndx,
    prm_ipv6_tos_ndx,
    &prm_action_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_cos_opcode_ipv6_tos_map_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_cos_opcode_ipv6_tos_map_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_OPCODE_IPV6_TOS_MAP_SET_OPCODE_IPV6_TOS_MAP_SET_ACTION_INFO_AC_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_OPCODE_IPV6_TOS_MAP_SET_OPCODE_IPV6_TOS_MAP_SET_ACTION_INFO_AC_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_action_info.ac_offset = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_OPCODE_IPV6_TOS_MAP_SET_OPCODE_IPV6_TOS_MAP_SET_ACTION_INFO_IS_QOS_ONLY_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_OPCODE_IPV6_TOS_MAP_SET_OPCODE_IPV6_TOS_MAP_SET_ACTION_INFO_IS_QOS_ONLY_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_action_info.is_qos_only = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_OPCODE_IPV6_TOS_MAP_SET_OPCODE_IPV6_TOS_MAP_SET_ACTION_INFO_IS_PACKET_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_OPCODE_IPV6_TOS_MAP_SET_OPCODE_IPV6_TOS_MAP_SET_ACTION_INFO_IS_PACKET_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_action_info.is_packet_valid = (uint8)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_lif_cos_opcode_ipv6_tos_map_set(
    unit,
    prm_opcode_ndx,
    prm_ipv6_tos_ndx,
    &prm_action_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_cos_opcode_ipv6_tos_map_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_cos_opcode_ipv6_tos_map_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: opcode_ipv6_tos_map_get (section lif_cos)
 */
int
ui_ppd_api_lif_cos_opcode_ipv6_tos_map_get(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_opcode_ndx;
  SOC_SAND_PP_IPV6_TC   
    prm_ipv6_tos_ndx;
  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO
    prm_action_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif_cos");
  soc_sand_proc_name = "soc_ppd_lif_cos_opcode_ipv6_tos_map_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO_clear(&prm_action_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_OPCODE_IPV6_TOS_MAP_GET_OPCODE_IPV6_TOS_MAP_GET_OPCODE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_OPCODE_IPV6_TOS_MAP_GET_OPCODE_IPV6_TOS_MAP_GET_OPCODE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_opcode_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter opcode_ndx after opcode_ipv6_tos_map_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_OPCODE_IPV6_TOS_MAP_GET_OPCODE_IPV6_TOS_MAP_GET_IPV6_TOS_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_OPCODE_IPV6_TOS_MAP_GET_OPCODE_IPV6_TOS_MAP_GET_IPV6_TOS_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ipv6_tos_ndx = (uint8)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ipv6_tos_ndx after opcode_ipv6_tos_map_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_lif_cos_opcode_ipv6_tos_map_get(
    unit,
    prm_opcode_ndx,
    prm_ipv6_tos_ndx,
    &prm_action_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_cos_opcode_ipv6_tos_map_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_cos_opcode_ipv6_tos_map_get");
    goto exit;
  }

  send_string_to_screen("--> action_info:", TRUE);
  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO_print(&prm_action_info);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: opcode_ipv4_tos_map_set (section lif_cos)
 */
int
ui_ppd_api_lif_cos_opcode_ipv4_tos_map_set(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_opcode_ndx;
  SOC_SAND_PP_IPV4_TOS   
    prm_ipv4_tos_ndx;
  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO
    prm_action_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif_cos");
  soc_sand_proc_name = "soc_ppd_lif_cos_opcode_ipv4_tos_map_set";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO_clear(&prm_action_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_OPCODE_IPV4_TOS_MAP_SET_OPCODE_IPV4_TOS_MAP_SET_OPCODE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_OPCODE_IPV4_TOS_MAP_SET_OPCODE_IPV4_TOS_MAP_SET_OPCODE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_opcode_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter opcode_ndx after opcode_ipv4_tos_map_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_OPCODE_IPV4_TOS_MAP_SET_OPCODE_IPV4_TOS_MAP_SET_IPV4_TOS_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_OPCODE_IPV4_TOS_MAP_SET_OPCODE_IPV4_TOS_MAP_SET_IPV4_TOS_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ipv4_tos_ndx = (uint8)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ipv4_tos_ndx after opcode_ipv4_tos_map_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = soc_ppd_lif_cos_opcode_ipv4_tos_map_get(
    unit,
    prm_opcode_ndx,
    prm_ipv4_tos_ndx,
    &prm_action_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_cos_opcode_ipv4_tos_map_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_cos_opcode_ipv4_tos_map_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_OPCODE_IPV4_TOS_MAP_SET_OPCODE_IPV4_TOS_MAP_SET_ACTION_INFO_AC_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_OPCODE_IPV4_TOS_MAP_SET_OPCODE_IPV4_TOS_MAP_SET_ACTION_INFO_AC_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_action_info.ac_offset = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_OPCODE_IPV4_TOS_MAP_SET_OPCODE_IPV4_TOS_MAP_SET_ACTION_INFO_IS_QOS_ONLY_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_OPCODE_IPV4_TOS_MAP_SET_OPCODE_IPV4_TOS_MAP_SET_ACTION_INFO_IS_QOS_ONLY_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_action_info.is_qos_only = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_OPCODE_IPV4_TOS_MAP_SET_OPCODE_IPV4_TOS_MAP_SET_ACTION_INFO_IS_PACKET_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_OPCODE_IPV4_TOS_MAP_SET_OPCODE_IPV4_TOS_MAP_SET_ACTION_INFO_IS_PACKET_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_action_info.is_packet_valid = (uint8)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_lif_cos_opcode_ipv4_tos_map_set(
    unit,
    prm_opcode_ndx,
    prm_ipv4_tos_ndx,
    &prm_action_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_cos_opcode_ipv4_tos_map_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_cos_opcode_ipv4_tos_map_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: opcode_ipv4_tos_map_get (section lif_cos)
 */
int
ui_ppd_api_lif_cos_opcode_ipv4_tos_map_get(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_opcode_ndx;
  SOC_SAND_PP_IPV4_TOS   
    prm_ipv4_tos_ndx;
  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO
    prm_action_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif_cos");
  soc_sand_proc_name = "soc_ppd_lif_cos_opcode_ipv4_tos_map_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO_clear(&prm_action_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_OPCODE_IPV4_TOS_MAP_GET_OPCODE_IPV4_TOS_MAP_GET_OPCODE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_OPCODE_IPV4_TOS_MAP_GET_OPCODE_IPV4_TOS_MAP_GET_OPCODE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_opcode_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter opcode_ndx after opcode_ipv4_tos_map_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_OPCODE_IPV4_TOS_MAP_GET_OPCODE_IPV4_TOS_MAP_GET_IPV4_TOS_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_OPCODE_IPV4_TOS_MAP_GET_OPCODE_IPV4_TOS_MAP_GET_IPV4_TOS_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ipv4_tos_ndx = (uint8)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ipv4_tos_ndx after opcode_ipv4_tos_map_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_lif_cos_opcode_ipv4_tos_map_get(
    unit,
    prm_opcode_ndx,
    prm_ipv4_tos_ndx,
    &prm_action_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_cos_opcode_ipv4_tos_map_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_cos_opcode_ipv4_tos_map_get");
    goto exit;
  }

  send_string_to_screen("--> action_info:", TRUE);
  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO_print(&prm_action_info);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: opcode_tc_dp_map_set (section lif_cos)
 */
int
ui_ppd_api_lif_cos_opcode_tc_dp_map_set(
                                        CURRENT_LINE *current_line
                                        )
{
  uint32
    ret;
  uint32
    prm_opcode_ndx;
  SOC_SAND_PP_TC   
    prm_tc_ndx;
  SOC_SAND_PP_DP   
    prm_dp_ndx;
  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO
    prm_action_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif_cos");
  soc_sand_proc_name = "soc_ppd_lif_cos_opcode_tc_dp_map_set";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO_clear(&prm_action_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_OPCODE_TC_DP_MAP_SET_OPCODE_TC_DP_MAP_SET_OPCODE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_OPCODE_TC_DP_MAP_SET_OPCODE_TC_DP_MAP_SET_OPCODE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_opcode_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter opcode_ndx after opcode_tc_dp_map_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_OPCODE_TC_DP_MAP_SET_OPCODE_TC_DP_MAP_SET_TC_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_OPCODE_TC_DP_MAP_SET_OPCODE_TC_DP_MAP_SET_TC_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tc_ndx = (uint8)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter tc_ndx after opcode_tc_dp_map_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_OPCODE_TC_DP_MAP_SET_OPCODE_TC_DP_MAP_SET_DP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_OPCODE_TC_DP_MAP_SET_OPCODE_TC_DP_MAP_SET_DP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_dp_ndx = (uint8)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter dp_ndx after opcode_tc_dp_map_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = soc_ppd_lif_cos_opcode_tc_dp_map_get(
    unit,
    prm_opcode_ndx,
    prm_tc_ndx,
    prm_dp_ndx,
    &prm_action_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_cos_opcode_tc_dp_map_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_cos_opcode_tc_dp_map_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_OPCODE_TC_DP_MAP_SET_OPCODE_TC_DP_MAP_SET_ACTION_INFO_AC_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_OPCODE_TC_DP_MAP_SET_OPCODE_TC_DP_MAP_SET_ACTION_INFO_AC_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_action_info.ac_offset = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_OPCODE_TC_DP_MAP_SET_OPCODE_TC_DP_MAP_SET_ACTION_INFO_IS_QOS_ONLY_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_OPCODE_TC_DP_MAP_SET_OPCODE_TC_DP_MAP_SET_ACTION_INFO_IS_QOS_ONLY_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_action_info.is_qos_only = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_OPCODE_TC_DP_MAP_SET_OPCODE_TC_DP_MAP_SET_ACTION_INFO_IS_PACKET_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_OPCODE_TC_DP_MAP_SET_OPCODE_TC_DP_MAP_SET_ACTION_INFO_IS_PACKET_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_action_info.is_packet_valid = (uint8)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_lif_cos_opcode_tc_dp_map_set(
    unit,
    prm_opcode_ndx,
    prm_tc_ndx,
    prm_dp_ndx,
    &prm_action_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_cos_opcode_tc_dp_map_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_cos_opcode_tc_dp_map_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: opcode_tc_dp_map_get (section lif_cos)
 */
int
ui_ppd_api_lif_cos_opcode_tc_dp_map_get(
                                        CURRENT_LINE *current_line
                                        )
{
  uint32
    ret;
  uint32
    prm_opcode_ndx;
  SOC_SAND_PP_TC   
    prm_tc_ndx;
  SOC_SAND_PP_DP   
    prm_dp_ndx;
  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO
    prm_action_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif_cos");
  soc_sand_proc_name = "soc_ppd_lif_cos_opcode_tc_dp_map_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO_clear(&prm_action_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_OPCODE_TC_DP_MAP_GET_OPCODE_TC_DP_MAP_GET_OPCODE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_OPCODE_TC_DP_MAP_GET_OPCODE_TC_DP_MAP_GET_OPCODE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_opcode_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter opcode_ndx after opcode_tc_dp_map_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_OPCODE_TC_DP_MAP_GET_OPCODE_TC_DP_MAP_GET_TC_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_OPCODE_TC_DP_MAP_GET_OPCODE_TC_DP_MAP_GET_TC_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tc_ndx = (uint8)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter tc_ndx after opcode_tc_dp_map_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_OPCODE_TC_DP_MAP_GET_OPCODE_TC_DP_MAP_GET_DP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_OPCODE_TC_DP_MAP_GET_OPCODE_TC_DP_MAP_GET_DP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_dp_ndx = (uint8)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter dp_ndx after opcode_tc_dp_map_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_lif_cos_opcode_tc_dp_map_get(
    unit,
    prm_opcode_ndx,
    prm_tc_ndx,
    prm_dp_ndx,
    &prm_action_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_cos_opcode_tc_dp_map_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_cos_opcode_tc_dp_map_get");
    goto exit;
  }

  send_string_to_screen("--> action_info:", TRUE);
  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO_print(&prm_action_info);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: opcode_vlan_tag_map_set (section lif_cos)
 */
int
ui_ppd_api_lif_cos_opcode_vlan_tag_map_set(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_opcode_ndx;
  SOC_SAND_PP_VLAN_TAG_TYPE
    prm_tag_type_ndx;
  SOC_SAND_PP_PCP_UP   
    prm_pcp_ndx;
  SOC_SAND_PP_DEI_CFI   
    prm_dei_ndx;
  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO
    prm_action_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif_cos");
  soc_sand_proc_name = "soc_ppd_lif_cos_opcode_vlan_tag_map_set";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO_clear(&prm_action_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_OPCODE_VLAN_TAG_MAP_SET_OPCODE_VLAN_TAG_MAP_SET_OPCODE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_OPCODE_VLAN_TAG_MAP_SET_OPCODE_VLAN_TAG_MAP_SET_OPCODE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_opcode_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter opcode_ndx after opcode_vlan_tag_map_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_OPCODE_VLAN_TAG_MAP_SET_OPCODE_VLAN_TAG_MAP_SET_TAG_TYPE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_OPCODE_VLAN_TAG_MAP_SET_OPCODE_VLAN_TAG_MAP_SET_TAG_TYPE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tag_type_ndx = (uint8)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter tag_type_ndx after opcode_vlan_tag_map_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_OPCODE_VLAN_TAG_MAP_SET_OPCODE_VLAN_TAG_MAP_SET_PCP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_OPCODE_VLAN_TAG_MAP_SET_OPCODE_VLAN_TAG_MAP_SET_PCP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pcp_ndx = (uint8)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter pcp_ndx after opcode_vlan_tag_map_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_OPCODE_VLAN_TAG_MAP_SET_OPCODE_VLAN_TAG_MAP_SET_DEI_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_OPCODE_VLAN_TAG_MAP_SET_OPCODE_VLAN_TAG_MAP_SET_DEI_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_dei_ndx = (uint8)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter dei_ndx after opcode_vlan_tag_map_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = soc_ppd_lif_cos_opcode_vlan_tag_map_get(
    unit,
    prm_opcode_ndx,
    prm_tag_type_ndx,
    prm_pcp_ndx,
    prm_dei_ndx,
    &prm_action_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_cos_opcode_vlan_tag_map_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_cos_opcode_vlan_tag_map_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_OPCODE_VLAN_TAG_MAP_SET_OPCODE_VLAN_TAG_MAP_SET_ACTION_INFO_AC_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_OPCODE_VLAN_TAG_MAP_SET_OPCODE_VLAN_TAG_MAP_SET_ACTION_INFO_AC_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_action_info.ac_offset = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_OPCODE_VLAN_TAG_MAP_SET_OPCODE_VLAN_TAG_MAP_SET_ACTION_INFO_IS_QOS_ONLY_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_OPCODE_VLAN_TAG_MAP_SET_OPCODE_VLAN_TAG_MAP_SET_ACTION_INFO_IS_QOS_ONLY_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_action_info.is_qos_only = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_OPCODE_VLAN_TAG_MAP_SET_OPCODE_VLAN_TAG_MAP_SET_ACTION_INFO_IS_PACKET_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_OPCODE_VLAN_TAG_MAP_SET_OPCODE_VLAN_TAG_MAP_SET_ACTION_INFO_IS_PACKET_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_action_info.is_packet_valid = (uint8)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_lif_cos_opcode_vlan_tag_map_set(
    unit,
    prm_opcode_ndx,
    prm_tag_type_ndx,
    prm_pcp_ndx,
    prm_dei_ndx,
    &prm_action_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_cos_opcode_vlan_tag_map_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_cos_opcode_vlan_tag_map_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: opcode_vlan_tag_map_get (section lif_cos)
 */
int
ui_ppd_api_lif_cos_opcode_vlan_tag_map_get(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_opcode_ndx;
  SOC_SAND_PP_VLAN_TAG_TYPE
    prm_tag_type_ndx;
  SOC_SAND_PP_PCP_UP   
    prm_pcp_ndx;
  SOC_SAND_PP_DEI_CFI   
    prm_dei_ndx;
  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO
    prm_action_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif_cos");
  soc_sand_proc_name = "soc_ppd_lif_cos_opcode_vlan_tag_map_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO_clear(&prm_action_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_OPCODE_VLAN_TAG_MAP_GET_OPCODE_VLAN_TAG_MAP_GET_OPCODE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_OPCODE_VLAN_TAG_MAP_GET_OPCODE_VLAN_TAG_MAP_GET_OPCODE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_opcode_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter opcode_ndx after opcode_vlan_tag_map_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_OPCODE_VLAN_TAG_MAP_GET_OPCODE_VLAN_TAG_MAP_GET_TAG_TYPE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_OPCODE_VLAN_TAG_MAP_GET_OPCODE_VLAN_TAG_MAP_GET_TAG_TYPE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tag_type_ndx = (uint8)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter tag_type_ndx after opcode_vlan_tag_map_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_OPCODE_VLAN_TAG_MAP_GET_OPCODE_VLAN_TAG_MAP_GET_PCP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_OPCODE_VLAN_TAG_MAP_GET_OPCODE_VLAN_TAG_MAP_GET_PCP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pcp_ndx = (uint8)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter pcp_ndx after opcode_vlan_tag_map_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_OPCODE_VLAN_TAG_MAP_GET_OPCODE_VLAN_TAG_MAP_GET_DEI_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_COS_OPCODE_VLAN_TAG_MAP_GET_OPCODE_VLAN_TAG_MAP_GET_DEI_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_dei_ndx = (uint8)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter dei_ndx after opcode_vlan_tag_map_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_lif_cos_opcode_vlan_tag_map_get(
    unit,
    prm_opcode_ndx,
    prm_tag_type_ndx,
    prm_pcp_ndx,
    prm_dei_ndx,
    &prm_action_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_cos_opcode_vlan_tag_map_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_cos_opcode_vlan_tag_map_get");
    goto exit;
  }

  send_string_to_screen("--> action_info:", TRUE);
  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO_print(&prm_action_info);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: lif_table_get_block (section lif_cos)
 */
int
ui_ppd_api_lif_cos_lif_table_get_block(
                                       CURRENT_LINE *current_line
                                       )
{
  uint32
    ret;
  SOC_PPC_LIF_TBL_TRAVERSE_MATCH_RULE
    prm_rule;
  SOC_SAND_TABLE_BLOCK_RANGE
    prm_block_range;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif_cos");
  soc_sand_proc_name = "soc_ppd_lif_table_get_block";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_LIF_TBL_TRAVERSE_MATCH_RULE_clear(&prm_rule);
  soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&prm_block_range);

  prm_rule.entries_type_bm = SOC_PPC_LIF_ENTRY_TYPE_ALL;
  prm_block_range.entries_to_act = UI_PPD_BLOCK_SIZE;
  prm_block_range.entries_to_scan = SOC_SAND_TBL_ITER_SCAN_ALL;
  SOC_SAND_TBL_ITER_SET_BEGIN(&prm_block_range.iter);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_GET_BLOCK_LIF_TABLE_GET_BLOCK_RULE_ENTRIES_TYPE_BM_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_GET_BLOCK_LIF_TABLE_GET_BLOCK_RULE_ENTRIES_TYPE_BM_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_rule.entries_type_bm = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_GET_BLOCK_LIF_TABLE_GET_BLOCK_RULE_ENTRIES_ACCESSED_ONLY_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_GET_BLOCK_LIF_TABLE_GET_BLOCK_RULE_ENTRIES_ACCESSED_ONLY_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_rule.accessed_only = (uint8)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_GET_BLOCK_LIF_TABLE_GET_BLOCK_START_FROM_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_GET_BLOCK_LIF_TABLE_GET_BLOCK_START_FROM_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_block_range.iter = (uint8)param_val->value.ulong_value;
  }

  /* Call function */
  ret = cmd_diag_alloc_hw_lif_table_get_block_and_print(unit, 
                                                        &prm_rule, 
                                                        &prm_block_range,
                                                        UI_PPD_BLOCK_SIZE);

  if (ret != CMD_OK) {
    send_string_to_screen(" *** cmd_diag_alloc_hw_lif_table_get_block_and_print - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "cmd_diag_alloc_hw_lif_table_get_block_and_print");
    goto exit;
  }

exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: lif_table_entry_get (section lif_cos)
 */
int
ui_ppd_api_lif_cos_lif_table_entry_get(
                                       CURRENT_LINE *current_line
                                       )
{
  uint32
    ret;
  uint32
    prm_lif_ndx;
  SOC_PPC_LIF_ENTRY_INFO
    prm_lif_entry_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif_cos");
  soc_sand_proc_name = "soc_ppd_lif_table_entry_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_LIF_ENTRY_INFO_clear(&prm_lif_entry_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_GET_LIF_TABLE_ENTRY_GET_LIF_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_GET_LIF_TABLE_ENTRY_GET_LIF_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_lif_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter lif_ndx after lif_table_entry_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_lif_table_entry_get(
    unit,
    prm_lif_ndx,
    &prm_lif_entry_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_table_entry_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_table_entry_get");
    goto exit;
  }

  send_string_to_screen("--> lif_entry_info:", TRUE);
  SOC_PPC_LIF_ENTRY_INFO_print(&prm_lif_entry_info);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: lif_table_entry_update (section lif_cos)
 */
int
ui_ppd_api_lif_cos_lif_table_entry_update(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_lif_ndx;
  SOC_PPC_LIF_ENTRY_INFO
    prm_lif_entry_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif_cos");
  soc_sand_proc_name = "soc_ppd_lif_table_entry_update";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_LIF_ENTRY_INFO_clear(&prm_lif_entry_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_LIF_TABLE_ENTRY_UPDATE_LIF_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_LIF_TABLE_ENTRY_UPDATE_LIF_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_lif_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter lif_ndx after lif_table_entry_update***", TRUE);
    goto exit;
  }


  ret = soc_ppd_lif_table_entry_get(
    unit,
    prm_lif_ndx,
    &prm_lif_entry_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_table_entry_update - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_table_entry_update");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_LIF_TABLE_ENTRY_UPDATE_LIF_ENTRY_INFO_TYPE_ID,1))
  {
	  UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_LIF_TABLE_ENTRY_UPDATE_LIF_ENTRY_INFO_TYPE_ID);
	  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
	  /*new_type = param_val->numeric_equivalent;
	  if(new_type == SOC_PPC_LIF_ENTRY_TYPE_AC && prm_lif_entry_info.type == SOC_PPC_LIF_ENTRY_TYPE_EMPTY){
		  prm_lif_entry_info.value.ac.learn_record.learn_type = SOC_PPC_L2_LIF_AC_LEARN_INFO;
		  prm_lif_entry_info.value.ac.learn_record.learn_info.type = SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT;
		  prm_lif_entry_info.value.ac.learn_record.learn_info.additional_info.outlif.type = SOC_PPC_OUTLIF_ENCODE_TYPE_AC;
		  prm_lif_entry_info.value.ac.orientation = SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE;
		  prm_lif_entry_info.value.ac.default_frwrd.default_frwd_type = SOC_PPC_L2_LIF_DFLT_FRWRD_SRC_LIF;
	  }
	  prm_lif_entry_info.type = new_type;*/
	  prm_lif_entry_info.type = param_val->numeric_equivalent;
  }

  prm_lif_entry_info.index = prm_lif_ndx; 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_LIF_TABLE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_TRILL_FEC_ID_ID,1))
  {
    uint32 fec_index; 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_LIF_TABLE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_TRILL_FEC_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    fec_index = (uint32)param_val->value.ulong_value;

    SOC_PPD_FRWRD_DECISION_FEC_SET(
                   unit, &(prm_lif_entry_info.value.trill.learn_info), fec_index, ret); 
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_LIF_TABLE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_TRILL_LEARN_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_LIF_TABLE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_TRILL_LEARN_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_lif_entry_info.value.trill.learn_enable = (uint32)param_val->value.ulong_value;
  }


  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_LIF_TABLE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_RIF_ROUTING_ENABLERS_BM_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_LIF_TABLE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_RIF_ROUTING_ENABLERS_BM_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_lif_entry_info.value.rif.routing_enablers_bm = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_LIF_TABLE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_RIF_VRF_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_LIF_TABLE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_RIF_VRF_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_lif_entry_info.value.rif.vrf_id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_LIF_TABLE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_RIF_UC_RPF_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_LIF_TABLE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_RIF_UC_RPF_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_lif_entry_info.value.rif.uc_rpf_enable = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_LIF_TABLE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_ISID_TPID_PROFILE_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_LIF_TABLE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_ISID_TPID_PROFILE_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_lif_entry_info.value.isid.tpid_profile_index = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_LIF_TABLE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_ISID_LEARN_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_LIF_TABLE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_ISID_LEARN_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_lif_entry_info.value.isid.learn_enable = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_LIF_TABLE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_ISID_DEFAULT_FRWRD_DEFAULT_FRWD_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_LIF_TABLE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_ISID_DEFAULT_FRWRD_DEFAULT_FRWD_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
	if(prm_lif_entry_info.type == SOC_PPC_LIF_ENTRY_TYPE_PWE)
    {
      prm_lif_entry_info.value.pwe.default_frwrd.default_frwd_type = param_val->numeric_equivalent;
    }
    if(prm_lif_entry_info.type == SOC_PPC_LIF_ENTRY_TYPE_ISID)
    {
		prm_lif_entry_info.value.isid.default_frwrd.default_frwd_type = param_val->numeric_equivalent;
    }
	else{
		prm_lif_entry_info.value.ac.default_frwrd.default_frwd_type = param_val->numeric_equivalent;
	}
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_LIF_TABLE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_COS_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_LIF_TABLE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_COS_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_lif_entry_info.value.pwe.cos_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_LIF_TABLE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_TPID_PROFILE_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_LIF_TABLE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_TPID_PROFILE_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_lif_entry_info.value.pwe.tpid_profile_index = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_LIF_TABLE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_ORIENTATION_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_LIF_TABLE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_ORIENTATION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_lif_entry_info.value.pwe.orientation = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_LIF_TABLE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_LEARN_INFO_ENABLE_LEARNING_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_LIF_TABLE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_LEARN_INFO_ENABLE_LEARNING_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_lif_entry_info.value.pwe.learn_record.enable_learning = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_LIF_TABLE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_DEFAULT_FRWRD_DEFAULT_FRWD_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_LIF_TABLE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_DEFAULT_FRWRD_DEFAULT_FRWD_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_lif_entry_info.value.pwe.default_frwrd.default_frwd_type = param_val->numeric_equivalent;
  }


  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_LIF_TABLE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_VSID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_LIF_TABLE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_VSID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_lif_entry_info.value.pwe.vsid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_LIF_TABLE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_COS_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_LIF_TABLE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_COS_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_lif_entry_info.value.ac.cos_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_LIF_TABLE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_ORIENTATION_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_LIF_TABLE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_ORIENTATION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_lif_entry_info.value.ac.orientation = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_LIF_TABLE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_DEFAULT_FRWRD_DEFAULT_FRWD_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_LIF_TABLE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_DEFAULT_FRWRD_DEFAULT_FRWD_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_lif_entry_info.value.ac.default_frwrd.default_frwd_type = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_SERVICE_TYPE_ID,1)) 
  { 
	  UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_SERVICE_TYPE_ID); 
	  UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
	  prm_lif_entry_info.value.ac.service_type = param_val->numeric_equivalent;
  } 

  if (prm_lif_entry_info.value.ac.service_type != SOC_PPC_L2_LIF_AC_SERVICE_TYPE_MP)
  {
	  ret = ui_ppd_frwrd_decision_set(
		  current_line,
		  1,
		  &(prm_lif_entry_info.value.ac.default_frwrd.default_forwarding)
		  );
	  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
	  {
		  send_string_to_screen(" *** ui_ppd_frwrd_decision_set - FAIL", TRUE);
		  soc_sand_disp_result_proc(ret, "ui_ppd_frwrd_decision_set");
		  goto exit;
	  }
  }
  else
  {
	  ret = ui_ppd_frwrd_decision_set(
		  current_line,
		  1,
		  &(prm_lif_entry_info.value.ac.learn_record.learn_info)
		  );
	  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
	  {
		  send_string_to_screen(" *** ui_ppd_frwrd_decision_set - FAIL", TRUE);
		  soc_sand_disp_result_proc(ret, "ui_ppd_frwrd_decision_set");
		  goto exit;
	  }
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_LIF_TABLE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_ING_EDIT_INFO_EDIT_PCP_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_LIF_TABLE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_ING_EDIT_INFO_EDIT_PCP_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_lif_entry_info.value.ac.ing_edit_info.edit_pcp_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_LIF_TABLE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_ING_EDIT_INFO_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_LIF_TABLE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_ING_EDIT_INFO_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_lif_entry_info.value.ac.ing_edit_info.vid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_LIF_TABLE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_ING_EDIT_INFO_ING_VLAN_EDIT_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_LIF_TABLE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_ING_EDIT_INFO_ING_VLAN_EDIT_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_lif_entry_info.value.ac.ing_edit_info.ing_vlan_edit_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_LIF_TABLE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_VSID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_LIF_TABLE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_VSID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      prm_lif_entry_info.value.ac.vsid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_LIF_TABLE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_LEARN_TYPE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_LIF_TABLE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_LEARN_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_lif_entry_info.value.ac.learn_record.learn_type = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = soc_ppd_lif_table_entry_update(
    unit,
    prm_lif_ndx,
    &prm_lif_entry_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_lif_table_entry_update - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_table_entry_update");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Section handler: lif_cos
 */
int
ui_ppd_api_lif_cos(
                   CURRENT_LINE *current_line
                   )
{
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif_cos");

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_AC_PROFILE_INFO_SET_AC_PROFILE_INFO_SET_ID,1))
  {
    ui_ret = ui_ppd_api_lif_cos_ac_profile_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_AC_PROFILE_INFO_GET_AC_PROFILE_INFO_GET_ID,1))
  {
    ui_ret = ui_ppd_api_lif_cos_ac_profile_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PWE_PROFILE_INFO_SET_PWE_PROFILE_INFO_SET_ID,1))
  {
    ui_ret = ui_ppd_api_lif_cos_pwe_profile_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PWE_PROFILE_INFO_GET_PWE_PROFILE_INFO_GET_ID,1))
  {
    ui_ret = ui_ppd_api_lif_cos_pwe_profile_info_get(current_line);
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PROFILE_INFO_SET_PROFILE_INFO_SET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_lif_cos_profile_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PROFILE_INFO_GET_PROFILE_INFO_GET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_lif_cos_profile_info_get(current_line); 
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_L2_INFO_SET_PROFILE_MAP_L2_INFO_SET_ID,1))
  {
    ui_ret = ui_ppd_api_lif_cos_profile_map_l2_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_L2_INFO_GET_PROFILE_MAP_L2_INFO_GET_ID,1))
  {
    ui_ret = ui_ppd_api_lif_cos_profile_map_l2_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_IP_INFO_SET_PROFILE_MAP_IP_INFO_SET_ID,1))
  {
    ui_ret = ui_ppd_api_lif_cos_profile_map_ip_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_IP_INFO_GET_PROFILE_MAP_IP_INFO_GET_ID,1))
  {
    ui_ret = ui_ppd_api_lif_cos_profile_map_ip_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_SET_PROFILE_MAP_MPLS_LABEL_INFO_SET_ID,1))
  {
    ui_ret = ui_ppd_api_lif_cos_profile_map_mpls_label_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_GET_PROFILE_MAP_MPLS_LABEL_INFO_GET_ID,1))
  {
    ui_ret = ui_ppd_api_lif_cos_profile_map_mpls_label_info_get(current_line);
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_TC_DP_INFO_SET_PROFILE_MAP_TC_DP_INFO_SET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_lif_cos_profile_map_tc_dp_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_PROFILE_MAP_TC_DP_INFO_GET_PROFILE_MAP_TC_DP_INFO_GET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_lif_cos_profile_map_tc_dp_info_get(current_line); 
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_OPCODE_TYPES_SET_OPCODE_TYPES_SET_ID,1))
  {
    ui_ret = ui_ppd_api_lif_cos_opcode_types_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_OPCODE_TYPES_GET_OPCODE_TYPES_GET_ID,1))
  {
    ui_ret = ui_ppd_api_lif_cos_opcode_types_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_OPCODE_IPV6_TOS_MAP_SET_OPCODE_IPV6_TOS_MAP_SET_ID,1))
  {
    ui_ret = ui_ppd_api_lif_cos_opcode_ipv6_tos_map_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_OPCODE_IPV6_TOS_MAP_GET_OPCODE_IPV6_TOS_MAP_GET_ID,1))
  {
    ui_ret = ui_ppd_api_lif_cos_opcode_ipv6_tos_map_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_OPCODE_IPV4_TOS_MAP_SET_OPCODE_IPV4_TOS_MAP_SET_ID,1))
  {
    ui_ret = ui_ppd_api_lif_cos_opcode_ipv4_tos_map_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_OPCODE_IPV4_TOS_MAP_GET_OPCODE_IPV4_TOS_MAP_GET_ID,1))
  {
    ui_ret = ui_ppd_api_lif_cos_opcode_ipv4_tos_map_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_OPCODE_TC_DP_MAP_SET_OPCODE_TC_DP_MAP_SET_ID,1))
  {
    ui_ret = ui_ppd_api_lif_cos_opcode_tc_dp_map_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_OPCODE_TC_DP_MAP_GET_OPCODE_TC_DP_MAP_GET_ID,1))
  {
    ui_ret = ui_ppd_api_lif_cos_opcode_tc_dp_map_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_OPCODE_VLAN_TAG_MAP_SET_OPCODE_VLAN_TAG_MAP_SET_ID,1))
  {
    ui_ret = ui_ppd_api_lif_cos_opcode_vlan_tag_map_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_COS_OPCODE_VLAN_TAG_MAP_GET_OPCODE_VLAN_TAG_MAP_GET_ID,1))
  {
    ui_ret = ui_ppd_api_lif_cos_opcode_vlan_tag_map_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_GET_BLOCK_LIF_TABLE_GET_BLOCK_ID,1))
  {
    ui_ret = ui_ppd_api_lif_cos_lif_table_get_block(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_GET_LIF_TABLE_ENTRY_GET_ID,1))
  {
    ui_ret = ui_ppd_api_lif_cos_lif_table_entry_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_LIF_TABLE_ENTRY_UPDATE_ID,1))
  {
    ui_ret = ui_ppd_api_lif_cos_lif_table_entry_update(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after lif_cos***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif
#endif /* LINK_PPD_LIBRARIES */ 

