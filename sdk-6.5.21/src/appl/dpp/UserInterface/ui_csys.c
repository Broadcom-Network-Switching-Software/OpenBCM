/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/*
 * General include file for reference design.
 */
#include <appl/diag/dpp/ref_sys.h>
#include <appl/diag/dpp/utils_defx.h>
#include <appl/diag/dpp/ui_defx.h>

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <appl/diag/dpp/utils_defi.h>
#include <appl/diag/dpp/utils_ram_defi.h>
#include <appl/diag/dpp/utils_init_dpss_for_demo.h>

#include <appl/dpp/UserInterface/ui_pure_defi.h>
#include <appl/dpp/UserInterface/ui_defi.h>
#include <appl/dpp/UserInterface/ui_pure_defi_csys.h>

#include <SerDes/serdes_system.h>
#include <SerDes/serdes_screening.h>
#include <SerDes/serdes_link_topology.h>
#include <SerDes/serdes_fap.h>

#include <CSystem/csystem_sand_mem.h>
#include <CSystem/csystem_commands.h>
#include <CSystem/csystem_mngmnt.h>


/*
 */

int
  csys_status_fe_print_counters(
    unsigned long fap_id,
    unsigned int  flaver_bitmap
  )
{
  d_printf("csys_status_fe_print_counters in not implemented.\n\r");
  return 0;
}

int
  csys_status_fe_print_connect_map(
    unsigned long fap_id,
    unsigned int  flaver_bitmap
  )
{
  d_printf("csys_status_fe_print_connect_map in not implemented.\n\r");
  return 0;
}

int
  subject_csys_get_device(
    CURRENT_LINE  *current_line,
    unsigned long *dev_id,
    CSYS_DEV_TYPE *dev_type
  )
{
  unsigned int
    match_index;
  PARAM_VAL
    *param_val ;
  char
    err_msg[80*4] = "";

  *dev_type = CSYS_DEV_TYPE_UNKNOWN;

  if (!search_param_val_pairs(current_line,&match_index,
                              PARAM_CSYS_ALL_FAPS_ID,1))
  {
    *dev_id = CSYS_ALL_THE_DEVICES;
    *dev_type = CSYS_DEV_TYPE_FAP;
  }

  if (!search_param_val_pairs(current_line,&match_index,
                              PARAM_CSYS_ALL_FES_ID,1))
  {
    *dev_id = CSYS_ALL_THE_DEVICES;
    *dev_type = CSYS_DEV_TYPE_FE;
  }

  if (!search_param_val_pairs(current_line,&match_index,
                              PARAM_CSYS_ALL_PPS_ID,1))
  {
    *dev_id = CSYS_ALL_THE_DEVICES;
    *dev_type = CSYS_DEV_TYPE_PP;
  }

  if (!get_val_of(
          current_line,(int *)&match_index,PARAM_CSYS_FAP_ID_ID,1,
          &param_val,VAL_NUMERIC,err_msg))
  {
    *dev_id = param_val->value.ulong_value ;
    *dev_type = CSYS_DEV_TYPE_FAP;
  }

  if (!get_val_of(
          current_line,(int *)&match_index,PARAM_CSYS_FE_ID_ID,1,
          &param_val,VAL_NUMERIC,err_msg))
  {
    *dev_id = param_val->value.ulong_value ;
    *dev_type = CSYS_DEV_TYPE_FE;
  }

  if (!get_val_of(
          current_line,(int *)&match_index,PARAM_CSYS_PP_ID_ID,1,
          &param_val,VAL_NUMERIC,err_msg))
  {
    *dev_id = param_val->value.ulong_value ;
    *dev_type = CSYS_DEV_TYPE_PP;
  }
  return 0;
}


/*****************************************************
*NAME
*  csys_status_pp_print_counters
*TYPE: PROC
*FUNCTION:
*  Print all the PP MAC counters.
*  Might be called for one PP or all the PPs in the system.
*CALLING SEQUENCE:
*INPUT:
*  SOC_SAND_DIRECT:
*   unsigned long pp_id -
*     Absolute number (Slot ID - 1).
*     If (pp_id == CSYS_ALL_THE_DEVICES) print all
*       the counters of all the PPs in the system.
*   unsigned int  flaver_bitmap -
*     bit 0 ON:  Long counter description.
*     bit 0 OFF: Short counter description.
*     bit 1 ON:  If the counter value is '0', don't print it.
*     bit 1 OFF: If the counter value is '0', print it.
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
#if LINK_PSS_LIBRARIES
int
  csys_status_pp_print_counters(
    unsigned long pp_id,
    unsigned int  flaver_bitmap
  )
{
  char
    *proc_name = "csys_status_pp_print_counters";
  unsigned int
    last_pp, first_pp, pp_i,relative_pp, port_i;
  unsigned long
    ret=0,
    ip_address;

  if(!dune_rpc_are_all_line_type_x(LINE_CARD_01))
  {
    soc_sand_os_printf( 
      " %s: Not supported for SOC_SAND_FAP20V.\n\r",
      proc_name
    );
    ret= 1;
    goto exit;
  }

  if (pp_id == CSYS_ALL_THE_DEVICES)
  {
    first_pp = 0;
    last_pp  = csys_mngment_get_max_nof_faps() -1;
  }
  else
  {
    first_pp = pp_id;
    last_pp  = pp_id;
  }

  relative_pp = 0;
  for(pp_i=first_pp; pp_i <= last_pp; pp_i++)
  {
    if (csys_mngment_is_fap_exist(pp_i, &ip_address) == FALSE)
    {
      continue;
    }

    for(port_i=0;port_i<NUM_PORTS_ON_PP;port_i++)
    {
      display_all_mac_counters(
        FALSE,
        relative_pp,
        relative_pp,
        port_i,
        flaver_bitmap,
        TRUE
      );
    }
    relative_pp++;
  }
exit:
  return ret;
}
#endif
/*****************************************************
*NAME
*  subject_csys
*TYPE: PROC
*DATE: 29/DEC/2002
*FUNCTION:
*  Process input line which has an 'CSYS' subject.
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
int subject_csys(CURRENT_LINE *current_line, CURRENT_LINE **current_line_ptr)
{
  PARAM_VAL
    *param_val ;
  char
    err_msg[80*4] = "";
  char
    *proc_name;
  BOOL
    get_val_of_err ;
  unsigned int
    match_index,
    short_format,
    num_handled;
  int
    err,ui_ret;
  enum
  {
    NO_ERR = 0,
    ERR_001, ERR_002, ERR_003, ERR_004, ERR_005,
    NUM_EXIT_CODES
  } ret ;

  proc_name = "subject_csys";
  ret = NO_ERR ;
  get_val_of_err = FALSE;
  short_format = TRUE;
  num_handled = 0;

  send_string_to_screen("",TRUE);

  /*
   * the rest ot the line is empty
   */
  if (current_line->num_param_names == 0)
  {
    /*
     * Enter if there are no parameters on the line (just 'csys').
     */
    send_string_to_screen("Chassis System is connected to this CPU-card.\n\r", FALSE);
    goto exit ;
  }
  else if ( !search_param_val_pairs(current_line,&match_index,
                                    PARAM_CSYS_MNGT_ID,1))
  {
    /*
     * Enter if this is a 'csys management' request.
     */
    if (!search_param_val_pairs(current_line,&match_index,
                          PARAM_CSYS_MNGT_NET_ID,1))
    {
      if (!search_param_val_pairs(current_line,&match_index,
                            PARAM_CSYS_MNGT_STATUS_ID,1))
      {
        /*
         * Enter if this is a 'csys management net ..' request.
         */
        num_handled++ ;
        print_network_learning_table();
      }

    }
    else
    {
      /*
       * Parameter following 'management' is unrecognized.
       */
      send_string_to_screen(
          "\r\n\n"
          "*** Parameter following \'management\' is unrecognizable\r\n",TRUE) ;
      send_string_to_screen(err_msg,TRUE) ;
      ret = TRUE ;
      goto exit ;
    }


  }
#if LINK_SERDES_LIBRARIES
  else if ( !search_param_val_pairs(current_line,&match_index,
                                    PARAM_CSYS_SERDES_ID,1))
  {
    /*
     * Operation related to SERDES
     */
    unsigned long
      fap_id;
    unsigned int
      silent,
      start;

    silent = FALSE;
    fap_id = 0;


    /*
     * Enter if this is a 'general serdes' request.
     */
    if (!search_param_val_pairs(current_line,&match_index,
                          PARAM_CSYS_SERDES_PRBS_FAP_TX_LINKS_ID,1))
    {
      /*
       * Enter if this is a '' request.
       */
      num_handled++ ;

      if (!search_param_val_pairs(current_line,&match_index,
                          PARAM_CSYS_START_ID,1))
      {
        start = TRUE;
      }
      else
      {
        start = FALSE;
      }
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_CSYS_FAP_ID_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        /*
         * Value of filename parameter could not be retrieved.
         */
        ret = TRUE ;
        goto exit ;
      }
      fap_id = (int)param_val->value.ulong_value ;

      serdes_prbs_fap_tx_links(
        start,
        fap_id,
        silent
      );
    }
    else if (!search_param_val_pairs(current_line,&match_index,
                          PARAM_CSYS_SERDES_PRBS_FE_TX_LINKS_ID,1))
    {
      unsigned long
        fap_link_id;
      /*
       * Enter if this is a '' request.
       */
      num_handled++ ;

      if (!search_param_val_pairs(current_line,&match_index,
                          PARAM_CSYS_START_ID,1))
      {
        start = TRUE;
      }
      else
      {
        start = FALSE;
      }
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_CSYS_FAP_ID_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        /*
         * Value of filename parameter could not be retrieved.
         */
        ret = TRUE ;
        goto exit ;
      }
      fap_id = (int)param_val->value.ulong_value ;

      fap_link_id = SOC_SAND_ALL_LINKS;
      if (!get_val_of(
              current_line,(int *)&match_index,PARAM_CSYS_FAP_LINK_ID_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        fap_link_id = param_val->value.ulong_value ;

        if(dune_rpc_are_all_line_type_x(LINE_CARD_01) &&
           fap_link_id >= FAP10M_NOF_FABRIC_LINKS
          )
        {
          sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** When working with SOC_SAND_FAP10M, the fap link ID \r\n"
                  "     %ld is invalid.\n\r",
                  fap_link_id
          );

          send_string_to_screen(err_msg, TRUE) ;
          ret = TRUE ;
          goto exit ;
        }
      }
      else
      {
        if(dune_rpc_are_all_line_type_x(LINE_CARD_01))
        {
          send_string_to_screen(
              "\r\n\n"
              "*** When working with SOC_SAND_FAP10M, the fap link ID \r\n"
              "     should be specified.",TRUE) ;
          send_string_to_screen(err_msg,TRUE) ;
          ret = TRUE ;
          goto exit ;

        }
      }

      serdes_prbs_fe_tx_links(
        start,
        fap_id,
        fap_link_id,
        silent
      );
    }
    else if (!search_param_val_pairs(current_line,&match_index,
                          PARAM_CSYS_SERDES_GET_FE_BERS_ID,1))
    {
      /*
       * Enter if this is a '' request.
       */
      SERDES_LINK_STATE
        link_state[CSYS_MAX_NOF_FAP_LINKS];
      const SERDES_TPLG_FAP*
        fap_topology;
      unsigned long
        all_links_ok;

      num_handled++ ;

      if (get_val_of(
              current_line,(int *)&match_index,PARAM_CSYS_FAP_ID_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        /*
         * Value of filename parameter could not be retrieved.
         */
        ret = TRUE ;
        goto exit ;
      }
      fap_id = (int)param_val->value.ulong_value ;

      memset(link_state, 0x0, sizeof(SERDES_LINK_STATE)*CSYS_MAX_NOF_FAP_LINKS);
      serdes_tplg_get_fap_links(fap_id, &fap_topology);
      err = serdes_prbs_fe_rx_ber_cnt(
              fap_id,
              fap_topology,
              link_state,
              FALSE
            );

      if (!err)
      {
        serdes_print_fap_link_state(
          fap_id,
          fap_topology,
          link_state,
          FALSE,
          &all_links_ok
        );
      }

    }
    else if (!search_param_val_pairs(current_line,&match_index,
                          PARAM_CSYS_SERDES_GET_FAP_BERS_ID,1))
    {
      /*
       * Enter if this is a '' request.
       */
      SERDES_LINK_STATE
        link_state[CSYS_MAX_NOF_FAP_LINKS];
      const SERDES_TPLG_FAP*
        fap_topology;
      unsigned long
        all_links_ok;

      num_handled++ ;

      if (get_val_of(
              current_line,(int *)&match_index,PARAM_CSYS_FAP_ID_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        /*
         * Value of filename parameter could not be retrieved.
         */
        ret = TRUE ;
        goto exit ;
      }
      fap_id = (int)param_val->value.ulong_value ;

      memset(link_state, 0x0, sizeof(SERDES_LINK_STATE)*CSYS_MAX_NOF_FAP_LINKS);
      serdes_tplg_get_fap_links(fap_id, &fap_topology);
      err = serdes_prbs_fap_rx_ber_cnt(
              fap_id,
              fap_topology,
              link_state,
              TRUE
            );

      if (!err)
      {
        serdes_print_fap_link_state(
          fap_id,
          fap_topology,
          link_state,
          FALSE,
          &all_links_ok
        );
      }
    }
#if LINK_FE200_LIBRARIES
    else if (!search_param_val_pairs(current_line,&match_index,
                          PARAM_CSYS_SERDES_FE_LINKS_ID,1))
    {
      /*
       * Enter if this is a '' request.
       */
      num_handled++ ;
      serdes_sys_get_fe200_links_status(FALSE);

    }
#endif /*#if LINK_FE200_LIBRARIES*/
    else if (!search_param_val_pairs(current_line,&match_index,
                          PARAM_CSYS_SCREENING_ID,1))
    {
      SERDES_RX_TX_OP
        direction = SERDES_RX_TX_OP_BOTH;
      unsigned int
        time = 20,
        bitmap=0xF,
        printing_level =0;
      /*
       * Enter if this is a '' request.
       */
      num_handled++ ;
      UI_MACROS_GET_SYMBOL_VAL(PARAM_CSYS_SCREENING_RX_TX_ID);
      if (0 == ui_ret)
      {
        direction = param_val->numeric_equivalent;
      }

      UI_MACROS_GET_NUMMERIC_VAL(PARAM_CSYS_SCREENING_TIME_ID);
      if (0 == ui_ret)
      {
        time = param_val->value.ulong_value;
      }

      UI_MACROS_GET_NUMMERIC_VAL(PARAM_CSYS_SCREENING_PRNT_LVL_ID);
      if (0 == ui_ret)
      {
        printing_level = param_val->value.ulong_value;
      }
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_CSYS_SCREENING_BM_ID);
      if (0 == ui_ret)
      {
        bitmap = param_val->value.ulong_value;
      }
      serdes_negev_prbs_fap_tx_rx_links(direction,time,printing_level,bitmap);
    }
    else if (!search_param_val_pairs(current_line,&match_index,
                          PARAM_CSYS_SERDES_FAP_LINKS_ID,1))
    {
      /*
       * Enter if this is a '' request.
       */
      num_handled++ ;
      serdes_sys_get_fap_links_status(FALSE);

    }
#if LINK_FAP20V_LIBRARIES

    else if (!search_param_val_pairs(current_line,&match_index,
                          PARAM_CSYS_SERDES_LOAD_PARAMS_FAP20V_ID,1))
    {
      /*
       * Enter if this is a '' request.
       */
      num_handled++ ;

      if (get_val_of(
              current_line,(int *)&match_index,PARAM_CSYS_FILE_NAME_ID,1,
              &param_val,VAL_TEXT,err_msg))
      {
        /*
         * Value of filename parameter could not be retrieved.
         */
        ret = TRUE ;
        goto exit ;
      }

      serdes_sys_load_fap20v_serdes_params(
        DEFAULT_DOWNLOAD_IP,
        param_val->value.val_text,
        FALSE
      );
    }
#endif /*LINK_FAP20V_LIBRARIES*/

    else if (!search_param_val_pairs(current_line,&match_index,
                          PARAM_CSYS_SERDES_LOAD_PARAMS_FE200_ID,1))
    {
      /*
       * Enter if this is a '' request.
       */
      num_handled++ ;

      if (get_val_of(
              current_line,(int *)&match_index,PARAM_CSYS_FILE_NAME_ID,1,
              &param_val,VAL_TEXT,err_msg))
      {
        /*
         * Value of filename parameter could not be retrieved.
         */
        ret = TRUE ;
        goto exit ;
      }


      serdes_sys_load_fe_serdes_params(
        DEFAULT_DOWNLOAD_IP,
        param_val->value.val_text,
        FALSE
      );
    }
    else
    {
      /*
       * Parameter following 'serdes' is unrecognized.
       */
      send_string_to_screen(
          "\r\n\n"
          "*** Parameter following \'serdes\' is unrecognizable\r\n",TRUE) ;
      send_string_to_screen(err_msg,TRUE) ;
      ret = TRUE ;
      goto exit ;
    }


  }
#endif /*#if LINK_SERDES_LIBRARIES*/
  else if ( !search_param_val_pairs(current_line,&match_index,
                                    PARAM_CSYS_MEM_ID,1))
  {
    /*
     * Operation related to MEM
     */
    unsigned long
      dev_id,
      *data_ptr;
    unsigned int
      silent,
      read_op,
      write_op,
      short_format,
      fap_indicator,
      dev_i,
      data_i,
      address_in_long;
    CSYS_MEM_VALUE
      mem_val[CSYS_MAX_NOF_DEV_OF_ONE_TYPE_SYSTEM];
    PARAM_VAL
      *param_val_data ;

    silent = FALSE;
    dev_id = CSYS_ALL_THE_DEVICES;
    read_op = 0;
    write_op= 0;
    short_format = TRUE;
    data_ptr = NULL;
    address_in_long = FALSE;
    fap_indicator = TRUE;

    sal_memset(
      mem_val,
      0x0,
      sizeof(CSYS_MEM_VALUE)*CSYS_MAX_NOF_FAP_IN_SYSTEM
    );


    if (!search_param_val_pairs(current_line,&match_index,
                                PARAM_CSYS_READ_ID,1))
    {
      read_op = TRUE;
    }

    if (!search_param_val_pairs(current_line,&match_index,
                                PARAM_CSYS_WRITE_ID,1))
    {
      write_op = TRUE;
    }

    if (!search_param_val_pairs(current_line,&match_index,
                                PARAM_CSYS_SILENT_ID,1))
    {
      silent = TRUE;
    }
    else
    {
      silent = FALSE;
    }

    if (!search_param_val_pairs(current_line,&match_index,
                                PARAM_CSYS_ADDRESS_IN_LONGS_ID,1))
    {
      address_in_long = TRUE;
    }
    else
    {
      address_in_long = FALSE;
    }
    for (dev_i=0; dev_i<CSYS_MAX_NOF_DEV_OF_ONE_TYPE_SYSTEM; ++dev_i)
    {
      mem_val[dev_i].address_in_long = address_in_long;
    }


    if (!search_param_val_pairs(current_line,&match_index,
                                PARAM_CSYS_ALL_FAPS_ID,1))
    {
      dev_id = CSYS_ALL_THE_DEVICES;
      fap_indicator = TRUE;
    }

    if (!search_param_val_pairs(current_line,&match_index,
                                PARAM_CSYS_ALL_FES_ID,1))
    {
      dev_id = CSYS_ALL_THE_DEVICES;
      fap_indicator = FALSE;
    }

    if (!get_val_of(
            current_line,(int *)&match_index,PARAM_CSYS_FAP_ID_ID,1,
            &param_val,VAL_NUMERIC,err_msg))
    {
      dev_id = param_val->value.ulong_value ;
      fap_indicator = TRUE;
    }

    if (!get_val_of(
            current_line,(int *)&match_index,PARAM_CSYS_FE_ID_ID,1,
            &param_val,VAL_NUMERIC,err_msg))
    {
      dev_id = param_val->value.ulong_value ;
      fap_indicator = FALSE;
    }

    if (!get_val_of(
            current_line,(int *)&match_index,PARAM_CSYS_OFFSET_ID,1,
            &param_val,VAL_NUMERIC,err_msg))
    {
      for (dev_i=0; dev_i<CSYS_MAX_NOF_DEV_OF_ONE_TYPE_SYSTEM; ++dev_i)
      {
        mem_val[dev_i].offset = param_val->value.ulong_value;
      }
    }

    /*
     * This is for write
     */
    data_ptr = mem_val[0].value;
    mem_val[0].nof_valid_entries = 0;
    for (data_i=1; data_i<=CSYS_MEM_VALUE_NOF_ENTRIES; ++data_i)
    {
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_CSYS_DATA_ID, data_i,
                        &param_val_data, VAL_NUMERIC, err_msg
                      ) ;
      if (ret)
      {
        ret = 0;
        break ;
      }
      mem_val[0].nof_valid_entries += 1;
      *data_ptr = param_val_data->value.ulong_value ;
      data_ptr++;
    }


    /*
     * This is for read
     */
    if (!get_val_of(
            current_line,(int *)&match_index,PARAM_CSYS_SIZE_ID,1,
            &param_val,VAL_NUMERIC,err_msg))
    {
      for (dev_i=0; dev_i<CSYS_MAX_NOF_DEV_OF_ONE_TYPE_SYSTEM; ++dev_i)
      {
        mem_val[dev_i].nof_valid_entries = SOC_SAND_MIN(param_val->value.ulong_value, CSYS_MEM_VALUE_NOF_ENTRIES);
      }
    }


    if (fap_indicator)
    {
      if (read_op)
      {
        err = csys_faps_mem_read(
                dev_id,
                mem_val,
                silent
              );
        if (!err)
        {
          soc_sand_os_printf( "\n\r");
          soc_sand_os_printf( "\n\r");
          cys_print_CSYS_MEM_VALUE(
            dev_id,
            fap_indicator,
            mem_val,
            TRUE,
            short_format
          );
          soc_sand_os_printf( "\n\r");
        }
      }

      if (write_op)
      {
        err = csys_faps_mem_write(
                dev_id,
                &mem_val[0],
                silent
              );

      }
    }
    else
    {
      if (read_op)
      {
        err = csys_fes_mem_read(
                dev_id,
                mem_val,
                silent
              );
        if (!err)
        {
          cys_print_CSYS_MEM_VALUE(
            dev_id,
            fap_indicator,
            mem_val,
            TRUE,
            short_format
          );
        }
      }

      if (write_op)
      {
        err = csys_fes_mem_write(
                dev_id,
                &mem_val[0],
                silent
              );

      }
    }
  }
  else if ( !search_param_val_pairs(current_line,&match_index,
                                    PARAM_CSYS_STATUS_ID,1))
  {
    unsigned long
      dev_id;
    unsigned int
      flavor,
      flavor_bitmap=0;
    CSYS_DEV_TYPE
      dev_type;

    subject_csys_get_device(
      current_line,
      &dev_id,
      &dev_type
    );

#if LINK_PSS_LIBRARIES
    if (!search_param_val_pairs(current_line,&match_index,
                                PARAM_CSYS_GET_COUNTERS_ID,1))
    {
      UI_MACROS_GET_SYMBOL_VAL(PARAM_CSYS_GET_COUNTERS_ID);
      if(ui_ret == 0)
      {
        flavor = param_val->numeric_equivalent;

        SOC_SAND_SET_BIT(flavor_bitmap,1,SOC_SAND_PRINT_FLAVORS_SHORT);
        SOC_SAND_SET_BIT(flavor_bitmap,flavor,SOC_SAND_PRINT_FLAVORS_NO_ZEROS);
        if(dev_type == CSYS_DEV_TYPE_FAP)
        {
          csys_status_fap_print_counters(dev_id,flavor_bitmap);
        }
        else if(dev_type == CSYS_DEV_TYPE_FE)
        {
          csys_status_fe_print_counters(dev_id,flavor_bitmap);
        }
        else if(dev_type == CSYS_DEV_TYPE_PP)
        {
          csys_status_pp_print_counters(dev_id,flavor_bitmap);
        }
        else
        {
          soc_sand_os_printf( 
            "CSystem Status Get Counter with unknown device.\n\r"
          );
        }
      }
    }
    else
#endif
      if (!search_param_val_pairs(current_line,&match_index,
                                  PARAM_CSYS_GET_CONNECTIVITY_MAP_ID,1))
    {
      UI_MACROS_GET_SYMBOL_VAL(PARAM_CSYS_GET_CONNECTIVITY_MAP_ID);
      if(ui_ret == 0)
      {
        flavor = param_val->numeric_equivalent;

        SOC_SAND_SET_BIT(flavor_bitmap,flavor,SOC_SAND_PRINT_FLAVORS_ERRS_ONLY);

        if(dev_type == CSYS_DEV_TYPE_FAP)
        {
          csys_status_fap_print_connect_map(dev_id,flavor_bitmap);
        }
        else if(dev_type == CSYS_DEV_TYPE_FE)
        {
          csys_status_fe_print_connect_map(dev_id,flavor_bitmap);
        }
        else
        {
          soc_sand_os_printf( 
            "CSystem Status Get Connectivity Map with unknown device.\n\r"
          );
        }
      }
    }
    else
    {
      soc_sand_os_printf( 
        "CSystem Status Unknowen command.\n\r"
        );

    }
  }
  /*
   * Else, there are parameters on the line (not just 'csys').
   */
  else
  {
    /*
     * No valid CSYS variable has been entered.
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



