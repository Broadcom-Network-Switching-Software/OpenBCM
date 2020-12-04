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

#if (LINK_FAP20V_LIBRARIES || LINK_FAP21V_LIBRARIES)
/* { */

/*
 * INCLUDE FILES:
 */
#ifndef SAND_LOW_LEVEL_SIMULATION
/* { */
/* } */
#endif

#include <stdio.h>
/*
 * User interface internal include file.
 */
#include <appl/dpp/UserInterface/ui_defi.h>
#include <appl/dpp/UserInterface/ui_pure_defi_line_tgs.h>


#include <appl/diag/dpp/utils_line_TGS.h>
#include <appl/diag/dpp/utils_app_tgs.h>
#include <appl/diag/dpp/utils_app_tgs_fap20v.h>
#include <appl/diag/dpp/utils_app_tgs_fap21v.h>
#include <appl/diag/dpp/utils_dune_fpga_download.h>
#include <appl/diag/dpp/utils_front_end_host_card.h>

#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_workload_status.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>


/*
 */

/*
 * Error Handling
 * {
 */

void ui_tgs_reoprt_sw_error_to_user(unsigned int err_id)
{
  char
    err_msg[80*2] = "";

  sal_sprintf(err_msg, "process_tgs_app_line SW error %u\r\n", err_id) ;
  send_string_to_screen(err_msg,TRUE) ;
}


#define TGS_UI_REPORT_SW_ERR_AND_EXIT(err_id)  \
      if (0 != ui_ret)\
      {\
        ui_tgs_reoprt_sw_error_to_user(err_id);\
        ui_ret = TRUE ;\
        goto exit ;\
      }



void
  tgs_ui_report_driver_err(
    const unsigned long driver_ui_ret
  )
{
  if(soc_sand_get_error_code_from_error_word(driver_ui_ret) != SOC_SAND_OK)
  {
    send_string_to_screen("\n\r"
                          "Driver error while executing the command",TRUE);
    soc_sand_disp_result(driver_ui_ret);
    send_string_to_screen("\n\r", TRUE);
  }
}


/*
 * }
 */



/*****************************************************
*NAME
*  subject_line_tgs
*TYPE: PROC
*DATE: 29/DEC/2002
*FUNCTION:
*  Process input line which has an 'tgs' subject.
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
int subject_line_tgs(CURRENT_LINE *current_line, CURRENT_LINE **current_line_ptr)
{
  PARAM_VAL
    *param_val ;
  char
    err_msg[80*4] = "";
  char
    *proc_name;
  BOOL
    line_tgs_connected = 0 ;
  BOOL
    get_val_of_err ;
  unsigned int
    match_index,
    fpga_id,
    short_format,
    verbose,
    silent,
    all_chassis,
    tgs_mac_address; /*0x0 to 0xFF*/
  enum
  {
    NO_ERR = 0,
    ERR_001, ERR_002, ERR_003, ERR_004, ERR_005,
    ERR_006, ERR_007, ERR_008, ERR_009, ERR_010,
    ERR_011, ERR_012, ERR_013, ERR_014, ERR_015,
    NUM_EXIT_CODES
  } ui_ret ;

  proc_name = "subject_line_tgs";
  ui_ret = NO_ERR ;
  get_val_of_err = FALSE;
  short_format = TRUE;

  send_string_to_screen("",TRUE);


  line_tgs_connected = is_line_card_tgs_connected();
  if (!line_tgs_connected)
  {
    send_string_to_screen("Line-Card-TGS is NOT connected to this CPU-card.\n\r", FALSE);
    goto exit ;
  }

  /*
   * the rest ot the line is empty
   */
  if (current_line->num_param_names == 0)
  {
    /*
     * Enter if there are no parameters on the line (just 'line_card').
     */
    send_string_to_screen("Line-Card-TGS is connected to this CPU-card.\n\r", FALSE);
    goto exit ;
  }


  if (!search_param_val_pairs(current_line,&match_index,PARAM_TGS_VERBOSE_ID,1))
  {
    verbose = TRUE ;
  }
  else
  {
    verbose = FALSE ;
  }
  silent = !verbose;

  if (!search_param_val_pairs(current_line,&match_index,PARAM_TGS_ALL_CHASSIS_ID,1))
  {
    all_chassis = TRUE ;
  }
  else
  {
    all_chassis = FALSE ;
  }


  if ( !search_param_val_pairs(current_line,&match_index,PARAM_TGS_SPI_HEADER_TABLE_ID,1))
  {
    /*
     * Operation related to SPI_HEADER_TABLE
     */

    if (!search_param_val_pairs(current_line,&match_index,PARAM_TGS_PRINT_ID,1))
    {
      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_TGS_FPGA_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit;
      }
      else
      {
        fpga_id = (unsigned long)param_val->value.ulong_value ;
      }

      if (!search_param_val_pairs(current_line,&match_index,PARAM_TGS_LONG_FORMAT_ID,1))
      {
        short_format = FALSE;
      }

      tgs_spi_header_table_print(fpga_id, short_format);
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_TGS_ADD_RAW_ENTRY_ID,1))
    {
      unsigned long
        entry_val;

      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_TGS_FPGA_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit;
      }
      else
      {
        fpga_id = (unsigned long)param_val->value.ulong_value ;
      }

      if (get_val_of(
              current_line,(int *)&match_index,PARAM_TGS_IN_SPI_HEAD_ENTRY_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit;
      }
      else
      {
        tgs_mac_address = (unsigned long)param_val->value.ulong_value ;
      }

      if (get_val_of(
              current_line,(int *)&match_index,PARAM_TGS_FREE_VAL_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit;
      }
      else
      {
        entry_val = (unsigned long)param_val->value.ulong_value ;
      }

      tgs_spi_header_table_add_raw_entry(
        fpga_id,
        tgs_mac_address,
        entry_val,
        tgs_MAC_ADDRESS_FLAVOR_to_str(TGS_MAC_FLVR_CLI_DEFINITION, TRUE)
      );
    }
#if LINK_FAP20V_LIBRARIES

    else if (!search_param_val_pairs(current_line,&match_index,PARAM_TGS_MAC_ENTRY_ID,1))
    {
      FAP20V_INGRESS_PACKET_HEADER
        header;
      unsigned int
        dp;

      fap20v_header_clear(&header);

      UI_MACROS_GET_NUM_SYM_VAL(PARAM_TGS_MAC_ENTRY_ID)
      TGS_UI_REPORT_SW_ERR_AND_EXIT(ERR_006);
      UI_MACROS_LOAD_LONG_VAL(tgs_mac_address)

      UI_MACROS_GET_NUM_SYM_VAL(PARAM_TGS_FPGA_ID)
      TGS_UI_REPORT_SW_ERR_AND_EXIT(ERR_007);
      UI_MACROS_LOAD_LONG_VAL(fpga_id)

      UI_MACROS_GET_NUM_SYM_VAL(PARAM_TGS_DP_ENTRY_ID)
      TGS_UI_REPORT_SW_ERR_AND_EXIT(ERR_008);
      UI_MACROS_LOAD_LONG_VAL(dp)


      if (!search_param_val_pairs(current_line,&match_index,PARAM_TGS_FAP_DEST_ID_ENTRY_ID,1))
      {
        header.type = FAP20V_PKT_HDR_U_TYPE;
        UI_MACROS_GET_NUM_SYM_VAL(PARAM_TGS_FAP_DEST_ID_ENTRY_ID)
        TGS_UI_REPORT_SW_ERR_AND_EXIT(ERR_009);
        UI_MACROS_LOAD_LONG_VAL(header.data.unicast.destination_id)

        UI_MACROS_GET_NUM_SYM_VAL(PARAM_TGS_FAP_CLASS_ENTRY_ID)
        TGS_UI_REPORT_SW_ERR_AND_EXIT(ERR_010);
        UI_MACROS_LOAD_LONG_VAL(header.data.unicast.class_val)
        header.data.unicast.drp =dp;
      }
      else if (!search_param_val_pairs(current_line,&match_index,PARAM_TGS_FAP_MULTI_ID_ENTRY_ID,1))
      {
        header.type = FAP20V_PKT_HDR_M_TYPE;
        UI_MACROS_GET_NUM_SYM_VAL(PARAM_TGS_FAP_MULTI_ID_ENTRY_ID)
        TGS_UI_REPORT_SW_ERR_AND_EXIT(ERR_011);
        UI_MACROS_LOAD_LONG_VAL(header.data.multicast.multicast_id)

        UI_MACROS_GET_NUM_SYM_VAL(PARAM_TGS_FAP_CLASS_ENTRY_ID)
        TGS_UI_REPORT_SW_ERR_AND_EXIT(ERR_012);
        UI_MACROS_LOAD_LONG_VAL(header.data.multicast.class_val)
        header.data.multicast.drp =dp;
      }
      else
      {
        header.type = FAP20V_PKT_HDR_F_TYPE;

        UI_MACROS_GET_NUM_SYM_VAL(PARAM_TGS_FAP_FLOW_ID_ENTRY_ID)
        TGS_UI_REPORT_SW_ERR_AND_EXIT(ERR_013);
        UI_MACROS_LOAD_LONG_VAL(header.data.flow.flow_id)
        header.data.flow.drp =dp;
      }

      tgs_fap20v_spi_header_table_add_header_entry(
        fpga_id,
        tgs_mac_address,
        &header,
        tgs_MAC_ADDRESS_FLAVOR_to_str(TGS_MAC_FLVR_CLI_DEFINITION, TRUE)
      );
    }
#endif /*LINK_FAP20V_LIBRARIES*/


  else
    {
      /*
       * No valid operation variable has been entered.
       */
      send_string_to_screen(
          "*** Should not reach here.\r\n",
          FALSE) ;
      ui_ret = ERR_001 ;
      goto exit ;
    }
  }
  else if ( !search_param_val_pairs(current_line,&match_index,PARAM_TGS_IF_ID,1))
  {
    if ( !search_param_val_pairs(current_line,&match_index,PARAM_TGS_MAC_ID,1))
    {
      unsigned int
        do_internal_loopback;
      int
        err;

      do_internal_loopback = FALSE;

      UI_MACROS_GET_NUM_SYM_VAL(PARAM_TGS_INTERNAL_LOOPBACK_ID)
      TGS_UI_REPORT_SW_ERR_AND_EXIT(ERR_014)
      UI_MACROS_LOAD_LONG_VAL(do_internal_loopback)

      UI_MACROS_GET_NUM_SYM_VAL(PARAM_TGS_FPGA_ID)
      TGS_UI_REPORT_SW_ERR_AND_EXIT(ERR_015);
      UI_MACROS_LOAD_LONG_VAL(fpga_id)

      if(all_chassis)
      {
        d_printf("Start Loopback application all over the chassis\n\r");
        err = tgs_if_mac_do_internal_loopback_chassis(
                fpga_id,
                do_internal_loopback,
                silent
              );
        if (err)
        {
          d_printf("ERROR: tgs_if_mac_do_internal_loopback_chassis() - %d\n\r", err);
        }
      }
      else
      {
        tgs_if_mac_do_internal_loopback(
          fpga_id,
          do_internal_loopback,
          silent
        );
      }


    }
  }
  else if ( !search_param_val_pairs(current_line,&match_index,PARAM_TGS_HELP_ID,1))
  {
    if ( !search_param_val_pairs(current_line,&match_index,PARAM_TGS_MAC_ID,1))
    {
      tgs_help_mac();
    }
  }
  else if ( !search_param_val_pairs(current_line,&match_index,PARAM_TGS_FCT_ID,1))
  {
    if (!search_param_val_pairs(current_line,&match_index,PARAM_TGS_FCT_GET_ENTRY_ID,1))
    {
      unsigned int
        port_id;
      unsigned long
        rate_in_mbps;
      int
        err;

      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_TGS_FPGA_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit;
      }
      else
      {
        fpga_id = (unsigned long)param_val->value.ulong_value ;
      }

      if (get_val_of(
              current_line,(int *)&match_index,PARAM_TGS_PORT_ID_ENTRY_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit;
      }
      else
      {
        port_id = (unsigned long)param_val->value.ulong_value ;
      }

      err = tgs_aux_fc_get_rate(
              fpga_id,
              port_id,
              &rate_in_mbps
            );
      if (err)
      {
        d_printf("ERROR: tgs_aux_fc_get_rate - %d\n\r", err);
      }
      else
      {
        d_printf("Port %u, rate_in_mbps %lu\n\r",
                 port_id, rate_in_mbps);
      }

    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_TGS_FCT_SET_ENTRY_ID,1))
    {
      unsigned int
        port_id;
      unsigned long
        rate_in_mbps,
        rounded_mbps;
      int
        err;

      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_TGS_FPGA_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit;
      }
      else
      {
        fpga_id = (unsigned long)param_val->value.ulong_value ;
      }

      if (get_val_of(
              current_line,(int *)&match_index,PARAM_TGS_PORT_ID_ENTRY_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit;
      }
      else
      {
        port_id = (unsigned long)param_val->value.ulong_value ;
      }

      if (get_val_of(
              current_line,(int *)&match_index,PARAM_TGS_FREE_VAL_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit;
      }
      else
      {
        rate_in_mbps = (unsigned long)param_val->value.ulong_value ;
      }

      err = tgs_aux_fc_set_rate(
              fpga_id,
              port_id,
              rate_in_mbps,
              &rounded_mbps
            );
      if (err)
      {
        d_printf("ERROR: tgs_aux_fc_set_sate - %d\n\r", err);
      }
      else
      {
        d_printf("FC was set with  - %lu Mbps (rounded)\n\r", rounded_mbps);
      }
    }
    else
    {
      /*
       * No valid operation variable has been entered.
       */
      send_string_to_screen(
          "*** Should not reach here.\r\n",
          FALSE) ;
      ui_ret = ERR_005 ;
      goto exit ;
    }
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_TGS_APP_SNAKE_MESH_ID,1))
  {
    int
      err;

    if(all_chassis)
    {
      d_printf("Start Snake-Mesh application all over the chassis\n\r");
      err = tgs_app_snake_mesh_chassis(silent);
      if (err)
      {
        d_printf("ERROR: tgs_app_snake_mesh_chassis() - %d\n\r", err);
      }
    }
    else
    {
      d_printf("Start Snake-Mesh application both FPGAs\n\r");
      d_printf(" This CLI should run on all cards in the system to have an effect\n\r");
      err = tgs_app_snake_mesh(silent);
      if (err)
      {
        d_printf("ERROR: tgs_app_snake_mesh() - %d\n\r", err);
      }
    }
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_TGS_APP_TRIPLE_PLAY_ID,1))
  {
    int
      err;

    if(all_chassis)
    {
      d_printf("Start Triple-Play application all over the chassis\n\r");
      err = tgs_app_triple_play_chassis(silent);
      if (err)
      {
        d_printf("ERROR: tgs_app_triple_play() - %d\n\r", err);
      }
    }
    else
    {
      d_printf("Start Triple-Play application both FPGAs\n\r");
      d_printf(" This CLI should run on all cards in the system to have an effect\n\r");
      err = tgs_app_triple_play(silent);
      if (err)
      {
        d_printf("ERROR: tgs_app_triple_play() - %d\n\r", err);
      }
    }
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_TGS_RESET_AND_APP_ENTRY_ID,1))
  {
    int
      err;

    if(all_chassis)
    {
      d_printf("Start APP both FPGAs (Including reset) all over the chassis\n\r");
      err = tgs_application_chassis(TRUE, silent);
      if (err)
      {
        d_printf("ERROR: tgs_application_chassis() - %d\n\r", err);
      }
    }
    else
    {
      d_printf("Start APP both FPGAs (Including reset)\n\r");
      err = tgs_application(0, TRUE, 4, silent);
      if (err)
      {
        d_printf("ERROR: tgs_application() - %d\n\r", err);
      }
    }
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_TGS_APP_FPGA_ID,1))
  {
    int
      err;

    if(all_chassis)
    {
      d_printf("Start APP both FPGAs (No reset) all over the chassis\n\r");
      err = tgs_application_chassis(FALSE, silent);
      if (err)
      {
        d_printf("ERROR: tgs_application_chassis() - %d\n\r", err);
      }
    }
    else
    {
      d_printf("Start APP both FPGAs (No Reset)\n\r");
      err = tgs_application(0, FALSE, 4, silent);
      if (err)
      {
        d_printf("ERROR: tgs_application() - %d\n\r", err);
      }
    }
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_TGS_CLR_INT_ENTRY_ID,1))
  {
    d_printf("\n\r");
    tgs_clear_counters_and_indications(TRUE);
  }
  /*
   * Else, there are parameters on the line (not just 'tgs').
   */
  else
  {
    /*
     * No valid TGS variable has been entered.
     * There must be at least one!
     */
    send_string_to_screen(
        "\r\n"
        "*** No valid parameter has been indicated for handling.\r\n"
        "    At least one must be entered.\r\n",
        FALSE) ;
    ui_ret = ERR_002 ;
    goto exit ;
  }

exit:


  if (get_val_of_err)
  {
    /*
     * Value of some parameter could not be ui_retrieved.
     */
    send_string_to_screen(
        "\r\n\n"
        "*** Procedure \'get_val_of\' ui_returned with error indication:\r\n",TRUE) ;
    send_string_to_screen(err_msg,TRUE) ;
    ui_ret = TRUE ;
  }
  return (ui_ret) ;
}


/* } */
#endif /*LINK_FAP20V_LIBRARIES || LINK_FAP21V_LIBRARIES*/


