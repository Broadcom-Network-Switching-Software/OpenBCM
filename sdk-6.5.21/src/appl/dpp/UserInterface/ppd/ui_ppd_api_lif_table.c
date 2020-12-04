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
 


#include <soc/dpp/PPD/ppd_api_lif_table.h>                                                               
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_lif_table.h>

#if LINK_PPD_LIBRARIES

#ifdef UI_LIF_TABLE
/******************************************************************** 
 *  Function handler: get_block (section lif_table)
 */
int 
  ui_ppd_api_lif_table_get_block(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_LIF_TBL_TRAVERSE_MATCH_RULE   
    prm_rule;
  SOC_SAND_TABLE_BLOCK_RANGE   
    prm_block_range;
  SOC_PPC_LIF_ENTRY_INFO   
    prm_entries_array;
  uint32   
    prm_nof_entries;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif_table"); 
  soc_sand_proc_name = "soc_ppd_lif_table_get_block"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_LIF_TBL_TRAVERSE_MATCH_RULE_clear(&prm_rule);
  SOC_SAND_TABLE_BLOCK_RANGE_clear(&prm_block_range);
  SOC_PPC_LIF_ENTRY_INFO_clear(&prm_entries_array);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_GET_BLOCK_GET_BLOCK_RULE_ENTRIES_TYPE_BM_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_GET_BLOCK_GET_BLOCK_RULE_ENTRIES_TYPE_BM_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rule.entries_type_bm = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_lif_table_get_block(
          unit,
          &prm_rule,
          &prm_block_range,
          &prm_entries_array,
          &prm_nof_entries
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_lif_table_get_block - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_lif_table_get_block");   
    goto exit; 
  } 

  SOC_PPC_LIF_ENTRY_INFO_print(&prm_entries_array);

  cli_out("nof_entries: %u\n\r",prm_nof_entries);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: entry_get (section lif_table)
 */
int 
  ui_ppd_api_lif_table_entry_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_LIF_ID   
    prm_lif_ndx;
  SOC_PPC_LIF_ENTRY_INFO   
    prm_lif_entry_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif_table"); 
  soc_sand_proc_name = "soc_ppd_lif_table_entry_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_LIF_ENTRY_INFO_clear(&prm_lif_entry_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_GET_ENTRY_GET_LIF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_GET_ENTRY_GET_LIF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_lif_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter lif_ndx after entry_get***", TRUE); 
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

  SOC_PPC_LIF_ENTRY_INFO_print(&prm_lif_entry_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: entry_update (section lif_table)
 */
int 
  ui_ppd_api_lif_table_entry_update(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_LIF_ID   
    prm_lif_ndx;
  SOC_PPC_LIF_ENTRY_INFO   
    prm_lif_entry_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif_table"); 
  soc_sand_proc_name = "soc_ppd_lif_table_entry_update"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_LIF_ENTRY_INFO_clear(&prm_lif_entry_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_lif_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter lif_ndx after entry_update***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_INDEX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.index = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_FEC_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_FEC_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.fec_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_RIF_ROUTING_ENABLERS_BM_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_RIF_ROUTING_ENABLERS_BM_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.rif.routing_enablers_bm = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_RIF_COS_PROFILE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_RIF_COS_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.rif.cos_profile = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_RIF_PROCESSING_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_RIF_PROCESSING_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.rif.processing_type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_RIF_VRF_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_RIF_VRF_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.rif.vrf_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_RIF_UC_RPF_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_RIF_UC_RPF_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.rif.uc_rpf_enable = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_RIF_RIF_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_RIF_RIF_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.rif.rif_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_ISID_SERVICE_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_ISID_SERVICE_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.isid.service_type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_ISID_COS_PROFILE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_ISID_COS_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.isid.cos_profile = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_ISID_TPID_PROFILES_TPID2_INDEX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_ISID_TPID_PROFILES_TPID2_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.isid.tpid_profiles.tpid2_index = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_ISID_TPID_PROFILES_TPID1_INDEX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_ISID_TPID_PROFILES_TPID1_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.isid.tpid_profiles.tpid1_index = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_ISID_LEARN_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_ISID_LEARN_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.isid.learn_enable = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_ISID_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_TRAP_INFO_ACTION_PROFILE_SNOOP_ACTION_STRENGTH_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_ISID_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_TRAP_INFO_ACTION_PROFILE_SNOOP_ACTION_STRENGTH_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.isid.default_frwrd.default_forwarding.additional_info.trap_info.action_profile.snoop_action_strength = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_ISID_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_TRAP_INFO_ACTION_PROFILE_FRWRD_ACTION_STRENGTH_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_ISID_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_TRAP_INFO_ACTION_PROFILE_FRWRD_ACTION_STRENGTH_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.isid.default_frwrd.default_forwarding.additional_info.trap_info.action_profile.frwrd_action_strength = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_ISID_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_TRAP_INFO_ACTION_PROFILE_TRAP_CODE_LSB_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_ISID_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_TRAP_INFO_ACTION_PROFILE_TRAP_CODE_LSB_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.isid.default_frwrd.default_forwarding.additional_info.trap_info.action_profile.trap_code = (SOC_PPC_TRAP_CODE)param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_ISID_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_OUTLIF_VAL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_ISID_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_OUTLIF_VAL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.isid.default_frwrd.default_forwarding.additional_info.outlif.val = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_ISID_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_OUTLIF_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_ISID_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_OUTLIF_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.isid.default_frwrd.default_forwarding.additional_info.outlif.type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_ISID_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_EEI_VAL_ISID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_ISID_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_EEI_VAL_ISID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.isid.default_frwrd.default_forwarding.additional_info.eei.val.isid = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_ISID_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_EEI_VAL_MPLS_COMMAND_PUSH_PROFILE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_ISID_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_EEI_VAL_MPLS_COMMAND_PUSH_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.isid.default_frwrd.default_forwarding.additional_info.eei.val.mpls_command.push_profile = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_ISID_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_EEI_VAL_MPLS_COMMAND_LABEL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_ISID_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_EEI_VAL_MPLS_COMMAND_LABEL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.isid.default_frwrd.default_forwarding.additional_info.eei.val.mpls_command.label = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_ISID_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_EEI_VAL_MPLS_COMMAND_COMMAND_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_ISID_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_EEI_VAL_MPLS_COMMAND_COMMAND_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.isid.default_frwrd.default_forwarding.additional_info.eei.val.mpls_command.command = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_ISID_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_EEI_VAL_TRILL_DEST_DEST_NICK_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_ISID_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_EEI_VAL_TRILL_DEST_DEST_NICK_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.isid.default_frwrd.default_forwarding.additional_info.eei.val.trill_dest.dest_nick = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_ISID_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_EEI_VAL_TRILL_DEST_IS_MULTICAST_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_ISID_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_EEI_VAL_TRILL_DEST_IS_MULTICAST_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.isid.default_frwrd.default_forwarding.additional_info.eei.val.trill_dest.is_multicast = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_ISID_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_EEI_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_ISID_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_EEI_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.isid.default_frwrd.default_forwarding.additional_info.eei.type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_ISID_DEFAULT_FRWRD_DEFAULT_FORWARDING_DEST_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_ISID_DEFAULT_FRWRD_DEFAULT_FORWARDING_DEST_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.isid.default_frwrd.default_forwarding.dest_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_ISID_DEFAULT_FRWRD_DEFAULT_FORWARDING_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_ISID_DEFAULT_FRWRD_DEFAULT_FORWARDING_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.isid.default_frwrd.default_forwarding.type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_ISID_DEFAULT_FRWRD_DEFAULT_FRWD_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_ISID_DEFAULT_FRWRD_DEFAULT_FRWD_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.isid.default_frwrd.default_frwd_type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_SERVICE_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_SERVICE_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.pwe.service_type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_HAS_CW_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_HAS_CW_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.pwe.has_cw = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_COS_PROFILE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_COS_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.pwe.cos_profile = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_TPID_PROFILE_INDEX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_TPID_PROFILE_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.pwe.tpid_profile_index = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_ORIENTATION_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_ORIENTATION_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.pwe.orientation = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_TRAP_INFO_ACTION_PROFILE_SNOOP_ACTION_STRENGTH_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_TRAP_INFO_ACTION_PROFILE_SNOOP_ACTION_STRENGTH_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.pwe.default_frwrd.default_forwarding.additional_info.trap_info.action_profile.snoop_action_strength = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_TRAP_INFO_ACTION_PROFILE_FRWRD_ACTION_STRENGTH_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_TRAP_INFO_ACTION_PROFILE_FRWRD_ACTION_STRENGTH_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.pwe.default_frwrd.default_forwarding.additional_info.trap_info.action_profile.frwrd_action_strength = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_TRAP_INFO_ACTION_PROFILE_TRAP_CODE_LSB_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_TRAP_INFO_ACTION_PROFILE_TRAP_CODE_LSB_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.pwe.default_frwrd.default_forwarding.additional_info.trap_info.action_profile.trap_code = (SOC_PPC_TRAP_CODE)param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_OUTLIF_VAL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_OUTLIF_VAL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.pwe.default_frwrd.default_forwarding.additional_info.outlif.val = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_OUTLIF_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_OUTLIF_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.pwe.default_frwrd.default_forwarding.additional_info.outlif.type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_EEI_VAL_ISID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_EEI_VAL_ISID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.pwe.default_frwrd.default_forwarding.additional_info.eei.val.isid = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_EEI_VAL_MPLS_COMMAND_PUSH_PROFILE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_EEI_VAL_MPLS_COMMAND_PUSH_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.pwe.default_frwrd.default_forwarding.additional_info.eei.val.mpls_command.push_profile = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_EEI_VAL_MPLS_COMMAND_LABEL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_EEI_VAL_MPLS_COMMAND_LABEL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.pwe.default_frwrd.default_forwarding.additional_info.eei.val.mpls_command.label = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_EEI_VAL_MPLS_COMMAND_COMMAND_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_EEI_VAL_MPLS_COMMAND_COMMAND_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.pwe.default_frwrd.default_forwarding.additional_info.eei.val.mpls_command.command = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_EEI_VAL_TRILL_DEST_DEST_NICK_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_EEI_VAL_TRILL_DEST_DEST_NICK_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.pwe.default_frwrd.default_forwarding.additional_info.eei.val.trill_dest.dest_nick = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_EEI_VAL_TRILL_DEST_IS_MULTICAST_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_EEI_VAL_TRILL_DEST_IS_MULTICAST_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.pwe.default_frwrd.default_forwarding.additional_info.eei.val.trill_dest.is_multicast = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_EEI_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_EEI_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.pwe.default_frwrd.default_forwarding.additional_info.eei.type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_DEFAULT_FRWRD_DEFAULT_FORWARDING_DEST_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_DEFAULT_FRWRD_DEFAULT_FORWARDING_DEST_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.pwe.default_frwrd.default_forwarding.dest_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_DEFAULT_FRWRD_DEFAULT_FORWARDING_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_DEFAULT_FRWRD_DEFAULT_FORWARDING_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.pwe.default_frwrd.default_forwarding.type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_DEFAULT_FRWRD_DEFAULT_FRWD_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_DEFAULT_FRWRD_DEFAULT_FRWD_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.pwe.default_frwrd.default_frwd_type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_VSID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_PWE_VSID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.pwe.vsid = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_L2CP_PROFILE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_L2CP_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.ac.l2cp_profile = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_COS_PROFILE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_COS_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.ac.cos_profile = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_ORIENTATION_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_ORIENTATION_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.ac.orientation = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_TRAP_INFO_ACTION_PROFILE_SNOOP_ACTION_STRENGTH_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_TRAP_INFO_ACTION_PROFILE_SNOOP_ACTION_STRENGTH_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.ac.default_frwrd.default_forwarding.additional_info.trap_info.action_profile.snoop_action_strength = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_TRAP_INFO_ACTION_PROFILE_FRWRD_ACTION_STRENGTH_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_TRAP_INFO_ACTION_PROFILE_FRWRD_ACTION_STRENGTH_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.ac.default_frwrd.default_forwarding.additional_info.trap_info.action_profile.frwrd_action_strength = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_TRAP_INFO_ACTION_PROFILE_TRAP_CODE_LSB_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_TRAP_INFO_ACTION_PROFILE_TRAP_CODE_LSB_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.ac.default_frwrd.default_forwarding.additional_info.trap_info.action_profile.trap_code = (SOC_PPC_TRAP_CODE)param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_OUTLIF_VAL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_OUTLIF_VAL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.ac.default_frwrd.default_forwarding.additional_info.outlif.val = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_OUTLIF_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_OUTLIF_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.ac.default_frwrd.default_forwarding.additional_info.outlif.type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_EEI_VAL_ISID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_EEI_VAL_ISID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.ac.default_frwrd.default_forwarding.additional_info.eei.val.isid = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_EEI_VAL_MPLS_COMMAND_PUSH_PROFILE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_EEI_VAL_MPLS_COMMAND_PUSH_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.ac.default_frwrd.default_forwarding.additional_info.eei.val.mpls_command.push_profile = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_EEI_VAL_MPLS_COMMAND_LABEL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_EEI_VAL_MPLS_COMMAND_LABEL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.ac.default_frwrd.default_forwarding.additional_info.eei.val.mpls_command.label = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_EEI_VAL_MPLS_COMMAND_COMMAND_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_EEI_VAL_MPLS_COMMAND_COMMAND_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.ac.default_frwrd.default_forwarding.additional_info.eei.val.mpls_command.command = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_EEI_VAL_TRILL_DEST_DEST_NICK_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_EEI_VAL_TRILL_DEST_DEST_NICK_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.ac.default_frwrd.default_forwarding.additional_info.eei.val.trill_dest.dest_nick = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_EEI_VAL_TRILL_DEST_IS_MULTICAST_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_EEI_VAL_TRILL_DEST_IS_MULTICAST_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.ac.default_frwrd.default_forwarding.additional_info.eei.val.trill_dest.is_multicast = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_EEI_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_DEFAULT_FRWRD_DEFAULT_FORWARDING_ADDITIONAL_INFO_EEI_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.ac.default_frwrd.default_forwarding.additional_info.eei.type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_DEFAULT_FRWRD_DEFAULT_FORWARDING_DEST_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_DEFAULT_FRWRD_DEFAULT_FORWARDING_DEST_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.ac.default_frwrd.default_forwarding.dest_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_DEFAULT_FRWRD_DEFAULT_FORWARDING_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_DEFAULT_FRWRD_DEFAULT_FORWARDING_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.ac.default_frwrd.default_forwarding.type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_DEFAULT_FRWRD_DEFAULT_FRWD_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_DEFAULT_FRWRD_DEFAULT_FRWD_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.ac.default_frwrd.default_frwd_type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_ING_EDIT_INFO_EDIT_PCP_PROFILE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_ING_EDIT_INFO_EDIT_PCP_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.ac.ing_edit_info.edit_pcp_profile = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_ING_EDIT_INFO_VID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_ING_EDIT_INFO_VID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.ac.ing_edit_info.vid = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_ING_EDIT_INFO_ING_VLAN_EDIT_PROFILE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_ING_EDIT_INFO_ING_VLAN_EDIT_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.ac.ing_edit_info.ing_vlan_edit_profile = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_VSID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_VSID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.ac.vsid = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_SERVICE_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_VALUE_AC_SERVICE_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.value.ac.service_type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_LIF_ENTRY_INFO_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_entry_info.type = param_val->numeric_equivalent;
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
  
#endif
#ifdef UI_LIF_TABLE/* { lif_table*/
/******************************************************************** 
 *  Section handler: lif_table
 */ 
int 
  ui_ppd_api_lif_table( 
    CURRENT_LINE *current_line 
  ) 
{   
  int
    ret;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif_table"); 
 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_GET_BLOCK_GET_BLOCK_ID,1)) 
  { 
    ret = ui_ppd_api_lif_table_get_block(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_GET_ENTRY_GET_ID,1)) 
  { 
    ret = ui_ppd_api_lif_table_entry_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LIF_TABLE_ENTRY_UPDATE_ENTRY_UPDATE_ID,1)) 
  { 
    ret = ui_ppd_api_lif_table_entry_update(current_line); 
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting function name after lif_table***", TRUE); 
  } 
    
  goto exit; 
exit:        
  return ui_ret; 
}                
          
#endif /* lif_table */ 


#endif /* LINK_PPD_LIBRARIES */ 

