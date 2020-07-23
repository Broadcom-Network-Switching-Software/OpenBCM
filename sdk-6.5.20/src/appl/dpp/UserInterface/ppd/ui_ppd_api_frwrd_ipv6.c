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
#include <soc/dpp/PPD/ppd_api_frwrd_ipv6.h>
#include <appl/diag/dpp/utils_defi.h> 
#include <appl/dpp/UserInterface/ui_defi.h> 
 
#include <appl/dpp/UserInterface/ui_pure_defi_ppd_api.h> 

 
  
#include <soc/dpp/PPD/ppd_api_frwrd_ipv6.h>
#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_frwrd_ipv6.h>

#if LINK_PPD_LIBRARIES

#ifdef UI_FRWRD_IPV6

#define UI_PPD_API_FRWRD_IPV6_GET_BLOCK_NOF_ENTRIES 100

/******************************************************************** 
 *  Function handler: uc_route_add (section frwrd_ipv6)
 */
int 
  ui_ppd_api_frwrd_ipv6_uc_route_add(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY   
    prm_route_key;
  SOC_PPC_FEC_ID   
    prm_fec_id = 0xffffffff;
  SOC_SAND_SUCCESS_FAILURE   
    prm_success;
  SOC_PPC_FRWRD_DECISION_INFO
    info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv6"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv6_uc_route_add"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY_clear(&prm_route_key);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_UC_ROUTE_ADD_UC_ROUTE_ADD_ROUTE_KEY_SUBNET_PREFIX_LEN_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV6_UC_ROUTE_ADD_UC_ROUTE_ADD_ROUTE_KEY_SUBNET_PREFIX_LEN_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_route_key.subnet.prefix_len = (uint8)param_val->value.ulong_value;
  } 
  if (!get_val_of(
         current_line,(int *)&match_index,SOC_PARAM_PPD_FRWRD_IPV6_UC_ROUTE_ADD_UC_ROUTE_ADD_ROUTE_KEY_SUBNET_IPV6_ADDRESS_ID,1,  
         &param_val,VAL_TEXT,err_msg)) 
  { 
    soc_sand_pp_ipv6_address_string_parse(param_val->value.val_text, &(prm_route_key.subnet.ipv6_address));
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter route_key after uc_route_add***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_UC_ROUTE_ADD_UC_ROUTE_ADD_FEC_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV6_UC_ROUTE_ADD_UC_ROUTE_ADD_FEC_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fec_id = (uint32)param_val->value.ulong_value;
  } 

  SOC_PPC_FRWRD_DECISION_INFO_clear(&info);
  info.dest_id = prm_fec_id;
  info.type = SOC_PPC_FRWRD_DECISION_TYPE_FEC;

  /* Call function */
  ret = soc_ppd_frwrd_ipv6_uc_route_add(
          unit,
          &prm_route_key,
          &info,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_ipv6_uc_route_add - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv6_uc_route_add");   
    goto exit; 
  } 

  cli_out("success: %s\n\r",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: uc_route_get (section frwrd_ipv6)
 */
int 
  ui_ppd_api_frwrd_ipv6_uc_route_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY   
    prm_route_key;
  uint8   
    prm_exact_match = TRUE;
  SOC_PPC_FRWRD_DECISION_INFO
    prm_route_info;
  SOC_PPC_FRWRD_IP_ROUTE_STATUS   
    prm_route_status;
  SOC_PPC_FRWRD_IP_ROUTE_LOCATION   
    prm_location;
  uint8   
    prm_found;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv6"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv6_uc_route_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY_clear(&prm_route_key);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_UC_ROUTE_GET_UC_ROUTE_GET_ROUTE_KEY_SUBNET_PREFIX_LEN_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV6_UC_ROUTE_GET_UC_ROUTE_GET_ROUTE_KEY_SUBNET_PREFIX_LEN_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_route_key.subnet.prefix_len = (uint8)param_val->value.ulong_value;
  } 
  if (!get_val_of(
         current_line,(int *)&match_index,SOC_PARAM_PPD_FRWRD_IPV6_UC_ROUTE_GET_UC_ROUTE_GET_ROUTE_KEY_SUBNET_IPV6_ADDRESS_ID,1,  
         &param_val,VAL_TEXT,err_msg)) 
  { 
    soc_sand_pp_ipv6_address_string_parse(param_val->value.val_text, &(prm_route_key.subnet.ipv6_address));
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter route_key after uc_route_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_UC_ROUTE_GET_UC_ROUTE_GET_EXACT_MATCH_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV6_UC_ROUTE_GET_UC_ROUTE_GET_EXACT_MATCH_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_exact_match = (uint8)param_val->value.ulong_value;
  } 

  /* Call function */
  ret = soc_ppd_frwrd_ipv6_uc_route_get(
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
    send_string_to_screen(" *** soc_ppd_frwrd_ipv6_uc_route_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv6_uc_route_get");   
    goto exit; 
  } 

  cli_out("fec_id: %u\n\r",prm_route_info.dest_id);

  cli_out("route_status: %s\n\r",SOC_PPC_FRWRD_IP_ROUTE_STATUS_to_string(prm_route_status));

  cli_out("location: %s\n\r",SOC_PPC_FRWRD_IP_ROUTE_LOCATION_to_string(prm_location));

  cli_out("found: %u\n\r",prm_found);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: uc_route_get_block (section frwrd_ipv6)
 */
int 
  ui_ppd_api_frwrd_ipv6_uc_route_get_block(
    CURRENT_LINE *current_line 
  ) 
{   
 uint32 
    ret;   
  SOC_PPC_IP_ROUTING_TABLE_RANGE   
    prm_block_range;
  SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY   
    prm_route_keys[UI_PPD_API_FRWRD_IPV6_GET_BLOCK_NOF_ENTRIES];
  SOC_PPC_FRWRD_IP_ROUTE_STATUS   
    prm_routes_status[UI_PPD_API_FRWRD_IPV6_GET_BLOCK_NOF_ENTRIES];
  SOC_PPC_FRWRD_IP_ROUTE_LOCATION   
    prm_routes_location[UI_PPD_API_FRWRD_IPV6_GET_BLOCK_NOF_ENTRIES];
  uint32   
    prm_nof_entries;
  uint32
    i;
  SOC_PPC_FRWRD_DECISION_INFO
    prm_route_infos[UI_PPD_API_FRWRD_IPV6_GET_BLOCK_NOF_ENTRIES];

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv6"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv6_uc_route_get_block"; 

  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_IP_ROUTING_TABLE_RANGE_clear(&prm_block_range);

  /* Get parameters */ 
  /*
    if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_UC_ROUTE_GET_BLOCK_MC_ROUTE_GET_BLOCK_BLOCK_RANGE_KEY_ENTRIES_TO_ACT_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV6_UC_ROUTE_GET_BLOCK_MC_ROUTE_GET_BLOCK_BLOCK_RANGE_KEY_ENTRIES_TO_ACT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_block_range_key.entries_to_act = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_UC_ROUTE_GET_BLOCK_MC_ROUTE_GET_BLOCK_BLOCK_RANGE_KEY_ENTRIES_TO_SCAN_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV6_UC_ROUTE_GET_BLOCK_MC_ROUTE_GET_BLOCK_BLOCK_RANGE_KEY_ENTRIES_TO_SCAN_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_block_range_key.entries_to_scan = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_UC_ROUTE_GET_BLOCK_MC_ROUTE_GET_BLOCK_BLOCK_RANGE_KEY_START_PAYLOAD_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV6_UC_ROUTE_GET_BLOCK_MC_ROUTE_GET_BLOCK_BLOCK_RANGE_KEY_START_PAYLOAD_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    soc_sand_u64_add_long(&prm_block_range_key.start.payload, param_val->value.ulong_value);
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_UC_ROUTE_GET_BLOCK_MC_ROUTE_GET_BLOCK_BLOCK_RANGE_KEY_START_TYPE_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_IPV6_UC_ROUTE_GET_BLOCK_MC_ROUTE_GET_BLOCK_BLOCK_RANGE_KEY_START_TYPE_ID);  
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
          "|               Subnet                    | Fec  |                             |\n\r"
          "|                                         |      |                             |\n=\r"
          " ------------------------------------------------------------------------------ \n\r"
          );

  prm_block_range.entries_to_act = UI_PPD_API_FRWRD_IPV6_GET_BLOCK_NOF_ENTRIES;
  prm_block_range.entries_to_scan = 32 * 1024;
  prm_block_range.start.type = SOC_PPC_IP_ROUTING_TABLE_ITER_TYPE_IP_PREFIX_ORDERED;

  while(!SOC_PPC_IP_ROUTING_TABLE_ITER_IS_END(&prm_block_range.start.payload))
  {
    /* Call function */
    ret = soc_ppd_frwrd_ipv6_uc_route_get_block(
            unit,
            &prm_block_range,
            prm_route_keys,
            prm_route_infos,
            prm_routes_status,
            prm_routes_location,
            &prm_nof_entries
            );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
    { 
      send_string_to_screen(" *** soc_ppd_frwrd_ipv6_uc_route_get_block - FAIL", TRUE); 
      soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv6_uc_route_get_block");   
      goto exit; 
    } 

    for(i = 0; i < prm_nof_entries; i++)
    {
      soc_sand_SAND_PP_IPV6_ADDRESS_print(&prm_route_keys[i].subnet.ipv6_address);
      cli_out("/%-2d", prm_route_keys[i].subnet.prefix_len);

      
      cli_out("|%-5d", prm_route_infos[i].dest_id);
/*
      cli_out("|%-9s", SOC_PPC_FRWRD_IP_ROUTE_STATUS_to_string(prm_routes_status[i]));
      cli_out("|%-13s|", SOC_PPC_FRWRD_IP_ROUTE_LOCATION_to_string(prm_routes_location[i]));
*/
      cli_out("\n\r");
    }
  }
  
  goto exit; 
exit:
  return ui_ret;
} 
  
/******************************************************************** 
 *  Function handler: uc_route_remove (section frwrd_ipv6)
 */
int 
  ui_ppd_api_frwrd_ipv6_uc_route_remove(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY   
    prm_route_key;
  SOC_SAND_SUCCESS_FAILURE   
    prm_success;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv6"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv6_uc_route_remove"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY_clear(&prm_route_key);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_UC_ROUTE_REMOVE_UC_ROUTE_REMOVE_ROUTE_KEY_SUBNET_PREFIX_LEN_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV6_UC_ROUTE_REMOVE_UC_ROUTE_REMOVE_ROUTE_KEY_SUBNET_PREFIX_LEN_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_route_key.subnet.prefix_len = (uint8)param_val->value.ulong_value;
  } 
  if (!get_val_of(
         current_line,(int *)&match_index,SOC_PARAM_PPD_FRWRD_IPV6_UC_ROUTE_REMOVE_UC_ROUTE_REMOVE_ROUTE_KEY_SUBNET_IPV6_ADDRESS_ID,1,  
         &param_val,VAL_TEXT,err_msg)) 
  { 
    soc_sand_pp_ipv6_address_string_parse(param_val->value.val_text, &(prm_route_key.subnet.ipv6_address));
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter route_key after uc_route_remove***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_frwrd_ipv6_uc_route_remove(
          unit,
          &prm_route_key,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_ipv6_uc_route_remove - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv6_uc_route_remove");   
    goto exit; 
  } 

  cli_out("success: %s\n\r",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));

  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: uc_routing_table_clear (section frwrd_ipv6)
 */
int 
  ui_ppd_api_frwrd_ipv6_uc_routing_table_clear(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv6"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv6_uc_routing_table_clear"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_ppd_frwrd_ipv6_uc_routing_table_clear(
          unit
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_ipv6_uc_routing_table_clear - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv6_uc_routing_table_clear");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: mc_route_add (section frwrd_ipv6)
 */
int 
  ui_ppd_api_frwrd_ipv6_mc_route_add(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY   
    prm_route_key;
  SOC_PPC_FRWRD_IPV6_MC_ROUTE_INFO   
    prm_route_info;
  SOC_SAND_SUCCESS_FAILURE   
    prm_success;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv6"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv6_mc_route_add"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY_clear(&prm_route_key);
  SOC_PPC_FRWRD_IPV6_MC_ROUTE_INFO_clear(&prm_route_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_MC_ROUTE_ADD_MC_ROUTE_ADD_ROUTE_KEY_INRIF_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV6_MC_ROUTE_ADD_MC_ROUTE_ADD_ROUTE_KEY_INRIF_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_route_key.inrif.val = param_val->value.ulong_value;
    prm_route_key.inrif.mask = 0xff;
  } 
  if (!get_val_of(
         current_line,(int *)&match_index,SOC_PARAM_PPD_FRWRD_IPV6_MC_ROUTE_ADD_MC_ROUTE_ADD_ROUTE_KEY_GROUP_ID,1,  
         &param_val,VAL_TEXT,err_msg)) 
  { 
    soc_sand_pp_ipv6_address_string_parse(param_val->value.val_text, &(prm_route_key.group.ipv6_address));
    prm_route_key.group.prefix_len = _SHR_L3_IP6_MAX_NETLEN;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter route_key after mc_route_add***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_MC_ROUTE_ADD_MC_ROUTE_ADD_ROUTE_INFO_DEST_ID_DEST_VAL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV6_MC_ROUTE_ADD_MC_ROUTE_ADD_ROUTE_INFO_DEST_ID_DEST_VAL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_route_info.dest_id.dest_val = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_MC_ROUTE_ADD_MC_ROUTE_ADD_ROUTE_INFO_DEST_ID_DEST_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_IPV6_MC_ROUTE_ADD_MC_ROUTE_ADD_ROUTE_INFO_DEST_ID_DEST_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_route_info.dest_id.dest_type = param_val->numeric_equivalent;
  } 

  /* Call function */
  ret = soc_ppd_frwrd_ipv6_mc_route_add(
          unit,
          &prm_route_key,
          &prm_route_info,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_ipv6_mc_route_add - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv6_mc_route_add");   
    goto exit; 
  } 

  cli_out("success: %s\n\r",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: mc_route_get (section frwrd_ipv6)
 */
int 
  ui_ppd_api_frwrd_ipv6_mc_route_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY   
    prm_route_key;
  uint8   
    prm_exact_match = TRUE;
  SOC_PPC_FRWRD_IPV6_MC_ROUTE_INFO   
    prm_route_info;
  SOC_PPC_FRWRD_IP_ROUTE_STATUS   
    prm_route_status;
  SOC_PPC_FRWRD_IP_ROUTE_LOCATION   
    prm_location;
  uint8   
    prm_found;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv6"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv6_mc_route_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY_clear(&prm_route_key);
  SOC_PPC_FRWRD_IPV6_MC_ROUTE_INFO_clear(&prm_route_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_MC_ROUTE_GET_MC_ROUTE_GET_ROUTE_KEY_INRIF_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV6_MC_ROUTE_GET_MC_ROUTE_GET_ROUTE_KEY_INRIF_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_route_key.inrif.val  = param_val->value.ulong_value;
    prm_route_key.inrif.mask = 0xff;
  } 
  if (!get_val_of(
         current_line,(int *)&match_index,SOC_PARAM_PPD_FRWRD_IPV6_MC_ROUTE_GET_MC_ROUTE_GET_ROUTE_KEY_GROUP_ID,1,  
         &param_val,VAL_TEXT,err_msg)) 
  { 
    soc_sand_pp_ipv6_address_string_parse(param_val->value.val_text, &(prm_route_key.group.ipv6_address));
    prm_route_key.group.prefix_len = _SHR_L3_IP6_MAX_NETLEN;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter route_key after mc_route_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_MC_ROUTE_GET_MC_ROUTE_GET_EXACT_MATCH_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV6_MC_ROUTE_GET_MC_ROUTE_GET_EXACT_MATCH_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_exact_match = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_frwrd_ipv6_mc_route_get(
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
    send_string_to_screen(" *** soc_ppd_frwrd_ipv6_mc_route_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv6_mc_route_get");   
    goto exit; 
  } 

  SOC_PPC_FRWRD_IPV6_MC_ROUTE_INFO_print(&prm_route_info);

  cli_out("route_status: %s\n\r",SOC_PPC_FRWRD_IP_ROUTE_STATUS_to_string(prm_route_status));

  cli_out("location: %s\n\r",SOC_PPC_FRWRD_IP_ROUTE_LOCATION_to_string(prm_location));

  cli_out("found: %u\n\r",prm_found);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: mc_route_get_block (section frwrd_ipv6)
 */
int 
  ui_ppd_api_frwrd_ipv6_mc_route_get_block(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_IP_ROUTING_TABLE_RANGE   
    prm_block_range;
  SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY   
    prm_route_keys[UI_PPD_API_FRWRD_IPV6_GET_BLOCK_NOF_ENTRIES];
  SOC_PPC_FRWRD_IPV6_MC_ROUTE_INFO    
    prm_fec_ids[UI_PPD_API_FRWRD_IPV6_GET_BLOCK_NOF_ENTRIES];
  SOC_PPC_FRWRD_IP_ROUTE_STATUS   
    prm_routes_status[UI_PPD_API_FRWRD_IPV6_GET_BLOCK_NOF_ENTRIES];
  SOC_PPC_FRWRD_IP_ROUTE_LOCATION   
    prm_routes_location[UI_PPD_API_FRWRD_IPV6_GET_BLOCK_NOF_ENTRIES];
  uint32   
    prm_nof_entries;
  uint32
    i;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv6"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv6_mc_route_get_block"; 

  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_IP_ROUTING_TABLE_RANGE_clear(&prm_block_range);

  /* Get parameters */ 
  /*
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_MC_ROUTE_GET_BLOCK_MC_ROUTE_GET_BLOCK_BLOCK_RANGE_KEY_ENTRIES_TO_ACT_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV6_MC_ROUTE_GET_BLOCK_MC_ROUTE_GET_BLOCK_BLOCK_RANGE_KEY_ENTRIES_TO_ACT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_block_range_key.entries_to_act = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_MC_ROUTE_GET_BLOCK_MC_ROUTE_GET_BLOCK_BLOCK_RANGE_KEY_ENTRIES_TO_SCAN_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV6_MC_ROUTE_GET_BLOCK_MC_ROUTE_GET_BLOCK_BLOCK_RANGE_KEY_ENTRIES_TO_SCAN_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_block_range_key.entries_to_scan = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_MC_ROUTE_GET_BLOCK_MC_ROUTE_GET_BLOCK_BLOCK_RANGE_KEY_START_PAYLOAD_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV6_MC_ROUTE_GET_BLOCK_MC_ROUTE_GET_BLOCK_BLOCK_RANGE_KEY_START_PAYLOAD_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_block_range_key.start.payload = (SOC_SAND_U64)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_MC_ROUTE_GET_BLOCK_MC_ROUTE_GET_BLOCK_BLOCK_RANGE_KEY_START_TYPE_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_IPV6_MC_ROUTE_GET_BLOCK_MC_ROUTE_GET_BLOCK_BLOCK_RANGE_KEY_START_TYPE_ID);  
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
          "|               Subnet                    | Inrif  | Fec   |                   |\n\r"
          "|                                         |        |       |                   |\n\r"
          " ------------------------------------------------------------------------------ \n\r"
          );

  prm_block_range.entries_to_act = UI_PPD_API_FRWRD_IPV6_GET_BLOCK_NOF_ENTRIES;
  prm_block_range.entries_to_scan = 32 * 1024;
  prm_block_range.start.type = SOC_PPC_IP_ROUTING_TABLE_ITER_TYPE_IP_PREFIX_ORDERED;

  while(!SOC_PPC_IP_ROUTING_TABLE_ITER_IS_END(&prm_block_range.start.payload))
  {
    /* Call function */
    ret = soc_ppd_frwrd_ipv6_mc_route_get_block(
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
      send_string_to_screen(" *** soc_ppd_frwrd_ipv6_mc_route_get_block - FAIL", TRUE); 
      soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv6_mc_route_get_block");   
      goto exit; 
    } 

    for(i = 0; i < prm_nof_entries; i++)
    {
      cli_out("|");
      soc_sand_SAND_PP_IPV6_SUBNET_print(&prm_route_keys[i].group);
      
	    cli_out("      |%-7d", prm_route_keys[i].inrif.val);
      
      SOC_PPC_FRWRD_IPV6_MC_ROUTE_INFO_print(&prm_fec_ids[i]);
/*
      cli_out("|%-9s", SOC_PPC_FRWRD_IP_ROUTE_STATUS_to_string(prm_routes_status[i]));
      cli_out("|%-13s|", SOC_PPC_FRWRD_IP_ROUTE_LOCATION_to_string(prm_routes_location[i]));
*/
      cli_out("|\n\r");
    }
  }
  
  goto exit; 
exit:
  return ui_ret;
} 
  
/******************************************************************** 
 *  Function handler: mc_route_remove (section frwrd_ipv6)
 */
int 
  ui_ppd_api_frwrd_ipv6_mc_route_remove(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY   
    prm_route_key;
  SOC_SAND_SUCCESS_FAILURE   
    prm_success;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv6"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv6_mc_route_remove"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY_clear(&prm_route_key);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_MC_ROUTE_REMOVE_MC_ROUTE_REMOVE_ROUTE_KEY_INRIF_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV6_MC_ROUTE_REMOVE_MC_ROUTE_REMOVE_ROUTE_KEY_INRIF_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_route_key.inrif.val  = param_val->value.ulong_value;
    prm_route_key.inrif.mask = 0xff;
  } 
  if (!get_val_of(
         current_line,(int *)&match_index,SOC_PARAM_PPD_FRWRD_IPV6_MC_ROUTE_REMOVE_MC_ROUTE_REMOVE_ROUTE_KEY_GROUP_ID,1,  
         &param_val,VAL_TEXT,err_msg)) 
  { 
    soc_sand_pp_ipv6_address_string_parse(param_val->value.val_text, &(prm_route_key.group.ipv6_address));
    prm_route_key.group.prefix_len = _SHR_L3_IP6_MAX_NETLEN;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter route_key after mc_route_remove***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_frwrd_ipv6_mc_route_remove(
          unit,
          &prm_route_key,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_ipv6_mc_route_remove - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv6_mc_route_remove");   
    goto exit; 
  } 

  cli_out("success: %s\n\r",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));

  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: mc_routing_table_clear (section frwrd_ipv6)
 */
int 
  ui_ppd_api_frwrd_ipv6_mc_routing_table_clear(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv6"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv6_mc_routing_table_clear"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_ppd_frwrd_ipv6_mc_routing_table_clear(
          unit
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_ipv6_mc_routing_table_clear - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv6_mc_routing_table_clear");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: vrf_info_set (section frwrd_ipv6)
 */
int 
  ui_ppd_api_frwrd_ipv6_vrf_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_VRF_ID   
    prm_vrf_ndx;
  SOC_PPC_FRWRD_IPV6_VRF_INFO   
    prm_vrf_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv6"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv6_vrf_info_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_IPV6_VRF_INFO_clear(&prm_vrf_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_VRF_INFO_SET_VRF_INFO_SET_VRF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV6_VRF_INFO_SET_VRF_INFO_SET_VRF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_vrf_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter vrf_ndx after vrf_info_set***", TRUE); 
    goto exit; 
  } 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_frwrd_ipv6_vrf_info_get(
          unit,
          prm_vrf_ndx,
          &prm_vrf_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_ipv6_vrf_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv6_vrf_info_get");   
    goto exit; 
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_VRF_INFO_SET_VRF_INFO_SET_VRF_INFO_ROUTER_INFO_UC_DEFAULT_ACTION_VALUE_FEC_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV6_VRF_INFO_SET_VRF_INFO_SET_VRF_INFO_ROUTER_INFO_UC_DEFAULT_ACTION_VALUE_FEC_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_vrf_info.router_info.uc_default_action.value.fec_id = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_frwrd_ipv6_vrf_info_set(
          unit,
          prm_vrf_ndx,
          &prm_vrf_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_ipv6_vrf_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv6_vrf_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: vrf_info_get (section frwrd_ipv6)
 */
int 
  ui_ppd_api_frwrd_ipv6_vrf_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_VRF_ID   
    prm_vrf_ndx;
  SOC_PPC_FRWRD_IPV6_VRF_INFO   
    prm_vrf_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv6"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv6_vrf_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_IPV6_VRF_INFO_clear(&prm_vrf_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_VRF_INFO_GET_VRF_INFO_GET_VRF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV6_VRF_INFO_GET_VRF_INFO_GET_VRF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_vrf_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter vrf_ndx after vrf_info_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_frwrd_ipv6_vrf_info_get(
          unit,
          prm_vrf_ndx,
          &prm_vrf_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_ipv6_vrf_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv6_vrf_info_get");   
    goto exit; 
  } 

  SOC_PPC_FRWRD_IPV6_VRF_INFO_print(&prm_vrf_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: vrf_route_add (section frwrd_ipv6)
 */
int 
  ui_ppd_api_frwrd_ipv6_vrf_route_add(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_VRF_ID   
    prm_vrf_ndx;
  SOC_PPC_FRWRD_IPV6_VPN_ROUTE_KEY   
    prm_route_key;
  SOC_PPC_FEC_ID   
    prm_fec_id = 0xffffffff;
  SOC_SAND_SUCCESS_FAILURE   
    prm_success;
  SOC_PPC_FRWRD_DECISION_INFO
    info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv6"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv6_vrf_route_add"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_IPV6_VPN_ROUTE_KEY_clear(&prm_route_key);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_VRF_ROUTE_ADD_VRF_ROUTE_ADD_VRF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV6_VRF_ROUTE_ADD_VRF_ROUTE_ADD_VRF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_vrf_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter vrf_ndx after vrf_route_add***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_VRF_ROUTE_ADD_VRF_ROUTE_ADD_ROUTE_KEY_SUBNET_PREFIX_LEN_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV6_VRF_ROUTE_ADD_VRF_ROUTE_ADD_ROUTE_KEY_SUBNET_PREFIX_LEN_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_route_key.subnet.prefix_len = (uint8)param_val->value.ulong_value;
  } 
  if (!get_val_of(
         current_line,(int *)&match_index,SOC_PARAM_PPD_FRWRD_IPV6_VRF_ROUTE_ADD_VRF_ROUTE_ADD_ROUTE_KEY_SUBNET_IPV6_ADDRESS_ID,1,  
         &param_val,VAL_TEXT,err_msg)) 
  { 
    soc_sand_pp_ipv6_address_string_parse(param_val->value.val_text, &(prm_route_key.subnet.ipv6_address));
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter route_key after vrf_route_add***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_VRF_ROUTE_ADD_VRF_ROUTE_ADD_FEC_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV6_VRF_ROUTE_ADD_VRF_ROUTE_ADD_FEC_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fec_id = (uint32)param_val->value.ulong_value;
  } 

  SOC_PPC_FRWRD_DECISION_INFO_clear(&info);
  info.dest_id = prm_fec_id;
  info.type = SOC_PPC_FRWRD_DECISION_TYPE_FEC;

  /* Call function */
  ret = soc_ppd_frwrd_ipv6_vrf_route_add(
          unit,
          prm_vrf_ndx,
          &prm_route_key,
          &info,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_ipv6_vrf_route_add - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv6_vrf_route_add");   
    goto exit; 
  } 

  cli_out("success: %s\n\r",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: vrf_route_get (section frwrd_ipv6)
 */
int 
  ui_ppd_api_frwrd_ipv6_vrf_route_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_VRF_ID   
    prm_vrf_ndx;
  SOC_PPC_FRWRD_IPV6_VPN_ROUTE_KEY   
    prm_route_key;
  uint8   
    prm_exact_match = TRUE;
  SOC_PPC_FRWRD_IP_ROUTE_STATUS   
    prm_route_status;
  SOC_PPC_FRWRD_IP_ROUTE_LOCATION   
    prm_location;
  uint8   
    prm_found;
  SOC_PPC_FRWRD_DECISION_INFO
    prm_route_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv6"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv6_vrf_route_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_IPV6_VPN_ROUTE_KEY_clear(&prm_route_key);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_VRF_ROUTE_GET_VRF_ROUTE_GET_VRF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV6_VRF_ROUTE_GET_VRF_ROUTE_GET_VRF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_vrf_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter vrf_ndx after vrf_route_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_VRF_ROUTE_GET_VRF_ROUTE_GET_ROUTE_KEY_SUBNET_PREFIX_LEN_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV6_VRF_ROUTE_GET_VRF_ROUTE_GET_ROUTE_KEY_SUBNET_PREFIX_LEN_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_route_key.subnet.prefix_len = (uint8)param_val->value.ulong_value;
  } 
  if (!get_val_of(
         current_line,(int *)&match_index,SOC_PARAM_PPD_FRWRD_IPV6_VRF_ROUTE_GET_VRF_ROUTE_GET_ROUTE_KEY_SUBNET_IPV6_ADDRESS_ID,1,  
         &param_val,VAL_TEXT,err_msg)) 
  { 
    soc_sand_pp_ipv6_address_string_parse(param_val->value.val_text, &(prm_route_key.subnet.ipv6_address));
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter route_key after vrf_route_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_VRF_ROUTE_GET_VRF_ROUTE_GET_EXACT_MATCH_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV6_VRF_ROUTE_GET_VRF_ROUTE_GET_EXACT_MATCH_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_exact_match = (uint8)param_val->value.ulong_value;
  } 

  /* Call function */
  ret = soc_ppd_frwrd_ipv6_vrf_route_get(
          unit,
          prm_vrf_ndx,
          &prm_route_key,
          prm_exact_match,
          &prm_route_info,
          &prm_route_status,
          &prm_location,
          &prm_found
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_ipv6_vrf_route_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv6_vrf_route_get");   
    goto exit; 
  } 

  cli_out("fec_id: %u\n\r",prm_route_info.dest_id);

  cli_out("route_status: %s\n\r",SOC_PPC_FRWRD_IP_ROUTE_STATUS_to_string(prm_route_status));

  cli_out("location: %s\n\r",SOC_PPC_FRWRD_IP_ROUTE_LOCATION_to_string(prm_location));

  cli_out("found: %u\n\r",prm_found);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: vrf_route_get_block (section frwrd_ipv6)
 */
int 
  ui_ppd_api_frwrd_ipv6_vrf_route_get_block(
    CURRENT_LINE *current_line 
  ) 
{  
  uint32 
    ret;   
  SOC_PPC_IP_ROUTING_TABLE_RANGE   
    prm_block_range;
  SOC_PPC_FRWRD_IPV6_VPN_ROUTE_KEY   
    prm_route_keys[UI_PPD_API_FRWRD_IPV6_GET_BLOCK_NOF_ENTRIES];
  SOC_PPC_FRWRD_DECISION_INFO   
    prm_route_infos[UI_PPD_API_FRWRD_IPV6_GET_BLOCK_NOF_ENTRIES];
  SOC_PPC_FRWRD_IP_ROUTE_STATUS   
    prm_routes_status[UI_PPD_API_FRWRD_IPV6_GET_BLOCK_NOF_ENTRIES];
  SOC_PPC_FRWRD_IP_ROUTE_LOCATION   
    prm_routes_location[UI_PPD_API_FRWRD_IPV6_GET_BLOCK_NOF_ENTRIES];
  uint32   
    prm_nof_entries,
	prm_vrf_ndx;
  uint32
    i;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv6"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv6_mc_route_get_block"; 

  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_IP_ROUTING_TABLE_RANGE_clear(&prm_block_range);

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_VRF_ROUTE_GET_BLOCK_VRF_ROUTE_GET_BLOCK_VRF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV6_VRF_ROUTE_GET_BLOCK_VRF_ROUTE_GET_BLOCK_VRF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_vrf_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter vrf_ndx after vrf_route_get_block***", TRUE); 
    goto exit; 
  } 

  /* Get parameters */ 
  /*
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_VRF_ROUTE_GET_BLOCK_VRF_ROUTE_GET_BLOCK_BLOCK_RANGE_KEY_ENTRIES_TO_ACT_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV6_VRF_ROUTE_GET_BLOCK_VRF_ROUTE_GET_BLOCK_BLOCK_RANGE_KEY_ENTRIES_TO_ACT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_block_range_key.entries_to_act = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_VRF_ROUTE_GET_BLOCK_VRF_ROUTE_GET_BLOCK_BLOCK_RANGE_KEY_ENTRIES_TO_SCAN_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV6_VRF_ROUTE_GET_BLOCK_VRF_ROUTE_GET_BLOCK_BLOCK_RANGE_KEY_ENTRIES_TO_SCAN_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_block_range_key.entries_to_scan = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_VRF_ROUTE_GET_BLOCK_VRF_ROUTE_GET_BLOCK_BLOCK_RANGE_KEY_START_PAYLOAD_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV6_VRF_ROUTE_GET_BLOCK_VRF_ROUTE_GET_BLOCK_BLOCK_RANGE_KEY_START_PAYLOAD_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_block_range_key.start.payload = (SOC_SAND_U64)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_VRF_ROUTE_GET_BLOCK_VRF_ROUTE_GET_BLOCK_BLOCK_RANGE_KEY_START_TYPE_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_IPV6_VRF_ROUTE_GET_BLOCK_VRF_ROUTE_GET_BLOCK_BLOCK_RANGE_KEY_START_TYPE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_block_range_key.start.type = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter block_range_key after vrf_route_get_block***", TRUE); 
    goto exit; 
  } 
 
  */

  cli_out(" ------------------------------------------------------------------------------\n\r"
          "|                                   Routing Table                              |\n\r"
          "|------------------------------------------------------------------------------|\n\r"
          "| VRF |               Subnet                    | Inrif  | Fec   |             |\n\r"
          "|     |                                         |        |       |             |\n=\r"
          " ------------------------------------------------------------------------------ \n\r"
          );

  prm_block_range.entries_to_act = UI_PPD_API_FRWRD_IPV6_GET_BLOCK_NOF_ENTRIES;
  prm_block_range.entries_to_scan = 32 * 1024;
  prm_block_range.start.type = SOC_PPC_IP_ROUTING_TABLE_ITER_TYPE_IP_PREFIX_ORDERED;

  while(!SOC_PPC_IP_ROUTING_TABLE_ITER_IS_END(&prm_block_range.start.payload))
  {
    /* Call function */
    ret = soc_ppd_frwrd_ipv6_vrf_route_get_block(
            unit,
			      prm_vrf_ndx,
            &prm_block_range,
            prm_route_keys,
            prm_route_infos,
            prm_routes_status,
            prm_routes_location,
            &prm_nof_entries
            );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
    { 
      send_string_to_screen(" *** soc_ppd_frwrd_ipv6_mc_route_get_block - FAIL", TRUE); 
      soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv6_mc_route_get_block");   
      goto exit; 
    } 

    for(i = 0; i < prm_nof_entries; i++)
    {
      cli_out("|%-5d", prm_vrf_ndx);

      soc_sand_SAND_PP_IPV6_ADDRESS_print(&prm_route_keys[i].subnet.ipv6_address);
      cli_out("/%-2d", prm_route_keys[i].subnet.prefix_len);

	  cli_out("|%-7d", prm_route_infos[i].dest_id);
/*
      cli_out("|%-9s", SOC_PPC_FRWRD_IP_ROUTE_STATUS_to_string(prm_routes_status[i]));
      cli_out("|%-13s|", SOC_PPC_FRWRD_IP_ROUTE_LOCATION_to_string(prm_routes_location[i]));
*/
      cli_out("\n\r");
    }
  }
  
  goto exit; 
exit:
  return ui_ret;
} 

  
/******************************************************************** 
 *  Function handler: vrf_route_remove (section frwrd_ipv6)
 */
int 
  ui_ppd_api_frwrd_ipv6_vrf_route_remove(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_VRF_ID   
    prm_vrf_ndx;
  SOC_PPC_FRWRD_IPV6_VPN_ROUTE_KEY   
    prm_route_key;
  SOC_SAND_SUCCESS_FAILURE   
    prm_success;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv6"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv6_vrf_route_remove"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_IPV6_VPN_ROUTE_KEY_clear(&prm_route_key);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_VRF_ROUTE_REMOVE_VRF_ROUTE_REMOVE_VRF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV6_VRF_ROUTE_REMOVE_VRF_ROUTE_REMOVE_VRF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_vrf_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter vrf_ndx after vrf_route_remove***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_VRF_ROUTE_REMOVE_VRF_ROUTE_REMOVE_ROUTE_KEY_SUBNET_PREFIX_LEN_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV6_VRF_ROUTE_REMOVE_VRF_ROUTE_REMOVE_ROUTE_KEY_SUBNET_PREFIX_LEN_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_route_key.subnet.prefix_len = (uint8)param_val->value.ulong_value;
  } 
  if (!get_val_of(
         current_line,(int *)&match_index,SOC_PARAM_PPD_FRWRD_IPV6_VRF_ROUTE_REMOVE_VRF_ROUTE_REMOVE_ROUTE_KEY_SUBNET_IPV6_ADDRESS_ID,1,  
         &param_val,VAL_TEXT,err_msg)) 
  { 
    soc_sand_pp_ipv6_address_string_parse(param_val->value.val_text, &(prm_route_key.subnet.ipv6_address));
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter route_key after vrf_route_remove***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_frwrd_ipv6_vrf_route_remove(
          unit,
          prm_vrf_ndx,
          &prm_route_key,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_ipv6_vrf_route_remove - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv6_vrf_route_remove");   
    goto exit; 
  } 

  cli_out("success: %s\n\r",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));

  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: vrf_routing_table_clear (section frwrd_ipv6)
 */
int 
  ui_ppd_api_frwrd_ipv6_vrf_routing_table_clear(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_VRF_ID   
    prm_vrf_ndx;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv6"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv6_vrf_routing_table_clear"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_VRF_ROUTING_TABLE_CLEAR_VRF_ROUTING_TABLE_CLEAR_VRF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV6_VRF_ROUTING_TABLE_CLEAR_VRF_ROUTING_TABLE_CLEAR_VRF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_vrf_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter vrf_ndx after vrf_routing_table_clear***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_frwrd_ipv6_vrf_routing_table_clear(
          unit,
          prm_vrf_ndx
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_ipv6_vrf_routing_table_clear - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv6_vrf_routing_table_clear");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: vrf_all_routing_tables_clear (section frwrd_ipv6)
 */
int 
  ui_ppd_api_frwrd_ipv6_vrf_all_routing_tables_clear(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv6"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv6_vrf_all_routing_tables_clear"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_ppd_frwrd_ipv6_vrf_all_routing_tables_clear(
          unit
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_ipv6_vrf_all_routing_tables_clear - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv6_vrf_all_routing_tables_clear");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
#endif
#ifdef UI_FRWRD_IPV6/* { frwrd_ipv6*/
/******************************************************************** 
 *  Section handler: frwrd_ipv6
 */ 
int 
  ui_ppd_api_frwrd_ipv6( 
    CURRENT_LINE *current_line 
  ) 
{   
  int
    ret = FALSE;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ipv6"); 
 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_UC_ROUTE_ADD_UC_ROUTE_ADD_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_ipv6_uc_route_add(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_UC_ROUTE_GET_UC_ROUTE_GET_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_ipv6_uc_route_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_UC_ROUTE_GET_BLOCK_UC_ROUTE_GET_BLOCK_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_ipv6_uc_route_get_block(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_UC_ROUTE_REMOVE_UC_ROUTE_REMOVE_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_ipv6_uc_route_remove(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_UC_ROUTING_TABLE_CLEAR_UC_ROUTING_TABLE_CLEAR_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_ipv6_uc_routing_table_clear(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_MC_ROUTE_ADD_MC_ROUTE_ADD_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_ipv6_mc_route_add(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_MC_ROUTE_GET_MC_ROUTE_GET_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_ipv6_mc_route_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_MC_ROUTE_GET_BLOCK_MC_ROUTE_GET_BLOCK_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_ipv6_mc_route_get_block(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_MC_ROUTE_REMOVE_MC_ROUTE_REMOVE_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_ipv6_mc_route_remove(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_MC_ROUTING_TABLE_CLEAR_MC_ROUTING_TABLE_CLEAR_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_ipv6_mc_routing_table_clear(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_VRF_INFO_SET_VRF_INFO_SET_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_ipv6_vrf_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_VRF_INFO_GET_VRF_INFO_GET_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_ipv6_vrf_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_VRF_ROUTE_ADD_VRF_ROUTE_ADD_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_ipv6_vrf_route_add(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_VRF_ROUTE_GET_VRF_ROUTE_GET_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_ipv6_vrf_route_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_VRF_ROUTE_GET_BLOCK_VRF_ROUTE_GET_BLOCK_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_ipv6_vrf_route_get_block(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_VRF_ROUTE_REMOVE_VRF_ROUTE_REMOVE_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_ipv6_vrf_route_remove(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_VRF_ROUTING_TABLE_CLEAR_VRF_ROUTING_TABLE_CLEAR_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_ipv6_vrf_routing_table_clear(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV6_VRF_ALL_ROUTING_TABLES_CLEAR_VRF_ALL_ROUTING_TABLES_CLEAR_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_ipv6_vrf_all_routing_tables_clear(current_line); 
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting function name after frwrd_ipv6***", TRUE); 
  } 
  
  ui_ret = ret;
    
  goto exit; 
exit:        
  return ui_ret; 
}                
          
#endif /* frwrd_ipv6 */ 


#endif /* LINK_PPD_LIBRARIES */ 

