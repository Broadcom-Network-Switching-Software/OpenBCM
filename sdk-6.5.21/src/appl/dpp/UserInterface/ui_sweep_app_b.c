/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/* we need this junk function only to avoid building error of pedantic compilation */
void ___junk_function_ui_sweep_app(void){
}

#if LINK_FAP20V_LIBRARIES

#ifdef _MSC_VER
  /*
   * On MS-Windows platform this attribute is not defined.
   */
  #define __ATTRIBUTE_PACKED__
  #pragma pack(push)
  #pragma pack(1)

#elif __GNUC__
    /*
     * GNUC packing attribute
     */
    #define __ATTRIBUTE_PACKED__  __attribute__ ((packed))
#else
    #error  "Add your system support for packed attribute."
#endif

/*************
 * INCLUDES  *
 */
/* { */
#include <bcm_app/dpp/UserInterface/ui_sweep_app_b.h>
#include <bcm_app/dpp/UserInterface/ui_sweep_app.h>

#include <appl/dpp/UserInterface/ui_defi.h>
#include <appl/dpp/UserInterface/ui_pure_defi_sweep_app.h>

#include <FMC/fmc_transport_layer.h>
#include <FMC/fmc_transport_layer_dcl.h>

#include <soc/dpp/SOC_SAND_FAP20V/fap20v_api_links.h>
#include <soc/dpp/SOC_SAND_FAP20V/fap20v_api_auto_flow_management.h>
#include <soc/dpp/SOC_SAND_FAP20V/fap20v_api_links.h>

#include <sweep/fap20v/sweep_fap20v_app.h>
#include <sweep/fap20v/sweep_fap20v_db.h>
#include <sweep/fap20v/sweep_fap20v_qos.h>

#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_workload_status.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>

#include <CSystem/csystem_mngmnt.h>

/* } */

/*************
 * DEFINES   *
 */
/* { */

/* } */

/*************
 *  MACROS   *
 */
/* { */

/* } */

/*************
 * TYPE DEFS *
 */
/* { */

/* } */

/*************
 * GLOBALS   *
 */
/* { */

/* } */

/*************
 * FUNCTIONS *
 */
/* { */



/*****************************************************
*NAME
*  process_sweep_app_line_b
 */
int process_sweep_app_line_b(CURRENT_LINE *current_line, CURRENT_LINE **current_line_ptr)
{
  int
    ui_ret;
  PARAM_VAL
    *param_val;
  unsigned int
    match_index,
    err,
    absolute_dev_id;
  char
    err_msg[80*6] = "",
    *proc_name;
  BOOL
    get_val_of_err;
  FMC_COMMON_OUT_STRUCT
    out_struct ;
  FMC_COMMON_IN_STRUCT
    in_struct ;
  unsigned long
    dst_fap,
    dst_port,
    fe_to_access,
    driver_ret;
  unsigned int
    silent;
  SOC_SAND_DEVICE_ENTITY
    entity_type = SOC_SAND_DONT_CARE_ENTITY;

  enum
  {
    NO_ERR = 0,
    ERR_001, ERR_002, ERR_003, ERR_004, ERR_005,
    ERR_006, ERR_007, ERR_008, ERR_009,
    NOF_ERR
  };


  proc_name = "process_sweep_app_line_b" ;
  ui_ret = 0 ;
  get_val_of_err = FALSE;
  send_string_to_screen("",TRUE);
  driver_ret = SOC_SAND_OK;

  absolute_dev_id = TRUE;


  if (!search_param_val_pairs(current_line,&match_index,PARAM_SWEEP_QOS_ID,1))
  {
    /*
     * the command "get/set_QOS_params"
     */
    SWEEP_FAP20V_B_SCH_SCHEME_INFO
      qos_sch;
    swp_clear_SWP_QOS_SCH(&qos_sch);

    UI_MACROS_GET_SYMBOL_VAL(PARAM_SWEEP_VERBOSE_ID);
    SWEEP_UI_REPORT_SW_ERR_AND_EXIT(ERR_001);
    UI_MACROS_LOAD_LONG_VAL(silent);
    silent = !silent;

    UI_MACROS_GET_NUM_SYM_VAL(PARAM_SWEEP_APP_FAP_DEVICE_ID);
    SWEEP_UI_REPORT_SW_ERR_AND_EXIT(ERR_002);
    UI_MACROS_LOAD_LONG_VAL(dst_fap)

    UI_MACROS_GET_NUM_SYM_VAL(PARAM_SWEEP_APP_DESTINATION_PORT_ID)
    SWEEP_UI_REPORT_SW_ERR_AND_EXIT(ERR_003);
    UI_MACROS_LOAD_LONG_VAL(dst_port)

    if (!search_param_val_pairs(current_line,&match_index,PARAM_SWEEP_QOS_GET_ID,1))
    {/*Get*/
      err = swp_qos_sys_get(dst_fap, dst_port, &qos_sch, silent);
      if( !err )
      {
        swp_print_SWP_QOS_SCH(&qos_sch, silent);
      }
    }
    else
    {/*Set*/

      UI_MACROS_GET_SYMBOL_VAL(PARAM_SWEEP_QOS_SCH_ID);
      SWEEP_UI_REPORT_SW_ERR_AND_EXIT(ERR_004);
      UI_MACROS_LOAD_LONG_VAL(qos_sch.scheme_id);
      SWEEP_UI_REPORT_SW_ERR_AND_EXIT(ERR_006);
      UI_MACROS_LOAD_LONG_VAL(qos_sch.maximum_rate_by_percentage);
      SWEEP_UI_REPORT_SW_ERR_AND_EXIT(ERR_007);
      UI_MACROS_LOAD_LONG_VAL(qos_sch.minimum_rate_by_percentage);
      SWEEP_UI_REPORT_SW_ERR_AND_EXIT(ERR_008);
      UI_MACROS_LOAD_LONG_VAL(qos_sch.support_low_latency);
      SWEEP_UI_REPORT_SW_ERR_AND_EXIT(ERR_009);

      err = swp_qos_sys_set(dst_fap, dst_port, &qos_sch, silent);
    }

    sweep_ui_report_driver_err(driver_ret);
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_SWEEP_APP_GET_CONNECTIVITY_MAP_ID,1))
  {
    FAP20V_ALL_LINK_CONNECTIVITY
      *connectivity_map=NULL ;
    unsigned int
      link_i;
    const char
      *entity_str=NULL;

    /**/
    ui_ret = get_val_of( current_line,(int *)&match_index,
                      PARAM_SWEEP_APP_FAP_DEVICE_ID,1,
                      &param_val,VAL_NUMERIC,err_msg
                    ) ;
    if (0 != ui_ret)
    {
      sal_sprintf(err_msg,
              "\r\n\n"
              "*** \'get connectivity map\' error \r\n"
              "*** or \'unknown parameter \'.\r\n"
              "    Probably SW error\r\n"
      ) ;
      send_string_to_screen(err_msg,TRUE) ;
      ui_ret = TRUE ;
      goto exit ;
    }

    dst_fap = param_val->value.ulong_value ;

    in_struct.out_struct_type = FAP20V_STRUCTURE_TYPE ;
    in_struct.common_union.fap20v_struct.proc_id = FAP20V_GET_CONNECTIVITY_MAP ;
    in_struct.common_union.fap20v_struct.data.just_unit_data.unit = csys_mngment_local_get_unit(SOC_SAND_DEV_FAP20V,dst_fap) ;

    err = fmc_send_buffer_to_device(
            SOC_SAND_DEV_FAP20V,
            dst_fap,
            absolute_dev_id,
            sizeof(in_struct),
            (unsigned char*)&in_struct,
            sizeof(out_struct),
            (unsigned char*)&out_struct
          );
    if(err)
    {
      sal_sprintf(err_msg,
              "\r\n\n"
              "*** 'get connectivity map' error = %d. \r\n",
              err
      ) ;
      send_string_to_screen(err_msg,TRUE) ;
      ui_ret = TRUE ;
      goto exit ;
    }

    connectivity_map = &out_struct.common_union.fap20v_struct.data.links_connectivity_data.all_link_connectivity ;
    soc_sand_os_printf( "\r\n");
    soc_sand_os_printf( " #|other| other|other|accpt\r\n");
    soc_sand_os_printf( " #| id  | type |link |cell \r\n");
    soc_sand_os_printf( "--+-----+------+-----+-----\r\n") ;
    for(link_i = 0 ; link_i < FAP20V_NUMBER_OF_LINKS ; ++link_i)
    {
      entity_type =
        soc_sand_entity_from_level(
          connectivity_map->single_link_connectivity[link_i].other_entity_type
        );

      entity_str =
        soc_sand_entity_enum_to_str(entity_type);

      if(connectivity_map->single_link_connectivity[link_i].accepting_cells == FALSE)
      {
        soc_sand_os_printf( "%2u| --- | ---  | --- |  -  \r\n", link_i) ;
      }
      else
      {
        if ( entity_type == SOC_SAND_FE2_ENTITY )
        {
          connectivity_map->single_link_connectivity[link_i].other_chip_id -= 1058;
        }
        soc_sand_os_printf( "%2u|%4u | %3s  | %02d  |  %c\r\n",
          link_i,
          connectivity_map->single_link_connectivity[link_i].other_chip_id,
          entity_str,
          connectivity_map->single_link_connectivity[link_i].other_link,
          (connectivity_map->single_link_connectivity[link_i].accepting_cells?'+':'-')
        );
      }
    }


  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_SWEEP_APP_POWER_DOWN_SERDES_ID,1))
  {
    if(
        search_param_val_pairs(current_line,&match_index,PARAM_SWEEP_APP_FE_DEVICE_ID,1)
      )
    {
      sal_sprintf(err_msg,
              "\r\n\n"
              "*** \'power down serdes\' error \r\n"
              "*** or \'expecting fap or fe id \'.\r\n"
              "    Probably SW error\r\n"
      ) ;
      send_string_to_screen(err_msg,TRUE) ;
      ui_ret = TRUE ;
      goto exit ;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_SWEEP_APP_FE_DEVICE_ID,1))
    {
      ui_ret = get_val_of( current_line,(int *)&match_index,
                        PARAM_SWEEP_APP_FE_DEVICE_ID,1,
                        &param_val,VAL_NUMERIC,err_msg
                      ) ;
      if (0 != ui_ret)
      {
        sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'power down serdes\' error \r\n"
                "*** or \'expecting fe id \'.\r\n"
                "    Probably SW error\r\n"
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
        ui_ret = TRUE ;
        goto exit ;
      }

      fe_to_access = param_val->value.ulong_value ;

      ui_ret = get_val_of( current_line,(int *)&match_index,
                        PARAM_SWEEP_APP_SERDES_ID,1,
                        &param_val,VAL_NUMERIC,err_msg
                      ) ;
      if (0 != ui_ret)
      {
        sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'power down serdes\' error \r\n"
                "*** or \'expecting serdes# \'.\r\n"
                "    Probably SW error\r\n"
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
        ui_ret = TRUE ;
        goto exit ;
      }
      in_struct.out_struct_type = FE200_STRUCTURE_TYPE ;
      in_struct.common_union.fe200_struct.proc_id = FE200_RESET_SINGLE_SERDES ;
      in_struct.common_union.fe200_struct.data.reset_serdes_data.unit = csys_mngment_local_get_unit(SOC_SAND_DEV_FE200, fe_to_access);
      in_struct.common_union.fe200_struct.data.reset_serdes_data.serdes_number = param_val->value.ulong_value ;
      in_struct.common_union.fe200_struct.data.reset_serdes_data.remain_active = TRUE ;

      err = fmc_send_buffer_to_device(
              SOC_SAND_DEV_FE200,
              fe_to_access,
              absolute_dev_id,
              sizeof(in_struct),
              (unsigned char*)&in_struct,
              sizeof(out_struct),
              (unsigned char*)&out_struct
            );
      if (err)
      {
        sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'power down serdes\' returned with error \r\n"
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      else
      {
        sal_sprintf(err_msg,
                "\r\n\n"
                "--> \'power down serdes\' succeeded \r\n"
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
    }
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_SWEEP_APP_POWER_UP_SERDES_ID,1))
  {
    if (search_param_val_pairs(current_line,&match_index,PARAM_SWEEP_APP_FAP_DEVICE_ID,1) &&
       search_param_val_pairs(current_line,&match_index,PARAM_SWEEP_APP_FE_DEVICE_ID,1)
      )
    {
      sal_sprintf(err_msg,
              "\r\n\n"
              "*** \'power up serdes\' error \r\n"
              "*** or \'expecting fap or fe id \'.\r\n"
              "    Probably SW error\r\n"
      ) ;
      send_string_to_screen(err_msg,TRUE) ;
      ui_ret = TRUE ;
      goto exit ;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_SWEEP_APP_FAP_DEVICE_ID,1))
    {
      ui_ret = get_val_of( current_line,(int *)&match_index,
                        PARAM_SWEEP_APP_FAP_DEVICE_ID,1,
                        &param_val,VAL_NUMERIC,err_msg
                      ) ;
      if (0 != ui_ret)
      {
        sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'power up serdes\' error \r\n"
                "*** or \'expecting fap id \'.\r\n"
                "    Probably SW error\r\n"
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
        ui_ret = TRUE ;
        goto exit ;
      }
      dst_fap = param_val->value.ulong_value;

      ui_ret = get_val_of( current_line,(int *)&match_index,
                        PARAM_SWEEP_APP_SERDES_ID,1,
                        &param_val,VAL_NUMERIC,err_msg
                      ) ;
      if (0 != ui_ret)
      {
        sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'power up serdes\' error \r\n"
                "*** or \'expecting serdes# \'.\r\n"
                "    Probably SW error\r\n"
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
        ui_ret = TRUE ;
        goto exit ;
      }
      if (FAP20V_NOF_FABRIC_LINKS <= param_val->value.ulong_value)
      {
        sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'power up serdes\' error \r\n"
                "*** or \'fap20v has 24 serdeses [0-23] \'.\r\n"
                "    Probably SW error\r\n"
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
        ui_ret = TRUE ;
        goto exit ;
      }
      in_struct.out_struct_type = FAP20V_STRUCTURE_TYPE ;
      in_struct.common_union.fap20v_struct.proc_id = FAP20V_SET_SERDES_POWER_DOWN ;
      in_struct.common_union.fap20v_struct.data.reset_serdes_data.unit = csys_mngment_local_get_unit(SOC_SAND_DEV_FAP20V,dst_fap)  ;
      in_struct.common_union.fap20v_struct.data.reset_serdes_data.serdes_number = param_val->value.ulong_value ;
      in_struct.common_union.fap20v_struct.data.reset_serdes_data.remain_active = FALSE ;

      err = fmc_send_buffer_to_device(
              SOC_SAND_DEV_FAP20V,
              dst_fap,
              absolute_dev_id,
              sizeof(in_struct),
              (unsigned char*)&in_struct,
              sizeof(out_struct),
              (unsigned char*)&out_struct
            );
      if(err)
      {
        sal_sprintf(err_msg,
                "\r\n\n"
                "*** 'power up serdes' error = %d. \r\n",
                err
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
        ui_ret = TRUE ;
        goto exit ;
      }
      else
      {
        sal_sprintf(err_msg,
                "\r\n\n"
                "--> \'power up serdes\' succeeded \r\n"
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_SWEEP_APP_FE_DEVICE_ID,1))
    {
      ui_ret = get_val_of( current_line,(int *)&match_index,
                        PARAM_SWEEP_APP_FE_DEVICE_ID,1,
                        &param_val,VAL_NUMERIC,err_msg
                      ) ;
      if (0 != ui_ret)
      {
        sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'power up serdes\' error \r\n"
                "*** or \'expecting fe id \'.\r\n"
                "    Probably SW error\r\n"
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
        ui_ret = TRUE ;
        goto exit ;
      }

      fe_to_access = param_val->value.ulong_value ;

      ui_ret = get_val_of( current_line,(int *)&match_index,
                        PARAM_SWEEP_APP_SERDES_ID,1,
                        &param_val,VAL_NUMERIC,err_msg
                      ) ;
      if (0 != ui_ret)
      {
        sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'power up serdes\' error \r\n"
                "*** or \'expecting serdes# \'.\r\n"
                "    Probably SW error\r\n"
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
        ui_ret = TRUE ;
        goto exit ;
      }
      in_struct.out_struct_type = FE200_STRUCTURE_TYPE ;
      in_struct.common_union.fe200_struct.proc_id = FE200_RESET_SINGLE_SERDES ;
      in_struct.common_union.fe200_struct.data.reset_serdes_data.unit = csys_mngment_local_get_unit(SOC_SAND_DEV_FE200,fe_to_access) ;
      in_struct.common_union.fe200_struct.data.reset_serdes_data.serdes_number = param_val->value.ulong_value ;
      in_struct.common_union.fe200_struct.data.reset_serdes_data.remain_active = FALSE ;

      err = fmc_send_buffer_to_device(
              SOC_SAND_DEV_FE200,
              fe_to_access,
              absolute_dev_id,
              sizeof(in_struct),
              (unsigned char*)&in_struct,
              sizeof(out_struct),
              (unsigned char*)&out_struct
            );
      if (err)
      {
        sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'power up serdes\' returned with error \r\n"
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      else
      {
        sal_sprintf(err_msg,
                "\r\n\n"
                "--> \'power up serdes\' succeeded \r\n"
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
    }
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_SWEEP_APP_GRACEFUL_SHUTDOWN_FE_ID,1))
  {
    FE200_AGENT_IN_STRUCT
      *fe200_agent_in_struct ;
    ui_ret = get_val_of(current_line,(int *)&match_index,
                      PARAM_SWEEP_APP_FE_DEVICE_ID,1,
                      &param_val,VAL_NUMERIC,err_msg
                    ) ;
    if (0 != ui_ret)
    {
      sal_sprintf(err_msg,
              "\r\n\n"
              "*** \'graceful_shutdown_fe\' error: \'fe#\' field not found on command line.\r\n"
              "    Probably SW error\r\n"
      ) ;
      send_string_to_screen(err_msg,TRUE) ;
      ui_ret = TRUE ;
      goto exit ;
    }
    fe_to_access = param_val->value.ulong_value ;


    in_struct.out_struct_type = FE200_STRUCTURE_TYPE ;
    fe200_agent_in_struct = &(in_struct.common_union.fe200_struct) ;
    fe200_agent_in_struct->proc_id = FE200_SHUTDOWN_DEVICE ;
    fe200_agent_in_struct->data.just_unit_data.unit = csys_mngment_local_get_unit(SOC_SAND_DEV_FE200,fe_to_access);

    err = fmc_send_buffer_to_device(
            SOC_SAND_DEV_FE200,
            fe_to_access,
            absolute_dev_id,
            sizeof(in_struct),
            (unsigned char*)&in_struct,
            sizeof(out_struct),
            (unsigned char*)&out_struct
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
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_SWEEP_APP_GRACEFUL_RESTORE_FE_ID,1))
  {
    FE200_AGENT_IN_STRUCT
      *fe200_agent_in_struct ;
    ui_ret = get_val_of(current_line,(int *)&match_index,
                      PARAM_SWEEP_APP_FE_DEVICE_ID,1,
                      &param_val,VAL_NUMERIC,err_msg
                    ) ;
    if (0 != ui_ret)
    {
      sal_sprintf(err_msg,
              "\r\n\n"
              "*** \'graceful_restore_fe\' error: \'fe#\' field not found on command line.\r\n"
              "    Probably SW error\r\n"
      ) ;
      send_string_to_screen(err_msg,TRUE) ;
      ui_ret = TRUE ;
      goto exit ;
    }
    fe_to_access = param_val->value.ulong_value ;


    in_struct.out_struct_type = FE200_STRUCTURE_TYPE ;
    fe200_agent_in_struct = &(in_struct.common_union.fe200_struct) ;
    fe200_agent_in_struct->proc_id = FE200_ACTIVATE_FE_AND_LINKS ;
    fe200_agent_in_struct->data.just_unit_data.unit = csys_mngment_local_get_unit(SOC_SAND_DEV_FE200,fe_to_access);

    err = fmc_send_buffer_to_device(
            SOC_SAND_DEV_FE200,
            fe_to_access,
            absolute_dev_id,
            sizeof(in_struct),
            (unsigned char*)&in_struct,
            sizeof(out_struct),
            (unsigned char*)&out_struct
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
  }
  else
  {
    send_string_to_screen(" *** Un-supported sweep option",TRUE);
    send_string_to_screen(__FILE__,TRUE);
    ui_ret = TRUE;
    goto exit;
  }

exit:
  return ui_ret;
}



/*******************************************************
*NAME
*  subject_fap20v_sweep_app_b
*TYPE: PROC
*DATE: 15/JUN/2004
*FUNCTION:
*  Process input line which has an 'sweep' subject.
*  Input line is assumed to have been checked and
*  found to be of right format.
*CALLING SEQUENCE:
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
  subject_fap20v_sweep_app_b(
    CURRENT_LINE *current_line, CURRENT_LINE **current_line_ptr
  )
{

  return process_sweep_app_line_b(
            current_line, current_line_ptr
         );
}



/* } */


#ifdef _MSC_VER
  #pragma pack(pop)
#endif

#endif /*LINK_FAP20V_LIBRARIES*/
