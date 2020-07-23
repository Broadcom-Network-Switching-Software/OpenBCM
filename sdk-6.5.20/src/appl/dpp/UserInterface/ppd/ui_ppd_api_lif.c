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
 
#include <soc/dpp/PPD/ppd_api_lif.h>                                                               
#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_lif.h>

#ifdef BCM_PETRA_SUPPORT
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/arad_api_general.h>
#endif

#if LINK_PPD_LIBRARIES

#ifdef UI_LIF

static SOC_PPC_L2_LIF_AC_GROUP_INFO
  prm_acs_group_info;

/********************************************************************
*  Function handler: pwe_map_range_set (section lif)
 */
int
ui_ppd_api_lif_pwe_map_range_set(
                                 CURRENT_LINE *current_line
                                 )
{
  uint32
    ret;
  SOC_PPC_L2_LIF_IN_VC_RANGE
    prm_in_vc_range;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif");
  soc_sand_proc_name = "soc_ppd_l2_lif_pwe_map_range_set";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_L2_LIF_IN_VC_RANGE_clear(&prm_in_vc_range);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_ppd_l2_lif_pwe_map_range_get(
    unit,
    &prm_in_vc_range
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_l2_lif_pwe_map_range_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_l2_lif_pwe_map_range_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_PWE_MAP_RANGE_SET_PWE_MAP_RANGE_SET_IN_VC_RANGE_LAST_LABEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_PWE_MAP_RANGE_SET_PWE_MAP_RANGE_SET_IN_VC_RANGE_LAST_LABEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_in_vc_range.last_label = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_PWE_MAP_RANGE_SET_PWE_MAP_RANGE_SET_IN_VC_RANGE_FIRST_LABEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_PWE_MAP_RANGE_SET_PWE_MAP_RANGE_SET_IN_VC_RANGE_FIRST_LABEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_in_vc_range.first_label = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_l2_lif_pwe_map_range_set(
    unit,
    &prm_in_vc_range
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_l2_lif_pwe_map_range_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_l2_lif_pwe_map_range_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: pwe_map_range_get (section lif)
 */
int
ui_ppd_api_lif_pwe_map_range_get(
                                 CURRENT_LINE *current_line
                                 )
{
  uint32
    ret;
  SOC_PPC_L2_LIF_IN_VC_RANGE
    prm_in_vc_range;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif");
  soc_sand_proc_name = "soc_ppd_l2_lif_pwe_map_range_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_L2_LIF_IN_VC_RANGE_clear(&prm_in_vc_range);

  /* Get parameters */

  /* Call function */
  ret = soc_ppd_l2_lif_pwe_map_range_get(
    unit,
    &prm_in_vc_range
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_l2_lif_pwe_map_range_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_l2_lif_pwe_map_range_get");
    goto exit;
  }

  send_string_to_screen("--> in_vc_range:", TRUE);
  SOC_PPC_L2_LIF_IN_VC_RANGE_print(&prm_in_vc_range);

  goto exit;
exit:
  return ui_ret;
}

int
ui_ppd_frwrd_decision_set(
  CURRENT_LINE *current_line,
  uint32 occur,
  SOC_PPC_FRWRD_DECISION_INFO *fwd_decision
)
{
  uint32
    ret;
  uint32
    macro_type=SOC_PPD_FWD_DECISION_UI_DROP_SET;
  SOC_PPC_TRAP_CODE
    trap_code = SOC_PPC_NOF_TRAP_CODES;
  uint32
    dest_id=0,
    frwrd_strength=0,
    snp_strength=0,
    ac_id=0,
    label=0,
    push_profile=0,
    eep_ndx=0,
    offset=0,
    is_lag=0,
	isid_id=0;
  

  UI_MACROS_INIT_FUNCTION("ui_ppd_frwrd_decision_set");
  soc_sand_proc_name = "ui_ppd_frwrd_decision_set";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_FRWRD_DECISION_INFO_clear(fwd_decision);

  if (occur == 2)
  {
    offset += SOC_PARAM_PPD_FRWRD_DECISION_FD2_TRAP_CODE_ID - SOC_PARAM_PPD_FRWRD_DECISION_FD_TRAP_CODE_ID;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_DECISION_FD_DEST_ID+offset,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_DECISION_FD_DEST_ID+offset);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    dest_id = (uint32)param_val->value.ulong_value;
  }
    /*trap-code
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_DECISION_FD_TRAP_CODE_ID+offset,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_DECISION_FD_TRAP_CODE_ID+offset);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    trap_code = param_val->numeric_equivalent;
  }
  */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_DECISION_FD_TRAP_CODE_ID+offset,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_DECISION_FD_TRAP_CODE_ID+offset);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    trap_code = (SOC_PPC_TRAP_CODE)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_DECISION_FD_FWD_STRENGTH_ID+offset,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_DECISION_FD_FWD_STRENGTH_ID+offset);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    frwrd_strength = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_DECISION_FD_SNOOP_STRENGTH_ID+offset,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_DECISION_FD_SNOOP_STRENGTH_ID+offset);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    snp_strength = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_DECISION_FD_AC_ID+offset,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_DECISION_FD_AC_ID+offset);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    ac_id = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_DECISION_FD_IS_LAG_ID+offset,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_DECISION_FD_IS_LAG_ID+offset);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    is_lag = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_DECISION_FD_LABEL_ID+offset,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_DECISION_FD_LABEL_ID+offset);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    label = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_DECISION_FD_PUSH_PROFILE_ID+offset,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_DECISION_FD_PUSH_PROFILE_ID+offset);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    push_profile = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_DECISION_FD_EEP_NDX_ID+offset,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_DECISION_FD_EEP_NDX_ID+offset);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    eep_ndx = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_DECISION_FD_ISID_ID+offset,1))
  {
	  UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_DECISION_FD_ISID_ID+offset);
	  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
	  isid_id = (uint32)param_val->value.ulong_value;
  }

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_DECISION_FD_TYPE_ID+offset,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_DECISION_FD_TYPE_ID+offset);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    macro_type = param_val->numeric_equivalent;
  }

  if(macro_type == SOC_PPD_FWD_DECISION_UI_DROP_SET)
  {
    SOC_PPD_FRWRD_DECISION_DROP_SET(unit, fwd_decision, ret);
  }
  else if(macro_type == SOC_PPD_FWD_DECISION_UI_LOCAL_CPU_SET)
  {
    SOC_PPD_FRWRD_DECISION_LOCAL_CPU_SET(unit, fwd_decision, ret);
  }
  else if(macro_type == SOC_PPD_FWD_DECISION_UI_PHY_SYS_PORT_SET)
  {
    SOC_PPD_FRWRD_DECISION_PHY_SYS_PORT_SET(unit, fwd_decision, dest_id, ret);
  }
  else if(macro_type == SOC_PPD_FWD_DECISION_UI_LAG_SET)
  {
    SOC_PPD_FRWRD_DECISION_LAG_SET(unit, fwd_decision, dest_id, ret);
  }
  else if(macro_type == SOC_PPD_FWD_DECISION_UI_MC_GROUP_SET)
  {
    SOC_PPD_FRWRD_DECISION_MC_GROUP_SET(unit, fwd_decision, dest_id, ret);
  }
  else if(macro_type == SOC_PPD_FWD_DECISION_UI_FEC_SET)
  {
    SOC_PPD_FRWRD_DECISION_FEC_SET(unit, fwd_decision, dest_id, ret);
  }
  else if(macro_type == SOC_PPD_FWD_DECISION_UI_EXPL_FLOW_SET)
  {
    SOC_PPD_FRWRD_DECISION_EXPL_FLOW_SET(unit, fwd_decision, dest_id, ret);
  }
  else if(macro_type == SOC_PPD_FWD_DECISION_UI_TRAP_SET)
  {
    SOC_PPD_FRWRD_DECISION_TRAP_SET(unit, fwd_decision, trap_code, frwrd_strength, snp_strength, ret);
  }
  else if(macro_type == SOC_PPD_FWD_DECISION_UI_AC_SET)
  {
    SOC_PPD_FRWRD_DECISION_AC_SET(unit, fwd_decision, ac_id, is_lag, dest_id, ret);
  }
  else if(macro_type == SOC_PPD_FWD_DECISION_UI_AC_WITH_COSQ_SET)
  {
    SOC_PPD_FRWRD_DECISION_AC_WITH_COSQ_SET(unit, fwd_decision, ac_id, dest_id, ret);
  }
  else if(macro_type == SOC_PPD_FWD_DECISION_UI_PROTECTED_AC_SET)
  {
    SOC_PPD_FRWRD_DECISION_PROTECTED_AC_SET(unit, fwd_decision, ac_id, dest_id, ret);
  }
  else if(macro_type == SOC_PPD_FWD_DECISION_UI_MAC_IN_MAC_SET)
  {
	SOC_PPD_FRWRD_DECISION_MAC_IN_MAC_SET(unit, fwd_decision, isid_id, dest_id, ret);
  }
  else if(macro_type == SOC_PPD_FWD_DECISION_UI_PWE_SET)
  {
    SOC_PPD_FRWRD_DECISION_PWE_SET(unit, fwd_decision, eep_ndx, is_lag, dest_id, ret);
  }
  else if(macro_type == SOC_PPD_FWD_DECISION_UI_PWE_WITH_COSQ_SET)
  {
    SOC_PPD_FRWRD_DECISION_PWE_WITH_COSQ_SET(unit, fwd_decision, eep_ndx, dest_id, ret);
  }
  else if(macro_type == SOC_PPD_FWD_DECISION_UI_PWE_PROTECTED_TUNNEL_SET)
  {
    SOC_PPD_FRWRD_DECISION_PWE_PROTECTED_TUNNEL_SET(unit, fwd_decision, label, push_profile, dest_id, ret);
  }
  else if(macro_type == SOC_PPD_FWD_DECISION_UI_PROTECTED_PWE_SET)
  {
    SOC_PPD_FRWRD_DECISION_PROTECTED_PWE_SET(unit, fwd_decision, dest_id, ret);
  }
  else if(macro_type == SOC_PPD_FWD_DECISION_UI_ILM_SWAP_SET)
  {
    SOC_PPD_FRWRD_DECISION_ILM_SWAP_SET(unit, fwd_decision, label, dest_id, ret);
  }
  else if(macro_type == SOC_PPD_FWD_DECISION_UI_ILM_PUSH_SET)
  {
    SOC_PPD_FRWRD_DECISION_ILM_PUSH_SET(unit, fwd_decision, label, push_profile, dest_id, ret);
  }
  else if(macro_type == SOC_PPD_FWD_DECISION_UI_ILM_EEP_SET)
  {
    SOC_PPD_FRWRD_DECISION_PWE_SET(unit, fwd_decision, eep_ndx, is_lag, dest_id, ret);
  }
  else if(macro_type == SOC_PPD_FWD_DECISION_UI_TRILL_SET)
  {
    SOC_PPD_FRWRD_DECISION_TRILL_SET(unit, fwd_decision, label, push_profile, dest_id, ret);
  }
  else if(macro_type == SOC_PPD_FWD_DECISION_UI_MAC_IN_MAC_SET)
  {
	SOC_PPD_FRWRD_DECISION_MAC_IN_MAC_SET(unit, fwd_decision, isid_id, dest_id, ret);
  } else {
      ret = SOC_SAND_OK;
  }

  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** ui_ppd_frwrd_decision_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "ui_ppd_frwrd_decision_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}


/********************************************************************
*  Function handler: pwe_add (section lif)
 */
int
ui_ppd_api_lif_pwe_add(
                       CURRENT_LINE *current_line
                       )
{
  uint32
    ret;
  SOC_SAND_PP_MPLS_LABEL   
    prm_in_vc_label=0;
  uint32
    idx,
    nof_reps = 1;
  SOC_PPC_LIF_ID   
    prm_lif_index=0;
  SOC_PPC_L2_LIF_PWE_INFO
    prm_pwe_info;
  SOC_PPC_L2_LIF_PWE_ADDITIONAL_INFO
    prm_pwe_additional_info;
  SOC_SAND_SUCCESS_FAILURE
    prm_success;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif");
  soc_sand_proc_name = "soc_ppd_l2_lif_pwe_add";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_L2_LIF_PWE_INFO_clear(&prm_pwe_info);
  SOC_PPC_L2_LIF_PWE_ADDITIONAL_INFO_clear(&prm_pwe_additional_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_PWE_ADD_PWE_ADD_IN_VC_LABEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_PWE_ADD_PWE_ADD_IN_VC_LABEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_in_vc_label = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_PWE_ADD_PWE_NOF_REPS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_PWE_ADD_PWE_NOF_REPS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    nof_reps = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_PWE_ADD_PWE_ADD_LIF_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_PWE_ADD_PWE_ADD_LIF_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_lif_index = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_PWE_ADD_PWE_ADD_PWE_INFO_SERVICE_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_L2_LIF_PWE_ADD_PWE_ADD_PWE_INFO_SERVICE_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_pwe_info.service_type = param_val->numeric_equivalent;

	cli_out("prm_pwe_info.service_type: %d\n", prm_pwe_info.service_type);
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_PWE_ADD_PWE_ADD_PWE_INFO_COS_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_PWE_ADD_PWE_ADD_PWE_INFO_COS_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pwe_info.cos_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_PWE_ADD_PWE_ADD_PWE_INFO_TPID_PROFILE_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_PWE_ADD_PWE_ADD_PWE_INFO_TPID_PROFILE_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pwe_info.tpid_profile_index = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_PWE_ADD_PWE_ADD_PWE_INFO_HAS_CW_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_PWE_ADD_PWE_ADD_PWE_INFO_HAS_CW_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pwe_info.has_cw = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_PWE_ADD_PWE_ADD_PWE_INFO_VSI_ASSIGNMENT_MODE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_PWE_ADD_PWE_ADD_PWE_INFO_VSI_ASSIGNMENT_MODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pwe_info.vsi_assignment_mode = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_PWE_ADD_PWE_ADD_PWE_INFO_LIF_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_PWE_ADD_PWE_ADD_PWE_INFO_LIF_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pwe_info.lif_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_PWE_ADD_PWE_ADD_PWE_INFO_OAM_INSTANCE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_PWE_ADD_PWE_ADD_PWE_INFO_OAM_INSTANCE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pwe_info.protection_pointer = (uint32)param_val->value.ulong_value;
	  prm_pwe_info.oam_valid = TRUE;
  }
  else 
  {
	  prm_pwe_info.oam_valid = FALSE;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_PWE_ADD_PWE_ADD_PWE_INFO_TERMINATION_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_PWE_ADD_PWE_ADD_PWE_INFO_TERMINATION_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pwe_info.term_profile = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_PWE_ADD_PWE_ADD_PWE_INFO_ACTION_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_PWE_ADD_PWE_ADD_PWE_INFO_ACTION_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pwe_info.action_profile = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_PWE_ADD_PWE_ADD_PWE_INFO_DEFAULT_FRWRD_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_PWE_ADD_PWE_ADD_PWE_INFO_DEFAULT_FRWRD_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pwe_info.default_forward_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_PWE_ADD_PWE_ADD_PWE_INFO_PROTECTION_PASS_VAL_ID,1))
  {
	UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_PWE_ADD_PWE_ADD_PWE_INFO_PROTECTION_PASS_VAL_ID);
	UI_MACROS_CHECK_GET_VAL_OF_ERROR;
	prm_pwe_info.protection_pass_value = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_PWE_ADD_PWE_ADD_PWE_INFO_MODEL_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_L2_LIF_PWE_ADD_PWE_ADD_PWE_INFO_MODEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pwe_info.model = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_PWE_ADD_PWE_ADD_PWE_INFO_ORIENTATION_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_L2_LIF_PWE_ADD_PWE_ADD_PWE_INFO_ORIENTATION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pwe_info.orientation = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_PWE_ADD_PWE_ADD_PWE_INFO_LEARN_INFO_ENABLE_LEARNING_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_PWE_ADD_PWE_ADD_PWE_INFO_LEARN_INFO_ENABLE_LEARNING_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pwe_info.learn_record.enable_learning = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_PWE_ADD_PWE_ADD_PWE_INFO_USE_IN_LIF_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_PWE_ADD_PWE_ADD_PWE_INFO_USE_IN_LIF_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pwe_info.use_lif = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_PWE_ADD_PWE_ADD_PWE_INFO_DEFAULT_FRWRD_DEFAULT_FRWD_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_L2_LIF_PWE_ADD_PWE_ADD_PWE_INFO_DEFAULT_FRWRD_DEFAULT_FRWD_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pwe_info.default_frwrd.default_frwd_type = param_val->numeric_equivalent;
  }

  cli_out("prm_pwe_info.service_type: %d\n", prm_pwe_info.service_type);

  if (prm_pwe_info.service_type != SOC_PPC_L2_LIF_PWE_SERVICE_TYPE_MP)
  {
	  cli_out("prm_pwe_info.service_type != SOC_PPC_L2_LIF_PWE_SERVICE_TYPE_MP\n");

    ret = ui_ppd_frwrd_decision_set(
            current_line,
            1, 
            &(prm_pwe_info.default_frwrd.default_forwarding)
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** ui_ppd_frwrd_decision_set - FAIL", TRUE);
      soc_sand_disp_result_proc(ret, "ui_ppd_frwrd_decision_set");
      goto exit;
    }
  }
  else
  {
	  cli_out("prm_pwe_info.service_type == SOC_PPC_L2_LIF_PWE_SERVICE_TYPE_MP\n");

    ret = ui_ppd_frwrd_decision_set(
            current_line,
            1,
            &(prm_pwe_info.learn_record.learn_info)
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** ui_ppd_frwrd_decision_set - FAIL", TRUE);
      soc_sand_disp_result_proc(ret, "ui_ppd_frwrd_decision_set");
      goto exit;
    }
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_PWE_ADD_PWE_ADD_PWE_INFO_VSID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_PWE_ADD_PWE_ADD_PWE_INFO_VSID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pwe_info.vsid = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  for (idx = 0; idx < nof_reps; ++idx)
  {
    if (prm_pwe_info.default_frwrd.default_forwarding.additional_info.eei.type == SOC_PPC_EEI_TYPE_MPLS)
    {
      prm_pwe_info.default_frwrd.default_forwarding.additional_info.eei.val.mpls_command.label += idx;
    }
    if (prm_pwe_info.default_frwrd.default_forwarding.additional_info.outlif.type == SOC_PPC_OUTLIF_ENCODE_TYPE_AC)
    {
      prm_pwe_info.default_frwrd.default_forwarding.additional_info.outlif.val += idx;
    }
    
    ret = soc_ppd_l2_lif_pwe_add(
            unit,
            prm_in_vc_label + idx,
            prm_lif_index,
			      &prm_pwe_additional_info,
            &prm_pwe_info,
            &prm_success
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** soc_ppd_l2_lif_pwe_add - FAIL", TRUE);
      soc_sand_disp_result_proc(ret, "soc_ppd_l2_lif_pwe_add");
      goto exit;
    }
  }

  send_string_to_screen("--> success:", TRUE);
  cli_out("success: %s\n",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: pwe_get (section lif)
 */
int
ui_ppd_api_lif_pwe_get(
                       CURRENT_LINE *current_line
                       )
{
  uint32
    ret;
  SOC_SAND_PP_MPLS_LABEL   
    prm_in_vc_label=0;
  SOC_PPC_LIF_ID   
    prm_lif_index=0;
  SOC_PPC_L2_LIF_PWE_INFO
    prm_pwe_info;
  uint8
    prm_found;
  SOC_PPC_L2_LIF_PWE_ADDITIONAL_INFO
		pwe_additional_info;
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif");
  soc_sand_proc_name = "soc_ppd_l2_lif_pwe_get";

  SOC_PPC_L2_LIF_PWE_ADDITIONAL_INFO_clear(&pwe_additional_info);

  unit = soc_ppd_get_default_unit();
  SOC_PPC_L2_LIF_PWE_INFO_clear(&prm_pwe_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_PWE_GET_PWE_GET_IN_VC_LABEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_PWE_GET_PWE_GET_IN_VC_LABEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_in_vc_label = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_l2_lif_pwe_get(
    unit,
    prm_in_vc_label,
    &prm_lif_index,
	  &pwe_additional_info,
    &prm_pwe_info,
    &prm_found
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_l2_lif_pwe_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_l2_lif_pwe_get");
    goto exit;
  }

  send_string_to_screen("--> pwe_info:", TRUE);
  SOC_PPC_L2_LIF_PWE_INFO_print(&prm_pwe_info);

  send_string_to_screen("--> found:", TRUE);
  cli_out("found: %u\n",prm_found);

  cli_out("lif_index: %u\n",prm_lif_index);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: pwe_remove (section lif)
 */
int
ui_ppd_api_lif_pwe_remove(
                          CURRENT_LINE *current_line
                          )
{
  uint32
    ret;
  SOC_SAND_PP_MPLS_LABEL   
    prm_in_vc_label=0;
  SOC_PPC_L2_LIF_PWE_ADDITIONAL_INFO
		pwe_additional_info;
  uint32
    prm_lif_index;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif");
  soc_sand_proc_name = "soc_ppd_l2_lif_pwe_remove";

  unit = soc_ppd_get_default_unit();

  SOC_PPC_L2_LIF_PWE_ADDITIONAL_INFO_clear(&pwe_additional_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_PWE_REMOVE_PWE_REMOVE_IN_VC_LABEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_PWE_REMOVE_PWE_REMOVE_IN_VC_LABEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_in_vc_label = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_l2_lif_pwe_remove(
    unit,
    prm_in_vc_label,
    TRUE/*remove from SEM*/,
    0,/*not supporting gal deletion*/ 
    &prm_lif_index,
	  &pwe_additional_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_l2_lif_pwe_remove - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_l2_lif_pwe_remove");
    goto exit;
  }

  send_string_to_screen("--> lif_index:", TRUE);
  cli_out("lif_index: %u\n",prm_lif_index);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: vlan_compression_add (section lif)
 */
int
ui_ppd_api_lif_vlan_compression_add(
                                    CURRENT_LINE *current_line
                                    )
{
  uint32
    ret;
  SOC_PPC_L2_VLAN_RANGE_KEY
    prm_vlan_range_key;
  SOC_SAND_SUCCESS_FAILURE
    prm_success;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif");
  soc_sand_proc_name = "soc_ppd_l2_lif_vlan_compression_add";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_L2_VLAN_RANGE_KEY_clear(&prm_vlan_range_key);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_VLAN_COMPRESSION_ADD_VLAN_COMPRESSION_ADD_VLAN_RANGE_KEY_LAST_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_VLAN_COMPRESSION_ADD_VLAN_COMPRESSION_ADD_VLAN_RANGE_KEY_LAST_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vlan_range_key.last_vid = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_VLAN_COMPRESSION_ADD_VLAN_COMPRESSION_ADD_VLAN_RANGE_KEY_FIRST_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_VLAN_COMPRESSION_ADD_VLAN_COMPRESSION_ADD_VLAN_RANGE_KEY_FIRST_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vlan_range_key.first_vid = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_VLAN_COMPRESSION_ADD_VLAN_COMPRESSION_ADD_VLAN_RANGE_KEY_IS_OUTER_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_VLAN_COMPRESSION_ADD_VLAN_COMPRESSION_ADD_VLAN_RANGE_KEY_IS_OUTER_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vlan_range_key.is_outer = (uint8)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_VLAN_COMPRESSION_ADD_VLAN_COMPRESSION_ADD_VLAN_RANGE_KEY_LOCAL_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_VLAN_COMPRESSION_ADD_VLAN_COMPRESSION_ADD_VLAN_RANGE_KEY_LOCAL_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vlan_range_key.local_port_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vlan_range_key after vlan_compression_add***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_l2_lif_vlan_compression_add(
    unit,
    &prm_vlan_range_key,
    &prm_success
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_l2_lif_vlan_compression_add - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_l2_lif_vlan_compression_add");
    goto exit;
  }

  send_string_to_screen("--> success:", TRUE);
  cli_out("success: %s\n",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: vlan_compression_remove (section lif)
 */
int
ui_ppd_api_lif_vlan_compression_remove(
                                       CURRENT_LINE *current_line
                                       )
{
  uint32
    ret;
  SOC_PPC_L2_VLAN_RANGE_KEY
    prm_vlan_range_key;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif");
  soc_sand_proc_name = "soc_ppd_l2_lif_vlan_compression_remove";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_L2_VLAN_RANGE_KEY_clear(&prm_vlan_range_key);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_VLAN_COMPRESSION_REMOVE_VLAN_COMPRESSION_REMOVE_VLAN_RANGE_KEY_LAST_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_VLAN_COMPRESSION_REMOVE_VLAN_COMPRESSION_REMOVE_VLAN_RANGE_KEY_LAST_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vlan_range_key.last_vid = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_VLAN_COMPRESSION_REMOVE_VLAN_COMPRESSION_REMOVE_VLAN_RANGE_KEY_FIRST_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_VLAN_COMPRESSION_REMOVE_VLAN_COMPRESSION_REMOVE_VLAN_RANGE_KEY_FIRST_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vlan_range_key.first_vid = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_VLAN_COMPRESSION_REMOVE_VLAN_COMPRESSION_REMOVE_VLAN_RANGE_KEY_IS_OUTER_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_VLAN_COMPRESSION_REMOVE_VLAN_COMPRESSION_REMOVE_VLAN_RANGE_KEY_IS_OUTER_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vlan_range_key.is_outer = (uint8)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_VLAN_COMPRESSION_REMOVE_VLAN_COMPRESSION_REMOVE_VLAN_RANGE_KEY_LOCAL_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_VLAN_COMPRESSION_REMOVE_VLAN_COMPRESSION_REMOVE_VLAN_RANGE_KEY_LOCAL_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vlan_range_key.local_port_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vlan_range_key after vlan_compression_remove***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_l2_lif_vlan_compression_remove(
    unit,
    &prm_vlan_range_key
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_l2_lif_vlan_compression_remove - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_l2_lif_vlan_compression_remove");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: ac_map_key_set (section lif)
 */
int
ui_ppd_api_lif_ac_map_key_set(
                              CURRENT_LINE *current_line
                              )
{
  uint32
    ret;
  SOC_PPC_L2_LIF_AC_KEY_QUALIFIER
    prm_ac_key_qualifier;
  SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE
    prm_ac_map_key;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif");
  soc_sand_proc_name = "soc_ppd_l2_lif_ac_map_key_set";

  unit = soc_ppd_get_default_unit();

  SOC_PPC_L2_LIF_AC_KEY_QUALIFIER_clear(&prm_ac_key_qualifier);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_MAP_KEY_SET_AC_MAP_KEY_SET_PORT_PROFILE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_MAP_KEY_SET_AC_MAP_KEY_SET_PORT_PROFILE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_key_qualifier.port_profile = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter port_profile_ndx after ac_map_key_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_MAP_KEY_SET_AC_MAP_KEY_SET_VLAN_FORMAT_OUTER_TPID_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_L2_LIF_AC_MAP_KEY_SET_AC_MAP_KEY_SET_VLAN_FORMAT_OUTER_TPID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_key_qualifier.pkt_parse_info.outer_tpid = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vlan_format_ndx after ac_map_key_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_MAP_KEY_SET_AC_MAP_KEY_SET_VLAN_FORMAT_INNER_TPID_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_L2_LIF_AC_MAP_KEY_SET_AC_MAP_KEY_SET_VLAN_FORMAT_INNER_TPID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_key_qualifier.pkt_parse_info.inner_tpid = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vlan_format_ndx after ac_map_key_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_MAP_KEY_SET_AC_MAP_KEY_SET_VLAN_FORMAT_IS_OUTER_PRIO_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_MAP_KEY_SET_AC_MAP_KEY_SET_VLAN_FORMAT_IS_OUTER_PRIO_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_key_qualifier.pkt_parse_info.is_outer_prio = (uint8)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vlan_format_ndx after ac_map_key_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first*/
  ret = soc_ppd_l2_lif_ac_map_key_get(
          unit,
          &prm_ac_key_qualifier,
          &prm_ac_map_key
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
  send_string_to_screen(" *** soc_ppd_l2_lif_ac_map_key_get - FAIL", TRUE);
  soc_sand_disp_result_proc(ret, "soc_ppd_l2_lif_ac_map_key_get");
  goto exit;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_MAP_KEY_SET_AC_MAP_KEY_SET_AC_MAP_KEY_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_L2_LIF_AC_MAP_KEY_SET_AC_MAP_KEY_SET_AC_MAP_KEY_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_map_key = param_val->numeric_equivalent;
  }

  /* Call function */
  ret = soc_ppd_l2_lif_ac_map_key_set(
          unit,
          &prm_ac_key_qualifier,
          prm_ac_map_key
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_l2_lif_ac_map_key_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_l2_lif_ac_map_key_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: ac_map_key_get (section lif)
 */
int
ui_ppd_api_lif_ac_map_key_get(
                              CURRENT_LINE *current_line
                              )
{
  uint32
    ret;
  SOC_PPC_L2_LIF_AC_KEY_QUALIFIER
    prm_ac_key_qualifier;
  SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE
    prm_ac_map_key;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif");
  soc_sand_proc_name = "soc_ppd_l2_lif_ac_map_key_get";

  unit = soc_ppd_get_default_unit();

  SOC_PPC_L2_LIF_AC_KEY_QUALIFIER_clear(&prm_ac_key_qualifier);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_MAP_KEY_GET_AC_MAP_KEY_GET_PORT_PROFILE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_MAP_KEY_GET_AC_MAP_KEY_GET_PORT_PROFILE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_key_qualifier.port_profile = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter port_profile_ndx after ac_map_key_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_MAP_KEY_GET_AC_MAP_KEY_GET_VLAN_FORMAT_OUTER_TPID_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_L2_LIF_AC_MAP_KEY_GET_AC_MAP_KEY_GET_VLAN_FORMAT_OUTER_TPID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_key_qualifier.pkt_parse_info.outer_tpid = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vlan_format_ndx after ac_map_key_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_MAP_KEY_GET_AC_MAP_KEY_GET_VLAN_FORMAT_INNER_TPID_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_L2_LIF_AC_MAP_KEY_GET_AC_MAP_KEY_GET_VLAN_FORMAT_INNER_TPID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_key_qualifier.pkt_parse_info.inner_tpid = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vlan_format_ndx after ac_map_key_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_MAP_KEY_GET_AC_MAP_KEY_GET_VLAN_FORMAT_IS_OUTER_PRIO_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_MAP_KEY_GET_AC_MAP_KEY_GET_VLAN_FORMAT_IS_OUTER_PRIO_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_key_qualifier.pkt_parse_info.is_outer_prio = (uint8)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vlan_format_ndx after ac_map_key_set***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_l2_lif_ac_map_key_get(
          unit,
          &prm_ac_key_qualifier,
          &prm_ac_map_key
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_l2_lif_ac_map_key_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_l2_lif_ac_map_key_get");
    goto exit;
  }

  send_string_to_screen("--> prm_ac_map_key:", TRUE);
  cli_out("%s\n",SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_to_string(prm_ac_map_key));

  goto exit;
exit:
  return ui_ret;
}

 
/******************************************************************** 
 *  Function handler: ac_mp_info_set (section lif)
 */
int 
  ui_ppd_api_lif_ac_mp_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_LIF_ID   
    prm_lif_ndx;
  SOC_PPC_L2_LIF_AC_MP_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif"); 
  soc_sand_proc_name = "soc_ppd_l2_lif_ac_mp_info_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_L2_LIF_AC_MP_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_MP_INFO_SET_AC_MP_INFO_SET_LIF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_MP_INFO_SET_AC_MP_INFO_SET_LIF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_lif_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter lif_ndx after ac_mp_info_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_l2_lif_ac_mp_info_get(
          unit,
          prm_lif_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_l2_lif_ac_mp_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_l2_lif_ac_mp_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_MP_INFO_SET_AC_MP_INFO_SET_INFO_IS_VALID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_MP_INFO_SET_AC_MP_INFO_SET_INFO_IS_VALID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.is_valid = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_MP_INFO_SET_AC_MP_INFO_SET_INFO_MP_LEVEL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_MP_INFO_SET_AC_MP_INFO_SET_INFO_MP_LEVEL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.mp_level = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_l2_lif_ac_mp_info_set(
          unit,
          prm_lif_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_l2_lif_ac_mp_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_l2_lif_ac_mp_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: ac_mp_info_get (section lif)
 */
int 
  ui_ppd_api_lif_ac_mp_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_LIF_ID   
    prm_lif_ndx;
  SOC_PPC_L2_LIF_AC_MP_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif"); 
  soc_sand_proc_name = "soc_ppd_l2_lif_ac_mp_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_L2_LIF_AC_MP_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_MP_INFO_GET_AC_MP_INFO_GET_LIF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_MP_INFO_GET_AC_MP_INFO_GET_LIF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_lif_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter lif_ndx after ac_mp_info_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_l2_lif_ac_mp_info_get(
          unit,
          prm_lif_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_l2_lif_ac_mp_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_l2_lif_ac_mp_info_get");   
    goto exit; 
  } 

  SOC_PPC_L2_LIF_AC_MP_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/********************************************************************
*  Function handler: ac_add (section lif)
 */
int
ui_ppd_api_lif_ac_add(
                      CURRENT_LINE *current_line
                      )
{
  uint32
    ret;
  SOC_PPC_L2_LIF_AC_KEY
    prm_ac_key;
  uint32
    idx,
    nof_reps = 1;
  SOC_PPC_LIF_ID   
    prm_lif_index=0;
  SOC_PPC_L2_LIF_AC_INFO
    prm_ac_info;
  SOC_SAND_SUCCESS_FAILURE
    prm_success;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif");
  soc_sand_proc_name = "soc_ppd_l2_lif_ac_add";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_L2_LIF_AC_KEY_clear(&prm_ac_key);
  SOC_PPC_L2_LIF_AC_INFO_clear(&prm_ac_info);

  prm_ac_key.inner_vid = SOC_PPC_LIF_IGNORE_INNER_VID;
  prm_ac_key.outer_vid = SOC_PPC_LIF_IGNORE_OUTER_VID;
  prm_ac_info.learn_record.learn_type = SOC_PPC_L2_LIF_AC_LEARN_INFO;
  prm_ac_info.learn_record.learn_info.type = SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT;
  prm_ac_info.learn_record.learn_info.additional_info.outlif.type = SOC_PPC_OUTLIF_ENCODE_TYPE_AC;
  prm_ac_info.orientation = SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE;
  prm_ac_info.default_frwrd.default_frwd_type = SOC_PPC_L2_LIF_DFLT_FRWRD_SRC_LIF;

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_ADD_AC_ADD_AC_KEY_RAW_KEY_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_ADD_AC_ADD_AC_KEY_RAW_KEY_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_ac_key.raw_key = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_ADD_AC_ADD_AC_KEY_INNER_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_ADD_AC_ADD_AC_KEY_INNER_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_key.inner_vid = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_ADD_AC_ADD_AC_KEY_OUTER_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_ADD_AC_ADD_AC_KEY_OUTER_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_key.outer_vid = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_ADD_AC_ADD_AC_KEY_VLAN_DOMAIN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_ADD_AC_ADD_AC_KEY_VLAN_DOMAIN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_key.vlan_domain = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_ADD_AC_ADD_AC_KEY_PCP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_ADD_AC_ADD_AC_KEY_PCP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_key.outer_pcp = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_ADD_AC_ADD_AC_KEY_TUNNEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_ADD_AC_ADD_AC_KEY_TUNNEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_key.tunnel = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_ADD_AC_ADD_AC_KEY_KEY_TYPE_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_L2_LIF_AC_ADD_AC_ADD_AC_KEY_KEY_TYPE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_ac_key.key_type = param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_ADD_AC_ADD_NOF_REPS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_ADD_AC_ADD_NOF_REPS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    nof_reps = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_ADD_AC_ADD_LIF_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_ADD_AC_ADD_LIF_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_lif_index = (uint32)param_val->value.ulong_value;
    prm_ac_info.learn_record.learn_info.additional_info.outlif.val = prm_lif_index;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_ADD_AC_ADD_AC_INFO_COS_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_ADD_AC_ADD_AC_INFO_COS_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_info.cos_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_ADD_AC_ADD_AC_INFO_ORIENTATION_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_L2_LIF_AC_ADD_AC_ADD_AC_INFO_ORIENTATION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_info.orientation = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_ADD_AC_ADD_AC_INFO_L2CP_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_ADD_AC_ADD_AC_INFO_L2CP_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_info.l2cp_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_ADD_AC_ADD_AC_INFO_ING_EDIT_INFO_EDIT_PCP_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_ADD_AC_ADD_AC_INFO_ING_EDIT_INFO_EDIT_PCP_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_info.ing_edit_info.edit_pcp_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_ADD_AC_ADD_AC_INFO_ING_EDIT_INFO_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_ADD_AC_ADD_AC_INFO_ING_EDIT_INFO_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_info.ing_edit_info.vid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_ADD_AC_ADD_AC_INFO_ING_EDIT_INFO_ING_VLAN_EDIT_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_ADD_AC_ADD_AC_INFO_ING_EDIT_INFO_ING_VLAN_EDIT_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_info.ing_edit_info.ing_vlan_edit_profile = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_ADD_AC_ADD_AC_INFO_LEARN_INFO_LEARN_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_L2_LIF_AC_ADD_AC_ADD_AC_INFO_LEARN_INFO_LEARN_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_info.learn_record.learn_type = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_ADD_AC_ADD_AC_INFO_VSID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_ADD_AC_ADD_AC_INFO_VSID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_info.vsid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_ADD_AC_ADD_AC_INFO_USE_IN_LIF_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_ADD_AC_ADD_AC_INFO_USE_IN_LIF_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_info.use_lif = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_ADD_AC_ADD_AC_INFO_VSI_ASSIGNMENT_MODE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_ADD_AC_ADD_AC_INFO_VSI_ASSIGNMENT_MODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_info.vsi_assignment_mode = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_ADD_AC_ADD_AC_INFO_LIF_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_ADD_AC_ADD_AC_INFO_LIF_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_info.lif_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_ADD_AC_ADD_AC_INFO_OAM_INSTANCE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_PWE_ADD_PWE_ADD_PWE_INFO_OAM_INSTANCE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_info.protection_pointer = (uint32)param_val->value.ulong_value;
	  prm_ac_info.oam_valid = TRUE;
  }
  else 
  {
	  prm_ac_info.oam_valid = FALSE;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_ADD_AC_ADD_AC_INFO_DEFAULT_FRWRD_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_ADD_AC_ADD_AC_INFO_DEFAULT_FRWRD_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_info.default_forward_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_ADD_AC_ADD_AC_INFO_PROTECTION_PASS_VAL_ID,1))
  {
	UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_ADD_AC_ADD_AC_INFO_PROTECTION_PASS_VAL_ID);
	UI_MACROS_CHECK_GET_VAL_OF_ERROR;
	prm_ac_info.protection_pass_value = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_ADD_AC_ADD_AC_INFO_SERVICE_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_L2_LIF_AC_ADD_AC_ADD_AC_INFO_SERVICE_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_ac_info.service_type = param_val->numeric_equivalent;
	  if(prm_ac_info.service_type == SOC_PPC_L2_LIF_AC_SERVICE_TYPE_MP)
    {
      prm_ac_info.default_frwrd.default_frwd_type = SOC_PPC_L2_LIF_DFLT_FRWRD_SRC_VSI;
    }
    else
    {
      prm_ac_info.default_frwrd.default_frwd_type = SOC_PPC_L2_LIF_DFLT_FRWRD_SRC_LIF;
    }
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_ADD_AC_ADD_AC_INFO_DEFAULT_FRWRD_DEFAULT_FRWD_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_L2_LIF_AC_ADD_AC_ADD_AC_INFO_DEFAULT_FRWRD_DEFAULT_FRWD_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_info.default_frwrd.default_frwd_type = param_val->numeric_equivalent;
  }

  if (prm_ac_info.service_type != SOC_PPC_L2_LIF_AC_SERVICE_TYPE_MP)
  {
    ret = ui_ppd_frwrd_decision_set(
            current_line,
            1,
            &(prm_ac_info.default_frwrd.default_forwarding)
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** ui_ppd_frwrd_decision_set - FAIL", TRUE);
      soc_sand_disp_result_proc(ret, "ui_ppd_frwrd_decision_set");
      goto exit;
    }
  }
  else
  {
    ret = ui_ppd_frwrd_decision_set(
            current_line,
            1,
            &(prm_ac_info.learn_record.learn_info)
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** ui_ppd_frwrd_decision_set - FAIL", TRUE);
      soc_sand_disp_result_proc(ret, "ui_ppd_frwrd_decision_set");
      goto exit;
    }
  }

  for (idx = 0; idx < nof_reps; ++idx)
  {
    /* Call function */
    ret = soc_ppd_l2_lif_ac_add(
            unit,
            &prm_ac_key,
            prm_lif_index + idx,
            &prm_ac_info,
            &prm_success
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** soc_ppd_l2_lif_ac_add - FAIL", TRUE);
      soc_sand_disp_result_proc(ret, "soc_ppd_l2_lif_ac_add");
      goto exit;
    }

    prm_ac_key.outer_vid = (prm_ac_key.inner_vid == SOC_PPC_LIF_IGNORE_INNER_VID) ? prm_ac_key.outer_vid + 1 : prm_ac_key.outer_vid + (prm_ac_key.inner_vid + 1) / 4096;
    prm_ac_key.inner_vid = (prm_ac_key.inner_vid == SOC_PPC_LIF_IGNORE_INNER_VID) ? SOC_PPC_LIF_IGNORE_INNER_VID : (prm_ac_key.inner_vid + 1) % 4096 + SOC_SAND_NUM2BOOL(prm_ac_key.inner_vid + 1== 4096);
  }

  send_string_to_screen("--> success:", TRUE);
  cli_out("success: %s\n",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: ac_get (section lif)
 */
int
ui_ppd_api_lif_ac_get(
                      CURRENT_LINE *current_line
                      )
{
  uint32
    ret;
  SOC_PPC_L2_LIF_AC_KEY
    prm_ac_key;
  SOC_PPC_LIF_ID   
    prm_lif_index;
  SOC_PPC_L2_LIF_AC_INFO
    prm_ac_info;
  uint8
    prm_found;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif");
  soc_sand_proc_name = "soc_ppd_l2_lif_ac_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_L2_LIF_AC_KEY_clear(&prm_ac_key);
  SOC_PPC_L2_LIF_AC_INFO_clear(&prm_ac_info);

  prm_ac_key.inner_vid = SOC_PPC_LIF_IGNORE_INNER_VID;
  prm_ac_key.outer_vid = SOC_PPC_LIF_IGNORE_OUTER_VID;

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_GET_AC_GET_AC_KEY_RAW_KEY_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_GET_AC_GET_AC_KEY_RAW_KEY_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_ac_key.raw_key = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_GET_AC_GET_AC_KEY_INNER_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_GET_AC_GET_AC_KEY_INNER_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_key.inner_vid = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_GET_AC_GET_AC_KEY_OUTER_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_GET_AC_GET_AC_KEY_OUTER_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_key.outer_vid = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_GET_AC_GET_AC_KEY_VLAN_DOMAIN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_GET_AC_GET_AC_KEY_VLAN_DOMAIN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_key.vlan_domain = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_GET_AC_GET_AC_KEY_PCP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_GET_AC_GET_AC_KEY_PCP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_key.outer_pcp = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_GET_AC_GET_AC_KEY_TUNNEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_GET_AC_GET_AC_KEY_TUNNEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_key.tunnel = (uint32)param_val->value.ulong_value;
  }


  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_GET_AC_GET_AC_KEY_KEY_TYPE_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_L2_LIF_AC_GET_AC_GET_AC_KEY_KEY_TYPE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_ac_key.key_type = param_val->numeric_equivalent;
  } 

  /* Call function */
  ret = soc_ppd_l2_lif_ac_get(
    unit,
    &prm_ac_key,
    &prm_lif_index,
    &prm_ac_info,
    &prm_found
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_l2_lif_ac_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_l2_lif_ac_get");
    goto exit;
  }

  send_string_to_screen("--> lif_index:", TRUE);
  cli_out("lif_index: %u\n",prm_lif_index);

  send_string_to_screen("--> ac_info:", TRUE);
  SOC_PPC_L2_LIF_AC_INFO_print(&prm_ac_info);

  send_string_to_screen("--> found:", TRUE);
  cli_out("found: %u\n",prm_found);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: ac_with_cos_add (section lif)
 */
int
ui_ppd_api_lif_ac_with_cos_add_acs_info_set(
                               CURRENT_LINE *current_line
                               )
{
  uint32
    ret;
  uint32
    prm_acs_info_index = 0xFFFFFFFF;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif");
  soc_sand_proc_name = "soc_ppd_l2_lif_ac_with_cos_add_acs_info_set";

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_WITH_COS_ADD_AC_WITH_COS_ADD_ACS_GROUP_INFO_ACS_INFO_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_WITH_COS_ADD_AC_WITH_COS_ADD_ACS_GROUP_INFO_ACS_INFO_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_acs_info_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_acs_info_index != 0xFFFFFFFF)
  {
    SOC_PPC_L2_LIF_AC_INFO_clear(prm_acs_group_info.acs_info + prm_acs_info_index);

    if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_WITH_COS_ADD_AC_WITH_COS_ADD_ACS_GROUP_INFO_ACS_INFO_COS_PROFILE_ID,1))
    {
      UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_WITH_COS_ADD_AC_WITH_COS_ADD_ACS_GROUP_INFO_ACS_INFO_COS_PROFILE_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      prm_acs_group_info.acs_info[ prm_acs_info_index].cos_profile = (uint32)param_val->value.ulong_value;
    }

    if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_WITH_COS_ADD_AC_WITH_COS_ADD_ACS_GROUP_INFO_ACS_INFO_ORIENTATION_ID,1))
    {
      UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_L2_LIF_AC_WITH_COS_ADD_AC_WITH_COS_ADD_ACS_GROUP_INFO_ACS_INFO_ORIENTATION_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      prm_acs_group_info.acs_info[ prm_acs_info_index].orientation = param_val->numeric_equivalent;
    }

    if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_WITH_COS_ADD_AC_WITH_COS_ADD_ACS_GROUP_INFO_ACS_INFO_DEFAULT_FRWRD_DEFAULT_FRWD_TYPE_ID,1))
    {
      UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_L2_LIF_AC_WITH_COS_ADD_AC_WITH_COS_ADD_ACS_GROUP_INFO_ACS_INFO_DEFAULT_FRWRD_DEFAULT_FRWD_TYPE_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      prm_acs_group_info.acs_info[ prm_acs_info_index].default_frwrd.default_frwd_type = param_val->numeric_equivalent;
    }

    if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_WITH_COS_ADD_AC_WITH_COS_ADD_ACS_GROUP_INFO_ACS_INFO_ING_EDIT_INFO_EDIT_PCP_PROFILE_ID,1))
    {
      UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_WITH_COS_ADD_AC_WITH_COS_ADD_ACS_GROUP_INFO_ACS_INFO_ING_EDIT_INFO_EDIT_PCP_PROFILE_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      prm_acs_group_info.acs_info[ prm_acs_info_index].ing_edit_info.edit_pcp_profile = (uint32)param_val->value.ulong_value;
    }

    if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_WITH_COS_ADD_AC_WITH_COS_ADD_ACS_GROUP_INFO_ACS_INFO_ING_EDIT_INFO_VID_ID,1))
    {
      UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_WITH_COS_ADD_AC_WITH_COS_ADD_ACS_GROUP_INFO_ACS_INFO_ING_EDIT_INFO_VID_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      prm_acs_group_info.acs_info[ prm_acs_info_index].ing_edit_info.vid = (uint32)param_val->value.ulong_value;
    }

    if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_WITH_COS_ADD_AC_WITH_COS_ADD_ACS_GROUP_INFO_ACS_INFO_ING_EDIT_INFO_ING_VLAN_EDIT_PROFILE_ID,1))
    {
      UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_WITH_COS_ADD_AC_WITH_COS_ADD_ACS_GROUP_INFO_ACS_INFO_ING_EDIT_INFO_ING_VLAN_EDIT_PROFILE_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      prm_acs_group_info.acs_info[ prm_acs_info_index].ing_edit_info.ing_vlan_edit_profile = (uint32)param_val->value.ulong_value;
    }

    if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_WITH_COS_ADD_AC_WITH_COS_ADD_ACS_GROUP_INFO_ACS_INFO_LEARN_TYPE_ID,1))
    {
      UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_L2_LIF_AC_WITH_COS_ADD_AC_WITH_COS_ADD_ACS_GROUP_INFO_ACS_INFO_LEARN_TYPE_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      prm_acs_group_info.acs_info[ prm_acs_info_index].learn_record.learn_type = (SOC_PPC_L2_LIF_AC_LEARN_TYPE)param_val->numeric_equivalent;
    }

    if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_WITH_COS_ADD_AC_WITH_COS_ADD_ACS_GROUP_INFO_ACS_INFO_VSID_ID,1))
    {
      UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_WITH_COS_ADD_AC_WITH_COS_ADD_ACS_GROUP_INFO_ACS_INFO_VSID_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      prm_acs_group_info.acs_info[ prm_acs_info_index].vsid = (uint32)param_val->value.ulong_value;
    }

    if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_WITH_COS_ADD_AC_WITH_COS_ADD_ACS_GROUP_INFO_ACS_INFO_SERVICE_TYPE_ID,1)) 
    { 
      UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_L2_LIF_AC_WITH_COS_ADD_AC_WITH_COS_ADD_ACS_GROUP_INFO_ACS_INFO_SERVICE_TYPE_ID); 
      UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
      prm_acs_group_info.acs_info[ prm_acs_info_index].service_type = param_val->numeric_equivalent;
    } 

    if (prm_acs_group_info.acs_info[ prm_acs_info_index].service_type == (uint32) SOC_PPC_L2_LIF_PWE_SERVICE_TYPE_MP)
    {
      ret = ui_ppd_frwrd_decision_set(
              current_line,
              1,
              &(prm_acs_group_info.acs_info[ prm_acs_info_index].learn_record.learn_info)
            );
      if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
      {
        send_string_to_screen(" *** ui_ppd_frwrd_decision_set - FAIL", TRUE);
        soc_sand_disp_result_proc(ret, "ui_ppd_frwrd_decision_set");
        goto exit;
      }
    }
    else
    {
      ret = ui_ppd_frwrd_decision_set(
              current_line,
              1,
              &(prm_acs_group_info.acs_info[ prm_acs_info_index].default_frwrd.default_forwarding)
            );
      if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
      {
        send_string_to_screen(" *** ui_ppd_frwrd_decision_set - FAIL", TRUE);
        soc_sand_disp_result_proc(ret, "ui_ppd_frwrd_decision_set");
        goto exit;
      }
    }
  }

  goto exit;
exit:
  return ui_ret;
}

int
ui_ppd_api_lif_ac_with_cos_add(
                               CURRENT_LINE *current_line
                               )
{
  uint32
    ret;
  SOC_PPC_L2_LIF_AC_KEY
    prm_ac_key;
  SOC_PPC_LIF_ID   
    prm_base_lif_index=0;
  SOC_SAND_SUCCESS_FAILURE
    prm_success;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif");
  soc_sand_proc_name = "soc_ppd_l2_lif_ac_with_cos_add";

  unit = soc_ppd_get_default_unit();

  SOC_PPC_L2_LIF_AC_KEY_clear(&prm_ac_key);
  prm_ac_key.inner_vid = SOC_PPC_LIF_IGNORE_INNER_VID;
  prm_ac_key.outer_vid = SOC_PPC_LIF_IGNORE_OUTER_VID;

  prm_acs_group_info.soc_sand_magic_num = SOC_SAND_MAGIC_NUM_VAL;
  prm_acs_group_info.nof_lif_entries = 0;
  prm_acs_group_info.opcode_id = 0;

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_WITH_COS_ADD_AC_WITH_COS_ADD_AC_KEY_RAW_KEY_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_WITH_COS_ADD_AC_WITH_COS_ADD_AC_KEY_RAW_KEY_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_ac_key.raw_key = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_WITH_COS_ADD_AC_WITH_COS_ADD_AC_KEY_INNER_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_WITH_COS_ADD_AC_WITH_COS_ADD_AC_KEY_INNER_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_key.inner_vid = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_WITH_COS_ADD_AC_WITH_COS_ADD_AC_KEY_OUTER_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_WITH_COS_ADD_AC_WITH_COS_ADD_AC_KEY_OUTER_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_key.outer_vid = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_WITH_COS_ADD_AC_WITH_COS_ADD_AC_KEY_VLAN_DOMAIN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_WITH_COS_ADD_AC_WITH_COS_ADD_AC_KEY_VLAN_DOMAIN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_key.vlan_domain = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_WITH_COS_ADD_AC_WITH_COS_ADD_AC_KEY_KEY_TYPE_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_L2_LIF_AC_WITH_COS_ADD_AC_WITH_COS_ADD_AC_KEY_KEY_TYPE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_ac_key.key_type = param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_WITH_COS_ADD_AC_WITH_COS_ADD_BASE_LIF_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_WITH_COS_ADD_AC_WITH_COS_ADD_BASE_LIF_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_base_lif_index = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_WITH_COS_ADD_AC_WITH_COS_ADD_ACS_GROUP_INFO_NOF_LIF_ENTRIES_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_WITH_COS_ADD_AC_WITH_COS_ADD_ACS_GROUP_INFO_NOF_LIF_ENTRIES_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_acs_group_info.nof_lif_entries = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_WITH_COS_ADD_AC_WITH_COS_ADD_ACS_GROUP_INFO_OPCODE_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_WITH_COS_ADD_AC_WITH_COS_ADD_ACS_GROUP_INFO_OPCODE_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_acs_group_info.opcode_id = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_l2_lif_ac_with_cos_add(
    unit,
    &prm_ac_key,
    prm_base_lif_index,
    &prm_acs_group_info,
    &prm_success
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_l2_lif_ac_with_cos_add - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_l2_lif_ac_with_cos_add");
    goto exit;
  }

  SOC_PPC_L2_LIF_AC_GROUP_INFO_clear(&prm_acs_group_info);

  send_string_to_screen("--> success:", TRUE);
  cli_out("success: %s\n",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: ac_with_cos_get (section lif)
 */
int
ui_ppd_api_lif_ac_with_cos_get(
                               CURRENT_LINE *current_line
                               )
{
  uint32
    ret;
  SOC_PPC_L2_LIF_AC_KEY
    prm_ac_key;
  SOC_PPC_LIF_ID   
    prm_base_lif_index=0;
  SOC_PPC_L2_LIF_AC_GROUP_INFO
    prm_acs_group_info;
  uint8
    prm_found;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif");
  soc_sand_proc_name = "soc_ppd_l2_lif_ac_with_cos_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_L2_LIF_AC_KEY_clear(&prm_ac_key);
  SOC_PPC_L2_LIF_AC_GROUP_INFO_clear(&prm_acs_group_info);

  prm_ac_key.inner_vid = SOC_PPC_LIF_IGNORE_INNER_VID;
  prm_ac_key.outer_vid = SOC_PPC_LIF_IGNORE_OUTER_VID;

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_WITH_COS_GET_AC_WITH_COS_GET_AC_KEY_INNER_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_WITH_COS_GET_AC_WITH_COS_GET_AC_KEY_INNER_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_key.inner_vid = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_WITH_COS_GET_AC_WITH_COS_GET_AC_KEY_OUTER_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_WITH_COS_GET_AC_WITH_COS_GET_AC_KEY_OUTER_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_key.outer_vid = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_WITH_COS_GET_AC_WITH_COS_GET_AC_KEY_VLAN_DOMAIN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_WITH_COS_GET_AC_WITH_COS_GET_AC_KEY_VLAN_DOMAIN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_key.vlan_domain = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ac_key after ac_with_cos_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_l2_lif_ac_with_cos_get(
    unit,
    &prm_ac_key,
    &prm_base_lif_index,
    &prm_acs_group_info,
    &prm_found
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_l2_lif_ac_with_cos_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_l2_lif_ac_with_cos_get");
    goto exit;
  }

  send_string_to_screen("--> base_lif_index:", TRUE);
  cli_out("base_lif_index: %u\n",prm_base_lif_index);

  send_string_to_screen("--> acs_group_info:", TRUE);
  SOC_PPC_L2_LIF_AC_GROUP_INFO_print(&prm_acs_group_info);

  send_string_to_screen("--> found:", TRUE);
  cli_out("found: %u\n",prm_found);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: ac_remove (section lif)
 */
int
ui_ppd_api_lif_ac_remove(
                         CURRENT_LINE *current_line
                         )
{
  uint32
    ret;
  SOC_PPC_L2_LIF_AC_KEY
    prm_ac_key;
  SOC_PPC_LIF_ID   
    prm_lif_index=0;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif");
  soc_sand_proc_name = "soc_ppd_l2_lif_ac_remove";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_L2_LIF_AC_KEY_clear(&prm_ac_key);

  prm_ac_key.inner_vid = SOC_PPC_LIF_IGNORE_INNER_VID;
  prm_ac_key.outer_vid = SOC_PPC_LIF_IGNORE_OUTER_VID;

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_REMOVE_AC_REMOVE_AC_KEY_RAW_KEY_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_REMOVE_AC_REMOVE_AC_KEY_RAW_KEY_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_ac_key.raw_key = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_REMOVE_AC_REMOVE_AC_KEY_INNER_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_REMOVE_AC_REMOVE_AC_KEY_INNER_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_key.inner_vid = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_REMOVE_AC_REMOVE_AC_KEY_OUTER_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_REMOVE_AC_REMOVE_AC_KEY_OUTER_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_key.outer_vid = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_REMOVE_AC_REMOVE_AC_KEY_VLAN_DOMAIN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_REMOVE_AC_REMOVE_AC_KEY_VLAN_DOMAIN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_key.vlan_domain = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_REMOVE_AC_REMOVE_AC_KEY_PCP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_AC_REMOVE_AC_REMOVE_AC_KEY_PCP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_key.outer_pcp = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_REMOVE_AC_REMOVE_AC_KEY_KEY_TYPE_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_L2_LIF_AC_REMOVE_AC_REMOVE_AC_KEY_KEY_TYPE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_ac_key.key_type = param_val->numeric_equivalent;
  } 

  /* Call function */
  ret = soc_ppd_l2_lif_ac_remove(
    unit,
    &prm_ac_key,
    &prm_lif_index
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_l2_lif_ac_remove - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_l2_lif_ac_remove");
    goto exit;
  }

  send_string_to_screen("--> lif_index:", TRUE);
  cli_out("lif_index: %u\n",prm_lif_index);

  goto exit;
exit:
  return ui_ret;
}
/******************************************************************** 
 *  Function handler: l2_lif_l2cp_trap_set (section default_section)
 */
int 
  ui_ppd_api_default_section_l2_lif_l2cp_trap_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_L2_LIF_L2CP_KEY   
    prm_l2cp_key;
  SOC_PPC_L2_LIF_L2CP_HANDLE_TYPE   
    prm_handle_type;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_default_section"); 
  soc_sand_proc_name = "soc_ppd_l2_lif_l2cp_trap_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_L2_LIF_L2CP_KEY_clear(&prm_l2cp_key);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_L2CP_TRAP_SET_L2_LIF_L2CP_TRAP_SET_L2CP_KEY_DA_MAC_ADDRESS_LSB_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_L2CP_TRAP_SET_L2_LIF_L2CP_TRAP_SET_L2CP_KEY_DA_MAC_ADDRESS_LSB_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_l2cp_key.da_mac_address_lsb = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_L2CP_TRAP_SET_L2_LIF_L2CP_TRAP_SET_L2CP_KEY_L2CP_PROFILE_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_L2CP_TRAP_SET_L2_LIF_L2CP_TRAP_SET_L2CP_KEY_L2CP_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_l2cp_key.l2cp_profile = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter l2cp_key after l2_lif_l2cp_trap_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_l2_lif_l2cp_trap_get(
          unit,
          &prm_l2cp_key,
          &prm_handle_type
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_l2_lif_l2cp_trap_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_l2_lif_l2cp_trap_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_L2CP_TRAP_SET_L2_LIF_L2CP_TRAP_SET_HANDLE_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_L2_LIF_L2CP_TRAP_SET_L2_LIF_L2CP_TRAP_SET_HANDLE_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_handle_type = param_val->numeric_equivalent;
  } 


  /* Call function */
  ret = soc_ppd_l2_lif_l2cp_trap_set(
          unit,
          &prm_l2cp_key,
          prm_handle_type
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_l2_lif_l2cp_trap_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_l2_lif_l2cp_trap_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: l2_lif_l2cp_trap_get (section default_section)
 */
int 
  ui_ppd_api_default_section_l2_lif_l2cp_trap_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPC_L2_LIF_L2CP_KEY   
    prm_l2cp_key;
  SOC_PPC_L2_LIF_L2CP_HANDLE_TYPE   
    prm_handle_type;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_default_section"); 
  soc_sand_proc_name = "soc_ppd_l2_lif_l2cp_trap_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPC_L2_LIF_L2CP_KEY_clear(&prm_l2cp_key);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_L2CP_TRAP_GET_L2_LIF_L2CP_TRAP_GET_L2CP_KEY_DA_MAC_ADDRESS_LSB_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_L2CP_TRAP_GET_L2_LIF_L2CP_TRAP_GET_L2CP_KEY_DA_MAC_ADDRESS_LSB_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_l2cp_key.da_mac_address_lsb = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_L2CP_TRAP_GET_L2_LIF_L2CP_TRAP_GET_L2CP_KEY_L2CP_PROFILE_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_L2CP_TRAP_GET_L2_LIF_L2CP_TRAP_GET_L2CP_KEY_L2CP_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_l2cp_key.l2cp_profile = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter l2cp_key after l2_lif_l2cp_trap_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_l2_lif_l2cp_trap_get(
          unit,
          &prm_l2cp_key,
          &prm_handle_type
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_l2_lif_l2cp_trap_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_l2_lif_l2cp_trap_get");   
    goto exit; 
  } 

  cli_out("handle_type: %s\n\r",SOC_PPC_L2_LIF_L2CP_HANDLE_TYPE_to_string(prm_handle_type));

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/********************************************************************
*  Function handler: isid_add (section lif)
 */
int
ui_ppd_api_lif_isid_add(
                        CURRENT_LINE *current_line
                        )
{
  uint32
    ret;
  SOC_PPC_SYS_VSI_ID   
    prm_vsi_ndx;
  SOC_PPC_L2_LIF_ISID_KEY
    prm_isid_key;
  SOC_PPC_LIF_ID   
    prm_lif_index=0;
  SOC_PPC_L2_LIF_ISID_INFO
    prm_isid_info;
  SOC_SAND_SUCCESS_FAILURE
    prm_success;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif");
  soc_sand_proc_name = "soc_ppd_l2_lif_isid_add";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_L2_LIF_ISID_KEY_clear(&prm_isid_key);
  SOC_PPC_L2_LIF_ISID_INFO_clear(&prm_isid_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_ISID_ADD_ISID_ADD_VSI_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_ISID_ADD_ISID_ADD_VSI_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsi_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vsi_ndx after isid_add***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_ISID_ADD_ISID_ADD_ISID_KEY_ISID_DOMAIN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_ISID_ADD_ISID_ADD_ISID_KEY_ISID_DOMAIN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_isid_key.isid_domain = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_ISID_ADD_ISID_ADD_ISID_KEY_ISID_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_ISID_ADD_ISID_ADD_ISID_KEY_ISID_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_isid_key.isid_id = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter isid_key after isid_add***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_ISID_ADD_ISID_ADD_LIF_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_ISID_ADD_ISID_ADD_LIF_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_lif_index = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_ISID_ADD_ISID_ADD_ISID_INFO_SERVICE_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_L2_LIF_ISID_ADD_ISID_ADD_ISID_INFO_SERVICE_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_isid_info.service_type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_ISID_ADD_ISID_ADD_ISID_INFO_TPID_PROFILE_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_ISID_ADD_ISID_ADD_ISID_INFO_TPID_PROFILE_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_isid_info.tpid_profile_index = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_ISID_ADD_ISID_ADD_ISID_INFO_COS_PROFILE_ID,1))
  {
	  UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_ISID_ADD_ISID_ADD_ISID_INFO_COS_PROFILE_ID);
	  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
	  prm_isid_info.cos_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_ISID_ADD_ISID_ADD_ISID_INFO_LEARN_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_ISID_ADD_ISID_ADD_ISID_INFO_LEARN_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_isid_info.learn_enable = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_ISID_ADD_ISID_ADD_ISID_INFO_VSI_ASSIGNMENT_MODE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_ISID_ADD_ISID_ADD_ISID_INFO_VSI_ASSIGNMENT_MODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_isid_info.vsi_assignment_mode = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_ISID_ADD_ISID_ADD_ISID_INFO_USE_IN_LIF_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_ISID_ADD_ISID_ADD_ISID_INFO_USE_IN_LIF_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_isid_info.use_lif = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_ISID_ADD_ISID_ADD_ISID_INFO_OAM_INSTANCE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_ISID_ADD_ISID_ADD_ISID_INFO_OAM_INSTANCE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_isid_info.protection_pointer = (uint32)param_val->value.ulong_value;
    prm_isid_info.oam_valid = TRUE;
  }
  else 
  {
    prm_isid_info.oam_valid = FALSE;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_ISID_ADD_ISID_ADD_ISID_INFO_DEFAULT_FRWRD_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_ISID_ADD_ISID_ADD_ISID_INFO_DEFAULT_FRWRD_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_isid_info.default_forward_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_ISID_ADD_ISID_ADD_ISID_INFO_PROTECTION_PASS_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_ISID_ADD_ISID_ADD_ISID_INFO_PROTECTION_PASS_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_isid_info.protection_pass_value = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_ISID_ADD_ISID_ADD_ISID_INFO_DEFAULT_FRWRD_DEFAULT_FRWD_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_L2_LIF_ISID_ADD_ISID_ADD_ISID_INFO_DEFAULT_FRWRD_DEFAULT_FRWD_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_isid_info.default_frwrd.default_frwd_type = param_val->numeric_equivalent;
  }

  ret = ui_ppd_frwrd_decision_set(
          current_line,
          1,
          &(prm_isid_info.default_frwrd.default_forwarding)
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** ui_ppd_frwrd_decision_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "ui_ppd_frwrd_decision_set");
    goto exit;
  }


  /* Call function */
  ret = soc_ppd_l2_lif_isid_add(
    unit,
    prm_vsi_ndx,
    &prm_isid_key,
    prm_lif_index,
    &prm_isid_info,
    TRUE,
    TRUE,
    0,
    &prm_success
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_l2_lif_isid_add - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_l2_lif_isid_add");
    goto exit;
  }

  send_string_to_screen("--> success:", TRUE);
  cli_out("success: %s\n",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: isid_get (section lif)
 */
int
ui_ppd_api_lif_isid_get(
                        CURRENT_LINE *current_line
                        )
{
  uint32
    ret;
  SOC_PPC_L2_LIF_ISID_KEY
    prm_isid_key;
  SOC_PPC_LIF_ID   
    prm_lif_index;
  SOC_PPC_SYS_VSI_ID   
    prm_vsi_index;
  SOC_PPC_L2_LIF_ISID_INFO
    prm_isid_info;
  uint8
    prm_found;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif");
  soc_sand_proc_name = "soc_ppd_l2_lif_isid_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_L2_LIF_ISID_KEY_clear(&prm_isid_key);
  SOC_PPC_L2_LIF_ISID_INFO_clear(&prm_isid_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_ISID_GET_ISID_GET_ISID_KEY_ISID_DOMAIN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_ISID_GET_ISID_GET_ISID_KEY_ISID_DOMAIN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_isid_key.isid_domain = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_ISID_GET_ISID_GET_ISID_KEY_ISID_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_ISID_GET_ISID_GET_ISID_KEY_ISID_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_isid_key.isid_id = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter isid_key after isid_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_l2_lif_isid_get(
    unit,
    &prm_isid_key,
    &prm_lif_index,
    &prm_vsi_index,
    &prm_isid_info,
    &prm_found
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_l2_lif_isid_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_l2_lif_isid_get");
    goto exit;
  }

  send_string_to_screen("--> lif_index:", TRUE);
  cli_out("lif_index: %u\n",prm_lif_index);

  send_string_to_screen("--> vsi_index:", TRUE);
  cli_out("vsi_index: %u\n",prm_vsi_index);

  send_string_to_screen("--> isid_info:", TRUE);
  SOC_PPC_L2_LIF_ISID_INFO_print(&prm_isid_info);

  send_string_to_screen("--> found:", TRUE);
  cli_out("found: %u\n",prm_found);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: isid_remove (section lif)
 */
int
ui_ppd_api_lif_isid_remove(
                           CURRENT_LINE *current_line
                           )
{
  uint32
    ret;
  SOC_PPC_L2_LIF_ISID_KEY
    prm_isid_key;
  SOC_PPC_LIF_ID   
    prm_lif_index=0;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif");
  soc_sand_proc_name = "soc_ppd_l2_lif_isid_remove";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_L2_LIF_ISID_KEY_clear(&prm_isid_key);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_ISID_REMOVE_ISID_REMOVE_ISID_KEY_ISID_DOMAIN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_ISID_REMOVE_ISID_REMOVE_ISID_KEY_ISID_DOMAIN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_isid_key.isid_domain = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_ISID_REMOVE_ISID_REMOVE_ISID_KEY_ISID_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_L2_LIF_ISID_REMOVE_ISID_REMOVE_ISID_KEY_ISID_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_isid_key.isid_id = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter isid_key after isid_remove***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_l2_lif_isid_remove(
    unit,
    &prm_isid_key,
    TRUE, 
    TRUE,
    0,
    0,
    &prm_lif_index
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_l2_lif_isid_remove - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_l2_lif_isid_remove");
    goto exit;
  }

  send_string_to_screen("--> lif_index:", TRUE);
  cli_out("lif_index: %u\n",prm_lif_index);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Section handler: lif
 */
int
ui_ppd_api_lif(
               CURRENT_LINE *current_line
               )
{
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_lif");

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_PWE_MAP_RANGE_SET_PWE_MAP_RANGE_SET_ID,1))
  {
    ui_ret = ui_ppd_api_lif_pwe_map_range_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_PWE_MAP_RANGE_GET_PWE_MAP_RANGE_GET_ID,1))
  {
    ui_ret = ui_ppd_api_lif_pwe_map_range_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_PWE_ADD_PWE_ADD_ID,1))
  {
    ui_ret = ui_ppd_api_lif_pwe_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_PWE_GET_PWE_GET_ID,1))
  {
    ui_ret = ui_ppd_api_lif_pwe_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_PWE_REMOVE_PWE_REMOVE_ID,1))
  {
    ui_ret = ui_ppd_api_lif_pwe_remove(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_VLAN_COMPRESSION_ADD_VLAN_COMPRESSION_ADD_ID,1))
  {
    ui_ret = ui_ppd_api_lif_vlan_compression_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_VLAN_COMPRESSION_REMOVE_VLAN_COMPRESSION_REMOVE_ID,1))
  {
    ui_ret = ui_ppd_api_lif_vlan_compression_remove(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_MAP_KEY_SET_AC_MAP_KEY_SET_ID,1))
  {
    ui_ret = ui_ppd_api_lif_ac_map_key_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_MAP_KEY_GET_AC_MAP_KEY_GET_ID,1))
  {
    ui_ret = ui_ppd_api_lif_ac_map_key_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_MP_INFO_SET_AC_MP_INFO_SET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_lif_ac_mp_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_MP_INFO_GET_AC_MP_INFO_GET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_lif_ac_mp_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_ADD_AC_ADD_ID,1))
  {
    ui_ret = ui_ppd_api_lif_ac_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_GET_AC_GET_ID,1))
  {
    ui_ret = ui_ppd_api_lif_ac_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_WITH_COS_ADD_AC_WITH_COS_ADD_ID,1))
  {
    ui_ret = ui_ppd_api_lif_ac_with_cos_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_WITH_COS_ADD_AC_WITH_COS_ADD_ACS_GROUP_INFO_ACS_INFO_SET,1))
  {
    ui_ret = ui_ppd_api_lif_ac_with_cos_add_acs_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_WITH_COS_GET_AC_WITH_COS_GET_ID,1))
  {
    ui_ret = ui_ppd_api_lif_ac_with_cos_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_AC_REMOVE_AC_REMOVE_ID,1))
  {
    ui_ret = ui_ppd_api_lif_ac_remove(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_L2CP_TRAP_SET_L2_LIF_L2CP_TRAP_SET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_default_section_l2_lif_l2cp_trap_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_L2CP_TRAP_GET_L2_LIF_L2CP_TRAP_GET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_default_section_l2_lif_l2cp_trap_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_ISID_ADD_ISID_ADD_ID,1))
  {
    ui_ret = ui_ppd_api_lif_isid_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_ISID_GET_ISID_GET_ID,1))
  {
    ui_ret = ui_ppd_api_lif_isid_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_L2_LIF_ISID_REMOVE_ISID_REMOVE_ID,1))
  {
    ui_ret = ui_ppd_api_lif_isid_remove(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after lif***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif
#endif /* LINK_PPD_LIBRARIES */ 

