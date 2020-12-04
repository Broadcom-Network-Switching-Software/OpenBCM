/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/*
 * General include file for reference design.
 */

/*
 * Utilities include file.
 */

#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <appl/diag/dpp/utils_defi.h>
#include <appl/dpp/UserInterface/ui_defi.h>

#include <appl/dpp/UserInterface/ui_pure_defi_ppa_api.h>

#include <appl/dpp/PPA/ppa_bridge_router_app.h>
#include <appl/dpp/PPA/ppa_vpls_app.h>

int 
  ui_ppa_bridge_router_app(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  PPA_BRIDGE_ROUTER_APP_INFO
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppa_bridge_router_app"); 
  soc_sand_proc_name = "ppa_bridge_router_app"; 
 
  prm_info.flow_based_mode = FALSE;
  prm_info.nof_vd = 1;

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PPA_BRIDGE_ROUTER_APP_FLOW_BASED_MODE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PPA_BRIDGE_ROUTER_APP_FLOW_BASED_MODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.flow_based_mode = (uint8)param_val->value.ulong_value;
  }

  /* This is a set function, so call GET function first */                                                                                
  ret = ppa_bridge_router_app(
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** ppa_bridge_router_app - FAIL", TRUE); 
    goto exit; 
  } 

  goto exit; 
exit: 
  return ui_ret; 
} 

int 
  ui_ppa_vpls_app(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  PPA_VPLS_APP_INFO    
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppa_vpls_app"); 
  soc_sand_proc_name = "ppa_vpls_app"; 
 
  prm_info.flow_based_mode = FALSE;
  prm_info.nof_mp_services = PPA_VPLS_APP_MP_NOF_SERVICES;
  prm_info.nof_p2p_services = PPA_VPLS_APP_P2P_NOF_SERVICES;
  prm_info.oam_app = FALSE;
  prm_info.interrupt_not_poll = TRUE;

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PPA_VPLS_APP_FLOW_BASED_MODE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PPA_VPLS_APP_FLOW_BASED_MODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.flow_based_mode = (uint8)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PPA_VPLS_APP_POLL_INTERRUPT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PPA_VPLS_APP_POLL_INTERRUPT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.interrupt_not_poll = (uint8)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PPA_VPLS_APP_OAM_APP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PPA_VPLS_APP_OAM_APP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.oam_app = (uint8)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PPA_VPLS_APP_NOF_MP_SERVICES_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PPA_VPLS_APP_NOF_MP_SERVICES_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.nof_mp_services = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PPA_VPLS_APP_NOF_P2P_SERVICES_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PPA_VPLS_APP_NOF_P2P_SERVICES_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.nof_p2p_services = (uint32)param_val->value.ulong_value;
  }

  /* This is a set function, so call GET function first */                                                                                
  ret = ppa_vpls_app(
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** ppa_vpls_app - FAIL", TRUE); 
    goto exit; 
  } 

  goto exit; 
exit: 
  return ui_ret; 
} 

