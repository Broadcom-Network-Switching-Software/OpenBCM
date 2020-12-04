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
#include <soc/dpp/SAND/Utils/sand_bitstream.h> 
#include <appl/diag/dpp/utils_defi.h> 
#include <appl/dpp/UserInterface/ui_defi.h> 
 

  
#include <soc/dpp/PPD/ppd_api_llp_filter.h>                                                               
#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_llp_filter.h>

#if LINK_PPD_LIBRARIES

  
#ifdef UI_LLP_FILTER
/********************************************************************
*  Function handler: ingress_vlan_membership_set (section llp_filter)
 */
int
ui_ppd_api_llp_filter_ingress_vlan_membership_set(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_VLAN_ID   
    prm_vid_ndx;
  uint32
    prm_nof_ports,
    indx;
  SOC_PPC_PORT   
    prm_ports[SOC_PPC_VLAN_MEMBERSHIP_BITMAP_SIZE];

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_filter");
  soc_sand_proc_name = "soc_ppd_llp_filter_ingress_vlan_membership_set";
  sal_memset(prm_ports,0x0,4 *SOC_PPC_VLAN_MEMBERSHIP_BITMAP_SIZE);

  unit = soc_ppd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_SET_INGRESS_VLAN_MEMBERSHIP_SET_VID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_SET_INGRESS_VLAN_MEMBERSHIP_SET_VID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vid_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vid_ndx after ingress_vlan_membership_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first
  ret = soc_ppd_llp_filter_ingress_vlan_membership_get(
  unit,
  prm_vid_ndx,
  prm_ports
  );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
  send_string_to_screen(" *** soc_ppd_llp_filter_ingress_vlan_membership_get - FAIL", TRUE);
  soc_sand_disp_result_proc(ret, "soc_ppd_llp_filter_ingress_vlan_membership_get");
  goto exit;
  } */

  prm_nof_ports = 0;

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_SET_INGRESS_VLAN_MEMBERSHIP_SET_PORTS_ID,1))
  {
    for (indx = 0; indx < 80; ++prm_nof_ports, ++indx, ret = 0)
    {
      UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(SOC_PARAM_PPD_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_SET_INGRESS_VLAN_MEMBERSHIP_SET_PORTS_ID, indx + 1);
      if (ui_ret)
      {
        ui_ret = 0;
        break;
      }
      if (param_val->value.ulong_value/32 >= sizeof(prm_ports))
      {
        send_string_to_screen(" *** soc_ppd_llp_filter_ingress_vlan_membership_set - FAIL. Local port exceeds bitmap", TRUE);
        goto exit;
      }

      prm_ports[param_val->value.ulong_value/32] |= SOC_SAND_BIT((uint32)param_val->value.ulong_value % 32);
    }
  }

  /* Call function */
  ret = soc_ppd_llp_filter_ingress_vlan_membership_set(
    unit,
    SOC_CORE_DEFAULT,
    prm_vid_ndx,
    prm_ports
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_llp_filter_ingress_vlan_membership_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_filter_ingress_vlan_membership_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: ingress_vlan_membership_get (section llp_filter)
 */
int
ui_ppd_api_llp_filter_ingress_vlan_membership_get(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_VLAN_ID   
    prm_vid_ndx;
  SOC_PPC_PORT   
    prm_ports[SOC_PPC_VLAN_MEMBERSHIP_BITMAP_SIZE];
  uint32
    indx;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_filter");
  soc_sand_proc_name = "soc_ppd_llp_filter_ingress_vlan_membership_get";

  unit = soc_ppd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_GET_INGRESS_VLAN_MEMBERSHIP_GET_VID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_GET_INGRESS_VLAN_MEMBERSHIP_GET_VID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vid_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vid_ndx after ingress_vlan_membership_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_llp_filter_ingress_vlan_membership_get(
    unit,
    SOC_CORE_DEFAULT,
    prm_vid_ndx,
    prm_ports
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_llp_filter_ingress_vlan_membership_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_filter_ingress_vlan_membership_get");
    goto exit;
  }

  cli_out(" member ports: ");
  for (indx = 0 ; indx < SOC_PPC_MAX_NOF_LOCAL_PORTS_ARAD; ++indx)
  {
    if (soc_sand_bitstream_test_bit(prm_ports,indx))
    {
      cli_out("%u  ",indx);
    }
  }
  cli_out("\n");

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: ingress_vlan_membership_port_add (section llp_filter)
 */
int
ui_ppd_api_llp_filter_ingress_vlan_membership_port_add(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_VLAN_ID   
    prm_vid_ndx;
  SOC_PPC_PORT   
    prm_local_port=0;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_filter");
  soc_sand_proc_name = "soc_ppd_llp_filter_ingress_vlan_membership_port_add";

  unit = soc_ppd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_ADD_INGRESS_VLAN_MEMBERSHIP_PORT_ADD_VID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_ADD_INGRESS_VLAN_MEMBERSHIP_PORT_ADD_VID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vid_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vid_ndx after ingress_vlan_membership_port_add***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_ADD_INGRESS_VLAN_MEMBERSHIP_PORT_ADD_LOCAL_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_ADD_INGRESS_VLAN_MEMBERSHIP_PORT_ADD_LOCAL_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_local_port = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_llp_filter_ingress_vlan_membership_port_add(
    unit,
    SOC_CORE_DEFAULT,
    prm_vid_ndx,
    prm_local_port
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_llp_filter_ingress_vlan_membership_port_add - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_filter_ingress_vlan_membership_port_add");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: ingress_vlan_membership_port_remove (section llp_filter)
 */
int
ui_ppd_api_llp_filter_ingress_vlan_membership_port_remove(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_VLAN_ID   
    prm_vid_ndx;
  SOC_PPC_PORT   
    prm_local_port=0;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_filter");
  soc_sand_proc_name = "soc_ppd_llp_filter_ingress_vlan_membership_port_remove";

  unit = soc_ppd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_REMOVE_INGRESS_VLAN_MEMBERSHIP_PORT_REMOVE_VID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_REMOVE_INGRESS_VLAN_MEMBERSHIP_PORT_REMOVE_VID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vid_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vid_ndx after ingress_vlan_membership_port_remove***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_REMOVE_INGRESS_VLAN_MEMBERSHIP_PORT_REMOVE_LOCAL_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_REMOVE_INGRESS_VLAN_MEMBERSHIP_PORT_REMOVE_LOCAL_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_local_port = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_llp_filter_ingress_vlan_membership_port_remove(
    unit,
    SOC_CORE_DEFAULT,
    prm_vid_ndx,
    prm_local_port
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_llp_filter_ingress_vlan_membership_port_remove - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_filter_ingress_vlan_membership_port_remove");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}
/********************************************************************
*  Function handler: ingress_acceptable_frames_set (section llp_filter)
 */
int
ui_ppd_api_llp_filter_ingress_acceptable_frames_set(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_port_profile_ndx;
  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT
    prm_vlan_format_ndx;
  SOC_PPC_ACTION_PROFILE
    prm_action_profile;
  SOC_SAND_SUCCESS_FAILURE
    prm_success;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_filter");
  soc_sand_proc_name = "soc_ppd_llp_filter_ingress_acceptable_frames_set";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_ACTION_PROFILE_clear(&prm_action_profile);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_SET_INGRESS_ACCEPTABLE_FRAMES_SET_PORT_PROFILE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_SET_INGRESS_ACCEPTABLE_FRAMES_SET_PORT_PROFILE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_profile_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter port_profile_ndx after ingress_acceptable_frames_set***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_SET_INGRESS_ACCEPTABLE_FRAMES_SET_VLAN_FORMAT_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_SET_INGRESS_ACCEPTABLE_FRAMES_SET_VLAN_FORMAT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vlan_format_ndx = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vlan_format_ndx after ingress_acceptable_frames_set***", TRUE);
    goto exit;
  }



  /*trap-code
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_SET_INGRESS_ACCEPTABLE_FRAMES_SET_ACTION_PROFILE_TRAP_CODE_LSB_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_SET_INGRESS_ACCEPTABLE_FRAMES_SET_ACTION_PROFILE_TRAP_CODE_LSB_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_action_profile.trap_code = (SOC_PPC_TRAP_CODE)param_val->numeric_equivalent;
  }
  */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_SET_INGRESS_ACCEPTABLE_FRAMES_SET_ACTION_PROFILE_TRAP_CODE_LSB_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_SET_INGRESS_ACCEPTABLE_FRAMES_SET_ACTION_PROFILE_TRAP_CODE_LSB_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_action_profile.trap_code = (SOC_PPC_TRAP_CODE)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_SET_INGRESS_ACCEPTABLE_FRAMES_SET_ACTION_PROFILE_FRWRD_STRENGTH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_SET_INGRESS_ACCEPTABLE_FRAMES_SET_ACTION_PROFILE_FRWRD_STRENGTH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_action_profile.frwrd_action_strength = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_SET_INGRESS_ACCEPTABLE_FRAMES_SET_ACTION_PROFILE_SNOOP_STRENGTH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_SET_INGRESS_ACCEPTABLE_FRAMES_SET_ACTION_PROFILE_SNOOP_STRENGTH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_action_profile.snoop_action_strength = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_llp_filter_ingress_acceptable_frames_set(
    unit,
    prm_port_profile_ndx,
    prm_vlan_format_ndx,
    &prm_action_profile,
    &prm_success
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_llp_filter_ingress_acceptable_frames_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_filter_ingress_acceptable_frames_set");
    goto exit;
  }

  send_string_to_screen("--> success:", TRUE);
  cli_out("success: %s\n",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: ingress_acceptable_frames_get (section llp_filter)
 */
int
ui_ppd_api_llp_filter_ingress_acceptable_frames_get(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_port_profile_ndx;
  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT
    prm_vlan_format_ndx;
  SOC_PPC_ACTION_PROFILE
    prm_action_profile;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_filter");
  soc_sand_proc_name = "soc_ppd_llp_filter_ingress_acceptable_frames_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_ACTION_PROFILE_clear(&prm_action_profile);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_GET_INGRESS_ACCEPTABLE_FRAMES_GET_PORT_PROFILE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_GET_INGRESS_ACCEPTABLE_FRAMES_GET_PORT_PROFILE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_port_profile_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter port_profile_ndx after ingress_acceptable_frames_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_GET_INGRESS_ACCEPTABLE_FRAMES_GET_VLAN_FORMAT_NDX_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_GET_INGRESS_ACCEPTABLE_FRAMES_GET_VLAN_FORMAT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vlan_format_ndx = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vlan_format_ndx after ingress_acceptable_frames_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_llp_filter_ingress_acceptable_frames_get(
    unit,
    prm_port_profile_ndx,
    prm_vlan_format_ndx,
    &prm_action_profile
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_llp_filter_ingress_acceptable_frames_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_filter_ingress_acceptable_frames_get");
    goto exit;
  }

  send_string_to_screen("--> action_profile:", TRUE);
  SOC_PPC_ACTION_PROFILE_print(&prm_action_profile);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: designated_vlan_set (section llp_filter)
 */
int
ui_ppd_api_llp_filter_designated_vlan_set(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_PPC_PORT   
    prm_local_port_ndx;
  SOC_SAND_PP_VLAN_ID   
    prm_vid;
  uint8
    prm_accept;
  SOC_SAND_SUCCESS_FAILURE   
    prm_success;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_filter");
  soc_sand_proc_name = "soc_ppd_llp_filter_designated_vlan_set";

  unit = soc_ppd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_FILTER_DESIGNATED_VLAN_SET_DESIGNATED_VLAN_SET_LOCAL_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_FILTER_DESIGNATED_VLAN_SET_DESIGNATED_VLAN_SET_LOCAL_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_local_port_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter local_port_ndx after designated_vlan_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = soc_ppd_llp_filter_designated_vlan_get(
    unit,
    SOC_CORE_DEFAULT,
    prm_local_port_ndx,
    &prm_vid,
    &prm_accept
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_llp_filter_designated_vlan_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_filter_designated_vlan_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_FILTER_DESIGNATED_VLAN_SET_DESIGNATED_VLAN_SET_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_FILTER_DESIGNATED_VLAN_SET_DESIGNATED_VLAN_SET_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_FILTER_DESIGNATED_VLAN_SET_DESIGNATED_VLAN_SET_ACCEPT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_FILTER_DESIGNATED_VLAN_SET_DESIGNATED_VLAN_SET_ACCEPT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_accept = (uint8)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_llp_filter_designated_vlan_set(
    unit,
    SOC_CORE_DEFAULT,
    prm_local_port_ndx,
    prm_vid,
    prm_accept,
    &prm_success
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_llp_filter_designated_vlan_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_filter_designated_vlan_set");
    goto exit;
  }

  cli_out("success: %s\n\r",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: designated_vlan_get (section llp_filter)
 */
int
ui_ppd_api_llp_filter_designated_vlan_get(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_local_port_ndx;
  SOC_SAND_PP_VLAN_ID   
    prm_vid;
  uint8
    prm_accept;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_filter");
  soc_sand_proc_name = "soc_ppd_llp_filter_designated_vlan_get";

  unit = soc_ppd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_FILTER_DESIGNATED_VLAN_GET_DESIGNATED_VLAN_GET_LOCAL_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_LLP_FILTER_DESIGNATED_VLAN_GET_DESIGNATED_VLAN_GET_LOCAL_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_local_port_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter local_port_ndx after designated_vlan_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_llp_filter_designated_vlan_get(
    unit,
    SOC_CORE_DEFAULT,
    prm_local_port_ndx,
    &prm_vid,
    &prm_accept
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_llp_filter_designated_vlan_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_llp_filter_designated_vlan_get");
    goto exit;
  }

  send_string_to_screen("--> vid:", TRUE);
  cli_out("vid: %u\n",prm_vid);

  send_string_to_screen("--> accept:", TRUE);
  cli_out("accept: %u\n",prm_accept);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Section handler: llp_filter
 */
int
ui_ppd_api_llp_filter(
                      CURRENT_LINE *current_line
                      )
{
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_llp_filter");

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_SET_INGRESS_VLAN_MEMBERSHIP_SET_ID,1))
  {
    ui_ret = ui_ppd_api_llp_filter_ingress_vlan_membership_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_GET_INGRESS_VLAN_MEMBERSHIP_GET_ID,1))
  {
    ui_ret = ui_ppd_api_llp_filter_ingress_vlan_membership_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_ADD_INGRESS_VLAN_MEMBERSHIP_PORT_ADD_ID,1))
  {
    ui_ret = ui_ppd_api_llp_filter_ingress_vlan_membership_port_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_REMOVE_INGRESS_VLAN_MEMBERSHIP_PORT_REMOVE_ID,1))
  {
    ui_ret = ui_ppd_api_llp_filter_ingress_vlan_membership_port_remove(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_SET_INGRESS_ACCEPTABLE_FRAMES_SET_ID,1))
  {
    ui_ret = ui_ppd_api_llp_filter_ingress_acceptable_frames_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_GET_INGRESS_ACCEPTABLE_FRAMES_GET_ID,1))
  {
    ui_ret = ui_ppd_api_llp_filter_ingress_acceptable_frames_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_FILTER_DESIGNATED_VLAN_SET_DESIGNATED_VLAN_SET_ID,1))
  {
    ui_ret = ui_ppd_api_llp_filter_designated_vlan_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_LLP_FILTER_DESIGNATED_VLAN_GET_DESIGNATED_VLAN_GET_ID,1))
  {
    ui_ret = ui_ppd_api_llp_filter_designated_vlan_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after llp_filter***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

          
#endif /* llp_filter */ 


#endif /* LINK_PPD_LIBRARIES */ 

