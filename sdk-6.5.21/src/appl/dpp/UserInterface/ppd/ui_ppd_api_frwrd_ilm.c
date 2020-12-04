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
 

#include <soc/dpp/PPD/ppd_api_frwrd_ilm.h>                                                               
#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_frwrd_ilm.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_lif.h>

#if LINK_PPD_LIBRARIES

#ifdef UI_FRWRD_ILM
/********************************************************************
*  Function handler: glbl_info_set (section frwrd_ilm)
 */
int
ui_ppd_api_frwrd_ilm_glbl_info_set(
                                   CURRENT_LINE *current_line
                                   )
{
  uint32
    ret;
  uint32
    prm_exp_map_tbl_index = 0xFFFFFFFF;
  SOC_PPC_FRWRD_ILM_GLBL_INFO
    prm_glbl_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ilm");
  soc_sand_proc_name = "soc_ppd_frwrd_ilm_glbl_info_set";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_FRWRD_ILM_GLBL_INFO_clear(&prm_glbl_info);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_ppd_frwrd_ilm_glbl_info_get(
    unit,
    &prm_glbl_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_ilm_glbl_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ilm_glbl_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_ILM_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_ELSP_INFO_EXP_MAP_TBL_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_ILM_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_ELSP_INFO_EXP_MAP_TBL_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_exp_map_tbl_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_exp_map_tbl_index != 0xFFFFFFFF)
  {

    if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_ILM_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_ELSP_INFO_EXP_MAP_TBL_ID,1))
    {
      UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_ILM_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_ELSP_INFO_EXP_MAP_TBL_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      prm_glbl_info.elsp_info.exp_map_tbl[ prm_exp_map_tbl_index] = (uint8)param_val->value.ulong_value;
    }

  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_ILM_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_ELSP_INFO_LABELS_RANGE_END_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_ILM_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_ELSP_INFO_LABELS_RANGE_END_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_glbl_info.elsp_info.labels_range.end = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_ILM_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_ELSP_INFO_LABELS_RANGE_START_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_ILM_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_ELSP_INFO_LABELS_RANGE_START_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_glbl_info.elsp_info.labels_range.start = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_ILM_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_KEY_INFO_MASK_INRIF_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_ILM_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_KEY_INFO_MASK_INRIF_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_glbl_info.key_info.mask_inrif = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_ILM_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_KEY_INFO_MASK_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_ILM_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_KEY_INFO_MASK_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_glbl_info.key_info.mask_port = (uint8)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_frwrd_ilm_glbl_info_set(
    unit,
    &prm_glbl_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_ilm_glbl_info_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ilm_glbl_info_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: glbl_info_get (section frwrd_ilm)
 */
int
ui_ppd_api_frwrd_ilm_glbl_info_get(
                                   CURRENT_LINE *current_line
                                   )
{
  uint32
    ret;
  SOC_PPC_FRWRD_ILM_GLBL_INFO
    prm_glbl_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ilm");
  soc_sand_proc_name = "soc_ppd_frwrd_ilm_glbl_info_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_FRWRD_ILM_GLBL_INFO_clear(&prm_glbl_info);

  /* Get parameters */

  /* Call function */
  ret = soc_ppd_frwrd_ilm_glbl_info_get(
    unit,
    &prm_glbl_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_ilm_glbl_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ilm_glbl_info_get");
    goto exit;
  }

  send_string_to_screen("--> glbl_info:", TRUE);
  SOC_PPC_FRWRD_ILM_GLBL_INFO_print(&prm_glbl_info);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: add (section frwrd_ilm)
 */
int
ui_ppd_api_frwrd_ilm_add(
                         CURRENT_LINE *current_line
                         )
{
  uint32
    ret;
  SOC_PPC_FRWRD_ILM_KEY
    prm_ilm_key;
  SOC_PPC_FRWRD_DECISION_INFO
    prm_ilm_val;
  SOC_SAND_SUCCESS_FAILURE
    prm_success;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ilm");
  soc_sand_proc_name = "soc_ppd_frwrd_ilm_add";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_FRWRD_ILM_KEY_clear(&prm_ilm_key);
  SOC_PPC_FRWRD_DECISION_INFO_clear(&prm_ilm_val);

  
  

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_ILM_ADD_ADD_ILM_KEY_INRIF_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_ILM_ADD_ADD_ILM_KEY_INRIF_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ilm_key.inrif = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_ILM_ADD_ADD_ILM_KEY_IN_LOCAL_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_ILM_ADD_ADD_ILM_KEY_IN_LOCAL_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ilm_key.in_local_port = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_ILM_ADD_ADD_ILM_KEY_MAPPED_EXP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_ILM_ADD_ADD_ILM_KEY_MAPPED_EXP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ilm_key.mapped_exp = (uint8)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_ILM_ADD_ADD_ILM_KEY_IN_LABEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_ILM_ADD_ADD_ILM_KEY_IN_LABEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ilm_key.in_label = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ilm_key after add***", TRUE);
    goto exit;
  }

  ret = ui_ppd_frwrd_decision_set(
          current_line,
          1,
          &(prm_ilm_val)
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** ui_ppd_frwrd_decision_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "ui_ppd_frwrd_decision_set");
    goto exit;
  }

  if(prm_ilm_val.additional_info.eei.type == SOC_PPC_EEI_TYPE_MPLS) {
    prm_ilm_val.additional_info.eei.val.mpls_command.pop_next_header = SOC_TMC_PKT_FRWRD_TYPE_MPLS;
  }

  /* Call function */
  ret = soc_ppd_frwrd_ilm_add(
    unit,
    &prm_ilm_key,
    &prm_ilm_val,
    &prm_success
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_ilm_add - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ilm_add");
    goto exit;
  }

  send_string_to_screen("--> success:", TRUE);
  cli_out("success: %s\n",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: get (section frwrd_ilm)
 */
int
ui_ppd_api_frwrd_ilm_get(
                         CURRENT_LINE *current_line
                         )
{
  uint32
    ret;
  SOC_PPC_FRWRD_ILM_KEY
    prm_ilm_key;
  SOC_PPC_FRWRD_DECISION_INFO
    prm_ilm_val;
  uint8
    prm_found;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ilm");
  soc_sand_proc_name = "soc_ppd_frwrd_ilm_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_FRWRD_ILM_KEY_clear(&prm_ilm_key);
  SOC_PPC_FRWRD_DECISION_INFO_clear(&prm_ilm_val);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_ILM_GET_GET_ILM_KEY_INRIF_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_ILM_GET_GET_ILM_KEY_INRIF_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ilm_key.inrif = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_ILM_GET_GET_ILM_KEY_IN_LOCAL_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_ILM_GET_GET_ILM_KEY_IN_LOCAL_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ilm_key.in_local_port = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_ILM_GET_GET_ILM_KEY_MAPPED_EXP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_ILM_GET_GET_ILM_KEY_MAPPED_EXP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ilm_key.mapped_exp = (uint8)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_ILM_GET_GET_ILM_KEY_IN_LABEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_ILM_GET_GET_ILM_KEY_IN_LABEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ilm_key.in_label = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ilm_key after get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_frwrd_ilm_get(
    unit,
    &prm_ilm_key,
    &prm_ilm_val,
    &prm_found
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_ilm_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ilm_get");
    goto exit;
  }

  send_string_to_screen("--> ilm_val:", TRUE);
  SOC_PPC_FRWRD_DECISION_INFO_print(&prm_ilm_val);

  send_string_to_screen("--> found:", TRUE);
  cli_out("found: %u\n",prm_found);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: get_block (section frwrd_ilm)
 */
int
ui_ppd_api_frwrd_ilm_get_block(
                               CURRENT_LINE *current_line
                               )
{
  uint32
    ret;
  SOC_SAND_TABLE_BLOCK_RANGE
    prm_block_range;
  SOC_PPC_FRWRD_ILM_KEY
    prm_ilm_keys[10];
  SOC_PPC_FRWRD_DECISION_INFO
    prm_ilm_vals[10];
  uint32
    prm_nof_entries = 0,
    i;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ilm");
  soc_sand_proc_name = "soc_ppd_frwrd_ilm_get_block";

  unit = soc_ppd_get_default_unit();
  soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&prm_block_range);

  /* Get parameters */
  if (SOC_SAND_DEVICE_TYPE_GET(unit) == SOC_SAND_DEV_PCP)
  {
	  prm_block_range.entries_to_scan =( (2*1024*1024) + 32 -1); /* same as  PCP_FRWRD_MACT_TABLE_ENTRY_MAX; */
  }
  else /*Soc_petra-B*/
  {
    prm_block_range.entries_to_scan = 64*1024+32;
  }

  prm_block_range.entries_to_act = 10;

  cli_out(" ------------------------------------------------------------------------------\n\r"
          "|                                   ILM Table                                  |\n\r"
          "|------------------------------------------------------------------------------|\n\r"
          "|         |      |       |           | Destination  |     Additional Info      |\n\r"
          "|  Label  | Port | InRIF | MappedEXP | Type  | Val  | Type |                   |\n=\r"
          " ------------------------------------------------------------------------------ \n\r"
          );

  do 
  {
    /* Call function */
    ret = soc_ppd_frwrd_ilm_get_block(
            unit,
            &prm_block_range,
            prm_ilm_keys,
            prm_ilm_vals,
            &prm_nof_entries
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** soc_ppd_frwrd_ilm_get_block - FAIL", TRUE);
      soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ilm_get_block");
      goto exit;
    }

    for(i = 0; i < prm_nof_entries; ++i)
    {
      cli_out("| %-7d ", prm_ilm_keys[i].in_label);
      cli_out("| %-4d ", prm_ilm_keys[i].in_local_port);
      cli_out("| %-5d ", prm_ilm_keys[i].inrif);
      cli_out("| %-9d| ", prm_ilm_keys[i].mapped_exp);

      SOC_PPC_FRWRD_DECISION_INFO_print_table_format(
        "",
        &(prm_ilm_vals[i])
      );
      cli_out("\n\r");
    }
#ifdef PLISIM
    /* Only one iteration for chipsim*/
  prm_nof_entries = 0;
#endif


  } while(prm_nof_entries != 0);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: remove (section frwrd_ilm)
 */
int
ui_ppd_api_frwrd_ilm_remove(
                            CURRENT_LINE *current_line
                            )
{
  uint32
    ret;
  SOC_PPC_FRWRD_ILM_KEY
    prm_ilm_key;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ilm");
  soc_sand_proc_name = "soc_ppd_frwrd_ilm_remove";

  unit = soc_ppd_get_default_unit();
  SOC_PPC_FRWRD_ILM_KEY_clear(&prm_ilm_key);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_ILM_REMOVE_REMOVE_ILM_KEY_INRIF_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_ILM_REMOVE_REMOVE_ILM_KEY_INRIF_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ilm_key.inrif = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_ILM_REMOVE_REMOVE_ILM_KEY_IN_LOCAL_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_ILM_REMOVE_REMOVE_ILM_KEY_IN_LOCAL_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ilm_key.in_local_port = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_ILM_REMOVE_REMOVE_ILM_KEY_MAPPED_EXP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_ILM_REMOVE_REMOVE_ILM_KEY_MAPPED_EXP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ilm_key.mapped_exp = (uint8)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_ILM_REMOVE_REMOVE_ILM_KEY_IN_LABEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_ILM_REMOVE_REMOVE_ILM_KEY_IN_LABEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ilm_key.in_label = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ilm_key after remove***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_frwrd_ilm_remove(
    unit,
    &prm_ilm_key
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_ilm_remove - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ilm_remove");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: table_clear (section frwrd_ilm)
 */
int
ui_ppd_api_frwrd_ilm_table_clear(
                                 CURRENT_LINE *current_line
                                 )
{
  uint32
    ret;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ilm");
  soc_sand_proc_name = "soc_ppd_frwrd_ilm_table_clear";

  unit = soc_ppd_get_default_unit();

  /* Get parameters */

  /* Call function */
  ret = soc_ppd_frwrd_ilm_table_clear(
    unit
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_ilm_table_clear - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ilm_table_clear");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Section handler: frwrd_ilm
 */
int
ui_ppd_api_frwrd_ilm(
                     CURRENT_LINE *current_line
                     )
{
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_ilm");

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_ILM_GLBL_INFO_SET_GLBL_INFO_SET_ID,1))
  {
    ui_ret = ui_ppd_api_frwrd_ilm_glbl_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_ILM_GLBL_INFO_GET_GLBL_INFO_GET_ID,1))
  {
    ui_ret = ui_ppd_api_frwrd_ilm_glbl_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_ILM_ADD_ADD_ID,1))
  {
    ui_ret = ui_ppd_api_frwrd_ilm_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_ILM_GET_GET_ID,1))
  {
    ui_ret = ui_ppd_api_frwrd_ilm_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_ILM_GET_BLOCK_GET_BLOCK_ID,1))
  {
    ui_ret = ui_ppd_api_frwrd_ilm_get_block(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_ILM_REMOVE_REMOVE_ID,1))
  {
    ui_ret = ui_ppd_api_frwrd_ilm_remove(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_ILM_TABLE_CLEAR_TABLE_CLEAR_ID,1))
  {
    ui_ret = ui_ppd_api_frwrd_ilm_table_clear(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after frwrd_ilm***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif
#endif /* LINK_PPD_LIBRARIES */ 

