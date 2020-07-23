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
 
  
#include <soc/dpp/PPD/ppd_api_frwrd_ipv4.h>
#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>
#include <soc/dpp/PPC/ppc_api_general.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_frwrd_ipv4.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lem_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_mact.h>

#if LINK_PPD_LIBRARIES

#ifdef UI_FRWRD_IPV4

#define UI_PPD_API_FRWRD_IPV4_GET_BLOCK_NOF_ENTRIES 130
/******************************************************************** 
 *  Function handler: frwrd_ip_routes_cache_mode_enable_set (section frwrd_ipv4)
 */
int 
  ui_ppd_api_frwrd_ipv4_frwrd_ip_routes_cache_mode_enable_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_VRF_ID   
    prm_vrf_ndx=0;
  uint32   
    prm_route_types=0;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv4"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ip_routes_cache_mode_enable_set"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_SET_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_SET_VRF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_SET_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_SET_VRF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_vrf_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter vrf_ndx after frwrd_ip_routes_cache_mode_enable_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_frwrd_ip_routes_cache_mode_enable_get(
          unit,
          prm_vrf_ndx,
          &prm_route_types
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_ip_routes_cache_mode_enable_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ip_routes_cache_mode_enable_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_SET_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_SET_ROUTE_TYPES_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_SET_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_SET_ROUTE_TYPES_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_route_types = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_frwrd_ip_routes_cache_mode_enable_set(
          unit,
          prm_vrf_ndx,
          prm_route_types
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_ip_routes_cache_mode_enable_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ip_routes_cache_mode_enable_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: frwrd_ip_routes_cache_mode_enable_get (section frwrd_ipv4)
 */
int 
  ui_ppd_api_frwrd_ipv4_frwrd_ip_routes_cache_mode_enable_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_VRF_ID   
    prm_vrf_ndx=0;
  uint32   
    prm_route_types=0;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv4"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ip_routes_cache_mode_enable_get"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_GET_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_GET_VRF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_GET_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_GET_VRF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_vrf_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter vrf_ndx after frwrd_ip_routes_cache_mode_enable_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_frwrd_ip_routes_cache_mode_enable_get(
          unit,
          prm_vrf_ndx,
          &prm_route_types
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_ip_routes_cache_mode_enable_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ip_routes_cache_mode_enable_get");   
    goto exit; 
  } 

  cli_out("route_types: %u\n\r",prm_route_types);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: frwrd_ip_routes_cache_commit (section frwrd_ipv4)
 */
int 
  ui_ppd_api_frwrd_ipv4_frwrd_ip_routes_cache_commit(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_route_types=0;
  SOC_PPC_VRF_ID   
    prm_vrf_ndx=0;
  SOC_SAND_SUCCESS_FAILURE   
    prm_success;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv4"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ip_routes_cache_commit"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IP_ROUTES_CACHE_COMMIT_FRWRD_IP_ROUTES_CACHE_COMMIT_VRF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IP_ROUTES_CACHE_COMMIT_FRWRD_IP_ROUTES_CACHE_COMMIT_VRF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_vrf_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter vrf_ndx after frwrd_ip_routes_cache_commit***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IP_ROUTES_CACHE_COMMIT_FRWRD_IP_ROUTES_CACHE_COMMIT_ROUTE_TYPES_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IP_ROUTES_CACHE_COMMIT_FRWRD_IP_ROUTES_CACHE_COMMIT_ROUTE_TYPES_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_route_types = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_frwrd_ip_routes_cache_commit(
          unit,
          prm_route_types,
          prm_vrf_ndx,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_ip_routes_cache_commit - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ip_routes_cache_commit");   
    goto exit; 
  } 

  cli_out("success: %s\n\r",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: glbl_info_set (section frwrd_ipv4)
 */
int 
  ui_ppd_api_frwrd_ipv4_glbl_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_FRWRD_IPV4_GLBL_INFO   
    prm_glbl_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv4"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv4_glbl_info_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_IPV4_GLBL_INFO_clear(&prm_glbl_info);
 
  /* Get parameters */ 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_frwrd_ipv4_glbl_info_get(
          unit,
          &prm_glbl_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_ipv4_glbl_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv4_glbl_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_MC_TABLE_RESOUCES_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_IPV4_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_MC_TABLE_RESOUCES_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_glbl_info.mc_table_resouces = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_UC_TABLE_RESOUCES_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_IPV4_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_UC_TABLE_RESOUCES_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_glbl_info.uc_table_resouces = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_ROUTER_INFO_MC_DEFAULT_ACTION_VALUE_ACTION_PROFILE_SNOOP_STRENGTH_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_ROUTER_INFO_MC_DEFAULT_ACTION_VALUE_ACTION_PROFILE_SNOOP_STRENGTH_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_glbl_info.router_info.mc_default_action.value.action_profile.snoop_action_strength = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_ROUTER_INFO_MC_DEFAULT_ACTION_VALUE_ACTION_PROFILE_FRWRD_STRENGTH_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_ROUTER_INFO_MC_DEFAULT_ACTION_VALUE_ACTION_PROFILE_FRWRD_STRENGTH_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_glbl_info.router_info.mc_default_action.value.action_profile.frwrd_action_strength = (uint32)param_val->value.ulong_value;
  } 

  /*trap-code
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_ROUTER_INFO_MC_DEFAULT_ACTION_VALUE_ACTION_PROFILE_TRAP_CODE_LSB_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_IPV4_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_ROUTER_INFO_MC_DEFAULT_ACTION_VALUE_ACTION_PROFILE_TRAP_CODE_LSB_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_glbl_info.router_info.mc_default_action.value.action_profile.trap_code = (SOC_PPC_TRAP_CODE)param_val->numeric_equivalent;
  } 
  */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_ROUTER_INFO_MC_DEFAULT_ACTION_VALUE_ACTION_PROFILE_TRAP_CODE_LSB_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_ROUTER_INFO_MC_DEFAULT_ACTION_VALUE_ACTION_PROFILE_TRAP_CODE_LSB_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_glbl_info.router_info.mc_default_action.value.action_profile.trap_code = (SOC_PPC_TRAP_CODE)param_val->value.ulong_value;
  } 


  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_ROUTER_INFO_MC_DEFAULT_ACTION_VALUE_ROUTE_VAL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_ROUTER_INFO_MC_DEFAULT_ACTION_VALUE_ROUTE_VAL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_glbl_info.router_info.mc_default_action.value.route_val = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_ROUTER_INFO_MC_DEFAULT_ACTION_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_IPV4_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_ROUTER_INFO_MC_DEFAULT_ACTION_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_glbl_info.router_info.mc_default_action.type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_ROUTER_INFO_UC_DEFAULT_ACTION_VALUE_ACTION_PROFILE_SNOOP_STRENGTH_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_ROUTER_INFO_UC_DEFAULT_ACTION_VALUE_ACTION_PROFILE_SNOOP_STRENGTH_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_glbl_info.router_info.uc_default_action.value.action_profile.snoop_action_strength = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_ROUTER_INFO_UC_DEFAULT_ACTION_VALUE_ACTION_PROFILE_FRWRD_STRENGTH_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_ROUTER_INFO_UC_DEFAULT_ACTION_VALUE_ACTION_PROFILE_FRWRD_STRENGTH_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_glbl_info.router_info.uc_default_action.value.action_profile.frwrd_action_strength = (uint32)param_val->value.ulong_value;
  } 

  /*trap-code
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_ROUTER_INFO_UC_DEFAULT_ACTION_VALUE_ACTION_PROFILE_TRAP_CODE_LSB_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_IPV4_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_ROUTER_INFO_UC_DEFAULT_ACTION_VALUE_ACTION_PROFILE_TRAP_CODE_LSB_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_glbl_info.router_info.uc_default_action.value.action_profile.trap_code = (SOC_PPC_TRAP_CODE)param_val->numeric_equivalent;
  } 
  */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_ROUTER_INFO_UC_DEFAULT_ACTION_VALUE_ACTION_PROFILE_TRAP_CODE_LSB_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_ROUTER_INFO_UC_DEFAULT_ACTION_VALUE_ACTION_PROFILE_TRAP_CODE_LSB_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_glbl_info.router_info.uc_default_action.value.action_profile.trap_code = (SOC_PPC_TRAP_CODE)param_val->value.ulong_value;
  } 


  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_ROUTER_INFO_UC_DEFAULT_ACTION_VALUE_ROUTE_VAL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_ROUTER_INFO_UC_DEFAULT_ACTION_VALUE_ROUTE_VAL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_glbl_info.router_info.uc_default_action.value.route_val = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_ROUTER_INFO_UC_DEFAULT_ACTION_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_IPV4_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_ROUTER_INFO_UC_DEFAULT_ACTION_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_glbl_info.router_info.uc_default_action.type = param_val->numeric_equivalent;
  } 


  /* Call function */
  ret = soc_ppd_frwrd_ipv4_glbl_info_set(
          unit,
          &prm_glbl_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_ipv4_glbl_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv4_glbl_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: glbl_info_get (section frwrd_ipv4)
 */
int 
  ui_ppd_api_frwrd_ipv4_glbl_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_FRWRD_IPV4_GLBL_INFO   
    prm_glbl_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv4"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv4_glbl_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_IPV4_GLBL_INFO_clear(&prm_glbl_info);
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_ppd_frwrd_ipv4_glbl_info_get(
          unit,
          &prm_glbl_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_ipv4_glbl_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv4_glbl_info_get");   
    goto exit; 
  } 

  SOC_PPC_FRWRD_IPV4_GLBL_INFO_print(&prm_glbl_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: uc_route_add (section frwrd_ipv4)
 */
int 
  ui_ppd_api_frwrd_ipv4_uc_route_add(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY   
    prm_route_key;
  SOC_PPC_FEC_ID   
    prm_fec_id = 0xffffffff;
  SOC_SAND_SUCCESS_FAILURE   
    prm_success;
  uint32
	  ip_address_incr = 0,
	  fec_id_incr = 0,
	  count = 0,
	  i, 
	  subnet_from;
  uint8
	  mask_len;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv4"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv4_uc_route_add"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY_clear(&prm_route_key);
  prm_route_key.subnet.prefix_len = 32;
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_UC_ROUTE_ADD_UC_ROUTE_ADD_NUMBER_OF_ENTRIES,1))          
  {  
	  UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_UC_ROUTE_ADD_UC_ROUTE_ADD_NUMBER_OF_ENTRIES);  
	  UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
	  count = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_UC_ROUTE_ADD_UC_ROUTE_ADD_ROUTE_KEY_SUBNET_IP_ADDRESS_INCR,1))          
  {  
	  UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_UC_ROUTE_ADD_UC_ROUTE_ADD_ROUTE_KEY_SUBNET_IP_ADDRESS_INCR);  
	  UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
	  ip_address_incr = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_UC_ROUTE_ADD_UC_ROUTE_ADD_ROUTE_KEY_SUBNET_PREFIX_LEN_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_UC_ROUTE_ADD_UC_ROUTE_ADD_ROUTE_KEY_SUBNET_PREFIX_LEN_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_route_key.subnet.prefix_len = (uint8)param_val->value.ulong_value;
  } 

  if (!get_val_of(
    current_line,(int *)&match_index,SOC_PARAM_PPD_FRWRD_IPV4_UC_ROUTE_ADD_UC_ROUTE_ADD_ROUTE_KEY_SUBNET_IP_ADDRESS_ID,1,  
    &param_val,VAL_IP,err_msg))                                                                
  {                                                                                                  
    prm_route_key.subnet.ip_address = param_val->value.ip_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter route_key after uc_route_add***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_UC_ROUTE_ADD_UC_ROUTE_ADD_FEC_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_UC_ROUTE_ADD_UC_ROUTE_ADD_FEC_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fec_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_UC_ROUTE_ADD_UC_ROUTE_ADD_FEC_ID_INCR,1))          
  {  
	  UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_UC_ROUTE_ADD_UC_ROUTE_ADD_FEC_ID_INCR);  
	  UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
	  fec_id_incr = (uint32)param_val->value.ulong_value;
  } 

  if (count>1)
  {
	  if (ip_address_incr<0)
	  {
		  send_string_to_screen(" *** SW error - parameter ip_address_incr is out of range***", TRUE); 
		  goto exit; 
	  }
	  if (fec_id_incr<0)
	  {
		  send_string_to_screen(" *** SW error - parameter fec_id_incr is out of range***", TRUE); 
		  goto exit; 
	  }
	  mask_len = SOC_SAND_PP_IPV4_SUBNET_PREF_MAX_LEN - prm_route_key.subnet.prefix_len;
	  subnet_from = prm_route_key.subnet.ip_address >> mask_len;
	  prm_fec_id -= fec_id_incr;

	  for (i=0; i<count; i++) 
	  {
		  prm_route_key.subnet.ip_address = subnet_from;
		  subnet_from += ip_address_incr;
		  prm_route_key.subnet.ip_address = prm_route_key.subnet.ip_address << mask_len;
		  prm_fec_id += fec_id_incr;
		
		  ret = soc_ppd_frwrd_ipv4_uc_route_add(
			  unit,
			  &prm_route_key,
			  prm_fec_id,
			  &prm_success
			  );
		  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
		  { 
			  send_string_to_screen(" *** soc_ppd_frwrd_ipv4_uc_route_add - FAIL", TRUE); 
			  soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv4_uc_route_add");   
			  goto exit; 
		  } 
      if (prm_success != SOC_SAND_SUCCESS)
      {
        cli_out("success: %s\n\r",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));
        cli_out("index: %u \n\r", i);
        SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY_print(&prm_route_key);
        cli_out("\n\r");
        cli_out("prm_fec_id: %u \n\r", prm_fec_id);
        goto exit;
      }
	  }
  }
  else
  {
	  /* Call function */
	  ret = soc_ppd_frwrd_ipv4_uc_route_add(
		  unit,
		  &prm_route_key,
		  prm_fec_id,
		  &prm_success
		  );
	  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
	  { 
		  send_string_to_screen(" *** soc_ppd_frwrd_ipv4_uc_route_add - FAIL", TRUE); 
		  soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv4_uc_route_add");   
		  goto exit; 
	  } 
  }

  cli_out("success: %s\n\r",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: uc_route_get (section frwrd_ipv4)
 */
int 
  ui_ppd_api_frwrd_ipv4_uc_route_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY   
    prm_route_key;
  uint8   
    prm_exact_match = TRUE;
  SOC_PPC_FEC_ID   
    prm_fec_id;
  SOC_PPC_FRWRD_IP_ROUTE_STATUS   
    prm_route_status;
  SOC_PPC_FRWRD_IP_ROUTE_LOCATION   
    prm_location;
  uint8   
    prm_found;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv4"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv4_uc_route_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY_clear(&prm_route_key);
  prm_route_key.subnet.prefix_len = 32;

 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_UC_ROUTE_GET_UC_ROUTE_GET_ROUTE_KEY_SUBNET_PREFIX_LEN_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_UC_ROUTE_GET_UC_ROUTE_GET_ROUTE_KEY_SUBNET_PREFIX_LEN_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_route_key.subnet.prefix_len = (uint8)param_val->value.ulong_value;
  } 
  if (!get_val_of(
    current_line,(int *)&match_index,SOC_PARAM_PPD_FRWRD_IPV4_UC_ROUTE_GET_UC_ROUTE_GET_ROUTE_KEY_SUBNET_IP_ADDRESS_ID,1,  
    &param_val,VAL_IP,err_msg))                                                                
  {                                                                                                  
    prm_route_key.subnet.ip_address = param_val->value.ip_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter route_key after uc_route_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_UC_ROUTE_GET_UC_ROUTE_GET_EXACT_MATCH_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_UC_ROUTE_GET_UC_ROUTE_GET_EXACT_MATCH_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_exact_match = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_frwrd_ipv4_uc_route_get(
          unit,
          &prm_route_key,
          prm_exact_match,
          &prm_fec_id,
          &prm_route_status,
          &prm_location,
          &prm_found
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_ipv4_uc_route_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv4_uc_route_get");   
    goto exit; 
  } 

  cli_out("fec_id: %u\n\r",prm_fec_id);

  cli_out("route_status: %s\n\r",SOC_PPC_FRWRD_IP_ROUTE_STATUS_to_string(prm_route_status));

  cli_out("location: %s\n\r",SOC_PPC_FRWRD_IP_ROUTE_LOCATION_to_string(prm_location));

  cli_out("found: %u\n\r",prm_found);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: uc_route_get_block (section frwrd_ipv4)
 */
int 
  ui_ppd_api_frwrd_ipv4_uc_route_get_block(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_IP_ROUTING_TABLE_RANGE   
    prm_block_range;
  SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY   
    prm_route_keys[UI_PPD_API_FRWRD_IPV4_GET_BLOCK_NOF_ENTRIES];
  SOC_PPC_FEC_ID   
    prm_fec_ids[UI_PPD_API_FRWRD_IPV4_GET_BLOCK_NOF_ENTRIES];
  SOC_PPC_FRWRD_IP_ROUTE_STATUS   
    prm_routes_status[UI_PPD_API_FRWRD_IPV4_GET_BLOCK_NOF_ENTRIES];
  SOC_PPC_FRWRD_IP_ROUTE_LOCATION   
    prm_routes_location[UI_PPD_API_FRWRD_IPV4_GET_BLOCK_NOF_ENTRIES];
  char  
    decimal_ip[SOC_SAND_PP_IPV4_MAX_IP_STRING];
  uint32
	  prm_nof_entries,
	  i,
	  fec_id = -1,
	  print_all = 0,
	  count_only = 0,
	  print_count = 0,
	  cur_indx,
	  nof_entries_to_print,
	  total_to_print;
  char
	  user_msg[5] = "";
  uint32
	  con;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv4"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv4_uc_route_get_block"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_IP_ROUTING_TABLE_RANGE_clear(&prm_block_range);
  prm_block_range.start.type = SOC_PPC_IP_ROUTING_TABLE_ITER_TYPE_PREFIX_IP_ORDERED;
 
  /* Get parameters */ 
  /*
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_UC_ROUTE_GET_BLOCK_UC_ROUTE_GET_BLOCK_BLOCK_RANGE_ENTRIES_TO_ACT_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_UC_ROUTE_GET_BLOCK_UC_ROUTE_GET_BLOCK_BLOCK_RANGE_ENTRIES_TO_ACT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_block_range.entries_to_act = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_UC_ROUTE_GET_BLOCK_UC_ROUTE_GET_BLOCK_BLOCK_RANGE_ENTRIES_TO_SCAN_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_UC_ROUTE_GET_BLOCK_UC_ROUTE_GET_BLOCK_BLOCK_RANGE_ENTRIES_TO_SCAN_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_block_range.entries_to_scan = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_UC_ROUTE_GET_BLOCK_UC_ROUTE_GET_BLOCK_BLOCK_RANGE_START_PAYLOAD_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_UC_ROUTE_GET_BLOCK_UC_ROUTE_GET_BLOCK_BLOCK_RANGE_START_PAYLOAD_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    soc_sand_u64_add_long(&prm_block_range.start.payload, param_val->value.ulong_value);
  } 
  */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_UC_ROUTE_GET_BLOCK_UC_ROUTE_GET_BLOCK_BLOCK_RANGE_START_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_IPV4_UC_ROUTE_GET_BLOCK_UC_ROUTE_GET_BLOCK_BLOCK_RANGE_START_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_block_range.start.type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_UC_ROUTE_GET_BLOCK_UC_ROUTE_GET_BLOCK_RULE_KEY_RULE_FEC_ID,1)) 
  { 
	  UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_UC_ROUTE_GET_BLOCK_UC_ROUTE_GET_BLOCK_RULE_KEY_RULE_FEC_ID);  
	  UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
	  fec_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_UC_ROUTE_GET_BLOCK_UC_ROUTE_GET_BLOCK_COUNT_ONLY,1)) 
  { 
	  UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_UC_ROUTE_GET_BLOCK_UC_ROUTE_GET_BLOCK_COUNT_ONLY);  
	  UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
	  count_only = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_UC_ROUTE_GET_BLOCK_UC_ROUTE_GET_BLOCK_PRINT_ALL,1)) 
  { 
	  UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_UC_ROUTE_GET_BLOCK_UC_ROUTE_GET_BLOCK_PRINT_ALL);  
	  UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
	  print_all = (uint32)param_val->value.ulong_value;
  } 

  if (count_only || fec_id != -1)
	  print_all = 1;

  cli_out(" ------------------------------------------------------------------------------\n\r"
          "|                                   Routing Table                              |\n\r"
          "|------------------------------------------------------------------------------|\n\r"
          "|  Destination     | Fec    | Status      | Location                           |\n\r"
          "|     Subnet       |        |             |                                    |\n=\r"
          " ------------------------------------------------------------------------------ \n\r"
          );

  prm_block_range.entries_to_act = UI_PPD_API_FRWRD_IPV4_GET_BLOCK_NOF_ENTRIES;
  prm_block_range.entries_to_scan = 32 * 1024;

  while(!SOC_PPC_IP_ROUTING_TABLE_ITER_IS_END(&prm_block_range.start.payload))
  {
    /* Call function */
    ret = soc_ppd_frwrd_ipv4_uc_route_get_block(
            unit,
            &prm_block_range,
            prm_route_keys,
            prm_fec_ids,
            prm_routes_status,
            prm_routes_location,
            &prm_nof_entries
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
    { 
      send_string_to_screen(" *** soc_ppd_frwrd_ipv4_uc_route_get_block - FAIL", TRUE); 
      soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv4_uc_route_get_block");   
      goto exit; 
    } 

	cur_indx = 0;
	total_to_print = prm_nof_entries;
	
	while (cur_indx <= total_to_print)
	{
		if (prm_nof_entries == 0)
		{
			break;
		}

		if (!print_all)
		{
			/* print one block each time and wait */
			nof_entries_to_print = SOC_SAND_MIN(UI_PPD_BLOCK_SIZE, total_to_print - cur_indx );
		}
		else 
		{
			/* print everything without stopping */
			nof_entries_to_print = total_to_print - cur_indx;
		}

		/* print */
		for(i = 0; i < nof_entries_to_print; i++)
		{
			if (fec_id != -1 && fec_id == prm_fec_ids[i])
			{
				if (!count_only)
				{
					/* print only entries with this fec_id */
					soc_sand_pp_ip_long_to_string(prm_route_keys[i].subnet.ip_address,1,decimal_ip);
					sal_sprintf(decimal_ip,"%1s/%-1u",decimal_ip,prm_route_keys[i].subnet.prefix_len);

					cli_out("|%-18s", decimal_ip);
					cli_out("|%-8d", prm_fec_ids[i]);

					cli_out("|%-13s", SOC_PPC_FRWRD_IP_ROUTE_STATUS_to_string(prm_routes_status[i]));
					cli_out("|%-36s|", SOC_PPC_FRWRD_IP_ROUTE_LOCATION_to_string(prm_routes_location[i]));

					cli_out("\n\r");
				}
				print_count++;
			}
			else
			{
				if (fec_id == -1)
				{
					/* no printing rules */
					if (!count_only)
					{
						/* print only entries with this fec_id */
						soc_sand_pp_ip_long_to_string(prm_route_keys[i].subnet.ip_address,1,decimal_ip);
						sal_sprintf(decimal_ip,"%1s/%-1u",decimal_ip,prm_route_keys[i].subnet.prefix_len);

						cli_out("|%-18s", decimal_ip);
						cli_out("|%-8d", prm_fec_ids[i]);

						cli_out("|%-13s", SOC_PPC_FRWRD_IP_ROUTE_STATUS_to_string(prm_routes_status[i]));
						cli_out("|%-36s|", SOC_PPC_FRWRD_IP_ROUTE_LOCATION_to_string(prm_routes_location[i]));

						cli_out("\n\r");
					}
					print_count++;
				}
			}
		}
		
		/*cli_out("total entries to print %u \n\r", prm_nof_entries);*/
		/*cli_out("%u more entries to print\n\r", total_to_print - cur_indx);*/
		
		cur_indx += nof_entries_to_print;

		if (!print_all)
		{
			cli_out("total entries to print %u \n\r", print_count);

			if ( cur_indx < total_to_print)
			{
				cli_out("Hit Space to continue/Any Key to Abort.\n\r");
				con = ask_get(user_msg,EXPECT_CONT_ABORT,FALSE,TRUE) ;
				if (!con)
				{
					break;
				}
			}
			else
			{
				/* exit the inner while by setting cur_indx > total_to_print */
				cur_indx++;
			}
		}
		else
		{
			/* exit the inner while by setting cur_indx > total_to_print */
			cur_indx = total_to_print +1;
		}
	}
  }

  if (print_all)
	  cli_out("total entries to print %u \n\r", print_count);
  
  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: uc_route_remove (section frwrd_ipv4)
 */
int 
  ui_ppd_api_frwrd_ipv4_uc_route_remove(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret,
	i,
	ip_address_incr = 0,
	count = 0,
	subnet_from;
  uint8
	  mask_len;
  SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY   
    prm_route_key;
  SOC_SAND_SUCCESS_FAILURE   
    prm_success;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv4"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv4_uc_route_remove"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY_clear(&prm_route_key);
  prm_route_key.subnet.prefix_len = 32;

 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_UC_ROUTE_REMOVE_UC_ROUTE_REMOVE_ROUTE_KEY_SUBNET_PREFIX_LEN_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_UC_ROUTE_REMOVE_UC_ROUTE_REMOVE_ROUTE_KEY_SUBNET_PREFIX_LEN_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_route_key.subnet.prefix_len = (uint8)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_UC_ROUTE_REMOVE_UC_ROUTE_REMOVE_ROUTE_KEY_SUBNET_IP_ADDRESS_INCR,1))          
  {  
	  UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_UC_ROUTE_REMOVE_UC_ROUTE_REMOVE_ROUTE_KEY_SUBNET_IP_ADDRESS_INCR);  
	  UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
	  ip_address_incr = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_UC_ROUTE_REMOVE_UC_ROUTE_REMOVE_NUMBER_OF_ENTRIES,1))          
  {  
	  UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_UC_ROUTE_REMOVE_UC_ROUTE_REMOVE_NUMBER_OF_ENTRIES);  
	  UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
	  count = (uint32)param_val->value.ulong_value;
  } 
  if (!get_val_of(
    current_line,(int *)&match_index,SOC_PARAM_PPD_FRWRD_IPV4_UC_ROUTE_REMOVE_UC_ROUTE_REMOVE_ROUTE_KEY_SUBNET_IP_ADDRESS_ID,1,  
    &param_val,VAL_IP,err_msg))                                                                
  {                                                                                                  
    prm_route_key.subnet.ip_address = param_val->value.ip_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter route_key after uc_route_remove***", TRUE); 
    goto exit; 
  } 
  
  if (count>1)
  {
	  /* remove a range of ip_addresses */
	  if (ip_address_incr<0)
	  {
		  send_string_to_screen(" *** SW error - parameter ip_address_incr is out of range***", TRUE); 
		  goto exit; 
	  }
	  mask_len = SOC_SAND_PP_IPV4_SUBNET_PREF_MAX_LEN - prm_route_key.subnet.prefix_len;
	  subnet_from = prm_route_key.subnet.ip_address >> mask_len;

	  for (i=0; i<count; i++) 
	  {
		  prm_route_key.subnet.ip_address = subnet_from;
		  subnet_from += ip_address_incr;
		  prm_route_key.subnet.ip_address = prm_route_key.subnet.ip_address << mask_len;

		  /* Call function */
		  ret = soc_ppd_frwrd_ipv4_uc_route_remove(
			  unit,
			  &prm_route_key,
			  &prm_success
			  );
		  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
		  { 
			  send_string_to_screen(" *** soc_ppd_frwrd_ipv4_uc_route_remove - FAIL", TRUE); 
			  soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv4_uc_route_remove");   
			  goto exit; 
		  } 
	  }
  }
  else
  {
	  /* Call function */
	  ret = soc_ppd_frwrd_ipv4_uc_route_remove(
		  unit,
		  &prm_route_key,
		  &prm_success
		  );
	  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
	  { 
		  send_string_to_screen(" *** soc_ppd_frwrd_ipv4_uc_route_remove - FAIL", TRUE); 
		  soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv4_uc_route_remove");   
		  goto exit; 
	  } 
  }

  cli_out("success: %s\n\r",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: uc_routing_table_clear (section frwrd_ipv4)
 */
int 
  ui_ppd_api_frwrd_ipv4_uc_routing_table_clear(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv4"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv4_uc_routing_table_clear"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_ppd_frwrd_ipv4_uc_routing_table_clear(
          unit
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_ipv4_uc_routing_table_clear - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv4_uc_routing_table_clear");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  

/******************************************************************** 
 *  Function handler: cache_mode_set (section frwrd_ipv4)
 */
int 
  ui_ppd_api_frwrd_ipv4_cache_mode_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32
    prm_vrf_ndx=0;
  uint32                                
    prm_route_types=0;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv4"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv4_cache_mode_set"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_CACHE_MODE_SET_VRF_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_CACHE_MODE_SET_VRF_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_vrf_ndx = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_CACHE_MODE_SET_MODE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_IPV4_CACHE_MODE_SET_MODE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_route_types = (uint32)param_val->numeric_equivalent;
  } 

  /* Call function */
  ret = soc_ppd_frwrd_ip_routes_cache_mode_enable_set(
          unit,
          prm_vrf_ndx,
          prm_route_types
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_ipv4_cache_mode_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv4_cache_mode_set");   
    goto exit; 
  } 

 
  goto exit; 
exit: 
  return ui_ret; 
} 


/******************************************************************** 
 *  Function handler: cache_mode_get (section frwrd_ipv4)
 */
int 
  ui_ppd_api_frwrd_ipv4_cache_mode_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32
    prm_vrf_ndx=0;
  uint32                                
    prm_route_types;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv4"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv4_cache_mode_get"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_CACHE_MODE_GET_VRF_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_CACHE_MODE_GET_VRF_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_vrf_ndx = (uint32)param_val->value.ulong_value;
  } 

  /* Call function */
  ret = soc_ppd_frwrd_ip_routes_cache_mode_enable_get(
          unit,
          prm_vrf_ndx,
          &prm_route_types
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_ipv4_cache_mode_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv4_cache_mode_get");   
    goto exit; 
  } 

  cli_out("route_status: %s\n\r",SOC_PPC_FRWRD_IP_CACHE_MODE_to_string(prm_route_types));
  
 
  goto exit; 
exit: 
  return ui_ret; 
} 

/******************************************************************** 
 *  Function handler: commit (section frwrd_ipv4)
 */
int 
  ui_ppd_api_frwrd_ipv4_commit(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32
    prm_vrf_ndx=0;
  SOC_SAND_SUCCESS_FAILURE   
    prm_success;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv4"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv4_commit"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_COMMIT_VRF_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_COMMIT_VRF_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_vrf_ndx = (uint32)param_val->value.ulong_value;
  } 

  /* Call function */
  ret = soc_ppd_frwrd_ip_routes_cache_commit(
          unit,
          SOC_PPC_FRWRD_IP_CACHE_MODE_IPV4_UC_LPM,
          prm_vrf_ndx,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_ipv4_commit - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv4_commit");   
    goto exit; 
  } 

  cli_out("success: %s\n\r",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));  
 
  goto exit; 
exit: 
  return ui_ret; 
} 

/******************************************************************** 
 *  Function handler: host_add (section frwrd_ipv4)
 */
int 
  ui_ppd_api_frwrd_ipv4_host_add(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
	  ret,
	  ip_address_incr = 0,
	  fec_id_incr = 0,
	  eep_incr = 0,
	  count = 0,
	  i, 
	  subnet_from;
  SOC_PPC_FRWRD_IPV4_HOST_KEY   
    prm_host_key;
  SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO   
    prm_routing_info;
  SOC_SAND_SUCCESS_FAILURE   
    prm_success;
  uint8
	  mask_len;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv4"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv4_host_add"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_IPV4_HOST_KEY_clear(&prm_host_key);
  SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO_clear(&prm_routing_info);
 
  /* Get parameters */ 
  if (!get_val_of(
    current_line,(int *)&match_index,SOC_PARAM_PPD_FRWRD_IPV4_HOST_ADD_HOST_ADD_HOST_KEY_IP_ADDRESS_ID,1,  
    &param_val,VAL_IP,err_msg))                                                                
  {                                                                                                  
    prm_host_key.ip_address = (uint32)param_val->value.ip_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_HOST_ADD_HOST_ADD_NUMBER_OF_ENTRIES,1))          
  {  
	  UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_HOST_ADD_HOST_ADD_NUMBER_OF_ENTRIES);  
	  UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
	  count = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_HOST_ADD_HOST_ADD_HOST_KEY_IP_ADDRESS_INCR,1))          
  {  
	  UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_HOST_ADD_HOST_ADD_HOST_KEY_IP_ADDRESS_INCR);  
	  UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
	  ip_address_incr = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_HOST_ADD_HOST_ADD_HOST_KEY_VRF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_HOST_ADD_HOST_ADD_HOST_KEY_VRF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_host_key.vrf_ndx = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_HOST_ADD_HOST_ADD_ROUTING_INFO_EEP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_HOST_ADD_HOST_ADD_ROUTING_INFO_EEP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_routing_info.eep = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_HOST_ADD_HOST_ADD_ROUTING_INFO_EEP_INCR,1)) 
  { 
	  UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_HOST_ADD_HOST_ADD_ROUTING_INFO_EEP_INCR);  
	  UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
	  eep_incr = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_HOST_ADD_HOST_ADD_ROUTING_INFO_FEC_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_HOST_ADD_HOST_ADD_ROUTING_INFO_FEC_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_routing_info.fec_id = (uint32)param_val->value.ulong_value;
    prm_routing_info.frwrd_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_FEC;
    prm_routing_info.frwrd_decision.dest_id = prm_routing_info.fec_id;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_HOST_ADD_HOST_ADD_ROUTING_INFO_FEC_ID_INCR,1))          
  {  
	  UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_HOST_ADD_HOST_ADD_ROUTING_INFO_FEC_ID_INCR);  
	  UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
	  fec_id_incr = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  if (count>1)
  {
	  if (ip_address_incr<0)
	  {
		  send_string_to_screen(" *** SW error - parameter ip_address_incr is out of range***", TRUE); 
		  goto exit; 
	  }
	  if (fec_id_incr<0)
	  {
		  send_string_to_screen(" *** SW error - parameter fec_id_incr is out of range***", TRUE); 
		  goto exit; 
	  }
	  if (eep_incr<0)
	  {
		  send_string_to_screen(" *** SW error - parameter eep_incr is out of range***", TRUE); 
		  goto exit; 
	  }
	  mask_len = SOC_SAND_PP_IPV4_SUBNET_PREF_MAX_LEN - 32;
	  subnet_from = prm_host_key.ip_address >> mask_len;
	  prm_routing_info.fec_id += fec_id_incr;
      prm_routing_info.frwrd_decision.dest_id += fec_id_incr;
	  prm_routing_info.eep += eep_incr;

	  for (i=0; i<count; i++) 
	  {
		  prm_host_key.ip_address = subnet_from;
		  subnet_from += ip_address_incr;
		  prm_host_key.ip_address = prm_host_key.ip_address << mask_len;
		  prm_routing_info.fec_id += fec_id_incr;
          prm_routing_info.frwrd_decision.dest_id += fec_id_incr;
		  prm_routing_info.eep += eep_incr;

      if (prm_routing_info.eep != 0xffffffff)
      {
		    ret = soc_ppd_frwrd_ipv4_host_add(
			    unit,
			    &prm_host_key,
			    &prm_routing_info,
			    &prm_success
			    );
		    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
		    { 
			    send_string_to_screen(" *** soc_ppd_frwrd_ipv4_host_add - FAIL", TRUE); 
			    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv4_host_add");   
			    goto exit; 
		    } 

        if (prm_success != SOC_SAND_SUCCESS)
        {
          cli_out("success: %s\n\r",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));
          cli_out("index: %u \n\r", i);
          SOC_PPC_FRWRD_IPV4_HOST_KEY_print(&prm_host_key);
          cli_out("\n\r");
          SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO_print(&prm_routing_info);
          cli_out("\n\r");
          goto exit;
        }
      }
      else{
        		  ret = soc_ppd_frwrd_ipv4_host_remove(
			  unit,
			  &prm_host_key
			  );
      }
	  }
  }
  else
  {
	ret = soc_ppd_frwrd_ipv4_host_add(
			unit,
			&prm_host_key,
			&prm_routing_info,
			&prm_success
			);
	if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
	{ 
	    send_string_to_screen(" *** soc_ppd_frwrd_ipv4_host_add - FAIL", TRUE); 
	    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv4_host_add");   
	    goto exit; 
	} 
  }

  cli_out("success: %s\n\r",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: host_get (section frwrd_ipv4)
 */
int 
  ui_ppd_api_frwrd_ipv4_host_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_FRWRD_IPV4_HOST_KEY   
    prm_host_key;
  SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO   
    prm_routing_info;
  SOC_PPC_FRWRD_IP_ROUTE_STATUS   
    prm_route_status;
  SOC_PPC_FRWRD_IP_ROUTE_LOCATION   
    prm_location;
  uint8   
    prm_found;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv4"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv4_host_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_IPV4_HOST_KEY_clear(&prm_host_key);
  SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO_clear(&prm_routing_info);
 
  /* Get parameters */ 
  if (!get_val_of(
    current_line,(int *)&match_index,SOC_PARAM_PPD_FRWRD_IPV4_HOST_GET_HOST_GET_HOST_KEY_IP_ADDRESS_ID,1,  
    &param_val,VAL_IP,err_msg))                                                                
  {                                                                                                  
    prm_host_key.ip_address = (uint32)param_val->value.ip_value;
  } 


  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_HOST_GET_HOST_GET_HOST_KEY_VRF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_HOST_GET_HOST_GET_HOST_KEY_VRF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_host_key.vrf_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter host_key after host_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_frwrd_ipv4_host_get(
          unit,
          &prm_host_key,
          SOC_PPC_FRWRD_IP_HOST_CLEAR_ON_GET,
          &prm_routing_info,
          &prm_route_status,
          &prm_location,
          &prm_found
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_ipv4_host_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv4_host_get");   
    goto exit; 
  } 

  SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO_print(&prm_routing_info);

  cli_out("route_status: %s\n\r",SOC_PPC_FRWRD_IP_ROUTE_STATUS_to_string(prm_route_status));

  cli_out("location: %s\n\r",SOC_PPC_FRWRD_IP_ROUTE_LOCATION_to_string(prm_location));

  cli_out("found: %u\n\r",prm_found);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: host_get_block (section frwrd_ipv4)
 */
int 
  ui_ppd_api_frwrd_ipv4_host_get_block(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_SAND_TABLE_BLOCK_RANGE   
    prm_block_range_key;
  SOC_PPC_FRWRD_IPV4_HOST_KEY   
    prm_host_keys[UI_PPD_API_FRWRD_IPV4_GET_BLOCK_NOF_ENTRIES];
  SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO   
    prm_routes_info[UI_PPD_API_FRWRD_IPV4_GET_BLOCK_NOF_ENTRIES];
  SOC_PPC_FRWRD_IP_ROUTE_STATUS   
    prm_routes_status[UI_PPD_API_FRWRD_IPV4_GET_BLOCK_NOF_ENTRIES];
  uint32
    indx,
	eep = -1,
	fec_id = -1,
	count_only = 0,
    flags=SOC_PPC_FRWRD_IP_HOST_GET_ACCESSS_STATUS,
	print_count = 0;
  uint8
    done = FALSE,
	printed_once = FALSE;
  uint32   
    prm_nof_entries;
  char  
    decimal_ip[SOC_SAND_PP_IPV4_MAX_IP_STRING];
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv4"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv4_host_get_block"; 
 
  unit = soc_ppd_get_default_unit(); 
  soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&prm_block_range_key);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_HOST_GET_BLOCK_HOST_GET_BLOCK_ID_RULE_KEY_RULE_FEC_ID,1))          
  {  
	  UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_HOST_GET_BLOCK_HOST_GET_BLOCK_ID_RULE_KEY_RULE_FEC_ID);  
	  UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
	  fec_id = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_HOST_GET_BLOCK_HOST_GET_BLOCK_ID_RULE_KEY_RULE_EEP,1))          
  {  
	  UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_HOST_GET_BLOCK_HOST_GET_BLOCK_ID_RULE_KEY_RULE_EEP);  
	  UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
	  eep = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_HOST_GET_BLOCK_HOST_GET_BLOCK_ID_RULE_KEY_ACCESSED_ONLY,1))          
  {  
      UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_HOST_GET_BLOCK_HOST_GET_BLOCK_ID_RULE_KEY_ACCESSED_ONLY);  
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
      flags |= SOC_PPC_FRWRD_IP_HOST_GET_ACCESSED_ONLY;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_HOST_GET_BLOCK_HOST_GET_BLOCK_ID_ACCESSED_CLEAR_ID,1))          
  {  
      UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_HOST_GET_BLOCK_HOST_GET_BLOCK_ID_ACCESSED_CLEAR_ID);  
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
      flags |= SOC_PPC_FRWRD_IP_HOST_CLEAR_ON_GET;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_HOST_GET_BLOCK_HOST_GET_BLOCK_ID_COUNT,1))          
  {  
	  UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_HOST_GET_BLOCK_HOST_GET_BLOCK_ID_COUNT);  
	  UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
	  count_only = (uint32)param_val->value.ulong_value;
  } 

  prm_block_range_key.entries_to_act = UI_PPD_API_FRWRD_IPV4_GET_BLOCK_NOF_ENTRIES;
  prm_block_range_key.entries_to_scan = 64*1024+32;
  prm_block_range_key.iter = 0;

  while (!done)
  {
    /* Call function */
    ret = soc_ppd_frwrd_ipv4_host_get_block(
            unit,
            &prm_block_range_key,
            flags,
            prm_host_keys,
            prm_routes_info,
            prm_routes_status,
            &prm_nof_entries
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
    { 
      send_string_to_screen(" *** soc_ppd_frwrd_ipv4_host_get_block - FAIL", TRUE); 
      soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv4_host_get_block");   
      goto exit; 
    } 
    if (prm_nof_entries == 0)
    {
      done = TRUE;
      /*goto exit;*/
    }

	if (!count_only && !printed_once && !done)
	{
		cli_out(" ------------------------------------------------------------------------- \n\r"
                        "|                          Host   Table                               |Acc|\n\r"
                        " ------------------------------------------------------------------------- \n\r"
                        "|  VRF  |     Host        | Destination  |     Additional Info        |   |\n\r"
                        "|       |                 | Type  | Val  | Type |                     |   |\n\r"
                        " ------------------------------------------------------------------------- \n\r"
                        );
	}

    for (indx = 0; indx < prm_nof_entries; ++indx)
    {
		if ((eep == -1 || eep == prm_routes_info[indx].eep) && (fec_id == -1 || fec_id == prm_routes_info[indx].fec_id))
		{
			print_count++;
			if (!count_only)
			{
				cli_out("|  %-5d", prm_host_keys[indx].vrf_ndx);
				soc_sand_pp_ip_long_to_string(prm_host_keys[indx].ip_address, 1,decimal_ip);
				cli_out("| %-16s", decimal_ip);
                SOC_PPC_FRWRD_DECISION_INFO_print_table_format(
                  "",
                  &(prm_routes_info[indx].frwrd_decision)
                );
                if (prm_routes_status[indx] & SOC_PPC_FRWRD_IP_ROUTE_STATUS_ACCESSED) {
                    cli_out("| V ");
                }
                else{
                    cli_out("| X ");
                }
				cli_out("|\n\r");
			}
		}
	}
	if (!count_only && !printed_once)
	{
		cli_out(" ------------------------------------------------------------------------- \n\r"
                        );
		if (eep != -1 || fec_id != -1)
			printed_once = TRUE;
	}
  }

	cli_out("total entries to print %u \n\r", print_count);

  goto exit; 
exit: 
  return ui_ret; 
} 
  

/******************************************************************** 
 *  Function handler: host_get_block (section frwrd_ipv4)
 */
int 
  ui_ppd_api_frwrd_lem_get_block(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   

  ARAD_PP_LEM_ACCESS_KEY            key;
  ARAD_PP_LEM_ACCESS_KEY            key_mask;
  uint32                            access_only=0;
  SOC_SAND_TABLE_BLOCK_RANGE   
    prm_block_range_key;
  ARAD_PP_LEM_ACCESS_KEY   
    prm_host_keys[UI_PPD_API_FRWRD_IPV4_GET_BLOCK_NOF_ENTRIES];
  ARAD_PP_LEM_ACCESS_PAYLOAD   
    prm_routes_info[UI_PPD_API_FRWRD_IPV4_GET_BLOCK_NOF_ENTRIES];
  uint32
    indx,
    prefix = 0,
    count_only = 0,
    print_count = 0;
  uint8
    done = FALSE,
    printed_once = FALSE;
  uint32   
    prm_nof_entries;

   
     
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv4"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv4_host_get_block"; 
 
  unit = soc_ppd_get_default_unit(); 
  soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&prm_block_range_key);
 

  ARAD_PP_LEM_ACCESS_KEY_clear(&key);
  ARAD_PP_LEM_ACCESS_KEY_clear(&key_mask);
  key.prefix.value = 2;


  /* Get parameters */ 

    if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_LEM_GET_BLOCK_LEM_GET_BLOCK_ID_TYPE,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_LEM_GET_BLOCK_LEM_GET_BLOCK_ID_TYPE);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    key.type  = param_val->numeric_equivalent;
     key_mask.prefix.nof_bits = 4;


    switch (key.type) {
        case ARAD_PP_LEM_ACCESS_KEY_TYPE_MAC:
          prefix = ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_ETH(unit);
        break;
        case ARAD_PP_LEM_ACCESS_KEY_TYPE_BACKBONE_MAC:
          prefix = ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_BMAC(unit);
        break;
        case ARAD_PP_LEM_ACCESS_KEY_TYPE_IPV4_MC:
          prefix = ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_IPV4_COMP;
        break;
        case ARAD_PP_LEM_ACCESS_KEY_TYPE_IP_HOST:
          prefix = ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_IP_HOST;
        break;
        case ARAD_PP_LEM_ACCESS_KEY_TYPE_SA_AUTH:
          prefix = ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_SA_AUTH;
        break;
        case ARAD_PP_LEM_ACCESS_KEY_TYPE_ILM:
          prefix = ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_ILM;
        break;
        case ARAD_PP_LEM_ACCESS_KEY_TYPE_EXTENDED:
          prefix = ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_EXTEND;
        break;
        case ARAD_PP_LEM_ACCESS_KEY_TYPE_TRILL_UC:
          prefix = ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_TRILL_UC;
        break;
        case ARAD_PP_LEM_ACCESS_KEY_TYPE_TRILL_MC:
          prefix = ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_TRILL_MC;
        break;
        case ARAD_PP_LEM_ACCESS_KEY_TYPE_IP_SPOOF_DHCP:
          prefix = ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_IP_SPOOF_DHCP;
        break;
        case ARAD_PP_LEM_ACCESS_KEY_TYPE_IPV4_SPOOF_STATIC:
          prefix = ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_IPV4_SPOOF_STATIC;
        break;
        case ARAD_PP_LEM_ACCESS_KEY_TYPE_FC_LOCAL:
          prefix = ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_FC_LOCAL;
        break;
        case ARAD_PP_LEM_ACCESS_KEY_TYPE_FC_REMOTE:
          prefix = ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_FC_REMOTE;
        break;
        case ARAD_PP_LEM_ACCESS_KEY_TYPE_FC_ZONING:
          prefix = ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_FC_ZONING;
        break;
        case ARAD_PP_LEM_ACCESS_KEY_TYPE_IP6_SPOOF_STATIC:
          prefix = ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_IP6_SPOOF_STATIC;
        break;
        case ARAD_PP_LEM_ACCESS_KEY_TYPE_IP6_COMPRESSION_DIP:
          prefix = ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_DIP6_COMPRESSION;
        break;

    default:
        break;
    }
    key_mask.prefix.value = prefix;
  } 
  else{
      key_mask.prefix.value = 0;
      key_mask.prefix.nof_bits = 4;
  }




  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_HOST_GET_BLOCK_HOST_GET_BLOCK_ID_RULE_KEY_ACCESSED_ONLY,1))          
  {  
      UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_HOST_GET_BLOCK_HOST_GET_BLOCK_ID_RULE_KEY_ACCESSED_ONLY);  
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
      access_only = SOC_PPC_FRWRD_IP_HOST_GET_ACCESSED_ONLY;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_HOST_GET_BLOCK_HOST_GET_BLOCK_ID_ACCESSED_CLEAR_ID,1))          
  {  
      UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_HOST_GET_BLOCK_HOST_GET_BLOCK_ID_ACCESSED_CLEAR_ID);  
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
      access_only |= SOC_PPC_FRWRD_IP_HOST_CLEAR_ON_GET;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_HOST_GET_BLOCK_HOST_GET_BLOCK_ID_COUNT,1))          
  {  
	  UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_HOST_GET_BLOCK_HOST_GET_BLOCK_ID_COUNT);  
	  UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
	  count_only = (uint32)param_val->value.ulong_value;
  } 

  prm_block_range_key.entries_to_act = UI_PPD_API_FRWRD_IPV4_GET_BLOCK_NOF_ENTRIES;
  prm_block_range_key.entries_to_scan = 64*1024+32;
  prm_block_range_key.iter = 0;

  prm_nof_entries = 0;

  while (!done)
  {

    ret = arad_pp_frwrd_lem_get_block_unsafe(unit,&key,&key_mask,NULL,access_only ,&prm_block_range_key,prm_host_keys,prm_routes_info,&prm_nof_entries);
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
    { 
      send_string_to_screen(" *** soc_ppd_frwrd_lem_get_block - FAIL", TRUE); 
      soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_lem_get_block");   
      goto exit; 
    } 


    if (prm_nof_entries == 0)
    {
      done = TRUE;
      /*goto exit;*/
    }

	if (!count_only && !printed_once && !done)
	{
		cli_out(" ----------------------------------------------------------------------\n\r"
                        "|                             LEM    Table                                |\n\r"
                        " -------------------------------------------------------------------------\n\r"
                        "|   LKUP    | prefix |              KEY              |       Pyaload      |\n\r"
                        "|   TYPE    |        |                               |                    |\n\r"
                        " -------------------------------------------------------------------------\n\r"
                        );
	}

    for (indx = 0; indx < prm_nof_entries; ++indx)
    {
        print_count++;
        if (!count_only)
        {
            cli_out("|");
            ARAD_PP_LEM_ACCESS_KEY_print(unit, &prm_host_keys[indx]);
            cli_out("|");
            ARAD_PP_LEM_ACCESS_PAYLOAD_print(unit, &prm_routes_info[indx]);
            cli_out("|\n\r");
        }
	}
	if (!count_only && !printed_once)
	{
		cli_out(" ------------------------------------------------------------------------- \n\r"
                        );
	}
  }

	cli_out("total entries to print %u \n\r", print_count);

  goto exit; 
exit: 
  return ui_ret; 
} 

/******************************************************************** 
 *  Function handler: host_remove (section frwrd_ipv4)
 */
int 
  ui_ppd_api_frwrd_ipv4_host_remove(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_FRWRD_IPV4_HOST_KEY   
    prm_host_key;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv4"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv4_host_remove"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_IPV4_HOST_KEY_clear(&prm_host_key);
 
  /* Get parameters */ 
  if (!get_val_of(
    current_line,(int *)&match_index,SOC_PARAM_PPD_FRWRD_IPV4_HOST_REMOVE_HOST_REMOVE_HOST_KEY_IP_ADDRESS_ID,1,  
    &param_val,VAL_IP,err_msg))                                                                
  {                                                                                                  
    prm_host_key.ip_address = (uint32)param_val->value.ip_value;
  } 


  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_HOST_REMOVE_HOST_REMOVE_HOST_KEY_VRF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_HOST_REMOVE_HOST_REMOVE_HOST_KEY_VRF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_host_key.vrf_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter host_key after host_remove***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_frwrd_ipv4_host_remove(
          unit,
          &prm_host_key
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_ipv4_host_remove - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv4_host_remove");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 

/********************************************************************  
 *  Function handler: mc_route_add (section frwrd_ipv4)
 */
int 
  ui_ppd_api_frwrd_ipv4_mc_route_add(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY   
    prm_route_key;
  SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO   
    prm_route_info;
  SOC_SAND_SUCCESS_FAILURE   
    prm_success;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv4"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv4_mc_route_add"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY_clear(&prm_route_key);
  SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO_clear(&prm_route_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_MC_ROUTE_ADD_MC_ROUTE_ADD_ROUTE_KEY_INRIF_VALID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_MC_ROUTE_ADD_MC_ROUTE_ADD_ROUTE_KEY_INRIF_VALID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_route_key.inrif_valid = (uint8)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_MC_ROUTE_ADD_MC_ROUTE_ADD_ROUTE_KEY_INRIF_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_MC_ROUTE_ADD_MC_ROUTE_ADD_ROUTE_KEY_INRIF_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_route_key.inrif = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_MC_ROUTE_ADD_MC_ROUTE_ADD_ROUTE_KEY_SOURCE_PREFIX_LEN_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_MC_ROUTE_ADD_MC_ROUTE_ADD_ROUTE_KEY_SOURCE_PREFIX_LEN_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_route_key.source.prefix_len = (uint8)param_val->value.ulong_value;
  } 
  if (!get_val_of(
    current_line,(int *)&match_index,SOC_PARAM_PPD_FRWRD_IPV4_MC_ROUTE_ADD_MC_ROUTE_ADD_ROUTE_KEY_SOURCE_IP_ADDRESS_ID,1,  
    &param_val,VAL_IP,err_msg))                                                                
  {                                                                                                  
    prm_route_key.source.ip_address = (uint32)param_val->value.ip_value;
  } 
  if (!get_val_of(
    current_line,(int *)&match_index,SOC_PARAM_PPD_FRWRD_IPV4_MC_ROUTE_ADD_MC_ROUTE_ADD_ROUTE_KEY_GROUP_ID,1,  
    &param_val,VAL_IP,err_msg))                                                                
  {                                                                                                  
    prm_route_key.group = (uint32)param_val->value.ip_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter route_key after mc_route_add***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_MC_ROUTE_ADD_MC_ROUTE_ADD_FEC_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_MC_ROUTE_ADD_MC_ROUTE_ADD_FEC_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_route_info.dest_id.dest_val = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_MC_ROUTE_ADD_MC_ROUTE_ADD_ROUTE_INFO_DEST_ID_DEST_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_IPV4_MC_ROUTE_ADD_MC_ROUTE_ADD_ROUTE_INFO_DEST_ID_DEST_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_route_info.dest_id.dest_type = param_val->numeric_equivalent;
  } 

  /* Call function */
  ret = soc_ppd_frwrd_ipv4_mc_route_add(
          unit,
          &prm_route_key,
          &prm_route_info,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_ipv4_mc_route_add - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv4_mc_route_add");   
    goto exit; 
  } 

  cli_out("success: %s\n\r",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: mc_route_get (section frwrd_ipv4)
 */
int 
  ui_ppd_api_frwrd_ipv4_mc_route_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY   
    prm_route_key;
  uint8   
    prm_exact_match = TRUE;
  SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO   
    prm_route_info;
  SOC_PPC_FRWRD_IP_ROUTE_STATUS   
    prm_route_status;
  SOC_PPC_FRWRD_IP_ROUTE_LOCATION   
    prm_location;
  uint8   
    prm_found;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv4"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv4_mc_route_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY_clear(&prm_route_key);
  SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO_clear(&prm_route_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_MC_ROUTE_GET_MC_ROUTE_GET_ROUTE_KEY_INRIF_VALID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_MC_ROUTE_GET_MC_ROUTE_GET_ROUTE_KEY_INRIF_VALID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_route_key.inrif_valid = (uint8)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_MC_ROUTE_GET_MC_ROUTE_GET_ROUTE_KEY_INRIF_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_MC_ROUTE_GET_MC_ROUTE_GET_ROUTE_KEY_INRIF_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_route_key.inrif = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_MC_ROUTE_GET_MC_ROUTE_GET_ROUTE_KEY_SOURCE_PREFIX_LEN_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_MC_ROUTE_GET_MC_ROUTE_GET_ROUTE_KEY_SOURCE_PREFIX_LEN_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_route_key.source.prefix_len = (uint8)param_val->value.ulong_value;
  } 

  if (!get_val_of(
    current_line,(int *)&match_index,SOC_PARAM_PPD_FRWRD_IPV4_MC_ROUTE_GET_MC_ROUTE_GET_ROUTE_KEY_SOURCE_IP_ADDRESS_ID,1,  
    &param_val,VAL_IP,err_msg))                                                                
  {                                                                                                  
    prm_route_key.source.ip_address = (uint32)param_val->value.ip_value;
  } 

  if (!get_val_of(
    current_line,(int *)&match_index,SOC_PARAM_PPD_FRWRD_IPV4_MC_ROUTE_GET_MC_ROUTE_GET_ROUTE_KEY_GROUP_ID,1,  
    &param_val,VAL_IP,err_msg))                                                                
  {                                                                                                  
    prm_route_key.group = (uint32)param_val->value.ip_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter route_key after mc_route_add***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_MC_ROUTE_GET_MC_ROUTE_GET_EXACT_MATCH_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_MC_ROUTE_GET_MC_ROUTE_GET_EXACT_MATCH_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_exact_match = ((uint8)param_val->value.ulong_value == 0x1)?SOC_PPC_FRWRD_IP_EXACT_MATCH:0;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_MC_ROUTE_GET_MC_ROUTE_GET_CLEAR_ACCESSED,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_MC_ROUTE_GET_MC_ROUTE_GET_EXACT_MATCH_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_exact_match |= ((uint8)param_val->value.ulong_value == 0x1)?SOC_PPC_FRWRD_IP_CLEAR_ON_GET:0;
  } 


  /* Call function */
  ret = soc_ppd_frwrd_ipv4_mc_route_get(
          unit,
          &prm_route_key,
          prm_exact_match,
          &prm_route_info,
          &prm_route_status,
          &prm_location,
          &prm_found
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_ipv4_mc_route_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv4_mc_route_get");   
    goto exit; 
  } 

  SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO_print(&prm_route_info);

  cli_out("route_status: %s\n\r",SOC_PPC_FRWRD_IP_ROUTE_STATUS_to_string(prm_route_status));

  cli_out("location: %s\n\r",SOC_PPC_FRWRD_IP_ROUTE_LOCATION_to_string(prm_location));

  cli_out("found: %u\n\r",prm_found);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: mc_route_get_block (section frwrd_ipv4)
 */
int 
  ui_ppd_api_frwrd_ipv4_mc_route_get_block(
    CURRENT_LINE *current_line 
  ) 
{   
 uint32 
    ret;   
  SOC_PPC_IP_ROUTING_TABLE_RANGE   
    prm_block_range;
  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY   
    prm_route_keys[UI_PPD_API_FRWRD_IPV4_GET_BLOCK_NOF_ENTRIES];
  SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO   
    prm_routes_info[UI_PPD_API_FRWRD_IPV4_GET_BLOCK_NOF_ENTRIES];
  SOC_PPC_FRWRD_IP_ROUTE_STATUS   
    prm_routes_status[UI_PPD_API_FRWRD_IPV4_GET_BLOCK_NOF_ENTRIES];
  SOC_PPC_FRWRD_IP_ROUTE_LOCATION   
    prm_routes_location[UI_PPD_API_FRWRD_IPV4_GET_BLOCK_NOF_ENTRIES];
  uint32   
    prm_nof_entries;
  char  
    decimal_ip[SOC_SAND_PP_IPV4_MAX_IP_STRING];
  uint32
    i;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv4"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv4_mc_route_get_block"; 

  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_IP_ROUTING_TABLE_RANGE_clear(&prm_block_range);
  prm_block_range.start.type = SOC_PPC_IP_ROUTING_TABLE_ITER_TYPE_PREFIX_IP_ORDERED;

  /* Get parameters */ 
  /*
    if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_MC_ROUTE_GET_BLOCK_MC_ROUTE_GET_BLOCK_BLOCK_RANGE_KEY_ENTRIES_TO_ACT_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_MC_ROUTE_GET_BLOCK_MC_ROUTE_GET_BLOCK_BLOCK_RANGE_KEY_ENTRIES_TO_ACT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_block_range_key.entries_to_act = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_MC_ROUTE_GET_BLOCK_MC_ROUTE_GET_BLOCK_BLOCK_RANGE_KEY_ENTRIES_TO_SCAN_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_MC_ROUTE_GET_BLOCK_MC_ROUTE_GET_BLOCK_BLOCK_RANGE_KEY_ENTRIES_TO_SCAN_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_block_range_key.entries_to_scan = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_MC_ROUTE_GET_BLOCK_MC_ROUTE_GET_BLOCK_BLOCK_RANGE_KEY_START_PAYLOAD_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_MC_ROUTE_GET_BLOCK_MC_ROUTE_GET_BLOCK_BLOCK_RANGE_KEY_START_PAYLOAD_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    soc_sand_u64_add_long(&prm_block_range_key.start.payload, param_val->value.ulong_value);
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_MC_ROUTE_GET_BLOCK_MC_ROUTE_GET_BLOCK_BLOCK_RANGE_KEY_START_TYPE_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_IPV4_MC_ROUTE_GET_BLOCK_MC_ROUTE_GET_BLOCK_BLOCK_RANGE_KEY_START_TYPE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_block_range_key.start.type = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter block_range_key after mc_route_get_block***", TRUE); 
    goto exit; 
  } 
  */

  cli_out(" ------------------------------------------------------------------------------\n\r"
          "|                                   Routing Table                              |\n\r"
          "|------------------------------------------------------------------------------|\n\r"
          "|     Group       | Inrif |     Source        | Fec  |  Status  |   Location   |\n\r"
          "|                 |       |                   |      |          |              |\n=\r"
          " ------------------------------------------------------------------------------ \n\r"
          );

  prm_block_range.entries_to_act = UI_PPD_API_FRWRD_IPV4_GET_BLOCK_NOF_ENTRIES;
  prm_block_range.entries_to_scan = 32 * 1024;

  while(!SOC_PPC_IP_ROUTING_TABLE_ITER_IS_END(&prm_block_range.start.payload))
  {
    /* Call function */
    ret = soc_ppd_frwrd_ipv4_mc_route_get_block(
            unit,
            &prm_block_range,
            prm_route_keys,
            prm_routes_info,
            prm_routes_status,
            prm_routes_location,
            &prm_nof_entries
            );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
    { 
      send_string_to_screen(" *** soc_ppd_frwrd_ipv4_mc_route_get_block - FAIL", TRUE); 
      soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv4_mc_route_get_block");   
      goto exit; 
    } 

    for(i = 0; i < prm_nof_entries; i++)
    {
      soc_sand_pp_ip_long_to_string(prm_route_keys[i].group, 1,decimal_ip);
      cli_out("|%-16s", decimal_ip);

      if(prm_route_keys[i].inrif_valid)
      {
        cli_out("|%-7d", prm_route_keys[i].inrif);
      }
      else
      {
        cli_out("|%-7c", '-');
      }

      soc_sand_pp_ip_long_to_string(prm_route_keys[i].source.ip_address, 1,decimal_ip);
      sal_sprintf(decimal_ip,"%1s/%-1u",decimal_ip,prm_route_keys[i].source.prefix_len);
      cli_out("|%-18s", decimal_ip);


      cli_out("|%-5d", prm_routes_info[i].dest_id.dest_val);

      cli_out("|%-9s", SOC_PPC_FRWRD_IP_ROUTE_STATUS_to_string(prm_routes_status[i]));
      cli_out("|%-13s|", SOC_PPC_FRWRD_IP_ROUTE_LOCATION_to_string(prm_routes_location[i]));

      cli_out("\n\r");
    }
  }
  
  goto exit; 
exit:
  return ui_ret;
} 
  
/******************************************************************** 
 *  Function handler: mc_route_remove (section frwrd_ipv4)
 */
int 
  ui_ppd_api_frwrd_ipv4_mc_route_remove(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY   
    prm_route_key;
  SOC_SAND_SUCCESS_FAILURE   
    prm_success;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv4"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv4_mc_route_remove"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY_clear(&prm_route_key);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_MC_ROUTE_REMOVE_MC_ROUTE_REMOVE_ROUTE_KEY_INRIF_VALID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_MC_ROUTE_REMOVE_MC_ROUTE_REMOVE_ROUTE_KEY_INRIF_VALID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_route_key.inrif_valid = (uint8)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_MC_ROUTE_REMOVE_MC_ROUTE_REMOVE_ROUTE_KEY_INRIF_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_MC_ROUTE_REMOVE_MC_ROUTE_REMOVE_ROUTE_KEY_INRIF_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_route_key.inrif = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_MC_ROUTE_REMOVE_MC_ROUTE_REMOVE_ROUTE_KEY_SOURCE_PREFIX_LEN_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_MC_ROUTE_REMOVE_MC_ROUTE_REMOVE_ROUTE_KEY_SOURCE_PREFIX_LEN_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_route_key.source.prefix_len = (uint8)param_val->value.ulong_value;
  } 
  if (!get_val_of(
    current_line,(int *)&match_index,SOC_PARAM_PPD_FRWRD_IPV4_MC_ROUTE_REMOVE_MC_ROUTE_REMOVE_ROUTE_KEY_SOURCE_IP_ADDRESS_ID,1,  
    &param_val,VAL_IP,err_msg))                                                                
  {                                                                                                  
    prm_route_key.source.ip_address = (uint32)param_val->value.ulong_value;
  } 
  if (!get_val_of(
    current_line,(int *)&match_index,SOC_PARAM_PPD_FRWRD_IPV4_MC_ROUTE_REMOVE_MC_ROUTE_REMOVE_ROUTE_KEY_GROUP_ID,1,  
    &param_val,VAL_IP,err_msg))                                                                
  {                                                                                                  
    prm_route_key.group = (uint32)param_val->value.ip_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter route_key after mc_route_remove***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_frwrd_ipv4_mc_route_remove(
          unit,
          &prm_route_key,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_ipv4_mc_route_remove - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv4_mc_route_remove");   
    goto exit; 
  } 

  cli_out("success: %s\n\r",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));
  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: mc_routing_table_clear (section frwrd_ipv4)
 */
int 
  ui_ppd_api_frwrd_ipv4_mc_routing_table_clear(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv4"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv4_mc_routing_table_clear"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_ppd_frwrd_ipv4_mc_routing_table_clear(
          unit
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_ipv4_mc_routing_table_clear - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv4_mc_routing_table_clear");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: vrf_info_set (section frwrd_ipv4)
 */
int 
  ui_ppd_api_frwrd_ipv4_vrf_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_VRF_ID   
    prm_vrf_ndx;
  SOC_PPC_FRWRD_IPV4_VRF_INFO   
    prm_vrf_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv4"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv4_vrf_info_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_IPV4_VRF_INFO_clear(&prm_vrf_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_VRF_INFO_SET_VRF_INFO_SET_VRF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_VRF_INFO_SET_VRF_INFO_SET_VRF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_vrf_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter vrf_ndx after vrf_info_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_frwrd_ipv4_vrf_info_get(
          unit,
          prm_vrf_ndx,
          &prm_vrf_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_ipv4_vrf_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv4_vrf_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_VRF_INFO_SET_VRF_INFO_SET_VRF_INFO_USE_DFLT_NON_VRF_ROUTING_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_VRF_INFO_SET_VRF_INFO_SET_VRF_INFO_USE_DFLT_NON_VRF_ROUTING_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_vrf_info.use_dflt_non_vrf_routing = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_VRF_INFO_SET_VRF_INFO_SET_VRF_INFO_ROUTER_INFO_MC_DEFAULT_ACTION_VALUE_ACTION_PROFILE_SNOOP_STRENGTH_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_VRF_INFO_SET_VRF_INFO_SET_VRF_INFO_ROUTER_INFO_MC_DEFAULT_ACTION_VALUE_ACTION_PROFILE_SNOOP_STRENGTH_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_vrf_info.router_info.mc_default_action.value.action_profile.snoop_action_strength = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_VRF_INFO_SET_VRF_INFO_SET_VRF_INFO_ROUTER_INFO_MC_DEFAULT_ACTION_VALUE_ACTION_PROFILE_FRWRD_STRENGTH_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_VRF_INFO_SET_VRF_INFO_SET_VRF_INFO_ROUTER_INFO_MC_DEFAULT_ACTION_VALUE_ACTION_PROFILE_FRWRD_STRENGTH_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_vrf_info.router_info.mc_default_action.value.action_profile.frwrd_action_strength = (uint32)param_val->value.ulong_value;
  } 

  /*trap-code
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_VRF_INFO_SET_VRF_INFO_SET_VRF_INFO_ROUTER_INFO_MC_DEFAULT_ACTION_VALUE_ACTION_PROFILE_TRAP_CODE_LSB_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_IPV4_VRF_INFO_SET_VRF_INFO_SET_VRF_INFO_ROUTER_INFO_MC_DEFAULT_ACTION_VALUE_ACTION_PROFILE_TRAP_CODE_LSB_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_vrf_info.router_info.mc_default_action.value.action_profile.trap_code = (SOC_PPC_TRAP_CODE)param_val->numeric_equivalent;
  } 
  */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_VRF_INFO_SET_VRF_INFO_SET_VRF_INFO_ROUTER_INFO_MC_DEFAULT_ACTION_VALUE_ACTION_PROFILE_TRAP_CODE_LSB_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_VRF_INFO_SET_VRF_INFO_SET_VRF_INFO_ROUTER_INFO_MC_DEFAULT_ACTION_VALUE_ACTION_PROFILE_TRAP_CODE_LSB_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_vrf_info.router_info.mc_default_action.value.action_profile.trap_code = (SOC_PPC_TRAP_CODE)param_val->value.ulong_value;
  } 


  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_VRF_INFO_SET_VRF_INFO_SET_VRF_INFO_ROUTER_INFO_MC_DEFAULT_ACTION_VALUE_ROUTE_VAL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_VRF_INFO_SET_VRF_INFO_SET_VRF_INFO_ROUTER_INFO_MC_DEFAULT_ACTION_VALUE_ROUTE_VAL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_vrf_info.router_info.mc_default_action.value.route_val = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_VRF_INFO_SET_VRF_INFO_SET_VRF_INFO_ROUTER_INFO_MC_DEFAULT_ACTION_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_IPV4_VRF_INFO_SET_VRF_INFO_SET_VRF_INFO_ROUTER_INFO_MC_DEFAULT_ACTION_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_vrf_info.router_info.mc_default_action.type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_VRF_INFO_SET_VRF_INFO_SET_VRF_INFO_ROUTER_INFO_UC_DEFAULT_ACTION_VALUE_ACTION_PROFILE_SNOOP_STRENGTH_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_VRF_INFO_SET_VRF_INFO_SET_VRF_INFO_ROUTER_INFO_UC_DEFAULT_ACTION_VALUE_ACTION_PROFILE_SNOOP_STRENGTH_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_vrf_info.router_info.uc_default_action.value.action_profile.snoop_action_strength = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_VRF_INFO_SET_VRF_INFO_SET_VRF_INFO_ROUTER_INFO_UC_DEFAULT_ACTION_VALUE_ACTION_PROFILE_FRWRD_STRENGTH_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_VRF_INFO_SET_VRF_INFO_SET_VRF_INFO_ROUTER_INFO_UC_DEFAULT_ACTION_VALUE_ACTION_PROFILE_FRWRD_STRENGTH_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_vrf_info.router_info.uc_default_action.value.action_profile.frwrd_action_strength = (uint32)param_val->value.ulong_value;
  } 

  /*trap-code
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_VRF_INFO_SET_VRF_INFO_SET_VRF_INFO_ROUTER_INFO_UC_DEFAULT_ACTION_VALUE_ACTION_PROFILE_TRAP_CODE_LSB_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_IPV4_VRF_INFO_SET_VRF_INFO_SET_VRF_INFO_ROUTER_INFO_UC_DEFAULT_ACTION_VALUE_ACTION_PROFILE_TRAP_CODE_LSB_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_vrf_info.router_info.uc_default_action.value.action_profile.trap_code = (SOC_PPC_TRAP_CODE)param_val->numeric_equivalent;
  } 
  */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_VRF_INFO_SET_VRF_INFO_SET_VRF_INFO_ROUTER_INFO_UC_DEFAULT_ACTION_VALUE_ACTION_PROFILE_TRAP_CODE_LSB_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_VRF_INFO_SET_VRF_INFO_SET_VRF_INFO_ROUTER_INFO_UC_DEFAULT_ACTION_VALUE_ACTION_PROFILE_TRAP_CODE_LSB_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_vrf_info.router_info.uc_default_action.value.action_profile.trap_code = (SOC_PPC_TRAP_CODE)param_val->value.ulong_value;
  } 


  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_VRF_INFO_SET_VRF_INFO_SET_VRF_INFO_ROUTER_INFO_UC_DEFAULT_ACTION_VALUE_ROUTE_VAL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_VRF_INFO_SET_VRF_INFO_SET_VRF_INFO_ROUTER_INFO_UC_DEFAULT_ACTION_VALUE_ROUTE_VAL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_vrf_info.router_info.uc_default_action.value.route_val = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_VRF_INFO_SET_VRF_INFO_SET_VRF_INFO_ROUTER_INFO_UC_DEFAULT_ACTION_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_IPV4_VRF_INFO_SET_VRF_INFO_SET_VRF_INFO_ROUTER_INFO_UC_DEFAULT_ACTION_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_vrf_info.router_info.uc_default_action.type = param_val->numeric_equivalent;
  } 


  /* Call function */
  ret = soc_ppd_frwrd_ipv4_vrf_info_set(
          unit,
          prm_vrf_ndx,
          &prm_vrf_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_ipv4_vrf_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv4_vrf_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: vrf_info_get (section frwrd_ipv4)
 */
int 
  ui_ppd_api_frwrd_ipv4_vrf_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_VRF_ID   
    prm_vrf_ndx;
  SOC_PPC_FRWRD_IPV4_VRF_INFO   
    prm_vrf_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv4"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv4_vrf_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_IPV4_VRF_INFO_clear(&prm_vrf_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_VRF_INFO_GET_VRF_INFO_GET_VRF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_VRF_INFO_GET_VRF_INFO_GET_VRF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_vrf_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter vrf_ndx after vrf_info_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_frwrd_ipv4_vrf_info_get(
          unit,
          prm_vrf_ndx,
          &prm_vrf_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_ipv4_vrf_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv4_vrf_info_get");   
    goto exit; 
  } 

  SOC_PPC_FRWRD_IPV4_VRF_INFO_print(&prm_vrf_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 


/******************************************************************** 
 *  Function handler: vrf_route_add (section frwrd_ipv4)
 */
int 
  ui_ppd_api_frwrd_ipv4_vrf_route_add(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret,
	ip_address_incr = 0,
	fec_id_incr = 0,
	count = 0,
	i, 
	subnet_from;
  SOC_PPC_VRF_ID   
    prm_vrf_ndx;
  SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY   
    prm_route_key;
  SOC_PPC_FEC_ID   
    prm_fec_id = 0xffffffff;
  SOC_SAND_SUCCESS_FAILURE   
    prm_success;
  uint8
	  mask_len;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv4"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv4_vrf_route_add"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY_clear(&prm_route_key);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_VRF_ROUTE_ADD_VRF_ROUTE_ADD_VRF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_VRF_ROUTE_ADD_VRF_ROUTE_ADD_VRF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_vrf_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter vrf_ndx after vrf_route_add***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_VRF_ROUTE_ADD_VRF_ROUTE_ADD_ROUTE_KEY_SUBNET_PREFIX_LEN_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_VRF_ROUTE_ADD_VRF_ROUTE_ADD_ROUTE_KEY_SUBNET_PREFIX_LEN_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_route_key.subnet.prefix_len = (uint8)param_val->value.ulong_value;
  } 

  if (!get_val_of(
    current_line,(int *)&match_index,SOC_PARAM_PPD_FRWRD_IPV4_VRF_ROUTE_ADD_VRF_ROUTE_ADD_ROUTE_KEY_SUBNET_IP_ADDRESS_ID,1,  
    &param_val,VAL_IP,err_msg))                                                                
  {                                                                                                  
    prm_route_key.subnet.ip_address = param_val->value.ip_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter route_key after vrf_route_add***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_VRF_ROUTE_ADD_VRF_ROUTE_ADD_NUMBER_OF_ENTRIES,1))          
  {  
	  UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_VRF_ROUTE_ADD_VRF_ROUTE_ADD_NUMBER_OF_ENTRIES);  
	  UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
	  count = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_VRF_ROUTE_ADD_VRF_ROUTE_ADD_ROUTE_KEY_SUBNET_IP_ADDRESS_INCR,1))          
  {  
	  UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_VRF_ROUTE_ADD_VRF_ROUTE_ADD_ROUTE_KEY_SUBNET_IP_ADDRESS_INCR);  
	  UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
	  ip_address_incr = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_VRF_ROUTE_ADD_VRF_ROUTE_ADD_FEC_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_VRF_ROUTE_ADD_VRF_ROUTE_ADD_FEC_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fec_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_VRF_ROUTE_ADD_VRF_ROUTE_ADD_FEC_ID_INCR,1))          
  {  
	  UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_VRF_ROUTE_ADD_VRF_ROUTE_ADD_FEC_ID_INCR);  
	  UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
	  fec_id_incr = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  if (count>1)
  {
	  if (ip_address_incr<0)
	  {
		  send_string_to_screen(" *** SW error - parameter ip_address_incr is out of range***", TRUE); 
		  goto exit; 
	  }
	  if (fec_id_incr<0)
	  {
		  send_string_to_screen(" *** SW error - parameter fec_id_incr is out of range***", TRUE); 
		  goto exit; 
	  }
	  mask_len = SOC_SAND_PP_IPV4_SUBNET_PREF_MAX_LEN - prm_route_key.subnet.prefix_len;
	  subnet_from = prm_route_key.subnet.ip_address >> mask_len;
	  prm_fec_id -= fec_id_incr;

      for (i = 0; i < count; i++) {
          prm_route_key.subnet.ip_address = subnet_from;
          subnet_from += ip_address_incr;
          prm_route_key.subnet.ip_address = prm_route_key.subnet.ip_address << mask_len;
          prm_fec_id += fec_id_incr;

          ret = soc_ppd_frwrd_ipv4_vrf_route_add(
              unit,
              prm_vrf_ndx,
              &prm_route_key,
              prm_fec_id,
              0, /*flags*/
              &prm_success
              );
          if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) {
              send_string_to_screen(" *** soc_ppd_frwrd_ipv4_vrf_route_add - FAIL", TRUE);
              soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv4_vrf_route_add");
              goto exit;
          }
          if (prm_success != SOC_SAND_SUCCESS) {
              cli_out("success: %s\n\r", soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));
              cli_out("index: %u \n\r", i);
              SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY_print(&prm_route_key);
              cli_out("\n\r");
              cli_out("prm_fec_id: %u \n\r", prm_fec_id);
              goto exit;
          }

      }
  } else {
      ret = soc_ppd_frwrd_ipv4_vrf_route_add(
          unit,
          prm_vrf_ndx,
          &prm_route_key,
          prm_fec_id,
          0, /*flags*/
          &prm_success
          );
      if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) {
          send_string_to_screen(" *** soc_ppd_frwrd_ipv4_vrf_route_add - FAIL", TRUE);
          soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv4_vrf_route_add");
          goto exit;
      }
  }

  cli_out("success: %s\n\r", soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));


  goto exit;
  exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: vrf_route_get (section frwrd_ipv4)
 */
int 
  ui_ppd_api_frwrd_ipv4_vrf_route_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_VRF_ID   
    prm_vrf_ndx;
  SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY   
    prm_route_key;
  uint8   
    prm_exact_match = TRUE;
  SOC_PPC_FEC_ID   
    prm_fec_id;
  SOC_PPC_FRWRD_IP_ROUTE_STATUS   
    prm_route_status;
  SOC_PPC_FRWRD_IP_ROUTE_LOCATION   
    prm_location;
  uint8   
    prm_found;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv4"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv4_vrf_route_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY_clear(&prm_route_key);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_VRF_ROUTE_GET_VRF_ROUTE_GET_VRF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_VRF_ROUTE_GET_VRF_ROUTE_GET_VRF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_vrf_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter vrf_ndx after vrf_route_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_VRF_ROUTE_GET_VRF_ROUTE_GET_ROUTE_KEY_SUBNET_PREFIX_LEN_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_VRF_ROUTE_GET_VRF_ROUTE_GET_ROUTE_KEY_SUBNET_PREFIX_LEN_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_route_key.subnet.prefix_len = (uint8)param_val->value.ulong_value;
  } 

  if (!get_val_of(
    current_line,(int *)&match_index,SOC_PARAM_PPD_FRWRD_IPV4_VRF_ROUTE_GET_VRF_ROUTE_GET_ROUTE_KEY_SUBNET_IP_ADDRESS_ID,1,  
    &param_val,VAL_IP,err_msg))                                                                
  {                                                                                                  
    prm_route_key.subnet.ip_address = param_val->value.ip_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter route_key after uc_route_add***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_VRF_ROUTE_GET_VRF_ROUTE_GET_EXACT_MATCH_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_VRF_ROUTE_GET_VRF_ROUTE_GET_EXACT_MATCH_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_exact_match = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_frwrd_ipv4_vrf_route_get(
          unit,
          prm_vrf_ndx,
          &prm_route_key,
          prm_exact_match,
          0, /* flags */
          &prm_fec_id,
          &prm_route_status,
          &prm_location,
          &prm_found
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_ipv4_vrf_route_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv4_vrf_route_get");   
    goto exit; 
  } 

  cli_out("fec_id: %u\n\r",prm_fec_id);

  cli_out("route_status: %s\n\r",SOC_PPC_FRWRD_IP_ROUTE_STATUS_to_string(prm_route_status));

  cli_out("location: %s\n\r",SOC_PPC_FRWRD_IP_ROUTE_LOCATION_to_string(prm_location));

  cli_out("found: %u\n\r",prm_found);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: vrf_route_get_block (section frwrd_ipv4)
 */
int 
  ui_ppd_api_frwrd_ipv4_vrf_route_get_block(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_VRF_ID   
    prm_vrf_ndx;
  SOC_PPC_IP_ROUTING_TABLE_RANGE   
    prm_block_range_key;
  SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY   
    prm_route_keys[UI_PPD_API_FRWRD_IPV4_GET_BLOCK_NOF_ENTRIES];
  SOC_PPC_FEC_ID   
    prm_fec_ids[UI_PPD_API_FRWRD_IPV4_GET_BLOCK_NOF_ENTRIES];
  SOC_PPC_FRWRD_IP_ROUTE_STATUS   
    prm_routes_status[UI_PPD_API_FRWRD_IPV4_GET_BLOCK_NOF_ENTRIES];
  SOC_PPC_FRWRD_IP_ROUTE_LOCATION   
    prm_routes_location[UI_PPD_API_FRWRD_IPV4_GET_BLOCK_NOF_ENTRIES];
  uint32   
    prm_nof_entries,
	fec_id = -1,
	count_only = 0,
	print_count = 0;
  char  
    decimal_ip[SOC_SAND_PP_IPV4_MAX_IP_STRING];
  uint32
    i;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv4"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv4_vrf_route_get_block"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_IP_ROUTING_TABLE_RANGE_clear(&prm_block_range_key);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_VRF_ROUTE_GET_BLOCK_VRF_ROUTE_GET_BLOCK_VRF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_VRF_ROUTE_GET_BLOCK_VRF_ROUTE_GET_BLOCK_VRF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_vrf_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter vrf_ndx after vrf_route_get_block***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_VRF_ROUTE_GET_BLOCK_VRF_ROUTE_GET_BLOCK_RULE_KEY_RULE_FEC_ID,1))          
  {  
	  UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_VRF_ROUTE_GET_BLOCK_VRF_ROUTE_GET_BLOCK_RULE_KEY_RULE_FEC_ID);  
	  UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
	  fec_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_VRF_ROUTE_GET_BLOCK_VRF_ROUTE_GET_BLOCK_COUNT,1))          
  {  
	  UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_VRF_ROUTE_GET_BLOCK_VRF_ROUTE_GET_BLOCK_COUNT);  
	  UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
	  count_only = (uint32)param_val->value.ulong_value;
  } 

  /*
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_VRF_ROUTE_GET_BLOCK_VRF_ROUTE_GET_BLOCK_BLOCK_RANGE_KEY_ENTRIES_TO_ACT_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_VRF_ROUTE_GET_BLOCK_VRF_ROUTE_GET_BLOCK_BLOCK_RANGE_KEY_ENTRIES_TO_ACT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_block_range_key.entries_to_act = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_VRF_ROUTE_GET_BLOCK_VRF_ROUTE_GET_BLOCK_BLOCK_RANGE_KEY_ENTRIES_TO_SCAN_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_VRF_ROUTE_GET_BLOCK_VRF_ROUTE_GET_BLOCK_BLOCK_RANGE_KEY_ENTRIES_TO_SCAN_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_block_range_key.entries_to_scan = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_VRF_ROUTE_GET_BLOCK_VRF_ROUTE_GET_BLOCK_BLOCK_RANGE_KEY_START_PAYLOAD_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_VRF_ROUTE_GET_BLOCK_VRF_ROUTE_GET_BLOCK_BLOCK_RANGE_KEY_START_PAYLOAD_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    soc_sand_u64_add_long(&prm_block_range_key.start.payload, param_val->value.ulong_value);
  } */

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_VRF_ROUTE_GET_BLOCK_VRF_ROUTE_GET_BLOCK_BLOCK_RANGE_KEY_START_TYPE_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_IPV4_VRF_ROUTE_GET_BLOCK_VRF_ROUTE_GET_BLOCK_BLOCK_RANGE_KEY_START_TYPE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_block_range_key.start.type = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter block_range_key after vrf_route_get_block***", TRUE); 
    goto exit; 
  } 


  if (!count_only)
	cli_out(" ------------------------------------------------------------------------------\n\r"
                "|                                   Routing Table                              |\n\r"
                "|------------------------------------------------------------------------------|\n\r"
                "| VRF | Destination     | Fec    | Status      | Location                      |\n\r"
                "|     |    Subnet       |        |             |                               |\n=\r"
                " ------------------------------------------------------------------------------ \n\r"
                );

  prm_block_range_key.entries_to_act = UI_PPD_API_FRWRD_IPV4_GET_BLOCK_NOF_ENTRIES;
  prm_block_range_key.entries_to_scan = 32 * 1024;

  while(!SOC_PPC_IP_ROUTING_TABLE_ITER_IS_END(&prm_block_range_key.start.payload))
  {
    /* Call function */
    ret = soc_ppd_frwrd_ipv4_vrf_route_get_block(
            unit,
            prm_vrf_ndx,
            &prm_block_range_key,
            prm_route_keys,
            prm_fec_ids,
            prm_routes_status,
            prm_routes_location,
            &prm_nof_entries
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
    { 
      send_string_to_screen(" *** soc_ppd_frwrd_ipv4_vrf_route_get_block - FAIL", TRUE); 
      soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv4_vrf_route_get_block"); 
      goto exit; 
    } 

    for(i = 0; i < prm_nof_entries; i++)
    {
		if (fec_id == -1 || fec_id == prm_fec_ids[i])
		{
			if (!count_only)
			{
				soc_sand_pp_ip_long_to_string(prm_route_keys[i].subnet.ip_address,1,decimal_ip);
				sal_sprintf(decimal_ip,"%1s/%-1u",decimal_ip,prm_route_keys[i].subnet.prefix_len);

				cli_out("|%-5d", prm_vrf_ndx);
				cli_out("|%-18s", decimal_ip);
				cli_out("|%-7d", prm_fec_ids[i]);

				cli_out("|%-13s", SOC_PPC_FRWRD_IP_ROUTE_STATUS_to_string(prm_routes_status[i]));
				cli_out("|%-30s|", SOC_PPC_FRWRD_IP_ROUTE_LOCATION_to_string(prm_routes_location[i]));

				cli_out("\n\r");
			}
			print_count++;
		}
	}
  }

  cli_out("total entries to print %u \n\r", print_count);

  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: vrf_route_remove (section frwrd_ipv4)
 */
int 
  ui_ppd_api_frwrd_ipv4_vrf_route_remove(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_VRF_ID   
    prm_vrf_ndx;
  SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY   
    prm_route_key;
  SOC_SAND_SUCCESS_FAILURE   
    prm_success;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv4"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv4_vrf_route_remove"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY_clear(&prm_route_key);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_VRF_ROUTE_REMOVE_VRF_ROUTE_REMOVE_VRF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_VRF_ROUTE_REMOVE_VRF_ROUTE_REMOVE_VRF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_vrf_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter vrf_ndx after vrf_route_remove***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_VRF_ROUTE_REMOVE_VRF_ROUTE_REMOVE_ROUTE_KEY_SUBNET_PREFIX_LEN_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_VRF_ROUTE_REMOVE_VRF_ROUTE_REMOVE_ROUTE_KEY_SUBNET_PREFIX_LEN_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_route_key.subnet.prefix_len = (uint8)param_val->value.ulong_value;
  } 

  if (!get_val_of(
    current_line,(int *)&match_index,SOC_PARAM_PPD_FRWRD_IPV4_VRF_ROUTE_REMOVE_VRF_ROUTE_REMOVE_ROUTE_KEY_SUBNET_IP_ADDRESS_ID,1,  
    &param_val,VAL_IP,err_msg))                                                                
  {                                                                                                  
    prm_route_key.subnet.ip_address = param_val->value.ip_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter route_key after uc_route_add***", TRUE); 
    goto exit; 
  } 

  /* Call function */
  ret = soc_ppd_frwrd_ipv4_vrf_route_remove(
          unit,
          prm_vrf_ndx,
          &prm_route_key,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_ipv4_vrf_route_remove - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv4_vrf_route_remove");   
    goto exit; 
  } 

  cli_out("success: %s\n\r",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));
  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: vrf_routing_table_clear (section frwrd_ipv4)
 */
int 
  ui_ppd_api_frwrd_ipv4_vrf_routing_table_clear(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_VRF_ID   
    prm_vrf_ndx;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv4"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv4_vrf_routing_table_clear"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_VRF_ROUTING_TABLE_CLEAR_VRF_ROUTING_TABLE_CLEAR_VRF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_VRF_ROUTING_TABLE_CLEAR_VRF_ROUTING_TABLE_CLEAR_VRF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_vrf_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter vrf_ndx after vrf_routing_table_clear***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_frwrd_ipv4_vrf_routing_table_clear(
          unit,
          prm_vrf_ndx
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_ipv4_vrf_routing_table_clear - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv4_vrf_routing_table_clear");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: vrf_all_routing_tables_clear (section frwrd_ipv4)
 */
int 
  ui_ppd_api_frwrd_ipv4_vrf_all_routing_tables_clear(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32 
    flags= 0; /* SOC_PPC_FRWRD_IP_HOST_ONLY | SOC_PPC_FRWRD_IP_LPM_ONLY;   */
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv4"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv4_vrf_all_routing_tables_clear"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_VRF_ALL_ROUTING_TABLES_CLEAR_VRF_ALL_ROUTING_TABLES_CLEAR_LPM,1))          
  {  
      flags = SOC_PPC_FRWRD_IP_LPM_ONLY;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_VRF_ALL_ROUTING_TABLES_CLEAR_VRF_ALL_ROUTING_TABLES_CLEAR_HOST,1))          
  {  
      if (flags == SOC_PPC_FRWRD_IP_LPM_ONLY) {
          cli_out("only one can be setl, lpm_only or host_only  \n\r");
          goto exit;
      }
      flags = SOC_PPC_FRWRD_IP_HOST_ONLY;
  }

  /* Call function */
  ret = soc_ppd_frwrd_ipv4_vrf_all_routing_tables_clear(
          unit,
          flags
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_ipv4_vrf_all_routing_tables_clear - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv4_vrf_all_routing_tables_clear");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: mem_status_get (section frwrd_ipv4)
 */
int 
  ui_ppd_api_frwrd_ipv4_mem_status_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_mem_ndx;
  SOC_PPC_FRWRD_IPV4_MEM_STATUS   
    prm_mem_status;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv4"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv4_mem_status_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_IPV4_MEM_STATUS_clear(&prm_mem_status);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_MEM_STATUS_GET_MEM_STATUS_GET_MEM_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_MEM_STATUS_GET_MEM_STATUS_GET_MEM_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_mem_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter mem_ndx after mem_status_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_frwrd_ipv4_mem_status_get(
          unit,
          prm_mem_ndx,
          &prm_mem_status
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_ipv4_mem_status_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv4_mem_status_get");   
    goto exit; 
  } 

  SOC_PPC_FRWRD_IPV4_MEM_STATUS_print(&prm_mem_status);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: mem_defrage (section frwrd_ipv4)
 */
int 
  ui_ppd_api_frwrd_ipv4_mem_defrage(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_mem_ndx;
  SOC_PPC_FRWRD_IPV4_MEM_DEFRAG_INFO   
    prm_defrag_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv4"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv4_mem_defrage"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_IPV4_MEM_DEFRAG_INFO_clear(&prm_defrag_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_MEM_DEFRAGE_MEM_DEFRAGE_MEM_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_MEM_DEFRAGE_MEM_DEFRAGE_MEM_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_mem_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter mem_ndx after mem_defrage***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_MEM_DEFRAGE_MEM_DEFRAGE_DEFRAG_INFO_RANGE_ENTRIES_TO_GET_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_MEM_DEFRAGE_MEM_DEFRAGE_DEFRAG_INFO_RANGE_ENTRIES_TO_GET_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_defrag_info.range.entries_to_act = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_MEM_DEFRAGE_MEM_DEFRAGE_DEFRAG_INFO_RANGE_ENTRIES_TO_SCAN_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_MEM_DEFRAGE_MEM_DEFRAGE_DEFRAG_INFO_RANGE_ENTRIES_TO_SCAN_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_defrag_info.range.entries_to_scan = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_MEM_DEFRAGE_MEM_DEFRAGE_DEFRAG_INFO_RANGE_ITER_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_MEM_DEFRAGE_MEM_DEFRAGE_DEFRAG_INFO_RANGE_ITER_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_defrag_info.range.iter = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_frwrd_ipv4_mem_defrage(
          unit,
          prm_mem_ndx,
          &prm_defrag_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_ipv4_mem_defrage - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv4_mem_defrage");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
#endif
#ifdef UI_FRWRD_IPV4/* { frwrd_ipv4*/
/******************************************************************** 
 *  Section handler: frwrd_ipv4
 */ 
int 
  ui_ppd_api_frwrd_ipv4( 
    CURRENT_LINE *current_line 
  ) 
{   
  int
    ret = FALSE;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv4"); 
 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_SET_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_SET_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_ipv4_frwrd_ip_routes_cache_mode_enable_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_GET_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_GET_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_ipv4_frwrd_ip_routes_cache_mode_enable_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IP_ROUTES_CACHE_COMMIT_FRWRD_IP_ROUTES_CACHE_COMMIT_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_ipv4_frwrd_ip_routes_cache_commit(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_GLBL_INFO_SET_GLBL_INFO_SET_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_ipv4_glbl_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_GLBL_INFO_GET_GLBL_INFO_GET_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_ipv4_glbl_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_UC_ROUTE_ADD_UC_ROUTE_ADD_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_ipv4_uc_route_add(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_UC_ROUTE_GET_UC_ROUTE_GET_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_ipv4_uc_route_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_UC_ROUTE_GET_BLOCK_UC_ROUTE_GET_BLOCK_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_ipv4_uc_route_get_block(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_UC_ROUTE_REMOVE_UC_ROUTE_REMOVE_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_ipv4_uc_route_remove(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_UC_ROUTING_TABLE_CLEAR_UC_ROUTING_TABLE_CLEAR_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_ipv4_uc_routing_table_clear(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_CACHE_MODE_SET_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_ipv4_cache_mode_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_CACHE_MODE_GET_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_ipv4_cache_mode_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_COMMIT_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_ipv4_commit(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_HOST_ADD_HOST_ADD_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_ipv4_host_add(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_HOST_GET_HOST_GET_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_ipv4_host_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_HOST_GET_BLOCK_HOST_GET_BLOCK_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_ipv4_host_get_block(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_LEM_GET_BLOCK_LEM_GET_BLOCK_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_lem_get_block(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_HOST_REMOVE_HOST_REMOVE_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_ipv4_host_remove(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_MC_ROUTE_ADD_MC_ROUTE_ADD_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_ipv4_mc_route_add(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_MC_ROUTE_GET_MC_ROUTE_GET_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_ipv4_mc_route_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_MC_ROUTE_GET_BLOCK_MC_ROUTE_GET_BLOCK_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_ipv4_mc_route_get_block(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_MC_ROUTE_REMOVE_MC_ROUTE_REMOVE_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_ipv4_mc_route_remove(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_MC_ROUTING_TABLE_CLEAR_MC_ROUTING_TABLE_CLEAR_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_ipv4_mc_routing_table_clear(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_VRF_INFO_SET_VRF_INFO_SET_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_ipv4_vrf_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_VRF_INFO_GET_VRF_INFO_GET_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_ipv4_vrf_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_VRF_ROUTE_ADD_VRF_ROUTE_ADD_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_ipv4_vrf_route_add(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_VRF_ROUTE_GET_VRF_ROUTE_GET_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_ipv4_vrf_route_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_VRF_ROUTE_GET_BLOCK_VRF_ROUTE_GET_BLOCK_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_ipv4_vrf_route_get_block(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_VRF_ROUTE_REMOVE_VRF_ROUTE_REMOVE_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_ipv4_vrf_route_remove(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_VRF_ROUTING_TABLE_CLEAR_VRF_ROUTING_TABLE_CLEAR_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_ipv4_vrf_routing_table_clear(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_VRF_ALL_ROUTING_TABLES_CLEAR_VRF_ALL_ROUTING_TABLES_CLEAR_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_ipv4_vrf_all_routing_tables_clear(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_MEM_STATUS_GET_MEM_STATUS_GET_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_ipv4_mem_status_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_MEM_DEFRAGE_MEM_DEFRAGE_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_ipv4_mem_defrage(current_line); 
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting function name after frwrd_ipv4***", TRUE); 
  } 
  
  ui_ret = ret;
    
  goto exit; 
exit:        
  return ui_ret; 
}

#endif /* frwrd_ipv4 */ 

#endif /* LINK_PPD_LIBRARIES */ 

