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
 
  
#include <soc/dpp/PPD/ppd_api_mpls_term.h>
#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_mpls_term.h>

#if LINK_PPD_LIBRARIES

#ifdef UI_MPLS_TERM
/******************************************************************** 
 *  Function handler: lkup_info_set (section mpls_term)
 */
int 
  ui_ppd_api_mpls_term_lkup_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_MPLS_TERM_LKUP_INFO   
    prm_lkup_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_mpls_term"); 
  soc_sand_proc_name = "soc_ppd_mpls_term_lkup_info_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_MPLS_TERM_LKUP_INFO_clear(&prm_lkup_info);
 
  /* Get parameters */ 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_mpls_term_lkup_info_get(
          unit,
          &prm_lkup_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_mpls_term_lkup_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_mpls_term_lkup_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_LKUP_INFO_SET_LKUP_INFO_SET_LKUP_INFO_KEY_TYPE_INDEX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MPLS_TERM_LKUP_INFO_SET_LKUP_INFO_SET_LKUP_INFO_KEY_TYPE_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_LKUP_INFO_SET_LKUP_INFO_SET_LKUP_INFO_KEY_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_MPLS_TERM_LKUP_INFO_SET_LKUP_INFO_SET_LKUP_INFO_KEY_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lkup_info.key_type = param_val->numeric_equivalent;
  } 


  /* Call function */
  ret = soc_ppd_mpls_term_lkup_info_set(
          unit,
          &prm_lkup_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_mpls_term_lkup_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_mpls_term_lkup_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: lkup_info_get (section mpls_term)
 */
int 
  ui_ppd_api_mpls_term_lkup_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_MPLS_TERM_LKUP_INFO   
    prm_lkup_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_mpls_term"); 
  soc_sand_proc_name = "soc_ppd_mpls_term_lkup_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_MPLS_TERM_LKUP_INFO_clear(&prm_lkup_info);
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_ppd_mpls_term_lkup_info_get(
          unit,
          &prm_lkup_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_mpls_term_lkup_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_mpls_term_lkup_info_get");   
    goto exit; 
  } 

  SOC_PPC_MPLS_TERM_LKUP_INFO_print(&prm_lkup_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: label_range_set (section mpls_term)
 */
int 
  ui_ppd_api_mpls_term_label_range_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_range_ndx;
  SOC_PPC_MPLS_TERM_LABEL_RANGE_INFO   
    prm_label_range_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_mpls_term"); 
  soc_sand_proc_name = "soc_ppd_mpls_term_label_range_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_MPLS_TERM_LABEL_RANGE_INFO_clear(&prm_label_range_info);
  prm_label_range_info.term_info.next_prtcl = SOC_PPC_L3_NEXT_PRTCL_TYPE_MPLS;
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_LABEL_RANGE_SET_LABEL_RANGE_SET_RANGE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MPLS_TERM_LABEL_RANGE_SET_LABEL_RANGE_SET_RANGE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_range_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter range_ndx after label_range_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_mpls_term_label_range_get(
          unit,
          prm_range_ndx,
          &prm_label_range_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_mpls_term_label_range_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_mpls_term_label_range_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_LABEL_RANGE_SET_LABEL_RANGE_SET_LABEL_RANGE_INFO_TERM_INFO_NEXT_PRTCL_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_MPLS_TERM_LABEL_RANGE_SET_LABEL_RANGE_SET_LABEL_RANGE_INFO_TERM_INFO_NEXT_PRTCL_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_label_range_info.term_info.next_prtcl = param_val->numeric_equivalent;
    if ((prm_label_range_info.term_info.next_prtcl == SOC_PPC_L3_NEXT_PRTCL_TYPE_IPV4) ||
       (prm_label_range_info.term_info.next_prtcl == SOC_PPC_L3_NEXT_PRTCL_TYPE_IPV6))
    {
      prm_label_range_info.term_info.next_prtcl = SOC_PPC_L3_NEXT_PRTCL_TYPE_IPV4;
    }
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_LABEL_RANGE_SET_LABEL_RANGE_SET_LABEL_RANGE_INFO_TERM_INFO_COS_PROFILE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MPLS_TERM_LABEL_RANGE_SET_LABEL_RANGE_SET_LABEL_RANGE_INFO_TERM_INFO_COS_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_label_range_info.term_info.cos_profile = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_LABEL_RANGE_SET_LABEL_RANGE_SET_LABEL_RANGE_INFO_TERM_INFO_RIF_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MPLS_TERM_LABEL_RANGE_SET_LABEL_RANGE_SET_LABEL_RANGE_INFO_TERM_INFO_RIF_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_label_range_info.term_info.rif = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_LABEL_RANGE_SET_LABEL_RANGE_SET_LABEL_RANGE_INFO_TERM_INFO_PROCESSING_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_MPLS_TERM_LABEL_RANGE_SET_LABEL_RANGE_SET_LABEL_RANGE_INFO_TERM_INFO_PROCESSING_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_label_range_info.term_info.processing_type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_LABEL_RANGE_SET_LABEL_RANGE_SET_LABEL_RANGE_INFO_RANGE_LAST_LABEL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MPLS_TERM_LABEL_RANGE_SET_LABEL_RANGE_SET_LABEL_RANGE_INFO_RANGE_LAST_LABEL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_label_range_info.range.last_label = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_LABEL_RANGE_SET_LABEL_RANGE_SET_LABEL_RANGE_INFO_RANGE_FIRST_LABEL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MPLS_TERM_LABEL_RANGE_SET_LABEL_RANGE_SET_LABEL_RANGE_INFO_RANGE_FIRST_LABEL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_label_range_info.range.first_label = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_mpls_term_label_range_set(
          unit,
          prm_range_ndx,
          &prm_label_range_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_mpls_term_label_range_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_mpls_term_label_range_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: label_range_get (section mpls_term)
 */
int 
  ui_ppd_api_mpls_term_label_range_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_range_ndx;
  SOC_PPC_MPLS_TERM_LABEL_RANGE_INFO   
    prm_label_range_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_mpls_term"); 
  soc_sand_proc_name = "soc_ppd_mpls_term_label_range_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_MPLS_TERM_LABEL_RANGE_INFO_clear(&prm_label_range_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_LABEL_RANGE_GET_LABEL_RANGE_GET_RANGE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MPLS_TERM_LABEL_RANGE_GET_LABEL_RANGE_GET_RANGE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_range_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter range_ndx after label_range_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_mpls_term_label_range_get(
          unit,
          prm_range_ndx,
          &prm_label_range_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_mpls_term_label_range_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_mpls_term_label_range_get");   
    goto exit; 
  } 

  SOC_PPC_MPLS_TERM_LABEL_RANGE_INFO_print(&prm_label_range_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: range_terminated_label_set (section mpls_term)
 */
int 
  ui_ppd_api_mpls_term_range_terminated_label_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_SAND_PP_MPLS_LABEL   
    prm_label_ndx;
  uint8   
    prm_is_terminated_label;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_mpls_term"); 
  soc_sand_proc_name = "soc_ppd_mpls_term_range_terminated_label_set"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_RANGE_TERMINATED_LABEL_SET_RANGE_TERMINATED_LABEL_SET_LABEL_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MPLS_TERM_RANGE_TERMINATED_LABEL_SET_RANGE_TERMINATED_LABEL_SET_LABEL_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_label_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter label_ndx after range_terminated_label_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_mpls_term_range_terminated_label_get(
          unit,
          prm_label_ndx,
          &prm_is_terminated_label
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_mpls_term_range_terminated_label_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_mpls_term_range_terminated_label_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_RANGE_TERMINATED_LABEL_SET_RANGE_TERMINATED_LABEL_SET_IS_TERMINATED_LABEL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MPLS_TERM_RANGE_TERMINATED_LABEL_SET_RANGE_TERMINATED_LABEL_SET_IS_TERMINATED_LABEL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_is_terminated_label = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_mpls_term_range_terminated_label_set(
          unit,
          prm_label_ndx,
          prm_is_terminated_label
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_mpls_term_range_terminated_label_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_mpls_term_range_terminated_label_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: range_terminated_label_get (section mpls_term)
 */
int 
  ui_ppd_api_mpls_term_range_terminated_label_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_SAND_PP_MPLS_LABEL   
    prm_label_ndx;
  uint8   
    prm_is_terminated_label;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_mpls_term"); 
  soc_sand_proc_name = "soc_ppd_mpls_term_range_terminated_label_get"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_RANGE_TERMINATED_LABEL_GET_RANGE_TERMINATED_LABEL_GET_LABEL_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MPLS_TERM_RANGE_TERMINATED_LABEL_GET_RANGE_TERMINATED_LABEL_GET_LABEL_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_label_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter label_ndx after range_terminated_label_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_mpls_term_range_terminated_label_get(
          unit,
          prm_label_ndx,
          &prm_is_terminated_label
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_mpls_term_range_terminated_label_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_mpls_term_range_terminated_label_get");   
    goto exit; 
  } 

  cli_out("is_terminated_label: %u\n\r",prm_is_terminated_label);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: reserved_labels_global_info_set (section mpls_term)
 */
int 
  ui_ppd_api_mpls_term_reserved_labels_global_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_MPLS_TERM_RESERVED_LABELS_GLBL_INFO   
    prm_reserved_labels_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_mpls_term"); 
  soc_sand_proc_name = "soc_ppd_mpls_term_reserved_labels_global_info_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_MPLS_TERM_RESERVED_LABELS_GLBL_INFO_clear(&prm_reserved_labels_info);
 
  /* Get parameters */ 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_mpls_term_reserved_labels_global_info_get(
          unit,
          &prm_reserved_labels_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_mpls_term_reserved_labels_global_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_mpls_term_reserved_labels_global_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_SET_RESERVED_LABELS_GLOBAL_INFO_SET_RESERVED_LABELS_INFO_UC_RPF_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_SET_RESERVED_LABELS_GLOBAL_INFO_SET_RESERVED_LABELS_INFO_UC_RPF_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_reserved_labels_info.uc_rpf_enable = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_SET_RESERVED_LABELS_GLOBAL_INFO_SET_RESERVED_LABELS_INFO_DEFAULT_RIF_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_SET_RESERVED_LABELS_GLOBAL_INFO_SET_RESERVED_LABELS_INFO_DEFAULT_RIF_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_reserved_labels_info.default_rif = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_SET_RESERVED_LABELS_GLOBAL_INFO_SET_RESERVED_LABELS_INFO_PROCESSING_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_SET_RESERVED_LABELS_GLOBAL_INFO_SET_RESERVED_LABELS_INFO_PROCESSING_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_reserved_labels_info.processing_type = param_val->numeric_equivalent;
  } 


  /* Call function */
  ret = soc_ppd_mpls_term_reserved_labels_global_info_set(
          unit,
          &prm_reserved_labels_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_mpls_term_reserved_labels_global_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_mpls_term_reserved_labels_global_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: reserved_labels_global_info_get (section mpls_term)
 */
int 
  ui_ppd_api_mpls_term_reserved_labels_global_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_MPLS_TERM_RESERVED_LABELS_GLBL_INFO   
    prm_reserved_labels_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_mpls_term"); 
  soc_sand_proc_name = "soc_ppd_mpls_term_reserved_labels_global_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_MPLS_TERM_RESERVED_LABELS_GLBL_INFO_clear(&prm_reserved_labels_info);
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_ppd_mpls_term_reserved_labels_global_info_get(
          unit,
          &prm_reserved_labels_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_mpls_term_reserved_labels_global_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_mpls_term_reserved_labels_global_info_get");   
    goto exit; 
  } 

  SOC_PPC_MPLS_TERM_RESERVED_LABELS_GLBL_INFO_print(&prm_reserved_labels_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: reserved_label_info_set (section mpls_term)
 */
int 
  ui_ppd_api_mpls_term_reserved_label_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_SAND_PP_MPLS_LABEL   
    prm_label_ndx;
  SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO   
    prm_label_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_mpls_term"); 
  soc_sand_proc_name = "soc_ppd_mpls_term_reserved_label_info_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO_clear(&prm_label_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_RESERVED_LABEL_INFO_SET_RESERVED_LABEL_INFO_SET_LABEL_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MPLS_TERM_RESERVED_LABEL_INFO_SET_RESERVED_LABEL_INFO_SET_LABEL_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_label_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter label_ndx after reserved_label_info_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_mpls_term_reserved_label_info_get(
          unit,
          prm_label_ndx,
          &prm_label_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_mpls_term_reserved_label_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_mpls_term_reserved_label_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_RESERVED_LABEL_INFO_SET_RESERVED_LABEL_INFO_SET_LABEL_INFO_TERM_INFO_NEXT_PRTCL_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_MPLS_TERM_RESERVED_LABEL_INFO_SET_RESERVED_LABEL_INFO_SET_LABEL_INFO_TERM_INFO_NEXT_PRTCL_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_label_info.term_info.next_prtcl = param_val->numeric_equivalent;
    if ((prm_label_info.term_info.next_prtcl == SOC_PPC_L3_NEXT_PRTCL_TYPE_IPV4) ||
      (prm_label_info.term_info.next_prtcl == SOC_PPC_L3_NEXT_PRTCL_TYPE_IPV6))
    {
      prm_label_info.term_info.next_prtcl = SOC_PPC_L3_NEXT_PRTCL_TYPE_IPV4;
    }
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_RESERVED_LABEL_INFO_SET_RESERVED_LABEL_INFO_SET_LABEL_INFO_TERM_INFO_COS_PROFILE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MPLS_TERM_RESERVED_LABEL_INFO_SET_RESERVED_LABEL_INFO_SET_LABEL_INFO_TERM_INFO_COS_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_label_info.term_info.cos_profile = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_RESERVED_LABEL_INFO_SET_RESERVED_LABEL_INFO_SET_LABEL_INFO_TERM_INFO_RIF_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MPLS_TERM_RESERVED_LABEL_INFO_SET_RESERVED_LABEL_INFO_SET_LABEL_INFO_TERM_INFO_RIF_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_label_info.term_info.rif = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_RESERVED_LABEL_INFO_SET_RESERVED_LABEL_INFO_SET_LABEL_INFO_TERM_INFO_PROCESSING_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_MPLS_TERM_RESERVED_LABEL_INFO_SET_RESERVED_LABEL_INFO_SET_LABEL_INFO_TERM_INFO_PROCESSING_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_label_info.term_info.processing_type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_RESERVED_LABEL_INFO_SET_RESERVED_LABEL_INFO_SET_LABEL_INFO_NON_BOS_ACTION_SNOOP_ACTION_STRENGTH_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MPLS_TERM_RESERVED_LABEL_INFO_SET_RESERVED_LABEL_INFO_SET_LABEL_INFO_NON_BOS_ACTION_SNOOP_ACTION_STRENGTH_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_label_info.non_bos_action.snoop_action_strength = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_RESERVED_LABEL_INFO_SET_RESERVED_LABEL_INFO_SET_LABEL_INFO_NON_BOS_ACTION_FRWRD_ACTION_STRENGTH_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MPLS_TERM_RESERVED_LABEL_INFO_SET_RESERVED_LABEL_INFO_SET_LABEL_INFO_NON_BOS_ACTION_FRWRD_ACTION_STRENGTH_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_label_info.non_bos_action.frwrd_action_strength = (uint32)param_val->value.ulong_value;
  } 

  /*trap-code
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_RESERVED_LABEL_INFO_SET_RESERVED_LABEL_INFO_SET_LABEL_INFO_NON_BOS_ACTION_TRAP_CODE_LSB_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_MPLS_TERM_RESERVED_LABEL_INFO_SET_RESERVED_LABEL_INFO_SET_LABEL_INFO_NON_BOS_ACTION_TRAP_CODE_LSB_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_label_info.non_bos_action.trap_code = (SOC_PPC_TRAP_CODE)param_val->numeric_equivalent;
  } 
  */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_RESERVED_LABEL_INFO_SET_RESERVED_LABEL_INFO_SET_LABEL_INFO_NON_BOS_ACTION_TRAP_CODE_LSB_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MPLS_TERM_RESERVED_LABEL_INFO_SET_RESERVED_LABEL_INFO_SET_LABEL_INFO_NON_BOS_ACTION_TRAP_CODE_LSB_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_label_info.non_bos_action.trap_code = (SOC_PPC_TRAP_CODE)param_val->value.ulong_value;
  } 


  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_RESERVED_LABEL_INFO_SET_RESERVED_LABEL_INFO_SET_LABEL_INFO_BOS_ACTION_SNOOP_ACTION_STRENGTH_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MPLS_TERM_RESERVED_LABEL_INFO_SET_RESERVED_LABEL_INFO_SET_LABEL_INFO_BOS_ACTION_SNOOP_ACTION_STRENGTH_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_label_info.bos_action.snoop_action_strength = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_RESERVED_LABEL_INFO_SET_RESERVED_LABEL_INFO_SET_LABEL_INFO_BOS_ACTION_FRWRD_ACTION_STRENGTH_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MPLS_TERM_RESERVED_LABEL_INFO_SET_RESERVED_LABEL_INFO_SET_LABEL_INFO_BOS_ACTION_FRWRD_ACTION_STRENGTH_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_label_info.bos_action.frwrd_action_strength = (uint32)param_val->value.ulong_value;
  } 

  /*trap-code
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_RESERVED_LABEL_INFO_SET_RESERVED_LABEL_INFO_SET_LABEL_INFO_BOS_ACTION_TRAP_CODE_LSB_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_MPLS_TERM_RESERVED_LABEL_INFO_SET_RESERVED_LABEL_INFO_SET_LABEL_INFO_BOS_ACTION_TRAP_CODE_LSB_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_label_info.bos_action.trap_code = (SOC_PPC_TRAP_CODE)param_val->numeric_equivalent;
  } 
  */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_RESERVED_LABEL_INFO_SET_RESERVED_LABEL_INFO_SET_LABEL_INFO_BOS_ACTION_TRAP_CODE_LSB_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MPLS_TERM_RESERVED_LABEL_INFO_SET_RESERVED_LABEL_INFO_SET_LABEL_INFO_BOS_ACTION_TRAP_CODE_LSB_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_label_info.bos_action.trap_code = (SOC_PPC_TRAP_CODE)param_val->value.ulong_value;
  } 



  /* Call function */
  ret = soc_ppd_mpls_term_reserved_label_info_set(
          unit,
          prm_label_ndx,
          &prm_label_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_mpls_term_reserved_label_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_mpls_term_reserved_label_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: reserved_label_info_get (section mpls_term)
 */
int 
  ui_ppd_api_mpls_term_reserved_label_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_SAND_PP_MPLS_LABEL   
    prm_label_ndx;
  SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO   
    prm_label_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_mpls_term"); 
  soc_sand_proc_name = "soc_ppd_mpls_term_reserved_label_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO_clear(&prm_label_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_RESERVED_LABEL_INFO_GET_RESERVED_LABEL_INFO_GET_LABEL_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MPLS_TERM_RESERVED_LABEL_INFO_GET_RESERVED_LABEL_INFO_GET_LABEL_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_label_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter label_ndx after reserved_label_info_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_mpls_term_reserved_label_info_get(
          unit,
          prm_label_ndx,
          &prm_label_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_mpls_term_reserved_label_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_mpls_term_reserved_label_info_get");   
    goto exit; 
  } 

  SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO_print(&prm_label_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: encountered_entries_get_block (section mpls_term)
 */
int 
  ui_ppd_api_mpls_term_encountered_entries_get_block(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_SAND_PP_MPLS_LABEL   
    prm_first_label_ndx=0;
  uint32   
    prm_nof_encountered_labels,
    strt_range,
    end_range,
    indx;
  uint8
    same_range = FALSE;
  uint32   
    prm_encountered_labels[100];
  SOC_SAND_PP_MPLS_LABEL   
    prm_next_label_id=0;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_mpls_term"); 
  soc_sand_proc_name = "soc_ppd_mpls_term_encountered_entries_get_block"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_ENCOUNTERED_ENTRIES_GET_BLOCK_ENCOUNTERED_ENTRIES_GET_BLOCK_FIRST_LABEL_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MPLS_TERM_ENCOUNTERED_ENTRIES_GET_BLOCK_ENCOUNTERED_ENTRIES_GET_BLOCK_FIRST_LABEL_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_first_label_ndx = (uint32)param_val->value.ulong_value;
  } 

  /* Call function */
  ret = soc_ppd_mpls_term_encountered_entries_get_block(
          unit,
          prm_first_label_ndx,
          &prm_nof_encountered_labels,
          prm_encountered_labels,
          &prm_next_label_id
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_mpls_term_encountered_entries_get_block - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_mpls_term_encountered_entries_get_block");   
    goto exit; 
  } 

  cli_out("nof_encountered_labels: %u\n\r",prm_nof_encountered_labels);
  
  for (indx = 0; indx < prm_nof_encountered_labels; ++indx)
  {
    strt_range = prm_encountered_labels[indx];
    end_range = prm_encountered_labels[indx];
    same_range = TRUE;

    while (same_range)
    {
      ++indx;
      if (indx >= prm_nof_encountered_labels || prm_encountered_labels[indx] != prm_encountered_labels[indx-1])
      {
        if (indx < prm_nof_encountered_labels)
        {
          --indx;
        }
        break;
      }
      end_range = prm_encountered_labels[indx];
    }
    if (end_range == strt_range)
    {
      cli_out("%u, ",strt_range);
    }
    else
    {
      cli_out("%u - %u, ",strt_range,end_range);
    }
  }
  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: cos_info_set (section mpls_term)
 */
int 
  ui_ppd_api_mpls_term_cos_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_MPLS_TERM_COS_INFO   
    prm_term_cos_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_mpls_term"); 
  soc_sand_proc_name = "soc_ppd_mpls_term_cos_info_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_MPLS_TERM_COS_INFO_clear(&prm_term_cos_info);
 
  /* Get parameters */ 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_mpls_term_cos_info_get(
          unit,
          &prm_term_cos_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_mpls_term_cos_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_mpls_term_cos_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_COS_INFO_SET_COS_INFO_SET_TERM_COS_INFO_USE_FOR_UNIFORM_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MPLS_TERM_COS_INFO_SET_COS_INFO_SET_TERM_COS_INFO_USE_FOR_UNIFORM_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_term_cos_info.use_for_uniform = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_COS_INFO_SET_COS_INFO_SET_TERM_COS_INFO_USE_FOR_PIPE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MPLS_TERM_COS_INFO_SET_COS_INFO_SET_TERM_COS_INFO_USE_FOR_PIPE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_term_cos_info.use_for_pipe = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_mpls_term_cos_info_set(
          unit,
          &prm_term_cos_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_mpls_term_cos_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_mpls_term_cos_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: cos_info_get (section mpls_term)
 */
int 
  ui_ppd_api_mpls_term_cos_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_MPLS_TERM_COS_INFO   
    prm_term_cos_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_mpls_term"); 
  soc_sand_proc_name = "soc_ppd_mpls_term_cos_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_MPLS_TERM_COS_INFO_clear(&prm_term_cos_info);
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_ppd_mpls_term_cos_info_get(
          unit,
          &prm_term_cos_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_mpls_term_cos_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_mpls_term_cos_info_get");   
    goto exit; 
  } 

  SOC_PPC_MPLS_TERM_COS_INFO_print(&prm_term_cos_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: label_to_cos_info_set (section mpls_term)
 */
int 
  ui_ppd_api_mpls_term_label_to_cos_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_MPLS_TERM_LABEL_COS_KEY   
    prm_cos_key;
  SOC_PPC_MPLS_TERM_LABEL_COS_VAL   
    prm_cos_val;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_mpls_term"); 
  soc_sand_proc_name = "soc_ppd_mpls_term_label_to_cos_info_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_MPLS_TERM_LABEL_COS_KEY_clear(&prm_cos_key);
  SOC_PPC_MPLS_TERM_LABEL_COS_VAL_clear(&prm_cos_val);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_LABEL_TO_COS_INFO_SET_LABEL_TO_COS_INFO_SET_COS_KEY_MODEL_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_MPLS_TERM_LABEL_TO_COS_INFO_SET_LABEL_TO_COS_INFO_SET_COS_KEY_MODEL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_cos_key.model = param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_LABEL_TO_COS_INFO_SET_LABEL_TO_COS_INFO_SET_COS_KEY_IN_EXP_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MPLS_TERM_LABEL_TO_COS_INFO_SET_LABEL_TO_COS_INFO_SET_COS_KEY_IN_EXP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_cos_key.in_exp = (uint8)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter cos_key after label_to_cos_info_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_mpls_term_label_to_cos_info_get(
          unit,
          &prm_cos_key,
          &prm_cos_val
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_mpls_term_label_to_cos_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_mpls_term_label_to_cos_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_LABEL_TO_COS_INFO_SET_LABEL_TO_COS_INFO_SET_COS_VAL_OVERWRITE_DP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MPLS_TERM_LABEL_TO_COS_INFO_SET_LABEL_TO_COS_INFO_SET_COS_VAL_OVERWRITE_DP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_cos_val.overwrite_dp = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_LABEL_TO_COS_INFO_SET_LABEL_TO_COS_INFO_SET_COS_VAL_DP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MPLS_TERM_LABEL_TO_COS_INFO_SET_LABEL_TO_COS_INFO_SET_COS_VAL_DP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_cos_val.dp = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_LABEL_TO_COS_INFO_SET_LABEL_TO_COS_INFO_SET_COS_VAL_OVERWRITE_TC_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MPLS_TERM_LABEL_TO_COS_INFO_SET_LABEL_TO_COS_INFO_SET_COS_VAL_OVERWRITE_TC_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_cos_val.overwrite_tc = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_LABEL_TO_COS_INFO_SET_LABEL_TO_COS_INFO_SET_COS_VAL_TC_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MPLS_TERM_LABEL_TO_COS_INFO_SET_LABEL_TO_COS_INFO_SET_COS_VAL_TC_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_cos_val.tc = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_mpls_term_label_to_cos_info_set(
          unit,
          &prm_cos_key,
          &prm_cos_val
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_mpls_term_label_to_cos_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_mpls_term_label_to_cos_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: label_to_cos_info_get (section mpls_term)
 */
int 
  ui_ppd_api_mpls_term_label_to_cos_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_MPLS_TERM_LABEL_COS_KEY   
    prm_cos_key;
  SOC_PPC_MPLS_TERM_LABEL_COS_VAL   
    prm_cos_val;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_mpls_term"); 
  soc_sand_proc_name = "soc_ppd_mpls_term_label_to_cos_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_MPLS_TERM_LABEL_COS_KEY_clear(&prm_cos_key);
  SOC_PPC_MPLS_TERM_LABEL_COS_VAL_clear(&prm_cos_val);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_LABEL_TO_COS_INFO_GET_LABEL_TO_COS_INFO_GET_COS_KEY_MODEL_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_MPLS_TERM_LABEL_TO_COS_INFO_GET_LABEL_TO_COS_INFO_GET_COS_KEY_MODEL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_cos_key.model = param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_LABEL_TO_COS_INFO_GET_LABEL_TO_COS_INFO_GET_COS_KEY_IN_EXP_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MPLS_TERM_LABEL_TO_COS_INFO_GET_LABEL_TO_COS_INFO_GET_COS_KEY_IN_EXP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_cos_key.in_exp = (uint8)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter cos_key after label_to_cos_info_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_mpls_term_label_to_cos_info_get(
          unit,
          &prm_cos_key,
          &prm_cos_val
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_mpls_term_label_to_cos_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_mpls_term_label_to_cos_info_get");   
    goto exit; 
  } 

  SOC_PPC_MPLS_TERM_LABEL_COS_VAL_print(&prm_cos_val);

  
  goto exit; 
exit: 
  return ui_ret; 
} 

/******************************************************************** 
 *  Function handler: label_to_cos_info_set (section mpls_term)
 */
int 
  ui_ppd_api_mpls_term_profile_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32
	prm_term_profile_ndx = 0,
    ret;   
  SOC_PPC_MPLS_TERM_PROFILE_INFO  
    prm_term_profile_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_mpls_term"); 
  soc_sand_proc_name = "soc_ppd_mpls_term_profile_info_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_MPLS_TERM_PROFILE_INFO_clear(&prm_term_profile_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_PROFILE_INFO_SET_PROFILE_INFO_SET_PROFILE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MPLS_TERM_PROFILE_INFO_SET_PROFILE_INFO_SET_PROFILE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_term_profile_ndx = (uint32)param_val->value.ulong_value;
  }
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter term_profile_ndx after profile_info_get***", TRUE); 
    goto exit; 
  }  

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_mpls_term_profile_info_get(
          unit,
          prm_term_profile_ndx,
          &prm_term_profile_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_mpls_term_profile_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_mpls_term_profile_info_set");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_PROFILE_INFO_SET_PROFILE_INFO_SET_PROFILE_INFO_NOF_HEADERS_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MPLS_TERM_PROFILE_INFO_SET_PROFILE_INFO_SET_PROFILE_INFO_NOF_HEADERS_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_term_profile_info.nof_headers = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_PROFILE_INFO_SET_PROFILE_INFO_SET_PROFILE_INFO_TTL_EXP_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MPLS_TERM_PROFILE_INFO_SET_PROFILE_INFO_SET_PROFILE_INFO_TTL_EXP_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_term_profile_info.ttl_exp_index = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_PROFILE_INFO_SET_PROFILE_INFO_SET_PROFILE_INFO_DISCARD_TTL_0_ID,1))          
  {  
	UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MPLS_TERM_PROFILE_INFO_SET_PROFILE_INFO_SET_PROFILE_INFO_DISCARD_TTL_0_ID);  
	UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
	prm_term_profile_info.flags |= (uint8)param_val->value.ulong_value ? SOC_PPC_MPLS_TERM_DISCARD_TTL_0 : 0;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_PROFILE_INFO_SET_PROFILE_INFO_SET_PROFILE_INFO_DISCARD_TTL_1_ID,1))          
  {  
	UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MPLS_TERM_PROFILE_INFO_SET_PROFILE_INFO_SET_PROFILE_INFO_DISCARD_TTL_1_ID);  
	UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
	prm_term_profile_info.flags |= (uint8)param_val->value.ulong_value ? SOC_PPC_MPLS_TERM_DISCARD_TTL_1 : 0;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_PROFILE_INFO_SET_PROFILE_INFO_SET_PROFILE_INFO_HAS_CW_ID,1))          
  {  
	UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MPLS_TERM_PROFILE_INFO_SET_PROFILE_INFO_SET_PROFILE_INFO_HAS_CW_ID);  
	UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
	prm_term_profile_info.flags |= (uint8)param_val->value.ulong_value ? SOC_PPC_MPLS_TERM_HAS_CW : 0;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_PROFILE_INFO_SET_PROFILE_INFO_SET_PROFILE_INFO_SKIP_ETH_ID,1))          
  {  
	UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MPLS_TERM_PROFILE_INFO_SET_PROFILE_INFO_SET_PROFILE_INFO_SKIP_ETH_ID);  
	UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
	prm_term_profile_info.flags |= (uint8)param_val->value.ulong_value ? SOC_PPC_MPLS_TERM_SKIP_ETH : 0;
  } 

  /* Call function */
  ret = soc_ppd_mpls_term_profile_info_set(
          unit,
          prm_term_profile_ndx,
          &prm_term_profile_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_mpls_term_profile_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_mpls_term_profile_info_set");   
    goto exit; 
  } 
  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: label_to_cos_info_get (section mpls_term)
 */
int 
  ui_ppd_api_mpls_term_profile_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32
	prm_term_profile_ndx = 0,
    ret;   
  SOC_PPC_MPLS_TERM_PROFILE_INFO  
    prm_term_profile_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_mpls_term"); 
  soc_sand_proc_name = "soc_ppd_mpls_term_profile_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_MPLS_TERM_PROFILE_INFO_clear(&prm_term_profile_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_PROFILE_INFO_GET_PROFILE_INFO_GET_PROFILE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_MPLS_TERM_PROFILE_INFO_GET_PROFILE_INFO_GET_PROFILE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_term_profile_ndx = (uint32)param_val->value.ulong_value;
  }
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter term_profile_ndx after profile_info_get***", TRUE); 
    goto exit; 
  }  

  /* Call function */
  ret = soc_ppd_mpls_term_profile_info_get(
          unit,
          prm_term_profile_ndx,
          &prm_term_profile_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_mpls_term_profile_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_mpls_term_profile_info_get");   
    goto exit; 
  } 

  SOC_PPC_MPLS_TERM_PROFILE_INFO_print(&prm_term_profile_info);
  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
#endif
#ifdef UI_MPLS_TERM/* { mpls_term*/
/******************************************************************** 
 *  Section handler: mpls_term
 */ 
int 
  ui_ppd_api_mpls_term( 
    CURRENT_LINE *current_line 
  ) 
{   
  int
    ret = FALSE;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_mpls_term"); 
 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_LKUP_INFO_SET_LKUP_INFO_SET_ID,1)) 
  { 
    ret = ui_ppd_api_mpls_term_lkup_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_LKUP_INFO_GET_LKUP_INFO_GET_ID,1)) 
  { 
    ret = ui_ppd_api_mpls_term_lkup_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_LABEL_RANGE_SET_LABEL_RANGE_SET_ID,1)) 
  { 
    ret = ui_ppd_api_mpls_term_label_range_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_LABEL_RANGE_GET_LABEL_RANGE_GET_ID,1)) 
  { 
    ret = ui_ppd_api_mpls_term_label_range_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_RANGE_TERMINATED_LABEL_SET_RANGE_TERMINATED_LABEL_SET_ID,1)) 
  { 
    ret = ui_ppd_api_mpls_term_range_terminated_label_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_RANGE_TERMINATED_LABEL_GET_RANGE_TERMINATED_LABEL_GET_ID,1)) 
  { 
    ret = ui_ppd_api_mpls_term_range_terminated_label_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_SET_RESERVED_LABELS_GLOBAL_INFO_SET_ID,1)) 
  { 
    ret = ui_ppd_api_mpls_term_reserved_labels_global_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_GET_RESERVED_LABELS_GLOBAL_INFO_GET_ID,1)) 
  { 
    ret = ui_ppd_api_mpls_term_reserved_labels_global_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_RESERVED_LABEL_INFO_SET_RESERVED_LABEL_INFO_SET_ID,1)) 
  { 
    ret = ui_ppd_api_mpls_term_reserved_label_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_RESERVED_LABEL_INFO_GET_RESERVED_LABEL_INFO_GET_ID,1)) 
  { 
    ret = ui_ppd_api_mpls_term_reserved_label_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_ENCOUNTERED_ENTRIES_GET_BLOCK_ENCOUNTERED_ENTRIES_GET_BLOCK_ID,1)) 
  { 
    ret = ui_ppd_api_mpls_term_encountered_entries_get_block(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_COS_INFO_SET_COS_INFO_SET_ID,1)) 
  { 
    ret = ui_ppd_api_mpls_term_cos_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_COS_INFO_GET_COS_INFO_GET_ID,1)) 
  { 
    ret = ui_ppd_api_mpls_term_cos_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_LABEL_TO_COS_INFO_SET_LABEL_TO_COS_INFO_SET_ID,1)) 
  { 
    ret = ui_ppd_api_mpls_term_label_to_cos_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_MPLS_TERM_LABEL_TO_COS_INFO_GET_LABEL_TO_COS_INFO_GET_ID,1)) 
  { 
    ret = ui_ppd_api_mpls_term_label_to_cos_info_get(current_line); 
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting function name after mpls_term***", TRUE); 
  } 
  
  ui_ret = ret;
    
  goto exit; 
exit:        
  return ui_ret; 
}                
          
#endif /* mpls_term */ 


#endif /* LINK_PPD_LIBRARIES */ 

