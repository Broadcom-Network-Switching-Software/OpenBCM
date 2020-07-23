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
 

  
#include <soc/dpp/PPD/ppd_api_llp_trap.h>
#include <soc/dpp/PPD/ppd_api_llp_mirror.h>
#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_llp_trap.h>

#if LINK_PPD_LIBRARIES

#ifdef UI_LLP_TRAP
/********************************************************************
*  Function handler: port_info_set (section llp_trap)
 */
int
  ui_ppd_api_llp_trap_port_info_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_PPC_PORT   
    prm_local_port_ndx;
  SOC_PPC_LLP_TRAP_PORT_INFO
    prm_port_info;
  uint32
    enabled = FALSE;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_trap");
  soc_sand_proc_name = "soc_ppd_llp_trap_port_info_set";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_LLP_TRAP_PORT_INFO_clear(&prm_port_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_TRAP_PORT_INFO_SET_PORT_INFO_SET_LOCAL_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_TRAP_PORT_INFO_SET_PORT_INFO_SET_LOCAL_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_local_port_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter local_port_ndx after port_info_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = soc_ppd_llp_trap_port_info_get(
    unit,
    SOC_CORE_DEFAULT,
    prm_local_port_ndx,
    &prm_port_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_llp_trap_port_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_trap_port_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_TRAP_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_TRAP_ENABLE_MASK_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LLP_TRAP_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_TRAP_ENABLE_MASK_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_info.trap_enable_mask |= (uint32)param_val->numeric_equivalent;
    if (param_val->numeric_equivalent == SOC_PPC_LLP_TRAP_PORT_ENABLE_NONE)
    {
      prm_port_info.trap_enable_mask = 0;
    }
    enabled = (uint32)param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_TRAP_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_TRAP_DISABLE_MASK_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LLP_TRAP_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_TRAP_DISABLE_MASK_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_info.trap_enable_mask &= ~(uint32)param_val->numeric_equivalent;
    if (enabled == (uint32)param_val->numeric_equivalent)
    {
      send_string_to_screen(" *** SW error - same type was enabled and disabled***", TRUE);     
    }
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_TRAP_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_RESERVED_MC_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_TRAP_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_RESERVED_MC_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_info.reserved_mc_profile = (uint32)param_val->value.ulong_value;
  }



  /* Call function */
  ret = soc_ppd_llp_trap_port_info_set(
    unit,
    SOC_CORE_DEFAULT,
    prm_local_port_ndx,
    &prm_port_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_llp_trap_port_info_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_trap_port_info_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: port_info_get (section llp_trap)
 */
int
ui_ppd_api_llp_trap_port_info_get(
                                  CURRENT_LINE *current_line
                                  )
{
  uint32
    ret;
  SOC_PPC_PORT   
    prm_local_port_ndx;
  SOC_PPC_LLP_TRAP_PORT_INFO
    prm_port_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_trap");
  soc_sand_proc_name = "soc_ppd_llp_trap_port_info_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_LLP_TRAP_PORT_INFO_clear(&prm_port_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_TRAP_PORT_INFO_GET_PORT_INFO_GET_LOCAL_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_TRAP_PORT_INFO_GET_PORT_INFO_GET_LOCAL_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_local_port_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter local_port_ndx after port_info_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_llp_trap_port_info_get(
    unit,
    SOC_CORE_DEFAULT,
    prm_local_port_ndx,
    &prm_port_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_llp_trap_port_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_trap_port_info_get");
    goto exit;
  }

  send_string_to_screen("--> port_info:", TRUE);
  SOC_PPC_LLP_TRAP_PORT_INFO_print(&prm_port_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: arp_info_set (section llp_trap)
 */
int 
  ui_ppd_api_llp_trap_arp_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;    
  SOC_PPC_LLP_TRAP_ARP_INFO   
    prm_arp_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_trap"); 
  soc_sand_proc_name = "soc_ppd_llp_trap_arp_info_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_LLP_TRAP_ARP_INFO_clear(&prm_arp_info);
 
  /* Get parameters */ 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_llp_trap_arp_info_get(
          unit,
          &prm_arp_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_llp_trap_arp_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_trap_arp_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_TRAP_ARP_INFO_SET_ARP_INFO_SET_ARP_INFO_IGNORE_DA_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_TRAP_ARP_INFO_SET_ARP_INFO_SET_ARP_INFO_IGNORE_DA_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_arp_info.ignore_da = (uint8)param_val->value.ulong_value;
  } 

  if (!get_val_of(
    current_line,(int *)&match_index,SOC_PARAM_PPD_LLP_TRAP_ARP_INFO_SET_ARP_INFO_SET_ARP_INFO_MY_IPS_INDEX_ID,1,  
    &param_val,VAL_IP,err_msg))                                                                
  {                                                                                                  
    prm_arp_info.my_ips[1] = param_val->value.ip_value;
  } 

  if (!get_val_of(
    current_line,(int *)&match_index,SOC_PARAM_PPD_LLP_TRAP_ARP_INFO_SET_ARP_INFO_SET_ARP_INFO_MY_IPS_ID,1,  
    &param_val,VAL_IP,err_msg))                                                                
  {                                                                                                  
    prm_arp_info.my_ips[0] = param_val->value.ip_value;
  } 

  /* Call function */
  ret = soc_ppd_llp_trap_arp_info_set(
          unit,
          &prm_arp_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_llp_trap_arp_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_trap_arp_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: arp_info_get (section llp_trap)
 */
int 
  ui_ppd_api_llp_trap_arp_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_LLP_TRAP_ARP_INFO   
    prm_arp_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_trap"); 
  soc_sand_proc_name = "soc_ppd_llp_trap_arp_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_LLP_TRAP_ARP_INFO_clear(&prm_arp_info);
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_ppd_llp_trap_arp_info_get(
          unit,
          &prm_arp_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_llp_trap_arp_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_trap_arp_info_get");   
    goto exit; 
  } 

  SOC_PPC_LLP_TRAP_ARP_INFO_print(&prm_arp_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
*  Function handler: reserved_mc_info_set (section llp_trap)
 */
int
ui_ppd_api_llp_trap_reserved_mc_info_set(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_PPC_LLP_TRAP_RESERVED_MC_KEY
    prm_reserved_mc_key;
  SOC_PPC_ACTION_PROFILE
    prm_trap_action;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_trap");
  soc_sand_proc_name = "soc_ppd_llp_trap_reserved_mc_info_set";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_LLP_TRAP_RESERVED_MC_KEY_clear(&prm_reserved_mc_key);
  SOC_PPC_ACTION_PROFILE_clear(&prm_trap_action);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_TRAP_RESERVED_MC_INFO_SET_RESERVED_MC_INFO_SET_RESERVED_MC_KEY_DA_MAC_ADDRESS_LSB_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_TRAP_RESERVED_MC_INFO_SET_RESERVED_MC_INFO_SET_RESERVED_MC_KEY_DA_MAC_ADDRESS_LSB_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_reserved_mc_key.da_mac_address_lsb = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_TRAP_RESERVED_MC_INFO_SET_RESERVED_MC_INFO_SET_RESERVED_MC_KEY_RESERVED_MC_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_TRAP_RESERVED_MC_INFO_SET_RESERVED_MC_INFO_SET_RESERVED_MC_KEY_RESERVED_MC_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_reserved_mc_key.reserved_mc_profile = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter reserved_mc_key after reserved_mc_info_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = soc_ppd_llp_trap_reserved_mc_info_get(
    unit,
    &prm_reserved_mc_key,
    &prm_trap_action
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_llp_trap_reserved_mc_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_trap_reserved_mc_info_get");
    goto exit;
  }

  /*trap-code
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_TRAP_RESERVED_MC_INFO_SET_RESERVED_MC_INFO_SET_TRAP_ACTION_PROFILE_TRAP_CODE_LSB_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LLP_TRAP_RESERVED_MC_INFO_SET_RESERVED_MC_INFO_SET_TRAP_ACTION_PROFILE_TRAP_CODE_LSB_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_trap_action.trap_code = (SOC_PPC_TRAP_CODE)param_val->numeric_equivalent;
  }
  */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_TRAP_RESERVED_MC_INFO_SET_RESERVED_MC_INFO_SET_TRAP_ACTION_PROFILE_TRAP_CODE_LSB_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_TRAP_RESERVED_MC_INFO_SET_RESERVED_MC_INFO_SET_TRAP_ACTION_PROFILE_TRAP_CODE_LSB_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_trap_action.trap_code = (SOC_PPC_TRAP_CODE)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_TRAP_RESERVED_MC_INFO_SET_RESERVED_MC_INFO_SET_TRAP_ACTION_PROFILE_FRWRD_STRENGTH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_TRAP_RESERVED_MC_INFO_SET_RESERVED_MC_INFO_SET_TRAP_ACTION_PROFILE_FRWRD_STRENGTH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_trap_action.frwrd_action_strength = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_TRAP_RESERVED_MC_INFO_SET_RESERVED_MC_INFO_SET_TRAP_ACTION_PROFILE_SNOOP_STRENGTH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_TRAP_RESERVED_MC_INFO_SET_RESERVED_MC_INFO_SET_TRAP_ACTION_PROFILE_SNOOP_STRENGTH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_trap_action.snoop_action_strength = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_llp_trap_reserved_mc_info_set(
    unit,
    &prm_reserved_mc_key,
    &prm_trap_action
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_llp_trap_reserved_mc_info_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_trap_reserved_mc_info_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: reserved_mc_info_get (section llp_trap)
 */
int
ui_ppd_api_llp_trap_reserved_mc_info_get(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_PPC_LLP_TRAP_RESERVED_MC_KEY
    prm_reserved_mc_key;
  SOC_PPC_ACTION_PROFILE
    prm_trap_action;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_trap");
  soc_sand_proc_name = "soc_ppd_llp_trap_reserved_mc_info_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_LLP_TRAP_RESERVED_MC_KEY_clear(&prm_reserved_mc_key);
  SOC_PPC_ACTION_PROFILE_clear(&prm_trap_action);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_TRAP_RESERVED_MC_INFO_GET_RESERVED_MC_INFO_GET_RESERVED_MC_KEY_DA_MAC_ADDRESS_LSB_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_TRAP_RESERVED_MC_INFO_GET_RESERVED_MC_INFO_GET_RESERVED_MC_KEY_DA_MAC_ADDRESS_LSB_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_reserved_mc_key.da_mac_address_lsb = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_TRAP_RESERVED_MC_INFO_GET_RESERVED_MC_INFO_GET_RESERVED_MC_KEY_RESERVED_MC_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_TRAP_RESERVED_MC_INFO_GET_RESERVED_MC_INFO_GET_RESERVED_MC_KEY_RESERVED_MC_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_reserved_mc_key.reserved_mc_profile = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter reserved_mc_key after reserved_mc_info_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_llp_trap_reserved_mc_info_get(
    unit,
    &prm_reserved_mc_key,
    &prm_trap_action
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_llp_trap_reserved_mc_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_trap_reserved_mc_info_get");
    goto exit;
  }

  send_string_to_screen("--> trap_action:", TRUE);
  SOC_PPC_ACTION_PROFILE_print(&prm_trap_action);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: general_trap_info_set (section llp_trap)
 */
int
ui_ppd_api_llp_trap_prog_trap_info_set(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_general_trap_ndx;
  SOC_PPC_LLP_TRAP_PROG_TRAP_QUALIFIER
    prm_general_trap_info;
  SOC_PPC_ACTION_PROFILE
    prm_trap_action;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_trap");
  soc_sand_proc_name = "soc_ppd_llp_trap_prog_trap_info_set";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_LLP_TRAP_PROG_TRAP_QUALIFIER_clear(&prm_general_trap_info);
  SOC_PPC_ACTION_PROFILE_clear(&prm_trap_action);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_TRAP_PROG_TRAP_QUALIFIER_SET_GENERAL_TRAP_INFO_SET_GENERAL_TRAP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_TRAP_PROG_TRAP_QUALIFIER_SET_GENERAL_TRAP_INFO_SET_GENERAL_TRAP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_general_trap_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter general_trap_ndx after general_trap_info_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = soc_ppd_llp_trap_prog_trap_info_get(
    unit,
    prm_general_trap_ndx,
    &prm_general_trap_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_llp_trap_prog_trap_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_trap_prog_trap_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_TRAP_PROG_TRAP_QUALIFIER_SET_GENERAL_TRAP_INFO_SET_GENERAL_TRAP_INFO_INVERSE_BITMAP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_TRAP_PROG_TRAP_QUALIFIER_SET_GENERAL_TRAP_INFO_SET_GENERAL_TRAP_INFO_INVERSE_BITMAP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_general_trap_info.inverse_bitmap = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_TRAP_PROG_TRAP_QUALIFIER_SET_GENERAL_TRAP_INFO_SET_GENERAL_TRAP_INFO_ENABLE_BITMAP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_TRAP_PROG_TRAP_QUALIFIER_SET_GENERAL_TRAP_INFO_SET_GENERAL_TRAP_INFO_ENABLE_BITMAP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_general_trap_info.enable_bitmap = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_TRAP_PROG_TRAP_QUALIFIER_SET_GENERAL_TRAP_INFO_SET_GENERAL_TRAP_INFO_L4_INFO_DEST_PORT_BITMAP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_TRAP_PROG_TRAP_QUALIFIER_SET_GENERAL_TRAP_INFO_SET_GENERAL_TRAP_INFO_L4_INFO_DEST_PORT_BITMAP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_general_trap_info.l4_info.dest_port_bitmap = (uint16)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_TRAP_PROG_TRAP_QUALIFIER_SET_GENERAL_TRAP_INFO_SET_GENERAL_TRAP_INFO_L4_INFO_DEST_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_TRAP_PROG_TRAP_QUALIFIER_SET_GENERAL_TRAP_INFO_SET_GENERAL_TRAP_INFO_L4_INFO_DEST_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_general_trap_info.l4_info.dest_port = (uint16)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_TRAP_PROG_TRAP_QUALIFIER_SET_GENERAL_TRAP_INFO_SET_GENERAL_TRAP_INFO_L4_INFO_SRC_PORT_BITMAP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_TRAP_PROG_TRAP_QUALIFIER_SET_GENERAL_TRAP_INFO_SET_GENERAL_TRAP_INFO_L4_INFO_SRC_PORT_BITMAP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_general_trap_info.l4_info.src_port_bitmap = (uint16)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_TRAP_PROG_TRAP_QUALIFIER_SET_GENERAL_TRAP_INFO_SET_GENERAL_TRAP_INFO_L4_INFO_SRC_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_TRAP_PROG_TRAP_QUALIFIER_SET_GENERAL_TRAP_INFO_SET_GENERAL_TRAP_INFO_L4_INFO_SRC_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_general_trap_info.l4_info.src_port = (uint16)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_TRAP_PROG_TRAP_QUALIFIER_SET_GENERAL_TRAP_INFO_SET_GENERAL_TRAP_INFO_L3_INFO_IP_PROTOCOL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_TRAP_PROG_TRAP_QUALIFIER_SET_GENERAL_TRAP_INFO_SET_GENERAL_TRAP_INFO_L3_INFO_IP_PROTOCOL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_general_trap_info.l3_info.ip_protocol = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_TRAP_PROG_TRAP_INFO_SET_PROG_TRAP_INFO_SET_PROG_TRAP_QUAL_L2_INFO_SUB_TYPE_BITMAP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_TRAP_PROG_TRAP_INFO_SET_PROG_TRAP_INFO_SET_PROG_TRAP_QUAL_L2_INFO_SUB_TYPE_BITMAP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_general_trap_info.l2_info.sub_type_bitmap = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_TRAP_PROG_TRAP_QUALIFIER_SET_GENERAL_TRAP_INFO_SET_GENERAL_TRAP_INFO_L2_INFO_SUB_TYPE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_TRAP_PROG_TRAP_QUALIFIER_SET_GENERAL_TRAP_INFO_SET_GENERAL_TRAP_INFO_L2_INFO_SUB_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_general_trap_info.l2_info.sub_type = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_TRAP_PROG_TRAP_QUALIFIER_SET_GENERAL_TRAP_INFO_SET_GENERAL_TRAP_INFO_L2_INFO_ETHER_TYPE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_TRAP_PROG_TRAP_QUALIFIER_SET_GENERAL_TRAP_INFO_SET_GENERAL_TRAP_INFO_L2_INFO_ETHER_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_general_trap_info.l2_info.ether_type = (uint16)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_TRAP_PROG_TRAP_QUALIFIER_SET_GENERAL_TRAP_INFO_SET_GENERAL_TRAP_INFO_L2_INFO_DEST_MAC_NOF_BITS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_TRAP_PROG_TRAP_QUALIFIER_SET_GENERAL_TRAP_INFO_SET_GENERAL_TRAP_INFO_L2_INFO_DEST_MAC_NOF_BITS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_general_trap_info.l2_info.dest_mac_nof_bits = (uint8)param_val->value.ulong_value;
  }

  if (!get_val_of(
    current_line,(int *)&match_index,SOC_PARAM_PPD_LLP_TRAP_PROG_TRAP_QUALIFIER_SET_GENERAL_TRAP_INFO_SET_GENERAL_TRAP_INFO_L2_INFO_DEST_MAC_ID,1,
    &param_val,VAL_TEXT,err_msg))

  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_general_trap_info.l2_info.dest_mac));
  }

  /* Call function */
  ret = soc_ppd_llp_trap_prog_trap_info_set(
    unit,
    prm_general_trap_ndx,
    &prm_general_trap_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_llp_trap_prog_trap_info_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_trap_prog_trap_info_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: general_trap_info_get (section llp_trap)
 */
int
ui_ppd_api_llp_trap_prog_trap_info_get(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_general_trap_ndx;
  SOC_PPC_LLP_TRAP_PROG_TRAP_QUALIFIER
    prm_general_trap_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_trap");
  soc_sand_proc_name = "soc_ppd_llp_trap_prog_trap_info_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_LLP_TRAP_PROG_TRAP_QUALIFIER_clear(&prm_general_trap_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_TRAP_PROG_TRAP_QUALIFIER_GET_GENERAL_TRAP_INFO_GET_GENERAL_TRAP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_TRAP_PROG_TRAP_QUALIFIER_GET_GENERAL_TRAP_INFO_GET_GENERAL_TRAP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_general_trap_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter general_trap_ndx after general_trap_info_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_llp_trap_prog_trap_info_get(
    unit,
    prm_general_trap_ndx,
    &prm_general_trap_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_llp_trap_prog_trap_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_trap_prog_trap_info_get");
    goto exit;
  }

  send_string_to_screen("--> general_trap_info:", TRUE);
  SOC_PPC_LLP_TRAP_PROG_TRAP_QUALIFIER_print(&prm_general_trap_info);

  goto exit;
exit:
  return ui_ret;
}

/******************************************************************** 
*  Function handler: llp_mirror_port_vlan_add (section llp_trap)
 */
int 
ui_ppd_api_llp_trap_llp_mirror_port_vlan_add(
  CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_local_port_ndx;
  uint32   
    prm_vid_ndx;
  uint32   
    prm_mirror_profile=0;
  SOC_SAND_SUCCESS_FAILURE   
    prm_success;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_trap"); 
  soc_sand_proc_name = "soc_ppd_llp_mirror_port_vlan_add"; 

  unit = soc_ppd_get_default_unit(); 

  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_MIRROR_PORT_VLAN_ADD_LLP_MIRROR_PORT_VLAN_ADD_LOCAL_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_MIRROR_PORT_VLAN_ADD_LLP_MIRROR_PORT_VLAN_ADD_LOCAL_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_local_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter local_port_ndx after llp_mirror_port_vlan_add***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_MIRROR_PORT_VLAN_ADD_LLP_MIRROR_PORT_VLAN_ADD_VID_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_MIRROR_PORT_VLAN_ADD_LLP_MIRROR_PORT_VLAN_ADD_VID_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_vid_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter vid_ndx after llp_mirror_port_vlan_add***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_MIRROR_PORT_VLAN_ADD_LLP_MIRROR_PORT_VLAN_ADD_MIRROR_PROFILE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_MIRROR_PORT_VLAN_ADD_LLP_MIRROR_PORT_VLAN_ADD_MIRROR_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_mirror_profile = (uint32)param_val->value.ulong_value;
  } 

  /* Call function */  
  ret = soc_ppd_llp_mirror_port_vlan_add(
    unit,
	SOC_CORE_DEFAULT,
    prm_local_port_ndx,
    prm_vid_ndx,
    prm_mirror_profile,
    &prm_success
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_llp_mirror_port_vlan_add - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_mirror_port_vlan_add");   
    goto exit; 
  } 

  cli_out("success: %s\n",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));


  goto exit; 
exit: 
  return ui_ret; 
} 

/******************************************************************** 
*  Function handler: llp_mirror_port_vlan_remove (section llp_trap)
 */
int 
ui_ppd_api_llp_trap_llp_mirror_port_vlan_remove(
  CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_local_port_ndx;
  uint32   
    prm_vid_ndx;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_trap"); 
  soc_sand_proc_name = "soc_ppd_llp_mirror_port_vlan_remove"; 

  unit = soc_ppd_get_default_unit(); 

  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_MIRROR_PORT_VLAN_REMOVE_LLP_MIRROR_PORT_VLAN_REMOVE_LOCAL_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_MIRROR_PORT_VLAN_REMOVE_LLP_MIRROR_PORT_VLAN_REMOVE_LOCAL_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_local_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter local_port_ndx after llp_mirror_port_vlan_remove***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_MIRROR_PORT_VLAN_REMOVE_LLP_MIRROR_PORT_VLAN_REMOVE_VID_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_MIRROR_PORT_VLAN_REMOVE_LLP_MIRROR_PORT_VLAN_REMOVE_VID_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_vid_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter vid_ndx after llp_mirror_port_vlan_remove***", TRUE); 
    goto exit; 
  } 

  /* Call function */  
  ret = soc_ppd_llp_mirror_port_vlan_remove(
    unit,
	SOC_CORE_DEFAULT,
    prm_local_port_ndx,
    prm_vid_ndx
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_llp_mirror_port_vlan_remove - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_mirror_port_vlan_remove");   
    goto exit; 
  } 


  goto exit; 
exit: 
  return ui_ret; 
} 

/******************************************************************** 
*  Function handler: llp_mirror_port_vlan_get (section llp_trap)
 */
int 
ui_ppd_api_llp_trap_llp_mirror_port_vlan_get(
  CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_local_port_ndx;
  uint32   
    prm_vid_ndx;
  uint32   
    prm_mirror_profile;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_trap"); 
  soc_sand_proc_name = "soc_ppd_llp_mirror_port_vlan_get"; 

  unit = soc_ppd_get_default_unit(); 

  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_MIRROR_PORT_VLAN_GET_LLP_MIRROR_PORT_VLAN_GET_LOCAL_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_MIRROR_PORT_VLAN_GET_LLP_MIRROR_PORT_VLAN_GET_LOCAL_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_local_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter local_port_ndx after llp_mirror_port_vlan_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_MIRROR_PORT_VLAN_GET_LLP_MIRROR_PORT_VLAN_GET_VID_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_MIRROR_PORT_VLAN_GET_LLP_MIRROR_PORT_VLAN_GET_VID_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_vid_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter vid_ndx after llp_mirror_port_vlan_get***", TRUE); 
    goto exit; 
  } 

  /* Call function */  
  ret = soc_ppd_llp_mirror_port_vlan_get(
    unit,
	SOC_CORE_DEFAULT,
    prm_local_port_ndx,
    prm_vid_ndx,
    &prm_mirror_profile
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_llp_mirror_port_vlan_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_mirror_port_vlan_get");   
    goto exit; 
  } 

  cli_out("mirror_profile: %u\n",prm_mirror_profile);


  goto exit; 
exit: 
  return ui_ret; 
} 

/******************************************************************** 
*  Function handler: llp_mirror_port_dflt_set (section llp_trap)
 */
int 
ui_ppd_api_llp_trap_llp_mirror_port_dflt_set(
  CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_local_port_ndx;
  SOC_PPC_LLP_MIRROR_PORT_DFLT_INFO   
    prm_dflt_mirroring_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_trap"); 
  soc_sand_proc_name = "soc_ppd_llp_mirror_port_dflt_set"; 

  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_LLP_MIRROR_PORT_DFLT_INFO_clear(&prm_dflt_mirroring_info);

  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_MIRROR_PORT_DFLT_SET_LLP_MIRROR_PORT_DFLT_SET_LOCAL_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_MIRROR_PORT_DFLT_SET_LLP_MIRROR_PORT_DFLT_SET_LOCAL_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_local_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter local_port_ndx after llp_mirror_port_dflt_set***", TRUE); 
    goto exit; 
  } 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_llp_mirror_port_dflt_get(
    unit,
	SOC_CORE_DEFAULT,
    prm_local_port_ndx,
    &prm_dflt_mirroring_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_llp_mirror_port_dflt_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_mirror_port_dflt_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_MIRROR_PORT_DFLT_SET_LLP_MIRROR_PORT_DFLT_SET_DFLT_MIRRORING_INFO_UNTAGGED_DFLT_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_MIRROR_PORT_DFLT_SET_LLP_MIRROR_PORT_DFLT_SET_DFLT_MIRRORING_INFO_UNTAGGED_DFLT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_dflt_mirroring_info.untagged_dflt = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_MIRROR_PORT_DFLT_SET_LLP_MIRROR_PORT_DFLT_SET_DFLT_MIRRORING_INFO_TAGGED_DFLT_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_MIRROR_PORT_DFLT_SET_LLP_MIRROR_PORT_DFLT_SET_DFLT_MIRRORING_INFO_TAGGED_DFLT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_dflt_mirroring_info.tagged_dflt = (uint32)param_val->value.ulong_value;
  } 

  /* Call function */  
  ret = soc_ppd_llp_mirror_port_dflt_set(
    unit,
    SOC_CORE_DEFAULT,
    prm_local_port_ndx,
    &prm_dflt_mirroring_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_llp_mirror_port_dflt_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_mirror_port_dflt_set");   
    goto exit; 
  } 


  goto exit; 
exit: 
  return ui_ret; 
} 

/******************************************************************** 
*  Function handler: llp_mirror_port_dflt_get (section llp_trap)
 */
int 
ui_ppd_api_llp_trap_llp_mirror_port_dflt_get(
  CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_local_port_ndx;
  SOC_PPC_LLP_MIRROR_PORT_DFLT_INFO   
    prm_dflt_mirroring_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_trap"); 
  soc_sand_proc_name = "soc_ppd_llp_mirror_port_dflt_get"; 

  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_LLP_MIRROR_PORT_DFLT_INFO_clear(&prm_dflt_mirroring_info);

  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_MIRROR_PORT_DFLT_GET_LLP_MIRROR_PORT_DFLT_GET_LOCAL_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_MIRROR_PORT_DFLT_GET_LLP_MIRROR_PORT_DFLT_GET_LOCAL_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_local_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter local_port_ndx after llp_mirror_port_dflt_get***", TRUE); 
    goto exit; 
  } 

  /* Call function */  
  ret = soc_ppd_llp_mirror_port_dflt_get(
    unit,
	SOC_CORE_DEFAULT,
    prm_local_port_ndx,
    &prm_dflt_mirroring_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_llp_mirror_port_dflt_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_mirror_port_dflt_get");   
    goto exit; 
  } 

  SOC_PPC_LLP_MIRROR_PORT_DFLT_INFO_print(&prm_dflt_mirroring_info);


  goto exit; 
exit: 
  return ui_ret; 
} 

/********************************************************************
*  Section handler: llp_trap
 */
int
ui_ppd_api_llp_trap(
                    CURRENT_LINE *current_line
                    )
{
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_trap");

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_TRAP_PORT_INFO_SET_PORT_INFO_SET_ID,1))
  {
    ui_ret = ui_ppd_api_llp_trap_port_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_TRAP_PORT_INFO_GET_PORT_INFO_GET_ID,1))
  {
    ui_ret = ui_ppd_api_llp_trap_port_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_TRAP_RESERVED_MC_INFO_SET_RESERVED_MC_INFO_SET_ID,1))
  {
    ui_ret = ui_ppd_api_llp_trap_reserved_mc_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_TRAP_RESERVED_MC_INFO_GET_RESERVED_MC_INFO_GET_ID,1))
  {
    ui_ret = ui_ppd_api_llp_trap_reserved_mc_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_TRAP_PROG_TRAP_QUALIFIER_SET_GENERAL_TRAP_INFO_SET_ID,1))
  {
    ui_ret = ui_ppd_api_llp_trap_prog_trap_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_TRAP_PROG_TRAP_QUALIFIER_GET_GENERAL_TRAP_INFO_GET_ID,1))
  {
    ui_ret = ui_ppd_api_llp_trap_prog_trap_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_TRAP_ARP_INFO_SET_ARP_INFO_SET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_llp_trap_arp_info_set(current_line); 
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_TRAP_ARP_INFO_GET_ARP_INFO_GET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_llp_trap_arp_info_get(current_line); 
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_MIRROR_PORT_VLAN_ADD_LLP_MIRROR_PORT_VLAN_ADD_ID,1)) 
  { 
    ui_ret = ui_ppd_api_llp_trap_llp_mirror_port_vlan_add(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_MIRROR_PORT_VLAN_REMOVE_LLP_MIRROR_PORT_VLAN_REMOVE_ID,1)) 
  { 
    ui_ret = ui_ppd_api_llp_trap_llp_mirror_port_vlan_remove(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_MIRROR_PORT_VLAN_GET_LLP_MIRROR_PORT_VLAN_GET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_llp_trap_llp_mirror_port_vlan_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_MIRROR_PORT_DFLT_SET_LLP_MIRROR_PORT_DFLT_SET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_llp_trap_llp_mirror_port_dflt_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_MIRROR_PORT_DFLT_GET_LLP_MIRROR_PORT_DFLT_GET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_llp_trap_llp_mirror_port_dflt_get(current_line); 
  } 
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after llp_trap***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif
#endif /* LINK_PPD_LIBRARIES */ 

