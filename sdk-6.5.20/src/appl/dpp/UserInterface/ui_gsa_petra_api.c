/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#if !DUNE_BCM

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

#include <appl/dpp/UserInterface/ui_pure_defi_gsa_petra_api.h>
#include <appl/dpp/UserInterface/ui_rom_defi_gsa_petra_api.h>

#include <FMC/fmc_common_agent.h>

#include <soc/dpp/Petra/petra_sw_db.h>

#include <DuneApp/gsa_api_multicast2.h>
#include <DuneApp/gsa_trans_layer.h>

#include <CSystem/csystem_mngmnt.h>


#define UI_MULTICAST

static
  uint32
    Default_unit = 0;

void
  gsa_petra_set_default_unit(uint32 dev_id)
{
  Default_unit = dev_id;
}

uint32
  gsa_petra_get_default_unit()
{
  return Default_unit;
}
#ifdef UI_MULTICAST
/********************************************************************
 *  Function handler: gsa_petra_mc_grp_open (section multicast)
 */
int
  ui_gsa_petra_api_multicast_gsa_petra_mc_grp_open(
    CURRENT_LINE *current_line
  )
{
  uint32
    param_i,
    ret;
  uint32
    prm_mc_id = 0;
  GSA_MC_TYPE
    prm_mc_type;
  uint32
    prm_mc_grp_size;
  GSA_DEST_SYS_PORT_INFO
    prm_mc_grp_members[UI_MC_GRP_MEMBERS_MAX];
  uint32
    prm_per_member_cud[UI_MC_GRP_MEMBERS_MAX];

  UI_MACROS_INIT_FUNCTION("ui_gsa_petra_api_multicast");
  soc_sand_proc_name = "gsa_petra_mc_grp_open";

  unit = gsa_petra_get_default_unit();

  /* Initialization */
  for (param_i=1; param_i<=UI_MC_GRP_MEMBERS_MAX; ++param_i)
  {
    prm_per_member_cud[param_i - 1] = 0;
    gsa_GSA_DEST_SYS_PORT_INFO_clear(&(prm_mc_grp_members[param_i - 1]));
  }

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_PETRA_MC_GRP_OPEN_MC_GRP_OPEN_MC_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_PETRA_MC_GRP_OPEN_MC_GRP_OPEN_MC_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_PETRA_MC_GRP_OPEN_MC_GRP_OPEN_MC_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_GSA_PETRA_MC_GRP_OPEN_MC_GRP_OPEN_MC_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_type = param_val->numeric_equivalent;
  }

  for (param_i=1; param_i<=UI_MC_GRP_MEMBERS_MAX; ++param_i)
  {
    ret = search_param_val_pairs(current_line,&match_index,PARAM_GSA_PETRA_MC_GRP_OPEN_MC_GRP_OPEN_MC_GRP_MEMBERS_ID,param_i);
    if (0 != ret)
    {
      ret=0;
      break;
    }
    UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(PARAM_GSA_PETRA_MC_GRP_OPEN_MC_GRP_OPEN_MC_GRP_MEMBERS_ID,param_i);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_grp_members[param_i-1].type = SOC_PETRA_DEST_SYS_PORT_TYPE_SYS_PHY_PORT;
    prm_mc_grp_members[param_i-1].id = param_val->value.ulong_value;
  }

  prm_mc_grp_size = param_i - 1;

  for (param_i=1; param_i<=prm_mc_grp_size; ++param_i)
  {
    ret = search_param_val_pairs(current_line,&match_index,PARAM_GSA_PETRA_MC_GRP_OPEN_MC_GRP_OPEN_PER_MEMBER_CUD_ID,param_i);
    if (0 != ret)
    {
      ret=0;
      break;
    }
    UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(PARAM_GSA_PETRA_MC_GRP_OPEN_MC_GRP_OPEN_PER_MEMBER_CUD_ID,param_i);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_per_member_cud[param_i-1] = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = gsa_mc_grp_open(
          prm_mc_id,
          prm_mc_type,
          prm_mc_grp_size,
          prm_mc_grp_members,
          prm_per_member_cud
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** gsa_mc_grp_open - FAIL", TRUE);
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: gsa_petra_mc_grp_close (section multicast)
 */
int
  ui_gsa_petra_api_multicast_gsa_petra_mc_grp_close(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  GSA_MC_TYPE
    prm_mc_type;
  uint32
    prm_mc_id;

  UI_MACROS_INIT_FUNCTION("ui_gsa_petra_api_multicast");
  soc_sand_proc_name = "gsa_petra_mc_grp_close";

  unit = gsa_petra_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_PETRA_MC_GRP_CLOSE_MC_GRP_CLOSE_MC_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_GSA_PETRA_MC_GRP_CLOSE_MC_GRP_CLOSE_MC_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_type = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_PETRA_MC_GRP_CLOSE_MC_GRP_CLOSE_MC_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_PETRA_MC_GRP_CLOSE_MC_GRP_CLOSE_MC_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_id = (uint32)param_val->value.ulong_value;
  }


  /* Call function */
  ret = gsa_mc_grp_close(
          prm_mc_type,
          prm_mc_id
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** gsa_mc_grp_close - FAIL", TRUE);
    goto exit;
  }


  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
 *  Function handler: gsa_mc_grp_get (section multicast)
 */
int
  ui_gsa_api_multicast_gsa_mc_grp_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    prm_mc_id;
  GSA_MC_TYPE
    prm_mc_type = SWP_P_MC_GET_TYPE_ALL;

  UI_MACROS_INIT_FUNCTION("ui_gsa_api_multicast");
  soc_sand_proc_name = "gsa_mc_grp_get";

  unit = gsa_petra_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_PETRA_MC_GRP_GET_MC_GRP_GET_MC_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_PETRA_MC_GRP_GET_MC_GRP_GET_MC_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_PETRA_MC_GRP_GET_MC_GRP_GET_MC_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_GSA_PETRA_MC_GRP_GET_MC_GRP_GET_MC_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mc_type = param_val->numeric_equivalent;
  }

  /* Call function */
  ret = gsa_petra_api_multicast_group_print(
          unit,
          prm_mc_id,
          prm_mc_type
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** gsa_petra_api_multicast_group_print - FAIL", TRUE);
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

#endif

#ifdef UI_SCHEMES
/********************************************************************
*  Function handler: ui_gsa_petra_api_gen_sch_update (section schemes)
 */
int
  ui_gsa_petra_api_schemes_gen_sch_update(
    CURRENT_LINE *current_line
  )
{
  int
    err;
  uint32
    sys_port_ndx = 0,
    port_ndx_u32 = 0,
    dsp_fap_u32 = 0,
    ret;
  SWP_P_TM_SCH_HR_CLASS
    sp_class = SWP_P_TM_SCH_FLOW_HR_CLASS_1;
  uint32
    dsp_fap = 0,
    nof_faps = 0,
    tmp_dsp_fap = 0,
    port_ndx = 0,
    class_of_service_ndx = 0,
    weight = 2,
    tmp_port_ndx = 0,
    nof_tmp_port_ndx = 0,
    tmp_class_of_service_ndx = 0,
    nof_tmp_class_of_service_ndx = 0,
    cir_by_percentage = 0,
    eir_by_percentage = 100,
    sys_devices[GSA_MAX_NOF_DEVICES],
    nof_devices;
  FMC_COMMON_IN_STRUCT
    in_struct;
  FMC_COMMON_OUT_STRUCT
    out_struct ;
  uint8
    is_sys_device = FALSE;

  UI_MACROS_INIT_FUNCTION("ui_gsa_petra_api_schemes_gen_sch_update");
  soc_sand_proc_name = "ui_gsa_petra_api_schemes_gen_sch_update";

  unit = gsa_petra_get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_PETRA_SCH_SCHEMES_GENERAL_UPDATE_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUM_SYM_VAL(PARAM_GSA_PETRA_SCH_SCHEMES_GENERAL_UPDATE_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    UI_MACROS_LOAD_LONG_VAL(sys_port_ndx);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter port_ndx after update***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_PETRA_SCH_SCHEMES_GENERAL_UPDATE_CLASS_NDX_ID,1))
  {
    UI_MACROS_GET_NUM_SYM_VAL(PARAM_GSA_PETRA_SCH_SCHEMES_GENERAL_UPDATE_CLASS_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    UI_MACROS_LOAD_LONG_VAL(class_of_service_ndx);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter nominal_rate_mbps after port_ndx***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_PETRA_SCH_SCHEMES_GENERAL_UPDATE_WEIGHT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_PETRA_SCH_SCHEMES_GENERAL_UPDATE_WEIGHT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    weight = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_PETRA_SCH_SCHEMES_GENERAL_UPDATE_SWP_P_TM_SCH_HR_CLASS_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_GSA_PETRA_SCH_SCHEMES_GENERAL_UPDATE_SWP_P_TM_SCH_HR_CLASS_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    sp_class = (SWP_P_TM_SCH_HR_CLASS)param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_PETRA_SCH_SCHEMES_GENERAL_UPDATE_MIN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_PETRA_SCH_SCHEMES_GENERAL_UPDATE_MIN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    cir_by_percentage = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_PETRA_SCH_SCHEMES_GENERAL_UPDATE_MAX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_PETRA_SCH_SCHEMES_GENERAL_UPDATE_MAX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    eir_by_percentage = (uint32)param_val->value.ulong_value;
  }

  if (sys_port_ndx == GSA_MAX_NOF_SYS_PORTS)
  {
    ret = soc_petra_aqfm_nof_relative_port_get(
            unit,
            &nof_tmp_port_ndx
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** soc_petra_aqfm_nof_relative_port_get - FAIL", TRUE);
      soc_petra_disp_result(ret, "soc_petra_aqfm_nof_relative_port_get");
      goto exit;
    }

    ret = gsa_trans_get_sys_devices(
            SOC_SAND_DEV_PETRA,
            sys_devices,
            &nof_devices
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** gsa_trans_get_sys_devices - FAIL", TRUE);
      soc_petra_disp_result(ret, "gsa_trans_get_sys_devices");
      goto exit;
    }

    nof_faps = nof_devices;
    is_sys_device = TRUE;

  }
  else
  {
    ret = gsa_sys_info_port_sys_to_local_get(
            unit,
            sys_port_ndx,
            &port_ndx_u32
          );
    port_ndx = (uint32) port_ndx_u32;

    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** gsa_sys_info_port_sys_to_local_get - FAIL", TRUE);
      soc_petra_disp_result(ret, "gsa_sys_info_port_sys_to_local_get");
      goto exit;
    }

    nof_tmp_port_ndx = 1 ;
    nof_faps = 1;
  }

  if (class_of_service_ndx == SOC_PETRA_NOF_TRAFFIC_CLASSES)
  {
    nof_tmp_class_of_service_ndx = SOC_PETRA_NOF_TRAFFIC_CLASSES ;
  }
  else
  {
    nof_tmp_class_of_service_ndx = 1 ;
  }

  for (tmp_port_ndx = 0; tmp_port_ndx < nof_tmp_port_ndx ; ++tmp_port_ndx)
  {
    if (nof_tmp_port_ndx != 1)
    {
      ret = soc_petra_aqfm_port_relative2actual_get(
              gsa_petra_get_default_unit(),
              tmp_port_ndx,
              &port_ndx
            );
      if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
      {
        send_string_to_screen(" *** soc_petra_aqfm_port_relative2actual_get - FAIL", TRUE);
        soc_petra_disp_result(ret, "soc_petra_aqfm_port_relative2actual_get");
        goto exit;
      }
    }

    for (tmp_class_of_service_ndx = 0;
      tmp_class_of_service_ndx < nof_tmp_class_of_service_ndx ;
      ++tmp_class_of_service_ndx)
    {
      if (nof_tmp_class_of_service_ndx != 1)
      {
        class_of_service_ndx = tmp_class_of_service_ndx ;
      }

      for (tmp_dsp_fap = 0; tmp_dsp_fap < nof_faps ; ++tmp_dsp_fap)
      {
        if (is_sys_device)
        {
          ret = gsa_sys_device_to_fap_id_get(
                  sys_devices[tmp_dsp_fap],
                  &dsp_fap
                 );
          if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
          {
            send_string_to_screen(" *** gsa_sys_device_to_fap_id_get - FAIL", TRUE);
            soc_petra_disp_result(ret, "gsa_sys_device_to_fap_id_get");
            goto exit;
          }
        }
        else
        {
          ret = gsa_sys_info_port_sys_to_fap_id_get(
                  sys_port_ndx,
                  &dsp_fap_u32
                );
          dsp_fap = dsp_fap_u32;
          if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
          {
            send_string_to_screen(" *** gsa_sys_info_port_sys_to_fap_id_get - FAIL", TRUE);
            soc_petra_disp_result(ret, "gsa_sys_info_port_sys_to_fap_id_get");
            goto exit;
          }
        }

        in_struct.out_struct_type = SOC_PETRA_STRUCTURE_TYPE ;
        in_struct.common_union.soc_petra_struct.proc_id = SOC_PETRA_AGENT_SCH_GEN_SCHEME_UPDATE ;
        in_struct.common_union.soc_petra_struct.data.scm_info_general_update_struct.unit = csys_mngment_local_get_unit(SOC_SAND_DEV_PETRA, dsp_fap)  ;
        in_struct.common_union.soc_petra_struct.data.scm_info_general_update_struct.port_ndx = port_ndx;
        in_struct.common_union.soc_petra_struct.data.scm_info_general_update_struct.class_of_service_ndx = class_of_service_ndx;
        in_struct.common_union.soc_petra_struct.data.scm_info_general_update_struct.sp_class = sp_class;
        in_struct.common_union.soc_petra_struct.data.scm_info_general_update_struct.weight = weight;
        in_struct.common_union.soc_petra_struct.data.scm_info_general_update_struct.cir_by_percentage = cir_by_percentage;
        in_struct.common_union.soc_petra_struct.data.scm_info_general_update_struct.eir_by_percentage = eir_by_percentage;

        err = fmc_send_buffer_to_device(
                SOC_SAND_DEV_PETRA,
                dsp_fap,
                TRUE,
                sizeof(in_struct),
                (uint8*)&in_struct,
                sizeof(out_struct),
                (uint8*)&out_struct
              );
        if(err)
        {
          send_string_to_screen(" *** fmc_send_buffer_to_device - FAIL", TRUE);
          soc_petra_disp_result(err, "fmc_send_buffer_to_device");
          goto exit;
        }
      }
    }
  }


exit:
  return ui_ret;
}


int
  ui_gsa_petra_api_schemes_gen_sch_get(
    CURRENT_LINE *current_line
  )
{
  uint32
    dsp_fap_u32 = 0,
    sys_port_ndx = 0,
    port_ndx_u32 = 0,
    ret;
  uint32
    dsp_fap = 0,
    nof_faps = 0,
    tmp_dsp_fap = 0,
    port_ndx = 0,
    tmp_port_ndx = 0,
    nof_tmp_port_ndx = 0,
    class_of_service_ndx = 0,
    tmp_class_of_service_ndx = 0,
    nof_tmp_class_of_service_ndx = 0,
    weight = 0,
    cir_by_percentage = 0,
    eir_by_percentage = 0,
    sys_devices[GSA_MAX_NOF_DEVICES],
    nof_devices;
  FMC_COMMON_IN_STRUCT
    in_struct;
  FMC_COMMON_OUT_STRUCT
    out_struct ;
  int
    err;
  SWP_P_TM_SCH_HR_CLASS
    sp_class = SWP_P_TM_SCH_FLOW_HR_CLASS_1;
  uint8
    is_sys_device = FALSE;

  UI_MACROS_INIT_FUNCTION("ui_gsa_petra_api_schemes_gen_sch_get");
  soc_sand_proc_name = "ui_gsa_petra_api_schemes_gen_sch_get";

  unit = gsa_petra_get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_PETRA_SCH_SCHEMES_GENERAL_GET_PORT_NDX_ID,1))
  {
    UI_MACROS_GET_NUM_SYM_VAL(PARAM_GSA_PETRA_SCH_SCHEMES_GENERAL_GET_PORT_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    UI_MACROS_LOAD_LONG_VAL(sys_port_ndx);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter port_ndx after get***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_PETRA_SCH_SCHEMES_GENERAL_GET_CLASS_NDX_ID,1))
  {
    UI_MACROS_GET_NUM_SYM_VAL(PARAM_GSA_PETRA_SCH_SCHEMES_GENERAL_GET_CLASS_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    UI_MACROS_LOAD_LONG_VAL(class_of_service_ndx);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter nominal_rate_mbps after port_ndx***", TRUE);
    goto exit;
  }


  if (sys_port_ndx == GSA_MAX_NOF_SYS_PORTS)
  {
    ret = soc_petra_aqfm_nof_relative_port_get(
            unit,
            &nof_tmp_port_ndx
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** soc_petra_aqfm_nof_relative_port_get - FAIL", TRUE);
      soc_petra_disp_result(ret, "soc_petra_aqfm_nof_relative_port_get");
      goto exit;
    }

    ret = gsa_trans_get_sys_devices(
            SOC_SAND_DEV_PETRA,
            sys_devices,
            &nof_devices
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** gsa_trans_get_sys_devices - FAIL", TRUE);
      soc_petra_disp_result(ret, "gsa_trans_get_sys_devices");
      goto exit;
    }

    nof_faps = nof_devices;
    is_sys_device = TRUE;
  }
  else
  {
    ret = gsa_sys_info_port_sys_to_local_get(
            unit,
            sys_port_ndx,
            &port_ndx_u32
          );
    port_ndx = port_ndx_u32;

    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      send_string_to_screen(" *** gsa_sys_info_port_sys_to_local_get - FAIL", TRUE);
      soc_petra_disp_result(ret, "gsa_sys_info_port_sys_to_local_get");
      goto exit;
    }

    nof_tmp_port_ndx = 1 ;
    nof_faps = 1;
  }

  if (class_of_service_ndx == SOC_PETRA_NOF_TRAFFIC_CLASSES)
  {
    nof_tmp_class_of_service_ndx = SOC_PETRA_NOF_TRAFFIC_CLASSES ;
  }
  else
  {
    nof_tmp_class_of_service_ndx = 1 ;
  }

  for (tmp_port_ndx = 0; tmp_port_ndx < nof_tmp_port_ndx ; ++tmp_port_ndx)
  {
    if (nof_tmp_port_ndx != 1)
    {
      ret = soc_petra_aqfm_port_relative2actual_get(
              unit,
              tmp_port_ndx,
              &port_ndx
            );
      if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
      {
        send_string_to_screen(" *** soc_petra_aqfm_port_relative2actual_get - FAIL", TRUE);
        soc_petra_disp_result(ret, "soc_petra_aqfm_port_relative2actual_get");
        goto exit;
      }
    }


    for (tmp_class_of_service_ndx = 0;
         tmp_class_of_service_ndx < nof_tmp_class_of_service_ndx ;
         ++tmp_class_of_service_ndx)
    {
      if (nof_tmp_class_of_service_ndx != 1)
      {
        class_of_service_ndx = tmp_class_of_service_ndx ;
      }

      for (tmp_dsp_fap = 0; tmp_dsp_fap < nof_faps ; ++tmp_dsp_fap)
      {
        if (is_sys_device)
        {
          ret = gsa_sys_device_to_fap_id_get(
                  sys_devices[tmp_dsp_fap],
                  &dsp_fap
                 );
          if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
          {
            send_string_to_screen(" *** gsa_sys_device_to_fap_id_get - FAIL", TRUE);
            soc_petra_disp_result(ret, "gsa_sys_device_to_fap_id_get");
            goto exit;
          }
        }
        else
        {
          ret = gsa_sys_info_port_sys_to_fap_id_get(
                  sys_port_ndx,
                  &dsp_fap_u32
                );
          dsp_fap = dsp_fap_u32;

          if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
          {
            send_string_to_screen(" *** gsa_sys_info_port_sys_to_fap_id_get - FAIL", TRUE);
            soc_petra_disp_result(ret, "gsa_sys_info_port_sys_to_fap_id_get");
            goto exit;
          }
        }
        in_struct.out_struct_type = SOC_PETRA_STRUCTURE_TYPE ;
        in_struct.common_union.soc_petra_struct.proc_id = SOC_PETRA_AGENT_SCH_GEN_SCHEME_GET ;
        in_struct.common_union.soc_petra_struct.data.scm_info_general_get_struct.unit = csys_mngment_local_get_unit(SOC_SAND_DEV_PETRA, dsp_fap)  ;
        in_struct.common_union.soc_petra_struct.data.scm_info_general_get_struct.port_ndx = port_ndx;
        in_struct.common_union.soc_petra_struct.data.scm_info_general_get_struct.class_of_service_ndx = class_of_service_ndx;

        err = fmc_send_buffer_to_device(
                SOC_SAND_DEV_PETRA,
                dsp_fap,
                TRUE,
                sizeof(in_struct),
                (uint8*)&in_struct,
                sizeof(out_struct),
                (uint8*)&out_struct
              );
        if(err)
        {
          send_string_to_screen(" *** ui_gsa_petra_api_gen_sch_get - FAIL", TRUE);
          soc_petra_disp_result(err, "ui_gsa_petra_api_gen_sch_get");
          goto exit;
        }
        cir_by_percentage = (out_struct.common_union.soc_petra_struct.data.sch_scheme_info_struct.cir_by_percentage);
        eir_by_percentage = (out_struct.common_union.soc_petra_struct.data.sch_scheme_info_struct.eir_by_percentage);
        sp_class = (out_struct.common_union.soc_petra_struct.data.sch_scheme_info_struct.sp_class);
        weight = (out_struct.common_union.soc_petra_struct.data.sch_scheme_info_struct.weight);

        soc_sand_os_printf( "Fap: %d, Sys.port %d, Class %d: \n\r", dsp_fap, port_ndx, class_of_service_ndx);
        soc_sand_os_printf( "  CIR: %d[%%], EIR %d[%%]", cir_by_percentage, eir_by_percentage);
        if (weight == 0)
        {
          soc_sand_os_printf( " and Strict Priority Class: %s \n\r", soc_petra_SWP_P_TM_SCH_HR_CLASS_to_string(sp_class));
        }
        else
        {
          soc_sand_os_printf( ", Strict Priority Class: %s and Weight: %d \n\r", soc_petra_SWP_P_TM_SCH_HR_CLASS_to_string(sp_class), weight);
        }
      }
    }
  }

exit:
  return ui_ret;
}
#endif/* {schemes */



#ifdef UI_LINKS/* { links*/
/********************************************************************
*  Section handler: links
 */
int
  ui_gsa_petra_api_connectlivity_map_get(
    uint32 unit,
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

  UI_MACROS_INIT_FUNCTION("ui_gsa_petra_api_update");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_PETRA_CONNECTIVITY_MAP_GET_FAP_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_PETRA_CONNECTIVITY_MAP_GET_FAP_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    dst_fap = (uint32)param_val->value.ulong_value;
  }

  in_struct.out_struct_type = SOC_PETRA_STRUCTURE_TYPE ;
  in_struct.common_union.soc_petra_struct.proc_id = SOC_PETRA_AGENT_FABRIC_LINKS_CONNECTIVITY_MAP_GET;

  in_struct.common_union.soc_petra_struct.data.connectivity_map_struct.unit = csys_mngment_local_get_unit(SOC_SAND_DEV_PETRA, dst_fap) ;

  err = fmc_send_buffer_to_device(
          SOC_SAND_DEV_PETRA,
          dst_fap,
          TRUE,
          sizeof(in_struct),
          (uint8*)&in_struct,
          sizeof(out_struct),
          (uint8*)&out_struct
        );
  if(err)
  {
    send_string_to_screen(" *** ui_gsa_petra_api_connectlivity_map_get - FAIL", TRUE);
    soc_petra_disp_result(err, "ui_gsa_petra_api_connectlivity_map_get");
    goto exit;
  }

  soc_petra_PETRA_FABRIC_LINKS_CON_STAT_INFO_ARR_print(
    unit,
    &(out_struct.common_union.soc_petra_struct.data.connectivity_map_struct.connectivity_map),
    TRUE
  );

exit:
  return ui_ret;
}


int
  ui_gsa_petra_api_fe_serdes_power_down(
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

  UI_MACROS_INIT_FUNCTION("ui_gsa_petra_api_fe_serdes_power_down");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_PETRA_SERDES_POWER_DOWN_FE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_PETRA_SERDES_POWER_DOWN_FE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    dst_fap = param_val->value.ulong_value ;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_PETRA_SERDES_POWER_DOWN_SERDES_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_PETRA_SERDES_POWER_DOWN_SERDES_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    serdes_ndx = param_val->value.ulong_value ;
  }


  in_struct.out_struct_type = SOC_PETRA_STRUCTURE_TYPE ;
  in_struct.common_union.soc_petra_struct.proc_id = SOC_PETRA_AGENT_FABRIC_LINKS_POWER_DOWN_SET ;
  in_struct.common_union.soc_petra_struct.data.serdes_power_down_struct.unit = csys_mngment_local_get_unit(SOC_SAND_DEV_PETRA, dst_fap)  ;
  in_struct.common_union.soc_petra_struct.data.serdes_power_down_struct.link_ndx = serdes_ndx ;
  in_struct.common_union.soc_petra_struct.data.serdes_power_down_struct.shut_down = TRUE;

  err = fmc_send_buffer_to_device(
          SOC_SAND_DEV_PETRA,
          dst_fap,
          TRUE,
          sizeof(in_struct),
          (uint8*)&in_struct,
          sizeof(out_struct),
          (uint8*)&out_struct
        );
  if(err)
  {
    send_string_to_screen(" *** ui_gsa_petra_api_fe_serdes_power_down - FAIL", TRUE);
    soc_petra_disp_result(err, "ui_gsa_petra_api_fe_serdes_power_down");
    goto exit;
  }

  if (SOC_SAND_OK != soc_sand_get_error_code_from_error_word(out_struct.common_union.soc_petra_struct.error_id))
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
  ui_gsa_petra_api_fe_serdes_power_up(
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

  UI_MACROS_INIT_FUNCTION("ui_gsa_petra_api_fe_serdes_power_up");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_PETRA_SERDES_POWER_UP_FE_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_PETRA_SERDES_POWER_UP_FE_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    dst_fap = param_val->value.ulong_value ;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_PETRA_SERDES_POWER_UP_SERDES_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GSA_PETRA_SERDES_POWER_UP_SERDES_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    serdes_ndx = param_val->value.ulong_value ;
  }

  in_struct.out_struct_type = SOC_PETRA_STRUCTURE_TYPE ;
  in_struct.common_union.soc_petra_struct.proc_id = SOC_PETRA_AGENT_FABRIC_LINKS_POWER_DOWN_SET ;
  in_struct.common_union.soc_petra_struct.data.serdes_power_down_struct.unit = csys_mngment_local_get_unit(SOC_SAND_DEV_PETRA, dst_fap)  ;
  in_struct.common_union.soc_petra_struct.data.serdes_power_down_struct.link_ndx = serdes_ndx ;
  in_struct.common_union.soc_petra_struct.data.serdes_power_down_struct.shut_down = FALSE ;

  err = fmc_send_buffer_to_device(
          SOC_SAND_DEV_PETRA,
          dst_fap,
          TRUE,
          sizeof(in_struct),
          (uint8*)&in_struct,
          sizeof(out_struct),
          (uint8*)&out_struct
        );
  if(err)
  {
    send_string_to_screen(" *** ui_gsa_petra_api_fe_serdes_power_up - FAIL", TRUE);
    soc_petra_disp_result(err, "ui_gsa_petra_api_fe_serdes_power_up");
    goto exit;
  }

  if (SOC_SAND_OK != soc_sand_get_error_code_from_error_word(out_struct.common_union.soc_petra_struct.error_id))
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
  ui_gsa_petra_api_fe_serdes_power(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_gsa_petra_api_fe_serdes_power");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_PETRA_SERDES_POWER_DOWN_ID,1))
  {
    ui_ret = ui_gsa_petra_api_fe_serdes_power_down(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_PETRA_SERDES_POWER_UP_ID,1))
  {
    ui_ret = ui_gsa_petra_api_fe_serdes_power_up(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after serdes_power***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* {links */

#ifdef UI_MULTICAST/* { multicast*/
/********************************************************************
 *  Section handler: multicast
 */
int
  ui_gsa_petra_api_multicast(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_gsa_petra_api_multicast");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_PETRA_MC_GRP_OPEN_MC_GRP_OPEN_ID,1))
  {
    ui_ret = ui_gsa_petra_api_multicast_gsa_petra_mc_grp_open(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_PETRA_MC_GRP_CLOSE_MC_GRP_CLOSE_ID,1))
  {
    ui_ret = ui_gsa_petra_api_multicast_gsa_petra_mc_grp_close(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_PETRA_MC_GRP_GET_MC_GRP_GET_ID,1))
  {
    ui_ret = ui_gsa_api_multicast_gsa_mc_grp_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after multicast***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* {multicast */


#ifdef UI_SCHEMES/* { schemes*/
/********************************************************************
*  Section handler: schemes
 */

int
  ui_gsa_petra_api_scheme(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_gsa_petra_api_scheme");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_PETRA_SCH_SCHEMES_GENERAL_UPDATE_ID,1))
  {
    ui_ret = ui_gsa_petra_api_schemes_gen_sch_update(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_PETRA_SCH_SCHEMES_GENERAL_GET_ID,1))
  {
    ui_ret = ui_gsa_petra_api_schemes_gen_sch_get(current_line);
  }

  else
  {
    send_string_to_screen(" *** SW error - expecting function name after scheme***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif/* {schemes */

#ifdef UI_LINKS/* { links*/
/********************************************************************
*  Section handler: links
 */

int
  ui_gsa_petra_api_links(
    SOC_SAND_IN uint32 unit,
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_gsa_petra_api_links");

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_PETRA_SERDES_POWER_ID,1))
  {
    ui_ret = ui_gsa_petra_api_fe_serdes_power(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_PETRA_CONNECTIVITY_MAP_GET_ID,1))
  {
    ui_ret = ui_gsa_petra_api_connectlivity_map_get(unit,current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after links***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}
#endif/* {links */


/*****************************************************
*NAME
*  subject_gsa_petra_api
*TYPE: PROC
*DATE: 29/DEC/2002
*FUNCTION:
*  Process input line which has an 'subject_gsa_petra_api' subject.
*  Input line is assumed to have been checked and
*  found to be of right format.
*CALLING SEQUENCE:
*  subject_gsa_petra_api(current_line,current_line_ptr)
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
  subject_gsa_petra_api(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  )
{
  unsigned int
    match_index;
  int
    ui_ret;
  char
    err_msg[80*8] = "" ;
  unsigned int
    ;
  char
    *proc_name ;


  proc_name = "subject_gsa_petra_api" ;
  ui_ret = FALSE ;
  unit = 0;


  /*
   * the rest of the line is empty
   */
  if (current_line->num_param_names == 0)
  {
    send_string_to_screen("\r\n",FALSE) ;
    send_string_to_screen("'subject_gsa_petra_api()' function was called with no parameters.\r\n",FALSE) ;
    goto exit ;
  }
  /*
   * Else, there are parameters on the line (not just 'subject_gsa_petra_api').
   */

  send_array_to_screen("\r\n",2) ;

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_PETRA_API_MULTICAST_ID,1))
  {
    ui_ret = ui_gsa_petra_api_multicast(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_PETRA_SCH_SCHEMES_ID,1))
  {
    ui_ret = ui_gsa_petra_api_scheme(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GSA_PETRA_LINKS_ID,1))
  {
    ui_ret = ui_gsa_petra_api_links(unit, current_line);
  }
  else
  {
    /*
     * Enter if an unknown request.
     */
    send_string_to_screen(
      "\r\n"
      "*** gsa_petra_api command with unknown parameters'.\r\n"
      "    Syntax error/sw error...\r\n",
      TRUE)  ;
    ui_ret = TRUE  ;
    goto exit  ;
  }

exit:
  return (ui_ret);
}

#else
int ui_gsa_petra_api = 0;
#endif

