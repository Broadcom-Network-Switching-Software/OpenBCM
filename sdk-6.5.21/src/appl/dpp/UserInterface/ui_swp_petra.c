/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#if LINK_PETRA_LIBRARIES

#include <appl/diag/dpp/ref_sys.h>

#if !(defined(LINUX) || defined(UNIX))
  #include <bcm_app/dpp/../H/usrApp.h>
#endif

#include <FMC/fmc_common_agent.h>

#include <appl/dpp/UserInterface/ui_defi.h>
#include <appl/dpp/UserInterface/ui_pure_defi_swp_petra.h>

#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/SAND/SAND_FM/sand_mem_access.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>

#include <soc/dpp/Petra/petra_general.h>

#include <sweep/Petra/swp_p_tm_sch_schemes.h>
#include <sweep/Petra/swp_p_tm_info.h>
#include <sweep/Petra/swp_p_tm_app.h>
#include <sweep/Petra/swp_p_diagnostics.h>
#include <sweep/Petra/swp_p_api_multicast.h>
#include <sweep/Petra/swp_p_tm_ssr.h>
#include <sweep/Petra/swp_p_tm_vsq.h>
#include <sweep/Petra/swp_p_tm_qos_ingr.h>                                                               
#include <sweep/Petra/swp_p_tm_system_conf.h>

#include <appl/diag/dpp/utils_defi.h>
#include <appl/diag/dpp/utils_screening.h>

#include <CSystem/csystem_mngmnt.h>

extern
  uint32
    Default_unit;

#define UI_SWP_P_DIAGNOSTICS
#define UI_SWP_P_MULTICAST

uint32
  swp_p_get_default_unit()
{
  return Default_unit;
}


int
  ui_swp_p_fe_graceful_restore(
    CURRENT_LINE *current_line
  )
{
  uint32
    err = 0;
  FMC_COMMON_OUT_STRUCT
    out_struct ;
  FMC_COMMON_IN_STRUCT
    in_struct ;
  uint32
    fe_to_access = 0;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_fe_graceful_restore");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_FE_GRACEFUL_RESTORE_FE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_FE_GRACEFUL_RESTORE_FE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    fe_to_access = param_val->value.ulong_value ;
  }

  in_struct.out_struct_type = FE200_STRUCTURE_TYPE ;
  in_struct.common_union.fe200_struct.proc_id = FE200_ACTIVATE_FE_AND_LINKS ;
  in_struct.common_union.fe200_struct.data.just_unit_data.unit = csys_mngment_local_get_unit(SOC_SAND_DEV_FE200,fe_to_access);

  err = fmc_send_buffer_to_device(
          SOC_SAND_DEV_FE200,
          fe_to_access,
          TRUE,
          sizeof(in_struct),
          (uint8*)&in_struct,
          sizeof(out_struct),
          (uint8*)&out_struct
        );
  if (err)
  {
    sal_sprintf(err_msg,
      "\r\n\n"
      "*** \'graceful_restore_fe\' returned with error \r\n"
      ) ;
    send_string_to_screen(err_msg,TRUE) ;
  }
  else
  {
    sal_sprintf(err_msg,
      "\r\n\n"
      "--> \'graceful_restore_fe\' succeeded \r\n"
      ) ;
    send_string_to_screen(err_msg,TRUE) ;
  }

exit:
  return ui_ret;
}


int
  ui_swp_p_fe_graceful_shut_down(
    CURRENT_LINE *current_line
  )
{
  uint32
    err = 0;
  FMC_COMMON_OUT_STRUCT
    out_struct ;
  FMC_COMMON_IN_STRUCT
    in_struct ;
  uint32
    fe_to_access = 0;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_fe_graceful_shut_down");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_FE_GRACEFUL_SHUT_DOWN_FE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_FE_GRACEFUL_SHUT_DOWN_FE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    fe_to_access = param_val->value.ulong_value ;
  }

  in_struct.out_struct_type = FE200_STRUCTURE_TYPE ;
  in_struct.common_union.fe200_struct.proc_id = FE200_SHUTDOWN_DEVICE ;
  in_struct.common_union.fe200_struct.data.just_unit_data.unit = csys_mngment_local_get_unit(SOC_SAND_DEV_FE200,fe_to_access);

  err = fmc_send_buffer_to_device(
          SOC_SAND_DEV_FE200,
          fe_to_access,
          TRUE,
          sizeof(in_struct),
          (uint8*)&in_struct,
          sizeof(out_struct),
          (uint8*)&out_struct
        );
  if (err)
  {
    sal_sprintf(err_msg,
      "\r\n\n"
      "*** \'graceful_shutdown_fe\' returned with error \r\n"
      ) ;
    send_string_to_screen(err_msg,TRUE) ;
  }
  else
  {
    sal_sprintf(err_msg,
      "\r\n\n"
      "--> \'graceful_shutdown_fe\' succeeded \r\n"
      ) ;
    send_string_to_screen(err_msg,TRUE) ;
  }
exit:
  return ui_ret;
}

int
  ui_swp_p_fe(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_swp_p_fe");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_FE_GRACEFUL_SHUT_DOWN_ID,1))
  {
    ui_ret = ui_swp_p_fe_graceful_shut_down(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_FE_GRACEFUL_RESTORE_ID,1))
  {
    ui_ret = ui_swp_p_fe_graceful_restore(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after fe***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

int
  ui_swp_p_ssr(
  CURRENT_LINE *current_line
  )
{
  uint8
    pas1 = FALSE;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_ssr");


  if (SOC_PETRA_IS_DEV_PETRA_B_OR_ABOVE)
  {
    pas1 = swp_pb_tm_ssr_test_xor_check(
      swp_p_get_default_unit(),
      FALSE
      );
  }
  else 
  {
    pas1 = swp_p_tm_ssr_test_xor_check(
      swp_p_get_default_unit(),
      FALSE
      );
  }
  
  soc_sand_os_printf( "SOC_PETRA SSR test %s\n", (pas1? "pass" : "fail"));

  goto exit;
exit:
  return ui_ret;
}


#ifdef UI_SWP_P_DIAGNOSTICS
/********************************************************************
 *  Function handler: nif_counters_get (section swp_p_diagnostics)
 */
int
  ui_swp_p_diagnostics_nif_counters_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = SOC_SAND_OK;
  SWP_P_NIF_COUNTERS
    prm_nif_counters;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_diagnostics");
  soc_sand_proc_name = "swp_p_diag_nif_counters_get";

  unit = swp_p_get_default_unit();
  swp_p_SWP_P_NIF_COUNTERS_clear(&prm_nif_counters);

  /* Get parameters */

  /* Call function */
  ret = swp_p_diag_nif_counters_get(
          unit,
          &prm_nif_counters,
          SWP_P_SILENT
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    goto exit;
  }

  send_string_to_screen("--> nif_counters:", TRUE);
  swp_p_SWP_P_NIF_COUNTERS_print(unit, &prm_nif_counters);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: srd_eye_scan_run (section swp_p_diagnostics)
 */
int
  ui_swp_p_diagnostics_srd_eye_scan_run(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = SOC_SAND_OK;
uint32
    prm_nof_lane_index = 0;
  SOC_PETRA_SRD_EYE_SCAN_INFO
    prm_info;
  uint32
    res_i;
  SOC_PETRA_SRD_EYE_SCAN_RES
    *prm_res = NULL;
  SOC_PETRA_SRD_CNT_SRC
    counting_src = SOC_PETRA_SRD_NOF_CNT_SRCS;
  SOC_PETRA_SRD_TRAFFIC_SRC 
    traffic_src = SOC_PETRA_SRD_NOF_TRAFFIC_SRCS;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_diagnostics");
  soc_sand_proc_name = "swp_p_srd_eye_scan_run";

  unit = swp_p_get_default_unit();
  soc_petra_PETRA_SRD_EYE_SCAN_INFO_clear(&prm_info);

/* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_INFO_TRAFFIC_SRC_ID,1))
  {
    UI_MACROS_GET_NUM_SYM_VAL(PARAM_SWP_P_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_INFO_TRAFFIC_SRC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    UI_MACROS_LOAD_LONG_VAL(traffic_src);
  }
  prm_info.traffic_src = traffic_src;

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_INFO_RESOLUTION_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_INFO_RESOLUTION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.resolution = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_INFO_TX_PRBS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_INFO_TX_PRBS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.params.prbs.is_tx_prbs_enabled = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_INFO_PRBS_DURATION_MIN_SEC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_INFO_PRBS_DURATION_MIN_SEC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.duration_min_sec = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_INFO_LANE_NDX_ID,1))
  {
    for (prm_nof_lane_index = 0; prm_nof_lane_index < SOC_PETRA_SRD_NOF_LANES; ++prm_nof_lane_index, ui_ret = 0)
    {
      UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(PARAM_SWP_P_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_INFO_LANE_NDX_ID, prm_nof_lane_index + 1);
      if (ui_ret)
      {
        ui_ret = 0;
        break;
      }
      prm_info.lane_ndx[prm_nof_lane_index] = (uint32)param_val->value.ulong_value;
    }
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_INFO_CRC_ID,1))
  {
    UI_MACROS_GET_NUM_SYM_VAL(PARAM_SWP_P_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_INFO_CRC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    UI_MACROS_LOAD_LONG_VAL(counting_src);
  }
  if (traffic_src == SOC_PETRA_SRD_TRAFFIC_SRC_PRBS) 
  {
    prm_info.params.prbs.cnt_src = counting_src;
  } 
  else 
  {
    prm_info.params.ext.cnt_src = counting_src;
  }

  prm_info.nof_lane_ndx_entries = prm_nof_lane_index;

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_SRD_EYE_SCAN_RUN_SRD_PRBS_MODE_SET_MODE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_SRD_EYE_SCAN_RUN_SRD_PRBS_MODE_SET_MODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.params.prbs.mode = param_val->numeric_equivalent;
  }

  /* allocate result structures */
  prm_res = sal_alloc(sizeof(SOC_PETRA_SRD_EYE_SCAN_RES) * prm_info.nof_lane_ndx_entries, "sal_alloc");
  if (!prm_res)
  {
    send_string_to_screen(" *** sal_alloc - FAIL", TRUE);
    goto exit;
  }


  for (res_i = 0; res_i < prm_info.nof_lane_ndx_entries; res_i++)
  {
    soc_petra_PETRA_SRD_EYE_SCAN_RES_clear(&(prm_res[res_i]));
  }

  /* Call function */
  ret = swp_p_diag_srd_eye_scan_run(
          unit,
          &prm_info,
          FALSE,
          prm_res
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_srd_eye_scan_run - FAIL", TRUE);
    goto exit;
  }

  send_string_to_screen("--> res:", TRUE);

  goto exit;
exit:
  sal_free(prm_res);
  return ui_ret;
}

#endif
#ifdef UI_SWP_P_MULTICAST
/********************************************************************
 *  Function handler: mc_grp_open (section swp_p_multicast)
 */
int
  ui_swp_p_api_multicast_mc_grp_open(
    CURRENT_LINE *current_line
  )
{
  uint32
    param_i,
    ret = SOC_SAND_OK;
  uint32
    prm_mc_id;
  SWP_P_MC_TYPE
    prm_mc_type;
  uint32
    prm_mc_grp_size;
  SOC_PETRA_DEST_SYS_PORT_INFO
    prm_mc_grp_members[UI_MC_GRP_MEMBERS_MAX];
  uint32
    prm_per_member_cud[UI_MC_GRP_MEMBERS_MAX];

  UI_MACROS_INIT_FUNCTION("ui_swp_p_api_multicast");
  soc_sand_proc_name = "swp_p_mc_grp_open";

  unit = swp_p_get_default_unit();

  /* Initialization */
  for (param_i=1; param_i<=UI_MC_GRP_MEMBERS_MAX; ++param_i)
  {
    prm_per_member_cud[param_i - 1] = 0;
    soc_petra_PETRA_DEST_SYS_PORT_INFO_clear(&(prm_mc_grp_members[param_i - 1]));
  }

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_MC_GRP_OPEN_MC_GRP_OPEN_MC_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_MC_GRP_OPEN_MC_GRP_OPEN_MC_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_MC_GRP_OPEN_MC_GRP_OPEN_MC_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_MC_GRP_OPEN_MC_GRP_OPEN_MC_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_type = param_val->numeric_equivalent;
  }

  for (param_i=1; param_i<=UI_MC_GRP_MEMBERS_MAX; ++param_i)
  {
    ret = search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_MC_GRP_OPEN_MC_GRP_OPEN_MC_GRP_MEMBERS_ID,param_i);
    if (0 != ret)
    {
      ret=0;
      break;
    }
    UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(PARAM_SWP_P_MC_GRP_OPEN_MC_GRP_OPEN_MC_GRP_MEMBERS_ID,param_i);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_grp_members[param_i-1].type = SOC_PETRA_DEST_SYS_PORT_TYPE_SYS_PHY_PORT;
    prm_mc_grp_members[param_i-1].id = param_val->value.ulong_value;
  }

  prm_mc_grp_size = param_i - 1;

  for (param_i=1; param_i<=prm_mc_grp_size; ++param_i)
  {
    ret = search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_MC_GRP_OPEN_MC_GRP_OPEN_PER_MEMBER_CUD_ID,param_i);
    if (0 != ret)
    {
      ret=0;
      break;
    }
    UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(PARAM_SWP_P_MC_GRP_OPEN_MC_GRP_OPEN_PER_MEMBER_CUD_ID,param_i);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_per_member_cud[param_i-1] = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = swp_p_mc_grp_open(
          unit,
          prm_mc_id,
          prm_mc_type,
          prm_mc_grp_size,
          prm_mc_grp_members,
          prm_per_member_cud,
          SWP_P_SILENT
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mc_grp_close (section swp_p_multicast)
 */
int
  ui_swp_p_api_multicast_mc_grp_close(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = SOC_SAND_OK;
  SWP_P_MC_TYPE
    prm_mc_type;
  uint32
    prm_mc_id;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_api_multicast");
  soc_sand_proc_name = "swp_p_mc_grp_close";

  unit = swp_p_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_MC_GRP_CLOSE_MC_GRP_CLOSE_MC_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_MC_GRP_CLOSE_MC_GRP_CLOSE_MC_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_type = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_MC_GRP_CLOSE_MC_GRP_CLOSE_MC_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_MC_GRP_CLOSE_MC_GRP_CLOSE_MC_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_id = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = swp_p_mc_grp_close(
          unit,
          prm_mc_type,
          prm_mc_id,
          SWP_P_SILENT
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_mc_grp_close - FAIL", TRUE);
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: mc_grp_get (section multicast)
 */
int
  ui_swp_p_api_multicast_mc_grp_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret = SOC_SAND_OK;
  uint32
    prm_mc_id;
  SWP_P_MC_TYPE
    prm_mc_type = SWP_P_MC_GET_TYPE_ALL;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_api_multicast");
  soc_sand_proc_name = "swp_p_mc_grp_get";

  unit = swp_p_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_MC_GRP_GET_MC_GRP_GET_MC_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_MC_GRP_GET_MC_GRP_GET_MC_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_MC_GRP_GET_MC_GRP_GET_MC_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWP_P_MC_GRP_GET_MC_GRP_GET_MC_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_type = param_val->numeric_equivalent;
  }

  ret = swp_p_api_multicast_group_print(
          unit,
          prm_mc_id,
          prm_mc_type,
          FALSE
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** swp_p_api_multicast_group_print - FAIL", TRUE);
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}
#endif
  
/******************************************************************** 
 *  Function handler: tm_vsq_tail_drop_app (section swp_p_tm_vsq)
 */
int 
  ui_swp_p_tm_vsq_tm_vsq_tail_drop_app(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret = SOC_SAND_OK;   
  uint8   
    prm_silent;
   
  UI_MACROS_INIT_FUNCTION("ui_swp_p_tm_vsq"); 
  soc_sand_proc_name = "swp_p_tm_vsq_tail_drop_app"; 
 
  unit = swp_p_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_TM_VSQ_TAIL_DROP_APP_TM_VSQ_TAIL_DROP_APP_SILENT_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_TM_VSQ_TAIL_DROP_APP_TM_VSQ_TAIL_DROP_APP_SILENT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_silent = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = swp_p_tm_vsq_tail_drop_app(
          unit,
          prm_silent
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** swp_p_tm_vsq_tail_drop_app - FAIL", TRUE); 
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ret; 
} 
  
/******************************************************************** 
 *  Function handler: tm_vsq_wred_per_stag_app (section swp_p_tm_vsq)
 */
int 
  ui_swp_p_tm_vsq_tm_vsq_wred_per_stag_app(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret = SOC_SAND_OK;   
  uint8   
    prm_silent;
   
  UI_MACROS_INIT_FUNCTION("ui_swp_p_tm_vsq"); 
  soc_sand_proc_name = "swp_p_tm_vsq_wred_per_stag_app"; 
 
  unit = swp_p_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_TM_VSQ_WRED_PER_STAG_APP_TM_VSQ_WRED_PER_STAG_APP_SILENT_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_TM_VSQ_WRED_PER_STAG_APP_TM_VSQ_WRED_PER_STAG_APP_SILENT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_silent = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = swp_p_tm_vsq_wred_per_stag_app(
          unit,
          prm_silent
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** swp_p_tm_vsq_wred_per_stag_app - FAIL", TRUE); 
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ret; 
} 
  
/******************************************************************** 
 *  Function handler: tm_vsq_app (section swp_p_tm_vsq)
 */
int 
  ui_swp_p_tm_vsq_tm_vsq_app(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret = SOC_SAND_OK;   
  uint8   
    prm_silent;
   
  UI_MACROS_INIT_FUNCTION("ui_swp_p_tm_vsq"); 
  soc_sand_proc_name = "swp_p_tm_vsq_app"; 
 
  unit = swp_p_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_TM_VSQ_APP_TM_VSQ_APP_SILENT_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_TM_VSQ_APP_TM_VSQ_APP_SILENT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_silent = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = swp_p_tm_vsq_app(
          unit,
          prm_silent
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** swp_p_tm_vsq_app - FAIL", TRUE); 
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ret; 
} 

/******************************************************************** 
 *  Function handler: tm_isp_sched_set (section swp_p_tm_qos_ingr)
 */
int 
  ui_swp_p_tm_qos_ingr_tm_isp_sched_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_port_ndx;
  uint32   
    prm_overall_rate_mbps;
  uint8   
    prm_silent = FALSE;
   
  UI_MACROS_INIT_FUNCTION("ui_swp_p_tm_qos_ingr"); 
  soc_sand_proc_name = "swp_p_tm_isp_sched_set"; 
 
  unit = swp_p_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_TM_ISP_SCHED_SET_TM_ISP_SCHED_SET_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_TM_ISP_SCHED_SET_TM_ISP_SCHED_SET_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter port_ndx after tm_isp_sched_set***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_TM_ISP_SCHED_SET_TM_ISP_SCHED_SET_OVERALL_RATE_MBPS_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_TM_ISP_SCHED_SET_TM_ISP_SCHED_SET_OVERALL_RATE_MBPS_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_overall_rate_mbps = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_TM_ISP_SCHED_SET_TM_ISP_SCHED_SET_SILENT_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_SWP_P_TM_ISP_SCHED_SET_TM_ISP_SCHED_SET_SILENT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_silent = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = swp_p_tm_isp_sched_set(
          unit,
          prm_port_ndx,
          prm_overall_rate_mbps,
          prm_silent
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** swp_p_tm_isp_sched_set - FAIL", TRUE); 
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ret; 
} 
/******************************************************************** 
 *  Function handler: tm_scheduling_schemes_open (section externals)
 */
int 
  ui_externals_tm_scheduling_schemes_open(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SWP_P_TM_INFO
    *prm_sweep_info = NULL;
  uint32   
    prm_max_nof_faps_in_system;
  uint8   
    prm_silent = FALSE;
   
  UI_MACROS_INIT_FUNCTION("ui_externals"); 
  soc_sand_proc_name = "swp_p_tm_scheduling_schemes_open"; 
 
  unit = swp_p_get_default_unit(); 
 
  /* Get parameters */ 
  ret = swp_p_tm_sys_sweep_info_get(
          unit,
          &prm_sweep_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** swp_p_tm_sys_sweep_info_get - FAIL", TRUE); 
    goto exit; 
  } 

  prm_max_nof_faps_in_system = swp_p_tm_bsp_max_nof_faps_in_system_get();

  /* Call function */
  ret = swp_p_tm_scheduling_schemes_open(
          unit,
          prm_sweep_info,
          prm_max_nof_faps_in_system,
          prm_silent
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** swp_p_tm_scheduling_schemes_open - FAIL", TRUE); 
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
#ifdef UI_SWP_P_DIAGNOSTICS/* { swp_p_diagnostics*/
/********************************************************************
 *  Section handler: swp_p_diagnostics
 */
int
  ui_swp_p_diagnostics(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_swp_p_diagnostics");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_DIAG_NIF_COUNTERS_GET_NIF_COUNTERS_GET_ID,1))
  {
    ui_ret = ui_swp_p_diagnostics_nif_counters_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_ID,1))
  {
    ui_ret = ui_swp_p_diagnostics_srd_eye_scan_run(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after swp_p_diagnostics***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* {swp_p_diagnostics */

#ifdef UI_SWP_P_MULTICAST/* { multicast*/
/********************************************************************
*  Section handler: swp_p_multicast
 */
int
  ui_swp_p_api_multicast(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_swp_p_api_multicast");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_MC_GRP_OPEN_MC_GRP_OPEN_ID,1))
  {
    ui_ret = ui_swp_p_api_multicast_mc_grp_open(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_MC_GRP_CLOSE_MC_GRP_CLOSE_ID,1))
  {
    ui_ret = ui_swp_p_api_multicast_mc_grp_close(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_MC_GRP_GET_MC_GRP_GET_ID,1))
  {
    ui_ret = ui_swp_p_api_multicast_mc_grp_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after multicast***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* {swp_p_multicast */

/******************************************************************** 
 *  Section handler: swp_p_tm_vsq
 */ 
int 
  ui_swp_p_tm_vsq( 
    CURRENT_LINE *current_line 
  ) 
{   
  UI_MACROS_INIT_FUNCTION("ui_swp_p_tm_vsq"); 
 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_TM_VSQ_TAIL_DROP_APP_TM_VSQ_TAIL_DROP_APP_ID,1)) 
  { 
    ui_ret = ui_swp_p_tm_vsq_tm_vsq_tail_drop_app(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_TM_VSQ_WRED_PER_STAG_APP_TM_VSQ_WRED_PER_STAG_APP_ID,1)) 
  { 
    ui_ret = ui_swp_p_tm_vsq_tm_vsq_wred_per_stag_app(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_TM_VSQ_APP_TM_VSQ_APP_ID,1)) 
  { 
    ui_ret = ui_swp_p_tm_vsq_tm_vsq_app(current_line); 
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting function name after swp_p_tm_vsq***", TRUE); 
  } 
    
  goto exit; 
exit:        
  return ui_ret; 
}                
/* { swp_p_tm_qos_ingr*/
/******************************************************************** 
 *  Section handler: swp_p_tm_qos_ingr
 */ 
int 
  ui_swp_p_tm_qos_ingr( 
    CURRENT_LINE *current_line 
  ) 
{   
  int
    ret;

  UI_MACROS_INIT_FUNCTION("ui_swp_p_tm_qos_ingr"); 
 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_TM_ISP_SCHED_SET_TM_ISP_SCHED_SET_ID,1)) 
  { 
    ret = ui_swp_p_tm_qos_ingr_tm_isp_sched_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_TM_SCHEDULING_SCHEMES_OPEN_TM_SCHEDULING_SCHEMES_OPEN_ID,1)) 
  { 
    ret = ui_externals_tm_scheduling_schemes_open(current_line); 
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting function name after swp_p_tm_qos_ingr***", TRUE); 
  } 
    
  goto exit; 
exit:        
  return ret; 
}                
          

/*****************************************************
*NAME
*  subject_swp_p_api
*TYPE: PROC
*DATE: 29/DEC/2002
*FUNCTION:
*  Process input line which has an 'subject_swp_p_api' subject.
*  Input line is assumed to have been checked and
*  found to be of right format.
*CALLING SEQUENCE:
*  subject_swp_p_api(current_line,current_line_ptr)
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
  subject_swp_p_api(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  )
{
  UI_MACROS_INIT_FUNCTION("subject_swp_p_api");

  /*
   * the rest of the line is empty
   */
  if (current_line->num_param_names == 0)
  {
    send_string_to_screen("\r\n",FALSE) ;
    send_string_to_screen("'subject_swp_p_api()' function was called with no parameters.\r\n",FALSE) ;
    goto exit ;
  }
  /*
   * Else, there are parameters on the line (not just 'subject_swp_p_api').
   */

  send_array_to_screen("\r\n",2) ;

  if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_FE_ID,1))
  {
    ui_ret = ui_swp_p_fe(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_SSR_ID,1))
  {
    ui_ret = ui_swp_p_ssr(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_DIAGNOSTICS_ID,1))
  {
    ui_ret = ui_swp_p_diagnostics(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_API_MULTICAST_ID,1))
  {
    ui_ret = ui_swp_p_api_multicast(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_TM_VSQ_ID,1)) 
  { 
    ui_ret = ui_swp_p_tm_vsq(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_SWP_P_TM_QOS_INGR_ID,1)) 
  { 
    ui_ret = ui_swp_p_tm_qos_ingr(current_line); 
  } 
  else
  {
    /*
     * Enter if an unknown request.
     */
    send_string_to_screen(
      "\r\n"
      "*** swp_p_api command with unknown parameters'.\r\n"
      "    Syntax error/sw error...\r\n",
      TRUE)  ;
    ui_ret = TRUE  ;
    goto exit  ;
  }

exit:
  return (ui_ret);
}

#endif

