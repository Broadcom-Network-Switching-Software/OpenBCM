/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#include <appl/diag/dpp/ref_sys.h>
#include <bcm_app/dpp/../H/usrApp.h>
#if LINK_FAP21V_LIBRARIES

#include <soc/dpp/SOC_SAND_FAP21V/fap21v_framework.h>
#include <soc/dpp/SOC_SAND_FAP21V/fap21v_api_dram_calib.h>
#include <soc/dpp/SOC_SAND_FAP21V/fap21v_chip_tbls.h>
#include <soc/dpp/SOC_SAND_FAP21V/fap21v_tbl_access.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/SAND/SAND_FM/sand_mem_access.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>
#include <appl/dpp/UserInterface/ui_defi.h>
#include <appl/dpp/UserInterface/ui_pure_defi_fap21v_gfa.h>

#include <appl/diag/dpp/utils_dffs_cpu_mez.h>
#include <appl/diag/dpp/utils_defi.h>
#include <appl/diag/dpp/utils_screening.h>
#include <appl/diag/dpp/utils_line_GFA_FAP21V.h>
#include <appl/diag/dpp/Utils_line_GFA_FAP21V_DB.h>

extern
  uint32
    Default_unit;

STATIC uint32
  get_default_unit()
{
  return Default_unit;
}

int
  ui_fap21v_gfa_general_reset(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    delay_to_be_down_in_micro_sec = 100;
  uint32
    stay_down = FALSE;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_gfa_general_reset");
  soc_sand_proc_name = "ui_fap21v_gfa_general_reset";

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_GFA_GENERAL_RESET_DOWN_MILISEC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_GFA_GENERAL_RESET_DOWN_MILISEC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    delay_to_be_down_in_micro_sec = (uint32)param_val->value.ulong_value;
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_GFA_GENERAL_RESET_STAY_IN_RESET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_GFA_GENERAL_RESET_STAY_IN_RESET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    stay_down = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(
      "\r\n"
      "*** fap21v_api command with unknown parameters'.\r\n"
      "    Syntax error/sw error...\r\n",
      TRUE)  ;
    ui_ret = TRUE  ;
    goto exit  ;
  }

  ret = gfa_fap21v_reset_device(
          delay_to_be_down_in_micro_sec,
          stay_down
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

exit:
  return ui_ret;
}
int
  ui_fap21v_gfa_general_iddr_test_rw(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_gfa_general_iddr_test_rw");
  soc_sand_proc_name = "ui_fap21v_gfa_general_iddr_test_rw";

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_GFA_GENERAL_IDDR_TEST_RW_DATA_TO_TEST_ID,1))
  {
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_GFA_GENERAL_IDDR_TEST_RW_NOF_ITERATION_ID,1))
  {
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_GFA_GENERAL_IDDR_TEST_RW_OFFSET_ID,1))
  {
  }
  else
  {
    send_string_to_screen(
      "\r\n"
      "*** fap21v_api command with unknown parameters'.\r\n"
      "    Syntax error/sw error...\r\n",
      TRUE)  ;
    ui_ret = TRUE  ;
    goto exit  ;
  }

exit:
  return ui_ret;
}
int
  ui_fap21v_gfa_general_calibrate_full(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  int32
    offset_correct = 0;
  FAP21V_CALIB_INFO
    clbrate_info;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_gfa_general_calibrate_bist_full");
  soc_sand_proc_name = "ui_fap21v_gfa_general_calibrate_bist_full";

  fap21v_FAP21V_CALIB_INFO_clear(&clbrate_info, 300);

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_GFA_GENERAL_CALIBRATE_BIST_FULL_ALL_OFFSET_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_GFA_GENERAL_CALIBRATE_BIST_FULL_ALL_OFFSET_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    offset_correct = (int32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_GFA_GENERAL_CALIBRATE_BIST_ID,1))
  {
    ret = fap21v_calib_calibrate_bist_full_all(
            get_default_unit(),
            offset_correct,
            4,
            &clbrate_info,
            FALSE
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** FAIL", TRUE);
      fap21v_disp_result(ret, "");
      goto exit;
    }
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_GFA_GENERAL_CALIBRATE_LBG_ID,1))
  {
    ret = fap21v_calib_calibrate_lbg_full_all(
            get_default_unit(),
            offset_correct,
            4,
            &clbrate_info,
            FALSE
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** FAIL", TRUE);
      fap21v_disp_result(ret, "");
      goto exit;
    }
  }


  ret = fap21v_calib_calibrate_set(
          unit,
          4,
          &clbrate_info,
          FALSE
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** FAIL", TRUE);
    fap21v_disp_result(ret, "");
    goto exit;
  }

exit:
  return ui_ret;
}

int
  ui_fap21v_gfa_general(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_gfa_general");
  soc_sand_proc_name = "ui_fap21v_gfa_general";

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_GFA_GENERAL_CALIBRATE_FULL_ALL_ID,1))
  {
    ui_ret = ui_fap21v_gfa_general_calibrate_full(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_GFA_GENERAL_CALIBRATE_DELETE_FILE_ID,1))
  {
    ui_ret = utils_dffs_cpu_delete_file("fap21v_dram_calib_info", TRUE);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_GFA_GENERAL_CPU_TEST_RW_ID,1))
  {
    send_string_to_screen(
      "\r\n"
      "*** fap21v_gfa not implemented'.\r\n",
      TRUE)  ;
    ui_ret = TRUE  ;
    goto exit  ;
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_GFA_GENERAL_IDDR_TEST_RW_ID,1))
  {
    send_string_to_screen(
      "\r\n"
      "*** fap21v_gfa not implemented'.\r\n",
      TRUE)  ;
    ui_ret = TRUE  ;
    goto exit  ;
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_GFA_GENERAL_INQ_TEST_RW_ID,1))
  {
    send_string_to_screen(
      "\r\n"
      "*** fap21v_gfa not implemented'.\r\n",
      TRUE)  ;
    ui_ret = TRUE  ;
    goto exit  ;
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_GFA_GENERAL_RESET_ID,1))
  {
    ui_ret = ui_fap21v_gfa_general_reset(current_line);
  }
  else
  {
    send_string_to_screen(
      "\r\n"
      "*** fap21v_api command with unknown parameters'.\r\n"
      "    Syntax error/sw error...\r\n",
      TRUE)  ;
    ui_ret = TRUE  ;
    goto exit  ;
  }

exit:
  return ui_ret;
}

int
  ui_fap21v_gfa_mb_fpga(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_gfa_fpga_mb");
  soc_sand_proc_name = "ui_fap21v_gfa_fpga_mb";

  if (gfa_fap21v_is_gfa_mb() == FALSE)
  {
    send_string_to_screen("*** All DB CLI must run only on DB.\r\n",FALSE) ;
    send_string_to_screen("*** Try CLI 'gfa status'.\r\n",FALSE) ;
    goto exit ;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_GFA_MB_FGPA_DOWNLOAD_ID,1))
  {
    gfa_fap21v_db_fpga_download(TRUE);
  }
  else
  {
    send_string_to_screen(
      "\r\n"
      "*** fap21v_api command with unknown parameters'.\r\n"
      "    Syntax error/sw error...\r\n",
      TRUE)  ;
    ui_ret = TRUE  ;
    goto exit  ;
  }

exit:
  return ui_ret;
}

int
  ui_fap21v_gfa_mb_flash(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_gfa_flash_mb");
  soc_sand_proc_name = "ui_fap21v_gfa_flash_mb";

  if (gfa_fap21v_is_gfa_mb() == FALSE)
  {
    send_string_to_screen("*** All DB CLI must run only on DB.\r\n",FALSE) ;
    send_string_to_screen("*** Try CLI 'gfa status'.\r\n",FALSE) ;
    goto exit ;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_GFA_MB_FLASH_STATUS_ID,1))
  {
    gfa_fap21v_db_flash_status(TRUE);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_GFA_MB_FLASH_ERASE_ID,1))
  {
    gfa_fap21v_db_flash_erase(TRUE);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_GFA_MB_FLASH_DOWNLOAD_ID,1))
  {
    gfa_fap21v_db_flash_download(TRUE);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_GFA_MB_FLASH_BURN_FPGA_ID,1))
  {
    gfa_fap21v_db_flash_burn_fpga(TRUE);
  }
  else
  {
    send_string_to_screen(
      "\r\n"
      "*** fap21v_api command with unknown parameters'.\r\n"
      "    Syntax error/sw error...\r\n",
      TRUE)  ;
    ui_ret = TRUE  ;
    goto exit  ;
  }

exit:
  return ui_ret;
}

int
  ui_fap21v_gfa_mb(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_gfa_mb");
  soc_sand_proc_name = "ui_fap21v_gfa_mb";

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_GFA_MB_FPGA_ID,1))
  {
    ui_ret = ui_fap21v_gfa_mb_fpga(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_GFA_MB_FLASH_ID,1))
  {
    ui_ret = ui_fap21v_gfa_mb_flash(current_line);
  }
  else
  {
    send_string_to_screen(
      "\r\n"
      "*** fap21v_api command with unknown parameters'.\r\n"
      "    Syntax error/sw error...\r\n",
      TRUE)  ;
    ui_ret = TRUE  ;
    goto exit  ;
  }

exit:
  return ui_ret;
}

int
  ui_fap21v_gfa_screening(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_gfa_screening");
  soc_sand_proc_name = "ui_fap21v_gfa_screening";

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_GFA_SCREENING_FTG_TEST_ID,1))
  {
    send_string_to_screen(
      "\r\n"
      "*** fap21v_gfa not implemented'.\r\n",
      TRUE)  ;
    ui_ret = TRUE  ;
    goto exit  ;
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_GFA_SCREENING_INTERFACES_ID,1))
  {
    send_string_to_screen(
      "\r\n"
      "*** fap21v_gfa not implemented'.\r\n",
      TRUE)  ;
    ui_ret = TRUE  ;
    goto exit  ;
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_GFA_SCREENING_PRINT_MODE_ID,1))
  {
    utils_print_user_mode();
  }
  else
  {
    send_string_to_screen(
      "\r\n"
      "*** fap21v_api command with unknown parameters'.\r\n"
      "    Syntax error/sw error...\r\n",
      TRUE)  ;
    ui_ret = TRUE  ;
    goto exit  ;
  }

exit:
  return ui_ret;
}


int
  ui_fap21v_gfa_status(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_gfa_status");
  soc_sand_proc_name = "ui_fap21v_gfa_status";

  gfa_fap21v_print_status();

  return ui_ret;
}

int
  subject_fap21v_gfa(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  )
{
  UI_MACROS_INIT_FUNCTION("subject_fap21v_gfa");
  soc_sand_proc_name = "subject_fap21v_gfa";

  proc_name = "subject_fap21v_gfa" ;
  ui_ret = FALSE ;
  unit = 0;


  /*
   * the rest of the line is empty
   */
  if (current_line->num_param_names == 0)
  {
    send_string_to_screen("\r\n",FALSE) ;
    send_string_to_screen("'subject_fap21v_gfa()' function was called with no parameters.\r\n",FALSE) ;
    goto exit ;
  }
  /*
   * Else, there are parameters on the line (not just 'subject_fap21v_api').
   */

  send_array_to_screen("\r\n",2) ;

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_GFA_MB_ID,1))
  {
    ui_ret = ui_fap21v_gfa_mb(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_GFA_STATUS_ID,1))
  {
    ui_ret = ui_fap21v_gfa_status(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_GFA_GENERAL_ID,1))
  {
    ui_ret = ui_fap21v_gfa_general(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_GFA_SCREENING_ID,1))
  {
    ui_ret = ui_fap21v_gfa_screening(current_line);
  }
  else
  {
    send_string_to_screen(
      "\r\n"
      "*** fap21v_api command with unknown parameters'.\r\n"
      "    Syntax error/sw error...\r\n",
      TRUE)  ;
    ui_ret = TRUE  ;
    goto exit  ;
  }

exit:
  return (ui_ret);
}
#endif
