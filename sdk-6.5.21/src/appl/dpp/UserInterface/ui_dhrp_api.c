/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


/*
 * General include file for reference design.
 */
#include <DHRP/dhrp_defs.h>

#include <appl/diag/dpp/ref_sys.h>

/*
 * Utilities include file.
 */

#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <appl/diag/dpp/utils_defi.h>
#include <appl/dpp/UserInterface/ui_defi.h>

#include <appl/dpp/UserInterface/ui_pure_defi_dhrp_api.h>


#include <DHRP/dhrp_api_management.h>
#include <DHRP/dhrp_api_scheduling_scheme.h>
#include <DHRP/dhrp_api_multicast.h>
#include <DHRP/dhrp_api_diagnostics.h>
static
  uint32
    Default_unit = 0;

STATIC void
dhrp_set_default_unit(uint32 dev_id)
{
  Default_unit = dev_id;
}

STATIC uint32
dhrp_get_default_unit()
{
  return Default_unit;
}

/********************************************************************
 *  Function handler: init_phase_1 (section management)
 */
int
  ui_dhrp_api_management_init_phase_1(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;

  UI_MACROS_INIT_FUNCTION("ui_dhrp_api_management");
  soc_sand_proc_name = "dhrp_init_phase_1";

  unit = dhrp_get_default_unit();

  /* Get parameters */

  /* Call function */
  ret = dhrp_init_phase_1(
          NULL 
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** dhrp_init_phase_1 - FAIL", TRUE);
    goto exit;
  }

  send_string_to_screen(" *** dhrp_init_phase_1 - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: init_phase_2 (section management)
 */
int
  ui_dhrp_api_management_init_phase_2(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  DHRP_INIT_INFO
    prm_start_up_information;

  UI_MACROS_INIT_FUNCTION("ui_dhrp_api_management");
  soc_sand_proc_name = "dhrp_init_phase_2";

  unit = dhrp_get_default_unit();
  dhrp_DHRP_INIT_INFO_clear(&prm_start_up_information);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_3_CLASS_INFO_3_MIN_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_3_CLASS_INFO_3_MIN_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[3].class_info[3].min_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_3_clss_nfo_3_min_rte after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_3_CLASS_INFO_3_MAX_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_3_CLASS_INFO_3_MAX_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[3].class_info[3].max_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_3_clss_nfo_3_max_rte after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_3_CLASS_INFO_3_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_3_CLASS_INFO_3_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[3].class_info[3].weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_3_clss_nfo_3_wght after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_3_CLASS_INFO_3_STRICT_PRIORITY_LEVEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_3_CLASS_INFO_3_STRICT_PRIORITY_LEVEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[3].class_info[3].strict_priority_level  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_3_clss_nfo_3_strct_prrty_lvl after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_3_CLASS_INFO_2_MIN_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_3_CLASS_INFO_2_MIN_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[3].class_info[2].min_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_3_clss_nfo_2_min_rte after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_3_CLASS_INFO_2_MAX_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_3_CLASS_INFO_2_MAX_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[3].class_info[2].max_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_3_clss_nfo_2_max_rte after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_3_CLASS_INFO_2_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_3_CLASS_INFO_2_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[3].class_info[2].weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_3_clss_nfo_2_wght after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_3_CLASS_INFO_2_STRICT_PRIORITY_LEVEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_3_CLASS_INFO_2_STRICT_PRIORITY_LEVEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[3].class_info[2].strict_priority_level  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_3_clss_nfo_2_strct_prrty_lvl after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_3_CLASS_INFO_1_MIN_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_3_CLASS_INFO_1_MIN_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[3].class_info[1].min_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_3_clss_nfo_1_min_rte after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_3_CLASS_INFO_1_MAX_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_3_CLASS_INFO_1_MAX_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[3].class_info[1].max_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_3_clss_nfo_1_max_rte after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_3_CLASS_INFO_1_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_3_CLASS_INFO_1_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[3].class_info[1].weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_3_clss_nfo_1_wght after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_3_CLASS_INFO_1_STRICT_PRIORITY_LEVEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_3_CLASS_INFO_1_STRICT_PRIORITY_LEVEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[3].class_info[1].strict_priority_level  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_3_clss_nfo_1_strct_prrty_lvl after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_3_CLASS_INFO_0_MIN_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_3_CLASS_INFO_0_MIN_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[3].class_info[0].min_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_3_clss_nfo_0_min_rte after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_3_CLASS_INFO_0_MAX_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_3_CLASS_INFO_0_MAX_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[3].class_info[0].max_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_3_clss_nfo_0_max_rte after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_3_CLASS_INFO_0_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_3_CLASS_INFO_0_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[3].class_info[0].weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_3_clss_nfo_0_wght after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_3_CLASS_INFO_0_STRICT_PRIORITY_LEVEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_3_CLASS_INFO_0_STRICT_PRIORITY_LEVEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[3].class_info[0].strict_priority_level  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_3_clss_nfo_0_strct_prrty_lvl after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_2_CLASS_INFO_3_MIN_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_2_CLASS_INFO_3_MIN_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[2].class_info[3].min_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_2_clss_nfo_3_min_rte after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_2_CLASS_INFO_3_MAX_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_2_CLASS_INFO_3_MAX_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[2].class_info[3].max_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_2_clss_nfo_3_max_rte after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_2_CLASS_INFO_3_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_2_CLASS_INFO_3_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[2].class_info[3].weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_2_clss_nfo_3_wght after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_2_CLASS_INFO_3_STRICT_PRIORITY_LEVEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_2_CLASS_INFO_3_STRICT_PRIORITY_LEVEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[2].class_info[3].strict_priority_level  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_2_clss_nfo_3_strct_prrty_lvl after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_2_CLASS_INFO_2_MIN_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_2_CLASS_INFO_2_MIN_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[2].class_info[2].min_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_2_clss_nfo_2_min_rte after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_2_CLASS_INFO_2_MAX_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_2_CLASS_INFO_2_MAX_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[2].class_info[2].max_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_2_clss_nfo_2_max_rte after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_2_CLASS_INFO_2_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_2_CLASS_INFO_2_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[2].class_info[2].weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_2_clss_nfo_2_wght after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_2_CLASS_INFO_2_STRICT_PRIORITY_LEVEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_2_CLASS_INFO_2_STRICT_PRIORITY_LEVEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[2].class_info[2].strict_priority_level  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_2_clss_nfo_2_strct_prrty_lvl after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_2_CLASS_INFO_1_MIN_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_2_CLASS_INFO_1_MIN_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[2].class_info[1].min_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_2_clss_nfo_1_min_rte after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_2_CLASS_INFO_1_MAX_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_2_CLASS_INFO_1_MAX_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[2].class_info[1].max_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_2_clss_nfo_1_max_rte after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_2_CLASS_INFO_1_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_2_CLASS_INFO_1_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[2].class_info[1].weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_2_clss_nfo_1_wght after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_2_CLASS_INFO_1_STRICT_PRIORITY_LEVEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_2_CLASS_INFO_1_STRICT_PRIORITY_LEVEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[2].class_info[1].strict_priority_level  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_2_clss_nfo_1_strct_prrty_lvl after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_2_CLASS_INFO_0_MIN_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_2_CLASS_INFO_0_MIN_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[2].class_info[0].min_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_2_clss_nfo_0_min_rte after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_2_CLASS_INFO_0_MAX_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_2_CLASS_INFO_0_MAX_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[2].class_info[0].max_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_2_clss_nfo_0_max_rte after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_2_CLASS_INFO_0_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_2_CLASS_INFO_0_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[2].class_info[0].weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_2_clss_nfo_0_wght after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_2_CLASS_INFO_0_STRICT_PRIORITY_LEVEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_2_CLASS_INFO_0_STRICT_PRIORITY_LEVEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[2].class_info[0].strict_priority_level  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_2_clss_nfo_0_strct_prrty_lvl after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_1_CLASS_INFO_3_MIN_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_1_CLASS_INFO_3_MIN_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[1].class_info[3].min_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_1_clss_nfo_3_min_rte after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_1_CLASS_INFO_3_MAX_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_1_CLASS_INFO_3_MAX_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[1].class_info[3].max_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_1_clss_nfo_3_max_rte after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_1_CLASS_INFO_3_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_1_CLASS_INFO_3_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[1].class_info[3].weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_1_clss_nfo_3_wght after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_1_CLASS_INFO_3_STRICT_PRIORITY_LEVEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_1_CLASS_INFO_3_STRICT_PRIORITY_LEVEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[1].class_info[3].strict_priority_level  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_1_clss_nfo_3_strct_prrty_lvl after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_1_CLASS_INFO_2_MIN_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_1_CLASS_INFO_2_MIN_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[1].class_info[2].min_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_1_clss_nfo_2_min_rte after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_1_CLASS_INFO_2_MAX_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_1_CLASS_INFO_2_MAX_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[1].class_info[2].max_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_1_clss_nfo_2_max_rte after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_1_CLASS_INFO_2_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_1_CLASS_INFO_2_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[1].class_info[2].weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_1_clss_nfo_2_wght after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_1_CLASS_INFO_2_STRICT_PRIORITY_LEVEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_1_CLASS_INFO_2_STRICT_PRIORITY_LEVEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[1].class_info[2].strict_priority_level  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_1_clss_nfo_2_strct_prrty_lvl after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_1_CLASS_INFO_1_MIN_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_1_CLASS_INFO_1_MIN_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[1].class_info[1].min_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_1_clss_nfo_1_min_rte after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_1_CLASS_INFO_1_MAX_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_1_CLASS_INFO_1_MAX_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[1].class_info[1].max_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_1_clss_nfo_1_max_rte after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_1_CLASS_INFO_1_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_1_CLASS_INFO_1_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[1].class_info[1].weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_1_clss_nfo_1_wght after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_1_CLASS_INFO_1_STRICT_PRIORITY_LEVEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_1_CLASS_INFO_1_STRICT_PRIORITY_LEVEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[1].class_info[1].strict_priority_level  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_1_clss_nfo_1_strct_prrty_lvl after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_1_CLASS_INFO_0_MIN_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_1_CLASS_INFO_0_MIN_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[1].class_info[0].min_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_1_clss_nfo_0_min_rte after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_1_CLASS_INFO_0_MAX_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_1_CLASS_INFO_0_MAX_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[1].class_info[0].max_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_1_clss_nfo_0_max_rte after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_1_CLASS_INFO_0_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_1_CLASS_INFO_0_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[1].class_info[0].weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_1_clss_nfo_0_wght after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_1_CLASS_INFO_0_STRICT_PRIORITY_LEVEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_1_CLASS_INFO_0_STRICT_PRIORITY_LEVEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[1].class_info[0].strict_priority_level  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_1_clss_nfo_0_strct_prrty_lvl after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_0_CLASS_INFO_3_MIN_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_0_CLASS_INFO_3_MIN_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[0].class_info[3].min_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_0_clss_nfo_3_min_rte after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_0_CLASS_INFO_3_MAX_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_0_CLASS_INFO_3_MAX_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[0].class_info[3].max_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_0_clss_nfo_3_max_rte after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_0_CLASS_INFO_3_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_0_CLASS_INFO_3_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[0].class_info[3].weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_0_clss_nfo_3_wght after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_0_CLASS_INFO_3_STRICT_PRIORITY_LEVEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_0_CLASS_INFO_3_STRICT_PRIORITY_LEVEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[0].class_info[3].strict_priority_level  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_0_clss_nfo_3_strct_prrty_lvl after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_0_CLASS_INFO_2_MIN_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_0_CLASS_INFO_2_MIN_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[0].class_info[2].min_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_0_clss_nfo_2_min_rte after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_0_CLASS_INFO_2_MAX_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_0_CLASS_INFO_2_MAX_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[0].class_info[2].max_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_0_clss_nfo_2_max_rte after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_0_CLASS_INFO_2_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_0_CLASS_INFO_2_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[0].class_info[2].weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_0_clss_nfo_2_wght after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_0_CLASS_INFO_2_STRICT_PRIORITY_LEVEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_0_CLASS_INFO_2_STRICT_PRIORITY_LEVEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[0].class_info[2].strict_priority_level  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_0_clss_nfo_2_strct_prrty_lvl after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_0_CLASS_INFO_1_MIN_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_0_CLASS_INFO_1_MIN_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[0].class_info[1].min_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_0_clss_nfo_1_min_rte after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_0_CLASS_INFO_1_MAX_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_0_CLASS_INFO_1_MAX_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[0].class_info[1].max_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_0_clss_nfo_1_max_rte after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_0_CLASS_INFO_1_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_0_CLASS_INFO_1_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[0].class_info[1].weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_0_clss_nfo_1_wght after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_0_CLASS_INFO_1_STRICT_PRIORITY_LEVEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_0_CLASS_INFO_1_STRICT_PRIORITY_LEVEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[0].class_info[1].strict_priority_level  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_0_clss_nfo_1_strct_prrty_lvl after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_0_CLASS_INFO_0_MIN_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_0_CLASS_INFO_0_MIN_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[0].class_info[0].min_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_0_clss_nfo_0_min_rte after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_0_CLASS_INFO_0_MAX_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_0_CLASS_INFO_0_MAX_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[0].class_info[0].max_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_0_clss_nfo_0_max_rte after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_0_CLASS_INFO_0_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_0_CLASS_INFO_0_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[0].class_info[0].weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_0_clss_nfo_0_wght after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_0_CLASS_INFO_0_STRICT_PRIORITY_LEVEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_PORTS_SCH_SCM_0_CLASS_INFO_0_STRICT_PRIORITY_LEVEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_ports_sch_scm[0].class_info[0].strict_priority_level  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_prts_sch_scm_0_clss_nfo_0_strct_prrty_lvl after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_NOF_MULT_GROUPS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_NOF_MULT_GROUPS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.nof_mult_groups  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_nof_mlt_grps after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_3_DOMAIN_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_3_DOMAIN_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_mult_groups[3].domain_id  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_mlt_grps_3_dmn_id after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_3_NOF_PORTS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_3_NOF_PORTS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_mult_groups[3].nof_ports  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_mlt_grps_3_nof_prts after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_3_HRP_PORTS_3_NOF_COPIES_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_3_HRP_PORTS_3_NOF_COPIES_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_mult_groups[3].hrp_ports[3].nof_copies  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_mlt_grps_3_hrp_prts_3_nof_cps after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_3_HRP_PORTS_3_HRP_PORT_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_3_HRP_PORTS_3_HRP_PORT_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_mult_groups[3].hrp_ports[3].hrp_port_id  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_mlt_grps_3_hrp_prts_3_hrp_prt_id after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_3_HRP_PORTS_2_NOF_COPIES_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_3_HRP_PORTS_2_NOF_COPIES_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_mult_groups[3].hrp_ports[2].nof_copies  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_mlt_grps_3_hrp_prts_2_nof_cps after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_3_HRP_PORTS_2_HRP_PORT_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_3_HRP_PORTS_2_HRP_PORT_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_mult_groups[3].hrp_ports[2].hrp_port_id  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_mlt_grps_3_hrp_prts_2_hrp_prt_id after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_3_HRP_PORTS_1_NOF_COPIES_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_3_HRP_PORTS_1_NOF_COPIES_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_mult_groups[3].hrp_ports[1].nof_copies  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_mlt_grps_3_hrp_prts_1_nof_cps after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_3_HRP_PORTS_1_HRP_PORT_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_3_HRP_PORTS_1_HRP_PORT_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_mult_groups[3].hrp_ports[1].hrp_port_id  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_mlt_grps_3_hrp_prts_1_hrp_prt_id after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_3_HRP_PORTS_0_NOF_COPIES_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_3_HRP_PORTS_0_NOF_COPIES_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_mult_groups[3].hrp_ports[0].nof_copies  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_mlt_grps_3_hrp_prts_0_nof_cps after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_3_HRP_PORTS_0_HRP_PORT_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_3_HRP_PORTS_0_HRP_PORT_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_mult_groups[3].hrp_ports[0].hrp_port_id  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_mlt_grps_3_hrp_prts_0_hrp_prt_id after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_2_DOMAIN_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_2_DOMAIN_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_mult_groups[2].domain_id  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_mlt_grps_2_dmn_id after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_2_NOF_PORTS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_2_NOF_PORTS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_mult_groups[2].nof_ports  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_mlt_grps_2_nof_prts after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_2_HRP_PORTS_3_NOF_COPIES_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_2_HRP_PORTS_3_NOF_COPIES_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_mult_groups[2].hrp_ports[3].nof_copies  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_mlt_grps_2_hrp_prts_3_nof_cps after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_2_HRP_PORTS_3_HRP_PORT_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_2_HRP_PORTS_3_HRP_PORT_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_mult_groups[2].hrp_ports[3].hrp_port_id  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_mlt_grps_2_hrp_prts_3_hrp_prt_id after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_2_HRP_PORTS_2_NOF_COPIES_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_2_HRP_PORTS_2_NOF_COPIES_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_mult_groups[2].hrp_ports[2].nof_copies  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_mlt_grps_2_hrp_prts_2_nof_cps after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_2_HRP_PORTS_2_HRP_PORT_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_2_HRP_PORTS_2_HRP_PORT_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_mult_groups[2].hrp_ports[2].hrp_port_id  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_mlt_grps_2_hrp_prts_2_hrp_prt_id after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_2_HRP_PORTS_1_NOF_COPIES_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_2_HRP_PORTS_1_NOF_COPIES_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_mult_groups[2].hrp_ports[1].nof_copies  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_mlt_grps_2_hrp_prts_1_nof_cps after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_2_HRP_PORTS_1_HRP_PORT_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_2_HRP_PORTS_1_HRP_PORT_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_mult_groups[2].hrp_ports[1].hrp_port_id  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_mlt_grps_2_hrp_prts_1_hrp_prt_id after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_2_HRP_PORTS_0_NOF_COPIES_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_2_HRP_PORTS_0_NOF_COPIES_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_mult_groups[2].hrp_ports[0].nof_copies  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_mlt_grps_2_hrp_prts_0_nof_cps after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_2_HRP_PORTS_0_HRP_PORT_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_2_HRP_PORTS_0_HRP_PORT_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_mult_groups[2].hrp_ports[0].hrp_port_id  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_mlt_grps_2_hrp_prts_0_hrp_prt_id after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_1_DOMAIN_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_1_DOMAIN_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_mult_groups[1].domain_id  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_mlt_grps_1_dmn_id after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_1_NOF_PORTS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_1_NOF_PORTS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_mult_groups[1].nof_ports  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_mlt_grps_1_nof_prts after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_1_HRP_PORTS_3_NOF_COPIES_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_1_HRP_PORTS_3_NOF_COPIES_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_mult_groups[1].hrp_ports[3].nof_copies  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_mlt_grps_1_hrp_prts_3_nof_cps after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_1_HRP_PORTS_3_HRP_PORT_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_1_HRP_PORTS_3_HRP_PORT_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_mult_groups[1].hrp_ports[3].hrp_port_id  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_mlt_grps_1_hrp_prts_3_hrp_prt_id after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_1_HRP_PORTS_2_NOF_COPIES_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_1_HRP_PORTS_2_NOF_COPIES_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_mult_groups[1].hrp_ports[2].nof_copies  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_mlt_grps_1_hrp_prts_2_nof_cps after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_1_HRP_PORTS_2_HRP_PORT_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_1_HRP_PORTS_2_HRP_PORT_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_mult_groups[1].hrp_ports[2].hrp_port_id  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_mlt_grps_1_hrp_prts_2_hrp_prt_id after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_1_HRP_PORTS_1_NOF_COPIES_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_1_HRP_PORTS_1_NOF_COPIES_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_mult_groups[1].hrp_ports[1].nof_copies  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_mlt_grps_1_hrp_prts_1_nof_cps after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_1_HRP_PORTS_1_HRP_PORT_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_1_HRP_PORTS_1_HRP_PORT_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_mult_groups[1].hrp_ports[1].hrp_port_id  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_mlt_grps_1_hrp_prts_1_hrp_prt_id after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_1_HRP_PORTS_0_NOF_COPIES_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_1_HRP_PORTS_0_NOF_COPIES_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_mult_groups[1].hrp_ports[0].nof_copies  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_mlt_grps_1_hrp_prts_0_nof_cps after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_1_HRP_PORTS_0_HRP_PORT_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_1_HRP_PORTS_0_HRP_PORT_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_mult_groups[1].hrp_ports[0].hrp_port_id  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_mlt_grps_1_hrp_prts_0_hrp_prt_id after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_0_DOMAIN_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_0_DOMAIN_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_mult_groups[0].domain_id  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_mlt_grps_0_dmn_id after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_0_NOF_PORTS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_0_NOF_PORTS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_mult_groups[0].nof_ports  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_mlt_grps_0_nof_prts after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_0_HRP_PORTS_3_NOF_COPIES_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_0_HRP_PORTS_3_NOF_COPIES_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_mult_groups[0].hrp_ports[3].nof_copies  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_mlt_grps_0_hrp_prts_3_nof_cps after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_0_HRP_PORTS_3_HRP_PORT_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_0_HRP_PORTS_3_HRP_PORT_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_mult_groups[0].hrp_ports[3].hrp_port_id  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_mlt_grps_0_hrp_prts_3_hrp_prt_id after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_0_HRP_PORTS_2_NOF_COPIES_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_0_HRP_PORTS_2_NOF_COPIES_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_mult_groups[0].hrp_ports[2].nof_copies  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_mlt_grps_0_hrp_prts_2_nof_cps after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_0_HRP_PORTS_2_HRP_PORT_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_0_HRP_PORTS_2_HRP_PORT_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_mult_groups[0].hrp_ports[2].hrp_port_id  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_mlt_grps_0_hrp_prts_2_hrp_prt_id after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_0_HRP_PORTS_1_NOF_COPIES_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_0_HRP_PORTS_1_NOF_COPIES_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_mult_groups[0].hrp_ports[1].nof_copies  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_mlt_grps_0_hrp_prts_1_nof_cps after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_0_HRP_PORTS_1_HRP_PORT_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_0_HRP_PORTS_1_HRP_PORT_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_mult_groups[0].hrp_ports[1].hrp_port_id  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_mlt_grps_0_hrp_prts_1_hrp_prt_id after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_0_HRP_PORTS_0_NOF_COPIES_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_0_HRP_PORTS_0_NOF_COPIES_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_mult_groups[0].hrp_ports[0].nof_copies  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_mlt_grps_0_hrp_prts_0_nof_cps after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_0_HRP_PORTS_0_HRP_PORT_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_0_HRP_PORTS_0_HRP_PORT_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_mult_groups[0].hrp_ports[0].hrp_port_id  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_mlt_grps_0_hrp_prts_0_hrp_prt_id after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_IDS_3_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_IDS_3_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_mult_groups_ids[3]  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_mlt_grps_ids_3 after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_IDS_2_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_IDS_2_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_mult_groups_ids[2]  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_mlt_grps_ids_2 after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_IDS_1_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_IDS_1_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_mult_groups_ids[1]  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_mlt_grps_ids_1 after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_IDS_0_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_HRP_MULT_GROUPS_IDS_0_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.hrp_mult_groups_ids[0]  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_hrp_mlt_grps_ids_0 after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FORTS_INFO_3_MULTICAST_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FORTS_INFO_3_MULTICAST_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.mult_global_info.mult_forts_info[3].Multicast_weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_mlt_glbl_nfo_mlt_frts_nfo_3_Mltcst_wght after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FORTS_INFO_3_UNICAST_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FORTS_INFO_3_UNICAST_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.mult_global_info.mult_forts_info[3].Unicast_weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_mlt_glbl_nfo_mlt_frts_nfo_3_Uncst_wght after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FORTS_INFO_2_MULTICAST_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FORTS_INFO_2_MULTICAST_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.mult_global_info.mult_forts_info[2].Multicast_weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_mlt_glbl_nfo_mlt_frts_nfo_2_Mltcst_wght after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FORTS_INFO_2_UNICAST_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FORTS_INFO_2_UNICAST_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.mult_global_info.mult_forts_info[2].Unicast_weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_mlt_glbl_nfo_mlt_frts_nfo_2_Uncst_wght after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FORTS_INFO_1_MULTICAST_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FORTS_INFO_1_MULTICAST_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.mult_global_info.mult_forts_info[1].Multicast_weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_mlt_glbl_nfo_mlt_frts_nfo_1_Mltcst_wght after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FORTS_INFO_1_UNICAST_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FORTS_INFO_1_UNICAST_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.mult_global_info.mult_forts_info[1].Unicast_weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_mlt_glbl_nfo_mlt_frts_nfo_1_Uncst_wght after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FORTS_INFO_0_MULTICAST_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FORTS_INFO_0_MULTICAST_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.mult_global_info.mult_forts_info[0].Multicast_weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_mlt_glbl_nfo_mlt_frts_nfo_0_Mltcst_wght after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FORTS_INFO_0_UNICAST_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FORTS_INFO_0_UNICAST_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.mult_global_info.mult_forts_info[0].Unicast_weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_mlt_glbl_nfo_mlt_frts_nfo_0_Uncst_wght after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_1_RECYCLE_MAX_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_1_RECYCLE_MAX_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.mult_global_info.mult_faps_info[1].Recycle_max_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_mlt_glbl_nfo_mlt_fps_nfo_1_Rcycl_max_rte after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_3_MIN_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_3_MIN_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.mult_global_info.mult_faps_info[1].fap_mult_sch_scm.class_info[3].min_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_mlt_glbl_nfo_mlt_fps_nfo_1_fap_mlt_sch_scm_clss_nfo_3_min_rte after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_3_MAX_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_3_MAX_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.mult_global_info.mult_faps_info[1].fap_mult_sch_scm.class_info[3].max_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_mlt_glbl_nfo_mlt_fps_nfo_1_fap_mlt_sch_scm_clss_nfo_3_max_rte after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_3_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_3_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.mult_global_info.mult_faps_info[1].fap_mult_sch_scm.class_info[3].weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_mlt_glbl_nfo_mlt_fps_nfo_1_fap_mlt_sch_scm_clss_nfo_3_wght after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_3_STRICT_PRIORITY_LEVEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_3_STRICT_PRIORITY_LEVEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.mult_global_info.mult_faps_info[1].fap_mult_sch_scm.class_info[3].strict_priority_level  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_mlt_glbl_nfo_mlt_fps_nfo_1_fap_mlt_sch_scm_clss_nfo_3_strct_prrty_lvl after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_2_MIN_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_2_MIN_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.mult_global_info.mult_faps_info[1].fap_mult_sch_scm.class_info[2].min_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_mlt_glbl_nfo_mlt_fps_nfo_1_fap_mlt_sch_scm_clss_nfo_2_min_rte after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_2_MAX_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_2_MAX_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.mult_global_info.mult_faps_info[1].fap_mult_sch_scm.class_info[2].max_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_mlt_glbl_nfo_mlt_fps_nfo_1_fap_mlt_sch_scm_clss_nfo_2_max_rte after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_2_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_2_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.mult_global_info.mult_faps_info[1].fap_mult_sch_scm.class_info[2].weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_mlt_glbl_nfo_mlt_fps_nfo_1_fap_mlt_sch_scm_clss_nfo_2_wght after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_2_STRICT_PRIORITY_LEVEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_2_STRICT_PRIORITY_LEVEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.mult_global_info.mult_faps_info[1].fap_mult_sch_scm.class_info[2].strict_priority_level  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_mlt_glbl_nfo_mlt_fps_nfo_1_fap_mlt_sch_scm_clss_nfo_2_strct_prrty_lvl after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_1_MIN_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_1_MIN_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.mult_global_info.mult_faps_info[1].fap_mult_sch_scm.class_info[1].min_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_mlt_glbl_nfo_mlt_fps_nfo_1_fap_mlt_sch_scm_clss_nfo_1_min_rte after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_1_MAX_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_1_MAX_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.mult_global_info.mult_faps_info[1].fap_mult_sch_scm.class_info[1].max_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_mlt_glbl_nfo_mlt_fps_nfo_1_fap_mlt_sch_scm_clss_nfo_1_max_rte after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_1_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_1_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.mult_global_info.mult_faps_info[1].fap_mult_sch_scm.class_info[1].weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_mlt_glbl_nfo_mlt_fps_nfo_1_fap_mlt_sch_scm_clss_nfo_1_wght after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_1_STRICT_PRIORITY_LEVEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_1_STRICT_PRIORITY_LEVEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.mult_global_info.mult_faps_info[1].fap_mult_sch_scm.class_info[1].strict_priority_level  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_mlt_glbl_nfo_mlt_fps_nfo_1_fap_mlt_sch_scm_clss_nfo_1_strct_prrty_lvl after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_0_MIN_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_0_MIN_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.mult_global_info.mult_faps_info[1].fap_mult_sch_scm.class_info[0].min_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_mlt_glbl_nfo_mlt_fps_nfo_1_fap_mlt_sch_scm_clss_nfo_0_min_rte after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_0_MAX_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_0_MAX_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.mult_global_info.mult_faps_info[1].fap_mult_sch_scm.class_info[0].max_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_mlt_glbl_nfo_mlt_fps_nfo_1_fap_mlt_sch_scm_clss_nfo_0_max_rte after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_0_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_0_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.mult_global_info.mult_faps_info[1].fap_mult_sch_scm.class_info[0].weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_mlt_glbl_nfo_mlt_fps_nfo_1_fap_mlt_sch_scm_clss_nfo_0_wght after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_0_STRICT_PRIORITY_LEVEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_0_STRICT_PRIORITY_LEVEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.mult_global_info.mult_faps_info[1].fap_mult_sch_scm.class_info[0].strict_priority_level  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_mlt_glbl_nfo_mlt_fps_nfo_1_fap_mlt_sch_scm_clss_nfo_0_strct_prrty_lvl after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_0_RECYCLE_MAX_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_0_RECYCLE_MAX_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.mult_global_info.mult_faps_info[0].Recycle_max_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_mlt_glbl_nfo_mlt_fps_nfo_0_Rcycl_max_rte after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_3_MIN_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_3_MIN_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.mult_global_info.mult_faps_info[0].fap_mult_sch_scm.class_info[3].min_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_mlt_glbl_nfo_mlt_fps_nfo_0_fap_mlt_sch_scm_clss_nfo_3_min_rte after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_3_MAX_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_3_MAX_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.mult_global_info.mult_faps_info[0].fap_mult_sch_scm.class_info[3].max_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_mlt_glbl_nfo_mlt_fps_nfo_0_fap_mlt_sch_scm_clss_nfo_3_max_rte after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_3_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_3_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.mult_global_info.mult_faps_info[0].fap_mult_sch_scm.class_info[3].weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_mlt_glbl_nfo_mlt_fps_nfo_0_fap_mlt_sch_scm_clss_nfo_3_wght after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_3_STRICT_PRIORITY_LEVEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_3_STRICT_PRIORITY_LEVEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.mult_global_info.mult_faps_info[0].fap_mult_sch_scm.class_info[3].strict_priority_level  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_mlt_glbl_nfo_mlt_fps_nfo_0_fap_mlt_sch_scm_clss_nfo_3_strct_prrty_lvl after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_2_MIN_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_2_MIN_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.mult_global_info.mult_faps_info[0].fap_mult_sch_scm.class_info[2].min_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_mlt_glbl_nfo_mlt_fps_nfo_0_fap_mlt_sch_scm_clss_nfo_2_min_rte after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_2_MAX_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_2_MAX_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.mult_global_info.mult_faps_info[0].fap_mult_sch_scm.class_info[2].max_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_mlt_glbl_nfo_mlt_fps_nfo_0_fap_mlt_sch_scm_clss_nfo_2_max_rte after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_2_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_2_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.mult_global_info.mult_faps_info[0].fap_mult_sch_scm.class_info[2].weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_mlt_glbl_nfo_mlt_fps_nfo_0_fap_mlt_sch_scm_clss_nfo_2_wght after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_2_STRICT_PRIORITY_LEVEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_2_STRICT_PRIORITY_LEVEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.mult_global_info.mult_faps_info[0].fap_mult_sch_scm.class_info[2].strict_priority_level  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_mlt_glbl_nfo_mlt_fps_nfo_0_fap_mlt_sch_scm_clss_nfo_2_strct_prrty_lvl after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_1_MIN_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_1_MIN_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.mult_global_info.mult_faps_info[0].fap_mult_sch_scm.class_info[1].min_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_mlt_glbl_nfo_mlt_fps_nfo_0_fap_mlt_sch_scm_clss_nfo_1_min_rte after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_1_MAX_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_1_MAX_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.mult_global_info.mult_faps_info[0].fap_mult_sch_scm.class_info[1].max_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_mlt_glbl_nfo_mlt_fps_nfo_0_fap_mlt_sch_scm_clss_nfo_1_max_rte after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_1_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_1_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.mult_global_info.mult_faps_info[0].fap_mult_sch_scm.class_info[1].weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_mlt_glbl_nfo_mlt_fps_nfo_0_fap_mlt_sch_scm_clss_nfo_1_wght after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_1_STRICT_PRIORITY_LEVEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_1_STRICT_PRIORITY_LEVEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.mult_global_info.mult_faps_info[0].fap_mult_sch_scm.class_info[1].strict_priority_level  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_mlt_glbl_nfo_mlt_fps_nfo_0_fap_mlt_sch_scm_clss_nfo_1_strct_prrty_lvl after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_0_MIN_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_0_MIN_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.mult_global_info.mult_faps_info[0].fap_mult_sch_scm.class_info[0].min_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_mlt_glbl_nfo_mlt_fps_nfo_0_fap_mlt_sch_scm_clss_nfo_0_min_rte after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_0_MAX_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_0_MAX_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.mult_global_info.mult_faps_info[0].fap_mult_sch_scm.class_info[0].max_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_mlt_glbl_nfo_mlt_fps_nfo_0_fap_mlt_sch_scm_clss_nfo_0_max_rte after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_0_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_0_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.mult_global_info.mult_faps_info[0].fap_mult_sch_scm.class_info[0].weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_mlt_glbl_nfo_mlt_fps_nfo_0_fap_mlt_sch_scm_clss_nfo_0_wght after init_phase_2***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_0_STRICT_PRIORITY_LEVEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_START_UP_INFORMATION_MULT_GLOBAL_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_0_STRICT_PRIORITY_LEVEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_start_up_information.mult_global_info.mult_faps_info[0].fap_mult_sch_scm.class_info[0].strict_priority_level  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter strt_up_nfrmtn_mlt_glbl_nfo_mlt_fps_nfo_0_fap_mlt_sch_scm_clss_nfo_0_strct_prrty_lvl after init_phase_2***", TRUE);
  }


  /* Call function */
  ret = dhrp_init_phase_2(
          &prm_start_up_information
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** dhrp_init_phase_2 - FAIL", TRUE);
    goto exit;
  }

  send_string_to_screen(" *** dhrp_init_phase_2 - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: fill_default_init_info (section management)
 */
int
  ui_dhrp_api_management_fill_default_init_info(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  DHRP_INIT_INFO
    prm_start_up_information;

  UI_MACROS_INIT_FUNCTION("ui_dhrp_api_management");
  soc_sand_proc_name = "dhrp_fill_default_init_info";

  unit = dhrp_get_default_unit();
  dhrp_DHRP_INIT_INFO_clear(&prm_start_up_information);

  /* Get parameters */

  /* Call function */
  ret = dhrp_fill_default_init_info(
          &prm_start_up_information
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** dhrp_fill_default_init_info - FAIL", TRUE);
    goto exit;
  }

  send_string_to_screen(" *** dhrp_fill_default_init_info - SUCCEEDED", TRUE);
  send_string_to_screen("--> start_up_information:", TRUE);
  dhrp_DHRP_INIT_INFO_print(&prm_start_up_information);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: preinit_advanced_settings (section management)
 */
int
  ui_dhrp_api_management_preinit_advanced_settings(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  DHRP_PREINIT_ADVANCED_INFO
    prm_internal_info;

  UI_MACROS_INIT_FUNCTION("ui_dhrp_api_management");
  soc_sand_proc_name = "dhrp_preinit_advanced_settings";

  unit = dhrp_get_default_unit();
  dhrp_DHRP_PREINIT_ADVANCED_INFO_clear(&prm_internal_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_PREINIT_ADVANCED_SETTINGS_INTERNAL_INFO_DOWNLOAD_SWEEP_TEXT_FILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_PREINIT_ADVANCED_SETTINGS_INTERNAL_INFO_DOWNLOAD_SWEEP_TEXT_FILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_internal_info.download_sweep_text_file  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ntrnl_nfo_dwnld_swp_txt_fil after preinit_advanced_settings***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_PREINIT_ADVANCED_SETTINGS_INTERNAL_INFO_DISABLE_MULTICAST_APP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MANAGEMENT_PREINIT_ADVANCED_SETTINGS_INTERNAL_INFO_DISABLE_MULTICAST_APP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_internal_info.disable_multicast_app  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ntrnl_nfo_dsbl_mltcst_app after preinit_advanced_settings***", TRUE);
  }


  /* Call function */
  ret = dhrp_preinit_advanced_settings(
          &prm_internal_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** dhrp_preinit_advanced_settings - FAIL", TRUE);
    goto exit;
  }

  send_string_to_screen(" *** dhrp_preinit_advanced_settings - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: close (section management)
 */
int
  ui_dhrp_api_management_close(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;

  UI_MACROS_INIT_FUNCTION("ui_dhrp_api_management");
  soc_sand_proc_name = "dhrp_close";

  unit = dhrp_get_default_unit();

  /* Get parameters */

  /* Call function */
  ret = dhrp_close(
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** dhrp_close - FAIL", TRUE);
    goto exit;
  }

  send_string_to_screen(" *** dhrp_close - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: sch_scm_clb_update_port (section scheduling_scheme)
 */
int
  ui_dhrp_api_scheduling_scheme_sch_scm_clb_update_port(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  DHRP_PORT_ID
    prm_hrp_port_id;
  DHRP_SCH_SCM_CLB
    prm_scheme_info;

  UI_MACROS_INIT_FUNCTION("ui_dhrp_api_scheduling_scheme");
  soc_sand_proc_name = "dhrp_sch_scm_clb_update_port";

  unit = dhrp_get_default_unit();
  dhrp_DHRP_SCH_SCM_CLB_clear(&prm_scheme_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_UPDATE_PORT_HRP_PORT_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_UPDATE_PORT_HRP_PORT_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_hrp_port_id  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter hrp_prt_id after sch_scm_clb_update_port***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_UPDATE_PORT_SCHEME_INFO_CLASS_INFO_3_MIN_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_UPDATE_PORT_SCHEME_INFO_CLASS_INFO_3_MIN_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_scheme_info.class_info[3].min_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter schm_nfo_clss_nfo_3_min_rte after sch_scm_clb_update_port***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_UPDATE_PORT_SCHEME_INFO_CLASS_INFO_3_MAX_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_UPDATE_PORT_SCHEME_INFO_CLASS_INFO_3_MAX_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_scheme_info.class_info[3].max_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter schm_nfo_clss_nfo_3_max_rte after sch_scm_clb_update_port***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_UPDATE_PORT_SCHEME_INFO_CLASS_INFO_3_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_UPDATE_PORT_SCHEME_INFO_CLASS_INFO_3_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_scheme_info.class_info[3].weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter schm_nfo_clss_nfo_3_wght after sch_scm_clb_update_port***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_UPDATE_PORT_SCHEME_INFO_CLASS_INFO_3_STRICT_PRIORITY_LEVEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_UPDATE_PORT_SCHEME_INFO_CLASS_INFO_3_STRICT_PRIORITY_LEVEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_scheme_info.class_info[3].strict_priority_level  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter schm_nfo_clss_nfo_3_strct_prrty_lvl after sch_scm_clb_update_port***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_UPDATE_PORT_SCHEME_INFO_CLASS_INFO_2_MIN_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_UPDATE_PORT_SCHEME_INFO_CLASS_INFO_2_MIN_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_scheme_info.class_info[2].min_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter schm_nfo_clss_nfo_2_min_rte after sch_scm_clb_update_port***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_UPDATE_PORT_SCHEME_INFO_CLASS_INFO_2_MAX_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_UPDATE_PORT_SCHEME_INFO_CLASS_INFO_2_MAX_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_scheme_info.class_info[2].max_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter schm_nfo_clss_nfo_2_max_rte after sch_scm_clb_update_port***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_UPDATE_PORT_SCHEME_INFO_CLASS_INFO_2_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_UPDATE_PORT_SCHEME_INFO_CLASS_INFO_2_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_scheme_info.class_info[2].weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter schm_nfo_clss_nfo_2_wght after sch_scm_clb_update_port***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_UPDATE_PORT_SCHEME_INFO_CLASS_INFO_2_STRICT_PRIORITY_LEVEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_UPDATE_PORT_SCHEME_INFO_CLASS_INFO_2_STRICT_PRIORITY_LEVEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_scheme_info.class_info[2].strict_priority_level  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter schm_nfo_clss_nfo_2_strct_prrty_lvl after sch_scm_clb_update_port***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_UPDATE_PORT_SCHEME_INFO_CLASS_INFO_1_MIN_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_UPDATE_PORT_SCHEME_INFO_CLASS_INFO_1_MIN_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_scheme_info.class_info[1].min_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter schm_nfo_clss_nfo_1_min_rte after sch_scm_clb_update_port***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_UPDATE_PORT_SCHEME_INFO_CLASS_INFO_1_MAX_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_UPDATE_PORT_SCHEME_INFO_CLASS_INFO_1_MAX_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_scheme_info.class_info[1].max_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter schm_nfo_clss_nfo_1_max_rte after sch_scm_clb_update_port***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_UPDATE_PORT_SCHEME_INFO_CLASS_INFO_1_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_UPDATE_PORT_SCHEME_INFO_CLASS_INFO_1_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_scheme_info.class_info[1].weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter schm_nfo_clss_nfo_1_wght after sch_scm_clb_update_port***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_UPDATE_PORT_SCHEME_INFO_CLASS_INFO_1_STRICT_PRIORITY_LEVEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_UPDATE_PORT_SCHEME_INFO_CLASS_INFO_1_STRICT_PRIORITY_LEVEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_scheme_info.class_info[1].strict_priority_level  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter schm_nfo_clss_nfo_1_strct_prrty_lvl after sch_scm_clb_update_port***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_UPDATE_PORT_SCHEME_INFO_CLASS_INFO_0_MIN_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_UPDATE_PORT_SCHEME_INFO_CLASS_INFO_0_MIN_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_scheme_info.class_info[0].min_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter schm_nfo_clss_nfo_0_min_rte after sch_scm_clb_update_port***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_UPDATE_PORT_SCHEME_INFO_CLASS_INFO_0_MAX_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_UPDATE_PORT_SCHEME_INFO_CLASS_INFO_0_MAX_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_scheme_info.class_info[0].max_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter schm_nfo_clss_nfo_0_max_rte after sch_scm_clb_update_port***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_UPDATE_PORT_SCHEME_INFO_CLASS_INFO_0_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_UPDATE_PORT_SCHEME_INFO_CLASS_INFO_0_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_scheme_info.class_info[0].weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter schm_nfo_clss_nfo_0_wght after sch_scm_clb_update_port***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_UPDATE_PORT_SCHEME_INFO_CLASS_INFO_0_STRICT_PRIORITY_LEVEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_UPDATE_PORT_SCHEME_INFO_CLASS_INFO_0_STRICT_PRIORITY_LEVEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_scheme_info.class_info[0].strict_priority_level  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter schm_nfo_clss_nfo_0_strct_prrty_lvl after sch_scm_clb_update_port***", TRUE);
  }


  /* Call function */
  ret = dhrp_sch_scm_clb_update_port(
          prm_hrp_port_id,
          &prm_scheme_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** dhrp_sch_scm_clb_update_port - FAIL", TRUE);
    goto exit;
  }

  send_string_to_screen(" *** dhrp_sch_scm_clb_update_port - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: sch_scm_clb_fill_template_scheme (section scheduling_scheme)
 */
int
  ui_dhrp_api_scheduling_scheme_sch_scm_clb_fill_template_scheme(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  DHRP_SCH_SCM_CLB_TMPLT_ID
    prm_scheme_id;
  DHRP_SCH_SCM_CLB
    prm_scheme_info;
  DHRP_PORT_ID
    port_i;

  UI_MACROS_INIT_FUNCTION("ui_dhrp_api_scheduling_scheme");
  soc_sand_proc_name = "dhrp_sch_scm_clb_fill_template_scheme";

  unit = dhrp_get_default_unit();
  dhrp_DHRP_SCH_SCM_CLB_clear(&prm_scheme_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_FILL_TEMPLATE_SCHEME_SCHEME_ID_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_FILL_TEMPLATE_SCHEME_SCHEME_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_scheme_id = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter schm_id after sch_scm_clb_fill_template_scheme***", TRUE);
  }


  /* Call function */
  ret = dhrp_sch_scm_clb_fill_template_scheme(
          prm_scheme_id,
          &prm_scheme_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** dhrp_sch_scm_clb_fill_template_scheme - FAIL", TRUE);
    goto exit;
  }

  /* NON-Autocoder CODE ! */

  for(port_i=1;port_i<=DHRP_NOF_PORTS;port_i++)
  {
    ret = dhrp_sch_scm_clb_update_port(
            port_i,
            &prm_scheme_info
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** dhrp_sch_scm_clb_update_port - FAIL", TRUE);
      goto exit;
    }
  }

  send_string_to_screen(" *** dhrp_sch_scm_clb_fill_template_scheme - SUCCEEDED", TRUE);
  send_string_to_screen("--> scheme_info:", TRUE);
  dhrp_DHRP_SCH_SCM_CLB_print(&prm_scheme_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: sch_scm_clb_update_class_attribute (section scheduling_scheme)
 */
int
  ui_dhrp_api_scheduling_scheme_sch_scm_clb_update_class_attribute(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  DHRP_PORT_ID
    prm_hrp_port_id;
  uint32
    prm_class_id;
  DHRP_SCH_SCM_CLB_ATTR
    prm_attribute_type;
  uint32
    prm_attribute_value;

  UI_MACROS_INIT_FUNCTION("ui_dhrp_api_scheduling_scheme");
  soc_sand_proc_name = "dhrp_sch_scm_clb_update_class_attribute";

  unit = dhrp_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_UPDATE_CLASS_ATTRIBUTE_HRP_PORT_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_UPDATE_CLASS_ATTRIBUTE_HRP_PORT_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_hrp_port_id  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter hrp_prt_id after sch_scm_clb_update_class_attribute***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_UPDATE_CLASS_ATTRIBUTE_CLASS_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_UPDATE_CLASS_ATTRIBUTE_CLASS_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_class_id  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter clss_id after sch_scm_clb_update_class_attribute***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_UPDATE_CLASS_ATTRIBUTE_ATTRIBUTE_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_UPDATE_CLASS_ATTRIBUTE_ATTRIBUTE_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_attribute_type = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ttrbt_typ after sch_scm_clb_update_class_attribute***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_UPDATE_CLASS_ATTRIBUTE_ATTRIBUTE_VALUE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_UPDATE_CLASS_ATTRIBUTE_ATTRIBUTE_VALUE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_attribute_value  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ttrbt_vlu after sch_scm_clb_update_class_attribute***", TRUE);
  }


  /* Call function */
  ret = dhrp_sch_scm_clb_update_class_attribute(
          prm_hrp_port_id,
          prm_class_id,
          prm_attribute_type,
          prm_attribute_value
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** dhrp_sch_scm_clb_update_class_attribute - FAIL", TRUE);
    goto exit;
  }

  send_string_to_screen(" *** dhrp_sch_scm_clb_update_class_attribute - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: sch_scm_clb_get_port (section scheduling_scheme)
 */
int
  ui_dhrp_api_scheduling_scheme_sch_scm_clb_get_port(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  DHRP_PORT_ID
    prm_hrp_port_id;
  DHRP_SCH_SCM_CLB
    prm_scheme_info;

  UI_MACROS_INIT_FUNCTION("ui_dhrp_api_scheduling_scheme");
  soc_sand_proc_name = "dhrp_sch_scm_clb_get_port";

  unit = dhrp_get_default_unit();
  dhrp_DHRP_SCH_SCM_CLB_clear(&prm_scheme_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_GET_PORT_HRP_PORT_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_GET_PORT_HRP_PORT_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_hrp_port_id  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter hrp_prt_id after sch_scm_clb_get_port***", TRUE);
  }


  /* Call function */
  ret = dhrp_sch_scm_clb_get_port(
          prm_hrp_port_id,
          &prm_scheme_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** dhrp_sch_scm_clb_get_port - FAIL", TRUE);
    goto exit;
  }

  send_string_to_screen(" *** dhrp_sch_scm_clb_get_port - SUCCEEDED", TRUE);
  send_string_to_screen("--> scheme_info:", TRUE);
  dhrp_DHRP_SCH_SCM_CLB_print(&prm_scheme_info);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: sch_scm_clb_print_port_scheme (section scheduling_scheme)
 */
int
  ui_dhrp_api_scheduling_scheme_sch_scm_clb_print_port_scheme(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  DHRP_PORT_ID
    prm_hrp_port_id;

  UI_MACROS_INIT_FUNCTION("ui_dhrp_api_scheduling_scheme");
  soc_sand_proc_name = "dhrp_sch_scm_clb_print_port_scheme";

  unit = dhrp_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_PRINT_PORT_SCHEME_HRP_PORT_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_PRINT_PORT_SCHEME_HRP_PORT_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_hrp_port_id  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter hrp_prt_id after sch_scm_clb_print_port_scheme***", TRUE);
  }


  /* Call function */
  ret = dhrp_sch_scm_clb_print_port_scheme(
          prm_hrp_port_id
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** dhrp_sch_scm_clb_print_port_scheme - FAIL", TRUE);
    goto exit;
  }

  send_string_to_screen(" *** dhrp_sch_scm_clb_print_port_scheme - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: set_global_info (section multicast)
 */
int
  ui_dhrp_api_multicast_set_global_info(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  DHRP_MULT_GLBL_INFO
    prm_multicast_info;

  UI_MACROS_INIT_FUNCTION("ui_dhrp_api_multicast");
  soc_sand_proc_name = "dhrp_multicast_set_global_info";

  unit = dhrp_get_default_unit();
  dhrp_DHRP_MULT_GLBL_INFO_clear(&prm_multicast_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FORTS_INFO_3_MULTICAST_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FORTS_INFO_3_MULTICAST_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_info.mult_forts_info[3].Multicast_weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_nfo_mlt_frts_nfo_3_Mltcst_wght after set_global_info***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FORTS_INFO_3_UNICAST_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FORTS_INFO_3_UNICAST_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_info.mult_forts_info[3].Unicast_weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_nfo_mlt_frts_nfo_3_Uncst_wght after set_global_info***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FORTS_INFO_2_MULTICAST_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FORTS_INFO_2_MULTICAST_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_info.mult_forts_info[2].Multicast_weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_nfo_mlt_frts_nfo_2_Mltcst_wght after set_global_info***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FORTS_INFO_2_UNICAST_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FORTS_INFO_2_UNICAST_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_info.mult_forts_info[2].Unicast_weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_nfo_mlt_frts_nfo_2_Uncst_wght after set_global_info***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FORTS_INFO_1_MULTICAST_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FORTS_INFO_1_MULTICAST_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_info.mult_forts_info[1].Multicast_weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_nfo_mlt_frts_nfo_1_Mltcst_wght after set_global_info***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FORTS_INFO_1_UNICAST_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FORTS_INFO_1_UNICAST_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_info.mult_forts_info[1].Unicast_weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_nfo_mlt_frts_nfo_1_Uncst_wght after set_global_info***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FORTS_INFO_0_MULTICAST_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FORTS_INFO_0_MULTICAST_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_info.mult_forts_info[0].Multicast_weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_nfo_mlt_frts_nfo_0_Mltcst_wght after set_global_info***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FORTS_INFO_0_UNICAST_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FORTS_INFO_0_UNICAST_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_info.mult_forts_info[0].Unicast_weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_nfo_mlt_frts_nfo_0_Uncst_wght after set_global_info***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_1_RECYCLE_MAX_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_1_RECYCLE_MAX_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_info.mult_faps_info[1].Recycle_max_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_nfo_mlt_fps_nfo_1_Rcycl_max_rte after set_global_info***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_3_MIN_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_3_MIN_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_info.mult_faps_info[1].fap_mult_sch_scm.class_info[3].min_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_nfo_mlt_fps_nfo_1_fap_mlt_sch_scm_clss_nfo_3_min_rte after set_global_info***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_3_MAX_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_3_MAX_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_info.mult_faps_info[1].fap_mult_sch_scm.class_info[3].max_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_nfo_mlt_fps_nfo_1_fap_mlt_sch_scm_clss_nfo_3_max_rte after set_global_info***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_3_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_3_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_info.mult_faps_info[1].fap_mult_sch_scm.class_info[3].weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_nfo_mlt_fps_nfo_1_fap_mlt_sch_scm_clss_nfo_3_wght after set_global_info***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_3_STRICT_PRIORITY_LEVEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_3_STRICT_PRIORITY_LEVEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_info.mult_faps_info[1].fap_mult_sch_scm.class_info[3].strict_priority_level  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_nfo_mlt_fps_nfo_1_fap_mlt_sch_scm_clss_nfo_3_strct_prrty_lvl after set_global_info***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_2_MIN_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_2_MIN_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_info.mult_faps_info[1].fap_mult_sch_scm.class_info[2].min_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_nfo_mlt_fps_nfo_1_fap_mlt_sch_scm_clss_nfo_2_min_rte after set_global_info***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_2_MAX_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_2_MAX_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_info.mult_faps_info[1].fap_mult_sch_scm.class_info[2].max_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_nfo_mlt_fps_nfo_1_fap_mlt_sch_scm_clss_nfo_2_max_rte after set_global_info***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_2_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_2_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_info.mult_faps_info[1].fap_mult_sch_scm.class_info[2].weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_nfo_mlt_fps_nfo_1_fap_mlt_sch_scm_clss_nfo_2_wght after set_global_info***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_2_STRICT_PRIORITY_LEVEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_2_STRICT_PRIORITY_LEVEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_info.mult_faps_info[1].fap_mult_sch_scm.class_info[2].strict_priority_level  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_nfo_mlt_fps_nfo_1_fap_mlt_sch_scm_clss_nfo_2_strct_prrty_lvl after set_global_info***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_1_MIN_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_1_MIN_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_info.mult_faps_info[1].fap_mult_sch_scm.class_info[1].min_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_nfo_mlt_fps_nfo_1_fap_mlt_sch_scm_clss_nfo_1_min_rte after set_global_info***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_1_MAX_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_1_MAX_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_info.mult_faps_info[1].fap_mult_sch_scm.class_info[1].max_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_nfo_mlt_fps_nfo_1_fap_mlt_sch_scm_clss_nfo_1_max_rte after set_global_info***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_1_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_1_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_info.mult_faps_info[1].fap_mult_sch_scm.class_info[1].weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_nfo_mlt_fps_nfo_1_fap_mlt_sch_scm_clss_nfo_1_wght after set_global_info***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_1_STRICT_PRIORITY_LEVEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_1_STRICT_PRIORITY_LEVEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_info.mult_faps_info[1].fap_mult_sch_scm.class_info[1].strict_priority_level  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_nfo_mlt_fps_nfo_1_fap_mlt_sch_scm_clss_nfo_1_strct_prrty_lvl after set_global_info***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_0_MIN_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_0_MIN_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_info.mult_faps_info[1].fap_mult_sch_scm.class_info[0].min_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_nfo_mlt_fps_nfo_1_fap_mlt_sch_scm_clss_nfo_0_min_rte after set_global_info***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_0_MAX_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_0_MAX_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_info.mult_faps_info[1].fap_mult_sch_scm.class_info[0].max_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_nfo_mlt_fps_nfo_1_fap_mlt_sch_scm_clss_nfo_0_max_rte after set_global_info***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_0_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_0_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_info.mult_faps_info[1].fap_mult_sch_scm.class_info[0].weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_nfo_mlt_fps_nfo_1_fap_mlt_sch_scm_clss_nfo_0_wght after set_global_info***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_0_STRICT_PRIORITY_LEVEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_1_FAP_MULT_SCH_SCM_CLASS_INFO_0_STRICT_PRIORITY_LEVEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_info.mult_faps_info[1].fap_mult_sch_scm.class_info[0].strict_priority_level  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_nfo_mlt_fps_nfo_1_fap_mlt_sch_scm_clss_nfo_0_strct_prrty_lvl after set_global_info***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_0_RECYCLE_MAX_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_0_RECYCLE_MAX_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_info.mult_faps_info[0].Recycle_max_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_nfo_mlt_fps_nfo_0_Rcycl_max_rte after set_global_info***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_3_MIN_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_3_MIN_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_info.mult_faps_info[0].fap_mult_sch_scm.class_info[3].min_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_nfo_mlt_fps_nfo_0_fap_mlt_sch_scm_clss_nfo_3_min_rte after set_global_info***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_3_MAX_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_3_MAX_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_info.mult_faps_info[0].fap_mult_sch_scm.class_info[3].max_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_nfo_mlt_fps_nfo_0_fap_mlt_sch_scm_clss_nfo_3_max_rte after set_global_info***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_3_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_3_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_info.mult_faps_info[0].fap_mult_sch_scm.class_info[3].weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_nfo_mlt_fps_nfo_0_fap_mlt_sch_scm_clss_nfo_3_wght after set_global_info***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_3_STRICT_PRIORITY_LEVEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_3_STRICT_PRIORITY_LEVEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_info.mult_faps_info[0].fap_mult_sch_scm.class_info[3].strict_priority_level  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_nfo_mlt_fps_nfo_0_fap_mlt_sch_scm_clss_nfo_3_strct_prrty_lvl after set_global_info***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_2_MIN_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_2_MIN_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_info.mult_faps_info[0].fap_mult_sch_scm.class_info[2].min_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_nfo_mlt_fps_nfo_0_fap_mlt_sch_scm_clss_nfo_2_min_rte after set_global_info***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_2_MAX_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_2_MAX_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_info.mult_faps_info[0].fap_mult_sch_scm.class_info[2].max_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_nfo_mlt_fps_nfo_0_fap_mlt_sch_scm_clss_nfo_2_max_rte after set_global_info***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_2_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_2_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_info.mult_faps_info[0].fap_mult_sch_scm.class_info[2].weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_nfo_mlt_fps_nfo_0_fap_mlt_sch_scm_clss_nfo_2_wght after set_global_info***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_2_STRICT_PRIORITY_LEVEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_2_STRICT_PRIORITY_LEVEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_info.mult_faps_info[0].fap_mult_sch_scm.class_info[2].strict_priority_level  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_nfo_mlt_fps_nfo_0_fap_mlt_sch_scm_clss_nfo_2_strct_prrty_lvl after set_global_info***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_1_MIN_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_1_MIN_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_info.mult_faps_info[0].fap_mult_sch_scm.class_info[1].min_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_nfo_mlt_fps_nfo_0_fap_mlt_sch_scm_clss_nfo_1_min_rte after set_global_info***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_1_MAX_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_1_MAX_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_info.mult_faps_info[0].fap_mult_sch_scm.class_info[1].max_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_nfo_mlt_fps_nfo_0_fap_mlt_sch_scm_clss_nfo_1_max_rte after set_global_info***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_1_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_1_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_info.mult_faps_info[0].fap_mult_sch_scm.class_info[1].weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_nfo_mlt_fps_nfo_0_fap_mlt_sch_scm_clss_nfo_1_wght after set_global_info***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_1_STRICT_PRIORITY_LEVEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_1_STRICT_PRIORITY_LEVEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_info.mult_faps_info[0].fap_mult_sch_scm.class_info[1].strict_priority_level  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_nfo_mlt_fps_nfo_0_fap_mlt_sch_scm_clss_nfo_1_strct_prrty_lvl after set_global_info***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_0_MIN_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_0_MIN_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_info.mult_faps_info[0].fap_mult_sch_scm.class_info[0].min_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_nfo_mlt_fps_nfo_0_fap_mlt_sch_scm_clss_nfo_0_min_rte after set_global_info***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_0_MAX_RATE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_0_MAX_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_info.mult_faps_info[0].fap_mult_sch_scm.class_info[0].max_rate  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_nfo_mlt_fps_nfo_0_fap_mlt_sch_scm_clss_nfo_0_max_rte after set_global_info***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_0_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_0_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_info.mult_faps_info[0].fap_mult_sch_scm.class_info[0].weight  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_nfo_mlt_fps_nfo_0_fap_mlt_sch_scm_clss_nfo_0_wght after set_global_info***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_0_STRICT_PRIORITY_LEVEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_MULTICAST_INFO_MULT_FAPS_INFO_0_FAP_MULT_SCH_SCM_CLASS_INFO_0_STRICT_PRIORITY_LEVEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_info.mult_faps_info[0].fap_mult_sch_scm.class_info[0].strict_priority_level  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_nfo_mlt_fps_nfo_0_fap_mlt_sch_scm_clss_nfo_0_strct_prrty_lvl after set_global_info***", TRUE);
  }


  /* Call function */
  ret = dhrp_multicast_set_global_info(
          &prm_multicast_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** dhrp_multicast_set_global_info - FAIL", TRUE);
    goto exit;
  }

  send_string_to_screen(" *** dhrp_multicast_set_global_info - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: get_global_info (section multicast)
 */
int
  ui_dhrp_api_multicast_get_global_info(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  DHRP_MULT_GLBL_INFO
    prm_multicast_info;

  UI_MACROS_INIT_FUNCTION("ui_dhrp_api_multicast");
  soc_sand_proc_name = "dhrp_multicast_get_global_info";

  unit = dhrp_get_default_unit();
  dhrp_DHRP_MULT_GLBL_INFO_clear(&prm_multicast_info);

  /* Get parameters */

  /* Call function */
  ret = dhrp_multicast_get_global_info(
          &prm_multicast_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** dhrp_multicast_get_global_info - FAIL", TRUE);
    goto exit;
  }

  send_string_to_screen(" *** dhrp_multicast_get_global_info - SUCCEEDED", TRUE);
  send_string_to_screen("--> multicast_info:", TRUE);
  dhrp_DHRP_MULT_GLBL_INFO_print(&prm_multicast_info);


  goto exit;
exit:
  return ui_ret;
}

int
  ui_dhrp_api_multicast_group_open_get_parameters(
    CURRENT_LINE    *current_line,
    uint32       *prm_multicast_group_id,
    DHRP_MULT_GRP   *prm_multicast_group
  )
{
  unsigned int
    param_i;
  PARAM_VAL
    *param_val_general_purpose;

  UI_MACROS_INIT_FUNCTION("ui_dhrp_api_multicast");
  soc_sand_proc_name = "dhrp_multicast_group_open_get_parameters";

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_GROUP_OPEN_MULTICAST_GROUP_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_GROUP_OPEN_MULTICAST_GROUP_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    *prm_multicast_group_id  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter prm_multicast_group_id after group_open***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_GROUP_OPEN_MULTICAST_GROUP_DOMAIN_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_GROUP_OPEN_MULTICAST_GROUP_DOMAIN_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_group->domain_id  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    prm_multicast_group->domain_id  = DHRP_MULT_DFLT_DOMAIN_ID;
  }


  prm_multicast_group->nof_ports = 0;

  for (param_i=1; param_i<=MAX_REPEATED_PARAM_VAL; ++param_i)
  {
    ui_ret = get_val_of( current_line, (int *)&match_index,
      PARAM_DHRP_API_MULTICAST_GROUP_OPEN_COPY_PORT_LIST_ID, param_i,
      &param_val_general_purpose, VAL_NUMERIC, err_msg
      );
    if (0 != ui_ret)
    {
      ui_ret = 0;
      break;
    }
    prm_multicast_group->hrp_ports[prm_multicast_group->nof_ports].hrp_port_id = param_val_general_purpose->value.ulong_value;

    ++param_i;

    ui_ret = get_val_of( current_line, (int *)&match_index,
      PARAM_DHRP_API_MULTICAST_GROUP_OPEN_COPY_PORT_LIST_ID, param_i,
      &param_val_general_purpose, VAL_NUMERIC, err_msg
      );
    if (0 != ui_ret)
    {
      send_string_to_screen("** Error.",TRUE);
      send_string_to_screen("A port was typed in with no number of copies.",TRUE);
      send_string_to_screen(err_msg,TRUE);
      ui_ret = TRUE;
      goto exit;
    }
    prm_multicast_group->hrp_ports[prm_multicast_group->nof_ports].nof_copies = param_val_general_purpose->value.ulong_value;

    prm_multicast_group->nof_ports++;
  }

  if (param_i == 1)
  {
    send_string_to_screen("** Error.",TRUE);
    send_string_to_screen("At list one port-copies couple should appear.",TRUE);
    ui_ret = TRUE;
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: group_open (section multicast)
 */
int
  ui_dhrp_api_multicast_group_open(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_multicast_group_id;
  DHRP_MULT_GRP
    prm_multicast_group;

  UI_MACROS_INIT_FUNCTION("ui_dhrp_api_multicast");
  soc_sand_proc_name = "dhrp_multicast_group_open";

  unit = dhrp_get_default_unit();
  dhrp_DHRP_MULT_GRP_clear(&prm_multicast_group);

  /* Get parameters */
  ui_dhrp_api_multicast_group_open_get_parameters(
    current_line,
    &prm_multicast_group_id,
    &prm_multicast_group
  );

  /*
  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_GROUP_OPEN_MULTICAST_GROUP_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_GROUP_OPEN_MULTICAST_GROUP_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_group_id  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_grp_id after group_open***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_GROUP_OPEN_MULTICAST_GROUP_DOMAIN_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_GROUP_OPEN_MULTICAST_GROUP_DOMAIN_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_group.domain_id  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_grp_dmn_id after group_open***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_GROUP_OPEN_MULTICAST_GROUP_NOF_PORTS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_GROUP_OPEN_MULTICAST_GROUP_NOF_PORTS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_group.nof_ports  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_grp_nof_prts after group_open***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_GROUP_OPEN_MULTICAST_GROUP_HRP_PORTS_3_NOF_COPIES_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_GROUP_OPEN_MULTICAST_GROUP_HRP_PORTS_3_NOF_COPIES_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_group.hrp_ports[3].nof_copies  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_grp_hrp_prts_3_nof_cps after group_open***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_GROUP_OPEN_MULTICAST_GROUP_HRP_PORTS_3_HRP_PORT_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_GROUP_OPEN_MULTICAST_GROUP_HRP_PORTS_3_HRP_PORT_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_group.hrp_ports[3].hrp_port_id  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_grp_hrp_prts_3_hrp_prt_id after group_open***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_GROUP_OPEN_MULTICAST_GROUP_HRP_PORTS_2_NOF_COPIES_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_GROUP_OPEN_MULTICAST_GROUP_HRP_PORTS_2_NOF_COPIES_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_group.hrp_ports[2].nof_copies  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_grp_hrp_prts_2_nof_cps after group_open***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_GROUP_OPEN_MULTICAST_GROUP_HRP_PORTS_2_HRP_PORT_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_GROUP_OPEN_MULTICAST_GROUP_HRP_PORTS_2_HRP_PORT_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_group.hrp_ports[2].hrp_port_id  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_grp_hrp_prts_2_hrp_prt_id after group_open***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_GROUP_OPEN_MULTICAST_GROUP_HRP_PORTS_1_NOF_COPIES_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_GROUP_OPEN_MULTICAST_GROUP_HRP_PORTS_1_NOF_COPIES_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_group.hrp_ports[1].nof_copies  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_grp_hrp_prts_1_nof_cps after group_open***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_GROUP_OPEN_MULTICAST_GROUP_HRP_PORTS_1_HRP_PORT_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_GROUP_OPEN_MULTICAST_GROUP_HRP_PORTS_1_HRP_PORT_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_group.hrp_ports[1].hrp_port_id  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_grp_hrp_prts_1_hrp_prt_id after group_open***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_GROUP_OPEN_MULTICAST_GROUP_HRP_PORTS_0_NOF_COPIES_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_GROUP_OPEN_MULTICAST_GROUP_HRP_PORTS_0_NOF_COPIES_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_group.hrp_ports[0].nof_copies  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_grp_hrp_prts_0_nof_cps after group_open***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_GROUP_OPEN_MULTICAST_GROUP_HRP_PORTS_0_HRP_PORT_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_GROUP_OPEN_MULTICAST_GROUP_HRP_PORTS_0_HRP_PORT_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_group.hrp_ports[0].hrp_port_id  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_grp_hrp_prts_0_hrp_prt_id after group_open***", TRUE);
  }
  */

  /* Call function */
  ret = dhrp_multicast_group_open(
          prm_multicast_group_id,
          &prm_multicast_group
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** dhrp_multicast_group_open - FAIL", TRUE);
    goto exit;
  }

  send_string_to_screen(" *** dhrp_multicast_group_open - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: group_get (section multicast)
 */
int
  ui_dhrp_api_multicast_group_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_multicast_group_id;
  DHRP_MULT_GRP
    prm_multicast_group;

  UI_MACROS_INIT_FUNCTION("ui_dhrp_api_multicast");
  soc_sand_proc_name = "dhrp_multicast_group_get";

  unit = dhrp_get_default_unit();
  dhrp_DHRP_MULT_GRP_clear(&prm_multicast_group);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_GROUP_GET_MULTICAST_GROUP_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_GROUP_GET_MULTICAST_GROUP_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_group_id  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_grp_id after group_get***", TRUE);
  }


  /* Call function */
  ret = dhrp_multicast_group_get(
          prm_multicast_group_id,
          &prm_multicast_group
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** dhrp_multicast_group_get - FAIL", TRUE);
    goto exit;
  }

  send_string_to_screen(" *** dhrp_multicast_group_get - SUCCEEDED", TRUE);
  send_string_to_screen("--> multicast_group:", TRUE);
  dhrp_DHRP_MULT_GRP_print(&prm_multicast_group);


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: group_close (section multicast)
 */
int
  ui_dhrp_api_multicast_group_close(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_multicast_group_id;

  UI_MACROS_INIT_FUNCTION("ui_dhrp_api_multicast");
  soc_sand_proc_name = "dhrp_multicast_group_close";

  unit = dhrp_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_GROUP_CLOSE_MULTICAST_GROUP_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_GROUP_CLOSE_MULTICAST_GROUP_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_group_id  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_grp_id after group_close***", TRUE);
  }


  /* Call function */
  ret = dhrp_multicast_group_close(
          prm_multicast_group_id
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** dhrp_multicast_group_close - FAIL", TRUE);
    goto exit;
  }

  send_string_to_screen(" *** dhrp_multicast_group_close - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: group_update_port (section multicast)
 */
int
  ui_dhrp_api_multicast_group_update_port(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_multicast_group_id;
  DHRP_MULT_GRP_PORT
    prm_multicast_port;

  UI_MACROS_INIT_FUNCTION("ui_dhrp_api_multicast");
  soc_sand_proc_name = "dhrp_multicast_group_update_port";

  unit = dhrp_get_default_unit();
  dhrp_DHRP_MULT_GRP_PORT_clear(&prm_multicast_port);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_GROUP_UPDATE_PORT_MULTICAST_GROUP_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_GROUP_UPDATE_PORT_MULTICAST_GROUP_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_group_id  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_grp_id after group_update_port***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_GROUP_UPDATE_PORT_MULTICAST_PORT_NOF_COPIES_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_GROUP_UPDATE_PORT_MULTICAST_PORT_NOF_COPIES_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_port.nof_copies  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_prt_nof_cps after group_update_port***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_GROUP_UPDATE_PORT_MULTICAST_PORT_HRP_PORT_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_DHRP_API_MULTICAST_GROUP_UPDATE_PORT_MULTICAST_PORT_HRP_PORT_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_multicast_port.hrp_port_id  = (unsigned long)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mltcst_prt_hrp_prt_id after group_update_port***", TRUE);
  }


  /* Call function */
  ret = dhrp_multicast_group_update_port(
          prm_multicast_group_id,
          &prm_multicast_port
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** dhrp_multicast_group_update_port - FAIL", TRUE);
    goto exit;
  }

  send_string_to_screen(" *** dhrp_multicast_group_update_port - SUCCEEDED", TRUE);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: print_diag_info (section diagnostics)
 */
int
  ui_dhrp_api_diagnostics_print_diag_info(
    CURRENT_LINE *current_line
  )
{
  /*
  uint32
    ret;
  */
  DHRP_PRINT_LEVEL
    prm_print_level;
  DHRP_MDLS_BITMAP
    prm_modules_bitmap;

  UI_MACROS_INIT_FUNCTION("ui_dhrp_api_diagnostics");
  soc_sand_proc_name = "dhrp_print_diag_info";

  unit = dhrp_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_DIAGNOSTICS_PRINT_DIAG_INFO_PRINT_LEVEL_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_DHRP_API_DIAGNOSTICS_PRINT_DIAG_INFO_PRINT_LEVEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_print_level = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter prnt_lvl after print_diag_info***", TRUE);
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_DIAGNOSTICS_PRINT_DIAG_INFO_MODULES_BITMAP_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_DHRP_API_DIAGNOSTICS_PRINT_DIAG_INFO_MODULES_BITMAP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_modules_bitmap = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mdls_btmp after print_diag_info***", TRUE);
  }


  /* Call function */
  send_string_to_screen(" *** SW error - not implemented. See ui_dhrp_api.c ", TRUE);
  /*  TODO - doesn't link.
  ret = dhrp_print_diag_info(
          prm_print_level,
          prm_modules_bitmap
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** dhrp_print_diag_info - FAIL", TRUE);
    goto exit;
  }

  send_string_to_screen(" *** dhrp_print_diag_info - SUCCEEDED", TRUE);
  */
  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Section handler: management
 */
int
  ui_dhrp_api_management(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_dhrp_api_management");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_1_ID,1))
  {
    ui_ret = ui_dhrp_api_management_init_phase_1(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_INIT_PHASE_2_ID,1))
  {
    ui_ret = ui_dhrp_api_management_init_phase_2(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_FILL_DEFAULT_INIT_INFO_ID,1))
  {
    ui_ret = ui_dhrp_api_management_fill_default_init_info(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_PREINIT_ADVANCED_SETTINGS_ID,1))
  {
    ui_ret = ui_dhrp_api_management_preinit_advanced_settings(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_CLOSE_ID,1))
  {
    ui_ret = ui_dhrp_api_management_close(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after management***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Section handler: scheduling_scheme
 */
int
  ui_dhrp_api_scheduling_scheme(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_dhrp_api_scheduling_scheme");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_UPDATE_PORT_ID,1))
  {
    ui_ret = ui_dhrp_api_scheduling_scheme_sch_scm_clb_update_port(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_FILL_TEMPLATE_SCHEME_ID,1))
  {
    ui_ret = ui_dhrp_api_scheduling_scheme_sch_scm_clb_fill_template_scheme(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_UPDATE_CLASS_ATTRIBUTE_ID,1))
  {
    ui_ret = ui_dhrp_api_scheduling_scheme_sch_scm_clb_update_class_attribute(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_GET_PORT_ID,1))
  {
    ui_ret = ui_dhrp_api_scheduling_scheme_sch_scm_clb_get_port(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_SCHEDULING_SCHEME_SCH_SCM_CLB_PRINT_PORT_SCHEME_ID,1))
  {
    ui_ret = ui_dhrp_api_scheduling_scheme_sch_scm_clb_print_port_scheme(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after scheduling_scheme***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Section handler: multicast
 */
int
  ui_dhrp_api_multicast(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_dhrp_api_multicast");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_SET_GLOBAL_INFO_ID,1))
  {
    ui_ret = ui_dhrp_api_multicast_set_global_info(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_GET_GLOBAL_INFO_ID,1))
  {
    ui_ret = ui_dhrp_api_multicast_get_global_info(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_GROUP_OPEN_ID,1))
  {
    ui_ret = ui_dhrp_api_multicast_group_open(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_GROUP_GET_ID,1))
  {
    ui_ret = ui_dhrp_api_multicast_group_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_GROUP_CLOSE_ID,1))
  {
    ui_ret = ui_dhrp_api_multicast_group_close(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_GROUP_UPDATE_PORT_ID,1))
  {
    ui_ret = ui_dhrp_api_multicast_group_update_port(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after multicast***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Section handler: diagnostics
 */
int
  ui_dhrp_api_diagnostics(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_dhrp_api_diagnostics");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_DIAGNOSTICS_PRINT_DIAG_INFO_ID,1))
  {
    ui_ret = ui_dhrp_api_diagnostics_print_diag_info(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after diagnostics***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}


/*****************************************************
*NAME
*  subject_dhrp_api
*TYPE: PROC
*DATE: 29/DEC/2002
*FUNCTION:
*  Process input line which has an 'subject_dhrp_api' subject.
*  Input line is assumed to have been checked and
*  found to be of right format.
*CALLING SEQUENCE:
*  subject_dhrp_api(current_line,current_line_ptr)
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
  subject_dhrp_api(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  )
{
  unsigned int
    match_index;
  int
    ui_ret;
  /*
  char
    err_msg[80*8] = "" ;
  */
  unsigned int
    ;
  char
    *proc_name ;


  proc_name = "subject_dhrp_api" ;
  ui_ret = FALSE ;
  unit = 0;


  /*
   * the rest of the line is empty
   */
  if (current_line->num_param_names == 0)
  {
    send_string_to_screen("\r\n",FALSE) ;
    send_string_to_screen("'subject_dhrp_api()' function was called with no parameters.\r\n",FALSE) ;
    goto exit ;
  }
  /*
   * Else, there are parameters on the line (not just 'subject_dhrp_api').
   */

  send_array_to_screen("\r\n",2) ;



  if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MANAGEMENT_ID,1))
  {
    ui_ret = ui_dhrp_api_management(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_SCHEDULING_SCHEME_ID,1))
  {
    ui_ret = ui_dhrp_api_scheduling_scheme(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_MULTICAST_ID,1))
  {
    ui_ret = ui_dhrp_api_multicast(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_DHRP_API_DIAGNOSTICS_ID,1))
  {
    ui_ret = ui_dhrp_api_diagnostics(current_line);
  }
  else
  {
    /*
     * Enter if an unknown request.
     */
    send_string_to_screen(
      "\r\n"
      "*** dhrp_api command with unknown parameters'.\r\n"
      "    Syntax error/sw error...\r\n",
      TRUE)  ;
    ui_ret = TRUE  ;
    goto exit  ;
  }

exit:
  return (ui_ret);
}

