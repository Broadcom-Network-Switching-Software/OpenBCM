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
 
#include <appl/dpp/UserInterface/ui_pure_defi_ppd_api.h> 
 
  
#include <soc/dpp/PPD/ppd_api_frwrd_fec.h>
#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_frwrd_fec.h>




#include <appl/dpp/UserInterface/ppd/ui_ppd_api_frwrd_mact.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_lif.h> 



#include <soc/dpp/PPD/ppd_api_lif_cos.h>
#include <soc/dpp/PPD/ppd_api_lif_table.h>

#include <appl/diag/diag_alloc.h>




#if LINK_PPD_LIBRARIES

#ifdef UI_FRWRD_FEC
/******************************************************************** 
 *  Function handler: glbl_info_set (section frwrd_fec)
 */
int 
  ui_ppd_api_frwrd_fec_glbl_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32 
    prm_ecmp_sizes_index = 0xFFFFFFFF;  
  SOC_PPC_FRWRD_FEC_GLBL_INFO   
    prm_glbl_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_fec"); 
  soc_sand_proc_name = "soc_ppd_frwrd_fec_glbl_info_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_FEC_GLBL_INFO_clear(unit, &prm_glbl_info);
 
  /* Get parameters */ 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_frwrd_fec_glbl_info_get(
          unit,
          &prm_glbl_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_fec_glbl_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_fec_glbl_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_ECMP_SIZES_NOF_ENTRIES_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_ECMP_SIZES_NOF_ENTRIES_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_glbl_info.ecmp_sizes_nof_entries = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_ECMP_SIZES_INDEX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_ECMP_SIZES_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_ecmp_sizes_index = (uint32)param_val->value.ulong_value;
  } 

  if(prm_ecmp_sizes_index != 0xFFFFFFFF)          
  { 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_ECMP_SIZES_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_ECMP_SIZES_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_glbl_info.ecmp_sizes[ prm_ecmp_sizes_index] = (uint32)param_val->value.ulong_value;
  } 

  }   


  /* Call function */
  ret = soc_ppd_frwrd_fec_glbl_info_set(
          unit,
          &prm_glbl_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_fec_glbl_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_fec_glbl_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: glbl_info_get (section frwrd_fec)
 */
int 
  ui_ppd_api_frwrd_fec_glbl_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_FRWRD_FEC_GLBL_INFO   
    prm_glbl_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_fec"); 
  soc_sand_proc_name = "soc_ppd_frwrd_fec_glbl_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_FEC_GLBL_INFO_clear(unit, &prm_glbl_info);
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_ppd_frwrd_fec_glbl_info_get(
          unit,
          &prm_glbl_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_fec_glbl_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_fec_glbl_info_get");   
    goto exit; 
  } 

  SOC_PPC_FRWRD_FEC_GLBL_INFO_print(unit, &prm_glbl_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: entry_add (section frwrd_fec)
 */
int 
  ui_ppd_api_frwrd_fec_entry_add(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_FEC_ID   
    prm_fec_ndx;
  SOC_PPC_FRWRD_FEC_PROTECT_TYPE   
    prm_protect_type;
  SOC_PPC_FRWRD_FEC_ENTRY_INFO   
    prm_working_fec;
  SOC_PPC_FRWRD_FEC_ENTRY_INFO   
    prm_protect_fec;
  SOC_PPC_FRWRD_FEC_PROTECT_INFO   
    prm_protect_info;
  uint8   
    prm_success;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_fec"); 
  soc_sand_proc_name = "soc_ppd_frwrd_fec_entry_add"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_FEC_ENTRY_INFO_clear(&prm_working_fec);
  SOC_PPC_FRWRD_FEC_ENTRY_INFO_clear(&prm_protect_fec);
  SOC_PPC_FRWRD_FEC_PROTECT_INFO_clear(&prm_protect_info);
 
  prm_protect_fec.rpf_info.rpf_mode = SOC_PPC_FRWRD_FEC_RPF_MODE_UC_LOOSE;
  prm_working_fec.rpf_info.rpf_mode = SOC_PPC_FRWRD_FEC_RPF_MODE_UC_LOOSE;

  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_FEC_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_FEC_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_fec_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter fec_ndx after entry_add***", TRUE); 
    goto exit; 
  } 

  ret = soc_ppd_frwrd_fec_entry_get(
          unit,
          prm_fec_ndx,
          &prm_protect_type,
          &prm_working_fec,
          &prm_protect_fec,
          &prm_protect_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_fec_entry_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_fec_entry_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_PROTECT_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_PROTECT_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_protect_type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_WORKING_FEC_TRAP_INFO_SNOOP_ACTION_STRENGTH_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_WORKING_FEC_TRAP_INFO_SNOOP_ACTION_STRENGTH_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_working_fec.trap_info.snoop_action_strength = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_WORKING_FEC_TRAP_INFO_FRWRD_ACTION_STRENGTH_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_WORKING_FEC_TRAP_INFO_FRWRD_ACTION_STRENGTH_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_working_fec.trap_info.frwrd_action_strength = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_WORKING_FEC_TRAP_INFO_TRAP_CODE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_WORKING_FEC_TRAP_INFO_TRAP_CODE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_working_fec.trap_info.trap_code = (SOC_PPC_TRAP_CODE)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_WORKING_FEC_RPF_INFO_EXPECTED_IN_RIF_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_WORKING_FEC_RPF_INFO_EXPECTED_IN_RIF_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_working_fec.rpf_info.expected_in_rif = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_WORKING_FEC_RPF_INFO_RPF_MODE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_WORKING_FEC_RPF_INFO_RPF_MODE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    if(param_val->numeric_equivalent != SOC_PPC_FRWRD_FEC_RPF_MODE_UC_STRICT) {
        prm_working_fec.rpf_info.rpf_mode |= param_val->numeric_equivalent;
    }
    else
    {
        prm_working_fec.rpf_info.rpf_mode = param_val->numeric_equivalent;
    }
    
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_WORKING_FEC_APP_INFO_DIST_TREE_NICK_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_WORKING_FEC_APP_INFO_DIST_TREE_NICK_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_working_fec.app_info.dist_tree_nick = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_WORKING_FEC_RPF_INFO_RPF_MODE_VAL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_WORKING_FEC_RPF_INFO_RPF_MODE_VAL_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_working_fec.rpf_info.rpf_mode = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_WORKING_FEC_APP_INFO_OUT_RIF_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_WORKING_FEC_APP_INFO_OUT_RIF_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_working_fec.app_info.out_rif = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_WORKING_FEC_APP_INFO_OUT_RIF2_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_WORKING_FEC_APP_INFO_OUT_RIF2_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_working_fec.app_info.out_rif = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_WORKING_FEC_APP_INFO_OUT_AC_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_WORKING_FEC_APP_INFO_OUT_AC_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_working_fec.app_info.out_ac_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_WORKING_FEC_EEP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_WORKING_FEC_EEP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_working_fec.eep = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_WORKING_FEC_DEST_DEST_VAL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_WORKING_FEC_DEST_DEST_VAL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_working_fec.dest.dest_val = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_WORKING_FEC_DEST_DEST_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_WORKING_FEC_DEST_DEST_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_working_fec.dest.dest_type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_WORKING_FEC_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_WORKING_FEC_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_working_fec.type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_PROTECT_FEC_TRAP_INFO_SNOOP_ACTION_STRENGTH_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_PROTECT_FEC_TRAP_INFO_SNOOP_ACTION_STRENGTH_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_protect_fec.trap_info.snoop_action_strength = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_PROTECT_FEC_TRAP_INFO_FRWRD_ACTION_STRENGTH_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_PROTECT_FEC_TRAP_INFO_FRWRD_ACTION_STRENGTH_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_protect_fec.trap_info.frwrd_action_strength = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_PROTECT_FEC_TRAP_INFO_TRAP_CODE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_PROTECT_FEC_TRAP_INFO_TRAP_CODE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_protect_fec.trap_info.trap_code = (SOC_PPC_TRAP_CODE)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_PROTECT_FEC_RPF_INFO_EXPECTED_IN_RIF_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_PROTECT_FEC_RPF_INFO_EXPECTED_IN_RIF_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_protect_fec.rpf_info.expected_in_rif = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_PROTECT_FEC_RPF_INFO_RPF_MODE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_PROTECT_FEC_RPF_INFO_RPF_MODE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    if(param_val->numeric_equivalent != SOC_PPC_FRWRD_FEC_RPF_MODE_UC_STRICT) {
        prm_protect_fec.rpf_info.rpf_mode |= param_val->numeric_equivalent;
    }
    else
    {
        prm_protect_fec.rpf_info.rpf_mode |= param_val->numeric_equivalent;
    }
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_PROTECT_FEC_RPF_INFO_RPF_MODE_VAL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_PROTECT_FEC_RPF_INFO_RPF_MODE_VAL_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_protect_fec.rpf_info.rpf_mode = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_PROTECT_FEC_APP_INFO_DIST_TREE_NICK_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_PROTECT_FEC_APP_INFO_DIST_TREE_NICK_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_protect_fec.app_info.dist_tree_nick = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_PROTECT_FEC_APP_INFO_OUT_RIF_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_PROTECT_FEC_APP_INFO_OUT_RIF_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_protect_fec.app_info.out_rif = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_PROTECT_FEC_APP_INFO_OUT_RIF2_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_PROTECT_FEC_APP_INFO_OUT_RIF2_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_protect_fec.app_info.out_rif = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_PROTECT_FEC_APP_INFO_OUT_AC_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_PROTECT_FEC_APP_INFO_OUT_AC_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_protect_fec.app_info.out_ac_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_PROTECT_FEC_EEP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_PROTECT_FEC_EEP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_protect_fec.eep = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_PROTECT_FEC_DEST_DEST_VAL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_PROTECT_FEC_DEST_DEST_VAL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_protect_fec.dest.dest_val = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_PROTECT_FEC_DEST_DEST_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_PROTECT_FEC_DEST_DEST_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_protect_fec.dest.dest_type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_PROTECT_FEC_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_PROTECT_FEC_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_protect_fec.type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_PROTECT_INFO_OAM_INSTANCE_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_PROTECT_INFO_OAM_INSTANCE_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_protect_info.oam_instance_id = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_frwrd_fec_entry_add(
          unit,
          prm_fec_ndx,
          prm_protect_type,
          &prm_working_fec,
          &prm_protect_fec,
          &prm_protect_info,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_fec_entry_add - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_fec_entry_add");   
    goto exit; 
  } 

  cli_out("success: %u\n\r",prm_success);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: ecmp_add (section frwrd_fec)
 */
int 
  ui_ppd_api_frwrd_fec_ecmp_add(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32
    indx,
    inc_eep=0,
    inc_outrif=0;
  SOC_PPC_FEC_ID   
    prm_fec_ndx;
  SOC_PPC_FRWRD_FEC_ENTRY_INFO   
    prm_fec_array[290];
  uint32   
    prm_nof_entries = 0;
  uint8   
    prm_success;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_fec"); 
  soc_sand_proc_name = "soc_ppd_frwrd_fec_ecmp_add"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_FEC_ENTRY_INFO_clear(&prm_fec_array[0]);

  prm_fec_array[0].rpf_info.rpf_mode = SOC_PPC_FRWRD_FEC_RPF_MODE_UC_LOOSE;
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_ADD_ECMP_ADD_FEC_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_ADD_ECMP_ADD_FEC_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_fec_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter fec_ndx after ecmp_add***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_ADD_ECMP_ADD_FEC_ARRAY_TRAP_INFO_SNOOP_ACTION_STRENGTH_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_ADD_ECMP_ADD_FEC_ARRAY_TRAP_INFO_SNOOP_ACTION_STRENGTH_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fec_array[0].trap_info.snoop_action_strength = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_ADD_ECMP_ADD_FEC_ARRAY_TRAP_INFO_FRWRD_ACTION_STRENGTH_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_ADD_ECMP_ADD_FEC_ARRAY_TRAP_INFO_FRWRD_ACTION_STRENGTH_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fec_array[0].trap_info.frwrd_action_strength = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_ADD_ECMP_ADD_FEC_ARRAY_TRAP_INFO_TRAP_CODE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_ADD_ECMP_ADD_FEC_ARRAY_TRAP_INFO_TRAP_CODE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fec_array[0].trap_info.trap_code = (SOC_PPC_TRAP_CODE)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_ADD_ECMP_ADD_FEC_ARRAY_RPF_INFO_EXPECTED_IN_RIF_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_ADD_ECMP_ADD_FEC_ARRAY_RPF_INFO_EXPECTED_IN_RIF_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fec_array[0].rpf_info.expected_in_rif = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_ADD_ECMP_ADD_FEC_ARRAY_RPF_INFO_RPF_MODE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_ADD_ECMP_ADD_FEC_ARRAY_RPF_INFO_RPF_MODE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    if(param_val->numeric_equivalent != SOC_PPC_FRWRD_FEC_RPF_MODE_UC_STRICT) {
        prm_fec_array[0].rpf_info.rpf_mode |= param_val->numeric_equivalent;
    }
    else
    {
        prm_fec_array[0].rpf_info.rpf_mode |= param_val->numeric_equivalent;
    }
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_ADD_ECMP_ADD_FEC_ARRAY_RPF_INFO_RPF_MODE_VAL_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_ADD_ECMP_ADD_FEC_ARRAY_RPF_INFO_RPF_MODE_VAL_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fec_array[0].rpf_info.rpf_mode = (uint32)param_val->value.ulong_value;
  } 


  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_ADD_ECMP_ADD_FEC_ARRAY_APP_INFO_DIST_TREE_NICK_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_ADD_ECMP_ADD_FEC_ARRAY_APP_INFO_DIST_TREE_NICK_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fec_array[0].app_info.dist_tree_nick = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_ADD_ECMP_ADD_FEC_ARRAY_APP_INFO_OUT_RIF_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_ADD_ECMP_ADD_FEC_ARRAY_APP_INFO_OUT_RIF_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fec_array[0].app_info.out_rif = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_ADD_ECMP_ADD_FEC_ARRAY_APP_INFO_OUT_RIF2_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_ADD_ECMP_ADD_FEC_ARRAY_APP_INFO_OUT_RIF2_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fec_array[0].app_info.out_rif = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_ADD_ECMP_ADD_FEC_ARRAY_APP_INFO_OUT_AC_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_ADD_ECMP_ADD_FEC_ARRAY_APP_INFO_OUT_AC_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fec_array[0].app_info.out_ac_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_ADD_ECMP_ADD_FEC_ARRAY_EEP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_ADD_ECMP_ADD_FEC_ARRAY_EEP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fec_array[0].eep = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_ADD_ECMP_ADD_FEC_ARRAY_DEST_DEST_VAL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_ADD_ECMP_ADD_FEC_ARRAY_DEST_DEST_VAL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fec_array[0].dest.dest_val = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_ADD_ECMP_ADD_FEC_ARRAY_DEST_DEST_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_ADD_ECMP_ADD_FEC_ARRAY_DEST_DEST_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fec_array[0].dest.dest_type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_ADD_ECMP_ADD_FEC_ARRAY_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_ADD_ECMP_ADD_FEC_ARRAY_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fec_array[0].type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_ADD_ECMP_ADD_NOF_ENTRIES_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_ADD_ECMP_ADD_NOF_ENTRIES_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_nof_entries = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_ADD_ECMP_ADD_NOF_ENTRIES_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_ADD_ECMP_ADD_NOF_ENTRIES_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_nof_entries = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_ADD_ECMP_ADD_INC_EEP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_ADD_ECMP_ADD_INC_EEP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    inc_eep = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_ADD_ECMP_ADD_INC_OUTRIF_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_ADD_ECMP_ADD_INC_OUTRIF_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    inc_outrif = (uint32)param_val->value.ulong_value;
  } 

  for (indx = 1; indx < prm_nof_entries; ++indx)
  {
    sal_memcpy(&prm_fec_array[indx],&prm_fec_array[0],sizeof(SOC_PPC_FRWRD_FEC_ENTRY_INFO));
    if (inc_eep)
    {
      prm_fec_array[indx].eep += (indx*inc_eep);
    }
    if (inc_outrif)
    {
      prm_fec_array[indx].app_info.out_rif += (indx*inc_outrif);
    }
  }

  /* Call function */
  ret = soc_ppd_frwrd_fec_ecmp_add(
          unit,
          prm_fec_ndx,
          prm_fec_array,
          prm_nof_entries,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_fec_ecmp_add - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_fec_ecmp_add");   
    goto exit; 
  } 

  cli_out("success: %u\n\r",prm_success);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
/******************************************************************** 
 *  Function handler: ecmp_update (section frwrd_fec)
 */
int 
  ui_ppd_api_frwrd_fec_ecmp_update(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_FEC_ID   
    prm_fec_ndx;
  uint32
    indx;
  SOC_PPC_FRWRD_FEC_ENTRY_INFO   
    prm_fec_array[290];
  SOC_SAND_U32_RANGE   
    prm_fec_range;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_fec"); 
  soc_sand_proc_name = "soc_ppd_frwrd_fec_ecmp_update"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_FEC_ENTRY_INFO_clear(&prm_fec_array[0]);
  soc_sand_SAND_U32_RANGE_clear(&prm_fec_range);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_UPDATE_ECMP_UPDATE_FEC_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_UPDATE_ECMP_UPDATE_FEC_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_fec_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter fec_ndx after ecmp_update***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_UPDATE_ECMP_UPDATE_FEC_ARRAY_TRAP_INFO_SNOOP_ACTION_STRENGTH_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_UPDATE_ECMP_UPDATE_FEC_ARRAY_TRAP_INFO_SNOOP_ACTION_STRENGTH_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fec_array[0].trap_info.snoop_action_strength = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_UPDATE_ECMP_UPDATE_FEC_ARRAY_TRAP_INFO_FRWRD_ACTION_STRENGTH_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_UPDATE_ECMP_UPDATE_FEC_ARRAY_TRAP_INFO_FRWRD_ACTION_STRENGTH_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fec_array[0].trap_info.frwrd_action_strength = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_UPDATE_ECMP_UPDATE_FEC_ARRAY_TRAP_INFO_TRAP_CODE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_UPDATE_ECMP_UPDATE_FEC_ARRAY_TRAP_INFO_TRAP_CODE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fec_array[0].trap_info.trap_code = (SOC_PPC_TRAP_CODE)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_UPDATE_ECMP_UPDATE_FEC_ARRAY_RPF_INFO_EXPECTED_IN_RIF_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_UPDATE_ECMP_UPDATE_FEC_ARRAY_RPF_INFO_EXPECTED_IN_RIF_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fec_array[0].rpf_info.expected_in_rif = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_UPDATE_ECMP_UPDATE_FEC_ARRAY_RPF_INFO_RPF_MODE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_UPDATE_ECMP_UPDATE_FEC_ARRAY_RPF_INFO_RPF_MODE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    if(param_val->numeric_equivalent != SOC_PPC_FRWRD_FEC_RPF_MODE_UC_STRICT) {
        prm_fec_array[0].rpf_info.rpf_mode |= param_val->numeric_equivalent;
    }
    else
    {
        prm_fec_array[0].rpf_info.rpf_mode |= param_val->numeric_equivalent;
    }
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_UPDATE_ECMP_UPDATE_FEC_ARRAY_RPF_INFO_RPF_MODE_VAL_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_UPDATE_ECMP_UPDATE_FEC_ARRAY_RPF_INFO_RPF_MODE_VAL_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fec_array[0].rpf_info.rpf_mode = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_UPDATE_ECMP_UPDATE_FEC_ARRAY_APP_INFO_DIST_TREE_NICK_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_UPDATE_ECMP_UPDATE_FEC_ARRAY_APP_INFO_DIST_TREE_NICK_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fec_array[0].app_info.dist_tree_nick = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_UPDATE_ECMP_UPDATE_FEC_ARRAY_APP_INFO_OUT_RIF_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_UPDATE_ECMP_UPDATE_FEC_ARRAY_APP_INFO_OUT_RIF_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fec_array[0].app_info.out_rif = (uint32)param_val->value.ulong_value;
  } 


  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_UPDATE_ECMP_UPDATE_FEC_ARRAY_APP_INFO_OUT_RIF2_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_UPDATE_ECMP_UPDATE_FEC_ARRAY_APP_INFO_OUT_RIF2_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fec_array[0].app_info.out_rif = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_UPDATE_ECMP_UPDATE_FEC_ARRAY_APP_INFO_OUT_AC_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_UPDATE_ECMP_UPDATE_FEC_ARRAY_APP_INFO_OUT_AC_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fec_array[0].app_info.out_ac_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_UPDATE_ECMP_UPDATE_FEC_ARRAY_EEP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_UPDATE_ECMP_UPDATE_FEC_ARRAY_EEP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fec_array[0].eep = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_UPDATE_ECMP_UPDATE_FEC_ARRAY_DEST_DEST_VAL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_UPDATE_ECMP_UPDATE_FEC_ARRAY_DEST_DEST_VAL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fec_array[0].dest.dest_val = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_UPDATE_ECMP_UPDATE_FEC_ARRAY_DEST_DEST_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_UPDATE_ECMP_UPDATE_FEC_ARRAY_DEST_DEST_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fec_array[0].dest.dest_type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_UPDATE_ECMP_UPDATE_FEC_ARRAY_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_UPDATE_ECMP_UPDATE_FEC_ARRAY_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fec_array[0].type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_UPDATE_ECMP_UPDATE_FEC_RANGE_START_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_UPDATE_ECMP_UPDATE_FEC_RANGE_START_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fec_range.start = (uint32)param_val->value.ulong_value;
    prm_fec_range.end = prm_fec_range.start;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_UPDATE_ECMP_UPDATE_FEC_RANGE_END_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_UPDATE_ECMP_UPDATE_FEC_RANGE_END_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fec_range.end = (uint32)param_val->value.ulong_value;
  } 

  if (prm_fec_range.end < prm_fec_range.start || prm_fec_range.end > 287)
  {
    cli_out("error in range\n\r");
    goto exit;
  }
  for (indx = 1; indx < prm_fec_range.end - prm_fec_range.start + 1; ++indx)
  {
    sal_memcpy(&prm_fec_array[indx],&prm_fec_array[0],sizeof(SOC_PPC_FRWRD_FEC_ENTRY_INFO));
  }

  /* Call function */
  ret = soc_ppd_frwrd_fec_ecmp_update(
          unit,
          prm_fec_ndx,
          prm_fec_array,
          &prm_fec_range
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_fec_ecmp_update - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_fec_ecmp_update");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: entry_use_info_get (section frwrd_fec)
 */
int 
  ui_ppd_api_frwrd_fec_entry_use_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_FEC_ID   
    prm_fec_ndx;
  SOC_PPC_FRWRD_FEC_ENTRY_USE_INFO   
    prm_fec_entry_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_fec"); 
  soc_sand_proc_name = "soc_ppd_frwrd_fec_entry_use_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_FEC_ENTRY_USE_INFO_clear(&prm_fec_entry_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ENTRY_USE_INFO_GET_ENTRY_USE_INFO_GET_FEC_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ENTRY_USE_INFO_GET_ENTRY_USE_INFO_GET_FEC_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_fec_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter fec_ndx after entry_use_info_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_frwrd_fec_entry_use_info_get(
          unit,
          prm_fec_ndx,
          &prm_fec_entry_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_fec_entry_use_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_fec_entry_use_info_get");   
    goto exit; 
  } 

  SOC_PPC_FRWRD_FEC_ENTRY_USE_INFO_print(&prm_fec_entry_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: entry_get (section frwrd_fec)
 */
int 
  ui_ppd_api_frwrd_fec_entry_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_FEC_ID   
    prm_fec_ndx;
  SOC_PPC_FRWRD_FEC_PROTECT_TYPE   
    prm_protect_type;
  SOC_PPC_FRWRD_FEC_ENTRY_INFO   
    prm_working_fec;
  SOC_PPC_FRWRD_FEC_ENTRY_INFO   
    prm_protect_fec;
  SOC_PPC_FRWRD_FEC_PROTECT_INFO   
    prm_protect_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_fec"); 
  soc_sand_proc_name = "soc_ppd_frwrd_fec_entry_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_FEC_ENTRY_INFO_clear(&prm_working_fec);
  SOC_PPC_FRWRD_FEC_ENTRY_INFO_clear(&prm_protect_fec);
  SOC_PPC_FRWRD_FEC_PROTECT_INFO_clear(&prm_protect_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ENTRY_GET_ENTRY_GET_FEC_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ENTRY_GET_ENTRY_GET_FEC_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_fec_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter fec_ndx after entry_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_frwrd_fec_entry_get(
          unit,
          prm_fec_ndx,
          &prm_protect_type,
          &prm_working_fec,
          &prm_protect_fec,
          &prm_protect_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_fec_entry_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_fec_entry_get");   
    goto exit; 
  } 

  cli_out("protect_type: %s\n\r",SOC_PPC_FRWRD_FEC_PROTECT_TYPE_to_string(prm_protect_type));

  cli_out("working entry: \n\r");
  SOC_PPC_FRWRD_FEC_ENTRY_INFO_print(&prm_working_fec);
  if (prm_protect_type != SOC_PPC_FRWRD_FEC_PROTECT_TYPE_NONE)
  {
    cli_out("protecting entry: \n\r");
    SOC_PPC_FRWRD_FEC_ENTRY_INFO_print(&prm_protect_fec);
    if (prm_protect_type == SOC_PPC_FRWRD_FEC_PROTECT_TYPE_PATH)
    {
      SOC_PPC_FRWRD_FEC_PROTECT_INFO_print(&prm_protect_info);
    }
  }

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: ecmp_get (section frwrd_fec)
 */
int 
  ui_ppd_api_frwrd_fec_ecmp_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32
    indx;
  SOC_PPC_FEC_ID   
    prm_fec_ndx;
  SOC_SAND_U32_RANGE   
    prm_fec_range;
  SOC_PPC_FRWRD_FEC_ENTRY_INFO   
    prm_fec_array[290];
  uint32   
    prm_nof_entries;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_fec"); 
  soc_sand_proc_name = "soc_ppd_frwrd_fec_ecmp_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  soc_sand_SAND_U32_RANGE_clear(&prm_fec_range);
  SOC_PPC_FRWRD_FEC_ENTRY_INFO_clear(&prm_fec_array[0]);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_GET_ECMP_GET_FEC_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_GET_ECMP_GET_FEC_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_fec_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter fec_ndx after ecmp_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_GET_ECMP_GET_FEC_RANGE_END_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_GET_ECMP_GET_FEC_RANGE_END_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fec_range.end = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_GET_ECMP_GET_FEC_RANGE_START_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_GET_ECMP_GET_FEC_RANGE_START_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fec_range.start = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_frwrd_fec_ecmp_get(
          unit,
          prm_fec_ndx,
          &prm_fec_range,
          prm_fec_array,
          &prm_nof_entries
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_fec_ecmp_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_fec_ecmp_get");   
    goto exit; 
  } 

  for (indx = 0; indx < prm_nof_entries; ++indx)
  {
    SOC_PPC_FRWRD_FEC_ENTRY_INFO_print(&prm_fec_array[indx]);
  }

  cli_out("nof_entries: %u\n\r",prm_nof_entries);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: remove (section frwrd_fec)
 */
int 
  ui_ppd_api_frwrd_fec_remove(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_FEC_ID   
    prm_fec_ndx;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_fec"); 
  soc_sand_proc_name = "soc_ppd_frwrd_fec_remove"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_REMOVE_REMOVE_FEC_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_REMOVE_REMOVE_FEC_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_fec_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter fec_ndx after remove***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_frwrd_fec_remove(
          unit,
          prm_fec_ndx
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_fec_remove - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_fec_remove");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: protection_oam_instance_status_set (section frwrd_fec)
 */
int 
  ui_ppd_api_frwrd_fec_protection_oam_instance_status_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_oam_instance_ndx;
  uint8   
    prm_up;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_fec"); 
  soc_sand_proc_name = "soc_ppd_frwrd_fec_protection_oam_instance_status_set"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_SET_PROTECTION_OAM_INSTANCE_STATUS_SET_OAM_INSTANCE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_SET_PROTECTION_OAM_INSTANCE_STATUS_SET_OAM_INSTANCE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_oam_instance_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter oam_instance_ndx after protection_oam_instance_status_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_frwrd_fec_protection_oam_instance_status_get(
          unit,
          prm_oam_instance_ndx,
          &prm_up
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_fec_protection_oam_instance_status_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_fec_protection_oam_instance_status_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_SET_PROTECTION_OAM_INSTANCE_STATUS_SET_UP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_SET_PROTECTION_OAM_INSTANCE_STATUS_SET_UP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_up = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_frwrd_fec_protection_oam_instance_status_set(
          unit,
          prm_oam_instance_ndx,
          prm_up
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_fec_protection_oam_instance_status_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_fec_protection_oam_instance_status_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: protection_oam_instance_status_get (section frwrd_fec)
 */
int 
  ui_ppd_api_frwrd_fec_protection_oam_instance_status_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_oam_instance_ndx;
  uint8   
    prm_up;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_fec"); 
  soc_sand_proc_name = "soc_ppd_frwrd_fec_protection_oam_instance_status_get"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_GET_PROTECTION_OAM_INSTANCE_STATUS_GET_OAM_INSTANCE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_GET_PROTECTION_OAM_INSTANCE_STATUS_GET_OAM_INSTANCE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_oam_instance_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter oam_instance_ndx after protection_oam_instance_status_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_frwrd_fec_protection_oam_instance_status_get(
          unit,
          prm_oam_instance_ndx,
          &prm_up
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_fec_protection_oam_instance_status_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_fec_protection_oam_instance_status_get");   
    goto exit; 
  } 

  cli_out("up: %u\n\r",prm_up);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: protection_sys_port_status_set (section frwrd_fec)
 */
int 
  ui_ppd_api_frwrd_fec_protection_sys_port_status_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_SAND_PP_SYS_PORT_ID   
    prm_sys_port_ndx;
  uint8   
    prm_up;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_fec"); 
  soc_sand_proc_name = "soc_ppd_frwrd_fec_protection_sys_port_status_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  soc_sand_SAND_PP_SYS_PORT_ID_clear(&prm_sys_port_ndx);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_SET_PROTECTION_SYS_PORT_STATUS_SET_SYS_PORT_NDX_SYS_ID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_SET_PROTECTION_SYS_PORT_STATUS_SET_SYS_PORT_NDX_SYS_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_sys_port_ndx.sys_id = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_SET_PROTECTION_SYS_PORT_STATUS_SET_SYS_PORT_NDX_SYS_PORT_TYPE_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_SET_PROTECTION_SYS_PORT_STATUS_SET_SYS_PORT_NDX_SYS_PORT_TYPE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_sys_port_ndx.sys_port_type = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter sys_port_ndx after protection_sys_port_status_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_frwrd_fec_protection_sys_port_status_get(
          unit,
          &prm_sys_port_ndx,
          &prm_up
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_fec_protection_sys_port_status_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_fec_protection_sys_port_status_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_SET_PROTECTION_SYS_PORT_STATUS_SET_UP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_SET_PROTECTION_SYS_PORT_STATUS_SET_UP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_up = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_frwrd_fec_protection_sys_port_status_set(
          unit,
          &prm_sys_port_ndx,
          prm_up
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_fec_protection_sys_port_status_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_fec_protection_sys_port_status_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: protection_sys_port_status_get (section frwrd_fec)
 */
int 
  ui_ppd_api_frwrd_fec_protection_sys_port_status_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_SAND_PP_SYS_PORT_ID   
    prm_sys_port_ndx;
  uint8   
    prm_up;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_fec"); 
  soc_sand_proc_name = "soc_ppd_frwrd_fec_protection_sys_port_status_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  soc_sand_SAND_PP_SYS_PORT_ID_clear(&prm_sys_port_ndx);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_GET_PROTECTION_SYS_PORT_STATUS_GET_SYS_PORT_NDX_SYS_ID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_GET_PROTECTION_SYS_PORT_STATUS_GET_SYS_PORT_NDX_SYS_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_sys_port_ndx.sys_id = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_GET_PROTECTION_SYS_PORT_STATUS_GET_SYS_PORT_NDX_SYS_PORT_TYPE_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_GET_PROTECTION_SYS_PORT_STATUS_GET_SYS_PORT_NDX_SYS_PORT_TYPE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_sys_port_ndx.sys_port_type = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter sys_port_ndx after protection_sys_port_status_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_frwrd_fec_protection_sys_port_status_get(
          unit,
          &prm_sys_port_ndx,
          &prm_up
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_fec_protection_sys_port_status_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_fec_protection_sys_port_status_get");   
    goto exit; 
  } 

  cli_out("up: %u\n\r",prm_up);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: get_block (section frwrd_fec)
 */
int 
  ui_ppd_api_frwrd_fec_get_block(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_FRWRD_FEC_MATCH_RULE   
    prm_rule;
  SOC_SAND_TABLE_BLOCK_RANGE   
    prm_block_range;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_fec"); 
  soc_sand_proc_name = "soc_ppd_frwrd_fec_get_block"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_FEC_MATCH_RULE_clear(&prm_rule);
  soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&prm_block_range);
 
  prm_rule.type = SOC_PPC_FRWRD_FEC_MATCH_RULE_TYPE_ALL;
  prm_block_range.entries_to_act = UI_PPD_BLOCK_SIZE;
  prm_block_range.entries_to_scan = 10000;
  SOC_SAND_TBL_ITER_SET_BEGIN(&prm_block_range.iter);
  

  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_GET_BLOCK_GET_BLOCK_RULE_VALUE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_GET_BLOCK_GET_BLOCK_RULE_VALUE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rule.value = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_GET_BLOCK_GET_BLOCK_RULE_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_FEC_GET_BLOCK_GET_BLOCK_RULE_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rule.type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_GET_BLOCK_GET_BLOCK_START_FROM_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_GET_BLOCK_GET_BLOCK_START_FROM_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_block_range.iter = (uint32)param_val->value.ulong_value;
  } 

  ret = cmd_diag_alloc_hw_fec_get_block_and_print(unit, 
                                                  &prm_rule, 
                                                  &prm_block_range, 
                                                  UI_PPD_BLOCK_SIZE);

  if (ret != CMD_OK) {
      send_string_to_screen(" *** cmd_diag_alloc_hw_fec_get_block_and_print - FAIL", TRUE); 
      soc_sand_disp_result_proc(ret, "cmd_diag_alloc_hw_fec_get_block_and_print");      
  }

exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: entry_accessed_info_set (section frwrd_fec)
 */
int
ui_ppd_api_frwrd_fec_entry_accessed_info_set(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint8
    prm_clear_access_stat=FALSE;
  SOC_PPC_FEC_ID   
    prm_fec_id_ndx;
  SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO   
    prm_accessed_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_fec"); 
  soc_sand_proc_name = "soc_ppd_frwrd_fec_entry_accessed_info_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO_clear(&prm_accessed_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ACCESSED_INFO_SET_ENTRY_ACCESSED_INFO_SET_FEC_ID_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ACCESSED_INFO_SET_ENTRY_ACCESSED_INFO_SET_FEC_ID_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_fec_id_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter fec_id_ndx after entry_accessed_info_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_frwrd_fec_entry_accessed_info_get(
          unit,
          prm_fec_id_ndx,
          prm_clear_access_stat,
          &prm_accessed_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_fec_entry_accessed_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_fec_entry_accessed_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ACCESSED_INFO_SET_ENTRY_ACCESSED_INFO_SET_ACCESSED_INFO_ACCESSED_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ACCESSED_INFO_SET_ENTRY_ACCESSED_INFO_SET_ACCESSED_INFO_ACCESSED_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_accessed_info.accessed = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ACCESSED_INFO_SET_ENTRY_ACCESSED_INFO_SET_ACCESSED_INFO_TRAP_IF_ACCESSED_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ACCESSED_INFO_SET_ENTRY_ACCESSED_INFO_SET_ACCESSED_INFO_TRAP_IF_ACCESSED_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_accessed_info.trap_if_accessed = (uint8)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_frwrd_fec_entry_accessed_info_set(
    unit,
    prm_fec_id_ndx,
          &prm_accessed_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_fec_entry_accessed_info_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_fec_entry_accessed_info_set");
    goto exit;
  } 

  
  goto exit; 
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: entry_accessed_info_get (section frwrd_fec)
 */
int
ui_ppd_api_frwrd_fec_entry_accessed_info_get(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint8
    prm_clear_access_stat=TRUE;
  SOC_PPC_FEC_ID   
    prm_fec_id_ndx;
  SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO
    prm_accessed_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_fec");
  soc_sand_proc_name = "soc_ppd_frwrd_fec_entry_accessed_info_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO_clear(&prm_accessed_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ACCESSED_INFO_GET_ENTRY_ACCESSED_INFO_GET_FEC_ID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ACCESSED_INFO_GET_ENTRY_ACCESSED_INFO_GET_FEC_ID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_fec_id_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {    
    send_string_to_screen(" *** SW error - expecting parameter fec_id_ndx after entry_accessed_info_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ACCESSED_INFO_GET_ENTRY_ACCESSED_INFO_GET_CLEAR_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ACCESSED_INFO_GET_ENTRY_ACCESSED_INFO_GET_CLEAR_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_clear_access_stat = (uint8)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_frwrd_fec_entry_accessed_info_get(
    unit,
    prm_fec_id_ndx,
    prm_clear_access_stat,
          &prm_accessed_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_fec_entry_accessed_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_fec_entry_accessed_info_get");
    goto exit;
  }

  SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO_print(&prm_accessed_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: ecmp_hashing_global_info_set (section frwrd_fec)
 */
int 
  ui_ppd_api_frwrd_fec_ecmp_hashing_global_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO   
    prm_glbl_hash_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_fec"); 
  soc_sand_proc_name = "soc_ppd_frwrd_fec_ecmp_hashing_global_info_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO_clear(&prm_glbl_hash_info);
 
  /* Get parameters */ 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_frwrd_fec_ecmp_hashing_global_info_get(
          unit,
          &prm_glbl_hash_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_fec_ecmp_hashing_global_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_fec_ecmp_hashing_global_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_SET_ECMP_HASHING_GLOBAL_INFO_SET_GLBL_HASH_INFO_KEY_SHIFT_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_SET_ECMP_HASHING_GLOBAL_INFO_SET_GLBL_HASH_INFO_KEY_SHIFT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_glbl_hash_info.key_shift = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_SET_ECMP_HASHING_GLOBAL_INFO_SET_GLBL_HASH_INFO_HASH_FUNC_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_SET_ECMP_HASHING_GLOBAL_INFO_SET_GLBL_HASH_INFO_HASH_FUNC_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_glbl_hash_info.hash_func_id = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_SET_ECMP_HASHING_GLOBAL_INFO_SET_GLBL_HASH_INFO_SEED_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_SET_ECMP_HASHING_GLOBAL_INFO_SET_GLBL_HASH_INFO_SEED_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_glbl_hash_info.seed = (uint16)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_SET_ECMP_HASHING_GLOBAL_INFO_SET_GLBL_HASH_INFO_USE_PORT_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_SET_ECMP_HASHING_GLOBAL_INFO_SET_GLBL_HASH_INFO_USE_PORT_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_glbl_hash_info.use_port_id = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_frwrd_fec_ecmp_hashing_global_info_set(
          unit,
          &prm_glbl_hash_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_fec_ecmp_hashing_global_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_fec_ecmp_hashing_global_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: ecmp_hashing_global_info_get (section frwrd_fec)
 */
int 
  ui_ppd_api_frwrd_fec_ecmp_hashing_global_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO   
    prm_glbl_hash_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_fec"); 
  soc_sand_proc_name = "soc_ppd_frwrd_fec_ecmp_hashing_global_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO_clear(&prm_glbl_hash_info);
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_ppd_frwrd_fec_ecmp_hashing_global_info_get(
          unit,
          &prm_glbl_hash_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_fec_ecmp_hashing_global_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_fec_ecmp_hashing_global_info_get");   
    goto exit; 
  } 

  SOC_PPC_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO_print(&prm_glbl_hash_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: ecmp_info_set (section frwrd_fec)
 */
int 
  ui_ppd_api_frwrd_fec_ecmp_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_PORT   
    ecmp_ndx;
  SOC_PPC_FRWRD_FEC_ECMP_INFO   
    prm_ecmp_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_fec"); 
  soc_sand_proc_name = "soc_ppd_frwrd_fec_ecmp_info_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_FEC_ECMP_INFO_clear(&prm_ecmp_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_UPDATE_ECMP_INFO_SET_ECMP_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_UPDATE_ECMP_INFO_SET_ECMP_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    ecmp_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter port_ndx after ecmp_info_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_frwrd_fec_ecmp_info_get(
          unit,
          ecmp_ndx,
          &prm_ecmp_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_fec_ecmp_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_fec_ecmp_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_UPDATE_ECMP_INFO_SET_BASE_FEC_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_UPDATE_ECMP_INFO_SET_BASE_FEC_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_ecmp_info.base_fec_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_UPDATE_ECMP_INFO_SET_SIZE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_UPDATE_ECMP_INFO_SET_SIZE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_ecmp_info.size = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_UPDATE_ECMP_INFO_SET_IS_PROTECTED_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_UPDATE_ECMP_INFO_SET_IS_PROTECTED_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_ecmp_info.is_protected = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_UPDATE_ECMP_INFO_SET_RPF_FEC_INDEX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_UPDATE_ECMP_INFO_SET_RPF_FEC_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_ecmp_info.rpf_fec_index = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_frwrd_fec_ecmp_info_set(
          unit,
          ecmp_ndx,
          &prm_ecmp_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_fec_ecmp_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_fec_ecmp_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: ecmp_info_get (section frwrd_fec)
 */
int 
  ui_ppd_api_frwrd_fec_ecmp_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_PORT   
    ecmp_ndx;
  SOC_PPC_FRWRD_FEC_ECMP_INFO   
    prm_ecmp_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_fec"); 
  soc_sand_proc_name = "soc_ppd_frwrd_fec_ecmp_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_FEC_ECMP_INFO_clear(&prm_ecmp_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_UPDATE_ECMP_INFO_GET_ECMP_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_UPDATE_ECMP_INFO_GET_ECMP_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    ecmp_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter port_ndx after ecmp_info_get***", TRUE); 
    goto exit; 
  } 

  /* Call function */
  ret = soc_ppd_frwrd_fec_ecmp_info_get(
          unit,
          ecmp_ndx,
          &prm_ecmp_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_fec_ecmp_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_fec_ecmp_info_get");   
    goto exit; 
  } 

  SOC_PPC_FRWRD_FEC_ECMP_INFO_print(&prm_ecmp_info);

  
  goto exit; 
exit: 
  return ui_ret; 
}   
  
/******************************************************************** 
 *  Function handler: ecmp_hashing_port_info_set (section frwrd_fec)
 */
int 
  ui_ppd_api_frwrd_fec_ecmp_hashing_port_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_PORT   
    prm_port_ndx;
  SOC_PPC_FRWRD_FEC_ECMP_HASH_PORT_INFO   
    prm_port_hash_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_fec"); 
  soc_sand_proc_name = "soc_ppd_frwrd_fec_ecmp_hashing_port_info_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_FEC_ECMP_HASH_PORT_INFO_clear(&prm_port_hash_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_HASHING_PORT_INFO_SET_ECMP_HASHING_PORT_INFO_SET_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_HASHING_PORT_INFO_SET_ECMP_HASHING_PORT_INFO_SET_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter port_ndx after ecmp_hashing_port_info_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_frwrd_fec_ecmp_hashing_port_info_get(
          unit,
          SOC_CORE_DEFAULT,
          prm_port_ndx,
          &prm_port_hash_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_fec_ecmp_hashing_port_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_fec_ecmp_hashing_port_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_HASHING_PORT_INFO_SET_ECMP_HASHING_PORT_INFO_SET_PORT_HASH_INFO_NOF_HEADERS_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_HASHING_PORT_INFO_SET_ECMP_HASHING_PORT_INFO_SET_PORT_HASH_INFO_NOF_HEADERS_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_hash_info.nof_headers = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_frwrd_fec_ecmp_hashing_port_info_set(
          unit,
          SOC_CORE_DEFAULT,
          prm_port_ndx,
          &prm_port_hash_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_fec_ecmp_hashing_port_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_fec_ecmp_hashing_port_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: ecmp_hashing_port_info_get (section frwrd_fec)
 */
int 
  ui_ppd_api_frwrd_fec_ecmp_hashing_port_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_PORT   
    prm_port_ndx;
  SOC_PPC_FRWRD_FEC_ECMP_HASH_PORT_INFO   
    prm_port_hash_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_fec"); 
  soc_sand_proc_name = "soc_ppd_frwrd_fec_ecmp_hashing_port_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_FRWRD_FEC_ECMP_HASH_PORT_INFO_clear(&prm_port_hash_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_HASHING_PORT_INFO_GET_ECMP_HASHING_PORT_INFO_GET_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_FEC_ECMP_HASHING_PORT_INFO_GET_ECMP_HASHING_PORT_INFO_GET_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter port_ndx after ecmp_hashing_port_info_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_frwrd_fec_ecmp_hashing_port_info_get(
          unit,
          SOC_CORE_DEFAULT,
          prm_port_ndx,
          &prm_port_hash_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_fec_ecmp_hashing_port_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_fec_ecmp_hashing_port_info_get");   
    goto exit; 
  } 

  SOC_PPC_FRWRD_FEC_ECMP_HASH_PORT_INFO_print(&prm_port_hash_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
#endif
#ifdef UI_FRWRD_FEC/* { frwrd_fec*/
/******************************************************************** 
 *  Section handler: frwrd_fec
 */ 
int 
  ui_ppd_api_frwrd_fec( 
    CURRENT_LINE *current_line 
  ) 
{   
  int
    ret = FALSE;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_fec"); 
 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_GLBL_INFO_SET_GLBL_INFO_SET_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_fec_glbl_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_GLBL_INFO_GET_GLBL_INFO_GET_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_fec_glbl_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ADD_ENTRY_ADD_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_fec_entry_add(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_ADD_ECMP_ADD_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_fec_ecmp_add(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_UPDATE_ECMP_UPDATE_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_fec_ecmp_update(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ENTRY_USE_INFO_GET_ENTRY_USE_INFO_GET_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_fec_entry_use_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ENTRY_GET_ENTRY_GET_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_fec_entry_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_GET_ECMP_GET_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_fec_ecmp_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_REMOVE_REMOVE_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_fec_remove(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_SET_PROTECTION_OAM_INSTANCE_STATUS_SET_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_fec_protection_oam_instance_status_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_GET_PROTECTION_OAM_INSTANCE_STATUS_GET_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_fec_protection_oam_instance_status_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_SET_PROTECTION_SYS_PORT_STATUS_SET_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_fec_protection_sys_port_status_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_GET_PROTECTION_SYS_PORT_STATUS_GET_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_fec_protection_sys_port_status_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_GET_BLOCK_GET_BLOCK_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_fec_get_block(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ACCESSED_INFO_SET_ENTRY_ACCESSED_INFO_SET_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_fec_entry_accessed_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ENTRY_ACCESSED_INFO_GET_ENTRY_ACCESSED_INFO_GET_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_fec_entry_accessed_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_SET_ECMP_HASHING_GLOBAL_INFO_SET_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_fec_ecmp_hashing_global_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_GET_ECMP_HASHING_GLOBAL_INFO_GET_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_fec_ecmp_hashing_global_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_HASHING_PORT_INFO_SET_ECMP_HASHING_PORT_INFO_SET_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_fec_ecmp_hashing_port_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_HASHING_PORT_INFO_GET_ECMP_HASHING_PORT_INFO_GET_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_fec_ecmp_hashing_port_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_UPDATE_ECMP_INFO_SET_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_fec_ecmp_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_FEC_ECMP_UPDATE_ECMP_INFO_GET_ID,1)) 
  { 
    ret = ui_ppd_api_frwrd_fec_ecmp_info_get(current_line); 
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting function name after frwrd_fec***", TRUE); 
  } 
  
  ui_ret = ret;
    
  goto exit; 
exit:        
  return ui_ret; 
}                
          
#endif /* frwrd_fec */ 


#endif /* LINK_PPD_LIBRARIES */ 

