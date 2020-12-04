/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/* we need this junk function only to avoid building error of pedantic compilation */
void ___junk_function_ui_swp_p_pp_api(void){
}

#if PETRA_PP
/*
 * General include file for reference design.
 */
#include <appl/diag/dpp/ref_sys.h>
#include <bcm_app/dpp/../H/usrApp.h>

/*
 * Utilities include file.
 */

#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <appl/diag/dpp/utils_defi.h>
#include <appl/dpp/UserInterface/ui_defi.h>

#include <appl/dpp/UserInterface/ui_pure_defi_swp_p_pp_api.h>
#include <appl/dpp/UserInterface/ui_rom_defi_swp_p_pp_api.h>




#include <sweep/Petra/PP/swp_p_pp_app.h>
#include <sweep/Petra/PP/swp_p_pp_api_init.h>
#include <sweep/Petra/PP/swp_p_pp_api_vlan.h>
#include <sweep/Petra/PP/swp_p_pp_api_mact.h>
#include <sweep/Petra/PP/swp_p_pp_api_ipv4.h>
#include <sweep/Petra/PP/swp_p_pp_api_ipv4_mc.h>
#include <sweep/Petra/PP/swp_p_pp_api_ipv6.h>
#include <sweep/Petra/PP/swp_p_pp_api_acl.h>
#include <sweep/Petra/PP/swp_p_pp_api_fec.h>



static
  uint32
    Default_unit = 0;

#define UI_SWP_P_PP_APP
#define UI_INIT
#define UI_VLAN
#define UI_MACT
#define UI_IPV4_UC
#define UI_IPV4_MC
#define UI_FEC

#define UI_SWP_ACL_MASK_SET_ALL 0xFFFFFFFF
#define UI_SWP_P_PP_SCREEN_LINES 30
#define SWP_P_PP_UI_BLOCK_SIZE (UI_SWP_P_PP_SCREEN_LINES + 2)


void
  swp_p_pp_set_default_unit(uint32 dev_id)
{
  Default_unit = dev_id;
}

uint32
  swp_p_pp_get_default_unit()
{
  return Default_unit;
}



#ifdef UI_FEC
/********************************************************************
 *  Function handler: uc_ecmp_add (section fec)
 */
int
  ui_swp_p_pp_api_fec_uc_ecmp_add(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_SYSTEM_FEC_ID
    prm_sys_fec_ndx;
  SWP_P_PP_FEC_ADD_TYPE
    prm_add_type = SWP_P_PP_FEC_ADD_TYPE_CONCAT;
  SWP_P_PP_FEC_UC_ENTRY
    prm_uc_fec_array[SOC_PETRA_PP_ECMP_MAX_NOF_FECS];
  uint32
    prm_nof_entries,
    static_cntr_indx = 0,
    dest_val_indx = 0,
    cpu_code_indx = 0,
    out_vid_indx = 0,
    indx;
  uint32
    tmp,
    prm_up_to_sys_fec_ndx;
  SWP_P_PP_FEC_UC_RANGE_INFO
    prm_fec_uc_range_info;
  uint8
    prm_success;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_fec");
  soc_sand_proc_name = "swp_p_pp_fec_uc_ecmp_add";

  unit = swp_p_pp_get_default_unit();
  swp_p_pp_SWP_P_PP_FEC_UC_ENTRY_clear(prm_uc_fec_array);
  swp_p_pp_SWP_P_PP_FEC_UC_RANGE_INFO_clear(&prm_fec_uc_range_info);



  prm_uc_fec_array[0].action = SOC_SAND_PP_FEC_COMMAND_TYPE_ROUTE;
  prm_uc_fec_array[0].dest.dest_type = SOC_SAND_PP_DEST_SINGLE_PORT;
  prm_uc_fec_array[0].enable_rpf = FALSE;

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_UC_ECMP_ADD_UC_ECMP_ADD_SYS_FEC_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_FEC_UC_ECMP_ADD_UC_ECMP_ADD_SYS_FEC_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sys_fec_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter sys_fec_ndx after uc_ecmp_add***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_UC_ECMP_ADD_UC_ECMP_ADD_ADD_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_FEC_UC_ECMP_ADD_UC_ECMP_ADD_ADD_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_add_type = param_val->numeric_equivalent;
  }

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_SWP_P_PP_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_ARRAY_MAC_ADDR_ID,1,
         &param_val,VAL_TEXT,err_msg))
  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_uc_fec_array[0].mac_addr));
  }


  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_ARRAY_ENABLE_RPF_ID,1))
  {
    prm_uc_fec_array[0].enable_rpf = TRUE;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_ARRAY_DEST_DEST_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_ARRAY_DEST_DEST_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_uc_fec_array[0].dest.dest_type = (SOC_SAND_PP_DEST_TYPE)param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_ARRAY_ACTION_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_ARRAY_ACTION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_uc_fec_array[0].action = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_ARRAY_DEST_DEST_VAL_ID,1))
  {
    for (indx = 0; indx < 80; ++dest_val_indx, ++indx, ui_ret = 0)
    {
      UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(PARAM_SWP_P_PP_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_ARRAY_DEST_DEST_VAL_ID, indx + 1);
      if (ui_ret)
      {
        ui_ret = 0;
        break;
      }
      prm_uc_fec_array[dest_val_indx].dest.dest_val = (uint32)param_val->value.ulong_value;
    }
  }

  dest_val_indx = SOC_SAND_MAX(1, dest_val_indx);

  for (indx = 1; indx < dest_val_indx; ++indx)
  {
    tmp = prm_uc_fec_array[indx].dest.dest_val;
    sal_memcpy(
      &(prm_uc_fec_array[indx]),
      &(prm_uc_fec_array[0]),
      sizeof(SWP_P_PP_FEC_UC_ENTRY)
    );
    prm_uc_fec_array[indx].dest.dest_val = tmp;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_ARRAY_STATISTICS_COUNTER_ID,1))
  {
    for (indx = 0; indx < 80; ++static_cntr_indx, ++indx, ui_ret = 0)
    {
      UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(PARAM_SWP_P_PP_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_ARRAY_STATISTICS_COUNTER_ID, indx + 1);
      if (ui_ret)
      {
        ui_ret = 0;
        break;
      }
      prm_uc_fec_array[static_cntr_indx].statistics_counter = (uint32)param_val->value.ulong_value;
    }
  }


  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_ARRAY_CPU_CODE_ID,1))
  {
    for (indx = 0; indx < 80; ++cpu_code_indx, ++indx, ui_ret = 0)
    {
      UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(PARAM_SWP_P_PP_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_ARRAY_CPU_CODE_ID, indx + 1);
      if (ui_ret)
      {
        ui_ret = 0;
        break;
      }
      prm_uc_fec_array[cpu_code_indx].cpu_code = (uint32)param_val->value.ulong_value;
    }
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_ARRAY_OUT_VID_ID,1))
  {
    for (indx = 0; indx < 80; ++out_vid_indx, ++indx, ui_ret = 0)
    {
      UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(PARAM_SWP_P_PP_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_ARRAY_OUT_VID_ID, indx + 1);
      if (ui_ret)
      {
        ui_ret = 0;
        break;
      }
      prm_uc_fec_array[out_vid_indx].out_vid = (uint32)param_val->value.ulong_value;
    }
  }


  prm_nof_entries = dest_val_indx;

  for (indx = 0; indx < prm_nof_entries; ++indx )
  {
    if (cpu_code_indx > 0 && cpu_code_indx <= indx)
    {
      prm_uc_fec_array[indx].cpu_code = prm_uc_fec_array[cpu_code_indx - 1].cpu_code;
    }
    if (static_cntr_indx > 0 && static_cntr_indx <= indx)
    {
      prm_uc_fec_array[indx].statistics_counter = prm_uc_fec_array[static_cntr_indx - 1].statistics_counter;
    }
    if (out_vid_indx > 0 && out_vid_indx <= indx)
    {
      prm_uc_fec_array[indx].out_vid = prm_uc_fec_array[out_vid_indx - 1].out_vid;
    }
  }


 /*
  * for range
  */
  prm_up_to_sys_fec_ndx = prm_sys_fec_ndx;

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_RANGE_UP_TO_SYS_FEC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_RANGE_UP_TO_SYS_FEC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_up_to_sys_fec_ndx = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_RANGE_INC_MAC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_RANGE_INC_MAC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_fec_uc_range_info.inc_mac = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_RANGE_INC_CPU_CODE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_RANGE_INC_CPU_CODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_fec_uc_range_info.inc_cpu_code = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_RANGE_INC_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_RANGE_INC_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_fec_uc_range_info.inc_vid = (uint32)param_val->value.ulong_value;
  }

  if (prm_up_to_sys_fec_ndx == prm_sys_fec_ndx)
  {
    /* Call function */
    ret = swp_p_pp_fec_uc_ecmp_add(
            unit,
            prm_sys_fec_ndx,
            prm_add_type,
            prm_uc_fec_array,
            prm_nof_entries,
            &prm_success
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** swp_p_pp_fec_uc_ecmp_add - FAIL", TRUE);
      soc_petra_disp_result(ret, "swp_p_pp_fec_uc_ecmp_add");
      goto exit;
    }
  }
  else
  {
    ret = swp_p_pp_fec_uc_ecmp_range_add(
            unit,
            prm_sys_fec_ndx,
            prm_up_to_sys_fec_ndx,
            prm_add_type,
            prm_uc_fec_array,
            prm_nof_entries,
            &prm_fec_uc_range_info,
            &prm_success
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** swp_p_pp_fec_uc_ecmp_add - FAIL", TRUE);
      soc_petra_disp_result(ret, "swp_p_pp_fec_uc_ecmp_add");
      goto exit;
    }

  }

  send_string_to_screen(" ---> swp_p_pp_fec_uc_ecmp_add - SUCCEEDED", TRUE);
  send_string_to_screen("--> success:", TRUE);
  soc_sand_os_printf( "success: %u\n\r",prm_success);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: uc_ecmp_update_entry (section fec)
 */
int
  ui_swp_p_pp_api_fec_uc_ecmp_update_entry(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_SYSTEM_FEC_ID
    prm_sys_fec_ndx;
  uint32
    prm_entry_ndx;
  SWP_P_PP_FEC_UC_ENTRY
    prm_uc_fec_entry;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_fec");
  soc_sand_proc_name = "swp_p_pp_fec_uc_ecmp_update_entry";

  unit = swp_p_pp_get_default_unit();
  swp_p_pp_SWP_P_PP_FEC_UC_ENTRY_clear(&prm_uc_fec_entry);
  prm_uc_fec_entry.enable_rpf = FALSE;
  prm_uc_fec_entry.dest.dest_type = SOC_SAND_PP_DEST_SINGLE_PORT;
  prm_uc_fec_entry.action = SOC_SAND_PP_FEC_COMMAND_TYPE_ROUTE;

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_SYS_FEC_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_SYS_FEC_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sys_fec_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter sys_fec_ndx after uc_ecmp_update_entry***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_ENTRY_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_ENTRY_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_entry_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter entry_ndx after uc_ecmp_update_entry***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_UC_FEC_ENTRY_STATISTICS_COUNTER_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_UC_FEC_ENTRY_STATISTICS_COUNTER_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_uc_fec_entry.statistics_counter = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_UC_FEC_ENTRY_ENABLE_RPF_ID,1))
  {
    prm_uc_fec_entry.enable_rpf = TRUE;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_UC_FEC_ENTRY_CPU_CODE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_UC_FEC_ENTRY_CPU_CODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_uc_fec_entry.cpu_code = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_UC_FEC_ENTRY_OUT_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_UC_FEC_ENTRY_OUT_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_uc_fec_entry.out_vid = (uint32)param_val->value.ulong_value;
  }

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_SWP_P_PP_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_UC_FEC_ENTRY_MAC_ADDR_ID,1,
         &param_val,VAL_TEXT,err_msg))
  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_uc_fec_entry.mac_addr));
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_UC_FEC_ENTRY_DEST_DEST_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_UC_FEC_ENTRY_DEST_DEST_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_uc_fec_entry.dest.dest_val = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_UC_FEC_ENTRY_DEST_DEST_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_UC_FEC_ENTRY_DEST_DEST_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_uc_fec_entry.dest.dest_type = (SOC_SAND_PP_DEST_TYPE)param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_UC_FEC_ENTRY_ACTION_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_UC_FEC_ENTRY_ACTION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_uc_fec_entry.action = param_val->numeric_equivalent;
  }


  /* Call function */
  ret = swp_p_pp_fec_uc_ecmp_update_entry(
          unit,
          prm_sys_fec_ndx,
          prm_entry_ndx,
          &prm_uc_fec_entry
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_pp_fec_uc_ecmp_update_entry - FAIL", TRUE);
    soc_petra_disp_result(ret, "swp_p_pp_fec_uc_ecmp_update_entry");
    goto exit;
  }

  send_string_to_screen(" ---> swp_p_pp_fec_uc_ecmp_update_entry - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mc_fec_add (section fec)
 */
int
  ui_swp_p_pp_api_fec_mc_fec_add(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_SYSTEM_FEC_ID
    prm_sys_fec_ndx;
  SOC_SAND_PP_SYSTEM_FEC_ID
    prm_up_to_sys_fec_ndx;
  SWP_P_PP_FEC_ADD_TYPE
    prm_add_type = SWP_P_PP_FEC_ADD_TYPE_NEW_OVERRIDER;
  SWP_P_PP_FEC_MC_ENTRY
    prm_mc_fec_entry;
  SWP_P_PP_FEC_MC_RANGE_INFO
    prm_range_info;
  uint8
    prm_success;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_fec");
  soc_sand_proc_name = "swp_p_pp_fec_mc_fec_add";

  unit = swp_p_pp_get_default_unit();
  swp_p_pp_SWP_P_PP_FEC_MC_ENTRY_clear(&prm_mc_fec_entry);
  prm_mc_fec_entry.action = SOC_SAND_PP_FEC_COMMAND_TYPE_ROUTE;
  prm_mc_fec_entry.dest.dest_type = SOC_SAND_PP_DEST_MULTICAST;

  swp_p_pp_SWP_P_PP_FEC_MC_RANGE_INFO_clear(&prm_range_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_MC_FEC_ADD_MC_FEC_ADD_SYS_FEC_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_FEC_MC_FEC_ADD_MC_FEC_ADD_SYS_FEC_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sys_fec_ndx = (uint32)param_val->value.ulong_value;
    prm_up_to_sys_fec_ndx = prm_sys_fec_ndx;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter sys_fec_ndx after mc_fec_add***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_MC_FEC_ADD_MC_FEC_ADD_UP_TO_SYS_FEC_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_FEC_MC_FEC_ADD_MC_FEC_ADD_UP_TO_SYS_FEC_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_up_to_sys_fec_ndx = (uint32)param_val->value.ulong_value;
    prm_add_type = SWP_P_PP_FEC_ADD_TYPE_NEW_OVERRIDER;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_MC_FEC_ADD_MC_FEC_ADD_ADD_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_FEC_MC_FEC_ADD_MC_FEC_ADD_ADD_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_add_type = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_STATISTICS_COUNTER_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_STATISTICS_COUNTER_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_fec_entry.statistics_counter = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_CPU_CODE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_CPU_CODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_fec_entry.cpu_code = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_RPF_CHECK_EXP_IN_PORT_SYS_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_RPF_CHECK_EXP_IN_PORT_SYS_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_fec_entry.rpf_check.exp_in_port.sys_id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_RPF_CHECK_EXP_IN_PORT_SYS_PORT_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_RPF_CHECK_EXP_IN_PORT_SYS_PORT_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_fec_entry.rpf_check.exp_in_port.sys_port_type = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_RPF_CHECK_EXP_IN_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_RPF_CHECK_EXP_IN_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_fec_entry.rpf_check.exp_in_vid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_DEST_DEST_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_DEST_DEST_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_fec_entry.dest.dest_val = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_DEST_DEST_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_DEST_DEST_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_fec_entry.dest.dest_type = (SOC_SAND_PP_DEST_TYPE)param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_ACTION_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_ACTION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_fec_entry.action = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_RPF_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_RPF_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_fec_entry.rpf_check.rpf_check = param_val->numeric_equivalent;
  }

 /*
  * range info
  */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_MC_FEC_ADD_MC_FEC_ADD_RANGE_INFO_INC_CPU_CODE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_FEC_MC_FEC_ADD_MC_FEC_ADD_RANGE_INFO_INC_CPU_CODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_info.inc_cpu_code = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_MC_FEC_ADD_MC_FEC_ADD_RANGE_INFO_INC_DEST_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_FEC_MC_FEC_ADD_MC_FEC_ADD_RANGE_INFO_INC_DEST_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_info.inc_dest = (uint32)param_val->value.ulong_value;
  }


  if (prm_up_to_sys_fec_ndx == prm_sys_fec_ndx)
  {
    /* Call function */
    ret = swp_p_pp_fec_mc_fec_add(
            unit,
            prm_sys_fec_ndx,
            prm_add_type,
            &prm_mc_fec_entry,
            &prm_success
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** swp_p_pp_fec_mc_fec_add - FAIL", TRUE);
      soc_petra_disp_result(ret, "swp_p_pp_fec_mc_fec_add");
      goto exit;
    }
  }
  else
  {
    if (prm_add_type != SWP_P_PP_FEC_ADD_TYPE_NEW_OVERRIDER)
    {
      send_string_to_screen(" Add type can be new_override only for MC range add - FAIL", TRUE);
      goto exit;

    }
    /* Call function */
    ret = swp_p_pp_fec_mc_fec_range_add(
            unit,
            prm_sys_fec_ndx,
            prm_up_to_sys_fec_ndx,
            &prm_mc_fec_entry,
            &prm_range_info,
            &prm_success
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** swp_p_pp_fec_mc_fec_range_add - FAIL", TRUE);
      soc_petra_disp_result(ret, "swp_p_pp_fec_mc_fec_range_add");
      goto exit;
    }
  }

  send_string_to_screen(" ---> swp_p_pp_fec_mc_fec_add - SUCCEEDED", TRUE);
  send_string_to_screen("--> success:", TRUE);
  soc_sand_os_printf( "success: %u\n\r",prm_success);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: uc_emcp_remove_entry (section fec)
 */
int
  ui_swp_p_pp_api_fec_uc_emcp_remove_entry(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_SYSTEM_FEC_ID
    prm_sys_fec_ndx;
  uint32
    prm_fec_entry_ndx;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_fec");
  soc_sand_proc_name = "swp_p_pp_fec_uc_emcp_remove_entry";

  unit = swp_p_pp_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_UC_EMCP_REMOVE_ENTRY_UC_EMCP_REMOVE_ENTRY_SYS_FEC_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_FEC_UC_EMCP_REMOVE_ENTRY_UC_EMCP_REMOVE_ENTRY_SYS_FEC_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sys_fec_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter sys_fec_ndx after uc_emcp_remove_entry***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_UC_EMCP_REMOVE_ENTRY_UC_EMCP_REMOVE_ENTRY_FEC_ENTRY_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_FEC_UC_EMCP_REMOVE_ENTRY_UC_EMCP_REMOVE_ENTRY_FEC_ENTRY_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_fec_entry_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter fec_entry_ndx after uc_emcp_remove_entry***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = swp_p_pp_fec_uc_emcp_remove_entry(
          unit,
          prm_sys_fec_ndx,
          prm_fec_entry_ndx
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_pp_fec_uc_emcp_remove_entry - FAIL", TRUE);
    soc_petra_disp_result(ret, "swp_p_pp_fec_uc_emcp_remove_entry");
    goto exit;
  }

  send_string_to_screen(" ---> swp_p_pp_fec_uc_emcp_remove_entry - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: remove (section fec)
 */
int
  ui_swp_p_pp_api_fec_remove(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_SYSTEM_FEC_ID
    prm_sys_fec_ndx;
  SOC_SAND_PP_SYSTEM_FEC_ID
    prm_up_to_sys_fec_ndx;


  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_fec");
  soc_sand_proc_name = "swp_p_pp_fec_remove";

  unit = swp_p_pp_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_REMOVE_REMOVE_SYS_FEC_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_FEC_REMOVE_REMOVE_SYS_FEC_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sys_fec_ndx = (uint32)param_val->value.ulong_value;
    prm_up_to_sys_fec_ndx = prm_sys_fec_ndx;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter sys_fec_ndx after remove***", TRUE);
    goto exit;
  }


  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_REMOVE_UP_TO_SYS_FEC_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_FEC_REMOVE_UP_TO_SYS_FEC_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_up_to_sys_fec_ndx = (uint32)param_val->value.ulong_value;
  }

  if (prm_up_to_sys_fec_ndx == prm_sys_fec_ndx)
  {
    /* Call function */
    ret = swp_p_pp_fec_remove(
            unit,
            prm_sys_fec_ndx
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** swp_p_pp_fec_remove - FAIL", TRUE);
      soc_petra_disp_result(ret, "swp_p_pp_fec_remove");
      goto exit;
    }
  }
  else
  {
    /* Call function */
    ret = swp_p_pp_fec_range_remove(
            unit,
            prm_sys_fec_ndx,
            prm_up_to_sys_fec_ndx
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** swp_p_pp_fec_range_remove - FAIL", TRUE);
      soc_petra_disp_result(ret, "swp_p_pp_fec_range_remove");
      goto exit;
    }
  }
  send_string_to_screen(" ---> swp_p_pp_fec_remove - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: uc_get (section fec)
 */
int
  ui_swp_p_pp_api_fec_uc_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_SYSTEM_FEC_ID
    prm_sys_fec_ndx;
  SWP_P_PP_FEC_UC_ENTRY
    prm_uc_fec_array[SOC_PETRA_PP_ECMP_MAX_NOF_FECS];
  uint32
    prm_nof_entries;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_fec");
  soc_sand_proc_name = "swp_p_pp_fec_uc_get";

  unit = swp_p_pp_get_default_unit();
  swp_p_pp_SWP_P_PP_FEC_UC_ENTRY_clear(prm_uc_fec_array);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_UC_GET_UC_GET_SYS_FEC_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_FEC_UC_GET_UC_GET_SYS_FEC_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sys_fec_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter sys_fec_ndx after uc_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = swp_p_pp_fec_uc_get(
          unit,
          prm_sys_fec_ndx,
          prm_uc_fec_array,
          &prm_nof_entries
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_pp_fec_uc_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "swp_p_pp_fec_uc_get");
    goto exit;
  }

  send_string_to_screen(" ---> swp_p_pp_fec_uc_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> uc_fec_array:", TRUE);
  swp_p_pp_SWP_P_PP_FEC_UC_ENTRY_print(prm_uc_fec_array);

  send_string_to_screen("--> nof_entries:", TRUE);
  soc_sand_os_printf( "nof_entries: %d\n\r",prm_nof_entries);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mc_get (section fec)
 */
int
  ui_swp_p_pp_api_fec_mc_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_SYSTEM_FEC_ID
    prm_sys_fec_ndx;
  SWP_P_PP_FEC_MC_ENTRY
    prm_mc_fec;
  uint8
    prm_associated;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_fec");
  soc_sand_proc_name = "swp_p_pp_fec_mc_get";

  unit = swp_p_pp_get_default_unit();
  swp_p_pp_SWP_P_PP_FEC_MC_ENTRY_clear(&prm_mc_fec);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_MC_GET_MC_GET_SYS_FEC_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_FEC_MC_GET_MC_GET_SYS_FEC_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sys_fec_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter sys_fec_ndx after mc_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = swp_p_pp_fec_mc_get(
          unit,
          prm_sys_fec_ndx,
          &prm_mc_fec,
          &prm_associated
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_pp_fec_mc_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "swp_p_pp_fec_mc_get");
    goto exit;
  }

  send_string_to_screen(" ---> swp_p_pp_fec_mc_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> mc_fec:", TRUE);
  swp_p_pp_SWP_P_PP_FEC_MC_ENTRY_print(&prm_mc_fec);

  send_string_to_screen("--> associated:", TRUE);
  soc_sand_os_printf( "associated: %u\n\r",prm_associated);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: table_uc_get_block (section fec)
 */
int
  ui_swp_p_pp_api_fec_table_uc_get_block(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_PETRA_PP_BLOCK_RANGE
    prm_range;
  SOC_SAND_PP_SYSTEM_FEC_ID
    prm_sys_fec_ndx;
  SWP_P_PP_FEC_UC_ENTRY
    prm_uc_fec_table;
  uint8
    prm_is_first;
  uint32
    prm_nof_entries;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_fec");
  soc_sand_proc_name = "swp_p_pp_fec_table_uc_get_block";

  unit = swp_p_pp_get_default_unit();
  swp_p_pp_SWP_P_PP_FEC_UC_ENTRY_clear(&prm_uc_fec_table);

  /* Get parameters */

  /* Call function */
  ret = swp_p_pp_fec_table_uc_get_block(
          unit,
          &prm_range,
          &prm_sys_fec_ndx,
          &prm_uc_fec_table,
          &prm_is_first,
          &prm_nof_entries
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_pp_fec_table_uc_get_block - FAIL", TRUE);
    soc_petra_disp_result(ret, "swp_p_pp_fec_table_uc_get_block");
    goto exit;
  }

  send_string_to_screen(" ---> swp_p_pp_fec_table_uc_get_block - SUCCEEDED", TRUE);
  send_string_to_screen("--> sys_fec_ndx:", TRUE);
  soc_sand_os_printf( "sys_fec_ndx: %lu\n\r",prm_sys_fec_ndx);

  send_string_to_screen("--> uc_fec_table:", TRUE);
  swp_p_pp_SWP_P_PP_FEC_UC_ENTRY_print(&prm_uc_fec_table);

  send_string_to_screen("--> nof_entries:", TRUE);
  soc_sand_os_printf( "nof_entries: %lu\n\r",prm_nof_entries);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: table_mc_get_block (section fec)
 */
int
  ui_swp_p_pp_api_fec_table_mc_get_block(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_PETRA_PP_BLOCK_RANGE
    prm_range;
  SOC_SAND_PP_SYSTEM_FEC_ID
    prm_sys_fec_ndx;
  SWP_P_PP_FEC_MC_ENTRY
    prm_mc_fec_table;
  uint32
    prm_nof_entries;
  uint8
    prm_is_first;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_fec");
  soc_sand_proc_name = "swp_p_pp_fec_table_mc_get_block";

  unit = swp_p_pp_get_default_unit();
  swp_p_pp_SWP_P_PP_FEC_MC_ENTRY_clear(&prm_mc_fec_table);

  /* Get parameters */

  /* Call function */
  ret = swp_p_pp_fec_table_mc_get_block(
          unit,
          &prm_range,
          &prm_sys_fec_ndx,
          &prm_mc_fec_table,
          &prm_is_first,
          &prm_nof_entries
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_pp_fec_table_mc_get_block - FAIL", TRUE);
    soc_petra_disp_result(ret, "swp_p_pp_fec_table_mc_get_block");
    goto exit;
  }

  send_string_to_screen(" ---> swp_p_pp_fec_table_mc_get_block - SUCCEEDED", TRUE);
  send_string_to_screen("--> sys_fec_ndx:", TRUE);
  soc_sand_os_printf( "sys_fec_ndx: %lu\n\r",prm_sys_fec_ndx);

  send_string_to_screen("--> mc_fec_table:", TRUE);
  swp_p_pp_SWP_P_PP_FEC_MC_ENTRY_print(&prm_mc_fec_table);

  send_string_to_screen("--> nof_entries:", TRUE);
  soc_sand_os_printf( "nof_entries: %lu\n\r",prm_nof_entries);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: uc_ecmp_range_add (section fec)
 */
int
  ui_swp_p_pp_api_fec_uc_ecmp_range_add(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_SYSTEM_FEC_ID
    prm_sys_fec_ndx;
  SOC_SAND_PP_SYSTEM_FEC_ID
    prm_up_to_sys_fec_ndx;
  uint32
    prm_uc_fec_array_index = 0xFFFFFFFF;
  SWP_P_PP_FEC_UC_ENTRY
    prm_uc_fec_array[SOC_PETRA_PP_ECMP_MAX_NOF_FECS];
  uint32
    prm_nof_entries=1;
  SWP_P_PP_FEC_UC_RANGE_INFO
    prm_range_info;
  SWP_P_PP_FEC_ADD_TYPE
    prm_add_type = SWP_P_PP_FEC_ADD_TYPE_CONCAT;
  uint8
    prm_success;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_fec");
  soc_sand_proc_name = "swp_p_pp_fec_uc_ecmp_range_add";

  unit = swp_p_pp_get_default_unit();
  swp_p_pp_SWP_P_PP_FEC_UC_ENTRY_clear(prm_uc_fec_array);
  swp_p_pp_SWP_P_PP_FEC_UC_RANGE_INFO_clear(&prm_range_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_SYS_FEC_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_SYS_FEC_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sys_fec_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter sys_fec_ndx after uc_ecmp_range_add***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UP_TO_SYS_FEC_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UP_TO_SYS_FEC_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_up_to_sys_fec_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter up_to_sys_fec_ndx after uc_ecmp_range_add***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_uc_fec_array_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_uc_fec_array_index != 0xFFFFFFFF)
  {

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_STATISTICS_COUNTER_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_STATISTICS_COUNTER_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_uc_fec_array[ prm_uc_fec_array_index].statistics_counter = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_ENABLE_RPF_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_ENABLE_RPF_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_uc_fec_array[ prm_uc_fec_array_index].enable_rpf = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_CPU_CODE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_CPU_CODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_uc_fec_array[ prm_uc_fec_array_index].cpu_code = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_OUT_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_OUT_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_uc_fec_array[ prm_uc_fec_array_index].out_vid = (uint32)param_val->value.ulong_value;
  }

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_SWP_P_PP_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_MAC_ADDR_ID,1,
         &param_val,VAL_TEXT,err_msg))
  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_uc_fec_array[ prm_uc_fec_array_index].mac_addr));
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_DEST_DEST_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_DEST_DEST_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_uc_fec_array[ prm_uc_fec_array_index].dest.dest_val = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_DEST_DEST_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_DEST_DEST_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_uc_fec_array[ prm_uc_fec_array_index].dest.dest_type = (SOC_SAND_PP_DEST_TYPE)param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_ACTION_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_ACTION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_uc_fec_array[ prm_uc_fec_array_index].action = param_val->numeric_equivalent;
  }

  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_NOF_ENTRIES_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_NOF_ENTRIES_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_nof_entries = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_RANGE_INFO_INC_CPU_CODE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_RANGE_INFO_INC_CPU_CODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_info.inc_cpu_code = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_RANGE_INFO_INC_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_RANGE_INFO_INC_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_info.inc_vid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_RANGE_INFO_INC_MAC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_RANGE_INFO_INC_MAC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_info.inc_mac = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = swp_p_pp_fec_uc_ecmp_range_add(
          unit,
          prm_sys_fec_ndx,
          prm_up_to_sys_fec_ndx,
          prm_add_type,
          prm_uc_fec_array,
          prm_nof_entries,
          &prm_range_info,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_pp_fec_uc_ecmp_range_add - FAIL", TRUE);
    soc_petra_disp_result(ret, "swp_p_pp_fec_uc_ecmp_range_add");
    goto exit;
  }

  send_string_to_screen(" ---> swp_p_pp_fec_uc_ecmp_range_add - SUCCEEDED", TRUE);
  send_string_to_screen("--> success:", TRUE);
  soc_sand_os_printf( "success: %u\n\r",prm_success);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mc_fec_range_add (section fec)
 */
int
  ui_swp_p_pp_api_fec_mc_fec_range_add(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_SYSTEM_FEC_ID
    prm_sys_fec_ndx;
  SOC_SAND_PP_SYSTEM_FEC_ID
    prm_up_to_sys_fec_ndx;
  SWP_P_PP_FEC_MC_ENTRY
    prm_mc_fec_entry;
  SWP_P_PP_FEC_MC_RANGE_INFO
    prm_range_info;
  uint8
    prm_success;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_fec");
  soc_sand_proc_name = "swp_p_pp_fec_mc_fec_range_add";

  unit = swp_p_pp_get_default_unit();
  swp_p_pp_SWP_P_PP_FEC_MC_ENTRY_clear(&prm_mc_fec_entry);
  swp_p_pp_SWP_P_PP_FEC_MC_RANGE_INFO_clear(&prm_range_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_SYS_FEC_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_SYS_FEC_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sys_fec_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter sys_fec_ndx after mc_fec_range_add***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_UP_TO_SYS_FEC_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_UP_TO_SYS_FEC_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_up_to_sys_fec_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter up_to_sys_fec_ndx after mc_fec_range_add***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_STATISTICS_COUNTER_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_STATISTICS_COUNTER_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_fec_entry.statistics_counter = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_CPU_CODE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_CPU_CODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_fec_entry.cpu_code = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_RPF_CHECK_EXP_IN_PORT_SYS_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_RPF_CHECK_EXP_IN_PORT_SYS_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_fec_entry.rpf_check.exp_in_port.sys_id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_RPF_CHECK_EXP_IN_PORT_SYS_PORT_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_RPF_CHECK_EXP_IN_PORT_SYS_PORT_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_fec_entry.rpf_check.exp_in_port.sys_port_type = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_RPF_CHECK_EXP_IN_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_RPF_CHECK_EXP_IN_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_fec_entry.rpf_check.exp_in_vid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_DEST_DEST_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_DEST_DEST_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_fec_entry.dest.dest_val = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_DEST_DEST_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_DEST_DEST_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_fec_entry.dest.dest_type = (SOC_SAND_PP_DEST_TYPE)param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_ACTION_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_ACTION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_fec_entry.action = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_RANGE_INFO_INC_CPU_CODE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_RANGE_INFO_INC_CPU_CODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_info.inc_cpu_code = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_RANGE_INFO_INC_DEST_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_RANGE_INFO_INC_DEST_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_info.inc_dest = (uint32)param_val->value.ulong_value;
  }
  /* Call function */
  ret = swp_p_pp_fec_mc_fec_range_add(
          unit,
          prm_sys_fec_ndx,
          prm_up_to_sys_fec_ndx,
          &prm_mc_fec_entry,
          &prm_range_info,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_pp_fec_mc_fec_range_add - FAIL", TRUE);
    soc_petra_disp_result(ret, "swp_p_pp_fec_mc_fec_range_add");
    goto exit;
  }

  send_string_to_screen(" ---> swp_p_pp_fec_mc_fec_range_add - SUCCEEDED", TRUE);
  send_string_to_screen("--> success:", TRUE);
  soc_sand_os_printf( "success: %u\n\r",prm_success);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: range_remove (section fec)
 */
int
  ui_swp_p_pp_api_fec_range_remove(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_SYSTEM_FEC_ID
    prm_sys_fec_ndx;
  SOC_SAND_PP_SYSTEM_FEC_ID
    prm_up_to_sys_fec_ndx;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_fec");
  soc_sand_proc_name = "swp_p_pp_fec_range_remove";

  unit = swp_p_pp_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_RANGE_REMOVE_RANGE_REMOVE_SYS_FEC_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_FEC_RANGE_REMOVE_RANGE_REMOVE_SYS_FEC_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sys_fec_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter sys_fec_ndx after range_remove***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_RANGE_REMOVE_RANGE_REMOVE_UP_TO_SYS_FEC_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_FEC_RANGE_REMOVE_RANGE_REMOVE_UP_TO_SYS_FEC_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_up_to_sys_fec_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter up_to_sys_fec_ndx after range_remove***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = swp_p_pp_fec_range_remove(
          unit,
          prm_sys_fec_ndx,
          prm_up_to_sys_fec_ndx
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_pp_fec_range_remove - FAIL", TRUE);
    soc_petra_disp_result(ret, "swp_p_pp_fec_range_remove");
    goto exit;
  }

  send_string_to_screen(" ---> swp_p_pp_fec_range_remove - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: table_clear (section fec)
 */
int
  ui_swp_p_pp_api_fec_table_clear(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_fec");
  soc_sand_proc_name = "swp_p_pp_fec_table_clear";

  unit = swp_p_pp_get_default_unit();



  /* Call function */
  ret = swp_p_pp_fec_table_clear(
          unit
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_pp_fec_table_clear - FAIL", TRUE);
    soc_petra_disp_result(ret, "swp_p_pp_fec_table_clear");
    goto exit;
  }

  send_string_to_screen(" ---> swp_p_pp_fec_table_clear - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

#endif


#ifdef UI_SWP_P_PP_APP
/********************************************************************
 *  Function handler: app (section swp_p_pp_app)
 */
int
  ui_swp_p_pp_app_app(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint8
    prm_silent=TRUE;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_app");
  soc_sand_proc_name = "swp_p_pp_app";

  unit = swp_p_pp_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_APP_APP_SILENT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_APP_APP_SILENT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_silent = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = swp_p_pp_app_clear(
          unit,
          prm_silent
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_pp_app - FAIL", TRUE);

    goto exit;
  }


  send_string_to_screen(" ---> swp_p_pp_app - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: app_reset (section swp_p_pp_app)
 */
int
  ui_swp_p_pp_app_app_reset(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint8
    prm_silent = TRUE;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_app");
  soc_sand_proc_name = "swp_p_pp_app_reset";

  unit = swp_p_pp_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_APP_RESET_APP_RESET_SILENT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_APP_RESET_APP_RESET_SILENT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_silent = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = swp_p_pp_app_reset(
          unit,
          prm_silent
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_pp_app_reset - FAIL", TRUE);
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

#endif
#ifdef UI_INIT
/********************************************************************
 *  Function handler: sequence (section init)
 */
int
  ui_swp_p_pp_api_init_sequence(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SWP_P_PP_INFO
    prm_sweep_info;
  uint8
    prm_silent=TRUE;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_init");
  soc_sand_proc_name = "swp_p_pp_api_device_init_sequence";

  unit = swp_p_pp_get_default_unit();
  swp_p_pp_SWP_P_PP_INFO_clear(&prm_sweep_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_NOF_ECMPS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_NOF_ECMPS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sweep_info.l3_info.nof_ecmps = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_NOF_MC_ROUTES_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_NOF_MC_ROUTES_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sweep_info.l3_info.nof_mc_routes = (uint32)param_val->value.ulong_value;
  }
/*
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_MC_FEC_STATISTICS_COUNTER_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_MC_FEC_STATISTICS_COUNTER_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sweep_info.l3_info.default_mc_fec.statistics_counter = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_MC_FEC_RPF_CHECK_EXP_IN_PORT_SYS_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_MC_FEC_RPF_CHECK_EXP_IN_PORT_SYS_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sweep_info.l3_info.default_mc_fec.rpf_check.exp_in_port.sys_id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_MC_FEC_RPF_CHECK_EXP_IN_PORT_SYS_PORT_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_MC_FEC_RPF_CHECK_EXP_IN_PORT_SYS_PORT_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sweep_info.l3_info.default_mc_fec.rpf_check.exp_in_port.sys_port_type = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_MC_FEC_RPF_CHECK_EXP_IN_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_MC_FEC_RPF_CHECK_EXP_IN_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sweep_info.l3_info.default_mc_fec.rpf_check.exp_in_vid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_MC_FEC_RPF_CHECK_RPF_CHECK_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_MC_FEC_RPF_CHECK_RPF_CHECK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sweep_info.l3_info.default_mc_fec.rpf_check.rpf_check = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_MC_FEC_CPU_CODE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_MC_FEC_CPU_CODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sweep_info.l3_info.default_mc_fec.cpu_code = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_MC_FEC_FRWRD_DESTINATION_DEST_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_MC_FEC_FRWRD_DESTINATION_DEST_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sweep_info.l3_info.default_mc_fec.frwrd.destination.dest_val = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_MC_FEC_FRWRD_DESTINATION_DEST_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_MC_FEC_FRWRD_DESTINATION_DEST_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sweep_info.l3_info.default_mc_fec.frwrd.destination.dest_type = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_MC_FEC_FRWRD_OVERWRITE_COS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_MC_FEC_FRWRD_OVERWRITE_COS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sweep_info.l3_info.default_mc_fec.overwrite_cos = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_MC_FEC_FRWRD_COMMAND_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_MC_FEC_FRWRD_COMMAND_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sweep_info.l3_info.default_mc_fec.frwrd.command = param_val->numeric_equivalent;
  }
*/
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_UC_FEC_STATISTICS_COUNTER_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_UC_FEC_STATISTICS_COUNTER_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sweep_info.l3_info.default_fec.statistics_counter = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_UC_FEC_ENABLE_RPF_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_UC_FEC_ENABLE_RPF_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sweep_info.l3_info.default_fec.enable_rpf = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_UC_FEC_CPU_CODE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_UC_FEC_CPU_CODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sweep_info.l3_info.default_fec.cpu_code = (uint32)param_val->value.ulong_value;
  }

  if (!get_val_of(
    current_line,(int *)&match_index,PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_UC_FEC_L2_EDIT_ARP_PTR_ID,1,
    &param_val,VAL_TEXT,err_msg))
  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_sweep_info.l3_info.my_mac));
  }


  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_UC_FEC_L2_EDIT_OUT_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_UC_FEC_L2_EDIT_OUT_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sweep_info.l3_info.default_fec.out_vid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_UC_FEC_FRWRD_DESTINATION_DEST_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_UC_FEC_FRWRD_DESTINATION_DEST_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sweep_info.l3_info.default_fec.dest.dest_val = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_UC_FEC_FRWRD_DESTINATION_DEST_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_UC_FEC_FRWRD_DESTINATION_DEST_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sweep_info.l3_info.default_fec.dest.dest_type = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_UC_FEC_FRWRD_COMMAND_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_UC_FEC_FRWRD_COMMAND_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sweep_info.l3_info.default_fec.action = param_val->numeric_equivalent;
  }

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_MY_MAC_ID,1,
         &param_val,VAL_TEXT,err_msg))
  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_sweep_info.l3_info.my_mac));
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_L3_INIT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_L3_INIT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sweep_info.l3_info.l3_init = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L2_INFO_MACT_INFO_NOF_MC_MAC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L2_INFO_MACT_INFO_NOF_MC_MAC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sweep_info.l2_info.mact_info.nof_mc_mac = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L2_INFO_MACT_INFO_AGING_TIME_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L2_INFO_MACT_INFO_AGING_TIME_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sweep_info.l2_info.mact_info.aging_time = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L2_INFO_MACT_INFO_ENABLE_AGING_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L2_INFO_MACT_INFO_ENABLE_AGING_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sweep_info.l2_info.mact_info.enable_aging = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L2_INFO_MACT_INIT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L2_INFO_MACT_INIT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sweep_info.l2_info.mact_init = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L2_INFO_NOF_SYS_PORTS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L2_INFO_NOF_SYS_PORTS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sweep_info.l2_info.nof_sys_ports = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L2_INFO_NOF_VLANS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L2_INFO_NOF_VLANS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sweep_info.l2_info.nof_vlans = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_OP_MODE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_OP_MODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sweep_info.op_mode.qinq_enable = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SILENT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SILENT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_silent = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = swp_p_pp_api_device_init_sequence(
          unit,
          &prm_sweep_info,
          prm_silent
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_pp_api_device_init_sequence - FAIL", TRUE);

    goto exit;
  }

  send_string_to_screen(" ---> swp_p_pp_api_device_init_sequence - SUCCEEDED", TRUE);
  send_string_to_screen("--> unit:", TRUE);


  goto exit;
exit:
  return ui_ret;
}

#endif
#ifdef UI_VLAN
/********************************************************************
 *  Function handler: set (section vlan)
 */
int
  ui_swp_p_pp_api_vlan_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_vlan_ndx,
    indx,
    prm_up_to_vlan_ndx,
    prm_nof_ports;
  SWP_P_PP_VLAN_INFO
    prm_vlan_info;


  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_vlan");
  soc_sand_proc_name = "swp_p_pp_vlan_set";

  unit = swp_p_pp_get_default_unit();
  swp_p_pp_SWP_P_PP_VLAN_INFO_clear(&prm_vlan_info);
  prm_vlan_info.valid = TRUE;

  prm_vlan_info.topology = 0;

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_VLAN_SET_SET_VLAN_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_VLAN_SET_SET_VLAN_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vlan_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vlan_ndx after set***", TRUE);
    goto exit;
  }


  /* This is a set function, so call GET function first */
  ret = swp_p_pp_vlan_get(
          unit,
          prm_vlan_ndx,
          &prm_vlan_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_pp_vlan_get - FAIL", TRUE);

    goto exit;
  }
  prm_vlan_info.enable_flooding = TRUE;
  prm_up_to_vlan_ndx = prm_vlan_ndx;


  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_VLAN_SET_SET_VLAN_UP_TO_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_VLAN_SET_SET_VLAN_UP_TO_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_up_to_vlan_ndx = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_VLAN_SET_SET_VLAN_INFO_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_VLAN_SET_SET_VLAN_INFO_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vlan_info.valid = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_VLAN_SET_SET_VLAN_PORTS_ENABLE_FLDING_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_VLAN_SET_SET_VLAN_PORTS_ENABLE_FLDING_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vlan_info.enable_flooding = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_VLAN_SET_SET_VLAN_PORTS_TOPLOGY_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_VLAN_SET_SET_VLAN_PORTS_TOPLOGY_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vlan_info.topology = (uint8)param_val->value.ulong_value;
  }

  prm_nof_ports = 0;

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_VLAN_SET_SET_VLAN_MEMBERSHIP_PORTS_SET_MEMBER_PORTS_ID,1))
  {
    for (indx = 0; indx < 80; ++prm_nof_ports, ++indx, ui_ret = 0)
    {
      UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(PARAM_SWP_P_PP_VLAN_SET_SET_VLAN_MEMBERSHIP_PORTS_SET_MEMBER_PORTS_ID, indx + 1);
      if (ui_ret)
      {
        ui_ret = 0;
        break;
      }
      prm_vlan_info.membership.member_ports[prm_nof_ports] = (uint32)param_val->value.ulong_value;
      prm_vlan_info.membership.tagged[prm_nof_ports] = FALSE;
    }
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_VLAN_SET_SET_VLAN_PORTS_TAG_VALID_ID,1))
  {
    for (indx = 0 ;prm_nof_ports < 100; ++prm_nof_ports, ++indx, ui_ret = 0)
    {
      UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(PARAM_SWP_P_PP_VLAN_SET_SET_VLAN_PORTS_TAG_VALID_ID, indx + 1);
      if (ui_ret)
      {
        ui_ret = 0;
        break;
      }
      prm_vlan_info.membership.member_ports[prm_nof_ports] = (uint32)param_val->value.ulong_value;
      prm_vlan_info.membership.tagged[prm_nof_ports] = TRUE;
    }
  }

  prm_vlan_info.membership.nof_members = prm_nof_ports;

  for (indx = prm_vlan_ndx; indx <= prm_up_to_vlan_ndx; ++indx)
  {
    /* Call function */
    unit = 0;
    ret = swp_p_pp_vlan_set(
            unit,
            indx,
            &prm_vlan_info
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** swp_p_pp_vlan_set - FAIL", TRUE);

      goto exit;
    }
  }

  send_string_to_screen(" ---> swp_p_pp_vlan_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: get (section vlan)
 */
int
  ui_swp_p_pp_api_vlan_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_vlan_ndx;
  SWP_P_PP_VLAN_INFO
    prm_vlan_info;


  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_vlan");
  soc_sand_proc_name = "swp_p_pp_vlan_get";

  unit = swp_p_pp_get_default_unit();
  swp_p_pp_SWP_P_PP_VLAN_INFO_clear(&prm_vlan_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_VLAN_GET_GET_VLAN_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_VLAN_GET_GET_VLAN_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vlan_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vlan_ndx after get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = swp_p_pp_vlan_get(
          unit,
          prm_vlan_ndx,
          &prm_vlan_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_pp_vlan_get - FAIL", TRUE);

    goto exit;
  }

  send_string_to_screen(" ---> swp_p_pp_vlan_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> vlan_info:", TRUE);
  swp_p_pp_SWP_P_PP_VLAN_INFO_print(&prm_vlan_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: member_add (section vlan)
 */
int
  ui_swp_p_pp_api_vlan_member_add(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_vlan_ndx;
  uint32
    prm_sys_port;
  uint8
    prm_tagged=TRUE;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_vlan");
  soc_sand_proc_name = "swp_p_pp_vlan_member_add";

  unit = swp_p_pp_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_VLAN_MEMBER_ADD_MEMBER_ADD_VLAN_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_VLAN_MEMBER_ADD_MEMBER_ADD_VLAN_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vlan_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vlan_ndx after member_add***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_VLAN_MEMBER_ADD_MEMBER_ADD_SYS_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_VLAN_MEMBER_ADD_MEMBER_ADD_SYS_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sys_port = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter prm_sys_port after vlan_ndx***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_VLAN_MEMBER_ADD_MEMBER_ADD_TAGGED_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_VLAN_MEMBER_ADD_MEMBER_ADD_TAGGED_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_tagged = (uint8)param_val->value.ulong_value;
  }

  /* Call function */
  ret = gsa_distr_swp_p_pp_vlan_member_add(
          unit,
          prm_vlan_ndx,
          prm_sys_port,
          prm_tagged
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_pp_vlan_member_add - FAIL", TRUE);
    goto exit;
  }

  send_string_to_screen(" ---> swp_p_pp_vlan_member_add - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: member_remove (section vlan)
 */
int
  ui_swp_p_pp_api_vlan_member_remove(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_vlan_ndx;
  uint32
    prm_sys_port;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_vlan");
  soc_sand_proc_name = "swp_p_pp_vlan_member_remove";

  unit = swp_p_pp_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_VLAN_MEMBER_REMOVE_MEMBER_REMOVE_VLAN_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_VLAN_MEMBER_REMOVE_MEMBER_REMOVE_VLAN_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vlan_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vlan_ndx after member_remove***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_VLAN_MEMBER_REMOVE_MEMBER_REMOVE_SYS_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_VLAN_MEMBER_REMOVE_MEMBER_REMOVE_SYS_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sys_port = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vlan_ndx after member_add***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = swp_p_pp_vlan_member_remove(
          unit,
          prm_vlan_ndx,
          prm_sys_port
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_pp_vlan_member_remove - FAIL", TRUE);

    goto exit;
  }

  send_string_to_screen(" ---> swp_p_pp_vlan_member_remove - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

#endif
#ifdef UI_MACT
/********************************************************************
 *  Function handler: entry_add (section mact)
 */
int
  ui_swp_p_pp_api_mact_entry_add(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SWP_P_PP_MACT_KEY
    prm_key;
  SWP_P_PP_MACT_VALUE
    prm_value;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_mact");
  soc_sand_proc_name = "swp_p_pp_mact_entry_add";

  unit = swp_p_pp_get_default_unit();
  swp_p_pp_SWP_P_PP_MACT_KEY_clear(&prm_key);
  swp_p_pp_SWP_P_PP_MACT_VALUE_clear(&prm_value);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MACT_ENTRY_ADD_ENTRY_ADD_KEY_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_MACT_ENTRY_ADD_ENTRY_ADD_KEY_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_key.vid = (uint32)param_val->value.ulong_value;
  }

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_SWP_P_PP_MACT_ENTRY_ADD_ENTRY_ADD_KEY_MAC_ID,1,
         &param_val,VAL_TEXT,err_msg))
  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_key.mac));
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MACT_ENTRY_ADD_ENTRY_ADD_VALUE_DESTINATION_DEST_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_MACT_ENTRY_ADD_ENTRY_ADD_VALUE_DESTINATION_DEST_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_value.destination.dest_val = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MACT_ENTRY_ADD_ENTRY_ADD_VALUE_DESTINATION_DEST_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_MACT_ENTRY_ADD_ENTRY_ADD_VALUE_DESTINATION_DEST_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_value.destination.dest_type = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MACT_ENTRY_ADD_ENTRY_ADD_VALUE_FORWARD_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_MACT_ENTRY_ADD_ENTRY_ADD_VALUE_FORWARD_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_value.forward_type = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MACT_ENTRY_ADD_ENTRY_ADD_VALUE_IS_DYNAMIC_ID,1))
  {
    prm_value.is_dynamic = TRUE;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MACT_ENTRY_ADD_ENTRY_ADD_VALUE_SA_DROP_ID,1))
  {
    prm_value.sa_drop = TRUE;
  }

  /* Call function */
  ret = swp_p_pp_mact_entry_add(
          unit,
          &prm_key,
          &prm_value
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_pp_mact_entry_add - FAIL", TRUE);

    goto exit;
  }

  send_string_to_screen(" ---> swp_p_pp_mact_entry_add - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: range_add (section mact)
 */
int
  ui_swp_p_pp_api_mact_range_add(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SWP_P_PP_MACT_KEY
    prm_key;
  SOC_SAND_PP_MAC_ADDRESS
    prm_up_to_mac;
  SWP_P_PP_MACT_VALUE
    prm_value;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_mact");
  soc_sand_proc_name = "swp_p_pp_mact_range_add";

  unit = swp_p_pp_get_default_unit();
  swp_p_pp_SWP_P_PP_MACT_KEY_clear(&prm_key);
  swp_p_pp_SWP_P_PP_MACT_VALUE_clear(&prm_value);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MACT_RANGE_ADD_RANGE_ADD_KEY_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_MACT_RANGE_ADD_RANGE_ADD_KEY_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_key.vid = (uint32)param_val->value.ulong_value;
  }

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_SWP_P_PP_MACT_RANGE_ADD_RANGE_ADD_KEY_MAC_ID,1,
         &param_val,VAL_TEXT,err_msg))
  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_key.mac));
  }
  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_SWP_P_PP_MACT_RANGE_ADD_RANGE_ADD_UP_TO_MAC_ID,1,
         &param_val,VAL_TEXT,err_msg))
  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_up_to_mac));
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MACT_RANGE_ADD_RANGE_ADD_VALUE_DESTINATION_DEST_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_MACT_RANGE_ADD_RANGE_ADD_VALUE_DESTINATION_DEST_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_value.destination.dest_val = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MACT_RANGE_ADD_RANGE_ADD_VALUE_DESTINATION_DEST_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_MACT_RANGE_ADD_RANGE_ADD_VALUE_DESTINATION_DEST_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_value.destination.dest_type = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MACT_RANGE_ADD_RANGE_ADD_VALUE_FORWARD_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_MACT_RANGE_ADD_RANGE_ADD_VALUE_FORWARD_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_value.forward_type = param_val->numeric_equivalent;
  }


  /* Call function */
  ret = swp_p_pp_mact_range_add(
          unit,
          &prm_key,
          &prm_up_to_mac,
          &prm_value
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_pp_mact_range_add - FAIL", TRUE);

    goto exit;
  }

  send_string_to_screen(" ---> swp_p_pp_mact_range_add - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: entry_remove (section mact)
 */
int
  ui_swp_p_pp_api_mact_entry_remove(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SWP_P_PP_MACT_KEY
    prm_key;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_mact");
  soc_sand_proc_name = "swp_p_pp_mact_entry_remove";

  unit = swp_p_pp_get_default_unit();
  swp_p_pp_SWP_P_PP_MACT_KEY_clear(&prm_key);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MACT_ENTRY_REMOVE_ENTRY_REMOVE_KEY_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_MACT_ENTRY_REMOVE_ENTRY_REMOVE_KEY_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_key.vid = (uint32)param_val->value.ulong_value;
  }

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_SWP_P_PP_MACT_ENTRY_REMOVE_ENTRY_REMOVE_KEY_MAC_ID,1,
         &param_val,VAL_TEXT,err_msg))
  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_key.mac));
  }

  /* Call function */
  ret = swp_p_pp_mact_entry_remove(
          unit,
          &prm_key
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_pp_mact_entry_remove - FAIL", TRUE);

    goto exit;
  }

  send_string_to_screen(" ---> swp_p_pp_mact_entry_remove - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: range_remove (section mact)
 */
int
  ui_swp_p_pp_api_mact_range_remove(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SWP_P_PP_MACT_KEY
    prm_key;
  SOC_SAND_PP_MAC_ADDRESS
    prm_up_to_mac;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_mact");
  soc_sand_proc_name = "swp_p_pp_mact_range_remove";

  unit = swp_p_pp_get_default_unit();
  swp_p_pp_SWP_P_PP_MACT_KEY_clear(&prm_key);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MACT_RANGE_REMOVE_RANGE_REMOVE_KEY_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_MACT_RANGE_REMOVE_RANGE_REMOVE_KEY_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_key.vid = (uint32)param_val->value.ulong_value;
  }

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_SWP_P_PP_MACT_RANGE_REMOVE_RANGE_REMOVE_KEY_MAC_ID,1,
         &param_val,VAL_TEXT,err_msg))
  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_key.mac));
  }
  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_SWP_P_PP_MACT_RANGE_REMOVE_RANGE_REMOVE_UP_TO_MAC_ID,1,
         &param_val,VAL_TEXT,err_msg))
  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_up_to_mac));
  }

  /* Call function */
  ret = swp_p_pp_mact_range_remove(
          unit,
          &prm_key,
          &prm_up_to_mac
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_pp_mact_range_remove - FAIL", TRUE);

    goto exit;
  }

  send_string_to_screen(" ---> swp_p_pp_mact_range_remove - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: flush_block (section mact)
 */
int
  ui_swp_p_pp_api_mact_flush_block(
    CURRENT_LINE *current_line
  )
{
  SOC_PETRA_PP_MACT_FLUSH_INFO
    prm_flush_info;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_mact");
  soc_sand_proc_name = "swp_p_pp_mact_flush_block";

  unit = swp_p_pp_get_default_unit();
  soc_petra_pp_PETRA_PP_MACT_FLUSH_INFO_clear(&prm_flush_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MACT_FLUSH_BLOCK_FLUSH_BLOCK_FLUSH_INFO_NEW_DEST_DEST_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_MACT_FLUSH_BLOCK_FLUSH_BLOCK_FLUSH_INFO_NEW_DEST_DEST_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flush_info.new_dest.dest_val = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MACT_FLUSH_BLOCK_FLUSH_BLOCK_FLUSH_INFO_NEW_DEST_DEST_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_MACT_FLUSH_BLOCK_FLUSH_BLOCK_FLUSH_INFO_NEW_DEST_DEST_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flush_info.new_dest.dest_type = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MACT_FLUSH_BLOCK_FLUSH_BLOCK_FLUSH_INFO_RULE_INCLUDING_DYNAMIC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_MACT_FLUSH_BLOCK_FLUSH_BLOCK_FLUSH_INFO_RULE_INCLUDING_DYNAMIC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flush_info.rule.including_dynamic = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MACT_FLUSH_BLOCK_FLUSH_BLOCK_FLUSH_INFO_RULE_INCLUDING_STATIC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_MACT_FLUSH_BLOCK_FLUSH_BLOCK_FLUSH_INFO_RULE_INCLUDING_STATIC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flush_info.rule.including_static = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MACT_FLUSH_BLOCK_FLUSH_BLOCK_FLUSH_INFO_RULE_DEST_DEST_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_MACT_FLUSH_BLOCK_FLUSH_BLOCK_FLUSH_INFO_RULE_DEST_DEST_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flush_info.rule.dest.dest_val = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MACT_FLUSH_BLOCK_FLUSH_BLOCK_FLUSH_INFO_RULE_DEST_DEST_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_MACT_FLUSH_BLOCK_FLUSH_BLOCK_FLUSH_INFO_RULE_DEST_DEST_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flush_info.rule.dest.dest_type = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MACT_FLUSH_BLOCK_FLUSH_BLOCK_FLUSH_INFO_RULE_COMPARE_DEST_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_MACT_FLUSH_BLOCK_FLUSH_BLOCK_FLUSH_INFO_RULE_COMPARE_DEST_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flush_info.rule.compare_dest = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MACT_FLUSH_BLOCK_FLUSH_BLOCK_FLUSH_INFO_RULE_FID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_MACT_FLUSH_BLOCK_FLUSH_BLOCK_FLUSH_INFO_RULE_FID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flush_info.rule.fid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MACT_FLUSH_BLOCK_FLUSH_BLOCK_FLUSH_INFO_RULE_COMPARE_FID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_MACT_FLUSH_BLOCK_FLUSH_BLOCK_FLUSH_INFO_RULE_COMPARE_FID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flush_info.rule.compare_fid = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MACT_FLUSH_BLOCK_FLUSH_BLOCK_FLUSH_INFO_MODE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_MACT_FLUSH_BLOCK_FLUSH_BLOCK_FLUSH_INFO_MODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_flush_info.mode = param_val->numeric_equivalent;
  }


  /* Call function */

  send_string_to_screen(" ---> swp_p_pp_mact_flush_block - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: get_block (section mact)
 */
int
  ui_swp_p_pp_api_mact_get_block(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_PETRA_PP_MACT_ENTRIES_RANGE
    prm_range_info;
  SOC_PETRA_PP_MACT_MATCH_RULE
    prm_rule;
  SWP_P_PP_MACT_KEY
    prm_keys;
  SWP_P_PP_MACT_VALUE
    prm_values;
  uint32
    prm_nof_entries,
    total_entries;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_mact");
  soc_sand_proc_name = "swp_p_pp_mact_get_block";

  unit = swp_p_pp_get_default_unit();
  soc_petra_pp_PETRA_PP_MACT_ENTRIES_RANGE_clear(&prm_range_info);
  soc_petra_pp_PETRA_PP_MACT_MATCH_RULE_clear(&prm_rule);
  swp_p_pp_SWP_P_PP_MACT_KEY_clear(&prm_keys);
  swp_p_pp_SWP_P_PP_MACT_VALUE_clear(&prm_values);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MACT_GET_BLOCK_GET_BLOCK_RANGE_INFO_ENTRIES_TO_ACT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_MACT_GET_BLOCK_GET_BLOCK_RANGE_INFO_ENTRIES_TO_ACT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_info.entries_to_act = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MACT_GET_BLOCK_GET_BLOCK_RANGE_INFO_ENTRIES_TO_SCAN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_MACT_GET_BLOCK_GET_BLOCK_RANGE_INFO_ENTRIES_TO_SCAN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_info.entries_to_scan = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MACT_GET_BLOCK_GET_BLOCK_RANGE_INFO_ITER_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_MACT_GET_BLOCK_GET_BLOCK_RANGE_INFO_ITER_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_info.iter = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MACT_GET_BLOCK_GET_BLOCK_RULE_INCLUDING_DYNAMIC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_MACT_GET_BLOCK_GET_BLOCK_RULE_INCLUDING_DYNAMIC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_rule.including_dynamic = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MACT_GET_BLOCK_GET_BLOCK_RULE_INCLUDING_STATIC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_MACT_GET_BLOCK_GET_BLOCK_RULE_INCLUDING_STATIC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_rule.including_static = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MACT_GET_BLOCK_GET_BLOCK_RULE_DEST_DEST_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_MACT_GET_BLOCK_GET_BLOCK_RULE_DEST_DEST_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_rule.dest.dest_val = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MACT_GET_BLOCK_GET_BLOCK_RULE_DEST_DEST_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_MACT_GET_BLOCK_GET_BLOCK_RULE_DEST_DEST_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_rule.dest.dest_type = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MACT_GET_BLOCK_GET_BLOCK_RULE_COMPARE_DEST_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_MACT_GET_BLOCK_GET_BLOCK_RULE_COMPARE_DEST_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_rule.compare_dest = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MACT_GET_BLOCK_GET_BLOCK_RULE_FID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_MACT_GET_BLOCK_GET_BLOCK_RULE_FID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_rule.fid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MACT_GET_BLOCK_GET_BLOCK_RULE_COMPARE_FID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_MACT_GET_BLOCK_GET_BLOCK_RULE_COMPARE_FID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_rule.compare_fid = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = swp_p_pp_mact_get_block(
          unit,
          &prm_range_info,
          &prm_rule,
          &prm_keys,
          &prm_values,
          &prm_nof_entries,
          &total_entries
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_pp_mact_get_block - FAIL", TRUE);

    goto exit;
  }

  send_string_to_screen(" ---> swp_p_pp_mact_get_block - SUCCEEDED", TRUE);
  send_string_to_screen("--> keys:", TRUE);
  swp_p_pp_SWP_P_PP_MACT_KEY_print(&prm_keys);

  send_string_to_screen("--> values:", TRUE);
  swp_p_pp_SWP_P_PP_MACT_VALUE_print(&prm_values);

  send_string_to_screen("--> nof_entries:", TRUE);
  soc_sand_os_printf( "nof_entries: %lu\n\r",prm_nof_entries);

  send_string_to_screen("--> total_entries:", TRUE);
  soc_sand_os_printf( "nof_entries: %lu\n\r",total_entries);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: get_all (section mact)
 */
int
  ui_swp_p_pp_api_mact_get_all(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SWP_P_PP_MACT_KEY
    prm_keys;
  SWP_P_PP_MACT_VALUE
    prm_values;
  uint32
    prm_nof_entries;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_mact");
  soc_sand_proc_name = "swp_p_pp_mact_get_all";

  unit = swp_p_pp_get_default_unit();
  swp_p_pp_SWP_P_PP_MACT_KEY_clear(&prm_keys);
  swp_p_pp_SWP_P_PP_MACT_VALUE_clear(&prm_values);

  /* Get parameters */

  /* Call function */
  ret = swp_p_pp_mact_get_all(
          unit,
          &prm_keys,
          &prm_values,
          &prm_nof_entries
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_pp_mact_get_all - FAIL", TRUE);

    goto exit;
  }

  send_string_to_screen(" ---> swp_p_pp_mact_get_all - SUCCEEDED", TRUE);
  send_string_to_screen("--> keys:", TRUE);
  swp_p_pp_SWP_P_PP_MACT_KEY_print(&prm_keys);

  send_string_to_screen("--> values:", TRUE);
  swp_p_pp_SWP_P_PP_MACT_VALUE_print(&prm_values);

  send_string_to_screen("--> nof_entries:", TRUE);
  soc_sand_os_printf( "nof_entries: %lu\n\r",prm_nof_entries);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: print_all (section mact)
 */
int
  ui_swp_p_pp_api_mact_print_all(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_mact");
  soc_sand_proc_name = "swp_p_pp_mact_print_all";

  unit = swp_p_pp_get_default_unit();

  /* Get parameters */

  /* Call function */
  ret = swp_p_pp_mact_print_all(
          unit
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_pp_mact_print_all - FAIL", TRUE);

    goto exit;
  }

  send_string_to_screen(" ---> swp_p_pp_mact_print_all - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: aging_info_set (section mact)
 */
int
  ui_swp_p_pp_api_mact_aging_info_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_PETRA_PP_MACT_AGING_INFO
    prm_aging_info;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_mact");
  soc_sand_proc_name = "swp_p_pp_mact_aging_info_set";

  unit = swp_p_pp_get_default_unit();
  soc_petra_pp_PETRA_PP_MACT_AGING_INFO_clear(&prm_aging_info);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = swp_p_pp_mact_aging_info_get(
          unit,
          &prm_aging_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_pp_mact_aging_info_get - FAIL", TRUE);

    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MACT_AGING_INFO_SET_AGING_INFO_SET_AGING_INFO_META_CYCLE_NANO_SEC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_MACT_AGING_INFO_SET_AGING_INFO_SET_AGING_INFO_META_CYCLE_NANO_SEC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_aging_info.meta_cycle.nano_sec = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MACT_AGING_INFO_SET_AGING_INFO_SET_AGING_INFO_META_CYCLE_SEC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_MACT_AGING_INFO_SET_AGING_INFO_SET_AGING_INFO_META_CYCLE_SEC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_aging_info.meta_cycle.sec = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MACT_AGING_INFO_SET_AGING_INFO_SET_AGING_INFO_ENABLE_AGING_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_MACT_AGING_INFO_SET_AGING_INFO_SET_AGING_INFO_ENABLE_AGING_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_aging_info.enable_aging = (uint8)param_val->value.ulong_value;
  }


  /* Call function */
  ret = swp_p_pp_mact_aging_info_set(
          unit,
          &prm_aging_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_pp_mact_aging_info_set - FAIL", TRUE);

    goto exit;
  }

  send_string_to_screen(" ---> swp_p_pp_mact_aging_info_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: aging_info_get (section mact)
 */
int
  ui_swp_p_pp_api_mact_aging_info_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_PETRA_PP_MACT_AGING_INFO
    prm_aging_info;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_mact");
  soc_sand_proc_name = "swp_p_pp_mact_aging_info_get";

  unit = swp_p_pp_get_default_unit();
  soc_petra_pp_PETRA_PP_MACT_AGING_INFO_clear(&prm_aging_info);

  /* Get parameters */

  /* Call function */
  ret = swp_p_pp_mact_aging_info_get(
          unit,
          &prm_aging_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_pp_mact_aging_info_get - FAIL", TRUE);

    goto exit;
  }

  send_string_to_screen(" ---> swp_p_pp_mact_aging_info_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> aging_info:", TRUE);
  soc_petra_pp_PETRA_PP_MACT_AGING_INFO_print(&prm_aging_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: learn_mgmt_set (section mact)
 */
int
  ui_swp_p_pp_api_mact_learn_mgmt_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SWP_P_PP_MACT_LEARN_MGMT_INFO
    prm_learn_info;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_mact");
  soc_sand_proc_name = "swp_p_pp_mact_learn_mgmt_set";

  unit = swp_p_pp_get_default_unit();
  swp_p_pp_SWP_P_PP_MACT_LEARN_MGMT_INFO_clear(&prm_learn_info);
  prm_learn_info.olp_port_type  = SOC_PETRA_PORT_HEADER_TYPE_RAW;

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = swp_p_pp_mact_learn_mgmt_get(
          unit,
          &prm_learn_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_pp_mact_learn_mgmt_get - FAIL", TRUE);

    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MACT_LEARN_MGMT_SET_LEARN_MGMT_SET_LEARN_INFO_DESTINATION_DEST_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_MACT_LEARN_MGMT_SET_LEARN_MGMT_SET_LEARN_INFO_DESTINATION_DEST_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_learn_info.destination.dest_val = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MACT_LEARN_MGMT_SET_LEARN_MGMT_SET_LEARN_INFO_DESTINATION_DEST_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_MACT_LEARN_MGMT_SET_LEARN_MGMT_SET_LEARN_INFO_DESTINATION_DEST_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_learn_info.destination.dest_type = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MACT_LEARN_MGMT_SET_LEARN_MGMT_SET_LEARN_INFO_OPER_MODE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_MACT_LEARN_MGMT_SET_LEARN_MGMT_SET_LEARN_INFO_OPER_MODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_learn_info.oper_mode = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MACT_LEARN_MGMT_SET_LEARN_MGMT_SET_LEARN_INFO_OLP_PORT_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_MACT_LEARN_MGMT_SET_LEARN_MGMT_SET_LEARN_INFO_OLP_PORT_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_learn_info.olp_port_type = param_val->numeric_equivalent;
  }


  /* Call function */
  ret = swp_p_pp_mact_learn_mgmt_set(
          unit,
          &prm_learn_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_pp_mact_learn_mgmt_set - FAIL", TRUE);

    goto exit;
  }

  send_string_to_screen(" ---> swp_p_pp_mact_learn_mgmt_set - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: learn_mgmt_get (section mact)
 */
int
  ui_swp_p_pp_api_mact_learn_mgmt_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SWP_P_PP_MACT_LEARN_MGMT_INFO
    prm_learn_info;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_mact");
  soc_sand_proc_name = "swp_p_pp_mact_learn_mgmt_get";

  unit = swp_p_pp_get_default_unit();
  swp_p_pp_SWP_P_PP_MACT_LEARN_MGMT_INFO_clear(&prm_learn_info);

  /* Get parameters */

  /* Call function */
  ret = swp_p_pp_mact_learn_mgmt_get(
          unit,
          &prm_learn_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_pp_mact_learn_mgmt_get - FAIL", TRUE);

    goto exit;
  }

  send_string_to_screen(" ---> swp_p_pp_mact_learn_mgmt_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> learn_info:", TRUE);
  swp_p_pp_SWP_P_PP_MACT_LEARN_MGMT_INFO_print(&prm_learn_info);


  goto exit;
exit:
  return ui_ret;
}

#endif
#ifdef UI_IPV4_UC
/********************************************************************
 *  Function handler: route_add (section ipv4_uc)
 */
int
  ui_swp_p_pp_api_ipv4_uc_route_add(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_IPV4_SUBNET
    prm_subnet;
  uint32
    prm_sys_fec_id=0;
  uint8
    prm_override=FALSE;
  uint8
    prm_success;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_ipv4_uc");
  soc_sand_proc_name = "swp_p_pp_ipv4_uc_route_add";

  unit = swp_p_pp_get_default_unit();
  soc_sand_SAND_PP_IPV4_SUBNET_clear(&prm_subnet);

  prm_subnet.ip_address = 0xA000001;
  prm_subnet.prefix_len = 32;
  prm_override = 1;


  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_UC_ROUTE_ADD_ROUTE_ADD_SUBNET_PREFIX_LEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV4_UC_ROUTE_ADD_ROUTE_ADD_SUBNET_PREFIX_LEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_subnet.prefix_len = (uint8)param_val->value.ulong_value;
  }

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_SWP_P_PP_IPV4_UC_ROUTE_ADD_ROUTE_ADD_SUBNET_IP_ADDRESS_ID,1,
         &param_val,VAL_IP,err_msg))
  {
    prm_subnet.ip_address = param_val->value.ip_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_UC_ROUTE_ADD_ROUTE_ADD_OVERRIDE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV4_UC_ROUTE_ADD_ROUTE_ADD_OVERRIDE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_override = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_UC_ROUTE_ADD_ROUTE_ADD_SYS_FE_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV4_UC_ROUTE_ADD_ROUTE_ADD_SYS_FE_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sys_fec_id = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter sys_fec_id ***", TRUE);
    goto exit;
  }



  /* Call function */
  ret = swp_p_pp_ipv4_uc_route_add(
          unit,
          &prm_subnet,
          prm_sys_fec_id,
          prm_override,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_pp_ipv4_uc_route_add - FAIL", TRUE);

    goto exit;
  }

  send_string_to_screen(" ---> swp_p_pp_ipv4_uc_route_add - SUCCEEDED", TRUE);
  send_string_to_screen("--> success:", TRUE);
  soc_sand_os_printf( "success: %lu\n\r",prm_success);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: route_remove (section ipv4_uc)
 */
int
  ui_swp_p_pp_api_ipv4_uc_route_remove(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_IPV4_SUBNET
    prm_subnet;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_ipv4_uc");
  soc_sand_proc_name = "swp_p_pp_ipv4_uc_route_remove";

  unit = swp_p_pp_get_default_unit();
  soc_sand_SAND_PP_IPV4_SUBNET_clear(&prm_subnet);

  unit = swp_p_pp_get_default_unit();
  soc_sand_SAND_PP_IPV4_SUBNET_clear(&prm_subnet);

  prm_subnet.ip_address = 0xA000001;
  prm_subnet.prefix_len = 32;

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_UC_ROUTE_REMOVE_ROUTE_REMOVE_SUBNET_PREFIX_LEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV4_UC_ROUTE_REMOVE_ROUTE_REMOVE_SUBNET_PREFIX_LEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_subnet.prefix_len = (uint8)param_val->value.ulong_value;
  }

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_SWP_P_PP_IPV4_UC_ROUTE_REMOVE_ROUTE_REMOVE_SUBNET_IP_ADDRESS_ID,1,
         &param_val,VAL_IP,err_msg))
  {
    prm_subnet.ip_address = param_val->value.ip_value;
  }

  /* Call function */
  ret = swp_p_pp_ipv4_uc_route_remove(
          unit,
          &prm_subnet
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_pp_ipv4_uc_route_remove - FAIL", TRUE);

    goto exit;
  }

  send_string_to_screen(" ---> swp_p_pp_ipv4_uc_route_remove - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: range_add (section ipv4_uc)
 */
int
  ui_swp_p_pp_api_ipv4_uc_range_add(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SWP_P_PP_IPV4_RANGE
    prm_range_info;
  SOC_SAND_PP_SYSTEM_FEC_ID
    prm_sys_fec_ndx=0;
  uint8
    prm_success,
    prm_inc_sys_fec_id;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_ipv4_uc");
  soc_sand_proc_name = "swp_p_pp_ipv4_uc_range_add";

  unit = swp_p_pp_get_default_unit();
  swp_p_pp_SWP_P_PP_IPV4_RANGE_clear(&prm_range_info);

  prm_range_info.type = SWP_P_PP_IPV4_RANGE_TYPE_INC;
  prm_range_info.step = 1;
  prm_range_info.prefix = 32;
  prm_range_info.count = 1;
  prm_inc_sys_fec_id = FALSE;

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_UC_RANGE_ADD_RANGE_ADD_RANGE_INFO_STEP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV4_UC_RANGE_ADD_RANGE_ADD_RANGE_INFO_STEP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_info.step = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_UC_RANGE_ADD_RANGE_ADD_RANGE_INFO_COUNT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV4_UC_RANGE_ADD_RANGE_ADD_RANGE_INFO_COUNT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_info.count = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_UC_RANGE_ADD_RANGE_ADD_RANGE_INFO_PREFIX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV4_UC_RANGE_ADD_RANGE_ADD_RANGE_INFO_PREFIX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_info.prefix = (uint8)param_val->value.ulong_value;
  }

  if (!get_val_of(
    current_line,(int *)&match_index,PARAM_SWP_P_PP_IPV4_UC_RANGE_ADD_RANGE_ADD_RANGE_INFO_TO_ID,1,
    &param_val,VAL_IP,err_msg))
  {
    prm_range_info.to = param_val->value.ip_value;
  }

  if (!get_val_of(
    current_line,(int *)&match_index,PARAM_SWP_P_PP_IPV4_UC_RANGE_ADD_RANGE_ADD_RANGE_INFO_FROM_ID,1,
    &param_val,VAL_IP,err_msg))
  {
    prm_range_info.from = param_val->value.ip_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_UC_RANGE_ADD_RANGE_ADD_RANGE_INFO_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_IPV4_UC_RANGE_ADD_RANGE_ADD_RANGE_INFO_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_info.type = param_val->numeric_equivalent;
  }


  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_UC_RANGE_ADD_RANGE_ADD_SYS_FEC_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV4_UC_RANGE_ADD_RANGE_ADD_SYS_FEC_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sys_fec_ndx = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_UC_RANGE_ADD_RANGE_ADD_INC_SYS_FEC_ID_ID,1))
  {
    prm_inc_sys_fec_id = TRUE;
  }


  /* Call function */
  ret = swp_p_pp_ipv4_uc_range_add(
          unit,
          &prm_range_info,
          prm_sys_fec_ndx,
          prm_inc_sys_fec_id,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_pp_ipv4_uc_range_add - FAIL", TRUE);

    goto exit;
  }

  send_string_to_screen(" ---> swp_p_pp_ipv4_uc_range_add - SUCCEEDED", TRUE);
  send_string_to_screen("--> success:", TRUE);
  soc_sand_os_printf( "success: %lu\n\r",prm_success);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: range_remove (section ipv4_uc)
 */
int
  ui_swp_p_pp_api_ipv4_uc_range_remove(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SWP_P_PP_IPV4_RANGE
    prm_range_info;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_ipv4_uc");
  soc_sand_proc_name = "swp_p_pp_ipv4_uc_range_remove";

  unit = swp_p_pp_get_default_unit();
  swp_p_pp_SWP_P_PP_IPV4_RANGE_clear(&prm_range_info);
  prm_range_info.type = SWP_P_PP_IPV4_RANGE_TYPE_INC;
  prm_range_info.step = 1;
  prm_range_info.prefix = 24;
  prm_range_info.type = SWP_P_PP_IPV4_RANGE_TYPE_INC;


  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_UC_RANGE_REMOVE_RANGE_REMOVE_RANGE_INFO_STEP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV4_UC_RANGE_REMOVE_RANGE_REMOVE_RANGE_INFO_STEP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_info.step = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_UC_RANGE_REMOVE_RANGE_REMOVE_RANGE_INFO_COUNT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV4_UC_RANGE_REMOVE_RANGE_REMOVE_RANGE_INFO_COUNT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_info.count = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_UC_RANGE_REMOVE_RANGE_REMOVE_RANGE_INFO_PREFIX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV4_UC_RANGE_REMOVE_RANGE_REMOVE_RANGE_INFO_PREFIX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_info.prefix = (uint8)param_val->value.ulong_value;
  }

  if (!get_val_of(
    current_line,(int *)&match_index,PARAM_SWP_P_PP_IPV4_UC_RANGE_REMOVE_RANGE_REMOVE_RANGE_INFO_TO_ID,1,
    &param_val,VAL_IP,err_msg))
  {
    prm_range_info.to = param_val->value.ip_value;
  }


  if (!get_val_of(
    current_line,(int *)&match_index,PARAM_SWP_P_PP_IPV4_UC_RANGE_REMOVE_RANGE_REMOVE_RANGE_INFO_FROM_ID,1,
    &param_val,VAL_IP,err_msg))
  {
    prm_range_info.from = param_val->value.ip_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_UC_RANGE_REMOVE_RANGE_REMOVE_RANGE_INFO_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_IPV4_UC_RANGE_REMOVE_RANGE_REMOVE_RANGE_INFO_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_info.type = param_val->numeric_equivalent;
  }


  /* Call function */
  ret = swp_p_pp_ipv4_uc_range_remove(
          unit,
          &prm_range_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_pp_ipv4_uc_range_remove - FAIL", TRUE);

    goto exit;
  }

  send_string_to_screen(" ---> swp_p_pp_ipv4_uc_range_remove - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}



/********************************************************************
 *  Function handler: route_get_block (section ipv4_uc)
 */


/********************************************************************
 *  Function handler: ui_swp_p_pp_api_ipv4_uc_route_clear_all (section ipv4_uc)
 */
int
  ui_swp_p_pp_api_ipv4_uc_route_clear_all(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_ipv4_uc");
  soc_sand_proc_name = "swp_p_pp_ipv4_uc_route_clear_all";

  unit = swp_p_pp_get_default_unit();

  /* Get parameters */


  /* Call function */
  ret = swp_p_pp_ipv4_uc_routes_clear(
          unit
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_pp_ipv4_uc_routes_clear - FAIL", TRUE);

    goto exit;
  }

  send_string_to_screen(" ---> swp_p_pp_ipv4_uc_routes_clear - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

#endif

#ifdef UI_IPV4_MC


/********************************************************************
 *  Function handler: ipv4_mc_route_add (section ipv4_mc)
 */
int
  ui_swp_p_pp_api_ipv4_mc_ipv4_mc_init(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_PETRA_PP_IPV4_MC_VRF_INFO
    prm_init_info;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_ipv4_mc");
  soc_sand_proc_name = "swp_p_pp_ipv4_mc_init";


  unit = swp_p_pp_get_default_unit();
  soc_petra_pp_PETRA_PP_IPV4_MC_VRF_INFO_clear(&prm_init_info);




  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_INIT_USE_PORT_ID,1))
  {
    prm_init_info.use_mapped_port = TRUE;
  }

    /* Call function */
    ret = swp_p_pp_ipv4_mc_vrf_info_set(
            unit,
            &prm_init_info
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** swp_p_pp_ipv4_mc_route_add - FAIL", TRUE);
      soc_petra_disp_result(ret, "swp_p_pp_ipv4_mc_route_add");
      goto exit;
    }

  send_string_to_screen(" ---> swp_p_pp_ipv4_mc_init - SUCCEEDED", TRUE);


  goto exit;
exit:
  return ui_ret;
}
/********************************************************************
 *  Function handler: ipv4_mc_route_add (section ipv4_mc)
 */
int
  ui_swp_p_pp_api_ipv4_mc_ipv4_mc_route_add(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    mask_len,
    from;
  SOC_SAND_PP_IPV4_MC_ROUTE_KEY
    prm_route_key;
  SOC_SAND_PP_SYSTEM_FEC_ID
    prm_sys_fec_ndx=0;
  uint32
    prm_count,
    indx;
  uint8
    prm_override = FALSE,
    inc_sys_fec = FALSE;
  uint8
    prm_success;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_ipv4_mc");
  soc_sand_proc_name = "swp_p_pp_ipv4_mc_route_add";


  unit = swp_p_pp_get_default_unit();
  soc_sand_SAND_PP_IPV4_MC_ROUTE_KEY_clear(&prm_route_key);


  prm_route_key.vid.mask = 0xfff;
  prm_count = 1;

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_SYS_FEC_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_SYS_FEC_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sys_fec_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter sys_fec_ndx after ipv4_mc_route_add***", TRUE);
    goto exit;
  }


  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ROUTE_KEY_IN_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ROUTE_KEY_IN_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.vid.val = (uint32)param_val->value.ulong_value;
    prm_route_key.vid.mask = 0xFFF;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ROUTE_KEY_IN_VID_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ROUTE_KEY_IN_VID_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.vid.mask = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ROUTE_KEY_COUNT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ROUTE_KEY_COUNT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_count = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_INC_SYS_FEC_NDX_ID,1))
  {
    inc_sys_fec = TRUE;
  }


  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ROUTE_KEY_IN_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ROUTE_KEY_IN_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.port.val = (uint32)param_val->value.ulong_value;
    prm_route_key.port.mask = 0xFFFFFFFF;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ROUTE_KEY_IN_PORT_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ROUTE_KEY_IN_PORT_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.port.mask = ((uint8)param_val->value.ulong_value == 0)?0:0xFFFFFFFF;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ROUTE_KEY_GROUP_PREFIX_LEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ROUTE_KEY_GROUP_PREFIX_LEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.group.prefix_len = (uint8)param_val->value.ulong_value;
  }

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_SWP_P_PP_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ROUTE_KEY_GROUP_IP_ADDRESS_ID,1,
         &param_val,VAL_IP,err_msg))
  {
    prm_route_key.group.ip_address = param_val->value.ip_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ROUTE_KEY_SOURCE_PREFIX_LEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ROUTE_KEY_SOURCE_PREFIX_LEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.source.prefix_len = (uint8)param_val->value.ulong_value;
  }

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_SWP_P_PP_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ROUTE_KEY_SOURCE_IP_ADDRESS_ID,1,
         &param_val,VAL_IP,err_msg))
  {
    prm_route_key.source.ip_address = param_val->value.ip_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_OVERRIDE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_OVERRIDE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_override = (uint8)param_val->value.ulong_value;
  }

  for (indx = 0; indx < prm_count; ++indx )
  {
    /* Call function */
    ret = swp_p_pp_ipv4_mc_route_add(
            unit,
            &prm_route_key,
            prm_sys_fec_ndx,
            prm_override,
            &prm_success
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** swp_p_pp_ipv4_mc_route_add - FAIL", TRUE);
      soc_petra_disp_result(ret, "swp_p_pp_ipv4_mc_route_add");
      goto exit;
    }
    if (!prm_success)
    {
      break;
    }
    mask_len = 32 - prm_route_key.group.prefix_len;
    from = prm_route_key.group.ip_address >> mask_len;
    from += 1;
    from <<= mask_len;
    prm_route_key.group.ip_address = from;
    if (inc_sys_fec)
    {
      ++prm_sys_fec_ndx;
    }
  }

  send_string_to_screen(" ---> swp_p_pp_ipv4_mc_route_add - SUCCEEDED", TRUE);
  send_string_to_screen("--> success:", TRUE);
  soc_sand_os_printf( "success: %u\n\r",prm_success);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ipv4_mc_route_get (section ipv4_mc)
 */
int
  ui_swp_p_pp_api_ipv4_mc_ipv4_mc_route_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_IPV4_MC_ROUTE_KEY
    prm_route_key;
  SOC_SAND_PP_SYSTEM_FEC_ID
    prm_sys_fec_ndx=0;
  uint8
    prm_found;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_ipv4_mc");
  soc_sand_proc_name = "swp_p_pp_ipv4_mc_route_get";

  prm_route_key.vid.mask = 0;


  unit = swp_p_pp_get_default_unit();
  soc_sand_SAND_PP_IPV4_MC_ROUTE_KEY_clear(&prm_route_key);

  /* Get parameters */

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_MC_ROUTE_GET_IPV4_MC_ROUTE_GET_ROUTE_KEY_IN_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV4_MC_ROUTE_GET_IPV4_MC_ROUTE_GET_ROUTE_KEY_IN_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.port.val = (uint32)param_val->value.ulong_value;
    prm_route_key.port.mask = 0xFFFFFFFF;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_MC_ROUTE_GET_IPV4_MC_ROUTE_GET_ROUTE_KEY_IN_PORT_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV4_MC_ROUTE_GET_IPV4_MC_ROUTE_GET_ROUTE_KEY_IN_PORT_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.port.mask = ((uint8)param_val->value.ulong_value == 0)?0:0xFFFFFFFF;
  }


  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_MC_ROUTE_GET_IPV4_MC_ROUTE_GET_ROUTE_KEY_IN_VID_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV4_MC_ROUTE_GET_IPV4_MC_ROUTE_GET_ROUTE_KEY_IN_VID_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.vid.mask = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_MC_ROUTE_GET_IPV4_MC_ROUTE_GET_ROUTE_KEY_IN_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV4_MC_ROUTE_GET_IPV4_MC_ROUTE_GET_ROUTE_KEY_IN_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.vid.val = (uint32)param_val->value.ulong_value;
    prm_route_key.vid.mask = 0xFFF;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_MC_ROUTE_GET_IPV4_MC_ROUTE_GET_ROUTE_KEY_GROUP_PREFIX_LEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV4_MC_ROUTE_GET_IPV4_MC_ROUTE_GET_ROUTE_KEY_GROUP_PREFIX_LEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.group.prefix_len = (uint8)param_val->value.ulong_value;
  }

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_SWP_P_PP_IPV4_MC_ROUTE_GET_IPV4_MC_ROUTE_GET_ROUTE_KEY_GROUP_IP_ADDRESS_ID,1,
         &param_val,VAL_IP,err_msg))
  {
    prm_route_key.group.ip_address = param_val->value.ip_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_MC_ROUTE_GET_IPV4_MC_ROUTE_GET_ROUTE_KEY_SOURCE_PREFIX_LEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV4_MC_ROUTE_GET_IPV4_MC_ROUTE_GET_ROUTE_KEY_SOURCE_PREFIX_LEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.source.prefix_len = (uint8)param_val->value.ulong_value;
  }

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_SWP_P_PP_IPV4_MC_ROUTE_GET_IPV4_MC_ROUTE_GET_ROUTE_KEY_SOURCE_IP_ADDRESS_ID,1,
         &param_val,VAL_IP,err_msg))
  {
    prm_route_key.source.ip_address = param_val->value.ip_value;
  }


  /* Call function */
  ret = swp_p_pp_ipv4_mc_route_get(
          unit,
          &prm_route_key,
          &prm_sys_fec_ndx,
          &prm_found
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_pp_ipv4_mc_route_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "swp_p_pp_ipv4_mc_route_get");
    goto exit;
  }

  send_string_to_screen(" ---> swp_p_pp_ipv4_mc_route_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> sys_fec_ndx:", TRUE);
  soc_sand_os_printf( "sys_fec_ndx: %lu\n\r",prm_sys_fec_ndx);

  send_string_to_screen("--> found:", TRUE);
  soc_sand_os_printf( "found: %u\n\r",prm_found);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ipv4_mc_route_lpm_get (section ipv4_mc)
 */
int
  ui_swp_p_pp_api_ipv4_mc_ipv4_mc_route_lpm_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_IPV4_MC_ROUTE_KEY
    prm_route_key;
  SOC_SAND_PP_IPV4_MC_ROUTE_KEY
    prm_longest_match;
  uint8
    prm_found;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_ipv4_mc");
  soc_sand_proc_name = "swp_p_pp_ipv4_mc_route_lpm_get";

  unit = swp_p_pp_get_default_unit();
  soc_sand_SAND_PP_IPV4_MC_ROUTE_KEY_clear(&prm_route_key);
  soc_sand_SAND_PP_IPV4_MC_ROUTE_KEY_clear(&prm_longest_match);

  /* Get parameters */


  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_MC_ROUTE_LPM_GET_IPV4_MC_ROUTE_LPM_GET_ROUTE_KEY_IN_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV4_MC_ROUTE_LPM_GET_IPV4_MC_ROUTE_LPM_GET_ROUTE_KEY_IN_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.vid.val = (uint32)param_val->value.ulong_value;
    prm_route_key.vid.mask = 0xFFF;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_MC_ROUTE_LPM_GET_IPV4_MC_ROUTE_LPM_GET_ROUTE_KEY_IN_VID_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV4_MC_ROUTE_LPM_GET_IPV4_MC_ROUTE_LPM_GET_ROUTE_KEY_IN_VID_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.vid.mask = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_MC_ROUTE_LPM_GET_IPV4_MC_ROUTE_LPM_GET_ROUTE_KEY_IN_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV4_MC_ROUTE_LPM_GET_IPV4_MC_ROUTE_LPM_GET_ROUTE_KEY_IN_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.port.val = (uint32)param_val->value.ulong_value;
    prm_route_key.port.mask = 0xFFFFFFFF;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_MC_ROUTE_LPM_GET_IPV4_MC_ROUTE_LPM_GET_ROUTE_KEY_IN_PORT_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV4_MC_ROUTE_LPM_GET_IPV4_MC_ROUTE_LPM_GET_ROUTE_KEY_IN_PORT_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.port.mask = ((uint8)param_val->value.ulong_value == 0)?0:0xFFFFFFFF;
  }


  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_MC_ROUTE_LPM_GET_IPV4_MC_ROUTE_LPM_GET_ROUTE_KEY_GROUP_PREFIX_LEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV4_MC_ROUTE_LPM_GET_IPV4_MC_ROUTE_LPM_GET_ROUTE_KEY_GROUP_PREFIX_LEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.group.prefix_len = (uint8)param_val->value.ulong_value;
  }

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_SWP_P_PP_IPV4_MC_ROUTE_LPM_GET_IPV4_MC_ROUTE_LPM_GET_ROUTE_KEY_GROUP_IP_ADDRESS_ID,1,
         &param_val,VAL_IP,err_msg))
  {
    prm_route_key.group.ip_address = param_val->value.ip_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_MC_ROUTE_LPM_GET_IPV4_MC_ROUTE_LPM_GET_ROUTE_KEY_SOURCE_PREFIX_LEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV4_MC_ROUTE_LPM_GET_IPV4_MC_ROUTE_LPM_GET_ROUTE_KEY_SOURCE_PREFIX_LEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.source.prefix_len = (uint8)param_val->value.ulong_value;
  }

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_SWP_P_PP_IPV4_MC_ROUTE_LPM_GET_IPV4_MC_ROUTE_LPM_GET_ROUTE_KEY_SOURCE_IP_ADDRESS_ID,1,
         &param_val,VAL_IP,err_msg))
  {
    prm_route_key.source.ip_address = param_val->value.ip_value;
  }

  /* Call function */
  ret = swp_p_pp_ipv4_mc_route_lpm_get(
          unit,
          &prm_route_key,
          &prm_longest_match,
          &prm_found
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_pp_ipv4_mc_route_lpm_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "swp_p_pp_ipv4_mc_route_lpm_get");
    goto exit;
  }

  send_string_to_screen(" ---> swp_p_pp_ipv4_mc_route_lpm_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> longest_match:", TRUE);
  soc_sand_SAND_PP_IPV4_MC_ROUTE_KEY_print(&prm_longest_match);

  send_string_to_screen("--> found:", TRUE);
  soc_sand_os_printf( "found: %u\n\r",prm_found);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ipv4_mc_route_get_block (section ipv4_mc)
 */
int
  ui_swp_p_pp_api_ipv4_mc_ipv4_mc_route_get_block(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_PETRA_PP_BLOCK_RANGE
    prm_block_range;
  SOC_PETRA_PP_IPV4_MC_ROUTE_ENTRY
    prm_route_table[10];
  uint32
    prm_nof_entries;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_ipv4_mc");
  soc_sand_proc_name = "swp_p_pp_ipv4_mc_route_get_block";

  unit = swp_p_pp_get_default_unit();
  soc_petra_pp_PETRA_PP_IPV4_MC_ROUTE_ENTRY_clear(&prm_route_table[0]);
  prm_block_range.entries_to_act = 10000;
  prm_block_range.entries_to_scan = 10000;
  prm_block_range.iter = SOC_PETRA_PP_TCAM_LIST_ITER_BEGIN(10);



  /* Get parameters */

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_MC_ROUTE_GET_BLOCK_IPV4_MC_ROUTE_GET_BLOCK_BLOCK_RANGE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV4_MC_ROUTE_GET_BLOCK_IPV4_MC_ROUTE_GET_BLOCK_BLOCK_RANGE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_block_range.iter = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = swp_p_pp_ipv4_mc_route_get_block(
          unit,
          &prm_block_range,
          prm_route_table,
          &prm_nof_entries
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_pp_ipv4_mc_route_get_block - FAIL", TRUE);
    soc_petra_disp_result(ret, "swp_p_pp_ipv4_mc_route_get_block");
    goto exit;
  }

  soc_petra_pp_ipv4_multicast_print_block(
    unit,
    0,
    prm_route_table,
    prm_nof_entries,
    0
  );

  send_string_to_screen(" ---> swp_p_pp_ipv4_mc_route_get_block - SUCCEEDED", TRUE);
  send_string_to_screen("--> route_table:", TRUE);

  send_string_to_screen("--> nof_entries:", TRUE);
  soc_sand_os_printf( "nof_entries: %lu\n\r",prm_nof_entries);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ipv4_mc_route_remove (section ipv4_mc)
 */
int
  ui_swp_p_pp_api_ipv4_mc_ipv4_mc_route_remove(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_IPV4_MC_ROUTE_KEY
    prm_route_key;
  uint8
    prm_success;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_ipv4_mc");
  soc_sand_proc_name = "swp_p_pp_ipv4_mc_route_remove";

  prm_route_key.vid.mask = 0;


  unit = swp_p_pp_get_default_unit();
  soc_sand_SAND_PP_IPV4_MC_ROUTE_KEY_clear(&prm_route_key);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_MC_ROUTE_REMOVE_IPV4_MC_ROUTE_REMOVE_ROUTE_KEY_IN_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV4_MC_ROUTE_REMOVE_IPV4_MC_ROUTE_REMOVE_ROUTE_KEY_IN_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.vid.val = (uint32)param_val->value.ulong_value;
    prm_route_key.vid.mask = 0xFFF;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_MC_ROUTE_REMOVE_IPV4_MC_ROUTE_REMOVE_ROUTE_KEY_IN_VID_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV4_MC_ROUTE_REMOVE_IPV4_MC_ROUTE_REMOVE_ROUTE_KEY_IN_VID_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.vid.mask = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_MC_ROUTE_REMOVE_IPV4_MC_ROUTE_REMOVE_ROUTE_KEY_IN_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV4_MC_ROUTE_REMOVE_IPV4_MC_ROUTE_REMOVE_ROUTE_KEY_IN_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.port.val = (uint32)param_val->value.ulong_value;
    prm_route_key.port.mask = 0xFFFFFFFF;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_MC_ROUTE_REMOVE_IPV4_MC_ROUTE_REMOVE_ROUTE_KEY_IN_PORT_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV4_MC_ROUTE_REMOVE_IPV4_MC_ROUTE_REMOVE_ROUTE_KEY_IN_PORT_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.port.mask = ((uint8)param_val->value.ulong_value == 0)?0:0xFFFFFFFF;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_MC_ROUTE_REMOVE_IPV4_MC_ROUTE_REMOVE_ROUTE_KEY_GROUP_PREFIX_LEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV4_MC_ROUTE_REMOVE_IPV4_MC_ROUTE_REMOVE_ROUTE_KEY_GROUP_PREFIX_LEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.group.prefix_len = (uint8)param_val->value.ulong_value;
  }

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_SWP_P_PP_IPV4_MC_ROUTE_REMOVE_IPV4_MC_ROUTE_REMOVE_ROUTE_KEY_GROUP_IP_ADDRESS_ID,1,
         &param_val,VAL_IP,err_msg))
  {
    prm_route_key.group.ip_address = param_val->value.ip_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_MC_ROUTE_REMOVE_IPV4_MC_ROUTE_REMOVE_ROUTE_KEY_SOURCE_PREFIX_LEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV4_MC_ROUTE_REMOVE_IPV4_MC_ROUTE_REMOVE_ROUTE_KEY_SOURCE_PREFIX_LEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.source.prefix_len = (uint8)param_val->value.ulong_value;
  }

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_SWP_P_PP_IPV4_MC_ROUTE_REMOVE_IPV4_MC_ROUTE_REMOVE_ROUTE_KEY_SOURCE_IP_ADDRESS_ID,1,
         &param_val,VAL_IP,err_msg))
  {
    prm_route_key.source.ip_address = param_val->value.ip_value;
  }

  /* Call function */
  ret = swp_p_pp_ipv4_mc_route_remove(
          unit,
          &prm_route_key,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_pp_ipv4_mc_route_remove - FAIL", TRUE);
    soc_petra_disp_result(ret, "swp_p_pp_ipv4_mc_route_remove");
    goto exit;
  }

  send_string_to_screen(" ---> swp_p_pp_ipv4_mc_route_remove - SUCCEEDED", TRUE);
  send_string_to_screen("--> success:", TRUE);
  soc_sand_os_printf( "success: %u\n\r",prm_success);


  goto exit;
exit:
  return ui_ret;
}



#endif



#ifdef UI_IPV6

/********************************************************************
 *  Function handler: ipv6_route_add (section ipv6)
 */
int
  ui_swp_p_pp_api_ipv6_init(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_PETRA_PP_IPV6_VRF_INFO
    prm_init_info;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_ipv6");
  soc_sand_proc_name = "swp_p_pp_ipv6_init";


  unit = swp_p_pp_get_default_unit();
  soc_petra_pp_PETRA_PP_IPV6_VRF_INFO_clear(&prm_init_info);


  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV6_ROUTE_ADD_IPV6_ROUTE_INIT_USE_PORT_ID,1))
  {
    prm_init_info.use_mapped_port = TRUE;
  }

    /* Call function */
    ret = swp_p_pp_ipv6_vrf_info_set(
            unit,
            &prm_init_info
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** swp_p_pp_ipv6_route_add - FAIL", TRUE);
      soc_petra_disp_result(ret, "swp_p_pp_ipv6_route_add");
      goto exit;
    }

  send_string_to_screen(" ---> swp_p_pp_ipv6_init - SUCCEEDED", TRUE);


  goto exit;
exit:
  return ui_ret;
}
/********************************************************************
 *  Function handler: ipv6_route_add (section ipv6)
 */
int
  ui_swp_p_pp_api_ipv6_route_add(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    mask_len,
    from;
  uint32
    prm_count = 1,
    indx;
  SOC_SAND_PP_IPV6_ROUTE_KEY
    prm_route_key;
  SOC_SAND_PP_SYSTEM_FEC_ID
    prm_sys_fec_ndx=0;
  uint8
    prm_override = FALSE,
    inc_sys_fec = FALSE;
  uint8
    prm_success;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_ipv6");
  soc_sand_proc_name = "swp_p_pp_ipv6_route_add";


  unit = swp_p_pp_get_default_unit();
  soc_sand_SAND_PP_IPV6_ROUTE_KEY_clear(&prm_route_key);

  prm_route_key.vid.mask = 0;

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_SYS_FEC_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_SYS_FEC_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sys_fec_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter sys_fec_ndx after ipv6_route_add***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_INC_SYS_FEC_NDX_ID,1))
  {
    inc_sys_fec = TRUE;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_ROUTE_KEY_COUNT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_ROUTE_KEY_COUNT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_count = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_ROUTE_KEY_IN_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_ROUTE_KEY_IN_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.vid.val = (uint32)param_val->value.ulong_value;
    prm_route_key.vid.mask = 0xFFF;
  }


  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_ROUTE_KEY_IN_VID_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_ROUTE_KEY_IN_VID_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.vid.mask = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_ROUTE_KEY_IN_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_ROUTE_KEY_IN_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.port.val = (uint32)param_val->value.ulong_value;
    prm_route_key.port.mask = 0xFFFFFFFF;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_ROUTE_KEY_IN_PORT_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_ROUTE_KEY_IN_PORT_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.port.mask = ((uint8)param_val->value.ulong_value == 0)?0:0xFFFFFFFF;
  }


  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_ROUTE_KEY_GROUP_PREFIX_LEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_ROUTE_KEY_GROUP_PREFIX_LEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.dest.prefix_len = (uint8)param_val->value.ulong_value;
  }


  if (!get_val_of(
    current_line,(int *)&match_index,PARAM_SWP_P_PP_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_ROUTE_KEY_GROUP_IP_ADDRESS_ID,1,
    &param_val,VAL_TEXT,err_msg))
  {
    ret = soc_sand_pp_ipv6_address_string_parse(param_val->value.val_text, &(prm_route_key.dest.ipv6_address));
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** soc_sand_pp_ipv6_address_string_parse - FAIL", TRUE);
      soc_petra_disp_result(ret, "swp_p_pp_ipv6_route_add");
      goto exit;
    }
 }



  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_OVERRIDE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_OVERRIDE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_override = (uint8)param_val->value.ulong_value;
  }
  for (indx = 0; indx < prm_count; ++indx )
  {
    /* Call function */
    ret = swp_p_pp_ipv6_route_add(
            unit,
            &prm_route_key,
            prm_sys_fec_ndx,
            prm_override,
            &prm_success
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** swp_p_pp_ipv6_route_add - FAIL", TRUE);
      soc_petra_disp_result(ret, "swp_p_pp_ipv6_route_add");
      goto exit;
    }
    if (!prm_success)
    {
      break;
    }
    if (inc_sys_fec)
    {
      ++prm_sys_fec_ndx;
    }
    mask_len = (32 - (prm_route_key.dest.prefix_len % 32))%32;
    from = prm_route_key.dest.ipv6_address.address[(128 - prm_route_key.dest.prefix_len) /32] >> mask_len;
    from += 1;
    from <<= mask_len;
    prm_route_key.dest.ipv6_address.address[(128 - prm_route_key.dest.prefix_len) /32] = from;

  }

  /*prm_route_key.dest.ipv6_address*/

  send_string_to_screen(" ---> swp_p_pp_ipv6_route_add - SUCCEEDED", TRUE);
  send_string_to_screen("--> success:", TRUE);
  soc_sand_os_printf( "success: %u\n\r",prm_success);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ipv6_route_get (section ipv6)
 */
int
  ui_swp_p_pp_api_ipv6_route_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_IPV6_ROUTE_KEY
    prm_route_key;
  SOC_SAND_PP_SYSTEM_FEC_ID
    prm_sys_fec_ndx;
  uint8
    prm_found;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_ipv6");
  soc_sand_proc_name = "swp_p_pp_ipv6_route_get";

  prm_route_key.vid.mask = 0;


  unit = swp_p_pp_get_default_unit();
  soc_sand_SAND_PP_IPV6_ROUTE_KEY_clear(&prm_route_key);

  /* Get parameters */

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV6_ROUTE_GET_IPV6_ROUTE_GET_ROUTE_KEY_IN_PORT_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV6_ROUTE_GET_IPV6_ROUTE_GET_ROUTE_KEY_IN_PORT_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.port.mask = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV6_ROUTE_GET_IPV6_ROUTE_GET_ROUTE_KEY_IN_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV6_ROUTE_GET_IPV6_ROUTE_GET_ROUTE_KEY_IN_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.port.val = (uint32)param_val->value.ulong_value;
    prm_route_key.port.mask = 0xFFFFFFFF;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV6_ROUTE_GET_IPV6_ROUTE_GET_ROUTE_KEY_IN_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV6_ROUTE_GET_IPV6_ROUTE_GET_ROUTE_KEY_IN_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.vid.val = (uint32)param_val->value.ulong_value;
    prm_route_key.vid.mask = 0xFFF;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV6_ROUTE_GET_IPV6_ROUTE_GET_ROUTE_KEY_IN_VID_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV6_ROUTE_GET_IPV6_ROUTE_GET_ROUTE_KEY_IN_VID_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.port.mask = ((uint8)param_val->value.ulong_value == 0)?0:0xFFFFFFFF;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV6_ROUTE_GET_IPV6_ROUTE_GET_ROUTE_KEY_GROUP_PREFIX_LEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV6_ROUTE_GET_IPV6_ROUTE_GET_ROUTE_KEY_GROUP_PREFIX_LEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.dest.prefix_len = (uint8)param_val->value.ulong_value;
  }

  if (!get_val_of(
    current_line,(int *)&match_index,PARAM_SWP_P_PP_IPV6_ROUTE_GET_IPV6_ROUTE_GET_ROUTE_KEY_GROUP_IP_ADDRESS_ID,1,
    &param_val,VAL_TEXT,err_msg))
  {
    ret = soc_sand_pp_ipv6_address_string_parse(param_val->value.val_text, &(prm_route_key.dest.ipv6_address));
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** soc_sand_pp_ipv6_address_string_parse - FAIL", TRUE);
      soc_petra_disp_result(ret, "swp_p_pp_ipv6_route_get");
      goto exit;
    }
  }


  /* Call function */
  ret = swp_p_pp_ipv6_route_get(
          unit,
          &prm_route_key,
          &prm_sys_fec_ndx,
          &prm_found
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_pp_ipv6_route_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "swp_p_pp_ipv6_route_get");
    goto exit;
  }

  send_string_to_screen(" ---> swp_p_pp_ipv6_route_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> sys_fec_ndx:", TRUE);
  soc_sand_os_printf( "sys_fec_ndx: %lu\n\r",prm_sys_fec_ndx);

  send_string_to_screen("--> found:", TRUE);
  soc_sand_os_printf( "found: %u\n\r",prm_found);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ipv6_route_lpm_get (section ipv6)
 */
int
  ui_swp_p_pp_api_ipv6_route_lpm_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_IPV6_ROUTE_KEY
    prm_route_key;
  SOC_SAND_PP_IPV6_ROUTE_KEY
    prm_longest_match;
  uint8
    prm_found;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_ipv6");
  soc_sand_proc_name = "swp_p_pp_ipv6_route_lpm_get";

  unit = swp_p_pp_get_default_unit();
  soc_sand_SAND_PP_IPV6_ROUTE_KEY_clear(&prm_route_key);
  soc_sand_SAND_PP_IPV6_ROUTE_KEY_clear(&prm_longest_match);


  /* Get parameters */


  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV6_ROUTE_LPM_GET_IPV6_ROUTE_LPM_GET_ROUTE_KEY_IN_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV6_ROUTE_LPM_GET_IPV6_ROUTE_LPM_GET_ROUTE_KEY_IN_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.vid.val = (uint32)param_val->value.ulong_value;
    prm_route_key.vid.mask = 0xFFF;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV6_ROUTE_LPM_GET_IPV6_ROUTE_LPM_GET_ROUTE_KEY_IN_VID_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV6_ROUTE_LPM_GET_IPV6_ROUTE_LPM_GET_ROUTE_KEY_IN_VID_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.vid.mask = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV6_ROUTE_LPM_GET_IPV6_ROUTE_LPM_GET_ROUTE_KEY_GROUP_PREFIX_LEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV6_ROUTE_LPM_GET_IPV6_ROUTE_LPM_GET_ROUTE_KEY_GROUP_PREFIX_LEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.dest.prefix_len = (uint8)param_val->value.ulong_value;
  }

  if (!get_val_of(
    current_line,(int *)&match_index,PARAM_SWP_P_PP_IPV6_ROUTE_LPM_GET_IPV6_ROUTE_LPM_GET_ROUTE_KEY_GROUP_IP_ADDRESS_ID,1,
    &param_val,VAL_TEXT,err_msg))
  {
    ret = soc_sand_pp_ipv6_address_string_parse(param_val->value.val_text, &(prm_route_key.dest.ipv6_address));
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** soc_sand_pp_ipv6_address_string_parse - FAIL", TRUE);
      soc_petra_disp_result(ret, "swp_p_pp_ipv6_route_lpm_get");
      goto exit;
    }
}


  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV6_ROUTE_LPM_GET_IPV6_ROUTE_LPM_GET_ROUTE_KEY_IN_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV6_ROUTE_LPM_GET_IPV6_ROUTE_LPM_GET_ROUTE_KEY_IN_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.port.val = (uint32)param_val->value.ulong_value;
    prm_route_key.port.mask = 0xFFFFFFFF;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV6_ROUTE_LPM_GET_IPV6_ROUTE_LPM_GET_ROUTE_KEY_IN_PORT_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV6_ROUTE_LPM_GET_IPV6_ROUTE_LPM_GET_ROUTE_KEY_IN_PORT_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.port.mask = ((uint8)param_val->value.ulong_value == 0)?0:0xFFFFFFFF;
  }




  /* Call function */
  ret = swp_p_pp_ipv6_route_lpm_get(
          unit,
          &prm_route_key,
          &prm_longest_match,
          &prm_found
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_pp_ipv6_route_lpm_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "swp_p_pp_ipv6_route_lpm_get");
    goto exit;
  }

  send_string_to_screen(" ---> swp_p_pp_ipv6_route_lpm_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> longest_match:", TRUE);
  soc_sand_SAND_PP_IPV6_ROUTE_KEY_print(&prm_longest_match);

  send_string_to_screen("--> found:", TRUE);
  soc_sand_os_printf( "found: %u\n\r",prm_found);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ipv6_route_get_block (section ipv6)
 */
int
  ui_swp_p_pp_api_ipv6_route_get_block(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_PETRA_PP_BLOCK_RANGE
    prm_block_range;
  SOC_PETRA_PP_IPV6_ROUTE_ENTRY
    prm_route_table[10];
  uint32
    prm_nof_entries;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_ipv6");
  soc_sand_proc_name = "swp_p_pp_ipv6_route_get_block";

  unit = swp_p_pp_get_default_unit();
  prm_block_range.entries_to_act = 10000;
  prm_block_range.entries_to_scan = 10000;
  prm_block_range.iter = SOC_PETRA_PP_TCAM_LIST_ITER_BEGIN(SOC_PETRA_PP_TCAM_ENTRY_USE_IPV4_MC);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV6_ROUTE_GET_BLOCK_IPV6_ROUTE_GET_BLOCK_BLOCK_RANGE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV6_ROUTE_GET_BLOCK_IPV6_ROUTE_GET_BLOCK_BLOCK_RANGE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_block_range.iter = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = swp_p_pp_ipv6_route_get_block(
          unit,
          &prm_block_range,
          prm_route_table,
          &prm_nof_entries
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_pp_ipv6_route_get_block - FAIL", TRUE);
    soc_petra_disp_result(ret, "swp_p_pp_ipv6_route_get_block");
    goto exit;
  }
/*
  soc_petra_pp_ipv6_print_block(
    unit,
    prm_route_table,
    prm_nof_entries,
    0
  );

*/

  send_string_to_screen(" ---> swp_p_pp_ipv6_route_get_block - SUCCEEDED", TRUE);
  send_string_to_screen("--> route_table:", TRUE);

  send_string_to_screen("--> nof_entries:", TRUE);
  soc_sand_os_printf( "nof_entries: %lu\n\r",prm_nof_entries);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ipv6_route_remove (section ipv6)
 */
int
  ui_swp_p_pp_api_ipv6_route_remove(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_SAND_PP_IPV6_ROUTE_KEY
    prm_route_key;
  uint8
    prm_success;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_ipv6");
  soc_sand_proc_name = "swp_p_pp_ipv6_route_remove";

  prm_route_key.vid.mask = 0;


  unit = swp_p_pp_get_default_unit();
  soc_sand_SAND_PP_IPV6_ROUTE_KEY_clear(&prm_route_key);

  /* Get parameters */

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV6_ROUTE_REMOVE_IPV6_ROUTE_REMOVE_ROUTE_KEY_IN_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV6_ROUTE_REMOVE_IPV6_ROUTE_REMOVE_ROUTE_KEY_IN_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.vid.val = (uint32)param_val->value.ulong_value;
    prm_route_key.vid.mask = 0xFFF;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV6_ROUTE_REMOVE_IPV6_ROUTE_REMOVE_ROUTE_KEY_IN_VID_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV6_ROUTE_REMOVE_IPV6_ROUTE_REMOVE_ROUTE_KEY_IN_VID_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.vid.mask = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV6_ROUTE_REMOVE_IPV6_ROUTE_REMOVE_ROUTE_KEY_IN_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV6_ROUTE_REMOVE_IPV6_ROUTE_REMOVE_ROUTE_KEY_IN_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.port.val = (uint32)param_val->value.ulong_value;
    prm_route_key.port.mask = 0xFFFFFFFF;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV6_ROUTE_REMOVE_IPV6_ROUTE_REMOVE_ROUTE_KEY_IN_PORT_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV6_ROUTE_REMOVE_IPV6_ROUTE_REMOVE_ROUTE_KEY_IN_PORT_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.port.mask = ((uint8)param_val->value.ulong_value == 0)?0:0xFFFFFFFF;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV6_ROUTE_REMOVE_IPV6_ROUTE_REMOVE_ROUTE_KEY_GROUP_PREFIX_LEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_IPV6_ROUTE_REMOVE_IPV6_ROUTE_REMOVE_ROUTE_KEY_GROUP_PREFIX_LEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_route_key.dest.prefix_len = (uint8)param_val->value.ulong_value;
  }

  if (!get_val_of(
    current_line,(int *)&match_index,PARAM_SWP_P_PP_IPV6_ROUTE_REMOVE_IPV6_ROUTE_REMOVE_ROUTE_KEY_GROUP_IP_ADDRESS_ID,1,
    &param_val,VAL_TEXT,err_msg))
  {
    ret = soc_sand_pp_ipv6_address_string_parse(param_val->value.val_text, &(prm_route_key.dest.ipv6_address));
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** soc_sand_pp_ipv6_address_string_parse - FAIL", TRUE);
      soc_petra_disp_result(ret, "swp_p_pp_ipv6_route_remove");
      goto exit;
    }
  }

  /* Call function */
  ret = swp_p_pp_ipv6_route_remove(
          unit,
          &prm_route_key,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_pp_ipv6_route_remove - FAIL", TRUE);
    soc_petra_disp_result(ret, "swp_p_pp_ipv6_route_remove");
    goto exit;
  }

  send_string_to_screen(" ---> swp_p_pp_ipv6_route_remove - SUCCEEDED", TRUE);
  send_string_to_screen("--> success:", TRUE);
  soc_sand_os_printf( "success: %u\n\r",prm_success);


  goto exit;
exit:
  return ui_ret;
}



#endif


#ifdef UI_ACL
/********************************************************************
 *  Function handler: ace_add (section acl)
 */
int
  ui_swp_p_pp_api_acl_ace_add(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_acl_ndx=0;
  SOC_PETRA_PP_ACL_ACE_INFO
    prm_ace;
  SWP_P_PP_ACL_RANGE
    prm_range_info;
  SWP_P_PP_ACL_ACTION_INFO
    prm_action_info;
  uint8
    prm_override = FALSE;
  uint8
    prm_success;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_acl");
  soc_sand_proc_name = "swp_p_pp_acl_ace_add";

  unit = swp_p_pp_get_default_unit();
  soc_petra_pp_PETRA_PP_ACL_ACE_INFO_clear(&prm_ace);
  swp_p_pp_SWP_P_PP_ACL_ACTION_INFO_clear(&prm_action_info);
  swp_p_pp_SWP_P_PP_ACL_RANGE_clear(&prm_range_info);
  prm_ace.val.l2_info.c_tag.vid.val = 0;
  prm_ace.val.l2_info.s_tag.vid.val = 0;

  prm_range_info.inc_sys_fec_id = FALSE;
  prm_range_info.inc_vid = FALSE;

  prm_ace.val.l2_info.in_port_type = SOC_SAND_PP_PORT_L2_TYPE_VBP;
  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACL_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACL_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_acl_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter acl_ndx after ace_add***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_PORT_RANGE_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_PORT_RANGE_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l4_info.port_range_valid = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_PORT_RANGE_MAX_DEST_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_PORT_RANGE_MAX_DEST_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l4_info.port_range.max_dest_port = (uint16)param_val->value.ulong_value;
    prm_ace.val.l3_l4_info.l4_info.port_range_valid = TRUE;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_PORT_RANGE_MIN_DEST_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_PORT_RANGE_MIN_DEST_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l4_info.port_range.min_dest_port = (uint16)param_val->value.ulong_value;
    prm_ace.val.l3_l4_info.l4_info.port_range_valid = TRUE;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_PORT_RANGE_MAX_SRC_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_PORT_RANGE_MAX_SRC_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l4_info.port_range.max_src_port = (uint16)param_val->value.ulong_value;
    prm_ace.val.l3_l4_info.l4_info.port_range_valid = TRUE;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_PORT_RANGE_MIN_SRC_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_PORT_RANGE_MIN_SRC_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l4_info.port_range.min_src_port = (uint16)param_val->value.ulong_value;
    prm_ace.val.l3_l4_info.l4_info.port_range_valid = TRUE;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_DEST_PORT_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_DEST_PORT_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l4_info.dest_port.val = (uint16)param_val->value.ulong_value;
    prm_ace.val.l3_l4_info.l4_info.dest_port.mask = (uint16)UI_SWP_ACL_MASK_SET_ALL;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_DEST_PORT_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_DEST_PORT_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l4_info.dest_port.mask = (uint16)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_SRC_PORT_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_SRC_PORT_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l4_info.src_port.val = (uint16)param_val->value.ulong_value;
    prm_ace.val.l3_l4_info.l4_info.src_port.mask = (uint16)UI_SWP_ACL_MASK_SET_ALL;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_SRC_PORT_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_SRC_PORT_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l4_info.src_port.mask = (uint16)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_TCP_CTRL_OPS_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_TCP_CTRL_OPS_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l4_info.tcp_ctrl_ops.val = (uint8)param_val->value.ulong_value;
    prm_ace.val.l3_l4_info.l4_info.tcp_ctrl_ops.mask = (uint8)UI_SWP_ACL_MASK_SET_ALL;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_TCP_CTRL_OPS_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_TCP_CTRL_OPS_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l4_info.tcp_ctrl_ops.mask = (uint8)param_val->value.ulong_value;
  }

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_DEST_IP_IP_ADDRESS_ID,1,
         &param_val,VAL_IP,err_msg))
  {
    prm_ace.val.l3_l4_info.l3_info.dest_ip.ip_address = param_val->value.ip_value;
    prm_ace.val.l3_l4_info.l3_info.dest_ip.prefix_len = 32;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_DEST_IP_PREFIX_LEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_DEST_IP_PREFIX_LEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l3_info.dest_ip.prefix_len = (uint8)param_val->value.ulong_value;
  }


  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_SRC_IP_IP_ADDRESS_ID,1,
         &param_val,VAL_IP,err_msg))
  {
    prm_ace.val.l3_l4_info.l3_info.src_ip.ip_address = param_val->value.ip_value;
    prm_ace.val.l3_l4_info.l3_info.src_ip.prefix_len = 32;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_SRC_IP_PREFIX_LEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_SRC_IP_PREFIX_LEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l3_info.src_ip.prefix_len = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_PROTOCOL_CODE_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_PROTOCOL_CODE_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l3_info.protocol_code.val = (uint8)param_val->value.ulong_value;
    prm_ace.val.l3_l4_info.l3_info.protocol_code.mask = (uint8)UI_SWP_ACL_MASK_SET_ALL;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_PROTOCOL_CODE_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_PROTOCOL_CODE_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l3_info.protocol_code.mask = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_TOS_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_TOS_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l3_info.tos.val = (uint8)param_val->value.ulong_value;
    prm_ace.val.l3_l4_info.l3_info.tos.mask = (uint8)UI_SWP_ACL_MASK_SET_ALL;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_TOS_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_TOS_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l3_info.tos.mask = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_IN_VID_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_IN_VID_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l3_info.in_vid.val = (uint32)param_val->value.ulong_value;
    prm_ace.val.l3_l4_info.l3_info.in_vid.mask = UI_SWP_ACL_MASK_SET_ALL;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_IN_VID_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_IN_VID_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l3_info.in_vid.mask = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_IN_LOCAL_PORT_ID_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_IN_LOCAL_PORT_ID_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l3_info.in_local_port_id.val = (uint32)param_val->value.ulong_value;
    prm_ace.val.l3_l4_info.l3_info.in_local_port_id.mask = (uint32)UI_SWP_ACL_MASK_SET_ALL;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_IN_LOCAL_PORT_ID_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_IN_LOCAL_PORT_ID_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l3_info.in_local_port_id.mask = (uint32)param_val->value.ulong_value;
  }

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_DEST_MAC_ID,1,
         &param_val,VAL_TEXT,err_msg))
  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_ace.val.l2_info.dest_mac));
    soc_sand_pp_mac_address_string_parse("FFFFFFFF", &(prm_ace.val.l2_info.dest_mac_mask));
  }

  if (!get_val_of(
    current_line,(int *)&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_DEST_MAC_MASK_ID,1,
    &param_val,VAL_TEXT,err_msg))
  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_ace.val.l2_info.dest_mac_mask));
  }


  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_SRC_MAC_ID,1,
         &param_val,VAL_TEXT,err_msg))
  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_ace.val.l2_info.src_mac));
    soc_sand_pp_mac_address_string_parse("FFFFFFFF", &(prm_ace.val.l2_info.src_mac_mask));
  }

  if (!get_val_of(
    current_line,(int *)&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_SRC_MAC_MASK_ID,1,
    &param_val,VAL_TEXT,err_msg))
  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_ace.val.l2_info.src_mac_mask));
  }


  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_ETHER_TYPE_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_ETHER_TYPE_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.ether_type.val = (uint16)param_val->value.ulong_value;
    prm_ace.val.l2_info.ether_type.mask = (uint16)UI_SWP_ACL_MASK_SET_ALL;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_ETHER_TYPE_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_ETHER_TYPE_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.ether_type.mask = (uint16)param_val->value.ulong_value;
  }


  if(!search_param_val_pairs(current_line,&match_index,PARAM_PETRA_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_ETHER_TYPE_DSAP_ID,1))
  {
    prm_ace.val.l2_info.ether_type_dsap_ssap = TRUE;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_S_TAG_DEI_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_S_TAG_DEI_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.s_tag.dei.val = (uint8)param_val->value.ulong_value;
    prm_ace.val.l2_info.s_tag.dei.mask = (uint8)UI_SWP_ACL_MASK_SET_ALL;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_S_TAG_DEI_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_S_TAG_DEI_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.s_tag.dei.mask = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_S_TAG_PCP_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_S_TAG_PCP_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.s_tag.pcp.val = (uint8)param_val->value.ulong_value;
    prm_ace.val.l2_info.s_tag.pcp.mask = (uint8)UI_SWP_ACL_MASK_SET_ALL;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_S_TAG_PCP_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_S_TAG_PCP_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.s_tag.pcp.mask = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_S_TAG_VID_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_S_TAG_VID_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.s_tag.vid.val = (uint32)param_val->value.ulong_value;
    prm_ace.val.l2_info.s_tag.vid.mask = (uint32)UI_SWP_ACL_MASK_SET_ALL;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_S_TAG_VID_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_S_TAG_VID_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.s_tag.vid.mask = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_S_TAG_TAG_EXIST_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_S_TAG_TAG_EXIST_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.s_tag.tag_exist.val = (uint8)param_val->value.ulong_value;
    prm_ace.val.l2_info.s_tag.tag_exist.mask = (uint8)UI_SWP_ACL_MASK_SET_ALL;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_S_TAG_TAG_EXIST_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_S_TAG_TAG_EXIST_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.s_tag.tag_exist.mask = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_C_TAG_DEI_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_C_TAG_DEI_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.c_tag.dei.val = (uint8)param_val->value.ulong_value;
    prm_ace.val.l2_info.c_tag.dei.mask = (uint8)UI_SWP_ACL_MASK_SET_ALL;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_C_TAG_DEI_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_C_TAG_DEI_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.c_tag.dei.mask = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_C_TAG_PCP_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_C_TAG_PCP_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.c_tag.pcp.val = (uint8)param_val->value.ulong_value;
    prm_ace.val.l2_info.c_tag.pcp.mask = (uint8)UI_SWP_ACL_MASK_SET_ALL;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_C_TAG_PCP_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_C_TAG_PCP_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.c_tag.pcp.mask = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_C_TAG_VID_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_C_TAG_VID_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.c_tag.vid.val = (uint32)param_val->value.ulong_value;
    prm_ace.val.l2_info.c_tag.vid.mask = (uint32)UI_SWP_ACL_MASK_SET_ALL;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_C_TAG_VID_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_C_TAG_VID_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.c_tag.vid.mask = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_C_TAG_TAG_EXIST_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_C_TAG_TAG_EXIST_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.c_tag.tag_exist.val = (uint8)param_val->value.ulong_value;
    prm_ace.val.l2_info.c_tag.tag_exist.mask = (uint8)UI_SWP_ACL_MASK_SET_ALL;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_C_TAG_TAG_EXIST_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_C_TAG_TAG_EXIST_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.c_tag.tag_exist.mask = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_IN_LOCAL_PORT_ID_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_IN_LOCAL_PORT_ID_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.in_local_port_id.val = (uint32)param_val->value.ulong_value;
    prm_ace.val.l2_info.in_local_port_id.mask = (uint32)UI_SWP_ACL_MASK_SET_ALL;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_IN_LOCAL_PORT_ID_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_IN_LOCAL_PORT_ID_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.in_local_port_id.mask = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_IN_PORT_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_IN_PORT_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.in_port_type = param_val->numeric_equivalent;
    prm_ace.val.l2_info.in_port_type_valid = TRUE;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_IN_PORT_TYPE_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_IN_PORT_TYPE_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.in_port_type_valid = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_L2_ID,1))
  {
    prm_ace.type = SOC_PETRA_PP_ACL_ACE_TYPE_L2;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_L3A_ID,1))
  {
    prm_ace.type = SOC_PETRA_PP_ACL_ACE_TYPE_L3A;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_L3B_ID,1))
  {
    prm_ace.type = SOC_PETRA_PP_ACL_ACE_TYPE_L3B;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_PRIORITY_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_PRIORITY_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.priority = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_UPDATE_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_UPDATE_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_action_info.update_type = param_val->numeric_equivalent;
  }

  prm_action_info.action_mask = 0;
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_COS_ACTION_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_COS_ACTION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_action_info.cos_action = (uint8)param_val->value.ulong_value;
    prm_action_info.action_mask |= SOC_PETRA_PP_ACL_ACTION_MASK_COS;
  }

  prm_action_info.action_mask |= SOC_PETRA_PP_ACL_ACTION_UPDATE_FEC;
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_L3_SYS_FEC_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_L3_SYS_FEC_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_action_info.l3_info.sys_fec_id = (uint32)param_val->value.ulong_value;
    prm_action_info.update_type = SOC_PETRA_PP_ACL_ACTION_TYPE_UPDATE_L3;
  }
  else
  {
    prm_action_info.update_type = SOC_PETRA_PP_ACL_ACTION_TYPE_UPDATE_L2;
  }


  if (prm_action_info.update_type == SOC_PETRA_PP_ACL_ACTION_TYPE_UPDATE_L2)
  {
    if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_L3_INFO_VID_ID,1))
    {
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_L3_INFO_VID_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      prm_action_info.l2_info.vid = (uint32)param_val->value.ulong_value;
      prm_action_info.action_mask |= SOC_PETRA_PP_ACL_ACTION_MASK_VID;
    }

    if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_L3_INFO_DESTINATION_DEST_VAL_ID,1))
    {
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_L3_INFO_DESTINATION_DEST_VAL_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      prm_action_info.l2_info.destination.dest_val = (uint32)param_val->value.ulong_value;
      prm_action_info.action_mask |= SOC_PETRA_PP_ACL_ACTION_MASK_DEST;
    }

    if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_L3_INFO_DESTINATION_DEST_TYPE_ID,1))
    {
      UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_L3_INFO_DESTINATION_DEST_TYPE_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      prm_action_info.l2_info.destination.dest_type = param_val->numeric_equivalent;
      prm_action_info.action_mask |= SOC_PETRA_PP_ACL_ACTION_MASK_DEST;
    }
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_OVERRIDE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_OVERRIDE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_override = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_INC_VID_ID,1))
  {
    prm_range_info.inc_vid = TRUE;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_INC_SYS_FEC_ID_ID,1))
  {
    prm_range_info.inc_sys_fec_id = TRUE;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_STEP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_STEP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_info.step = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_COUNT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_COUNT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_info.count = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_FIELD_TO_INC_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_FIELD_TO_INC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_info.field_to_inc = param_val->numeric_equivalent;
  }

  if (prm_range_info.count <= 1)
  {
    /* Call function */
    ret = swp_p_pp_acl_ace_add(
            unit,
            prm_acl_ndx,
            &prm_ace,
            &prm_action_info,
            prm_override,
            &prm_success
          );
  }
  else
  {
    /* Call function */
    ret = swp_p_pp_acl_range_add(
            unit,
            prm_acl_ndx,
            &prm_ace,
            &prm_range_info,
            &prm_action_info,
            &prm_success
          );
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_pp_acl_ace_add - FAIL", TRUE);
    soc_petra_disp_result(ret, "swp_p_pp_acl_ace_add");
    goto exit;
  }

  send_string_to_screen(" ---> swp_p_pp_acl_ace_add - SUCCEEDED", TRUE);
  send_string_to_screen("--> success:", TRUE);
  soc_sand_os_printf( "success: %u\n\r",prm_success);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ace_remove (section acl)
 */
int
  ui_swp_p_pp_api_acl_ace_remove(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_acl_ndx=0;
  uint32
    prm_ace_id_ndx=0;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_acl");
  soc_sand_proc_name = "swp_p_pp_acl_ace_remove";

  unit = swp_p_pp_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_REMOVE_ACE_REMOVE_ACL_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_REMOVE_ACE_REMOVE_ACL_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_acl_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter acl_ndx after ace_remove***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_REMOVE_ACE_REMOVE_ACE_ID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_REMOVE_ACE_REMOVE_ACE_ID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace_id_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ace_id_ndx after ace_remove***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = swp_p_pp_acl_ace_remove(
          unit,
          prm_acl_ndx,
          prm_ace_id_ndx
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_pp_acl_ace_remove - FAIL", TRUE);
    soc_petra_disp_result(ret, "swp_p_pp_acl_ace_remove");
    goto exit;
  }

  send_string_to_screen(" ---> swp_p_pp_acl_ace_remove - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ace_get (section acl)
 */
int
  ui_swp_p_pp_api_acl_ace_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_acl_ndx=0;
  uint32
    prm_ace_id_ndx=0;
  uint8
    prm_valid_ace;
  SOC_PETRA_PP_ACL_ACE_INFO
    prm_ace;
  SWP_P_PP_ACL_ACTION_INFO
    prm_action_info;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_acl");
  soc_sand_proc_name = "swp_p_pp_acl_ace_get";

  unit = swp_p_pp_get_default_unit();
  soc_petra_pp_PETRA_PP_ACL_ACE_INFO_clear(&prm_ace);
  swp_p_pp_SWP_P_PP_ACL_ACTION_INFO_clear(&prm_action_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_GET_ACE_GET_ACL_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_GET_ACE_GET_ACL_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_acl_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter acl_ndx after ace_get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_GET_ACE_GET_ACE_ID_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_GET_ACE_GET_ACE_ID_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace_id_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ace_id_ndx after ace_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = swp_p_pp_acl_ace_get(
          unit,
          prm_acl_ndx,
          prm_ace_id_ndx,
          &prm_valid_ace,
          &prm_ace,
          &prm_action_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_pp_acl_ace_get - FAIL", TRUE);
    soc_petra_disp_result(ret, "swp_p_pp_acl_ace_get");
    goto exit;
  }

  send_string_to_screen(" ---> swp_p_pp_acl_ace_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> valid_ace:", TRUE);
  soc_sand_os_printf( "valid_ace: %u\n\r",prm_valid_ace);

  send_string_to_screen("--> ace:", TRUE);
  soc_petra_pp_PETRA_PP_ACL_ACE_INFO_print(&prm_ace);

  send_string_to_screen("--> action_info:", TRUE);
  swp_p_pp_SWP_P_PP_ACL_ACTION_INFO_print(&prm_action_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: ace_get_block (section acl)
 */
int
  ui_swp_p_pp_api_acl_ace_get_block(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_acl_ndx=0;
  SOC_PETRA_PP_BLOCK_RANGE
    prm_range;
  SOC_PETRA_PP_ACL_ACE_INFO
    prm_aces;
  SWP_P_PP_ACL_ACTION_INFO
    prm_actions;
  uint32
    prm_nof_entries;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_acl");
  soc_sand_proc_name = "swp_p_pp_acl_ace_get_block";

  unit = swp_p_pp_get_default_unit();
  soc_petra_pp_PETRA_PP_ACL_ACE_INFO_clear(&prm_aces);
  swp_p_pp_SWP_P_PP_ACL_ACTION_INFO_clear(&prm_actions);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_GET_BLOCK_ACE_GET_BLOCK_ACL_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_ACE_GET_BLOCK_ACE_GET_BLOCK_ACL_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_acl_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter acl_ndx after ace_get_block***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = swp_p_pp_acl_ace_get_block(
          unit,
          prm_acl_ndx,
          &prm_range,
          &prm_aces,
          &prm_actions,
          &prm_nof_entries
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_pp_acl_ace_get_block - FAIL", TRUE);
    soc_petra_disp_result(ret, "swp_p_pp_acl_ace_get_block");
    goto exit;
  }

  send_string_to_screen(" ---> swp_p_pp_acl_ace_get_block - SUCCEEDED", TRUE);
  send_string_to_screen("--> aces:", TRUE);
  soc_petra_pp_PETRA_PP_ACL_ACE_INFO_print(&prm_aces);

  send_string_to_screen("--> actions:", TRUE);
  swp_p_pp_SWP_P_PP_ACL_ACTION_INFO_print(&prm_actions);

  send_string_to_screen("--> nof_entries:", TRUE);
  soc_sand_os_printf( "nof_entries: %lu\n\r",prm_nof_entries);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: range_add (section acl)
 */
int
  ui_swp_p_pp_api_acl_range_add(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_acl_ndx=0;
  uint32
    prm_dest_mac_mask_index = 0xFFFFFFFF;
  uint32
    prm_src_mac_mask_index = 0xFFFFFFFF;
  SOC_PETRA_PP_ACL_ACE_INFO
    prm_ace;
  SWP_P_PP_ACL_RANGE
    prm_range_info;
  SWP_P_PP_ACL_ACTION_INFO
    prm_action;
  uint8
    prm_success;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_acl");
  soc_sand_proc_name = "swp_p_pp_acl_range_add";

  unit = swp_p_pp_get_default_unit();
  soc_petra_pp_PETRA_PP_ACL_ACE_INFO_clear(&prm_ace);
  swp_p_pp_SWP_P_PP_ACL_RANGE_clear(&prm_range_info);
  swp_p_pp_SWP_P_PP_ACL_ACTION_INFO_clear(&prm_action);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACL_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACL_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_acl_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter acl_ndx after range_add***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_PORT_RANGE_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_PORT_RANGE_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l4_info.port_range_valid = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_PORT_RANGE_MAX_DEST_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_PORT_RANGE_MAX_DEST_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l4_info.port_range.max_dest_port = (uint16)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_PORT_RANGE_MIN_DEST_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_PORT_RANGE_MIN_DEST_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l4_info.port_range.min_dest_port = (uint16)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_PORT_RANGE_MAX_SRC_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_PORT_RANGE_MAX_SRC_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l4_info.port_range.max_src_port = (uint16)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_PORT_RANGE_MIN_SRC_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_PORT_RANGE_MIN_SRC_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l4_info.port_range.min_src_port = (uint16)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_DEST_PORT_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_DEST_PORT_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l4_info.dest_port.mask = (uint16)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_DEST_PORT_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_DEST_PORT_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l4_info.dest_port.val = (uint16)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_SRC_PORT_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_SRC_PORT_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l4_info.src_port.mask = (uint16)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_SRC_PORT_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_SRC_PORT_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l4_info.src_port.val = (uint16)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_TCP_CTRL_OPS_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_TCP_CTRL_OPS_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l4_info.tcp_ctrl_ops.mask = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_TCP_CTRL_OPS_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_TCP_CTRL_OPS_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l4_info.tcp_ctrl_ops.val = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_DEST_IP_PREFIX_LEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_DEST_IP_PREFIX_LEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l3_info.dest_ip.prefix_len = (uint8)param_val->value.ulong_value;
  }

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_DEST_IP_IP_ADDRESS_ID,1,
         &param_val,VAL_IP,err_msg))
  {
    prm_ace.val.l3_l4_info.l3_info.dest_ip.ip_address = param_val->value.ip_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_SRC_IP_PREFIX_LEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_SRC_IP_PREFIX_LEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l3_info.src_ip.prefix_len = (uint8)param_val->value.ulong_value;
  }

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_SRC_IP_IP_ADDRESS_ID,1,
         &param_val,VAL_IP,err_msg))
  {
    prm_ace.val.l3_l4_info.l3_info.src_ip.ip_address = param_val->value.ip_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_PROTOCOL_CODE_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_PROTOCOL_CODE_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l3_info.protocol_code.mask = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_PROTOCOL_CODE_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_PROTOCOL_CODE_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l3_info.protocol_code.val = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_TOS_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_TOS_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l3_info.tos.mask = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_TOS_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_TOS_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l3_info.tos.val = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_IN_VID_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_IN_VID_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l3_info.in_vid.mask = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_IN_VID_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_IN_VID_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l3_info.in_vid.val = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_IN_LOCAL_PORT_ID_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_IN_LOCAL_PORT_ID_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l3_info.in_local_port_id.mask = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_IN_LOCAL_PORT_ID_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_IN_LOCAL_PORT_ID_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l3_l4_info.l3_info.in_local_port_id.val = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_DEST_MAC_MASK_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_DEST_MAC_MASK_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_dest_mac_mask_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_dest_mac_mask_index != 0xFFFFFFFF)
  {

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_DEST_MAC_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_DEST_MAC_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.dest_mac_mask.address[ prm_dest_mac_mask_index] = (uint8)param_val->value.ulong_value;
  }

  }

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_DEST_MAC_ID,1,
         &param_val,VAL_TEXT,err_msg))
  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_ace.val.l2_info.dest_mac));
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_SRC_MAC_MASK_INDEX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_SRC_MAC_MASK_INDEX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_src_mac_mask_index = (uint32)param_val->value.ulong_value;
  }

  if(prm_src_mac_mask_index != 0xFFFFFFFF)
  {

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_SRC_MAC_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_SRC_MAC_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.src_mac_mask.address[ prm_src_mac_mask_index] = (uint8)param_val->value.ulong_value;
  }

  }

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_SRC_MAC_ID,1,
         &param_val,VAL_TEXT,err_msg))
  {
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_ace.val.l2_info.src_mac));
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_ETHER_TYPE_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_ETHER_TYPE_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.ether_type.mask = (uint16)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_ETHER_TYPE_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_ETHER_TYPE_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.ether_type.val = (uint16)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_S_TAG_DEI_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_S_TAG_DEI_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.s_tag.dei.mask = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_S_TAG_DEI_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_S_TAG_DEI_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.s_tag.dei.val = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_S_TAG_PCP_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_S_TAG_PCP_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.s_tag.pcp.mask = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_S_TAG_PCP_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_S_TAG_PCP_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.s_tag.pcp.val = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_S_TAG_VID_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_S_TAG_VID_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.s_tag.vid.mask = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_S_TAG_VID_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_S_TAG_VID_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.s_tag.vid.val = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_S_TAG_TAG_EXIST_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_S_TAG_TAG_EXIST_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.s_tag.tag_exist.mask = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_S_TAG_TAG_EXIST_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_S_TAG_TAG_EXIST_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.s_tag.tag_exist.val = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_C_TAG_DEI_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_C_TAG_DEI_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.c_tag.dei.mask = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_C_TAG_DEI_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_C_TAG_DEI_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.c_tag.dei.val = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_C_TAG_PCP_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_C_TAG_PCP_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.c_tag.pcp.mask = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_C_TAG_PCP_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_C_TAG_PCP_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.c_tag.pcp.val = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_C_TAG_VID_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_C_TAG_VID_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.c_tag.vid.mask = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_C_TAG_VID_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_C_TAG_VID_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.c_tag.vid.val = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_C_TAG_TAG_EXIST_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_C_TAG_TAG_EXIST_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.c_tag.tag_exist.mask = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_C_TAG_TAG_EXIST_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_C_TAG_TAG_EXIST_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.c_tag.tag_exist.val = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_IN_LOCAL_PORT_ID_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_IN_LOCAL_PORT_ID_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.in_local_port_id.mask = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_IN_LOCAL_PORT_ID_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_IN_LOCAL_PORT_ID_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.in_local_port_id.val = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_IN_PORT_TYPE_VALID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_IN_PORT_TYPE_VALID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.in_port_type_valid = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_IN_PORT_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_IN_PORT_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.val.l2_info.in_port_type = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.type = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_PRIORITY_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_PRIORITY_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.priority = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ace.id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_INC_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_INC_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_info.inc_vid = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_INC_SYS_FEC_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_INC_SYS_FEC_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_info.inc_sys_fec_id = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_STEP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_STEP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_info.step = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_COUNT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_COUNT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_info.count = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_TO_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_TO_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_info.to = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_FIELD_TO_INC_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_FIELD_TO_INC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_info.field_to_inc = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_range_info.type = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACTION_ACTION_MASK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACTION_ACTION_MASK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_action.action_mask = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACTION_COS_ACTION_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACTION_COS_ACTION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_action.cos_action = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACTION_L3_INFO_SYS_FEC_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACTION_L3_INFO_SYS_FEC_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_action.l3_info.sys_fec_id = (uint32)param_val->value.ulong_value;
  }



  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACTION_L2_INFO_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACTION_L2_INFO_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_action.l2_info.vid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACTION_L2_INFO_DESTINATION_DEST_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACTION_L2_INFO_DESTINATION_DEST_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_action.l2_info.destination.dest_val = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACTION_L2_INFO_DESTINATION_DEST_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACTION_L2_INFO_DESTINATION_DEST_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_action.l2_info.destination.dest_type = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACTION_UPDATE_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACTION_UPDATE_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_action.update_type = param_val->numeric_equivalent;
  }


  /* Call function */
  ret = swp_p_pp_acl_range_add(
          unit,
          prm_acl_ndx,
          &prm_ace,
          &prm_range_info,
          &prm_action,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_pp_acl_range_add - FAIL", TRUE);
    soc_petra_disp_result(ret, "swp_p_pp_acl_range_add");
    goto exit;
  }

  send_string_to_screen(" ---> swp_p_pp_acl_range_add - SUCCEEDED", TRUE);
  send_string_to_screen("--> success:", TRUE);
  soc_sand_os_printf( "success: %u\n\r",prm_success);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: range_remove (section acl)
 */
int
  ui_swp_p_pp_api_acl_range_remove(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_acl_ndx=0;
  uint32
    prm_from_ace_ndx=0;
  uint32
    prm_to_ace_ndx=0;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_acl");
  soc_sand_proc_name = "swp_p_pp_acl_range_remove";

  unit = swp_p_pp_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_REMOVE_RANGE_REMOVE_ACL_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_REMOVE_RANGE_REMOVE_ACL_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_acl_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter acl_ndx after range_remove***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_REMOVE_RANGE_REMOVE_FROM_ACE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_REMOVE_RANGE_REMOVE_FROM_ACE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_from_ace_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter from_ace_ndx after range_remove***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_REMOVE_RANGE_REMOVE_TO_ACE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_ACL_RANGE_REMOVE_RANGE_REMOVE_TO_ACE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_to_ace_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter to_ace_ndx after range_remove***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = swp_p_pp_acl_range_remove(
          unit,
          prm_acl_ndx,
          prm_from_ace_ndx,
          prm_to_ace_ndx
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_pp_acl_range_remove - FAIL", TRUE);
    soc_petra_disp_result(ret, "swp_p_pp_acl_range_remove");
    goto exit;
  }

  send_string_to_screen(" ---> swp_p_pp_acl_range_remove - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

#endif


#ifdef UI_FEC/* { fec*/
/********************************************************************
 *  Section handler: fec
 */
int
  ui_swp_p_pp_api_fec(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_fec");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_UC_ECMP_ADD_UC_ECMP_ADD_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_fec_uc_ecmp_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_fec_uc_ecmp_update_entry(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_MC_FEC_ADD_MC_FEC_ADD_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_fec_mc_fec_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_UC_EMCP_REMOVE_ENTRY_UC_EMCP_REMOVE_ENTRY_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_fec_uc_emcp_remove_entry(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_REMOVE_REMOVE_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_fec_remove(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_UC_GET_UC_GET_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_fec_uc_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_MC_GET_MC_GET_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_fec_mc_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_TABLE_UC_GET_BLOCK_TABLE_UC_GET_BLOCK_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_fec_table_uc_get_block(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_TABLE_MC_GET_BLOCK_TABLE_MC_GET_BLOCK_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_fec_table_mc_get_block(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_fec_uc_ecmp_range_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_fec_mc_fec_range_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_RANGE_REMOVE_RANGE_REMOVE_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_fec_range_remove(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_FEC_TABLE_CLEAR_TABLE_CLEAR_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_fec_table_clear(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after fec***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* { */


#ifdef UI_SWP_P_PP_TS_APP
/******************************************************************** 
 *  Function handler: ts_app_init (section swp_p_pp_ts_app)
 */
int 
  ui_swp_p_pp_ts_app_ts_app_init(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
   
  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_ts_app"); 
  soc_sand_proc_name = "swp_p_pp_ts_app_init"; 
 
  unit = swp_p_pp_get_default_unit(); 


  /* Call function */
  ret = swp_p_pp_ts_app_close(
          unit
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** swp_p_pp_ts_app_init - FAIL", TRUE); 
    soc_petra_disp_result(ret, "swp_p_pp_ts_app_init");   
    goto exit; 
  } 

  ret = swp_p_pp_ts_app_run(
          unit
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** swp_p_pp_ts_app_init - FAIL", TRUE); 
    soc_petra_disp_result(ret, "swp_p_pp_ts_app_init");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: ts_entry_add (section swp_p_pp_ts_app)
 */
int 
  ui_swp_p_pp_ts_app_ts_entry_add(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SWP_P_PP_TS_KEY   
    prm_key;
  SWP_P_PP_TS_ACTION   
    prm_action;
  SOC_SAND_SUCCESS_FAILURE   
    prm_success;
   
  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_ts_app"); 
  soc_sand_proc_name = "swp_p_pp_ts_entry_add"; 
 
  unit = swp_p_pp_get_default_unit(); 
  swp_p_pp_SWP_P_PP_TS_KEY_clear(&prm_key);
  swp_p_pp_SWP_P_PP_TS_ACTION_clear(&prm_action);
  prm_action.dest.dest_type = SOC_SAND_PP_DEST_SINGLE_PORT;
 
  /* Get parameters */ 
  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_KEY_VALUE_L2_TP_DIP_ID,1,  
         &param_val,VAL_IP,err_msg)) 
  { 
    prm_key.value.l2_tp.dip = param_val->value.ip_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_KEY_VALUE_L2_TP_SESSION_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_KEY_VALUE_L2_TP_SESSION_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_key.value.l2_tp.session = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_KEY_VALUE_L2_TP_IN_LOCAL_PORT_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_KEY_VALUE_L2_TP_IN_LOCAL_PORT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_key.value.l2_tp.in_local_port = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_KEY_VALUE_L2_TP_TABLE_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_KEY_VALUE_L2_TP_TABLE_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_key.value.l2_tp.table_id = (uint32)param_val->value.ulong_value;
  } 

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_KEY_VALUE_UDP_MC_SIP_ID,1,  
         &param_val,VAL_IP,err_msg)) 
  { 
    prm_key.value.udp_mc.sip = param_val->value.ip_value;
  } 
  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_KEY_VALUE_UDP_MC_DIP_ID,1,  
         &param_val,VAL_IP,err_msg)) 
  { 
    prm_key.value.udp_mc.dip = param_val->value.ip_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_KEY_VALUE_UDP_MC_UDP_DEST_PORT_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_KEY_VALUE_UDP_MC_UDP_DEST_PORT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_key.value.udp_mc.udp_dest_port = (uint16)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_KEY_VALUE_UDP_MC_IN_LOCAL_PORT_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_KEY_VALUE_UDP_MC_IN_LOCAL_PORT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_key.value.udp_mc.in_local_port = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_KEY_VALUE_UDP_MC_TABLE_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_KEY_VALUE_UDP_MC_TABLE_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_key.value.udp_mc.table_id = (uint32)param_val->value.ulong_value;
  } 

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_KEY_VALUE_UDP_UC_DIP_ID,1,  
         &param_val,VAL_IP,err_msg)) 
  { 
    prm_key.value.udp_uc.dip = param_val->value.ip_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_KEY_VALUE_UDP_UC_UDP_DEST_PORT_VALID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_KEY_VALUE_UDP_UC_UDP_DEST_PORT_VALID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_key.value.udp_uc.udp_dest_port_valid = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_KEY_VALUE_UDP_UC_UDP_DEST_PORT_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_KEY_VALUE_UDP_UC_UDP_DEST_PORT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_key.value.udp_uc.udp_dest_port = (uint16)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_KEY_VALUE_UDP_UC_IN_LOCAL_PORT_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_KEY_VALUE_UDP_UC_IN_LOCAL_PORT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_key.value.udp_uc.in_local_port = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_KEY_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_KEY_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_key.type = param_val->numeric_equivalent;
    if (prm_key.type == SWP_P_PP_TS_KEY_TYPE_L2_TPV3)
    {
      prm_key.value.l2_tp.table_id = 4;
    }
    else if (prm_key.type == SWP_P_PP_TS_KEY_TYPE_UDP_MC)
    {
      prm_key.value.l2_tp.table_id = 2;
    }
    else
    {
      prm_key.value.l2_tp.table_id = 1;
    }

  } 


  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_KEY_VALUE_UDP_UC_TABLE_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_KEY_VALUE_UDP_UC_TABLE_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_key.value.udp_uc.table_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_ACTION_DEST_DEST_VAL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_ACTION_DEST_DEST_VAL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_action.dest.dest_val = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_ACTION_DEST_DEST_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_ACTION_DEST_DEST_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_action.dest.dest_type = param_val->numeric_equivalent;
  } 


  /* Call function */
  ret = swp_p_pp_ts_entry_add(
          unit,
          &prm_key,
          &prm_action,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** swp_p_pp_ts_entry_add - FAIL", TRUE); 
    soc_petra_disp_result(ret, "swp_p_pp_ts_entry_add");   
    goto exit; 
  } 

  send_string_to_screen("--> success:", TRUE);
  soc_sand_os_printf( "success: %s\n",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: ts_entry_get (section swp_p_pp_ts_app)
 */
int 
  ui_swp_p_pp_ts_app_ts_entry_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SWP_P_PP_TS_KEY   
    prm_key;
  SWP_P_PP_TS_ACTION   
    prm_action;
  uint8   
    prm_found;
   
  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_ts_app"); 
  soc_sand_proc_name = "swp_p_pp_ts_entry_get"; 
 
  unit = swp_p_pp_get_default_unit(); 
  swp_p_pp_SWP_P_PP_TS_KEY_clear(&prm_key);
  swp_p_pp_SWP_P_PP_TS_ACTION_clear(&prm_action);
 
  /* Get parameters */ 
  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_KEY_VALUE_L2_TP_DIP_ID,1,  
         &param_val,VAL_IP,err_msg)) 
  { 
    prm_key.value.l2_tp.dip = param_val->value.ip_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_KEY_VALUE_L2_TP_SESSION_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_KEY_VALUE_L2_TP_SESSION_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_key.value.l2_tp.session = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_KEY_VALUE_L2_TP_IN_LOCAL_PORT_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_KEY_VALUE_L2_TP_IN_LOCAL_PORT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_key.value.l2_tp.in_local_port = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_KEY_VALUE_L2_TP_TABLE_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_KEY_VALUE_L2_TP_TABLE_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_key.value.l2_tp.table_id = (uint32)param_val->value.ulong_value;
  } 

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_KEY_VALUE_UDP_MC_SIP_ID,1,  
         &param_val,VAL_IP,err_msg)) 
  { 
    prm_key.value.udp_mc.sip = param_val->value.ip_value;
  } 
  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_KEY_VALUE_UDP_MC_DIP_ID,1,  
         &param_val,VAL_IP,err_msg)) 
  { 
    prm_key.value.udp_mc.dip = param_val->value.ip_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_KEY_VALUE_UDP_MC_UDP_DEST_PORT_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_KEY_VALUE_UDP_MC_UDP_DEST_PORT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_key.value.udp_mc.udp_dest_port = (uint16)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_KEY_VALUE_UDP_MC_IN_LOCAL_PORT_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_KEY_VALUE_UDP_MC_IN_LOCAL_PORT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_key.value.udp_mc.in_local_port = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_KEY_VALUE_UDP_MC_TABLE_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_KEY_VALUE_UDP_MC_TABLE_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_key.value.udp_mc.table_id = (uint32)param_val->value.ulong_value;
  } 

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_KEY_VALUE_UDP_UC_DIP_ID,1,  
         &param_val,VAL_IP,err_msg)) 
  { 
    prm_key.value.udp_uc.dip = param_val->value.ip_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_KEY_VALUE_UDP_UC_UDP_DEST_PORT_VALID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_KEY_VALUE_UDP_UC_UDP_DEST_PORT_VALID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_key.value.udp_uc.udp_dest_port_valid = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_KEY_VALUE_UDP_UC_UDP_DEST_PORT_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_KEY_VALUE_UDP_UC_UDP_DEST_PORT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_key.value.udp_uc.udp_dest_port = (uint16)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_KEY_VALUE_UDP_UC_IN_LOCAL_PORT_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_KEY_VALUE_UDP_UC_IN_LOCAL_PORT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_key.value.udp_uc.in_local_port = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_KEY_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_KEY_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_key.type = param_val->numeric_equivalent;
    if (prm_key.type == SWP_P_PP_TS_KEY_TYPE_L2_TPV3)
    {
      prm_key.value.l2_tp.table_id = 4;
    }
    else if (prm_key.type == SWP_P_PP_TS_KEY_TYPE_UDP_MC)
    {
      prm_key.value.l2_tp.table_id = 2;
    }
    else
    {
      prm_key.value.l2_tp.table_id = 1;
    }
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_KEY_VALUE_UDP_UC_TABLE_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_KEY_VALUE_UDP_UC_TABLE_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_key.value.udp_uc.table_id = (uint32)param_val->value.ulong_value;
  } 



  /* Call function */
  ret = swp_p_pp_ts_entry_get(
          unit,
          &prm_key,
          &prm_action,
          &prm_found
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** swp_p_pp_ts_entry_get - FAIL", TRUE); 
    soc_petra_disp_result(ret, "swp_p_pp_ts_entry_get");   
    goto exit; 
  } 

  swp_p_pp_SWP_P_PP_TS_ACTION_print(&prm_action);

  soc_sand_os_printf( "found: %u\n\r",prm_found);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: ts_get_block (section swp_p_pp_ts_app)
 */
int 
  ui_swp_p_pp_ts_app_ts_get_block(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SWP_P_PP_TS_KEY_TYPE   
    prm_key_type_ndx;
  SOC_SAND_TABLE_BLOCK_RANGE   
    prm_block_range;
  SWP_P_PP_TS_KEY   
    prm_keys_list[SWP_P_PP_UI_BLOCK_SIZE];
  SOC_PETRA_PP_TCAM_PLACE_ID 
    places[SWP_P_PP_UI_BLOCK_SIZE];
  SWP_P_PP_TS_ACTION   
    prm_actions_list[SWP_P_PP_UI_BLOCK_SIZE];
  uint32   
    prm_nof_entries;
  uint32                                                                                            
    total_entries=0,
    flavor=0;
  char
    user_msg[5] = "";
  uint32 
    con;

   
  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_ts_app"); 
  soc_sand_proc_name = "swp_p_pp_ts_get_block";
  soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&prm_block_range);
  prm_block_range.entries_to_act = SWP_P_PP_UI_BLOCK_SIZE;
  prm_block_range.entries_to_scan = 100000;
  prm_block_range.iter = 0;
  prm_nof_entries = 0;
 
  unit = swp_p_pp_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_GET_BLOCK_TS_GET_BLOCK_KEY_TYPE_NDX_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_TS_GET_BLOCK_TS_GET_BLOCK_KEY_TYPE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_key_type_ndx = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter key_type_ndx after ts_get_block***", TRUE); 
    goto exit; 
  } 

  while (prm_block_range.iter < SOC_PETRA_PP_MACT_END && prm_block_range.entries_to_act && prm_block_range.entries_to_scan)
  {

    /* Call function */
    ret = swp_p_pp_ts_get_block(
            unit,
            prm_key_type_ndx,
            &prm_block_range,
            prm_keys_list,
            prm_actions_list,
            places,
            &prm_nof_entries
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
    { 
      send_string_to_screen(" *** swp_p_pp_ts_get_block - FAIL", TRUE); 
      soc_petra_disp_result(ret, "swp_p_pp_ts_get_block");   
      goto exit; 
    } 

    total_entries += prm_nof_entries;
    if (prm_nof_entries == 0)
    {
      break;
    }

    ret = swp_p_pp_ts_print_block(
            unit,
            prm_key_type_ndx,
            prm_keys_list,
            prm_actions_list,
            places,
            prm_nof_entries,
            flavor
          );
    if (prm_block_range.iter < SWP_P_PP_BR_ITER_END && prm_nof_entries >= prm_block_range.entries_to_act)
    {
      soc_sand_os_printf( "Hit Space to continue/Any Key to Abort.\n\r");
      con = ask_get(user_msg,EXPECT_CONT_ABORT,FALSE,TRUE) ;
      if (!con)
      {
        break;
      }
    }
  }
  soc_sand_os_printf( "nof returned entries: %lu\n\r",total_entries);

  
  /*swp_p_pp_SWP_P_PP_TS_KEY_print(&prm_keys_list);
  swp_p_pp_SWP_P_PP_TS_ACTION_print(&prm_actions_list);
  soc_sand_os_printf( "nof_entries: %lu\n\r",prm_nof_entries);*/

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: ts_trap_get_block (section swp_p_pp_ts_app)
 */
int 
  ui_swp_p_pp_ts_app_ts_trap_get_block(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_SAND_TABLE_BLOCK_RANGE   
    prm_block_range;
  SWP_P_PP_TS_TRAP_KEY   
    prm_keys_list[SWP_P_PP_UI_BLOCK_SIZE];
  SOC_PETRA_PP_TCAM_PLACE_ID 
    places[SWP_P_PP_UI_BLOCK_SIZE];
  SWP_P_PP_TS_ACTION   
    prm_actions_list[SWP_P_PP_UI_BLOCK_SIZE];
  uint32   
    prm_nof_entries;
  uint32                                                                                            
    total_entries=0,
    flavor=0;
  char
    user_msg[5] = "";
  uint32 
    con;

   
  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_ts_app"); 
  soc_sand_proc_name = "swp_p_pp_ts_trap_get_block";
  soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&prm_block_range);
  prm_block_range.entries_to_act = SWP_P_PP_UI_BLOCK_SIZE;
  prm_block_range.entries_to_scan = 100000;
  prm_block_range.iter = 0;
  prm_nof_entries = 0;
 
  unit = swp_p_pp_get_default_unit(); 
 
  /* Get parameters */ 


  while (prm_block_range.iter < SOC_PETRA_PP_MACT_END && prm_block_range.entries_to_act && prm_block_range.entries_to_scan)
  {

    /* Call function */
    ret = swp_p_pp_ts_trap_get_block(
            unit,
            &prm_block_range,
            prm_keys_list,
            prm_actions_list,
            places,
            &prm_nof_entries
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
    { 
      send_string_to_screen(" *** swp_p_pp_ts_trap_get_block - FAIL", TRUE); 
      soc_petra_disp_result(ret, "swp_p_pp_ts_trap_get_block");   
      goto exit; 
    } 

    total_entries += prm_nof_entries;
    if (prm_nof_entries == 0)
    {
      break;
    }

    ret = swp_p_pp_ts_trap_print_block(
            unit,
            prm_keys_list,
            prm_actions_list,
            places,
            prm_nof_entries,
            flavor
          );
    if (prm_block_range.iter < SWP_P_PP_TS_ITER_END && prm_nof_entries >= prm_block_range.entries_to_act)
    {
      soc_sand_os_printf( "Hit Space to continue/Any Key to Abort.\n\r");
      con = ask_get(user_msg,EXPECT_CONT_ABORT,FALSE,TRUE) ;
      if (!con)
      {
        break;
      }
    }
  }
  soc_sand_os_printf( "nof returned entries: %lu\n\r",total_entries);

  
  /*swp_p_pp_SWP_P_PP_TS_KEY_print(&prm_keys_list);
  swp_p_pp_SWP_P_PP_TS_ACTION_print(&prm_actions_list);
  soc_sand_os_printf( "nof_entries: %lu\n\r",prm_nof_entries);*/

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  

/******************************************************************** 
 *  Function handler: ts_entry_remove (section swp_p_pp_ts_app)
 */
int 
  ui_swp_p_pp_ts_app_ts_entry_remove(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SWP_P_PP_TS_KEY   
    prm_key;
   
  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_ts_app"); 
  soc_sand_proc_name = "swp_p_pp_ts_entry_remove"; 
 
  unit = swp_p_pp_get_default_unit(); 
  swp_p_pp_SWP_P_PP_TS_KEY_clear(&prm_key);
 
  /* Get parameters */ 
  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_KEY_VALUE_L2_TP_DIP_ID,1,  
         &param_val,VAL_IP,err_msg)) 
  { 
    prm_key.value.l2_tp.dip = param_val->value.ip_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_KEY_VALUE_L2_TP_SESSION_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_KEY_VALUE_L2_TP_SESSION_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_key.value.l2_tp.session = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_KEY_VALUE_L2_TP_IN_LOCAL_PORT_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_KEY_VALUE_L2_TP_IN_LOCAL_PORT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_key.value.l2_tp.in_local_port = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_KEY_VALUE_L2_TP_TABLE_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_KEY_VALUE_L2_TP_TABLE_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_key.value.l2_tp.table_id = (uint32)param_val->value.ulong_value;
  } 

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_KEY_VALUE_UDP_MC_SIP_ID,1,  
         &param_val,VAL_IP,err_msg)) 
  { 
    prm_key.value.udp_mc.sip = param_val->value.ip_value;
  } 
  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_KEY_VALUE_UDP_MC_DIP_ID,1,  
         &param_val,VAL_IP,err_msg)) 
  { 
    prm_key.value.udp_mc.dip = param_val->value.ip_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_KEY_VALUE_UDP_MC_UDP_DEST_PORT_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_KEY_VALUE_UDP_MC_UDP_DEST_PORT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_key.value.udp_mc.udp_dest_port = (uint16)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_KEY_VALUE_UDP_MC_IN_LOCAL_PORT_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_KEY_VALUE_UDP_MC_IN_LOCAL_PORT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_key.value.udp_mc.in_local_port = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_KEY_VALUE_UDP_MC_TABLE_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_KEY_VALUE_UDP_MC_TABLE_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_key.value.udp_mc.table_id = (uint32)param_val->value.ulong_value;
  } 

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_KEY_VALUE_UDP_UC_DIP_ID,1,  
         &param_val,VAL_IP,err_msg)) 
  { 
    prm_key.value.udp_uc.dip = param_val->value.ip_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_KEY_VALUE_UDP_UC_UDP_DEST_PORT_VALID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_KEY_VALUE_UDP_UC_UDP_DEST_PORT_VALID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_key.value.udp_uc.udp_dest_port_valid = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_KEY_VALUE_UDP_UC_UDP_DEST_PORT_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_KEY_VALUE_UDP_UC_UDP_DEST_PORT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_key.value.udp_uc.udp_dest_port = (uint16)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_KEY_VALUE_UDP_UC_IN_LOCAL_PORT_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_KEY_VALUE_UDP_UC_IN_LOCAL_PORT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_key.value.udp_uc.in_local_port = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_KEY_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_KEY_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_key.type = param_val->numeric_equivalent;
    if (prm_key.type == SWP_P_PP_TS_KEY_TYPE_L2_TPV3)
    {
      prm_key.value.l2_tp.table_id = 4;
    }
    else if (prm_key.type == SWP_P_PP_TS_KEY_TYPE_UDP_MC)
    {
      prm_key.value.l2_tp.table_id = 2;
    }
    else
    {
      prm_key.value.l2_tp.table_id = 1;
    }
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_KEY_VALUE_UDP_UC_TABLE_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_KEY_VALUE_UDP_UC_TABLE_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_key.value.udp_uc.table_id = (uint32)param_val->value.ulong_value;
  } 



  /* Call function */
  ret = swp_p_pp_ts_entry_remove(
          unit,
          &prm_key
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** swp_p_pp_ts_entry_remove - FAIL", TRUE); 
    soc_petra_disp_result(ret, "swp_p_pp_ts_entry_remove");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: ts_clear_all (section swp_p_pp_ts_app)
 */
int 
  ui_swp_p_pp_ts_app_ts_clear_all(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
   
  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_ts_app"); 
  soc_sand_proc_name = "swp_p_pp_ts_clear_all"; 
 
  unit = swp_p_pp_get_default_unit(); 
 
  /* Get parameters */ 

  /* Call function */
  ret = swp_p_pp_ts_clear_all(
          unit
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** swp_p_pp_ts_clear_all - FAIL", TRUE); 
    soc_petra_disp_result(ret, "swp_p_pp_ts_clear_all");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: ts_trap_set (section swp_p_pp_ts_app)
 */
int 
  ui_swp_p_pp_ts_app_ts_trap_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SWP_P_PP_TS_TRAP_KEY   
    prm_trap_key;
  uint8   
    prm_trap;
  SWP_P_PP_TS_ACTION
    prm_trap_action;
  SOC_SAND_SUCCESS_FAILURE   
    prm_success;
   
  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_ts_app"); 
  soc_sand_proc_name = "swp_p_pp_ts_trap_set"; 
 
  unit = swp_p_pp_get_default_unit(); 
  swp_p_pp_SWP_P_PP_TS_TRAP_KEY_clear(&prm_trap_key);
  swp_p_pp_SWP_P_PP_TS_ACTION_clear(&prm_trap_action);
  prm_trap_action.dest.dest_type = SOC_SAND_PP_DEST_SINGLE_PORT;
 
  /* Get parameters */ 
  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_SWP_P_PP_TS_TRAP_SET_TS_TRAP_SET_TRAP_KEY_MAC_ID,1,  
         &param_val,VAL_TEXT,err_msg)) 
  { 
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_trap_key.mac));
  } 

  if (!get_val_of(
    current_line,(int *)&match_index,PARAM_SWP_P_PP_TS_TRAP_SET_TS_TRAP_SET_TRAP_KEY_VALUE_ID,1,
    &param_val,VAL_IP,err_msg))
  {
    prm_trap_key.value = param_val->value.ip_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_TRAP_SET_TS_TRAP_SET_TRAP_KEY_QUAL_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_TS_TRAP_SET_TS_TRAP_SET_TRAP_KEY_QUAL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_trap_key.qual = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter trap_key after ts_trap_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = swp_p_pp_ts_trap_get(
          unit,
          &prm_trap_key,
          &prm_trap,
          &prm_trap_action
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** swp_p_pp_ts_trap_get - FAIL", TRUE); 
    soc_petra_disp_result(ret, "swp_p_pp_ts_trap_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_TRAP_SET_TS_TRAP_SET_TRAP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_TS_TRAP_SET_TS_TRAP_SET_TRAP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_trap = (uint8)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_TRAP_SET_TS_TRAP_SET_ACTION_DEST_DEST_VAL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_TS_TRAP_SET_TS_TRAP_SET_ACTION_DEST_DEST_VAL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_trap_action.dest.dest_val = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_TRAP_SET_TS_TRAP_SET_ACTION_DEST_DEST_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_TS_TRAP_SET_TS_TRAP_SET_ACTION_DEST_DEST_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_trap_action.dest.dest_type = param_val->numeric_equivalent;
  } 


  /* Call function */
  ret = swp_p_pp_ts_trap_set(
          unit,
          &prm_trap_key,
          prm_trap,
          &prm_trap_action,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** swp_p_pp_ts_trap_set - FAIL", TRUE); 
    soc_petra_disp_result(ret, "swp_p_pp_ts_trap_set");   
    goto exit; 
  } 

  send_string_to_screen("--> success:", TRUE);
  soc_sand_os_printf( "success: %s\n",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: ts_trap_get (section swp_p_pp_ts_app)
 */
int 
  ui_swp_p_pp_ts_app_ts_trap_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SWP_P_PP_TS_TRAP_KEY   
    prm_trap_key;
  uint8   
    prm_trap;
  SWP_P_PP_TS_ACTION
    prm_trap_action;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_ts_app"); 
  soc_sand_proc_name = "swp_p_pp_ts_trap_get"; 
 
  unit = swp_p_pp_get_default_unit(); 
  swp_p_pp_SWP_P_PP_TS_TRAP_KEY_clear(&prm_trap_key);
 
  /* Get parameters */ 
  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_SWP_P_PP_TS_TRAP_GET_TS_TRAP_GET_TRAP_KEY_MAC_ID,1,  
         &param_val,VAL_TEXT,err_msg)) 
  { 
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_trap_key.mac));
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_TRAP_GET_TS_TRAP_GET_TRAP_KEY_VALUE_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_TS_TRAP_GET_TS_TRAP_GET_TRAP_KEY_VALUE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_trap_key.value = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_TRAP_GET_TS_TRAP_GET_TRAP_KEY_QUAL_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_TS_TRAP_GET_TS_TRAP_GET_TRAP_KEY_QUAL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_trap_key.qual = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter trap_key after ts_trap_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = swp_p_pp_ts_trap_get(
          unit,
          &prm_trap_key,
          &prm_trap,
          &prm_trap_action
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** swp_p_pp_ts_trap_get - FAIL", TRUE); 
    soc_petra_disp_result(ret, "swp_p_pp_ts_trap_get");   
    goto exit; 
  } 

  soc_sand_os_printf( "trap: %u\n\r",prm_trap);
  swp_p_pp_SWP_P_PP_TS_ACTION_print(&prm_trap_action);
  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
#endif


#ifdef UI_VLAN
/******************************************************************** 
 *  Function handler: br_app_init (section swp_p_pp_tst6)
 */
int 
  ui_swp_p_pp_tst6_br_app_init(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
   
  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_tst6"); 
  soc_sand_proc_name = "swp_p_pp_br_app_init"; 
 
  unit = swp_p_pp_get_default_unit(); 
 
  /* Call function */
  ret = swp_p_pp_br_app_init(
          unit
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** swp_p_pp_br_app_init - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "swp_p_pp_br_app_init");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: br_app_close (section swp_p_pp_tst6)
 */
int 
  ui_swp_p_pp_tst6_br_app_close(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
   
  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_tst6"); 
  soc_sand_proc_name = "swp_p_pp_br_app_close"; 
 
  unit = swp_p_pp_get_default_unit(); 
 

  /* Call function */
  ret = swp_p_pp_br_app_close(
          unit
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** swp_p_pp_br_app_close - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "swp_p_pp_br_app_close");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: br_entry_add (section swp_p_pp_tst6)
 */
int 
  ui_swp_p_pp_tst6_br_entry_add(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SWP_P_PP_BR_KEY   
    prm_key;
  SWP_P_PP_BR_ACTION   
    prm_action;
  SOC_SAND_SUCCESS_FAILURE   
    prm_success;
   
  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_tst6"); 
  soc_sand_proc_name = "swp_p_pp_br_entry_add"; 
 
  unit = swp_p_pp_get_default_unit(); 
  SWP_P_PP_BR_KEY_clear(&prm_key);
  SWP_P_PP_BR_ACTION_clear(&prm_action);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_BR_ENTRY_ADD_BR_ENTRY_ADD_KEY_PRIO_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_BR_ENTRY_ADD_BR_ENTRY_ADD_KEY_PRIO_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_key.prio = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_BR_ENTRY_ADD_BR_ENTRY_ADD_KEY_VALUE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_BR_ENTRY_ADD_BR_ENTRY_ADD_KEY_VALUE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_key.value[0] = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_BR_ENTRY_ADD_BR_ENTRY_ADD_KEY_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_BR_ENTRY_ADD_BR_ENTRY_ADD_KEY_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_key.type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_BR_ENTRY_ADD_BR_ENTRY_ADD_ACTION_DEST_DEST_VAL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_BR_ENTRY_ADD_BR_ENTRY_ADD_ACTION_DEST_DEST_VAL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_action.dest.dest_val = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_BR_ENTRY_ADD_BR_ENTRY_ADD_ACTION_DEST_DEST_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_BR_ENTRY_ADD_BR_ENTRY_ADD_ACTION_DEST_DEST_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_action.dest.dest_type = param_val->numeric_equivalent;
  } 


  /* Call function */
  ret = swp_p_pp_br_entry_add(
          unit,
          &prm_key,
          &prm_action,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** swp_p_pp_br_entry_add - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "swp_p_pp_br_entry_add");   
    goto exit; 
  } 


  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: br_entry_get (section swp_p_pp_tst6)
 */
int 
  ui_swp_p_pp_tst6_br_entry_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SWP_P_PP_BR_KEY   
    prm_key;
  SWP_P_PP_BR_ACTION   
    prm_action;
  uint8   
    prm_found;
   
  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_tst6"); 
  soc_sand_proc_name = "swp_p_pp_br_entry_get"; 
 
  unit = swp_p_pp_get_default_unit(); 
  SWP_P_PP_BR_KEY_clear(&prm_key);
  SWP_P_PP_BR_ACTION_clear(&prm_action);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_BR_ENTRY_GET_BR_ENTRY_GET_KEY_PRIO_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_BR_ENTRY_GET_BR_ENTRY_GET_KEY_PRIO_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_key.prio = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_BR_ENTRY_GET_BR_ENTRY_GET_KEY_VALUE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_BR_ENTRY_GET_BR_ENTRY_GET_KEY_VALUE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_key.value[0] = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_BR_ENTRY_GET_BR_ENTRY_GET_KEY_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_BR_ENTRY_GET_BR_ENTRY_GET_KEY_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_key.type = param_val->numeric_equivalent;
  } 


  /* Call function */
  ret = swp_p_pp_br_entry_get(
          unit,
          &prm_key,
          &prm_action,
          &prm_found
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** swp_p_pp_br_entry_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "swp_p_pp_br_entry_get");   
    goto exit; 
  } 

  SWP_P_PP_BR_ACTION_print(&prm_action);

  soc_sand_os_printf( "found: %u\n\r",prm_found);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: br_get_block (section swp_p_pp_tst6)
 */
int 
  ui_swp_p_pp_tst6_br_get_block(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SWP_P_PP_BR_KEY_TYPE   
    prm_key_type_ndx;
  SOC_SAND_TABLE_BLOCK_RANGE   
    prm_block_range;
  SWP_P_PP_BR_KEY   
    prm_keys_list[SWP_P_PP_UI_BLOCK_SIZE];
  SWP_P_PP_BR_ACTION   
    prm_actions_list[SWP_P_PP_UI_BLOCK_SIZE];
  SOC_PETRA_PP_TCAM_PLACE_ID   
    prm_places[SWP_P_PP_UI_BLOCK_SIZE];
  uint32   
    prm_nof_entries;
  uint32                                                                                            
    total_entries=0,
    flavor=0;
  char
    user_msg[5] = "";
  uint32 
    con;

   
  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_tst6"); 
  soc_sand_proc_name = "swp_p_pp_br_get_block"; 
 
  soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&prm_block_range);
  prm_block_range.entries_to_act = SWP_P_PP_UI_BLOCK_SIZE;
  prm_block_range.entries_to_scan = 100000;
  prm_block_range.iter = 0;
  prm_nof_entries = 0;
  prm_key_type_ndx = SWP_P_PP_BR_KEY_TYPE_IPTV_HQOS;

  unit = swp_p_pp_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_BR_GET_BLOCK_BR_GET_BLOCK_KEY_TYPE_NDX_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_BR_GET_BLOCK_BR_GET_BLOCK_KEY_TYPE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_key_type_ndx = param_val->numeric_equivalent;
  } 

  while (prm_block_range.iter != SWP_P_PP_BR_ITER_END && prm_block_range.entries_to_act && prm_block_range.entries_to_scan)
  {

    /* Call function */
    ret = swp_p_pp_br_get_block(
            unit,
            prm_key_type_ndx,
            &prm_block_range,
            prm_keys_list,
            prm_actions_list,
            prm_places,
            &prm_nof_entries
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
    { 
      send_string_to_screen(" *** swp_p_pp_br_get_block - FAIL", TRUE); 
      soc_sand_disp_result_proc(ret, "swp_p_pp_br_get_block");   
      goto exit; 
    }

    total_entries += prm_nof_entries;
    if (prm_nof_entries == 0)
    {
      break;
    }

    ret = swp_p_pp_br_print_block(
            unit,
            prm_key_type_ndx,
            prm_keys_list,
            prm_actions_list,
            prm_places,
            prm_nof_entries,
            flavor
          );
    if (prm_block_range.iter < SWP_P_PP_BR_ITER_END && prm_nof_entries >= prm_block_range.entries_to_act)
    {
      soc_sand_os_printf( "Hit Space to continue/Any Key to Abort.\n\r");
      con = ask_get(user_msg,EXPECT_CONT_ABORT,FALSE,TRUE) ;
      if (!con)
      {
        break;
      }
    }
  }
  soc_sand_os_printf( "nof returned entries: %lu\n\r",total_entries);

    
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: br_print_block (section swp_p_pp_tst6)
 */
int 
  ui_swp_p_pp_tst6_br_print_block(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SWP_P_PP_BR_KEY_TYPE   
    prm_key_type_ndx;
  SWP_P_PP_BR_KEY   
    prm_keys_list;
  SWP_P_PP_BR_ACTION   
    prm_actions_list;
  SOC_PETRA_PP_TCAM_PLACE_ID   
    prm_places;
  uint32   
    prm_nof_entries=0;
  uint32   
    prm_flavors=0;
   
  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_tst6"); 
  soc_sand_proc_name = "swp_p_pp_br_print_block"; 
 
  unit = swp_p_pp_get_default_unit(); 
  SWP_P_PP_BR_KEY_clear(&prm_keys_list);
  SWP_P_PP_BR_ACTION_clear(&prm_actions_list);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_BR_PRINT_BLOCK_BR_PRINT_BLOCK_KEY_TYPE_NDX_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_BR_PRINT_BLOCK_BR_PRINT_BLOCK_KEY_TYPE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_key_type_ndx = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter key_type_ndx after br_print_block***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_BR_PRINT_BLOCK_BR_PRINT_BLOCK_KEYS_LIST_PRIO_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_BR_PRINT_BLOCK_BR_PRINT_BLOCK_KEYS_LIST_PRIO_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_keys_list.prio = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_BR_PRINT_BLOCK_BR_PRINT_BLOCK_KEYS_LIST_VALUE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_BR_PRINT_BLOCK_BR_PRINT_BLOCK_KEYS_LIST_VALUE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_keys_list.value[0] = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_BR_PRINT_BLOCK_BR_PRINT_BLOCK_KEYS_LIST_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_BR_PRINT_BLOCK_BR_PRINT_BLOCK_KEYS_LIST_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_keys_list.type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_BR_PRINT_BLOCK_BR_PRINT_BLOCK_ACTIONS_LIST_DEST_DEST_VAL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_BR_PRINT_BLOCK_BR_PRINT_BLOCK_ACTIONS_LIST_DEST_DEST_VAL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_actions_list.dest.dest_val = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_BR_PRINT_BLOCK_BR_PRINT_BLOCK_ACTIONS_LIST_DEST_DEST_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_BR_PRINT_BLOCK_BR_PRINT_BLOCK_ACTIONS_LIST_DEST_DEST_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_actions_list.dest.dest_type = param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_BR_PRINT_BLOCK_BR_PRINT_BLOCK_NOF_ENTRIES_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_BR_PRINT_BLOCK_BR_PRINT_BLOCK_NOF_ENTRIES_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_nof_entries = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_BR_PRINT_BLOCK_BR_PRINT_BLOCK_FLAVORS_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_BR_PRINT_BLOCK_BR_PRINT_BLOCK_FLAVORS_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_flavors = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = swp_p_pp_br_print_block(
          unit,
          prm_key_type_ndx,
          &prm_keys_list,
          &prm_actions_list,
          &prm_places,
          prm_nof_entries,
          prm_flavors
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** swp_p_pp_br_print_block - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "swp_p_pp_br_print_block");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: br_entry_remove (section swp_p_pp_tst6)
 */
int 
  ui_swp_p_pp_tst6_br_entry_remove(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SWP_P_PP_BR_KEY   
    prm_key;
   
  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_tst6"); 
  soc_sand_proc_name = "swp_p_pp_br_entry_remove"; 
 
  unit = swp_p_pp_get_default_unit(); 
  SWP_P_PP_BR_KEY_clear(&prm_key);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_BR_ENTRY_REMOVE_BR_ENTRY_REMOVE_KEY_PRIO_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_BR_ENTRY_REMOVE_BR_ENTRY_REMOVE_KEY_PRIO_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_key.prio = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_BR_ENTRY_REMOVE_BR_ENTRY_REMOVE_KEY_VALUE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_BR_ENTRY_REMOVE_BR_ENTRY_REMOVE_KEY_VALUE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_key.value[0] = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_BR_ENTRY_REMOVE_BR_ENTRY_REMOVE_KEY_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_BR_ENTRY_REMOVE_BR_ENTRY_REMOVE_KEY_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_key.type = param_val->numeric_equivalent;
  } 


  /* Call function */
  ret = swp_p_pp_br_entry_remove(
          unit,
          &prm_key
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** swp_p_pp_br_entry_remove - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "swp_p_pp_br_entry_remove");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: br_clear_all (section swp_p_pp_tst6)
 */
int 
  ui_swp_p_pp_tst6_br_clear_all(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
   
  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_tst6"); 
  soc_sand_proc_name = "swp_p_pp_br_clear_all"; 
 
  unit = swp_p_pp_get_default_unit(); 
 
  /* Get parameters */ 

  /* Call function */
  ret = swp_p_pp_br_clear_all(
          unit
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** swp_p_pp_br_clear_all - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "swp_p_pp_br_clear_all");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: br_port_info_set (section swp_p_pp_tst6)
 */
int 
  ui_swp_p_pp_tst6_br_port_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_in_local_port=0;
  SWP_P_PP_BR_PORT_INFO   
    prm_port_info;
   
  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_tst6"); 
  soc_sand_proc_name = "swp_p_pp_br_port_info_set"; 
 
  unit = swp_p_pp_get_default_unit(); 
  SWP_P_PP_BR_PORT_INFO_clear(&prm_port_info);
 
  /* Get parameters */ 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_BR_PORT_INFO_SET_BR_PORT_INFO_SET_IN_LOCAL_PORT_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_BR_PORT_INFO_SET_BR_PORT_INFO_SET_IN_LOCAL_PORT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_in_local_port = (uint32)param_val->value.ulong_value;
  } 

  /* This is a set function, so call GET function first */
  ret = swp_p_pp_br_port_info_get(
          unit,
          prm_in_local_port,
          &prm_port_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** swp_p_pp_br_port_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "swp_p_pp_br_port_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_BR_PORT_INFO_SET_BR_PORT_INFO_SET_PORT_INFO_DFLT_ACTION_DEST_DEST_VAL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_BR_PORT_INFO_SET_BR_PORT_INFO_SET_PORT_INFO_DFLT_ACTION_DEST_DEST_VAL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_info.dflt_action.dest.dest_val = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_BR_PORT_INFO_SET_BR_PORT_INFO_SET_PORT_INFO_DFLT_ACTION_DEST_DEST_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_BR_PORT_INFO_SET_BR_PORT_INFO_SET_PORT_INFO_DFLT_ACTION_DEST_DEST_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_info.dflt_action.dest.dest_type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_BR_PORT_INFO_SET_BR_PORT_INFO_SET_PORT_INFO_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_PP_BR_PORT_INFO_SET_BR_PORT_INFO_SET_PORT_INFO_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_info.type = param_val->numeric_equivalent;
  } 


  /* Call function */
  ret = swp_p_pp_br_port_info_set(
          unit,
          prm_in_local_port,
          &prm_port_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** swp_p_pp_br_port_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "swp_p_pp_br_port_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: br_port_info_get (section swp_p_pp_tst6)
 */
int 
  ui_swp_p_pp_tst6_br_port_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_in_local_port=0;
  SWP_P_PP_BR_PORT_INFO   
    prm_port_info;
   
  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_tst6"); 
  soc_sand_proc_name = "swp_p_pp_br_port_info_get"; 
 
  unit = swp_p_pp_get_default_unit(); 
  SWP_P_PP_BR_PORT_INFO_clear(&prm_port_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_BR_PORT_INFO_GET_BR_PORT_INFO_GET_IN_LOCAL_PORT_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_BR_PORT_INFO_GET_BR_PORT_INFO_GET_IN_LOCAL_PORT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_in_local_port = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = swp_p_pp_br_port_info_get(
          unit,
          prm_in_local_port,
          &prm_port_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** swp_p_pp_br_port_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "swp_p_pp_br_port_info_get");   
    goto exit; 
  } 

  soc_sand_os_printf( "in_local_port: %lu\n\r",prm_in_local_port);

  SWP_P_PP_BR_PORT_INFO_print(&prm_port_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: br_app_run (section swp_p_pp_tst6)
 */
int 
  ui_swp_p_pp_tst6_br_app_run(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
   
  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_tst6"); 
  soc_sand_proc_name = "swp_p_pp_br_app_run"; 
 
  unit = swp_p_pp_get_default_unit(); 
 
  /* Get parameters */ 

  /* Call function */
  ret = swp_p_pp_br_app_run(
          unit
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** swp_p_pp_br_app_run - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "swp_p_pp_br_app_run");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
#endif
#ifdef UI_VLAN/* { swp_p_pp_tst6*/
/******************************************************************** 
 *  Section handler: swp_p_pp_tst6
 */ 
int 
  ui_swp_p_pp_tst6( 
    CURRENT_LINE *current_line 
  ) 
{   
  int
    ret;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_tst6"); 
 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_BR_APP_INIT_BR_APP_INIT_ID,1)) 
  { 
    ret = ui_swp_p_pp_tst6_br_app_init(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_BR_APP_CLOSE_BR_APP_CLOSE_ID,1)) 
  { 
    ret = ui_swp_p_pp_tst6_br_app_close(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_BR_ENTRY_ADD_BR_ENTRY_ADD_ID,1)) 
  { 
    ret = ui_swp_p_pp_tst6_br_entry_add(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_BR_ENTRY_GET_BR_ENTRY_GET_ID,1)) 
  { 
    ret = ui_swp_p_pp_tst6_br_entry_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_BR_GET_BLOCK_BR_GET_BLOCK_ID,1)) 
  { 
    ret = ui_swp_p_pp_tst6_br_get_block(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_BR_PRINT_BLOCK_BR_PRINT_BLOCK_ID,1)) 
  { 
    ret = ui_swp_p_pp_tst6_br_print_block(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_BR_ENTRY_REMOVE_BR_ENTRY_REMOVE_ID,1)) 
  { 
    ret = ui_swp_p_pp_tst6_br_entry_remove(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_BR_CLEAR_ALL_BR_CLEAR_ALL_ID,1)) 
  { 
    ret = ui_swp_p_pp_tst6_br_clear_all(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_BR_PORT_INFO_SET_BR_PORT_INFO_SET_ID,1)) 
  { 
    ret = ui_swp_p_pp_tst6_br_port_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_BR_PORT_INFO_GET_BR_PORT_INFO_GET_ID,1)) 
  { 
    ret = ui_swp_p_pp_tst6_br_port_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_BR_APP_RUN_BR_APP_RUN_ID,1)) 
  { 
    ret = ui_swp_p_pp_tst6_br_app_run(current_line); 
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting function name after swp_p_pp_tst6***", TRUE); 
  } 
    
  goto exit; 
exit:        
  return ui_ret; 
}                
          
#endif /* swp_p_pp_tst6 */ 


#if UI_SWP_P_PP_MLAG_APP/* { swp_p_pp_mlag_app*/
/******************************************************************** 
 *  Function handler: mlag_port_set (section swp_p_pp_mlag_app)
 */
int 
  ui_swp_p_pp_mlag_app_mlag_port_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint8   
    prm_learn_enable;
  uint32
    prm_local_port_id;
  SOC_SAND_SUCCESS_FAILURE   
    prm_success;
   
  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_mlag_app"); 
  soc_sand_proc_name = "swp_p_pp_mlag_port_set"; 
 
  unit = swp_p_pp_get_default_unit(); 


  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MLAG_APP_INIT_MLAG_APP_PORT_ID_SET_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_MLAG_APP_INIT_MLAG_APP_PORT_ID_SET_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_local_port_id = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter trap_key after mlag_port_set***", TRUE); 
    goto exit; 
  } 
  
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MLAG_APP_INIT_MLAG_APP_LEARN_ENABLE_SET_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_MLAG_APP_INIT_MLAG_APP_LEARN_ENABLE_SET_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_learn_enable = (uint8)param_val->value.ulong_value;
  } 


  /* This is a set function, so call GET function first                                                                                
  ret = swp_p_pp_mlag_trap_get(
          unit,
          &prm_trap_key,
          &prm_trap,
          &prm_trap_action
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** swp_p_pp_mlag_trap_get - FAIL", TRUE); 
    soc_petra_disp_result(ret, "swp_p_pp_mlag_trap_get");   
    goto exit; 
  } */ 

  /* Call function */
  ret = swp_p_pp_mlag_port_learn_enable_set(
          unit,
          prm_local_port_id,
          prm_learn_enable,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** swp_p_pp_mlag_port_learn_enable_set - FAIL", TRUE); 
    soc_petra_disp_result(ret, "swp_p_pp_mlag_port_learn_enable_set");   
    goto exit; 
  } 

  send_string_to_screen("--> success:", TRUE);
  soc_sand_os_printf( "success: %s\n",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));

  
  goto exit; 
exit: 
  return ui_ret; 
} 

/******************************************************************** 
 *  Function handler: mlag_port_get (section swp_p_pp_mlag_app)
 */
int 
  ui_swp_p_pp_mlag_app_mlag_port_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint8   
    prm_learn_enable;
  uint32
    prm_local_port_id;
   
  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_mlag_app"); 
  soc_sand_proc_name = "swp_p_pp_mlag_port_get"; 
 
  unit = swp_p_pp_get_default_unit(); 


  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MLAG_APP_INIT_MLAG_APP_PORT_ID_GET_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_PP_MLAG_APP_INIT_MLAG_APP_PORT_ID_GET_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_local_port_id = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter trap_key after mlag_port_get***", TRUE); 
    goto exit; 
  } 
 

  /* Call function */
  ret = swp_p_pp_mlag_port_learn_enable_get(
          unit,
          prm_local_port_id,
          &prm_learn_enable
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** swp_p_pp_mlag_port_learn_enable_get - FAIL", TRUE); 
    soc_petra_disp_result(ret, "swp_p_pp_mlag_port_learn_enable_get");   
    goto exit; 
  } 

  soc_sand_os_printf( "prm_learn_enable: %u\n\r",prm_learn_enable);
  
  
  goto exit; 
exit: 
  return ui_ret; 
} 

int 
  ui_swp_p_pp_mlag_app( 
    CURRENT_LINE *current_line 
  ) 
{
  int
    ret;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_mlag_app"); 
 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MLAG_APP_INIT_MLAG_APP_INIT_ID,1)) 
  { 
    ret = SOC_SAND_OK;/*ui_swp_p_pp_mlag_app_mlag_app_init(current_line); */
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MLAG_APP_INIT_MLAG_APP_PORT_SET_ID,1)) 
  { 
    ret = ui_swp_p_pp_mlag_app_mlag_port_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MLAG_APP_INIT_MLAG_APP_PORT_GET_ID,1)) 
  { 
    ret = ui_swp_p_pp_mlag_app_mlag_port_get(current_line); 
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting function name after swp_p_pp_mlag_app***", TRUE); 
  } 
    
  goto exit; 
exit:        
  return ui_ret; 
}      
#endif /* swp_p_pp_mlag_app */ 

#if UI_SWP_P_PP_TS_APP/* { swp_p_pp_ts_app*/
/******************************************************************** 
 *  Section handler: swp_p_pp_ts_app
 */ 
int 
  ui_swp_p_pp_ts_app( 
    CURRENT_LINE *current_line 
  ) 
{   
  int
    ret;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_ts_app"); 
 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_APP_INIT_TS_APP_INIT_ID,1)) 
  { 
    ret = ui_swp_p_pp_ts_app_ts_app_init(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_ID,1)) 
  { 
    ret = ui_swp_p_pp_ts_app_ts_entry_add(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_ID,1)) 
  { 
    ret = ui_swp_p_pp_ts_app_ts_entry_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_GET_BLOCK_TS_GET_BLOCK_ID,1)) 
  { 
    ret = ui_swp_p_pp_ts_app_ts_get_block(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_TRAP_GET_BLOCK_TS_TRAP_GET_BLOCK_ID,1)) 
  { 
    ret = ui_swp_p_pp_ts_app_ts_trap_get_block(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_ID,1)) 
  { 
    ret = ui_swp_p_pp_ts_app_ts_entry_remove(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_CLEAR_ALL_TS_CLEAR_ALL_ID,1)) 
  { 
    ret = ui_swp_p_pp_ts_app_ts_clear_all(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_TRAP_SET_TS_TRAP_SET_ID,1)) 
  { 
    ret = ui_swp_p_pp_ts_app_ts_trap_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_TRAP_GET_TS_TRAP_GET_ID,1)) 
  { 
    ret = ui_swp_p_pp_ts_app_ts_trap_get(current_line); 
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting function name after swp_p_pp_ts_app***", TRUE); 
  } 
    
  goto exit; 
exit:        
  return ui_ret; 
}                
          
#endif /* swp_p_pp_ts_app */ 

#ifdef UI_SWP_P_PP_APP/* { swp_p_pp_app*/
/********************************************************************
 *  Section handler: swp_p_pp_app
 */
int
  ui_swp_p_pp_app(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_app");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_APP_APP_ID,1))
  {
    ui_ret = ui_swp_p_pp_app_app(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_APP_RESET_APP_RESET_ID,1))
  {
    ui_ret = ui_swp_p_pp_app_app_reset(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after swp_p_pp_app***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* { */swp_p_pp_app

#ifdef UI_INIT/* { init*/
/********************************************************************
 *  Section handler: init
 */
int
  ui_swp_p_pp_api_init(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_init");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_init_sequence(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after init***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* { */init

#ifdef UI_VLAN/* { vlan*/
/********************************************************************
 *  Section handler: vlan
 */
int
  ui_swp_p_pp_api_vlan(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_vlan");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_VLAN_SET_SET_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_vlan_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_VLAN_GET_GET_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_vlan_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_VLAN_MEMBER_ADD_MEMBER_ADD_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_vlan_member_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_VLAN_MEMBER_REMOVE_MEMBER_REMOVE_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_vlan_member_remove(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after vlan***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* { */vlan

#ifdef UI_MACT/* { mact*/
/********************************************************************
 *  Section handler: mact
 */
int
  ui_swp_p_pp_api_mact(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_mact");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MACT_ENTRY_ADD_ENTRY_ADD_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_mact_entry_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MACT_RANGE_ADD_RANGE_ADD_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_mact_range_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MACT_ENTRY_REMOVE_ENTRY_REMOVE_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_mact_entry_remove(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MACT_RANGE_REMOVE_RANGE_REMOVE_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_mact_range_remove(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MACT_FLUSH_BLOCK_FLUSH_BLOCK_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_mact_flush_block(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MACT_GET_BLOCK_GET_BLOCK_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_mact_get_block(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MACT_GET_ALL_GET_ALL_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_mact_get_all(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MACT_PRINT_ALL_PRINT_ALL_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_mact_print_all(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MACT_AGING_INFO_SET_AGING_INFO_SET_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_mact_aging_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MACT_AGING_INFO_GET_AGING_INFO_GET_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_mact_aging_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MACT_LEARN_MGMT_SET_LEARN_MGMT_SET_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_mact_learn_mgmt_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MACT_LEARN_MGMT_GET_LEARN_MGMT_GET_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_mact_learn_mgmt_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after mact***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* { */mact

#ifdef UI_IPV4_UC/* { ipv4_uc*/
/********************************************************************
 *  Section handler: ipv4_uc
 */
int
  ui_swp_p_pp_api_ipv4_uc(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_ipv4_uc");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_UC_ROUTE_ADD_ROUTE_ADD_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_ipv4_uc_route_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_UC_ROUTE_REMOVE_ROUTE_REMOVE_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_ipv4_uc_route_remove(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_UC_RANGE_ADD_RANGE_ADD_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_ipv4_uc_range_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_UC_RANGE_REMOVE_RANGE_REMOVE_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_ipv4_uc_range_remove(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_UC_ROUTE_GET_ALL_ROUTE_GET_ALL_ID,1))
  {
    /*ui_ret = ui_swp_p_pp_api_ipv4_uc_route_get_all(current_line);                                                              */
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_UC_ROUTE_GET_BLOCK_ROUTE_GET_BLOCK_ID,1))
  {
    /*ui_ret = ui_swp_p_pp_api_ipv4_uc_route_get_block(current_line);                                                              */
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_UC_ROUTE_PRINT_ALL_ROUTE_CLEAR_ALL_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_ipv4_uc_route_clear_all(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_RANGE_ADD_IPV4_RANGE_ADD_ID,1))
  {
    /*ui_ret = ui_swp_p_pp_api_ipv4_uc_ipv4_range_add(current_line);                                                              */
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_RANGE_REMOVE_IPV4_RANGE_REMOVE_ID,1))
  {
    /*ui_ret = ui_swp_p_pp_api_ipv4_uc_ipv4_range_remove(current_line);                                                              */
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_ROUTING_TBL_GET_ALL_IPV4_ROUTING_TBL_GET_ALL_ID,1))
  {
    /*ui_ret = ui_swp_p_pp_api_ipv4_uc_ipv4_routing_tbl_get_all(current_line);                                                              */
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_ROUTING_TBL_GET_BLOCK_IPV4_ROUTING_TBL_GET_BLOCK_ID,1))
  {
    /*ui_ret = ui_swp_p_pp_api_ipv4_uc_ipv4_routing_tbl_get_block(current_line);                                                              */
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_ROUTING_TBL_PRINT_ALL_IPV4_ROUTING_TBL_PRINT_ALL_ID,1))
  {
    /*ui_ret = ui_swp_p_pp_api_ipv4_uc_ipv4_routing_tbl_print_all(current_line);                                                              */
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after ipv4_uc***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* { */ipv4_uc


#ifdef UI_IPV4_MC/* { ipv4_mc*/
/********************************************************************
 *  Section handler: ipv4_mc
 */
int
  ui_swp_p_pp_api_ipv4_mc(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_petra_pp_api_ipv4_mc");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_INIT_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_ipv4_mc_ipv4_mc_init(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_ipv4_mc_ipv4_mc_route_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_MC_ROUTE_GET_IPV4_MC_ROUTE_GET_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_ipv4_mc_ipv4_mc_route_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_MC_ROUTE_LPM_GET_IPV4_MC_ROUTE_LPM_GET_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_ipv4_mc_ipv4_mc_route_lpm_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_MC_ROUTE_GET_BLOCK_IPV4_MC_ROUTE_GET_BLOCK_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_ipv4_mc_ipv4_mc_route_get_block(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_MC_ROUTE_REMOVE_IPV4_MC_ROUTE_REMOVE_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_ipv4_mc_ipv4_mc_route_remove(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after ipv4_mc***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* { ipv4_mc*/

#ifdef UI_IPV6/* { ipv6*/
/********************************************************************
 *  Section handler: ipv6
 */
int
  ui_swp_p_pp_api_ipv6(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_petra_pp_api_ipv6");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV6_ROUTE_ADD_IPV6_ROUTE_INIT_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_ipv6_init(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_ipv6_route_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV6_ROUTE_GET_IPV6_ROUTE_GET_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_ipv6_route_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV6_ROUTE_LPM_GET_IPV6_ROUTE_LPM_GET_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_ipv6_route_lpm_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV6_ROUTE_GET_BLOCK_IPV6_ROUTE_GET_BLOCK_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_ipv6_route_get_block(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV6_ROUTE_REMOVE_IPV6_ROUTE_REMOVE_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_ipv6_route_remove(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after ipv6***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* { ipv6*/

#ifdef UI_ACL/* { acl*/
/********************************************************************
 *  Section handler: acl
 */
int
  ui_swp_p_pp_api_acl(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_swp_p_pp_api_acl");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_acl_ace_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_REMOVE_ACE_REMOVE_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_acl_ace_remove(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_GET_ACE_GET_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_acl_ace_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_ACE_GET_BLOCK_ACE_GET_BLOCK_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_acl_ace_get_block(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_acl_range_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_ACL_RANGE_REMOVE_RANGE_REMOVE_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_acl_range_remove(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after acl***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* { */acl
/*****************************************************
*NAME
*  subject_swp_p_pp_api
*TYPE: PROC
*DATE: 29/DEC/2002
*FUNCTION:
*  Process input line which has an 'subject_swp_p_pp_api' subject.
*  Input line is assumed to have been checked and
*  found to be of right format.
*CALLING SEQUENCE:
*  subject_swp_p_pp_api(current_line,current_line_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    CURRENT_LINE *current_line -
*      Pointer to prompt line to process.
*    CURRENT_LINE **current_line_ptr -
*      Pointer to prompt line to be displayed after
*      this procedure finishes execution. Caller
*      points this variable to the pointer to
*      the next line to display. If called function wishes
*      to set the next line to display, it replaces
*      the pointer to the next line to display.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then some error has occurred.
*  SOC_SAND_INDIRECT:
*    Processing results. See 'current_line_ptr'.
*REMARKS:
*  This procedure should be carried out under 'task_safe'
*  state (i.e., task can not be deleted while this
*  procedure is being carried out).
*SEE ALSO:
 */
int
  subject_swp_p_pp_api(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  )
{
  unsigned int
    match_index;
  int
    ui_ret;
  unsigned int
    ;
  char
    *proc_name ;


  proc_name = "subject_swp_p_pp_api" ;
  ui_ret = FALSE ;
  unit = 0;


  /*
   * the rest of the line is empty
   */
  if (current_line->num_param_names == 0)
  {
    send_string_to_screen("\r\n\r",FALSE) ;
    send_string_to_screen("'subject_swp_p_pp_api()' function was called with no parameters.\r\n\r",FALSE) ;
    goto exit ;
  }
  /*
   * Else, there are parameters on the line (not just 'subject_swp_p_pp_api').
   */

  send_array_to_screen("\r\n\r",2) ;


  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_API_FEC_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_fec(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_APP_ID,1))
  {
    ui_ret = ui_swp_p_pp_app(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_API_INIT_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_init(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_API_VLAN_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_vlan(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_API_MACT_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_mact(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_API_IPV4_UC_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_ipv4_uc(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_IPV4_MC_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_ipv4_mc(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_API_IPV6_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_ipv6(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_API_ACL_ID,1))
  {
    ui_ret = ui_swp_p_pp_api_acl(current_line);
  }
/*#if SWP_P_CSTMR_SPEC_TS_APP*/
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TS_APP_ID,1)) 
  { 
    ui_ret = ui_swp_p_pp_ts_app(current_line); 
  } 
/*#endif*/
#if SWP_P_CSTMR_SPEC_MLAG_APP
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_MLAG_APP_ID,1)) 
  { 
    ui_ret = ui_swp_p_pp_mlag_app(current_line); 
  } 
#endif
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_PP_TST6_ID,1)) 
  { 
    ui_ret = ui_swp_p_pp_tst6(current_line); 
  } 

  else
  {
    /*
     * Enter if an unknown request.
     */
    send_string_to_screen(
      "\r\n\r"
      "*** swp_p_pp_api command with unknown parameters'.\r\n\r"
      "    Syntax error/sw error...\r\n\r",
      TRUE)  ;
    ui_ret = TRUE  ;
    goto exit  ;
  }

exit:
  return (ui_ret);
}


#endif
