/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#include <appl/diag/dpp/ref_sys.h>
#include <bcm_app/dpp/../H/usrApp.h>

#if LINK_FAP21V_LIBRARIES

#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/SAND/SAND_FM/sand_mem_access.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>

#include <soc/dpp/SOC_SAND_FAP21V/fap21v_general.h>

#include <Sweep/SOC_SAND_FAP21V/sweep21v_sch_schemes.h>
#include <Sweep/SOC_SAND_FAP21V/sweep21v_info.h>
#include <Sweep/SOC_SAND_FAP21V/sweep21v_app.h>
#include <Sweep/SOC_SAND_FAP21V/sweep21v_ssr.h>

#include <appl/diag/dpp/utils_defi.h>
#include <appl/diag/dpp/utils_screening.h>

#include <CSystem/csystem_mngmnt.h>

#include <appl/dpp/UserInterface/ui_defi.h>
#include <appl/dpp/UserInterface/ui_pure_defi_fap21v_app.h>


extern
  uint32
    Default_unit;

STATIC uint32
  get_default_unit()
{
  return Default_unit;
}

int
  ui_fap21v_app_connectlivity_map_get(
    CURRENT_LINE *current_line
  )
{
  int
    err;
  FMC_COMMON_OUT_STRUCT
    out_struct;
  FMC_COMMON_IN_STRUCT
    in_struct;
  uint32
    dst_fap = 0;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_app_update");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_APP_CONNECTIVITY_MAP_GET_FAP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_APP_CONNECTIVITY_MAP_GET_FAP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    dst_fap = (uint32)param_val->value.ulong_value;
  }

  in_struct.out_struct_type = FAP21V_STRUCTURE_TYPE ;
  in_struct.common_union.fap21v_struct.proc_id = FAP21V_AGENT_FABRIC_LINKS_CONNECTIVITY_MAP_GET;
  in_struct.common_union.fap21v_struct.data.connectivity_map_struct.unit = csys_mngment_local_get_unit(SOC_SAND_DEV_FAP21V, dst_fap) ;

  err = fmc_send_buffer_to_device(
          SOC_SAND_DEV_FAP21V,
          dst_fap,
          TRUE,
          sizeof(in_struct),
          (uint8*)&in_struct,
          sizeof(out_struct),
          (uint8*)&out_struct
        );
  if(err)
  {
    send_string_to_screen(" *** ui_fap21v_app_connectlivity_map_get - FAIL", TRUE);
    fap21v_disp_result(err, "ui_fap21v_app_connectlivity_map_get");
    goto exit;
  }

  fap21v_FAP21V_FABRIC_LINKS_CONNECTIVITY_MAP_print(
    &(out_struct.common_union.fap21v_struct.data.connectivity_map_struct.connectivity_map)
  );

exit:
  return ui_ret;
}

int
  ui_fap21v_app_update(
    CURRENT_LINE *current_line
  )
{
  int
    err;
  uint32
    ret;
  SWEEP21V_SCH_SCHEME_INFO
    sch_scheme_info;
  uint32
    dsp_fap,
    port_ndx;
  uint32
    nominal_rate_per_port_kbps = 10000000;
  uint8
    silent = TRUE;
  FMC_COMMON_OUT_STRUCT
    out_struct ;
  FMC_COMMON_IN_STRUCT
    in_struct ;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_app_update");
  soc_sand_proc_name = "ui_fap21v_app_update";

  sweep21v_SWEEP21V_SCH_SCHEME_INFO_clear(&sch_scheme_info);

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_APP_SCHEDULING_SCHEMES_UPDATE_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_APP_SCHEDULING_SCHEMES_UPDATE_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    port_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter port_ndx after update***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_APP_SCHEDULING_SCHEMES_UPDATE_NOMINAL_RATE_MBPS_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_APP_SCHEDULING_SCHEMES_UPDATE_NOMINAL_RATE_MBPS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    nominal_rate_per_port_kbps = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_APP_SCHEDULING_SCHEMES_UPDATE_SCHEME_ID_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_APP_SCHEDULING_SCHEMES_UPDATE_SCHEME_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    sch_scheme_info.scheme_id = (SWEEP21V_SCH_SCHEME)param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_APP_SCHEDULING_SCHEMES_UPDATE_MINIMUM_RATE_BY_PERCENTAGE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_APP_SCHEDULING_SCHEMES_UPDATE_MINIMUM_RATE_BY_PERCENTAGE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    sch_scheme_info.minimum_rate_by_percentage = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_APP_SCHEDULING_SCHEMES_UPDATE_MAXIMUM_RATE_BY_PERCENTAGE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_APP_SCHEDULING_SCHEMES_UPDATE_MAXIMUM_RATE_BY_PERCENTAGE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    sch_scheme_info.maximum_rate_by_percentage = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_APP_SCHEDULING_SCHEMES_UPDATE_FAP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_APP_SCHEDULING_SCHEMES_UPDATE_FAP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    dsp_fap = (uint32)param_val->value.ulong_value;

    in_struct.out_struct_type = FAP21V_STRUCTURE_TYPE ;
    in_struct.common_union.fap21v_struct.proc_id = FAP21V_AGENT_SCH_SCHEME_UPDATE ;
    in_struct.common_union.fap21v_struct.data.scheduling_schemes_update_struct.unit = csys_mngment_local_get_unit(SOC_SAND_DEV_FAP21V, dsp_fap)  ;
    in_struct.common_union.fap21v_struct.data.scheduling_schemes_update_struct.port_ndx = port_ndx;
    in_struct.common_union.fap21v_struct.data.scheduling_schemes_update_struct.nominal_rate_kbps = nominal_rate_per_port_kbps;
    in_struct.common_union.fap21v_struct.data.scheduling_schemes_update_struct.scheme_id = sch_scheme_info.scheme_id;
    in_struct.common_union.fap21v_struct.data.scheduling_schemes_update_struct.minimum_rate_by_percentage = sch_scheme_info.minimum_rate_by_percentage;
    in_struct.common_union.fap21v_struct.data.scheduling_schemes_update_struct.maximum_rate_by_percentage = sch_scheme_info.maximum_rate_by_percentage;

    err = fmc_send_buffer_to_device(
            SOC_SAND_DEV_FAP21V,
            dsp_fap,
            TRUE,
            sizeof(in_struct),
            (uint8*)&in_struct,
            sizeof(out_struct),
            (uint8*)&out_struct
          );
    if(err)
    {
      send_string_to_screen(" *** ui_fap21v_app_gen_sch_update - FAIL", TRUE);
      fap21v_disp_result(err, "ui_fap21v_app_gen_sch_update");
      goto exit;
    }
  }
  else
  {
    /* Call function */
    ret = sweep21v_scheduling_schemes_update(
            get_default_unit(),
            port_ndx,
            nominal_rate_per_port_kbps,
            &sch_scheme_info,
            silent
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** ui_fap21v_app_update - FAIL", TRUE);
      fap21v_disp_result(ret, "ui_fap21v_app_update");
      goto exit;
    }
  }

exit:
  return ui_ret;
}

int
  ui_fap21v_app_gen_sch_update(
    CURRENT_LINE *current_line
  )
{
  int
    err;
  uint32
    ret;
  SWEEP21V_SCH_HR_CLASS
    sp_class = SWEEP21V_SCH_FLOW_HR_CLASS_1;
  uint32
    dsp_fap = 0,
    port_ndx = 0,
    class_of_service_ndx = 0,
    weight = 2,
    min_rate_kbps = 0,
    max_rate_kbps = 100;
  FMC_COMMON_IN_STRUCT
    in_struct;
  FMC_COMMON_OUT_STRUCT
    out_struct ;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_app_gen_sch_update");
  soc_sand_proc_name = "ui_fap21v_app_gen_sch_update";

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_APP_SCHEDULING_SCHEMES_GENERAL_UPDATE_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_APP_SCHEDULING_SCHEMES_GENERAL_UPDATE_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    port_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter port_ndx after update***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_APP_SCHEDULING_SCHEMES_GENERAL_UPDATE_CLASS_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_APP_SCHEDULING_SCHEMES_GENERAL_UPDATE_CLASS_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    class_of_service_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter nominal_rate_mbps after port_ndx***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_APP_SCHEDULING_SCHEMES_GENERAL_UPDATE_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_APP_SCHEDULING_SCHEMES_GENERAL_UPDATE_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    weight = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_APP_SCHEDULING_SCHEMES_GENERAL_UPDATE_SWEEP21V_SCH_HR_CLASS_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_FAP21V_APP_SCHEDULING_SCHEMES_GENERAL_UPDATE_SWEEP21V_SCH_HR_CLASS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    sp_class = (SWEEP21V_SCH_HR_CLASS)param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_APP_SCHEDULING_SCHEMES_GENERAL_UPDATE_MIN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_APP_SCHEDULING_SCHEMES_GENERAL_UPDATE_MIN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    min_rate_kbps = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_APP_SCHEDULING_SCHEMES_GENERAL_UPDATE_MAX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_APP_SCHEDULING_SCHEMES_GENERAL_UPDATE_MAX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    max_rate_kbps = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_APP_SCHEDULING_SCHEMES_GENERAL_UPDATE_FAP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_APP_SCHEDULING_SCHEMES_GENERAL_UPDATE_FAP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    dsp_fap = (uint32)param_val->value.ulong_value;

    in_struct.out_struct_type = FAP21V_STRUCTURE_TYPE ;
    in_struct.common_union.fap21v_struct.proc_id = FAP21V_AGENT_SCH_GEN_SCHEME_UPDATE ;
    in_struct.common_union.fap21v_struct.data.scm_info_general_update_struct.unit = csys_mngment_local_get_unit(SOC_SAND_DEV_FAP21V, dsp_fap)  ;
    in_struct.common_union.fap21v_struct.data.scm_info_general_update_struct.port_ndx = port_ndx;
    in_struct.common_union.fap21v_struct.data.scm_info_general_update_struct.class_of_service_ndx = class_of_service_ndx;
    in_struct.common_union.fap21v_struct.data.scm_info_general_update_struct.sp_class = sp_class;
    in_struct.common_union.fap21v_struct.data.scm_info_general_update_struct.weight = weight;
    in_struct.common_union.fap21v_struct.data.scm_info_general_update_struct.min_rate_per = min_rate_kbps;
    in_struct.common_union.fap21v_struct.data.scm_info_general_update_struct.min_rate_per = max_rate_kbps;

    err = fmc_send_buffer_to_device(
            SOC_SAND_DEV_FAP21V,
            dsp_fap,
            TRUE,
            sizeof(in_struct),
            (uint8*)&in_struct,
            sizeof(out_struct),
            (uint8*)&out_struct
          );
    if(err)
    {
      send_string_to_screen(" *** ui_fap21v_app_gen_sch_update - FAIL", TRUE);
      fap21v_disp_result(err, "ui_fap21v_app_gen_sch_update");
      goto exit;
    }
  }
  else
  {
    /* Call function */
    ret = sweep21v_scm_info_general_update(
            get_default_unit(),
            port_ndx,
            class_of_service_ndx,
            sp_class,
            weight,
            min_rate_kbps,
            max_rate_kbps
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** ui_fap21v_app_gen_sch_update - FAIL", TRUE);
      fap21v_disp_result(ret, "ui_fap21v_app_gen_sch_update");
      goto exit;
    }
  }

exit:
  return ui_ret;
}

int
  ui_fap21v_app_fe_serdes_power_down(
    CURRENT_LINE *current_line
  )
{
  uint32
    serdes_ndx = 0;
  int
    err;
  FMC_COMMON_IN_STRUCT
    in_struct;
  FMC_COMMON_OUT_STRUCT
    out_struct ;
  uint32
    dst_fap = 0;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_app_fe_serdes_power_down");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_APP_SERDES_POWER_DOWN_FE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_APP_SERDES_POWER_DOWN_FE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    dst_fap = param_val->value.ulong_value ;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_APP_SERDES_POWER_DOWN_SERDES_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_APP_SERDES_POWER_DOWN_SERDES_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    serdes_ndx = param_val->value.ulong_value ;
  }


  in_struct.out_struct_type = FAP21V_STRUCTURE_TYPE ;
  in_struct.common_union.fap21v_struct.proc_id = FAP21V_AGENT_FABRIC_LINKS_SERDES_POWER_DOWN_SET ;
  in_struct.common_union.fap21v_struct.data.serdes_power_down_struct.unit = csys_mngment_local_get_unit(SOC_SAND_DEV_FAP21V, dst_fap)  ;
  in_struct.common_union.fap21v_struct.data.serdes_power_down_struct.link_ndx = serdes_ndx ;
  in_struct.common_union.fap21v_struct.data.serdes_power_down_struct.shut_down = TRUE;

  err = fmc_send_buffer_to_device(
          SOC_SAND_DEV_FAP21V,
          dst_fap,
          TRUE,
          sizeof(in_struct),
          (uint8*)&in_struct,
          sizeof(out_struct),
          (uint8*)&out_struct
        );
  if(err)
  {
    send_string_to_screen(" *** ui_fap21v_app_fe_serdes_power_down - FAIL", TRUE);
    fap21v_disp_result(err, "ui_fap21v_app_fe_serdes_power_down");
    goto exit;
  }

  if (SOC_SAND_OK != soc_sand_get_error_code_from_error_word(out_struct.common_union.fap21v_struct.error_id))
  {
    ui_ret = TRUE;
  }
  else
  {
    ui_ret = FALSE;
  }

exit:
  return ui_ret;
}

int
  ui_fap21v_app_fe_serdes_power_up(
    CURRENT_LINE *current_line
  )
{
  uint32
    serdes_ndx = 0;
  int
    err;
  FMC_COMMON_IN_STRUCT
    in_struct;
  FMC_COMMON_OUT_STRUCT
    out_struct ;
  uint32
    dst_fap = 0;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_app_fe_serdes_power_up");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_APP_SERDES_POWER_UP_FE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_APP_SERDES_POWER_UP_FE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    dst_fap = param_val->value.ulong_value ;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_APP_SERDES_POWER_UP_SERDES_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_APP_SERDES_POWER_UP_SERDES_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    serdes_ndx = param_val->value.ulong_value ;
  }

  in_struct.out_struct_type = FAP21V_STRUCTURE_TYPE ;
  in_struct.common_union.fap21v_struct.proc_id = FAP21V_AGENT_FABRIC_LINKS_SERDES_POWER_DOWN_SET ;
  in_struct.common_union.fap21v_struct.data.serdes_power_down_struct.unit = csys_mngment_local_get_unit(SOC_SAND_DEV_FAP21V, dst_fap)  ;
  in_struct.common_union.fap21v_struct.data.serdes_power_down_struct.link_ndx = serdes_ndx ;
  in_struct.common_union.fap21v_struct.data.serdes_power_down_struct.shut_down = FALSE ;

  err = fmc_send_buffer_to_device(
          SOC_SAND_DEV_FAP21V,
          dst_fap,
          TRUE,
          sizeof(in_struct),
          (uint8*)&in_struct,
          sizeof(out_struct),
          (uint8*)&out_struct
        );
  if(err)
  {
    send_string_to_screen(" *** ui_fap21v_app_fe_serdes_power_up - FAIL", TRUE);
    fap21v_disp_result(err, "ui_fap21v_app_fe_serdes_power_up");
    goto exit;
  }

  if (SOC_SAND_OK != soc_sand_get_error_code_from_error_word(out_struct.common_union.fap21v_struct.error_id))
  {
    ui_ret = TRUE;
  }
  else
  {
    ui_ret = FALSE;
  }

exit:
  return ui_ret;
}

int
  ui_fap21v_app_fe_serdes_power(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_app_fe_serdes_power");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_APP_SERDES_POWER_DOWN_ID,1))
  {
    ui_ret = ui_fap21v_app_fe_serdes_power_down(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_APP_SERDES_POWER_UP_ID,1))
  {
    ui_ret = ui_fap21v_app_fe_serdes_power_up(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after serdes_power***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

int
  ui_fap21v_app_fe_graceful_restore(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_app_fe_graceful_restore");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_APP_FE_GRACEFUL_RESTORE_FE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_APP_FE_GRACEFUL_RESTORE_FE_ID);
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
  ui_fap21v_app_fe_graceful_shut_down(
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

  UI_MACROS_INIT_FUNCTION("ui_fap21v_app_fe_graceful_shut_down");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_APP_FE_GRACEFUL_SHUT_DOWN_FE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_FAP21V_APP_FE_GRACEFUL_SHUT_DOWN_FE_ID);
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
  ui_fap21v_app_fe(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_app_fe");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_APP_FE_GRACEFUL_SHUT_DOWN_ID,1))
  {
    ui_ret = ui_fap21v_app_fe_graceful_shut_down(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_APP_FE_GRACEFUL_RESTORE_ID,1))
  {
    ui_ret = ui_fap21v_app_fe_graceful_restore(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_APP_SERDES_POWER_ID,1))
  {
    ui_ret = ui_fap21v_app_fe_serdes_power(current_line);
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
  ui_fap21v_app_links(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_app_links");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_APP_SERDES_POWER_ID,1))
  {
    ui_ret = ui_fap21v_app_fe_serdes_power(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_APP_CONNECTIVITY_MAP_GET_ID,1))
  {
    ui_ret = ui_fap21v_app_connectlivity_map_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after links***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

int
  ui_fap21v_app_ssr(
  CURRENT_LINE *current_line
  )
{
  uint8
    pas1 = FALSE,
    pas2 = FALSE;

  UI_MACROS_INIT_FUNCTION("ui_fap21v_app_ssr");

  pas1 = sweep21v_ssr_test_reset(
           get_default_unit(),
           FALSE
         );

  pas2 = sweep21v_ssr_test_xor_check(
           get_default_unit(),
           FALSE
         );
  soc_sand_os_printf( "SOC_SAND_FAP21V SSR test %s\n", (pas1 && pas2 ? "pass" : "fail"));

  goto exit;
exit:
  return ui_ret;
}

int
  ui_fap21v_app_scheme(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_fap21v_app_scheme");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_APP_SCHEDULING_SCHEMES_UPDATE_ID,1))
  {
    ui_ret = ui_fap21v_app_update(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_APP_SCHEDULING_SCHEMES_GENERAL_UPDATE_ID,1))
  {
    ui_ret = ui_fap21v_app_gen_sch_update(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after scheme***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

int
  subject_fap21v_app(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  )
{
  UI_MACROS_INIT_FUNCTION("subject_fap21v_app");
  soc_sand_proc_name = "subject_fap21v_app";

  proc_name = "subject_fap21v_app" ;
  ui_ret = FALSE ;
  unit = 0;


  /*
   * the rest of the line is empty
   */
  if (current_line->num_param_names == 0)
  {
    send_string_to_screen("\r\n",FALSE) ;
    send_string_to_screen("'subject_fap21v_app()' function was called with no parameters.\r\n",FALSE) ;
    goto exit ;
  }
  /*
   * Else, there are parameters on the line (not just 'subject_fap21v_api').
   */

  send_array_to_screen("\r\n",2) ;

  if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_APP_SCHEDULING_SCHEMES_ID,1))
  {
    ui_ret = ui_fap21v_app_scheme(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_APP_SSR_ID,1))
  {
    ui_ret = ui_fap21v_app_ssr(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_APP_LINKS_ID,1))
  {
    ui_ret = ui_fap21v_app_links(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_FAP21V_APP_FE_ID,1))
  {
    ui_ret = ui_fap21v_app_fe(current_line);
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
