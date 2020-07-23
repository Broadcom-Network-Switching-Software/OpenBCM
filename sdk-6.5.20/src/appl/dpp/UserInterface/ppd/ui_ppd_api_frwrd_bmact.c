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
 

#include <soc/dpp/PPD/ppd_api_frwrd_bmact.h>                                                               
#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_frwrd_bmact.h>

#if LINK_PPD_LIBRARIES

#ifdef UI_FRWRD_BMACT
/********************************************************************
*  Function handler: init (section frwrd_bmact)
 */
int
ui_ppd_api_frwrd_bmact_init(
                                     CURRENT_LINE *current_line
                                     )
{
  uint32
    ret;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_bmact");
  soc_sand_proc_name = "soc_ppd_frwrd_bmact_init";

  unit = soc_ppd_get_default_unit();

  /* Call function */
  ret = soc_ppd_frwrd_bmact_init(
    unit
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_bmact_init - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_bmact_init");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: bvid_info_set (section frwrd_bmact)
 */
int
ui_ppd_api_frwrd_bmact_bvid_info_set(
                                     CURRENT_LINE *current_line
                                     )
{
  uint32
    ret;
  SOC_SAND_PP_VLAN_ID   
    prm_bvid_ndx;
  SOC_PPC_BMACT_BVID_INFO
    prm_bvid_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_bmact");
  soc_sand_proc_name = "soc_ppd_frwrd_bmact_bvid_info_set";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_BMACT_BVID_INFO_clear(&prm_bvid_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_BMACT_BVID_INFO_SET_BVID_INFO_SET_BVID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_BMACT_BVID_INFO_SET_BVID_INFO_SET_BVID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_bvid_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter bvid_ndx after bvid_info_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = soc_ppd_frwrd_bmact_bvid_info_get(
    unit,
    prm_bvid_ndx,
    &prm_bvid_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_bmact_bvid_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_bmact_bvid_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_BMACT_BVID_INFO_SET_BVID_INFO_SET_BVID_INFO_UKNOWN_DA_DEST_DEST_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_BMACT_BVID_INFO_SET_BVID_INFO_SET_BVID_INFO_UKNOWN_DA_DEST_DEST_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_bvid_info.default_forward_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_BMACT_BVID_INFO_SET_BVID_INFO_SET_BVID_INFO_B_FID_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_BMACT_BVID_INFO_SET_BVID_INFO_SET_BVID_INFO_B_FID_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_bvid_info.b_fid_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_BMACT_BVID_INFO_SET_BVID_INFO_SET_BVID_INFO_STP_TOPOLOGY_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_BMACT_BVID_INFO_SET_BVID_INFO_SET_BVID_INFO_STP_TOPOLOGY_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_bvid_info.stp_topology_id = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_frwrd_bmact_bvid_info_set(
    unit,
    prm_bvid_ndx,
    &prm_bvid_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_bmact_bvid_info_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_bmact_bvid_info_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: bvid_info_get (section frwrd_bmact)
 */
int
ui_ppd_api_frwrd_bmact_bvid_info_get(
                                     CURRENT_LINE *current_line
                                     )
{
  uint32
    ret;
  SOC_SAND_PP_VLAN_ID   
    prm_bvid_ndx;
  SOC_PPC_BMACT_BVID_INFO
    prm_bvid_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_bmact");
  soc_sand_proc_name = "soc_ppd_frwrd_bmact_bvid_info_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_BMACT_BVID_INFO_clear(&prm_bvid_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_BMACT_BVID_INFO_GET_BVID_INFO_GET_BVID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_BMACT_BVID_INFO_GET_BVID_INFO_GET_BVID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_bvid_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter bvid_ndx after bvid_info_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_frwrd_bmact_bvid_info_get(
    unit,
    prm_bvid_ndx,
    &prm_bvid_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_bmact_bvid_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_bmact_bvid_info_get");
    goto exit;
  }

  send_string_to_screen("--> bvid_info:", TRUE);
  SOC_PPC_BMACT_BVID_INFO_print(&prm_bvid_info);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: pbb_te_bvid_range_set (section frwrd_bmact)
 */
int
ui_ppd_api_frwrd_bmact_pbb_te_bvid_range_set(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_PPC_BMACT_PBB_TE_VID_RANGE
    prm_pbb_te_bvids;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_bmact");
  soc_sand_proc_name = "soc_ppd_frwrd_bmact_pbb_te_bvid_range_set";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_BMACT_PBB_TE_VID_RANGE_clear(&prm_pbb_te_bvids);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_ppd_frwrd_bmact_pbb_te_bvid_range_get(
    unit,
    &prm_pbb_te_bvids
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_bmact_pbb_te_bvid_range_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_bmact_pbb_te_bvid_range_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_BMACT_PBB_TE_BVID_RANGE_SET_PBB_TE_BVID_RANGE_SET_PBB_TE_BVIDS_LAST_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_BMACT_PBB_TE_BVID_RANGE_SET_PBB_TE_BVID_RANGE_SET_PBB_TE_BVIDS_LAST_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pbb_te_bvids.last_vid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_BMACT_PBB_TE_BVID_RANGE_SET_PBB_TE_BVID_RANGE_SET_PBB_TE_BVIDS_FIRST_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_BMACT_PBB_TE_BVID_RANGE_SET_PBB_TE_BVID_RANGE_SET_PBB_TE_BVIDS_FIRST_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pbb_te_bvids.first_vid = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_frwrd_bmact_pbb_te_bvid_range_set(
    unit,
    &prm_pbb_te_bvids
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_bmact_pbb_te_bvid_range_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_bmact_pbb_te_bvid_range_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: pbb_te_bvid_range_get (section frwrd_bmact)
 */
int
ui_ppd_api_frwrd_bmact_pbb_te_bvid_range_get(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_PPC_BMACT_PBB_TE_VID_RANGE
    prm_pbb_te_bvids;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_bmact");
  soc_sand_proc_name = "soc_ppd_frwrd_bmact_pbb_te_bvid_range_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_BMACT_PBB_TE_VID_RANGE_clear(&prm_pbb_te_bvids);

  /* Get parameters */

  /* Call function */
  ret = soc_ppd_frwrd_bmact_pbb_te_bvid_range_get(
    unit,
    &prm_pbb_te_bvids
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_bmact_pbb_te_bvid_range_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_bmact_pbb_te_bvid_range_get");
    goto exit;
  }

  send_string_to_screen("--> pbb_te_bvids:", TRUE);
  SOC_PPC_BMACT_PBB_TE_VID_RANGE_print(&prm_pbb_te_bvids);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: entry_add (section frwrd_bmact)
 */
int
ui_ppd_api_frwrd_bmact_entry_add(
    CURRENT_LINE *current_line
    )
{
  uint32
    ret;
  SOC_PPC_BMACT_ENTRY_KEY
    prm_bmac_key;
  SOC_PPC_BMACT_ENTRY_INFO
    prm_bmact_entry_info;
  SOC_SAND_SUCCESS_FAILURE
    prm_success;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_bmact");
  soc_sand_proc_name = "soc_ppd_frwrd_bmact_entry_add";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_BMACT_ENTRY_KEY_clear(&prm_bmac_key);
  SOC_PPC_BMACT_ENTRY_INFO_clear(&prm_bmact_entry_info);

  /* Get parameters */
  if (!get_val_of(
    current_line,(int *)&match_index,SOC_PARAM_PPD_FRWRD_BMACT_ENTRY_ADD_ENTRY_ADD_BMAC_KEY_B_MAC_ADDR_ID,1,
    &param_val,VAL_TEXT,err_msg))
  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_bmac_key.b_mac_addr));
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_BMACT_ENTRY_ADD_ENTRY_ADD_BMAC_KEY_B_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_BMACT_ENTRY_ADD_ENTRY_ADD_BMAC_KEY_B_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_bmac_key.b_vid = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter bmac_key after entry_add***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_BMACT_ENTRY_ADD_ENTRY_ADD_BMAC_KEY_FLAGS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_BMACT_ENTRY_ADD_ENTRY_ADD_BMAC_KEY_FLAGS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_bmac_key.flags = (uint32)param_val->value.ulong_value;
  } else {
    prm_bmac_key.flags = SOC_PPC_BMACT_ENTRY_TYPE_FRWRD;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_BMACT_ENTRY_ADD_ENTRY_ADD_BMACT_ENTRY_INFO_DROP_SA_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_BMACT_ENTRY_ADD_ENTRY_ADD_BMACT_ENTRY_INFO_DROP_SA_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_bmact_entry_info.drop_sa = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_BMACT_ENTRY_ADD_ENTRY_ADD_BMACT_ENTRY_INFO_SA_LEARN_FEC_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_BMACT_ENTRY_ADD_ENTRY_ADD_BMACT_ENTRY_INFO_SA_LEARN_FEC_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_bmact_entry_info.sa_learn_fec_id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_BMACT_ENTRY_ADD_ENTRY_ADD_BMACT_ENTRY_INFO_I_SID_DOMAIN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_BMACT_ENTRY_ADD_ENTRY_ADD_BMACT_ENTRY_INFO_I_SID_DOMAIN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_bmact_entry_info.i_sid_domain = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_BMACT_ENTRY_ADD_ENTRY_ADD_BMACT_ENTRY_INFO_SYS_PORT_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_BMACT_ENTRY_ADD_ENTRY_ADD_BMACT_ENTRY_INFO_SYS_PORT_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_bmact_entry_info.sys_port_id = (uint32)param_val->value.ulong_value;
  }  

  /* Call function */
  ret = soc_ppd_frwrd_bmact_entry_add(
    unit,
    &prm_bmac_key,
    &prm_bmact_entry_info,
    &prm_success
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_bmact_entry_add - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_bmact_entry_add");
    goto exit;
  }

  send_string_to_screen("--> success:", TRUE);
  cli_out("success: %s\n",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: entry_remove (section frwrd_bmact)
 */
int
ui_ppd_api_frwrd_bmact_entry_remove(
    CURRENT_LINE *current_line
    )
{
  uint32
    ret;
  SOC_PPC_BMACT_ENTRY_KEY
    prm_bmac_key;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_bmact");
  soc_sand_proc_name = "soc_ppd_frwrd_bmact_entry_remove";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_BMACT_ENTRY_KEY_clear(&prm_bmac_key);

  /* Get parameters */
  if (!get_val_of(
    current_line,(int *)&match_index,SOC_PARAM_PPD_FRWRD_BMACT_ENTRY_REMOVE_ENTRY_REMOVE_BMAC_KEY_B_MAC_ADDR_ID,1,
    &param_val,VAL_TEXT,err_msg))
  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_bmac_key.b_mac_addr));
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_BMACT_ENTRY_REMOVE_ENTRY_REMOVE_BMAC_KEY_B_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_BMACT_ENTRY_REMOVE_ENTRY_REMOVE_BMAC_KEY_B_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_bmac_key.b_vid = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter bmac_key after entry_remove***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_BMACT_ENTRY_REMOVE_ENTRY_REMOVE_BMAC_KEY_FLAGS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_BMACT_ENTRY_REMOVE_ENTRY_REMOVE_BMAC_KEY_FLAGS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_bmac_key.flags = (uint32)param_val->value.ulong_value;
  } else {
    prm_bmac_key.flags = SOC_PPC_BMACT_ENTRY_TYPE_FRWRD;
  }

  /* Call function */
  ret = soc_ppd_frwrd_bmact_entry_remove(
    unit,
    &prm_bmac_key
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_bmact_entry_remove - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_bmact_entry_remove");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: entry_get (section frwrd_bmact)
 */
int
ui_ppd_api_frwrd_bmact_entry_get(
	CURRENT_LINE *current_line
	)
{
	uint32
		ret;
	SOC_PPC_BMACT_ENTRY_KEY
		prm_bmac_key;
	SOC_PPC_BMACT_ENTRY_INFO
		bmact_entry_info;
	uint8
		found;

	UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_bmact");
	soc_sand_proc_name = "soc_ppd_frwrd_bmact_entry_get";

	unit = soc_ppd_get_default_unit();
	SOC_PPC_BMACT_ENTRY_KEY_clear(&prm_bmac_key);

	/* Get parameters */
	if (!get_val_of(
		current_line,(int *)&match_index,SOC_PARAM_PPD_FRWRD_BMACT_ENTRY_GET_ENTRY_GET_BMAC_KEY_B_MAC_ADDR_ID,1,
		&param_val,VAL_TEXT,err_msg))
	{
		soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_bmac_key.b_mac_addr));
	}
	if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_BMACT_ENTRY_GET_ENTRY_GET_BMAC_KEY_B_VID_ID,1))
	{
		UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_BMACT_ENTRY_GET_ENTRY_GET_BMAC_KEY_B_VID_ID);
		UI_MACROS_CHECK_GET_VAL_OF_ERROR;
		prm_bmac_key.b_vid = (uint32)param_val->value.ulong_value;
	}
	else
	{
		send_string_to_screen(" *** SW error - expecting parameter bmac_key after entry_remove***", TRUE);
		goto exit;
	}

       if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_BMACT_ENTRY_GET_ENTRY_GET_BMAC_KEY_FLAGS_ID,1))
       {
         UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_BMACT_ENTRY_GET_ENTRY_GET_BMAC_KEY_FLAGS_ID);
         UI_MACROS_CHECK_GET_VAL_OF_ERROR;
         prm_bmac_key.flags = (uint32)param_val->value.ulong_value;
       } else {
         prm_bmac_key.flags = SOC_PPC_BMACT_ENTRY_TYPE_FRWRD;
       }

	/* Call function */
	ret = soc_ppd_frwrd_bmact_entry_get(
		unit,
		&prm_bmac_key,
		&bmact_entry_info,
		&found
		);
	if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
	{
		send_string_to_screen(" *** soc_ppd_frwrd_bmact_entry_get - FAIL", TRUE);
		soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_bmact_entry_get");
		goto exit;
	}

	send_string_to_screen("--> bmact_entry_info:", TRUE);
	SOC_PPC_BMACT_ENTRY_INFO_print(&bmact_entry_info);

	cli_out("found: %u\n\r",found);

	goto exit;
exit:
	return ui_ret;
}

int
ui_ppd_api_frwrd_bmact_edit_pcp_map_set(
    CURRENT_LINE *current_line
    )
{
    uint32
        ret;
    uint32
        prm_pcp_profile_ndx;
    SOC_SAND_PP_TC   
        prm_tc_ndx;
    SOC_SAND_PP_DP   
        prm_dp_ndx;
    SOC_SAND_PP_PCP_UP   
        prm_out_pcp;
    SOC_SAND_PP_DEI_CFI   
        prm_out_dei;

    UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_vlan_edit");
    soc_sand_proc_name = "soc_ppd_eg_vlan_edit_pcp_map_untagged_set";

    unit = soc_ppd_get_default_unit();

    /* Get parameters */
    if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_BMACT_PCP_MAP_SET_PCP_MAP_SET_PCP_PROFILE_NDX_ID,1))
    {
        UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_BMACT_PCP_MAP_SET_PCP_MAP_SET_PCP_PROFILE_NDX_ID);
        UI_MACROS_CHECK_GET_VAL_OF_ERROR;
        prm_pcp_profile_ndx = (uint32)param_val->value.ulong_value;
    }
    else
    {
        send_string_to_screen(" *** SW error - expecting parameter pcp_profile_ndx after pcp_map_untagged_set***", TRUE);
        goto exit;
    }

    if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_BMACT_PCP_MAP_SET_PCP_MAP_SET_TC_NDX_ID,1))
    {
        UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_BMACT_PCP_MAP_SET_PCP_MAP_SET_TC_NDX_ID);
        UI_MACROS_CHECK_GET_VAL_OF_ERROR;
        prm_tc_ndx = (uint8)param_val->value.ulong_value;
    }
    else
    {
        send_string_to_screen(" *** SW error - expecting parameter tc_ndx after pcp_map_untagged_set***", TRUE);
        goto exit;
    }

    if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_BMACT_PCP_MAP_SET_PCP_MAP_SET_DP_NDX_ID,1))
    {
        UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_BMACT_PCP_MAP_SET_PCP_MAP_SET_DP_NDX_ID);
        UI_MACROS_CHECK_GET_VAL_OF_ERROR;
        prm_dp_ndx = (uint8)param_val->value.ulong_value;
    }
    else
    {
        send_string_to_screen(" *** SW error - expecting parameter dp_ndx after pcp_map_untagged_set***", TRUE);
        goto exit;
    }

    /* This is a set function, so call GET function first */
    ret = soc_ppd_frwrd_bmact_eg_vlan_pcp_map_get(
        unit,
        prm_pcp_profile_ndx,
        prm_tc_ndx,
        prm_dp_ndx,
        &prm_out_pcp,
        &prm_out_dei
        );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
        send_string_to_screen(" *** soc_ppd_frwrd_bmact_eg_vlan_pcp_map_get - FAIL", TRUE);
        soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_bmact_eg_vlan_pcp_map_get");
        goto exit;
    }

    if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_BMACT_PCP_MAP_SET_PCP_MAP_SET_OUT_PCP_ID,1))
    {
        UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_BMACT_PCP_MAP_SET_PCP_MAP_SET_OUT_PCP_ID);
        UI_MACROS_CHECK_GET_VAL_OF_ERROR;
        prm_out_pcp = (uint8)param_val->value.ulong_value;
    }

    if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_BMACT_PCP_MAP_SET_PCP_MAP_SET_OUT_DEI_ID,1))
    {
        UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_BMACT_PCP_MAP_SET_PCP_MAP_SET_OUT_DEI_ID);
        UI_MACROS_CHECK_GET_VAL_OF_ERROR;
        prm_out_dei = (uint8)param_val->value.ulong_value;
    }

    /* Call function */
    ret = soc_ppd_frwrd_bmact_eg_vlan_pcp_map_set(
        unit,
        prm_pcp_profile_ndx,
        prm_tc_ndx,
        prm_dp_ndx,
        prm_out_pcp,
        prm_out_dei
        );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
        send_string_to_screen(" *** soc_ppd_frwrd_bmact_eg_vlan_pcp_map_set - FAIL", TRUE);
        soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_bmact_eg_vlan_pcp_map_set");
        goto exit;
    }

    goto exit;
exit:
    return ui_ret;
}

int
ui_ppd_api_frwrd_bmact_edit_pcp_map_get(
    CURRENT_LINE *current_line
    )
{
    uint32
        ret;
    uint32
        prm_pcp_profile_ndx;
    SOC_SAND_PP_TC   
        prm_tc_ndx;
    SOC_SAND_PP_DP   
        prm_dp_ndx;
    SOC_SAND_PP_PCP_UP   
        prm_out_pcp;
    SOC_SAND_PP_DEI_CFI   
        prm_out_dei;

    UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_vlan_edit");
    soc_sand_proc_name = "soc_ppd_eg_vlan_edit_pcp_map_untagged_get";

    unit = soc_ppd_get_default_unit();

    /* Get parameters */
    if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_BMACT_PCP_MAP_GET_PCP_MAP_GET_PCP_PROFILE_NDX_ID,1))
    {
        UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_BMACT_PCP_MAP_GET_PCP_MAP_GET_PCP_PROFILE_NDX_ID);
        UI_MACROS_CHECK_GET_VAL_OF_ERROR;
        prm_pcp_profile_ndx = (uint32)param_val->value.ulong_value;
    }
    else
    {
        send_string_to_screen(" *** SW error - expecting parameter pcp_profile_ndx after pcp_map_untagged_get***", TRUE);
        goto exit;
    }

    if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_BMACT_PCP_MAP_GET_PCP_MAP_GET_TC_NDX_ID,1))
    {
        UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_BMACT_PCP_MAP_GET_PCP_MAP_GET_TC_NDX_ID);
        UI_MACROS_CHECK_GET_VAL_OF_ERROR;
        prm_tc_ndx = (uint8)param_val->value.ulong_value;
    }
    else
    {
        send_string_to_screen(" *** SW error - expecting parameter tc_ndx after pcp_map_untagged_get***", TRUE);
        goto exit;
    }

    if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_BMACT_PCP_MAP_GET_PCP_MAP_GET_DP_NDX_ID,1))
    {
        UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_BMACT_PCP_MAP_GET_PCP_MAP_GET_DP_NDX_ID);
        UI_MACROS_CHECK_GET_VAL_OF_ERROR;
        prm_dp_ndx = (uint8)param_val->value.ulong_value;
    }
    else
    {
        send_string_to_screen(" *** SW error - expecting parameter dp_ndx after pcp_map_untagged_get***", TRUE);
        goto exit;
    }

    /* Call function */
    ret = soc_ppd_frwrd_bmact_eg_vlan_pcp_map_get(
        unit,
        prm_pcp_profile_ndx,
        prm_tc_ndx,
        prm_dp_ndx,
        &prm_out_pcp,
        &prm_out_dei
        );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
        send_string_to_screen(" *** soc_ppd_frwrd_bmact_eg_vlan_pcp_map_get - FAIL", TRUE);
        soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_bmact_eg_vlan_pcp_map_get");
        goto exit;
    }

    send_string_to_screen("--> out_pcp:", TRUE);
    cli_out("out_pcp: %u\n",prm_out_pcp);

    send_string_to_screen("--> out_dei:", TRUE);
    cli_out("out_dei: %u\n",prm_out_dei);

    goto exit;
exit:
    return ui_ret;
}

/********************************************************************
*  Section handler: frwrd_bmact
 */
int
ui_ppd_api_frwrd_bmact(
                       CURRENT_LINE *current_line
                       )
{
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_bmact");

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_BMACT_BVID_INFO_SET_BVID_INFO_SET_ID,1))
  {
    ui_ret = ui_ppd_api_frwrd_bmact_bvid_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_BMACT_BVID_INFO_GET_BVID_INFO_GET_ID,1))
  {
    ui_ret = ui_ppd_api_frwrd_bmact_bvid_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_BMACT_PBB_TE_BVID_RANGE_SET_PBB_TE_BVID_RANGE_SET_ID,1))
  {
    ui_ret = ui_ppd_api_frwrd_bmact_pbb_te_bvid_range_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_BMACT_PBB_TE_BVID_RANGE_GET_PBB_TE_BVID_RANGE_GET_ID,1))
  {
    ui_ret = ui_ppd_api_frwrd_bmact_pbb_te_bvid_range_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_BMACT_ENTRY_ADD_ENTRY_ADD_ID,1))
  {
    ui_ret = ui_ppd_api_frwrd_bmact_entry_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_BMACT_ENTRY_REMOVE_ENTRY_REMOVE_ID,1))
  {
    ui_ret = ui_ppd_api_frwrd_bmact_entry_remove(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_BMACT_ENTRY_GET_ENTRY_GET_ID,1))
  {
	  ui_ret = ui_ppd_api_frwrd_bmact_entry_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_BMACT_PCP_MAP_SET_PCP_MAP_SET_ID,1))
  {
	  ui_ret = ui_ppd_api_frwrd_bmact_edit_pcp_map_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_BMACT_PCP_MAP_GET_PCP_MAP_GET_ID,1))
  {
      ui_ret = ui_ppd_api_frwrd_bmact_edit_pcp_map_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after frwrd_bmact***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif
#endif /* LINK_PPD_LIBRARIES */ 

