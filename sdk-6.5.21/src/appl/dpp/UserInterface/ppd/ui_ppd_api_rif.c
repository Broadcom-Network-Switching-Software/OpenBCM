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
 
  
#include <soc/dpp/PPD/ppd_api_rif.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_rif.h>

#if LINK_PPD_LIBRARIES

#ifdef UI_RIF
/******************************************************************** 
 *  Function handler: mpls_labels_range_set (section rif)
 */
int 
  ui_ppd_api_rif_mpls_labels_range_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_RIF_MPLS_LABELS_RANGE   
    prm_rif_labels_range;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_rif"); 
  soc_sand_proc_name = "soc_ppd_rif_mpls_labels_range_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_RIF_MPLS_LABELS_RANGE_clear(&prm_rif_labels_range);
 
  /* Get parameters */ 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_rif_mpls_labels_range_get(
          unit,
          &prm_rif_labels_range
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_rif_mpls_labels_range_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_rif_mpls_labels_range_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_MPLS_LABELS_RANGE_SET_MPLS_LABELS_RANGE_SET_RIF_LABELS_RANGE_LAST_LABEL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_RIF_MPLS_LABELS_RANGE_SET_MPLS_LABELS_RANGE_SET_RIF_LABELS_RANGE_LAST_LABEL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rif_labels_range.last_label = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_MPLS_LABELS_RANGE_SET_MPLS_LABELS_RANGE_SET_RIF_LABELS_RANGE_FIRST_LABEL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_RIF_MPLS_LABELS_RANGE_SET_MPLS_LABELS_RANGE_SET_RIF_LABELS_RANGE_FIRST_LABEL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rif_labels_range.first_label = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_rif_mpls_labels_range_set(
          unit,
          &prm_rif_labels_range
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_rif_mpls_labels_range_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_rif_mpls_labels_range_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: mpls_labels_range_get (section rif)
 */
int 
  ui_ppd_api_rif_mpls_labels_range_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_RIF_MPLS_LABELS_RANGE   
    prm_rif_labels_range;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_rif"); 
  soc_sand_proc_name = "soc_ppd_rif_mpls_labels_range_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_RIF_MPLS_LABELS_RANGE_clear(&prm_rif_labels_range);
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_ppd_rif_mpls_labels_range_get(
          unit,
          &prm_rif_labels_range
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_rif_mpls_labels_range_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_rif_mpls_labels_range_get");   
    goto exit; 
  } 

  SOC_PPC_RIF_MPLS_LABELS_RANGE_print(&prm_rif_labels_range);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: mpls_label_map_add (section rif)
 */
int 
  ui_ppd_api_rif_mpls_label_map_add(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_MPLS_LABEL_RIF_KEY   
    prm_mpls_key;
  SOC_PPC_LIF_ID   
    prm_lif_index=0;
  SOC_PPC_MPLS_TERM_INFO   
    prm_term_info;
  SOC_PPC_RIF_INFO   
    prm_rif_info;
  SOC_SAND_SUCCESS_FAILURE   
    prm_success;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_rif"); 
  soc_sand_proc_name = "soc_ppd_rif_mpls_label_map_add"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_MPLS_LABEL_RIF_KEY_clear(&prm_mpls_key);
  SOC_PPC_MPLS_TERM_INFO_clear(&prm_term_info);
  SOC_PPC_RIF_INFO_clear(&prm_rif_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_ADD_MPLS_LABEL_MAP_ADD_MPLS_KEY_VSID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_ADD_MPLS_LABEL_MAP_ADD_MPLS_KEY_VSID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_mpls_key.vsid = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_ADD_MPLS_LABEL_MAP_ADD_MPLS_KEY_LABEL_ID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_ADD_MPLS_LABEL_MAP_ADD_MPLS_KEY_LABEL_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_mpls_key.label_id = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter mpls_key after mpls_label_map_add***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_ADD_MPLS_LABEL_MAP_ADD_MPLS_KEY_LABEL_ID_SECOND_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_ADD_MPLS_LABEL_MAP_ADD_MPLS_KEY_LABEL_ID_SECOND_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mpls_key.label_id_second = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_ADD_MPLS_LABEL_MAP_ADD_MPLS_KEY_LABEL_ID_FLAGS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_ADD_MPLS_LABEL_MAP_ADD_MPLS_KEY_LABEL_ID_FLAGS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mpls_key.flags = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_ADD_MPLS_LABEL_MAP_ADD_MPLS_KEY_LABEL_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_ADD_MPLS_LABEL_MAP_ADD_MPLS_KEY_LABEL_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mpls_key.label_index = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_ADD_MPLS_LABEL_MAP_ADD_LIF_INDEX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_ADD_MPLS_LABEL_MAP_ADD_LIF_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lif_index = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_ADD_MPLS_LABEL_MAP_ADD_TERM_INFO_NEXT_PRTCL_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_ADD_MPLS_LABEL_MAP_ADD_TERM_INFO_NEXT_PRTCL_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_term_info.next_prtcl = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_ADD_MPLS_LABEL_MAP_ADD_TERM_INFO_COS_PROFILE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_ADD_MPLS_LABEL_MAP_ADD_TERM_INFO_COS_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_term_info.cos_profile = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_ADD_MPLS_LABEL_MAP_ADD_TERM_INFO_RIF_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_ADD_MPLS_LABEL_MAP_ADD_TERM_INFO_RIF_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_term_info.rif = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_ADD_MPLS_LABEL_MAP_ADD_TERM_INFO_PROCESSING_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_ADD_MPLS_LABEL_MAP_ADD_TERM_INFO_PROCESSING_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_term_info.processing_type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_ADD_MPLS_LABEL_MAP_ADD_RIF_INFO_ROUTING_ENABLERS_BM_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_ADD_MPLS_LABEL_MAP_ADD_RIF_INFO_ROUTING_ENABLERS_BM_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rif_info.routing_enablers_bm = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_ADD_MPLS_LABEL_MAP_ADD_RIF_INFO_TTL_SCOPE_INDEX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_ADD_MPLS_LABEL_MAP_ADD_RIF_INFO_TTL_SCOPE_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rif_info.ttl_scope_index = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_ADD_MPLS_LABEL_MAP_ADD_RIF_INFO_COS_PROFILE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_ADD_MPLS_LABEL_MAP_ADD_RIF_INFO_COS_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rif_info.cos_profile = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_ADD_MPLS_LABEL_MAP_ADD_RIF_INFO_VRF_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_ADD_MPLS_LABEL_MAP_ADD_RIF_INFO_VRF_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rif_info.vrf_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_ADD_MPLS_LABEL_MAP_ADD_RIF_INFO_UC_RPF_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_ADD_MPLS_LABEL_MAP_ADD_RIF_INFO_UC_RPF_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rif_info.uc_rpf_enable = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_rif_mpls_label_map_add(
          unit,
          &prm_mpls_key,
          prm_lif_index,
          &prm_term_info,
          &prm_rif_info,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_rif_mpls_label_map_add - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_rif_mpls_label_map_add");   
    goto exit; 
  } 

  cli_out("success: %s\n\r",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: mpls_label_map_remove (section rif)
 */
int 
  ui_ppd_api_rif_mpls_label_map_remove(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_MPLS_LABEL_RIF_KEY   
    prm_mpls_key;
  SOC_PPC_LIF_ID   
    prm_lif_index;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_rif"); 
  soc_sand_proc_name = "soc_ppd_rif_mpls_label_map_remove"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_MPLS_LABEL_RIF_KEY_clear(&prm_mpls_key);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_REMOVE_MPLS_LABEL_MAP_REMOVE_MPLS_KEY_VSID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_REMOVE_MPLS_LABEL_MAP_REMOVE_MPLS_KEY_VSID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_mpls_key.vsid = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_REMOVE_MPLS_LABEL_MAP_REMOVE_MPLS_KEY_LABEL_ID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_REMOVE_MPLS_LABEL_MAP_REMOVE_MPLS_KEY_LABEL_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_mpls_key.label_id = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter mpls_key after mpls_label_map_remove***", TRUE); 
    goto exit; 
  } 
  
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_REMOVE_MPLS_LABEL_MAP_REMOVE_MPLS_KEY_LABEL_ID_SECOND_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_REMOVE_MPLS_LABEL_MAP_REMOVE_MPLS_KEY_LABEL_ID_SECOND_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mpls_key.label_id_second = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_REMOVE_MPLS_LABEL_MAP_REMOVE_MPLS_KEY_LABEL_ID_FLAGS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_REMOVE_MPLS_LABEL_MAP_REMOVE_MPLS_KEY_LABEL_ID_FLAGS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mpls_key.flags = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_REMOVE_MPLS_LABEL_MAP_REMOVE_MPLS_KEY_LABEL_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_REMOVE_MPLS_LABEL_MAP_REMOVE_MPLS_KEY_LABEL_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mpls_key.label_index = (uint32)param_val->value.ulong_value;
  }



  /* Call function */
  ret = soc_ppd_rif_mpls_label_map_remove(
          unit,
          &prm_mpls_key,
          &prm_lif_index
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_rif_mpls_label_map_remove - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_rif_mpls_label_map_remove");   
    goto exit; 
  } 

  cli_out("lif_index: %u\n\r",prm_lif_index);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: mpls_label_map_get (section rif)
 */
int 
  ui_ppd_api_rif_mpls_label_map_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_MPLS_LABEL_RIF_KEY   
    prm_mpls_key;
  SOC_PPC_LIF_ID   
    prm_lif_index;
  SOC_PPC_MPLS_TERM_INFO   
    prm_term_info;
  SOC_PPC_RIF_INFO   
    prm_rif_info;
  uint8   
    prm_found;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_rif"); 
  soc_sand_proc_name = "soc_ppd_rif_mpls_label_map_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_MPLS_LABEL_RIF_KEY_clear(&prm_mpls_key);
  SOC_PPC_MPLS_TERM_INFO_clear(&prm_term_info);
  SOC_PPC_RIF_INFO_clear(&prm_rif_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_GET_MPLS_LABEL_MAP_GET_MPLS_KEY_VSID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_GET_MPLS_LABEL_MAP_GET_MPLS_KEY_VSID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_mpls_key.vsid = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_GET_MPLS_LABEL_MAP_GET_MPLS_KEY_LABEL_ID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_GET_MPLS_LABEL_MAP_GET_MPLS_KEY_LABEL_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_mpls_key.label_id = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter mpls_key after mpls_label_map_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_GET_MPLS_LABEL_MAP_GET_MPLS_KEY_LABEL_ID_SECOND_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_GET_MPLS_LABEL_MAP_GET_MPLS_KEY_LABEL_ID_SECOND_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mpls_key.label_id_second = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_GET_MPLS_LABEL_MAP_GET_MPLS_KEY_LABEL_ID_FLAGS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_GET_MPLS_LABEL_MAP_GET_MPLS_KEY_LABEL_ID_FLAGS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mpls_key.flags = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_GET_MPLS_LABEL_MAP_GET_MPLS_KEY_LABEL_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_GET_MPLS_LABEL_MAP_GET_MPLS_KEY_LABEL_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mpls_key.label_index = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = soc_ppd_rif_mpls_label_map_get(
          unit,
          &prm_mpls_key,
          &prm_lif_index,
          &prm_term_info,
          &prm_rif_info,
          &prm_found
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_rif_mpls_label_map_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_rif_mpls_label_map_get");   
    goto exit; 
  } 

  cli_out("lif_index: %u\n\r",prm_lif_index);

  SOC_PPC_MPLS_TERM_INFO_print(&prm_term_info);

  SOC_PPC_RIF_INFO_print(&prm_rif_info);

  cli_out("found: %u\n\r",prm_found);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: vsid_map_set (section rif)
 */
int 
  ui_ppd_api_rif_vsid_map_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_VSI_ID   
    prm_vsid_ndx;
  SOC_PPC_RIF_INFO   
    prm_rif_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_rif"); 
  soc_sand_proc_name = "soc_ppd_rif_vsid_map_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_RIF_INFO_clear(&prm_rif_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_VSID_MAP_SET_VSID_MAP_SET_VSID_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_RIF_VSID_MAP_SET_VSID_MAP_SET_VSID_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_vsid_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter vsid_ndx after vsid_map_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_rif_vsid_map_get(
          unit,
          prm_vsid_ndx,
          &prm_rif_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_rif_vsid_map_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_rif_vsid_map_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_VSID_MAP_SET_VSID_MAP_SET_RIF_INFO_ROUTING_ENABLERS_BM_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_RIF_VSID_MAP_SET_VSID_MAP_SET_RIF_INFO_ROUTING_ENABLERS_BM_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rif_info.routing_enablers_bm = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_VSID_MAP_SET_VSID_MAP_SET_RIF_INFO_TTL_SCOPE_INDEX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_RIF_VSID_MAP_SET_VSID_MAP_SET_RIF_INFO_TTL_SCOPE_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rif_info.ttl_scope_index = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_VSID_MAP_SET_VSID_MAP_SET_RIF_INFO_COS_PROFILE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_RIF_VSID_MAP_SET_VSID_MAP_SET_RIF_INFO_COS_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rif_info.cos_profile = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_VSID_MAP_SET_VSID_MAP_SET_RIF_INFO_VRF_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_RIF_VSID_MAP_SET_VSID_MAP_SET_RIF_INFO_VRF_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rif_info.vrf_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_VSID_MAP_SET_VSID_MAP_SET_RIF_INFO_UC_RPF_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_RIF_VSID_MAP_SET_VSID_MAP_SET_RIF_INFO_UC_RPF_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rif_info.uc_rpf_enable = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_rif_vsid_map_set(
          unit,
          prm_vsid_ndx,
          &prm_rif_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_rif_vsid_map_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_rif_vsid_map_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: vsid_map_get (section rif)
 */
int 
  ui_ppd_api_rif_vsid_map_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_VSI_ID   
    prm_vsid_ndx;
  SOC_PPC_RIF_INFO   
    prm_rif_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_rif"); 
  soc_sand_proc_name = "soc_ppd_rif_vsid_map_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_RIF_INFO_clear(&prm_rif_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_VSID_MAP_GET_VSID_MAP_GET_VSID_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_RIF_VSID_MAP_GET_VSID_MAP_GET_VSID_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_vsid_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter vsid_ndx after vsid_map_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_rif_vsid_map_get(
          unit,
          prm_vsid_ndx,
          &prm_rif_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_rif_vsid_map_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_rif_vsid_map_get");   
    goto exit; 
  } 

  SOC_PPC_RIF_INFO_print(&prm_rif_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: info_set (section rif)
 */
int 
  ui_ppd_api_rif_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_RIF_ID   
    prm_rif_ndx;
  SOC_PPC_RIF_INFO   
    prm_rif_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_rif"); 
  soc_sand_proc_name = "soc_ppd_rif_info_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_RIF_INFO_clear(&prm_rif_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_INFO_SET_INFO_SET_RIF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_RIF_INFO_SET_INFO_SET_RIF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_rif_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter rif_ndx after info_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_rif_info_get(
          unit,
          prm_rif_ndx,
          &prm_rif_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_rif_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_rif_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_INFO_SET_INFO_SET_RIF_INFO_ROUTING_ENABLERS_BM_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_RIF_INFO_SET_INFO_SET_RIF_INFO_ROUTING_ENABLERS_BM_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rif_info.routing_enablers_bm = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_INFO_SET_INFO_SET_RIF_INFO_TTL_SCOPE_INDEX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_RIF_INFO_SET_INFO_SET_RIF_INFO_TTL_SCOPE_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rif_info.ttl_scope_index = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_INFO_SET_INFO_SET_RIF_INFO_COS_PROFILE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_RIF_INFO_SET_INFO_SET_RIF_INFO_COS_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rif_info.cos_profile = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_INFO_SET_INFO_SET_RIF_INFO_VRF_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_RIF_INFO_SET_INFO_SET_RIF_INFO_VRF_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rif_info.vrf_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_INFO_SET_INFO_SET_RIF_INFO_UC_RPF_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_RIF_INFO_SET_INFO_SET_RIF_INFO_UC_RPF_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rif_info.uc_rpf_enable = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_INFO_SET_INFO_SET_RIF_INFO_DEFAULT_ROUTING_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_RIF_INFO_SET_INFO_SET_RIF_INFO_DEFAULT_ROUTING_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rif_info.enable_default_routing = (uint8)param_val->value.ulong_value;
  } 

  /* Call function */
  ret = soc_ppd_rif_info_set(
          unit,
          prm_rif_ndx,
          &prm_rif_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_rif_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_rif_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: info_get (section rif)
 */
int 
  ui_ppd_api_rif_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_RIF_ID   
    prm_rif_ndx;
  SOC_PPC_RIF_INFO   
    prm_rif_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_rif"); 
  soc_sand_proc_name = "soc_ppd_rif_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_RIF_INFO_clear(&prm_rif_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_INFO_GET_INFO_GET_RIF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_RIF_INFO_GET_INFO_GET_RIF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_rif_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter rif_ndx after info_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_rif_info_get(
          unit,
          prm_rif_ndx,
          &prm_rif_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_rif_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_rif_info_get");   
    goto exit; 
  } 

  SOC_PPC_RIF_INFO_print(&prm_rif_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: ttl_scope_set (section rif)
 */
int 
  ui_ppd_api_rif_ttl_scope_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_ttl_scope_ndx;
  SOC_SAND_PP_IP_TTL   
    prm_ttl_val;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_rif"); 
  soc_sand_proc_name = "soc_ppd_rif_ttl_scope_set"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_TTL_SCOPE_SET_TTL_SCOPE_SET_TTL_SCOPE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_RIF_TTL_SCOPE_SET_TTL_SCOPE_SET_TTL_SCOPE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_ttl_scope_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter ttl_scope_ndx after ttl_scope_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_rif_ttl_scope_get(
          unit,
          prm_ttl_scope_ndx,
          &prm_ttl_val
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_rif_ttl_scope_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_rif_ttl_scope_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_TTL_SCOPE_SET_TTL_SCOPE_SET_TTL_VAL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_RIF_TTL_SCOPE_SET_TTL_SCOPE_SET_TTL_VAL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_ttl_val = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_rif_ttl_scope_set(
          unit,
          prm_ttl_scope_ndx,
          prm_ttl_val
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_rif_ttl_scope_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_rif_ttl_scope_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: ttl_scope_get (section rif)
 */
int 
  ui_ppd_api_rif_ttl_scope_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_ttl_scope_ndx;
  SOC_SAND_PP_IP_TTL   
    prm_ttl_val;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_rif"); 
  soc_sand_proc_name = "soc_ppd_rif_ttl_scope_get"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_TTL_SCOPE_GET_TTL_SCOPE_GET_TTL_SCOPE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_RIF_TTL_SCOPE_GET_TTL_SCOPE_GET_TTL_SCOPE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_ttl_scope_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter ttl_scope_ndx after ttl_scope_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_rif_ttl_scope_get(
          unit,
          prm_ttl_scope_ndx,
          &prm_ttl_val
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_rif_ttl_scope_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_rif_ttl_scope_get");   
    goto exit; 
  } 

  cli_out("ttl_val: %u\n\r",prm_ttl_val);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
#endif
#ifdef UI_RIF/* { rif*/
/******************************************************************** 
 *  Section handler: rif
 */ 
int 
  ui_ppd_api_rif( 
    CURRENT_LINE *current_line 
  ) 
{   
  int
    ret = FALSE;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_rif"); 
 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_MPLS_LABELS_RANGE_SET_MPLS_LABELS_RANGE_SET_ID,1)) 
  { 
    ret = ui_ppd_api_rif_mpls_labels_range_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_MPLS_LABELS_RANGE_GET_MPLS_LABELS_RANGE_GET_ID,1)) 
  { 
    ret = ui_ppd_api_rif_mpls_labels_range_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_ADD_MPLS_LABEL_MAP_ADD_ID,1)) 
  { 
    ret = ui_ppd_api_rif_mpls_label_map_add(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_REMOVE_MPLS_LABEL_MAP_REMOVE_ID,1)) 
  { 
    ret = ui_ppd_api_rif_mpls_label_map_remove(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_MPLS_LABEL_MAP_GET_MPLS_LABEL_MAP_GET_ID,1)) 
  { 
    ret = ui_ppd_api_rif_mpls_label_map_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_VSID_MAP_SET_VSID_MAP_SET_ID,1)) 
  { 
    ret = ui_ppd_api_rif_vsid_map_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_VSID_MAP_GET_VSID_MAP_GET_ID,1)) 
  { 
    ret = ui_ppd_api_rif_vsid_map_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_INFO_SET_INFO_SET_ID,1)) 
  { 
    ret = ui_ppd_api_rif_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_INFO_GET_INFO_GET_ID,1)) 
  { 
    ret = ui_ppd_api_rif_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_TTL_SCOPE_SET_TTL_SCOPE_SET_ID,1)) 
  { 
    ret = ui_ppd_api_rif_ttl_scope_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_RIF_TTL_SCOPE_GET_TTL_SCOPE_GET_ID,1)) 
  { 
    ret = ui_ppd_api_rif_ttl_scope_get(current_line); 
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting function name after rif***", TRUE); 
  }
  
  ui_ret = ret; 
    
  goto exit; 
exit:        
  return ui_ret; 
}                
          
#endif /* rif */ 


#endif /* LINK_PPD_LIBRARIES */ 

