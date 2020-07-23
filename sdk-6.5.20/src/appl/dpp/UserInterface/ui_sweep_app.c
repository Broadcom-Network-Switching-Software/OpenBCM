
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/*
 * Basic_include_file.
 */

/*
 * General include file for reference design.
 */
#include <appl/diag/dpp/ref_sys.h>
/*
 * INCLUDE FILES:
 */
/* { */

#include <appl/dpp/UserInterface/ui_sweep_app.h>


#include <appl/dpp/UserInterface/ui_pure_defi_sweep_app.h>

#include <FMC/fmc_transport_layer.h>
#include <FMC/fmc_transport_layer_dcl.h>

#if LINK_FAP20V_LIBRARIES

#include <soc/dpp/SOC_SAND_FAP20V/fap20v_api_links.h>
#include <soc/dpp/SOC_SAND_FAP20V/fap20v_api_auto_flow_management.h>
#include <soc/dpp/SOC_SAND_FAP20V/fap20v_api_links.h>

#include <sweep/fap20v/sweep_fap20v_app.h>
#include <sweep/fap20v/sweep_fap20v_db.h>

#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_workload_status.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>

#include <CSystem/csystem_mngmnt.h>
/* } */


/*
 * Error Handling
 * {
 */

void
  ui_sweep_reoprt_sw_error_to_user(
    unsigned int err_id
  )
{
  char
    err_msg[80*2] = "";

  sal_sprintf(err_msg, "process_sweep_app_line SW error %u\r\n", err_id) ;
  send_string_to_screen(err_msg,TRUE) ;
}



void
  sweep_ui_report_driver_err(
    const unsigned long driver_ret
  )
{
  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    send_string_to_screen("\n\r"
                          "Driver error while executing the command",TRUE);
    soc_sand_disp_result(driver_ret);
    send_string_to_screen("\n\r", TRUE);
  }
}


/*
 * }
 */

/*****************************************************
*NAME
*  process_sweep_app_line
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
int process_sweep_app_line(CURRENT_LINE *current_line, CURRENT_LINE **current_line_ptr)
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
    src_fap,
    class_id;
  SOC_SAND_DEVICE_ENTITY
    entity_type = SOC_SAND_DONT_CARE_ENTITY;

  enum
  {
    NO_ERR = 0,
    ERR_001, ERR_002, ERR_003, ERR_004, ERR_005,
    NOF_ERR
  };


  proc_name = "process_sweep_app_line" ;
  ui_ret = 0 ;
  get_val_of_err = FALSE;
  send_string_to_screen("",TRUE);
  driver_ret = SOC_SAND_OK;

  absolute_dev_id = TRUE;



 if (!search_param_val_pairs(current_line,&match_index,PARAM_SWEEP_APP_GET_QOS_PARAMS_ID,1) ||
           !search_param_val_pairs(current_line,&match_index,PARAM_SWEEP_APP_SET_QOS_PARAMS_ID,1)
     )
  {
    unsigned int
      flow_id,
      queue_id,
      base_flow_id,
      base_queue_id,
      nof_ids,
      np_i,
      relative_port,
      max_nof_faps,
      max_nof_ports,
      coexist_multply_factor;

    /*
     * get the source fap device
     */
    ui_ret = get_val_of( current_line,(int *)&match_index,
                      PARAM_SWEEP_APP_SOURCE_FAP_DEVICE_ID,1,
                      &param_val,VAL_NUMERIC,err_msg
                    ) ;
    if (0 != ui_ret)
    {
      sal_sprintf(err_msg,
              "\r\n\n"
              "*** 'get/set QOS params' error \r\n"
              "*** or 'expecting source fap# '.\r\n"
              "    Probably SW error\r\n"
      ) ;
      send_string_to_screen(err_msg,TRUE) ;
      ui_ret = TRUE ;
      goto exit ;
    }
    src_fap = param_val->value.ulong_value ;

    /*
     * get the destination fap device
     */
    ui_ret = get_val_of( current_line,(int *)&match_index,
                      PARAM_SWEEP_APP_DESTINATION_FAP_DEVICE_ID,1,
                      &param_val,VAL_NUMERIC,err_msg
                    ) ;
    if (0 != ui_ret)
    {
      sal_sprintf(err_msg,
              "\r\n\n"
              "*** 'get/set QOS params' error \r\n"
              "*** or 'expecting destination fap# '.\r\n"
              "    Probably SW error\r\n"
      ) ;
      send_string_to_screen(err_msg,TRUE) ;
      ui_ret = TRUE ;
      goto exit ;
    }
    dst_fap = param_val->value.ulong_value ;

    /*
     * get the destination port id
     */
    ui_ret = get_val_of( current_line,(int *)&match_index,
                      PARAM_SWEEP_APP_DESTINATION_PORT_ID,1,
                      &param_val,VAL_NUMERIC,err_msg
                    ) ;
    if (0 != ui_ret)
    {
      sal_sprintf(err_msg,
              "\r\n\n"
              "*** 'get/set QOS params' error \r\n"
              "*** or 'expecting destination port# '.\r\n"
              "    Probably SW error\r\n"
      ) ;
      send_string_to_screen(err_msg,TRUE) ;
      ui_ret = TRUE ;
      goto exit ;
    }
    dst_port = param_val->value.ulong_value ;

    /*
     * get the destination class
     */
    ui_ret = get_val_of( current_line,(int *)&match_index,
                      PARAM_SWEEP_APP_DESTINATION_CLASS_ID,1,
                      &param_val,VAL_NUMERIC,err_msg
                    ) ;
    if (0 != ui_ret)
    {
      sal_sprintf(err_msg,
              "\r\n\n"
              "*** 'get/set QOS params' error \r\n"
              "*** or 'expecting class id '.\r\n"
              "    Probably SW error\r\n"
      ) ;
      send_string_to_screen(err_msg,TRUE) ;
      ui_ret = TRUE ;
      goto exit ;
    }
    class_id = param_val->value.ulong_value ;

    /* check validity of fap/ports indices */
    /* get the number of faps/ports used */
    fap20v_auto_flow_mngt_get_params(&max_nof_faps,
                                     &max_nof_ports);


    if(dst_port < FAP20V_AUTO_FLOW_NP_0_FIRST_DATA_PORT ||
       (dst_port >= (FAP20V_AUTO_FLOW_NP_0_FIRST_DATA_PORT + max_nof_ports) &&
        dst_port < FAP20V_AUTO_FLOW_NP_1_FIRST_DATA_PORT) ||
        dst_port  >= (FAP20V_AUTO_FLOW_NP_1_FIRST_DATA_PORT + max_nof_ports)
      )
    {
       sal_sprintf(err_msg,
               "\r\n\n"
                "*** \'get/set QOS params\' error \r\n"
               "*** destination port is inavlid: %lu...\r\n", dst_port
       ) ;
       send_string_to_screen(err_msg,TRUE) ;
       ui_ret = TRUE ;
       goto exit ;
    }

    if (sweep_db_get_fap20v_id_in_2_steps())
    {
      /*
       * In Co-Exist system, when accessing the H/W FAP-Id
       * we need to use multiply factor of 2.
       * This is relevant to Negev System with specific mini-back-plane.
       */
      coexist_multply_factor = 2;
    }
    else
    {
      coexist_multply_factor = 1;
    }
    if(src_fap < 0 ||
       coexist_multply_factor*src_fap >= max_nof_faps ||
       dst_fap < 0 ||
       coexist_multply_factor*dst_fap >= max_nof_faps
      )
    {
       sal_sprintf(err_msg,
               "\r\n\n"
               "*** \'get/set QOS params\' error \r\n"
               "*** source/destination fap is inavlid. Source, Dest: %d, %lu ...\r\n", src_fap, dst_fap
       ) ;
       send_string_to_screen(err_msg,TRUE) ;
       ui_ret = TRUE ;
       goto exit ;
    }

    /* decide which spi4 is it...*/
    if(dst_port >= FAP20V_AUTO_FLOW_NP_1_FIRST_DATA_PORT)
    {
      np_i = 1;
      relative_port = dst_port - FAP20V_AUTO_FLOW_NP_1_FIRST_DATA_PORT;
    }
    else
    {
      np_i = 0;
      relative_port = dst_port - FAP20V_AUTO_FLOW_NP_0_FIRST_DATA_PORT;
    }

    /* get the base flow/queue from the auto flows management */
    if(fap20v_auto_map_fap_id_to_base_ids(coexist_multply_factor*src_fap,
                                          coexist_multply_factor*dst_fap,
                                          np_i,
                                          &base_queue_id,
                                          &base_flow_id,
                                          &nof_ids)
      )
    {
       sal_sprintf(err_msg,
               "\r\n\n"
               "*** \'get/set QOS params\' error \r\n"
               "*** or \'failed to calculate flow id \'.\r\n"
               "    Probably SW error\r\n"
       ) ;
       send_string_to_screen(err_msg,TRUE) ;
       ui_ret = TRUE ;
       goto exit ;
    }

    /* compute the flow from the flow_base + class & port */
    flow_id = base_flow_id +
      (FAP20V_FABRIC_UNICAST_CLASSES * FAP20V_AUTO_FLOW_MAX_NOF_SUB_FLOWS * relative_port) +
      (class_id *                      FAP20V_AUTO_FLOW_MAX_NOF_SUB_FLOWS);

    queue_id = base_queue_id + (FAP20V_FABRIC_UNICAST_CLASSES * relative_port) + class_id;

    /*
     * the command "get_QOS_params"
     */
    if (!search_param_val_pairs(current_line,&match_index,PARAM_SWEEP_APP_GET_QOS_PARAMS_ID,1))
    {
      /* get the flow id params:
       *    a. get the target fap remote handle.
       *    b. get (and present) it's flow params.
       */

      in_struct.out_struct_type = FAP20V_STRUCTURE_TYPE;
      in_struct.common_union.fap20v_struct.proc_id = FAP20V_GET_SCHEDULER_FLOW_PARAMS;
      in_struct.common_union.fap20v_struct.data.get_sched_flow_params.unit = csys_mngment_local_get_unit(SOC_SAND_DEV_FAP20V,dst_fap);
      in_struct.common_union.fap20v_struct.data.get_sched_flow_params.sched_flow_id = flow_id;

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
                "*** 'fap20v_get_scheduler_flow_params' error = %d. \r\n",
                err
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
        ui_ret = TRUE ;
        goto exit ;
      }

      start_print_services();
      send_string_to_screen("",TRUE);
      fap20v_print_FAP20V_FLOW(
        &out_struct.common_union.fap20v_struct.data.get_scheduler_flow.flow,
        ui_printing_policy_get()
      );
      send_string_to_screen("",TRUE);
      end_print_services();
    }

  /*
   * the command "set_QOS_params"
   */
    else
    {
      FAP20V_FLOW
        scheduler_flow,
        exact_flow_given;

      fap20v_clear_FLOW(&scheduler_flow);
      fap20v_clear_FLOW(&exact_flow_given);

      scheduler_flow.low_delay = FALSE;

      /*
       * get the FIRST sub flow TYPE
       */
      ui_ret = get_val_of(current_line,(int *)&match_index,
                        PARAM_SWEEP_APP_SUB_FLOW_0_TYPE_ID,1,
                        &param_val,VAL_SYMBOL,err_msg
                      );
      if(0 != ui_ret)
      {
        send_string_to_screen(err_msg,TRUE);
        ui_ret = TRUE;
        goto exit;
      }
      scheduler_flow.sub_flow[0].type = (FAP20V_SUB_FLOW_TYPE) param_val->numeric_equivalent;


    /*
     * get the FIRTS sub flow VALUE
     */
      ui_ret = get_val_of(current_line,(int *)&match_index,
                        PARAM_SWEEP_APP_SUB_FLOW_0_VALUE_ID,1,
                        &param_val,VAL_NUMERIC,err_msg
                      );
      if(0 != ui_ret)
      {
        send_string_to_screen(err_msg,TRUE);
        ui_ret = TRUE;
        goto exit;
      }
      scheduler_flow.sub_flow[0].value = param_val->value.ulong_value;


      /*
       * get the SECOND sub flow TYPE
       */
      ui_ret = get_val_of(current_line,(int *)&match_index,
                        PARAM_SWEEP_APP_SUB_FLOW_1_TYPE_ID,1,
                        &param_val,VAL_SYMBOL,err_msg
                      );
      if(0 != ui_ret)
      {
        /*
         * There is only one sub-flow.
         */
        scheduler_flow.sub_flow[1].type = FAP20V_UNDEF_FLOW;
        ui_ret = 0;
      }
      else
      {
        scheduler_flow.sub_flow[1].type = (FAP20V_SUB_FLOW_TYPE) param_val->numeric_equivalent;
        /*
         * get the SECOND sub flow VALUE
         */
        ui_ret = get_val_of(current_line,(int *)&match_index,
                          PARAM_SWEEP_APP_SUB_FLOW_1_VALUE_ID,1,
                          &param_val,VAL_NUMERIC,err_msg
                        );
        if(0 != ui_ret)
        {
          send_string_to_screen(err_msg,TRUE);
          ui_ret = TRUE;
          goto exit;
        }
        scheduler_flow.sub_flow[1].value = param_val->value.ulong_value;
      }

      /*
       * update the flow QOS parameters
       */
      in_struct.out_struct_type = FAP20V_STRUCTURE_TYPE;
      in_struct.common_union.fap20v_struct.proc_id = FAP20V_UPDATE_SCHEDULER_FLOW_QOS;
      in_struct.common_union.fap20v_struct.data.update_sched_flow_qos.unit = csys_mngment_local_get_unit(SOC_SAND_DEV_FAP20V,dst_fap);
      in_struct.common_union.fap20v_struct.data.update_sched_flow_qos.flow_id = flow_id;
      sal_memcpy(
        &in_struct.common_union.fap20v_struct.data.update_sched_flow_qos.flow,
        &scheduler_flow,
        sizeof(scheduler_flow)
      );

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
                "*** 'update flow qos' error = %d. \r\n",
                err
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
        ui_ret = TRUE ;
        goto exit ;
      }


      start_print_services();
      send_string_to_screen("",TRUE);
      fap20v_print_FAP20V_FLOW(
        &out_struct.common_union.fap20v_struct.data.update_scheduler_flow_qos.exact_flow_given,
        ui_printing_policy_get()
      );
      send_string_to_screen("",TRUE);
      end_print_services();
    }
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
        if ( entity_type == SOC_SAND_FE2_ENTITY)
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
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_SWEEP_APP_QUEUE_ON_MSG_INTERVAL_ID,1))
  {
    if (!search_param_val_pairs(current_line,&match_index,PARAM_SWEEP_APP_GET_INTERVAL_ID,1))
    {
      sal_sprintf(err_msg,
              "\r\n\n"
              "queue ON message interval is %lu microSeconds\r\n",
              queue_on_msg_task_interval
      ) ;
      send_string_to_screen(err_msg,TRUE) ;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_SWEEP_APP_SET_INTERVAL_ID,1))
    {
      ui_ret = get_val_of(current_line,(int *)&match_index,
                        PARAM_SWEEP_APP_SET_INTERVAL_ID,1,
                        &param_val,VAL_NUMERIC,err_msg
                      ) ;
      if (0 != ui_ret)
      {
        sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'queue_on_msg_task_interval\' error: interval value is missing.\r\n"
                "    Probably SW error\r\n"
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
        ui_ret = TRUE ;
        goto exit ;
      }
      queue_on_msg_task_interval = param_val->value.ulong_value ;
    }
    else
    {
      send_string_to_screen(" *** Un-supported sweep option",TRUE);
      send_string_to_screen(__FILE__,TRUE);
      ui_ret = TRUE;
      goto exit;
    }
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_SWEEP_APP_AGGR_ON_MSG_INTERVAL_ID,1))
  {
    if (!search_param_val_pairs(current_line,&match_index,PARAM_SWEEP_APP_GET_INTERVAL_ID,1))
    {
      sal_sprintf(err_msg,
              "\r\n\n"
              "aggregate ON message interval is %lu microSeconds\r\n"
              "Last cycle took %lu microseconds, last call %lu microseconds\r\n",
              aggr_on_msg_task_interval,
              time_diff,
              time_diff_single
      ) ;
      send_string_to_screen(err_msg,TRUE) ;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_SWEEP_APP_SET_INTERVAL_ID,1))
    {
      ui_ret = get_val_of(current_line,(int *)&match_index,
                        PARAM_SWEEP_APP_SET_INTERVAL_ID,1,
                        &param_val,VAL_NUMERIC,err_msg
                      ) ;
      if (0 != ui_ret)
      {
        sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'aggr_on_msg_task_interval\' error: interval value is missing.\r\n"
                "    Probably SW error\r\n"
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
        ui_ret = TRUE ;
        goto exit ;
      }
      aggr_on_msg_task_interval = param_val->value.ulong_value ;
      if (!search_param_val_pairs(current_line,&match_index,PARAM_SWEEP_APP_REFRESH_ALL_AGGRS,1))
      {
        refresh_all_aggrs = TRUE;
      }
      else
      {
        refresh_all_aggrs = FALSE;
      }
    }
    else
    {
      send_string_to_screen(" *** Un-supported sweep option",TRUE);
      send_string_to_screen(__FILE__,TRUE);
      ui_ret = TRUE;
      goto exit;
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
*  subject_fap20v_sweep_app
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
  subject_fap20v_sweep_app(
    CURRENT_LINE *current_line, CURRENT_LINE **current_line_ptr
  )
{

  return process_sweep_app_line(
            current_line, current_line_ptr
         );
}
#endif /*LINK_FAP20V_LIBRARIES*/


