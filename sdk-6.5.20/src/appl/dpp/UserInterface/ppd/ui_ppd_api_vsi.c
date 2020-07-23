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
 

  
#include <soc/dpp/PPD/ppd_api_vsi.h>
#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_vsi.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_lif.h>

#if LINK_PPD_LIBRARIES

#ifdef UI_VSI

/********************************************************************
*  Function handler: info_set (section vsi)
 */
int
ui_ppd_api_vsi_info_set(
                        CURRENT_LINE *current_line
                        )
{
  uint32
    ret;
  SOC_PPC_VSI_ID   
    prm_vsi_ndx;
  SOC_PPC_VSI_INFO
    prm_vsi_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_vsi");
  soc_sand_proc_name = "soc_ppd_vsi_info_set";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_VSI_INFO_clear(&prm_vsi_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_VSI_INFO_SET_INFO_SET_VSI_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_VSI_INFO_SET_INFO_SET_VSI_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsi_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vsi_ndx after info_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = soc_ppd_vsi_info_get(
    unit,
    prm_vsi_ndx,
    &prm_vsi_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_vsi_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_vsi_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_VSI_INFO_SET_INFO_SET_VSI_INFO_LIMIT_PROFILE_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_VSI_INFO_SET_INFO_SET_VSI_INFO_LIMIT_PROFILE_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsi_info.mac_learn_profile_id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_VSI_INFO_SET_INFO_SET_VSI_INFO_FID_PROFILE_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_VSI_INFO_SET_INFO_SET_VSI_INFO_FID_PROFILE_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsi_info.fid_profile_id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_VSI_INFO_SET_INFO_SET_VSI_INFO_ENABLE_MY_MAC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_VSI_INFO_SET_INFO_SET_VSI_INFO_ENABLE_MY_MAC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsi_info.enable_my_mac = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_VSI_INFO_SET_INFO_SET_VSI_INFO_STP_TOPOLOGY_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_VSI_INFO_SET_INFO_SET_VSI_INFO_STP_TOPOLOGY_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsi_info.stp_topology_id = (uint32)param_val->value.ulong_value;
  }

  ret = ui_ppd_frwrd_decision_set(
          current_line,
          1,
          &(prm_vsi_info.default_forwarding)
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** ui_ppd_frwrd_decision_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "ui_ppd_frwrd_decision_set");
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_vsi_info_set(
    unit,
    prm_vsi_ndx,
    &prm_vsi_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_vsi_info_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_vsi_info_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: info_get (section vsi)
 */
int
ui_ppd_api_vsi_info_get(
                        CURRENT_LINE *current_line
                        )
{
  uint32
    ret;
  SOC_PPC_VSI_ID   
    prm_vsi_ndx;
  SOC_PPC_VSI_INFO
    prm_vsi_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_vsi");
  soc_sand_proc_name = "soc_ppd_vsi_info_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_VSI_INFO_clear(&prm_vsi_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_VSI_INFO_GET_INFO_GET_VSI_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_VSI_INFO_GET_INFO_GET_VSI_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsi_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vsi_ndx after info_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_vsi_info_get(
    unit,
    prm_vsi_ndx,
    &prm_vsi_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_vsi_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_vsi_info_get");
    goto exit;
  }

  send_string_to_screen("--> vsi_info:", TRUE);
  SOC_PPC_VSI_INFO_print(&prm_vsi_info);

  goto exit;
exit:
  return ui_ret;
}


/********************************************************************
*  Function handler: egress_mtu_set (section vsi)
 */
int
ui_ppd_api_vsi_egress_mtu_set(
                        CURRENT_LINE *current_line
                        )
{   
  uint32 
    ret;   
  uint32   
    prm_vsi_profile_ndx;
  uint32   
    prm_mtu_val;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_vsi"); 
  soc_sand_proc_name = "soc_ppd_vsi_egress_mtu_set"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_VSI_EGRESS_MTU_SET_VSI_EGRESS_MTU_SET_VSI_PROFILE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_VSI_EGRESS_MTU_SET_VSI_EGRESS_MTU_SET_VSI_PROFILE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_vsi_profile_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter vsi_ndx after vsi_egress_mtu_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_vsi_egress_mtu_get(
          unit,
          FALSE,
          prm_vsi_profile_ndx,
          &prm_mtu_val
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_vsi_egress_mtu_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_vsi_egress_mtu_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_VSI_EGRESS_MTU_SET_VSI_EGRESS_MTU_SET_MTU_VAL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_VSI_EGRESS_MTU_SET_VSI_EGRESS_MTU_SET_MTU_VAL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_mtu_val = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_vsi_egress_mtu_set(
          unit,
          FALSE,
          prm_vsi_profile_ndx,
          prm_mtu_val
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_vsi_egress_mtu_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_vsi_egress_mtu_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: egress_mtu_get (section vsi)
 */
int 
  ui_ppd_api_vsi_egress_mtu_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_vsi_profile_ndx;
  uint32   
    prm_mtu_val;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_vsi"); 
  soc_sand_proc_name = "soc_ppd_vsi_egress_mtu_get"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_VSI_EGRESS_MTU_GET_VSI_EGRESS_MTU_GET_VSI_PROFILE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_VSI_EGRESS_MTU_GET_VSI_EGRESS_MTU_GET_VSI_PROFILE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_vsi_profile_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter ttl_scope_ndx after ttl_scope_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_vsi_egress_mtu_get(
          unit,
          FALSE,
          prm_vsi_profile_ndx,
          &prm_mtu_val
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_vsi_egress_mtu_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_vsi_egress_mtu_get");   
    goto exit; 
  } 

  cli_out("mtu_val: %u\n\r",prm_mtu_val);

  
  goto exit; 
exit: 
  return ui_ret; 
} 

/********************************************************************
*  Section handler: vsi
 */
int
ui_ppd_api_vsi(
               CURRENT_LINE *current_line
               )
{
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_vsi");

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_VSI_INFO_SET_INFO_SET_ID,1))
  {
    ui_ret = ui_ppd_api_vsi_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_VSI_INFO_GET_INFO_GET_ID,1))
  {
    ui_ret = ui_ppd_api_vsi_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_VSI_EGRESS_MTU_SET_VSI_EGRESS_MTU_SET_ID,1))
  {
    ui_ret = ui_ppd_api_vsi_egress_mtu_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_VSI_EGRESS_MTU_GET_VSI_EGRESS_MTU_GET_ID,1))
  {
    ui_ret = ui_ppd_api_vsi_egress_mtu_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after vsi***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}


#endif
#endif /* LINK_PPD_LIBRARIES */ 

