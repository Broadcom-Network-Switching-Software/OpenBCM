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
 


#include <soc/dpp/PPD/ppd_api_eg_encap.h>                                                               
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_eg_encap.h>

#if LINK_PPD_LIBRARIES

#ifdef UI_EG_ENCAP
/********************************************************************
*  Function handler: range_info_set (section eg_encap)
 */
int
ui_ppd_api_eg_encap_range_info_set(
                                   CURRENT_LINE *current_line
                                   )
{
  uint32
    bank_ndx,
    ret;
  SOC_PPC_EG_ENCAP_ACCESS_PHASE
    access_phase;
  SOC_PPC_EG_ENCAP_RANGE_INFO
    prm_range_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_encap");
  soc_sand_proc_name = "soc_ppd_eg_encap_range_info_set";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_EG_ENCAP_RANGE_INFO_clear(&prm_range_info);
  bank_ndx = SOC_PPC_EG_ENCAP_NOF_BANKS(unit);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_ppd_eg_encap_range_info_get(
    unit,
    &prm_range_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_encap_range_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_encap_range_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_RANGE_INFO_SET_RANGE_INFO_SET_RANGE_INFO_IP_TNL_LIMIT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_RANGE_INFO_SET_RANGE_INFO_SET_RANGE_INFO_IP_TNL_LIMIT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_info.ip_tnl_limit = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_RANGE_INFO_SET_RANGE_INFO_SET_RANGE_INFO_LL_LIMIT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_RANGE_INFO_SET_RANGE_INFO_SET_RANGE_INFO_LL_LIMIT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_info.ll_limit = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_RANGE_INFO_SET_RANGE_INFO_SET_RANGE_INFO_BANK_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_RANGE_INFO_SET_RANGE_INFO_SET_RANGE_INFO_BANK_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    bank_ndx = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_RANGE_INFO_SET_RANGE_INFO_SET_RANGE_INFO_ACCESS_PHASE_ID,1)) 
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_EG_ENCAP_RANGE_INFO_SET_RANGE_INFO_SET_RANGE_INFO_ACCESS_PHASE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    access_phase = param_val->numeric_equivalent;
    if(bank_ndx >  SOC_PPC_EG_ENCAP_BANK_NDX_MAX(unit))
    {
        send_string_to_screen("Bank index out of range. Range: 0-15", TRUE);
        goto exit;
    }
    prm_range_info.bank_access_phase[bank_ndx] = access_phase;
  }

  /* Call function */
  ret = soc_ppd_eg_encap_range_info_set(
    unit,
    &prm_range_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_encap_range_info_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_encap_range_info_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: range_info_get (section eg_encap)
 */
int
ui_ppd_api_eg_encap_range_info_get(
                                   CURRENT_LINE *current_line
                                   )
{
  uint32
    ret;
  SOC_PPC_EG_ENCAP_RANGE_INFO
    prm_range_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_encap");
  soc_sand_proc_name = "soc_ppd_eg_encap_range_info_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_EG_ENCAP_RANGE_INFO_clear(&prm_range_info);

  /* Get parameters */

  /* Call function */
  ret = soc_ppd_eg_encap_range_info_get(
    unit,
    &prm_range_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_encap_range_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_encap_range_info_get");
    goto exit;
  }

  send_string_to_screen("--> range_info:", TRUE);
  SOC_PPC_EG_ENCAP_RANGE_INFO_print(&prm_range_info);

  goto exit;
exit:
  return ui_ret;
}
/******************************************************************** 
*  Function handler: null_lif_entry_add (section eg_encap)
 */
int 
ui_ppd_api_eg_encap_null_lif_entry_add(
                                       CURRENT_LINE *current_line 
                                       ) 
{   
  uint32 
    ret;   
  uint32   
    prm_lif_eep_ndx;
  uint32   
    prm_next_eep = 0;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_encap"); 
  soc_sand_proc_name = "soc_ppd_eg_encap_null_lif_entry_add"; 

  unit = soc_ppd_get_default_unit(); 

  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_NULL_LIF_ENTRY_ADD_NULL_LIF_ENTRY_ADD_LIF_EEP_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_NULL_LIF_ENTRY_ADD_NULL_LIF_ENTRY_ADD_LIF_EEP_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_lif_eep_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter lif_eep_ndx after null_lif_entry_add***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_NULL_LIF_ENTRY_ADD_NULL_LIF_ENTRY_ADD_NEXT_EEP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_NULL_LIF_ENTRY_ADD_NULL_LIF_ENTRY_ADD_NEXT_EEP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_next_eep = (uint32)param_val->value.ulong_value;
  } 

  /* Call function */  
  ret = soc_ppd_eg_encap_null_lif_entry_add(
    unit,
    prm_lif_eep_ndx,
    prm_next_eep
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_eg_encap_null_lif_entry_add - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_encap_null_lif_entry_add");   
    goto exit; 
  } 


  goto exit; 
exit: 
  return ui_ret; 
} 


/********************************************************************
*  Function handler: swap_command_entry_add (section eg_encap)
 */
int
ui_ppd_api_eg_encap_swap_command_entry_add(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_lif_eep_ndx;
  SOC_PPC_EG_ENCAP_SWAP_INFO
    prm_swap_info;
  uint32
    prm_next_eep = 0;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_encap");
  soc_sand_proc_name = "soc_ppd_eg_encap_swap_command_entry_add";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_EG_ENCAP_SWAP_INFO_clear(&prm_swap_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_SWAP_COMMAND_ENTRY_ADD_SWAP_COMMAND_ENTRY_ADD_LIF_EEP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_SWAP_COMMAND_ENTRY_ADD_SWAP_COMMAND_ENTRY_ADD_LIF_EEP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_lif_eep_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter lif_eep_ndx after swap_command_entry_add***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_SWAP_COMMAND_ENTRY_ADD_SWAP_COMMAND_ENTRY_ADD_SWAP_INFO_SWAP_LABEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_SWAP_COMMAND_ENTRY_ADD_SWAP_COMMAND_ENTRY_ADD_SWAP_INFO_SWAP_LABEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_swap_info.swap_label = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_SWAP_COMMAND_ENTRY_ADD_SWAP_COMMAND_ENTRY_ADD_NEXT_EEP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_SWAP_COMMAND_ENTRY_ADD_SWAP_COMMAND_ENTRY_ADD_NEXT_EEP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_next_eep = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_SWAP_COMMAND_ENTRY_ADD_SWAP_COMMAND_ENTRY_ADD_OUT_VSI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_SWAP_COMMAND_ENTRY_ADD_SWAP_COMMAND_ENTRY_ADD_OUT_VSI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_swap_info.out_vsi = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_SWAP_COMMAND_ENTRY_ADD_SWAP_COMMAND_ENTRY_ADD_OAM_LIF_SET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_SWAP_COMMAND_ENTRY_ADD_SWAP_COMMAND_ENTRY_ADD_OAM_LIF_SET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_swap_info.oam_lif_set = (uint32)param_val->value.ulong_value;
  }
  /* Call function */
  ret = soc_ppd_eg_encap_swap_command_entry_add(
    unit,
    prm_lif_eep_ndx,
    &prm_swap_info,
    prm_next_eep
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_encap_swap_command_entry_add - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_encap_swap_command_entry_add");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: pwe_entry_add (section eg_encap)
 */
int
ui_ppd_api_eg_encap_pwe_entry_add(
                                  CURRENT_LINE *current_line
                                  )
{
  uint32
    ret;
  uint32
    prm_lif_eep_ndx;
  SOC_PPC_EG_ENCAP_PWE_INFO
    prm_pwe_info;
  uint32
    prm_next_eep = 0;
  uint32
    idx,
    nof_mpls_reps = 1,
    nof_reps = 1;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_encap");
  soc_sand_proc_name = "soc_ppd_eg_encap_pwe_entry_add";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_EG_ENCAP_PWE_INFO_clear(&prm_pwe_info);

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_PWE_ENTRY_ADD_PWE_ENTRY_ADD_NOF_REPS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_PWE_ENTRY_ADD_PWE_ENTRY_ADD_NOF_REPS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    nof_reps = (uint32)param_val->value.ulong_value;
  }

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_PWE_ENTRY_ADD_PWE_ENTRY_ADD_LIF_EEP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_PWE_ENTRY_ADD_PWE_ENTRY_ADD_LIF_EEP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_lif_eep_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter lif_eep_ndx after pwe_entry_add***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_PWE_ENTRY_ADD_PWE_ENTRY_ADD_PWE_INFO_PUSH_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_PWE_ENTRY_ADD_PWE_ENTRY_ADD_PWE_INFO_PUSH_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pwe_info.push_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_PWE_ENTRY_ADD_PWE_ENTRY_ADD_PWE_INFO_LABEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_PWE_ENTRY_ADD_PWE_ENTRY_ADD_PWE_INFO_LABEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pwe_info.label = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_PWE_ENTRY_ADD_PWE_ENTRY_ADD_PWE_INFO_OAM_LIF_SET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_PWE_ENTRY_ADD_PWE_ENTRY_ADD_PWE_INFO_OAM_LIF_SET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pwe_info.oam_lif_set = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_PWE_ENTRY_ADD_PWE_ENTRY_ADD_NEXT_EEP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_PWE_ENTRY_ADD_PWE_ENTRY_ADD_NEXT_EEP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_next_eep = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_PWE_ENTRY_ADD_PWE_ENTRY_ADD_NEXT_EEP_NOF_REPS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_PWE_ENTRY_ADD_PWE_ENTRY_ADD_NEXT_EEP_NOF_REPS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    nof_mpls_reps = (uint32)param_val->value.ulong_value;
  }

  for (idx = 0; idx < nof_reps; ++idx)
  {
  /* Call function */
  ret = soc_ppd_eg_encap_pwe_entry_add(
    unit,
    prm_lif_eep_ndx,
    &prm_pwe_info,
            prm_next_eep + idx % nof_mpls_reps
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_encap_pwe_entry_add - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_encap_pwe_entry_add");
    goto exit;
  }
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: pop_command_entry_add (section eg_encap)
 */
int
ui_ppd_api_eg_encap_pop_command_entry_add(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_lif_eep_ndx;
  SOC_PPC_EG_ENCAP_POP_INFO
    prm_pop_info;
  uint32
    prm_next_eep=0;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_encap");
  soc_sand_proc_name = "soc_ppd_eg_encap_pop_command_entry_add";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_EG_ENCAP_POP_INFO_clear(&prm_pop_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_POP_COMMAND_ENTRY_ADD_POP_COMMAND_ENTRY_ADD_LIF_EEP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_POP_COMMAND_ENTRY_ADD_POP_COMMAND_ENTRY_ADD_LIF_EEP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_lif_eep_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter lif_eep_ndx after pop_command_entry_add***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_POP_COMMAND_ENTRY_ADD_POP_COMMAND_ENTRY_ADD_POP_INFO_ETHERNET_INFO_HAS_CW_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_POP_COMMAND_ENTRY_ADD_POP_COMMAND_ENTRY_ADD_POP_INFO_ETHERNET_INFO_HAS_CW_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pop_info.ethernet_info.has_cw = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_POP_COMMAND_ENTRY_ADD_POP_COMMAND_ENTRY_ADD_POP_INFO_ETHERNET_INFO_TPID_PROFILE_TPID1_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_POP_COMMAND_ENTRY_ADD_POP_COMMAND_ENTRY_ADD_POP_INFO_ETHERNET_INFO_TPID_PROFILE_TPID1_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pop_info.ethernet_info.tpid_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_POP_COMMAND_ENTRY_ADD_POP_COMMAND_ENTRY_ADD_POP_INFO_ETHERNET_INFO_SYS_VSI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_POP_COMMAND_ENTRY_ADD_POP_COMMAND_ENTRY_ADD_POP_INFO_ETHERNET_INFO_SYS_VSI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pop_info.out_vsi = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_POP_COMMAND_ENTRY_ADD_POP_COMMAND_ENTRY_ADD_POP_INFO_POP_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_EG_ENCAP_POP_COMMAND_ENTRY_ADD_POP_COMMAND_ENTRY_ADD_POP_INFO_POP_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pop_info.pop_type = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_POP_COMMAND_ENTRY_ADD_POP_COMMAND_ENTRY_ADD_POP_INFO_POP_NEXT_HDR_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_EG_ENCAP_POP_COMMAND_ENTRY_ADD_POP_COMMAND_ENTRY_ADD_POP_INFO_POP_NEXT_HDR_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pop_info.pop_next_header = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_POP_COMMAND_ENTRY_ADD_POP_COMMAND_ENTRY_ADD_POP_INFO_MODEL_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_EG_ENCAP_POP_COMMAND_ENTRY_ADD_POP_COMMAND_ENTRY_ADD_POP_INFO_MODEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pop_info.model = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_POP_COMMAND_ENTRY_ADD_POP_COMMAND_ENTRY_ADD_POP_INFO_POP_TYPE_ETHERNET_ID,1))
  {
    prm_pop_info.pop_type = SOC_PPC_MPLS_COMMAND_TYPE_POP_INTO_ETHERNET;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_POP_COMMAND_ENTRY_ADD_POP_COMMAND_ENTRY_ADD_NEXT_EEP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_POP_COMMAND_ENTRY_ADD_POP_COMMAND_ENTRY_ADD_NEXT_EEP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_next_eep = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_POP_COMMAND_ENTRY_ADD_POP_COMMAND_ENTRY_ADD_POP_INFO_OAM_LIF_SET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_POP_COMMAND_ENTRY_ADD_POP_COMMAND_ENTRY_ADD_POP_INFO_OAM_LIF_SET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pop_info.oam_lif_set = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_eg_encap_pop_command_entry_add(
    unit,
    prm_lif_eep_ndx,
    &prm_pop_info,
    prm_next_eep
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_encap_pop_command_entry_add - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_encap_pop_command_entry_add");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: vsi_entry_add (section eg_encap)
 */
int
ui_ppd_api_eg_encap_vsi_entry_add(
                                  CURRENT_LINE *current_line
                                  )
{
  uint32
    ret;
  uint32
    prm_lif_eep_ndx;
  SOC_PPC_EG_ENCAP_VSI_ENCAP_INFO
      prm_vsi_encap_info;
  uint32
    prm_next_eep = 0;
  uint8
    prm_next_eep_valid = 0; 

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_encap");
  soc_sand_proc_name = "soc_ppd_eg_encap_vsi_entry_add";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_EG_ENCAP_VSI_ENCAP_INFO_clear(&prm_vsi_encap_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_VSI_ENTRY_ADD_VSI_ENTRY_ADD_LIF_EEP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_VSI_ENTRY_ADD_VSI_ENTRY_ADD_LIF_EEP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_lif_eep_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter lif_eep_ndx after vsi_entry_add***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_VSI_ENTRY_ADD_VSI_ENTRY_ADD_OUT_VSI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_VSI_ENTRY_ADD_VSI_ENTRY_ADD_OUT_VSI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsi_encap_info.out_vsi = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_VSI_ENTRY_ADD_VSI_ENTRY_ADD_REMARK_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_VSI_ENTRY_ADD_VSI_ENTRY_ADD_REMARK_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsi_encap_info.remark_profile = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_VSI_ENTRY_ADD_VSI_ENTRY_ADD_OAM_LIF_SET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_VSI_ENTRY_ADD_VSI_ENTRY_ADD_OAM_LIF_SET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsi_encap_info.oam_lif_set = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_VSI_ENTRY_ADD_VSI_ENTRY_ADD_NEXT_EEP_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_VSI_ENTRY_ADD_VSI_ENTRY_ADD_NEXT_EEP_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_next_eep_valid = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_VSI_ENTRY_ADD_VSI_ENTRY_ADD_NEXT_EEP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_VSI_ENTRY_ADD_VSI_ENTRY_ADD_NEXT_EEP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_next_eep = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_eg_encap_vsi_entry_add(
    unit,
    prm_lif_eep_ndx,
    &prm_vsi_encap_info,
    prm_next_eep_valid,
    prm_next_eep
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_encap_vsi_entry_add - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_encap_vsi_entry_add");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: mpls_entry_add (section eg_encap)
 */
int
ui_ppd_api_eg_encap_mpls_entry_add(
                                   CURRENT_LINE *current_line
                                   )
{
  uint32
    ret;
  uint32
    prm_tunnel_eep_ndx;
  SOC_PPC_EG_ENCAP_MPLS_ENCAP_INFO
    prm_mpls_encap_info;
  uint32
    idx,
    nof_ll_reps = 1,
    nof_reps = 1;
  uint32
    prm_ll_eep = 0;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_encap");
  soc_sand_proc_name = "soc_ppd_eg_encap_mpls_entry_add";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_EG_ENCAP_MPLS_ENCAP_INFO_clear(&prm_mpls_encap_info);

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_MPLS_ENTRY_ADD_MPLS_ENTRY_ADD_NOF_REPS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_MPLS_ENTRY_ADD_MPLS_ENTRY_ADD_NOF_REPS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    nof_reps = (uint32)param_val->value.ulong_value;
  }

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_MPLS_ENTRY_ADD_MPLS_ENTRY_ADD_TUNNEL_EEP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_MPLS_ENTRY_ADD_MPLS_ENTRY_ADD_TUNNEL_EEP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tunnel_eep_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter tunnel_eep_ndx after mpls_entry_add***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_MPLS_ENTRY_ADD_MPLS_ENTRY_ADD_MPLS_ENCAP_INFO_OUT_VSI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_MPLS_ENTRY_ADD_MPLS_ENTRY_ADD_MPLS_ENCAP_INFO_OUT_VSI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mpls_encap_info.out_vsi = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_MPLS_ENTRY_ADD_MPLS_ENTRY_ADD_MPLS_ENCAP_INFO_ORIENTATION_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_EG_ENCAP_MPLS_ENTRY_ADD_MPLS_ENTRY_ADD_MPLS_ENCAP_INFO_ORIENTATION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mpls_encap_info.orientation = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_MPLS_ENTRY_ADD_MPLS_ENTRY_ADD_MPLS_ENCAP_INFO_TUNNELS1_PUSH_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_MPLS_ENTRY_ADD_MPLS_ENTRY_ADD_MPLS_ENCAP_INFO_TUNNELS1_PUSH_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mpls_encap_info.tunnels[0].push_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_MPLS_ENTRY_ADD_MPLS_ENTRY_ADD_MPLS_ENCAP_INFO_TUNNELS1_TUNNEL_LABEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_MPLS_ENTRY_ADD_MPLS_ENTRY_ADD_MPLS_ENCAP_INFO_TUNNELS1_TUNNEL_LABEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mpls_encap_info.tunnels[0].tunnel_label = (uint32)param_val->value.ulong_value;
    prm_mpls_encap_info.nof_tunnels = 1;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_MPLS_ENTRY_ADD_MPLS_ENTRY_ADD_MPLS_ENCAP_INFO_TUNNELS2_PUSH_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_MPLS_ENTRY_ADD_MPLS_ENTRY_ADD_MPLS_ENCAP_INFO_TUNNELS2_PUSH_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mpls_encap_info.tunnels[1].push_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_MPLS_ENTRY_ADD_MPLS_ENTRY_ADD_MPLS_ENCAP_INFO_TUNNELS2_TUNNEL_LABEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_MPLS_ENTRY_ADD_MPLS_ENTRY_ADD_MPLS_ENCAP_INFO_TUNNELS2_TUNNEL_LABEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mpls_encap_info.tunnels[1].tunnel_label = (uint32)param_val->value.ulong_value;
    prm_mpls_encap_info.nof_tunnels = 2;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_MPLS_ENTRY_ADD_MPLS_ENTRY_ADD_OAM_LIF_SET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_MPLS_ENTRY_ADD_MPLS_ENTRY_ADD_OAM_LIF_SET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mpls_encap_info.oam_lif_set = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_MPLS_ENTRY_ADD_MPLS_ENTRY_ADD_LL_EEP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_MPLS_ENTRY_ADD_MPLS_ENTRY_ADD_LL_EEP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ll_eep = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_MPLS_ENTRY_ADD_MPLS_ENTRY_ADD_LL_EEP_NOF_REPS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_MPLS_ENTRY_ADD_MPLS_ENTRY_ADD_LL_EEP_NOF_REPS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    nof_ll_reps = (uint32)param_val->value.ulong_value;
  }

  for (idx = 0; idx < nof_reps; ++idx)
  {
  /* Call function */
  ret = soc_ppd_eg_encap_mpls_entry_add(
    unit,
    prm_tunnel_eep_ndx,
    &prm_mpls_encap_info,
            prm_ll_eep + idx % nof_ll_reps
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_encap_mpls_entry_add - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_encap_mpls_entry_add");
    goto exit;
  }
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: ipv4_entry_add (section eg_encap)
 */
int
ui_ppd_api_eg_encap_ipv4_entry_add(
                                   CURRENT_LINE *current_line
                                   )
{
  uint32
    ret;
  uint32
    prm_tunnel_eep_ndx;
  SOC_PPC_EG_ENCAP_IPV4_ENCAP_INFO
    prm_ipv4_encap_info;
  uint32
    prm_ll_eep = 0;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_encap");
  soc_sand_proc_name = "soc_ppd_eg_encap_ipv4_entry_add";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_EG_ENCAP_IPV4_ENCAP_INFO_clear(&prm_ipv4_encap_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_IPV4_ENTRY_ADD_IPV4_ENTRY_ADD_TUNNEL_EEP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_IPV4_ENTRY_ADD_IPV4_ENTRY_ADD_TUNNEL_EEP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tunnel_eep_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter tunnel_eep_ndx after ipv4_entry_add***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_IPV4_ENTRY_ADD_IPV4_ENTRY_ADD_IPV4_ENCAP_INFO_OUT_VSI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_IPV4_ENTRY_ADD_IPV4_ENTRY_ADD_IPV4_ENCAP_INFO_OUT_VSI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ipv4_encap_info.out_vsi = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_IPV4_ENTRY_ADD_IPV4_ENTRY_ADD_IPV4_ENCAP_INFO_DEST_ENABLE_GRE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_IPV4_ENTRY_ADD_IPV4_ENTRY_ADD_IPV4_ENCAP_INFO_DEST_ENABLE_GRE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ipv4_encap_info.dest.enable_gre = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_IPV4_ENTRY_ADD_IPV4_ENTRY_ADD_IPV4_ENCAP_INFO_DEST_TOS_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_IPV4_ENTRY_ADD_IPV4_ENTRY_ADD_IPV4_ENCAP_INFO_DEST_TOS_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ipv4_encap_info.dest.tos_index = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_IPV4_ENTRY_ADD_IPV4_ENTRY_ADD_IPV4_ENCAP_INFO_DEST_TTL_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_IPV4_ENTRY_ADD_IPV4_ENTRY_ADD_IPV4_ENCAP_INFO_DEST_TTL_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ipv4_encap_info.dest.ttl_index = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_IPV4_ENTRY_ADD_IPV4_ENTRY_ADD_IPV4_ENCAP_INFO_DEST_SRC_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_IPV4_ENTRY_ADD_IPV4_ENTRY_ADD_IPV4_ENCAP_INFO_DEST_SRC_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ipv4_encap_info.dest.src_index = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_IPV4_ENTRY_ADD_IPV4_ENTRY_ADD_IPV4_ENCAP_INFO_DEST_OAM_LIF_SET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_IPV4_ENTRY_ADD_IPV4_ENTRY_ADD_IPV4_ENCAP_INFO_DEST_OAM_LIF_SET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ipv4_encap_info.oam_lif_set = (uint32)param_val->value.ulong_value;

  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_IPV4_ENTRY_ADD_IPV4_ENTRY_ADD_IPV4_ENCAP_INFO_DEST_ENCAPSULATION_MODE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_EG_ENCAP_IPV4_ENTRY_ADD_IPV4_ENTRY_ADD_IPV4_ENCAP_INFO_DEST_ENCAPSULATION_MODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ipv4_encap_info.dest.encapsulation_mode = param_val->numeric_equivalent;
  }

  if (!get_val_of(
    current_line,(int *)&match_index,SOC_PARAM_PPD_EG_ENCAP_IPV4_ENTRY_ADD_IPV4_ENTRY_ADD_IPV4_ENCAP_INFO_DEST_DEST_ID,1,  
    &param_val,VAL_IP,err_msg))                                                                
  {                                                                                                  
    prm_ipv4_encap_info.dest.dest = param_val->value.ip_value;
  } 


  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_IPV4_ENTRY_ADD_IPV4_ENTRY_ADD_LL_EEP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_IPV4_ENTRY_ADD_IPV4_ENTRY_ADD_LL_EEP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ll_eep = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_eg_encap_ipv4_entry_add(
    unit,
    prm_tunnel_eep_ndx,
    &prm_ipv4_encap_info,
    prm_ll_eep
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_encap_ipv4_entry_add - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_encap_ipv4_entry_add");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: ll_entry_add (section eg_encap)
 */
int
ui_ppd_api_eg_encap_ll_entry_add(
                                 CURRENT_LINE *current_line
                                 )
{
  uint32
    ret;
  uint32
      prm_ll_eep_ndx;
  SOC_PPC_EG_ENCAP_LL_INFO
    prm_ll_encap_info;
  uint32
    nof_entries=1,
    indx;
  uint32
    inc_mac=0;
  
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_encap");
  soc_sand_proc_name = "soc_ppd_eg_encap_ll_entry_add";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_EG_ENCAP_LL_INFO_clear(&prm_ll_encap_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_LL_ENTRY_ADD_LL_ENTRY_ADD_LL_EEP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_LL_ENTRY_ADD_LL_ENTRY_ADD_LL_EEP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ll_eep_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ll_eep_ndx after ll_entry_add***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_LL_ENTRY_ADD_LL_ENTRY_ADD_LL_ENCAP_INFO_TPID_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_LL_ENTRY_ADD_LL_ENTRY_ADD_LL_ENCAP_INFO_TPID_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ll_encap_info.tpid_index = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_LL_ENTRY_ADD_LL_ENTRY_ADD_LL_ENCAP_INFO_PCP_DEI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_LL_ENTRY_ADD_LL_ENTRY_ADD_LL_ENCAP_INFO_PCP_DEI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ll_encap_info.pcp_dei = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_LL_ENTRY_ADD_LL_ENTRY_ADD_LL_ENCAP_INFO_PCP_DEI_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_LL_ENTRY_ADD_LL_ENTRY_ADD_LL_ENCAP_INFO_PCP_DEI_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ll_encap_info.pcp_dei_valid = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_LL_ENTRY_ADD_LL_ENTRY_ADD_LL_ENCAP_INFO_OUT_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_LL_ENTRY_ADD_LL_ENTRY_ADD_LL_ENCAP_INFO_OUT_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ll_encap_info.out_vid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_LL_ENTRY_ADD_LL_ENTRY_ADD_LL_ENCAP_INFO_OUT_VID_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_LL_ENTRY_ADD_LL_ENTRY_ADD_LL_ENCAP_INFO_OUT_VID_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ll_encap_info.out_vid_valid = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_LL_ENTRY_ADD_LL_ENTRY_ADD_LL_ENCAP_INFO_LL_REMARK_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_LL_ENTRY_ADD_LL_ENTRY_ADD_LL_ENCAP_INFO_LL_REMARK_PROFILE_ID);
    prm_ll_encap_info.ll_remark_profile = (uint8)param_val->value.ulong_value;
  }

  if (!get_val_of(
    current_line,(int *)&match_index,SOC_PARAM_PPD_EG_ENCAP_LL_ENTRY_ADD_LL_ENTRY_ADD_LL_ENCAP_INFO_DEST_MAC_ID,1,
    &param_val,VAL_TEXT,err_msg))

  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_ll_encap_info.dest_mac));
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_LL_ENTRY_ADD_LL_ENTRY_ADD_NOF_ENTRIES_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_LL_ENTRY_ADD_LL_ENTRY_ADD_NOF_ENTRIES_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    nof_entries = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_LL_ENTRY_ADD_LL_ENTRY_ADD_INC_DA_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_LL_ENTRY_ADD_LL_ENTRY_ADD_INC_DA_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    inc_mac = (uint32)param_val->value.ulong_value;
  }
 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_LL_ENTRY_ADD_LL_ENTRY_ADD_LL_ENCAP_INFO_OUT_AC_VALID_ID, 1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_LL_ENTRY_ADD_LL_ENTRY_ADD_LL_ENCAP_INFO_OUT_AC_VALID_ID);
    prm_ll_encap_info.out_ac_valid = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_LL_ENTRY_ADD_LL_ENTRY_ADD_LL_ENCAP_INFO_OUT_AC_LSB_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_LL_ENTRY_ADD_LL_ENTRY_ADD_LL_ENCAP_INFO_OUT_AC_LSB_ID);
    prm_ll_encap_info.out_ac_lsb = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_LL_ENTRY_ADD_LL_ENTRY_ADD_LL_ENCAP_INFO_OUT_OAM_LIF_SET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_LL_ENTRY_ADD_LL_ENTRY_ADD_LL_ENCAP_INFO_OUT_OAM_LIF_SET_ID);
    prm_ll_encap_info.oam_lif_set = (uint32)param_val->value.ulong_value;
  }

  for (indx = 0; indx < nof_entries; ++ indx)
  {
    /* Call function */
    ret = soc_ppd_eg_encap_ll_entry_add(
      unit,
      prm_ll_eep_ndx+indx,
      &prm_ll_encap_info
      );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** soc_ppd_eg_encap_ll_entry_add - FAIL", TRUE);
      soc_sand_disp_result_proc(ret, "soc_ppd_eg_encap_ll_entry_add");
      goto exit;
    }
    if (inc_mac)
    {
      soc_sand_pp_mac_address_inc(&(prm_ll_encap_info.dest_mac));
    }
  }

  goto exit;
exit:
  return ui_ret;
}

/******************************************************************** 
*  Function handler: entry_remove (section eg_encap)
 */
int
ui_ppd_api_eg_encap_entry_remove(
                                 CURRENT_LINE *current_line
                                 )
{
  uint32
    ret;
  SOC_PPC_EG_ENCAP_EEP_TYPE
    prm_eep_type_ndx;
  uint32
    prm_eep_ndx;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_encap");
  soc_sand_proc_name = "soc_ppd_eg_encap_entry_remove";

  unit = soc_ppd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_ENTRY_REMOVE_ENTRY_REMOVE_EEP_TYPE_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_EG_ENCAP_ENTRY_REMOVE_ENTRY_REMOVE_EEP_TYPE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_eep_type_ndx = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter eep_type_ndx after entry_remove***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_ENTRY_REMOVE_ENTRY_REMOVE_EEP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_ENTRY_REMOVE_ENTRY_REMOVE_EEP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_eep_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter eep_ndx after entry_remove***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_eg_encap_entry_remove(
    unit,
    prm_eep_type_ndx,
    prm_eep_ndx
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_encap_entry_remove - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_encap_entry_remove");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: entry_get (section eg_encap)
 */
int
ui_ppd_api_eg_encap_entry_get(
                              CURRENT_LINE *current_line
                              )
{
  uint32
    ret;
  SOC_PPC_EG_ENCAP_EEP_TYPE
    prm_eep_type_ndx;
  uint32
    prm_eep_ndx,
    cur_eep;
  uint32
    prm_depth;
  SOC_PPC_EG_ENCAP_ENTRY_INFO
    prm_encap_entry_info[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX];
  uint32
    prm_next_eep[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX];
  uint32
    prm_nof_entries;
  uint32
    indx;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_encap");
  soc_sand_proc_name = "soc_ppd_eg_encap_entry_get";

  unit = soc_ppd_get_default_unit();

  for (indx = 0; indx < SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX; ++indx)
  {
    SOC_PPC_EG_ENCAP_ENTRY_INFO_clear(&(prm_encap_entry_info[indx]));
  }

  /* defaults */
  prm_depth =3;
  prm_eep_type_ndx = SOC_PPC_EG_ENCAP_EEP_TYPE_LIF_EEP;

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_ENTRY_GET_ENTRY_GET_EEP_TYPE_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_EG_ENCAP_ENTRY_GET_ENTRY_GET_EEP_TYPE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_eep_type_ndx = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_ENTRY_GET_ENTRY_GET_EEP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_ENTRY_GET_ENTRY_GET_EEP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_eep_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter eep_ndx after entry_get***", TRUE);
    goto exit;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_ENTRY_GET_ENTRY_GET_DEPTH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_ENTRY_GET_ENTRY_GET_DEPTH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_depth = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_eg_encap_entry_get(
    unit,
    prm_eep_type_ndx,
    prm_eep_ndx,
    prm_depth,
    prm_encap_entry_info,
    prm_next_eep,
    &prm_nof_entries
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_encap_entry_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_encap_entry_get");
    goto exit;
  }

  cur_eep = prm_eep_ndx;
  for (indx = 0; indx < prm_nof_entries; ++indx)
  {
    cli_out("Encap Pointer: %u \n\r", cur_eep);
    SOC_PPC_EG_ENCAP_ENTRY_INFO_print(unit, &(prm_encap_entry_info[indx]));
    cur_eep = prm_next_eep[indx];
    cli_out("\n\r");
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: push_profile_info_set (section eg_encap)
 */
int
ui_ppd_api_eg_encap_push_profile_info_set(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_profile_ndx;
  SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO
    prm_profile_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_encap");
  soc_sand_proc_name = "soc_ppd_eg_encap_push_profile_info_set";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO_clear(&prm_profile_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_PUSH_PROFILE_INFO_SET_PUSH_PROFILE_INFO_SET_PROFILE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_PUSH_PROFILE_INFO_SET_PUSH_PROFILE_INFO_SET_PROFILE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_profile_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter profile_ndx after push_profile_info_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = soc_ppd_eg_encap_push_profile_info_get(
    unit,
    prm_profile_ndx,
    &prm_profile_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_encap_push_profile_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_encap_push_profile_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_PUSH_PROFILE_INFO_SET_PUSH_PROFILE_INFO_SET_PROFILE_INFO_EXP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_PUSH_PROFILE_INFO_SET_PUSH_PROFILE_INFO_SET_PROFILE_INFO_EXP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_profile_info.exp = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_PUSH_PROFILE_INFO_SET_PUSH_PROFILE_INFO_SET_PROFILE_INFO_OVERWRITE_EXP_ID,1))
  {
    prm_profile_info.exp_mark_mode = SOC_PPC_EG_ENCAP_EXP_MARK_MODE_FROM_PUSH_PROFILE;
  }
  else
  {
    prm_profile_info.exp_mark_mode = SOC_PPC_EG_ENCAP_EXP_MARK_MODE_MAP_TC_DP;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_PUSH_PROFILE_INFO_SET_PUSH_PROFILE_INFO_SET_PROFILE_INFO_TTL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_PUSH_PROFILE_INFO_SET_PUSH_PROFILE_INFO_SET_PROFILE_INFO_TTL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_profile_info.ttl = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_PUSH_PROFILE_INFO_SET_PUSH_PROFILE_INFO_SET_PROFILE_INFO_HAS_CW_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_PUSH_PROFILE_INFO_SET_PUSH_PROFILE_INFO_SET_PROFILE_INFO_HAS_CW_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_profile_info.has_cw = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_PUSH_PROFILE_INFO_SET_PUSH_PROFILE_INFO_SET_PROFILE_INFO_MODEL_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_EG_ENCAP_PUSH_PROFILE_INFO_SET_PUSH_PROFILE_INFO_SET_PROFILE_INFO_MODEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_profile_info.model = param_val->numeric_equivalent;
  }

  /* Call function */
  ret = soc_ppd_eg_encap_push_profile_info_set(
    unit,
    prm_profile_ndx,
    &prm_profile_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_encap_push_profile_info_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_encap_push_profile_info_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: push_profile_info_get (section eg_encap)
 */
int
ui_ppd_api_eg_encap_push_profile_info_get(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_profile_ndx;
  SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO
    prm_profile_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_encap");
  soc_sand_proc_name = "soc_ppd_eg_encap_push_profile_info_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO_clear(&prm_profile_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_PUSH_PROFILE_INFO_GET_PUSH_PROFILE_INFO_GET_PROFILE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_PUSH_PROFILE_INFO_GET_PUSH_PROFILE_INFO_GET_PROFILE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_profile_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter profile_ndx after push_profile_info_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_eg_encap_push_profile_info_get(
    unit,
    prm_profile_ndx,
    &prm_profile_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_encap_push_profile_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_encap_push_profile_info_get");
    goto exit;
  }

  send_string_to_screen("--> profile_info:", TRUE);
  SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO_print(&prm_profile_info);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: push_exp_info_set (section eg_encap)
 */
int
ui_ppd_api_eg_encap_push_exp_info_set(
                                      CURRENT_LINE *current_line
                                      )
{
  uint32
    ret;
  SOC_PPC_EG_ENCAP_PUSH_EXP_KEY
    prm_exp_key;
  SOC_SAND_PP_MPLS_EXP   
    prm_exp;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_encap");
  soc_sand_proc_name = "soc_ppd_eg_encap_push_exp_info_set";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_EG_ENCAP_PUSH_EXP_KEY_clear(&prm_exp_key);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_PUSH_EXP_INFO_SET_PUSH_EXP_INFO_SET_EXP_KEY_DP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_PUSH_EXP_INFO_SET_PUSH_EXP_INFO_SET_EXP_KEY_DP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_exp_key.dp = (uint8)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_PUSH_EXP_INFO_SET_PUSH_EXP_INFO_SET_EXP_KEY_TC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_PUSH_EXP_INFO_SET_PUSH_EXP_INFO_SET_EXP_KEY_TC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_exp_key.tc = (uint8)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_PUSH_EXP_INFO_SET_PUSH_EXP_INFO_SET_EXP_KEY_PUSH_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_PUSH_EXP_INFO_SET_PUSH_EXP_INFO_SET_EXP_KEY_PUSH_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_exp_key.push_profile = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter exp_key after push_exp_info_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = soc_ppd_eg_encap_push_exp_info_get(
    unit,
    &prm_exp_key,
    &prm_exp
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_encap_push_exp_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_encap_push_exp_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_PUSH_EXP_INFO_SET_PUSH_EXP_INFO_SET_EXP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_PUSH_EXP_INFO_SET_PUSH_EXP_INFO_SET_EXP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_exp = (uint8)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_eg_encap_push_exp_info_set(
    unit,
    &prm_exp_key,
    prm_exp
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_encap_push_exp_info_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_encap_push_exp_info_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: push_exp_info_get (section eg_encap)
 */
int
ui_ppd_api_eg_encap_push_exp_info_get(
                                      CURRENT_LINE *current_line
                                      )
{
  uint32
    ret;
  SOC_PPC_EG_ENCAP_PUSH_EXP_KEY
    prm_exp_key;
  SOC_SAND_PP_MPLS_EXP   
    prm_exp;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_encap");
  soc_sand_proc_name = "soc_ppd_eg_encap_push_exp_info_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_EG_ENCAP_PUSH_EXP_KEY_clear(&prm_exp_key);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_PUSH_EXP_INFO_GET_PUSH_EXP_INFO_GET_EXP_KEY_DP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_PUSH_EXP_INFO_GET_PUSH_EXP_INFO_GET_EXP_KEY_DP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_exp_key.dp = (uint8)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_PUSH_EXP_INFO_GET_PUSH_EXP_INFO_GET_EXP_KEY_TC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_PUSH_EXP_INFO_GET_PUSH_EXP_INFO_GET_EXP_KEY_TC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_exp_key.tc = (uint8)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_PUSH_EXP_INFO_GET_PUSH_EXP_INFO_GET_EXP_KEY_PUSH_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_PUSH_EXP_INFO_GET_PUSH_EXP_INFO_GET_EXP_KEY_PUSH_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_exp_key.push_profile = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter exp_key after push_exp_info_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_eg_encap_push_exp_info_get(
    unit,
    &prm_exp_key,
    &prm_exp
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_encap_push_exp_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_encap_push_exp_info_get");
    goto exit;
  }

  send_string_to_screen("--> exp:", TRUE);
  cli_out("exp: %u\n",prm_exp);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: pwe_glbl_info_set (section eg_encap)
 */
int
ui_ppd_api_eg_encap_pwe_glbl_info_set(
                                      CURRENT_LINE *current_line
                                      )
{
  uint32
    ret;
  SOC_PPC_EG_ENCAP_PWE_GLBL_INFO
    prm_glbl_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_encap");
  soc_sand_proc_name = "soc_ppd_eg_encap_pwe_glbl_info_set";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_EG_ENCAP_PWE_GLBL_INFO_clear(&prm_glbl_info);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_ppd_eg_encap_pwe_glbl_info_get(
    unit,
    &prm_glbl_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_encap_pwe_glbl_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_encap_pwe_glbl_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_PWE_GLBL_INFO_SET_PWE_GLBL_INFO_SET_GLBL_INFO_CW_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_PWE_GLBL_INFO_SET_PWE_GLBL_INFO_SET_GLBL_INFO_CW_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_glbl_info.cw = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_eg_encap_pwe_glbl_info_set(
    unit,
    &prm_glbl_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_encap_pwe_glbl_info_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_encap_pwe_glbl_info_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: pwe_glbl_info_get (section eg_encap)
 */
int
ui_ppd_api_eg_encap_pwe_glbl_info_get(
                                      CURRENT_LINE *current_line
                                      )
{
  uint32
    ret;
  SOC_PPC_EG_ENCAP_PWE_GLBL_INFO
    prm_glbl_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_encap");
  soc_sand_proc_name = "soc_ppd_eg_encap_pwe_glbl_info_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_EG_ENCAP_PWE_GLBL_INFO_clear(&prm_glbl_info);

  /* Get parameters */

  /* Call function */
  ret = soc_ppd_eg_encap_pwe_glbl_info_get(
    unit,
    &prm_glbl_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_encap_pwe_glbl_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_encap_pwe_glbl_info_get");
    goto exit;
  }

  send_string_to_screen("--> glbl_info:", TRUE);
  SOC_PPC_EG_ENCAP_PWE_GLBL_INFO_print(&prm_glbl_info);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: ipv4_tunnel_glbl_src_ip_set (section eg_encap)
 */
int
ui_ppd_api_eg_encap_ipv4_tunnel_glbl_src_ip_set(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  uint32
    prm_src_ip;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_encap");
  soc_sand_proc_name = "soc_ppd_eg_encap_ipv4_tunnel_glbl_src_ip_set";

  unit = soc_ppd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_SET_IPV4_TUNNEL_GLBL_SRC_IP_SET_ENTRY_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_SET_IPV4_TUNNEL_GLBL_SRC_IP_SET_ENTRY_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_ndx after ipv4_tunnel_glbl_src_ip_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = soc_ppd_eg_encap_ipv4_tunnel_glbl_src_ip_get(
    unit,
    prm_entry_ndx,
    &prm_src_ip
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_encap_ipv4_tunnel_glbl_src_ip_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_encap_ipv4_tunnel_glbl_src_ip_get");
    goto exit;
  }

  if (!get_val_of(
    current_line,(int *)&match_index,SOC_PARAM_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_SET_IPV4_TUNNEL_GLBL_SRC_IP_SET_SRC_IP_ID,1,  
    &param_val,VAL_IP,err_msg))                                                                
  {                                                                                                  
    prm_src_ip = param_val->value.ip_value;
  } 

  /* Call function */
  ret = soc_ppd_eg_encap_ipv4_tunnel_glbl_src_ip_set(
    unit,
    prm_entry_ndx,
    prm_src_ip
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_encap_ipv4_tunnel_glbl_src_ip_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_encap_ipv4_tunnel_glbl_src_ip_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: ipv4_tunnel_glbl_src_ip_get (section eg_encap)
 */
int
ui_ppd_api_eg_encap_ipv4_tunnel_glbl_src_ip_get(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  uint32
    prm_src_ip;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_encap");
  soc_sand_proc_name = "soc_ppd_eg_encap_ipv4_tunnel_glbl_src_ip_get";

  unit = soc_ppd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_GET_IPV4_TUNNEL_GLBL_SRC_IP_GET_ENTRY_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_GET_IPV4_TUNNEL_GLBL_SRC_IP_GET_ENTRY_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_ndx after ipv4_tunnel_glbl_src_ip_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_eg_encap_ipv4_tunnel_glbl_src_ip_get(
    unit,
    prm_entry_ndx,
    &prm_src_ip
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_encap_ipv4_tunnel_glbl_src_ip_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_encap_ipv4_tunnel_glbl_src_ip_get");
    goto exit;
  }

  send_string_to_screen("--> src_ip:", TRUE);
  cli_out("src_ip: %u\n",prm_src_ip);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: ipv4_tunnel_glbl_ttl_set (section eg_encap)
 */
int
ui_ppd_api_eg_encap_ipv4_tunnel_glbl_ttl_set(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_SAND_PP_IP_TTL   
    prm_ttl;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_encap");
  soc_sand_proc_name = "soc_ppd_eg_encap_ipv4_tunnel_glbl_ttl_set";

  unit = soc_ppd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_SET_IPV4_TUNNEL_GLBL_TTL_SET_ENTRY_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_SET_IPV4_TUNNEL_GLBL_TTL_SET_ENTRY_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_ndx after ipv4_tunnel_glbl_ttl_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = soc_ppd_eg_encap_ipv4_tunnel_glbl_ttl_get(
    unit,
    prm_entry_ndx,
    &prm_ttl
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_encap_ipv4_tunnel_glbl_ttl_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_encap_ipv4_tunnel_glbl_ttl_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_SET_IPV4_TUNNEL_GLBL_TTL_SET_TTL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_SET_IPV4_TUNNEL_GLBL_TTL_SET_TTL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ttl = (uint8)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_eg_encap_ipv4_tunnel_glbl_ttl_set(
    unit,
    prm_entry_ndx,
    prm_ttl
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_encap_ipv4_tunnel_glbl_ttl_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_encap_ipv4_tunnel_glbl_ttl_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: ipv4_tunnel_glbl_ttl_get (section eg_encap)
 */
int
ui_ppd_api_eg_encap_ipv4_tunnel_glbl_ttl_get(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_SAND_PP_IP_TTL   
    prm_ttl;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_encap");
  soc_sand_proc_name = "soc_ppd_eg_encap_ipv4_tunnel_glbl_ttl_get";

  unit = soc_ppd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_GET_IPV4_TUNNEL_GLBL_TTL_GET_ENTRY_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_GET_IPV4_TUNNEL_GLBL_TTL_GET_ENTRY_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_ndx after ipv4_tunnel_glbl_ttl_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_eg_encap_ipv4_tunnel_glbl_ttl_get(
    unit,
    prm_entry_ndx,
    &prm_ttl
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_encap_ipv4_tunnel_glbl_ttl_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_encap_ipv4_tunnel_glbl_ttl_get");
    goto exit;
  }

  send_string_to_screen("--> ttl:", TRUE);
  cli_out("ttl: %u\n",prm_ttl);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: ipv4_tunnel_glbl_tos_set (section eg_encap)
 */
int
ui_ppd_api_eg_encap_ipv4_tunnel_glbl_tos_set(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_SAND_PP_IPV4_TOS   
    prm_tos;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_encap");
  soc_sand_proc_name = "soc_ppd_eg_encap_ipv4_tunnel_glbl_tos_set";

  unit = soc_ppd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_SET_IPV4_TUNNEL_GLBL_TOS_SET_ENTRY_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_SET_IPV4_TUNNEL_GLBL_TOS_SET_ENTRY_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_ndx after ipv4_tunnel_glbl_tos_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = soc_ppd_eg_encap_ipv4_tunnel_glbl_tos_get(
    unit,
    prm_entry_ndx,
    &prm_tos
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_encap_ipv4_tunnel_glbl_tos_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_encap_ipv4_tunnel_glbl_tos_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_SET_IPV4_TUNNEL_GLBL_TOS_SET_TOS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_SET_IPV4_TUNNEL_GLBL_TOS_SET_TOS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tos = (uint8)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_eg_encap_ipv4_tunnel_glbl_tos_set(
    unit,
    prm_entry_ndx,
    prm_tos
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_encap_ipv4_tunnel_glbl_tos_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_encap_ipv4_tunnel_glbl_tos_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: ipv4_tunnel_glbl_tos_get (section eg_encap)
 */
int
ui_ppd_api_eg_encap_ipv4_tunnel_glbl_tos_get(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_entry_ndx;
  SOC_SAND_PP_IPV4_TOS   
    prm_tos;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_encap");
  soc_sand_proc_name = "soc_ppd_eg_encap_ipv4_tunnel_glbl_tos_get";

  unit = soc_ppd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_GET_IPV4_TUNNEL_GLBL_TOS_GET_ENTRY_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_GET_IPV4_TUNNEL_GLBL_TOS_GET_ENTRY_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_ndx after ipv4_tunnel_glbl_tos_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_eg_encap_ipv4_tunnel_glbl_tos_get(
    unit,
    prm_entry_ndx,
    &prm_tos
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_eg_encap_ipv4_tunnel_glbl_tos_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_encap_ipv4_tunnel_glbl_tos_get");
    goto exit;
  }

  send_string_to_screen("--> tos:", TRUE);
  cli_out("tos: %u\n",prm_tos);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Section handler: eg_encap
 */
int
ui_ppd_api_eg_encap(
                    CURRENT_LINE *current_line
                    )
{
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_encap");

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_RANGE_INFO_SET_RANGE_INFO_SET_ID,1))
  {
    ui_ret = ui_ppd_api_eg_encap_range_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_RANGE_INFO_GET_RANGE_INFO_GET_ID,1))
  {
    ui_ret = ui_ppd_api_eg_encap_range_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_NULL_LIF_ENTRY_ADD_NULL_LIF_ENTRY_ADD_ID,1)) 
  { 
    ui_ret = ui_ppd_api_eg_encap_null_lif_entry_add(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_SWAP_COMMAND_ENTRY_ADD_SWAP_COMMAND_ENTRY_ADD_ID,1))
  {
    ui_ret = ui_ppd_api_eg_encap_swap_command_entry_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_PWE_ENTRY_ADD_PWE_ENTRY_ADD_ID,1))
  {
    ui_ret = ui_ppd_api_eg_encap_pwe_entry_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_POP_COMMAND_ENTRY_ADD_POP_COMMAND_ENTRY_ADD_ID,1))
  {
    ui_ret = ui_ppd_api_eg_encap_pop_command_entry_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_VSI_ENTRY_ADD_VSI_ENTRY_ADD_ID,1))
  {
    ui_ret = ui_ppd_api_eg_encap_vsi_entry_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_MPLS_ENTRY_ADD_MPLS_ENTRY_ADD_ID,1))
  {
    ui_ret = ui_ppd_api_eg_encap_mpls_entry_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_IPV4_ENTRY_ADD_IPV4_ENTRY_ADD_ID,1))
  {
    ui_ret = ui_ppd_api_eg_encap_ipv4_entry_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_LL_ENTRY_ADD_LL_ENTRY_ADD_ID,1))
  {
    ui_ret = ui_ppd_api_eg_encap_ll_entry_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_ENTRY_REMOVE_ENTRY_REMOVE_ID,1))
  {
    ui_ret = ui_ppd_api_eg_encap_entry_remove(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_ENTRY_GET_ENTRY_GET_ID,1))
  {
    ui_ret = ui_ppd_api_eg_encap_entry_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_PUSH_PROFILE_INFO_SET_PUSH_PROFILE_INFO_SET_ID,1))
  {
    ui_ret = ui_ppd_api_eg_encap_push_profile_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_PUSH_PROFILE_INFO_GET_PUSH_PROFILE_INFO_GET_ID,1))
  {
    ui_ret = ui_ppd_api_eg_encap_push_profile_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_PUSH_EXP_INFO_SET_PUSH_EXP_INFO_SET_ID,1))
  {
    ui_ret = ui_ppd_api_eg_encap_push_exp_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_PUSH_EXP_INFO_GET_PUSH_EXP_INFO_GET_ID,1))
  {
    ui_ret = ui_ppd_api_eg_encap_push_exp_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_PWE_GLBL_INFO_SET_PWE_GLBL_INFO_SET_ID,1))
  {
    ui_ret = ui_ppd_api_eg_encap_pwe_glbl_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_PWE_GLBL_INFO_GET_PWE_GLBL_INFO_GET_ID,1))
  {
    ui_ret = ui_ppd_api_eg_encap_pwe_glbl_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_SET_IPV4_TUNNEL_GLBL_SRC_IP_SET_ID,1))
  {
    ui_ret = ui_ppd_api_eg_encap_ipv4_tunnel_glbl_src_ip_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_GET_IPV4_TUNNEL_GLBL_SRC_IP_GET_ID,1))
  {
    ui_ret = ui_ppd_api_eg_encap_ipv4_tunnel_glbl_src_ip_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_SET_IPV4_TUNNEL_GLBL_TTL_SET_ID,1))
  {
    ui_ret = ui_ppd_api_eg_encap_ipv4_tunnel_glbl_ttl_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_GET_IPV4_TUNNEL_GLBL_TTL_GET_ID,1))
  {
    ui_ret = ui_ppd_api_eg_encap_ipv4_tunnel_glbl_ttl_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_SET_IPV4_TUNNEL_GLBL_TOS_SET_ID,1))
  {
    ui_ret = ui_ppd_api_eg_encap_ipv4_tunnel_glbl_tos_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_GET_IPV4_TUNNEL_GLBL_TOS_GET_ID,1))
  {
    ui_ret = ui_ppd_api_eg_encap_ipv4_tunnel_glbl_tos_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after eg_encap***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif
#endif /* LINK_PPD_LIBRARIES */ 

