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
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>
#include <appl/diag/dpp/utils_defi.h> 
#include <appl/dpp/UserInterface/ui_defi.h>

#include <soc/dpp/PPD/ppd_api_frwrd_trill.h>                                                               
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_frwrd_trill.h>

#if LINK_PPD_LIBRARIES

#ifdef UI_FRWRD_TRILL
/********************************************************************
*  Function handler: multicast_key_mask_set (section frwrd_trill)
 */
int
ui_ppd_api_frwrd_trill_multicast_key_mask_set(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_PPC_TRILL_MC_MASKED_FIELDS
    prm_masked_fields;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_trill");
  soc_sand_proc_name = "soc_ppd_frwrd_trill_multicast_key_mask_set";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_TRILL_MC_MASKED_FIELDS_clear(&prm_masked_fields);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_ppd_frwrd_trill_multicast_key_mask_get(
    unit,
    &prm_masked_fields
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_trill_multicast_key_mask_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_trill_multicast_key_mask_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_TRILL_MULTICAST_KEY_MASK_SET_MULTICAST_KEY_MASK_SET_MASKED_FIELDS_MASK_FID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_TRILL_MULTICAST_KEY_MASK_SET_MULTICAST_KEY_MASK_SET_MASKED_FIELDS_MASK_FID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_masked_fields.mask_fid = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_TRILL_MULTICAST_KEY_MASK_SET_MULTICAST_KEY_MASK_SET_MASKED_FIELDS_MASK_ADJACENT_NICKNAME_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_TRILL_MULTICAST_KEY_MASK_SET_MULTICAST_KEY_MASK_SET_MASKED_FIELDS_MASK_ADJACENT_NICKNAME_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_masked_fields.mask_adjacent_nickname = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_TRILL_MULTICAST_KEY_MASK_SET_MULTICAST_KEY_MASK_SET_MASKED_FIELDS_MASK_ING_NICKNAME_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_TRILL_MULTICAST_KEY_MASK_SET_MULTICAST_KEY_MASK_SET_MASKED_FIELDS_MASK_ING_NICKNAME_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_masked_fields.mask_ing_nickname = (uint8)param_val->value.ulong_value;
  }
  /* Call function */
  ret = soc_ppd_frwrd_trill_multicast_key_mask_set(
    unit,
    &prm_masked_fields
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_trill_multicast_key_mask_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_trill_multicast_key_mask_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: multicast_key_mask_get (section frwrd_trill)
 */
int
ui_ppd_api_frwrd_trill_multicast_key_mask_get(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_PPC_TRILL_MC_MASKED_FIELDS
    prm_masked_fields;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_trill");
  soc_sand_proc_name = "soc_ppd_frwrd_trill_multicast_key_mask_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_TRILL_MC_MASKED_FIELDS_clear(&prm_masked_fields);

  /* Get parameters */

  /* Call function */
  ret = soc_ppd_frwrd_trill_multicast_key_mask_get(
    unit,
    &prm_masked_fields
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_trill_multicast_key_mask_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_trill_multicast_key_mask_get");
    goto exit;
  }

  send_string_to_screen("--> masked_fields:", TRUE);
  SOC_PPC_TRILL_MC_MASKED_FIELDS_print(&prm_masked_fields);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: unicast_route_add (section frwrd_trill)
 */
int
ui_ppd_api_frwrd_trill_unicast_route_add(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_nickname_key;

  SOC_PPC_FRWRD_DECISION_INFO prm_fwd_decision_info; 


  SOC_SAND_SUCCESS_FAILURE
    prm_success;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_trill");
  soc_sand_proc_name = "soc_ppd_frwrd_trill_unicast_route_add";

  SOC_PPC_FRWRD_DECISION_INFO_clear(&prm_fwd_decision_info); 

  unit = soc_ppd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_TRILL_UNICAST_ROUTE_ADD_UNICAST_ROUTE_ADD_NICKNAME_KEY_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_TRILL_UNICAST_ROUTE_ADD_UNICAST_ROUTE_ADD_NICKNAME_KEY_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_nickname_key = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter nickname_key after unicast_route_add***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_TRILL_UNICAST_ROUTE_ADD_UNICAST_ROUTE_ADD_LIF_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_TRILL_UNICAST_ROUTE_ADD_UNICAST_ROUTE_ADD_LIF_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_TRILL_UNICAST_ROUTE_ADD_UNICAST_ROUTE_ADD_FEC_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_TRILL_UNICAST_ROUTE_ADD_UNICAST_ROUTE_ADD_FEC_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;

    SOC_PPD_FRWRD_DECISION_FEC_SET(
       unit, &prm_fwd_decision_info, (uint32)param_val->value.ulong_value, ret); 
  }

  /* Call function */
  ret = soc_ppd_frwrd_trill_unicast_route_add(
    unit,
    prm_nickname_key,
    &prm_fwd_decision_info,
    &prm_success
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_trill_unicast_route_add - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_trill_unicast_route_add");
    goto exit;
  }

  send_string_to_screen("--> success:", TRUE);
  cli_out("success: %s\n",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: unicast_route_get (section frwrd_trill)
 */
int
ui_ppd_api_frwrd_trill_unicast_route_get(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_nickname_key;
  SOC_PPC_FRWRD_DECISION_INFO prm_fwd_decision_info; 
  uint8   
    prm_is_found;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_trill");
  soc_sand_proc_name = "soc_ppd_frwrd_trill_unicast_route_get";

  unit = soc_ppd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_TRILL_UNICAST_ROUTE_GET_UNICAST_ROUTE_GET_NICKNAME_KEY_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_TRILL_UNICAST_ROUTE_GET_UNICAST_ROUTE_GET_NICKNAME_KEY_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_nickname_key = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter nickname_key after unicast_route_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_frwrd_trill_unicast_route_get(
    unit,
    prm_nickname_key,
    &prm_fwd_decision_info,
    &prm_is_found
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_trill_unicast_route_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_trill_unicast_route_get");
    goto exit;
  }

  send_string_to_screen("--> fec_id:", TRUE);
  cli_out("fec_id: %u\n",prm_fwd_decision_info.dest_id);

  send_string_to_screen("--> is_found:", TRUE);
  cli_out("is_found: %u\n\r",prm_is_found);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: unicast_route_remove (section frwrd_trill)
 */
int
ui_ppd_api_frwrd_trill_unicast_route_remove(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_nickname_key;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_trill");
  soc_sand_proc_name = "soc_ppd_frwrd_trill_unicast_route_remove";

  unit = soc_ppd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_TRILL_UNICAST_ROUTE_REMOVE_UNICAST_ROUTE_REMOVE_NICKNAME_KEY_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_TRILL_UNICAST_ROUTE_REMOVE_UNICAST_ROUTE_REMOVE_NICKNAME_KEY_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_nickname_key = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter nickname_key after unicast_route_remove***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_frwrd_trill_unicast_route_remove(
    unit,
    prm_nickname_key
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_trill_unicast_route_remove - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_trill_unicast_route_remove");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: multicast_route_add (section frwrd_trill)
 */
int
ui_ppd_api_frwrd_trill_multicast_route_add(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_PPC_TRILL_MC_ROUTE_KEY
    prm_trill_mc_key;
  uint32
    prm_mc_id = 0xffffffff;
  SOC_SAND_SUCCESS_FAILURE
    prm_success;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_trill");
  soc_sand_proc_name = "soc_ppd_frwrd_trill_multicast_route_add";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_TRILL_MC_ROUTE_KEY_clear(&prm_trill_mc_key);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_TRILL_MULTICAST_ROUTE_ADD_MULTICAST_ROUTE_ADD_TRILL_MC_KEY_ADJACENT_EEP_ID,1))          
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_TRILL_MULTICAST_ROUTE_ADD_MULTICAST_ROUTE_ADD_TRILL_MC_KEY_ADJACENT_EEP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_trill_mc_key.adjacent_eep = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_TRILL_MULTICAST_ROUTE_ADD_MULTICAST_ROUTE_ADD_TRILL_MC_KEY_ING_NICK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_TRILL_MULTICAST_ROUTE_ADD_MULTICAST_ROUTE_ADD_TRILL_MC_KEY_ING_NICK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_trill_mc_key.ing_nick = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_TRILL_MULTICAST_ROUTE_ADD_MULTICAST_ROUTE_ADD_TRILL_MC_KEY_FID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_TRILL_MULTICAST_ROUTE_ADD_MULTICAST_ROUTE_ADD_TRILL_MC_KEY_FID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_trill_mc_key.fid = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_TRILL_MULTICAST_ROUTE_ADD_MULTICAST_ROUTE_ADD_TRILL_MC_KEY_ESADI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_TRILL_MULTICAST_ROUTE_ADD_MULTICAST_ROUTE_ADD_TRILL_MC_KEY_ESADI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_trill_mc_key.esadi = (uint8)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_TRILL_MULTICAST_ROUTE_ADD_MULTICAST_ROUTE_ADD_TRILL_MC_KEY_TREE_NICK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_TRILL_MULTICAST_ROUTE_ADD_MULTICAST_ROUTE_ADD_TRILL_MC_KEY_TREE_NICK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_trill_mc_key.tree_nick = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter trill_mc_key after multicast_route_add***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_TRILL_MULTICAST_ROUTE_ADD_MULTICAST_ROUTE_ADD_MC_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_TRILL_MULTICAST_ROUTE_ADD_MULTICAST_ROUTE_ADD_MC_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_id = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_frwrd_trill_multicast_route_add(
    unit,
    &prm_trill_mc_key,
    prm_mc_id,
    &prm_success
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_trill_multicast_route_add - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_trill_multicast_route_add");
    goto exit;
  }

  send_string_to_screen("--> success:", TRUE);
  cli_out("success: %s\n",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));

  goto exit;
exit:
  return ui_ret;
}
/******************************************************************** 
 *  Function handler: multicast_route_get (section frwrd_trill)
 */
int 
  ui_ppd_api_frwrd_trill_multicast_route_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_TRILL_MC_ROUTE_KEY   
    prm_trill_mc_key;
  uint32   
    prm_mc_id;
  uint8   
    prm_is_found;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_trill"); 
  soc_sand_proc_name = "soc_ppd_frwrd_trill_multicast_route_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_TRILL_MC_ROUTE_KEY_clear(&prm_trill_mc_key);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_TRILL_MULTICAST_ROUTE_GET_MULTICAST_ROUTE_GET_TRILL_MC_KEY_ADJACENT_EEP_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_TRILL_MULTICAST_ROUTE_GET_MULTICAST_ROUTE_GET_TRILL_MC_KEY_ADJACENT_EEP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_trill_mc_key.adjacent_eep = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_TRILL_MULTICAST_ROUTE_GET_MULTICAST_ROUTE_GET_TRILL_MC_KEY_ING_NICK_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_TRILL_MULTICAST_ROUTE_GET_MULTICAST_ROUTE_GET_TRILL_MC_KEY_ING_NICK_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_trill_mc_key.ing_nick = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_TRILL_MULTICAST_ROUTE_GET_MULTICAST_ROUTE_GET_TRILL_MC_KEY_FID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_TRILL_MULTICAST_ROUTE_GET_MULTICAST_ROUTE_GET_TRILL_MC_KEY_FID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_trill_mc_key.fid = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_TRILL_MULTICAST_ROUTE_GET_MULTICAST_ROUTE_GET_TRILL_MC_KEY_TREE_NICK_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_TRILL_MULTICAST_ROUTE_GET_MULTICAST_ROUTE_GET_TRILL_MC_KEY_TREE_NICK_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_trill_mc_key.tree_nick = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_TRILL_MULTICAST_ROUTE_GET_MULTICAST_ROUTE_GET_TRILL_MC_KEY_ESADI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_TRILL_MULTICAST_ROUTE_GET_MULTICAST_ROUTE_GET_TRILL_MC_KEY_ESADI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_trill_mc_key.esadi = (uint8)param_val->value.ulong_value;
  }
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter trill_mc_key after multicast_route_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_frwrd_trill_multicast_route_get(
          unit,
          &prm_trill_mc_key,
          &prm_mc_id,
          &prm_is_found
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_trill_multicast_route_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_trill_multicast_route_get");   
    goto exit; 
  } 

  cli_out("mc_id: %u\n\r",prm_mc_id);

  cli_out("is_found: %u\n\r",prm_is_found);

  
  goto exit; 
exit: 
  return ui_ret; 
} 

/********************************************************************
*  Function handler: multicast_route_remove (section frwrd_trill)
 */
int
ui_ppd_api_frwrd_trill_multicast_route_remove(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_PPC_TRILL_MC_ROUTE_KEY
    prm_trill_mc_key;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_trill");
  soc_sand_proc_name = "soc_ppd_frwrd_trill_multicast_route_remove";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_TRILL_MC_ROUTE_KEY_clear(&prm_trill_mc_key);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_TRILL_MULTICAST_ROUTE_REMOVE_MULTICAST_ROUTE_REMOVE_TRILL_MC_KEY_ADJACENT_EEP_ID,1))          
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_TRILL_MULTICAST_ROUTE_REMOVE_MULTICAST_ROUTE_REMOVE_TRILL_MC_KEY_ADJACENT_EEP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_trill_mc_key.adjacent_eep = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_TRILL_MULTICAST_ROUTE_REMOVE_MULTICAST_ROUTE_REMOVE_TRILL_MC_KEY_ING_NICK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_TRILL_MULTICAST_ROUTE_REMOVE_MULTICAST_ROUTE_REMOVE_TRILL_MC_KEY_ING_NICK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_trill_mc_key.ing_nick = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_TRILL_MULTICAST_ROUTE_REMOVE_MULTICAST_ROUTE_REMOVE_TRILL_MC_KEY_FID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_TRILL_MULTICAST_ROUTE_REMOVE_MULTICAST_ROUTE_REMOVE_TRILL_MC_KEY_FID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_trill_mc_key.fid = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_TRILL_MULTICAST_ROUTE_REMOVE_MULTICAST_ROUTE_REMOVE_TRILL_MC_KEY_ESADI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_TRILL_MULTICAST_ROUTE_REMOVE_MULTICAST_ROUTE_REMOVE_TRILL_MC_KEY_ESADI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_trill_mc_key.esadi = (uint8)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_TRILL_MULTICAST_ROUTE_REMOVE_MULTICAST_ROUTE_REMOVE_TRILL_MC_KEY_TREE_NICK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_TRILL_MULTICAST_ROUTE_REMOVE_MULTICAST_ROUTE_REMOVE_TRILL_MC_KEY_TREE_NICK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_trill_mc_key.tree_nick = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_TRILL_MULTICAST_ROUTE_REMOVE_MULTICAST_ROUTE_REMOVE_TRILL_MC_KEY_ESADI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_TRILL_MULTICAST_ROUTE_REMOVE_MULTICAST_ROUTE_REMOVE_TRILL_MC_KEY_ESADI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_trill_mc_key.esadi = (uint8)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter trill_mc_key after multicast_route_remove***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_frwrd_trill_multicast_route_remove(
    unit,
    &prm_trill_mc_key
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_trill_multicast_route_remove - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_trill_multicast_route_remove");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/******************************************************************** 
 *  Function handler: adj_info_set (section frwrd_trill)
 */
int 
  ui_ppd_api_frwrd_trill_adj_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_SAND_PP_MAC_ADDRESS   
    prm_mac_address_key;
  SOC_PPC_TRILL_ADJ_INFO   
    prm_mac_auth_info;
  uint8   
    prm_enable;
  SOC_SAND_SUCCESS_FAILURE   
    prm_success;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_trill"); 
  soc_sand_proc_name = "soc_ppd_frwrd_trill_adj_info_set"; 
 
  unit = soc_ppd_get_default_unit();
  sal_memset(&prm_mac_address_key, 0x0, sizeof(SOC_SAND_PP_MAC_ADDRESS));  
  /*SOC_SAND_PP_MAC_ADDRESS_clear(&prm_mac_address_key);*/
  SOC_PPC_TRILL_ADJ_INFO_clear(&prm_mac_auth_info);
 
  /* Get parameters */ 
  if (!get_val_of(
         current_line,(int *)&match_index,SOC_PARAM_PPD_FRWRD_TRILL_ADJ_INFO_SET_ADJ_INFO_SET_MAC_ADDRESS_KEY_ID,1,  
         &param_val,VAL_TEXT,err_msg)) 
  { 
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_mac_address_key));
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter mac_address_key after adj_info_set***", TRUE); 
    goto exit; 
  }
  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_frwrd_trill_adj_info_get(
          unit,
          &prm_mac_address_key,
          &prm_mac_auth_info,
          &prm_enable
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_trill_adj_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_trill_adj_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_TRILL_ADJ_INFO_SET_ADJ_INFO_SET_MAC_AUTH_INFO_EXPECT_SYSTEM_PORT_SYS_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_TRILL_ADJ_INFO_SET_ADJ_INFO_SET_MAC_AUTH_INFO_EXPECT_SYSTEM_PORT_SYS_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_mac_auth_info.expect_system_port.sys_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_TRILL_ADJ_INFO_SET_ADJ_INFO_SET_MAC_AUTH_INFO_EXPECT_SYSTEM_PORT_SYS_PORT_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_TRILL_ADJ_INFO_SET_ADJ_INFO_SET_MAC_AUTH_INFO_EXPECT_SYSTEM_PORT_SYS_PORT_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_mac_auth_info.expect_system_port.sys_port_type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_TRILL_ADJ_INFO_SET_ADJ_INFO_SET_MAC_AUTH_INFO_EXPECT_ADJACENT_EEP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_TRILL_ADJ_INFO_SET_ADJ_INFO_SET_MAC_AUTH_INFO_EXPECT_ADJACENT_EEP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_mac_auth_info.expect_adjacent_eep = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_TRILL_ADJ_INFO_SET_ADJ_INFO_SET_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_TRILL_ADJ_INFO_SET_ADJ_INFO_SET_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_enable = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_frwrd_trill_adj_info_set(
          unit,
          &prm_mac_address_key,
          &prm_mac_auth_info,
          prm_enable,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_trill_adj_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_trill_adj_info_set");   
    goto exit; 
  } 

  cli_out("success: %s\n\r",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: adj_info_get (section frwrd_trill)
 */
int 
  ui_ppd_api_frwrd_trill_adj_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_SAND_PP_MAC_ADDRESS   
    prm_mac_address_key;
  SOC_PPC_TRILL_ADJ_INFO   
    prm_mac_auth_info;
  uint8   
    prm_enable;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_trill"); 
  soc_sand_proc_name = "soc_ppd_frwrd_trill_adj_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  sal_memset(&prm_mac_address_key, 0x0, sizeof(SOC_SAND_PP_MAC_ADDRESS));  
  /*SOC_SAND_PP_MAC_ADDRESS_clear(&prm_mac_address_key);*/  
  SOC_PPC_TRILL_ADJ_INFO_clear(&prm_mac_auth_info);
 
  /* Get parameters */ 
  if (!get_val_of(
         current_line,(int *)&match_index,SOC_PARAM_PPD_FRWRD_TRILL_ADJ_INFO_GET_ADJ_INFO_GET_MAC_ADDRESS_KEY_ID,1,  
         &param_val,VAL_TEXT,err_msg)) 
  { 
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_mac_address_key));
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter mac_address_key after adj_info_get***", TRUE); 
    goto exit; 
  } 
  if (!get_val_of(
         current_line,(int *)&match_index,SOC_PARAM_PPD_FRWRD_TRILL_ADJ_INFO_GET_ADJ_INFO_GET_MAC_AUTH_INFO_EXPECT_SYSTEM_PORT_SYS_ID_ID,1,  
         &param_val,VAL_NUMERIC,err_msg)) 
  { 
    prm_mac_auth_info.expect_system_port.sys_id = (uint32)param_val->value.ulong_value;;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter sys_id after mac_address_key***", TRUE); 
    goto exit; 
  } 

  /* Call function */
  ret = soc_ppd_frwrd_trill_adj_info_get(
          unit,
          &prm_mac_address_key,
          &prm_mac_auth_info,
          &prm_enable
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_trill_adj_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_trill_adj_info_get");   
    goto exit; 
  } 

  SOC_PPC_TRILL_ADJ_INFO_print(&prm_mac_auth_info);

  cli_out("enable: %u\n\r",prm_enable);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: global_info_set (section frwrd_trill)
 */
int 
  ui_ppd_api_frwrd_trill_global_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_FRWRD_TRILL_GLOBAL_INFO   
    prm_glbl_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_trill"); 
  soc_sand_proc_name = "soc_ppd_frwrd_trill_global_info_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_TRILL_GLOBAL_INFO_clear(&prm_glbl_info);
 
  /* Get parameters */ 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_frwrd_trill_global_info_get(
          unit,
          &prm_glbl_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_trill_global_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_trill_global_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_TRILL_GLOBAL_INFO_SET_GLOBAL_INFO_SET_GLBL_INFO_CFG_TTL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_TRILL_GLOBAL_INFO_SET_GLOBAL_INFO_SET_GLBL_INFO_CFG_TTL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_glbl_info.cfg_ttl = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_frwrd_trill_global_info_set(
          unit,
          &prm_glbl_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_trill_global_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_trill_global_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: global_info_get (section frwrd_trill)
 */
int 
  ui_ppd_api_frwrd_trill_global_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_FRWRD_TRILL_GLOBAL_INFO   
    prm_glbl_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_trill"); 
  soc_sand_proc_name = "soc_ppd_frwrd_trill_global_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_TRILL_GLOBAL_INFO_clear(&prm_glbl_info);
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_ppd_frwrd_trill_global_info_get(
          unit,
          &prm_glbl_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_trill_global_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_trill_global_info_get");   
    goto exit; 
  } 

  SOC_PPC_FRWRD_TRILL_GLOBAL_INFO_print(&prm_glbl_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
#endif
#ifdef UI_FRWRD_TRILL/* { frwrd_trill*/
/********************************************************************
*  Section handler: frwrd_trill
 */
int
ui_ppd_api_frwrd_trill(
                       CURRENT_LINE *current_line
                       )
{
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_trill");

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_TRILL_MULTICAST_KEY_MASK_SET_MULTICAST_KEY_MASK_SET_ID,1))
  {
    ui_ret = ui_ppd_api_frwrd_trill_multicast_key_mask_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_TRILL_MULTICAST_KEY_MASK_GET_MULTICAST_KEY_MASK_GET_ID,1))
  {
    ui_ret = ui_ppd_api_frwrd_trill_multicast_key_mask_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_TRILL_UNICAST_ROUTE_ADD_UNICAST_ROUTE_ADD_ID,1))
  {
    ui_ret = ui_ppd_api_frwrd_trill_unicast_route_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_TRILL_UNICAST_ROUTE_GET_UNICAST_ROUTE_GET_ID,1))
  {
    ui_ret = ui_ppd_api_frwrd_trill_unicast_route_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_TRILL_UNICAST_ROUTE_REMOVE_UNICAST_ROUTE_REMOVE_ID,1))
  {
    ui_ret = ui_ppd_api_frwrd_trill_unicast_route_remove(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_TRILL_MULTICAST_ROUTE_ADD_MULTICAST_ROUTE_ADD_ID,1))
  {
    ui_ret = ui_ppd_api_frwrd_trill_multicast_route_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_TRILL_MULTICAST_ROUTE_GET_MULTICAST_ROUTE_GET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_frwrd_trill_multicast_route_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_TRILL_MULTICAST_ROUTE_REMOVE_MULTICAST_ROUTE_REMOVE_ID,1))
  {
    ui_ret = ui_ppd_api_frwrd_trill_multicast_route_remove(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_TRILL_ADJ_INFO_SET_ADJ_INFO_SET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_frwrd_trill_adj_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_TRILL_ADJ_INFO_GET_ADJ_INFO_GET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_frwrd_trill_adj_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_TRILL_GLOBAL_INFO_SET_GLOBAL_INFO_SET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_frwrd_trill_global_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_TRILL_GLOBAL_INFO_GET_GLOBAL_INFO_GET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_frwrd_trill_global_info_get(current_line); 
  } 
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after frwrd_trill***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif
#endif /* LINK_PPD_LIBRARIES */ 

