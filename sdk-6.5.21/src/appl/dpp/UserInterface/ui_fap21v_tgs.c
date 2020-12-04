/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#include <appl/diag/dpp/ref_sys.h>
#include <bcm_app/dpp/../H/usrApp.h>

#include <soc/dpp/SOC_SAND_FAP21V/fap21v_framework.h>
#include <soc/dpp/SOC_SAND_FAP21V/fap21v_api_dram_calib.h>
#include <soc/dpp/SOC_SAND_FAP21V/fap21v_chip_tbls.h>
#include <soc/dpp/SOC_SAND_FAP21V/fap21v_tbl_access.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/SAND/SAND_FM/sand_mem_access.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>
#include <appl/dpp/UserInterface/ui_defi.h>
#include <appl/dpp/UserInterface/ui_pure_defi_fap21v_tgs.h>

#include <appl/diag/dpp/utils_defi.h>
#include <appl/diag/dpp/utils_screening.h>
#include <appl/diag/dpp/utils_app_tgs.h>

extern
  uint32
    Default_unit;

STATIC uint32
  get_default_unit()
{
  return Default_unit;
}

int
  ui_fap21v_tgs_app_run_app(
    CURRENT_LINE *current_line
  )
{
  int32
    ret;
  uint8
    silent = TRUE;
  uint8
    reset = FALSE;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_tgs_app");
  soc_sand_proc_name = "ui_fap21v_tgs_app";

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_TGS_APP_RUN_RESET_ID,1))
  {
    reset = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_TGS_APP_RUN_SILENT_ID,1))
  {
    silent = (uint8)param_val->value.ulong_value;
  }

  ret = tgs_application(
          get_default_unit(),
          reset,
          1,
          silent
        );
  if (ret != SOC_SAND_OK)
  {
    send_string_to_screen(" *** ui_tgs_app_run_app - FAIL", TRUE);
    fap21v_disp_result(ret, "ui_tgs_app_run_app");
    goto exit;
  }

  send_string_to_screen(" *** tgs_application - SUCCEEDED", TRUE);

exit:
  return ui_ret;
}

int
  ui_fap21v_tgs_app(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_tgs_app");
  soc_sand_proc_name = "ui_fap21v_tgs_app";

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_TGS_APP_RUN_ID,1))
  {
    ui_ret = ui_fap21v_tgs_app_run_app(current_line);
  }
  else
  {
    send_string_to_screen(
      "\r\n"
      "*** ui_fap21v_tgs_app command with unknown parameters'.\r\n"
      "    Syntax error/sw error...\r\n",
      TRUE)  ;
    ui_ret = TRUE  ;
    goto exit  ;
  }

exit:
  return ui_ret;
}

int
  subject_fap21v_tgs(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  )
{
  UI_MACROS_INIT_FUNCTION("subject_fap21v_tgs");
  soc_sand_proc_name = "subject_fap21v_tgs";

  proc_name = "subject_fap21v_tgs" ;
  ui_ret = FALSE ;
  unit = 0;


  /*
   * the rest of the line is empty
   */
  if (current_line->num_param_names == 0)
  {
    send_string_to_screen("\r\n",FALSE) ;
    send_string_to_screen("'subject_fap21v_tgs()' function was called with no parameters.\r\n",FALSE) ;
    goto exit ;
  }
  /*
   * Else, there are parameters on the line (not just 'subject_fap21v_api').
   */

  send_array_to_screen("\r\n",2) ;

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_TGS_APP_ID,1))
  {
    ui_ret = ui_fap21v_tgs_app(current_line);
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

