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
 
/*  */
#include <soc/dpp/PPD/ppd_api_frwrd_mact.h>
#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_frwrd_mact.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_lif.h>

#include <soc/dpp/PPD/ppd_api_frwrd_mact.h>

#include <soc/dpp/PPD/ppd_api_frwrd_mact.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_mact.h>

#if LINK_PPD_LIBRARIES

#ifdef UI_FRWRD_MACT
/********************************************************************
*  Function handler: entry_add (section frwrd_mact)
 */
int
ui_ppd_api_frwrd_mact_entry_add(
                                CURRENT_LINE *current_line
                                )
{
  uint32
    ret;
  SOC_PPC_FRWRD_MACT_ADD_TYPE
    prm_add_type = SOC_PPC_FRWRD_MACT_ADD_TYPE_INSERT;
  SOC_PPC_FRWRD_MACT_ENTRY_KEY
    prm_mac_entry_key;
  SOC_PPC_FRWRD_MACT_ENTRY_VALUE
    prm_mac_entry_value;
  SOC_SAND_SUCCESS_FAILURE
    prm_success;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_mact");
  soc_sand_proc_name = "soc_ppd_frwrd_mact_entry_add";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_FRWRD_MACT_ENTRY_KEY_clear(&prm_mac_entry_key);
  SOC_PPC_FRWRD_MACT_ENTRY_VALUE_clear(&prm_mac_entry_value);

  /* Get parameters */
  if (!get_val_of(
    current_line,(int *)&match_index,SOC_PARAM_PPD_FRWRD_MACT_ENTRY_ADD_ENTRY_ADD_MAC_ENTRY_KEY_MAC_ID,1,
    &param_val,VAL_TEXT,err_msg))
  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_mac_entry_key.key_val.mac.mac));
    prm_mac_entry_key.key_type = SOC_PPC_FRWRD_MACT_KEY_TYPE_MAC_ADDR;
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_ENTRY_ADD_ENTRY_ADD_MAC_ENTRY_KEY_DIP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_ENTRY_ADD_ENTRY_ADD_MAC_ENTRY_KEY_DIP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mac_entry_key.key_val.ipv4_mc.dip = (uint32)param_val->value.ulong_value;
    prm_mac_entry_key.key_type = SOC_PPC_FRWRD_MACT_KEY_TYPE_IPV4_MC;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mac_entry_key after entry_add***", TRUE);
    goto exit;
  }


  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_ENTRY_ADD_ENTRY_ADD_MAC_ENTRY_KEY_FID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_ENTRY_ADD_ENTRY_ADD_MAC_ENTRY_KEY_FID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    if (prm_mac_entry_key.key_type == SOC_PPC_FRWRD_MACT_KEY_TYPE_IPV4_MC)
    {
      prm_mac_entry_key.key_val.ipv4_mc.fid = (uint32)param_val->value.ulong_value;
    }
    else
    {
      prm_mac_entry_key.key_val.mac.fid = (uint32)param_val->value.ulong_value;
    }
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_ENTRY_ADD_ENTRY_ADD_ADD_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_MACT_ENTRY_ADD_ENTRY_ADD_ADD_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_add_type = param_val->numeric_equivalent;
  }


  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_ENTRY_ADD_ENTRY_ADD_MAC_ENTRY_VALUE_AGING_INFO_AGE_STATUS_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_ENTRY_ADD_ENTRY_ADD_MAC_ENTRY_VALUE_AGING_INFO_AGE_STATUS_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_mac_entry_value.aging_info.age_status = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_ENTRY_ADD_ENTRY_ADD_MAC_ENTRY_VALUE_AGING_INFO_IS_DYNAMIC_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_ENTRY_ADD_ENTRY_ADD_MAC_ENTRY_VALUE_AGING_INFO_IS_DYNAMIC_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_mac_entry_value.aging_info.is_dynamic = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_ENTRY_ADD_ENTRY_ADD_MAC_ENTRY_VALUE_FRWRD_INFO_DROP_WHEN_SA_IS_KNOWN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_ENTRY_ADD_ENTRY_ADD_MAC_ENTRY_VALUE_FRWRD_INFO_DROP_WHEN_SA_IS_KNOWN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mac_entry_value.frwrd_info.drop_when_sa_is_known = (uint8)param_val->value.ulong_value;
  }

  ret = ui_ppd_frwrd_decision_set(
          current_line,
          1,
          &(prm_mac_entry_value.frwrd_info.forward_decision)
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** ui_ppd_frwrd_decision_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "ui_ppd_frwrd_decision_set");
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_frwrd_mact_entry_add(
    unit,
    prm_add_type,
    &prm_mac_entry_key,
    &prm_mac_entry_value,
    &prm_success
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_mact_entry_add - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_entry_add");
    goto exit;
  }

  send_string_to_screen("--> success:", TRUE);
  cli_out("success: %s\n",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: entry_remove (section frwrd_mact)
 */
int
ui_ppd_api_frwrd_mact_entry_remove(
                                   CURRENT_LINE *current_line
                                   )
{
  uint32
    ret;
  SOC_PPC_FRWRD_MACT_ENTRY_KEY
    prm_mac_entry_key;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_mact");
  soc_sand_proc_name = "soc_ppd_frwrd_mact_entry_remove";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_FRWRD_MACT_ENTRY_KEY_clear(&prm_mac_entry_key);

  /* Get parameters */

  if (!get_val_of(
    current_line,(int *)&match_index,SOC_PARAM_PPD_FRWRD_MACT_ENTRY_REMOVE_ENTRY_REMOVE_MAC_ENTRY_KEY_MAC_ID,1,
    &param_val,VAL_TEXT,err_msg))
  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_mac_entry_key.key_val.mac.mac));
    prm_mac_entry_key.key_type = SOC_PPC_FRWRD_MACT_KEY_TYPE_MAC_ADDR;
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_ENTRY_REMOVE_ENTRY_REMOVE_MAC_ENTRY_KEY_DIP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_ENTRY_REMOVE_ENTRY_REMOVE_MAC_ENTRY_KEY_DIP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mac_entry_key.key_val.ipv4_mc.dip = (uint32)param_val->value.ulong_value;
    prm_mac_entry_key.key_type = SOC_PPC_FRWRD_MACT_KEY_TYPE_IPV4_MC;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mac_entry_key after entry_remove***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_ENTRY_REMOVE_ENTRY_REMOVE_MAC_ENTRY_KEY_FID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_ENTRY_REMOVE_ENTRY_REMOVE_MAC_ENTRY_KEY_FID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    if (prm_mac_entry_key.key_type == SOC_PPC_FRWRD_MACT_KEY_TYPE_IPV4_MC)
    {
      prm_mac_entry_key.key_val.ipv4_mc.fid = (uint32)param_val->value.ulong_value;
    }
    else
    {
      prm_mac_entry_key.key_val.mac.fid = (uint32)param_val->value.ulong_value;
    }
  }

  /* Call function */
  ret = soc_ppd_frwrd_mact_entry_remove(
    unit,
    &prm_mac_entry_key,
    0
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_mact_entry_remove - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_entry_remove");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: entry_get (section frwrd_mact)
 */
int
ui_ppd_api_frwrd_mact_entry_get(
                                CURRENT_LINE *current_line
                                )
{
  uint32
    ret;
  SOC_PPC_FRWRD_MACT_ENTRY_KEY
    prm_mac_entry_key;
  SOC_PPC_FRWRD_MACT_ENTRY_VALUE
    prm_mac_entry_value;
  uint8
    prm_found;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_mact");
  soc_sand_proc_name = "soc_ppd_frwrd_mact_entry_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_FRWRD_MACT_ENTRY_KEY_clear(&prm_mac_entry_key);
  SOC_PPC_FRWRD_MACT_ENTRY_VALUE_clear(&prm_mac_entry_value);
  prm_mac_entry_key.key_type = SOC_PPC_FRWRD_MACT_KEY_TYPE_MAC_ADDR;

  /* Get parameters */ 
  if (!get_val_of(
    current_line,(int *)&match_index,SOC_PARAM_PPD_FRWRD_MACT_ENTRY_GET_ENTRY_GET_MAC_ENTRY_KEY_MAC_ID,1,
    &param_val,VAL_TEXT,err_msg))
  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_mac_entry_key.key_val.mac.mac));
    prm_mac_entry_key.key_type = SOC_PPC_FRWRD_MACT_KEY_TYPE_MAC_ADDR;
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_ENTRY_GET_ENTRY_GET_MAC_ENTRY_KEY_DIP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_ENTRY_GET_ENTRY_GET_MAC_ENTRY_KEY_DIP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mac_entry_key.key_val.ipv4_mc.dip = (uint32)param_val->value.ulong_value;
    prm_mac_entry_key.key_type = SOC_PPC_FRWRD_MACT_KEY_TYPE_IPV4_MC;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mac_entry_key after entry_add***", TRUE);
    goto exit;
  }

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_ENTRY_GET_ENTRY_GET_MAC_ENTRY_KEY_FID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_ENTRY_GET_ENTRY_GET_MAC_ENTRY_KEY_FID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    if (prm_mac_entry_key.key_type == SOC_PPC_FRWRD_MACT_KEY_TYPE_IPV4_MC)
    {
      prm_mac_entry_key.key_val.ipv4_mc.fid = (uint32)param_val->value.ulong_value;
    }
    else
    {
      prm_mac_entry_key.key_val.mac.fid = (uint32)param_val->value.ulong_value;
    }
  }

  /* Call function */
  ret = soc_ppd_frwrd_mact_entry_get(
    unit,
    &prm_mac_entry_key,
    &prm_mac_entry_value,
    &prm_found
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_mact_entry_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_entry_get");
    goto exit;
  }

  SOC_PPC_FRWRD_MACT_ENTRY_VALUE_print(&prm_mac_entry_value);

  cli_out("found: %u\n\r",prm_found);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: traverse (section frwrd_mact)
 */
int
ui_ppd_api_frwrd_mact_traverse(
                               CURRENT_LINE *current_line
                               )
{
  uint32
    ret;
  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE
    prm_rule;
  SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION
    prm_action;
  uint8
    prm_wait_till_finish=TRUE;
  uint32
    prm_nof_matched_entries;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_mact");
  soc_sand_proc_name = "soc_ppd_frwrd_mact_traverse";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE_clear(&prm_rule);
  SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_clear(&prm_action);

  /* Get parameters */
  prm_rule.key_type = SOC_PPC_FRWRD_MACT_KEY_TYPE_MAC_ADDR;
  prm_action.updated_val.frwrd_info.forward_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT;
  prm_rule.value_rule.val.frwrd_info.forward_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT;

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_TRAVERSE_TRAVERSE_RULE_KEY_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_MACT_TRAVERSE_TRAVERSE_RULE_KEY_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rule.key_type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_TRAVERSE_TRAVERSE_RULE_VALUE_RULE_VAL_AGING_INFO_AGE_STATUS_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_TRAVERSE_TRAVERSE_RULE_VALUE_RULE_VAL_AGING_INFO_AGE_STATUS_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rule.value_rule.val.aging_info.age_status = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_TRAVERSE_TRAVERSE_RULE_VALUE_RULE_VAL_AGING_INFO_IS_DYNAMIC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_TRAVERSE_TRAVERSE_RULE_VALUE_RULE_VAL_AGING_INFO_IS_DYNAMIC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_rule.value_rule.val.aging_info.is_dynamic = (uint8)param_val->value.ulong_value;
    prm_rule.value_rule.compare_mask |= SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_IS_DYNAMIC;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_TRAVERSE_TRAVERSE_RULE_VALUE_RULE_VAL_FRWRD_INFO_DROP_WHEN_SA_IS_KNOWN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_TRAVERSE_TRAVERSE_RULE_VALUE_RULE_VAL_FRWRD_INFO_DROP_WHEN_SA_IS_KNOWN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_rule.value_rule.val.frwrd_info.drop_when_sa_is_known = (uint8)param_val->value.ulong_value;
    prm_rule.value_rule.compare_mask |= SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DROP_SA;
  }


  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_TRAVERSE_TRAVERSE_RULE_VALUE_RULE_VAL_ACCESSED_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_TRAVERSE_TRAVERSE_RULE_VALUE_RULE_VAL_ACCESSED_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_rule.value_rule.val.accessed = (uint8)param_val->value.ulong_value;
    prm_rule.value_rule.compare_mask |= SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_ACCESSED;
  }
  ret = ui_ppd_frwrd_decision_set(
          current_line,
          1,
          &(prm_rule.value_rule.val.frwrd_info.forward_decision)
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** ui_ppd_frwrd_decision_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "ui_ppd_frwrd_decision_set");
    goto exit;
  }

  if (  (!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_DECISION_FD_DEST_ID,1)) ||
        (!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_DECISION_FD_IS_LAG_ID,1))
     )
  {
    prm_rule.value_rule.compare_mask |= SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DEST_VAL;
  }

  if((!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_DECISION_FD_TRAP_CODE_ID,1))  ||
     (!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_DECISION_FD_FWD_STRENGTH_ID,1))  ||
     (!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_DECISION_FD_SNOOP_STRENGTH_ID,1)) ||
     (!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_DECISION_FD_AC_ID,1)) ||
     (!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_DECISION_FD_LABEL_ID,1)) ||
     (!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_DECISION_FD_PUSH_PROFILE_ID,1)) ||
     (!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_DECISION_FD_EEP_NDX_ID,1))
    )
  {
    prm_rule.value_rule.compare_mask |= SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_ADDITIONAL_INFO;
  }


  if ((prm_rule.value_rule.compare_mask & SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DEST_VAL) == 0
    && prm_rule.value_rule.val.frwrd_info.forward_decision.type != SOC_PPC_FRWRD_DECISION_TYPE_TRAP)
  {
    prm_rule.value_rule.val.frwrd_info.forward_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT;
  }

  if (prm_rule.value_rule.compare_mask == SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_ADDITIONAL_INFO && 
      prm_rule.value_rule.val.frwrd_info.forward_decision.additional_info.eei.type != SOC_PPC_EEI_TYPE_EMPTY) {
      prm_rule.value_rule.compare_mask |= SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DEST_TYPE;

      if (!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_DECISION_FD_LABEL_ID,1)) {
          prm_rule.value_rule.val.frwrd_info.forward_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_FEC;
      }
      else{
          prm_rule.value_rule.val.frwrd_info.forward_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_MC;
      }
  }
  prm_rule.value_rule.val_mask.frwrd_info.forward_decision.dest_id = 0xFFFFFFFF;

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_TRAVERSE_TRAVERSE_ACTION_UPDATED_VAL_AGING_INFO_IS_DYNAMIC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_TRAVERSE_TRAVERSE_ACTION_UPDATED_VAL_AGING_INFO_IS_DYNAMIC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_action.updated_val.aging_info.is_dynamic = (uint8)param_val->value.ulong_value;
    prm_action.update_mask |= SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_IS_DYNAMIC;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_TRAVERSE_TRAVERSE_ACTION_UPDATED_VAL_FRWRD_INFO_DROP_WHEN_SA_IS_KNOWN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_TRAVERSE_TRAVERSE_ACTION_UPDATED_VAL_FRWRD_INFO_DROP_WHEN_SA_IS_KNOWN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_action.updated_val.frwrd_info.drop_when_sa_is_known = (uint8)param_val->value.ulong_value;
    prm_action.update_mask |= SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DROP_SA;
  }

  ret = ui_ppd_frwrd_decision_set(
          current_line,
          2,
          &(prm_action.updated_val.frwrd_info.forward_decision)
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** ui_ppd_frwrd_decision_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "ui_ppd_frwrd_decision_set");
    goto exit;
  }

  if (  (!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_DECISION_FD2_DEST_ID,1)) ||
        (!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_DECISION_FD2_IS_LAG_ID,1))
     )
  {
    prm_action.update_mask |= SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DEST_VAL;
  }

  if((!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_DECISION_FD2_TRAP_CODE_ID,1))  ||
     (!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_DECISION_FD2_FWD_STRENGTH_ID,1))  ||
     (!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_DECISION_FD2_SNOOP_STRENGTH_ID,1)) ||
     (!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_DECISION_FD2_AC_ID,1)) ||
     (!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_DECISION_FD2_LABEL_ID,1)) ||
     (!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_DECISION_FD2_PUSH_PROFILE_ID,1)) ||
     (!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_DECISION_FD2_EEP_NDX_ID,1))
    )
  {
    prm_action.update_mask |= SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_ADDITIONAL_INFO;
  }

  /* just as MACT has no drop */
  if ((prm_action.update_mask & SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DEST_VAL) == 0
     && prm_action.updated_val.frwrd_info.forward_decision.type != SOC_PPC_FRWRD_DECISION_TYPE_TRAP)
  {
    prm_action.updated_val.frwrd_info.forward_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT;
  }
  
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_TRAVERSE_TRAVERSE_ACTION_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_MACT_TRAVERSE_TRAVERSE_ACTION_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_action.type = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_TRAVERSE_TRAVERSE_WAIT_TILL_FINISH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_TRAVERSE_TRAVERSE_WAIT_TILL_FINISH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_wait_till_finish = (uint8)param_val->value.ulong_value;
  }

  /* prevent CPU stuck
  if (prm_action.type == SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_RETRIEVE)
  {
    prm_wait_till_finish = 0;
  }*/

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_TRAVERSE_TRAVERSE_RULE_VALUE_RULE_COMPARE_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_TRAVERSE_TRAVERSE_RULE_VALUE_RULE_COMPARE_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_rule.value_rule.compare_mask = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_TRAVERSE_TRAVERSE_RULE_KEY_RULE_FID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_TRAVERSE_TRAVERSE_RULE_KEY_RULE_FID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    if (prm_rule.key_type == SOC_PPC_FRWRD_MACT_KEY_TYPE_IPV4_MC)
    {
      prm_rule.key_rule.ipv4_mc.fid = (uint32)param_val->value.ulong_value;
      prm_rule.key_rule.ipv4_mc.fid_mask |= 0x7FFF;
    }
    else
    {
      prm_rule.key_rule.mac.fid = (uint32)param_val->value.ulong_value;
      prm_rule.key_rule.mac.fid_mask |= 0x7FFF;
    }
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_TRAVERSE_TRAVERSE_RULE_KEY_RULE_FID_MASK_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_TRAVERSE_TRAVERSE_RULE_KEY_RULE_FID_MASK_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    if (prm_rule.key_type == SOC_PPC_FRWRD_MACT_KEY_TYPE_IPV4_MC)
    {
      prm_rule.key_rule.ipv4_mc.fid_mask = (uint32)param_val->value.ulong_value;
    }
    else
    {
      prm_rule.key_rule.mac.fid_mask = (uint32)param_val->value.ulong_value;
    }
  } 

  if (!get_val_of(
    current_line,(int *)&match_index,SOC_PARAM_PPD_FRWRD_MACT_TRAVERSE_TRAVERSE_RULE_KEY_RULE_MAC_MAC_ID,1,  
    &param_val,VAL_TEXT,err_msg)) 
  { 
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_rule.key_rule.mac.mac));
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_TRAVERSE_TRAVERSE_ACTION_UPDATE_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_TRAVERSE_TRAVERSE_ACTION_UPDATE_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_action.update_mask = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_frwrd_mact_traverse(
    unit,
    &prm_rule,
    &prm_action,
    prm_wait_till_finish,
    &prm_nof_matched_entries
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_mact_traverse - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_traverse");
    goto exit;
  }

  send_string_to_screen("--> nof_matched_entries:", TRUE);
  cli_out("nof_matched_entries: %u\n",prm_nof_matched_entries);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: traverse_status_get (section frwrd_mact)
 */
int
ui_ppd_api_frwrd_mact_traverse_status_get(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_PPC_FRWRD_MACT_TRAVERSE_STATUS_INFO
    prm_status;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_mact");
  soc_sand_proc_name = "soc_ppd_frwrd_mact_traverse_status_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_FRWRD_MACT_TRAVERSE_STATUS_INFO_clear(&prm_status);

  /* Get parameters */

  /* Call function */
  ret = soc_ppd_frwrd_mact_traverse_status_get(
    unit,
    &prm_status
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_mact_traverse_status_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_traverse_status_get");
    goto exit;
  }

  send_string_to_screen("--> status:", TRUE);
  SOC_PPC_FRWRD_MACT_TRAVERSE_STATUS_INFO_print(&prm_status);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: get_block (section frwrd_mact)
 */
int
ui_ppd_api_frwrd_mact_get_block(
                                CURRENT_LINE *current_line
                                )
{
  uint32
    ret;
  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE
    prm_rule;
  SOC_SAND_TABLE_BLOCK_RANGE
    prm_block_range,
    prm_block_range_count;
  SOC_PPC_FRWRD_MACT_ENTRY_KEY
    *prm_mact_keys = NULL;/*[UI_PPD_BLOCK_SIZE];*/
  SOC_PPC_FRWRD_MACT_ENTRY_VALUE
    *prm_mact_vals = NULL;/*[UI_PPD_BLOCK_SIZE];*/
  ARAD_PP_LEM_ACCESS_KEY
    rule_key,
    rule_key_mask;
  uint32
    prm_nof_entries;
  uint32
    cur_indx,
    nof_printed= 0,
    nof_readed=0,
    do_print = 1,
    nof_entries_to_print,
    total_to_print=0;
  char
    user_msg[5] = "";
  uint32
    con;
  SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION
    prm_action;
  uint8
    prm_wait_till_finish=TRUE,
    no_rule = 1;
  uint32
    prm_nof_matched_entries;


  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_mact");
  soc_sand_proc_name = "soc_ppd_frwrd_mact_get_block";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE_clear(&prm_rule);
  soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&prm_block_range);
  SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_clear(&prm_action);

  prm_rule.key_type = SOC_PPC_FRWRD_MACT_KEY_TYPE_MAC_ADDR;

  prm_block_range.entries_to_act = 130;
  if (SOC_SAND_DEVICE_TYPE_GET(unit) == SOC_SAND_DEV_T20E) 
  {
    prm_block_range.entries_to_scan = UI_PPD_BLOCK_SIZE;
  } 
  else if (SOC_SAND_DEVICE_TYPE_GET(unit) == SOC_SAND_DEV_PCP)
  {
	  prm_block_range.entries_to_scan =( (2*1024*1024) + 32 -1); /* same as  PCP_FRWRD_MACT_TABLE_ENTRY_MAX; */
  }
  else if (SOC_SAND_DEVICE_TYPE_GET(unit) == SOC_SAND_DEV_ARAD)
  {
      prm_block_range.entries_to_scan = SOC_SAND_TBL_ITER_SCAN_ALL; 
  }
  else /*Soc_petra-B*/
  {
    prm_block_range.entries_to_scan = 64*1024+32;
  }
  prm_block_range.iter = 0;

  /* Get parameters */


  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_GET_BLOCK_GET_BLOCK_RULE_KEY_RULE_ENTRY_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_MACT_GET_BLOCK_GET_BLOCK_RULE_KEY_RULE_ENTRY_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_rule.key_type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_GET_BLOCK_GET_BLOCK_RULE_VALUE_RULE_VAL_AGING_INFO_AGE_STATUS_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_GET_BLOCK_GET_BLOCK_RULE_VALUE_RULE_VAL_AGING_INFO_AGE_STATUS_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rule.value_rule.val.aging_info.age_status = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_GET_BLOCK_GET_BLOCK_RULE_VALUE_RULE_VAL_AGING_INFO_IS_DYNAMIC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_GET_BLOCK_GET_BLOCK_RULE_VALUE_RULE_VAL_AGING_INFO_IS_DYNAMIC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_rule.value_rule.val.aging_info.is_dynamic = (uint8)param_val->value.ulong_value;
    prm_rule.value_rule.compare_mask |= SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_IS_DYNAMIC;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_GET_BLOCK_GET_BLOCK_RULE_VALUE_RULE_VAL_FRWRD_INFO_DROP_WHEN_SA_IS_KNOWN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_GET_BLOCK_GET_BLOCK_RULE_VALUE_RULE_VAL_FRWRD_INFO_DROP_WHEN_SA_IS_KNOWN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_rule.value_rule.val.frwrd_info.drop_when_sa_is_known = (uint8)param_val->value.ulong_value;
    prm_rule.value_rule.compare_mask |= SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DROP_SA;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_GET_BLOCK_GET_BLOCK_RULE_VALUE_RULE_VAL_ACCESSED_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_GET_BLOCK_GET_BLOCK_RULE_VALUE_RULE_VAL_ACCESSED_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_rule.value_rule.val.accessed = (uint8)param_val->value.ulong_value;
    prm_rule.value_rule.compare_mask |= SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_ACCESSED;
  }

  ret = ui_ppd_frwrd_decision_set(
          current_line,
          1,
          &(prm_rule.value_rule.val.frwrd_info.forward_decision)
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** ui_ppd_frwrd_decision_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "ui_ppd_frwrd_decision_set");
    goto exit;
  }

  if (  (!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_DECISION_FD_DEST_ID,1)) ||
        (!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_DECISION_FD_IS_LAG_ID,1))
     )
  {
    prm_rule.value_rule.compare_mask |= SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DEST_VAL;
  }

  if((!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_DECISION_FD_TRAP_CODE_ID,1))  ||
     (!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_DECISION_FD_FWD_STRENGTH_ID,1))  ||
     (!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_DECISION_FD_SNOOP_STRENGTH_ID,1)) ||
     (!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_DECISION_FD_AC_ID,1)) ||
     (!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_DECISION_FD_LABEL_ID,1)) ||
     (!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_DECISION_FD_PUSH_PROFILE_ID,1)) ||
     (!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_DECISION_FD_EEP_NDX_ID,1))
    )
  {
    prm_rule.value_rule.compare_mask |= SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_ADDITIONAL_INFO;
  }

  if (prm_rule.value_rule.compare_mask == SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_ADDITIONAL_INFO && 
      prm_rule.value_rule.val.frwrd_info.forward_decision.additional_info.eei.type != SOC_PPC_EEI_TYPE_EMPTY) {
      prm_rule.value_rule.compare_mask |= SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DEST_TYPE;
      if (!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_DECISION_FD_LABEL_ID,1)) {
          prm_rule.value_rule.val.frwrd_info.forward_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_FEC;
      }
      else{
          prm_rule.value_rule.val.frwrd_info.forward_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_MC;
      }
  }
  prm_rule.value_rule.val_mask.frwrd_info.forward_decision.dest_id = 0xFFFFFFFF;

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_GET_BLOCK_GET_BLOCK_RULE_KEY_RULE_FID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_GET_BLOCK_GET_BLOCK_RULE_KEY_RULE_FID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    if (prm_rule.key_type == SOC_PPC_FRWRD_MACT_KEY_TYPE_IPV4_MC)
    {
      prm_rule.key_rule.ipv4_mc.fid = (uint32)param_val->value.ulong_value;
      if(SOC_IS_ARAD(unit)){
          prm_rule.key_rule.ipv4_mc.fid_mask = 0x7FFF;
      }
    }
    else
    {
      prm_rule.key_rule.mac.fid = (uint32)param_val->value.ulong_value;
      if(SOC_IS_ARAD(unit)){
          prm_rule.key_rule.mac.fid_mask = 0x7FFF;
      }

    }

    no_rule = 0;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_GET_BLOCK_GET_BLOCK_RULE_KEY_RULE_FID_MASK_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_GET_BLOCK_GET_BLOCK_RULE_KEY_RULE_FID_MASK_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    if (prm_rule.key_type == SOC_PPC_FRWRD_MACT_KEY_TYPE_IPV4_MC)
    {
      prm_rule.key_rule.ipv4_mc.fid_mask = (uint32)param_val->value.ulong_value;
    }
    else
    {
      prm_rule.key_rule.mac.fid_mask = (uint32)param_val->value.ulong_value;
    }
    no_rule = 0;
  } 

  if (!get_val_of(
    current_line,(int *)&match_index,SOC_PARAM_PPD_FRWRD_MACT_GET_BLOCK_GET_BLOCK_RULE_KEY_RULE_MAC_MAC_ADDR_ID,1,
    &param_val,VAL_TEXT,err_msg))
  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_rule.key_rule.mac.mac));
    soc_sand_pp_mac_address_string_parse("FFFFFFFFFFFF", &(prm_rule.key_rule.mac.mac_mask));
  }

  if (!get_val_of(
    current_line,(int *)&match_index,SOC_PARAM_PPD_FRWRD_MACT_GET_BLOCK_GET_BLOCK_RULE_KEY_RULE_MAC_MAC_ADDR_MASK_ID,1,
    &param_val,VAL_TEXT,err_msg))
  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_rule.key_rule.mac.mac_mask));
    no_rule = 0;
  }


  if(!search_param_val_pairs(current_line,&match_index,SOC_ARAM_PPD_FRWRD_MACT_GET_BLOCK_GET_BLOCK_RANGE_KEY_ENTRIES_TO_ACT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_ARAM_PPD_FRWRD_MACT_GET_BLOCK_GET_BLOCK_RANGE_KEY_ENTRIES_TO_ACT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_block_range.entries_to_act = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_ARAM_PPD_FRWRD_MACT_GET_BLOCK_GET_BLOCK_RANGE_KEY_ENTRIES_TO_SCAN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_ARAM_PPD_FRWRD_MACT_GET_BLOCK_GET_BLOCK_RANGE_KEY_ENTRIES_TO_SCAN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_block_range.entries_to_scan = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_ARAM_PPD_FRWRD_MACT_GET_BLOCK_GET_BLOCK_RANGE_KEY_START_PAYLOAD_ARR_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_ARAM_PPD_FRWRD_MACT_GET_BLOCK_GET_BLOCK_RANGE_KEY_START_PAYLOAD_ARR_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_block_range.iter = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_GET_BLOCK_GET_BLOCK_RULE_VALUE_RULE_COMPARE_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_MACT_GET_BLOCK_GET_BLOCK_RULE_VALUE_RULE_COMPARE_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_rule.value_rule.compare_mask = (uint32)param_val->value.ulong_value;
  }

  if (prm_rule.value_rule.compare_mask != 0){
      no_rule = 0;
  }

  prm_action.type = SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_COUNT;
  /* print total */

  if (no_rule){
      
      soc_sand_os_memcpy(&prm_block_range_count,&prm_block_range,sizeof(SOC_SAND_TABLE_BLOCK_RANGE));

        ret = arad_pp_frwrd_mact_rule_to_key_convert(
                unit,
                &prm_rule,
                &rule_key,
                &rule_key_mask,
                0
              );
        if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
        {
          send_string_to_screen(" *** arad_pp_frwrd_mact_rule_to_key_convert - FAIL", TRUE);
          soc_sand_disp_result_proc(ret, "arad_pp_frwrd_mact_rule_to_key_convert");
          goto exit;
        }

      ret = arad_pp_frwrd_mact_traverse_internal_unsafe(
        unit,
        &rule_key,
        &rule_key_mask,
        &prm_rule.value_rule,
        &prm_block_range_count,
        &prm_action,
        prm_wait_till_finish,
        &prm_nof_matched_entries
        ); 
      if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
      {
        send_string_to_screen(" *** soc_ppd_frwrd_mact_traverse - FAIL", TRUE);
        soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_traverse");
        goto exit;
      }
      cli_out("Total Match entries %u \n\r", prm_nof_matched_entries);
  }


  prm_mact_keys = (SOC_PPC_FRWRD_MACT_ENTRY_KEY*)sal_alloc(sizeof(SOC_PPC_FRWRD_MACT_ENTRY_KEY) * prm_block_range.entries_to_act, "prm_mact_keys");
  if (prm_mact_keys == NULL)
  {
    ui_ret = 1;
    goto exit;
  }

  prm_mact_vals = (SOC_PPC_FRWRD_MACT_ENTRY_VALUE*)sal_alloc(sizeof(SOC_PPC_FRWRD_MACT_ENTRY_VALUE) * prm_block_range.entries_to_act, "prm_mact_vals");
  if (prm_mact_vals == NULL)
  {
    ui_ret = 1;
    goto exit;
  }

  while(!SOC_SAND_TBL_ITER_IS_END(&prm_block_range.iter)) {

      /* Call function */
      ret = soc_ppd_frwrd_mact_get_block(
        unit,
        &prm_rule,
        SOC_PPC_FRWRD_MACT_TABLE_SW_HW,
        &prm_block_range,
        prm_mact_keys,
        prm_mact_vals,
        &prm_nof_entries
        );
      if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
      {
        send_string_to_screen(" *** soc_ppd_frwrd_mact_get_block - FAIL", TRUE);
        soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_get_block");
        goto exit;
      }

      nof_readed += prm_nof_entries;
      if(!do_print) {
          continue;
      }

      cur_indx = 0;
      total_to_print = prm_nof_entries;
  while (cur_indx <= total_to_print)
  {

    if (prm_nof_entries == 0)
    {
      break;
    }

    nof_entries_to_print = SOC_SAND_MIN(UI_PPD_BLOCK_SIZE, total_to_print - cur_indx );
    ui_ret = soc_ppd_frwrd_mact_print_block(
      unit,
      &(prm_mact_keys[cur_indx]),
      &(prm_mact_vals[cur_indx]),
      nof_entries_to_print,
      0
      );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
          send_string_to_screen(" *** soc_ppd_frwrd_mact_get_block - FAIL", TRUE);
          soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_mact_get_block");
          goto exit;
        }

        nof_printed += nof_entries_to_print;

        cur_indx+= nof_entries_to_print;
        if (no_rule){
            cli_out("total entries to print %u \n\r", prm_nof_matched_entries);
            cli_out("%u more entries to print\n\r", prm_nof_matched_entries - nof_printed);
        }

        if (cur_indx < total_to_print)
        {
          cli_out("Hit Space to continue/Any Key to Abort.\n\r");
          con = ask_get(user_msg,EXPECT_CONT_ABORT,FALSE,TRUE) ;
          if (!con)
          {
            goto done_print;
          }
        }
        else
        {
          break;
        }
      }
  }

done_print:
  send_string_to_screen("--> nof_entries:", TRUE);
  if (no_rule && !SOC_SAND_TBL_ITER_IS_END(&prm_block_range.iter)){
      cli_out("%u more entries to print\n\r", prm_nof_matched_entries - nof_printed);
  }
  cli_out("nof_printed_entries: %u\n",nof_printed);
  cli_out("nof_read_entries: %u\n",nof_readed);
  if(SOC_SAND_TBL_ITER_IS_END(&prm_block_range.iter)) {
      cli_out("end of table \n");
  }
  else{
      cli_out("iter: %u\n",prm_block_range.iter);
  }
  

  goto exit;
exit:
  if (prm_mact_keys != NULL)
  {
    sal_free(prm_mact_keys);
  }

  if (prm_mact_vals != NULL)
  {
    sal_free(prm_mact_vals);
  }

  return ui_ret;
}

/********************************************************************
*  Section handler: frwrd_mact
 */
int
ui_ppd_api_frwrd_mact(
                      CURRENT_LINE *current_line
                      )
{
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_mact");

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_ENTRY_ADD_ENTRY_ADD_ID,1))
  {
    ui_ret = ui_ppd_api_frwrd_mact_entry_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_ENTRY_REMOVE_ENTRY_REMOVE_ID,1))
  {
    ui_ret = ui_ppd_api_frwrd_mact_entry_remove(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_ENTRY_GET_ENTRY_GET_ID,1))
  {
    ui_ret = ui_ppd_api_frwrd_mact_entry_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_TRAVERSE_TRAVERSE_ID,1))
  {
    ui_ret = ui_ppd_api_frwrd_mact_traverse(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_TRAVERSE_STATUS_GET_TRAVERSE_STATUS_GET_ID,1))
  {
    ui_ret = ui_ppd_api_frwrd_mact_traverse_status_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_MACT_GET_BLOCK_GET_BLOCK_ID,1))
  {
    ui_ret = ui_ppd_api_frwrd_mact_get_block(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after frwrd_mact***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif
#endif /* LINK_PPD_LIBRARIES */ 

