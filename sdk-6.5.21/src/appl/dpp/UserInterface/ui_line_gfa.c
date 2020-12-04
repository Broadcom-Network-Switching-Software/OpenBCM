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

#if LINK_FAP20V_LIBRARIES
/* { */

#include <bcm_app/dpp/../h/usrApp.h>


/*
 * INCLUDE FILES:
 */
#include <appl/dpp/UserInterface/ui_defi.h>
#include <appl/dpp/UserInterface/ui_pure_defi_line_gfa.h>


#include <sweep/fap20v/sweep_fap20v_app.h>
#include <sweep/fap20v/sweep_fap20v_spi_calibrate.h>
#include <sweep/fap20v/sweep_fap20v_app_qdp_size_scaner.h>

#include <appl/diag/dpp/utils_line_GFA.h>
#include <appl/diag/dpp/utils_line_GFA_DB.h>
#include <appl/diag/dpp/utils_line_TGS.h>
#include <appl/diag/dpp/utils_fap20v_tests.h>
#include <appl/diag/dpp/utils_dune_fpga_download.h>
#include <appl/diag/dpp/utils_fap20v_calibrate.h>


#include <FMC/fap20v_agent_configuration.h>
#include <FMC/fap20v_agent_diagnostics.h>

#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_workload_status.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>

#include <appl/diag/dpp/dune_rpc_auto_network_learning.h>
#include <appl/diag/dpp/utils_screening.h>

#ifdef SAND_LOW_LEVEL_SIMULATION
  #include <soc/dpp/DriverTestWin_01/fap20v_sim_main.h>
#endif

/*
 */

void
  ui_screening_utils(CURRENT_LINE *current_line)
{
  UI_MACROS_INIT_FUNCTION("ui_screening_utils");

  if ((!search_param_val_pairs(current_line,&match_index,PARAM_GFA_SCRN_FTG_TEST_ID,1)))
  {
    utils_dune_ftg_screening_main(FALSE);
  }
  else if ((!search_param_val_pairs(current_line,&match_index,PARAM_GFA_SCRN_IF_TEST_ID,1)))
  {
    d_printf("currently not supported\n\r");
  }
  else if ((!search_param_val_pairs(current_line,&match_index,PARAM_GFA_SCRN_PRNT_USR_MD_ID,1)))
  {
    utils_print_user_mode();
  }
  else
  {
    d_printf("Unknown option\n\r");
  }
  return;
}
/*****************************************************
*NAME
*  subject_line_gfa
*TYPE: PROC
*DATE: 29/DEC/2002
*FUNCTION:
*  Process input line which has an 'gfa' subject.
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
int subject_line_gfa(CURRENT_LINE *current_line, CURRENT_LINE **current_line_ptr)
{
  PARAM_VAL
    *param_val ;
  char
    err_msg[80*4] = "";
  char
    *soc_sand_proc_name;
  BOOL
    line_gfa_connected = 0 ;
  BOOL
    get_val_of_err ;
  unsigned int
    unit,
    match_index;
  int
    ui_ret;

  enum
  {
    NO_ERR = 0,
    ERR_001, ERR_002, ERR_003, ERR_004, ERR_005,
    ERR_006,
    NUM_EXIT_CODES
  } ret ;
  FAP20V_SWEEP_PARAMS
    *sweep_params = NULL;
  unsigned long
    driver_ret;
  unsigned long
    verbose,
    silent;
  unsigned int
    is_hot_start = FALSE;

  soc_sand_proc_name = "subject_line_gfa";
  ret = NO_ERR ;
  get_val_of_err = FALSE;
  driver_ret = SOC_SAND_OK;

  send_string_to_screen("",TRUE);


  line_gfa_connected = is_line_card_gfa_connected();
  if (!line_gfa_connected)
  {
    send_string_to_screen("Line-Card-GFA is NOT connected to this CPU-card.\n\r", FALSE);
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
    send_string_to_screen("Line-Card-GFA is connected to this CPU-card.\n\r", FALSE);
    goto exit ;
  }
  else if ((!search_param_val_pairs(current_line,&match_index,PARAM_GFA_SCRN_ID,1)))
  {
    ui_screening_utils(current_line);
  }
  /*
   * Else, there are parameters on the line (not just 'gfa').
   */
  else if ( !search_param_val_pairs(current_line,&match_index,PARAM_GFA_FAP20V_1_ID,1))
  {
    /*
     * FAP20V_1
     */
    GFA_DEV_TYPE
      dev_type;
    PARAM_VAL
      *param_val_general_purpose = NULL;

    if (!search_param_val_pairs(current_line,&match_index,PARAM_GFA_FAP20V_1_ID,1))
    {
      dev_type = GFA_FAP20V_1;
    }
    else
    {
      send_string_to_screen(" 2. Can not be here ....", TRUE);
      send_string_to_screen(__FILE__, TRUE);
      ret= TRUE;
      goto exit;
    }

    if ((!search_param_val_pairs(current_line,&match_index,PARAM_GFA_RESET_ID,1)))
    {
      BOOL
        stay_down;
      unsigned long
        down_milisec;
      /*
 */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_GFA_MILISEC_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        /*
         * Value of down-time could not be retrieved set default.
         */
        down_milisec = 100 ;
      }
      else
      {
        down_milisec = (unsigned long)param_val->value.ulong_value ;
      }
      /*
       */
      if (search_param_val_pairs(current_line,&match_index,PARAM_GFA_STAY_DOWN_ID,1))
      {
        /*
         * Value of stay-down could not be retrieved set default.
         */
        stay_down = FALSE ;
      }
      else
      {
        stay_down = TRUE ;
      }
      ret = gfa_reset_device(
              dev_type,
              down_milisec,
              stay_down
            );
      if (ret)
      {
        sal_sprintf(
          err_msg,
          "-->> gfa_reset_device(%s, %lu, %u)-- failed.",
          gfa_dev_type_to_string(dev_type),
          down_milisec,
          stay_down
        );
        send_string_to_screen(err_msg, TRUE);
      }
      goto exit;

    }
    else if ((!search_param_val_pairs(current_line,&match_index,PARAM_GFA_READ_AND_CLEAR_CPU_PKT_ID,1)))
    {
      fap20v_read_and_clear_cpu_pkt(0, FALSE) ;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_GFA_LBG_ID,1))
    {
      static FAP20V_LOCAL_BW_GEN
        orig_lbg;

      if (!search_param_val_pairs(current_line,&match_index,PARAM_GFA_VERBOSE_ID,1))
      {
        verbose = TRUE ;
      }
      else
      {
        verbose = FALSE ;
      }
      silent = !verbose;

      /*
       * LBG - Local Bandwidth Generation
       */
      if (!search_param_val_pairs(current_line,&match_index,PARAM_GFA_LBG_START_CONF_ID,1))
      {
        driver_ret = fap20v_lbg_start_conf(0, &orig_lbg, silent);
      }
      if (!search_param_val_pairs(current_line,&match_index,PARAM_GFA_LBG_VALIDATE_RATE_ID,1))
      {
        unsigned long
          rate = 12000000,
          exact_rate;

         if (!get_val_of(
                current_line,(int *)&match_index,PARAM_GFA_LBG_VALIDATE_RATE_RATE_ID,1,
                &param_val,VAL_NUMERIC,err_msg))
        {
          rate = (unsigned long)param_val->value.ulong_value ;
        }

        driver_ret = fap20v_diag_validate_lbg_rate(0, rate, &exact_rate);
        disp_fap20v_result(driver_ret, "LBG function:");
      }
      else if (!search_param_val_pairs(current_line,&match_index,PARAM_GFA_LBG_SEND_PKT_ID,1))
      {
        unsigned long
          nof_pkt,
          pkt_byte_size,
          byte_i,
          type_i;
        unsigned char
          pad_data[FAP20V_CPU_TX_FRAG_BYTE_SIZE];

        /*
         */
        if (get_val_of(
                current_line,(int *)&match_index,PARAM_GFA_NOF_PKTS_ID,1,
                &param_val,VAL_NUMERIC,err_msg))
        {
          get_val_of_err = TRUE ;
          goto exit;
        }
        else
        {
          nof_pkt = (unsigned long)param_val->value.ulong_value ;
        }
        /*
         */
        if (get_val_of(
                current_line,(int *)&match_index,PARAM_GFA_PKT_BYTE_SIZE_ID,1,
                &param_val,VAL_NUMERIC,err_msg))
        {
          get_val_of_err = TRUE ;
          goto exit;
        }
        else
        {
          pkt_byte_size = (unsigned long)param_val->value.ulong_value ;
        }

        type_i = 0;
        for (byte_i = 0; byte_i< FAP20V_CPU_TX_FRAG_BYTE_SIZE; ++byte_i)
        {
          if (( (byte_i+0) % 4)==0)
          {
            type_i ^= 1;
          }
          if (type_i)
          {
            pad_data[byte_i] = 0xFF;
          }
          else
          {
            pad_data[byte_i] = 0x00;
          }
        }

        for (byte_i=1; byte_i<=FAP20V_CPU_TX_FRAG_BYTE_SIZE; ++byte_i)
        {
          ret = get_val_of( current_line, (int *)&match_index,
                            PARAM_GFA_PAD_DATA_ID, byte_i,
                            &param_val, VAL_NUMERIC, err_msg
                          ) ;
          if (ret)
          {
            break;
          }
          pad_data[byte_i-1] = ((unsigned char)param_val->value.ulong_value)&0xFF;
        }
        ret = 0;


        driver_ret = fap20v_lbg_send_packet(0, pkt_byte_size, nof_pkt, pad_data, silent);
      }
      else if (!search_param_val_pairs(current_line,&match_index,PARAM_GFA_GET_RATE_ID,1))
      {
        unsigned long
          pkt_byte_size;
        FAP20V_LBG_RATE_REPORT
          report;


        /*
         */
        if (get_val_of(
                current_line,(int *)&match_index,PARAM_GFA_PKT_BYTE_SIZE_ID,1,
                &param_val,VAL_NUMERIC,err_msg))
        {
          get_val_of_err = TRUE ;
          goto exit;
        }
        else
        {
          pkt_byte_size = (unsigned long)param_val->value.ulong_value ;
        }

        fap20v_lbg_get_rate(
          0, pkt_byte_size, FAP20V_LBG_MEASUREMENT_DELAY_MILIS,
          &report
        );
        fap20v_print_FAP20V_LBG_RATE_REPORT(&report, ui_printing_policy_get());
      }
      else if (!search_param_val_pairs(current_line,&match_index,PARAM_GFA_LBG_RESTORE_CONF_ID,1))
      {
        driver_ret = fap20v_lbg_restore_conf(0, &orig_lbg, silent);
      }
      else
      {
        /*
         * No valid GFA variable has been entered.
         * There must be at least one!
         */
        send_string_to_screen(
            "\r\n"
            "*** LBG *** No valid parameter has been indicated for handling.\r\n"
            "    At least one must be entered.\r\n",
            FALSE) ;
        ret = ERR_003 ;
        goto exit ;
      }
    }
    else if ((!search_param_val_pairs(current_line,&match_index,PARAM_GFA_SWEEP_APP_ID,1)))
    {
      unsigned int
        coexist_system,
        max_nof_faps,
        fap_id,
        port_i,
        faps_id_in_2_steps;
      const char*
        tmp_str = NULL;

      faps_id_in_2_steps = FALSE;
      sweep_params = sal_alloc(sizeof(FAP20V_SWEEP_PARAMS));
      if (NULL == sweep_params)
      {
        gen_err(TRUE,FALSE,(int)TRUE,0,
                "Malloc Failed",soc_sand_proc_name,
                SVR_ERR, 0, TRUE,0,600,FALSE) ;
        ret = ERR_004 ;
        goto exit ;
      }

      if (is_line_card_tgs_connected())
      {
        /*
         * If we are on TGS board, use TGS file.
         */
        tmp_str = "TGS";
        if( sweep_load_TGS_SWEEP_PARAMS(sweep_params) )
        {
          ret = -1;
          goto exit;
        }
      }
      else
      {
        tmp_str = "FTG";
        if( sweep_load_FTG_SWEEP_PARAMS(sweep_params) )
        {
          ret = -2;
          goto exit;
        }
      }
      if( sweep_get_params_from_host(DEFAULT_DOWNLOAD_IP, FAP20V_SWEEP_FILE_NAME, sweep_params) )
      {

        soc_sand_os_printf( 
          "\n\n\n"
          "Could not read SWEEP-APP parameter file '%s' from host\n"
          "Stays with the default parameters for %s.\n"
          "\n\n\n",
          FAP20V_SWEEP_FILE_NAME,
          tmp_str
        );
      }
      else
      {
        soc_sand_os_printf( 
          "\n\r"
          "File '%s', download and parsed OK.\n\r",
          FAP20V_SWEEP_FILE_NAME);
      }

      coexist_system  = FALSE;
      max_nof_faps    = 1;
      fap_id          = 0;

      /*
       * get the spi4 SOURCE cal length from the line
       */
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_GFA_SPI4_SRC_CAL_LEN_ID, 1,
                        &param_val, VAL_NUMERIC, err_msg
                      );
      if (0 != ret)
      {
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      sweep_params->spi_calendar[0].calendar_len = param_val->value.ulong_value;
      sweep_params->spi_calendar[2].calendar_len = param_val->value.ulong_value;

      /*
       * get the spi4 SOURCE cal M from the line
       */
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_GFA_SPI4_SRC_CAL_M_ID, 1,
                        &param_val, VAL_NUMERIC, err_msg
                      );
      if (0 != ret)
      {
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      sweep_params->spi_calendar[0].calendar_m = param_val->value.ulong_value;
      sweep_params->spi_calendar[2].calendar_m = param_val->value.ulong_value;

      /*
       * get the spi4 SINK cal length from the line
       */
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_GFA_SPI4_SNK_CAL_LEN_ID, 1,
                        &param_val, VAL_NUMERIC, err_msg
                      );
      if (0 != ret)
      {
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      sweep_params->spi_calendar[1].calendar_len = param_val->value.ulong_value;
      sweep_params->spi_calendar[3].calendar_len = param_val->value.ulong_value;


      /*
       * get the spi4 SINK cal M from the line
       */
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_GFA_SPI4_SNK_CAL_M_ID, 1,
                        &param_val, VAL_NUMERIC, err_msg
                      );
      if (0 != ret)
      {
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      sweep_params->spi_calendar[1].calendar_m = param_val->value.ulong_value;
      sweep_params->spi_calendar[3].calendar_m = param_val->value.ulong_value;


      /*
       * get the spi4 TSCLK from the line
       */
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_GFA_SPI4_SRC_TSCLK_SEL_ID, 1,
                        &param_val, VAL_NUMERIC, err_msg
                      );
      if (0 != ret)
      {
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      sweep_params->spi_source_tsclk_sel = param_val->value.ulong_value;

      /*
       * get the spi4 RSCLK from the line
       */
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_GFA_SPI4_SNK_RSCLK_SEL_ID, 1,
                        &param_val, VAL_NUMERIC, err_msg
                      );
      if (0 != ret)
      {
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      sweep_params->spi_sink_rsclk_sel = param_val->value.ulong_value;

      /*
       * get the spi4 DATA_MAX_T from the line
       */
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_GFA_SPI4_SRC_DATA_MAX_T_ID, 1,
                        &param_val, VAL_NUMERIC, err_msg
                      );
      if (0 != ret)
      {
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      sweep_params->spi_source_data_max_t = (unsigned long)param_val->value.ulong_value;

      /*
       * get the spi4 INIT_TRAIN_SEQ from the line
       */
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_GFA_SPI4_SRC_INIT_TRAIN_SEQ_ID, 1,
                        &param_val, VAL_NUMERIC, err_msg
                      );
      if (0 != ret)
      {
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      sweep_params->spi_source_init_train_seq = (unsigned long)param_val->value.ulong_value;


      /*
       * get the credit size from the line
       */
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_GFA_CREDIT_SIZE_ID, 1,
                        &param_val_general_purpose, VAL_SYMBOL, err_msg
                      );
      if (0 != ret)
      {
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      sweep_params->credit_size = param_val_general_purpose->numeric_equivalent;
      /*
       */
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_GFA_USE_OFC_ID, 1,
                        &param_val_general_purpose, VAL_SYMBOL, err_msg
                      );
      if (0 != ret)
      {
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }

      if (param_val_general_purpose->numeric_equivalent == TRUE)
      {
        /*
         * OFC
         */
        sweep_params->fc_port_mode = FAP20V_FC_PORT_AUXILIARY_CHANNEL;
      }
      else
      {
        sweep_params->fc_port_mode = FAP20V_FC_PORT_SPI_STATUS_CHANNEL;
      }

      /*
       */
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_GFA_USE_ZBT_ID, 1,
                        &param_val_general_purpose, VAL_SYMBOL, err_msg
                      );
      if (0 != ret)
      {
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      sweep_params->use_zbt = param_val_general_purpose->numeric_equivalent;
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_GFA_MAP_CPU_PORT_ID, 1,
                        &param_val_general_purpose, VAL_SYMBOL, err_msg
                      );
      if(0 == ret)
      {
        sweep_params->port_0_is_fap_cpu_port =
          param_val_general_purpose->numeric_equivalent;
      }

      /*
       */
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_GFA_USE_DUNE_FTG_ID, 1,
                        &param_val_general_purpose, VAL_SYMBOL, err_msg
                      );
      if (0 != ret)
      {
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      sweep_params->use_dune_ftg = param_val_general_purpose->numeric_equivalent;


      /*
       */
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_GFA_USE_COEXIST_ID, 1,
                        &param_val_general_purpose, VAL_SYMBOL, err_msg
                      );
      if (0 != ret)
      {
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      coexist_system = param_val_general_purpose->numeric_equivalent;

      /*
       * get FAP ID
       */
      ret = get_val_of(
              current_line,(int *)&match_index,PARAM_GFA_FAP_ID,1,
              &param_val,VAL_NUMERIC,err_msg
            );
      if (0 != ret)
      {
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      fap_id = (unsigned long)param_val->value.ulong_value ;

      /*
       * get number of FAPs
       */
      ret = get_val_of(
              current_line,(int *)&match_index,PARAM_GFA_MAX_NOF_FAPS_ID,1,
              &param_val,VAL_NUMERIC,err_msg
            );
      if (0 != ret)
      {
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      max_nof_faps = (unsigned long)param_val->value.ulong_value ;

      /*
       */
      ret = get_val_of(
              current_line,(int *)&match_index,PARAM_GFA_MAX_NOF_PORTS_ID,1,
              &param_val,VAL_NUMERIC,err_msg
            );
      if (0 != ret)
      {
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      sweep_params->max_nof_ports_per_spi = (unsigned long)param_val->value.ulong_value ;

      /*
       */
      ret = get_val_of(
              current_line,(int *)&match_index,PARAM_GFA_BW_RATE_ID,1,
              &param_val,VAL_NUMERIC,err_msg
            );
      if (0 != ret)
      {
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      for (port_i=0; port_i<sweep_params->max_nof_ports_per_spi; ++port_i)
      {
        sweep_params->nominal_rate_per_port_mbps[port_i] = param_val->value.ulong_value ;
      }
      if(sweep_params->max_nof_ports_per_spi == 1)
      {
        sweep_params->nominal_rate_per_port_mbps[1] = param_val->value.ulong_value ;
      }



      /*
       */
      ret = get_val_of(
              current_line,(int *)&match_index,PARAM_GFA_ZBT_NOF_BUFF_MEM_FIELD_ID,1,
              &param_val,VAL_NUMERIC,err_msg
            );
      if (0 != ret)
      {
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      sweep_params->zbt_nof_buff_mem_field = (unsigned long)param_val->value.ulong_value ;

      /*
       */
      ret = get_val_of(
              current_line,(int *)&match_index,PARAM_GFA_DRAM_BUFF_SIZE_FIELD_ID,1,
              &param_val,VAL_NUMERIC,err_msg
            );
      if (0 != ret)
      {
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      sweep_params->dram_buff_size_field = (unsigned long)param_val->value.ulong_value ;

      sweep_params->reset_fap = FALSE;

      if( utils_is_crate_negev(get_crate_type()) )
      {
        faps_id_in_2_steps      = TRUE;
        max_nof_faps *= 2;
        fap_id       *= 2;
      }

      unit = 0;


      sweep_fap20v_app(
        &unit,
        sweep_params,
        coexist_system,
        faps_id_in_2_steps,
        max_nof_faps,
        dune_rpc_get_number_of_fabric_cards()*2,
        fap_id,
        (unsigned long)FAP20V_1_BASE_ADDR,
        is_hot_start,
        FALSE
      );
    }

    else if ((!search_param_val_pairs(current_line,&match_index,PARAM_GFA_FAP20V_1_CLR_CNT_INT_ID,1)))
    {
      gfa_clear_counters_and_indications(0);
    }
    else if ((!search_param_val_pairs(current_line,&match_index,PARAM_GFA_DO_STATIC_SPI_CALIB,1)))
    {
      sweep_fap20v_spi_static_calibraiton_app(0,FALSE);
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_GFA_QUEUE_SCAN_ID,1))
    {
      UI_MACROS_INIT_FUNCTION("ui_fap20v_b_queue_size_scans");

      UI_MACROS_GET_NUMMERIC_VAL(PARAM_GFA_QUEUE_SCAN_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      unit = param_val->value.ulong_value;
      if(!search_param_val_pairs(current_line,&match_index,PARAM_GFA_QUEUE_SCAN_EN_ID,1))
      {
        sweep_fap20v_b_q_size_checker_enable_set(TRUE);
      }
      else if(!search_param_val_pairs(current_line,&match_index,PARAM_GFA_QUEUE_SCAN_DIS_ID,1))
      {
        sweep_fap20v_b_q_size_checker_enable_set(FALSE);
      }
      else if(!search_param_val_pairs(current_line,&match_index,PARAM_GFA_QUEUE_SCAN_PRINT_ID,1))
      {
        sweep_fap20v_b_q_size_cecker_print(unit);
      }
      else if(!search_param_val_pairs(current_line,&match_index,PARAM_GFA_QUEUE_SCAN_SET_PARAM_ID,1))
      {
        unsigned int  nof_retry,do_print;
        unsigned long max_q_size;
        UI_MACROS_GET_NUMMERIC_VAL(PARAM_GFA_QUEUE_SCAN_NOF_REWRITE_ID);
        UI_MACROS_CHECK_GET_VAL_OF_ERROR;
        nof_retry = param_val->value.ulong_value;
        UI_MACROS_GET_NUMMERIC_VAL(PARAM_GFA_QUEUE_SCAN_READ_SIZE_ID);
        UI_MACROS_CHECK_GET_VAL_OF_ERROR;
        do_print = param_val->value.ulong_value;
        UI_MACROS_GET_NUMMERIC_VAL(PARAM_GFA_QUEUE_SCAN_Q_SIZE_ID);
        UI_MACROS_CHECK_GET_VAL_OF_ERROR;
        max_q_size = param_val->value.ulong_value;

        sweep_fap20v_b_q_s_ckr_params_set(nof_retry,do_print,max_q_size);
      }

    }
    else if ((!search_param_val_pairs(current_line,&match_index,PARAM_GFA_FAP20V_1_IDDR_TEST_RW_ID,1)))
    {
      unsigned int
        data_i;
      unsigned long
        dram_offset,
        data_to_test[FAP20V_IDRAM_WORD_NOF_LONGS],
        nof_iteration;

      unit = 0;

      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_GFA_FAP20V_1_IDDR_OFFSET_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit;
      }
      else
      {
        dram_offset = (unsigned long)param_val->value.ulong_value ;
      }

      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_GFA_NOF_ITERATION_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit;
      }
      else
      {
        nof_iteration = (unsigned long)param_val->value.ulong_value ;
      }

      for (data_i=1; data_i<=FAP20V_IDRAM_WORD_NOF_LONGS; ++data_i)
      {
        ret = get_val_of( current_line, (int *)&match_index,
                          PARAM_GFA_PAD_DATA_ID, data_i,
                          &param_val, VAL_NUMERIC, err_msg
                        ) ;
        if (ret)
        {
          if (data_i == 1)
          {
            get_val_of_err = TRUE ;
            goto exit;
          }
          else
          {
            break;
          }
        }
        data_to_test[data_i-1] = (unsigned long)param_val->value.ulong_value;
      }
      ret = 0;

      for (; data_i<=FAP20V_IDRAM_WORD_NOF_LONGS; ++data_i)
      {
        data_to_test[data_i-1] = 0x55AA55AA;
      }
      fap20v_iddr_test_rw(
        unit,
        dram_offset,
        data_to_test,
        nof_iteration,
        TRUE,
        FALSE
      );
    }
    else if ((!search_param_val_pairs(current_line,&match_index,PARAM_GFA_FAP20V_1_ZBT_TEST_RW_ID,1)))
    {
      fap20v_zbt_test_rw(0, FALSE);
    }
    else if ((!search_param_val_pairs(current_line,&match_index,PARAM_GFA_FAP20V_1_INQ_TEST_RW_ID,1)))
    {
      fap20v_inq_test_rw(0, FALSE);
    }
    else if ((!search_param_val_pairs(current_line,&match_index,PARAM_GFA_FAP20V_1_CPU_TEST_RW_ID,1)))
    {
      fap20v_cpu_test_rw(0, FALSE);
    }
    else if ((!search_param_val_pairs(current_line,&match_index,PARAM_GFA_SEARCH_DRAM_DELAY_LINE_ID,1)))
    {
      unsigned long
        delay_in_milisec = 5000,
        tap_i,
        nof_tap_to_start,
        start_tap[FAP20V_DPI_NOF_TAPS];
      FAP20V_OVER_ALL_TAPS
        *over_all_taps = NULL;
      unsigned int
        fail;
      FAP20V_DLY_CRC_ERRS
        crc_errs;

      tap_i = 1;


      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_GFA_MILISEC_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit;
      }
      else
      {
        delay_in_milisec = (unsigned long)param_val->value.ulong_value ;
      }


      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_GFA_NOF_TAP_TO_START_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit;
      }
      else
      {
        nof_tap_to_start = (unsigned long)param_val->value.ulong_value ;
      }

      sal_memset(start_tap, 0, sizeof(start_tap));

      for (tap_i=1; tap_i<=FAP20V_DPI_NOF_TAPS; tap_i++)
      {
        ret = get_val_of( current_line, (int *)&match_index,
                          PARAM_GFA_START_TAPS_ID, tap_i,
                          &param_val, VAL_NUMERIC, err_msg
                        ) ;
        if (ret)
        {
          send_string_to_screen(
            "*** Need to type 5 Starting TAPs.\r\n",
            FALSE) ;
          get_val_of_err = TRUE ;
          goto exit;
        }
        start_tap[tap_i-1] = (unsigned long)param_val->value.ulong_value;
      }

      fap20v_search_dram_delay_line(
        0, &(start_tap[0]), delay_in_milisec, nof_tap_to_start, FALSE,
        0, 3,
        FALSE, &crc_errs, over_all_taps,
        &fail,
        FALSE);
    }
    else if ((!search_param_val_pairs(current_line,&match_index,PARAM_GFA_SEARCH_DRAM_DELAY_LINE_AUTO_FULL_ID,1)))
    {
      unsigned long
        delay_in_milisec = 5000,
        lbg_on,
        inverse_sso;

      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_GFA_MILISEC_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit;
      }
      else
      {
        delay_in_milisec = (unsigned long)param_val->value.ulong_value ;
      }

      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_LBG_ON_ID,1,
              &param_val,VAL_SYMBOL,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit;
      }
      else
      {
        lbg_on = param_val_general_purpose->numeric_equivalent ;
      }

      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_INVERSE_SSO_ID,1,
              &param_val,VAL_SYMBOL,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit;
      }
      else
      {
        inverse_sso = param_val_general_purpose->numeric_equivalent ;
      }


      fap20v_search_dram_delay_line_auto_full(
        0,
        delay_in_milisec, lbg_on,
        inverse_sso,
        FALSE
      );

    }
    else if ((!search_param_val_pairs(current_line,&match_index,PARAM_GFA_SEARCH_DRAM_DELAY_LINE_AUTO_ID,1)))
    {
      unsigned long
        delay_in_milisec = 5000,
        start_ddr,
        end_ddr,
        lbg_on,
        inverse_sso,
        best_taps[FAP20V_DPI_NOF_TAPS];
      unsigned int
        fail;

      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_GFA_MILISEC_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit;
      }
      else
      {
        delay_in_milisec = (unsigned long)param_val->value.ulong_value ;
      }

      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_LBG_ON_ID,1,
              &param_val,VAL_SYMBOL,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit;
      }
      else
      {
        lbg_on = param_val_general_purpose->numeric_equivalent ;
      }

      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_INVERSE_SSO_ID,1,
              &param_val,VAL_SYMBOL,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit;
      }
      else
      {
        inverse_sso = param_val_general_purpose->numeric_equivalent ;
      }

      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_GFA_START_DDR_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit;
      }
      else
      {
        start_ddr = (unsigned long)param_val->value.ulong_value ;
      }

      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_GFA_END_DDR_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit;
      }
      else
      {
        end_ddr = (unsigned long)param_val->value.ulong_value ;
      }

      fap20v_search_dram_delay_line_auto(
        0, delay_in_milisec, lbg_on,
        start_ddr, end_ddr,
        inverse_sso, best_taps,
        &fail,
        FALSE
      );

    }
    else if ((!search_param_val_pairs(current_line,&match_index,PARAM_GFA_SCAN_DRAM_DELAY_LINE_ID,1)))
    {
      unsigned long
        start_ddr,
        end_ddr,
        start_scan,
        end_scan,
        delay_in_milisec;
      FAP20V_DLY_CRC_ERRS
        crc_errs;


      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_GFA_MILISEC_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit;
      }
      else
      {
        delay_in_milisec = (unsigned long)param_val->value.ulong_value ;
      }


      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_GFA_START_DDR_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit;
      }
      else
      {
        start_ddr = (unsigned long)param_val->value.ulong_value ;
      }

      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_GFA_END_DDR_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit;
      }
      else
      {
        end_ddr = (unsigned long)param_val->value.ulong_value ;
      }
      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_GFA_START_SCAN_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit;
      }
      else
      {
        start_scan = (unsigned long)param_val->value.ulong_value ;
      }
      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_GFA_END_SCAN_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit;
      }
      else
      {
        end_scan = (unsigned long)param_val->value.ulong_value ;
      }
      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_GFA_END_SCAN_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit;
      }
      else
      {
        end_scan = (unsigned long)param_val->value.ulong_value ;
      }

      if (!search_param_val_pairs(current_line,&match_index,PARAM_GFA_VERBOSE_ID,1))
      {
        verbose = TRUE ;
      }
      else
      {
        verbose = FALSE ;
      }
      silent = !verbose;

      fap20v_scan_delay_line(
        0,
        start_ddr, end_ddr,
        start_scan, end_scan,
        delay_in_milisec, &crc_errs,
        silent
      );
    }
    else if ((!search_param_val_pairs(current_line,&match_index,PARAM_GFA_SCAN_DRAM_DELAY_AUTO_ID,1)))
    {
      unsigned long
        fix_factor,
        delay_in_milisec;

      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_GFA_MILISEC_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit;
      }
      else
      {
        delay_in_milisec = (unsigned long)param_val->value.ulong_value ;
      }


      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_GFA_FIX_FACTOR_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit;
      }
      else
      {
        fix_factor = (unsigned long)param_val->value.ulong_value ;
      }


      if (!search_param_val_pairs(current_line,&match_index,PARAM_GFA_VERBOSE_ID,1))
      {
        verbose = TRUE ;
      }
      else
      {
        verbose = FALSE ;
      }
      silent = !verbose;


      fap20v_scan_delay_line_auto(
        0,
        fix_factor, delay_in_milisec,
        silent
      );
    }
    else if ((!search_param_val_pairs(current_line,&match_index,PARAM_GFA_CALIBRATE_READ_FROM_NV_ID,1)))
    {
      FAP20V_CLBRATE_PARAMS
        clbrate_params;


      fap20v_clear_CLBRATE_PARAMS(&clbrate_params);
      gfa_nv_FAP20V_CLBRATE_PARAMS_get(&clbrate_params);
      fap20v_print_CLBRATE_PARAMS(&clbrate_params);
    }
    else if ((!search_param_val_pairs(current_line,&match_index,PARAM_GFA_CALIBRATE_CLEAR_NV_ID,1)))
    {
      FAP20V_CLBRATE_PARAMS
        clbrate_params;

      fap20v_clear_CLBRATE_PARAMS(&clbrate_params);
      gfa_nv_FAP20V_CLBRATE_PARAMS_set(&clbrate_params);
    }


    else if ((!search_param_val_pairs(current_line,&match_index,PARAM_GFA_CALIB_WRITE_DQDLY_STATE,1)))
    {
      unsigned long
        ddr_id,pos;
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_GFA_CALIB_WRITE_DQDLY_STATE_DDR);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      ddr_id = param_val->value.ulong_value;
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_GFA_CALIB_WRITE_DQDLY_STATE_POS);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      pos = param_val->value.ulong_value;
      fap20v_write_dqdly_state(0,ddr_id,ddr_id,pos);
    }
    else if ((!search_param_val_pairs(current_line,&match_index,PARAM_GFA_CALIBRATE_LOAD_ID,1)))
    {
      unsigned long
        ddr_i;
      FAP20V_CLBRATE_PARAMS
        clbrate_params;


      fap20v_clear_CLBRATE_PARAMS(&clbrate_params);

      /*
       */
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_GFA_NOF_DDR_ID, 1,
                        &param_val_general_purpose, VAL_SYMBOL, err_msg
                      );
      if (0 != ret)
      {
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      clbrate_params.nof_ddrs = param_val_general_purpose->numeric_equivalent;

      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_GFA_SAMPLE_DELAY_REG_ID, 1,
                        &param_val_general_purpose, VAL_NUMERIC, err_msg
                      );
      if (0 != ret)
      {
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      for (ddr_i=0; ddr_i<=FAP20V_DRC_NOF_MODULES; ++ddr_i)
      {
        clbrate_params.sample_delay_reg[ddr_i] = param_val_general_purpose->value.ulong_value;
      }


      for (ddr_i=1; ddr_i<=FAP20V_DRC_NOF_MODULES; ++ddr_i)
      {
        ret = get_val_of( current_line, (int *)&match_index,
                          PARAM_GFA_END_WIN_ID, ddr_i,
                          &param_val, VAL_NUMERIC, err_msg
                        ) ;
        if (ret)
        {
          break;
        }
        clbrate_params.end_win[ddr_i-1] = (unsigned long)param_val->value.ulong_value;
      }

      for (ddr_i=1; ddr_i<=FAP20V_DRC_NOF_MODULES; ++ddr_i)
      {
        ret = get_val_of( current_line, (int *)&match_index,
                          PARAM_GFA_AVRG_ID, ddr_i,
                          &param_val, VAL_NUMERIC, err_msg
                        ) ;
        if (ret)
        {
          break;
        }
        clbrate_params.avrg[ddr_i-1] = (unsigned long)param_val->value.ulong_value;
      }

      ret = 0;

      clbrate_params.valid = TRUE;
      fap20v_print_CLBRATE_PARAMS(&clbrate_params);

      fap20v_calibrate_load_params(
        0,
        &clbrate_params,
        FALSE
      );

    }
    else if ((!search_param_val_pairs(current_line,&match_index,PARAM_GFA_CALIBRATE_DDR_FULL_ID,1)))
    {
      unsigned int
        fail,
        nof_drr,
        do_exhaustive_search;
      unsigned long
        offset_correct[FAP20V_DRC_NOF_MODULES],
        offset_i;
      FAP20V_CLBRATE_PARAMS
        clbrate_params;


      nof_drr = FAP20V_DRC_NOF_MODULES;
      do_exhaustive_search = FALSE;
      offset_correct[0] = 0;
      offset_correct[1] = 3;
      offset_correct[2] = 1;
      offset_correct[3] = 3;


      if (!search_param_val_pairs(current_line,&match_index,PARAM_GFA_VERBOSE_ID,1))
      {
        verbose = TRUE ;
      }
      else
      {
        verbose = FALSE ;
      }
      silent = !verbose;

      /*
       */
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_GFA_NOF_DDR_ID, 1,
                        &param_val_general_purpose, VAL_SYMBOL, err_msg
                      );
      if (0 != ret)
      {
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      nof_drr = param_val_general_purpose->numeric_equivalent;


      for (offset_i=1; offset_i<=FAP20V_DRC_NOF_MODULES; ++offset_i)
      {
        ret = get_val_of( current_line, (int *)&match_index,
                          PARAM_GFA_OFFSET_CORRECT_ID, offset_i,
                          &param_val, VAL_NUMERIC, err_msg
                        ) ;
        if (ret)
        {
          break;
        }
        offset_correct[offset_i-1] = (unsigned long)param_val->value.ulong_value;
      }
      ret = 0;

      if (!search_param_val_pairs(current_line,&match_index,PARAM_GFA_DO_EXHAUSTIVE_SEARCH_ID,1))
      {
        do_exhaustive_search = TRUE ;
      }
      else
      {
        do_exhaustive_search = FALSE ;
      }


      d_printf(
        "\n\r"
        " -- Using offset_correct: %lu %lu %lu %lu\n\r",
        offset_correct[0],
        offset_correct[1],
        offset_correct[2],
        offset_correct[3]
      );

      clbrate_params.nof_ddrs = nof_drr;

      if (gfa_is_ddr2())
      {
        fap20v_calibrate_ddr_nof_scan_set(FAP20V_CALIBRATE_DDR2_NOF_SCAN);
      }

      fap20v_calibrate_ddr_full(
        0,
        nof_drr,
        offset_correct,
        do_exhaustive_search,
        &clbrate_params,
        &fail,
        silent
      );
      if (fail)
      {
        soc_sand_os_printf( " *** fap20v_calibrate_ddr_full - FAIL \n\r");
      }
      else
      {
        soc_sand_os_printf( " *** fap20v_calibrate_ddr_full - SUCCESS \n\r");
        fap20v_print_CLBRATE_PARAMS(&clbrate_params);
        soc_sand_os_printf( "\n\r");
      }
    }
    else if ((!search_param_val_pairs(current_line,&match_index,PARAM_GFA_CALIBRATE_DDR_ID,1)))
    {
      unsigned long
        delay_in_milisec,
        max_nof_send_pkts;
      unsigned int
        fail;
      unsigned long
        offset_correct[FAP20V_DRC_NOF_MODULES];
      FAP20V_CLBRATE_PARAMS
        clbrate_params;


      offset_correct[0] = 0;
      offset_correct[1] = 3;
      offset_correct[2] = 1;
      offset_correct[3] = 3;


      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_GFA_MILISEC_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit;
      }
      else
      {
        delay_in_milisec = (unsigned long)param_val->value.ulong_value ;
      }



      if (!search_param_val_pairs(current_line,&match_index,PARAM_GFA_VERBOSE_ID,1))
      {
        verbose = TRUE ;
      }
      else
      {
        verbose = FALSE ;
      }
      silent = !verbose;



      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_GFA_NOF_PKTS_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit;
      }
      else
      {
        max_nof_send_pkts = (unsigned long)param_val->value.ulong_value ;
      }

      if (gfa_is_ddr2())
      {
        fap20v_calibrate_ddr_nof_scan_set(FAP20V_CALIBRATE_DDR2_NOF_SCAN);
      }

      fap20v_calibrate_ddr(
        0,
        FAP20V_DRC_NOF_MODULES,
        delay_in_milisec, max_nof_send_pkts,
        offset_correct, FALSE,
        &clbrate_params,
        &fail,
        silent
      );
      if (fail)
      {
        soc_sand_os_printf( "fap20v_calibrate_ddr - FAIL \n\r");
      }
    }
    else if ((!search_param_val_pairs(current_line,&match_index,PARAM_GFA_SCAN_DRAM_DELAY_AUTO_B_ID,1)))
    {
      unsigned long
        fix_factor,
        delay_in_milisec,
        sample_num,
        sample_add,
        start_ddr,
        end_ddr,
        packet_scheme,
        max_nof_send_pkts,
        end_win[FAP20V_DRC_NOF_MODULES],
        sample_delay_reg;

      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_GFA_MILISEC_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit;
      }
      else
      {
        delay_in_milisec = (unsigned long)param_val->value.ulong_value ;
      }


      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_GFA_FIX_FACTOR_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit;
      }
      else
      {
        fix_factor = (unsigned long)param_val->value.ulong_value ;
      }

      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_GFA_SAMPLE_NUM_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit;
      }
      else
      {
        sample_num = (unsigned long)param_val->value.ulong_value ;
      }

      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_GFA_SAMPLE_ADD_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit;
      }
      else
      {
        sample_add = (unsigned long)param_val->value.ulong_value ;
      }

      if (!search_param_val_pairs(current_line,&match_index,PARAM_GFA_VERBOSE_ID,1))
      {
        verbose = TRUE ;
      }
      else
      {
        verbose = FALSE ;
      }
      silent = !verbose;

      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_GFA_START_DDR_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit;
      }
      else
      {
        start_ddr = (unsigned long)param_val->value.ulong_value ;
      }

      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_GFA_END_DDR_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit;
      }
      else
      {
        end_ddr = (unsigned long)param_val->value.ulong_value ;
      }

      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_GFA_PACKET_SCHEME_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit;
      }
      else
      {
        packet_scheme = (unsigned long)param_val->value.ulong_value ;
      }

      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_GFA_NOF_PKTS_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit;
      }
      else
      {
        max_nof_send_pkts = (unsigned long)param_val->value.ulong_value ;
      }

      fap20v_scan_delay_line_auto_b(
        0,
        fix_factor, delay_in_milisec,
        sample_num, sample_add,
        start_ddr, end_ddr,
        packet_scheme, max_nof_send_pkts,
        &sample_delay_reg,
        end_win,
        silent
      );
    }
    else
    {
      /*
       * No valid operation variable has been entered.
       */
      send_string_to_screen(
          "*** Should not reach here.\r\n",
          FALSE) ;
      ret = ERR_001 ;
      goto exit ;
    }

  }
  else if ( !search_param_val_pairs(current_line,&match_index,PARAM_GFA_PRINT_STATUS_ID,1))
  {
    gfa_print_status();
  }
  else if ( !search_param_val_pairs(current_line,&match_index,PARAM_GFA_MB_ID,1))
  {
    verbose = TRUE;

    if (gfa_is_gfa_mb() == FALSE)
    {
      send_string_to_screen("*** All DB CLI must run only on DB.\r\n",FALSE) ;
      send_string_to_screen("*** Try CLI 'gfa status'.\r\n",FALSE) ;
      ret = ERR_005 ;
      goto exit ;
    }

    if ((!search_param_val_pairs(current_line,&match_index,PARAM_GFA_FLASH_ID,1)))
    {
      if ((!search_param_val_pairs(current_line,&match_index,PARAM_GFA_DOWNLOAD_ID,1)))
      {
        gfa_db_flash_download(verbose);
      }
      else if ((!search_param_val_pairs(current_line,&match_index,PARAM_GFA_ERASE_ID,1)))
      {
        gfa_db_flash_erase(verbose);
      }
      else if ((!search_param_val_pairs(current_line,&match_index,PARAM_GFA_BURN_FPGA_ID,1)))
      {
        gfa_db_flash_burn_fpga(verbose);
      }
      else if ((!search_param_val_pairs(current_line,&match_index,PARAM_GFA_PRINT_STATUS_ID,1)))
      {
        gfa_db_flash_status(verbose);
      }
    }
    else if ((!search_param_val_pairs(current_line,&match_index,PARAM_GFA_FPGA_ID,1)))
    {
      if ((!search_param_val_pairs(current_line,&match_index,PARAM_GFA_DOWNLOAD_ID,1)))
      {
        ret = gfa_db_fpga_download(verbose);

        if(ret != NO_ERR)
        {
          soc_sand_os_printf( "gfa_db_fpga_download returned with error %u\n\r", ret);
          goto exit;
        }
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
      ret = ERR_006 ;
      goto exit ;
    }



  }
  else
  {
    /*
     * No valid GFA variable has been entered.
     * There must be at least one!
     */
    send_string_to_screen(
        "\r\n"
        "*** No valid parameter has been indicated for handling.\r\n"
        "    At least one must be entered.\r\n",
        FALSE) ;
    ret = ERR_002 ;
    goto exit ;
  }

exit:

  if (NULL != sweep_params)
  {
    sal_free(sweep_params);
    sweep_params = NULL;
  }

  if (get_val_of_err)
  {
    /*
     * Value of some parameter could not be retrieved.
     */
    send_string_to_screen(
        "\r\n\n"
        "*** Procedure \'get_val_of\' returned with error indication:\r\n",TRUE) ;
    send_string_to_screen(err_msg,TRUE) ;
    ret = TRUE ;
  }
  return (ret) ;
}


/* } */
#endif /*LINK_FAP20V_LIBRARIES*/


