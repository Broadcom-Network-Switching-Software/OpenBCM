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
 
  
#include <soc/dpp/PPD/ppd_api_port.h>
#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_port.h>

#include <soc/dpp/mbcm.h>


#if LINK_PPD_LIBRARIES

#ifdef UI_PORT
/******************************************************************** 
 *  Function handler: info_set (section port)
 */
int 
  ui_ppd_api_port_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  bcm_port_t
    prm_local_port_ndx;
  SOC_PPC_PORT
    soc_ppd_port;
  SOC_PPC_PORT_INFO
    prm_port_info;
  int
    core = 0;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_port"); 
  soc_sand_proc_name = "soc_ppd_port_info_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_PORT_INFO_clear(&prm_port_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_LOCAL_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_LOCAL_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_local_port_ndx = (bcm_port_t)param_val->value.ulong_value;
    ret = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, prm_local_port_ndx, &soc_ppd_port, &core));
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
        send_string_to_screen(" *** SW error - fail to get core number from  parameter local_port_ndx***", TRUE);
        goto exit;
    }
  }
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter local_port_ndx after info_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_port_info_get(
          unit,
          core,
          soc_ppd_port,
          &prm_port_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_port_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_port_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_MTU_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_MTU_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_info.mtu = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_NOT_AUTHORIZED_802_1X_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_NOT_AUTHORIZED_802_1X_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_info.not_authorized_802_1x = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_ETHER_TYPE_BASED_PROFILE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_ETHER_TYPE_BASED_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_info.ether_type_based_profile = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_ORIENTATION_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_ORIENTATION_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_info.orientation = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_SAME_INTERFACE_FILTER_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_SAME_INTERFACE_FILTER_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_info.enable_same_interfac_filter = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_IS_LEARN_AC_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_IS_LEARN_AC_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_info.is_learn_ac = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_ENABLE_LEARNING_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_ENABLE_LEARNING_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_info.enable_learning = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_IS_PBP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_IS_PBP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_info.is_pbp = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_VLAN_TRANSLATION_PROFILE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_VLAN_TRANSLATION_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_info.vlan_translation_profile = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_TUNNEL_TERMINATION_PROFILE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_TUNNEL_TERMINATION_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_info.tunnel_termination_profile = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_INITIAL_ACTION_PROFILE_SNOOP_ACTION_STRENGTH_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_INITIAL_ACTION_PROFILE_SNOOP_ACTION_STRENGTH_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_info.initial_action_profile.snoop_action_strength = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_INITIAL_ACTION_PROFILE_FRWRD_ACTION_STRENGTH_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_INITIAL_ACTION_PROFILE_FRWRD_ACTION_STRENGTH_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_info.initial_action_profile.frwrd_action_strength = (uint32)param_val->value.ulong_value;
  } 

  /*trap-code
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_INITIAL_ACTION_PROFILE_TRAP_CODE_LSB_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_INITIAL_ACTION_PROFILE_TRAP_CODE_LSB_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_info.initial_action_profile.trap_code = (SOC_PPC_TRAP_CODE)param_val->numeric_equivalent;
  } 
  */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_INITIAL_ACTION_PROFILE_TRAP_CODE_LSB_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_INITIAL_ACTION_PROFILE_TRAP_CODE_LSB_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_info.initial_action_profile.trap_code = (SOC_PPC_TRAP_CODE)param_val->value.ulong_value;
  } 


  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_PORT_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_PORT_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_info.port_type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_VLAN_DOMAIN_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_VLAN_DOMAIN_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_info.vlan_domain = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_TPID_PROFILE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_TPID_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_info.tpid_profile = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_DA_NOT_FOUND_PROFILE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_DA_NOT_FOUND_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_info.da_not_found_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_PORT_PROFILE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_PORT_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_info.port_profile = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  prm_port_info.flags = SOC_PPC_PORT_ALL_TBL;
  ret = soc_ppd_port_info_set(
          unit,
		  core,
          prm_local_port_ndx,
          &prm_port_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_port_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_port_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: info_get (section port)
 */
int 
  ui_ppd_api_port_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  bcm_port_t
    prm_local_port_ndx;
  SOC_PPC_PORT
    soc_ppd_port;
  SOC_PPC_PORT_INFO
    prm_port_info;
  int
    core = 0;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_port"); 
  soc_sand_proc_name = "soc_ppd_port_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_PORT_INFO_clear(&prm_port_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_INFO_GET_INFO_GET_LOCAL_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_INFO_GET_INFO_GET_LOCAL_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_local_port_ndx = (uint32)param_val->value.ulong_value;
  ret = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, prm_local_port_ndx, &soc_ppd_port, &core));
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
        send_string_to_screen(" *** SW error - fail to get core number from  parameter local_port_ndx***", TRUE);
        goto exit;
    }
  }
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter local_port_ndx after info_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_port_info_get(
          unit,
          core,
          soc_ppd_port,
          &prm_port_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_port_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_port_info_get");   
    goto exit; 
  } 

  SOC_PPC_PORT_INFO_print(&prm_port_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: stp_state_set (section port)
 */
int 
  ui_ppd_api_port_stp_state_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_PORT   
    prm_local_port_ndx;
  uint32   
    prm_topology_id_ndx;
  SOC_PPC_PORT_STP_STATE   
    prm_stp_state;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_port"); 
  soc_sand_proc_name = "soc_ppd_port_stp_state_set"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_STP_STATE_SET_STP_STATE_SET_LOCAL_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_STP_STATE_SET_STP_STATE_SET_LOCAL_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_local_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter local_port_ndx after stp_state_set***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_STP_STATE_SET_STP_STATE_SET_TOPOLOGY_ID_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_STP_STATE_SET_STP_STATE_SET_TOPOLOGY_ID_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_topology_id_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter topology_id_ndx after stp_state_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_port_stp_state_get(
          unit,
          SOC_CORE_DEFAULT,
          prm_local_port_ndx,
          prm_topology_id_ndx,
          &prm_stp_state
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_port_stp_state_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_port_stp_state_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_STP_STATE_SET_STP_STATE_SET_STP_STATE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_PORT_STP_STATE_SET_STP_STATE_SET_STP_STATE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_stp_state = param_val->numeric_equivalent;
  } 


  /* Call function */
  ret = soc_ppd_port_stp_state_set(
          unit,
          SOC_CORE_DEFAULT,
          prm_local_port_ndx,
          prm_topology_id_ndx,
          prm_stp_state
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_port_stp_state_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_port_stp_state_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: stp_state_get (section port)
 */
int 
  ui_ppd_api_port_stp_state_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_PORT   
    prm_local_port_ndx;
  uint32   
    prm_topology_id_ndx;
  SOC_PPC_PORT_STP_STATE   
    prm_stp_state;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_port"); 
  soc_sand_proc_name = "soc_ppd_port_stp_state_get"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_STP_STATE_GET_STP_STATE_GET_LOCAL_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_STP_STATE_GET_STP_STATE_GET_LOCAL_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_local_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter local_port_ndx after stp_state_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_STP_STATE_GET_STP_STATE_GET_TOPOLOGY_ID_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_STP_STATE_GET_STP_STATE_GET_TOPOLOGY_ID_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_topology_id_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter topology_id_ndx after stp_state_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_port_stp_state_get(
          unit,
          SOC_CORE_DEFAULT,
          prm_local_port_ndx,
          prm_topology_id_ndx,
          &prm_stp_state
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_port_stp_state_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_port_stp_state_get");   
    goto exit; 
  } 

  cli_out("stp_state: %s\n\r",SOC_PPC_PORT_STP_STATE_to_string(prm_stp_state));

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
#endif
#ifdef UI_PORT/* { port*/
/******************************************************************** 
 *  Section handler: port
 */ 
int 
  ui_ppd_api_port( 
    CURRENT_LINE *current_line 
  ) 
{   
  int
    ret = FALSE;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_port"); 
 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_ID,1)) 
  { 
    ret = ui_ppd_api_port_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_INFO_GET_INFO_GET_ID,1)) 
  { 
    ret = ui_ppd_api_port_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_STP_STATE_SET_STP_STATE_SET_ID,1)) 
  { 
    ret = ui_ppd_api_port_stp_state_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_STP_STATE_GET_STP_STATE_GET_ID,1)) 
  { 
    ret = ui_ppd_api_port_stp_state_get(current_line); 
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting function name after port***", TRUE); 
  }
  
  ui_ret = ret; 
    
  goto exit; 
exit:        
  return ui_ret; 
}                
          
#endif /* port */ 


#endif /* LINK_PPD_LIBRARIES */ 

